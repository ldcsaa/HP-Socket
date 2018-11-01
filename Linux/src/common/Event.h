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
#include "Singleton.h"
#include "FuncHelper.h"
#include "PollHelper.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <sys/signalfd.h>

class CPipeEvent
{
public:

	enum
	{
		EVT_1		= 0x01,
		EVT_WAKEUP	= EVT_1,
		EVT_EXIT	= 0x7F,
		EVT_SIG_0	= 0x80,
		EVT_SIG_MAX	= EVT_SIG_0 + _NSIG,
	};

public:

	int Wait(long lTimeout = INFINITE, const sigset_t* pSigSet = nullptr)
	{
		pollfd pfd = {m_fd[0], POLLIN};

		while(TRUE)
		{
			int rs = (int)::PollForSingleObject(pfd, lTimeout, pSigSet);

			if(rs <= TIMEOUT) return rs;

			if(pfd.revents & POLLIN)
			{
				BYTE v;

				if(!Get(v))
					return HAS_ERROR;

				if(v == 0)
					continue;

				return (int)v;
			}

			if(pfd.revents & _POLL_ALL_ERROR_EVENTS)
			{
				::SetLastError(ERROR_BROKEN_PIPE);
				return HAS_ERROR;
			}

			ASSERT(FALSE);
		}
	}

	BOOL Set(BYTE bVal = EVT_WAKEUP)
	{
		ASSERT_CHECK_EINVAL(bVal != 0);

		return VERIFY(write(m_fd[1], &bVal, 1) > 0);
	}

	BOOL Get(BYTE& v)
	{
		ASSERT(IsValid());

		int rs = (int)read(m_fd[0], &v, 1);

		if(IS_HAS_ERROR(rs))
		{
			if(IS_WOULDBLOCK_ERROR())
				v = 0;
			else
				return FALSE;
		}
		else if(rs == 0)
		{
			::SetLastError(ERROR_BROKEN_PIPE);
			return FALSE;
		}

		return TRUE;
	}

	BOOL Reset()
	{
		BYTE v;

		while(TRUE)
		{
			if(!Get(v))
				return FALSE;

			if(v == 0)
				break;
		}

		return TRUE;
	}

	BOOL SetSignal(BYTE bSigVal)
	{
		ASSERT_CHECK_EINVAL(bSigVal > 0 && bSigVal < _NSIG);

		return Set((BYTE)(EVT_SIG_0 + bSigVal));
	}

	static inline BYTE ToSignalValue(int iWaitResult)
	{
		if(iWaitResult <= EVT_SIG_0 || iWaitResult >= EVT_SIG_MAX)
			return 0;

		return (BYTE)(iWaitResult - EVT_SIG_0);
	}

	BOOL IsValid()	{return IS_VALID_FD(m_fd[0]) && IS_VALID_FD(m_fd[1]);}

	operator FD	()	{return m_fd[0];}
	FD GetFD	()	{return m_fd[0];}

public:
	CPipeEvent()
	{
		VERIFY_IS_NO_ERROR(pipe2(m_fd, O_NONBLOCK | O_CLOEXEC));
		VERIFY(::fcntl_SETFL(m_fd[0], O_NOATIME));
		VERIFY(::fcntl_SETFL(m_fd[1], O_NOATIME));
	}

	~CPipeEvent()
	{
		close(m_fd[1]);
		close(m_fd[0]);
	}

	DECLARE_NO_COPY_CLASS(CPipeEvent)

private:
	FD m_fd[2] = {INVALID_FD, INVALID_FD};
};

