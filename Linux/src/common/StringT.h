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

#include "FuncHelper.h"

#include <stdarg.h>
#include <string.h>
#include <string>

using namespace std;

template<typename _CharT, typename _Traits = char_traits<_CharT>, typename _Alloc = allocator<_CharT>>
class CStringT : public basic_string<_CharT, _Traits, _Alloc>
{
public:

	using __super				 = basic_string<_CharT, _Traits, _Alloc>;

	using XCHAR					 = _CharT;
	using PXSTR					 = _CharT*;
	using PCXSTR				 = const _CharT*;

	using traits_type			 = typename __super::traits_type;
	using value_type			 = typename __super::value_type;
	using allocator_type		 = typename __super::allocator_type;
	using size_type				 = typename __super::size_type;
	using difference_type		 = typename __super::difference_type;
	using reference				 = typename __super::reference;
	using const_reference		 = typename __super::const_reference;
	using pointer				 = typename __super::pointer;
	using const_pointer			 = typename __super::const_pointer;
	using iterator				 = typename __super::iterator;
	using const_iterator		 = typename __super::const_iterator;
	using const_reverse_iterator = typename __super::const_reverse_iterator;
	using reverse_iterator		 = typename __super::reverse_iterator;

	using __super::clear;
	using __super::empty;
	using __super::size;
	using __super::resize;
	using __super::data;
	using __super::c_str;

private:

	constexpr static PCXSTR SPACE_CHARS = _T(" \t\r\n\f\v");

public:

	void Empty()						{clear();}
	bool IsEmpty()				const	{return empty();}
	int GetLength()				const	{return (int)size();}
	const _CharT* GetString()	const	{return c_str();}

	operator const _CharT* ()	const	{return __super::c_str();}

	_CharT* GetBuffer(int length)
	{
		resize((size_type)length);
		return (_CharT*)data();
	}

	void ReleaseBuffer(int length = -1)
	{
		if(length == -1)
			length = lstrlen(data());
		
		resize(length);
	}

	void ReleaseBufferSetLength(int length)
	{
		ASSERT(length >=0);
		ReleaseBuffer(length);
	}

	void Truncate(int length)
	{
		if(length >= GetLength())
			return;

		ReleaseBuffer(length);
	}

	int Format(const _CharT* format, ...)
	{
		_CharT* p = nullptr;

		va_list arg_ptr;
		va_start(arg_ptr, format);
		int count = vasprintf(&p, format, arg_ptr);
		va_end(arg_ptr);

		if(count >= 0)
		{
			this->assign(p, count);
			free(p);
		}

		return count;
	}

	int AppendFormat(const _CharT* format, ...)
	{
		_CharT* p = nullptr;

		va_list arg_ptr;
		va_start(arg_ptr, format);
		int count = vasprintf(&p, format, arg_ptr);
		va_end(arg_ptr);

		if(count >= 0)
		{
			this->append(p, count);
			free(p);
		}

		return count;
	}

	CStringT& Append(const _CharT* __s)
	{
		append(__s);
		return *this;
	}

	CStringT& Append(const _CharT* __s, int __n)
	{
		append(__s, __n);
		return *this;
	}

	CStringT& AppendChar(_CharT __c)
	{
		push_back(__c);
		return *this;
	}

	int Compare(const _CharT* __s) const
	{
		return lstrcmp(c_str(), __s);
	}

	int CompareNoCase(const _CharT* __s) const
	{
		return lstricmp(c_str(), __s);
	}

	bool Equals(const _CharT* __s) const
	{
		return (Compare(__s) == 0);
	}

	bool EqualsNoCase(const _CharT* __s) const
	{
		return (CompareNoCase(__s) == 0);
	}

	CStringT& MakeLower()
	{
		size_type s	= size();
		_CharT* p	= (_CharT*)c_str();
		_CharT c;

		for(size_type i = 0; i < s; i++)
		{
			c = p[i];

			if(c >= 'A' && c <= 'Z')
				p[i] = (_CharT)(c + 32);
		}

		return *this;
	}

