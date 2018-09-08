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
#include "TcpClient.h"
#include "../Common/Src/WaitFor.h"

#include <process.h>

const CInitSocket CTcpClient::sm_wsSocket;

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
	m_itPool.SetItemCapacity((int)m_dwSocketBufferSize);
	m_itPool.SetPoolSize((int)m_dwFreeBufferPoolSize);
	m_itPool.SetPoolHold((int)m_dwFreeBufferPoolHold);

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

BOOL CTcpClient::CheckStoping(DWORD dwCurrentThreadID)
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

BOOL CTcpClient::CreateClientSocket(LPCTSTR lpszRemoteAddress, HP_SOCKADDR& addrRemote, USHORT usPort, LPCTSTR lpszBindAddress, HP_SOCKADDR& addrBind)
{
	if(!::GetSockAddrByHostName(lpszRemoteAddress, usPort, addrRemote))
		return FALSE;

	if(lpszBindAddress && lpszBindAddress[0] != 0)
	{
		if(!::sockaddr_A_2_IN(lpszBindAddress, 0, addrBind))
			return FALSE;

		if(addrRemote.family != addrBind.family)
		{
			::WSASetLastError(WSAEAFNOSUPPORT);
			return FALSE;
		}
	}

	m_soClient = socket(addrRemote.family, SOCK_STREAM, IPPROTO_TCP);

	if(m_soClient == INVALID_SOCKET)
		return FALSE;

	BOOL bOnOff	= (m_dwKeepAliveTime > 0 && m_dwKeepAliveInterval > 0);
	ENSURE(::SSO_KeepAliveVals(m_soClient, bOnOff, m_dwKeepAliveTime, m_dwKeepAliveInterval) == NO_ERROR);
	ENSURE(::SSO_ReuseAddress(m_soClient, TRUE) == NO_ERROR);

	m_evSocket = ::WSACreateEvent();
	ASSERT(m_evSocket != WSA_INVALID_EVENT);

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
		if(::WSAEventSelect(m_soClient, m_evSocket, FD_CONNECT | FD_CLOSE) != SOCKET_ERROR)
		{
			int rc = ::connect(m_soClient, addrRemote.Addr(), addrRemote.AddrSize());
			isOK = (rc == NO_ERROR || (rc == SOCKET_ERROR && ::WSAGetLastError() == WSAEWOULDBLOCK));
		}
	}
	else
	{
		if(::connect(m_soClient, addrRemote.Addr(), addrRemote.AddrSize()) != SOCKET_ERROR)
		{
			if(::WSAEventSelect(m_soClient, m_evSocket, FD_READ | FD_WRITE | FD_CLOSE) != SOCKET_ERROR)
			{
				SetConnected();

				if(TRIGGER(FireConnect()) == HR_ERROR)
					::WSASetLastError(ENSURE_ERROR_CANCELLED);
				else
					isOK = TRUE;
			}
		}
	}

	return isOK;
}

BOOL CTcpClient::CreateWorkerThread()
{
	m_hWorker = (HANDLE)_beginthreadex(nullptr, 0, WorkerThreadProc, (LPVOID)this, 0, &m_dwWorkerID);

	return m_hWorker != nullptr;
}

