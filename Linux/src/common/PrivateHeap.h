/*
* Copyright: JessMA Open Source (ldcsaa@gmail.com)
*
* Author	: Bruce Liang
* Website	: https://github.com/ldcsaa
* Project	: https://github.com/ldcsaa/HP-Socket
* Blog		: http://www.cnblogs.com/ldcsaa
* Wiki		: http://www.oschina.net/p/hp-socket
* QQ Group	: 44636872, 75375912
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
#include "Singleton.h"

#include <malloc.h>

#define HEAP_ZERO_MEMORY	0x08     

class CGlobalHeapImpl
{
public:
	PVOID Alloc(SIZE_T dwSize, DWORD dwFlags = 0)
	{
		PVOID pv = malloc(dwSize);

		if(!pv)
			throw std::bad_alloc();

		if(dwFlags & HEAP_ZERO_MEMORY)
			ZeroMemory(pv, dwSize);
		
		return pv;
	}

	PVOID ReAlloc(PVOID pvMemory, SIZE_T dwSize, DWORD dwFlags = 0)
	{
		PVOID pv = realloc(pvMemory, dwSize);

		if(!pv)
		{
			if(pvMemory)
				free(pvMemory);

			throw std::bad_alloc();
		}

		if(dwFlags & HEAP_ZERO_MEMORY)
			ZeroMemory(pv, dwSize);

		return pv;
	}

	BOOL Free(PVOID pvMemory, DWORD dwFlags = 0)
	{
		if(pvMemory)
		{
			free(pvMemory);
			return TRUE;
		}

		return FALSE;
	}

	SIZE_T Compact	(DWORD dwFlags = 0)					{return -1;}
	SIZE_T Size		(PVOID pvMemory, DWORD dwFlags = 0)	{return _msize(pvMemory);}

	BOOL IsValid()	{return TRUE;}
	BOOL Reset()	{return TRUE;}

public:
	CGlobalHeapImpl	(DWORD dwOptions = 0, SIZE_T dwInitSize = 0, SIZE_T dwMaxSize = 0) {}
	~CGlobalHeapImpl()	{}

	DECLARE_NO_COPY_CLASS(CGlobalHeapImpl)
};

#if !defined (_USE_CUSTOM_PRIVATE_HEAP)
	using CPrivateHeap = CGlobalHeapImpl;
#endif

template<class T> class CPrivateHeapBuffer
{
public:
	CPrivateHeapBuffer(CPrivateHeap& hpPrivate, SIZE_T dwSize = 0)
	: m_hpPrivate	(hpPrivate)
	, m_pvMemory	(nullptr)
	{
		ASSERT(m_hpPrivate.IsValid());
		Alloc(dwSize);
	}

	~CPrivateHeapBuffer() {Free();}

public:

	T* Alloc(SIZE_T dwSize, DWORD dwFlags = 0)
	{
		if(IsValid())
			Free();

		if(dwSize > 0)
			m_pvMemory = (T*)m_hpPrivate.Alloc(dwSize * sizeof(T), dwFlags);

		return m_pvMemory;
	}

	T* ReAlloc(SIZE_T dwSize, DWORD dwFlags = 0)
		{return m_pvMemory = (T*)m_hpPrivate.ReAlloc(m_pvMemory, dwSize * sizeof(T), dwFlags);}

	SIZE_T Size(DWORD dwFlags = 0)
		{return m_hpPrivate.Size(m_pvMemory, dwFlags) / sizeof(T);}

	BOOL Free(DWORD dwFlags = 0)
	{
		BOOL isOK = TRUE;

		if(IsValid())
		{
			isOK		= m_hpPrivate.Free(m_pvMemory, dwFlags);
			m_pvMemory	= nullptr;
		}

		return isOK;
	}

	BOOL IsValid()					{return m_pvMemory != nullptr;}
	operator T* ()			const	{return m_pvMemory;}
	T& operator [] (int i)	const	{return *(m_pvMemory + i);}

private:
	CPrivateHeap&	m_hpPrivate;
	T*				m_pvMemory;

	DECLARE_NO_COPY_CLASS(CPrivateHeapBuffer)
};

using CPrivateHeapByteBuffer	= CPrivateHeapBuffer<BYTE>;
using CPrivateHeapStrBuffer		= CPrivateHeapBuffer<TCHAR>;