	CStringT& MakeUpper()
	{
		size_type s	= size();
		_CharT* p	= (_CharT*)c_str();
		_CharT c;

		for(size_type i = 0; i < s; i++)
		{
			c = p[i];

			if(c >= 'a' && c <= 'z')
				p[i] = (_CharT)(c - 32);
		}

		return *this;
	}

	CStringT Mid(int iFirst, int nCount = (int)__super::npos) const
	{
		return substr(iFirst, nCount);
	}

	CStringT Left(int nCount) const
	{
		return Mid(0, nCount);
	}

	CStringT Right(int nCount) const
	{
		int nLength = GetLength();

		if(nCount >= nLength)
			return *this;

		return Mid(nLength - nCount, nCount);
	}

	CStringT Tokenize(PCXSTR lpszTokens, int& iStart) const
	{
		ASSERT(iStart >= 0);

		if((lpszTokens == nullptr) || (*lpszTokens == (_CharT)0))
		{
			if(iStart < GetLength())
				return CStringT(GetString() + iStart);
		}
		else
		{
			PCXSTR pszPlace	= GetString() + iStart;
			PCXSTR pszEnd	= GetString() + GetLength();

			if(pszPlace < pszEnd)
			{
				int nIncluding = lstrspn(pszPlace, lpszTokens);

				if((pszPlace + nIncluding) < pszEnd)
				{
					pszPlace		+= nIncluding;
					int nExcluding	 = lstrcspn(pszPlace, lpszTokens);

					int iFrom	= iStart + nIncluding;
					int nUntil	= nExcluding;
					iStart		= iFrom + nUntil + 1;

					return Mid(iFrom, nUntil);
				}
			}
		}

		iStart = -1;

		return CStringT();
	}

	CStringT& Trim()
	{
		return Trim(SPACE_CHARS);
	}

	CStringT& TrimRight()
	{
		return TrimRight(SPACE_CHARS);
	}

	CStringT& TrimLeft()
	{
		return TrimLeft(SPACE_CHARS);
	}

	CStringT& Trim(XCHAR c)
	{
		return(TrimRight(c).TrimLeft(c));
	}

	CStringT& TrimRight(XCHAR c)
	{
		int iLength = GetLength();

		if(iLength == 0)
			return *this;

		PCXSTR lpszBegin = GetString();
		PCXSTR lpszEnd	 = lpszBegin + iLength;

		while(lpszEnd > lpszBegin)
		{
			if(*(lpszEnd - 1) != c)
				break;

			--lpszEnd;
		}

		int iNewLength = (int)(lpszEnd - lpszBegin);

		if(iNewLength < iLength)
			Truncate(iNewLength);

		return *this;
	}

	CStringT& TrimLeft(XCHAR c)
	{
		int iLength = GetLength();

		if(iLength == 0)
			return *this;

		PCXSTR lpszBegin = GetString();
		PCXSTR lpszEnd	 = lpszBegin;
		int iOffset		 = 0;

		while(*lpszEnd == c)
		{
			++lpszEnd;
			++iOffset;

			if(iOffset == iLength)
				break;
		}

		if(iOffset != 0)
		{
			int iNewLength = iLength - iOffset;

			if(iNewLength > 0)
				memcpy((PXSTR)lpszBegin, lpszEnd, (iLength - iOffset) * sizeof(XCHAR));

			ReleaseBufferSetLength(iNewLength);
		}

		return *this;
	}

	CStringT& Trim(PCXSTR lpszChars)
	{
		return(TrimRight(lpszChars).TrimLeft(lpszChars));
	}

