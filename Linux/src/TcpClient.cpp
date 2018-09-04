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
 
#include "TcpClient.h"

#include "./common/FileHelper.h"

BOOL CTcpClient::Start(LPCTSTR lpszRemoteAddress, USHORT usPort, BOOL bAsyncConnect, LPCTSTR lpszBindAddress, USHORT usLocalPort)
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

BOOL CTcpClient::CheckParams()
{
	if	(((int)m_dwSocketBufferSize > 0)									&&
		((int)m_dwFreeBufferPoolSize >= 0)									&&
		((int)m_dwFreeBufferPoolHold >= 0)									&&
		((int)m_dwKeepAliveTime >= 1000 || m_dwKeepAliveTime == 0)			&&
		((int)m_dwKeepAliveInterval >= 1000 || m_dwKeepAliveInterval == 0)	)
		return TRUE;

	SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
	return FALSE;
}

void CTcpClient::PrepareStart()
{
	m_itPool.SetItemCapacity(m_dwSocketBufferSize);
	m_itPool.SetPoolSize(m_dwFreeBufferPoolSize);
	m_itPool.SetPoolHold(m_dwFreeBufferPoolHold);

	m_itPool.Prepare();
}

BOOL CTcpClient::CheckStarting()
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

BOOL CTcpClient::CheckStoping()
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

BOOL CTcpClient::CreateClientSocket(LPCTSTR lpszRemoteAddress, HP_SOCKADDR& addrRemote, USHORT usPort, LPCTSTR lpszBindAddress, HP_SOCKADDR& addrBind)
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

	m_soClient = socket(addrRemote.family, SOCK_STREAM, IPPROTO_TCP);

	if(m_soClient == INVALID_SOCKET)
		return FALSE;

	BOOL bOnOff	= (m_dwKeepAliveTime > 0 && m_dwKeepAliveInterval > 0);
	VERIFY(::SSO_KeepAliveVals(m_soClient, bOnOff, m_dwKeepAliveTime, m_dwKeepAliveInterval) == NO_ERROR);
	VERIFY(::SSO_ReuseAddress(m_soClient, TRUE) == NO_ERROR);

	SetRemoteHost(lpszRemoteAddress, usPort);

	return TRUE;
}

BOOL CTcpClient::BindClientSocket(const HP_SOCKADDR& addrBind, const HP_SOCKADDR& addrRemote, USHORT usLocalPort)
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

BOOL CTcpClient::ConnectToServer(const HP_SOCKADDR& addrRemote, BOOL bAsyncConnect)
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
				m_nEvents = (SHORT)((m_lsSend.IsEmpty() ? 0 : POLLOUT) | (m_bPaused ? 0 : POLLIN) | POLLRDHUP);				
				isOK	  = TRUE;
			}
		}
	}

	return isOK;
}

BOOL CTcpClient::Stop()
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

void CTcpClient::Reset()
{
	CCriSecLock locallock(m_csSend);

	m_evSend.Reset();
	m_evRecv.Reset();
	m_evStop.Reset();

	m_lsSend.Clear();
	m_itPool.Clear();
	m_rcBuffer.Free();

	m_strHost.Empty();

	m_usPort	= 0;
	m_nEvents	= 0;
	m_bPaused	= FALSE;
	m_enState	= SS_STOPPED;
}

void CTcpClient::WaitForWorkerThreadEnd()
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

BOOL CTcpClient::CreateWorkerThread()
{
	return m_thWorker.Start(this, &CTcpClient::WorkerThreadProc);
}

UINT WINAPI CTcpClient::WorkerThreadProc(LPVOID pv)
{
	TRACE("---------------> Client Worker Thread 0x%08X started <---------------", SELF_THREAD_ID);

	BOOL bCallStop	= TRUE;
	pollfd pfds[]	= {	{m_soClient, m_nEvents}, 
						{m_evSend.GetFD(), POLLIN}, 
						{m_evRecv.GetFD(), POLLIN}, 
						{m_evStop.GetFD(), POLLIN}	};
	int size		= (int)(sizeof(pfds) / sizeof(pfds[0]));

	m_rcBuffer.Malloc(m_dwSocketBufferSize);

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

BOOL CTcpClient::ProcessNetworkEvent(SHORT events)
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

BOOL CTcpClient::HandleConnect(SHORT events)
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

	if(TRIGGER(FireConnect()) == HR_ERROR)
	{
		m_ccContext.Reset(FALSE);
		return FALSE;
	}

	return TRUE;
}

