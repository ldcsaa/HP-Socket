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

#include <sched.h>
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

void __EXIT_FN_(void (*fn)(int), LPCTSTR lpszFnName, int* lpiExitCode, int iErrno, LPCSTR lpszFile, int iLine, LPCSTR lpszFunc, LPCSTR lpszTitle)
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
