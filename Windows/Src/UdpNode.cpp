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
#include "UdpNode.h"
#include "../Common/Src/WaitFor.h"

#ifdef _UDP_SUPPORT

#include <malloc.h>

const CInitSocket CUdpNode::sm_wsSocket;

BOOL CUdpNode::Start(LPCTSTR lpszBindAddress, USHORT usPort, EnCastMode enCastMode, LPCTSTR lpszCastAddress)
{
	m_enCastMode = enCastMode;

	if(!CheckParams() || !CheckStarting())
		return FALSE;

	PrepareStart();

	if(CreateListenSocket(lpszBindAddress, usPort, lpszCastAddress))
		if(CreateCompletePort())
			if(CreateWorkerThreads())
				if(StartAccept())
				{
					m_enState = SS_STARTED;

					m_evWait.Reset();

					return TRUE;
				}

	EXECUTE_RESTORE_ERROR(Stop());

	return FALSE;
}

BOOL CUdpNode::CheckParams()
{
	if	(((int)m_dwFreeBufferPoolSize >= 0)														&&
		((int)m_dwFreeBufferPoolHold >= 0)														&&
		((int)m_dwWorkerThreadCount > 0 && m_dwWorkerThreadCount <= MAX_WORKER_THREAD_COUNT)	&&
		(m_enCastMode >= CM_UNICAST && m_enCastMode <= CM_BROADCAST)							&&
		(m_iMCTtl >= 0 && m_iMCTtl <= 255)														&&
		(m_bMCLoop == TRUE || m_bMCLoop == FALSE)												&&
		((int)m_dwMaxDatagramSize > 0 && m_dwMaxDatagramSize <= MAXIMUM_UDP_MAX_DATAGRAM_SIZE)	&&
		((int)m_dwPostReceiveCount > 0)															)
		return TRUE;

	SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
	return FALSE;
}

BOOL CUdpNode::CheckStarting()
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

void CUdpNode::PrepareStart()
{
	m_bfObjPool.SetItemCapacity(m_dwMaxDatagramSize);
	m_bfObjPool.SetPoolSize(m_dwFreeBufferPoolSize);
	m_bfObjPool.SetPoolHold(m_dwFreeBufferPoolHold);

	m_bfObjPool.Prepare();
}

