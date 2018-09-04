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
 
#include "UdpClient.h"

BOOL CUdpClient::Start(LPCTSTR lpszRemoteAddress, USHORT usPort, BOOL bAsyncConnect, LPCTSTR lpszBindAddress, USHORT usLocalPort)
{
	if(!CheckParams() || !CheckStarting())
		return FALSE;

	PrepareStart();
	m_ccContext.Reset();

	BOOL isOK = FALSE;
	HP_SOCKADDR addrRemote, addrBind;

	if(CreateClientSocket(lpszRemoteAddress, addrRemote, usPort, lpszBindAddress, addrBind))
	{
		if(BindClientSocket(addrBind, addrRemote, usLocalPort))
		{
			if(TRIGGER(FirePrepareConnect(m_soClient)) != HR_ERROR)
			{
				if(ConnectToServer(addrRemote, bAsyncConnect))
				{
					if(CreateWorkerThread())
						isOK = TRUE;
					else
						SetLastError(SE_WORKER_THREAD_CREATE, __FUNCTION__, ERROR_CREATE_FAILED);
				}
				else
					SetLastError(SE_CONNECT_SERVER, __FUNCTION__, ::WSAGetLastError());
			}
			else
				SetLastError(SE_SOCKET_PREPARE, __FUNCTION__, ENSURE_ERROR_CANCELLED);
		}
		else
			SetLastError(SE_SOCKET_BIND, __FUNCTION__, ::WSAGetLastError());
	}
	else
		SetLastError(SE_SOCKET_CREATE, __FUNCTION__, ::WSAGetLastError());

	if(!isOK)
	{
		m_ccContext.Reset(FALSE);
		EXECUTE_RESTORE_ERROR(Stop());
	}

	return isOK;
}

BOOL CUdpClient::CheckParams()
{
	if	(((int)m_dwMaxDatagramSize > 0)		&&
		((int)m_dwFreeBufferPoolSize >= 0)	&&
		((int)m_dwFreeBufferPoolHold >= 0)	&&
		((int)m_dwDetectAttempts >= 0)		&&
		((int)m_dwDetectInterval >= 0)		)
		return TRUE;

	SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
	return FALSE;
}

void CUdpClient::PrepareStart()
{
	m_itPool.SetItemCapacity(m_dwMaxDatagramSize);
	m_itPool.SetPoolSize(m_dwFreeBufferPoolSize);
	m_itPool.SetPoolHold(m_dwFreeBufferPoolHold);

	m_itPool.Prepare();
}

BOOL CUdpClient::CheckStarting()
{
	CSpinLock locallock(m_csState);

	if(m_enState == SS_STOPPED)
		m_enState = SS_STARTING;
	else
	{
		SetLastError(SE_ILLEGAL_STATE, __FUNCTION__, ERROR_INVALID_OPERATION);
		return FALSE;
	}

	return TRUE;
}

BOOL CUdpClient::CheckStoping()
{
	if(m_enState != SS_STOPPED)
	{
		CSpinLock locallock(m_csState);

		if(HasStarted())
		{
			m_enState = SS_STOPPING;
			return TRUE;
		}

		if(!m_thWorker.IsInMyThread())
		{
			while(m_enState != SS_STOPPED)
				::WaitFor(10);
		}
	}

	SetLastError(SE_ILLEGAL_STATE, __FUNCTION__, ERROR_INVALID_OPERATION);

	return FALSE;
}

BOOL CUdpClient::CreateClientSocket(LPCTSTR lpszRemoteAddress, HP_SOCKADDR& addrRemote, USHORT usPort, LPCTSTR lpszBindAddress, HP_SOCKADDR& addrBind)
{
	if(!::GetSockAddrByHostName(lpszRemoteAddress, usPort, addrRemote))
		return FALSE;

	if(!::IsStrEmpty(lpszBindAddress))
	{
		if(!::sockaddr_A_2_IN(lpszBindAddress, 0, addrBind))
			return FALSE;

		if(addrRemote.family != addrBind.family)
		{
			::WSASetLastError(ERROR_AFNOSUPPORT);
			return FALSE;
		}
	}

	m_soClient = socket(addrRemote.family, SOCK_DGRAM, IPPROTO_UDP);

	if(m_soClient == INVALID_SOCKET)
		return FALSE;

	SetRemoteHost(lpszRemoteAddress, usPort);

	return TRUE;
}

