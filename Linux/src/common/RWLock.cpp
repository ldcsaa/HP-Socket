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

#include "RWLock.h"

CMutexRWLock::CMutexRWLock()
	: m_nActive			(0)
	, m_nReadCount		(0)
	, m_dwWriterTID		(0)
{

}

CMutexRWLock::~CMutexRWLock()
{
	ASSERT(m_nActive	 == 0);
	ASSERT(m_nReadCount	 == 0);
	ASSERT(m_dwWriterTID == 0);
}

VOID CMutexRWLock::WaitToRead()
{
	BOOL bWait = FALSE;

	{
		CSpinLock locallock(m_cs);

		if(m_nActive > 0)
			++m_nActive;
		else if(m_nActive == 0)
		{
			if(m_mtx.try_lock_shared())
			{
				++m_nReadCount;
				++m_nActive;
			}
			else
				bWait = TRUE;
		}
		else if(!IsOwner())
			bWait = TRUE;
	}

	if(bWait)
	{
		m_mtx.lock_shared();

		CSpinLock locallock(m_cs);

		{
			++m_nReadCount;
			++m_nActive;
		}
	}
}

VOID CMutexRWLock::WaitToWrite()
{
	BOOL bWait = FALSE;

	{
		CSpinLock locallock(m_cs);

		if(m_nActive > 0)
			bWait = TRUE;
		else if(m_nActive == 0)
		{
			if(m_mtx.try_lock())
			{
				SetOwner();
				--m_nActive;
			}
			else
				bWait = TRUE;
		}
		else
		{
			if(IsOwner())
				--m_nActive;
			else
				bWait = TRUE;
		}
	}

	if(bWait)
	{
		m_mtx.lock();

		SetOwner();
		--m_nActive;
	}
}

VOID CMutexRWLock::ReadDone()
{
	ASSERT(m_nActive != 0);

	if(m_nActive > 0)
	{
		ASSERT(m_nReadCount > 0);

		CSpinLock locallock(m_cs);

		if(--m_nActive == 0)
		{
			for(; m_nReadCount > 0; --m_nReadCount)
				m_mtx.unlock_shared();
		}
	}
	else
		ASSERT(IsOwner());
}

VOID CMutexRWLock::WriteDone()
{
	ASSERT(IsOwner());
	ASSERT(m_nActive < 0);

	CSpinLock locallock(m_cs);

	if(++m_nActive == 0)
	{
		DetachOwner();
		m_mtx.unlock();
	}		
}

CSEMRWLock::CSEMRWLock()
	: m_nWaitingReaders	(0)
	, m_nWaitingWriters	(0)
	, m_nActive			(0)
	, m_dwWriterTID		(0)
{

}

CSEMRWLock::~CSEMRWLock()
{
	ASSERT(m_nActive	 == 0);
	ASSERT(m_dwWriterTID == 0);
}

VOID CSEMRWLock::WaitToRead()
{
	CMutexLock2 lock(m_mtx);

	if(IsOwner())
		return;

	++m_nWaitingReaders;

	m_cvRead.wait(lock, [=]() -> BOOL
	{
		return m_nActive >= 0 && m_nWaitingWriters == 0;
	});

	--m_nWaitingReaders;
	++m_nActive;
}

VOID CSEMRWLock::WaitToWrite()
{
	CMutexLock2 lock(m_mtx);

	if(IsOwner())
	{
		--m_nActive;

		return;
	}

	++m_nWaitingWriters;

	m_cvWrite.wait(lock, [=]() -> BOOL
	{
		return m_nActive == 0;
	});

	--m_nWaitingWriters;
	--m_nActive;

	SetOwner();
}

VOID CSEMRWLock::ReadDone()
{
	CMutexLock2 locallock(m_mtx);

	if(IsOwner())
		return;

	ASSERT(m_nActive > 0);

	if(--m_nActive == 0 && m_nWaitingWriters > 0)
		m_cvWrite.notify_one();
}

VOID CSEMRWLock::WriteDone()
{
	ASSERT(IsOwner());

	CMutexLock2 lock(m_mtx);

	if(++m_nActive == 0)
	{
		DetachOwner();

		if(m_nWaitingWriters > 0)
			m_cvWrite.notify_one();
		else if(m_nWaitingReaders > 0)
			m_cvRead.notify_all();
	}
}
