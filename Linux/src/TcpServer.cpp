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
 
#include "TcpServer.h"

#include "./common/FileHelper.h"

BOOL CTcpServer::Start(LPCTSTR lpszBindAddress, USHORT usPort)
{
	if(!CheckParams() || !CheckStarting())
		return FALSE;

	PrepareStart();

	if(CreateListenSocket(lpszBindAddress, usPort))
		if(CreateWorkerThreads())
			if(StartAccept())
			{
				m_enState = SS_STARTED;
				return TRUE;
			}

	EXECUTE_RESTORE_ERROR(Stop());

	return FALSE;
}

void CTcpServer::SetLastError(EnSocketError code, LPCSTR func, int ec)
{
	m_enLastError = code;
	::SetLastError(ec);
}

BOOL CTcpServer::CheckParams()
{
	if	((m_enSendPolicy >= SP_PACK && m_enSendPolicy <= SP_DIRECT)								&&
		((int)m_dwMaxConnectionCount > 0)														&&
		((int)m_dwWorkerThreadCount > 0 && m_dwWorkerThreadCount <= MAX_WORKER_THREAD_COUNT)	&&
		((int)m_dwAcceptSocketCount > 0)														&&
		((int)m_dwSocketBufferSize >= MIN_SOCKET_BUFFER_SIZE)									&&
		((int)m_dwSocketListenQueue > 0)														&&
		((int)m_dwFreeSocketObjLockTime >= 0)													&&
		((int)m_dwFreeSocketObjPool >= 0)														&&
		((int)m_dwFreeBufferObjPool >= 0)														&&
		((int)m_dwFreeSocketObjHold >= 0)														&&
		((int)m_dwFreeBufferObjHold >= 0)														&&
		((int)m_dwKeepAliveTime >= 1000 || m_dwKeepAliveTime == 0)								&&
		((int)m_dwKeepAliveInterval >= 1000 || m_dwKeepAliveInterval == 0)						)
		return TRUE;

	SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
	return FALSE;
}

void CTcpServer::PrepareStart()
{
	m_bfActiveSockets.Reset(m_dwMaxConnectionCount);
	m_lsFreeSocket.Reset(m_dwFreeSocketObjPool);

	m_bfObjPool.SetItemCapacity(m_dwSocketBufferSize);
	m_bfObjPool.SetPoolSize(m_dwFreeBufferObjPool);
	m_bfObjPool.SetPoolHold(m_dwFreeBufferObjHold);

	m_bfObjPool.Prepare();
}

BOOL CTcpServer::CheckStarting()
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

BOOL CTcpServer::CheckStoping()
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

