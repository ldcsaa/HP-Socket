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
 
#include "UdpServer.h"

EnHandleResult CUdpServer::TriggerFireAccept(TUdpSocketObj* pSocketObj)
{
	EnHandleResult rs = TRIGGER(FireAccept(pSocketObj));
	pSocketObj->lcIo.unlock();

	return rs;
}

BOOL CUdpServer::Start(LPCTSTR lpszBindAddress, USHORT usPort)
{
	if(!CheckParams() || !CheckStarting())
		return FALSE;

	PrepareStart();

	if(CreateListenSocket(lpszBindAddress, usPort))
		if(CreateWorkerThreads())
			if(CreateDetectorThread())
				if(StartAccept())
				{
					m_enState = SS_STARTED;
					return TRUE;
				}

	EXECUTE_RESTORE_ERROR(Stop());

	return FALSE;
}

void CUdpServer::SetLastError(EnSocketError code, LPCSTR func, int ec)
{
	m_enLastError = code;
	::SetLastError(ec);
}

BOOL CUdpServer::CheckParams()
{
	if	((m_enSendPolicy >= SP_PACK && m_enSendPolicy <= SP_DIRECT)								&&
		((int)m_dwMaxConnectionCount > 0)														&&
		((int)m_dwWorkerThreadCount > 0 && m_dwWorkerThreadCount <= MAX_WORKER_THREAD_COUNT)	&&
		((int)m_dwFreeSocketObjLockTime >= 0)													&&
		((int)m_dwFreeSocketObjPool >= 0)														&&
		((int)m_dwFreeBufferObjPool >= 0)														&&
		((int)m_dwFreeSocketObjHold >= 0)														&&
		((int)m_dwFreeBufferObjHold >= 0)														&&
		((int)m_dwMaxDatagramSize > 0)															&&
		((int)m_dwPostReceiveCount > 0)															&&
		((int)m_dwDetectAttempts >= 0)															&&
		((int)m_dwDetectInterval >= 0)															)
		return TRUE;

	SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
	return FALSE;
}

void CUdpServer::PrepareStart()
{
	m_bfActiveSockets.Reset(m_dwMaxConnectionCount);
	m_lsFreeSocket.Reset(m_dwFreeSocketObjPool);

	m_bfObjPool.SetItemCapacity(m_dwMaxDatagramSize);
	m_bfObjPool.SetPoolSize(m_dwFreeBufferObjPool);
	m_bfObjPool.SetPoolHold(m_dwFreeBufferObjHold);

	m_bfObjPool.Prepare();
}

BOOL CUdpServer::CheckStarting()
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

BOOL CUdpServer::CheckStoping()
{
	if(m_enState != SS_STOPPED)
	{
		CSpinLock locallock(m_csState);

		if(HasStarted())
		{
			m_enState = SS_STOPPING;
			return TRUE;
		}

		while(m_enState != SS_STOPPED)
			::WaitFor(10);
	}

	SetLastError(SE_ILLEGAL_STATE, __FUNCTION__, ERROR_INVALID_OPERATION);

	return FALSE;
}

BOOL CUdpServer::CreateListenSocket(LPCTSTR lpszBindAddress, USHORT usPort)
{
	BOOL isOK = FALSE;

	if(::IsStrEmpty(lpszBindAddress))
		lpszBindAddress = DEFAULT_IPV4_BIND_ADDRESS;

	HP_SOCKADDR addr;

	if(::sockaddr_A_2_IN(lpszBindAddress, usPort, addr))
	{
		m_soListen = socket(addr.family, SOCK_DGRAM, IPPROTO_UDP);

		if(m_soListen != INVALID_SOCKET)
		{
			::fcntl_SETFL(m_soListen, O_NOATIME | O_NONBLOCK | O_CLOEXEC);

			if(::bind(m_soListen, addr.Addr(), addr.AddrSize()) != SOCKET_ERROR)
			{
				if(TRIGGER(FirePrepareListen(m_soListen)) != HR_ERROR)
					isOK = TRUE;
				else
					SetLastError(SE_SOCKET_PREPARE, __FUNCTION__, ENSURE_ERROR_CANCELLED);
			}
			else
				SetLastError(SE_SOCKET_BIND, __FUNCTION__, ::WSAGetLastError());
		}
		else
			SetLastError(SE_SOCKET_CREATE, __FUNCTION__, ::WSAGetLastError());
	}
	else
		SetLastError(SE_SOCKET_CREATE, __FUNCTION__, ::WSAGetLastError());

	return isOK;
}

