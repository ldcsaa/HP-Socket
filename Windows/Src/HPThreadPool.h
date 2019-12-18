/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Author	: Bruce Liang
 * Website	: https://github.com/ldcsaa
 * Project	: https://github.com/ldcsaa/HP-Socket/HP-Socket
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 44636872, 75375912
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

#pragma push_macro("_ATL_NO_DEFAULT_LIBS")

#ifndef _ATL_NO_DEFAULT_LIBS
	#define _ATL_NO_DEFAULT_LIBS
#endif

#include <atlutil.h>

#pragma pop_macro("_ATL_NO_DEFAULT_LIBS")

#include "SocketInterface.h"
#include "../Common/Src/SysHelper.h"

LPTSocketTask CreateSocketTaskObj(	Fn_SocketTaskProc fnTaskProc,
									PVOID pSender, CONNID dwConnID,
									LPCBYTE pBuffer, INT iBuffLen, EnTaskBufferType enBuffType = TBT_COPY,
									WPARAM wParam = 0, LPARAM lParam = 0);

void DestroySocketTaskObj(LPTSocketTask pTask);

class CHPThreadPool : public IHPThreadPool
{
	struct TTask
	{
		Fn_TaskProc	fn;
		PVOID		arg;
		BOOL		freeArg;

	public:
		static TTask* Construct(Fn_TaskProc fnTaskProc, PVOID pvArg, BOOL bFreeArg)
		{
			return new TTask(fnTaskProc, pvArg, bFreeArg);
		}

		static void Destruct(TTask* pTask)
		{
			delete pTask;
		}

	private:
		TTask(Fn_TaskProc fnTaskProc, PVOID pvArg, BOOL bFreeArg)
		: fn(fnTaskProc), arg(pvArg), freeArg(bFreeArg)
		{
			ASSERT(fn != nullptr);
		}
	};

	class CWorker
	{
	public:
		typedef TTask* RequestType;

	public:
		BOOL Initialize(PVOID pvWorkerParam);
		void Execute(TTask* pTask, PVOID pvWorkerParam, OVERLAPPED* pOverlapped);
		void Terminate(PVOID pvWorkerParam);

	public:
		CWorker() 
		: m_pthPool(nullptr)
		{

		}

	private:
		CHPThreadPool* m_pthPool;
	};

	typedef CThreadPool<CWorker> CInnerThreadPool;

	friend class CWorker;

private:
	enum EnSubmitResult{SUBMIT_OK, SUBMIT_FULL, SUBMIT_ERROR};

public:
	virtual BOOL Start(DWORD dwThreadCount = 0, DWORD dwMaxQueueSize = 0, EnRejectedPolicy enRejectedPolicy = TRP_CALL_FAIL, DWORD dwStackSize = 0);
	virtual BOOL Stop(DWORD dwMaxWait = INFINITE);
	virtual BOOL Wait(DWORD dwMilliseconds = INFINITE) {return m_evWait.Wait(dwMilliseconds);}

	virtual BOOL Submit(Fn_TaskProc fnTaskProc, PVOID pvArg, DWORD dwMaxWait = INFINITE);
	virtual BOOL Submit(LPTSocketTask pTask, DWORD dwMaxWait = INFINITE);
	virtual BOOL AdjustThreadCount(DWORD dwNewThreadCount);

public:
	virtual BOOL HasStarted()						{return m_enState == SS_STARTED || m_enState == SS_STARTING;}
	virtual EnServiceState GetState()				{return m_enState;}

	virtual DWORD GetQueueSize()					{return m_dwQueueSize;}
	virtual DWORD GetTaskCount()					{return m_dwTaskCount;}
	virtual DWORD GetThreadCount()					{return (DWORD)m_thPool.GetNumThreads();}
	virtual DWORD GetMaxQueueSize()					{return m_dwMaxQueueSize;}
	virtual EnRejectedPolicy GetRejectedPolicy()	{return m_enRejectedPolicy;}

private:
	BOOL CheckStarting();
	BOOL CheckStarted();
	BOOL CheckStoping();

	EnSubmitResult DirectSubmit(Fn_TaskProc fnTaskProc, PVOID pvArg, BOOL bFreeArg);
	BOOL CycleWaitSubmit(Fn_TaskProc fnTaskProc, PVOID pvArg, DWORD dwMaxWait, BOOL bFreeArg);
	BOOL DoSubmit(Fn_TaskProc fnTaskProc, PVOID pvArg, BOOL bFreeArg, DWORD dwMaxWait);

public:
	CHPThreadPool()
	: m_evWait(TRUE, TRUE)
	{
		Reset(FALSE);
	}

	virtual ~CHPThreadPool()
	{
		if(GetState() != SS_STOPPED) Stop();
	}

private:
	void Reset(BOOL bSetWaitEvent = TRUE);

private:
	CEvt					m_evWait;

#if _WIN32_WINNT >= _WIN32_WINNT_WS08
	CCriSec					m_csQueue;
	CConVar					m_cvQueue;
#endif

	DWORD					m_dwMaxQueueSize;
	EnRejectedPolicy		m_enRejectedPolicy;

	volatile DWORD			m_dwQueueSize;
	volatile DWORD			m_dwTaskCount;
	volatile EnServiceState	m_enState;

	CInnerThreadPool		m_thPool;

	DECLARE_NO_COPY_CLASS(CHPThreadPool)
};