BOOL CTcpClient::HandleClose(SHORT events)
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

BOOL CTcpClient::HandleRead(SHORT events)
{
	return ReadData();
}

BOOL CTcpClient::HandleWrite(SHORT events)
{
	return SendData();
}

BOOL CTcpClient::ReadData()
{
	while(TRUE)
	{
		int rc = (int)read(m_soClient, (char*)(BYTE*)m_rcBuffer, m_dwSocketBufferSize);

		if(rc > 0)
		{
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
			m_ccContext.Reset(TRUE, SO_CLOSE, SE_OK);
			return FALSE;
		}
		else
			ASSERT(FALSE);
	}

	return TRUE;
}

BOOL CTcpClient::PauseReceive(BOOL bPause)
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

BOOL CTcpClient::SendData()
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

BOOL CTcpClient::DoSendData(TItem* pItem)
{
	while(!pItem->IsEmpty())
	{
		int rc = (int)write(m_soClient, (char*)pItem->Ptr(), pItem->Size());

		if(rc > 0)
		{
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

			if(code == ERROR_WOULDBLOCK)
				break;
			else
			{
				m_ccContext.Reset(TRUE, SO_SEND, code);
				return FALSE;
			}
		}
		else
			ASSERT(FALSE);
	}

	return TRUE;
}

BOOL CTcpClient::Send(const BYTE* pBuffer, int iLength, int iOffset)
{
	ASSERT(pBuffer && iLength > 0);

	if(iOffset != 0) pBuffer += iOffset;

	WSABUF buffer;
	buffer.len = iLength;
	buffer.buf = (BYTE*)pBuffer;

	return SendPackets(&buffer, 1);
}

BOOL CTcpClient::DoSendPackets(const WSABUF pBuffers[], int iCount)
{
	ASSERT(pBuffers && iCount > 0);

	int result = NO_ERROR;

	if(pBuffers && iCount > 0)
	{
		if(IsConnected())
		{
			CCriSecLock locallock(m_csSend);

			if(IsConnected())
				result = SendInternal(pBuffers, iCount);
			else
				result = ERROR_INVALID_STATE;
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

int CTcpClient::SendInternal(const WSABUF pBuffers[], int iCount)
{
	int iPending = m_lsSend.Length();

	for(int i = 0; i < iCount; i++)
	{
		int iBufLen = pBuffers[i].len;

		if(iBufLen > 0)
		{
			BYTE* pBuffer = (BYTE*)pBuffers[i].buf;
			ASSERT(pBuffer);

			m_lsSend.Cat(pBuffer, iBufLen);
		}
	}

	if(iPending == 0 && m_lsSend.Length() > 0)
		m_evSend.Set();

	return NO_ERROR;
}

BOOL CTcpClient::SendSmallFile(LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail)
{
	CFile file;
	CFileMapping fmap;
	WSABUF szBuf[3];

	HRESULT hr = ::MakeSmallFilePackage(lpszFileName, file, fmap, szBuf, pHead, pTail);

	if(FAILED(hr))
	{
		::SetLastError(hr);
		return FALSE;
	}

	return SendPackets(szBuf, 3);
}

void CTcpClient::SetLastError(EnSocketError code, LPCSTR func, int ec)
{
	TRACE("%s --> Error: %d, EC: %d", func, code, ec);

	m_enLastError = code;
	::SetLastError(ec);
}

BOOL CTcpClient::GetLocalAddress(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	return ::GetSocketLocalAddress(m_soClient, lpszAddress, iAddressLen, usPort);
}

void CTcpClient::SetRemoteHost(LPCTSTR lpszHost, USHORT usPort)
{
	m_strHost = lpszHost;
	m_usPort  = usPort;
}

BOOL CTcpClient::GetRemoteHost(TCHAR lpszHost[], int& iHostLen, USHORT& usPort)
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

BOOL CTcpClient::GetRemoteHost(LPCSTR* lpszHost, USHORT* pusPort)
{
	*lpszHost = m_strHost;

	if(pusPort != nullptr)
		*pusPort = m_usPort;

	return !m_strHost.IsEmpty();
}
