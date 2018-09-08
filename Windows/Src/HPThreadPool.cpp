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
 
#include "stdafx.h"
#include "HPThreadPool.h"

#include "../Common/Src/WaitFor.h"
#include "../Common/Src/FuncHelper.h"

LPTSocketTask CreateSocketTaskObj(	Fn_SocketTaskProc fnTaskProc,
									PVOID pSender, CONNID dwConnID,
									LPCBYTE pBuffer, INT iBuffLen, EnTaskBufferType enBuffType,
									WPARAM wParam, LPARAM lParam)
{
	ASSERT(fnTaskProc != nullptr);
	ASSERT(iBuffLen >= 0);

	LPTSocketTask pTask = new TSocketTask;

	pTask->fn		= fnTaskProc;
	pTask->sender	= pSender;
	pTask->connID	= dwConnID;
	pTask->bufLen	= iBuffLen;
	pTask->bufType	= enBuffType;
	pTask->wparam	= wParam;
	pTask->lparam	= lParam;

	if(enBuffType != TBT_COPY || !pBuffer)
		pTask->buf = pBuffer;
	else
	{
		pTask->buf = MALLOC(BYTE, iBuffLen);
		::CopyMemory((LPBYTE)pTask->buf, pBuffer, iBuffLen);
	}

	return pTask;
}

void DestroySocketTaskObj(LPTSocketTask pTask)
{
	if(pTask)
	{
		if(pTask->bufType != TBT_REFER && pTask->buf)
			FREE(pTask->buf);

		delete pTask;
	}
}

BOOL CHPThreadPool::CWorker::Initialize(PVOID pvWorkerParam)
{
	ASSERT(pvWorkerParam != nullptr);

	m_pthPool = (CHPThreadPool*)pvWorkerParam;

	return TRUE;
}

void CHPThreadPool::CWorker::Execute(TTask* pTask, PVOID pvWorkerParam, OVERLAPPED* pOverlapped)
{
	ASSERT(pvWorkerParam == m_pthPool);

	::InterlockedDecrement(&m_pthPool->m_dwQueueSize);

	pTask->fn(pTask->arg);

	if(pTask->freeArg)
		::DestroySocketTaskObj((LPTSocketTask)pTask->arg);

	TTask::Destruct(pTask);
}

void CHPThreadPool::CWorker::Terminate(PVOID pvWorkerParam)
{
	ASSERT(pvWorkerParam == m_pthPool);
}

BOOL CHPThreadPool::Start(DWORD dwThreadCount, DWORD dwMaxQueueSize, EnRejectedPolicy enRejectedPolicy, DWORD dwStackSize)
{
	if(!CheckStarting())
		return FALSE;

	m_dwMaxQueueSize	= dwMaxQueueSize;
	m_enRejectedPolicy	= enRejectedPolicy;

	if(dwThreadCount == 0)
		dwThreadCount = ::GetDefaultWorkerThreadCount();

	HRESULT hr = m_thPool.Initialize(this, (int)dwThreadCount, dwStackSize);

	if(FAILED(hr))
	{
		Stop();

		::SetLastError(HRESULT_CODE(hr));

		return FALSE;
	}

	m_enState = SS_STARTED;

	return TRUE;;
}

BOOL CHPThreadPool::Stop(DWORD dwMaxWait)
{
	if(!CheckStoping())
		return FALSE;

	::WaitWithMessageLoop(15);

	if(dwMaxWait == 0)
		dwMaxWait = INFINITE;

	m_thPool.Shutdown(dwMaxWait);

	ASSERT(GetQueueSize()	== 0);
	ASSERT(GetThreadCount()	== 0);

	Reset();

	return TRUE;
}

BOOL CHPThreadPool::Submit(Fn_TaskProc fnTaskProc, PVOID pvArg, DWORD dwMaxWait)
{
	return DoSubmit(fnTaskProc, pvArg, FALSE, dwMaxWait);
}

BOOL CHPThreadPool::Submit(LPTSocketTask pTask, DWORD dwMaxWait)
{
	return DoSubmit((Fn_TaskProc)pTask->fn, (PVOID)pTask, TRUE, dwMaxWait);
}

