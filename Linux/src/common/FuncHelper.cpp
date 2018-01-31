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

#include "FuncHelper.h"

#include <ctype.h>
#include <sched.h>
#include <iconv.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/select.h>

using namespace std;

#define DEFAULT_CRISEC_SPIN_COUNT	4096

#if defined (__x86_64__)
	#define DEFAULT_PAUSE_RETRY		16
	#define DEFAULT_PAUSE_YIELD		128
	#define DEFAULT_PAUSE_CYCLE		8192
#else
	#define DEFAULT_PAUSE_RETRY		4
	#define DEFAULT_PAUSE_YIELD		32
	#define DEFAULT_PAUSE_CYCLE		4096
#endif

INT YieldThread(UINT i)
{
	if(IS_INFINITE(i))						return YieldProcessor();
	else if(i < DEFAULT_PAUSE_RETRY)		return TRUE;
	else if(i < DEFAULT_PAUSE_YIELD)		return YieldProcessor();
	else if(i < DEFAULT_PAUSE_CYCLE - 1)	return YieldProcessor();
	else if(i < DEFAULT_PAUSE_CYCLE)		return WaitFor(1);
	else									return YieldThread(i & (DEFAULT_PAUSE_CYCLE - 1));
}

INT WaitFor(DWORD dwMillSecond, DWORD dwSecond)
{
	timeval tv {(time_t)dwSecond, (suseconds_t)(dwMillSecond * 1000)};

	return NO_EINTR_INT(select(0, nullptr, nullptr, nullptr, &tv));
}

INT Sleep(DWORD dwMillSecond, DWORD dwSecond)
{
	timespec ts_req	= {(time_t)dwSecond, (long)(dwMillSecond * 1000000)};
	timespec ts_rem	= ts_req;
	INT rs			= NO_ERROR;

	while(IS_HAS_ERROR(rs = nanosleep(&ts_req, &ts_rem)))
	{
		if(!IS_WOULDBLOCK_ERROR())
			break;

		ts_req = ts_rem;
	}

	return rs;
}

__time64_t _time64(time_t* ptm)
{
	return (__time64_t)time(ptm);
}

__time64_t _mkgmtime64(tm* ptm)
{
	return (__time64_t)timegm(ptm);
}

tm* _gmtime64(tm* ptm, __time64_t* pt)
{
	time_t t = (time_t)(*pt);

	return gmtime_r(&t, ptm);
}

DWORD TimeGetTime()
{
	return (DWORD)TimeGetTime64();
}

ULLONG TimeGetTime64()
{
	timeb tb;

	if(ftime(&tb) == NO_ERROR)
		return (((ULLONG)(tb.time)) * 1000 + tb.millitm);

	/*
	timespec ts;

	if(clock_gettime(CLOCK_MONOTONIC, &ts) == NO_ERROR)
		return (((ULLONG)(ts.tv_sec)) * 1000 + ts.tv_nsec / 1000000);
	*/

	return 0ull;
}

DWORD GetTimeGap32(DWORD dwOriginal)
{
	return TimeGetTime() - dwOriginal;
}

ULLONG GetTimeGap64(ULLONG ullOriginal)
{
	return TimeGetTime64() - ullOriginal;
}

