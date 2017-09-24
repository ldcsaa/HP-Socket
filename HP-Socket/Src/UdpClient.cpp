/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 5.0.2
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
#include "UdpClient.h"
#include "../../Common/Src/WaitFor.h"

#include <process.h>

const CInitSocket CUdpClient::sm_wsSocket;

BOOL CUdpClient::Start(LPCTSTR lpszRemoteAddress, USHORT usPort, BOOL bAsyncConnect, LPCTSTR lpszBindAddress)
{
	if(!CheckParams() || !CheckStarting())
		return FALSE;

	PrepareStart();
	m_ccContext.Reset();

	BOOL isOK		= FALSE;
	m_bAsyncConnect	= bAsyncConnect;

	HP_SOCKADDR addrRemote, addrBind;

	if(CreateClientSocket(lpszRemoteAddress, addrRemote, usPort, lpszBindAddress, addrBind))
	{
		if(BindClientSocket(addrBind))
		{
			if(FirePrepareConnect(m_soClient) != HR_ERROR)
			{
				if(ConnectToServer(addrRemote))
				{
					if(CreateWorkerThread())
					{
						if(CreateDetectorThread())
							isOK = TRUE;
						else
							SetLastError(SE_DETECT_THREAD_CREATE, __FUNCTION__, ERROR_CREATE_FAILED);
					}
					else
						SetLastError(SE_WORKER_THREAD_CREATE, __FUNCTION__, ERROR_CREATE_FAILED);
				}
				else
					SetLastError(SE_CONNECT_SERVER, __FUNCTION__, ::WSAGetLastError());
			}
			else
				SetLastError(SE_SOCKET_PREPARE, __FUNCTION__, ERROR_CANCELLED);
		}
		else
			SetLastError(SE_SOCKET_BIND, __FUNCTION__, ::WSAGetLastError());
	}
	else
		SetLastError(SE_SOCKET_CREATE, __FUNCTION__, ::WSAGetLastError());

	if(!isOK)
	{
		DWORD dwCode = ::GetLastError();

		m_ccContext.Reset(FALSE);
		Stop();

		::SetLastError(dwCode);
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
		SetLastError(SE_ILLEGAL_STATE, __FUNCTION__, ERROR_INVALID_OPERATION);
		return FALSE;
	}

	return TRUE;
}

BOOL CUdpClient::CheckStoping(DWORD dwCurrentThreadID)
{
	if(m_enState == SS_STOPPED)
		return FALSE;

	CSpinLock locallock(m_csState);

	if(HasStarted())
	{
		m_enState = SS_STOPPING;
		return TRUE;
	}
	else if(m_enState == SS_STOPPING)
	{
		if(dwCurrentThreadID != m_dwWorkerID && dwCurrentThreadID != m_dwDetectorID)
		{
			while(m_enState != SS_STOPPED)
				::Sleep(30);
		}

		SetLastError(SE_ILLEGAL_STATE, __FUNCTION__, ERROR_INVALID_OPERATION);
	}

	return FALSE;
}

BOOL CUdpClient::CreateClientSocket(LPCTSTR lpszRemoteAddress, HP_SOCKADDR& addrRemote, USHORT usPort, LPCTSTR lpszBindAddress, HP_SOCKADDR& addrBind)
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

	m_soClient = socket(addrRemote.family, SOCK_DGRAM, IPPROTO_UDP);

	if(m_soClient == INVALID_SOCKET)
		return FALSE;

	VERIFY(::SSO_UDP_ConnReset(m_soClient, FALSE) == NO_ERROR);

	m_evSocket = ::WSACreateEvent();
	ASSERT(m_evSocket != WSA_INVALID_EVENT);

	SetRemoteHost(lpszRemoteAddress, usPort);

	return TRUE;
}

BOOL CUdpClient::BindClientSocket(const HP_SOCKADDR& addrBind)
{
	if(addrBind.IsSpecified() && (::bind(m_soClient, addrBind.Addr(), addrBind.AddrSize()) == SOCKET_ERROR))
		return FALSE;

	m_dwConnID = ::GenerateConnectionID();

	return TRUE;
}

BOOL CUdpClient::ConnectToServer(const HP_SOCKADDR& addrRemote)
{
	BOOL isOK = FALSE;

	if(m_bAsyncConnect)
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
				if(FireConnect() != HR_ERROR)
				{
					VERIFY(NeedDetectorThread() || DetectConnection() == NO_ERROR);

					m_enState	= SS_STARTED;
					isOK		= TRUE;
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
	TRACE("---------------> Client Worker Thread 0x%08X started <---------------\n", ::GetCurrentThreadId());

	BOOL bCallStop		= TRUE;
	CUdpClient* pClient	= (CUdpClient*)pv;
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
			VERIFY(FALSE);
	}

	pClient->OnWorkerThreadEnd(::GetCurrentThreadId());

	if(bCallStop && pClient->HasStarted())
		pClient->Stop();

	TRACE("---------------> Client Worker Thread 0x%08X stoped <---------------\n", ::GetCurrentThreadId());

	return 0;
}

