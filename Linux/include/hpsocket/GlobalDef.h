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

#include <sys/types.h>

typedef int						BOOL;
typedef float					FLOAT;
typedef FLOAT					*PFLOAT, *LPFLOAT;
typedef double					DOUBLE;
typedef DOUBLE					*PDOUBLE, *LPDOUBLE;
typedef short					SHORT, INT16;
typedef SHORT					*PSHORT, *LPSHORT;
typedef unsigned short			USHORT, UINT16;
typedef USHORT					*PUSHORT, *LPUSHORT;
typedef unsigned short			WORD;
typedef WORD					*PWORD, *LPWORD;
typedef unsigned int			DWORD;
typedef DWORD					*PDWORD, *LPDWORD;
typedef long					LONG, LID;
typedef LONG					*PLONG, *LPLONG;
typedef unsigned long			ULONG, ULID;
typedef ULONG					*PULONG, *LPULONG;
typedef long long				LONGLONG, LLONG, INT64;
typedef LONGLONG				*PLONGLONG, *LPLONGLONG, *PLLONG, *LPLLONG;
typedef unsigned long long		ULONGLONG, ULLONG, UINT64;
typedef ULONGLONG				*PULONGLONG, *LPULONGLONG, *PULLONG, *LPULLONG;
typedef int						INT, IID, INT32;
typedef INT						*PINT, *LPINT;
typedef unsigned int			UINT, UIID, UINT32;
typedef UINT					*PUINT, *LPUINT;
typedef void					VOID;
typedef VOID					*PVOID, *LPVOID;
typedef char					CHAR, INT8;
typedef CHAR					*PCHAR, *LPCHAR, *PSTR, *LPSTR;
typedef const char				*PCSTR, *LPCSTR;
typedef unsigned char			BYTE, UINT8;
typedef BYTE					*PBYTE, *LPBYTE;
typedef const BYTE				*PCBYTE, *LPCBYTE;
typedef wchar_t					WCHAR;
typedef WCHAR					*PWSTR, *LPWSTR;
typedef const WCHAR				*PCWSTR, *LPCWSTR;
typedef LONG					INT_PTR, LONG_PTR, LPARAM;
typedef ULONG					UINT_PTR, ULONG_PTR, DWORD_PTR, WPARAM;

typedef IID						FD, HANDLE, SOCKET;
typedef INT						LRESULT, HRESULT;

typedef LLONG					__time64_t;
typedef INT						__time32_t;

typedef LID						NTHR_ID;

#ifdef __ANDROID__
	typedef LID					THR_ID;
#else
	typedef ULID				THR_ID;
#endif

typedef size_t					SIZE_T;
typedef ssize_t					SSIZE_T;

#ifdef _UNICODE
	typedef WCHAR				TCHAR;
#else
	typedef CHAR				TCHAR;
#endif

typedef TCHAR					*PTSTR, *LPTSTR;
typedef const TCHAR				*PCTSTR, *LPCTSTR;

#define MAXUINT8				((UINT8)~((UINT8)0))
#define MAXINT8					((INT8)(MAXUINT8 >> 1))
#define MININT8					((INT8)~MAXINT8)

#define MAXUINT16				((UINT16)~((UINT16)0))
#define MAXINT16				((INT16)(MAXUINT16 >> 1))
#define MININT16				((INT16)~MAXINT16)

#define MAXUINT32				((UINT32)~((UINT32)0))
#define MAXINT32				((INT32)(MAXUINT32 >> 1))
#define MININT32				((INT32)~MAXINT32)

#define MAXUINT64				((UINT64)~((UINT64)0))
#define MAXINT64				((INT64)(MAXUINT64 >> 1))
#define MININT64				((INT64)~MAXINT64)

#define MAXULONG				((ULONG)~((ULONG)0))
#define MAXLONG					((LONG)(MAXULONG >> 1))
#define MINLONG					((LONG)~MAXLONG)

#define MAXULONGLONG			((ULONGLONG)~((ULONGLONG)0))
#define MINLONGLONG				((LONGLONG)~MAXLONGLONG)