	CStringT& TrimRight(PCXSTR lpszChars)
	{
		ASSERT(!::IsStrEmpty(lpszChars));

		if(::IsStrEmpty(lpszChars))
			return *this;

		int iLength = GetLength();

		if(iLength == 0)
			return *this;

		PCXSTR lpszBegin = GetString();
		PCXSTR lpszEnd	 = lpszBegin + iLength;

		while(lpszEnd > lpszBegin)
		{
			if(::StrChr(lpszChars, *(lpszEnd - 1)) == nullptr)
				break;

			--lpszEnd;
		}

		int iNewLength = (int)(lpszEnd - lpszBegin);

		if(iNewLength < iLength)
			Truncate(iNewLength);

		return *this;
	}

	CStringT& TrimLeft(PCXSTR lpszChars)
	{
		ASSERT(!::IsStrEmpty(lpszChars));

		if(::IsStrEmpty(lpszChars))
			return *this;

		int iLength = GetLength();

		if(iLength == 0)
			return *this;

		PCXSTR lpszBegin = GetString();
		PCXSTR lpszEnd	 = lpszBegin;
		int iOffset		 = 0;

		while(::StrChr(lpszChars, *lpszEnd) != nullptr)
		{
			++lpszEnd;
			++iOffset;

			if(iOffset == iLength)
				break;
		}

		if(iOffset != 0)
		{
			int iNewLength = iLength - iOffset;

			if(iNewLength > 0)
				memcpy((PXSTR)lpszBegin, lpszEnd, (iLength - iOffset) * sizeof(XCHAR));

			ReleaseBufferSetLength(iNewLength);
		}

		return *this;
	}

	int Find(XCHAR c, int iStart = 0) const
	{
		ASSERT(iStart >= 0);

		int iLength = GetLength();

		if(iStart < 0 || iStart >= iLength)
			return -1;

		PCXSTR lpszBegin = GetString();
		PCXSTR lpszFind	 = ::StrChr(lpszBegin + iStart, c);

		return ((lpszFind == nullptr) ? -1 : (int)(lpszFind - lpszBegin));
	}

	int Find(PCXSTR lpszSub, int iStart = 0) const
	{
		ASSERT(iStart >= 0 && !::IsStrEmpty(lpszSub));

		int iLength = GetLength();

		if(lpszSub == nullptr || iStart < 0 || iStart > iLength)
			return -1;

		PCXSTR lpszBegin = GetString();
		PCXSTR lpszFind	 = ::StrStr(lpszBegin + iStart, lpszSub);
		
		return ((lpszFind == nullptr) ? -1 : (int)(lpszFind - lpszBegin));
	}

	int FindOneOf(PCXSTR lpszChars) const
	{
		ASSERT(!::IsStrEmpty(lpszChars));

		if(lpszChars == nullptr)
			return -1;

		PCXSTR lpszBegin = GetString();
		PCXSTR lpszFind	 = ::StrPBrk(lpszBegin, lpszChars);

		return ((lpszFind == nullptr) ? -1 : (int)(lpszFind - lpszBegin));
	}

	int ReverseFind(XCHAR c) const
	{
		PCXSTR lpszBegin = GetString();
		PCXSTR lpszFind	 = ::StrRChr(lpszBegin, c);

		return ((lpszFind == nullptr) ? -1 : (int)(lpszFind - lpszBegin));
	}

	int Remove(XCHAR c)
	{
		int iLength = GetLength();

		if(iLength == 0)
			return 0;

		PCXSTR lpszBegin = GetString();
		PXSTR lpszCur	 = (PXSTR)lpszBegin;
		PCXSTR lpszEnd	 = lpszBegin + iLength;
		int iRemoved	 = 0;

		while(lpszCur < lpszEnd)
		{
			if(*lpszCur == c)
				++iRemoved;
			else if(iRemoved > 0)
				*(lpszCur - iRemoved) = *lpszCur;

			++lpszCur;
		}

		if(iRemoved > 0)
			ReleaseBufferSetLength(iLength - iRemoved);

		return iRemoved;
	}

	XCHAR GetAt(int i) const
	{
		return (*this)[i];
	}

