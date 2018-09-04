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

#include "STLHelper.h"
#include "RWLock.h"
#include "CriticalSection.h"

#define CACHE_LINE		64
#define PACK_SIZE_OF(T)	(CACHE_LINE - sizeof(T) % CACHE_LINE)

#if !defined(MAXINT)
	#define MAXINT	INT_MAX
#endif

#if !defined(_WIN64)
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

		ULONGLONG seqGet = m_seqGet;

		if(!IsValid() || !HasGetSpace(seqGet))
			return FALSE;

		DoPeek(pElement, seqGet);

		return TRUE;
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
	CRingBuffer(DWORD uiExpect = DEFAULT_EXPECT)
	: m_pv(nullptr)
	, m_dwReal(0)
	, m_seqPut(0)
	, m_seqGet(0)
	{
		Reset(uiExpect);
	}

	~CRingBuffer()
	{
		Reset(0);
	}

	void Reset(DWORD uiExpect = DEFAULT_EXPECT)
	{
		if(IsValid())
			Destroy();
		if(uiExpect > 0)
			Create(uiExpect);
	}

	BOOL IsValid() {return m_pv != nullptr;}

private:
	void Create(DWORD dwExpect = DEFAULT_EXPECT)
	{
		ASSERT(!IsValid() && dwExpect > 0);

		m_seqPut = 0;
		m_seqGet = 0;
		m_dwReal = Revise(dwExpect);
		m_pv	 = (T**)malloc(m_dwReal * sizeof(T*));
	}

	void Destroy()
	{
		ASSERT(IsValid());

		CLocalLock<_PutGuard> locallock1(m_csPut);
		CLocalLock<_GetGuard> locallock2(m_csGet);

		free((void*)m_pv);
		m_pv	 = nullptr;
		m_dwReal = 0;
		m_seqPut = 0;
		m_seqGet = 0;
	}

private:
	CRingBuffer(const CRingBuffer&);
	CRingBuffer operator = (const CRingBuffer&);

private:
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

// ------------------------------------------------------------------------------------------------------------- //

