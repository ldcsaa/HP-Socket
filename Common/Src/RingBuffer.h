/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.11
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
	static const UINT DEFAULT_EXPECT = 4096;

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

	BOOL Create(DWORD dwExpect = DEFAULT_EXPECT)
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
	CRingBuffer(BOOL bCreate = FALSE, DWORD uiExpect = DEFAULT_EXPECT)
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

typedef CRingBuffer<void, CCriSec, CCriSec>				CCSRingBuffer;
typedef CRingBuffer<void, CInterCriSec, CInterCriSec>	CICSRingBuffer;
typedef CRingBuffer<void, CSpinGuard, CSpinGuard>		CSGRingBuffer;
typedef CRingBuffer<void, CFakeGuard, CFakeGuard>		CFKRingBuffer;

template <class T> class CRingPool
{
private:
	typedef T*			TPTR;
	typedef volatile T*	VTPTR;

	static TPTR const E_EMPTY;
	static TPTR const E_LOCKED;
	static TPTR const E_RELEASED;
	static TPTR const E_OCCUPIED;
	static TPTR const E_MAX_STATUS;
public:

	BOOL TryPut(TPTR pElement)
	{
		ASSERT(pElement != nullptr);

		BOOL isOK = FALSE;

		while(true)
		{
			BOOL bOccupy = FALSE;
			DWORD seqPut = m_seqPut;

			if(!HasPutSpace(seqPut))
				break;

			DWORD dwIndex = seqPut % m_dwSize;
			VTPTR& pValue = *(m_pv + dwIndex);

			if(pValue == E_RELEASED)
			{
				if(::InterlockedCompareExchangePointer((volatile PVOID*)&pValue, E_OCCUPIED, E_RELEASED) == E_RELEASED)
					bOccupy = TRUE;
				else
					continue;
			}

			if(pValue == E_EMPTY || bOccupy)
			{
				if(::InterlockedCompareExchange(&m_seqPut, seqPut + 1, seqPut) == seqPut)
				{
					pValue	= pElement;
					isOK	= TRUE;

					break;
				}
			}
			else if(pValue == E_LOCKED)
				break;
		}

		return isOK;
	}

	BOOL TryGet(TPTR* ppElement)
	{
		ASSERT(ppElement != nullptr);

		BOOL isOK = FALSE;

		while(true)
		{
			DWORD seqGet = m_seqGet;

			if(!HasGetSpace(seqGet))
				break;

			DWORD dwIndex = seqGet % m_dwSize;
			VTPTR& pValue = *(m_pv + dwIndex);

			if(pValue == E_LOCKED)
				break;
			else if(pValue != E_EMPTY && pValue != E_RELEASED && pValue != E_OCCUPIED)
			{
				if(::InterlockedCompareExchange(&m_seqGet, seqGet + 1, seqGet) == seqGet)
				{
					ASSERT(pValue > E_MAX_STATUS);

					*(ppElement)	= (TPTR)pValue;
					pValue			= E_EMPTY;
					isOK			= TRUE;

					break;
				}
			}
		}

		return isOK;
	}

	BOOL TryLock(TPTR* ppElement, DWORD& dwIndex)
	{
		ASSERT(ppElement != nullptr);

		BOOL isOK = FALSE;

		while(true)
		{
			DWORD seqGet = m_seqGet;

			if(!HasGetSpace(seqGet))
				break;

			dwIndex			= seqGet % m_dwSize;
			VTPTR& pValue	= *(m_pv + dwIndex);

			if(pValue == E_LOCKED)
				break;
			else if(pValue != E_EMPTY && pValue != E_RELEASED && pValue != E_OCCUPIED)
			{
				if(::InterlockedCompareExchange(&m_seqGet, seqGet + 1, seqGet) == seqGet)
				{
					ASSERT(pValue > E_MAX_STATUS);

					*(ppElement)	= (TPTR)pValue;
					pValue			= E_LOCKED;
					isOK			= TRUE;

					break;
				}
			}
		}

		return isOK;
	}

	void ReleaseLock(TPTR pElement, DWORD dwIndex)
	{
		ASSERT(dwIndex < m_dwSize);
		ASSERT(pElement == nullptr || (UINT_PTR)pElement > (UINT_PTR)E_MAX_STATUS);

		VTPTR& pValue = *(m_pv + dwIndex);
		VERIFY(pValue == E_LOCKED);

		if(pElement != nullptr)
		{
			for(DWORD i = 0; ; i++)
			{
				if(TryPut(pElement))
					break;

				DWORD dwPutIndex = m_seqPut % m_dwSize;

				if(dwIndex == dwPutIndex)
				{
					pValue = pElement;
					::InterlockedIncrement(&m_seqPut);

					return;
				}

				CSpinGuard::Pause(i);
			}
		}

		pValue = E_RELEASED;
	}

public:

	void Reset(DWORD dwSize = 0)
	{
		if(IsValid())
			Destroy();
		if(dwSize > 0)
			Create(dwSize);
	}

	DWORD Size()		{return m_dwSize;}
	DWORD Elements()	{return m_seqPut - m_seqGet;}
	BOOL IsFull()		{return Elements() == Size();}
	BOOL IsEmpty()		{return Elements() == 0;}
	BOOL IsValid()		{return m_pv != nullptr;}

private:

	BOOL HasPutSpace(DWORD seqPut)
	{
		return (seqPut - m_seqGet < m_dwSize);
	}

	BOOL HasGetSpace(DWORD seqGet)
	{
		return (m_seqPut - seqGet > 0);
	}

	void Create(DWORD dwSize)
	{
		ASSERT(!IsValid() && dwSize > 0);

		m_seqPut = 0;
		m_seqGet = 0;
		m_dwSize = dwSize;
		m_pv	 = (VTPTR*)malloc(m_dwSize * sizeof(TPTR));

		::memset(m_pv, (int)E_EMPTY, m_dwSize * sizeof(TPTR));
	}

	void Destroy()
	{
		ASSERT(IsValid());

		free((void*)m_pv);
		m_pv = nullptr;
		m_dwSize = 0;
		m_seqPut = 0;
		m_seqGet = 0;
	}


public:
	CRingPool(DWORD dwSize = 0)
		: m_pv(nullptr)
		, m_dwSize(0)
		, m_seqPut(0)
		, m_seqGet(0)
	{
		Reset(dwSize);
	}

	~CRingPool()
	{
		Reset(0);
	}

private:
	CRingPool(const CRingPool&);
	CRingPool operator = (const CRingPool&);

private:
	DWORD				m_dwSize;
	VTPTR*				m_pv;
	char				pack1[PACK_SIZE_OF(VTPTR*)];
	volatile DWORD		m_seqPut;
	char				pack4[PACK_SIZE_OF(DWORD)];
	volatile DWORD		m_seqGet;
	char				pack5[PACK_SIZE_OF(DWORD)];
};

