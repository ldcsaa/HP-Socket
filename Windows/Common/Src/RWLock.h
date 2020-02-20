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
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "CriticalSection.h"
#include "Semaphore.h"

class CSWMR
{
public:
	VOID WaitToRead();
	VOID WaitToWrite();
	VOID ReadDone()  {Done();}
	VOID WriteDone() {Done();}

private:
	VOID Done();

public:
	CSWMR();
	~CSWMR();

private:
	CSWMR(const CSWMR&);
	CSWMR operator = (const CSWMR&);

private:
	int m_nWaitingReaders;
	int m_nWaitingWriters;
	int m_nActive;

	CSpinGuard	m_cs;
	CSEM		m_smRead;
	CSEM		m_smWrite;
};

#if _WIN32_WINNT >= _WIN32_WINNT_WS08

class CSlimLock
{
public:
	VOID WaitToRead()		{::AcquireSRWLockShared(&m_lock);}
	VOID WaitToWrite()		{::AcquireSRWLockExclusive(&m_lock);}
	VOID ReadDone()			{::ReleaseSRWLockShared(&m_lock);}
	VOID WriteDone()		{::ReleaseSRWLockExclusive(&m_lock);}
	BOOL TryWaitToRead()	{return ::TryAcquireSRWLockShared(&m_lock);}
	BOOL TryWaitToWrite()	{return ::TryAcquireSRWLockExclusive(&m_lock);}

	SRWLOCK* GetObject()	{return &m_lock;}

public:
	CSlimLock()		{::InitializeSRWLock(&m_lock);}
	~CSlimLock()	{}

private:
	CSlimLock(const CSlimLock&);
	CSlimLock operator = (const CSlimLock&);

private:
	SRWLOCK m_lock;
};

class CSlimRWLock
{
public:
	VOID WaitToRead();
	VOID WaitToWrite();
	VOID ReadDone();
	VOID WriteDone();

private:
	BOOL IsOwner()		{return m_dwWriterTID == ::GetCurrentThreadId();}
	VOID SetOwner()		{m_dwWriterTID = ::GetCurrentThreadId();}
	VOID DetachOwner()	{m_dwWriterTID = 0;}

public:
	CSlimRWLock();
	~CSlimRWLock();

private:
	CSlimRWLock(const CSlimRWLock&);
	CSlimRWLock operator = (const CSlimRWLock&);

private:
	int m_nActive;
	DWORD m_dwWriterTID;

	CSpinGuard	m_cs;
	CSlimLock	m_smLock;
};

#endif

class CSEMRWLock
{
public:
	VOID WaitToRead();
	VOID WaitToWrite();
	VOID ReadDone();
	VOID WriteDone();

private:
	VOID Done			(CSEM** ppSem, LONG& lCount);
	BOOL IsOwner()		{return m_dwWriterTID == ::GetCurrentThreadId();}
	VOID SetOwner()		{m_dwWriterTID = ::GetCurrentThreadId();}
	VOID DetachOwner()	{m_dwWriterTID = 0;}

public:
	CSEMRWLock();
	~CSEMRWLock();

private:
	CSEMRWLock(const CSEMRWLock&);
	CSEMRWLock operator = (const CSEMRWLock&);

private:
	int m_nWaitingReaders;
	int m_nWaitingWriters;
	int m_nActive;
	DWORD m_dwWriterTID;

	CSpinGuard	m_cs;
	CSEM		m_smRead;
	CSEM		m_smWrite;
};

template<class CLockObj> class CLocalReadLock
{
public:
	CLocalReadLock(CLockObj& obj) : m_wait(obj) {m_wait.WaitToRead();}
	~CLocalReadLock() {m_wait.ReadDone();}
private:
	CLocalReadLock(const CLocalReadLock&);
	CLocalReadLock operator = (const CLocalReadLock&);
private:
	CLockObj& m_wait;
};

template<class CLockObj> class CLocalWriteLock
{
public:
	CLocalWriteLock(CLockObj& obj) : m_wait(obj) {m_wait.WaitToWrite();}
	~CLocalWriteLock() {m_wait.WriteDone();}
private:
	CLocalWriteLock(const CLocalWriteLock&);
	CLocalWriteLock operator = (const CLocalWriteLock&);
private:
	CLockObj& m_wait;
};

#if _WIN32_WINNT >= _WIN32_WINNT_WS08
	typedef CSlimLock	CSimpleRWLock;
#else
	typedef CSWMR		CSimpleRWLock;
#endif

typedef CLocalReadLock<CSimpleRWLock>	CReadLock;
typedef CLocalWriteLock<CSimpleRWLock>	CWriteLock;

typedef CSEMRWLock						CRWLock;
typedef CLocalReadLock<CRWLock>			CReentrantReadLock;
typedef CLocalWriteLock<CRWLock>		CReentrantWriteLock;

#if _WIN32_WINNT >= _WIN32_WINNT_WS08

class ICVCondition
{
public:
	virtual BOOL Detect() = 0;

public:
	virtual ~ICVCondition() {}
};

class CCVCriSec
{
public:
	CCVCriSec(CInterCriSec& cs)
	: m_cs(cs)
	{
		::InitializeConditionVariable(&m_cv);
	}

	~CCVCriSec() {}

	BOOL WaitToRead(ICVCondition* pCondition, DWORD dwMilliseconds = INFINITE)
	{
		Wait(pCondition, dwMilliseconds);
	}

	BOOL WaitToWrite(ICVCondition* pCondition, DWORD dwMilliseconds = INFINITE)
	{
		Wait(pCondition, dwMilliseconds);
	}

	void ReadDone()
	{
		Done();
	}