BOOL CUdpServer::CreateWorkerThreads()
{
	return m_ioDispatcher.Start(this, m_dwPostReceiveCount, m_dwWorkerThreadCount);
}

BOOL CUdpServer::CreateDetectorThread()
{
	if(!IsNeedRunDetector())
		return TRUE;
		
	return m_thDetector.Start(this, &CUdpServer::DetecotrThreadProc);
}

BOOL CUdpServer::StartAccept()
{
	return m_ioDispatcher.AddFD(m_soListen, _EPOLL_READ_EVENTS | EPOLLET, TO_PVOID(&m_soListen));
}

BOOL CUdpServer::Stop()
{
	if(!CheckStoping())
		return FALSE;

	CloseListenSocket();

	DisconnectClientSocket();
	WaitForClientSocketClose();

	WaitForDetectorThreadEnd();
	WaitForWorkerThreadEnd();
	
	ReleaseClientSocket();

	FireShutdown();

	ReleaseFreeSocket();

	Reset();

	return TRUE;
}

void CUdpServer::CloseListenSocket()
{
	if(m_soListen != INVALID_SOCKET)
	{
		::ManualCloseSocket(m_soListen);
		m_soListen = INVALID_SOCKET;

		::WaitFor(100);
	}
}

void CUdpServer::DisconnectClientSocket()
{
	DWORD size					= 0;
	unique_ptr<CONNID[]> ids	= m_bfActiveSockets.GetAllElementIndexes(size);

	for(DWORD i = 0; i < size; i++)
		Disconnect(ids[i]);
}

void CUdpServer::WaitForClientSocketClose()
{
	while(m_bfActiveSockets.Elements() > 0)
		::WaitFor(50);
}

void CUdpServer::WaitForDetectorThreadEnd()
{
	if(m_thDetector.IsRunning())
	{
		m_evStop.Set();
		m_thDetector.Join();
		m_evStop.Reset();
	}
}

void CUdpServer::WaitForWorkerThreadEnd()
{
	m_ioDispatcher.Stop();
}

void CUdpServer::ReleaseClientSocket()
{
	VERIFY(m_bfActiveSockets.IsEmpty());
	m_bfActiveSockets.Reset();

	CWriteLock locallock(m_csClientSocket);
	m_mpClientAddr.clear();
}

void CUdpServer::ReleaseFreeSocket()
{
	TUdpSocketObj* pSocketObj = nullptr;

	while(m_lsFreeSocket.TryGet(&pSocketObj))
		DeleteSocketObj(pSocketObj);

	VERIFY(m_lsFreeSocket.IsEmpty());
	m_lsFreeSocket.Reset();

	ReleaseGCSocketObj(TRUE);
	VERIFY(m_lsGCSocket.IsEmpty());
}

void CUdpServer::Reset()
{
	m_phSocket.Reset();
	m_bfObjPool.Clear();
	m_quSend.UnsafeClear();

	m_enState = SS_STOPPED;
}

TUdpSocketObj* CUdpServer::GetFreeSocketObj(CONNID dwConnID)
{
	DWORD dwIndex;
	TUdpSocketObj* pSocketObj = nullptr;

	if(m_lsFreeSocket.TryLock(&pSocketObj, dwIndex))
	{
		if(::GetTimeGap32(pSocketObj->freeTime) >= m_dwFreeSocketObjLockTime)
			VERIFY(m_lsFreeSocket.ReleaseLock(nullptr, dwIndex));
		else
		{
			VERIFY(m_lsFreeSocket.ReleaseLock(pSocketObj, dwIndex));
			pSocketObj = nullptr;
		}
	}

	if(!pSocketObj) pSocketObj = CreateSocketObj();
	pSocketObj->Reset(dwConnID);

	return pSocketObj;
}

