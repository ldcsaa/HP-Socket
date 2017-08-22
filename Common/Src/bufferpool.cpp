/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.20
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
 
/******************************************************************************
Module:  BufferPool.cpp
Notices: Copyright (c) 2013 Bruce Liang
Purpose: ���ڴ滺���
Desc:
******************************************************************************/

#include "stdafx.h"
#include "bufferpool.h"
#include "SysHelper.h"
#include "WaitFor.h"

const DWORD TItem::DEFAULT_ITEM_CAPACITY			= ::SysGetPageSize();
const DWORD CBufferPool::DEFAULT_MAX_CACHE_SIZE		= 0;
const DWORD CBufferPool::DEFAULT_ITEM_CAPACITY		= CItemPool::DEFAULT_ITEM_CAPACITY;
const DWORD CBufferPool::DEFAULT_ITEM_POOL_SIZE		= CItemPool::DEFAULT_POOL_SIZE;
const DWORD CBufferPool::DEFAULT_ITEM_POOL_HOLD		= CItemPool::DEFAULT_POOL_HOLD;
const DWORD CBufferPool::DEFAULT_BUFFER_LOCK_TIME	= 10 * 1000;
const DWORD CBufferPool::DEFAULT_BUFFER_POOL_SIZE	= 150;
const DWORD CBufferPool::DEFAULT_BUFFER_POOL_HOLD	= 600;

TItem* TItem::Construct(CPrivateHeap& heap, int capacity, BYTE* pData, int length)
{
	ASSERT(capacity > 0);

	int item_size	= sizeof(TItem);
	TItem* pItem	= (TItem*)heap.Alloc(item_size + capacity);
	pItem->head		= (BYTE*)pItem + item_size;

	pItem->TItem::TItem(heap, capacity, pData, length);

	return pItem;
}

void TItem::Destruct(TItem* pItem)
{
	ASSERT(pItem != nullptr);

	CPrivateHeap& heap = pItem->heap;
	pItem->TItem::~TItem();
	heap.Free(pItem);
}

inline int TItem::Cat(const BYTE* pData, int length)
{
	ASSERT(pData != nullptr && length > 0);

	int cat = min(Remain(), length);

	if(cat > 0)
	{
		memcpy(end, pData, cat);
		end += cat;
	}

	return cat;
}

inline int TItem::Cat(const TItem& other)
{
	ASSERT(this != &other);
	return Cat(other.Ptr(), other.Size());
}

inline int TItem::Fetch(BYTE* pData, int length)
{
	ASSERT(pData != nullptr && length > 0);

	int fetch = min(Size(), length);
	memcpy(pData, begin, fetch);
	begin	 += fetch;

	return fetch;
}

inline int TItem::Peek(BYTE* pData, int length)
{
	ASSERT(pData != nullptr && length > 0);

	int peek = min(Size(), length);
	memcpy(pData, begin, peek);

	return peek;
}

inline int TItem::Reduce(int length)
{
	ASSERT(length > 0);

	int reduce   = min(Size(), length);
	begin		+= reduce;

	return reduce;
}

inline void	TItem::Reset(int first, int last)
{
	ASSERT(first >= -1 && first <= capacity);
	ASSERT(last >= -1 && last <= capacity);

	if(first >= 0)	begin	= head + min(first, capacity);
	if(last >= 0)	end		= head + min(last, capacity);
}

int TItemList::Cat(const BYTE* pData, int length)
{
	int remain = length;

	while(remain > 0)
	{
		TItem* pItem = Back();

		if(pItem == nullptr || pItem->IsFull())
			pItem = PushBack(itPool.PickFreeItem());

		int cat  = pItem->Cat(pData, remain);

		pData	+= cat;
		remain	-= cat;
	}

	return length;
}

int TItemList::Cat(const TItem* pItem)
{
	return Cat(pItem->Ptr(), pItem->Size());
}

int TItemList::Cat(const TItemList& other)
{
	ASSERT(this != &other);

	int length = 0;

	for(TItem* pItem = other.Front(); pItem != nullptr; pItem = pItem->next)
		length += Cat(pItem);

	return length;
}

int TItemList::Fetch(BYTE* pData, int length)
{
	int remain = length;

	while(remain > 0 && Size() > 0)
	{
		TItem* pItem = Front();
		int fetch	 = pItem->Fetch(pData, remain);

		pData	+= fetch;
		remain	-= fetch;

		if(pItem->IsEmpty())
			itPool.PutFreeItem(PopFront());
	}

	return length - remain;
}

int TItemList::Peek(BYTE* pData, int length)
{
	int remain	 = length;
	TItem* pItem = Front();

	while(remain > 0 && pItem != nullptr)
	{
		int peek = pItem->Peek(pData, remain);

		pData	+= peek;
		remain	-= peek;
		pItem	 = pItem->next;
	}

	return length - remain;
}

int TItemList::Reduce(int length)
{
	int remain = length;

	while(remain > 0 && Size() > 0)
	{
		TItem* pItem = Front();
		remain		-= pItem->Reduce(remain);

		if(pItem->IsEmpty())
			itPool.PutFreeItem(PopFront());
	}

	return length - remain;
}

void TItemList::Release()
{
	itPool.PutFreeItem(*this);
}

TBuffer* TBuffer::Construct(CBufferPool& pool, ULONG_PTR dwID)
{
	ASSERT(dwID != 0);

	CPrivateHeap& heap	= pool.GetPrivateHeap();
	TBuffer* pBuffer	= (TBuffer*)heap.Alloc(sizeof(TBuffer));

	pBuffer->TBuffer::TBuffer(heap, pool.GetItemPool(), dwID);

	return pBuffer;
}