BOOL CUdpClient::ProcessNetworkEvent()
{
	BOOL bContinue = TRUE;
	WSANETWORKEVENTS events;
	
	int rc = ::WSAEnumNetworkEvents(m_soClient, m_evSocket, &events);

	if(rc == SOCKET_ERROR)
		bContinue = HandleError(events);

	if(m_bAsyncConnect && bContinue && events.lNetworkEvents & FD_CONNECT)
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

	VERIFY(::WSAResetEvent(m_evSocket));
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
	BOOL bContinue	= TRUE;
	int iCode		= events.iErrorCode[FD_CONNECT_BIT];

	if(iCode == 0)
	{
		if(::WSAEventSelect(m_soClient, m_evSocket, FD_READ | FD_WRITE | FD_CLOSE) != SOCKET_ERROR)
		{
			if(FireConnect() != HR_ERROR)
			{
				VERIFY(NeedDetectorThread() || DetectConnection() == NO_ERROR);

				m_enState = SS_STARTED;
			}
			else
				iCode = ERROR_CANCELLED;
		}
		else
			iCode = ::WSAGetLastError();
	}

	if(iCode != 0)
	{
		if(iCode != ERROR_CANCELLED)
			m_ccContext.Reset(TRUE, SO_CONNECT, iCode);
		else
			m_ccContext.Reset(FALSE);

		bContinue = FALSE;
	}

	return bContinue;
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
		int rc = recv(m_soClient, (char*)(BYTE*)m_rcBuffer, m_dwMaxDatagramSize, 0);

		if(rc > 0)
		{
			if(FireReceive(m_rcBuffer, rc) == HR_ERROR)
			{
				TRACE("<C-CNNID: %Iu> OnReceive() event return 'HR_ERROR', connection will be closed !\n", m_dwConnID);

				m_ccContext.Reset(TRUE, SO_RECEIVE, ERROR_CANCELLED);
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

BOOL CUdpClient::SendData()
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

				rc = send(m_soClient, (char*)itPtr->Ptr(), itPtr->Size(), 0);
				if(rc > 0) m_iPending -= rc;
			}

			if(rc > 0)
			{
				ASSERT(rc == itPtr->Size());

				if(FireSend(itPtr->Ptr(), rc) == HR_ERROR)
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

	if(m_lsSend.Size() > 0)
	{
		CCriSecLock locallock(m_csSend);

		if(m_lsSend.Size() > 0)
			pItem = m_lsSend.PopFront();
	}

	return pItem;
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

BOOL CUdpClient::CreateDetectorThread()
{
	BOOL isOK = TRUE;

	if(NeedDetectorThread())
	{
		m_hDetector = (HANDLE)_beginthreadex(nullptr, 0, DetecotrThreadProc, (LPVOID)this, 0, &m_dwDetectorID);
		isOK = m_hDetector != nullptr;
	}

	return isOK;
}

UINT WINAPI CUdpClient::DetecotrThreadProc(LPVOID pv)
{
	TRACE("---------------> Client Detecotr Thread 0x%08X started <---------------\n", ::GetCurrentThreadId());

	CUdpClient* pClient	= (CUdpClient*)pv;
	DWORD retval		= WAIT_TIMEOUT;

	ASSERT(pClient->NeedDetectorThread());

	while(pClient->HasStarted())
	{
		retval = ::WaitForSingleObject(pClient->m_evDetector, pClient->m_dwDetectInterval * 1000L);
		ASSERT(retval == WAIT_TIMEOUT || retval == WAIT_OBJECT_0);

		if(retval == WAIT_OBJECT_0)
			break;

		int iCode = NO_ERROR;

		if(pClient->m_dwDetectFails++ < pClient->m_dwDetectAttempts)
			iCode = pClient->DetectConnection();
		else
			iCode = WSAECONNRESET;

		if(iCode != NO_ERROR)
		{
			pClient->m_ccContext.Reset(TRUE, SO_CLOSE, WSAECONNRESET);
			pClient->Stop();
			break;
		}
	}

	TRACE("---------------> Client Detecotr Thread 0x%08X stoped <---------------\n", ::GetCurrentThreadId());

	return 0;
}

BOOL CUdpClient::Stop()
{
	DWORD dwCurrentThreadID = ::GetCurrentThreadId();

	if(!CheckStoping(dwCurrentThreadID))
		return FALSE;

	WaitForDetectorThreadEnd(dwCurrentThreadID);
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

void CUdpClient::Reset()
{
	CCriSecLock locallock(m_csSend);

	m_rcBuffer.Free();
	m_evBuffer.Reset();
	m_evWorker.Reset();
	m_evDetector.Reset();
	m_lsSend.Clear();
	m_itPool.Clear();

	m_strHost.Empty();

	m_usPort		= 0;
	m_iPending		= 0;
	m_dwDetectFails	= 0;
	m_enState		= SS_STOPPED;
}

void CUdpClient::WaitForWorkerThreadEnd(DWORD dwCurrentThreadID)
{
	if(m_hWorker != nullptr)
	{
		if(dwCurrentThreadID != m_dwWorkerID)
		{
			m_evWorker.Set();
			VERIFY(::WaitForSingleObject(m_hWorker, INFINITE) == WAIT_OBJECT_0);
		}

		::CloseHandle(m_hWorker);

		m_hWorker		= nullptr;
		m_dwWorkerID	= 0;
	}
}

void CUdpClient::WaitForDetectorThreadEnd(DWORD dwCurrentThreadID)
{
	if(m_hDetector != nullptr)
	{
		if(dwCurrentThreadID != m_dwDetectorID)
		{
			m_evDetector.Set();
			VERIFY(::WaitForSingleObject(m_hDetector, INFINITE) == WAIT_OBJECT_0);
		}

		::CloseHandle(m_hDetector);

		m_hDetector		= nullptr;
		m_dwDetectorID	= 0;
	}
}

BOOL CUdpClient::Send(const BYTE* pBuffer, int iLength, int iOffset)
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

BOOL CUdpClient::SendPackets(const WSABUF pBuffers[], int iCount)
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

int CUdpClient::SendInternal(const BYTE* pBuffer, int iLength)
{
	int result = NO_ERROR;

	if(HasStarted())
	{
		CCriSecLock locallock(m_csSend);

		if(HasStarted())
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