BOOL CTcpServer::CreateListenSocket(LPCTSTR lpszBindAddress, USHORT usPort)
{
	BOOL isOK = FALSE;

	if(::IsStrEmpty(lpszBindAddress))
		lpszBindAddress = DEFAULT_IPV4_BIND_ADDRESS;

	HP_SOCKADDR addr;

	if(::sockaddr_A_2_IN(lpszBindAddress, usPort, addr))
	{
		m_soListen = socket(addr.family, SOCK_STREAM, IPPROTO_TCP);

		if(m_soListen != INVALID_SOCKET)
		{
			::fcntl_SETFL(m_soListen, O_NOATIME | O_NONBLOCK | O_CLOEXEC);

			BOOL bOnOff	= (m_dwKeepAliveTime > 0 && m_dwKeepAliveInterval > 0);
			VERIFY(IS_NO_ERROR(::SSO_KeepAliveVals(m_soListen, bOnOff, m_dwKeepAliveTime, m_dwKeepAliveInterval)));
			VERIFY(IS_NO_ERROR(::SSO_ReuseAddress(m_soListen)));

			if(::bind(m_soListen, addr.Addr(), addr.AddrSize()) != SOCKET_ERROR)
			{
				if(TRIGGER(FirePrepareListen(m_soListen)) != HR_ERROR)
				{
					if(::listen(m_soListen, m_dwSocketListenQueue) != SOCKET_ERROR)
					{
						isOK = TRUE;
					}
					else
						SetLastError(SE_SOCKET_LISTEN, __FUNCTION__, ::WSAGetLastError());
				}
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

BOOL CTcpServer::CreateWorkerThreads()
{
	if(!m_ioDispatcher.Start(this, m_dwAcceptSocketCount, m_dwWorkerThreadCount))
		return FALSE;

	const CIODispatcher::CWorkerThread* pWorkerThread = m_ioDispatcher.GetWorkerThreads();

	for(DWORD i = 0; i < m_dwWorkerThreadCount; i++)
		m_rcBufferMap[pWorkerThread[i].GetThreadID()] = new CBufferPtr(m_dwSocketBufferSize);

	return TRUE;
}

BOOL CTcpServer::StartAccept()
{
	return m_ioDispatcher.AddFD(m_soListen, _EPOLL_READ_EVENTS | EPOLLET, TO_PVOID(&m_soListen));
}

BOOL CTcpServer::Stop()
{
	if(!CheckStoping())
		return FALSE;

	CloseListenSocket();

	DisconnectClientSocket();
	WaitForClientSocketClose();
	WaitForWorkerThreadEnd();
	
	ReleaseClientSocket();

	FireShutdown();

	ReleaseFreeSocket();

	Reset();

	return TRUE;
}

void CTcpServer::CloseListenSocket()
{
	if(m_soListen != INVALID_SOCKET)
	{
		::ManualCloseSocket(m_soListen);
		m_soListen = INVALID_SOCKET;

		::WaitFor(100);
	}
}

void CTcpServer::DisconnectClientSocket()
{
	DWORD size					= 0;
	unique_ptr<CONNID[]> ids	= m_bfActiveSockets.GetAllElementIndexes(size);

	for(DWORD i = 0; i < size; i++)
		Disconnect(ids[i]);
}

void CTcpServer::WaitForClientSocketClose()
{
	while(m_bfActiveSockets.Elements() > 0)
		::WaitFor(50);
}

void CTcpServer::WaitForWorkerThreadEnd()
{
	m_ioDispatcher.Stop();
}

void CTcpServer::ReleaseClientSocket()
{
	VERIFY(m_bfActiveSockets.IsEmpty());
	m_bfActiveSockets.Reset();
}

void CTcpServer::ReleaseFreeSocket()
{
	TSocketObj* pSocketObj = nullptr;

	while(m_lsFreeSocket.TryGet(&pSocketObj))
		DeleteSocketObj(pSocketObj);

	VERIFY(m_lsFreeSocket.IsEmpty());
	m_lsFreeSocket.Reset();

	ReleaseGCSocketObj(TRUE);
	VERIFY(m_lsGCSocket.IsEmpty());
}

void CTcpServer::Reset()
{
	m_phSocket.Reset();
	m_bfObjPool.Clear();

	::ClearPtrMap(m_rcBufferMap);

	m_enState = SS_STOPPED;
}

TSocketObj* CTcpServer::GetFreeSocketObj(CONNID dwConnID, SOCKET soClient)
{
	DWORD dwIndex;
	TSocketObj* pSocketObj = nullptr;

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
	pSocketObj->Reset(dwConnID, soClient);

	return pSocketObj;
}

TSocketObj* CTcpServer::CreateSocketObj()
{
	return TSocketObj::Construct(m_phSocket, m_bfObjPool);
}

void CTcpServer::DeleteSocketObj(TSocketObj* pSocketObj)
{
	TSocketObj::Destruct(pSocketObj);
}

void CTcpServer::AddFreeSocketObj(TSocketObj* pSocketObj, EnSocketCloseFlag enFlag, EnSocketOperation enOperation, int iErrorCode)
{
	if(!InvalidSocketObj(pSocketObj))
		return;

	CloseClientSocketObj(pSocketObj, enFlag, enOperation, iErrorCode);

	m_bfActiveSockets.Remove(pSocketObj->connID);
	TSocketObj::Release(pSocketObj);

	ReleaseGCSocketObj();

	if(!m_lsFreeSocket.TryPut(pSocketObj))
		m_lsGCSocket.PushBack(pSocketObj);
}

void CTcpServer::ReleaseGCSocketObj(BOOL bForce)
{
	::ReleaseGCObj(m_lsGCSocket, m_dwFreeSocketObjLockTime, bForce);
}

BOOL CTcpServer::InvalidSocketObj(TSocketObj* pSocketObj)
{
	return TSocketObj::InvalidSocketObj(pSocketObj);
}

void CTcpServer::AddClientSocketObj(CONNID dwConnID, TSocketObj* pSocketObj, const HP_SOCKADDR& remoteAddr)
{
	ASSERT(FindSocketObj(dwConnID) == nullptr);

	pSocketObj->connTime	= ::TimeGetTime();
	pSocketObj->activeTime	= pSocketObj->connTime;

	remoteAddr.Copy(pSocketObj->remoteAddr);
	pSocketObj->SetConnected();

	VERIFY(m_bfActiveSockets.ReleaseLock(dwConnID, pSocketObj));
}

TSocketObj* CTcpServer::FindSocketObj(CONNID dwConnID)
{
	TSocketObj* pSocketObj = nullptr;

	if(m_bfActiveSockets.Get(dwConnID, &pSocketObj) != TSocketObjPtrPool::GR_VALID)
		pSocketObj = nullptr;

	return pSocketObj;
}

void CTcpServer::CloseClientSocketObj(TSocketObj* pSocketObj, EnSocketCloseFlag enFlag, EnSocketOperation enOperation, int iErrorCode, int iShutdownFlag)
{
	ASSERT(TSocketObj::IsExist(pSocketObj));

	if(enFlag == SCF_CLOSE)
		FireClose(pSocketObj, SO_CLOSE, SE_OK);
	else if(enFlag == SCF_ERROR)
		FireClose(pSocketObj, enOperation, iErrorCode);

	SOCKET socket = pSocketObj->socket;
	pSocketObj->socket = INVALID_SOCKET;

	::ManualCloseSocket(socket, iShutdownFlag);
}

BOOL CTcpServer::GetListenAddress(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	return ::GetSocketLocalAddress(m_soListen, lpszAddress, iAddressLen, usPort);
}

BOOL CTcpServer::GetLocalAddress(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
		return ::GetSocketLocalAddress(pSocketObj->socket, lpszAddress, iAddressLen, usPort);

	return FALSE;
}

BOOL CTcpServer::GetRemoteAddress(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsExist(pSocketObj))
	{
		ADDRESS_FAMILY usFamily;
		return ::sockaddr_IN_2_A(pSocketObj->remoteAddr, usFamily, lpszAddress, iAddressLen, usPort);
	}

	return FALSE;
}

BOOL CTcpServer::SetConnectionExtra(CONNID dwConnID, PVOID pExtra)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return SetConnectionExtra(pSocketObj, pExtra);
}

BOOL CTcpServer::SetConnectionExtra(TSocketObj* pSocketObj, PVOID pExtra)
{
	if(TSocketObj::IsExist(pSocketObj))
	{
		pSocketObj->extra = pExtra;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpServer::GetConnectionExtra(CONNID dwConnID, PVOID* ppExtra)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return GetConnectionExtra(pSocketObj, ppExtra);
}

BOOL CTcpServer::GetConnectionExtra(TSocketObj* pSocketObj, PVOID* ppExtra)
{
	ASSERT(ppExtra != nullptr);

	if(TSocketObj::IsExist(pSocketObj))
	{
		*ppExtra = pSocketObj->extra;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpServer::SetConnectionReserved(CONNID dwConnID, PVOID pReserved)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return SetConnectionReserved(pSocketObj, pReserved);
}

BOOL CTcpServer::SetConnectionReserved(TSocketObj* pSocketObj, PVOID pReserved)
{
	if(TSocketObj::IsExist(pSocketObj))
	{
		pSocketObj->reserved = pReserved;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpServer::GetConnectionReserved(CONNID dwConnID, PVOID* ppReserved)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return GetConnectionReserved(pSocketObj, ppReserved);
}

BOOL CTcpServer::GetConnectionReserved(TSocketObj* pSocketObj, PVOID* ppReserved)
{
	ASSERT(ppReserved != nullptr);

	if(TSocketObj::IsExist(pSocketObj))
	{
		*ppReserved = pSocketObj->reserved;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpServer::SetConnectionReserved2(CONNID dwConnID, PVOID pReserved2)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return SetConnectionReserved2(pSocketObj, pReserved2);
}

BOOL CTcpServer::SetConnectionReserved2(TSocketObj* pSocketObj, PVOID pReserved2)
{
	if(TSocketObj::IsExist(pSocketObj))
	{
		pSocketObj->reserved2 = pReserved2;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpServer::GetConnectionReserved2(CONNID dwConnID, PVOID* ppReserved2)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return GetConnectionReserved2(pSocketObj, ppReserved2);
}

BOOL CTcpServer::GetConnectionReserved2(TSocketObj* pSocketObj, PVOID* ppReserved2)
{
	ASSERT(ppReserved2 != nullptr);

	if(TSocketObj::IsExist(pSocketObj))
	{
		*ppReserved2 = pSocketObj->reserved2;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpServer::IsPauseReceive(CONNID dwConnID, BOOL& bPaused)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
	{
		bPaused = pSocketObj->paused;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpServer::IsConnected(CONNID dwConnID)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
		return pSocketObj->HasConnected();

	return FALSE;
}

BOOL CTcpServer::GetPendingDataLength(CONNID dwConnID, int& iPending)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
	{
		iPending = pSocketObj->Pending();
		return TRUE;
	}

	return FALSE;
}

DWORD CTcpServer::GetConnectionCount()
{
	return m_bfActiveSockets.Elements();
}

BOOL CTcpServer::GetAllConnectionIDs(CONNID pIDs[], DWORD& dwCount)
{
	return m_bfActiveSockets.GetAllElementIndexes(pIDs, dwCount);
}

BOOL CTcpServer::GetConnectPeriod(CONNID dwConnID, DWORD& dwPeriod)
{
	BOOL isOK				= TRUE;
	TSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
		dwPeriod = ::GetTimeGap32(pSocketObj->connTime);
	else
		isOK = FALSE;

	return isOK;
}

BOOL CTcpServer::GetSilencePeriod(CONNID dwConnID, DWORD& dwPeriod)
{
	if(!m_bMarkSilence)
		return FALSE;

	BOOL isOK				= TRUE;
	TSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
		dwPeriod = ::GetTimeGap32(pSocketObj->activeTime);
	else
		isOK = FALSE;

	return isOK;
}

BOOL CTcpServer::Disconnect(CONNID dwConnID, BOOL bForce)
{
	BOOL isOK				= FALSE;
	TSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
		isOK = m_ioDispatcher.SendCommand(DISP_CMD_DISCONNECT, dwConnID, bForce);

	return isOK;
}

BOOL CTcpServer::DisconnectLongConnections(DWORD dwPeriod, BOOL bForce)
{
	if(dwPeriod > MAX_CONNECTION_PERIOD)
		return FALSE;

	DWORD size					= 0;
	unique_ptr<CONNID[]> ids	= m_bfActiveSockets.GetAllElementIndexes(size);
	DWORD now					= ::TimeGetTime();

	for(DWORD i = 0; i < size; i++)
	{
		CONNID connID			= ids[i];
		TSocketObj* pSocketObj	= FindSocketObj(connID);

		if(TSocketObj::IsValid(pSocketObj) && (int)(now - pSocketObj->connTime) >= (int)dwPeriod)
			Disconnect(connID, bForce);
	}

	return TRUE;
}

BOOL CTcpServer::DisconnectSilenceConnections(DWORD dwPeriod, BOOL bForce)
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
		CONNID connID			= ids[i];
		TSocketObj* pSocketObj	= FindSocketObj(connID);

		if(TSocketObj::IsValid(pSocketObj) && (int)(now - pSocketObj->activeTime) >= (int)dwPeriod)
			Disconnect(connID, bForce);
	}

	return TRUE;
}

BOOL CTcpServer::PauseReceive(CONNID dwConnID, BOOL bPause)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(!TSocketObj::IsValid(pSocketObj))
	{
		::SetLastError(ERROR_OBJECT_NOT_FOUND);
		return FALSE;
	}

	if(pSocketObj->paused == bPause)
		return TRUE;

	pSocketObj->paused = bPause;

	if(!bPause)
		return m_ioDispatcher.SendCommand(DISP_CMD_UNPAUSE, pSocketObj->connID);

	return TRUE;
}

BOOL CTcpServer::OnBeforeProcessIo(PVOID pv, UINT events)
{
	if(pv == &m_soListen)
	{
		HandleAccept(events);
		return FALSE;
	}

	TSocketObj* pSocketObj = (TSocketObj*)(pv);

	if(!TSocketObj::IsValid(pSocketObj))
		return FALSE;

	if(events & _EPOLL_ALL_ERROR_EVENTS)
		pSocketObj->SetConnected(FALSE);

	pSocketObj->csIo.lock();

	if(!TSocketObj::IsValid(pSocketObj))
	{
		pSocketObj->csIo.unlock();
		return FALSE;
	}

	return TRUE;
}

VOID CTcpServer::OnAfterProcessIo(PVOID pv, UINT events, BOOL rs)
{
	TSocketObj* pSocketObj = (TSocketObj*)(pv);

	if(TSocketObj::IsValid(pSocketObj))
	{
		ASSERT(rs && !(events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)));

		UINT evts = (pSocketObj->IsPending() ? EPOLLOUT : 0) | (pSocketObj->IsPaused() ? 0 : EPOLLIN);
		m_ioDispatcher.ModFD(pSocketObj->socket, evts | EPOLLRDHUP | EPOLLONESHOT, pSocketObj);
	}

	pSocketObj->csIo.unlock();
}

VOID CTcpServer::OnCommand(TDispCommand* pCmd)
{
	switch(pCmd->type)
	{
	case DISP_CMD_SEND:
		HandleCmdSend((CONNID)(pCmd->wParam));
		break;
	case DISP_CMD_UNPAUSE:
		HandleCmdUnpause((CONNID)(pCmd->wParam));
		break;
	case DISP_CMD_DISCONNECT:
		HandleCmdDisconnect((CONNID)(pCmd->wParam), (BOOL)pCmd->lParam);
		break;
	}
}

VOID CTcpServer::HandleCmdSend(CONNID dwConnID)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj) && pSocketObj->IsPending())
		m_ioDispatcher.ProcessIo(pSocketObj, EPOLLOUT);
}

VOID CTcpServer::HandleCmdUnpause(CONNID dwConnID)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj) && !pSocketObj->IsPaused())
		m_ioDispatcher.ProcessIo(pSocketObj, EPOLLIN);
}

