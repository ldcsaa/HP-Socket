/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.20
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

#include <intrin.h>

#pragma intrinsic(_ReadBarrier)
#pragma intrinsic(_WriteBarrier)
#pragma intrinsic(_ReadWriteBarrier)

#define DEFAULT_CRISEC_SPIN_COUNT	4096

#if defined (_WIN64)
	#define DEFAULT_PAUSE_RETRY		16
	#define DEFAULT_PAUSE_YIELD		128
	#define DEFAULT_PAUSE_CYCLE		8192
#else
	#define DEFAULT_PAUSE_RETRY		4
	#define DEFAULT_PAUSE_YIELD		32
	#define DEFAULT_PAUSE_CYCLE		4096
#endif

#ifndef YieldProcessor
	#pragma intrinsic(_mm_pause)
	#define YieldProcessor _mm_pause
#endif

static inline void YieldThread(UINT i = DEFAULT_PAUSE_RETRY)
{
	if		(i < DEFAULT_PAUSE_RETRY)		;
	else if	(i < DEFAULT_PAUSE_YIELD)		YieldProcessor();
	else if	(i < DEFAULT_PAUSE_CYCLE - 1)	SwitchToThread();
	else if	(i < DEFAULT_PAUSE_CYCLE)		Sleep(1);
	else									YieldThread(i & (DEFAULT_PAUSE_CYCLE - 1));
}

class CInterCriSec
{
public:
	CInterCriSec(DWORD dwSpinCount = DEFAULT_CRISEC_SPIN_COUNT)
		{VERIFY(::InitializeCriticalSectionAndSpinCount(&m_crisec, dwSpinCount));}
	~CInterCriSec()
		{::DeleteCriticalSection(&m_crisec);}

	void Lock()								{::EnterCriticalSection(&m_crisec);}
	void Unlock()							{::LeaveCriticalSection(&m_crisec);}
	BOOL TryLock()							{return ::TryEnterCriticalSection(&m_crisec);}
	DWORD SetSpinCount(DWORD dwSpinCount)	{return ::SetCriticalSectionSpinCount(&m_crisec, dwSpinCount);}

	CRITICAL_SECTION* GetObject()			{return &m_crisec;}

private:
	CInterCriSec(const CInterCriSec& cs);
	CInterCriSec operator = (const CInterCriSec& cs);

private:
	CRITICAL_SECTION m_crisec;
};

class CInterCriSec2
{
public:
	CInterCriSec2(DWORD dwSpinCount = DEFAULT_CRISEC_SPIN_COUNT, BOOL bInitialize = TRUE)
	{
		if(bInitialize)
		{
			m_pcrisec = new CRITICAL_SECTION;
			VERIFY(::InitializeCriticalSectionAndSpinCount(m_pcrisec, dwSpinCount));
		}
		else
			m_pcrisec = nullptr;
	}

	~CInterCriSec2() {Reset();}

	void Attach(CRITICAL_SECTION* pcrisec)
	{
		Reset();
		m_pcrisec = pcrisec;
	}

	CRITICAL_SECTION* Detach()
	{
		CRITICAL_SECTION* pcrisec = m_pcrisec;
		m_pcrisec = nullptr;
		return pcrisec;
	}

	void Lock()								{::EnterCriticalSection(m_pcrisec);}
	void Unlock()							{::LeaveCriticalSection(m_pcrisec);}
	BOOL TryLock()							{return ::TryEnterCriticalSection(m_pcrisec);}
	DWORD SetSpinCount(DWORD dwSpinCount)	{return ::SetCriticalSectionSpinCount(m_pcrisec, dwSpinCount);}

	CRITICAL_SECTION* GetObject()			{return m_pcrisec;}

private:
	CInterCriSec2(const CInterCriSec2& cs);
	CInterCriSec2 operator = (const CInterCriSec2& cs);

	void Reset()
	{
		if(m_pcrisec)
		{
			::DeleteCriticalSection(m_pcrisec);
			delete m_pcrisec;
			m_pcrisec = nullptr;
		}
	}

private:
	CRITICAL_SECTION* m_pcrisec;
};

class CMTX
{
public:
	CMTX(BOOL bInitialOwner = FALSE, LPCTSTR pszName = nullptr, LPSECURITY_ATTRIBUTES pSecurity = nullptr)	
	{
		m_hMutex = ::CreateMutex(pSecurity, bInitialOwner, pszName);
		ASSERT(IsValid());
	}

