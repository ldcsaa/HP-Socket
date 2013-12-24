/*
 * Copyright Bruce Liang (ldcsaa@gmail.com)
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

#include "StdAfx.h"
#include "RWLock.h"

///////////////////////////////// CRWLock Functions ///////////////////////////////////

CSWMR::CSWMR()
{
	// Initially no readers want access, no writers want access, and 
	// no threads are accessing the resource
	m_nWaitingReaders = m_nWaitingWriters = m_nActive = 0;
	m_hsemReaders = ::CreateSemaphore(NULL, 0, MAXLONG, NULL);
	m_hsemWriters = ::CreateSemaphore(NULL, 0, MAXLONG, NULL);
	::InitializeCriticalSection(&m_cs);
}

CSWMR::~CSWMR()
{
	// A SWMRG shouldn't be destroyed if any threads are using the resource
	ASSERT(m_nActive == 0);

	m_nWaitingReaders = m_nWaitingWriters = m_nActive = 0;
	::DeleteCriticalSection(&m_cs);
	::CloseHandle(m_hsemReaders);
	::CloseHandle(m_hsemWriters);
}

VOID CSWMR::WaitToRead()
{
	// Ensure exclusive access to the member variables
	::EnterCriticalSection(&m_cs);

	// Are there writers waiting or is a writer writing?
	BOOL fResourceWritePending = (m_nWaitingWriters || (m_nActive < 0));

	if(fResourceWritePending)
	{
		// This reader must wait, increment the count of waiting readers
		m_nWaitingReaders++;
	}
	else
	{
		// This reader can read, increment the count of active readers
		m_nActive++;
	}

	// Allow other threads to attempt reading/writing
	::LeaveCriticalSection(&m_cs);

	if(fResourceWritePending)
	{
		// This thread must wait
		::WaitForSingleObject(m_hsemReaders, INFINITE);
	}
}

VOID CSWMR::WaitToWrite()
{
	// Ensure exclusive access to the member variables
	::EnterCriticalSection(&m_cs);

	// Are there any threads accessing the resource?
	BOOL fResourceOwned = (m_nActive != 0);

	if(fResourceOwned)
	{
		// This writer must wait, increment the count of waiting writers
		m_nWaitingWriters++;
	}
	else
	{
		// This writer can write, decrement the count of active writers
		m_nActive = -1;
	}

	// Allow other threads to attempt reading/writing
	::LeaveCriticalSection(&m_cs);

	if(fResourceOwned)
	{
		// This thread must wait
		::WaitForSingleObject(m_hsemWriters, INFINITE);
	}
}

VOID CSWMR::Done()
{
	// Ensure exclusive access to the member variables
	::EnterCriticalSection(&m_cs);

	if(m_nActive > 0)
	{
		// Readers have control so a reader must be done
		m_nActive--;
	}
	else
	{
		// Writers have control so a writer must be done
		m_nActive++;
	}

	HANDLE hsem = NULL;  // Assume no threads are waiting
	LONG lCount = 1;	  // Assume only 1 waiter wakes; always true for writers

	if(m_nActive == 0)
	{
		// No thread has access, who should wake up?
		// NOTE: It is possible that readers could never get access
		//		 if there are always writers wanting to write

		if(m_nWaitingWriters > 0)
		{
			// Writers are waiting and they take priority over readers
			m_nActive = -1;			// A writer will get access
			m_nWaitingWriters--;	// One less writer will be waiting
			hsem = m_hsemWriters;	// Writers wait on this semaphore
			// NOTE: The semaphore will release only 1 writer thread
		}
		else if(m_nWaitingReaders > 0)
		{
			// Readers are waiting and no writers are waiting
			m_nActive = m_nWaitingReaders;	// All readers will get access
			m_nWaitingReaders = 0;			// No readers will be waiting
			hsem = m_hsemReaders;			// Readers wait on this semaphore
			lCount = m_nActive;				// Semaphore releases all readers
		}
		else
		{
			// There are no threads waiting at all; no semaphore gets released
		}
	}

	// Allow other threads to attempt reading/writing
	::LeaveCriticalSection(&m_cs);

	if(hsem != NULL)
	{
		// Some threads are to be released
		::ReleaseSemaphore(hsem, lCount, NULL);
	}
}

//////////////////////////////// CRWLock Functions //////////////////////////////////


CRWLock::CRWLock()
: m_nWaitingReaders(0)
, m_nWaitingWriters(0)
, m_nActive(0)
, m_dwWriterTID(0)
, m_smRead(MAXLONG)
, m_smWrite(1)
{

}

CRWLock::~CRWLock()
{
	ASSERT(m_nActive	 == 0);
	ASSERT(m_dwWriterTID == 0);
}

VOID CRWLock::WaitToRead()
{
	BOOL bWait = FALSE;

	{
		CCriSecLock locallock(m_cs);

		if(m_nActive > 0)
			++m_nActive;
		else if(m_nActive == 0)
		{
			if(m_nWaitingWriters == 0)
				++m_nActive;
			else
			{
				++m_nWaitingReaders;
				bWait = TRUE;
			}
		}
		else
		{
			if(!IsOwner())
			{
				++m_nWaitingReaders;
				bWait = TRUE;
			}
		}
	}

	if(bWait)
	{
		m_smRead.Wait();
	}
}

VOID CRWLock::WaitToWrite()
{
	BOOL bWait = FALSE;

	{
		CCriSecLock locallock(m_cs);

		if(m_nActive > 0)
		{
			++m_nWaitingWriters;
			bWait = TRUE;
		}
		else if(m_nActive == 0)
		{
			--m_nActive;
			SetOwner();
		}
		else
		{
			if(IsOwner())
				--m_nActive;
			else
			{
				++m_nWaitingWriters;
				bWait = TRUE;
			}
		}
	}

	if(bWait)
	{
		m_smWrite.Wait();
		SetOwner();
	}
}

VOID CRWLock::ReadDone()
{
	CCriSecLock locallock(m_cs);

	ASSERT(m_nActive != 0);

	if(m_nActive > 0)
	{
		if(--m_nActive == 0)
			Done();
	}
	else
		ASSERT(IsOwner());
}

VOID CRWLock::WriteDone()
{
	CCriSecLock locallock(m_cs);

	ASSERT(m_nActive < 0);

	if(++m_nActive == 0)
	{
		DetachOwner();
		Done();
	}
	else
		ASSERT(IsOwner());
}

VOID CRWLock::Done()
{
	ASSERT(m_nActive	 == 0);
	ASSERT(m_dwWriterTID == 0);

	CSEM* pSem	 = nullptr;
	LONG  lCount = 1;

	{
		CCriSecLock locallock(m_cs);

		if(m_nWaitingWriters > 0)
		{
			--m_nActive;
			--m_nWaitingWriters;
			pSem = &m_smWrite;
		}
		else if(m_nWaitingReaders > 0)
		{
			m_nActive	= m_nWaitingReaders;
			lCount		= m_nWaitingReaders;
			pSem		= &m_smRead;
			m_nWaitingReaders = 0;
		}
	}

	if(pSem != nullptr)
	{
		pSem->Release(lCount);
	}
}

/////////////////////////////////// End of File /////////////////////////////////////