VOID CTcpServer::HandleCmdDisconnect(CONNID dwConnID, BOOL bForce)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
	{
		if(bForce)
			m_ioDispatcher.ProcessIo(pSocketObj, EPOLLHUP);
		else
			::shutdown(pSocketObj->socket, SHUT_WR);
	}
}

BOOL CTcpServer::OnReadyRead(PVOID pv, UINT events)
{
	return HandleReceive((TSocketObj*)pv, RETRIVE_EVENT_FLAG_H(events));
}

BOOL CTcpServer::OnReadyWrite(PVOID pv, UINT events)
{
	return HandleSend((TSocketObj*)pv, RETRIVE_EVENT_FLAG_H(events));
}

BOOL CTcpServer::OnHungUp(PVOID pv, UINT events)
{
	return HandleClose((TSocketObj*)pv, SCF_CLOSE, events);
}

BOOL CTcpServer::OnError(PVOID pv, UINT events)
{
	return HandleClose((TSocketObj*)pv, SCF_ERROR, events);
}

VOID CTcpServer::OnDispatchThreadEnd(THR_ID tid)
{
	OnWorkerThreadEnd(tid);
}

BOOL CTcpServer::HandleClose(TSocketObj* pSocketObj, EnSocketCloseFlag enFlag, UINT events)
{
	EnSocketOperation enOperation = SO_CLOSE;

	if(events & _EPOLL_HUNGUP_EVENTS)
		enOperation = SO_CLOSE;
	else if(events & EPOLLIN)
		enOperation = SO_RECEIVE;
	else if(events & EPOLLOUT)
		enOperation = SO_SEND;

	int iErrorCode = 0;

	if(enFlag == SCF_ERROR)
		iErrorCode = ::SSO_GetError(pSocketObj->socket);

	AddFreeSocketObj(pSocketObj, enFlag, enOperation, iErrorCode);

	return TRUE;
}