template<bool is_sem_mode = false> class CCounterEvent
{
public:

	eventfd_t Wait(long lTimeout = INFINITE, const sigset_t* pSigSet = nullptr)
	{
		pollfd pfd = {m_evt, POLLIN};

		while(TRUE)
		{
			long rs = ::PollForSingleObject(pfd, lTimeout, pSigSet);

			if(rs <= TIMEOUT) return (eventfd_t)rs;

			if(pfd.revents & POLLIN)
			{
				eventfd_t v;

				if(!Get(v))
					return HAS_ERROR;

				if(v == 0)
					continue;

				return v;
			}

			if(pfd.revents & _POLL_ALL_ERROR_EVENTS)
			{
				::SetLastError(ERROR_HANDLES_CLOSED);
				return HAS_ERROR;
			}

			ASSERT(FALSE);
		}
	}

	BOOL Set(eventfd_t val = 1)
	{
		ASSERT_CHECK_EINVAL(val > 0);

		int rs = eventfd_write(m_evt, val);
		return VERIFY_IS_NO_ERROR(rs);
	}

	BOOL Get(eventfd_t& v)
	{
		ASSERT(IsValid());

		if(IS_HAS_ERROR(eventfd_read(m_evt, &v)))
		{
			if(IS_WOULDBLOCK_ERROR())
				v = 0;
			else
				return FALSE;
		}
		
		return TRUE;
	}

	BOOL Reset()
	{
		eventfd_t v;

		while(TRUE)
		{
			if(!Get(v))
				return FALSE;

			if(v == 0)
				break;
		}

		return TRUE;
	}

	BOOL IsValid()	{return IS_VALID_FD(m_evt);}

	operator FD	()	{return m_evt;}
	FD GetFD	()	{return m_evt;}

public:
	CCounterEvent(int iInitCount = 0)
	{ 
		int iFlag	= EFD_NONBLOCK | EFD_CLOEXEC | (is_sem_mode ? EFD_SEMAPHORE : 0);
		m_evt		= eventfd(iInitCount, iFlag);

		VERIFY(IsValid());
	}

	~CCounterEvent()
	{
		if(IsValid()) close(m_evt);
	}

	DECLARE_NO_COPY_CLASS(CCounterEvent)

private:
	FD m_evt = INVALID_FD;
};

using CSimpleEvent		= CCounterEvent<false>;
using CSemaphoreEvent	= CCounterEvent<true>;
using CEvt				= CSimpleEvent;

class CTimerEvent
{
public:

	ULLONG Wait(long lTimeout = INFINITE, const sigset_t* pSigSet = nullptr)
	{
		pollfd pfd = {m_tmr, POLLIN};

		while(TRUE)
		{
			SSIZE_T rs = ::PollForSingleObject(pfd, lTimeout, pSigSet);

			if(rs <= TIMEOUT) return (ULLONG)rs;

			if(pfd.revents & POLLIN)
			{
				BOOL	ok;
				ULLONG	v;

				if(!Get(v, ok))
					return HAS_ERROR;

				if(!ok)
					continue;

				return v;
			}

			if(pfd.revents & _POLL_ALL_ERROR_EVENTS)
			{
				::SetLastError(ERROR_HANDLES_CLOSED);
				return HAS_ERROR;
			}

			ASSERT(FALSE);
		}
	}

	BOOL Set(LLONG lStart, LLONG lInterval)
	{
		ASSERT_CHECK_EINVAL(lStart >= 0L && lInterval >= 0L);

		itimerspec its;

		::MillisecondToTimespec(lStart, its.it_value);
		::MillisecondToTimespec(lInterval, its.it_interval);

		int rs = timerfd_settime(m_tmr, 0, &its, nullptr);
		return VERIFY_IS_NO_ERROR(rs);
	}

	BOOL Get(ULLONG &v, BOOL& ok)
	{
		ASSERT(IsValid());

		static const SSIZE_T SIZE = sizeof(ULLONG);

		if(read(m_tmr, &v, SIZE) == SIZE)
			ok = TRUE;
		else
		{
			if(IS_WOULDBLOCK_ERROR())
				ok = FALSE;
			else
				return FALSE;
		}

		return ok;
	}

	BOOL Reset()
	{
		BOOL	ok;
		ULLONG	v;

		while(TRUE)
		{
			if(!Get(v, ok))
				return FALSE;

			if(!ok)
				break;
		}

		return TRUE;
	}

	BOOL GetTime(LLONG& lStart, LLONG& lInterval)
	{
		itimerspec its;

		if(IS_HAS_ERROR(timerfd_gettime(m_tmr, &its)))
			return FALSE;

		lStart		= ::TimespecToMillisecond(its.it_value);
		lInterval	= ::TimespecToMillisecond(its.it_interval);

		return TRUE;
	}