TUdpSocketObj* CUdpServer::CreateSocketObj()
{
	return TUdpSocketObj::Construct(m_phSocket, m_bfObjPool);
}

void CUdpServer::DeleteSocketObj(TUdpSocketObj* pSocketObj)
{
	TUdpSocketObj::Destruct(pSocketObj);
}

void CUdpServer::AddFreeSocketObj(TUdpSocketObj* pSocketObj, EnSocketCloseFlag enFlag, EnSocketOperation enOperation, int iErrorCode)
{
	if(!InvalidSocketObj(pSocketObj))
		return;

	CloseClientSocketObj(pSocketObj, enFlag, enOperation, iErrorCode);

	{
		m_bfActiveSockets.Remove(pSocketObj->connID);

		CWriteLock locallock(m_csClientSocket);
		m_mpClientAddr.erase(&pSocketObj->remoteAddr);
	}

	TUdpSocketObj::Release(pSocketObj);

	ReleaseGCSocketObj();

	if(!m_lsFreeSocket.TryPut(pSocketObj))
		m_lsGCSocket.PushBack(pSocketObj);
}

void CUdpServer::ReleaseGCSocketObj(BOOL bForce)
{
	::ReleaseGCObj(m_lsGCSocket, m_dwFreeSocketObjLockTime, bForce);
}

BOOL CUdpServer::InvalidSocketObj(TUdpSocketObj* pSocketObj)
{
	return TUdpSocketObj::InvalidSocketObj(pSocketObj);
}

void CUdpServer::AddClientSocketObj(CONNID dwConnID, TUdpSocketObj* pSocketObj, const HP_SOCKADDR& remoteAddr)
{
	ASSERT(FindSocketObj(dwConnID) == nullptr);

	pSocketObj->connTime	= ::TimeGetTime();
	pSocketObj->activeTime	= pSocketObj->connTime;

	remoteAddr.Copy(pSocketObj->remoteAddr);
	pSocketObj->SetConnected();

	VERIFY(m_bfActiveSockets.ReleaseLock(dwConnID, pSocketObj));

	CWriteLock locallock(m_csClientSocket);
	m_mpClientAddr[&pSocketObj->remoteAddr]	= dwConnID;
}

TUdpSocketObj* CUdpServer::FindSocketObj(CONNID dwConnID)
{
	TUdpSocketObj* pSocketObj = nullptr;

	if(m_bfActiveSockets.Get(dwConnID, &pSocketObj) != TUdpSocketObjPtrPool::GR_VALID)
		pSocketObj = nullptr;

	return pSocketObj;
}

CONNID CUdpServer::FindConnectionID(const HP_SOCKADDR* pAddr)
{
	CONNID dwConnID = 0;

	CReadLock locallock(m_csClientSocket);

	TSockAddrMapCI it = m_mpClientAddr.find(pAddr);
	if(it != m_mpClientAddr.end())
		dwConnID = it->second;

	return dwConnID;
}

void CUdpServer::CloseClientSocketObj(TUdpSocketObj* pSocketObj, EnSocketCloseFlag enFlag, EnSocketOperation enOperation, int iErrorCode)
{
	ASSERT(TUdpSocketObj::IsExist(pSocketObj));

	if(enFlag == SCF_CLOSE)
		FireClose(pSocketObj, SO_CLOSE, SE_OK);
	else if(enFlag == SCF_ERROR)
		FireClose(pSocketObj, enOperation, iErrorCode);
}

BOOL CUdpServer::GetListenAddress(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	return ::GetSocketLocalAddress(m_soListen, lpszAddress, iAddressLen, usPort);
}

BOOL CUdpServer::GetLocalAddress(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TUdpSocketObj::IsValid(pSocketObj))
		return ::GetSocketLocalAddress(m_soListen, lpszAddress, iAddressLen, usPort);

	return FALSE;
}

BOOL CUdpServer::GetRemoteAddress(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TUdpSocketObj::IsExist(pSocketObj))
	{
		ADDRESS_FAMILY usFamily;
		return ::sockaddr_IN_2_A(pSocketObj->remoteAddr, usFamily, lpszAddress, iAddressLen, usPort);
	}

	return FALSE;
}

