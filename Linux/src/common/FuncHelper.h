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

#include "GlobalDef.h"
#include "GlobalErrno.h"
#include "SysHelper.h"

#include <stdlib.h>
#include <sysexits.h>
#include <stdio.h>
#include <wchar.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <alloca.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include <atomic>
#include <utility>

using namespace std;

#if !defined(__ANDROID__)
	typedef atomic_ulong				atomic_tid;

	#define FPRINTLN(fd, fmt, ...)		fprintf((fd), fmt "\n", ##__VA_ARGS__)
#else
	typedef atomic_long					atomic_tid;

	#if !defined(EFD_SEMAPHORE)
		#define EFD_SEMAPHORE			00000001
	#endif

	#if defined(stdout)
		#undef stdout
	#endif

	#if defined(stderr)
		#undef stderr
	#endif

	#define stdout	nullptr
	#define stderr	nullptr

	#define FPRINTLN(fd, fmt, ...)		printf(fmt "\n", ##__VA_ARGS__)
#endif

#define PRINTLN(fmt, ...)				FPRINTLN(stdout, fmt, ##__VA_ARGS__)

#if defined(DEBUG) && defined(DEBUG_TRACE)
	#define TRACE(fmt, ...)				PRINTLN("> TRC (0x%8X, %d) " fmt, SELF_THREAD_ID, SELF_NATIVE_THREAD_ID, ##__VA_ARGS__)
#else
	#define TRACE(fmt, ...)
#endif

#define ASSERT							assert
#define VERIFY(expr)					((expr) ? TRUE : (ERROR_EXIT2(EXIT_CODE_SOFTWARE, ERROR_VERIFY_CHECK), FALSE))
#define ASSERT_IS_NO_ERROR(expr)		ASSERT(IS_NO_ERROR(expr))
#define VERIFY_IS_NO_ERROR(expr)		VERIFY(IS_NO_ERROR(expr))
#define	ENSURE(expr)					VERIFY(expr)
#define ENSURE_IS_NO_ERROR(expr)		VERIFY_IS_NO_ERROR(expr)

#define TEMP_FAILURE_RETRY_INT(exp)		((int)TEMP_FAILURE_RETRY(exp))

#define NO_EINTR						TEMP_FAILURE_RETRY
#define NO_EINTR_INT					TEMP_FAILURE_RETRY_INT

#define CHECK_IS_OK(expr)				{if(IS_NOT_OK(expr)) return FALSE;}
#define CHECK_ERROR_FD(fd)				{if(IS_INVALID_FD(fd)) return FALSE;}
#define CHECK_ERROR_INVOKE(expr)		{if(!IS_NO_ERROR(expr)) return FALSE;}
#define CHECK_ERROR_CODE(rs)			{if(!IS_NO_ERROR(rs)) {::SetLastError(rs); return FALSE;}}
#define CHECK_ERROR(expr, code)			{if(!(expr)) {::SetLastError(code); return FALSE;}}
#define CHECK_EINVAL(expr)				CHECK_ERROR(expr, ERROR_INVALID_PARAMETER)
#define ASSERT_CHECK_ERROR(expr, code)	{ASSERT(expr); CHECK_ERROR(expr, code);}
#define ASSERT_CHECK_EINVAL(expr)		{ASSERT(expr); CHECK_EINVAL(expr);}

#define SUCCEEDED(rs)					IS_NO_ERROR(rs)
#define FAILED(rs)						(!SUCCEEDED(rs))
#define IS_OK(rs)						((BOOL)(rs))
#define IS_NOT_OK(rs)					(!IS_OK(rs))

#define IS_ERROR(code)					(::GetLastError() == code)
#define CONTINUE_IF_ERROR(code)			{if(IS_ERROR(code)) continue;}
#define BREAK_IF_ERROR(code)			{if(IS_ERROR(code)) break;}

