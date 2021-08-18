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
 
#include "UdpNode.h"

#ifdef _UDP_SUPPORT

BOOL CUdpNode::Start(LPCTSTR lpszBindAddress, USHORT usPort, EnCastMode enCastMode, LPCTSTR lpszCastAddress)
{
	m_enCastMode = enCastMode;

	if(!CheckParams() || !CheckStarting())
		return FALSE;

	PrepareStart();

	if(CreateListenSocket(lpszBindAddress, usPort, lpszCastAddress))
		if(CreateWorkerThreads())
			if(StartAccept())
			{
				m_enState = SS_STARTED;
				return TRUE;
			}

	EXECUTE_RESTORE_ERROR(Stop());

	return FALSE;
}

BOOL CUdpNode::CheckParams()
{
	if	(((int)m_dwFreeBufferPoolSize >= 0)														&&
		((int)m_dwFreeBufferPoolHold >= 0)														&&
		((int)m_dwPostReceiveCount > 0)															&&
		((int)m_dwWorkerThreadCount > 0 && m_dwWorkerThreadCount <= MAX_WORKER_THREAD_COUNT)	&&
		(m_enCastMode >= CM_UNICAST && m_enCastMode <= CM_BROADCAST)							&&
		(m_iMCTtl >= 0 && m_iMCTtl <= 255)														&&
		(m_bMCLoop == TRUE || m_bMCLoop == FALSE)												&&
		((int)m_dwMaxDatagramSize > 0 && m_dwMaxDatagramSize <= MAXIMUM_UDP_MAX_DATAGRAM_SIZE)	)
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
			::WSASetLastError(ERROR_ADDRNOTAVAIL);
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
		::WSASetLastError(ERROR_PFNOSUPPORT);
		return FALSE;
	}

	if(m_enCastMode != CM_UNICAST && m_castAddr.family != bindAddr.family)
	{
		::WSASetLastError(ERROR_AFNOSUPPORT);
		return FALSE;
	}

	m_soListen = socket(bindAddr.family, SOCK_DGRAM, IPPROTO_UDP);

	if(m_soListen == INVALID_SOCKET)
		return FALSE;

	VERIFY(::fcntl_SETFL(m_soListen, O_NOATIME | O_NONBLOCK | O_CLOEXEC));
	VERIFY(::SSO_ReuseAddress(m_soListen, m_enReusePolicy) == NO_ERROR);

	return TRUE;
}

BOOL CUdpNode::BindListenSocket(HP_SOCKADDR& bindAddr)
{
	if(::bind(m_soListen, bindAddr.Addr(), bindAddr.AddrSize()) == SOCKET_ERROR)
		return FALSE;

	socklen_t dwAddrLen = (socklen_t)bindAddr.AddrSize();
	ENSURE(::getsockname(m_soListen, m_localAddr.Addr(), &dwAddrLen) != SOCKET_ERROR);

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

BOOL CUdpNode::CreateWorkerThreads()
{
	return m_ioDispatcher.Start(this, m_dwPostReceiveCount, m_dwWorkerThreadCount);
}

BOOL CUdpNode::StartAccept()
{
	return m_ioDispatcher.AddFD(m_soListen, _EPOLL_READ_EVENTS | EPOLLET, TO_PVOID(&m_soListen));
}

BOOL CUdpNode::Stop()
{
	if(!CheckStoping())
		return FALSE;

	CloseListenSocket();

	WaitForWorkerThreadEnd();

	FireShutdown();

	ReleaseFreeBuffer();

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

	::WaitFor(100);
}

void CUdpNode::WaitForWorkerThreadEnd()
{
	m_ioDispatcher.Stop();
}

void CUdpNode::ReleaseFreeBuffer()
{
	TNodeBufferObj* pBufferObj = nullptr;

	while(m_recvQueue.PopFront(&pBufferObj))
		m_bfObjPool.PutFreeItem(pBufferObj);

	VERIFY(m_recvQueue.IsEmpty());

	m_sndBuff.Clear();
	m_bfObjPool.Clear();
}

void CUdpNode::Reset()
{
	m_castAddr.Reset();
	m_localAddr.Reset();

	m_iSending	= 0;
	m_enState	= SS_STOPPED;

	m_evWait.SyncNotifyAll();
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

				TNodeBufferObjPtr bufPtr(m_bfObjPool, m_bfObjPool.PickFreeItem());
				bufPtr->Cat(pBuffer, iLength);

				result = SendInternal(addrRemote, bufPtr);
			}
			else
				result = ERROR_INVALID_PARAMETER;
		}
		else
			result = ERROR_AFNOSUPPORT;
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
		::SetLastError(ERROR_AFNOSUPPORT);
		return FALSE;
	}

	int result	= NO_ERROR;
	int iLength = 0;
	int iMaxLen = (int)m_dwMaxDatagramSize;

	TNodeBufferObjPtr bufPtr(m_bfObjPool, m_bfObjPool.PickFreeItem());

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