BOOL CUdpServer::SetConnectionExtra(CONNID dwConnID, PVOID pExtra)
{
	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return SetConnectionExtra(pSocketObj, pExtra);
}

BOOL CUdpServer::SetConnectionExtra(TUdpSocketObj* pSocketObj, PVOID pExtra)
{
	if(TUdpSocketObj::IsExist(pSocketObj))
	{
		pSocketObj->extra = pExtra;
		return TRUE;
	}

	return FALSE;
}

BOOL CUdpServer::GetConnectionExtra(CONNID dwConnID, PVOID* ppExtra)
{
	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return GetConnectionExtra(pSocketObj, ppExtra);
}

BOOL CUdpServer::GetConnectionExtra(TUdpSocketObj* pSocketObj, PVOID* ppExtra)
{
	ASSERT(ppExtra != nullptr);

	if(TUdpSocketObj::IsExist(pSocketObj))
	{
		*ppExtra = pSocketObj->extra;
		return TRUE;
	}

	return FALSE;
}

BOOL CUdpServer::SetConnectionReserved(CONNID dwConnID, PVOID pReserved)
{
	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return SetConnectionReserved(pSocketObj, pReserved);
}

BOOL CUdpServer::SetConnectionReserved(TUdpSocketObj* pSocketObj, PVOID pReserved)
{
	if(TUdpSocketObj::IsExist(pSocketObj))
	{
		pSocketObj->reserved = pReserved;
		return TRUE;
	}

	return FALSE;
}

BOOL CUdpServer::GetConnectionReserved(CONNID dwConnID, PVOID* ppReserved)
{
	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return GetConnectionReserved(pSocketObj, ppReserved);
}

BOOL CUdpServer::GetConnectionReserved(TUdpSocketObj* pSocketObj, PVOID* ppReserved)
{
	ASSERT(ppReserved != nullptr);

	if(TUdpSocketObj::IsExist(pSocketObj))
	{
		*ppReserved = pSocketObj->reserved;
		return TRUE;
	}

	return FALSE;
}

BOOL CUdpServer::SetConnectionReserved2(CONNID dwConnID, PVOID pReserved2)
{
	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return SetConnectionReserved2(pSocketObj, pReserved2);
}

BOOL CUdpServer::SetConnectionReserved2(TUdpSocketObj* pSocketObj, PVOID pReserved2)
{
	if(TUdpSocketObj::IsExist(pSocketObj))
	{
		pSocketObj->reserved2 = pReserved2;
		return TRUE;
	}

	return FALSE;
}

BOOL CUdpServer::GetConnectionReserved2(CONNID dwConnID, PVOID* ppReserved2)
{
	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return GetConnectionReserved2(pSocketObj, ppReserved2);
}

BOOL CUdpServer::GetConnectionReserved2(TUdpSocketObj* pSocketObj, PVOID* ppReserved2)
{
	ASSERT(ppReserved2 != nullptr);

	if(TUdpSocketObj::IsExist(pSocketObj))
	{
		*ppReserved2 = pSocketObj->reserved2;
		return TRUE;
	}

	return FALSE;
}

BOOL CUdpServer::IsPauseReceive(CONNID dwConnID, BOOL& bPaused)
{
	::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

	bPaused = FALSE;

	return FALSE;
}

BOOL CUdpServer::IsConnected(CONNID dwConnID)
{
	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TUdpSocketObj::IsValid(pSocketObj))
		return pSocketObj->HasConnected();

	return FALSE;
}

BOOL CUdpServer::GetPendingDataLength(CONNID dwConnID, int& iPending)
{
	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TUdpSocketObj::IsValid(pSocketObj))
	{
		iPending = pSocketObj->Pending();
		return TRUE;
	}

	return FALSE;
}

DWORD CUdpServer::GetConnectionCount()
{
	return m_bfActiveSockets.Elements();
}

BOOL CUdpServer::GetAllConnectionIDs(CONNID pIDs[], DWORD& dwCount)
{
	return m_bfActiveSockets.GetAllElementIndexes(pIDs, dwCount);
}