#define IS_WOULDBLOCK_ERROR()			IS_ERROR(ERROR_WOULDBLOCK)
#define CONTINUE_WOULDBLOCK_ERROR()		CONTINUE_IF_ERROR(ERROR_WOULDBLOCK)
#define BREAK_WOULDBLOCK_ERROR()		BREAK_IF_ERROR(ERROR_WOULDBLOCK)
#define IS_IO_PENDING_ERROR()			IS_ERROR(ERROR_IO_PENDING)
#define CONTINUE_IO_PENDING_ERROR()		CONTINUE_IF_ERROR(ERROR_IO_PENDING)
#define BREAK_IO_PENDING_ERROR()		BREAK_IF_ERROR(ERROR_IO_PENDING)
#define IS_INTR_ERROR()					IS_ERROR(ERROR_INTR)
#define CONTINUE_INTR_ERROR()			CONTINUE_IF_ERROR(ERROR_INTR)
#define BREAK_INTR_ERROR()				BREAK_IF_ERROR(ERROR_INTR)

#define EqualMemory(dest, src, len)		(!memcmp((dest), (src), (len)))
#define MoveMemory(dest, src, len)		memmove((dest), (src), (len))
#define CopyMemory(dest, src, len)		memcpy((dest), (src), (len))
#define FillMemory(dest, len, ch)		memset((dest), (ch), (len))
#define ZeroMemory(dest, len)			FillMemory((dest), (len), 0)
#define ZeroObject(obj)					ZeroMemory((&(obj)), sizeof(obj))

inline void SetLastError(int code)		{errno = code;}
inline int GetLastError()				{return errno;}
inline LPCSTR GetErrorStr(int code)		{return strerror(code);}
inline LPCSTR GetLastErrorStr()			{return GetErrorStr(errno);}
inline void PrintError(LPCSTR subject)	{perror(subject);}

#define EXECUTE_RESET_ERROR(expr)		(::SetLastError(0), (expr))
#define EXECUTE_RESTORE_ERROR(expr)		{int __le_ = ::GetLastError(); (expr); ::SetLastError(__le_);}
#define EXECUTE_RESTORE_ERROR_RT(T, expr)\
										({int __le_ = ::GetLastError(); T __rs_ = (expr); ::SetLastError(__le_); __rs_;})
#define ENSURE_ERROR(def_code)			({int __le_ = ::GetLastError(); if(__le_ == 0) __le_ = (def_code);  __le_;})
#define ENSURE_ERROR_CANCELLED			ENSURE_ERROR(ERROR_CANCELLED)
#define TRIGGER(expr)					EXECUTE_RESET_ERROR((expr))

#define _msize(p)						malloc_usable_size(p)
#define CreateLocalObjects(T, n)		((T*)alloca(sizeof(T) * (n)))
#define CreateLocalObject(T)			CreateLocalObjects(T, 1)
#define CallocObjects(T, n)				((T*)calloc((n), sizeof(T)))

#define MALLOC(T, n)					((T*)malloc(sizeof(T) * (n)))
#define REALLOC(p, T, n)				((T*)realloc((PVOID)(p), sizeof(T) * (n)))
#define FREE(p)							free((PVOID)(p))

#define InterlockedExchangeAdd(p, n)	__atomic_add_fetch((p), (n), memory_order_seq_cst)
#define InterlockedExchangeSub(p, n)	__atomic_sub_fetch((p), (n), memory_order_seq_cst)
#define InterlockedIncrement(p)			InterlockedExchangeAdd((p), 1)
#define InterlockedDecrement(p)			InterlockedExchangeSub((p), 1)

#define ERROR_EXIT2(code, err)			EXIT((code), (err), __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define ERROR__EXIT2(code, err)			_EXIT((code), (err), __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define ERROR_ABORT2(err)				ABORT((err), __FILE__, __LINE__, __PRETTY_FUNCTION__)

#define ERROR_EXIT(code)				ERROR_EXIT2((code), -1)
#define ERROR__EXIT(code)				ERROR__EXIT2((code), -1)
#define ERROR_ABORT()					ERROR_ABORT2(-1)

#define IS_VALID_FD(fd)					((fd) != INVALID_FD)
#define IS_INVALID_FD(fd)				(!IS_VALID_FD(fd))

#define IS_VALID_PVOID(pv)				((pv) != INVALID_PVOID)
#define IS_INVALID_PVOID(pv)			(!IS_VALID_PVOID(pv))

#define TO_PVOID(v)						((PVOID)(UINT_PTR)(v))
#define FROM_PVOID(T, pv)				((T)(UINT_PTR)(pv))

#define stricmp							strcasecmp
#define strnicmp						strncasecmp
#define wcsicmp							wcscasecmp
#define wcsnicmp						wcsncasecmp

