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
 
#include "TcpAgent.h"

#include "./common/FileHelper.h"

BOOL CTcpAgent::Start(LPCTSTR lpszBindAddress, BOOL bAsyncConnect)
{
	if(!CheckParams() || !CheckStarting())
		return FALSE;

	PrepareStart();

	if(ParseBindAddress(lpszBindAddress))
		if(CreateWorkerThreads())
		{
			m_bAsyncConnect	= bAsyncConnect;
			m_enState		= SS_STARTED;

			return TRUE;
		}

	EXECUTE_RESTORE_ERROR(Stop());

	return FALSE;
}

void CTcpAgent::SetLastError(EnSocketError code, LPCSTR func, int ec)
{
	m_enLastError = code;
	::SetLastError(ec);
}

BOOL CTcpAgent::CheckParams()
{
	if	((m_enSendPolicy >= SP_PACK && m_enSendPolicy <= SP_DIRECT)								&&
		((int)m_dwMaxConnectionCount > 0)														&&
		((int)m_dwWorkerThreadCount > 0 && m_dwWorkerThreadCount <= MAX_WORKER_THREAD_COUNT)	&&
		((int)m_dwSocketBufferSize >= MIN_SOCKET_BUFFER_SIZE)									&&
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

void CTcpAgent::PrepareStart()
{
	m_bfActiveSockets.Reset(m_dwMaxConnectionCount);
	m_lsFreeSocket.Reset(m_dwFreeSocketObjPool);

	m_bfObjPool.SetItemCapacity(m_dwSocketBufferSize);
	m_bfObjPool.SetPoolSize(m_dwFreeBufferObjPool);
	m_bfObjPool.SetPoolHold(m_dwFreeBufferObjHold);

	m_bfObjPool.Prepare();
}

BOOL CTcpAgent::CheckStarting()
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

BOOL CTcpAgent::CheckStoping()
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

BOOL CTcpAgent::ParseBindAddress(LPCTSTR lpszBindAddress)
{
	if(::IsStrEmpty(lpszBindAddress))
		return TRUE;

	HP_SOCKADDR addr;

	if(::sockaddr_A_2_IN(lpszBindAddress, 0, addr))
	{
		SOCKET sock	= socket(addr.family, SOCK_STREAM, IPPROTO_TCP);

		if(sock != INVALID_SOCKET)
		{
			if(::bind(sock, addr.Addr(), addr.AddrSize()) != SOCKET_ERROR)
			{
				addr.Copy(m_soAddr);
				return TRUE;
			}
			else
				SetLastError(SE_SOCKET_BIND, __FUNCTION__, ::WSAGetLastError());

			::ManualCloseSocket(sock);
		}
		else
			SetLastError(SE_SOCKET_CREATE, __FUNCTION__, ::WSAGetLastError());
	}
	else
		SetLastError(SE_SOCKET_CREATE, __FUNCTION__, ::WSAGetLastError());

	return FALSE;
}

BOOL CTcpAgent::CreateWorkerThreads()
{
	if(!m_ioDispatcher.Start(this, DEFAULT_WORKER_MAX_EVENT_COUNT, m_dwWorkerThreadCount))
		return FALSE;

	const CIODispatcher::CWorkerThread* pWorkerThread = m_ioDispatcher.GetWorkerThreads();

	for(DWORD i = 0; i < m_dwWorkerThreadCount; i++)
		m_rcBufferMap[pWorkerThread[i].GetThreadID()] = new CBufferPtr(m_dwSocketBufferSize);

	return TRUE;
}

BOOL CTcpAgent::Stop()
{
	if(!CheckStoping())
		return FALSE;
	
	DisconnectClientSocket();
	WaitForClientSocketClose();
	WaitForWorkerThreadEnd();
	
	ReleaseClientSocket();

	FireShutdown();

	ReleaseFreeSocket();

	Reset();

	return TRUE;
}

void CTcpAgent::DisconnectClientSocket()
{
	::WaitFor(100);

	DWORD size					= 0;
	unique_ptr<CONNID[]> ids	= m_bfActiveSockets.GetAllElementIndexes(size);

	for(DWORD i = 0; i < size; i++)
		Disconnect(ids[i]);
}

void CTcpAgent::WaitForClientSocketClose()
{
	while(m_bfActiveSockets.Elements() > 0)
		::WaitFor(50);
}

void CTcpAgent::WaitForWorkerThreadEnd()
{
	m_ioDispatcher.Stop();
}

void CTcpAgent::ReleaseClientSocket()
{
	VERIFY(m_bfActiveSockets.IsEmpty());
	m_bfActiveSockets.Reset();
}

void CTcpAgent::ReleaseFreeSocket()
{
	TAgentSocketObj* pSocketObj = nullptr;

	while(m_lsFreeSocket.TryGet(&pSocketObj))
		DeleteSocketObj(pSocketObj);

	VERIFY(m_lsFreeSocket.IsEmpty());
	m_lsFreeSocket.Reset();

	ReleaseGCSocketObj(TRUE);
	VERIFY(m_lsGCSocket.IsEmpty());
}

void CTcpAgent::Reset()
{
	m_bfObjPool.Clear();
	m_phSocket.Reset();
	m_soAddr.Reset();

	::ClearPtrMap(m_rcBufferMap);

	m_enState = SS_STOPPED;
}

BOOL CTcpAgent::Connect(LPCTSTR lpszRemoteAddress, USHORT usPort, CONNID* pdwConnID, PVOID pExtra, USHORT usLocalPort)
{
	ASSERT(lpszRemoteAddress && usPort != 0);

	DWORD result	= NO_ERROR;
	SOCKET soClient	= INVALID_SOCKET;

	if(!pdwConnID)
		pdwConnID	= CreateLocalObject(CONNID);

	*pdwConnID = 0;

	HP_SOCKADDR addr;

	if(!HasStarted())
		result = ERROR_INVALID_STATE;
	else
	{
		result = CreateClientSocket(lpszRemoteAddress, usPort, usLocalPort, soClient, addr);

		if(result == NO_ERROR)
		{
			result = PrepareConnect(*pdwConnID, soClient);

			if(result == NO_ERROR)
			{
				result	 = ConnectToServer(*pdwConnID, lpszRemoteAddress, soClient, addr, pExtra);
				soClient = INVALID_SOCKET;
			}
		}
	}

	if(result != NO_ERROR)
	{
		if(soClient != INVALID_SOCKET)
			::ManualCloseSocket(soClient);

		::SetLastError(result);
	}

	return (result == NO_ERROR);
}

int CTcpAgent::CreateClientSocket(LPCTSTR lpszRemoteAddress, USHORT usPort, USHORT usLocalPort, SOCKET& soClient, HP_SOCKADDR& addr)
{
	if(!::GetSockAddrByHostName(lpszRemoteAddress, usPort, addr))
		return ERROR_ADDRNOTAVAIL;

	BOOL bBind = m_soAddr.IsSpecified();

	if(bBind && m_soAddr.family != addr.family)
		return ERROR_AFNOSUPPORT;

	int result	= NO_ERROR;
	soClient	= socket(addr.family, SOCK_STREAM, IPPROTO_TCP);

	if(soClient == INVALID_SOCKET)
		result = ::WSAGetLastError();
	else
	{
		BOOL bOnOff	= (m_dwKeepAliveTime > 0 && m_dwKeepAliveInterval > 0);
		VERIFY(IS_NO_ERROR(::SSO_KeepAliveVals(soClient, bOnOff, m_dwKeepAliveTime, m_dwKeepAliveInterval)));
		VERIFY(IS_NO_ERROR(::SSO_ReuseAddress(soClient, m_bReuseAddress)));

		if(bBind && usLocalPort == 0)
		{
			if(::bind(soClient, m_soAddr.Addr(), m_soAddr.AddrSize()) == SOCKET_ERROR)
				result = ::WSAGetLastError();
		}
		else if(usLocalPort != 0)
		{
			HP_SOCKADDR bindAddr = bBind ? m_soAddr : HP_SOCKADDR::AnyAddr(addr.family);

			bindAddr.SetPort(usLocalPort);

			if(::bind(soClient, bindAddr.Addr(), bindAddr.AddrSize()) == SOCKET_ERROR)
				result = ::WSAGetLastError();
		}
	}

	return result;
}

int CTcpAgent::PrepareConnect(CONNID& dwConnID, SOCKET soClient)
{
	if(!m_bfActiveSockets.AcquireLock(dwConnID))
		return ERROR_CONNECTION_COUNT_LIMIT;

	if(TRIGGER(FirePrepareConnect(dwConnID, soClient)) == HR_ERROR)
	{
		VERIFY(m_bfActiveSockets.ReleaseLock(dwConnID, nullptr));
		return ENSURE_ERROR_CANCELLED;
	}

	return NO_ERROR;
}

int CTcpAgent::ConnectToServer(CONNID dwConnID, LPCTSTR lpszRemoteAddress, SOCKET soClient, const HP_SOCKADDR& addr, PVOID pExtra)
{
	TAgentSocketObj* pSocketObj = GetFreeSocketObj(dwConnID, soClient);

	AddClientSocketObj(dwConnID, pSocketObj, addr, lpszRemoteAddress, pExtra);

	int result = HAS_ERROR;

	if(m_bAsyncConnect)
	{
		VERIFY(::fcntl_SETFL(pSocketObj->socket, O_NOATIME | O_NONBLOCK | O_CLOEXEC));

		int rc = ::connect(pSocketObj->socket, addr.Addr(), addr.AddrSize());

		if(IS_NO_ERROR(rc) || IS_IO_PENDING_ERROR())
		{
			if(m_ioDispatcher.AddFD(pSocketObj->socket, EPOLLOUT | EPOLLONESHOT, pSocketObj))
				result = NO_ERROR;
		}
	}
	else
	{
		if(::connect(pSocketObj->socket, addr.Addr(), addr.AddrSize()) != SOCKET_ERROR)
		{
			VERIFY(::fcntl_SETFL(pSocketObj->socket, O_NOATIME | O_NONBLOCK | O_CLOEXEC));

			pSocketObj->SetConnected();

			if(TRIGGER(FireConnect(pSocketObj)) == HR_ERROR)
				result = ENSURE_ERROR_CANCELLED;
			else
			{
				UINT evts = (pSocketObj->IsPending() ? EPOLLOUT : 0) | (pSocketObj->IsPaused() ? 0 : EPOLLIN);

				if(m_ioDispatcher.AddFD(pSocketObj->socket, evts | EPOLLRDHUP | EPOLLONESHOT, pSocketObj))
					result = NO_ERROR;
			}
		}
	}

	if(result == HAS_ERROR)
		result = ::WSAGetLastError();
	if(result != NO_ERROR)
		AddFreeSocketObj(pSocketObj, SCF_NONE);

	return result;
}

TAgentSocketObj* CTcpAgent::GetFreeSocketObj(CONNID dwConnID, SOCKET soClient)
{
	DWORD dwIndex;
	TAgentSocketObj* pSocketObj = nullptr;

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

TAgentSocketObj* CTcpAgent::CreateSocketObj()
{
	return TAgentSocketObj::Construct(m_phSocket, m_bfObjPool);
}

void CTcpAgent::DeleteSocketObj(TAgentSocketObj* pSocketObj)
{
	TAgentSocketObj::Destruct(pSocketObj);
}

void CTcpAgent::AddFreeSocketObj(TAgentSocketObj* pSocketObj, EnSocketCloseFlag enFlag, EnSocketOperation enOperation, int iErrorCode)
{
	if(!InvalidSocketObj(pSocketObj))
		return;

	CloseClientSocketObj(pSocketObj, enFlag, enOperation, iErrorCode);

	m_bfActiveSockets.Remove(pSocketObj->connID);
	TAgentSocketObj::Release(pSocketObj);

	ReleaseGCSocketObj();

	if(!m_lsFreeSocket.TryPut(pSocketObj))
		m_lsGCSocket.PushBack(pSocketObj);
}

void CTcpAgent::ReleaseGCSocketObj(BOOL bForce)
{
	::ReleaseGCObj(m_lsGCSocket, m_dwFreeSocketObjLockTime, bForce);
}

BOOL CTcpAgent::InvalidSocketObj(TAgentSocketObj* pSocketObj)
{
	return TAgentSocketObj::InvalidSocketObj(pSocketObj);
}

void CTcpAgent::AddClientSocketObj(CONNID dwConnID, TAgentSocketObj* pSocketObj, const HP_SOCKADDR& remoteAddr, LPCTSTR lpszRemoteAddress, PVOID pExtra)
{
	ASSERT(FindSocketObj(dwConnID) == nullptr);

	pSocketObj->connTime	= ::TimeGetTime();
	pSocketObj->activeTime	= pSocketObj->connTime;
	pSocketObj->host		= lpszRemoteAddress;
	pSocketObj->extra		= pExtra;

	remoteAddr.Copy(pSocketObj->remoteAddr);

	VERIFY(m_bfActiveSockets.ReleaseLock(dwConnID, pSocketObj));
}

TAgentSocketObj* CTcpAgent::FindSocketObj(CONNID dwConnID)
{
	TAgentSocketObj* pSocketObj = nullptr;

	if(m_bfActiveSockets.Get(dwConnID, &pSocketObj) != TAgentSocketObjPtrPool::GR_VALID)
		pSocketObj = nullptr;

	return pSocketObj;
}

void CTcpAgent::CloseClientSocketObj(TAgentSocketObj* pSocketObj, EnSocketCloseFlag enFlag, EnSocketOperation enOperation, int iErrorCode, int iShutdownFlag)
{
	ASSERT(TAgentSocketObj::IsExist(pSocketObj));

	if(enFlag == SCF_CLOSE)
		FireClose(pSocketObj, SO_CLOSE, SE_OK);
	else if(enFlag == SCF_ERROR)
		FireClose(pSocketObj, enOperation, iErrorCode);

	SOCKET socket = pSocketObj->socket;
	pSocketObj->socket = INVALID_SOCKET;

	::ManualCloseSocket(socket, iShutdownFlag);
}

BOOL CTcpAgent::GetLocalAddress(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	TAgentSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TAgentSocketObj::IsValid(pSocketObj))
		return ::GetSocketLocalAddress(pSocketObj->socket, lpszAddress, iAddressLen, usPort);

	return FALSE;
}

BOOL CTcpAgent::GetRemoteAddress(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	TAgentSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TAgentSocketObj::IsExist(pSocketObj))
	{
		ADDRESS_FAMILY usFamily;
		return ::sockaddr_IN_2_A(pSocketObj->remoteAddr, usFamily, lpszAddress, iAddressLen, usPort);
	}

	return FALSE;
}

BOOL CTcpAgent::GetRemoteHost(CONNID dwConnID, TCHAR lpszHost[], int& iHostLen, USHORT& usPort)
{
	ASSERT(lpszHost != nullptr && iHostLen > 0);

	BOOL isOK				= FALSE;
	TAgentSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TAgentSocketObj::IsExist(pSocketObj))
	{
		int iLen = pSocketObj->host.GetLength() + 1;

		if(iHostLen >= iLen)
		{
			memcpy(lpszHost, CA2CT((LPCSTR)pSocketObj->host), iLen * sizeof(TCHAR));
			usPort = pSocketObj->remoteAddr.Port();

			isOK = TRUE;
		}

		iHostLen = iLen;
	}

	return isOK;
}