BOOL CTcpServer::HandleAccept(UINT events)
{
	if(events & _EPOLL_ALL_ERROR_EVENTS)
	{
		VERIFY(!HasStarted());
		return FALSE;
	}

	while(TRUE)
	{
		HP_SOCKADDR addr;

		socklen_t addrLen	= (socklen_t)addr.AddrSize();
		SOCKET soClient		= ::accept(m_soListen, addr.Addr(), &addrLen);

		if(soClient == INVALID_SOCKET)
		{
			int code = ::WSAGetLastError();

			if(code == ERROR_WOULDBLOCK)
				return TRUE;
			else if(code == ERROR_CONNABORTED)
				continue;
			else if(code == ERROR_HANDLES_CLOSED)
				return FALSE;
			
			ERROR_EXIT2(EXIT_CODE_SOFTWARE, code);
		}

		VERIFY(::fcntl_SETFL(soClient, O_NOATIME | O_NONBLOCK | O_CLOEXEC));

		CONNID dwConnID = 0;

		if(!m_bfActiveSockets.AcquireLock(dwConnID))
		{
			::ManualCloseSocket(soClient, SHUT_RDWR);
			continue;
		}

		TSocketObj* pSocketObj = GetFreeSocketObj(dwConnID, soClient);

		AddClientSocketObj(dwConnID, pSocketObj, addr);

		if(TRIGGER(FireAccept(pSocketObj)) == HR_ERROR)
		{
			AddFreeSocketObj(pSocketObj, SCF_NONE);
			continue;
		}

		UINT evts = (pSocketObj->IsPending() ? EPOLLOUT : 0) | (pSocketObj->IsPaused() ? 0 : EPOLLIN);
		VERIFY(m_ioDispatcher.AddFD(pSocketObj->socket, evts | EPOLLRDHUP | EPOLLONESHOT, pSocketObj));
	}

	return TRUE;
}

