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

#include "IODispatcher.h"
#include "FuncHelper.h"

#include <signal.h>
#include <pthread.h>

BOOL CIODispatcher::Start(IIOHandler* pHandler, int iWorkerMaxEvents, int iWorkers, LLONG llTimerInterval)
{
	ASSERT_CHECK_EINVAL(pHandler && iWorkerMaxEvents >= 0 && iWorkers >= 0);
	CHECK_ERROR(!HasStarted(), ERROR_INVALID_STATE);

	if(iWorkerMaxEvents == 0)	iWorkerMaxEvents = DEF_WORKER_MAX_EVENTS;
	if(iWorkers == 0)			iWorkers = DEFAULT_WORKER_THREAD_COUNT;

	m_iMaxEvents = iWorkerMaxEvents;
	m_iWorkers	 = iWorkers;
	m_pHandler	 = pHandler;

	m_epoll = epoll_create1(EPOLL_CLOEXEC);
	CHECK_ERROR_FD(m_epoll);

	m_evCmd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);

	if(IS_INVALID_FD(m_evCmd))
		goto START_ERROR;

	if(!VERIFY(AddFD(m_evCmd, EPOLLIN | EPOLLET, &m_evCmd)))
		goto START_ERROR;

	m_evExit = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC | EFD_SEMAPHORE);

	if(IS_INVALID_FD(m_evExit))
		goto START_ERROR;

	if(!VERIFY(AddFD(m_evExit, EPOLLIN, &m_evExit)))
		goto START_ERROR;

	if(llTimerInterval > 0)
	{
		m_evTimer = AddTimer(llTimerInterval, &m_evTimer);

		if(IS_INVALID_FD(m_evTimer))
			goto START_ERROR;
	}

	sigset_t ss;
	sigemptyset(&ss);
	sigaddset(&ss, SIGPIPE);

	VERIFY_IS_NO_ERROR(pthread_sigmask(SIG_BLOCK, &ss, nullptr));

	m_pWorkers = make_unique<CWorkerThread[]>(m_iWorkers);

	for(int i = 0; i < m_iWorkers; i++)
	{
		if(!VERIFY(m_pWorkers[i].Start(this, &CIODispatcher::WorkerProc)))
			goto START_ERROR;
	}

	return TRUE;

START_ERROR:
	EXECUTE_RESTORE_ERROR(Stop(FALSE));
	return FALSE;
}

BOOL CIODispatcher::Stop(BOOL bCheck)
{
	if(bCheck) CHECK_ERROR(HasStarted(), ERROR_INVALID_STATE);

	BOOL isOK = TRUE;

	if(m_pWorkers)
	{
		isOK &= IS_NO_ERROR(eventfd_write(m_evExit, m_iWorkers));

		for(int i = 0; i < m_iWorkers; i++)
			isOK &= m_pWorkers[i].Join();
	}

	if(!m_queue.IsEmpty())
	{
		TDispCommand* pCmd = nullptr;

		while(m_queue.PopFront(&pCmd))
			TDispCommand::Destruct(pCmd);

		VERIFY(m_queue.IsEmpty());
	}

	if(IS_VALID_FD(m_evExit))
		isOK &= IS_NO_ERROR(close(m_evExit));

	if(IS_VALID_FD(m_evCmd))
		isOK &= IS_NO_ERROR(close(m_evCmd));

	if(IS_VALID_FD(m_evTimer))
		isOK &= IS_NO_ERROR(close(m_evTimer));

	if(IS_VALID_FD(m_epoll))
		isOK &= IS_NO_ERROR(close(m_epoll));

	Reset();

	return isOK;
}

VOID CIODispatcher::Reset()
{
	m_iWorkers	= 0;
	m_iMaxEvents= 0;
	m_pHandler	= nullptr;
	m_pWorkers	= nullptr;
	m_epoll		= INVALID_FD;
	m_evCmd		= INVALID_FD;
	m_evExit	= INVALID_FD;
	m_evTimer	= INVALID_FD;
}

BOOL CIODispatcher::SendCommand(USHORT t, UINT_PTR wp, UINT_PTR lp)
{
	return SendCommand(TDispCommand::Construct(t, wp, lp));
}

BOOL CIODispatcher::SendCommand(TDispCommand* pCmd)
{
	m_queue.PushBack(pCmd);
	return VERIFY_IS_NO_ERROR(eventfd_write(m_evCmd, 1));
}

BOOL CIODispatcher::CtlFD(FD fd, int op, UINT mask, PVOID pv)
{
	epoll_event evt = {mask, pv};
	return IS_NO_ERROR(epoll_ctl(m_epoll, op, fd, &evt));
}