#define MAXSIZE_T				((SIZE_T)~((SIZE_T)0))
#define MAXSSIZE_T				((SSIZE_T)(MAXSIZE_T >> 1))
#define MINSSIZE_T				((SSIZE_T)~MAXSSIZE_T)

#define MAXUINT					((UINT)~((UINT)0))
#define MAXINT					((INT)(MAXUINT >> 1))
#define MININT					((INT)~MAXINT)

#define MAXDWORD32				((DWORD32)~((DWORD32)0))
#define MAXDWORD64				((DWORD64)~((DWORD64)0))

#define MINBYTE					0x00
#define MAXBYTE					0xFF
#define MINCHAR					0x80
#define MAXCHAR					0x7F
#define MINSHORT				0x8000
#define MAXSHORT				0x7FFF
#define MINUSHORT				0x0000
#define MAXUSHORT				0xFFFF
#define MINWORD					0x0000
#define MAXWORD					0xFFFF
#define MINDWORD				0x00000000
#define MAXDWORD				0xFFFFFFFF


#ifdef _UNICODE
	#define __T(x)				L ## x
#else
	#define __T(x)				x
	#define T2A(p)				(p)
	#define A2T(p)				(p)
	#define A2CT(p)				(p)
	#define T2CA(p)				(p)
	#define CT2A(p)				(p)
	#define CA2T(p)				(p)
	#define CA2CT(p)			(p)
#endif

#define _T(x)					__T(x)
#define _TEXT(x)				__T(x)

#define _In_
#define _Out_
#define _Inout_
#define _In_opt_
#define _Out_opt_
#define _Inout_opt_
#define USES_CONVERSION

#define INFINITE				-1
#define NO_ERROR				0
#define HAS_ERROR				-1
#define TIMEOUT					0
#define RS_OK					NO_ERROR
#define RS_FAIL					HAS_ERROR
#define RS_TIMEOUT				TIMEOUT
#define INVALID_FD				-1
#define INVALID_HANDLE_VALUE	INVALID_FD
#define INVALID_PVOID			((PVOID)-1)
#define _MAX_PATH				256
#define MAX_PATH				_MAX_PATH
#define TRUE					1
#define FALSE					0
#define CONST					const

#define MAKEWORD(a, b)			((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)			((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define LOWORD(l)				((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)				((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)				((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)				((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))

#if !defined(MAX)
	#define MAX(a,b)			(((a) >= (b)) ? (a) : (b))
#endif

#if !defined(MIN)
	#define MIN(a,b)			(((a) <= (b)) ? (a) : (b))
#endif

#if !defined(_max)
	#define _max(a,b)			MAX(a,b)
#endif

#if !defined(_min)
	#define _min(a,b)			MIN(a,b)
#endif

#if defined(NDEBUG)
	#if !defined(_NDEBUG)
		#define _NDEBUG
	#endif
	#if defined(DEBUG)
		#undef DEBUG
	#endif
	#if defined(_DEBUG)
		#undef _DEBUG
	#endif
	#if defined(DEBUG_TRACE)
		#undef DEBUG_TRACE
	#endif
#else
	#if defined(_NDEBUG)
		#undef _NDEBUG
	#endif
	#if !defined(DEBUG)
		#define DEBUG
	#endif
	#if !defined(_DEBUG)
		#define _DEBUG
	#endif
#endif

#if defined(__arm64__) && !defined(__aarch64__)
	#define __aarch64__	__arm64__
#elif defined(__aarch64__) && !defined(__arm64__)
	#define __arm64__	__aarch64__
#endif

#ifdef __cplusplus
	#define EXTERN_C			extern "C"
	#define EXTERN_C_BEGIN		EXTERN_C {
	#define EXTERN_C_END		}
#else
	#define EXTERN_C			extern
	#define EXTERN_C_BEGIN
	#define EXTERN_C_END
#endif

#if !defined(__stdcall)
	#define __stdcall			__attribute__ ((__stdcall__))
#endif

#if !defined(__cdecl)
	#define __cdecl				__attribute__ ((__cdecl__))
#endif

#if !defined(_GNU_SOURCE)
	#define _GNU_SOURCE
#endif

#if !defined(CALLBACK)
	#define CALLBACK
#endif

#if !defined(WINAPI)
	#define WINAPI
#endif
