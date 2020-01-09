/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Author	: Bruce Liang
 * Website	: https://github.com/ldcsaa
 * Project	: https://github.com/ldcsaa/HP-Socket/HP-Socket
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
 
#include "stdafx.h"
#include "UdpClient.h"
#include "../Common/Src/WaitFor.h"

#ifdef _UDP_SUPPORT

const CInitSocket CUdpClient::sm_wsSocket;

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
					{
						isOK = TRUE;
						m_evWait.Reset();
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

BOOL CUdpClient::CheckParams()
{
	if	(((int)m_dwMaxDatagramSize > 0 && m_dwMaxDatagramSize <= MAXIMUM_UDP_MAX_DATAGRAM_SIZE)	&&
		((int)m_dwFreeBufferPoolSize >= 0)														&&
		((int)m_dwFreeBufferPoolHold >= 0)														&&
		((int)m_dwDetectAttempts >= 0)															&&
		((int)m_dwDetectInterval >= 1000 || m_dwDetectInterval == 0)							)
		return TRUE;

	SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
	return FALSE;
}

void CUdpClient::PrepareStart()
{
	m_itPool.SetItemCapacity((int)m_dwMaxDatagramSize);
	m_itPool.SetPoolSize((int)m_dwFreeBufferPoolSize);
	m_itPool.SetPoolHold((int)m_dwFreeBufferPoolHold);

	m_itPool.Prepare();
}

BOOL CUdpClient::CheckStarting()
{
	CSpinLock locallock(m_csState);

	if(m_enState == SS_STOPPED)
		m_enState = SS_STARTING;
	else
	{
		SetLastError(SE_ILLEGAL_STATE, __FUNCTION__, ERROR_INVALID_STATE);
		return FALSE;
	}

	return TRUE;
}

BOOL CUdpClient::CheckStoping(DWORD dwCurrentThreadID)
{
	if(m_enState != SS_STOPPED)
	{
		CSpinLock locallock(m_csState);

		if(HasStarted())
		{
			m_enState = SS_STOPPING;
			return TRUE;
		}
	}

	SetLastError(SE_ILLEGAL_STATE, __FUNCTION__, ERROR_INVALID_STATE);

	return FALSE;
}

BOOL CUdpClient::CreateClientSocket(LPCTSTR lpszRemoteAddress, HP_SOCKADDR& addrRemote, USHORT usPort, LPCTSTR lpszBindAddress, HP_SOCKADDR& addrBind)
{
	if(!::GetSockAddrByHostName(lpszRemoteAddress, usPort, addrRemote))
		return FALSE;

	if(::IsStrNotEmpty(lpszBindAddress))
	{
		if(!::sockaddr_A_2_IN(lpszBindAddress, 0, addrBind))
			return FALSE;

		if(addrRemote.family != addrBind.family)
		{
			::WSASetLastError(WSAEAFNOSUPPORT);
			return FALSE;
		}
	}

	m_soClient = socket(addrRemote.family, SOCK_DGRAM, IPPROTO_UDP);

	if(m_soClient == INVALID_SOCKET)
		return FALSE;

	ENSURE(::SSO_UDP_ConnReset(m_soClient, FALSE) == NO_ERROR);
	ENSURE(::SSO_ReuseAddress(m_soClient, m_enReusePolicy) == NO_ERROR);

	m_evSocket = ::WSACreateEvent();
	ASSERT(m_evSocket != WSA_INVALID_EVENT);

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
				{
					ENSURE(DetectConnection() == NO_ERROR);

					isOK = TRUE;
				}
			}
		}
	}

	return isOK;
}

BOOL CUdpClient::CreateWorkerThread()
{
	m_hWorker = (HANDLE)_beginthreadex(nullptr, 0, WorkerThreadProc, (LPVOID)this, 0, &m_dwWorkerID);

	return m_hWorker != nullptr;
}