	~CMTX()
	{
		if(IsValid())
			::CloseHandle(m_hMutex);
	}

	BOOL Open(DWORD dwAccess, BOOL bInheritHandle, LPCTSTR pszName)
	{
		if(IsValid())
			VERIFY(::CloseHandle(m_hMutex));

		m_hMutex = ::OpenMutex(dwAccess, bInheritHandle, pszName);
		return(IsValid());
	}

	void Lock(DWORD dwMilliseconds = INFINITE)	{::WaitForSingleObject(m_hMutex, dwMilliseconds);}
	void Unlock()								{::ReleaseMutex(m_hMutex);}

	HANDLE& GetHandle	() 	{return m_hMutex;}
	operator HANDLE		()	{return m_hMutex;}
	BOOL IsValid		()	{return m_hMutex != nullptr;}

private:
	CMTX(const CMTX& mtx);
	CMTX operator = (const CMTX& mtx);

private:
	HANDLE m_hMutex;
};

class CSpinGuard
{
public:
	CSpinGuard() : m_lFlag(0)
	{

	}

	~CSpinGuard()
	{
		ASSERT(m_lFlag == 0);
	}

	void Lock()
	{
		for(UINT i = 0; !TryLock(); ++i)
			YieldThread(i);
	}

	BOOL TryLock()
	{
		if(::InterlockedCompareExchange(&m_lFlag, 1, 0) == 0)
		{
			::_ReadWriteBarrier();
			return TRUE;
		}

		return FALSE;
	}

	void Unlock()
	{
		ASSERT(m_lFlag == 1);
		m_lFlag = 0;
	}

private:
	CSpinGuard(const CSpinGuard& cs);
	CSpinGuard operator = (const CSpinGuard& cs);

private:
	volatile LONG m_lFlag;
};

class CReentrantSpinGuard
{
public:
	CReentrantSpinGuard()
	: m_dwThreadID	(0)
	, m_iCount		(0)
	{

	}

	~CReentrantSpinGuard()
	{
		ASSERT(m_dwThreadID	== 0);
		ASSERT(m_iCount		== 0);
	}

	void Lock()
	{
		for(UINT i = 0; !_TryLock(i == 0); ++i)
			YieldThread(i);
	}

	BOOL TryLock()
	{
		return _TryLock(TRUE);
	}

	void Unlock()
	{
		ASSERT(m_dwThreadID == ::GetCurrentThreadId());

		if((--m_iCount) == 0)
			m_dwThreadID = 0;
	}

private:
	CReentrantSpinGuard(const CReentrantSpinGuard& cs);
	CReentrantSpinGuard operator = (const CReentrantSpinGuard& cs);

	BOOL _TryLock(BOOL bFirst)
	{
		DWORD dwCurrentThreadID = ::GetCurrentThreadId();

		if(bFirst && m_dwThreadID == dwCurrentThreadID)
		{
			++m_iCount;
			return TRUE;
		}

		if(::InterlockedCompareExchange(&m_dwThreadID, dwCurrentThreadID, 0) == 0)
		{
			::_ReadWriteBarrier();
			ASSERT(m_iCount == 0);

			m_iCount = 1;

			return TRUE;
		}

		return FALSE;
	}

private:
	volatile DWORD	m_dwThreadID;
	int				m_iCount;
};

class CFakeGuard
{
public:
	void Lock()		{}
	void Unlock()	{}
	BOOL TryLock()	{return TRUE;}
};

template<class CLockObj> class CLocalLock
{
public:
	CLocalLock(CLockObj& obj) : m_lock(obj) {m_lock.Lock();}
	~CLocalLock() {m_lock.Unlock();}
private:
	CLockObj& m_lock;
};

typedef CInterCriSec					CCriSec;

typedef CLocalLock<CCriSec>				CCriSecLock;
typedef CLocalLock<CInterCriSec>		CInterCriSecLock;
typedef CLocalLock<CInterCriSec2>		CInterCriSecLock2;
typedef CLocalLock<CMTX>				CMutexLock;
typedef CLocalLock<CSpinGuard>			CSpinLock;
typedef CLocalLock<CReentrantSpinGuard>	CReentrantSpinLock;
typedef	CLocalLock<CFakeGuard>			CFakeLock;