BOOL CUdpClient::BindClientSocket(const HP_SOCKADDR& addrBind, const HP_SOCKADDR& addrRemote, USHORT usLocalPort)
{
	if(addrBind.IsSpecified() && usLocalPort == 0)
	{
		if(::bind(m_soClient, addrBind.Addr(), addrBind.AddrSize()) == SOCKET_ERROR)
			return FALSE;
	}
	else if(usLocalPort != 0)
	{
		HP_SOCKADDR realBindAddr = addrBind.IsSpecified() ? addrBind : HP_SOCKADDR::AnyAddr(addrRemote.family);

		realBindAddr.SetPort(usLocalPort);

		if(::bind(m_soClient, realBindAddr.Addr(), realBindAddr.AddrSize()) == SOCKET_ERROR)
			return FALSE;
	}

	m_dwConnID = ::GenerateConnectionID();

	return TRUE;
}

BOOL CUdpClient::ConnectToServer(const HP_SOCKADDR& addrRemote, BOOL bAsyncConnect)
{
	BOOL isOK = FALSE;

	if(bAsyncConnect)
	{
		VERIFY(::fcntl_SETFL(m_soClient, O_NOATIME | O_NONBLOCK | O_CLOEXEC));

		int rc = ::connect(m_soClient, addrRemote.Addr(), addrRemote.AddrSize());

		if(IS_NO_ERROR(rc) || IS_IO_PENDING_ERROR())
		{
			m_nEvents	= POLLOUT;
			isOK		= TRUE;
		}
	}
	else
	{
		if(::connect(m_soClient, addrRemote.Addr(), addrRemote.AddrSize()) != SOCKET_ERROR)
		{
			VERIFY(::fcntl_SETFL(m_soClient, O_NOATIME | O_NONBLOCK | O_CLOEXEC));

			SetConnected();

			if(TRIGGER(FireConnect()) == HR_ERROR)
				::WSASetLastError(ENSURE_ERROR_CANCELLED);
			else
			{
				VERIFY(DetectConnection());

				m_nEvents = (SHORT)((m_lsSend.IsEmpty() ? 0 : POLLOUT) | (m_bPaused ? 0 : POLLIN) | POLLRDHUP);
				isOK	  = TRUE;
			}
		}
	}

	return isOK;
}

BOOL CUdpClient::Stop()
{
	if(!CheckStoping())
		return FALSE;

	SetConnected(FALSE);

	WaitForWorkerThreadEnd();

	if(m_ccContext.bFireOnClose)
		FireClose(m_ccContext.enOperation, m_ccContext.iErrorCode);

	if(m_soClient != INVALID_SOCKET)
	{
		shutdown(m_soClient, SHUT_WR);
		closesocket(m_soClient);

		m_soClient = INVALID_SOCKET;
	}

	Reset();

	return TRUE;
}

void CUdpClient::Reset()
{
	CCriSecLock locallock(m_csSend);

	m_evSend.Reset();
	m_evRecv.Reset();
	m_evStop.Reset();
	m_evDetect.Set(0, 0);

	m_lsSend.Clear();
	m_itPool.Clear();
	m_rcBuffer.Free();

	m_strHost.Empty();

	m_usPort		= 0;
	m_nEvents		= 0;
	m_dwDetectFails	= 0;
	m_bPaused		= FALSE;
	m_enState		= SS_STOPPED;
}

void CUdpClient::WaitForWorkerThreadEnd()
{
	if(!m_thWorker.IsRunning())
		return;

	if(m_thWorker.IsInMyThread())
		m_thWorker.Detach();
	else
	{
		m_evStop.Set();
		m_thWorker.Join();
	}
}

BOOL CUdpClient::CreateWorkerThread()
{
	return m_thWorker.Start(this, &CUdpClient::WorkerThreadProc);
}