UINT WINAPI CUdpClient::WorkerThreadProc(LPVOID pv)
{
	TRACE("---------------> Client Worker Thread 0x%08X started <---------------\n", SELF_THREAD_ID);

	CUdpClient* pClient	= (CUdpClient*)pv;
	pClient->OnWorkerThreadStart(SELF_THREAD_ID);

	BOOL bCallStop	= TRUE;
	DWORD dwSize	= 4;
	DWORD dwIndex	= 0;
	BOOL bDetect	= pClient->IsNeedDetect();
	HANDLE hUserEvt	= pClient->GetUserEvent();

	if(bDetect) ++dwSize;
	if(hUserEvt != nullptr) ++dwSize;

	HANDLE* hEvents	= CreateLocalObjects(HANDLE, dwSize);

	hEvents[dwIndex++] = pClient->m_evSocket;
	hEvents[dwIndex++] = pClient->m_evBuffer;
	hEvents[dwIndex++] = pClient->m_evWorker;
	hEvents[dwIndex++] = pClient->m_evUnpause;

	unique_ptr<CTimerEvt> evDetectPtr;

	if(bDetect)
	{
		evDetectPtr.reset(new CTimerEvt());
		evDetectPtr->Set(pClient->m_dwDetectInterval);
		hEvents[dwIndex++] = evDetectPtr->GetHandle();
	}

	if(hUserEvt != nullptr)
		hEvents[dwIndex++] = hUserEvt;

	pClient->m_rcBuffer.Malloc(pClient->m_dwMaxDatagramSize);

	while(pClient->HasStarted())
	{
		DWORD retval = ::WSAWaitForMultipleEvents(dwSize, hEvents, FALSE, WSA_INFINITE, FALSE);

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
		else if(retval == WSA_WAIT_EVENT_0 + 3)
		{
			if(!pClient->ReadData())
				break;
		}
		else if(retval == WSA_WAIT_EVENT_0 + 4)
		{
			if(bDetect)
			{
				if(!pClient->CheckConnection())
					break;
			}
			else
			{
				if(!pClient->OnUserEvent())
				{
					pClient->m_ccContext.Reset(TRUE, SO_CLOSE, ENSURE_ERROR_CANCELLED);
					break;
				}
			}
		}
		else if(retval == WSA_WAIT_EVENT_0 + 5)
		{
			if(!pClient->OnUserEvent())
			{
				pClient->m_ccContext.Reset(TRUE, SO_CLOSE, ENSURE_ERROR_CANCELLED);
				break;
			}
		}
		else if(retval == WSA_WAIT_FAILED)
		{
			pClient->m_ccContext.Reset(TRUE, SO_UNKNOWN, ::WSAGetLastError());
			break;
		}
		else
			ENSURE(FALSE);
	}

	pClient->OnWorkerThreadEnd(SELF_THREAD_ID);

	if(bCallStop && pClient->HasStarted())
		pClient->Stop();

	TRACE("---------------> Client Worker Thread 0x%08X stoped <---------------\n", SELF_THREAD_ID);

	return 0;
}

BOOL CUdpClient::CheckConnection()
{
	if(m_dwDetectFails++ >= m_dwDetectAttempts)
	{
		m_ccContext.Reset(TRUE, SO_CLOSE, NO_ERROR, FALSE);
		return FALSE;
	}

	int result = DetectConnection();

	if(result != NO_ERROR)
	{
		m_ccContext.Reset(TRUE, SO_CLOSE, result);
		return FALSE;
	}

	return TRUE;
}

int CUdpClient::DetectConnection()
{
	int result = NO_ERROR;

	if(send(m_soClient, nullptr, 0, 0) == SOCKET_ERROR)
	{
		result = ::WSAGetLastError();
		if(result == WSAEWOULDBLOCK)
			result = NO_ERROR;
	}

	TRACE("<C-CNNID: %Iu> send 0 bytes (detect package)\n", m_dwConnID);

	return result;
}

BOOL CUdpClient::ProcessNetworkEvent()
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

BOOL CUdpClient::HandleError(WSANETWORKEVENTS& events)
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

BOOL CUdpClient::HandleRead(WSANETWORKEVENTS& events)
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

BOOL CUdpClient::HandleWrite(WSANETWORKEVENTS& events)
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

BOOL CUdpClient::HandleConnect(WSANETWORKEVENTS& events)
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

	if(TRIGGER(FireConnect()) != HR_ERROR)
		ENSURE(DetectConnection() == NO_ERROR);
	else
	{
		m_ccContext.Reset(FALSE, SO_CLOSE, ENSURE_ERROR_CANCELLED, FALSE);
		return FALSE;
	}

	return TRUE;
}