BOOL CUdpServer::GetConnectPeriod(CONNID dwConnID, DWORD& dwPeriod)
{
	BOOL isOK					= TRUE;
	TUdpSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TUdpSocketObj::IsValid(pSocketObj))
		dwPeriod = ::GetTimeGap32(pSocketObj->connTime);
	else
		isOK = FALSE;

	return isOK;
}

BOOL CUdpServer::GetSilencePeriod(CONNID dwConnID, DWORD& dwPeriod)
{
	if(!m_bMarkSilence)
		return FALSE;

	BOOL isOK					= TRUE;
	TUdpSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TUdpSocketObj::IsValid(pSocketObj))
		dwPeriod = ::GetTimeGap32(pSocketObj->activeTime);
	else
		isOK = FALSE;

	return isOK;
}

BOOL CUdpServer::Disconnect(CONNID dwConnID, BOOL bForce)
{
	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(!TUdpSocketObj::IsValid(pSocketObj))
		return FALSE;

	return VERIFY(m_ioDispatcher.SendCommand(DISP_CMD_DISCONNECT, dwConnID, bForce));
}

BOOL CUdpServer::DisconnectLongConnections(DWORD dwPeriod, BOOL bForce)
{
	if(dwPeriod > MAX_CONNECTION_PERIOD)
		return FALSE;

	DWORD size					= 0;
	unique_ptr<CONNID[]> ids	= m_bfActiveSockets.GetAllElementIndexes(size);
	DWORD now					= ::TimeGetTime();

	for(DWORD i = 0; i < size; i++)
	{
		CONNID connID				= ids[i];
		TUdpSocketObj* pSocketObj	= FindSocketObj(connID);

		if(TUdpSocketObj::IsValid(pSocketObj) && (int)(now - pSocketObj->connTime) >= (int)dwPeriod)
			Disconnect(connID, bForce);
	}

	return TRUE;
}

BOOL CUdpServer::DisconnectSilenceConnections(DWORD dwPeriod, BOOL bForce)
{
	if(!m_bMarkSilence)
		return FALSE;
	if(dwPeriod > MAX_CONNECTION_PERIOD)
		return FALSE;

	DWORD size					= 0;
	unique_ptr<CONNID[]> ids	= m_bfActiveSockets.GetAllElementIndexes(size);
	DWORD now					= ::TimeGetTime();

	for(DWORD i = 0; i < size; i++)
	{
		CONNID connID				= ids[i];
		TUdpSocketObj* pSocketObj	= FindSocketObj(connID);

		if(TUdpSocketObj::IsValid(pSocketObj) && (int)(now - pSocketObj->activeTime) >= (int)dwPeriod)
			Disconnect(connID, bForce);
	}

	return TRUE;
}

