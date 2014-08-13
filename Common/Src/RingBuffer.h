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
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#pragma once

#include "CriticalSection.h"

#define CACHE_LINE		64
#define PACK_SIZE_OF(T)	(CACHE_LINE - sizeof(T) % CACHE_LINE)

#if !defined (_WIN64)
	#pragma pack(push, 4)
#endif

template <class T, class _PutGuard = CCriSec, class _GetGuard = CCriSec> class CRingBuffer
{
public:
	static const UINT DEFAULT_SIZE = 4096;

public:
	BOOL Put(T* pElement)
	{
		ASSERT(pElement != nullptr);

		{
			CLocalLock<_PutGuard> locallock(m_csPut);

			ULONGLONG seqPut = m_seqPut;

			WaitForPut(seqPut);
			if(!IsValid()) return FALSE;

			DoPut(pElement, seqPut);
		}

		return TRUE;
	}

	BOOL TryPut(T* pElement)
	{
		ASSERT(pElement != nullptr);

		if(!IsValid() || !HasPutSpace(m_seqPut))
			return FALSE;

		{
			CLocalLock<_PutGuard> locallock(m_csPut);

			ULONGLONG seqPut = m_seqPut;

			if(!IsValid() || !HasPutSpace(seqPut))
				return FALSE;

			DoPut(pElement, seqPut);
		}

		return TRUE;
	}

	BOOL PutBatch(T* pElements[], int& iCount)
	{
		ASSERT(pElements != nullptr && iCount > 0);

		{
			CLocalLock<_PutGuard> locallock(m_csPut);

			ULONGLONG seqPut = m_seqPut;

			for(int i = 0; i < iCount; ++i)
			{
				WaitForPut(seqPut);

				if(!IsValid())
				{
					iCount = i;
					return FALSE;
				}

				DoPut(*(pElements + i), seqPut);
			}
		}

		return TRUE;
	}

	BOOL TryPutBatch(T* pElements[], int& iCount)
	{
		ASSERT(pElements != nullptr && iCount > 0);

		if(!IsValid() || !HasPutSpace(m_seqPut))
		{
			iCount = 0;
			return FALSE;
		}

		{
			CLocalLock<_PutGuard> locallock(m_csPut);

			ULONGLONG seqPut = m_seqPut;

			for(int i = 0; i < iCount; ++i)
			{
				if(!IsValid() || !HasPutSpace(seqPut))
				{
					iCount = i;
					return FALSE;
				}

				DoPut(*(pElements + i), seqPut);
			}
		}

		return TRUE;
	}

	BOOL Get(T** pElement)
	{
		ASSERT(pElement != nullptr);

		{
			CLocalLock<_GetGuard> locallock(m_csGet);

			ULONGLONG seqGet = m_seqGet;

			WaitForGet(seqGet);
			if(!IsValid()) return FALSE;

			DoGet(pElement, seqGet);
		}

		return TRUE;
	}

	BOOL TryGet(T** pElement)
	{
		ASSERT(pElement != nullptr);

		if(!IsValid() || !HasGetSpace(m_seqGet))
			return FALSE;

		{
			CLocalLock<_GetGuard> locallock(m_csGet);

			ULONGLONG seqGet = m_seqGet;

			if(!IsValid() || !HasGetSpace(seqGet))
				return FALSE;

			DoGet(pElement, seqGet);
		}

		return TRUE;
	}

	BOOL GetBatch(T* pElements[], int& iCount)
	{
		ASSERT(pElements != nullptr && iCount > 0);

		{
			CLocalLock<_GetGuard> locallock(m_csGet);

			ULONGLONG seqGet = m_seqGet;

			for(int i = 0; i < iCount; ++i)
			{
				WaitForGet(seqGet);

				if(!IsValid())
				{
					iCount = i;
					return FALSE;
				}

				DoGet(pElements + i, seqGet);
			}
		}

		return TRUE;
	}

	BOOL TryGetBatch(T* pElements[], int& iCount)
	{
		ASSERT(pElements != nullptr && iCount > 0);

		if(!IsValid() || !HasGetSpace(m_seqGet))
		{
			iCount = 0;
			return FALSE;
		}

		{
			CLocalLock<_GetGuard> locallock(m_csGet);

			ULONGLONG seqGet = m_seqGet;

			for(int i = 0; i < iCount; ++i)
			{
				if(!IsValid() || !HasGetSpace(seqGet))
				{
					iCount = i;
					return FALSE;
				}

				DoGet(pElements + i, seqGet);
			}
		}

		return TRUE;
	}

	BOOL Peek(T** pElement)
	{
		ASSERT(pElement != nullptr);

		{
			//CLocalLock<_GetGuard> locallock(m_csGet);

			ULONGLONG seqGet = m_seqGet;

			if(!IsValid() || !HasGetSpace(seqGet))
				return FALSE;

			DoPeek(pElement, seqGet);
		}

		return TRUE;
	}

	BOOL Create(DWORD dwExpect = DEFAULT_SIZE)
	{
		ASSERT(!IsValid() && dwExpect > 0);

		if(IsValid()) return FALSE;

		m_seqPut = 0;
		m_seqGet = 0;
		m_dwReal = Revise(dwExpect);
		m_pv	 = (T**)malloc(m_dwReal * sizeof(T*));
		m_bValid = (m_pv != nullptr);

		return IsValid();
	}

	BOOL Destroy()
	{
		if(IsValid())
		{
			m_bValid = FALSE;

			CLocalLock<_PutGuard> locallock1(m_csPut);
			CLocalLock<_GetGuard> locallock2(m_csGet);

			free((void*)m_pv);
			m_pv	 = nullptr;
			m_dwReal = 0;
			m_seqPut = 0;
			m_seqGet = 0;

			return TRUE;
		}

		return FALSE;
	}

private:
	void DoPut(T* pElement, ULONGLONG& seqPut)
	{
		DWORD index = seqPut & (m_dwReal - 1);
		*(m_pv + index)	= pElement;

		++seqPut;
		m_seqPut = seqPut;
	}

	void DoGet(T** pElement, ULONGLONG& seqGet)
	{
		DWORD index = seqGet & (m_dwReal - 1);
		*(pElement) = *(m_pv + index);

		++seqGet;
		m_seqGet = seqGet;
	}

	void DoPeek(T** pElement, ULONGLONG& seqGet)
	{
		DWORD index = seqGet & (m_dwReal - 1);
		*(pElement) = *(m_pv + index);
	}

	BOOL HasPutSpace(ULONGLONG seqPut)
	{
		return (seqPut - m_seqGet < m_dwReal);
	}

	void WaitForPut(ULONGLONG seqPut)
	{
		for(DWORD w = 0; IsValid(); ++w)
		{
			if(HasPutSpace(seqPut))
				break;

			CSpinGuard::Pause(w);
		}
	}

	BOOL HasGetSpace(ULONGLONG seqGet)
	{
		return (m_seqPut - seqGet > 0);
	}

	void WaitForGet(ULONGLONG seqGet)
	{
		for(DWORD w = 0; IsValid(); ++w)
		{
			if(HasGetSpace(seqGet))
				break;

			CSpinGuard::Pause(w);
		}
	}

	DWORD Revise(DWORD dwExpect)
	{
		int index = 0;
		int shift = sizeof(DWORD) * 8 - 1;

		for(int i = shift; i >= 0; i--)
		{
			if(index == 0)
			{
				if(dwExpect & (1 << i))
				{
					index = i;

					if(index == shift)
						break;
				}
			}
			else
			{
				if(dwExpect & (1 << i))
					++index;

				break;
			}
		}

		return 1 << index;
	}

public:
	CRingBuffer(BOOL bCreate = FALSE, DWORD uiExpect = DEFAULT_SIZE)
	: m_pv(nullptr)
	, m_bValid(FALSE)
	, m_dwReal(0)
	, m_seqPut(0)
	, m_seqGet(0)
	{
		ASSERT(uiExpect > 0);

		if(bCreate)
		{
			Create(uiExpect);
			ASSERT(IsValid());
		}
	}

	~CRingBuffer()
	{
		Destroy();
	}

	BOOL IsValid() {return m_bValid;}

private:
	CRingBuffer(const CRingBuffer&);
	CRingBuffer operator = (const CRingBuffer&);

private:
	BOOL				m_bValid;
	DWORD				m_dwReal;
	T**					m_pv;
	char				pack1[PACK_SIZE_OF(T**)];
	volatile ULONGLONG	m_seqPut;
	char				pack4[PACK_SIZE_OF(ULONGLONG)];
	volatile ULONGLONG	m_seqGet;
	char				pack5[PACK_SIZE_OF(ULONGLONG)];
	_PutGuard			m_csPut;
	char				pack2[PACK_SIZE_OF(_PutGuard)];
	_GetGuard			m_csGet;
	char				pack3[PACK_SIZE_OF(_GetGuard)];
};

#if !defined (_WIN64)
	#pragma pack(pop)
#endif

typedef CRingBuffer<void, CCriSec, CCriSec>				CCSRingBuffer;
typedef CRingBuffer<void, CInterCriSec, CInterCriSec>	CICSRingBuffer;
typedef CRingBuffer<void, CSpinGuard, CSpinGuard>		CSGRingBuffer;
typedef CRingBuffer<void, CFakeGuard, CFakeGuard>		CFKRingBuffer;
