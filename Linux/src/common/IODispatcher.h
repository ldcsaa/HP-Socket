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

#include "GlobalDef.h"
#include "Singleton.h"
#include "RingBuffer.h"
#include "Thread.h"

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>

#include <memory>

using namespace std;

#define _EPOLL_READ_PRI_EVENTS		(EPOLLPRI | EPOLLRDHUP)
#define _EPOLL_READ_EVENTS			(EPOLLIN | EPOLLRDHUP)
#define _EPOLL_ALL_READ_EVENTS		(_EPOLL_READ_EVENTS | _EPOLL_READ_PRI_EVENTS)
#define _EPOLL_WRITE_EVENTS			(EPOLLOUT)
#define _EPOLL_NORMAL_RW_EVENTS		(_EPOLL_READ_EVENTS | _EPOLL_WRITE_EVENTS)
#define _EPOLL_ALL_RW_EVENTS		(_EPOLL_ALL_READ_EVENTS | _EPOLL_WRITE_EVENTS)
#define _EPOLL_ERROR_EVENTS			(EPOLLERR)
#define _EPOLL_HUNGUP_EVENTS		(EPOLLHUP | EPOLLRDHUP)
#define _EPOLL_ALL_ERROR_EVENTS		(_EPOLL_ERROR_EVENTS | _EPOLL_HUNGUP_EVENTS)
#define _EPOLL_ALL_NORMAL_EVENTS	(_EPOLL_NORMAL_RW_EVENTS | _EPOLL_ALL_ERROR_EVENTS)
#define _EPOLL_ALL_EVENTS			(_EPOLL_ALL_RW_EVENTS | _EPOLL_ALL_ERROR_EVENTS)

#define DISP_EVENT_FLAG_R			1
#define DISP_EVENT_FLAG_W			2
#define DISP_EVENT_FLAG_H			4
#define RETRIVE_EVENT_FLAG_R(evt)	((evt) & (_EPOLL_ALL_READ_EVENTS) ? DISP_EVENT_FLAG_R : 0)
#define RETRIVE_EVENT_FLAG_W(evt)	((evt) & (_EPOLL_WRITE_EVENTS) ? DISP_EVENT_FLAG_W : 0)
#define RETRIVE_EVENT_FLAG_RW(evt)	(RETRIVE_EVENT_FLAG_R(evt) | RETRIVE_EVENT_FLAG_W(evt))
#define RETRIVE_EVENT_FLAG_H(evt)	((evt) & (_EPOLL_HUNGUP_EVENTS) ? DISP_EVENT_FLAG_H : 0)

// ------------------------------------------------------------------------------------------------------------------------------------------------------- //

struct TDispCommand
{
	USHORT	 type;
	UINT_PTR wParam;
	UINT_PTR lParam;

	static TDispCommand* Construct(USHORT t, UINT_PTR wp = 0, UINT_PTR lp = 0)
		{return new TDispCommand(t, wp, lp);}

	static VOID Destruct(TDispCommand* p)
		{if(p) delete p;}

private:
	TDispCommand(USHORT t, UINT_PTR wp = 0, UINT_PTR lp = 0)
	: type(t), wParam(wp), lParam(lp)
	{
	}

	~TDispCommand() = default;
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------- //

class IIOHandler
{
public:

	virtual VOID OnCommand(TDispCommand* pCmd)						= 0;
	virtual VOID OnTimer(ULLONG llExpirations)						= 0;

	virtual BOOL OnBeforeProcessIo(PVOID pv, UINT events)			= 0;
	virtual VOID OnAfterProcessIo(PVOID pv, UINT events, BOOL rs)	= 0;
	virtual BOOL OnReadyRead(PVOID pv, UINT events)					= 0;
	virtual BOOL OnReadyWrite(PVOID pv, UINT events)				= 0;
	virtual BOOL OnHungUp(PVOID pv, UINT events)					= 0;
	virtual BOOL OnError(PVOID pv, UINT events)						= 0;
	virtual BOOL OnReadyPrivilege(PVOID pv, UINT events)			= 0;

