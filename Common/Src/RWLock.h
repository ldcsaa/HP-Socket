/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.5
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

#include "GeneralHelper.h"

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
	CRITICAL_SECTION m_cs;
	HANDLE m_hsemReaders;
	HANDLE m_hsemWriters;
	int m_nWaitingReaders;
	int m_nWaitingWriters;
	int m_nActive;
};

class CSlimLock
{
public:
	VOID WaitToRead()		{::AcquireSRWLockShared(&m_lock);}
	VOID WaitToWrite()		{::AcquireSRWLockExclusive(&m_lock);}
	VOID ReadDone()			{::ReleaseSRWLockShared(&m_lock);}
	VOID WriteDone()		{::ReleaseSRWLockExclusive(&m_lock);}
	BOOL TryWaitToRead()	{return ::TryAcquireSRWLockShared(&m_lock);}
	BOOL TryWaitToWrite()	{return ::TryAcquireSRWLockExclusive(&m_lock);}

public:
	CSlimLock()		{::InitializeSRWLock(&m_lock);}
	~CSlimLock()	{}

private:
	CSlimLock(const CSlimLock&);
	CSlimLock operator = (const CSlimLock&);

private:
	SRWLOCK m_lock;
};

class CRWLock
{
public:
	VOID WaitToRead();
	VOID WaitToWrite();
	VOID ReadDone();
	VOID WriteDone();

private:
	VOID Done();
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
	CCriSec	m_cs;
	CSEM	m_smRead;
	CSEM	m_smWrite;

	int m_nWaitingReaders;
	int m_nWaitingWriters;
	int m_nActive;

	DWORD m_dwWriterTID;
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
