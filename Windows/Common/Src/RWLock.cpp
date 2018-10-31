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

////////////////////////////////// CSWMR Functions ////////////////////////////////////

CSWMR::CSWMR()
: m_smRead			(MAXLONG)
, m_smWrite			(1)
, m_nWaitingReaders	(0)
, m_nWaitingWriters	(0)
, m_nActive			(0)
{

}

CSWMR::~CSWMR()
{
	ASSERT(m_nWaitingReaders == 0);
	ASSERT(m_nWaitingWriters == 0);
	ASSERT(m_nActive		 == 0);
}

VOID CSWMR::WaitToRead()
{
	BOOL fResourceWritePending;

	{
		CSpinLock locallock(m_cs);

		fResourceWritePending = (m_nWaitingWriters || (m_nActive < 0));

		if(fResourceWritePending)
			++m_nWaitingReaders;
		else
			++m_nActive;
	}

	if(fResourceWritePending)
		m_smRead.Wait();
}

VOID CSWMR::WaitToWrite()
{
	BOOL fResourceOwned;

	{
		CSpinLock locallock(m_cs);

		fResourceOwned = (m_nActive != 0);

		if(fResourceOwned)
			++m_nWaitingWriters;
		else
			m_nActive = -1;
	}

	if(fResourceOwned)
		m_smWrite.Wait();
}

VOID CSWMR::Done()
{
	CSEM* pSem  = nullptr;
	LONG lCount = 1;

	{
		CSpinLock locallock(m_cs);

		if(m_nActive > 0)
			--m_nActive;
		else
			++m_nActive;

		if(m_nActive == 0)
		{
			if(m_nWaitingWriters > 0)
			{
				m_nActive = -1;
				--m_nWaitingWriters;
				pSem = &m_smWrite;
			}
			else if(m_nWaitingReaders > 0)
			{
				m_nActive = m_nWaitingReaders;
				m_nWaitingReaders = 0;
				pSem = &m_smRead;
				lCount = m_nActive;
			}
		}
	}

	if(pSem != nullptr)
		pSem->Release(lCount);
}

//////////////////////////////// CRWLock Functions //////////////////////////////////

#if _WIN32_WINNT >= _WIN32_WINNT_WS08

CSlimRWLock::CSlimRWLock()
	: m_nActive			(0)
	, m_nReadCount		(0)
	, m_dwWriterTID		(0)
{

}

CSlimRWLock::~CSlimRWLock()
{
	ASSERT(m_nActive	 == 0);
	ASSERT(m_nReadCount	 == 0);
	ASSERT(m_dwWriterTID == 0);
}

VOID CSlimRWLock::WaitToRead()
{
	BOOL bWait = FALSE;

	{
		CSpinLock locallock(m_cs);

		if(m_nActive > 0)
			++m_nActive;
		else if(m_nActive == 0)
		{
			if(m_smLock.TryWaitToRead())
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
		m_smLock.WaitToRead();

		CSpinLock locallock(m_cs);
		{
			++m_nReadCount;
			++m_nActive;
		}
	}
}

VOID CSlimRWLock::WaitToWrite()
{
	BOOL bWait = FALSE;

	{
		CSpinLock locallock(m_cs);

		if(m_nActive > 0)
			bWait = TRUE;
		else if(m_nActive == 0)
		{
			if(m_smLock.TryWaitToWrite())
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
		m_smLock.WaitToWrite();

		SetOwner();
		--m_nActive;
	}
}

VOID CSlimRWLock::ReadDone()
{
	ASSERT(m_nActive != 0);

	if(m_nActive > 0)
	{
		ASSERT(m_nReadCount > 0);

		CSpinLock locallock(m_cs);

		if(--m_nActive == 0)
		{
			for(; m_nReadCount > 0; --m_nReadCount)
				m_smLock.ReadDone();
		}
	}
	else
		ASSERT(IsOwner());
}

VOID CSlimRWLock::WriteDone()
{
	ASSERT(m_nActive < 0);

	CSpinLock locallock(m_cs);

	if(++m_nActive == 0)
	{
		DetachOwner();
		m_smLock.WriteDone();
	}
	else
		ASSERT(IsOwner());
}

#endif

CSEMRWLock::CSEMRWLock()
: m_smRead			(MAXLONG)
, m_smWrite			(1)
, m_nWaitingReaders	(0)
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
	BOOL bWait = FALSE;

	{
		CSpinLock locallock(m_cs);

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

VOID CSEMRWLock::WaitToWrite()
{
	BOOL bWait = FALSE;

	{
		CSpinLock locallock(m_cs);

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

VOID CSEMRWLock::ReadDone()
{
	CSEM* pSem	 = nullptr;
	LONG  lCount = 0;

	ASSERT(m_nActive != 0);

	if(m_nActive > 0)
	{
		CSpinLock locallock(m_cs);

		if(--m_nActive == 0)
			Done(&pSem, lCount);
	}
	else
		ASSERT(IsOwner());

	if(pSem != nullptr)
		pSem->Release(lCount);
}

VOID CSEMRWLock::WriteDone()
{
	CSEM* pSem	 = nullptr;
	LONG  lCount = 0;

	{
		ASSERT(m_nActive < 0);

		CSpinLock locallock(m_cs);

		if(++m_nActive == 0)
		{
			DetachOwner();
			Done(&pSem, lCount);
		}
		else
			ASSERT(IsOwner());
	}

	if(pSem != nullptr)
		pSem->Release(lCount);
}

VOID CSEMRWLock::Done(CSEM** ppSem, LONG& lCount)
{
	ASSERT(m_nActive	 == 0);
	ASSERT(m_dwWriterTID == 0);

	if(m_nWaitingWriters > 0)
	{
		--m_nActive;
		--m_nWaitingWriters;

		lCount = 1;
		*ppSem = &m_smWrite;
	}
	else if(m_nWaitingReaders > 0)
	{
		m_nActive			= m_nWaitingReaders;
		m_nWaitingReaders	= 0;
		lCount				= m_nActive;
		*ppSem				= &m_smRead;
	}
}

/////////////////////////////////// End of File /////////////////////////////////////