int CUdpNode::SendInternal(HP_SOCKADDR& addrRemote, TNodeBufferObjPtr& bufPtr)
{
	addrRemote.Copy(bufPtr->remoteAddr);

	BOOL bPending;
	int iBufferSize;
	
	{
		CSpinLock locallock(m_csState);

		if(!IsValid())
			return ERROR_INVALID_STATE;

		bPending	= IsPending();
		iBufferSize	= bufPtr->Size();

		m_sndBuff.PushBack(bufPtr.Detach());
		if(iBufferSize == 0) m_sndBuff.IncreaseLength(1);
	}

	if(!bPending && IsPending())
		VERIFY(m_ioDispatcher.SendCommand(DISP_CMD_SEND));

	return NO_ERROR;
}

BOOL CUdpNode::OnBeforeProcessIo(PVOID pv, UINT events)
{
	ASSERT(pv == &m_soListen);
	
	return TRUE;
}

VOID CUdpNode::OnAfterProcessIo(PVOID pv, UINT events, BOOL rs)
{

}

VOID CUdpNode::OnCommand(TDispCommand* pCmd)
{
	switch(pCmd->type)
	{
	case DISP_CMD_SEND:
		HandleCmdSend((int)(pCmd->wParam));
		break;
	case DISP_CMD_RECEIVE:
		HandleCmdReceive((int)(pCmd->wParam));
		break;
	}
}

BOOL CUdpNode::OnReadyRead(PVOID pv, UINT events)
{
	return HandleReceive(RETRIVE_EVENT_FLAG_H(events));
}

BOOL CUdpNode::OnReadyWrite(PVOID pv, UINT events)
{
	return HandleSend(RETRIVE_EVENT_FLAG_H(events), RETRIVE_EVENT_FLAG_R(events));
}

BOOL CUdpNode::OnHungUp(PVOID pv, UINT events)
{
	return HandleClose(nullptr, SO_CLOSE, 0);
}

BOOL CUdpNode::OnError(PVOID pv, UINT events)
{
	return HandleClose(nullptr, SO_CLOSE, -1);
}

VOID CUdpNode::OnDispatchThreadStart(THR_ID tid)
{
	OnWorkerThreadStart(tid);
}

VOID CUdpNode::OnDispatchThreadEnd(THR_ID tid)
{
	OnWorkerThreadEnd(tid);
}

BOOL CUdpNode::HandleClose(TNodeBufferObj* pBufferObj, EnSocketOperation enOperation, int iErrorCode)
{
	if(!HasStarted())
		return FALSE;

	if(iErrorCode == -1)
		iErrorCode = ::SSO_GetError(m_soListen);

	TRIGGER(FireError(pBufferObj, enOperation, iErrorCode));

	return TRUE;
}

BOOL CUdpNode::HandleReceive(int flag)
{
	while(TRUE)
	{
		TNodeBufferObjPtr itPtr(m_bfObjPool, m_bfObjPool.PickFreeItem());

		int iBufferLen		= itPtr->Capacity();
		socklen_t dwAddrLen	= (socklen_t)itPtr->remoteAddr.AddrSize();

		int rc = (int)recvfrom(m_soListen, itPtr->Ptr(), iBufferLen, MSG_TRUNC, itPtr->remoteAddr.Addr(), &dwAddrLen);

		if(rc >= 0)
		{
			if(rc > iBufferLen)
			{
				itPtr->Increase(iBufferLen);
				TRIGGER(FireError(itPtr, SO_RECEIVE, ERROR_BAD_LENGTH));

				continue;
			}

			itPtr->Increase(rc);
			m_recvQueue.PushBack(itPtr.Detach());

			VERIFY(m_ioDispatcher.SendCommand(DISP_CMD_RECEIVE, flag));
		}
		else if(rc == SOCKET_ERROR)
		{
			int code = ::WSAGetLastError();

			if(code == ERROR_WOULDBLOCK)
				break;
			else if(!HandleClose(itPtr, SO_RECEIVE, code))
				return FALSE;
		}
		else
		{
			ASSERT(FALSE);
		}
	}

	return TRUE;
}