	void SetAt(int i, XCHAR c)
	{
		(*this)[i] = c;
	}

	XCHAR operator[](int i) const
	{
		ASSERT(i >= 0 && i < GetLength());

		return *(GetString() + i);
	}

	XCHAR& operator[](int i)
	{
		ASSERT(i >= 0 && i < GetLength());

		return *(PXSTR)(GetString() + i);
	}

	CStringT& Insert(int i, XCHAR c)
	{
		return insert((size_type)i, 1, c);
	}

	CStringT& Insert(int i, PCXSTR lpszChars)
	{
		return insert((size_type)i, lpszChars);
	}

	CStringT& SetString(PCXSTR lpszStr)
	{
		return assign(lpszStr);
	}

	CStringT& SetString(PCXSTR lpszStr, int iLength)
	{
		return assign(lpszStr, iLength);
	}

	friend bool operator==(const CStringT& str1, const CStringT& str2)
	{
		return (str1.Compare(str2) == 0);
	}

	friend bool operator==(const CStringT& str1, const _CharT* psz2)
	{
		return (str1.Compare(psz2) == 0);
	}

	friend bool operator==(const _CharT* psz1, const CStringT& str2)
	{
		return (str2.Compare(psz1) == 0);
	}

	friend bool operator!=(const CStringT& str1, const CStringT& str2)
	{
		return !(str1 == str2);
	}

	friend bool operator!=(const CStringT& str1, const _CharT* psz2)
	{
		return !(str1 == psz2);
	}

	friend bool operator!=(const _CharT* psz1, const CStringT& str2)
	{
		return !(psz1 == str2);
	}

public:
	CStringT() : __super() {};

	explicit CStringT(const _Alloc& __a)
	: __super(__a) {}

	CStringT(const __super& __str)
	: __super(__str) {}

	CStringT(const CStringT& __str)
	: __super(__str) {}

	CStringT(const __super& __str, size_type __pos, size_type __n = __super::npos)
	: __super(__str, __pos, __n) {}

	CStringT(const __super& __str, size_type __pos, size_type __n, const _Alloc& __a)
	: __super(__str, __pos, __n, __a) {}

	CStringT(const _CharT* __s, size_type __n, const _Alloc& __a = _Alloc())
	: __super(::SafeStr(__s), __n, __a) {}

	CStringT(const _CharT* __s, const _Alloc& __a = _Alloc())
	: __super(::SafeStr(__s), __a) {}

	CStringT(size_type __n, _CharT __c, const _Alloc& __a = _Alloc())
	: __super(__n, __c, __a) {}

#if __cplusplus >= 201103L
	CStringT(__super&& __str)
	: __super(__str) {}

	CStringT(CStringT&& __str)
	: __super(__str) {}

	CStringT(initializer_list<_CharT> __l, const _Alloc& __a = _Alloc())
	: __super(__l, __a) {}
#endif // C++11

	template<class _InputIterator>
	CStringT(_InputIterator __beg, _InputIterator __end, const _Alloc& __a = _Alloc())
	: __super(__beg, __end, __a) {}

	~CStringT() = default;

	CStringT& operator=(const __super& __str)
	{__super::operator=(__str); return *this;}

	CStringT& operator=(const CStringT& __str)
	{__super::operator=(__str); return *this;}

	CStringT& operator=(const _CharT* __s)
	{__super::operator=(::SafeStr(__s)); return *this;}

	CStringT& operator=(_CharT __c)
	{__super::operator=(__c); return *this;}

#if __cplusplus >= 201103L
	CStringT& operator=(__super&& __str)
	{__super::operator=(__str); return *this;}

	CStringT& operator=(CStringT&& __str)
	{__super::operator=(__str); return *this;}

	CStringT& operator=(initializer_list<_CharT> __l)
	{__super::operator=(__l); return *this;}
#endif // C++11

public:
	CStringT& operator+=(const __super& __str)
	{__super::operator+=(__str); return *this;}

