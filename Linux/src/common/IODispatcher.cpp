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

volatile UINT CIODispatcher::sm_uiNum		= MAXUINT;
LPCTSTR CIODispatcher::WORKER_THREAD_PREFIX	= _T("io-disp-");

BOOL CIODispatcher::Start(IIOHandler* pHandler, int iWorkerMaxEvents, int iWorkers)
{
	ASSERT_CHECK_EINVAL(pHandler && iWorkerMaxEvents >= 0 && iWorkers >= 0);
	CHECK_ERROR(!HasStarted(), ERROR_INVALID_STATE);

	if(iWorkerMaxEvents == 0)	iWorkerMaxEvents = DEF_WORKER_MAX_EVENTS;
	if(iWorkers == 0)			iWorkers = DEFAULT_WORKER_THREAD_COUNT;

	m_iMaxEvents = iWorkerMaxEvents;
	m_iWorkers	 = iWorkers;
	m_pHandler	 = pHandler;

	m_evExit = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC | EFD_SEMAPHORE);

	if(IS_INVALID_FD(m_evExit))
		goto START_ERROR;

	m_pContexts = make_unique<TDispContext[]>(m_iWorkers);

	for(int i = 0; i < m_iWorkers; i++)
	{
		TDispContext& ctx = m_pContexts[i];

		ctx.m_iIndex = i;

		ctx.m_epoll = epoll_create1(EPOLL_CLOEXEC);
		CHECK_ERROR_FD(ctx.m_epoll);

		ctx.m_evCmd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);

		if(IS_INVALID_FD(ctx.m_evCmd))
			goto START_ERROR;

		if(!VERIFY(AddFD(i, ctx.m_evCmd, EPOLLIN | EPOLLET, &ctx.m_evCmd)))
			goto START_ERROR;

		if(!VERIFY(AddFD(i, m_evExit, EPOLLIN, &m_evExit)))
			goto START_ERROR;

		sigset_t ss;
		sigemptyset(&ss);
		sigaddset(&ss, SIGPIPE);

		VERIFY_IS_NO_ERROR(pthread_sigmask(SIG_BLOCK, &ss, nullptr));

		ctx.m_pWorker = make_unique<CWorkerThread>();

		if(!VERIFY(ctx.m_pWorker->Start(this, &CIODispatcher::WorkerProc, &ctx)))
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

	if(m_pContexts)
	{
		isOK &= IS_NO_ERROR(eventfd_write(m_evExit, m_iWorkers));

		for(int i = 0; i < m_iWorkers; i++)
		{
			TDispContext& ctx = m_pContexts[i];

			if(ctx.m_pWorker)
				isOK &= ctx.m_pWorker->Join();

			if(!ctx.m_queue.IsEmpty())
			{
				TDispCommand* pCmd = nullptr;

				while(ctx.m_queue.PopFront(&pCmd))
					TDispCommand::Destruct(pCmd);

				VERIFY(ctx.m_queue.IsEmpty());
			}

			if(IS_VALID_FD(ctx.m_evCmd))
				isOK &= IS_NO_ERROR(close(ctx.m_evCmd));

			if(IS_VALID_FD(ctx.m_epoll))
				isOK &= IS_NO_ERROR(close(ctx.m_epoll));
		}
	}

	if(IS_VALID_FD(m_evExit))
		isOK &= IS_NO_ERROR(close(m_evExit));

	Reset();

	return isOK;
}

VOID CIODispatcher::Reset()
{
	m_uiSeq		= MAXUINT;
	m_iWorkers	= 0;
	m_iMaxEvents= 0;
	m_evExit	= INVALID_FD;
	m_pHandler	= nullptr;
	m_pContexts	= nullptr;
}

VOID CIODispatcher::MakePrefix()
{
	UINT uiNumber = ::InterlockedIncrement(&sm_uiNum);

	m_strPrefix.Format(_T("%s%u-"), WORKER_THREAD_PREFIX, uiNumber);
}

TDispContext& CIODispatcher::GetContext(int idx, FD fd)
{
	if(idx < 0) idx = fd;
	ASSERT(idx >= 0);
	if(idx >= m_iWorkers) idx %= m_iWorkers;

	return m_pContexts[idx];
}

BOOL CIODispatcher::SendCommandByIndex(int idx, USHORT t, UINT_PTR wp, UINT_PTR lp)
{
	return SendCommandByIndex(idx, TDispCommand::Construct(t, wp, lp));
}

BOOL CIODispatcher::SendCommandByIndex(int idx, TDispCommand* pCmd)
{
	TDispContext& ctx = GetContextByIndex(idx);
	return SendCommand(ctx, pCmd);
}

