/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.7
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 75375912
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

#endif

class CRWLock
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
	CRWLock();
	~CRWLock();

private:
	CRWLock(const CRWLock&);
	CRWLock operator = (const CRWLock&);

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

	void WaitToRead(ICVCondition* pCondition)
	{
		Wait(pCondition);
	}

	void WaitToWrite(ICVCondition* pCondition)
	{
		Wait(pCondition);
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

private:
	void Wait(ICVCondition* pCondition)
	{
		ASSERT(pCondition);

		m_cs.Lock();

		while(!pCondition->Detect()) 
			::SleepConditionVariableCS(&m_cv, m_cs.GetObject(), INFINITE);
	}

	void Done()
	{
		m_cs.Unlock();
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

	void WaitToRead(ICVCondition* pCondition)
	{
		ASSERT(pCondition);

		m_cs.WaitToRead();

		while(!pCondition->Detect()) 
			::SleepConditionVariableSRW(&m_cv, m_cs.GetObject(), INFINITE, CONDITION_VARIABLE_LOCKMODE_SHARED);
	}

	void WaitToWrite(ICVCondition* pCondition)
	{
		ASSERT(pCondition);

		m_cs.WaitToWrite();

		while(!pCondition->Detect())  
			::SleepConditionVariableSRW(&m_cv, m_cs.GetObject(), INFINITE, 0);
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
	void WaitForProduce()
	{
		m_cvP.WaitToWrite(m_pcdtProduce);
	}

	void WaitForConsume()
	{
		m_cvC.WaitToRead(m_pcdtConsume);
	}

	void ProduceDone()
	{
		m_cvP.WriteDone();
	}

	void WakeUpProduce()
	{
		m_cvP.WakeUp();
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