	virtual VOID OnDispatchThreadStart(THR_ID tid)					= 0;
	virtual VOID OnDispatchThreadEnd(THR_ID tid)					= 0;

public:
	virtual ~IIOHandler() = default;
};

class CIOHandler : public IIOHandler
{
public:
	virtual VOID OnCommand(TDispCommand* pCmd)						override {}
	virtual VOID OnTimer(ULLONG llExpirations)						override {}

	virtual BOOL OnBeforeProcessIo(PVOID pv, UINT events)			override {return TRUE;}
	virtual VOID OnAfterProcessIo(PVOID pv, UINT events, BOOL rs)	override {}
	virtual BOOL OnReadyWrite(PVOID pv, UINT events)				override {return TRUE;}
	virtual BOOL OnHungUp(PVOID pv, UINT events)					override {return TRUE;}
	virtual BOOL OnError(PVOID pv, UINT events)						override {return TRUE;}
	virtual BOOL OnReadyPrivilege(PVOID pv, UINT events)			override {return TRUE;}
	virtual VOID OnDispatchThreadStart(THR_ID tid)					override {}
	virtual VOID OnDispatchThreadEnd(THR_ID tid)					override {}
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------- //

class CIODispatcher
{
public:
	static const int DEF_WORKER_MAX_EVENTS	= 64;

	using CCommandQueue	= CCASQueue<TDispCommand>;
	using CWorkerThread	= CThread<CIODispatcher, VOID, int>;

public:
	BOOL Start(IIOHandler* pHandler, int iWorkerMaxEvents = DEF_WORKER_MAX_EVENTS, int iWorkers = 0, LLONG llTimerInterval = 0);
	BOOL Stop(BOOL bCheck = TRUE);

	BOOL SendCommand(TDispCommand* pCmd);
	BOOL SendCommand(USHORT t, UINT_PTR wp = 0, UINT_PTR lp = 0);

	template<class _List, typename = enable_if_t<is_same<remove_reference_t<typename _List::reference>, TDispCommand*>::value>>
	BOOL SendCommands(const _List& cmds)
	{
		size_t size = cmds.size();
		if(size == 0) return FALSE;

		for(auto it = cmds.begin(), end = cmds.end(); it != end; ++it)
			m_queue.PushBack(*it);

		return VERIFY_IS_NO_ERROR(eventfd_write(m_evCmd, size));
	}

	BOOL AddFD(FD fd, UINT mask, PVOID pv)	{return CtlFD(fd, EPOLL_CTL_ADD, mask, pv);}
	BOOL ModFD(FD fd, UINT mask, PVOID pv)	{return CtlFD(fd, EPOLL_CTL_MOD, mask, pv);}
	BOOL DelFD(FD fd)						{return CtlFD(fd, EPOLL_CTL_DEL, 0, nullptr);}
	BOOL CtlFD(FD fd, int op, UINT mask, PVOID pv);

	BOOL ProcessIo(PVOID pv, UINT events);

	FD AddTimer		(LLONG llInterval, PVOID pv);
	BOOL DelTimer	(FD fdTimer);

private:
	int WorkerProc(PVOID pv = nullptr);
	BOOL ProcessExit(UINT events);
	BOOL ProcessTimer(UINT events);
	BOOL ProcessCommand(UINT events);
	BOOL DoProcessIo(PVOID pv, UINT events);
	
	VOID Reset();

public:
	BOOL HasStarted()	{return m_pHandler && m_pWorkers;}
	const CWorkerThread* GetWorkerThreads() {return m_pWorkers.get();}

	CIODispatcher()		{Reset();}
	~CIODispatcher()	{if(HasStarted()) Stop();}

private:
	IIOHandler*	m_pHandler;
	FD			m_epoll;
	FD			m_evCmd;
	FD			m_evExit;
	FD			m_evTimer;
	int			m_iWorkers;
	int			m_iMaxEvents;

	CCommandQueue				m_queue;
	unique_ptr<CWorkerThread[]>	m_pWorkers;
};
