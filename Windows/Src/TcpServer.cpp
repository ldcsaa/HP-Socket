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
#include "TcpServer.h"
#include "../Common/Src/WaitFor.h"

#include <malloc.h>
#include <process.h>

const CInitSocket CTcpServer::sm_wsSocket;

EnHandleResult CTcpServer::TriggerFireAccept(TSocketObj* pSocketObj)
{
	CCriSecLock locallock(pSocketObj->csRecv);
	return TRIGGER(FireAccept(pSocketObj));
}

EnHandleResult CTcpServer::TriggerFireReceive(TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	EnHandleResult rs = (EnHandleResult)HR_CLOSED;

	if(TSocketObj::IsValid(pSocketObj))
	{
		CCriSecLock locallock(pSocketObj->csRecv);

		if(TSocketObj::IsValid(pSocketObj))
		{
			rs = TRIGGER(FireReceive(pSocketObj, (BYTE*)pBufferObj->buff.buf, pBufferObj->buff.len));
		}
	}

	return rs;
}

EnHandleResult CTcpServer::TriggerFireSend(TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	EnHandleResult rs = TRIGGER(FireSend(pSocketObj, (BYTE*)pBufferObj->buff.buf, pBufferObj->buff.len));

	if(rs == HR_ERROR)
	{
		TRACE("<S-CNNID: %Iu> OnSend() event should not return 'HR_ERROR' !!\n", pSocketObj->connID);
		ASSERT(FALSE);
	}

	if(pBufferObj->ReleaseSendCounter() == 0)
		AddFreeBufferObj(pBufferObj);

	return rs;
}

EnHandleResult CTcpServer::TriggerFireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
{
	CCriSecLock locallock(pSocketObj->csRecv);
	return FireClose(pSocketObj, enOperation, iErrorCode);
}

void CTcpServer::SetLastError(EnSocketError code, LPCSTR func, int ec)
{
	m_enLastError = code;
	::SetLastError(ec);

	TRACE("%s --> Error: %d, EC: %d\n", func, code, ec);
}