BOOL CUdpClient::HandleClose(WSANETWORKEVENTS& events)
{
	int iCode = events.iErrorCode[FD_CLOSE_BIT];

	if(iCode == 0)
		m_ccContext.Reset(TRUE, SO_CLOSE, SE_OK);
	else
		m_ccContext.Reset(TRUE, SO_CLOSE, iCode);

	return FALSE;
}

BOOL CUdpClient::ReadData()
{
	while(TRUE)
	{
		if(m_bPaused)
			break;

		int rc = recv(m_soClient, (char*)(BYTE*)m_rcBuffer, m_dwMaxDatagramSize, 0);

		if(rc > 0)
		{
			m_dwDetectFails = 0;

			if(::IsUdpCloseNotify(m_rcBuffer, rc))
			{
				m_ccContext.Reset(TRUE, SO_CLOSE, NO_ERROR, FALSE);
				return FALSE;
			}

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
			m_dwDetectFails = 0;
			TRACE("<C-CNNID: %Iu> recv 0 bytes (detect package)\n", m_dwConnID);
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

	if(::WSAEventSelect(m_soClient, m_evSocket, bPause ? 0 : FD_READ | FD_WRITE | FD_CLOSE) != SOCKET_ERROR)
	{
		m_bPaused = bPause;

		if(!bPause)
			m_evUnpause.Set();

		return TRUE;
	}

	return FALSE;
}

BOOL CUdpClient::SendData()
{
	while(TRUE)
	{
		TItemPtr itPtr(m_itPool, GetSendBuffer());

		if(itPtr.IsValid())
		{
			ASSERT(!itPtr->IsEmpty());

			int rc = send(m_soClient, (char*)itPtr->Ptr(), itPtr->Size(), 0);

			if(rc > 0)
			{
				ASSERT(rc == itPtr->Size());

				{
					CCriSecLock locallock(m_csSend);
					m_iPending -= rc;
				}

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

TItem* CUdpClient::GetSendBuffer()
{
	TItem* pItem = nullptr;

	if(m_iPending > 0)
	{
		CCriSecLock locallock(m_csSend);
		pItem = m_lsSend.PopFront();
	}

	return pItem;
}

BOOL CUdpClient::Stop()
{
	DWORD dwCurrentThreadID = SELF_THREAD_ID;

	if(!CheckStoping(dwCurrentThreadID))
		return FALSE;

	WaitForWorkerThreadEnd(dwCurrentThreadID);

	CheckConnected();

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

void CUdpClient::Reset()
{
	CCriSecLock locallock(m_csSend);

	m_rcBuffer.Free();
	m_evBuffer.Reset();
	m_evWorker.Reset();
	m_evUnpause.Reset();
	m_lsSend.Clear();
	m_itPool.Clear();

	m_strHost.Empty();

	m_usPort		= 0;
	m_iPending		= 0;
	m_dwDetectFails	= 0;
	m_bPaused		= FALSE;
	m_enState		= SS_STOPPED;

	m_evWait.Set();
}

void CUdpClient::WaitForWorkerThreadEnd(DWORD dwCurrentThreadID)
{
	if(m_hWorker != nullptr)
	{
		if(dwCurrentThreadID != m_dwWorkerID)
		{
			m_evWorker.Set();
			ENSURE(::MsgWaitForSingleObject(m_hWorker));
		}

		::CloseHandle(m_hWorker);

		m_hWorker		= nullptr;
		m_dwWorkerID	= 0;
	}
}

void CUdpClient::CheckConnected()
{
	if(!IsConnected())
		return;

	if(m_ccContext.bNotify)
		::SendUdpCloseNotify(m_soClient);

	SetConnected(FALSE);
}

BOOL CUdpClient::DoSend(const BYTE* pBuffer, int iLength, int iOffset)
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

	int result	= NO_ERROR;
	int iLength	= 0;
	int iMaxLen	= (int)m_dwMaxDatagramSize;

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
	int iPending;

	{
		CCriSecLock locallock(m_csSend);

		if(!IsConnected())
			return ERROR_INVALID_STATE;

		iPending	= m_iPending;
		m_iPending += itPtr->Size();

		m_lsSend.PushBack(itPtr.Detach());
	}

	if(iPending == 0 && m_iPending > 0) m_evBuffer.Set();

	return NO_ERROR;
}

void CUdpClient::SetLastError(EnSocketError code, LPCSTR func, int ec)
{
	TRACE("%s --> Error: %d, EC: %d\n", func, code, ec);

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

#endif
