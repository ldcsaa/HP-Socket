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
 
#include "UdpCast.h"

BOOL CUdpCast::Start(LPCTSTR lpszRemoteAddress, USHORT usPort, BOOL bAsyncConnect, LPCTSTR lpszBindAddress, USHORT usLocalPort)
{
	ASSERT(usLocalPort == 0);

	if(!CheckParams() || !CheckStarting())
		return FALSE;

	PrepareStart();
	m_ccContext.Reset();

	BOOL isOK = FALSE;
	HP_SOCKADDR bindAddr(AF_UNSPEC, TRUE);

	if(CreateClientSocket(lpszRemoteAddress, usPort, lpszBindAddress, bindAddr))
	{
		if(BindClientSocket(bindAddr))
		{
			if(TRIGGER(FirePrepareConnect(m_soRecv)) != HR_ERROR)
			{
				if(ConnectToGroup(bindAddr))
				{
					if(CreateWorkerThread())
					{
							isOK = TRUE;
					}
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

BOOL CUdpCast::CheckParams()
{
	if	(((int)m_dwMaxDatagramSize > 0)									&&
		((int)m_dwFreeBufferPoolSize >= 0)								&&
		((int)m_dwFreeBufferPoolHold >= 0)								&&
		(m_enCastMode >= CM_MULTICAST && m_enCastMode <= CM_BROADCAST)	&&
		(m_iMCTtl >= 0 && m_iMCTtl <= 255)								)
		return TRUE;

	SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
	return FALSE;
}

void CUdpCast::PrepareStart()
{
	m_itPool.SetItemCapacity(m_dwMaxDatagramSize);
	m_itPool.SetPoolSize(m_dwFreeBufferPoolSize);
	m_itPool.SetPoolHold(m_dwFreeBufferPoolHold);

	m_itPool.Prepare();
}

BOOL CUdpCast::CheckStarting()
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

BOOL CUdpCast::CheckStoping()
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

BOOL CUdpCast::CreateClientSocket(LPCTSTR lpszRemoteAddress, USHORT usPort, LPCTSTR lpszBindAddress, HP_SOCKADDR& bindAddr)
{
	if(m_enCastMode == CM_MULTICAST)
	{
		if(!::GetSockAddrByHostName(lpszRemoteAddress, usPort, m_castAddr))
			return FALSE;
	}
	else
	{
		m_castAddr.family				 = AF_INET;
		m_castAddr.addr4.sin_addr.s_addr = INADDR_BROADCAST;
		m_castAddr.SetPort(usPort);
	}

	if(::IsStrEmpty(lpszBindAddress))
	{
		bindAddr.family = m_castAddr.family;
		bindAddr.SetPort(0);
	}
	else
	{
		if(!::sockaddr_A_2_IN(lpszBindAddress, 0, bindAddr))
			return FALSE;

		if(m_enCastMode == CM_BROADCAST && bindAddr.IsIPv6())
		{
			::WSASetLastError(ERROR_PFNOSUPPORT);
			return FALSE;
		}

		if(m_castAddr.family != bindAddr.family)
		{
			::WSASetLastError(ERROR_AFNOSUPPORT);
			return FALSE;
		}
	}

	m_soRecv = socket(m_castAddr.family, SOCK_DGRAM, IPPROTO_UDP);

	if(m_soRecv == INVALID_SOCKET)
		return FALSE;

	m_soSend = socket(m_castAddr.family, SOCK_DGRAM, IPPROTO_UDP);

	if(m_soSend == INVALID_SOCKET)
		return FALSE;

	VERIFY(::SSO_ReuseAddress(m_soRecv, m_bReuseAddress) != SOCKET_ERROR);

	SetRemoteHost(lpszRemoteAddress, usPort);

	return TRUE;
}

BOOL CUdpCast::BindClientSocket(HP_SOCKADDR& bindAddr)
{
	if(::bind(m_soRecv, m_castAddr.Addr(), m_castAddr.AddrSize()) == SOCKET_ERROR)
		return FALSE;
	if(::bind(m_soSend, bindAddr.Addr(), bindAddr.AddrSize()) == SOCKET_ERROR)
		return FALSE;

	m_dwConnID = ::GenerateConnectionID();

	return TRUE;
}

BOOL CUdpCast::ConnectToGroup(const HP_SOCKADDR& bindAddr)
{
	if(m_enCastMode == CM_MULTICAST)
	{
		if(!SetMultiCastSocketOptions(bindAddr))
			return FALSE;
	}
	else
	{
		ASSERT(m_castAddr.IsIPv4());

		UINT iSet = 1;
		VERIFY(::SSO_SetSocketOption(m_soSend, SOL_SOCKET, SO_BROADCAST, &iSet, sizeof(UINT)) != SOCKET_ERROR);
	}

	VERIFY(::fcntl_SETFL(m_soRecv, O_NOATIME | O_NONBLOCK | O_CLOEXEC));
	VERIFY(::fcntl_SETFL(m_soSend, O_NOATIME | O_NONBLOCK | O_CLOEXEC));

	SetConnected();

	if(TRIGGER(FireConnect()) == HR_ERROR)
	{
		::WSASetLastError(ENSURE_ERROR_CANCELLED);
		return FALSE;
	}

	m_nRecvEvents = (SHORT)((m_bPaused ? 0 : POLLIN) | POLLRDHUP);
	m_nSendEvents = (SHORT)((m_lsSend.IsEmpty() ? 0 : POLLOUT) | POLLRDHUP);

	return TRUE;
}

BOOL CUdpCast::SetMultiCastSocketOptions(const HP_SOCKADDR& bindAddr)
{
	if(m_castAddr.IsIPv4())
	{
		BYTE ttl  = (BYTE)m_iMCTtl;
		BYTE loop = (BYTE)m_bMCLoop;

		VERIFY(::SSO_SetSocketOption(m_soSend, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(BYTE)) != SOCKET_ERROR);
		VERIFY(::SSO_SetSocketOption(m_soSend, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(BYTE)) != SOCKET_ERROR);

		ip_mreq mcast;
		::ZeroMemory(&mcast, sizeof(mcast));

		mcast.imr_multiaddr = m_castAddr.addr4.sin_addr;
		mcast.imr_interface = bindAddr.addr4.sin_addr;

		if(::SSO_SetSocketOption(m_soRecv, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mcast, sizeof(ip_mreq)) == SOCKET_ERROR)
			return FALSE;
	}
	else
	{
		INT ttl	  = (INT)m_iMCTtl;
		UINT loop = (UINT)m_bMCLoop;

		VERIFY(::SSO_SetSocketOption(m_soSend, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &ttl, sizeof(INT)) != SOCKET_ERROR);
		VERIFY(::SSO_SetSocketOption(m_soSend, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &loop, sizeof(UINT)) != SOCKET_ERROR);

		ipv6_mreq mcast;
		::ZeroMemory(&mcast, sizeof(mcast));

		mcast.ipv6mr_multiaddr = m_castAddr.addr6.sin6_addr;
		mcast.ipv6mr_interface = bindAddr.addr6.sin6_scope_id;

		if(::SSO_SetSocketOption(m_soRecv, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mcast, sizeof(ipv6_mreq)) == SOCKET_ERROR)
			return FALSE;
	}

	return TRUE;
}

BOOL CUdpCast::Stop()
{
	if(!CheckStoping())
		return FALSE;

	SetConnected(FALSE);

	WaitForWorkerThreadEnd();

	if(m_ccContext.bFireOnClose)
		FireClose(m_ccContext.enOperation, m_ccContext.iErrorCode);

	if(m_soRecv != INVALID_SOCKET)
	{
		shutdown(m_soRecv, SHUT_WR);
		closesocket(m_soRecv);

		m_soRecv = INVALID_SOCKET;
	}

	if(m_soSend != INVALID_SOCKET)
	{
		shutdown(m_soSend, SHUT_WR);
		closesocket(m_soSend);

		m_soSend = INVALID_SOCKET;
	}

	Reset();

	return TRUE;
}

void CUdpCast::Reset()
{
	CCriSecLock locallock(m_csSend);

	m_evSend.Reset();
	m_evRecv.Reset();
	m_evStop.Reset();

	m_lsSend.Clear();
	m_itPool.Clear();
	m_rcBuffer.Free();

	m_castAddr.Reset();
	m_remoteAddr.Reset();

	m_strHost.Empty();

	m_usPort		= 0;
	m_nRecvEvents	= 0;
	m_nSendEvents	= 0;
	m_bPaused		= FALSE;
	m_enState		= SS_STOPPED;
}

void CUdpCast::WaitForWorkerThreadEnd()
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

BOOL CUdpCast::CreateWorkerThread()
{
	return m_thWorker.Start(this, &CUdpCast::WorkerThreadProc);
}

UINT WINAPI CUdpCast::WorkerThreadProc(LPVOID pv)
{
	TRACE("---------------> Cast Worker Thread 0x%08X started <---------------", SELF_THREAD_ID);

	BOOL bCallStop	= TRUE;
	pollfd pfds[]	= {	{m_soRecv, m_nRecvEvents},
						{m_soSend, m_nSendEvents},
						{m_evSend.GetFD(), POLLIN},
						{m_evRecv.GetFD(), POLLIN},
						{m_evStop.GetFD(), POLLIN}	};
	int size		= (int)(sizeof(pfds) / sizeof(pfds[0]));

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

				if(i == 0 || i == 1)
				{
					if(!ProcessNetworkEvent(revents))
						goto EXIT_WORKER_THREAD;
				}
				else if(i == 2)
				{
					m_evSend.Reset();

					if(!SendData())
						goto EXIT_WORKER_THREAD;
				}
				else if(i == 3)
				{
					m_evRecv.Reset();

					if(!ReadData())
						goto EXIT_WORKER_THREAD;
				}
				else if(i == 4)
				{
					m_evStop.Reset();

					bCallStop = FALSE;
					goto EXIT_WORKER_THREAD;
				}
				else
					VERIFY(FALSE);
			}
		}

		m_nRecvEvents = (SHORT)((m_bPaused ? 0 : POLLIN) | POLLRDHUP);
		m_nSendEvents = (SHORT)((m_lsSend.IsEmpty() ? 0 : POLLOUT) | POLLRDHUP);

		pfds[0].events = m_nRecvEvents;
		pfds[1].events = m_nSendEvents;
	}

EXIT_WORKER_THREAD:

	OnWorkerThreadEnd(SELF_THREAD_ID);

	if(bCallStop && HasStarted())
		Stop();

	TRACE("---------------> Cast Worker Thread 0x%08X stoped <---------------", SELF_THREAD_ID);

	return 0;
}

BOOL CUdpCast::ProcessNetworkEvent(SHORT events)
{
	ASSERT(IsConnected());

	BOOL bContinue = TRUE;

	if(bContinue && events & POLLERR)
		bContinue = HandleClose(events);

	if(bContinue && events & POLLIN)
		bContinue = HandleRead(events);

	if(bContinue && events & POLLOUT)
		bContinue = HandleWrite(events);

	if(bContinue && events & _POLL_HUNGUP_EVENTS)
		bContinue = HandleClose(events);

	return bContinue;
}

BOOL CUdpCast::HandleClose(SHORT events)
{
	EnSocketOperation enOperation = SO_CLOSE;

	if(events & _POLL_HUNGUP_EVENTS)
		enOperation = SO_CLOSE;
	else if(events & POLLIN)
		enOperation = SO_RECEIVE;
	else if(events & POLLOUT)
		enOperation = SO_SEND;

	m_ccContext.Reset(TRUE, enOperation, ::SSO_GetError(m_soRecv));

	return FALSE;
}

BOOL CUdpCast::HandleRead(SHORT events)
{
	return ReadData();
}

BOOL CUdpCast::HandleWrite(SHORT events)
{
	return SendData();
}

BOOL CUdpCast::ReadData()
{
	while(TRUE)
	{
		socklen_t addrLen = (socklen_t)m_remoteAddr.AddrSize();
		int rc			  = (int)recvfrom(m_soRecv, (char*)(BYTE*)m_rcBuffer, m_dwMaxDatagramSize, MSG_TRUNC, m_remoteAddr.Addr(), &addrLen);

		if(rc >= 0)
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
		else
			ASSERT(FALSE);
	}

	return TRUE;
}

BOOL CUdpCast::PauseReceive(BOOL bPause)
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

BOOL CUdpCast::SendData()
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

BOOL CUdpCast::DoSendData(TItem* pItem)
{
	int rc = (int)sendto(m_soSend, (char*)pItem->Ptr(), pItem->Size(), 0, m_castAddr.Addr(), m_castAddr.AddrSize());

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

BOOL CUdpCast::Send(const BYTE* pBuffer, int iLength, int iOffset)
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

BOOL CUdpCast::SendPackets(const WSABUF pBuffers[], int iCount)
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

int CUdpCast::SendInternal(TItemPtr& itPtr)
{
	CCriSecLock locallock(m_csSend);

	if(!IsConnected())
		return ERROR_INVALID_STATE;

	BOOL isPending = !m_lsSend.IsEmpty();

	m_lsSend.PushBack(itPtr.Detach());

	if(!isPending) m_evSend.Set();

	return NO_ERROR;
}

void CUdpCast::SetLastError(EnSocketError code, LPCSTR func, int ec)
{
	TRACE("%s --> Error: %d, EC: %d", func, code, ec);

	m_enLastError = code;
	::SetLastError(ec);
}

BOOL CUdpCast::GetLocalAddress(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	return ::GetSocketLocalAddress(m_soSend, lpszAddress, iAddressLen, usPort);
}

void CUdpCast::SetRemoteHost(LPCTSTR lpszHost, USHORT usPort)
{
	m_strHost = lpszHost;
	m_usPort  = usPort;
}

BOOL CUdpCast::GetRemoteHost(TCHAR lpszHost[], int& iHostLen, USHORT& usPort)
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


BOOL CUdpCast::GetRemoteHost(LPCSTR* lpszHost, USHORT* pusPort)
{
	*lpszHost = m_strHost;

	if(pusPort != nullptr)
		*pusPort = m_usPort;

	return !m_strHost.IsEmpty();
}