BOOL CHPThreadPool::DoSubmit(Fn_TaskProc fnTaskProc, PVOID pvArg, BOOL bFreeArg, DWORD dwMaxWait)
{
	EnSubmitResult sr = DirectSubmit(fnTaskProc, pvArg, bFreeArg);

	if(sr != SUBMIT_FULL)
		return (sr == SUBMIT_OK);

	if(m_enRejectedPolicy == TRP_CALL_FAIL)
	{
		::SetLastError(ERROR_DESTINATION_ELEMENT_FULL);
		return FALSE;
	}
	else if(m_enRejectedPolicy == TRP_WAIT_FOR)
	{
		return CycleWaitSubmit(fnTaskProc, pvArg, dwMaxWait, bFreeArg);
	}
	else if(m_enRejectedPolicy == TRP_CALLER_RUN)
	{
		fnTaskProc(pvArg);
	}
	else
	{
		ENSURE(FALSE);

		::SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	return TRUE;
}

CHPThreadPool::EnSubmitResult CHPThreadPool::DirectSubmit(Fn_TaskProc fnTaskProc, PVOID pvArg, BOOL bFreeArg)
{
	if(!CheckStarted())
		return SUBMIT_ERROR;

	if(m_dwMaxQueueSize == 0)
		::InterlockedIncrement(&m_dwQueueSize);
	else
	{
		for(UINT i = 0; ; i++)
		{
			DWORD dwQueueSize = m_dwQueueSize;

			if(dwQueueSize >= m_dwMaxQueueSize)
				return SUBMIT_FULL;

			DWORD dwInitialQueueSize = ::InterlockedCompareExchange(&m_dwQueueSize, dwQueueSize + 1, dwQueueSize);

			if(dwInitialQueueSize == dwQueueSize)
				break;
			else if(dwInitialQueueSize == m_dwMaxQueueSize)
				return SUBMIT_FULL;

			::YieldThread(i);

			if(!CheckStarted())
				return SUBMIT_ERROR;
		}
	}

	TTask* pTask = TTask::Construct(fnTaskProc, pvArg, bFreeArg);

	if(!m_thPool.QueueRequest(pTask))
	{
		::InterlockedDecrement(&m_dwQueueSize);

		TTask::Destruct(pTask);

		return SUBMIT_ERROR;
	}

	return SUBMIT_OK;
}

BOOL CHPThreadPool::CycleWaitSubmit(Fn_TaskProc fnTaskProc, PVOID pvArg, DWORD dwMaxWait, BOOL bFreeArg)
{
	EnSubmitResult sr	= SUBMIT_FULL;
	DWORD dwTime		= ::TimeGetTime();

	for(DWORD i = 0; dwMaxWait == INFINITE || dwMaxWait == 0 || ::GetTimeGap32(dwTime) <= dwMaxWait; i++)
	{
		((i & 8191) == 8191) ? ::WaitFor(1) : ::SwitchToThread();

		sr = DirectSubmit(fnTaskProc, pvArg, bFreeArg);

		if(sr == SUBMIT_OK)
			return TRUE;
		else if(sr == SUBMIT_ERROR)
			return FALSE;
	}

	::SetLastError(ERROR_DESTINATION_ELEMENT_FULL);

	return FALSE;
}

BOOL CHPThreadPool::AdjustThreadCount(DWORD dwNewThreadCount)
{
	if(!CheckStarted())
		return FALSE;

	if(dwNewThreadCount == 0)
		dwNewThreadCount = ::GetDefaultWorkerThreadCount();

	HRESULT hr = m_thPool.SetSize(dwNewThreadCount);

	if(!SUCCEEDED(hr))
	{
		::SetLastError(HRESULT_CODE(hr));
		return FALSE;
	}

	return TRUE;
}

BOOL CHPThreadPool::CheckStarting()
{
	if(::InterlockedCompareExchange((volatile LONG*)&m_enState, SS_STARTING, SS_STOPPED) != SS_STOPPED)
	{
		::SetLastError(ERROR_INVALID_OPERATION);
		return FALSE;
	}

	return TRUE;
}

BOOL CHPThreadPool::CheckStarted()
{
	if(m_enState != SS_STARTED)
	{
		::SetLastError(ERROR_INVALID_OPERATION);
		return FALSE;
	}

	return TRUE;
}

BOOL CHPThreadPool::CheckStoping()
{
	if( ::InterlockedCompareExchange((volatile LONG*)&m_enState, SS_STOPPING, SS_STARTED) != SS_STARTED &&
		::InterlockedCompareExchange((volatile LONG*)&m_enState, SS_STOPPING, SS_STARTING) != SS_STARTING)
	{
		while(m_enState != SS_STOPPED)
			::WaitFor(5);

		::SetLastError(ERROR_INVALID_OPERATION);
		return FALSE;
	}

	return TRUE;
}

void CHPThreadPool::Reset()
{
	m_dwQueueSize		= 0;
	m_dwMaxQueueSize	= 0;
	m_enRejectedPolicy	= TRP_CALL_FAIL;
	m_enState			= SS_STOPPED;
}