BOOL CTcpServer::Start(LPCTSTR lpszBindAddress, USHORT usPort)
{
	if(!CheckParams() || !CheckStarting())
		return FALSE;

	PrepareStart();

	if(CreateListenSocket(lpszBindAddress, usPort))
		if(CreateCompletePort())
			if(CreateWorkerThreads())
				if(StartAccept())
				{
					m_enState = SS_STARTED;
					return TRUE;
				}

	EXECUTE_RESTORE_ERROR(Stop());

	return FALSE;
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

	if(!lpszBindAddress || lpszBindAddress[0] == 0)
		lpszBindAddress = DEFAULT_IPV4_BIND_ADDRESS;

	HP_SOCKADDR addr;

	if(::sockaddr_A_2_IN(lpszBindAddress, usPort, addr))
	{
		m_usFamily = addr.family;
		m_soListen = socket(m_usFamily, SOCK_STREAM, IPPROTO_TCP);

		if(m_soListen != INVALID_SOCKET)
		{
			BOOL bOnOff	= (m_dwKeepAliveTime > 0 && m_dwKeepAliveInterval > 0);
			ENSURE(::SSO_KeepAliveVals(m_soListen, bOnOff, m_dwKeepAliveTime, m_dwKeepAliveInterval) == NO_ERROR);
			ENSURE(::SSO_ExclusiveAddressUse(m_soListen, TRUE) == NO_ERROR);
			ENSURE(::SSO_NoBlock(m_soListen) == NO_ERROR);
			
			if(::bind(m_soListen, addr.Addr(), addr.AddrSize()) != SOCKET_ERROR)
			{
				if(TRIGGER(FirePrepareListen(m_soListen)) != HR_ERROR)
				{
					if(::listen(m_soListen, m_dwSocketListenQueue) != SOCKET_ERROR)
					{
						m_pfnAcceptEx				= ::Get_AcceptEx_FuncPtr(m_soListen);
						m_pfnGetAcceptExSockaddrs	= ::Get_GetAcceptExSockaddrs_FuncPtr(m_soListen);
						m_pfnDisconnectEx			= ::Get_DisconnectEx_FuncPtr(m_soListen);

						ASSERT(m_pfnAcceptEx);
						ASSERT(m_pfnGetAcceptExSockaddrs);
						ASSERT(m_pfnDisconnectEx);

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

BOOL CTcpServer::CreateCompletePort()
{
	m_hCompletePort	= ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
	if(m_hCompletePort == nullptr)
		SetLastError(SE_CP_CREATE, __FUNCTION__, ::GetLastError());

	return (m_hCompletePort != nullptr);
}

BOOL CTcpServer::CreateWorkerThreads()
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

BOOL CTcpServer::StartAccept()
{
	BOOL isOK = TRUE;

	if(::CreateIoCompletionPort((HANDLE)m_soListen, m_hCompletePort, m_soListen, 0))
	{
		m_iRemainAcceptSockets = m_dwAcceptSocketCount;

		for(DWORD i = 0; i < m_dwAcceptSocketCount; i++)
			ENSURE(::PostIocpAccept(m_hCompletePort));
	}
	else
	{
		SetLastError(SE_SOCKE_ATTACH_TO_CP, __FUNCTION__, ::GetLastError());
		isOK = FALSE;
	}

	return isOK;
}

BOOL CTcpServer::Stop()
{
	if(!CheckStoping())
		return FALSE;

	CloseListenSocket();

	WaitForAcceptSocketClose();

	DisconnectClientSocket();
	WaitForClientSocketClose();
	WaitForWorkerThreadEnd();
	
	ReleaseClientSocket();

	FireShutdown();

	ReleaseFreeSocket();
	ReleaseFreeBuffer();

	CloseCompletePort();

	Reset();

	return TRUE;
}

void CTcpServer::Reset()
{
	m_phSocket.Reset();

	m_iRemainAcceptSockets		= 0;
	m_pfnAcceptEx				= nullptr;
	m_pfnGetAcceptExSockaddrs	= nullptr;
	m_pfnDisconnectEx			= nullptr;
	m_enState					= SS_STOPPED;
	m_usFamily					= AF_UNSPEC;
}

void CTcpServer::CloseListenSocket()
{
	if(m_soListen != INVALID_SOCKET)
	{
		::ManualCloseSocket(m_soListen);
		m_soListen = INVALID_SOCKET;
		
		::WaitWithMessageLoop(100);
	}
}

void CTcpServer::DisconnectClientSocket()
{
	DWORD size					= 0;
	unique_ptr<CONNID[]> ids	= m_bfActiveSockets.GetAllElementIndexes(size);

	for(DWORD i = 0; i < size; i++)
		Disconnect(ids[i]);
}

void CTcpServer::ReleaseClientSocket()
{
	ENSURE(m_bfActiveSockets.IsEmpty());
	m_bfActiveSockets.Reset();
}

TSocketObj*	CTcpServer::GetFreeSocketObj(CONNID dwConnID, SOCKET soClient)
{
	DWORD dwIndex;
	TSocketObj* pSocketObj = nullptr;

	if(m_lsFreeSocket.TryLock(&pSocketObj, dwIndex))
	{
		if(::GetTimeGap32(pSocketObj->freeTime) >= m_dwFreeSocketObjLockTime)
			ENSURE(m_lsFreeSocket.ReleaseLock(nullptr, dwIndex));
		else
		{
			ENSURE(m_lsFreeSocket.ReleaseLock(pSocketObj, dwIndex));
			pSocketObj = nullptr;
		}
	}

	if(!pSocketObj) pSocketObj = CreateSocketObj();
	pSocketObj->Reset(dwConnID, soClient);

	return pSocketObj;
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

	ENSURE(m_bfActiveSockets.ReleaseLock(dwConnID, pSocketObj));
}

void CTcpServer::ReleaseFreeSocket()
{
	TSocketObj* pSocketObj = nullptr;

	while(m_lsFreeSocket.TryGet(&pSocketObj))
		DeleteSocketObj(pSocketObj);

	ENSURE(m_lsFreeSocket.IsEmpty());
	m_lsFreeSocket.Reset();

	ReleaseGCSocketObj(TRUE);
	ENSURE(m_lsGCSocket.IsEmpty());
}

TSocketObj* CTcpServer::CreateSocketObj()
{
	return TSocketObj::Construct(m_phSocket, m_bfObjPool);
}

void CTcpServer::DeleteSocketObj(TSocketObj* pSocketObj)
{
	TSocketObj::Destruct(pSocketObj);
}

TBufferObj* CTcpServer::GetFreeBufferObj(int iLen)
{
	ASSERT(iLen >= -1 && iLen <= (int)m_dwSocketBufferSize);

	TBufferObj* pBufferObj	= m_bfObjPool.PickFreeItem();
	if(iLen < 0) iLen		= m_dwSocketBufferSize;
	pBufferObj->buff.len	= iLen;

	return pBufferObj;
}

void CTcpServer::AddFreeBufferObj(TBufferObj* pBufferObj)
{
	m_bfObjPool.PutFreeItem(pBufferObj);
}

void CTcpServer::ReleaseFreeBuffer()
{
	m_bfObjPool.Clear();
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
		TriggerFireClose(pSocketObj, SO_CLOSE, SE_OK);
	else if(enFlag == SCF_ERROR)
		TriggerFireClose(pSocketObj, enOperation, iErrorCode);

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
	{
		if(bForce)
			isOK = ::PostIocpDisconnect(m_hCompletePort, dwConnID);
		else
			isOK = m_pfnDisconnectEx(pSocketObj->socket, nullptr, 0, 0);
	}

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

void CTcpServer::WaitForAcceptSocketClose()
{
	while(m_iRemainAcceptSockets > 0)
		::WaitWithMessageLoop(50);
}

void CTcpServer::WaitForClientSocketClose()
{
	while(m_bfActiveSockets.Elements() > 0)
		::WaitWithMessageLoop(50);
}

void CTcpServer::WaitForWorkerThreadEnd()
{
	int count = (int)m_vtWorkerThreads.size();

	for(int i = 0; i < count; i++)
		::PostIocpExit(m_hCompletePort);

	int remain	= count;
	int index	= 0;

	while(remain > 0)
	{
		int wait = min(remain, MAXIMUM_WAIT_OBJECTS);
		HANDLE* pHandles = (HANDLE*)_alloca(sizeof(HANDLE) * wait);

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

void CTcpServer::CloseCompletePort()
{
	if(m_hCompletePort != nullptr)
	{
		::CloseHandle(m_hCompletePort);
		m_hCompletePort = nullptr;
	}
}

BOOL CTcpServer::DoAccept()
{
	BOOL isOK = FALSE;

	if(HasStarted())
	{
		SOCKET		soClient	= socket(m_usFamily, SOCK_STREAM, IPPROTO_TCP);
		TBufferObj*	pBufferObj	= GetFreeBufferObj();

		ASSERT(soClient != INVALID_SOCKET);

		isOK = (::PostAccept(m_pfnAcceptEx, m_soListen, soClient, pBufferObj, m_usFamily) == NO_ERROR);

		if(!isOK)
		{
			ENSURE(!HasStarted());

			::ManualCloseSocket(soClient);
			AddFreeBufferObj(pBufferObj);
		}
	}

	if(!isOK)
	{
		::InterlockedDecrement(&m_iRemainAcceptSockets);
		ASSERT(m_iRemainAcceptSockets >= 0);
	}

	return isOK;
}

UINT WINAPI CTcpServer::WorkerThreadProc(LPVOID pv)
{
	CTcpServer* pServer = (CTcpServer*)pv;

	while(TRUE)
	{
		DWORD dwErrorCode = NO_ERROR;

		DWORD dwBytes;
		OVERLAPPED* pOverlapped;
		TSocketObj* pSocketObj;
		
		BOOL result = ::GetQueuedCompletionStatus
												(
													pServer->m_hCompletePort,
													&dwBytes,
													(PULONG_PTR)&pSocketObj,
													&pOverlapped,
													INFINITE
												);

		if(pOverlapped == nullptr)
		{
			EnIocpAction action = pServer->CheckIocpCommand(pOverlapped, dwBytes, (ULONG_PTR)pSocketObj);

			if(action == IOCP_ACT_CONTINUE)
				continue;
			else if(action == IOCP_ACT_BREAK)
				break;
		}

		TBufferObj* pBufferObj	= CONTAINING_RECORD(pOverlapped, TBufferObj, ov);
		CONNID dwConnID			= pBufferObj->operation != SO_ACCEPT ? pSocketObj->connID : 0;

		if (!result)
		{
			DWORD dwFlag	= 0;
			DWORD dwSysCode = ::GetLastError();

			if(pServer->HasStarted())
			{
				SOCKET sock	= pBufferObj->operation != SO_ACCEPT ? pBufferObj->client : (SOCKET)pSocketObj;
				result		= ::WSAGetOverlappedResult(sock, &pBufferObj->ov, &dwBytes, FALSE, &dwFlag);

				if (!result)
				{
					dwErrorCode = ::WSAGetLastError();
					TRACE("GetQueuedCompletionStatus error (<S-CNNID: %Iu> SYS: %d, SOCK: %d, FLAG: %d)\n", dwConnID, dwSysCode, dwErrorCode, dwFlag);
				}
			}
			else
				dwErrorCode = dwSysCode;

			ASSERT(dwSysCode != 0 && dwErrorCode != 0);
		}

		pServer->HandleIo(dwConnID, pSocketObj, pBufferObj, dwBytes, dwErrorCode);
	}

	pServer->OnWorkerThreadEnd(::GetCurrentThreadId());

	return 0;
}

EnIocpAction CTcpServer::CheckIocpCommand(OVERLAPPED* pOverlapped, DWORD dwBytes, ULONG_PTR ulCompKey)
{
	ASSERT(pOverlapped == nullptr);

	EnIocpAction action = IOCP_ACT_CONTINUE;
	CONNID dwConnID		= (CONNID)ulCompKey;

	switch(dwBytes)
	{
	case IOCP_CMD_SEND		: DoSend(dwConnID)			; break;
	case IOCP_CMD_ACCEPT	: DoAccept()				; break;
	case IOCP_CMD_DISCONNECT: ForceDisconnect(dwConnID)	; break;
	case IOCP_CMD_EXIT		: action = IOCP_ACT_BREAK	; break;
	default					: CheckError(FindSocketObj(dwConnID), SO_CLOSE, (int)dwBytes);
	}

	return action;
}

void CTcpServer::ForceDisconnect(CONNID dwConnID)
{
	AddFreeSocketObj(FindSocketObj(dwConnID), SCF_CLOSE);
}

void CTcpServer::HandleIo(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj, DWORD dwBytes, DWORD dwErrorCode)
{
	ASSERT(pBufferObj != nullptr);
	ASSERT(pSocketObj != nullptr);

	if(dwErrorCode != NO_ERROR)
	{
		HandleError(dwConnID, pSocketObj, pBufferObj, dwErrorCode);
		return;
	}

	if(dwBytes == 0 && pBufferObj->operation != SO_ACCEPT)
	{
		AddFreeSocketObj(pSocketObj, SCF_CLOSE);
		AddFreeBufferObj(pBufferObj);
		return;
	}

	pBufferObj->buff.len = dwBytes;

	switch(pBufferObj->operation)
	{
	case SO_ACCEPT:
		HandleAccept((SOCKET)pSocketObj, pBufferObj);
		break;
	case SO_SEND:
		HandleSend(dwConnID, pSocketObj, pBufferObj);
		break;
	case SO_RECEIVE:
		HandleReceive(dwConnID, pSocketObj, pBufferObj);
		break;
	default:
		ASSERT(FALSE);
	}
}

void CTcpServer::HandleError(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj, DWORD dwErrorCode)
{
	if(pBufferObj->operation != SO_ACCEPT)
		CheckError(pSocketObj, pBufferObj->operation, dwErrorCode);
	else
	{
		::ManualCloseSocket(pBufferObj->client);
		ENSURE(::PostIocpAccept(m_hCompletePort));
	}

	AddFreeBufferObj(pBufferObj);
}

void CTcpServer::HandleAccept(SOCKET soListen, TBufferObj* pBufferObj)
{
	ENSURE(::PostIocpAccept(m_hCompletePort));

	int iLocalSockaddrLen;
	int iRemoteSockaddrLen;
	HP_PSOCKADDR pLocalSockAddr;
	HP_PSOCKADDR pRemoteSockAddr;

	int iAddrLen = HP_SOCKADDR::AddrSize(m_usFamily) + 16;

	m_pfnGetAcceptExSockaddrs
							(
								pBufferObj->buff.buf,
								0,
								iAddrLen,
								iAddrLen,
								(SOCKADDR**)&pLocalSockAddr,
								&iLocalSockaddrLen,
								(SOCKADDR**)&pRemoteSockAddr,
								&iRemoteSockaddrLen
							);

	CONNID dwConnID = 0;
	SOCKET socket	= pBufferObj->client;

	if(!HasStarted() || !m_bfActiveSockets.AcquireLock(dwConnID))
	{
		::ManualCloseSocket(socket, SD_BOTH);
		AddFreeBufferObj(pBufferObj);

		return;
	}

	TSocketObj* pSocketObj = GetFreeSocketObj(dwConnID, socket);

	AddClientSocketObj(dwConnID, pSocketObj, *pRemoteSockAddr);

	::SSO_UpdateAcceptContext(socket, soListen);
	::CreateIoCompletionPort((HANDLE)socket, m_hCompletePort, (ULONG_PTR)pSocketObj, 0);

	if(TriggerFireAccept(pSocketObj) != HR_ERROR)
		DoReceive(pSocketObj, pBufferObj);
	else
	{
		AddFreeSocketObj(pSocketObj, SCF_NONE);
		AddFreeBufferObj(pBufferObj);
	}
}

void CTcpServer::HandleSend(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	int iLength = -(long)(pBufferObj->buff.len);

	switch(m_enSendPolicy)
	{
	case SP_PACK:
		{
			::InterlockedExchangeAdd(&pSocketObj->sndCount, iLength);

			TriggerFireSend(pSocketObj, pBufferObj);

			DoSendPack(pSocketObj);
		}

		break;
	case SP_SAFE:
		{
			::InterlockedExchangeAdd(&pSocketObj->sndCount, iLength);

			TriggerFireSend(pSocketObj, pBufferObj);

			DoSendSafe(pSocketObj);
		}

		break;
	case SP_DIRECT:
		{
			TriggerFireSend(pSocketObj, pBufferObj);
		}

		break;
	default:
		ASSERT(FALSE);
	}
}

void CTcpServer::HandleReceive(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	if(m_bMarkSilence) pSocketObj->activeTime = ::TimeGetTime();

	EnHandleResult hr = TriggerFireReceive(pSocketObj, pBufferObj);

	if(hr == HR_OK || hr == HR_IGNORE)
	{
		if(ContinueReceive(pSocketObj, pBufferObj, hr))
		{
			{
				CSpinLock locallock(pSocketObj->sgPause);

				pSocketObj->recving = FALSE;
			}

			DoReceive(pSocketObj, pBufferObj);
		}
	}

	if(hr == HR_CLOSED)
	{
		AddFreeBufferObj(pBufferObj);
	}
	else if(hr == HR_ERROR)
	{
		TRACE("<S-CNNID: %Iu> OnReceive() event return 'HR_ERROR', connection will be closed !\n", dwConnID);

		AddFreeSocketObj(pSocketObj, SCF_ERROR, SO_RECEIVE, ENSURE_ERROR_CANCELLED);
		AddFreeBufferObj(pBufferObj);
	}
}

BOOL CTcpServer::ContinueReceive(TSocketObj* pSocketObj, TBufferObj* pBufferObj, EnHandleResult& hr)
{
	int rs = NO_ERROR;

	for(int i = 0; i < MAX_IOCP_CONTINUE_RECEIVE || MAX_IOCP_CONTINUE_RECEIVE < 0; i++)
	{
		if(hr != HR_OK && hr != HR_IGNORE)
			break;

		if(pBufferObj->buff.len != m_dwSocketBufferSize)
			break;

		pBufferObj->buff.len = m_dwSocketBufferSize;
		rs = ::NoBlockReceiveNotCheck(pBufferObj);

		if(rs != NO_ERROR)
			break;

		hr = TriggerFireReceive(pSocketObj, pBufferObj);
	}

	if(hr != HR_OK && hr != HR_IGNORE)
		return FALSE;

	if(rs != NO_ERROR && rs != WSAEWOULDBLOCK)
	{
		if(rs == WSAEDISCON)
			AddFreeSocketObj(pSocketObj, SCF_CLOSE);
		else
			CheckError(pSocketObj, SO_RECEIVE, rs);
		
		AddFreeBufferObj(pBufferObj);

		return FALSE;
	}

	return TRUE;
}

int CTcpServer::DoReceive(TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	int result		= NO_ERROR;
	BOOL bNeedFree	= FALSE;

	{
		CSpinLock locallock(pSocketObj->sgPause);

		if(pSocketObj->paused || pSocketObj->recving)
			bNeedFree = TRUE;
		else
		{
			pSocketObj->recving	 = TRUE;
			pBufferObj->buff.len = m_dwSocketBufferSize;

			result = ::PostReceive(pSocketObj, pBufferObj);
		}
	}

	if(result != NO_ERROR)
	{
		CheckError(pSocketObj, SO_RECEIVE, result);
		bNeedFree = TRUE;
	}

	if(bNeedFree) AddFreeBufferObj(pBufferObj);

	return result;
}

BOOL CTcpServer::PauseReceive(CONNID dwConnID, BOOL bPause)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(!TSocketObj::IsValid(pSocketObj))
	{
		::SetLastError(ERROR_OBJECT_NOT_FOUND);
		return FALSE;
	}

	{
		CSpinLock locallock(pSocketObj->sgPause);

		if(pSocketObj->paused == bPause)
			return TRUE;

		pSocketObj->paused = bPause;
	}

	if(!bPause)
		return (DoReceive(pSocketObj, GetFreeBufferObj()) == NO_ERROR);

	return TRUE;
}

BOOL CTcpServer::Send(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset)
{
	ASSERT(pBuffer && iLength > 0);

	if(iOffset != 0) pBuffer += iOffset;

	WSABUF buffer;
	buffer.len = iLength;
	buffer.buf = (char*)pBuffer;

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
		CCriSecLock locallock(pSocketObj->csSend);

		if(TSocketObj::IsValid(pSocketObj))
			result = SendInternal(pSocketObj, pBuffers, iCount);
		else
			result = ERROR_OBJECT_NOT_FOUND;
	}
	else
		result = ERROR_INVALID_PARAMETER;

	if(result != NO_ERROR)
	{
		if(m_enSendPolicy == SP_DIRECT && TSocketObj::IsValid(pSocketObj))
			::PostIocpClose(m_hCompletePort, pSocketObj->connID, result);

		::SetLastError(result);
	}

	return (result == NO_ERROR);
}

int CTcpServer::SendInternal(TSocketObj* pSocketObj, const WSABUF pBuffers[], int iCount)
{
	int result = NO_ERROR;

	for(int i = 0; i < iCount; i++)
	{
		int iBufLen = pBuffers[i].len;

		if(iBufLen > 0)
		{
			BYTE* pBuffer = (BYTE*)pBuffers[i].buf;
			ASSERT(pBuffer);

			switch(m_enSendPolicy)
			{
			case SP_PACK:	result = SendPack(pSocketObj, pBuffer, iBufLen);	break;
			case SP_SAFE:	result = SendSafe(pSocketObj, pBuffer, iBufLen);	break;
			case SP_DIRECT:	result = SendDirect(pSocketObj, pBuffer, iBufLen);	break;
			default: ASSERT(FALSE);	result = ERROR_INVALID_INDEX;				break;
			}

			if(result != NO_ERROR)
				break;
		}
	}

	return result;
}

int CTcpServer::SendPack(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength)
{
	return CatAndPost(pSocketObj, pBuffer, iLength);
}

int CTcpServer::SendSafe(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength)
{
	return CatAndPost(pSocketObj, pBuffer, iLength);
}

int CTcpServer::SendDirect(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength)
{
	int result	= NO_ERROR;
	int iRemain	= iLength;

	while(iRemain > 0)
	{
		int iBufferSize = min(iRemain, (int)m_dwSocketBufferSize);
		TBufferObj* pBufferObj = GetFreeBufferObj(iBufferSize);
		memcpy(pBufferObj->buff.buf, pBuffer, iBufferSize);

		result			= ::PostSend(pSocketObj, pBufferObj);
		LONG sndCounter	= pBufferObj->ReleaseSendCounter();

		if(sndCounter == 0 || result != NO_ERROR)
		{
			AddFreeBufferObj(pBufferObj);
			
			if(result != NO_ERROR)
				break;
		}

		iRemain -= iBufferSize;
		pBuffer += iBufferSize;
	}

	return result;
}

int CTcpServer::CatAndPost(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength)
{
	int result = NO_ERROR;

	pSocketObj->sndBuff.Cat(pBuffer, iLength);
	pSocketObj->pending += iLength;

	if(pSocketObj->IsCanSend() && pSocketObj->IsSmooth() && !::PostIocpSend(m_hCompletePort, pSocketObj->connID))
		result = ::GetLastError();

	return result;
}

int CTcpServer::DoSend(CONNID dwConnID)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
		return DoSend(pSocketObj);

	return ERROR_OBJECT_NOT_FOUND;
}

int CTcpServer::DoSend(TSocketObj* pSocketObj)
{
	switch(m_enSendPolicy)
	{
	case SP_PACK:			return DoSendPack(pSocketObj);
	case SP_SAFE:			return DoSendSafe(pSocketObj);
	default: ASSERT(FALSE);	return ERROR_INVALID_INDEX;
	}
}

int CTcpServer::DoSendPack(TSocketObj* pSocketObj)
{
	if(!pSocketObj->IsCanSend())
		return NO_ERROR;

	int result = NO_ERROR;

	if(pSocketObj->IsPending() && pSocketObj->TurnOffSmooth())
	{
		{
			CCriSecLock locallock(pSocketObj->csSend);

			if(!TSocketObj::IsValid(pSocketObj))
				return ERROR_OBJECT_NOT_FOUND;

			if(pSocketObj->IsPending())
				result = SendItem(pSocketObj);

			pSocketObj->TurnOnSmooth();
		}

		if(result == WSA_IO_PENDING && pSocketObj->IsSmooth())
			::PostIocpSend(m_hCompletePort, pSocketObj->connID);
	}

	if(!IOCP_SUCCESS(result))
		CheckError(pSocketObj, SO_SEND, result);

	return result;
}

int CTcpServer::DoSendSafe(TSocketObj* pSocketObj)
{
	if(pSocketObj->sndCount == 0 && !pSocketObj->IsSmooth())
	{
		CCriSecLock locallock(pSocketObj->csSend);

		if(!TSocketObj::IsValid(pSocketObj))
			return ERROR_OBJECT_NOT_FOUND;

		if(pSocketObj->sndCount == 0)
			pSocketObj->smooth = TRUE;
	}

	if(!pSocketObj->IsCanSend())
		return NO_ERROR;

	int result = NO_ERROR;

	if(pSocketObj->IsPending() && pSocketObj->IsSmooth())
	{
		CCriSecLock locallock(pSocketObj->csSend);

		if(!TSocketObj::IsValid(pSocketObj))
			return ERROR_OBJECT_NOT_FOUND;

		if(pSocketObj->IsPending() && pSocketObj->IsSmooth())
		{
			pSocketObj->smooth = FALSE;

			result = SendItem(pSocketObj);

			if(result == NO_ERROR)
				pSocketObj->smooth = TRUE;
		}
	}

	if(!IOCP_SUCCESS(result))
		CheckError(pSocketObj, SO_SEND, result);

	return result;
}

int CTcpServer::SendItem(TSocketObj* pSocketObj)
{
	int result = NO_ERROR;

	while(pSocketObj->sndBuff.Size() > 0)
	{
		TBufferObj* pBufferObj	= pSocketObj->sndBuff.PopFront();
		int iBufferSize			= pBufferObj->buff.len;

		ASSERT(iBufferSize > 0 && iBufferSize <= (int)m_dwSocketBufferSize);

		pSocketObj->pending	   -= iBufferSize;
		::InterlockedExchangeAdd(&pSocketObj->sndCount, iBufferSize);

		result			= ::PostSendNotCheck(pSocketObj, pBufferObj);
		LONG sndCounter	= pBufferObj->ReleaseSendCounter();

		if(sndCounter == 0 || !IOCP_SUCCESS(result))
			AddFreeBufferObj(pBufferObj);

		if(result != NO_ERROR)
			break;
	}

	return result;
}

BOOL CTcpServer::SendSmallFile(CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail)
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

	return SendPackets(dwConnID, szBuf, 3);
}

void CTcpServer::CheckError(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
{
	if(iErrorCode != WSAENOTSOCK && iErrorCode != ERROR_OPERATION_ABORTED)
		AddFreeSocketObj(pSocketObj, SCF_ERROR, enOperation, iErrorCode);
}