BOOL CUdpNode::CreateListenSocket(LPCTSTR lpszBindAddress, USHORT usPort, LPCTSTR lpszCastAddress)
{
	HP_SOCKADDR bindAddr(AF_UNSPEC, TRUE);

	if(CreateListenSocket(lpszBindAddress, usPort, lpszCastAddress, bindAddr))
	{
		if(BindListenSocket(bindAddr))
		{
			if(TRIGGER(FirePrepareListen(m_soListen)) != HR_ERROR)
			{
				if(ConnectToGroup(bindAddr))
				{
					return TRUE;
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

	return FALSE;
}


BOOL CUdpNode::CreateListenSocket(LPCTSTR lpszBindAddress, USHORT usPort, LPCTSTR lpszCastAddress, HP_SOCKADDR& bindAddr)
{
	if(::IsStrEmpty(lpszCastAddress))
	{
		if(m_enCastMode == CM_BROADCAST)
			lpszCastAddress = DEFAULT_IPV4_BROAD_CAST_ADDRESS;
		else if(m_enCastMode == CM_MULTICAST)
		{
			::WSASetLastError(WSAEADDRNOTAVAIL);
			return FALSE;
		}
	}

	if(m_enCastMode != CM_UNICAST && !::sockaddr_A_2_IN(lpszCastAddress, usPort, m_castAddr))
		return FALSE;

	if(::IsStrEmpty(lpszBindAddress))
	{
		bindAddr.family = (m_enCastMode != CM_UNICAST) ? m_castAddr.family : AF_INET;
		bindAddr.SetPort(usPort);
	}
	else
	{
		if(!::sockaddr_A_2_IN(lpszBindAddress, usPort, bindAddr))
			return FALSE;
	}

	if(m_enCastMode == CM_BROADCAST && bindAddr.IsIPv6())
	{
		::WSASetLastError(WSAEPROTONOSUPPORT);
		return FALSE;
	}

	if(m_enCastMode != CM_UNICAST && m_castAddr.family != bindAddr.family)
	{
		::WSASetLastError(WSAEAFNOSUPPORT);
		return FALSE;
	}

	m_soListen = socket(bindAddr.family, SOCK_DGRAM, IPPROTO_UDP);

	if(m_soListen == INVALID_SOCKET)
		return FALSE;

	ENSURE(::SSO_UDP_ConnReset(m_soListen, FALSE) == NO_ERROR);
	ENSURE(::SSO_NoBlock(m_soListen) == NO_ERROR);
	ENSURE(::SSO_ReuseAddress(m_soListen, m_enReusePolicy) == NO_ERROR);

	return TRUE;
}

BOOL CUdpNode::BindListenSocket(HP_SOCKADDR& bindAddr)
{
	if(::bind(m_soListen, bindAddr.Addr(), bindAddr.AddrSize()) == SOCKET_ERROR)
		return FALSE;

	int iAddrLen = bindAddr.AddrSize();
	ENSURE(::getsockname(m_soListen, m_localAddr.Addr(), &iAddrLen) != SOCKET_ERROR);

	return TRUE;
}

BOOL CUdpNode::ConnectToGroup(const HP_SOCKADDR& bindAddr)
{
	if(m_enCastMode == CM_UNICAST)
		return TRUE;
	else if(m_enCastMode == CM_MULTICAST)
	{
		if(!::SetMultiCastSocketOptions(m_soListen, bindAddr, m_castAddr, m_iMCTtl, m_bMCLoop))
			return FALSE;
	}
	else
	{
		ASSERT(m_castAddr.IsIPv4());

		BOOL bSet = TRUE;
		ENSURE(::SSO_SetSocketOption(m_soListen, SOL_SOCKET, SO_BROADCAST, &bSet, sizeof(BOOL)) != SOCKET_ERROR);
	}

	return TRUE;
}

BOOL CUdpNode::CreateCompletePort()
{
	m_hCompletePort	= ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
	
	if(m_hCompletePort == nullptr)
		SetLastError(SE_CP_CREATE, __FUNCTION__, ::GetLastError());

	return (m_hCompletePort != nullptr);
}

BOOL CUdpNode::CreateWorkerThreads()
{
	BOOL isOK = TRUE;

	for(DWORD i = 0; i < m_dwWorkerThreadCount; i++)
	{
		HANDLE hThread = (HANDLE)_beginthreadex(nullptr, 0, WorkerThreadProc, (LPVOID)this, 0, nullptr);

		if(hThread)
			m_vtWorkerThreads.push_back(hThread);
		else
		{
			SetLastError(SE_WORKER_THREAD_CREATE, __FUNCTION__, ::GetLastError());
			isOK = FALSE;
			break;
		}
	}

	return isOK;
}

BOOL CUdpNode::StartAccept()
{
	BOOL isOK = TRUE;

	if(::CreateIoCompletionPort((HANDLE)m_soListen, m_hCompletePort, m_soListen, 0))
	{
		m_iRemainPostReceives = m_dwPostReceiveCount;

		for(DWORD i = 0; i < m_dwPostReceiveCount; i++)
			ENSURE(::PostIocpAccept(m_hCompletePort));
	}
	else
	{
		SetLastError(SE_SOCKE_ATTACH_TO_CP, __FUNCTION__, ::GetLastError());
		isOK = FALSE;
	}

	return isOK;
}

BOOL CUdpNode::Stop()
{
	if(!CheckStoping())
		return FALSE;

	CloseListenSocket();

	WaitForPostReceiveRelease();
	WaitForWorkerThreadEnd();

	FireShutdown();

	ReleaseFreeBuffer();
	CloseCompletePort();

	Reset();

	return TRUE;
}

BOOL CUdpNode::CheckStoping()
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

void CUdpNode::CloseListenSocket()
{
	if(m_soListen == INVALID_SOCKET)
		return;

	::ManualCloseSocket(m_soListen);
	m_soListen = INVALID_SOCKET;

	::WaitWithMessageLoop(100);

}

void CUdpNode::WaitForPostReceiveRelease()
{
	while(m_iRemainPostReceives > 0)
		::WaitWithMessageLoop(50);
}

void CUdpNode::WaitForWorkerThreadEnd()
{
	int count = (int)m_vtWorkerThreads.size();

	for(int i = 0; i < count; i++)
		::PostIocpExit(m_hCompletePort);

	int remain	= count;
	int index	= 0;

	while(remain > 0)
	{
		int wait = min(remain, MAXIMUM_WAIT_OBJECTS);
		HANDLE* pHandles = CreateLocalObjects(HANDLE, wait);

		for(int i = 0; i < wait; i++)
			pHandles[i]	= m_vtWorkerThreads[i + index];

		ENSURE(::WaitForMultipleObjects((DWORD)wait, pHandles, TRUE, INFINITE) == WAIT_OBJECT_0);

		for(int i = 0; i < wait; i++)
			::CloseHandle(pHandles[i]);

		remain	-= wait;
		index	+= wait;
	}

	m_vtWorkerThreads.clear();
}

void CUdpNode::ReleaseFreeBuffer()
{
	m_sndBuff.Clear();
	m_bfObjPool.Clear();
}

void CUdpNode::CloseCompletePort()
{
	if(m_hCompletePort != nullptr)
	{
		::CloseHandle(m_hCompletePort);
		m_hCompletePort = nullptr;
	}
}

void CUdpNode::Reset()
{
	m_castAddr.Reset();
	m_localAddr.Reset();

	m_iRemainPostReceives	= 0;
	m_iSending				= 0;
	m_iPending				= 0;
	m_enState				= SS_STOPPED;

	m_evWait.Set();
}

BOOL CUdpNode::Send(LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pBuffer, int iLength, int iOffset)
{
	HP_SOCKADDR addrRemote;

	if(!::GetSockAddrByHostName(lpszRemoteAddress, usRemotePort, addrRemote))
		return FALSE;

	return DoSend(addrRemote, pBuffer, iLength, iOffset);
}

BOOL CUdpNode::SendPackets(LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const WSABUF pBuffers[], int iCount)
{
	HP_SOCKADDR addrRemote;

	if(!::GetSockAddrByHostName(lpszRemoteAddress, usRemotePort, addrRemote))
		return FALSE;

	return DoSendPackets(addrRemote, pBuffers, iCount);
}

BOOL CUdpNode::SendCast(const BYTE* pBuffer, int iLength, int iOffset)
{
	if(m_enCastMode == CM_UNICAST)
	{
		
		::SetLastError(ERROR_INVALID_OPERATION);
		return FALSE;
	}

	return DoSend(m_castAddr, pBuffer, iLength, iOffset);
}

BOOL CUdpNode::SendCastPackets(const WSABUF pBuffers[], int iCount)
{
	if(m_enCastMode == CM_UNICAST)
	{
		::SetLastError(ERROR_INCORRECT_ADDRESS);
		return FALSE;
	}

	return DoSendPackets(m_castAddr, pBuffers, iCount);
}

BOOL CUdpNode::DoSend(HP_SOCKADDR& addrRemote, const BYTE* pBuffer, int iLength, int iOffset)
{
	ASSERT(pBuffer && iLength >= 0 && iLength <= (int)m_dwMaxDatagramSize);

	int result = NO_ERROR;

	if(IsValid())
	{
		if(addrRemote.family == m_localAddr.family)
		{
			if(pBuffer && iLength >= 0 && iLength <= (int)m_dwMaxDatagramSize)
			{
				if(iOffset != 0) pBuffer += iOffset;

				TUdpBufferObjPtr bufPtr(m_bfObjPool, m_bfObjPool.PickFreeItem());
				bufPtr->Cat(pBuffer, iLength);

				result = SendInternal(addrRemote, bufPtr);
			}
			else
				result = ERROR_INVALID_PARAMETER;
		}
		else
			result = WSAEAFNOSUPPORT;
	}
	else
		result = ERROR_INVALID_STATE;

	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);
}

BOOL CUdpNode::DoSendPackets(HP_SOCKADDR& addrRemote, const WSABUF pBuffers[], int iCount)
{
	ASSERT(pBuffers && iCount > 0);

	if(!pBuffers || iCount <= 0)
		return ERROR_INVALID_PARAMETER;;

	if(!IsValid())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	if(addrRemote.family != m_localAddr.family)
	{
		::SetLastError(WSAEAFNOSUPPORT);
		return FALSE;
	}

	int result	= NO_ERROR;
	int iLength = 0;
	int iMaxLen = (int)m_dwMaxDatagramSize;

	TUdpBufferObjPtr bufPtr(m_bfObjPool, m_bfObjPool.PickFreeItem());

	for(int i = 0; i < iCount; i++)
	{
		int iBufLen = pBuffers[i].len;

		if(iBufLen > 0)
		{
			BYTE* pBuffer = (BYTE*)pBuffers[i].buf;
			ASSERT(pBuffer);

			iLength += iBufLen;

			if(iLength <= iMaxLen)
				bufPtr->Cat(pBuffer, iBufLen);
			else
				break;
		}
	}

	if(iLength > 0 && iLength <= iMaxLen)
		result = SendInternal(addrRemote, bufPtr);
	else
		result = ERROR_INCORRECT_SIZE;

	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);
}

int CUdpNode::SendInternal(HP_SOCKADDR& addrRemote, TUdpBufferObjPtr& bufPtr)
{
	addrRemote.Copy(bufPtr->remoteAddr);

	BOOL bPending;
	int iBufferSize;
	
	{
		CSpinLock locallock(m_csState);

		if(!IsValid())
			return ERROR_INVALID_STATE;

		bPending	= IsPending();
		iBufferSize	= (int)(bufPtr->buff.len);
		m_iPending += max(iBufferSize, 1);

		m_sndBuff.PushBack(bufPtr.Detach());
	}

	if(!bPending && IsCanSend()) ENSURE(::PostIocpSend(m_hCompletePort, 0) || !IsValid());

	return NO_ERROR;
}

UINT WINAPI CUdpNode::WorkerThreadProc(LPVOID pv)
{
	CUdpNode* pNode	= (CUdpNode*)pv;
	pNode->OnWorkerThreadStart(SELF_THREAD_ID);

	while(TRUE)
	{
		DWORD dwErrorCode = NO_ERROR;
		DWORD dwBytes;
		ULONG_PTR ulCompKey;
		OVERLAPPED* pOverlapped;
		
		BOOL result = ::GetQueuedCompletionStatus
												(
													pNode->m_hCompletePort,
													&dwBytes,
													&ulCompKey,
													&pOverlapped,
													INFINITE
												);

		if(pOverlapped == nullptr)
		{
			EnIocpAction action = pNode->CheckIocpCommand(pOverlapped, dwBytes, ulCompKey);

			if(action == IOCP_ACT_CONTINUE)
				continue;
			else if(action == IOCP_ACT_BREAK)
				break;
		}

		TUdpBufferObj* pBufferObj = CONTAINING_RECORD(pOverlapped, TUdpBufferObj, ov);

		if (!result)
		{
			DWORD dwFlag	= 0;
			DWORD dwSysCode = ::GetLastError();

			if(pNode->HasStarted())
			{
				result = ::WSAGetOverlappedResult((SOCKET)ulCompKey, &pBufferObj->ov, &dwBytes, FALSE, &dwFlag);

				if (!result)
				{
					dwErrorCode = ::WSAGetLastError();
					TRACE("GetQueuedCompletionStatus error (<NODE: 0x%X> SYS: %d, SOCK: %d, FLAG: %d)\n", pNode, dwSysCode, dwErrorCode, dwFlag);
				}
			}
			else
				dwErrorCode = dwSysCode;

			ASSERT(dwSysCode != 0 && dwErrorCode != 0);
		}

		pNode->HandleIo(pBufferObj, dwBytes, dwErrorCode);
	}

	pNode->OnWorkerThreadEnd(SELF_THREAD_ID);

	return 0;
}

EnIocpAction CUdpNode::CheckIocpCommand(OVERLAPPED* pOverlapped, DWORD dwBytes, ULONG_PTR ulCompKey)
{
	ASSERT(pOverlapped == nullptr);

	EnIocpAction action = IOCP_ACT_CONTINUE;

	if(dwBytes == IOCP_CMD_SEND)
		ProcessSend();
	else if(dwBytes == IOCP_CMD_ACCEPT)
		ProcessReceive(GetFreeBufferObj());
	else if(dwBytes == IOCP_CMD_EXIT && ulCompKey == 0)
		action = IOCP_ACT_BREAK;
	else
		ENSURE(FALSE);

	return action;
}

void CUdpNode::HandleIo(TUdpBufferObj* pBufferObj, DWORD dwBytes, DWORD dwErrorCode)
{
	ASSERT(pBufferObj != nullptr);

	if(dwErrorCode != NO_ERROR)
	{
		HandleError(pBufferObj, dwErrorCode);
		return;
	}

	pBufferObj->buff.len = dwBytes;

	switch(pBufferObj->operation)
	{
	case SO_SEND:
		HandleSend(pBufferObj);
		break;
	case SO_RECEIVE:
		HandleReceive(pBufferObj);
		break;
	default:
		ASSERT(FALSE);
	}
}

void CUdpNode::HandleError(TUdpBufferObj* pBufferObj, DWORD dwErrorCode)
{
	if(pBufferObj->operation == SO_RECEIVE)
	{
		if(dwErrorCode != WSAENOTSOCK && dwErrorCode != ERROR_OPERATION_ABORTED)
			TRIGGER(FireError(pBufferObj, dwErrorCode));

		ProcessReceive(pBufferObj);
	}
	else if(pBufferObj->operation == SO_SEND)
	{
		ENSURE(!HasStarted());

		TRIGGER(FireError(pBufferObj, dwErrorCode));
		ProcessSend(pBufferObj);
	}
	else
		ASSERT(FALSE);
}

void CUdpNode::HandleReceive(TUdpBufferObj* pBufferObj)
{
	TRIGGER(FireReceive(pBufferObj));

	::ContinueReceiveFrom(this, pBufferObj);

	ProcessReceive(pBufferObj);
}

int CUdpNode::ProcessReceive(TUdpBufferObj* pBufferObj)
{
	int result = NO_ERROR;

	if(!HasStarted())
		result = ERROR_INVALID_STATE;
	else
	{
		pBufferObj->buff.len = m_dwMaxDatagramSize;
		result = ::PostReceiveFrom(m_soListen, pBufferObj);
	}

	if(result != NO_ERROR)
	{
		ENSURE(!HasStarted());

		::InterlockedDecrement(&m_iRemainPostReceives);
		ASSERT(m_iRemainPostReceives >= 0);

		AddFreeBufferObj(pBufferObj);
	}

	return result;
}

void CUdpNode::HandleSend(TUdpBufferObj* pBufferObj)
{
	TRIGGER(FireSend(pBufferObj));
	ProcessSend(pBufferObj);
}

int CUdpNode::ProcessSend(TUdpBufferObj* pBufferObj)
{
	long iLength = -(long)(pBufferObj->buff.len);
	::InterlockedExchangeAdd(&m_iSending, iLength);

	if(pBufferObj->ReleaseSendCounter() == 0)
		AddFreeBufferObj(pBufferObj);

	return ProcessSend();
}

int CUdpNode::ProcessSend()
{
	int result = NO_ERROR;

	if(!IsValid() || !IsCanSend())
		return result;

	int iBufferSize;
	TUdpBufferObj* pBufferObj;

	while(IsPending())
	{
		{
			CSpinLock locallock(m_csState);

			if(!IsValid() || !IsCanSend())
				break;

			pBufferObj = m_sndBuff.PopFront();

			if(pBufferObj == nullptr)
				break;

			iBufferSize	= pBufferObj->buff.len;
			m_iPending -= max(iBufferSize, 1);

			ASSERT(iBufferSize >= 0 && iBufferSize <= (int)m_dwMaxDatagramSize);
		}

		::InterlockedExchangeAdd(&m_iSending, iBufferSize);

		result			= ::PostSendToNotCheck(m_soListen, pBufferObj);
		LONG sndCounter	= pBufferObj->ReleaseSendCounter();

		if(sndCounter == 0)
			AddFreeBufferObj(pBufferObj);
		else if(!IOCP_SUCCESS(result))
		{
			TRIGGER(FireError(pBufferObj, result));

			::InterlockedExchangeAdd(&m_iSending, -iBufferSize);
			AddFreeBufferObj(pBufferObj);

			continue;
		}

		if(result != NO_ERROR)
			break;
	}

	return result;
}

BOOL CUdpNode::GetLocalAddress(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ADDRESS_FAMILY usFamily;
	return ::sockaddr_IN_2_A(m_localAddr, usFamily, lpszAddress, iAddressLen, usPort);
}

BOOL CUdpNode::GetCastAddress(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ADDRESS_FAMILY usFamily;
	return ::sockaddr_IN_2_A(m_castAddr, usFamily, lpszAddress, iAddressLen, usPort);
}

void CUdpNode::SetLastError(EnSocketError code, LPCSTR func, int ec)
{
	m_enLastError = code;
	::SetLastError(ec);

	TRACE("%s --> Error: %d, EC: %d\n", func, code, ec);
}

TUdpBufferObj* CUdpNode::GetFreeBufferObj(int iLen)
{
	ASSERT(iLen >= -1 && iLen <= (int)m_dwMaxDatagramSize);

	TUdpBufferObj* pBufferObj		= m_bfObjPool.PickFreeItem();;
	if(iLen < 0) iLen				= m_dwMaxDatagramSize;
	pBufferObj->buff.len			= iLen;
	pBufferObj->remoteAddr.family	= m_localAddr.family;
	pBufferObj->addrLen				= pBufferObj->remoteAddr.AddrSize();

	return pBufferObj;
}

void CUdpNode::AddFreeBufferObj(TUdpBufferObj* pBufferObj)
{
	m_bfObjPool.PutFreeItem(pBufferObj);
}

EnHandleResult CUdpNode::FireSend(TUdpBufferObj* pBufferObj)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	ADDRESS_FAMILY usFamily;
	USHORT usPort;

	::sockaddr_IN_2_A(pBufferObj->remoteAddr, usFamily, szAddress, iAddressLen, usPort);

	return m_pListener->OnSend(this, szAddress, usPort, (BYTE*)pBufferObj->buff.buf, pBufferObj->buff.len);
}

EnHandleResult CUdpNode::FireReceive(TUdpBufferObj* pBufferObj)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	ADDRESS_FAMILY usFamily;
	USHORT usPort;

	::sockaddr_IN_2_A(pBufferObj->remoteAddr, usFamily, szAddress, iAddressLen, usPort);

	return m_pListener->OnReceive(this, szAddress, usPort, (BYTE*)pBufferObj->buff.buf, pBufferObj->buff.len);
}

EnHandleResult CUdpNode::FireError(TUdpBufferObj* pBufferObj, int iErrorCode)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	ADDRESS_FAMILY usFamily;
	USHORT usPort;

	::sockaddr_IN_2_A(pBufferObj->remoteAddr, usFamily, szAddress, iAddressLen, usPort);

	return m_pListener->OnError(this, pBufferObj->operation, iErrorCode, szAddress, usPort, (BYTE*)pBufferObj->buff.buf, pBufferObj->buff.len);
}

#endif