#ifdef _UNICODE
	#define tstrchr						wcschr
	#define tstrrchr					wcsrchr
	#define tstrstr						wcsstr
	#define tstrpbrk					wcspbrk
	#define tstrtok						wcstok

	#define stscanf						swscanf
	#define tstrlen						wcslen
	#define tstrcpy						wcscpy
	#define tstrcmp						wcscmp
	#define tstricmp					wcsicmp
	#define tstrncpy					wcsncpy
	#define tstrncmp					wcsncmp
	#define tstrnicmp					wcsnicmp
	#define tstrspn						wcsspn
	#define tstrcspn					wcscspn
	#define wsprintf					swprintf
#else
	#define tstrchr						strchr
	#define tstrrchr					strrchr
	#define tstrstr						strstr
	#define tstrpbrk					strpbrk
	#define tstrtok						strtok_r

	#define stscanf						sscanf
	#define tstrlen						strlen
	#define tstrcpy						strcpy
	#define tstrcmp						strcmp
	#define tstricmp					stricmp
	#define tstrncpy					strncpy
	#define tstrncmp					strncmp
	#define tstrnicmp					strnicmp
	#define tstrspn						strspn
	#define tstrcspn					strcspn
	#define wsprintf					sprintf
#endif

inline const char* StrChr(const char* s, char c)					{return strchr(s, c);}
inline const char* StrRChr(const char* s, char c)					{return strrchr(s, c);}
inline const char* StrStr(const char* h, const char* n)				{return strstr(h, n);}
inline const char* StrPBrk(const char* s, const char* a)			{return strpbrk(s, a);}
inline const wchar_t* StrChr(const wchar_t* s, wchar_t c)			{return wcschr(s, c);}
inline const wchar_t* StrRChr(const wchar_t* s, wchar_t c)			{return wcsrchr(s, c);}
inline const wchar_t* StrStr(const wchar_t* h, const wchar_t* n)	{return wcsstr(h, n);}
inline const wchar_t* StrPBrk(const wchar_t* s, const wchar_t* a)	{return wcspbrk(s, a);}
inline LPSTR StrSep2(LPSTR* lpStr, LPCSTR lpDelim = " \t\r\n")		{LPSTR lpTok; while((lpTok = strsep(lpStr, lpDelim)) != nullptr && lpTok[0] == 0); return lpTok;}
inline LPSTR TrimLeft(LPSTR* lpStr, LPCSTR lpDelim = " \t\r\n")		{while((*lpStr)[0] != 0 && ::StrChr(lpDelim, (*lpStr)[0]) != nullptr) ++(*lpStr); return (*lpStr);}
inline LPSTR TrimRitht(LPSTR* lpStr, LPCSTR lpDelim = " \t\r\n")
{
	LPSTR lpEnd = (*lpStr) + strlen(*lpStr) - 1;
	LPSTR lpCur = lpEnd;

	while(lpCur >= (*lpStr) && ::StrChr(lpDelim, lpCur[0]) != nullptr)
		--lpCur;

	if(lpCur != lpEnd)
		lpCur[1] = 0;

	return (*lpStr);
}

inline BOOL IsStrEmptyA(LPCSTR lpsz)	{return (lpsz == nullptr || lpsz[0] == 0);}
inline BOOL IsStrEmptyW(LPCWSTR lpsz)	{return (lpsz == nullptr || lpsz[0] == 0);}
inline LPCSTR SafeStrA(LPCSTR lpsz)		{return (lpsz != nullptr) ? lpsz : "";}
inline LPCWSTR SafeStrW(LPCWSTR lpsz)	{return (lpsz != nullptr) ? lpsz : L"";}

#ifdef _UNICODE
	#define IsStrEmpty					IsStrEmptyW
	#define SafeStr						SafeStrW
#else
	#define IsStrEmpty					IsStrEmptyA
	#define SafeStr						SafeStrA
#endif