UINT WINAPI CUdpClient::WorkerThreadProc(LPVOID pv)
{
	TRACE("---------------> Client Worker Thread 0x%08X started <---------------", SELF_THREAD_ID);

	BOOL bCallStop	= TRUE;
	int size		= IsNeedDetect() ? 5 : 4;
	pollfd* pfds	= CreateLocalObjects(pollfd, size);

	pfds[0] = {m_soClient, m_nEvents};
	pfds[1] = {m_evSend.GetFD(), POLLIN};
	pfds[2] = {m_evRecv.GetFD(), POLLIN};
	pfds[3] = {m_evStop.GetFD(), POLLIN};

	if(IsNeedDetect())
	{
		m_evDetect.Set(m_dwDetectInterval * 1000, m_dwDetectInterval * 1000);
		pfds[4] = {m_evDetect.GetFD(), POLLIN};
	}

	m_rcBuffer.Malloc(m_dwMaxDatagramSize);

	while(HasStarted())
	{
		int rs = (int)::PollForMultipleObjects(pfds, size);
		ASSERT(rs > TIMEOUT);

		if(rs <= 0)
		{
			m_ccContext.Reset(TRUE, SO_UNKNOWN, ::WSAGetLastError());
			goto EXIT_WORKER_THREAD;
		}

		for(int i = 0; i < size; i++)
		{
			if((1 << i) & rs)
			{
				SHORT revents = pfds[i].revents;

				if(i == 0)
				{
					if(!ProcessNetworkEvent(revents))
						goto EXIT_WORKER_THREAD;
				}
				else if(i == 1)
				{
					m_evSend.Reset();

					if(!SendData())
						goto EXIT_WORKER_THREAD;
				}
				else if(i == 2)
				{
					m_evRecv.Reset();

					if(!ReadData())
						goto EXIT_WORKER_THREAD;
				}
				else if(i == 3)
				{
					m_evStop.Reset();

					bCallStop = FALSE;
					goto EXIT_WORKER_THREAD;
				}
				else if(i == 4)
				{
					m_evDetect.Reset();

					if(!CheckConnection())
						goto EXIT_WORKER_THREAD;
				}
				else
					VERIFY(FALSE);
			}
		}

		m_nEvents = (SHORT)((m_lsSend.IsEmpty() ? 0 : POLLOUT) | (m_bPaused ? 0 : POLLIN) | POLLRDHUP);
		pfds[0].events = m_nEvents;
	}

EXIT_WORKER_THREAD:

	OnWorkerThreadEnd(SELF_THREAD_ID);

	if(bCallStop && HasStarted())
		Stop();

	TRACE("---------------> Client Worker Thread 0x%08X stoped <---------------", SELF_THREAD_ID);

	return 0;
}

BOOL CUdpClient::CheckConnection()
{
	if(m_dwDetectFails++ >= m_dwDetectAttempts)
	{
		m_ccContext.Reset(TRUE, SO_CLOSE, ERROR_CONNRESET);
		return FALSE;
	}

	return VERIFY(DetectConnection());
}

BOOL CUdpClient::DetectConnection()
{
	int rc = (int)send(m_soClient, nullptr, 0, 0);

	TRACE("<C-CNNID: %zu> send 0 bytes (detect package - %s)", m_dwConnID, IS_HAS_ERROR(rc) ? "fail" : "succ");

	return rc >= 0 || IS_WOULDBLOCK_ERROR();
}

BOOL CUdpClient::ProcessNetworkEvent(SHORT events)
{
	BOOL bContinue = TRUE;

	if(bContinue && events & POLLERR)
		bContinue = HandleClose(events);

	if(bContinue && !IsConnected())
		bContinue = HandleConnect(events);

	if(bContinue && events & POLLIN)
		bContinue = HandleRead(events);

	if(bContinue && events & POLLOUT)
		bContinue = HandleWrite(events);

	if(bContinue && events & _POLL_HUNGUP_EVENTS)
		bContinue = HandleClose(events);

	return bContinue;
}

