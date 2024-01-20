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

#include "../../include/hpsocket/GlobalDef.h"
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

#define DISP_EVENT_FLAG_R			0x1
#define DISP_EVENT_FLAG_W			0x2
#define DISP_EVENT_FLAG_H			0x4

#define RETRIVE_EVENT_FLAG_R(evt)	((evt) & (_EPOLL_ALL_READ_EVENTS) ? DISP_EVENT_FLAG_R : 0)
#define RETRIVE_EVENT_FLAG_W(evt)	((evt) & (_EPOLL_WRITE_EVENTS) ? DISP_EVENT_FLAG_W : 0)
#define RETRIVE_EVENT_FLAG_RW(evt)	(RETRIVE_EVENT_FLAG_R(evt) | RETRIVE_EVENT_FLAG_W(evt))
#define RETRIVE_EVENT_FLAG_H(evt)	((evt) & (_EPOLL_HUNGUP_EVENTS) ? DISP_EVENT_FLAG_H : 0)

#ifndef EPOLLEXCLUSIVE
	#define EPOLLEXCLUSIVE			(1u << 28)
#endif

#define MAYBE_EPOLLEXCLUSIVE		(::IsKernelVersionAbove(4, 5, 0) ? EPOLLEXCLUSIVE : 0)

// ------------------------------------------------------------------------------------------------------------------------------------------------------- //

struct	TDispCommand;
class	CIODispatcher;

struct TDispContext
{
	friend class CIODispatcher;

	using CCommandQueue = CCASQueue<TDispCommand>;
	using CWorkerThread = CThread<CIODispatcher, TDispContext, int>;

public:
	int GetIndex()		 const {return m_iIndex;}
	THR_ID GetThreadId() const {return m_pWorker != nullptr ? m_pWorker->GetThreadID() : 0;}

public:
	TDispContext()	{Reset();}
	~TDispContext() = default;

	DECLARE_NO_COPY_CLASS(TDispContext)

private:
	VOID Reset()
	{
		m_iIndex	= -1;
		m_epoll		= INVALID_FD;
		m_evCmd		= INVALID_FD;
		m_pWorker	= nullptr;
	}

private:
	int	m_iIndex;
	FD	m_epoll;
	FD	m_evCmd;

	CCommandQueue				m_queue;
	unique_ptr<CWorkerThread>	m_pWorker;
};

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
	virtual VOID OnCommand(const TDispContext* pContext, TDispCommand* pCmd)					= 0;

	virtual BOOL OnBeforeProcessIo(const TDispContext* pContext, PVOID pv, UINT events)			= 0;
	virtual VOID OnAfterProcessIo(const TDispContext* pContext, PVOID pv, UINT events, BOOL rs)	= 0;
	virtual BOOL OnReadyRead(const TDispContext* pContext, PVOID pv, UINT events)				= 0;
	virtual BOOL OnReadyWrite(const TDispContext* pContext, PVOID pv, UINT events)				= 0;
	virtual BOOL OnHungUp(const TDispContext* pContext, PVOID pv, UINT events)					= 0;
	virtual BOOL OnError(const TDispContext* pContext, PVOID pv, UINT events)					= 0;
	virtual BOOL OnReadyPrivilege(const TDispContext* pContext, PVOID pv, UINT events)			= 0;

	virtual VOID OnDispatchThreadStart(THR_ID tid)												= 0;
	virtual VOID OnDispatchThreadEnd(THR_ID tid)												= 0;

public:
	virtual ~IIOHandler() = default;
};

class CIOHandler : public IIOHandler
{
public:
	virtual VOID OnCommand(const TDispContext* pContext, TDispCommand* pCmd)					override {}

	virtual BOOL OnBeforeProcessIo(const TDispContext* pContext, PVOID pv, UINT events)			override {return TRUE;}
	virtual VOID OnAfterProcessIo(const TDispContext* pContext, PVOID pv, UINT events, BOOL rs)	override {}
	virtual BOOL OnReadyWrite(const TDispContext* pContext, PVOID pv, UINT events)				override {return TRUE;}
	virtual BOOL OnHungUp(const TDispContext* pContext, PVOID pv, UINT events)					override {return TRUE;}
	virtual BOOL OnError(const TDispContext* pContext, PVOID pv, UINT events)					override {return TRUE;}
	virtual BOOL OnReadyPrivilege(const TDispContext* pContext, PVOID pv, UINT events)			override {return TRUE;}

	virtual VOID OnDispatchThreadStart(THR_ID tid)												override {}
	virtual VOID OnDispatchThreadEnd(THR_ID tid)												override {}
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------- //

class CIODispatcher
{
public:
	static const int DEF_WORKER_MAX_EVENTS	= 64;

	using CCommandQueue	= TDispContext::CCommandQueue;
	using CWorkerThread	= TDispContext::CWorkerThread;

public:
	BOOL Start(IIOHandler* pHandler, int iWorkerMaxEvents = DEF_WORKER_MAX_EVENTS, int iWorkers = 0);
	BOOL Stop(BOOL bCheck = TRUE);

