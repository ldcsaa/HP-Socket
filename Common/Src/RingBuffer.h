/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.14
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

#include "STLHelper.h"
#include "RWLock.h"
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

			::YieldThread(w);
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

			::YieldThread(w);
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

template <class T, class index_type = DWORD, bool adjust_index = false> class CRingCache
{
public:

	typedef T*							TPTR;
	typedef volatile T*					VTPTR;

	typedef unordered_set<index_type>	IndexSet;

	static TPTR const E_EMPTY;
	static TPTR const E_LOCKED;
	static TPTR const E_MAX_STATUS;

public:

	static index_type&	INDEX_INC(index_type& dwIndex)	{if(adjust_index) ++dwIndex; return dwIndex;}
	static index_type&	INDEX_DEC(index_type& dwIndex)	{if(adjust_index) --dwIndex; return dwIndex;}

private:

	VTPTR& INDEX_VAL(index_type dwIndex) {return *(m_pv + dwIndex);}

public:

	BOOL Put(TPTR pElement, index_type& dwIndex)
	{
		ASSERT(pElement != nullptr);

		BOOL isOK = FALSE;

		while(true)
		{
			if(!HasSpace())
				break;

			DWORD dwCurSeq	= m_dwCurSeq;
			dwIndex			= m_dwCurSeq % m_dwSize;
			VTPTR& pValue	= INDEX_VAL(dwIndex);

			if(pValue == E_EMPTY)
			{
				if(::InterlockedCompareExchangePointer((volatile PVOID*)&pValue, pElement, E_EMPTY) == E_EMPTY)
				{
					::InterlockedIncrement(&m_dwCount);
					::InterlockedCompareExchange(&m_dwCurSeq, dwCurSeq + 1, dwCurSeq);

					if(pElement != E_LOCKED)
						EmplaceIndex(dwIndex);

					isOK = TRUE;
					break;
				}
			}

			::InterlockedCompareExchange(&m_dwCurSeq, dwCurSeq + 1, dwCurSeq);
		}

		if(isOK) INDEX_INC(dwIndex);

		return isOK;
	}

	BOOL Get(index_type dwIndex, TPTR* ppElement)
	{
		INDEX_DEC(dwIndex);

		ASSERT(dwIndex < m_dwSize);
		ASSERT(ppElement != nullptr);

		if((dwIndex >= m_dwSize))
		{
			*ppElement = nullptr;
			return FALSE;
		}

		*ppElement = (TPTR)INDEX_VAL(dwIndex);

		return IsValidElement(*ppElement);
	}

	BOOL Set(index_type dwIndex, TPTR pElement, TPTR* ppOldElement = nullptr)
	{
		TPTR pElement2 = nullptr;
		Get(dwIndex, &pElement2);

		if(ppOldElement != nullptr)
			*ppOldElement = pElement2;

		if(pElement == pElement2)
			return FALSE;

		int f1 = 0;
		int f2 = 0;

		if(pElement == E_EMPTY)
		{
			if(pElement2 == E_LOCKED)
				f1 = -1;
			else
				f1 = f2 = -1;
		}
		else if(pElement == E_LOCKED)
		{
			if(pElement2 == E_EMPTY)
				f1 = 1;
			else
				f2 = -1;
		}
		else
		{
			if(pElement2 == E_EMPTY)
				f1 = f2 = 1;
			else if(pElement2 == E_LOCKED)
				f2 = 1;
		}

		INDEX_DEC(dwIndex);

		BOOL bSetValueFirst = (f1 + f2 >= 0);

		if(bSetValueFirst)	INDEX_VAL(dwIndex) = pElement;
		if(f1 > 0)			::InterlockedIncrement(&m_dwCount);
		if(f2 != 0)			(f2 > 0) ? EmplaceIndex(dwIndex) : EraseIndex(dwIndex);
		if(f1 < 0)			::InterlockedDecrement(&m_dwCount);
		if(!bSetValueFirst) INDEX_VAL(dwIndex) = pElement;

		ASSERT(Spaces() <= Size());

		return TRUE;
	}

	BOOL Remove(index_type dwIndex, TPTR* ppElement = nullptr)
	{
		return Set(dwIndex, E_EMPTY, ppElement);
	}

	BOOL AcquireLock(index_type& dwIndex)
	{
		return Put(E_LOCKED, dwIndex);
	}

	BOOL ReleaseLock(index_type dwIndex, TPTR pElement)
	{
		ASSERT(pElement == nullptr || IsValidElement(pElement));

		TPTR pElement2 = nullptr;
		Get(dwIndex, &pElement2);

		if(pElement2 != E_LOCKED)
			return FALSE;

		Set(dwIndex, pElement);

		return TRUE;
	}

public:

	void Reset(DWORD dwSize = 0)
	{
		if(IsValid())
			Destroy();
		if(dwSize > 0)
			Create(dwSize);
	}
	
	BOOL GetAllElementIndexes(index_type ids[], DWORD& dwCount, BOOL bCopy = TRUE)
	{
		if(ids == nullptr)
		{
			dwCount = Elements();
			return FALSE;
		}

		IndexSet* pIndexes = nullptr;
		IndexSet indexes;

		if(bCopy)
			pIndexes = &CopyIndexes(indexes);
		else
			pIndexes = &m_indexes;

		BOOL isOK	 = FALSE;
		DWORD dwSize = (DWORD)pIndexes->size();

		if(dwSize <= dwCount)
		{
			IndexSet::const_iterator it  = pIndexes->begin();
			IndexSet::const_iterator end = pIndexes->end();

			for(int i = 0; it != end; ++it, ++i)
			{
				index_type index = *it;
				ids[i]			 = INDEX_INC(index);
			}

			isOK = TRUE;
		}

		dwCount = dwSize;
		return isOK;
	}
	
	unique_ptr<index_type[]> GetAllElementIndexes(DWORD& dwCount, BOOL bCopy = TRUE)
	{
		IndexSet* pIndexes = nullptr;
		IndexSet indexes;

		if(bCopy)
			pIndexes = &CopyIndexes(indexes);
		else
			pIndexes = &m_indexes;

		unique_ptr<index_type[]> ids;
		dwCount = (DWORD)pIndexes->size();

		if(dwCount > 0)
		{
			ids.reset(new index_type[dwCount]);

			IndexSet::const_iterator it  = pIndexes->begin();
			IndexSet::const_iterator end = pIndexes->end();

			for(int i = 0; it != end; ++it, ++i)
			{
				index_type index = *it;
				ids[i]			 = INDEX_INC(index);
			}
		}

		return ids;
	}

	static BOOL IsValidElement(TPTR pElement) {return pElement > E_MAX_STATUS;}

	DWORD Size		()	{return m_dwSize;}
	DWORD Elements	()	{return (DWORD)m_indexes.size();}
	DWORD Spaces	()	{return m_dwSize - m_dwCount;}
	BOOL HasSpace	()	{return m_dwCount < m_dwSize;}
	BOOL IsEmpty	()	{return m_dwCount == 0;}
	BOOL IsValid	()	{return m_pv != nullptr;}

private:

	void Create(DWORD dwSize)
	{
		ASSERT(!IsValid() && dwSize > 0);

		m_dwCurSeq	= 0;
		m_dwCount	= 0;
		m_dwSize	= dwSize;
		m_pv		= (VTPTR*)malloc(m_dwSize * sizeof(TPTR));

		::memset(m_pv, (int)E_EMPTY, m_dwSize * sizeof(TPTR));
	}

	void Destroy()
	{
		ASSERT(IsValid());

		m_indexes.clear();
		free((void*)m_pv);

		m_pv		= nullptr;
		m_dwSize	= 0;
		m_dwCount	= 0;
		m_dwCurSeq	= 0;
	}

	IndexSet& CopyIndexes(IndexSet& indexes)
	{
		{
			CReadLock locallock(m_cs);
			indexes = m_indexes;
		}

		return indexes;
	}

	void EmplaceIndex(index_type dwIndex)
	{
		CWriteLock locallock(m_cs);
		m_indexes.emplace(dwIndex);
	}

	void EraseIndex(index_type dwIndex)
	{
		CWriteLock locallock(m_cs);
		m_indexes.erase(dwIndex);
	}

public:
	CRingCache	(DWORD dwSize = 0)
	: m_pv		(nullptr)
	, m_dwSize	(0)
	, m_dwCount	(0)
	, m_dwCurSeq(0)
	{
		Reset(dwSize);
	}

	~CRingCache()
	{
		Reset(0);
	}

private:
	CRingCache(const CRingCache&);
	CRingCache operator = (const CRingCache&);

private:
	DWORD				m_dwSize;
	VTPTR*				m_pv;
	char				pack1[PACK_SIZE_OF(VTPTR*)];
	volatile DWORD		m_dwCurSeq;
	char				pack2[PACK_SIZE_OF(DWORD)];
	volatile DWORD		m_dwCount;
	char				pack3[PACK_SIZE_OF(DWORD)];

	CSimpleRWLock		m_cs;
	IndexSet			m_indexes;
};

