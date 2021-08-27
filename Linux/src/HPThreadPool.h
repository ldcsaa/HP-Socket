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

#include "../include/hpsocket/SocketInterface.h"
#include "common/STLHelper.h"
#include "common/Semaphore.h"
#include "common/RingBuffer.h"
#include "InternalDef.h"

LPTSocketTask CreateSocketTaskObj(	Fn_SocketTaskProc fnTaskProc,
									PVOID pSender, CONNID dwConnID,
									LPCBYTE pBuffer, INT iBuffLen, EnTaskBufferType enBuffType = TBT_COPY,
									WPARAM wParam = 0, LPARAM lParam = 0);

void DestroySocketTaskObj(LPTSocketTask pTask);

class CHPThreadPool : public IHPThreadPool
{
private:
	enum EnSubmitResult{SUBMIT_OK, SUBMIT_FULL, SUBMIT_ERROR};

	struct TTask
	{
		Fn_TaskProc	fn;
		PVOID		arg;
		BOOL		freeArg;

	public:
		static TTask* Construct(Fn_TaskProc fnTaskProc, PVOID pvArg, BOOL bFreeArg)
		{
			return new TTask(fnTaskProc, pvArg, bFreeArg);
		}

		static void Destruct(TTask* pTask)
		{
			if(pTask)
			{
				delete pTask;
			}
		}

	private:
		TTask(Fn_TaskProc fnTaskProc, PVOID pvArg, BOOL bFreeArg)
		: fn(fnTaskProc), arg(pvArg), freeArg(bFreeArg)
		{
			ASSERT(fn != nullptr);
		}
	};

	using CTaskQueue = CCASQueue<TTask>;

public:
	virtual BOOL Start(DWORD dwThreadCount = 0, DWORD dwMaxQueueSize = 0, EnRejectedPolicy enRejectedPolicy = TRP_CALL_FAIL, DWORD dwStackSize = 0);
	virtual BOOL Stop(DWORD dwMaxWait = INFINITE);
	virtual BOOL Wait(DWORD dwMilliseconds = INFINITE) {return m_evWait.WaitFor(dwMilliseconds, CStopWaitingPredicate<IHPThreadPool>(this));}

	virtual BOOL Submit(Fn_TaskProc fnTaskProc, PVOID pvArg, DWORD dwMaxWait = INFINITE);
	virtual BOOL Submit(LPTSocketTask pTask, DWORD dwMaxWait = INFINITE);
	virtual BOOL AdjustThreadCount(DWORD dwNewThreadCount);

public:
	virtual BOOL HasStarted()						{return m_enState == SS_STARTED || m_enState == SS_STARTING;}
	virtual EnServiceState GetState()				{return m_enState;}

	virtual DWORD GetQueueSize()					{return m_lsTasks.Size();}
	virtual DWORD GetTaskCount()					{return m_dwTaskCount;}
	virtual DWORD GetThreadCount()					{return m_dwThreadCount;}
	virtual DWORD GetMaxQueueSize()					{return m_dwMaxQueueSize;}
	virtual EnRejectedPolicy GetRejectedPolicy()	{return m_enRejectedPolicy;}

private:
	BOOL CheckStarting();
	BOOL CheckStarted();
	BOOL CheckStoping();

	BOOL InternalAdjustThreadCount(DWORD dwNewThreadCount);
	BOOL DoAdjustThreadCount(DWORD dwNewThreadCount);

	BOOL CreateWorkerThreads(DWORD dwThreadCount);

	BOOL Shutdown(DWORD dwMaxWait);
	BOOL CheckWorkerThreadExit();

	static PVOID ThreadProc(LPVOID pv);
	int WorkerProc();

	EnSubmitResult DirectSubmit(Fn_TaskProc fnTaskProc, PVOID pvArg, BOOL bFreeArg);
	BOOL CycleWaitSubmit(Fn_TaskProc fnTaskProc, PVOID pvArg, DWORD dwMaxWait, BOOL bFreeArg);
	BOOL DoSubmit(Fn_TaskProc fnTaskProc, PVOID pvArg, BOOL bFreeArg, DWORD dwMaxWait);
	void DoRunTaskProc(Fn_TaskProc fnTaskProc, PVOID pvArg, BOOL bFreeArg);

	void FireStartup()
		{if(m_pListener != nullptr) m_pListener->OnStartup(this);}
	void FireShutdown()
		{if(m_pListener != nullptr) m_pListener->OnShutdown(this);}
	void FireWorkerThreadStart()
		{if(m_pListener != nullptr) m_pListener->OnWorkerThreadStart(this, SELF_THREAD_ID);}
	void FireWorkerThreadEnd()
		{if(m_pListener != nullptr) m_pListener->OnWorkerThreadEnd(this, SELF_THREAD_ID);}

public:
	CHPThreadPool(IHPThreadPoolListener* pListener = nullptr)
	: m_pListener(pListener)
	{
		Reset(FALSE);
	}

	virtual ~CHPThreadPool()
	{
		ENSURE_STOP();
	}

private:
	void Reset(BOOL bSetWaitEvent = TRUE);

private:
	IHPThreadPoolListener*	m_pListener;

	DWORD					m_dwStackSize;
	DWORD					m_dwMaxQueueSize;
	EnRejectedPolicy		m_enRejectedPolicy;

	volatile DWORD			m_dwTaskCount;
	volatile DWORD			m_dwThreadCount;
	volatile EnServiceState	m_enState;

	CSEM					m_evWait;
	CSEM					m_evShutdown;
	CSEM					m_evTask;
	CSEM					m_evQueue;
	CCriSec					m_csThread;

	CTaskQueue				m_lsTasks;
	unordered_set<THR_ID>	m_stThreads;

	DECLARE_NO_COPY_CLASS(CHPThreadPool)
};
