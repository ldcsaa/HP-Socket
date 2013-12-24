/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.2
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * WeiBo	: http://weibo.com/u/1402935851
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
	VOID WaitToRead();			// Call this to gain shared read access
	VOID WaitToWrite();			// Call this to gain exclusive write access
	VOID ReadDone()  {Done();}	// Call this when done reading the resource
	VOID WriteDone() {Done();}	// Call this when done writing the resource

private:
	VOID Done();				// Call this when done accessing the resource

public:
	CSWMR();					// Constructor
	~CSWMR();					// Destructor

private:
	CSWMR(const CSWMR&);
	CSWMR operator = (const CSWMR&);

private:
	CRITICAL_SECTION m_cs;		// Permits exclusive access to other members
	HANDLE m_hsemReaders;		// Readers wait on this if a writer has access
	HANDLE m_hsemWriters;		// Writers wait on this if a reader has access
	int m_nWaitingReaders;		// Number of readers waiting for access
	int m_nWaitingWriters;		// Number of writers waiting for access
	int m_nActive;				// Number of threads currently with access
								//	(0=no threads, >0=# of readers, -1=1 writer)
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

typedef CLocalReadLock<CSWMR>		CReadLock;
typedef CLocalWriteLock<CSWMR>		CWriteLock;
typedef CLocalReadLock<CRWLock>		CReentrantReadLock;
typedef CLocalWriteLock<CRWLock>	CReentrantWriteLock;
