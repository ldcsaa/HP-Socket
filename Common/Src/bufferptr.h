/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.8
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

#include <memory.h>
#include <malloc.h>

template<class T, size_t MAX_CACHE_SIZE = 0>
class CBufferPtrT
{
public:
	explicit CBufferPtrT(size_t size = 0, bool zero = false)		{Reset(); Malloc(size, zero);}
	explicit CBufferPtrT(const T* pch, size_t size)					{Reset(); Copy(pch, size);}
	CBufferPtrT(const CBufferPtrT& other)							{Reset(); Copy(other);}
	template<size_t S> CBufferPtrT(const CBufferPtrT<T, S>& other)	{Reset(); Copy(other);}

	~CBufferPtrT() {Free();}

	T* Malloc(size_t size = 1, bool zero = false)
	{
		Free();
		return Alloc(size, zero, false);
	}

	T* Realloc(size_t size, bool zero = false)
	{
		return Alloc(size, zero, true);
	}

	void Free()
	{
		if(m_pch)
		{
			free(m_pch);
			Reset();
		}
	}

	template<size_t S> CBufferPtrT& Copy(const CBufferPtrT<T, S>& other)
	{
		if((void*)&other != (void*)this)
			Copy(other.Ptr(), other.Size());

		return *this;
	}

	CBufferPtrT& Copy(const T* pch, size_t size)
	{
		Malloc(size);

		if(m_pch)
			memcpy(m_pch, pch, size * sizeof(T));

		return *this;
	}

	template<size_t S> CBufferPtrT& Cat(const CBufferPtrT<T, S>& other)
	{
		if((void*)&other != (void*)this)
			Cat(other.Ptr(), other.Size());

		return *this;
	}

	CBufferPtrT& Cat(const T* pch, size_t size = 1)
	{
		size_t pre_size = m_size;
		Realloc(m_size + size);

		if(m_pch)
			memcpy(m_pch + pre_size, pch, size * sizeof(T));

		return *this;
	}

	template<size_t S> bool Equal(const CBufferPtrT<T, S>& other) const
	{
		if((void*)&other == (void*)this)
			return true;
		else if(m_size != other.Size())
			return false;
		else if(m_size == 0)
			return true;
		else
			return (memcmp(m_pch, other.Ptr(), m_size * sizeof(T)) == 0);
	}

	bool Equal(T* pch) const
	{
		if(m_pch == pch)
			return true;
		else if(!m_pch || !pch)
			return false;
		else
			return (memcmp(m_pch, pch, m_size * sizeof(T)) == 0);
	}

	T*			Ptr()					{return m_pch;}
	const T*	Ptr()			const	{return m_pch;}
	T&			Get(int i)				{return *(m_pch + i);}
	const T&	Get(int i)		const	{return *(m_pch + i);}
	size_t		Size()			const	{return m_size;}
	bool		IsValid()		const	{return m_pch != 0;}

	operator							T*	()									{return Ptr();}
	operator const						T*	()			const					{return Ptr();}
	T& operator							[]	(int i)								{return Get(i);}
	const T& operator					[]	(int i)		const					{return Get(i);}
	bool operator						==	(T* pv)		const					{return Equal(pv);}
	template<size_t S> bool operator	==	(const CBufferPtrT<T, S>& other)	{return Equal(other);}
	CBufferPtrT& operator				=	(const CBufferPtrT& other)			{return Copy(other);}
	template<size_t S> CBufferPtrT& operator = (const CBufferPtrT<T, S>& other)	{return Copy(other);}

private:
	void Reset()						{m_pch = 0; m_size = 0; m_capacity = 0;}
	size_t GetAllocSize(size_t size)	{return max(size, min(size * 2, m_size + MAX_CACHE_SIZE));}

	T* Alloc(size_t size, bool zero = false, bool is_realloc = false)
	{
		if(size >= 0 && size != m_size)
		{
			size_t rsize = GetAllocSize(size);
			if(size > m_capacity || rsize < m_size)
			{
				m_pch = is_realloc							?
					(T*)realloc(m_pch, rsize * sizeof(T))	:
					(T*)malloc(rsize * sizeof(T))			;

				if(m_pch || rsize == 0)
				{
					m_size		= size;
					m_capacity	= rsize;
				}
				else
					Reset();
			}
			else
				m_size = size;
		}

		if(zero && m_pch)
			memset(m_pch, 0, m_size * sizeof(T));

		return m_pch;
	}

private:
	T*		m_pch;
	size_t	m_size;
	size_t	m_capacity;
};

typedef CBufferPtrT<char>			CCharBufferPtr;
typedef CBufferPtrT<wchar_t>		CWCharBufferPtr;
typedef CBufferPtrT<unsigned char>	CByteBufferPtr;
typedef CByteBufferPtr				CBufferPtr;

#ifdef _UNICODE
	typedef CWCharBufferPtr			CTCharBufferPtr;
#else
	typedef CCharBufferPtr			CTCharBufferPtr;
#endif