inline int lstrlen(LPCTSTR p)							{return (int)tstrlen(p);}
inline LPTSTR lstrcpy(LPTSTR d, LPCTSTR s)				{return tstrcpy(d, s);}
inline LPTSTR lstrncpy(LPTSTR d, LPCTSTR s, size_t n)	{return tstrncpy(d, s, n);}
inline int lstrcmp(LPCTSTR s1, LPCTSTR s2)				{return tstrcmp(s1, s2);}
inline int lstrncmp(LPCTSTR s1, LPCTSTR s2, size_t n)	{return tstrncmp(s1, s2, n);}
inline int lstricmp(LPCTSTR s1, LPCTSTR s2)				{return tstricmp(s1, s2);}
inline int lstrnicmp(LPCTSTR s1, LPCTSTR s2, size_t n)	{return tstrnicmp(s1, s2, n);}
inline int lstrspn(LPCTSTR s, LPCTSTR accept)			{return (int)tstrspn(s, accept);}
inline int lstrcspn(LPCTSTR s, LPCTSTR accept)			{return (int)tstrcspn(s, accept);}

template <typename T, size_t N> char (&_ArraySizeHelper(T(&arr)[N]))[N];
template <typename T, size_t N> char (&_ArraySizeHelper(const T(&arr)[N]))[N];

#define ARRAY_SIZE(arr)		(sizeof(_ArraySizeHelper(arr)))

#ifndef _countof
	#define _countof(arr)	ARRAY_SIZE(arr)
#endif

#ifndef __countof
	#define __countof(arr)	ARRAY_SIZE(arr)
#endif

#define THREAD_YIELD_CYCLE	63
#define THREAD_SWITCH_CYCLE	4095

inline void YieldThread(UINT i = THREAD_YIELD_CYCLE)
{
	if((i & THREAD_SWITCH_CYCLE) == THREAD_SWITCH_CYCLE)
		::SwitchToThread();
	else if((i & THREAD_YIELD_CYCLE) == THREAD_YIELD_CYCLE)
		::YieldProcessor();
}

INT WaitFor(DWORD dwMillSecond, DWORD dwSecond = 0, BOOL bExceptThreadInterrupted = FALSE);
INT Sleep(DWORD dwMillSecond, DWORD dwSecond = 0, BOOL bExceptThreadInterrupted = FALSE);

__time64_t	_time64(time_t* ptm = nullptr);
__time64_t	_mkgmtime64(tm* ptm);
tm*			_gmtime64(tm* ptm, __time64_t* pt);

DWORD		TimeGetTime();
ULLONG		TimeGetTime64();
DWORD		GetTimeGap32(DWORD dwOriginal, DWORD dwCurrent = 0);
ULLONG		GetTimeGap64(ULLONG ullOriginal, ULONGLONG ullCurrent = 0);
LLONG		TimevalToMillisecond(const timeval& tv);
timeval&	MillisecondToTimeval(LLONG ms, timeval& tv);
LLONG		TimespecToMillisecond(const timespec& ts);
timespec&	MillisecondToTimespec(LLONG ms, timespec& ts);
timeval&	GetFutureTimeval(LLONG ms, timeval& tv, struct timezone* ptz = nullptr);
timespec&	GetFutureTimespec(LLONG ms, timespec& ts, clockid_t clkid = CLOCK_MONOTONIC);

BOOL fcntl_SETFL(FD fd, INT fl, BOOL bSet = TRUE);

void EXIT(int iExitCode = 0, int iErrno = -1, LPCSTR lpszFile = nullptr, int iLine = 0, LPCSTR lpszFunc = nullptr, LPCSTR lpszTitle = nullptr);
void _EXIT(int iExitCode = 0, int iErrno = -1, LPCSTR lpszFile = nullptr, int iLine = 0, LPCSTR lpszFunc = nullptr, LPCSTR lpszTitle = nullptr);
void ABORT(int iErrno = -1, LPCSTR lpszFile = nullptr, int iLine = 0, LPCSTR lpszFunc = nullptr, LPCSTR lpszTitle = nullptr);

template<typename T, typename = enable_if_t<is_integral<T>::value>>
inline bool IS_INFINITE(T v)
{
	return v == (T)INFINITE;
}

template<typename T, typename = enable_if_t<is_integral<T>::value>>
inline bool IS_HAS_ERROR(T v)
{
	return v == (T)HAS_ERROR;
}

template<typename T, typename = enable_if_t<is_integral<T>::value>>
inline bool IS_NO_ERROR(T v)
{
	return v == (T)NO_ERROR;
}