BOOL CUdpServer::PauseReceive(CONNID dwConnID, BOOL bPause)
{
	::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

BOOL CUdpServer::OnBeforeProcessIo(PVOID pv, UINT events)
{
	ASSERT(pv == &m_soListen);

	return TRUE;
}

VOID CUdpServer::OnAfterProcessIo(PVOID pv, UINT events, BOOL rs)
{

}

VOID CUdpServer::OnCommand(TDispCommand* pCmd)
{
	switch(pCmd->type)
	{
	case DISP_CMD_SEND:
		HandleCmdSend((CONNID)(pCmd->wParam), (int)(pCmd->lParam));
		break;
	case DISP_CMD_RECEIVE:
		HandleCmdReceive((CONNID)(pCmd->wParam), (int)(pCmd->lParam));
		break;
	case DISP_CMD_DISCONNECT:
		HandleCmdDisconnect((CONNID)(pCmd->wParam), (BOOL)pCmd->lParam);
		break;
	}
}

VOID CUdpServer::HandleCmdSend(CONNID dwConnID, int flag)
{
	DoSend(dwConnID, flag);
}

VOID CUdpServer::HandleCmdReceive(CONNID dwConnID, int flag)
{
	DoReceive(dwConnID, flag);
}

VOID CUdpServer::HandleCmdDisconnect(CONNID dwConnID, BOOL bForce)
{
	AddFreeSocketObj(FindSocketObj(dwConnID), SCF_CLOSE);
}

BOOL CUdpServer::OnReadyRead(PVOID pv, UINT events)
{
	return HandleReceive(RETRIVE_EVENT_FLAG_H(events));
}

BOOL CUdpServer::OnReadyWrite(PVOID pv, UINT events)
{
	return HandleSend(RETRIVE_EVENT_FLAG_H(events));
}

BOOL CUdpServer::OnHungUp(PVOID pv, UINT events)
{
	return HandleClose();
}

BOOL CUdpServer::OnError(PVOID pv, UINT events)
{
	return HandleClose();
}

VOID CUdpServer::OnDispatchThreadEnd(THR_ID tid)
{
	OnWorkerThreadEnd(tid);
}

BOOL CUdpServer::HandleClose()
{
	VERIFY(!HasStarted());

	m_ioDispatcher.DelFD(m_soListen);

	return FALSE;
}

BOOL CUdpServer::HandleReceive(int flag)
{
	while(TRUE)
	{
		HP_SOCKADDR addr;
		socklen_t dwAddrLen = (socklen_t)addr.AddrSize();

		TItemPtr itPtr(m_bfObjPool, m_bfObjPool.PickFreeItem());
		int iBufferLen = itPtr->Capacity();

		int rc = (int)recvfrom(m_soListen, itPtr->Ptr(), iBufferLen, MSG_TRUNC, addr.Addr(), &dwAddrLen);

		if(rc >= 0)
		{
			CONNID dwConnID = FindConnectionID(&addr);

			if(dwConnID == 0)
			{
				if((dwConnID = HandleAccept(addr)) == 0)
					continue;
			}

			if(rc > iBufferLen)
				continue;

			TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

			if(!TUdpSocketObj::IsValid(pSocketObj))
				continue;

			if(rc == 0)
			{
				HandleZeroBytes(pSocketObj);
				continue;
			}

			itPtr->Increase(rc);
			
			{
				CReadLock locallock(pSocketObj->lcIo);

				if(!TUdpSocketObj::IsValid(pSocketObj))
					continue;

				pSocketObj->recvQueue.PushBack(itPtr.Detach());
			}

			VERIFY(m_ioDispatcher.SendCommand(DISP_CMD_RECEIVE, dwConnID, flag));
		}
		else
		{
			BREAK_WOULDBLOCK_ERROR();

			HandleClose();
			return FALSE;
		}
	}

	return TRUE;
}

CONNID CUdpServer::HandleAccept(HP_SOCKADDR& addr)
{
	CONNID dwConnID				= 0;
	TUdpSocketObj* pSocketObj	= nullptr;

	{
		CCriSecLock locallock(m_csAccept);

		dwConnID = FindConnectionID(&addr);

		if(dwConnID != 0)
			return dwConnID;
		else
		{
			if(!m_bfActiveSockets.AcquireLock(dwConnID))
				return 0;

			pSocketObj = GetFreeSocketObj(dwConnID);
			pSocketObj->lcIo.lock();
		}
	}

	AddClientSocketObj(dwConnID, pSocketObj, addr);

	if(TriggerFireAccept(pSocketObj) == HR_ERROR)
	{
		AddFreeSocketObj(pSocketObj);
		return 0;
	}

	return dwConnID;
}

void CUdpServer::HandleZeroBytes(TUdpSocketObj* pSocketObj)
{
	TRACE("<S-CNNID: %zu> recv 0 bytes (detect package)", pSocketObj->connID);

	pSocketObj->detectFails = 0;

#if defined(DEBUG_TRACE)
	int rc = (int)
#endif
	sendto(m_soListen, nullptr, 0, 0, pSocketObj->remoteAddr.Addr(), pSocketObj->remoteAddr.AddrSize());

	TRACE("<S-CNNID: %zu> send 0 bytes (detect ack package - %s)", pSocketObj->connID, IS_HAS_ERROR(rc) ? "fail" : "succ");
}

BOOL CUdpServer::DoReceive(CONNID dwConnID, int flag)
{
	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(!TUdpSocketObj::IsValid(pSocketObj))
		return FALSE;

	CReadLock locallock(pSocketObj->lcIo);

	if(!TUdpSocketObj::IsValid(pSocketObj))
		return FALSE;

	if(m_bMarkSilence) pSocketObj->activeTime = ::TimeGetTime();

	int reads = flag ? -1 : MAX_CONTINUE_READS;

	for(int i = 0; i < reads || reads < 0; i++)
	{
		TItemPtr itPtr(m_bfObjPool);

		if(!pSocketObj->recvQueue.PopFront(&itPtr.PtrRef()))
			break;

		if(TRIGGER(FireReceive(pSocketObj, itPtr->Ptr(), itPtr->Size())) == HR_ERROR)
		{
			TRACE("<S-CNNID: %zu> OnReceive() event return 'HR_ERROR', connection will be closed !", dwConnID);

			AddFreeSocketObj(pSocketObj, SCF_ERROR, SO_RECEIVE, ENSURE_ERROR_CANCELLED);
			return FALSE;
		}
	}

	if(TUdpSocketObj::IsValid(pSocketObj) && pSocketObj->HasRecvData())
		VERIFY(m_ioDispatcher.SendCommand(DISP_CMD_RECEIVE, dwConnID, flag));

	return TRUE;
}

BOOL CUdpServer::HandleSend(int flag)
{
	m_ioDispatcher.ModFD(m_soListen, _EPOLL_READ_EVENTS | EPOLLET, TO_PVOID(&m_soListen));

	CONNID dwConnID = 0;

	while(m_quSend.PopFront(&dwConnID))
		VERIFY(m_ioDispatcher.SendCommand(DISP_CMD_SEND, dwConnID));

	return TRUE;
}

BOOL CUdpServer::DoSend(CONNID dwConnID, int flag)
{
	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(!TUdpSocketObj::IsValid(pSocketObj) || !pSocketObj->IsPending())
		return FALSE;

	CReentrantCriSecLock locallock(pSocketObj->csSend);

	if(!TUdpSocketObj::IsValid(pSocketObj) || !pSocketObj->IsPending())
		return FALSE;

	int writes = flag ? -1 : MAX_CONTINUE_WRITES;
	TBufferObjList& sndBuff = pSocketObj->sndBuff;

	BOOL bBlocked = FALSE;

	for(int i = 0; i < writes || writes < 0; i++)
	{
		TItemPtr itPtr(sndBuff, sndBuff.PopFront());

		if(!itPtr.IsValid())
			break;

		ASSERT(!itPtr->IsEmpty());

		if(!SendItem(pSocketObj, itPtr, bBlocked))
		{
			HandleClose();
			return FALSE;
		}

		if(bBlocked)
		{
			sndBuff.PushFront(itPtr.Detach());
			m_quSend.PushBack(dwConnID);

			m_ioDispatcher.ModFD(m_soListen, EPOLLOUT | _EPOLL_READ_EVENTS | EPOLLET, TO_PVOID(&m_soListen));

			break;
		}
	}

	if(!bBlocked && !sndBuff.IsEmpty())
		VERIFY(m_ioDispatcher.SendCommand(DISP_CMD_SEND, dwConnID));

	return TRUE;
}

BOOL CUdpServer::SendItem(TUdpSocketObj* pSocketObj, TItem* pItem, BOOL& bBlocked)
{
	int rc = (int)sendto(m_soListen, pItem->Ptr(), pItem->Size(), 0, pSocketObj->remoteAddr.Addr(), pSocketObj->remoteAddr.AddrSize());

	if(rc > 0)
	{
		ASSERT(rc == pItem->Size());

		if(TRIGGER(FireSend(pSocketObj, pItem->Ptr(), rc)) == HR_ERROR)
		{
			TRACE("<C-CNNID: %zu> OnSend() event should not return 'HR_ERROR' !!", pSocketObj->connID);
			ASSERT(FALSE);
		}
	}
	else if(rc == SOCKET_ERROR)
	{
		int code = ::WSAGetLastError();

		if(code == ERROR_WOULDBLOCK)
			bBlocked = TRUE;
		else
			return FALSE;
	}
	else
		ASSERT(FALSE);

	return TRUE;
}

BOOL CUdpServer::Send(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset)
{
	ASSERT(pBuffer && iLength > 0 && iLength <= (int)m_dwMaxDatagramSize);

	int result = NO_ERROR;

	if(pBuffer && iLength > 0 && iLength <= (int)m_dwMaxDatagramSize)
	{
		if(iOffset != 0) pBuffer += iOffset;
		TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

		if(TUdpSocketObj::IsValid(pSocketObj))
		{
			TItemPtr itPtr(m_bfObjPool, m_bfObjPool.PickFreeItem());
			itPtr->Cat(pBuffer, iLength);

			result = SendInternal(pSocketObj, itPtr);
		}
		else
			result = ERROR_OBJECT_NOT_FOUND;
	}
	else
		result = ERROR_INVALID_PARAMETER;

	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);
}

