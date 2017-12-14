/*
* Copyright: JessMA Open Source (ldcsaa@gmail.com)
*
* Author	: Bruce Liang
* Website	: http://www.jessma.org
* Project	: https://github.com/ldcsaa
* Blog		: http://www.cnblogs.com/ldcsaa
* Wiki		: http://www.oschina.net/p/hp-socket
* QQ Group	: 75375912, 44636872
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#pragma once

#include "GlobalDef.h"
#include "Thread.h"

#include <signal.h>

#include <memory>

using namespace std;

template<class T> class CSignalHandler
{
public:
	using MT				= CSignalHandler<T>;
	using SI				= siginfo_t;
	using SS				= sigset_t;
	using CHandlerThread	= CThread<MT, const SS, VOID>;
	using F					= VOID (T::*)(const SI*);
	using SF				= VOID (*)(const SI*);
	using SSPTR				= unique_ptr<SS>;
	friend CHandlerThread;

public:

	BOOL Setup(SF pFunc, const SS* pSigSet, BOOL bRestorOnCancel = TRUE)
	{
		return Setup((__CFakeRunnerClass_*)nullptr, *(F*)&pFunc, pSigSet, bRestorOnCancel);
	}

	BOOL Setup(T* pRunner, F pFunc, const SS* pSigSet, BOOL bRestorOnCancel = TRUE)
	{
		ASSERT_CHECK_EINVAL(pSigSet != nullptr);

		m_pssPre = make_unique<SS>();
		int rs	 = pthread_sigmask(SIG_BLOCK, pSigSet, m_pssPre.get());

		if(rs != NO_ERROR)
		{
			m_pssPre = nullptr;
			::SetLastError(rs);

			return FALSE;
		}

		m_pRunner	= pRunner;
		m_pFunc		= pFunc;
		m_pssCur	= make_unique<SS>();

		::CopyPlainObject(m_pssCur.get(), pSigSet);

		BOOL isOK	= m_thHandler.Start(this, &MT::ThreadFunc, m_pssCur.get());

		if(isOK && !bRestorOnCancel)
			m_pssPre = nullptr;
		else if(!isOK)
			EXECUTE_RESTORE_ERROR(Reset());

		return isOK;
	}

	BOOL Cancel()
	{
		BOOL isOK = m_thHandler.IsRunning();

		if(isOK)
		{
			isOK  = m_thHandler.Cancel();
			isOK &= m_thHandler.Join();
		}

		isOK &= Reset();

		return isOK;
	}

	BOOL IsRunning		()	{return m_thHandler.IsRunning();}
	T* GetRunner		()	{return m_pRunner;}
	F GetFunc			()	{return m_pFunc;}
	SF GetSFunc			()	{return *(SF*)&m_pFunc;}
	THR_ID GetThreadID	()	{return m_thHandler.GetThreadID();}
	NTHR_ID GetNativeID	()	{return m_thHandler.GetNativeID();}


private:
	VOID ThreadFunc(const SS* pSigSet)
	{
		ASSERT(pSigSet == m_pssCur.get());

		SI si;

		while(TRUE)
		{
			int rs = NO_EINTR_INT(sigwaitinfo(pSigSet, &si));

			if(IS_HAS_ERROR(rs))
				ERROR_ABORT();

			Run((T*)nullptr, &si);
		}
	}

	template<typename T_, typename = enable_if_t<!is_same<T_, __CFakeRunnerClass_>::value>>
	VOID Run(T_*, const SI* pSigSet) {(m_pRunner->*m_pFunc)(pSigSet);}

	VOID Run(__CFakeRunnerClass_*, const SI* pSigSet) {(*(SF*)&m_pFunc)(pSigSet);}


	BOOL Reset()
	{
		BOOL isOK = TRUE;

		if(m_pssPre)
		{
			isOK	 = (pthread_sigmask(SIG_SETMASK, m_pssPre.get(), nullptr) == NO_ERROR);
			m_pssPre = nullptr;
		}

		m_pssCur	= nullptr;
		m_pRunner	= nullptr;
		m_pFunc		= nullptr;

		return isOK;
	}

public:
	CSignalHandler()
	{
	}

	virtual ~CSignalHandler()
	{
		Cancel();
	}

	DECLARE_NO_COPY_CLASS(CSignalHandler)

private:
	T*				m_pRunner;
	F				m_pFunc;
	SSPTR			m_pssCur;
	SSPTR			m_pssPre;
	CHandlerThread	m_thHandler;
};

using CStaticSignalHandler = CSignalHandler<__CFakeRunnerClass_>;