LLONG TimevalToMillisecond(const timeval& tv)
{
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

timeval& MillisecondToTimeval(LLONG ms, timeval& tv)
{
	tv.tv_sec	= (time_t)(ms / 1000);
	tv.tv_usec	= (suseconds_t)((ms % 1000) * 1000);

	return tv;
}

LLONG TimespecToMillisecond(const timespec& ts)
{
	return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

timespec& MillisecondToTimespec(LLONG ms, timespec& ts)
{
	ts.tv_sec	= (time_t)(ms / 1000);
	ts.tv_nsec	= (long)((ms % 1000) * 1000000);

	return ts;
}

timeval& GetFutureTimeval(LLONG ms, timeval& tv, __timezone_ptr_t ptz)
{
	gettimeofday(&tv, ptz);

	tv.tv_sec	+= (time_t)(ms / 1000);
	tv.tv_usec	+= (suseconds_t)((ms % 1000) * 1000);

	return tv;
}

timespec& GetFutureTimespec(LLONG ms, timespec& ts, clockid_t clkid)
{
	clock_gettime(clkid, &ts);

	ts.tv_sec	+= (time_t)(ms / 1000);
	ts.tv_nsec	+= (long)((ms % 1000) * 1000000);

	return ts;
}

BOOL fcntl_SETFL(FD fd, INT fl, BOOL bSet)
{
	int val = fcntl(fd, F_GETFL);

	if(IS_HAS_ERROR(val))
		return FALSE;

	val = bSet ? (val | fl) : (val & (~fl));

	return IS_NO_ERROR(fcntl(fd, F_SETFL , val));
}

void __EXIT_FN_(void (*fn)(int), LPCSTR lpszFnName, int* lpiExitCode, int iErrno, LPCSTR lpszFile, int iLine, LPCSTR lpszFunc, LPCSTR lpszTitle)
{
	if(iErrno >= 0)
		SetLastError(iErrno);
	else
		iErrno = GetLastError();

	if(!lpszTitle)
	{
		lpszTitle = CreateLocalObjects(char, 50);

		if(lpiExitCode)
			sprintf((LPSTR)lpszTitle, "(#%d, %u) > %s(%d) [%d]", SELF_NATIVE_THREAD_ID, SELF_THREAD_ID, lpszFnName, *lpiExitCode, iErrno);
		else
			sprintf((LPSTR)lpszTitle, "(#%d, %u) > %s() [%d]", SELF_NATIVE_THREAD_ID, SELF_THREAD_ID, lpszFnName, iErrno);
	}

	if(lpszFile && iLine > 0)
		FPRINTLN(stderr, "%s : %s\n    -> %s (%d) : %s", lpszTitle, strerror(iErrno), lpszFile, iLine, lpszFunc ? lpszFunc : "");
	else
		FPRINTLN(stderr, "%s : %s", lpszTitle, strerror(iErrno));

	if(lpiExitCode)
		fn(*lpiExitCode);
	else
		((void (*)())fn)();
}

void EXIT(int iExitCode, int iErrno, LPCSTR lpszFile, int iLine, LPCSTR lpszFunc, LPCSTR lpszTitle)
{
	__EXIT_FN_(exit, "exit", &iExitCode, iErrno, lpszFile, iLine, lpszFunc, lpszTitle);
}

void _EXIT(int iExitCode, int iErrno, LPCSTR lpszFile, int iLine, LPCSTR lpszFunc, LPCSTR lpszTitle)
{
	__EXIT_FN_(_exit, "_exit", &iExitCode, iErrno, lpszFile, iLine, lpszFunc, lpszTitle);
}

void ABORT(int iErrno, LPCSTR lpszFile, int iLine, LPCSTR lpszFunc, LPCSTR lpszTitle)
{
	__EXIT_FN_((void (*)(int))abort, "abort", nullptr, iErrno, lpszFile, iLine, lpszFunc, lpszTitle);
}

BOOL CharsetConvert(LPCSTR lpszFromCharset, LPCSTR lpszToCharset, LPCSTR lpszInBuf, int iInBufLen, LPSTR lpszOutBuf, int& iOutBufLen)
{
	ASSERT(lpszInBuf != nullptr);

	SIZE_T nInBufLeft	= iInBufLen;
	SIZE_T nOutBufLeft	= iOutBufLen;
	int iOutBufSize		= iOutBufLen;
	iOutBufLen			= 0;

	if(lpszInBuf == nullptr)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	iconv_t ic = iconv_open(lpszToCharset, lpszFromCharset);

	if(IS_INVALID_PVOID(ic))
		return FALSE;

	SIZE_T rs	= iconv(ic, (LPSTR*)&lpszInBuf, &nInBufLeft, &lpszOutBuf, &nOutBufLeft);
	iOutBufLen	= iOutBufSize - (int)nOutBufLeft;

	EXECUTE_RESTORE_ERROR(iconv_close(ic));

	return !IS_HAS_ERROR(rs);
}

BOOL GbkToUnicode(const char szSrc[], WCHAR szDest[], int& iDestLength)
{
	int iInBufLen	= (szSrc != nullptr) ? (int)strlen(szSrc) + 1 : 0;
	int iOutBufLen	= (int)(iDestLength * sizeof(wchar_t));

	BOOL isOK	= CharsetConvert(CHARSET_GBK, CHARSET_UTF_32LE, szSrc, iInBufLen, (char*)szDest, iOutBufLen);
	iDestLength	= (int)(iOutBufLen / sizeof(wchar_t));

	return isOK;
}

BOOL UnicodeToGbk(const WCHAR szSrc[], char szDest[], int& iDestLength)
{
	int iInBufLen = (szSrc != nullptr) ? (int)((wcslen(szSrc) + 1) * sizeof(wchar_t)) : 0;

	return CharsetConvert(CHARSET_UTF_32LE, CHARSET_GBK, (LPCSTR)szSrc, iInBufLen, szDest, iDestLength);
}

BOOL Utf8ToUnicode(const char szSrc[], WCHAR szDest[], int& iDestLength)
{
	int iInBufLen	= (szSrc != nullptr) ? (int)strlen(szSrc) + 1 : 0;
	int iOutBufLen	= (int)(iDestLength * sizeof(wchar_t));

	BOOL isOK	= CharsetConvert(CHARSET_UTF_8, CHARSET_UTF_32LE, szSrc, iInBufLen, (char*)szDest, iOutBufLen);
	iDestLength	= (int)(iOutBufLen / sizeof(wchar_t));

	return isOK;
}

BOOL UnicodeToUtf8(const WCHAR szSrc[], char szDest[], int& iDestLength)
{
	int iInBufLen = (szSrc != nullptr) ? (int)((wcslen(szSrc) + 1) * sizeof(wchar_t)) : 0;

	return CharsetConvert(CHARSET_UTF_32LE, CHARSET_UTF_8, (LPCSTR)szSrc, iInBufLen, szDest, iDestLength);
}

BOOL GbkToUtf8(const char szSrc[], char szDest[], int& iDestLength)
{
	int iInBufLen = (szSrc != nullptr) ? (int)strlen(szSrc) + 1 : 0;

	return CharsetConvert(CHARSET_GBK, CHARSET_UTF_8, szSrc, iInBufLen, szDest, iDestLength);
}

BOOL Utf8ToGbk(const char szSrc[], char szDest[], int& iDestLength)
{
	int iInBufLen = (szSrc != nullptr) ? (int)strlen(szSrc) + 1 : 0;

	return CharsetConvert(CHARSET_UTF_8, CHARSET_GBK, szSrc, iInBufLen, szDest, iDestLength);
}

DWORD GuessBase64EncodeBound(DWORD dwSrcLen)
{
	return 4 * ((dwSrcLen + 2) / 3);
}

DWORD GuessBase64DecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen)
{
	if(dwSrcLen < 2)
		return 0;

	if(lpszSrc[dwSrcLen - 2] == '=')
		dwSrcLen -= 2;
	else if(lpszSrc[dwSrcLen - 1] == '=')
			--dwSrcLen;

	DWORD dwMod = dwSrcLen % 4;
	DWORD dwAdd = dwMod == 2 ? 1 : (dwMod == 3 ? 2 : 0);

	return 3 * (dwSrcLen / 4) + dwAdd;
}

int Base64Encode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	static const BYTE CODES[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	DWORD dwRealLen = GuessBase64EncodeBound(dwSrcLen);

	if(lpszDest == nullptr || dwDestLen < dwRealLen)
	{
		dwDestLen = dwRealLen;
		return -5;
	}

	BYTE* p		= lpszDest;
	DWORD leven	= 3 * (dwSrcLen / 3);
	DWORD i		= 0;

	for (; i < leven; i += 3)
	{
		*p++ = CODES[lpszSrc[0] >> 2];
		*p++ = CODES[((lpszSrc[0] & 3) << 4) + (lpszSrc[1] >> 4)];
		*p++ = CODES[((lpszSrc[1] & 0xf) << 2) + (lpszSrc[2] >> 6)];
		*p++ = CODES[lpszSrc[2] & 0x3f];

		lpszSrc += 3;
	}

	if(i < dwSrcLen)
	{
		BYTE a = lpszSrc[0];
		BYTE b = (i + 1 < dwSrcLen) ? lpszSrc[1] : 0;

		*p++ = CODES[a >> 2];
		*p++ = CODES[((a & 3) << 4) + (b >> 4)];
		*p++ = (i + 1 < dwSrcLen) ? CODES[((b & 0xf) << 2)] : '=';
		*p++ = '=';
	}  

	ASSERT(dwRealLen == (DWORD)(p - lpszDest));

	if(dwDestLen > dwRealLen)
	{
		*p			= 0;
		dwDestLen	= dwRealLen;
	}

	return 0;  
}

int Base64Decode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	static const BYTE MAP[256]	=
	{ 
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 253, 255,
		255, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 253, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
		 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
		255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,
		  7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,
		 19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
		255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
		 37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
		 49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255
	};

	DWORD dwRealLen = GuessBase64DecodeBound(lpszSrc, dwSrcLen);

	if(lpszDest == nullptr || dwDestLen < dwRealLen)
	{
		dwDestLen = dwRealLen;
		return -5;
	}

	BYTE c;
	int g = 3;
	DWORD i, x, y, z;

	for(i = x = y = z = 0; i < dwSrcLen || x != 0;)
	{
		c = i < dwSrcLen ? MAP[lpszSrc[i++]] : 254;

		if(c == 255) {dwDestLen = 0; return -3;}
		else if(c == 254) {c = 0; g--;}
		else if(c == 253) continue;

		z = (z << 6) | c;

		if(++x == 4)
		{
			lpszDest[y++] = (BYTE)((z >> 16) & 255);
			if (g > 1) lpszDest[y++] = (BYTE)((z >> 8) & 255);
			if (g > 2) lpszDest[y++] = (BYTE)(z & 255);

			x = z = 0;
		}
	}

	BOOL isOK = (y == dwRealLen);

	if(!isOK)
		dwDestLen = 0;
	else
	{
		if(dwDestLen > dwRealLen)
		{
			lpszDest[dwRealLen]	= 0;
			dwDestLen			= dwRealLen;
		}
	}

	return isOK ? 0 : -3;
}