BOOL CUdpServer::SendPackets(CONNID dwConnID, const WSABUF pBuffers[], int iCount)
{
	ASSERT(pBuffers && iCount > 0);

	if(!pBuffers || iCount <= 0)
		return ERROR_INVALID_PARAMETER;;

	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(!TUdpSocketObj::IsValid(pSocketObj))
	{
		::SetLastError(ERROR_OBJECT_NOT_FOUND);
		return FALSE;
	}

	int result	= NO_ERROR;
	int iLength = 0;
	int iMaxLen = (int)m_dwMaxDatagramSize;
	
	TItemPtr itPtr(m_bfObjPool, m_bfObjPool.PickFreeItem());

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
		result = SendInternal(pSocketObj, itPtr);
	else
		result = ERROR_INCORRECT_SIZE;

	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);

}

int CUdpServer::SendInternal(TUdpSocketObj* pSocketObj, TItemPtr& itPtr)
{
	BOOL bPending = TRUE;

	{
		CReentrantCriSecLock locallock(pSocketObj->csSend);

		if(!TUdpSocketObj::IsValid(pSocketObj))
			return ERROR_OBJECT_NOT_FOUND;

		bPending = pSocketObj->IsPending();

		pSocketObj->sndBuff.PushBack(itPtr.Detach());
	}

	if(!bPending)
		VERIFY(m_ioDispatcher.SendCommand(DISP_CMD_SEND, pSocketObj->connID));

	return NO_ERROR;
}