template <class T, class index_type, bool adjust_index> T* const CRingCache<T, index_type, adjust_index>::E_EMPTY		= (T*)0x00;
template <class T, class index_type, bool adjust_index> T* const CRingCache<T, index_type, adjust_index>::E_LOCKED		= (T*)0x01;
template <class T, class index_type, bool adjust_index> T* const CRingCache<T, index_type, adjust_index>::E_MAX_STATUS	= (T*)0x0F;

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

private:

	VTPTR& INDEX_VAL(DWORD dwIndex) {return *(m_pv + dwIndex);}

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
			VTPTR& pValue = INDEX_VAL(dwIndex);

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
			VTPTR& pValue = INDEX_VAL(dwIndex);

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
			VTPTR& pValue	= INDEX_VAL(dwIndex);

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
		ASSERT(pElement == nullptr || pElement > E_MAX_STATUS);

		VTPTR& pValue = INDEX_VAL(dwIndex);
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

				::YieldThread(i);
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
	char				pack2[PACK_SIZE_OF(DWORD)];
	volatile DWORD		m_seqGet;
	char				pack3[PACK_SIZE_OF(DWORD)];
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
		T*		pValue;
		VNPTR	pNext;

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

			if(::InterlockedCompareExchangePointer((volatile PVOID*)&m_pTail, (PVOID)pNode, (PVOID)pTail) == pTail)
			{
				pTail->pNext = pNode;
				break;
			}
		}

		::InterlockedIncrement(&m_lSize);
	}

	BOOL PopFront(T** ppVal)
	{
		ASSERT(ppVal != nullptr);

		if(IsEmpty())
			return FALSE;

		BOOL isOK	= FALSE;
		NPTR pHead	= nullptr;
		NPTR pNext	= nullptr;
		T* pVal		= nullptr;

		while(true)
		{
			while(::InterlockedCompareExchange(&m_lLock, 1, 0) != 0)
				::YieldProcessor();

			pHead = (NPTR)m_pHead;
			pNext = (NPTR)pHead->pNext;

			if(pNext == nullptr)
			{
				m_lLock = 0;
				break;
			}

			*ppVal	= pNext->pValue;
			m_pHead	= pNext;

			m_lLock = 0;
			isOK	= TRUE;

			::InterlockedDecrement(&m_lSize);

			delete pHead;
			break;
		}

		return isOK;
	}

public:

	ULONG Size()	{return m_lSize;}
	BOOL IsEmpty()	{return m_lSize == 0;}

public:

	CCASQueue() : m_lLock(0), m_lSize(0)
	{
		NPTR pHead = new Node(nullptr);
		m_pHead = m_pTail = pHead;
	}

	~CCASQueue()
	{
		ASSERT(m_lLock == 0);
		ASSERT(m_lSize == 0);
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
	VLONG	m_lLock;
	VLONG	m_lSize;
	VNPTR	m_pHead;
	VNPTR	m_pTail;
};

#if !defined (_WIN64)
	#pragma pack(pop)
#endif