DWORD GuessUrlEncodeBound(const BYTE* lpszSrc, DWORD dwSrcLen)
{
	DWORD dwAdd = 0;

	for(DWORD i = 0; i < dwSrcLen; i++)
	{
		BYTE c	= lpszSrc[i];

		if(!(isalnum(c) || c == ' ' || c == '.' || c == '-' || c == '_' || c == '*'))
			dwAdd += 2;
	}

	return dwSrcLen + dwAdd;
}

DWORD GuessUrlDecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen)
{
	DWORD dwPercent = 0;

	for(DWORD i = 0; i < dwSrcLen; i++)
	{
		if(lpszSrc[i] == '%')
		{
			++dwPercent;
			i += 2;
		}
	}

	DWORD dwSub = dwPercent * 2;

	if(dwSrcLen < dwSub)
		return 0;

	return dwSrcLen - dwSub;
}

int UrlEncode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	BYTE c;
	DWORD j = 0;

	if(lpszDest == nullptr || dwDestLen == 0)
		goto ERROR_DEST_LEN;

	for(DWORD i = 0; i < dwSrcLen; i++)
	{
		if(j >= dwDestLen)
			goto ERROR_DEST_LEN;

		c = lpszSrc[i];

		if (isalnum(c) || c == '.' || c == '-' || c == '_' || c == '*')
			lpszDest[j++] = c;
		else if(c == ' ')
			lpszDest[j++] = '+';
		else
		{
			if(j + 3 >= dwDestLen)
				goto ERROR_DEST_LEN;

			lpszDest[j++] = '%';
			HEX_VALUE_TO_DOUBLE_CHAR(lpszDest + j, c);
			j += 2;
			
		}
	}

	if(dwDestLen > j)
	{
		lpszDest[j]	= 0;
		dwDestLen	= j;
	}

	return 0;

ERROR_DEST_LEN:
	dwDestLen = GuessUrlEncodeBound(lpszSrc, dwSrcLen);
	return -5;
}

int UrlDecode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	char c;
	DWORD j = 0;

	if(lpszDest == nullptr || dwDestLen == 0)
		goto ERROR_DEST_LEN;

	for(DWORD i = 0; i < dwSrcLen; i++)
	{
		if(j >= dwDestLen)
			goto ERROR_DEST_LEN;

		c = lpszSrc[i];

		if(c == '+')
			lpszDest[j++] = ' ';
		else if(c != '%')
			lpszDest[j++] = c;
		else
		{
			if(i + 2 >= dwSrcLen)
				goto ERROR_SRC_DATA;

			lpszDest[j++] = HEX_DOUBLE_CHAR_TO_VALUE(lpszSrc + i + 1);
			i += 2;
		}
	}

	if(dwDestLen > j)
	{
		lpszDest[j]	= 0;
		dwDestLen	= j;
	}

	return 0;

ERROR_SRC_DATA:
	dwDestLen = 0;
	return -3;

ERROR_DEST_LEN:
	dwDestLen = GuessUrlDecodeBound(lpszSrc, dwSrcLen);
	return -5;
}