UINT WINAPI CUdpServer::DetecotrThreadProc(LPVOID pv)
{
	ASSERT(IsNeedRunDetector());

	TRACE("---------------> Server Detecotr Thread 0x%08X started <---------------", SELF_THREAD_ID);

	pollfd pfd = {m_evStop.GetFD(), POLLIN};

	while(HasStarted())
	{
		int rs = (int)::PollForSingleObject(pfd, m_dwDetectInterval * 1000L);
		ASSERT(rs >= TIMEOUT);

		if(rs < TIMEOUT)
			ERROR_ABORT();

		if(rs == TIMEOUT)
			DetectConnections();
		else if(rs == 1)
		{
			m_evStop.Reset();
			goto END_DETECTOR;
		}
		else
			ASSERT(FALSE);
	}

END_DETECTOR:

	VERIFY(!HasStarted());

	TRACE("---------------> Server Detecotr Thread 0x%08X stoped <---------------", SELF_THREAD_ID);

	return 0;
}

void CUdpServer::DetectConnections()
{
	DWORD size					= 0;
	unique_ptr<CONNID[]> ids	= m_bfActiveSockets.GetAllElementIndexes(size);

	for(DWORD i = 0; i < size; i++)
	{
		CONNID dwConnID			  = ids[i];
		TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

		if(pSocketObj)
		{
			if(pSocketObj->detectFails >= m_dwDetectAttempts)
				VERIFY(m_ioDispatcher.SendCommand(DISP_CMD_DISCONNECT, dwConnID, TRUE));
			else
				::InterlockedIncrement(&pSocketObj->detectFails);
		}
	}
}