BOOL CTcpAgent::GetRemoteHost(CONNID dwConnID, LPCSTR* lpszHost, USHORT* pusPort)
{
	*lpszHost				= nullptr;
	TAgentSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TAgentSocketObj::IsExist(pSocketObj))
	{
		*lpszHost = pSocketObj->host;

		if(pusPort)
			*pusPort = pSocketObj->remoteAddr.Port();
	}

	return (*lpszHost != nullptr && (*lpszHost)[0] != 0);
}

BOOL CTcpAgent::SetConnectionExtra(CONNID dwConnID, PVOID pExtra)
{
	TAgentSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return SetConnectionExtra(pSocketObj, pExtra);
}

BOOL CTcpAgent::SetConnectionExtra(TAgentSocketObj* pSocketObj, PVOID pExtra)
{
	if(TAgentSocketObj::IsExist(pSocketObj))
	{
		pSocketObj->extra = pExtra;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpAgent::GetConnectionExtra(CONNID dwConnID, PVOID* ppExtra)
{
	TAgentSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return GetConnectionExtra(pSocketObj, ppExtra);
}

BOOL CTcpAgent::GetConnectionExtra(TAgentSocketObj* pSocketObj, PVOID* ppExtra)
{
	ASSERT(ppExtra != nullptr);

	if(TAgentSocketObj::IsExist(pSocketObj))
	{
		*ppExtra = pSocketObj->extra;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpAgent::SetConnectionReserved(CONNID dwConnID, PVOID pReserved)
{
	TAgentSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return SetConnectionReserved(pSocketObj, pReserved);
}

BOOL CTcpAgent::SetConnectionReserved(TAgentSocketObj* pSocketObj, PVOID pReserved)
{
	if(TAgentSocketObj::IsExist(pSocketObj))
	{
		pSocketObj->reserved = pReserved;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpAgent::GetConnectionReserved(CONNID dwConnID, PVOID* ppReserved)
{
	TAgentSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return GetConnectionReserved(pSocketObj, ppReserved);
}

BOOL CTcpAgent::GetConnectionReserved(TAgentSocketObj* pSocketObj, PVOID* ppReserved)
{
	ASSERT(ppReserved != nullptr);

	if(TAgentSocketObj::IsExist(pSocketObj))
	{
		*ppReserved = pSocketObj->reserved;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpAgent::SetConnectionReserved2(CONNID dwConnID, PVOID pReserved2)
{
	TAgentSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return SetConnectionReserved2(pSocketObj, pReserved2);
}

BOOL CTcpAgent::SetConnectionReserved2(TAgentSocketObj* pSocketObj, PVOID pReserved2)
{
	if(TAgentSocketObj::IsExist(pSocketObj))
	{
		pSocketObj->reserved2 = pReserved2;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpAgent::GetConnectionReserved2(CONNID dwConnID, PVOID* ppReserved2)
{
	TAgentSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return GetConnectionReserved2(pSocketObj, ppReserved2);
}

BOOL CTcpAgent::GetConnectionReserved2(TAgentSocketObj* pSocketObj, PVOID* ppReserved2)
{
	ASSERT(ppReserved2 != nullptr);

	if(TAgentSocketObj::IsExist(pSocketObj))
	{
		*ppReserved2 = pSocketObj->reserved2;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpAgent::IsPauseReceive(CONNID dwConnID, BOOL& bPaused)
{
	TAgentSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TAgentSocketObj::IsValid(pSocketObj))
	{
		bPaused = pSocketObj->paused;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpAgent::IsConnected(CONNID dwConnID)
{
	TAgentSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TAgentSocketObj::IsValid(pSocketObj))
		return pSocketObj->HasConnected();

	return FALSE;
}

BOOL CTcpAgent::GetPendingDataLength(CONNID dwConnID, int& iPending)
{
	TAgentSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TAgentSocketObj::IsValid(pSocketObj))
	{
		iPending = pSocketObj->Pending();
		return TRUE;
	}

	return FALSE;
}

DWORD CTcpAgent::GetConnectionCount()
{
	return m_bfActiveSockets.Elements();
}

BOOL CTcpAgent::GetAllConnectionIDs(CONNID pIDs[], DWORD& dwCount)
{
	return m_bfActiveSockets.GetAllElementIndexes(pIDs, dwCount);
}

BOOL CTcpAgent::GetConnectPeriod(CONNID dwConnID, DWORD& dwPeriod)
{
	BOOL isOK					= TRUE;
	TAgentSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TAgentSocketObj::IsValid(pSocketObj))
		dwPeriod = ::GetTimeGap32(pSocketObj->connTime);
	else
		isOK = FALSE;

	return isOK;
}

BOOL CTcpAgent::GetSilencePeriod(CONNID dwConnID, DWORD& dwPeriod)
{
	if(!m_bMarkSilence)
		return FALSE;

	BOOL isOK					= TRUE;
	TAgentSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TAgentSocketObj::IsValid(pSocketObj))
		dwPeriod = ::GetTimeGap32(pSocketObj->activeTime);
	else
		isOK = FALSE;

	return isOK;
}

BOOL CTcpAgent::Disconnect(CONNID dwConnID, BOOL bForce)
{
	BOOL isOK					= FALSE;
	TAgentSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TAgentSocketObj::IsValid(pSocketObj))
		isOK = m_ioDispatcher.SendCommand(DISP_CMD_DISCONNECT, dwConnID, bForce);

	return isOK;
}

BOOL CTcpAgent::DisconnectLongConnections(DWORD dwPeriod, BOOL bForce)
{
	if(dwPeriod > MAX_CONNECTION_PERIOD)
		return FALSE;

	DWORD size					= 0;
	unique_ptr<CONNID[]> ids	= m_bfActiveSockets.GetAllElementIndexes(size);
	DWORD now					= ::TimeGetTime();

	for(DWORD i = 0; i < size; i++)
	{
		CONNID connID			= ids[i];
		TAgentSocketObj* pSocketObj	= FindSocketObj(connID);

		if(TAgentSocketObj::IsValid(pSocketObj) && (int)(now - pSocketObj->connTime) >= (int)dwPeriod)
			Disconnect(connID, bForce);
	}

	return TRUE;
}

BOOL CTcpAgent::DisconnectSilenceConnections(DWORD dwPeriod, BOOL bForce)
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
		TAgentSocketObj* pSocketObj	= FindSocketObj(connID);

		if(TAgentSocketObj::IsValid(pSocketObj) && (int)(now - pSocketObj->activeTime) >= (int)dwPeriod)
			Disconnect(connID, bForce);
	}

	return TRUE;
}

BOOL CTcpAgent::PauseReceive(CONNID dwConnID, BOOL bPause)
{
	TAgentSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(!TAgentSocketObj::IsValid(pSocketObj))
	{
		::SetLastError(ERROR_OBJECT_NOT_FOUND);
		return FALSE;
	}

	if(!pSocketObj->HasConnected())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	if(pSocketObj->paused == bPause)
		return TRUE;

	pSocketObj->paused = bPause;

	if(!bPause)
		return m_ioDispatcher.SendCommand(DISP_CMD_UNPAUSE, pSocketObj->connID);

	return TRUE;
}

BOOL CTcpAgent::OnBeforeProcessIo(PVOID pv, UINT events)
{
	TAgentSocketObj* pSocketObj = (TAgentSocketObj*)(pv);

	if(!TAgentSocketObj::IsValid(pSocketObj))
		return FALSE;

	if(events & _EPOLL_ALL_ERROR_EVENTS)
		pSocketObj->SetConnected(FALSE);

	pSocketObj->csIo.lock();

	if(!TAgentSocketObj::IsValid(pSocketObj))
	{
		pSocketObj->csIo.unlock();
		return FALSE;
	}

	if(!pSocketObj->HasConnected())
	{
		HandleConnect(pSocketObj, events);

		pSocketObj->csIo.unlock();
		return FALSE;
	}

	return TRUE;
}

VOID CTcpAgent::OnAfterProcessIo(PVOID pv, UINT events, BOOL rs)
{
	TAgentSocketObj* pSocketObj = (TAgentSocketObj*)(pv);

	if(TAgentSocketObj::IsValid(pSocketObj))
	{
		ASSERT(rs && !(events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)));

		UINT evts = (pSocketObj->IsPending() ? EPOLLOUT : 0) | (pSocketObj->IsPaused() ? 0 : EPOLLIN);
		m_ioDispatcher.ModFD(pSocketObj->socket, evts | EPOLLRDHUP | EPOLLONESHOT, pSocketObj);
	}

	pSocketObj->csIo.unlock();
}

VOID CTcpAgent::OnCommand(TDispCommand* pCmd)
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

VOID CTcpAgent::HandleCmdSend(CONNID dwConnID)
{
	TAgentSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TAgentSocketObj::IsValid(pSocketObj) && pSocketObj->IsPending())
		m_ioDispatcher.ProcessIo(pSocketObj, EPOLLOUT);
}

VOID CTcpAgent::HandleCmdUnpause(CONNID dwConnID)
{
	TAgentSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TAgentSocketObj::IsValid(pSocketObj) && !pSocketObj->IsPaused())
		m_ioDispatcher.ProcessIo(pSocketObj, EPOLLIN);
}

VOID CTcpAgent::HandleCmdDisconnect(CONNID dwConnID, BOOL bForce)
{
	TAgentSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TAgentSocketObj::IsValid(pSocketObj))
	{
		if(bForce)
			m_ioDispatcher.ProcessIo(pSocketObj, EPOLLHUP);
		else
			::shutdown(pSocketObj->socket, SHUT_WR);
	}
}

BOOL CTcpAgent::OnReadyRead(PVOID pv, UINT events)
{
	return HandleReceive((TAgentSocketObj*)pv, RETRIVE_EVENT_FLAG_H(events));
}

BOOL CTcpAgent::OnReadyWrite(PVOID pv, UINT events)
{
	return HandleSend((TAgentSocketObj*)pv, RETRIVE_EVENT_FLAG_H(events));
}

BOOL CTcpAgent::OnHungUp(PVOID pv, UINT events)
{
	return HandleClose((TAgentSocketObj*)pv, SCF_CLOSE, events);
}

BOOL CTcpAgent::OnError(PVOID pv, UINT events)
{
	return HandleClose((TAgentSocketObj*)pv, SCF_ERROR, events);
}

VOID CTcpAgent::OnDispatchThreadEnd(THR_ID tid)
{
	OnWorkerThreadEnd(tid);
}

BOOL CTcpAgent::HandleClose(TAgentSocketObj* pSocketObj, EnSocketCloseFlag enFlag, UINT events)
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

BOOL CTcpAgent::HandleConnect(TAgentSocketObj* pSocketObj, UINT events)
{
	int code = ::SSO_GetError(pSocketObj->socket);

	if(!IS_NO_ERROR(code) || (events & _EPOLL_ERROR_EVENTS))
	{
		AddFreeSocketObj(pSocketObj, SCF_ERROR, SO_CONNECT, code);
		return FALSE;
	}

	if(events & _EPOLL_HUNGUP_EVENTS)
	{
		AddFreeSocketObj(pSocketObj, SCF_CLOSE, SO_CONNECT, SE_OK);
		return FALSE;
	}

	ASSERT(events & EPOLLOUT);

	pSocketObj->SetConnected();

	if(TRIGGER(FireConnect(pSocketObj)) == HR_ERROR)
	{
		AddFreeSocketObj(pSocketObj, SCF_NONE);
		return FALSE;
	}

	UINT evts = (pSocketObj->IsPending() ? EPOLLOUT : 0) | (pSocketObj->IsPaused() ? 0 : EPOLLIN);
	m_ioDispatcher.ModFD(pSocketObj->socket, evts | EPOLLRDHUP | EPOLLONESHOT, pSocketObj);

	return TRUE;
}

BOOL CTcpAgent::HandleReceive(TAgentSocketObj* pSocketObj, int flag)
{
	ASSERT(TAgentSocketObj::IsValid(pSocketObj));

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

BOOL CTcpAgent::HandleSend(TAgentSocketObj* pSocketObj, int flag)
{
	ASSERT(TAgentSocketObj::IsValid(pSocketObj));

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

BOOL CTcpAgent::SendItem(TAgentSocketObj* pSocketObj, TItem* pItem)
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

BOOL CTcpAgent::Send(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset)
{
	ASSERT(pBuffer && iLength > 0);

	if(iOffset != 0) pBuffer += iOffset;

	WSABUF buffer;
	buffer.len = iLength;
	buffer.buf = (BYTE*)pBuffer;

	return SendPackets(dwConnID, &buffer, 1);
}

BOOL CTcpAgent::DoSendPackets(CONNID dwConnID, const WSABUF pBuffers[], int iCount)
{
	ASSERT(pBuffers && iCount > 0);

	TAgentSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(!TAgentSocketObj::IsValid(pSocketObj))
	{
		::SetLastError(ERROR_OBJECT_NOT_FOUND);
		return FALSE;
	}

	return DoSendPackets(pSocketObj, pBuffers, iCount);
}

BOOL CTcpAgent::DoSendPackets(TAgentSocketObj* pSocketObj, const WSABUF pBuffers[], int iCount)
{
	ASSERT(pSocketObj && pBuffers && iCount > 0);

	int result = NO_ERROR;

	if(!pSocketObj->HasConnected())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	if(pBuffers && iCount > 0)
	{
		CReentrantCriSecLock locallock(pSocketObj->csSend);

		if(TAgentSocketObj::IsValid(pSocketObj))
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

int CTcpAgent::SendInternal(TAgentSocketObj* pSocketObj, const WSABUF pBuffers[], int iCount)
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

BOOL CTcpAgent::SendSmallFile(CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail)
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