BOOL CUdpClient::HandleConnect(SHORT events)
{
	ASSERT(events & POLLOUT);

	int code = ::SSO_GetError(m_soClient);

	if(!IS_NO_ERROR(code) || (events & _POLL_ERROR_EVENTS))
	{
		m_ccContext.Reset(TRUE, SO_CONNECT, code);
		return FALSE;
	}

	if(events & _POLL_HUNGUP_EVENTS)
	{
		m_ccContext.Reset(TRUE, SO_CONNECT, NO_ERROR);
		return FALSE;
	}

	SetConnected();

	if(TRIGGER(FireConnect()) != HR_ERROR)
		VERIFY(DetectConnection());
	else
	{
		m_ccContext.Reset(FALSE);
		return FALSE;
	}

	return TRUE;
}

BOOL CUdpClient::HandleClose(SHORT events)
{
	EnSocketOperation enOperation = SO_CLOSE;

	if(events & _POLL_HUNGUP_EVENTS)
		enOperation = SO_CLOSE;
	else if(events & POLLIN)
		enOperation = SO_RECEIVE;
	else if(events & POLLOUT)
		enOperation = SO_SEND;

	m_ccContext.Reset(TRUE, enOperation, ::SSO_GetError(m_soClient));

	return FALSE;
}

BOOL CUdpClient::HandleRead(SHORT events)
{
	return ReadData();
}

BOOL CUdpClient::HandleWrite(SHORT events)
{
	return SendData();
}

BOOL CUdpClient::ReadData()
{
	while(TRUE)
	{
		int rc = (int)recv(m_soClient, (char*)(BYTE*)m_rcBuffer, m_dwMaxDatagramSize, MSG_TRUNC);

		if(rc > 0)
		{
			if(rc > (int)m_dwMaxDatagramSize)
				continue;

			if(TRIGGER(FireReceive(m_rcBuffer, rc)) == HR_ERROR)
			{
				TRACE("<C-CNNID: %zu> OnReceive() event return 'HR_ERROR', connection will be closed !", m_dwConnID);

				m_ccContext.Reset(TRUE, SO_RECEIVE, ENSURE_ERROR_CANCELLED);
				return FALSE;
			}
		}
		else if(rc == SOCKET_ERROR)
		{
			int code = ::WSAGetLastError();

			if(code == ERROR_WOULDBLOCK)
				break;
			else
			{
				m_ccContext.Reset(TRUE, SO_RECEIVE, code);
				return FALSE;
			}
		}
		else if(rc == 0)
		{
			m_dwDetectFails = 0;
			TRACE("<C-CNNID: %zu> recv 0 bytes (detect ack package)", m_dwConnID);
		}
		else
			ASSERT(FALSE);
	}

	return TRUE;
}

BOOL CUdpClient::PauseReceive(BOOL bPause)
{
	if(!IsConnected())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	if(m_bPaused == bPause)
		return TRUE;

	m_bPaused = bPause;

	if(!bPause)
		return m_evRecv.Set();
	
	return TRUE;
}

BOOL CUdpClient::SendData()
{
	if(m_lsSend.IsEmpty())
		return TRUE;

	CCriSecLock locallock(m_csSend);

	if(m_lsSend.IsEmpty())
		return TRUE;

	BOOL isOK = TRUE;

	while(TRUE)
	{
		TItemPtr itPtr(m_itPool, m_lsSend.PopFront());

		if(!itPtr.IsValid())
			break;

		ASSERT(!itPtr->IsEmpty());

		isOK = DoSendData(itPtr);

		if(!isOK)
			break;

		if(!itPtr->IsEmpty())
		{
			m_lsSend.PushFront(itPtr.Detach());
			break;
		}
	}

	return isOK;
}

BOOL CUdpClient::DoSendData(TItem* pItem)
{
	int rc = (int)send(m_soClient, (char*)pItem->Ptr(), pItem->Size(), 0);

	if(rc > 0)
	{
		ASSERT(rc == pItem->Size());

		if(TRIGGER(FireSend(pItem->Ptr(), rc)) == HR_ERROR)
		{
			TRACE("<C-CNNID: %zu> OnSend() event should not return 'HR_ERROR' !!", m_dwConnID);
			ASSERT(FALSE);
		}

		pItem->Reduce(rc);
	}
	else if(rc == SOCKET_ERROR)
	{
		int code = ::WSAGetLastError();

		if(code != ERROR_WOULDBLOCK)
		{
			m_ccContext.Reset(TRUE, SO_SEND, code);
			return FALSE;
		}
	}
	else
		ASSERT(FALSE);

	return TRUE;
}

