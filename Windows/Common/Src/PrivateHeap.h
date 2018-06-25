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
 
/******************************************************************************
Module:  PrivateHeap.h
Notices: Copyright (c) 2006 Bruce Liang
Purpose: 管理进程私有堆.
Desc:
		 1. CPrivateHeap:		自动创建和销毁进程私有堆
								每一个该类的对象都代表一个私有堆, 所以
								该类对象的特点是: 一般声明周期都比较长
								通常作为全局对象, 其他类的静态成员对象
								或者一些长生命周期类对象的成员对象
		 2. CPrivateHeapBuffer: 在私有堆中自动分配和释放指定大小的内存
								一般用于在函数体内分配和释放局部作用域的堆内存
								从而避免对 CPrivateHeap::Alloc() 和 
								CPrivateHeap::Free() 的调用

Examples:
			CPrivateHeap g_hpPrivate;

			int _tmain(int argc, _TCHAR* argv[])
			{
				CPrivateHeapStrBuffer buff(g_hpPrivate, 32);
				lstrcpy(buff, _T("失败乃成功之母"));
				SIZE_T size = buff.Size();
				buff.ReAlloc(40);
				size = buff.Size();
				std::cout << (TCHAR*)buff << '\n';
				// OR
				// ASSERT(g_hpPrivate.IsValid());
				// TCHAR* pch	= (TCHAR*)g_hpPrivate.Alloc(32 * sizeof(TCHAR));
				// lstrcpy(pch, _T("失败乃成功之母"));
				// SIZE_T size = g_hpPrivate.Size(pch);
				// g_hpPrivate.ReAlloc(pch, 40 * sizeof(TCHAR));
				// size = g_hpPrivate.Size(pch);
				// std::cout << pch << '\n';
				// g_hpPrivate.Free(pch);
				// 
				return 0;
			}

******************************************************************************/

#pragma once

class CPrivateHeapImpl
{
public:
	PVOID Alloc(SIZE_T dwSize, DWORD dwFlags = 0)
		{return ::HeapAlloc(m_hHeap, dwFlags, dwSize);}

	PVOID ReAlloc(PVOID pvMemory, SIZE_T dwSize, DWORD dwFlags = 0)
		{return ::HeapReAlloc(m_hHeap, dwFlags, pvMemory, dwSize);}

	SIZE_T Size(PVOID pvMemory, DWORD dwFlags = 0)
		{return ::HeapSize(m_hHeap, dwFlags, pvMemory);}

	BOOL Free(PVOID pvMemory, DWORD dwFlags = 0)
		{return ::HeapFree(m_hHeap, dwFlags, pvMemory);}

	SIZE_T Compact(DWORD dwFlags = 0)
		{return ::HeapCompact(m_hHeap, dwFlags);}

	BOOL IsValid() {return m_hHeap != nullptr;}

	BOOL Reset()
	{
		if(IsValid()) ::HeapDestroy(m_hHeap);
		m_hHeap = ::HeapCreate(m_dwOptions, m_dwInitSize, m_dwMaxSize);

		return IsValid();
	}

public:
	CPrivateHeapImpl(DWORD dwOptions = 0, SIZE_T dwInitSize = 0, SIZE_T dwMaxSize = 0)
	: m_dwOptions(dwOptions), m_dwInitSize(dwInitSize), m_dwMaxSize(dwMaxSize)
		{m_hHeap = ::HeapCreate(m_dwOptions, m_dwInitSize, m_dwMaxSize);}

	~CPrivateHeapImpl	()	{if(IsValid()) ::HeapDestroy(m_hHeap);}

	operator HANDLE	()	{return m_hHeap;}

private:
	CPrivateHeapImpl(const CPrivateHeapImpl&);
	CPrivateHeapImpl operator = (const CPrivateHeapImpl&);

private:
	HANDLE	m_hHeap;
	DWORD	m_dwOptions;
	SIZE_T	m_dwInitSize;
	SIZE_T	m_dwMaxSize;
};

class CGlobalHeapImpl
{
public:
	PVOID Alloc(SIZE_T dwSize, DWORD dwFlags = 0)
	{
		PVOID pv = malloc(dwSize);
		
		if(pv && (dwFlags & HEAP_ZERO_MEMORY))
			ZeroMemory(pv, dwSize);
		
		return pv;
	}

	PVOID ReAlloc(PVOID pvMemory, SIZE_T dwSize, DWORD dwFlags = 0)
	{
		PVOID pv = realloc(pvMemory, dwSize);

		if(pv && (dwFlags & HEAP_ZERO_MEMORY))
			ZeroMemory(pv, dwSize);
		else if(!pv)
			free(pvMemory);

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

	SIZE_T Compact	(DWORD dwFlags = 0)					{return 0;}
	SIZE_T Size		(PVOID pvMemory, DWORD dwFlags = 0)	{return _msize(pvMemory);}

	BOOL IsValid()	{return TRUE;}
	BOOL Reset()	{return TRUE;}

public:
	CGlobalHeapImpl	(DWORD dwOptions = 0, SIZE_T dwInitSize = 0, SIZE_T dwMaxSize = 0) {}
	~CGlobalHeapImpl()	{}

	operator HANDLE	()	{return nullptr;}

private:
	CGlobalHeapImpl(const CGlobalHeapImpl&);
	CGlobalHeapImpl operator = (const CGlobalHeapImpl&);
};

#ifndef _NOT_USE_PRIVATE_HEAP
	typedef CPrivateHeapImpl	CPrivateHeap;
#else
	typedef CGlobalHeapImpl		CPrivateHeap;
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
	CPrivateHeapBuffer(const CPrivateHeapBuffer&);
	CPrivateHeapBuffer operator = (const CPrivateHeapBuffer&);

private:
	CPrivateHeap&	m_hpPrivate;
	T*				m_pvMemory;
};

typedef CPrivateHeapBuffer<BYTE>	CPrivateHeapByteBuffer;
typedef CPrivateHeapBuffer<TCHAR>	CPrivateHeapStrBuffer;
