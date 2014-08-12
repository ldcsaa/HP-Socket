/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.7
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
 
#include "stdafx.h"
#include "WaitFor.h"
#include "GeneralHelper.h"
#include "CriticalSection.h"

// Ö§³Ö timeGetTime() º¯Êý
#ifdef _WIN32_WCE
	#include <Mmsystem.h>
	#pragma comment(lib, "Mmtimer")
#else
	#include <MmSystem.h>
	#pragma comment(lib, "Winmm")
#endif

DWORD TimeGetTime()
{
	return ::timeGetTime();
}

DWORD GetTimeGap32(DWORD dwOriginal)
{
	return ::timeGetTime() - dwOriginal;
}

#if _WIN32_WINNT >= _WIN32_WINNT_WS08
ULONGLONG GetTimeGap64(ULONGLONG ullOriginal)
{
	return ::GetTickCount64() - ullOriginal;
}
#endif

BOOL PeekMessageLoop(BOOL bDispatchQuitMsg)
{
	BOOL value = TRUE;

	MSG msg;
	while(::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if(msg.message == WM_QUIT && !bDispatchQuitMsg)
		{
			value = FALSE;
			break;
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return value;
}

DWORD WaitForMultipleObjectsWithMessageLoop(DWORD dwHandles, HANDLE szHandles[], DWORD dwMilliseconds, DWORD dwWakeMask, DWORD dwFlags)
{
	DWORD dwResult		= WAIT_FAILED;
	DWORD dwBeginTime	= (dwMilliseconds == INFINITE) ? INFINITE : ::timeGetTime();

	while(TRUE)
	{
		int iWaitTime;
		if(dwBeginTime != INFINITE)
		{
			iWaitTime	= dwMilliseconds - (GetTimeGap32(dwBeginTime));

			if(iWaitTime <= 0)
			{
				dwResult = WAIT_TIMEOUT;
				break;
			}
		}
		else
			iWaitTime	= INFINITE;

		dwResult = ::MsgWaitForMultipleObjectsEx(dwHandles, szHandles, iWaitTime, dwWakeMask, dwFlags);
		ASSERT(dwResult != WAIT_FAILED);

		if(dwResult == (WAIT_OBJECT_0 + dwHandles))
			PeekMessageLoop();
		else
			break;
	}

	return dwResult;

}

BOOL MsgWaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds, DWORD dwWakeMask, DWORD dwFlags)
{
	DWORD dwResult = WaitForMultipleObjectsWithMessageLoop(1, &hHandle, dwMilliseconds, dwWakeMask, dwFlags);

	switch(dwResult)
	{
	case WAIT_OBJECT_0:
		return TRUE;
	case WAIT_FAILED:
		ASSERT(FALSE);
	case WAIT_TIMEOUT:
		return FALSE;
	default:
		ASSERT(FALSE);
	}

	return FALSE;
}

void WaitWithMessageLoop(DWORD dwMilliseconds, DWORD dwWakeMask, DWORD dwFlags)
{
	static CEvt evWait;

	VERIFY(MsgWaitForSingleObject(evWait, dwMilliseconds, dwWakeMask, dwFlags) == FALSE);
}

void WaitForWorkingQueue(long* plWorkingItemCount, long lMaxWorkingItemCount, DWORD dwCheckInterval)
{
	while(*plWorkingItemCount > lMaxWorkingItemCount)
		::Sleep(dwCheckInterval);
}

void WaitForComplete(long* plWorkingItemCount, DWORD dwCheckInterval)
{
	WaitForWorkingQueue(plWorkingItemCount, 0, dwCheckInterval);
}

void MsgWaitForWorkingQueue(long* plWorkingItemCount, long lMaxWorkingItemCount, DWORD dwCheckInterval)
{
	while(*plWorkingItemCount > lMaxWorkingItemCount)
		WaitWithMessageLoop(dwCheckInterval);
}

void MsgWaitForComplete(long* plWorkingItemCount, DWORD dwCheckInterval)
{
	MsgWaitForWorkingQueue(plWorkingItemCount, 0, dwCheckInterval);
}