void TBuffer::Destruct(TBuffer* pBuffer)
{
	ASSERT(pBuffer != nullptr);

	CPrivateHeap& heap = pBuffer->heap;
	pBuffer->TBuffer::~TBuffer();
	heap.Free(pBuffer);
}

inline void TBuffer::Reset()
{
	id		 = 0;
	length	 = 0;
	freeTime = ::TimeGetTime();
}

int TBuffer::Cat(const BYTE* pData, int len)
{
	items.Cat(pData, len);
	return IncreaseLength(len);
}

int TBuffer::Cat(const TItem* pItem)
{
	items.Cat(pItem);
	return IncreaseLength(pItem->Size());
}

int TBuffer::Cat(const TItemList& other)
{
	ASSERT(&items != &other);

	for(TItem* pItem = other.Front(); pItem != nullptr; pItem = pItem->next)
		Cat(pItem);

	return length;
}

int TBuffer::Fetch(BYTE* pData, int len)
{
	int fetch = items.Fetch(pData, len);
	DecreaseLength(fetch);

	return fetch;
}

int TBuffer::Peek(BYTE* pData, int len)
{
	return items.Peek(pData, len);
}

int TBuffer::Reduce(int len)
{
	int reduce = items.Reduce(len);
	DecreaseLength(reduce);

	return reduce;
}

void CBufferPool::PutFreeBuffer(ULONG_PTR dwID)
{
	ASSERT(dwID != 0);

	TBuffer* pBuffer = FindCacheBuffer(dwID);

	if(pBuffer != nullptr)
		PutFreeBuffer(pBuffer);
}

void CBufferPool::PutFreeBuffer(TBuffer* pBuffer)
{
	ASSERT(pBuffer != nullptr);

	if(!pBuffer->IsValid())
		return;

	m_bfCache.Remove(pBuffer->ID());

	BOOL bOK = FALSE;

	{
		CCriSecLock locallock(pBuffer->cs);

		if(pBuffer->IsValid())
		{
			pBuffer->Reset();
			bOK = TRUE;
		}
	}

	if(bOK)
	{
		m_itPool.PutFreeItem(pBuffer->items);

		if(!m_lsFreeBuffer.TryPut(pBuffer))
		{
			m_lsGCBuffer.PushBack(pBuffer);

			if(m_lsGCBuffer.Size() > m_dwBufferPoolSize)
				ReleaseGCBuffer();
		}
	}
}

void CBufferPool::ReleaseGCBuffer(BOOL bForce)
{
	TBuffer* pBuffer = nullptr;
	DWORD now		 = ::TimeGetTime();

	while(m_lsGCBuffer.PopFront(&pBuffer))
	{
		if(bForce || (int)(now - pBuffer->freeTime) >= (int)m_dwBufferLockTime)
			TBuffer::Destruct(pBuffer);
		else
		{
			m_lsGCBuffer.PushBack(pBuffer);
			break;
		}
	}
}

TBuffer* CBufferPool::PutCacheBuffer(ULONG_PTR dwID)
{
	ASSERT(dwID != 0);

	TBuffer* pBuffer = PickFreeBuffer(dwID);
	m_bfCache.Set(dwID, pBuffer);

	return pBuffer;
}

TBuffer* CBufferPool::PickFreeBuffer(ULONG_PTR dwID)
{
	ASSERT( dwID != 0);

	DWORD dwIndex;
	TBuffer* pBuffer = nullptr;

	if(m_lsFreeBuffer.TryLock(&pBuffer, dwIndex))
	{
		if(::GetTimeGap32(pBuffer->freeTime) >= m_dwBufferLockTime)
			VERIFY(m_lsFreeBuffer.ReleaseLock(nullptr, dwIndex));
		else
		{
			VERIFY(m_lsFreeBuffer.ReleaseLock(pBuffer, dwIndex));
			pBuffer = nullptr;
		}
	}

	if(pBuffer)	pBuffer->id	= dwID;
	else		pBuffer		= TBuffer::Construct(*this, dwID);

	ASSERT(pBuffer);
	return pBuffer;
}

TBuffer* CBufferPool::FindCacheBuffer(ULONG_PTR dwID)
{
	ASSERT(dwID != 0);

	TBuffer* pBuffer = nullptr;

	if(m_bfCache.Get(dwID, &pBuffer) != TBufferCache::GR_VALID)
		pBuffer = nullptr;

	return pBuffer;
}

void CBufferPool::Prepare()
{
	m_itPool.Prepare();

	m_bfCache.Reset(m_dwMaxCacheSize);
	m_lsFreeBuffer.Reset(m_dwBufferPoolHold);
}

void CBufferPool::Clear()
{
	DWORD size					= 0;
	unique_ptr<ULONG_PTR[]> ids	= m_bfCache.GetAllElementIndexes(size, FALSE);

	for(DWORD i = 0; i < size; i++)
	{
		TBuffer* pBuffer = FindCacheBuffer(ids[i]);
		if(pBuffer) TBuffer::Destruct(pBuffer);
	}

	m_bfCache.Reset();

	TBuffer* pBuffer = nullptr;

	while(m_lsFreeBuffer.TryGet(&pBuffer))
		TBuffer::Destruct(pBuffer);

	VERIFY(m_lsFreeBuffer.IsEmpty());
	m_lsFreeBuffer.Reset();

	ReleaseGCBuffer(TRUE);
	VERIFY(m_lsGCBuffer.IsEmpty());

	m_itPool.Clear();
	m_heap.Reset();
}
