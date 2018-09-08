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
#include "UdpCast.h"
#include "../Common/Src/WaitFor.h"

#include <process.h>

const CInitSocket CUdpCast::sm_wsSocket;

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
			if(TRIGGER(FirePrepareConnect(m_soClient)) != HR_ERROR)
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
		(m_iMCTtl >= 0 && m_iMCTtl <= 255)								&&
		(m_bMCLoop >= 0 && m_bMCLoop <= 1)								)
		return TRUE;

	SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
	return FALSE;
}

void CUdpCast::PrepareStart()
{
	m_itPool.SetItemCapacity((int)m_dwMaxDatagramSize);
	m_itPool.SetPoolSize((int)m_dwFreeBufferPoolSize);
	m_itPool.SetPoolHold((int)m_dwFreeBufferPoolHold);

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

BOOL CUdpCast::CheckStoping(DWORD dwCurrentThreadID)
{
	if(m_enState != SS_STOPPED)
	{
		CSpinLock locallock(m_csState);

		if(HasStarted())
		{
			m_enState = SS_STOPPING;
			return TRUE;
		}

		if(dwCurrentThreadID != m_dwWorkerID)
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

	if(!lpszBindAddress || lpszBindAddress[0] == 0)
	{
		bindAddr.family = m_castAddr.family;
		bindAddr.SetPort(usPort);
	}
	else
	{
		if(!::sockaddr_A_2_IN(lpszBindAddress, usPort, bindAddr))
			return FALSE;

		if(m_enCastMode == CM_BROADCAST && bindAddr.IsIPv6())
		{
			::WSASetLastError(WSAEPROTONOSUPPORT);
			return FALSE;
		}

		if(m_castAddr.family != bindAddr.family)
		{
			::WSASetLastError(WSAEAFNOSUPPORT);
			return FALSE;
		}
	}

	m_soClient = socket(m_castAddr.family, SOCK_DGRAM, IPPROTO_UDP);

	if(m_soClient == INVALID_SOCKET)
		return FALSE;

	ENSURE(::SSO_UDP_ConnReset(m_soClient, FALSE) == NO_ERROR);
	ENSURE(::SSO_ReuseAddress(m_soClient, m_bReuseAddress) != SOCKET_ERROR);

	m_evSocket = ::WSACreateEvent();
	ASSERT(m_evSocket != WSA_INVALID_EVENT);

	SetRemoteHost(lpszRemoteAddress, usPort);

	return TRUE;
}

BOOL CUdpCast::BindClientSocket(HP_SOCKADDR& bindAddr)
{
	if(::bind(m_soClient, bindAddr.Addr(), bindAddr.AddrSize()) == SOCKET_ERROR)
		return FALSE;

	int addr_len = bindAddr.AddrSize();
	ENSURE(::getsockname(m_soClient, bindAddr.Addr(), &addr_len) == NO_ERROR);

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

		BOOL bSet = TRUE;
		ENSURE(::SSO_SetSocketOption(m_soClient, SOL_SOCKET, SO_BROADCAST, &bSet, sizeof(BOOL)) != SOCKET_ERROR);
	}

	BOOL isOK = FALSE;

	if(::WSAEventSelect(m_soClient, m_evSocket, FD_READ | FD_WRITE | FD_CLOSE) != SOCKET_ERROR)
	{
		SetConnected();

		if(TRIGGER(FireConnect()) == HR_ERROR)
			::WSASetLastError(ENSURE_ERROR_CANCELLED);
		else
			isOK = TRUE;
	}

	return isOK;
}

BOOL CUdpCast::SetMultiCastSocketOptions(const HP_SOCKADDR& bindAddr)
{
	if(m_castAddr.IsIPv4())
	{
		ENSURE(::SSO_SetSocketOption(m_soClient, IPPROTO_IP, IP_MULTICAST_TTL, &m_iMCTtl, sizeof(int)) != SOCKET_ERROR);
		ENSURE(::SSO_SetSocketOption(m_soClient, IPPROTO_IP, IP_MULTICAST_LOOP, &m_bMCLoop, sizeof(BOOL)) != SOCKET_ERROR);

		ip_mreq mcast;
		::ZeroMemory(&mcast, sizeof(mcast));

		mcast.imr_multiaddr = m_castAddr.addr4.sin_addr;
		mcast.imr_interface = bindAddr.addr4.sin_addr;

		if(::SSO_SetSocketOption(m_soClient, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mcast, sizeof(ip_mreq)) == SOCKET_ERROR)
			return FALSE;
	}
	else
	{
		ENSURE(::SSO_SetSocketOption(m_soClient, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &m_iMCTtl, sizeof(int)) != SOCKET_ERROR);
		ENSURE(::SSO_SetSocketOption(m_soClient, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &m_bMCLoop, sizeof(BOOL)) != SOCKET_ERROR);

		ipv6_mreq mcast;
		::ZeroMemory(&mcast, sizeof(mcast));

		mcast.ipv6mr_multiaddr = m_castAddr.addr6.sin6_addr;
		mcast.ipv6mr_interface = bindAddr.addr6.sin6_scope_id;

		if(::SSO_SetSocketOption(m_soClient, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mcast, sizeof(ipv6_mreq)) == SOCKET_ERROR)
			return FALSE;
	}

	return TRUE;
}

BOOL CUdpCast::CreateWorkerThread()
{
	m_hWorker = (HANDLE)_beginthreadex(nullptr, 0, WorkerThreadProc, (LPVOID)this, 0, &m_dwWorkerID);

	return m_hWorker != nullptr;
}

UINT WINAPI CUdpCast::WorkerThreadProc(LPVOID pv)
{
	TRACE("---------------> Client Worker Thread 0x%08X started <---------------\n", ::GetCurrentThreadId());

	BOOL bCallStop		= TRUE;
	CUdpCast* pClient	= (CUdpCast*)pv;
	HANDLE hEvents[]	= {pClient->m_evSocket, pClient->m_evBuffer, pClient->m_evWorker};

	pClient->m_rcBuffer.Malloc(pClient->m_dwMaxDatagramSize);

	while(pClient->HasStarted())
	{
		DWORD retval = ::WSAWaitForMultipleEvents(3, hEvents, FALSE, WSA_INFINITE, FALSE);

		if(retval == WSA_WAIT_EVENT_0)
		{
			if(!pClient->ProcessNetworkEvent())
				break;
		}
		else if(retval == WSA_WAIT_EVENT_0 + 1)
		{
			if(!pClient->SendData())
				break;
		}
		else if(retval == WSA_WAIT_EVENT_0 + 2)
		{
			bCallStop = FALSE;
			break;
		}
		else if(retval == WSA_WAIT_FAILED)
		{
			pClient->m_ccContext.Reset(TRUE, SO_UNKNOWN, ::WSAGetLastError());
			break;
		}
		else
			ENSURE(FALSE);
	}

	pClient->OnWorkerThreadEnd(::GetCurrentThreadId());

	if(bCallStop && pClient->HasStarted())
		pClient->Stop();

	TRACE("---------------> Client Worker Thread 0x%08X stoped <---------------\n", ::GetCurrentThreadId());

	return 0;
}

BOOL CUdpCast::ProcessNetworkEvent()
{
	BOOL bContinue = TRUE;
	WSANETWORKEVENTS events;
	
	int rc = ::WSAEnumNetworkEvents(m_soClient, m_evSocket, &events);

	if(rc == SOCKET_ERROR)
		bContinue = HandleError(events);

	if(bContinue && events.lNetworkEvents & FD_READ)
		bContinue = HandleRead(events);

	if(bContinue && events.lNetworkEvents & FD_WRITE)
		bContinue = HandleWrite(events);

	if(bContinue && events.lNetworkEvents & FD_CLOSE)
		bContinue = HandleClose(events);

	return bContinue;
}

BOOL CUdpCast::HandleError(WSANETWORKEVENTS& events)
{
	int iCode						= ::WSAGetLastError();
	EnSocketOperation enOperation	= SO_UNKNOWN;

	if(events.lNetworkEvents & FD_CLOSE)
		enOperation = SO_CLOSE;
	else if(events.lNetworkEvents & FD_READ)
		enOperation = SO_RECEIVE;
	else if(events.lNetworkEvents & FD_WRITE)
		enOperation = SO_SEND;

	ENSURE(::WSAResetEvent(m_evSocket));
	m_ccContext.Reset(TRUE, enOperation, iCode);

	return FALSE;
}

BOOL CUdpCast::HandleRead(WSANETWORKEVENTS& events)
{
	BOOL bContinue	= TRUE;
	int iCode		= events.iErrorCode[FD_READ_BIT];

	if(iCode == 0)
		bContinue = ReadData();
	else
	{
		m_ccContext.Reset(TRUE, SO_RECEIVE, iCode);
		bContinue = FALSE;
	}

	return bContinue;
}

BOOL CUdpCast::HandleWrite(WSANETWORKEVENTS& events)
{
	BOOL bContinue	= TRUE;
	int iCode		= events.iErrorCode[FD_WRITE_BIT];

	if(iCode == 0)
		bContinue = SendData();
	else
	{
		m_ccContext.Reset(TRUE, SO_SEND, iCode);
		bContinue = FALSE;
	}

	return bContinue;
}

BOOL CUdpCast::HandleClose(WSANETWORKEVENTS& events)
{
	int iCode = events.iErrorCode[FD_CLOSE_BIT];

	if(iCode == 0)
		m_ccContext.Reset(TRUE, SO_CLOSE, SE_OK);
	else
		m_ccContext.Reset(TRUE, SO_CLOSE, iCode);

	return FALSE;
}

BOOL CUdpCast::ReadData()
{
	while(TRUE)
	{
		int addrLen	= m_remoteAddr.AddrSize();
		int rc		= recvfrom(m_soClient, (char*)(BYTE*)m_rcBuffer, m_dwMaxDatagramSize, 0, m_remoteAddr.Addr(), &addrLen);

		if(rc >= 0)
		{
			if(TRIGGER(FireReceive(m_rcBuffer, rc)) == HR_ERROR)
			{
				TRACE("<C-CNNID: %Iu> OnReceive() event return 'HR_ERROR', connection will be closed !\n", m_dwConnID);

				m_ccContext.Reset(TRUE, SO_RECEIVE, ENSURE_ERROR_CANCELLED);
				return FALSE;
			}
		}
		else if(rc == SOCKET_ERROR)
		{
			int code = ::WSAGetLastError();

			if(code == WSAEWOULDBLOCK)
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

	if(::WSAEventSelect(m_soClient, m_evSocket, bPause ? 0 : FD_READ | FD_WRITE | FD_CLOSE) != SOCKET_ERROR)
	{
		m_bPaused = bPause;
		return TRUE;
	}

	return FALSE;
}

BOOL CUdpCast::SendData()
{
	while(TRUE)
	{
		TItemPtr itPtr(m_itPool, GetSendBuffer());

		if(itPtr.IsValid())
		{
			ASSERT(!itPtr->IsEmpty());

			int rc = 0;

			{
				CCriSecLock locallock(m_csSend);

				rc = sendto(m_soClient, (char*)itPtr->Ptr(), itPtr->Size(), 0, m_castAddr.Addr(), m_castAddr.AddrSize());
				if(rc > 0) m_iPending -= rc;
			}

			if(rc > 0)
			{
				ASSERT(rc == itPtr->Size());

				if(TRIGGER(FireSend(itPtr->Ptr(), rc)) == HR_ERROR)
				{
					TRACE("<C-CNNID: %Iu> OnSend() event should not return 'HR_ERROR' !!\n", m_dwConnID);
					ASSERT(FALSE);
				}
			}
			else if(rc == SOCKET_ERROR)
			{
				int iCode = ::WSAGetLastError();

				if(iCode == WSAEWOULDBLOCK)
				{
					CCriSecLock locallock(m_csSend);
					m_lsSend.PushFront(itPtr.Detach());
					break;
				}
				else
				{
					m_ccContext.Reset(TRUE, SO_SEND, iCode);
					return FALSE;
				}
			}
			else
				ASSERT(FALSE);
		}
		else
			break;
	}

	return TRUE;
}

TItem* CUdpCast::GetSendBuffer()
{
	TItem* pItem = nullptr;

	if(m_lsSend.Size() > 0)
	{
		CCriSecLock locallock(m_csSend);

		if(m_lsSend.Size() > 0)
			pItem = m_lsSend.PopFront();
	}

	return pItem;
}

BOOL CUdpCast::Stop()
{
	DWORD dwCurrentThreadID = ::GetCurrentThreadId();

	if(!CheckStoping(dwCurrentThreadID))
		return FALSE;

	SetConnected(FALSE);

	WaitForWorkerThreadEnd(dwCurrentThreadID);

	if(m_ccContext.bFireOnClose)
		FireClose(m_ccContext.enOperation, m_ccContext.iErrorCode);

	if(m_evSocket != nullptr)
	{
		::WSACloseEvent(m_evSocket);
		m_evSocket	= nullptr;
	}

	if(m_soClient != INVALID_SOCKET)
	{
		shutdown(m_soClient, SD_SEND);
		closesocket(m_soClient);
		m_soClient	= INVALID_SOCKET;
	}

	Reset();

	return TRUE;
}

void CUdpCast::Reset()
{
	CCriSecLock locallock(m_csSend);

	m_rcBuffer.Free();
	m_evBuffer.Reset();
	m_evWorker.Reset();
	m_lsSend.Clear();
	m_itPool.Clear();

	m_castAddr.Reset();
	m_remoteAddr.Reset();

	m_strHost.Empty();

	m_usPort	= 0;
	m_iPending	= 0;
	m_bPaused	= FALSE;
	m_enState	= SS_STOPPED;
}

void CUdpCast::WaitForWorkerThreadEnd(DWORD dwCurrentThreadID)
{
	if(m_hWorker != nullptr)
	{
		if(dwCurrentThreadID != m_dwWorkerID)
		{
			m_evWorker.Set();
			ENSURE(::WaitForSingleObject(m_hWorker, INFINITE) == WAIT_OBJECT_0);
		}

		::CloseHandle(m_hWorker);

		m_hWorker		= nullptr;
		m_dwWorkerID	= 0;
	}
}

BOOL CUdpCast::Send(const BYTE* pBuffer, int iLength, int iOffset)
{
	int result = NO_ERROR;

	ASSERT(pBuffer && iLength > 0 && iLength <= (int)m_dwMaxDatagramSize);

	if(pBuffer && iLength > 0 && iLength <= (int)m_dwMaxDatagramSize)
	{
		if(iOffset != 0) pBuffer += iOffset;
		result = SendInternal(pBuffer, iLength);
	}
	else
		result = ERROR_INVALID_PARAMETER;

	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);
}

BOOL CUdpCast::SendPackets(const WSABUF pBuffers[], int iCount)
{
	int result = NO_ERROR;

	ASSERT(pBuffers && iCount > 0);

	if(pBuffers && iCount > 0)
	{
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
			result = SendInternal(itPtr->Ptr(), iLength);
		else
			result = ERROR_INCORRECT_SIZE;
	}
	else
		result = ERROR_INVALID_PARAMETER;

	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);
}

int CUdpCast::SendInternal(const BYTE* pBuffer, int iLength)
{
	int result = NO_ERROR;

	if(IsConnected())
	{
		CCriSecLock locallock(m_csSend);

		if(IsConnected())
		{
			ASSERT(m_iPending >= 0);

			BOOL isPending = m_iPending > 0;

			TItem* pItem = m_itPool.PickFreeItem();
			pItem->Cat(pBuffer, iLength);
			m_lsSend.PushBack(pItem);

			m_iPending += iLength;

			if(!isPending) m_evBuffer.Set();
		}
		else
			result = ERROR_INVALID_STATE;
	}
	else
		result = ERROR_INVALID_STATE;

	return result;
}

void CUdpCast::SetLastError(EnSocketError code, LPCSTR func, int ec)
{
	TRACE("%s --> Error: %d, EC: %d\n", func, code, ec);

	m_enLastError = code;
	::SetLastError(ec);
}

BOOL CUdpCast::GetLocalAddress(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	return ::GetSocketLocalAddress(m_soClient, lpszAddress, iAddressLen, usPort);
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