VOID CUdpNode::HandleCmdReceive(int flag)
{
	if(m_recvQueue.IsEmpty())
		return;

	int reads = flag ? -1 : MAX_CONTINUE_READS;

	for(int i = 0; i < reads || reads < 0; i++)
	{
		TNodeBufferObjPtr itPtr(m_bfObjPool);

		if(!m_recvQueue.PopFront(&itPtr.PtrRef()))
			break;

		TRIGGER(FireReceive(itPtr));
	}

	if(!m_recvQueue.IsEmpty())
		VERIFY(m_ioDispatcher.SendCommand(DISP_CMD_RECEIVE, flag));
}

BOOL CUdpNode::HandleSend(int flag, int rd)
{
	m_ioDispatcher.ModFD(m_soListen, _EPOLL_READ_EVENTS | EPOLLET, TO_PVOID(&m_soListen));

	if(rd)
		VERIFY(m_ioDispatcher.SendCommand(DISP_CMD_SEND));
	else
		HandleCmdSend(flag);

	return TRUE;
}

VOID CUdpNode::HandleCmdSend(int flag)
{
	BOOL bBlocked = FALSE;

	TNodeBufferObjPtr bufPtr(m_bfObjPool);

	while(IsPending())
	{
		{
			CSpinLock locallock(m_csState);
			bufPtr = m_sndBuff.PopFront();
		}

		if(!bufPtr.IsValid())
			break;

		if(!SendItem(bufPtr, bBlocked))
			return;

		if(bBlocked)
		{
			{
				CSpinLock locallock(m_csState);
				m_sndBuff.PushFront(bufPtr.Detach());
			}

			m_ioDispatcher.ModFD(m_soListen, EPOLLOUT | _EPOLL_READ_EVENTS | EPOLLET, TO_PVOID(&m_soListen));

			break;
		}
	}

	if(!bBlocked && IsPending())
		VERIFY(m_ioDispatcher.SendCommand(DISP_CMD_SEND));
}

BOOL CUdpNode::SendItem(TNodeBufferObj* pBufferObj, BOOL& bBlocked)
{
	int rc = (int)sendto(m_soListen, pBufferObj->Ptr(), pBufferObj->Size(), 0, pBufferObj->remoteAddr.Addr(), pBufferObj->remoteAddr.AddrSize());

	if(rc >= 0)
	{
		ASSERT(rc == pBufferObj->Size());

		if(rc == 0)
		{
			CSpinLock locallock(m_csState);
			m_sndBuff.ReduceLength(1);
		}

		TRIGGER(FireSend(pBufferObj));
	}
	else if(rc == SOCKET_ERROR)
	{
		int code = ::WSAGetLastError();

		if(code == ERROR_WOULDBLOCK)
			bBlocked = TRUE;
		else if(!HandleClose(pBufferObj, SO_SEND, code))
			return FALSE;
	}
	else
	{
		ASSERT(FALSE);
	}

	return TRUE;
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
}

EnHandleResult CUdpNode::FireSend(TNodeBufferObj* pBufferObj)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	ADDRESS_FAMILY usFamily;
	USHORT usPort;

	::sockaddr_IN_2_A(pBufferObj->remoteAddr, usFamily, szAddress, iAddressLen, usPort);

	return m_pListener->OnSend(this, szAddress, usPort, pBufferObj->Ptr(), pBufferObj->Size());
}

EnHandleResult CUdpNode::FireReceive(TNodeBufferObj* pBufferObj)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	ADDRESS_FAMILY usFamily;
	USHORT usPort;

	::sockaddr_IN_2_A(pBufferObj->remoteAddr, usFamily, szAddress, iAddressLen, usPort);

	return m_pListener->OnReceive(this, szAddress, usPort, pBufferObj->Ptr(), pBufferObj->Size());
}

EnHandleResult CUdpNode::FireError(TNodeBufferObj* pBufferObj, EnSocketOperation enOperation, int iErrorCode)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	ADDRESS_FAMILY usFamily;
	USHORT usPort;

	if(pBufferObj == nullptr)
	{
		::sockaddr_IN_2_A(m_localAddr, usFamily, szAddress, iAddressLen, usPort);
		return m_pListener->OnError(this, enOperation, iErrorCode, szAddress, usPort, nullptr, 0);
	}

	::sockaddr_IN_2_A(pBufferObj->remoteAddr, usFamily, szAddress, iAddressLen, usPort);
	return m_pListener->OnError(this, enOperation, iErrorCode, szAddress, usPort, pBufferObj->Ptr(), pBufferObj->Size());
}

#endif
