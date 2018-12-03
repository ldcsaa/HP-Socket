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
#include "CriSec.h"

#include <shared_mutex>
#include <condition_variable>

using namespace std;

class CMutexRWLock
{
public:
	VOID WaitToRead();
	VOID WaitToWrite();
	VOID ReadDone();
	VOID WriteDone();

private:
	BOOL IsOwner()		{return ::IsSelfThread(m_dwWriterTID);}
	VOID SetOwner()		{m_dwWriterTID = SELF_THREAD_ID;}
	VOID DetachOwner()	{m_dwWriterTID = 0;}

public:
	CMutexRWLock();
	~CMutexRWLock();

	DECLARE_NO_COPY_CLASS(CMutexRWLock)

private:
	int		m_nActive;
	int		m_nReadCount;
	THR_ID	m_dwWriterTID;

	CSpinGuard			m_cs;
	shared_timed_mutex	m_mtx;
};

class CSEMRWLock
{
public:
	VOID WaitToRead();
	VOID WaitToWrite();
	VOID ReadDone();
	VOID WriteDone();

private:
	BOOL IsOwner()		{BOOL bOwner = ::IsSelfThread(m_dwWriterTID); ASSERT(!bOwner || m_nActive < 0); return bOwner;}
	VOID SetOwner()		{m_dwWriterTID = SELF_THREAD_ID;}
	VOID DetachOwner()	{m_dwWriterTID = 0;}

public:
	CSEMRWLock();
	~CSEMRWLock();

	DECLARE_NO_COPY_CLASS(CSEMRWLock)

private:
	int		m_nWaitingReaders;
	int		m_nWaitingWriters;
	int		m_nActive;
	THR_ID	m_dwWriterTID;

	CMTX				m_mtx;
	condition_variable	m_cvRead;
	condition_variable	m_cvWrite;
};

template<class CLockObj> class CLocalReadLock
{
public:
	CLocalReadLock(CLockObj& obj) : m_wait(obj) {m_wait.WaitToRead();}
	~CLocalReadLock() {m_wait.ReadDone();}

	DECLARE_NO_COPY_CLASS(CLocalReadLock)

private:
	CLockObj& m_wait;
};

template<class CLockObj> class CLocalWriteLock
{
public:
	CLocalWriteLock(CLockObj& obj) : m_wait(obj) {m_wait.WaitToWrite();}
	~CLocalWriteLock() {m_wait.WriteDone();}

	DECLARE_NO_COPY_CLASS(CLocalWriteLock)

private:
	CLockObj& m_wait;
};


using CSimpleRWLock = shared_timed_mutex;
using CReadLock		= shared_lock<shared_timed_mutex>;
using CWriteLock	= lock_guard<shared_timed_mutex>;
using CWriteLock2	= unique_lock<shared_timed_mutex>;

#if !defined(_USE_MUTEX_RW_LOCK)
	using CRWLock = CSEMRWLock;
#else
	using CRWLock = CMutexRWLock;
#endif

using CReentrantReadLock	= CLocalReadLock<CRWLock>;
using CReentrantWriteLock	= CLocalWriteLock<CRWLock>;
