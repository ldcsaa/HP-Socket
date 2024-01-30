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

#if defined(__GNUC__) && __GNUC__ >= 11
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif

#include "UdpNode.h"

#ifdef _UDP_SUPPORT

BOOL CUdpNode::Start(LPCTSTR lpszBindAddress, USHORT usPort, EnCastMode enCastMode, LPCTSTR lpszCastAddress)
{
	m_enCastMode = enCastMode;

	if(!CheckParams() || !CheckStarting())
		return FALSE;

	PrepareStart();

	HP_SOCKADDR bindAddr(AF_UNSPEC, TRUE);

	if(ParseBindAddr(lpszBindAddress, usPort, lpszCastAddress, bindAddr))
		if(CreateListenSocket(bindAddr))
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
	CReentrantWriteLock locallock(m_lcState);

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

	TNodeBufferObjList* pBufferObjList = (TNodeBufferObjList*)malloc(m_dwWorkerThreadCount * sizeof(TNodeBufferObjList));

	for(int i = 0; i < (int)m_dwWorkerThreadCount; i++)
		new (pBufferObjList + i) TNodeBufferObjList(m_bfObjPool);

	m_sndBuffs.reset(pBufferObjList);

	m_csSends	= make_unique<CCriSec[]>(m_dwWorkerThreadCount);

	m_rcBuffers = make_unique<CBufferPtr[]>(m_dwWorkerThreadCount);
	for_each(m_rcBuffers.get(), m_rcBuffers.get() + m_dwWorkerThreadCount, [this](CBufferPtr& buff) {buff.Malloc(m_dwMaxDatagramSize);});

	m_soListens	= make_unique<SOCKET[]>(m_dwWorkerThreadCount);
	for_each(m_soListens.get(), m_soListens.get() + m_dwWorkerThreadCount, [](SOCKET& sock) {sock = INVALID_FD;});
}

BOOL CUdpNode::ParseBindAddr(LPCTSTR lpszBindAddress, USHORT usPort, LPCTSTR lpszCastAddress, HP_SOCKADDR& bindAddr)
{
	if(::IsStrEmpty(lpszCastAddress))
	{
		if(m_enCastMode == CM_BROADCAST)
			lpszCastAddress = DEFAULT_IPV4_BROAD_CAST_ADDRESS;
		else if(m_enCastMode == CM_MULTICAST)
		{
			SetLastError(SE_SOCKET_CREATE, __FUNCTION__, ERROR_ADDRNOTAVAIL);
			return FALSE;
		}
	}

	if(m_enCastMode != CM_UNICAST && !::sockaddr_A_2_IN(lpszCastAddress, usPort, m_castAddr))
	{
		SetLastError(SE_SOCKET_CREATE, __FUNCTION__, ::WSAGetLastError());
		return FALSE;
	}

	if(::IsStrEmpty(lpszBindAddress))
	{
		bindAddr.family = (m_enCastMode != CM_UNICAST) ? m_castAddr.family : AF_INET;
		bindAddr.SetPort(usPort);
	}
	else
	{
		if(!::sockaddr_A_2_IN(lpszBindAddress, usPort, bindAddr))
		{
			SetLastError(SE_SOCKET_CREATE, __FUNCTION__, ::WSAGetLastError());
			return FALSE;
		}
	}

	if(m_enCastMode == CM_BROADCAST && bindAddr.IsIPv6())
	{
		SetLastError(SE_SOCKET_CREATE, __FUNCTION__, ERROR_PFNOSUPPORT);
		return FALSE;
	}

	if(m_enCastMode != CM_UNICAST && m_castAddr.family != bindAddr.family)
	{
		SetLastError(SE_SOCKET_CREATE, __FUNCTION__, ERROR_AFNOSUPPORT);
		return FALSE;
	}

	return TRUE;
}