int CIODispatcher::WorkerProc(PVOID pv)
{
	m_pHandler->OnDispatchThreadStart(SELF_THREAD_ID);

	BOOL bRun							= TRUE;
	unique_ptr<epoll_event[]> pEvents	= make_unique<epoll_event[]>(m_iMaxEvents);

	while(bRun)
	{
		int rs = NO_EINTR_INT(epoll_pwait(m_epoll, pEvents.get(), m_iMaxEvents, INFINITE, nullptr));

		if(rs <= TIMEOUT)
			ERROR_ABORT();

		for(int i = 0; i < rs; i++)
		{
			UINT events	= pEvents[i].events;
			PVOID ptr	= pEvents[i].data.ptr;

			if(ptr == &m_evCmd)
				ProcessCommand(events);
			else if(ptr == &m_evTimer)
				ProcessTimer(events);
			else if(ptr == &m_evExit)
				bRun = ProcessExit(events);
			else
				ProcessIo(ptr, events);
		}
	}

	m_pHandler->OnDispatchThreadEnd(SELF_THREAD_ID);

	return 0;
}

BOOL CIODispatcher::ProcessCommand(UINT events)
{
	if(events & _EPOLL_ALL_ERROR_EVENTS)
		ERROR_ABORT();

	if(!(events & EPOLLIN))
		return FALSE;

	BOOL isOK = TRUE;

	eventfd_t v;

	int rs = eventfd_read(m_evCmd, &v);

	if(IS_NO_ERROR(rs))
	{
		ASSERT(v > 0);

		TDispCommand* pCmd = nullptr;

		while(m_queue.PopFront(&pCmd))
		{
			m_pHandler->OnCommand(pCmd);
			TDispCommand::Destruct(pCmd);
		}
	}
	else if(IS_HAS_ERROR(rs))
	{
		ASSERT(IS_WOULDBLOCK_ERROR());

		isOK = FALSE;
	}

	return isOK;
}

BOOL CIODispatcher::ProcessTimer(UINT events)
{
	if(events & _EPOLL_ALL_ERROR_EVENTS)
		ERROR_ABORT();

	if(!(events & EPOLLIN))
		return TRUE;

	BOOL isOK = FALSE;
	ULLONG ullExpirations;

	if(::ReadTimer(m_evTimer, &ullExpirations, &isOK) && isOK)
		m_pHandler->OnTimer(ullExpirations);
	else
		ASSERT(IS_WOULDBLOCK_ERROR());

	return isOK;
}

BOOL CIODispatcher::ProcessExit(UINT events)
{
	if(events & _EPOLL_ALL_ERROR_EVENTS)
		ERROR_ABORT();

	if(!(events & EPOLLIN))
		return TRUE;

	BOOL bRun = TRUE;

	eventfd_t v;

	int rs = eventfd_read(m_evExit, &v);

	if(IS_HAS_ERROR(rs))
		ASSERT(IS_WOULDBLOCK_ERROR());
	else
	{
		ASSERT(v == 1);
		bRun = FALSE;
	}

	return bRun;
}

BOOL CIODispatcher::ProcessIo(PVOID pv, UINT events)
{
	if(!m_pHandler->OnBeforeProcessIo(pv, events))
		return FALSE;

	BOOL rs = DoProcessIo(pv, events);
	m_pHandler->OnAfterProcessIo(pv, events, rs);

	return rs;
}

BOOL CIODispatcher::DoProcessIo(PVOID pv, UINT events)
{
	if(events & EPOLLERR)
		return m_pHandler->OnError(pv, events);
	if((events & EPOLLPRI) && !m_pHandler->OnReadyPrivilege(pv, events))
		return FALSE;
	if((events & EPOLLIN) && !m_pHandler->OnReadyRead(pv, events))
		return FALSE;
	if((events & EPOLLOUT) && !m_pHandler->OnReadyWrite(pv, events))
		return FALSE;
	if((events & (_EPOLL_HUNGUP_EVENTS)) && !m_pHandler->OnHungUp(pv, events))
		return FALSE;

	return TRUE;
}

FD CIODispatcher::AddTimer(LLONG llInterval, PVOID pv)
{
	FD fdTimer = ::CreateTimer(llInterval);

	if(IS_VALID_FD(fdTimer))
	{
		if(!AddFD(fdTimer, EPOLLIN | EPOLLET, pv))
		{
			close(fdTimer);
			fdTimer = INVALID_FD;
		}
	}

	return fdTimer;
}

BOOL CIODispatcher::DelTimer(FD fdTimer)
{
	BOOL isOK = FALSE;

	if(IS_VALID_FD(fdTimer))
	{
		if(DelFD(fdTimer))
			isOK = TRUE;

		close(fdTimer);
	}

	return isOK;
}