BOOL CTcpServer::HandleReceive(TSocketObj* pSocketObj, int flag)
{
	ASSERT(TSocketObj::IsValid(pSocketObj));

	if(m_bMarkSilence) pSocketObj->activeTime = ::TimeGetTime();

	CBufferPtr& buffer = *(m_rcBufferMap[SELF_THREAD_ID]);

	int reads = flag ? -1 : MAX_CONTINUE_READS;

	for(int i = 0; i < reads || reads < 0; i++)
	{
		int rc = (int)read(pSocketObj->socket, buffer.Ptr(), buffer.Size());

		if(rc > 0)
		{
			if(TRIGGER(FireReceive(pSocketObj, buffer.Ptr(), rc)) == HR_ERROR)
			{
				TRACE("<C-CNNID: %zu> OnReceive() event return 'HR_ERROR', connection will be closed !", pSocketObj->connID);

				AddFreeSocketObj(pSocketObj, SCF_ERROR, SO_RECEIVE, ENSURE_ERROR_CANCELLED);
				return FALSE;
			}
		}
		else if(rc == 0)
		{
			AddFreeSocketObj(pSocketObj, SCF_CLOSE, SO_RECEIVE, SE_OK);
			return FALSE;
		}
		else
		{
			ASSERT(rc == SOCKET_ERROR);

			int code = ::WSAGetLastError();

			if(code == ERROR_WOULDBLOCK)
				break;

			AddFreeSocketObj(pSocketObj, SCF_ERROR, SO_RECEIVE, code);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CTcpServer::HandleSend(TSocketObj* pSocketObj, int flag)
{
	ASSERT(TSocketObj::IsValid(pSocketObj));

	if(!pSocketObj->IsPending())
		return TRUE;

	CReentrantCriSecLock locallock(pSocketObj->csSend);

	if(!pSocketObj->IsPending())
		return TRUE;

	BOOL isOK = TRUE;

	int writes = flag ? -1 : MAX_CONTINUE_WRITES;
	TBufferObjList& sndBuff = pSocketObj->sndBuff;

	for(int i = 0; i < writes || writes < 0; i++)
	{
		TItemPtr itPtr(sndBuff, sndBuff.PopFront());

		if(!itPtr.IsValid())
			break;

		ASSERT(!itPtr->IsEmpty());

		isOK = SendItem(pSocketObj, itPtr);

		if(!isOK)
			break;

		if(!itPtr->IsEmpty())
		{
			sndBuff.PushFront(itPtr.Detach());
			break;
		}
	}

	return isOK;
}

BOOL CTcpServer::SendItem(TSocketObj* pSocketObj, TItem* pItem)
{
	while(!pItem->IsEmpty())
	{
		int rc = (int)write(pSocketObj->socket, pItem->Ptr(), pItem->Size());

		if(rc > 0)
		{
			if(TRIGGER(FireSend(pSocketObj, pItem->Ptr(), rc)) == HR_ERROR)
			{
				TRACE("<C-CNNID: %zu> OnSend() event should not return 'HR_ERROR' !!", pSocketObj->connID);
				ASSERT(FALSE);
			}

			pItem->Reduce(rc);
		}
		else if(rc == SOCKET_ERROR)
		{
			int code = ::WSAGetLastError();

			if(code == ERROR_WOULDBLOCK)
				break;

			AddFreeSocketObj(pSocketObj, SCF_ERROR, SO_SEND, code);
			return FALSE;
		}
		else
			ASSERT(FALSE);
	}

	return TRUE;
}

BOOL CTcpServer::Send(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset)
{
	ASSERT(pBuffer && iLength > 0);

	if(iOffset != 0) pBuffer += iOffset;

	WSABUF buffer;
	buffer.len = iLength;
	buffer.buf = (BYTE*)pBuffer;

	return SendPackets(dwConnID, &buffer, 1);
}

BOOL CTcpServer::DoSendPackets(CONNID dwConnID, const WSABUF pBuffers[], int iCount)
{
	ASSERT(pBuffers && iCount > 0);

	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(!TSocketObj::IsValid(pSocketObj))
	{
		::SetLastError(ERROR_OBJECT_NOT_FOUND);
		return FALSE;
	}

	return DoSendPackets(pSocketObj, pBuffers, iCount);
}

BOOL CTcpServer::DoSendPackets(TSocketObj* pSocketObj, const WSABUF pBuffers[], int iCount)
{
	ASSERT(pSocketObj && pBuffers && iCount > 0);

	int result = NO_ERROR;

	if(pBuffers && iCount > 0)
	{
		CReentrantCriSecLock locallock(pSocketObj->csSend);

		if(TSocketObj::IsValid(pSocketObj))
			result = SendInternal(pSocketObj, pBuffers, iCount);
		else
			result = ERROR_OBJECT_NOT_FOUND;
	}
	else
		result = ERROR_INVALID_PARAMETER;

	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);
}

int CTcpServer::SendInternal(TSocketObj* pSocketObj, const WSABUF pBuffers[], int iCount)
{
	int iPending = pSocketObj->Pending();

	for(int i = 0; i < iCount; i++)
	{
		int iBufLen = pBuffers[i].len;

		if(iBufLen > 0)
		{
			BYTE* pBuffer = (BYTE*)pBuffers[i].buf;
			ASSERT(pBuffer);

			pSocketObj->sndBuff.Cat(pBuffer, iBufLen);
		}
	}

	if(iPending == 0 && pSocketObj->IsPending())
	{
		if(!m_ioDispatcher.SendCommand(DISP_CMD_SEND, pSocketObj->connID))
			return ::GetLastError();
	}

	return NO_ERROR;
}

BOOL CTcpServer::SendSmallFile(CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail)
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

	return SendPackets(dwConnID, szBuf, 3);
}
