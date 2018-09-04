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

#define FPRINTLN(fd, fmt, ...)			fprintf((fd), fmt "\n", ##__VA_ARGS__)
#define PRINTLN(fmt, ...)				FPRINTLN(stdout, fmt, ##__VA_ARGS__)

#define IS_OK(rs)						((BOOL)(rs))
#define IS_NOT_OK(rs)					(!IS_OK(rs))

#define HAS_ERROR						-1
#define CHECK_IS_OK(expr)				{if(IS_NOT_OK(expr)) return FALSE;}
#define CHECK_ERROR_FD(fd)				{if(IS_INVALID_FD(fd)) return FALSE;}
#define CHECK_ERROR_INVOKE(expr)		{if(!IS_NO_ERROR(expr)) return FALSE;}
#define CHECK_ERROR_CODE(rs)			{if(!IS_NO_ERROR(rs)) {::SetLastError(rs); return FALSE;}}
#define CHECK_ERROR(expr, code)			{if(!(expr)) {::SetLastError(code); return FALSE;}}
#define CHECK_EINVAL(expr)				CHECK_ERROR(expr, ERROR_INVALID_PARAMETER)
#define ASSERT_CHECK_ERROR(expr, code)	{ASSERT(expr); CHECK_ERROR(expr, code);}
#define ASSERT_CHECK_EINVAL(expr)		{ASSERT(expr); CHECK_EINVAL(expr);}

#define CHECK_IS_ERROR(code)			(::GetLastError() == code)
#define CONTINUE_IF_ERROR(code)			{if(CHECK_IS_ERROR(code)) continue;}
#define BREAK_IF_ERROR(code)			{if(CHECK_IS_ERROR(code)) break;}

#define IS_WOULDBLOCK_ERROR()			CHECK_IS_ERROR(WSAEWOULDBLOCK)
#define CONTINUE_WOULDBLOCK_ERROR()		CONTINUE_IF_ERROR(WSAEWOULDBLOCK)
#define BREAK_WOULDBLOCK_ERROR()		BREAK_IF_ERROR(WSAEWOULDBLOCK)
#define IS_IO_PENDING_ERROR()			CHECK_IS_ERROR(ERROR_IO_PENDING)
#define CONTINUE_IO_PENDING_ERROR()		CONTINUE_IF_ERROR(ERROR_IO_PENDING)
#define BREAK_IO_PENDING_ERROR()		BREAK_IF_ERROR(ERROR_IO_PENDING)

#define EqualMemory(dest, src, len)		(!memcmp((dest), (src), (len)))
#define ZeroObject(obj)					ZeroMemory((&(obj)), sizeof(obj))

#define EXECUTE_RESET_ERROR(expr)		(::SetLastError(0), (expr))
#define EXECUTE_RESTORE_ERROR(expr)		{int __le_ = ::GetLastError(); (expr); ::SetLastError(__le_);}
inline int ENSURE_ERROR(int def_code)	{int __le_ = ::GetLastError(); if(__le_ == 0) __le_ = (def_code);  return __le_;}
#define ENSURE_ERROR_CANCELLED			ENSURE_ERROR(ERROR_CANCELLED)
#define TRIGGER(expr)					EXECUTE_RESET_ERROR((expr))

#define CreateLocalObjects(T, n)		((T*)alloca(sizeof(T) * (n)))
#define CreateLocalObject(T)			CreateLocalObjects(T, 1)
#define CallocObjects(T, n)				((T*)calloc((n), sizeof(T)))

#define MALLOC(T, n)					((T*)malloc(sizeof(T) * (n)))
#define REALLOC(p, T, n)				((T*)realloc((PVOID)(p), sizeof(T) * (n)))
#define FREE(p)							free((PVOID)(p))

#define ERROR_EXIT2(code, err)			EXIT((code), (err), __FILE__, __LINE__, __FUNCTION__)
#define ERROR__EXIT2(code, err)			_EXIT((code), (err), __FILE__, __LINE__, __FUNCTION__)
#define ERROR_ABORT2(err)				ABORT((err), __FILE__, __LINE__, __FUNCTION__)

#define ERROR_EXIT(code)				ERROR_EXIT2((code), -1)
#define ERROR__EXIT(code)				ERROR__EXIT2((code), -1)
#define ERROR_ABORT()					ERROR_ABORT2(-1)

typedef HANDLE							FD;
#define INVALID_FD						INVALID_HANDLE_VALUE
#define IS_VALID_FD(fd)					((fd) != INVALID_FD)
#define IS_INVALID_FD(fd)				(!IS_VALID_FD(fd))

#define INVALID_PVOID					INVALID_HANDLE_VALUE
#define IS_VALID_PVOID(pv)				((pv) != INVALID_PVOID)
#define IS_INVALID_PVOID(pv)			(!IS_VALID_PVOID(pv))

#define TO_PVOID(v)						((PVOID)(UINT_PTR)(v))
#define FROM_PVOID(T, pv)				((T)(UINT_PTR)(pv))

#define HEX_CHAR_TO_VALUE(c)			(c <= '9' ? c - '0' : (c <= 'F' ? c - 'A' + 0x0A : c - 'a' + 0X0A))
#define HEX_DOUBLE_CHAR_TO_VALUE(pc)	((BYTE)(((HEX_CHAR_TO_VALUE(*(pc))) << 4) | (HEX_CHAR_TO_VALUE(*((pc) + 1)))))
#define HEX_VALUE_TO_CHAR(n)			(n <= 9 ? n + '0' : (n <= 'F' ? n + 'A' - 0X0A : n + 'a' - 0X0A))
#define HEX_VALUE_TO_DOUBLE_CHAR(pc, n)	{*(pc) = (BYTE)HEX_VALUE_TO_CHAR((n >> 4)); *((pc) + 1) = (BYTE)HEX_VALUE_TO_CHAR((n & 0X0F));}

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

template<typename T> inline bool IS_HAS_ERROR(T v)
{
	return v == (T)HAS_ERROR;
}

template<typename T> inline bool IS_NO_ERROR(T v)
{
	return v == (T)NO_ERROR;
}

template<typename T1, typename T2> inline void CopyPlainObject(T1* p1, const T2* p2)
{
	CopyMemory(p1, p2, sizeof(T1));
}

void EXIT(int iExitCode = 0, int iErrno = -1, LPCSTR lpszFile = nullptr, int iLine = 0, LPCSTR lpszFunc = nullptr, LPCSTR lpszTitle = nullptr);
void _EXIT(int iExitCode = 0, int iErrno = -1, LPCSTR lpszFile = nullptr, int iLine = 0, LPCSTR lpszFunc = nullptr, LPCSTR lpszTitle = nullptr);
void ABORT(int iErrno = -1, LPCSTR lpszFile = nullptr, int iLine = 0, LPCSTR lpszFunc = nullptr, LPCSTR lpszTitle = nullptr);