	void WriteDone()
	{
		Done();
	}

	void WakeUp()
	{
		::WakeConditionVariable(&m_cv);
	}

	void WakeUpAll()
	{
		::WakeAllConditionVariable(&m_cv);
	}

	BOOL Wait(ICVCondition* pCondition, DWORD dwMilliseconds = INFINITE)
	{
		ASSERT(pCondition);

		m_cs.Lock();

		while(!pCondition->Detect())
		{
			if(!::SleepConditionVariableCS(&m_cv, m_cs.GetObject(), dwMilliseconds))
				return FALSE;
		}

		return TRUE;
	}

	void Done()
	{
		m_cs.Unlock();
	}

	CInterCriSec& GetLock()
	{
		return m_cs;
	}

private:
	CCVCriSec(const CCVCriSec& cs);
	CCVCriSec operator = (const CCVCriSec& cs);

private:
	CInterCriSec&		m_cs;
	CONDITION_VARIABLE	m_cv;
};

class CCVSlim
{
public:
	CCVSlim(CSlimLock& cs)
	: m_cs(cs)
	{
		::InitializeConditionVariable(&m_cv);
	}

	~CCVSlim() {}

	BOOL WaitToRead(ICVCondition* pCondition, DWORD dwMilliseconds = INFINITE)
	{
		ASSERT(pCondition);

		m_cs.WaitToRead();

		while(!pCondition->Detect()) 
		{
			if(!::SleepConditionVariableSRW(&m_cv, m_cs.GetObject(), dwMilliseconds, CONDITION_VARIABLE_LOCKMODE_SHARED))
				return FALSE;
		}

		return TRUE;
	}

	BOOL WaitToWrite(ICVCondition* pCondition, DWORD dwMilliseconds = INFINITE)
	{
		ASSERT(pCondition);

		m_cs.WaitToWrite();

		while(!pCondition->Detect()) 
		{
			if(!::SleepConditionVariableSRW(&m_cv, m_cs.GetObject(), dwMilliseconds, 0))
				return FALSE;
		}

		return TRUE;
	}

	void ReadDone()
	{
		m_cs.ReadDone();
	}

	void WriteDone()
	{
		m_cs.WriteDone();
	}

	void WakeUp()
	{
		::WakeConditionVariable(&m_cv);
	}

	void WakeUpAll()
	{
		::WakeAllConditionVariable(&m_cv);
	}

	CSlimLock& GetLock()
	{
		return m_cs;
	}

private:
	CCVSlim(const CCVSlim& cs);
	CCVSlim operator = (const CCVSlim& cs);

private:
	CSlimLock&			m_cs;
	CONDITION_VARIABLE	m_cv;
};

template<class _Lock, class _Var> class CCVGuard
{
public:
	BOOL WaitForProduce(DWORD dwMilliseconds = INFINITE)
	{
		m_cvP.WaitToWrite(m_pcdtProduce, dwMilliseconds);
	}

	BOOL WaitForConsume(DWORD dwMilliseconds = INFINITE)
	{
		m_cvC.WaitToRead(m_pcdtConsume, dwMilliseconds);
	}

	void ProduceDone()
	{
		m_cvP.WriteDone();
	}

	void WakeUpProduce()
	{
		m_cvP.WakeUp();
	}

	void WakeUpAllProduces()
	{
		m_cvP.WakeUpAll();
	}

	void ConsumeDone()
	{
		m_cvC.ReadDone();
	}

	void WakeUpConsume()
	{
		m_cvC.WakeUp();
	}

	void WakeUpAllConsumes()
	{
		m_cvC.WakeUpAll();
	}

public:
	CCVGuard(ICVCondition* pcdtProduce, ICVCondition* pcdtConsume)
	: m_cvP(m_cs)
	, m_cvC(m_cs)
	, m_pcdtProduce(pcdtProduce)
	, m_pcdtConsume(pcdtConsume)
	{
		ASSERT(m_pcdtConsume && m_pcdtProduce);
	}

	~CCVGuard()	{}

private:
	CCVGuard(const CCVGuard& cs);
	CCVGuard operator = (const CCVGuard& cs);

private:
	ICVCondition* m_pcdtProduce;
	ICVCondition* m_pcdtConsume;

	_Lock	m_cs;
	_Var	m_cvP;
	_Var	m_cvC;
};

template<class _GuardObj> class CConsumeLock
{
public:
	CConsumeLock(_GuardObj& obj) : m_guard(obj) {m_guard.WaitForConsume();}
	~CConsumeLock() {m_guard.ConsumeDone();}
private:
	CConsumeLock(const CConsumeLock&);
	CConsumeLock operator = (const CConsumeLock&);
private:
	_GuardObj& m_guard;
};

template<class _GuardObj> class CProduceLock
{
public:
	CProduceLock(_GuardObj& obj) : m_guard(obj) {m_guard.WaitForProduce();}
	~CProduceLock() {m_guard.ProduceDone();}
private:
	CProduceLock(const CProduceLock&);
	CProduceLock operator = (const CProduceLock&);
private:
	_GuardObj& m_guard;
};

typedef CCVGuard<CInterCriSec, CCVCriSec>	CCVGuardCS;
typedef CCVGuard<CSlimLock, CCVSlim>		CCVGuardSRW;
typedef CProduceLock<CCVGuardCS>			CProduceLockCS;
typedef CConsumeLock<CCVGuardCS>			CConsumeLockCS;
typedef CProduceLock<CCVGuardSRW>			CProduceLockSRW;
typedef CConsumeLock<CCVGuardSRW>			CConsumeLockSRW;

#endif