	CStringT& operator+=(const _CharT* __s)
	{__super::operator+=(::SafeStr(__s)); return *this;}

	CStringT& operator+=(_CharT __c)
	{__super::operator+=(__c); return *this;}

#if __cplusplus >= 201103L
	CStringT& operator+=(initializer_list<_CharT> __l)
	{__super::operator+=(__l); return *this;}
#endif // C++11

	CStringT& append(const __super& __str)
	{__super::append(__str); return *this;}

	CStringT& append(const __super& __str, size_type __pos, size_type __n)
	{__super::append(__str, __pos, __n); return *this;}

	CStringT& append(const _CharT* __s, size_type __n)
	{__super::append(::SafeStr(__s), __n); return *this;}

	CStringT& append(const _CharT* __s)
	{__super::append(::SafeStr(__s)); return *this;}

	CStringT& append(size_type __n, _CharT __c)
	{__super::append(__n, __c); return *this;}

#if __cplusplus >= 201103L
	CStringT& append(initializer_list<_CharT> __l)
	{__super::append(__l); return *this;}
#endif // C++11

	template<class _InputIterator>
	CStringT& append(_InputIterator __first, _InputIterator __last)
	{__super::append(__first, __last); return *this;}

	void push_back(_CharT __c)
	{__super::push_back(__c);}

	CStringT& assign(const __super& __str)
	{__super::assign(__str); return *this;}

#if __cplusplus >= 201103L
	CStringT& assign(__super&& __str)
	{__super::assign(__str); return *this;}
#endif // C++11

	CStringT& assign(const __super& __str, size_type __pos, size_type __n)
	{__super::assign(__str, __pos, __n); return *this;}

	CStringT& assign(const _CharT* __s, size_type __n)
	{__super::assign(::SafeStr(__s), __n); return *this;}

	CStringT& assign(const _CharT* __s)
	{__super::assign(::SafeStr(__s)); return *this;}

	CStringT& assign(size_type __n, _CharT __c)
	{__super::assign(__n, __c); return *this;}

	template<class _InputIterator>
	CStringT& assign(_InputIterator __first, _InputIterator __last)
	{__super::assign(__first, __last); return *this;}

#if __cplusplus >= 201103L
	CStringT& assign(initializer_list<_CharT> __l)
	{__super::assign(__l); return *this;}
#endif // C++11

	CStringT& insert(size_type __pos1, const __super& __str)
	{__super::insert(__pos1, __str); return *this;}

	CStringT& insert(size_type __pos1, const __super& __str, size_type __pos2, size_type __n)
	{__super::insert(__pos1, __str, __pos2, __n); return *this;}

	CStringT& insert(size_type __pos, const _CharT* __s, size_type __n)
	{__super::insert(__pos, __s, __n); return *this;}

	CStringT& insert(size_type __pos, const _CharT* __s)
	{__super::insert(__pos, __s); return *this;}

	CStringT& insert(size_type __pos, size_type __n, _CharT __c)
	{__super::insert(__pos, __n, __c); return *this;}

	CStringT& erase(size_type __pos = 0, size_type __n = __super::npos)
	{__super::erase(__pos, __n); return *this;}

	CStringT& replace(size_type __pos, size_type __n, const __super& __str)
	{__super::replace(__pos, __n, __str); return *this;}

	CStringT& replace(size_type __pos1, size_type __n1, const __super& __str, size_type __pos2, size_type __n2)
	{__super::replace(__pos1, __n1, __str, __pos2, __n2); return *this;}

	CStringT& replace(size_type __pos, size_type __n1, const _CharT* __s, size_type __n2)
	{__super::replace(__pos, __n1, __s, __n2); return *this;}

	CStringT& replace(size_type __pos, size_type __n1, const _CharT* __s)
	{__super::replace(__pos, __n1, __s); return *this;}