template <class T> T* const CRingPool<T>::E_EMPTY		= (T*)0x00;
template <class T> T* const CRingPool<T>::E_LOCKED		= (T*)0x01;
template <class T> T* const CRingPool<T>::E_RELEASED	= (T*)0x02;
template <class T> T* const CRingPool<T>::E_OCCUPIED	= (T*)0x03;
template <class T> T* const CRingPool<T>::E_MAX_STATUS	= (T*)0x0F;

template <class T> class CCASQueue
{
private:
	struct Node;
	typedef Node*			NPTR;
	typedef volatile Node*	VNPTR;
	typedef volatile ULONG	VLONG;

	struct Node
	{
		T* pValue;
		VNPTR pNext;

		Node(T* val, NPTR next = nullptr)
			: pValue(val), pNext(next)
		{

		}
	};

public:

	void PushBack(T* pVal)
	{
		ASSERT(pVal != nullptr);

		VNPTR pTail	= nullptr;
		NPTR pNode	= new Node(pVal);

		while(true)
		{
			pTail = m_pTail;

			if(::InterlockedCompareExchangePointer((volatile PVOID*)&m_pTail->pNext, pNode, nullptr) == nullptr)
				break;
		}

		::InterlockedCompareExchangePointer((volatile PVOID*)&m_pTail, pNode, (PVOID)pTail);
		::InterlockedIncrement(&m_lSize);
	}

	BOOL PopFront(T** ppVal)
	{
		ASSERT(ppVal != nullptr);

		BOOL isOK	= FALSE;
		VNPTR pNext	= nullptr;

		while(true) 
		{
			pNext = m_pHead->pNext;

			if(pNext == nullptr)
				break;

			if(::InterlockedCompareExchangePointer((volatile PVOID*)&m_pHead->pNext, (PVOID)pNext->pNext, (PVOID)pNext) == pNext)
			{
				*ppVal	= pNext->pValue;
				isOK	= TRUE;

				if(m_pTail == pNext)
					::InterlockedCompareExchangePointer((volatile PVOID*)&m_pTail, (PVOID)m_pHead, (PVOID)pNext);
				
				::InterlockedDecrement(&m_lSize);

				delete pNext;
				break;
			}
		}

		return isOK;
	}

public:

	ULONG Size()	{return m_lSize;}
	BOOL IsEmpty()	{return m_lSize == 0;}

public:

	CCASQueue() : m_lSize(0)
	{
		NPTR pHead = new Node(nullptr);
		m_pHead = m_pTail = pHead;
	}

	~CCASQueue()
	{
		ASSERT(m_pHead != nullptr);
		ASSERT(m_pHead->pNext == nullptr);

		while(m_pHead != nullptr)
		{
			VNPTR pNode = m_pHead->pNext;

			delete m_pHead;
			m_pHead = pNode;
		}
	}

private:
	VLONG m_lSize;
	VNPTR m_pHead;
	VNPTR m_pTail;
};

#if !defined (_WIN64)
	#pragma pack(pop)
#endif