template<typename T>
inline T InterlockedCompareExchange(volatile T* _Tgt, T _Value, T _Exp, BOOL _bWeek = FALSE, memory_order m1 = memory_order_seq_cst, memory_order m2 = memory_order_seq_cst)
{
	__atomic_compare_exchange_n(_Tgt, &_Exp, _Value, _bWeek, m1, m2);
	return _Exp;
}

template<typename T, typename V, typename E, typename = enable_if_t<is_same<decay_t<T>, decay_t<V>>::value && is_same<decay_t<V>, decay_t<E>>::value>>
inline V* InterlockedCompareExchangePointer(volatile T** _Tgt, V* _Value, E* _Exp, BOOL _bWeek = FALSE, memory_order m1 = memory_order_seq_cst, memory_order m2 = memory_order_seq_cst)
{
	return (V*)(ULONG_PTR)InterlockedCompareExchange((volatile ULONG_PTR*)(volatile PVOID*)_Tgt, (ULONG_PTR)(PVOID)_Value, (ULONG_PTR)(PVOID)_Exp, _bWeek, m1, m2);
}

template<typename T, typename ... A>
inline T* ConstructObject(T* p, A&& ... args)
{
	return new (p) T(forward<A>(args) ...);
}

template<typename T>
inline void DestructObject(T* p)
{
	p->T::~T();
}

template<typename T1, typename T2, typename = enable_if_t<is_same<decay_t<T1>, decay_t<T2>>::value>>
inline void CopyPlainObject(T1* p1, const T2* p2)
{
	CopyMemory(p1, p2, sizeof(T1));
}

template<typename T, typename C, typename = enable_if_t<is_integral<T>::value  && (is_same<C, char>::value || is_same<C, wchar_t>::value)>>
C* _n_2_c(T value, C* lpszDest, int radix)
{
	static const C* dig = "0123456789abcdefghijklmnopqrstuvwxyz";

	bool neg = false;

	if(is_signed<T>::value && value < 0)
	{
		value = -value;
		neg	  = true;
	}

	int n = 0;

	do
	{
		lpszDest[n++] = dig[value % radix];
		value /= radix;
	} while(value);

	if(neg) lpszDest[n++] = '-';
	lpszDest[n]			  = 0;

	C c, *p, *q;
	for(p = lpszDest, q = p + n - 1; p < q; ++p, --q)
		c = *p, *p = *q, *q = c;

	return lpszDest;
}

#define itoa(v, p, r)		_n_2_c<INT, char>((v), (p), (r))
#define ltoa(v, p, r)		_n_2_c<LONG, char>((v), (p), (r))
#define lltoa(v, p, r)		_n_2_c<LLONG, char>((v), (p), (r))
#define uitoa(v, p, r)		_n_2_c<UINT, char>((v), (p), (r))
#define ultoa(v, p, r)		_n_2_c<ULONG, char>((v), (p), (r))
#define ulltoa(v, p, r)		_n_2_c<ULLONG, char>((v), (p), (r))
#define itow(v, p, r)		_n_2_c<INT, wchar_t>((v), (p), (r))
#define ltow(v, p, r)		_n_2_c<LONG, wchar_t>((v), (p), (r))
#define lltow(v, p, r)		_n_2_c<LLONG, wchar_t>((v), (p), (r))
#define uitow(v, p, r)		_n_2_c<UINT, wchar_t>((v), (p), (r))
#define ultow(v, p, r)		_n_2_c<ULONG, wchar_t>((v), (p), (r))
#define ulltow(v, p, r)		_n_2_c<ULLONG, wchar_t>((v), (p), (r))

#define HEX_CHAR_TO_VALUE(c)			(c <= '9' ? c - '0' : (c <= 'F' ? c - 'A' + 0x0A : c - 'a' + 0X0A))
#define HEX_DOUBLE_CHAR_TO_VALUE(pc)	((BYTE)(((HEX_CHAR_TO_VALUE(*(pc))) << 4) | (HEX_CHAR_TO_VALUE(*((pc) + 1)))))
#define HEX_VALUE_TO_CHAR(n)			(n <= 9 ? n + '0' : (n <= 'F' ? n + 'A' - 0X0A : n + 'a' - 0X0A))
#define HEX_VALUE_TO_DOUBLE_CHAR(pc, n)	{*(pc) = (BYTE)HEX_VALUE_TO_CHAR((n >> 4)); *((pc) + 1) = (BYTE)HEX_VALUE_TO_CHAR((n & 0X0F));}