BOOL CUdpClient::Send(const BYTE* pBuffer, int iLength, int iOffset)
{
	ASSERT(pBuffer && iLength > 0 && iLength <= (int)m_dwMaxDatagramSize);

	int result = NO_ERROR;

	if(pBuffer && iLength > 0 && iLength <= (int)m_dwMaxDatagramSize)
	{
		if(IsConnected())
		{
			if(iOffset != 0) pBuffer += iOffset;

			TItemPtr itPtr(m_itPool, m_itPool.PickFreeItem());
			itPtr->Cat(pBuffer, iLength);

			result = SendInternal(itPtr);
		}
		else
			result = ERROR_INVALID_STATE;
	}
	else
		result = ERROR_INVALID_PARAMETER;

	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);
}

BOOL CUdpClient::SendPackets(const WSABUF pBuffers[], int iCount)
{
	ASSERT(pBuffers && iCount > 0);

	if(!pBuffers || iCount <= 0)
		return ERROR_INVALID_PARAMETER;
	if(!IsConnected())
		return ERROR_INVALID_STATE;

	int result = NO_ERROR;

	int iLength = 0;
	int iMaxLen = (int)m_dwMaxDatagramSize;

	TItemPtr itPtr(m_itPool, m_itPool.PickFreeItem());

	for(int i = 0; i < iCount; i++)
	{
		int iBufLen = pBuffers[i].len;

		if(iBufLen > 0)
		{
			BYTE* pBuffer = (BYTE*)pBuffers[i].buf;
			ASSERT(pBuffer);

			iLength += iBufLen;

			if(iLength <= iMaxLen)
				itPtr->Cat(pBuffer, iBufLen);
			else
				break;
		}
	}

	if(iLength > 0 && iLength <= iMaxLen)
		result = SendInternal(itPtr);
	else
		result = ERROR_INCORRECT_SIZE;

	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);
}

int CUdpClient::SendInternal(TItemPtr& itPtr)
{
	CCriSecLock locallock(m_csSend);

	if(!IsConnected())
		return ERROR_INVALID_STATE;

	BOOL isPending = !m_lsSend.IsEmpty();

	m_lsSend.PushBack(itPtr.Detach());

	if(!isPending) m_evSend.Set();

	return NO_ERROR;
}

void CUdpClient::SetLastError(EnSocketError code, LPCSTR func, int ec)
{
	TRACE("%s --> Error: %d, EC: %d", func, code, ec);

	m_enLastError = code;
	::SetLastError(ec);
}

BOOL CUdpClient::GetLocalAddress(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	return ::GetSocketLocalAddress(m_soClient, lpszAddress, iAddressLen, usPort);
}

void CUdpClient::SetRemoteHost(LPCTSTR lpszHost, USHORT usPort)
{
	m_strHost = lpszHost;
	m_usPort  = usPort;
}

BOOL CUdpClient::GetRemoteHost(TCHAR lpszHost[], int& iHostLen, USHORT& usPort)
{
	BOOL isOK = FALSE;

	if(m_strHost.IsEmpty())
		return isOK;

	int iLen = m_strHost.GetLength() + 1;

	if(iHostLen >= iLen)
	{
		memcpy(lpszHost, CA2CT(m_strHost), iLen * sizeof(TCHAR));
		usPort = m_usPort;

		isOK = TRUE;
	}

	iHostLen = iLen;

	return isOK;
}

BOOL CUdpClient::GetRemoteHost(LPCSTR* lpszHost, USHORT* pusPort)
{
	*lpszHost = m_strHost;

	if(pusPort != nullptr)
		*pusPort = m_usPort;

	return !m_strHost.IsEmpty();
}