	CStringT& replace(size_type __pos, size_type __n1, size_type __n2, _CharT __c)
	{__super::replace(__pos, __n1, __n2, __c); return *this;}

	CStringT& replace(iterator __i1, iterator __i2, const __super& __str)
	{__super::replace(__i1, __i2, __str); return *this;}

	CStringT& replace(iterator __i1, iterator __i2, const _CharT* __s, size_type __n)
	{__super::replace(__i1, __i2, __s, __n); return *this;}

	CStringT& replace(iterator __i1, iterator __i2, const _CharT* __s)
	{__super::replace(__i1, __i2, __s); return *this;}

	CStringT& replace(iterator __i1, iterator __i2, size_type __n, _CharT __c)
	{__super::replace(__i1, __i2, __n, __c); return *this;}

	template<class _InputIterator>
	CStringT& replace(iterator __i1, iterator __i2, _InputIterator __k1, _InputIterator __k2)
	{__super::replace(__i1, __i2, __k1, __k2); return *this;}

	CStringT& replace(iterator __i1, iterator __i2, _CharT* __k1, _CharT* __k2)
	{__super::replace(__i1, __i2, __k1, __k2); return *this;}

	CStringT& replace(iterator __i1, iterator __i2, const _CharT* __k1, const _CharT* __k2)
	{__super::replace(__i1, __i2, __k1, __k2); return *this;}

	CStringT& replace(iterator __i1, iterator __i2, iterator __k1, iterator __k2)
	{__super::replace(__i1, __i2, __k1, __k2); return *this;}

	CStringT& replace(iterator __i1, iterator __i2, const_iterator __k1, const_iterator __k2)
	{__super::replace(__i1, __i2, __k1, __k2); return *this;}

#if __cplusplus >= 201103L
	CStringT& replace(iterator __i1, iterator __i2, initializer_list<_CharT> __l)
	{__super::replace(__i1, __i2, __l); return *this;}
#endif // C++11

	CStringT substr(size_type __pos = 0, size_type __n = __super::npos) const
	{return __super::substr(__pos, __n);}

};

template<typename _CharT, typename _Traits, typename _Alloc>
CStringT<_CharT, _Traits, _Alloc>
	operator+(const CStringT<_CharT, _Traits, _Alloc>& __lhs, const CStringT<_CharT, _Traits, _Alloc>& __rhs)
{
	CStringT<_CharT, _Traits, _Alloc> __str(__lhs);
	__str.append(__rhs);
	return __str;
}

template<typename _CharT, typename _Traits, typename _Alloc>
CStringT<_CharT,_Traits,_Alloc>
	operator+(const _CharT* __lhs, const CStringT<_CharT,_Traits,_Alloc>& __rhs);

template<typename _CharT, typename _Traits, typename _Alloc>
CStringT<_CharT,_Traits,_Alloc>
	operator+(_CharT __lhs, const CStringT<_CharT,_Traits,_Alloc>& __rhs);

template<typename _CharT, typename _Traits, typename _Alloc>
inline CStringT<_CharT, _Traits, _Alloc>
	operator+(const CStringT<_CharT, _Traits, _Alloc>& __lhs, const _CharT* __rhs)
{
	CStringT<_CharT, _Traits, _Alloc> __str(__lhs);
	__str.append(__rhs);
	return __str;
}

template<typename _CharT, typename _Traits, typename _Alloc>
inline CStringT<_CharT, _Traits, _Alloc>
	operator+(const CStringT<_CharT, _Traits, _Alloc>& __lhs, _CharT __rhs)
{
	typedef CStringT<_CharT, _Traits, _Alloc>	__string_type;
	typedef typename __string_type::size_type	__size_type;
	__string_type __str(__lhs);
	__str.append(__size_type(1), __rhs);
	return __str;
}