BOOL CUdpNode::CreateListenSocket(const HP_SOCKADDR& bindAddr)
{
	for(DWORD i = 0; i < m_dwWorkerThreadCount; i++)
	{
		m_soListens[i]  = socket(bindAddr.family, SOCK_DGRAM, IPPROTO_UDP);
		SOCKET soListen = m_soListens[i];

		if(IS_INVALID_FD(soListen))
		{
			SetLastError(SE_SOCKET_CREATE, __FUNCTION__, ::WSAGetLastError());
			return FALSE;
		}

		::fcntl_SETFL(soListen, O_NOATIME | O_NONBLOCK | O_CLOEXEC);
		VERIFY(IS_NO_ERROR(::SSO_ReuseAddress(soListen, m_enReusePolicy)));

		if(IS_HAS_ERROR(::bind(soListen, bindAddr.Addr(), bindAddr.AddrSize())))
		{
			SetLastError(SE_SOCKET_BIND, __FUNCTION__, ::WSAGetLastError());
			return FALSE;
		}

		if(i == 0)
		{
			socklen_t dwAddrLen = (socklen_t)bindAddr.AddrSize();
			ENSURE(IS_NO_ERROR(::getsockname(soListen, m_localAddr.Addr(), &dwAddrLen)));
		}

		if(m_enCastMode == CM_MULTICAST)
		{
			if(!::SetMultiCastSocketOptions(soListen, bindAddr, m_castAddr, m_iMCTtl, m_bMCLoop))
			{
				SetLastError(SE_CONNECT_SERVER, __FUNCTION__, ::WSAGetLastError());
				return FALSE;
			}
		}
		else if(m_enCastMode == CM_BROADCAST)
		{
			ASSERT(m_castAddr.IsIPv4());

			BOOL bSet = TRUE;
			if(IS_HAS_ERROR(::SSO_SetSocketOption(soListen, SOL_SOCKET, SO_BROADCAST, &bSet, sizeof(BOOL))))
			{
				SetLastError(SE_CONNECT_SERVER, __FUNCTION__, ::WSAGetLastError());
				return FALSE;
			}
		}

		if(TRIGGER(FirePrepareListen(soListen)) == HR_ERROR)
		{
			SetLastError(SE_SOCKET_PREPARE, __FUNCTION__, ENSURE_ERROR_CANCELLED);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CUdpNode::CreateWorkerThreads()
{
	return m_ioDispatcher.Start(this, m_dwPostReceiveCount, m_dwWorkerThreadCount);
}

BOOL CUdpNode::StartAccept()
{
	for(int i = 0; i < (int)m_dwWorkerThreadCount; i++)
	{
		SOCKET& soListen = m_soListens[i];

		if(!m_ioDispatcher.AddFD(i, soListen, EPOLLIN | EPOLLOUT | EPOLLET, TO_PVOID(&soListen)))
			return FALSE;
	}

	return TRUE;
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
		CReentrantWriteLock locallock(m_lcState);

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
	if(m_soListens)
	{
		for_each(m_soListens.get(), m_soListens.get() + m_dwWorkerThreadCount, [](SOCKET& sock)
		{
			if(sock != INVALID_FD)
			{
				::ManualCloseSocket(sock);
				sock = INVALID_FD;
			}
		});

		::WaitFor(100);
	}
}

void CUdpNode::WaitForWorkerThreadEnd()
{
	m_ioDispatcher.Stop();
}

void CUdpNode::ReleaseFreeBuffer()
{
	for_each(m_sndBuffs.get(), m_sndBuffs.get() + m_dwWorkerThreadCount, [](TNodeBufferObjList& sndBuff)
	{
		sndBuff.Clear();
		sndBuff.~TNodeBufferObjList();
	});

	free(m_sndBuffs.release());

	m_csSends = nullptr;

	m_bfObjPool.Clear();
}

void CUdpNode::Reset()
{
	m_castAddr.Reset();
	m_localAddr.Reset();

	m_soListens = nullptr;
	m_rcBuffers	= nullptr;
	 
	m_iSending	= 0;
	m_enState	= SS_STOPPED;

	m_evWait.SyncNotifyAll();
}

int CUdpNode::GenerateBufferIndex(const HP_SOCKADDR& addrRemote)
{
	return (int)(addrRemote.Hash() % m_dwWorkerThreadCount);
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
		::SetLastError(ERROR_INVALID_OPERATION);
		return FALSE;
	}

	return DoSendPackets(m_castAddr, pBuffers, iCount);
}

BOOL CUdpNode::DoSend(const HP_SOCKADDR& addrRemote, const BYTE* pBuffer, int iLength, int iOffset)
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

BOOL CUdpNode::DoSendPackets(const HP_SOCKADDR& addrRemote, const WSABUF pBuffers[], int iCount)
{
	ASSERT(pBuffers && iCount > 0);

	if(!pBuffers || iCount <= 0)
		return ERROR_INVALID_PARAMETER;

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

int CUdpNode::SendInternal(const HP_SOCKADDR& addrRemote, TNodeBufferObjPtr& bufPtr)
{
	BOOL bPending;
	int iBufferSize	= bufPtr->Size();
	int idx			= GenerateBufferIndex(addrRemote);

	addrRemote.Copy(bufPtr->remoteAddr);

	{
		CReentrantReadLock locallock(m_lcState);

		if(!IsValid())
			return ERROR_INVALID_STATE;

		TNodeBufferObjList& sndBuff = m_sndBuffs[idx];

		CCriSecLock locallock2(m_csSends[idx]);

		bPending = IsPending(idx);
		sndBuff.PushBack(bufPtr.Detach());

		if(iBufferSize == 0) sndBuff.IncreaseLength(1);

		ASSERT(sndBuff.Length() > 0);
	}

	if(!bPending && IsPending(idx))
		VERIFY(m_ioDispatcher.SendCommandByIndex(idx, DISP_CMD_SEND));

	return NO_ERROR;
}

BOOL CUdpNode::OnBeforeProcessIo(const TDispContext* pContext, PVOID pv, UINT events)
{
	ASSERT(pv == &m_soListens[pContext->GetIndex()]);
	
	return TRUE;
}

VOID CUdpNode::OnAfterProcessIo(const TDispContext* pContext, PVOID pv, UINT events, BOOL rs)
{

}

VOID CUdpNode::OnCommand(const TDispContext* pContext, TDispCommand* pCmd)
{
	int idx  = pContext->GetIndex();
	int flag = (int)(pCmd->wParam);

	switch(pCmd->type)
	{
	case DISP_CMD_SEND:
		HandleCmdSend(idx, flag);
		break;
	}
}

BOOL CUdpNode::OnReadyRead(const TDispContext* pContext, PVOID pv, UINT events)
{
	return HandleReceive(pContext, RETRIVE_EVENT_FLAG_H(events));
}

BOOL CUdpNode::OnReadyWrite(const TDispContext* pContext, PVOID pv, UINT events)
{
	return HandleSend(pContext, RETRIVE_EVENT_FLAG_H(events), RETRIVE_EVENT_FLAG_R(events));
}

BOOL CUdpNode::OnHungUp(const TDispContext* pContext, PVOID pv, UINT events)
{
	return HandleClose(pContext->GetIndex(), nullptr, SO_CLOSE, 0);
}

BOOL CUdpNode::OnError(const TDispContext* pContext, PVOID pv, UINT events)
{
	return HandleClose(pContext->GetIndex(), nullptr, SO_CLOSE, -1);
}

VOID CUdpNode::OnDispatchThreadStart(THR_ID tid)
{
	OnWorkerThreadStart(tid);
}

VOID CUdpNode::OnDispatchThreadEnd(THR_ID tid)
{
	OnWorkerThreadEnd(tid);
}

BOOL CUdpNode::HandleClose(int idx, TNodeBufferObj* pBufferObj, EnSocketOperation enOperation, int iErrorCode)
{
	if(!HasStarted())
		return FALSE;

	if(iErrorCode == -1)
		iErrorCode = ::SSO_GetError(m_soListens[idx]);

	if(pBufferObj != nullptr)
		TRIGGER(FireError(&pBufferObj->remoteAddr, pBufferObj->Ptr(), pBufferObj->Size(), enOperation, iErrorCode));
	else
		TRIGGER(FireError(nullptr, nullptr, 0, enOperation, iErrorCode));

	return TRUE;
}

BOOL CUdpNode::HandleReceive(const TDispContext* pContext, int flag)
{
	int idx				= pContext->GetIndex();
	CBufferPtr& buffer	= m_rcBuffers[idx];
	int iBufferLen		= (int)buffer.Size();

	while(TRUE)
	{
		HP_SOCKADDR addr;
		socklen_t dwAddrLen = (socklen_t)addr.AddrSize();

		int rc = (int)recvfrom(m_soListens[idx], buffer.Ptr(), iBufferLen, MSG_TRUNC, addr.Addr(), &dwAddrLen);

		if(rc >= 0)
		{
			if(rc > iBufferLen)
			{
				TRIGGER(FireError(&addr, buffer.Ptr(), iBufferLen, SO_RECEIVE, ERROR_BAD_LENGTH));
				continue;
			}

			TRIGGER(FireReceive(&addr, buffer.Ptr(), rc));
		}
		else if(rc == SOCKET_ERROR)
		{
			int code = ::WSAGetLastError();

			if(code == ERROR_WOULDBLOCK)
				break;
			else if(!HandleClose(idx, nullptr, SO_RECEIVE, code))
				return FALSE;
		}
		else
		{
			ASSERT(FALSE);
		}
	}

	return TRUE;
}

BOOL CUdpNode::HandleSend(const TDispContext* pContext, int flag, int rd)
{
	HandleCmdSend(pContext->GetIndex(), flag);

	return TRUE;
}

VOID CUdpNode::HandleCmdSend(int idx, int flag)
{
	BOOL bBlocked				= FALSE;
	TNodeBufferObjList& sndBuff	= m_sndBuffs[idx];

	TNodeBufferObjPtr bufPtr(m_bfObjPool);

	while(IsPending(idx))
	{
		{
			CCriSecLock locallock(m_csSends[idx]);
			bufPtr = sndBuff.PopFront();
		}

		if(!bufPtr.IsValid())
			break;

		if(!SendItem(idx, sndBuff, bufPtr, bBlocked))
			return;

		if(bBlocked)
		{
			{
				CCriSecLock locallock(m_csSends[idx]);
				sndBuff.PushFront(bufPtr.Detach());
			}

			break;
		}
	}

	if(!bBlocked && IsPending(idx))
		VERIFY(m_ioDispatcher.SendCommandByIndex(idx, DISP_CMD_SEND));
}

BOOL CUdpNode::SendItem(int idx, TNodeBufferObjList& sndBuff, TNodeBufferObj* pBufferObj, BOOL& bBlocked)
{
	int rc = (int)sendto(m_soListens[idx], pBufferObj->Ptr(), pBufferObj->Size(), 0, pBufferObj->remoteAddr.Addr(), pBufferObj->remoteAddr.AddrSize());

	if(rc >= 0)
	{
		ASSERT(rc == pBufferObj->Size());

		if(rc == 0)
		{
			CCriSecLock locallock(m_csSends[idx]);
			sndBuff.ReduceLength(1);
		}

		TRIGGER(FireSend(pBufferObj));
	}
	else if(rc == SOCKET_ERROR)
	{
		int code = ::WSAGetLastError();

		if(code == ERROR_WOULDBLOCK)
			bBlocked = TRUE;
		else if(!HandleClose(idx, pBufferObj, SO_SEND, code))
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

BOOL CUdpNode::GetPendingDataLength(int& iPending)
{
	iPending = 0;

	{
		CReentrantReadLock locallock(m_lcState);

		if(!IsValid())
			return FALSE;

		for_each(m_sndBuffs.get(), m_sndBuffs.get() + m_dwWorkerThreadCount, [&iPending](TNodeBufferObjList& sndBuff) { iPending += sndBuff.Length(); });
	}

	return TRUE;
}

EnHandleResult CUdpNode::FireSend(TNodeBufferObj* pBufferObj)
{
	TCHAR szAddress[60];
	int iAddressLen = ARRAY_SIZE(szAddress);
	ADDRESS_FAMILY usFamily;
	USHORT usPort;

	::sockaddr_IN_2_A(pBufferObj->remoteAddr, usFamily, szAddress, iAddressLen, usPort);

	return m_pListener->OnSend(this, szAddress, usPort, pBufferObj->Ptr(), pBufferObj->Size());
}

EnHandleResult CUdpNode::FireReceive(const HP_SOCKADDR* pRemoteAddr, const BYTE* pData, int iLength)
{
	TCHAR szAddress[60];
	int iAddressLen = ARRAY_SIZE(szAddress);
	ADDRESS_FAMILY usFamily;
	USHORT usPort;

	::sockaddr_IN_2_A(*pRemoteAddr, usFamily, szAddress, iAddressLen, usPort);
	return m_pListener->OnReceive(this, szAddress, usPort, pData, iLength);
}

EnHandleResult CUdpNode::FireError(const HP_SOCKADDR* pRemoteAddr, const BYTE* pData, int iLength, EnSocketOperation enOperation, int iErrorCode)
{
	TCHAR szAddress[60];
	int iAddressLen = ARRAY_SIZE(szAddress);
	ADDRESS_FAMILY usFamily;
	USHORT usPort;

	if(pRemoteAddr == nullptr)
	{
		::sockaddr_IN_2_A(m_localAddr, usFamily, szAddress, iAddressLen, usPort);
		return m_pListener->OnError(this, enOperation, iErrorCode, szAddress, usPort, nullptr, 0);
	}

	::sockaddr_IN_2_A(*pRemoteAddr, usFamily, szAddress, iAddressLen, usPort);
	return m_pListener->OnError(this, enOperation, iErrorCode, szAddress, usPort, pData, iLength);
}

#endif

#if defined(__GNUC__) && __GNUC__ >= 11
#pragma GCC diagnostic pop
#endif