BOOL CIODispatcher::SendCommandByFD(FD fd, USHORT t, UINT_PTR wp, UINT_PTR lp)
{
	return SendCommandByFD(fd, TDispCommand::Construct(t, wp, lp));
}

BOOL CIODispatcher::SendCommandByFD(FD fd, TDispCommand* pCmd)
{
	TDispContext& ctx = GetContextByFD(fd);
	return SendCommand(ctx, pCmd);
}

BOOL CIODispatcher::SendCommand(TDispContext& ctx, TDispCommand* pCmd)
{
	ctx.m_queue.PushBack(pCmd);
	return VERIFY_IS_NO_ERROR(eventfd_write(ctx.m_evCmd, 1));
}

BOOL CIODispatcher::CtlFD(int idx, FD fd, int op, UINT mask, PVOID pv)
{
	const TDispContext& ctx = GetContext(idx, fd);

	epoll_event evt = {mask, pv};
	return IS_NO_ERROR(epoll_ctl(ctx.m_epoll, op, fd, &evt));
}

int CIODispatcher::WorkerProc(TDispContext* pContext)
{
	::SetSequenceThreadName(SELF_THREAD_ID, m_strPrefix, m_uiSeq);

	m_pHandler->OnDispatchThreadStart(SELF_THREAD_ID);

	BOOL bRun						  = TRUE;
	unique_ptr<epoll_event[]> pEvents = make_unique<epoll_event[]>(m_iMaxEvents);

	while(bRun)
	{
		int rs = NO_EINTR_INT(epoll_pwait(pContext->m_epoll, pEvents.get(), m_iMaxEvents, INFINITE, nullptr));

		if(rs <= TIMEOUT)
			ERROR_ABORT();

		for(int i = 0; i < rs; i++)
		{
			UINT events	= pEvents[i].events;
			PVOID ptr	= pEvents[i].data.ptr;

			if(ptr == &pContext->m_evCmd)
				ProcessCommand(pContext, events);
			else if(ptr == &m_evExit)
				bRun = ProcessExit(pContext, events);
			else
				ProcessIo(pContext, ptr, events);
		}
	}

	m_pHandler->OnDispatchThreadEnd(SELF_THREAD_ID);

	return 0;
}

BOOL CIODispatcher::ProcessCommand(TDispContext* pContext, UINT events)
{
	if(events & _EPOLL_ALL_ERROR_EVENTS)
		ERROR_ABORT();

	if(!(events & EPOLLIN))
		return FALSE;

	BOOL isOK = TRUE;

	eventfd_t v;

	int rs = eventfd_read(pContext->m_evCmd, &v);

	if(IS_NO_ERROR(rs))
	{
		ASSERT(v > 0);

		TDispCommand* pCmd = nullptr;

		while(pContext->m_queue.PopFront(&pCmd))
		{
			m_pHandler->OnCommand(pContext, pCmd);
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

BOOL CIODispatcher::ProcessExit(const TDispContext* pContext, UINT events)
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

BOOL CIODispatcher::ProcessIo(const TDispContext* pContext, PVOID pv, UINT events)
{
	if(!m_pHandler->OnBeforeProcessIo(pContext, pv, events))
		return FALSE;

	BOOL rs = DoProcessIo(pContext, pv, events);
	m_pHandler->OnAfterProcessIo(pContext, pv, events, rs);

	return rs;
}

BOOL CIODispatcher::DoProcessIo(const TDispContext* pContext, PVOID pv, UINT events)
{
	if(events & EPOLLERR)
		return m_pHandler->OnError(pContext, pv, events);
	if((events & EPOLLPRI) && !m_pHandler->OnReadyPrivilege(pContext, pv, events))
		return FALSE;
	if((events & EPOLLIN) && !m_pHandler->OnReadyRead(pContext, pv, events))
		return FALSE;
	if((events & EPOLLOUT) && !m_pHandler->OnReadyWrite(pContext, pv, events))
		return FALSE;
	if((events & (_EPOLL_HUNGUP_EVENTS)) && !m_pHandler->OnHungUp(pContext, pv, events))
		return FALSE;

	return TRUE;
}

FD CIODispatcher::AddTimer(int idx, LLONG llInterval, PVOID pv)
{
	FD fdTimer = ::CreateTimer(llInterval);

	if(IS_VALID_FD(fdTimer))
	{
		if(!AddFD(idx, fdTimer, EPOLLIN | EPOLLET, pv))
		{
			close(fdTimer);
			fdTimer = INVALID_FD;
		}
	}

	return fdTimer;
}

BOOL CIODispatcher::DelTimer(int idx, FD fdTimer)
{
	BOOL isOK = FALSE;

	if(IS_VALID_FD(fdTimer))
	{
		if(DelFD(idx, fdTimer))
			isOK = TRUE;

		close(fdTimer);
	}

	return isOK;
}