#if __cplusplus >= 201103L
template<typename _CharT, typename _Traits, typename _Alloc>
inline CStringT<_CharT, _Traits, _Alloc>
	operator+(CStringT<_CharT, _Traits, _Alloc>&& __lhs, const CStringT<_CharT, _Traits, _Alloc>& __rhs)
{return std::move(__lhs.append(__rhs));}

template<typename _CharT, typename _Traits, typename _Alloc>
inline CStringT<_CharT, _Traits, _Alloc>
	operator+(const CStringT<_CharT, _Traits, _Alloc>& __lhs, CStringT<_CharT, _Traits, _Alloc>&& __rhs)
{return std::move(__rhs.insert(0, __lhs));}

template<typename _CharT, typename _Traits, typename _Alloc>
inline CStringT<_CharT, _Traits, _Alloc>
	operator+(CStringT<_CharT, _Traits, _Alloc>&& __lhs, CStringT<_CharT, _Traits, _Alloc>&& __rhs)
{
	const auto __size = __lhs.size() + __rhs.size();
	const bool __cond = (__size > __lhs.capacity()
		&& __size <= __rhs.capacity());
	return __cond ? std::move(__rhs.insert(0, __lhs))
		: std::move(__lhs.append(__rhs));
}

template<typename _CharT, typename _Traits, typename _Alloc>
inline CStringT<_CharT, _Traits, _Alloc>
	operator+(const _CharT* __lhs, CStringT<_CharT, _Traits, _Alloc>&& __rhs)
{return std::move(__rhs.insert(0, __lhs));}

template<typename _CharT, typename _Traits, typename _Alloc>
inline CStringT<_CharT, _Traits, _Alloc>
	operator+(_CharT __lhs,	CStringT<_CharT, _Traits, _Alloc>&& __rhs)
{return std::move(__rhs.insert(0, 1, __lhs));}

template<typename _CharT, typename _Traits, typename _Alloc>
inline CStringT<_CharT, _Traits, _Alloc>
	operator+(CStringT<_CharT, _Traits, _Alloc>&& __lhs, const _CharT* __rhs)
{return std::move(__lhs.append(__rhs));}

template<typename _CharT, typename _Traits, typename _Alloc>
inline CStringT<_CharT, _Traits, _Alloc>
	operator+(CStringT<_CharT, _Traits, _Alloc>&& __lhs, _CharT __rhs)
{return std::move(__lhs.append(1, __rhs));}
#endif

using CStringA			= CStringT<char>;
using CStringW			= CStringT<wchar_t>;

using CStdStringA		= string;
using CStdStringW		= wstring;

#ifdef _UNICODE
	using CString		= CStringW;
	using CStdString	= CStdStringW;
#else
	using CString		= CStringA;
	using CStdString	= CStdStringA;
#endif

#define _HASH_SEED	(size_t)0xdeadbeef

template<class _Kty>
inline size_t hash_value(const _Kty& _Keyval)
{
	return ((size_t)_Keyval ^ _HASH_SEED);
}

template <class _InIt>
inline size_t _Hash_value(_InIt _Begin, _InIt _End)
{
	size_t _Val = 2166136261U;

	while(_Begin != _End)
		_Val = 16777619U * _Val ^ (size_t)*_Begin++;

	return (_Val);
}

template<class _Elem, class _Traits, class _Alloc>
inline size_t hash_value(const basic_string<_Elem, _Traits, _Alloc>& _Str)
{
	const _Elem *_Ptr = _Str.c_str();

	return (_Hash_value(_Ptr, _Ptr + _Str.size()));
}

template<class _Elem>
inline size_t hash_value(const CStringT<_Elem>& _Str)
{
	const _Elem *_Ptr = _Str.c_str();

	return (_Hash_value(_Ptr, _Ptr + _Str.size()));
}

inline size_t hash_value(const char *_Str)
{
	return (_Hash_value(_Str, _Str + strlen(_Str)));
}

inline size_t hash_value(const wchar_t *_Str)
{
	return (_Hash_value(_Str, _Str + wcslen(_Str)));
}