template <class T, class index_type = DWORD, bool adjust_index = false> class CRingCache
{
public:

	enum EnGetResult {GR_FAIL = -1, GR_INVALID = 0, GR_VALID = 1};

	typedef T*									TPTR;
	typedef volatile T*							VTPTR;

	typedef unordered_set<index_type>			IndexSet;
	typedef typename IndexSet::const_iterator	IndexSetCI;
	typedef typename IndexSet::iterator			IndexSetI;

	static TPTR const E_EMPTY;
	static TPTR const E_LOCKED;
	static TPTR const E_MAX_STATUS;

public:

	static index_type& INDEX_INC(index_type& dwIndex)	{if(adjust_index) ++dwIndex; return dwIndex;}
	static index_type& INDEX_DEC(index_type& dwIndex)	{if(adjust_index) --dwIndex; return dwIndex;}

private:

	index_type& INDEX_V2R(index_type& dwIndex)	{dwIndex %= m_dwSize; if(dwIndex == 0) dwIndex = m_dwSize; return dwIndex;}
	VTPTR& INDEX_VAL(index_type dwIndex) {return *(m_pv + dwIndex);}

public:

	BOOL Put(TPTR pElement, index_type& dwIndex)
	{
		ASSERT(pElement != nullptr);

		if(!IsValid()) return FALSE;

		BOOL isOK = FALSE;

		while(true)
		{
			if(!HasSpace())
				break;

			DWORD dwCurSeq			= m_dwCurSeq;
			index_type dwCurIndex	= dwCurSeq % m_dwSize;
			VTPTR& pValue			= INDEX_VAL(dwCurIndex);

			if(pValue == E_EMPTY)
			{
				if(::InterlockedCompareExchangePointer((volatile PVOID*)&pValue, pElement, E_EMPTY) == E_EMPTY)
				{
					::InterlockedIncrement(&m_dwCount);
					::InterlockedCompareExchange(&m_dwCurSeq, dwCurSeq + 1, dwCurSeq);

					dwIndex = INDEX_INC(dwCurIndex);
					isOK	= TRUE;

					if(pElement != E_LOCKED)
						EmplaceIndex(dwIndex);

					break;
				}
			}

			::InterlockedCompareExchange(&m_dwCurSeq, dwCurSeq + 1, dwCurSeq);
		}

		return isOK;
	}

	EnGetResult GetEx(index_type dwIndex, TPTR* ppElement)
	{
		return Get(INDEX_V2R(dwIndex), ppElement);
	}

	EnGetResult Get(index_type dwIndex, TPTR* ppElement)
	{
		ASSERT(dwIndex <= m_dwSize);
		ASSERT(ppElement != nullptr);

		if(!IsValid() || INDEX_DEC(dwIndex) >= m_dwSize)
		{
			*ppElement = nullptr;
			return GR_FAIL;
		}

		*ppElement = (TPTR)INDEX_VAL(dwIndex);

		return IsValidElement(*ppElement) ? GR_VALID : GR_INVALID;
	}

	BOOL SetEx(index_type dwIndex, TPTR pElement, TPTR* ppOldElement = nullptr)
	{
		return Set(INDEX_V2R(dwIndex), pElement, ppOldElement);
	}

	BOOL Set(index_type dwIndex, TPTR pElement, TPTR* ppOldElement = nullptr)
	{
		TPTR pElement2 = nullptr;

		if(Get(dwIndex, &pElement2) == GR_FAIL)
			return FALSE;

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

		BOOL bSetValueFirst		= (f1 + f2 >= 0);
		index_type dwOuterIndex	= dwIndex;

		INDEX_DEC(dwIndex);

		if(bSetValueFirst)	INDEX_VAL(dwIndex) = pElement;
		if(f1 > 0)			::InterlockedIncrement(&m_dwCount);
		if(f2 != 0)			(f2 > 0) ? EmplaceIndex(dwOuterIndex) : EraseIndex(dwOuterIndex);
		if(f1 < 0)			::InterlockedDecrement(&m_dwCount);
		if(!bSetValueFirst) INDEX_VAL(dwIndex) = pElement;

		ASSERT(Spaces() <= Size());

		return TRUE;
	}

	BOOL RemoveEx(index_type dwIndex, TPTR* ppElement = nullptr)
	{
		return Remove(INDEX_V2R(dwIndex), ppElement);
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

		ASSERT(pElement2 == E_LOCKED);

		if(pElement2 != E_LOCKED)
			return FALSE;

		return Set(dwIndex, pElement);
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
		if(ids == nullptr || dwCount == 0)
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

		if(dwSize > 0 && dwSize <= dwCount)
		{
			IndexSetCI it  = pIndexes->begin();
			IndexSetCI end = pIndexes->end();

			for(int i = 0; it != end; ++it, ++i)
				ids[i] = *it;

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

			IndexSetCI it  = pIndexes->begin();
			IndexSetCI end = pIndexes->end();

			for(int i = 0; it != end; ++it, ++i)
				ids[i] = *it;
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

		::ZeroMemory(m_pv, m_dwSize * sizeof(TPTR));
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

// ------------------------------------------------------------------------------------------------------------- //

template <class T, class index_type = DWORD, bool adjust_index = false> class CRingCache2
{
public:

	enum EnGetResult {GR_FAIL = -1, GR_INVALID = 0, GR_VALID = 1};

	typedef T*									TPTR;
	typedef volatile T*							VTPTR;

	typedef unordered_set<index_type>			IndexSet;
	typedef typename IndexSet::const_iterator	IndexSetCI;
	typedef typename IndexSet::iterator			IndexSetI;

	static TPTR const E_EMPTY;
	static TPTR const E_LOCKED;
	static TPTR const E_MAX_STATUS;
	static DWORD const MAX_SIZE;

public:

	static index_type& INDEX_INC(index_type& dwIndex)	{if(adjust_index) ++dwIndex; return dwIndex;}
	static index_type& INDEX_DEC(index_type& dwIndex)	{if(adjust_index) --dwIndex; return dwIndex;}

	index_type& INDEX_R2V(index_type& dwIndex)			{dwIndex += *(m_px + dwIndex) * m_dwSize; return dwIndex;}

	BOOL INDEX_V2R(index_type& dwIndex)
	{
		index_type m = dwIndex % m_dwSize;
		BYTE x		 = *(m_px + m);

		if(dwIndex / m_dwSize != x)
			return FALSE;

		dwIndex = m;
		return TRUE;
	}


private:

	VTPTR& INDEX_VAL(index_type dwIndex) {return *(m_pv + dwIndex);}

public:

	BOOL Put(TPTR pElement, index_type& dwIndex)
	{
		ASSERT(pElement != nullptr);

		if(!IsValid()) return FALSE;

		BOOL isOK = FALSE;

		while(true)
		{
			if(!HasSpace())
				break;

			DWORD dwCurSeq			= m_dwCurSeq;
			index_type dwCurIndex	= dwCurSeq % m_dwSize;
			VTPTR& pValue			= INDEX_VAL(dwCurIndex);

			if(pValue == E_EMPTY)
			{
				if(::InterlockedCompareExchangePointer((volatile PVOID*)&pValue, pElement, E_EMPTY) == E_EMPTY)
				{
					::InterlockedIncrement(&m_dwCount);
					::InterlockedCompareExchange(&m_dwCurSeq, dwCurSeq + 1, dwCurSeq);

					dwIndex = INDEX_INC(INDEX_R2V(dwCurIndex));
					isOK	= TRUE;

					if(pElement != E_LOCKED)
						EmplaceIndex(dwIndex);

					break;
				}
			}

			::InterlockedCompareExchange(&m_dwCurSeq, dwCurSeq + 1, dwCurSeq);
		}

		return isOK;
	}

	EnGetResult Get(index_type dwIndex, TPTR* ppElement, index_type* pdwRealIndex = nullptr)
	{
		ASSERT(ppElement != nullptr);

		if(!IsValid() || !INDEX_V2R(INDEX_DEC(dwIndex)))
		{
			*ppElement = nullptr;
			return GR_FAIL;
		}

		*ppElement = (TPTR)INDEX_VAL(dwIndex);
		if(pdwRealIndex) *pdwRealIndex = dwIndex;

		return IsValidElement(*ppElement) ? GR_VALID : GR_INVALID;
	}

	BOOL Set(index_type dwIndex, TPTR pElement, TPTR* ppOldElement = nullptr, index_type* pdwRealIndex = nullptr)
	{
		TPTR pElement2 = nullptr;

		if(pdwRealIndex == nullptr)
			pdwRealIndex = (index_type*)_alloca(sizeof(index_type));

		if(Get(dwIndex, &pElement2, pdwRealIndex) == GR_FAIL)
			return FALSE;

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

		BOOL bSetValueFirst		= (f1 + f2 >= 0);
		index_type dwRealIndex	= *pdwRealIndex;

		if(bSetValueFirst)	INDEX_VAL(dwRealIndex) = pElement;
		if(f1 > 0)			::InterlockedIncrement(&m_dwCount);
		if(f2 != 0)			(f2 > 0) ? EmplaceIndex(dwIndex) : EraseIndex(dwIndex);
		if(f1 < 0)			{::InterlockedDecrement(&m_dwCount); ++(*(m_px + dwRealIndex));}
		if(!bSetValueFirst) INDEX_VAL(dwRealIndex) = pElement;

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

		ASSERT(pElement2 == E_LOCKED);

		if(pElement2 != E_LOCKED)
			return FALSE;

		return Set(dwIndex, pElement);
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
		if(ids == nullptr || dwCount == 0)
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

		if(dwSize > 0 && dwSize <= dwCount)
		{
			IndexSetCI it  = pIndexes->begin();
			IndexSetCI end = pIndexes->end();

			for(int i = 0; it != end; ++it, ++i)
				ids[i] = *it;

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

			IndexSetCI it  = pIndexes->begin();
			IndexSetCI end = pIndexes->end();

			for(int i = 0; it != end; ++it, ++i)
				ids[i] = *it;
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
		ASSERT(!IsValid() && dwSize > 0 && dwSize <= MAX_SIZE);

		m_dwCurSeq	= 0;
		m_dwCount	= 0;
		m_dwSize	= dwSize;
		m_pv		= (VTPTR*)malloc(m_dwSize * sizeof(TPTR));
		m_px		= (BYTE*)malloc(m_dwSize * sizeof(BYTE));

		::ZeroMemory(m_pv, m_dwSize * sizeof(TPTR));
		::ZeroMemory(m_px, m_dwSize * sizeof(BYTE));
	}

	void Destroy()
	{
		ASSERT(IsValid());

		m_indexes.clear();
		free((void*)m_pv);
		free((void*)m_px);

		m_pv		= nullptr;
		m_px		= nullptr;
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
	CRingCache2	(DWORD dwSize = 0)
	: m_pv		(nullptr)
	, m_px		(nullptr)
	, m_dwSize	(0)
	, m_dwCount	(0)
	, m_dwCurSeq(0)
	{
		Reset(dwSize);
	}

	~CRingCache2()
	{
		Reset(0);
	}

private:
	CRingCache2(const CRingCache2&);
	CRingCache2 operator = (const CRingCache2&);

private:
	DWORD				m_dwSize;
	VTPTR*				m_pv;
	char				pack1[PACK_SIZE_OF(VTPTR*)];
	BYTE*				m_px;
	char				pack2[PACK_SIZE_OF(BYTE*)];
	volatile DWORD		m_dwCurSeq;
	char				pack3[PACK_SIZE_OF(DWORD)];
	volatile DWORD		m_dwCount;
	char				pack4[PACK_SIZE_OF(DWORD)];

	CSimpleRWLock		m_cs;
	IndexSet			m_indexes;
};

template <class T, class index_type, bool adjust_index> T* const CRingCache2<T, index_type, adjust_index>::E_EMPTY		= (T*)0x00;
template <class T, class index_type, bool adjust_index> T* const CRingCache2<T, index_type, adjust_index>::E_LOCKED		= (T*)0x01;
template <class T, class index_type, bool adjust_index> T* const CRingCache2<T, index_type, adjust_index>::E_MAX_STATUS	= (T*)0x0F;

template <class T, class index_type, bool adjust_index> DWORD const CRingCache2<T, index_type, adjust_index>::MAX_SIZE	= 
#if !defined(_WIN64)
																														  0x00FFFFFF
#else
																														  0xFFFFFFFF
#endif
																																	;
// ------------------------------------------------------------------------------------------------------------- //

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

		if(!IsValid()) return FALSE;

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

		if(!IsValid()) return FALSE;

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

		if(!IsValid()) return FALSE;

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

	BOOL ReleaseLock(TPTR pElement, DWORD dwIndex)
	{
		ASSERT(dwIndex < m_dwSize);
		ASSERT(pElement == nullptr || pElement > E_MAX_STATUS);

		if(!IsValid()) return FALSE;

		VTPTR& pValue = INDEX_VAL(dwIndex);
		ENSURE(pValue == E_LOCKED);

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

					return TRUE;
				}

				::YieldThread(i);
			}
		}

		pValue = E_RELEASED;

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

	DWORD Size()		{return m_dwSize;}
	DWORD Elements()	{return m_seqPut - m_seqGet;}
	BOOL IsFull()		{return Elements() == Size();}
	BOOL IsEmpty()		{return Elements() == 0;}
	BOOL IsValid()		{return m_pv != nullptr;}

private:

	BOOL HasPutSpace(DWORD seqPut)
	{
		return ((int)(seqPut - m_seqGet) < (int)m_dwSize);
	}

	BOOL HasGetSpace(DWORD seqGet)
	{
		return ((int)(m_seqPut - seqGet) > 0);
	}

	void Create(DWORD dwSize)
	{
		ASSERT(!IsValid() && dwSize > 0);

		m_seqPut = 0;
		m_seqGet = 0;
		m_dwSize = dwSize;
		m_pv	 = (VTPTR*)malloc(m_dwSize * sizeof(TPTR));

		::ZeroMemory(m_pv, m_dwSize * sizeof(TPTR));
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

// ------------------------------------------------------------------------------------------------------------- //

template <class T> class CCASQueueX
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

	void UnsafePushBack(T* pVal)
	{
		ASSERT(pVal != nullptr);

		NPTR pNode		= new Node(pVal);
		m_pTail->pNext	= pNode;
		m_pTail			= pNode;
		
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

		while(true)
		{
			Lock();

			pHead = (NPTR)m_pHead;
			pNext = (NPTR)pHead->pNext;

			if(pNext == nullptr)
			{
				Unlock();
				break;
			}

			*ppVal	= pNext->pValue;
			m_pHead	= pNext;

			Unlock();

			isOK	= TRUE;

			::InterlockedDecrement(&m_lSize);

			delete pHead;
			break;
		}

		return isOK;
	}

	BOOL UnsafePopFront(T** ppVal)
	{
		if(!UnsafePeekFront(ppVal))
			return FALSE;

		UnsafePopFrontNotCheck();

		return TRUE;
	}

	BOOL UnsafePeekFront(T** ppVal)
	{
		ASSERT(ppVal != nullptr);

		NPTR pNext = (NPTR)m_pHead->pNext;

		if(pNext == nullptr)
			return FALSE;

		*ppVal = pNext->pValue;

		return TRUE;
	}

	void UnsafePopFrontNotCheck()
	{
		NPTR pHead	= (NPTR)m_pHead;
		NPTR pNext	= (NPTR)pHead->pNext;
		m_pHead		= pNext;

		::InterlockedDecrement(&m_lSize);

		delete pHead;
	}

	void UnsafeClear()
	{
		ASSERT(m_pHead != nullptr);

		m_dwCheckTime = 0;

		while(m_pHead->pNext != nullptr)
			UnsafePopFrontNotCheck();
	}

public:

	ULONG Size()	{return m_lSize;}
	BOOL IsEmpty()	{return m_lSize == 0;}

	void Lock()		{while(!TryLock()) ::YieldProcessor();}
	void Unlock()	{m_lLock = 0;}
	BOOL TryLock()	{return (::InterlockedCompareExchange(&m_lLock, 1, 0) == 0);}

	DWORD GetCheckTime()
	{
		return m_dwCheckTime;
	}

	void UpdateCheckTime(DWORD dwCurrent = 0)
	{
		if(dwCurrent == 0)
			dwCurrent = ::TimeGetTime();

		m_dwCheckTime = dwCurrent;
	}

	int GetCheckTimeGap(DWORD dwCurrent = 0)
	{
		int rs = (int)GetTimeGap32(m_dwCheckTime, dwCurrent);

		if(rs < -60 * 1000)
			rs = MAXINT;

		return rs;
	}

public:

	CCASQueueX() : m_lLock(0), m_lSize(0), m_dwCheckTime(0)
	{
		m_pHead = m_pTail = new Node(nullptr);
	}

	~CCASQueueX()
	{
		ASSERT(m_lLock == 0);
		ASSERT(m_lSize == 0);
		ASSERT(m_pTail == m_pHead);
		ASSERT(m_pHead != nullptr);
		ASSERT(m_pHead->pNext == nullptr);

		UnsafeClear();

		delete m_pHead;
	}

	DECLARE_NO_COPY_CLASS(CCASQueueX)

private:
	VLONG	m_lLock;
	VLONG	m_lSize;
	VNPTR	m_pHead;
	VNPTR	m_pTail;

	volatile DWORD m_dwCheckTime;
};

template <class T> class CCASQueueY
{
public:

	void PushBack(T* pVal)
	{
		CCriSecLock locallock(m_csGuard);

		UnsafePushBack(pVal);
	}

	void UnsafePushBack(T* pVal)
	{
		ASSERT(pVal != nullptr);

		m_lsItems.push_back(pVal);
	}

	void PushFront(T* pVal)
	{
		CCriSecLock locallock(m_csGuard);

		UnsafePushFront(pVal);
	}

	void UnsafePushFront(T* pVal)
	{
		ASSERT(pVal != nullptr);

		m_lsItems.push_front(pVal);
	}

	BOOL PopFront(T** ppVal)
	{
		CCriSecLock locallock(m_csGuard);

		return UnsafePopFront(ppVal);
	}

	BOOL UnsafePopFront(T** ppVal)
	{
		if(!UnsafePeekFront(ppVal))
			return FALSE;

		UnsafePopFrontNotCheck();

		return TRUE;
	}

	BOOL PeekFront(T** ppVal)
	{
		CCriSecLock locallock(m_csGuard);

		return UnsafePeekFront(ppVal);
	}

	BOOL UnsafePeekFront(T** ppVal)
	{
		ASSERT(ppVal != nullptr);

		if(m_lsItems.empty())
			return FALSE;

		*ppVal = m_lsItems.front();

		return TRUE;
	}

	void UnsafePopFrontNotCheck()
	{
		m_lsItems.pop_front();
	}

	void Clear()
	{
		CCriSecLock locallock(m_csGuard);

		UnsafeClear();
	}

	void UnsafeClear()
	{
		m_dwCheckTime = 0;

		m_lsItems.clear();
	}

public:

	ULONG Size()	{return (ULONG)m_lsItems.size();}
	BOOL IsEmpty()	{return (BOOL)m_lsItems.empty();}

	void Lock()		{m_csGuard.Lock();}
	void Unlock()	{m_csGuard.Unlock();}
	BOOL TryLock()	{return m_csGuard.TryLock();}
	CCriSec& Guard(){return m_csGuard;}

	DWORD GetCheckTime()
	{
		return m_dwCheckTime;
	}

	void UpdateCheckTime(DWORD dwCurrent = 0)
	{
		if(dwCurrent == 0)
			dwCurrent = ::TimeGetTime();

		m_dwCheckTime = dwCurrent;
	}

	int GetCheckTimeGap(DWORD dwCurrent = 0)
	{
		int rs = (int)GetTimeGap32(m_dwCheckTime, dwCurrent);

		if(rs < -60 * 1000)
			rs = MAXINT;

		return rs;
	}

public:

	CCASQueueY()
	: m_dwCheckTime(0)
	{

	}

	~CCASQueueY()
	{
		ASSERT(IsEmpty());

		UnsafeClear();
	}

	DECLARE_NO_COPY_CLASS(CCASQueueY)

private:
	CCriSec		m_csGuard;
	deque<T*>	m_lsItems;
	
	volatile DWORD m_dwCheckTime;
};

#define CCASQueue	CCASQueueX

template<typename T>
void ReleaseGCObj(CCASQueue<T>& lsGC, DWORD dwLockTime, BOOL bForce = FALSE)
{
	static const int MAX_CHECK_INTERVAL = 15 * 1000;

	T* pObj = nullptr;

	if(bForce)
	{
		CLocalLock<CCASQueue<T>> locallock(lsGC);

		while(lsGC.UnsafePeekFront(&pObj))
		{
			lsGC.UnsafePopFrontNotCheck();
			T::Destruct(pObj);
		}
	}
	else
	{
		if(lsGC.IsEmpty() || lsGC.GetCheckTimeGap() < min((int)(dwLockTime / 3), MAX_CHECK_INTERVAL))
			return;

		BOOL bFirst	= TRUE;
		DWORD now	= 0;

		while(TRUE)
		{
			ASSERT((pObj = nullptr) == nullptr);

			{
				CLocalTryLock<CCASQueue<T>> locallock(lsGC);

				if(!locallock.IsValid())
					break;

				if(bFirst)
				{
					bFirst	= FALSE;
					now		= ::TimeGetTime();

					lsGC.UpdateCheckTime(now);
				}

				if(!lsGC.UnsafePeekFront(&pObj))
					break;

				if((int)(now - pObj->GetFreeTime()) < (int)dwLockTime)
					break;

				lsGC.UnsafePopFrontNotCheck();
			}

			ASSERT(pObj != nullptr);
			T::Destruct(pObj);
		}
	}
}

#if !defined(_WIN64)
	#pragma pack(pop)
#endif