	BOOL SendCommandByIndex(int idx, TDispCommand* pCmd);
	BOOL SendCommandByIndex(int idx, USHORT t, UINT_PTR wp = 0, UINT_PTR lp = 0);
	BOOL SendCommandByFD(FD fd, TDispCommand* pCmd);
	BOOL SendCommandByFD(FD fd, USHORT t, UINT_PTR wp = 0, UINT_PTR lp = 0);
	BOOL SendCommand(TDispContext& ctx, TDispCommand* pCmd);

	template<class _List, typename = enable_if_t<is_same<remove_reference_t<typename _List::reference>, TDispCommand*>::value>>
	BOOL SendCommandsByIndex(int idx, const _List& cmds)
	{
		TDispContext& ctx = GetContextByIndex(idx);
		return SendCommands(ctx, cmds);
	}

	template<class _List, typename = enable_if_t<is_same<remove_reference_t<typename _List::reference>, TDispCommand*>::value>>
	BOOL SendCommandsByFD(FD fd, const _List& cmds)
	{
		TDispContext& ctx = GetContextByFD(fd);
		return SendCommands(ctx, cmds);
	}

	template<class _List, typename = enable_if_t<is_same<remove_reference_t<typename _List::reference>, TDispCommand*>::value>>
	BOOL SendCommands(TDispContext& ctx, const _List& cmds)
	{
		size_t size = cmds.size();
		if(size == 0) return FALSE;

		for(auto it = cmds.begin(), end = cmds.end(); it != end; ++it)
			ctx.m_queue.PushBack(*it);

		return VERIFY_IS_NO_ERROR(eventfd_write(ctx.m_evCmd, size));
	}

	BOOL AddFD(int idx, FD fd, UINT mask, PVOID pv)	{return CtlFD(idx, fd, EPOLL_CTL_ADD, mask, pv);}
	BOOL ModFD(int idx, FD fd, UINT mask, PVOID pv)	{return CtlFD(idx, fd, EPOLL_CTL_MOD, mask, pv);}
	BOOL DelFD(int idx, FD fd)						{return CtlFD(idx, fd, EPOLL_CTL_DEL, 0, nullptr);}
	BOOL CtlFD(int idx, FD fd, int op, UINT mask, PVOID pv);


	BOOL AddFD(FD fd, UINT mask, PVOID pv)			{return CtlFD(-1, fd, EPOLL_CTL_ADD, mask, pv);}
	BOOL ModFD(FD fd, UINT mask, PVOID pv)			{return CtlFD(-1, fd, EPOLL_CTL_MOD, mask, pv);}
	BOOL DelFD(FD fd)								{return CtlFD(-1, fd, EPOLL_CTL_DEL, 0, nullptr);}
	BOOL CtlFD(FD fd, int op, UINT mask, PVOID pv)	{return CtlFD(-1, fd, op, mask, pv);}
											
	BOOL ProcessIo(const TDispContext* pContext, PVOID pv, UINT events);

	FD AddTimer		(int idx, LLONG llInterval, PVOID pv);
	BOOL DelTimer	(int idx, FD fdTimer);

	FD AddTimer		(LLONG llInterval, PVOID pv)	{return AddTimer(-1, llInterval, pv);}
	BOOL DelTimer	(FD fdTimer)					{return DelTimer(-1, fdTimer);}

private:
	int WorkerProc(TDispContext* pContext);
	BOOL ProcessExit(const TDispContext* pContext, UINT events);
	BOOL ProcessCommand(TDispContext* pContext, UINT events);
	BOOL DoProcessIo(const TDispContext* pContext, PVOID pv, UINT events);

	VOID Reset();
	VOID MakePrefix();

	TDispContext& GetContextByIndex(int idx)	{return GetContext(idx, -1);}
	TDispContext& GetContextByFD(FD fd)			{return GetContext(-1, fd);}
	TDispContext& GetContext(int idx, FD fd);

public:
	const TDispContext& GetContextRefByIndex(int idx)	{return GetContextByIndex(idx);}
	const TDispContext& GetContextRefByFD(FD fd)		{return GetContextByFD(fd);}
	const TDispContext& GetContextRef(int idx, FD fd)	{return GetContext(idx, fd);}

	BOOL HasStarted()	{return m_pHandler && m_pContexts;}
	int GetWorkers()	{return m_iWorkers;}
	const TDispContext* GetContexts() {return m_pContexts.get();}

	CIODispatcher()		{MakePrefix(); Reset();}
	~CIODispatcher()	{if(HasStarted()) Stop();}

	DECLARE_NO_COPY_CLASS(CIODispatcher)

private:
	static LPCTSTR WORKER_THREAD_PREFIX;
	static volatile UINT sm_uiNum;

	volatile UINT	m_uiSeq;
	CString			m_strPrefix;

private:
	int				m_iWorkers;
	int				m_iMaxEvents;

	FD				m_evExit;

	IIOHandler*				m_pHandler;
	unique_ptr<TDispContext[]>	m_pContexts;
};
