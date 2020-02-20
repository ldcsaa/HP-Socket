/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Author	: Bruce Liang
 * Website	: https://github.com/ldcsaa
 * Project	: https://github.com/ldcsaa/HP-Socket/HP-Socket
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

#include <malloc.h>

class CEvt
{
public:
	CEvt(BOOL bManualReset = FALSE, BOOL bInitialState = FALSE, LPCTSTR lpszName = nullptr, LPSECURITY_ATTRIBUTES pSecurity = nullptr)
	{
		m_hEvent = ::CreateEvent(pSecurity, bManualReset, bInitialState, lpszName);
		ENSURE(IsValid());
	}

	~CEvt()
	{
		if(IsValid())
			ENSURE(::CloseHandle(m_hEvent));
	}

	BOOL Open(DWORD dwAccess, BOOL bInheritHandle, LPCTSTR lpszName)
	{
		if(IsValid())
			ENSURE(::CloseHandle(m_hEvent));

		m_hEvent = ::OpenEvent(dwAccess, bInheritHandle, lpszName);
		return(IsValid());
	}

	BOOL Wait(DWORD dwMilliseconds = INFINITE)
	{
		DWORD rs = ::WaitForSingleObject(m_hEvent, dwMilliseconds);

		if(rs == WAIT_TIMEOUT) ::SetLastError(WAIT_TIMEOUT);

		return (rs == WAIT_OBJECT_0);
	}

	BOOL Pulse()	{return(::PulseEvent(m_hEvent));}
	BOOL Reset()	{return(::ResetEvent(m_hEvent));}
	BOOL Set()		{return(::SetEvent(m_hEvent));}

	BOOL IsValid()	{return m_hEvent != nullptr;}

	HANDLE GetHandle		()			{return m_hEvent;}
	const HANDLE GetHandle	()	const	{return m_hEvent;}

	operator HANDLE			()			{return m_hEvent;}
	operator const HANDLE	()	const	{return m_hEvent;}

private:
	CEvt(const CEvt&);
	CEvt operator = (const CEvt&);

private:
	HANDLE m_hEvent;
};

class CTimerEvt
{
public:
	CTimerEvt(BOOL bManualReset = FALSE, LPCTSTR lpszName = nullptr, LPSECURITY_ATTRIBUTES pSecurity = nullptr)
	{
		m_hTimer = ::CreateWaitableTimer(pSecurity, bManualReset, lpszName);
		ENSURE(IsValid());
	}

	~CTimerEvt()
	{
		if(IsValid())
			ENSURE(::CloseHandle(m_hTimer));
	}

	BOOL Open(DWORD dwAccess, BOOL bInheritHandle, LPCTSTR lpszName)
	{
		if(IsValid())
			ENSURE(::CloseHandle(m_hTimer));

		m_hTimer = ::OpenWaitableTimer(dwAccess, bInheritHandle, lpszName);
		return(IsValid());
	}

	BOOL Set(LONG lPeriod, LARGE_INTEGER* lpDueTime = nullptr, BOOL bResume = FALSE, PTIMERAPCROUTINE pfnAPC = nullptr, LPVOID lpArg = nullptr)
	{
		if(lpDueTime == nullptr)
		{
			lpDueTime = (LARGE_INTEGER*)alloca(sizeof(LARGE_INTEGER));
			lpDueTime->QuadPart = -(lPeriod * 10000LL);
		}

		return ::SetWaitableTimer(m_hTimer, lpDueTime, lPeriod, pfnAPC, lpArg, bResume);
	}


	BOOL Reset()	{return(::CancelWaitableTimer(m_hTimer));}
	BOOL IsValid()	{return m_hTimer != nullptr;}

	HANDLE GetHandle		()			{return m_hTimer;}
	const HANDLE GetHandle	()	const	{return m_hTimer;}

	operator HANDLE			()			{return m_hTimer;}
	operator const HANDLE	()	const	{return m_hTimer;}

private:
	CTimerEvt(const CTimerEvt&);
	CTimerEvt operator = (const CTimerEvt&);

private:
	HANDLE m_hTimer;
};

class CTimerQueue
{
public:
	CTimerQueue()
	{
		Create();
	}

	~CTimerQueue()
	{
		Delete();
	}

	HANDLE CreateTimer(WAITORTIMERCALLBACK fnCallback, PVOID lpParam, DWORD dwPeriod, DWORD dwDueTime = INFINITE, ULONG ulFlags = WT_EXECUTEDEFAULT)
	{
		HANDLE hTimer = nullptr;

		if(dwDueTime == INFINITE)
			dwDueTime = dwPeriod;

		::CreateTimerQueueTimer(&hTimer, m_hTimerQueue, fnCallback, lpParam, dwDueTime, dwPeriod, ulFlags);

		return hTimer;
	}

	BOOL ChangeTimer(HANDLE hTimer, DWORD dwPeriod, DWORD dwDueTime = INFINITE)
	{
		if(dwDueTime == INFINITE)
			dwDueTime = dwPeriod;

		return ::ChangeTimerQueueTimer(m_hTimerQueue, hTimer, dwDueTime, dwPeriod);
	}

	BOOL DeleteTimer(HANDLE hTimer, HANDLE hCompletionEvent = INVALID_HANDLE_VALUE)
	{
		return ::DeleteTimerQueueTimer(m_hTimerQueue, hTimer, hCompletionEvent);
	}

	BOOL Reset()
	{
		Delete();
		Create();

		return IsValid();
	}

	BOOL IsValid()	{return m_hTimerQueue != nullptr;}

	HANDLE GetHandle		()			{return m_hTimerQueue;}
	const HANDLE GetHandle	()	const	{return m_hTimerQueue;}

	operator HANDLE			()			{return m_hTimerQueue;}
	operator const HANDLE	()	const	{return m_hTimerQueue;}

private:
	void Create()
	{
		m_hTimerQueue = ::CreateTimerQueue();
		ENSURE(IsValid());
	}

	void Delete()
	{
		if(IsValid())
		{
			ENSURE(::DeleteTimerQueueEx(m_hTimerQueue, INVALID_HANDLE_VALUE));
			m_hTimerQueue = nullptr;
		}
	}

private:
	CTimerQueue(const CTimerQueue&);
	CTimerQueue operator = (const CTimerQueue&);

private:
	HANDLE m_hTimerQueue;
};