	BOOL IsValid()	{return IS_VALID_FD(m_tmr);}

	operator FD	()	{return m_tmr;}
	FD GetFD	()	{return m_tmr;}

public:
	CTimerEvent(bool bRealTimeClock =  FALSE)
	{ 
		int iCID = (bRealTimeClock ? CLOCK_REALTIME : CLOCK_MONOTONIC);
		m_tmr	 = timerfd_create(iCID, TFD_NONBLOCK | TFD_CLOEXEC);

		VERIFY(IsValid());
	}

	~CTimerEvent()
	{
		if(IsValid()) close(m_tmr);
	}

	DECLARE_NO_COPY_CLASS(CTimerEvent)

private:
	FD m_tmr = INVALID_FD;
};

class CSignalEvent
{
public:

	int Wait(signalfd_siginfo& sgInfo, long lTimeout = INFINITE, const sigset_t* pSigSet = nullptr)
	{
		m_dwTID		= SELF_THREAD_ID;
		pollfd pfd	= {m_sig, POLLIN};

		while(TRUE)
		{
			long rs = ::PollForSingleObject(pfd, lTimeout, pSigSet);

			if(rs <= TIMEOUT) return (int)rs;

			if(pfd.revents & POLLIN)
			{
				BOOL ok;

				if(!Get(sgInfo, ok))
					return HAS_ERROR;

				if(!ok)
					continue;

				return sgInfo.ssi_signo;
			}

			if(pfd.revents & _POLL_ALL_ERROR_EVENTS)
			{
				::SetLastError(ERROR_HANDLES_CLOSED);
				return HAS_ERROR;
			}

			ASSERT(FALSE);
		}

		m_dwTID = 0;
	}

	BOOL Set(int iSig, const sigval sgVal, THR_ID dwTID = 0)
	{
		if(dwTID == 0)
		{
			dwTID = m_dwTID;

			if(dwTID == 0)
			{
				::SetLastError(ERROR_INVALID_STATE);
				return FALSE;
			}
		}

#if !defined(__ANDROID__)
		int rs = pthread_sigqueue(dwTID, iSig, sgVal);
#else
		int rs = pthread_kill(dwTID, iSig);
#endif

		return IS_NO_ERROR(rs);
	}

	BOOL Get(signalfd_siginfo& v, BOOL& ok)
	{
		ASSERT(IsValid());

		static const long SIZE = sizeof(signalfd_siginfo);

		if(read(m_sig, &v, SIZE) == SIZE)
			ok = TRUE;
		{
			if(IS_WOULDBLOCK_ERROR())
				ok = FALSE;
			else
				return FALSE;
		}

		return ok;
	}

	BOOL Reset()
	{
		BOOL			 ok;
		signalfd_siginfo v;

		while(TRUE)
		{
			if(!Get(v, ok))
				return FALSE;

			if(!ok)
				break;
		}

		return TRUE;
	}

	BOOL Mask(const sigset_t* pSigMask)
	{
		if(!pSigMask)
		{
			if(!IsValid()) return TRUE;
			return IS_NO_ERROR(close(m_sig));
		}

		FD sig = signalfd(m_sig, pSigMask, SFD_NONBLOCK | SFD_CLOEXEC);

		if(IS_VALID_FD(sig))
		{
			m_sig = sig;
			return TRUE;
		}

		return FALSE;
	}

	BOOL IsValid()	{return IS_VALID_FD(m_sig);}

	operator FD	()	{return m_sig;}
	FD GetFD	()	{return m_sig;}

public:
	CSignalEvent(const sigset_t* pSigMask = nullptr)
	{
		if(pSigMask) VERIFY(Mask(pSigMask));
	}

	~CSignalEvent()
	{
		if(IsValid()) close(m_sig);
	}

	DECLARE_NO_COPY_CLASS(CSignalEvent)

private:
	FD m_sig		= INVALID_FD;
	THR_ID m_dwTID	= 0;
};