UINT WINAPI CTcpClient::WorkerThreadProc(LPVOID pv)
{
	TRACE("---------------> Client Worker Thread 0x%08X started <---------------\n", ::GetCurrentThreadId());

	BOOL bCallStop		= TRUE;
	CTcpClient* pClient	= (CTcpClient*)pv;
	HANDLE hEvents[]	= {pClient->m_evSocket, pClient->m_evBuffer, pClient->m_evWorker};

	pClient->m_rcBuffer.Malloc(pClient->m_dwSocketBufferSize);

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

BOOL CTcpClient::ProcessNetworkEvent()
{
	BOOL bContinue = TRUE;
	WSANETWORKEVENTS events;
	
	int rc = ::WSAEnumNetworkEvents(m_soClient, m_evSocket, &events);

	if(rc == SOCKET_ERROR)
		bContinue = HandleError(events);

	if(!IsConnected() && bContinue && events.lNetworkEvents & FD_CONNECT)
		bContinue = HandleConnect(events);

	if(bContinue && events.lNetworkEvents & FD_READ)
		bContinue = HandleRead(events);

	if(bContinue && events.lNetworkEvents & FD_WRITE)
		bContinue = HandleWrite(events);

	if(bContinue && events.lNetworkEvents & FD_CLOSE)
		bContinue = HandleClose(events);

	return bContinue;
}

BOOL CTcpClient::HandleError(WSANETWORKEVENTS& events)
{
	int iCode						= ::WSAGetLastError();
	EnSocketOperation enOperation	= SO_UNKNOWN;

	if(events.lNetworkEvents & FD_CONNECT)
		enOperation = SO_CONNECT;
	else if(events.lNetworkEvents & FD_CLOSE)
		enOperation = SO_CLOSE;
	else if(events.lNetworkEvents & FD_READ)
		enOperation = SO_RECEIVE;
	else if(events.lNetworkEvents & FD_WRITE)
		enOperation = SO_SEND;

	ENSURE(::WSAResetEvent(m_evSocket));
	m_ccContext.Reset(TRUE, enOperation, iCode);

	return FALSE;
}

BOOL CTcpClient::HandleRead(WSANETWORKEVENTS& events)
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

BOOL CTcpClient::HandleWrite(WSANETWORKEVENTS& events)
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

BOOL CTcpClient::HandleConnect(WSANETWORKEVENTS& events)
{
	int iCode = events.iErrorCode[FD_CONNECT_BIT];

	if(iCode != 0)
	{
		m_ccContext.Reset(TRUE, SO_CONNECT, iCode);
		return FALSE;
	}

	if(::WSAEventSelect(m_soClient, m_evSocket, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
	{
		m_ccContext.Reset(TRUE, SO_CONNECT, ::WSAGetLastError());
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

BOOL CTcpClient::HandleClose(WSANETWORKEVENTS& events)
{
	int iCode = events.iErrorCode[FD_CLOSE_BIT];

	if(iCode == 0)
		m_ccContext.Reset(TRUE, SO_CLOSE, SE_OK);
	else
		m_ccContext.Reset(TRUE, SO_CLOSE, iCode);

	return FALSE;
}

BOOL CTcpClient::ReadData()
{
	while(TRUE)
	{
		int rc = recv(m_soClient, (char*)(BYTE*)m_rcBuffer, m_dwSocketBufferSize, 0);

		if(rc > 0)
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

	if(::WSAEventSelect(m_soClient, m_evSocket, bPause ? 0 : FD_READ | FD_WRITE | FD_CLOSE) != SOCKET_ERROR)
	{
		m_bPaused = bPause;
		return TRUE;
	}
	
	return FALSE;
}

BOOL CTcpClient::SendData()
{
	BOOL isOK = TRUE;

	while(TRUE)
	{
		TItemPtr itPtr(m_itPool, GetSendBuffer());

		if(itPtr.IsValid())
		{
			ASSERT(!itPtr->IsEmpty());

			isOK = DoSendData(itPtr);

			if(isOK)
			{
				if(!itPtr->IsEmpty())
				{
					CCriSecLock locallock(m_csSend);
					m_lsSend.PushFront(itPtr.Detach());
					
					break;
				}
			}
			else
				break;
		}
		else
			break;
	}

	return isOK;
}

TItem* CTcpClient::GetSendBuffer()
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

BOOL CTcpClient::DoSendData(TItem* pItem)
{
	while(!pItem->IsEmpty())
	{
		int rc = 0;

		{
			CCriSecLock locallock(m_csSend);

			rc = send(m_soClient, (char*)pItem->Ptr(), min(pItem->Size(), (int)m_dwSocketBufferSize), 0);
			if(rc > 0) m_iPending -= rc;
		}

		if(rc > 0)
		{
			if(TRIGGER(FireSend(pItem->Ptr(), rc)) == HR_ERROR)
			{
				TRACE("<C-CNNID: %Iu> OnSend() event should not return 'HR_ERROR' !!\n", m_dwConnID);
				ASSERT(FALSE);
			}

			pItem->Reduce(rc);
		}
		else if(rc == SOCKET_ERROR)
		{
			int code = ::WSAGetLastError();

			if(code == WSAEWOULDBLOCK)
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

BOOL CTcpClient::Stop()
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

void CTcpClient::Reset()
{
	CCriSecLock locallock(m_csSend);

	m_rcBuffer.Free();
	m_evBuffer.Reset();
	m_evWorker.Reset();
	m_lsSend.Clear();
	m_itPool.Clear();

	m_strHost.Empty();

	m_usPort	= 0;
	m_iPending	= 0;
	m_bPaused	= FALSE;
	m_enState	= SS_STOPPED;
}

void CTcpClient::WaitForWorkerThreadEnd(DWORD dwCurrentThreadID)
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

BOOL CTcpClient::Send(const BYTE* pBuffer, int iLength, int iOffset)
{
	ASSERT(pBuffer && iLength > 0);

	if(iOffset != 0) pBuffer += iOffset;

	WSABUF buffer;
	buffer.len = iLength;
	buffer.buf = (char*)pBuffer;

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
	ASSERT(m_iPending >= 0);

	int iPending	= m_iPending;
	BOOL isPending	= m_iPending > 0;

	for(int i = 0; i < iCount; i++)
	{
		int iBufLen = pBuffers[i].len;

		if(iBufLen > 0)
		{
			BYTE* pBuffer = (BYTE*)pBuffers[i].buf;
			ASSERT(pBuffer);

			m_lsSend.Cat(pBuffer, iBufLen);
			m_iPending += iBufLen;
		}
	}

	if(!isPending && m_iPending > iPending) m_evBuffer.Set();

	return NO_ERROR;
}

BOOL CTcpClient::SendSmallFile(LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail)
{
	CAtlFile file;
	CAtlFileMapping<> fmap;
	WSABUF szBuf[3];

	HRESULT hr = ::MakeSmallFilePackage(lpszFileName, file, fmap, szBuf, pHead, pTail);

	if(FAILED(hr))
	{
		::SetLastError(HRESULT_CODE(hr));
		return FALSE;
	}

	return SendPackets(szBuf, 3);
}

void CTcpClient::SetLastError(EnSocketError code, LPCSTR func, int ec)
{
	TRACE("%s --> Error: %d, EC: %d\n", func, code, ec);

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
