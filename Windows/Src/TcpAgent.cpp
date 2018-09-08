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
#include "TcpAgent.h"
#include "../Common/Src/WaitFor.h"

#include <malloc.h>
#include <process.h>

const CInitSocket CTcpAgent::sm_wsSocket;

EnHandleResult CTcpAgent::TriggerFireConnect(TSocketObj* pSocketObj)
{
	CCriSecLock locallock(pSocketObj->csRecv);
	return TRIGGER(FireConnect(pSocketObj));
}

EnHandleResult CTcpAgent::TriggerFireReceive(TSocketObj* pSocketObj, TBufferObj* pBufferObj)
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

EnHandleResult CTcpAgent::TriggerFireSend(TSocketObj* pSocketObj, TBufferObj* pBufferObj)
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

EnHandleResult CTcpAgent::TriggerFireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
{
	CCriSecLock locallock(pSocketObj->csRecv);
	return FireClose(pSocketObj, enOperation, iErrorCode);
}

void CTcpAgent::SetLastError(EnSocketError code, LPCSTR func, int ec)
{
	m_enLastError = code;
	::SetLastError(ec);

	TRACE("%s --> Error: %d, EC: %d\n", func, code, ec);
}

BOOL CTcpAgent::Start(LPCTSTR lpszBindAddress, BOOL bAsyncConnect)
{
	if(!CheckParams() || !CheckStarting())
		return FALSE;

	PrepareStart();

	if(ParseBindAddress(lpszBindAddress))
		if(CreateCompletePort())
			if(CreateWorkerThreads())
			{
				m_bAsyncConnect	= bAsyncConnect;
				m_enState		= SS_STARTED;

				return TRUE;
			}

	EXECUTE_RESTORE_ERROR(Stop());

	return FALSE;
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
	BOOL isOK	= FALSE;
	BOOL bBind	= (lpszBindAddress && lpszBindAddress[0] != 0);

	if(!bBind) lpszBindAddress = DEFAULT_IPV4_BIND_ADDRESS;

	HP_SOCKADDR addr;

	if(::sockaddr_A_2_IN(lpszBindAddress, 0, addr))
	{
		SOCKET sock	= socket(addr.family, SOCK_STREAM, IPPROTO_TCP);

		if(sock != INVALID_SOCKET)
		{

			if(::bind(sock, addr.Addr(), addr.AddrSize()) != SOCKET_ERROR)
			{
				m_pfnConnectEx		= ::Get_ConnectEx_FuncPtr(sock);
				m_pfnDisconnectEx	= ::Get_DisconnectEx_FuncPtr(sock);

				ASSERT(m_pfnConnectEx);
				ASSERT(m_pfnDisconnectEx);

				if(bBind) addr.Copy(m_soAddr);

				isOK = TRUE;
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

	return isOK;
}

BOOL CTcpAgent::CreateCompletePort()
{
	m_hCompletePort	= ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
	if(m_hCompletePort == nullptr)
		SetLastError(SE_CP_CREATE, __FUNCTION__, ::GetLastError());

	return (m_hCompletePort != nullptr);
}

BOOL CTcpAgent::CreateWorkerThreads()
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
	ReleaseFreeBuffer();

	CloseCompletePort();

	Reset();

	return TRUE;
}

void CTcpAgent::Reset()
{
	m_phSocket.Reset();
	m_soAddr.Reset();

	m_pfnConnectEx				= nullptr;
	m_pfnDisconnectEx			= nullptr;
	m_enState					= SS_STOPPED;
}

void CTcpAgent::DisconnectClientSocket()
{
	::WaitWithMessageLoop(100);

	DWORD size					= 0;
	unique_ptr<CONNID[]> ids	= m_bfActiveSockets.GetAllElementIndexes(size);

	for(DWORD i = 0; i < size; i++)
		Disconnect(ids[i]);
}

void CTcpAgent::ReleaseClientSocket()
{
	ENSURE(m_bfActiveSockets.IsEmpty());
	m_bfActiveSockets.Reset();
}

TSocketObj*	CTcpAgent::GetFreeSocketObj(CONNID dwConnID, SOCKET soClient)
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

void CTcpAgent::AddFreeSocketObj(TSocketObj* pSocketObj, EnSocketCloseFlag enFlag, EnSocketOperation enOperation, int iErrorCode)
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

void CTcpAgent::ReleaseGCSocketObj(BOOL bForce)
{
	::ReleaseGCObj(m_lsGCSocket, m_dwFreeSocketObjLockTime, bForce);
}

BOOL CTcpAgent::InvalidSocketObj(TSocketObj* pSocketObj)
{
	return TSocketObj::InvalidSocketObj(pSocketObj);
}

void CTcpAgent::AddClientSocketObj(CONNID dwConnID, TSocketObj* pSocketObj, const HP_SOCKADDR& remoteAddr, LPCTSTR lpszRemoteAddress, PVOID pExtra)
{
	ASSERT(FindSocketObj(dwConnID) == nullptr);

	pSocketObj->connTime	= ::TimeGetTime();
	pSocketObj->activeTime	= pSocketObj->connTime;
	pSocketObj->host		= lpszRemoteAddress;
	pSocketObj->extra		= pExtra;

	remoteAddr.Copy(pSocketObj->remoteAddr);

	ENSURE(m_bfActiveSockets.ReleaseLock(dwConnID, pSocketObj));
}

void CTcpAgent::ReleaseFreeSocket()
{
	TSocketObj* pSocketObj = nullptr;

	while(m_lsFreeSocket.TryGet(&pSocketObj))
		DeleteSocketObj(pSocketObj);

	ENSURE(m_lsFreeSocket.IsEmpty());
	m_lsFreeSocket.Reset();

	ReleaseGCSocketObj(TRUE);
	ENSURE(m_lsGCSocket.IsEmpty());
}

TSocketObj* CTcpAgent::CreateSocketObj()
{
	return TSocketObj::Construct(m_phSocket, m_bfObjPool);
}

void CTcpAgent::DeleteSocketObj(TSocketObj* pSocketObj)
{
	TSocketObj::Destruct(pSocketObj);
}

TBufferObj* CTcpAgent::GetFreeBufferObj(int iLen)
{
	ASSERT(iLen >= -1 && iLen <= (int)m_dwSocketBufferSize);

	TBufferObj* pBufferObj	= m_bfObjPool.PickFreeItem();
	if(iLen < 0) iLen		= m_dwSocketBufferSize;
	pBufferObj->buff.len	= iLen;

	return pBufferObj;
}

void CTcpAgent::AddFreeBufferObj(TBufferObj* pBufferObj)
{
	m_bfObjPool.PutFreeItem(pBufferObj);
}

void CTcpAgent::ReleaseFreeBuffer()
{
	m_bfObjPool.Clear();
}

TSocketObj* CTcpAgent::FindSocketObj(CONNID dwConnID)
{
	TSocketObj* pSocketObj = nullptr;

	if(m_bfActiveSockets.Get(dwConnID, &pSocketObj) != TSocketObjPtrPool::GR_VALID)
		pSocketObj = nullptr;

	return pSocketObj;
}

void CTcpAgent::CloseClientSocketObj(TSocketObj* pSocketObj, EnSocketCloseFlag enFlag, EnSocketOperation enOperation, int iErrorCode, int iShutdownFlag)
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

BOOL CTcpAgent::GetLocalAddress(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
		return ::GetSocketLocalAddress(pSocketObj->socket, lpszAddress, iAddressLen, usPort);

	return FALSE;
}

BOOL CTcpAgent::GetRemoteAddress(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
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

BOOL CTcpAgent::GetRemoteHost(CONNID dwConnID, TCHAR lpszHost[], int& iHostLen, USHORT& usPort)
{
	ASSERT(lpszHost != nullptr && iHostLen > 0);

	BOOL isOK				= FALSE;
	TSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TSocketObj::IsExist(pSocketObj))
	{
		int iLen = pSocketObj->host.GetLength() + 1;

		if(iHostLen >= iLen)
		{
			memcpy(lpszHost, CA2CT(pSocketObj->host), iLen * sizeof(TCHAR));
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
	TSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TSocketObj::IsExist(pSocketObj))
	{
		*lpszHost = pSocketObj->host;

		if(pusPort)
			*pusPort = pSocketObj->remoteAddr.Port();
	}

	return (*lpszHost != nullptr && (*lpszHost)[0] != 0);
}

BOOL CTcpAgent::SetConnectionExtra(CONNID dwConnID, PVOID pExtra)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return SetConnectionExtra(pSocketObj, pExtra);
}

BOOL CTcpAgent::SetConnectionExtra(TSocketObj* pSocketObj, PVOID pExtra)
{
	if(TSocketObj::IsExist(pSocketObj))
	{
		pSocketObj->extra = pExtra;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpAgent::GetConnectionExtra(CONNID dwConnID, PVOID* ppExtra)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return GetConnectionExtra(pSocketObj, ppExtra);
}

BOOL CTcpAgent::GetConnectionExtra(TSocketObj* pSocketObj, PVOID* ppExtra)
{
	ASSERT(ppExtra != nullptr);

	if(TSocketObj::IsExist(pSocketObj))
	{
		*ppExtra = pSocketObj->extra;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpAgent::SetConnectionReserved(CONNID dwConnID, PVOID pReserved)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return SetConnectionReserved(pSocketObj, pReserved);
}

BOOL CTcpAgent::SetConnectionReserved(TSocketObj* pSocketObj, PVOID pReserved)
{
	if(TSocketObj::IsExist(pSocketObj))
	{
		pSocketObj->reserved = pReserved;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpAgent::GetConnectionReserved(CONNID dwConnID, PVOID* ppReserved)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return GetConnectionReserved(pSocketObj, ppReserved);
}

BOOL CTcpAgent::GetConnectionReserved(TSocketObj* pSocketObj, PVOID* ppReserved)
{
	ASSERT(ppReserved != nullptr);

	if(TSocketObj::IsExist(pSocketObj))
	{
		*ppReserved = pSocketObj->reserved;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpAgent::SetConnectionReserved2(CONNID dwConnID, PVOID pReserved2)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return SetConnectionReserved2(pSocketObj, pReserved2);
}

BOOL CTcpAgent::SetConnectionReserved2(TSocketObj* pSocketObj, PVOID pReserved2)
{
	if(TSocketObj::IsExist(pSocketObj))
	{
		pSocketObj->reserved2 = pReserved2;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpAgent::GetConnectionReserved2(CONNID dwConnID, PVOID* ppReserved2)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return GetConnectionReserved2(pSocketObj, ppReserved2);
}

BOOL CTcpAgent::GetConnectionReserved2(TSocketObj* pSocketObj, PVOID* ppReserved2)
{
	ASSERT(ppReserved2 != nullptr);

	if(TSocketObj::IsExist(pSocketObj))
	{
		*ppReserved2 = pSocketObj->reserved2;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpAgent::IsPauseReceive(CONNID dwConnID, BOOL& bPaused)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
	{
		bPaused = pSocketObj->paused;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpAgent::IsConnected(CONNID dwConnID)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
		return pSocketObj->HasConnected();

	return FALSE;
}

BOOL CTcpAgent::GetPendingDataLength(CONNID dwConnID, int& iPending)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
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
	BOOL isOK				= TRUE;
	TSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
		dwPeriod = ::GetTimeGap32(pSocketObj->connTime);
	else
		isOK = FALSE;

	return isOK;
}

BOOL CTcpAgent::GetSilencePeriod(CONNID dwConnID, DWORD& dwPeriod)
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

BOOL CTcpAgent::Disconnect(CONNID dwConnID, BOOL bForce)
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
		TSocketObj* pSocketObj	= FindSocketObj(connID);

		if(TSocketObj::IsValid(pSocketObj) && (int)(now - pSocketObj->connTime) >= (int)dwPeriod)
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
		TSocketObj* pSocketObj	= FindSocketObj(connID);

		if(TSocketObj::IsValid(pSocketObj) && (int)(now - pSocketObj->activeTime) >= (int)dwPeriod)
			Disconnect(connID, bForce);
	}

	return TRUE;
}

void CTcpAgent::WaitForClientSocketClose()
{
	while(m_bfActiveSockets.Elements() > 0)
		::WaitWithMessageLoop(50);
}

void CTcpAgent::WaitForWorkerThreadEnd()
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

void CTcpAgent::CloseCompletePort()
{
	if(m_hCompletePort != nullptr)
	{
		::CloseHandle(m_hCompletePort);
		m_hCompletePort = nullptr;
	}
}

UINT WINAPI CTcpAgent::WorkerThreadProc(LPVOID pv)
{
	CTcpAgent* pAgent = (CTcpAgent*)pv;

	while(TRUE)
	{
		DWORD dwErrorCode = NO_ERROR;

		DWORD dwBytes;
		OVERLAPPED* pOverlapped;
		TSocketObj* pSocketObj;
		
		BOOL result = ::GetQueuedCompletionStatus
												(
													pAgent->m_hCompletePort,
													&dwBytes,
													(PULONG_PTR)&pSocketObj,
													&pOverlapped,
													INFINITE
												);

		if(pOverlapped == nullptr)
		{
			EnIocpAction action = pAgent->CheckIocpCommand(pOverlapped, dwBytes, (ULONG_PTR)pSocketObj);

			if(action == IOCP_ACT_CONTINUE)
				continue;
			else if(action == IOCP_ACT_BREAK)
				break;
		}

		TBufferObj* pBufferObj	= CONTAINING_RECORD(pOverlapped, TBufferObj, ov);
		CONNID dwConnID			= pSocketObj->connID;

		if (!result)
		{
			DWORD dwFlag	= 0;
			DWORD dwSysCode = ::GetLastError();

			if(pAgent->HasStarted())
			{
				SOCKET sock	= pBufferObj->client;
				result		= ::WSAGetOverlappedResult(sock, &pBufferObj->ov, &dwBytes, FALSE, &dwFlag);

				if (!result)
				{
					dwErrorCode = ::WSAGetLastError();
					TRACE("GetQueuedCompletionStatus error (<A-CNNID: %Iu> SYS: %d, SOCK: %d, FLAG: %d)\n", dwConnID, dwSysCode, dwErrorCode, dwFlag);
				}
			}
			else
				dwErrorCode = dwSysCode;

			ASSERT(dwSysCode != 0 && dwErrorCode != 0);
		}

		pAgent->HandleIo(dwConnID, pSocketObj, pBufferObj, dwBytes, dwErrorCode);
	}

	pAgent->OnWorkerThreadEnd(::GetCurrentThreadId());

	return 0;
}

EnIocpAction CTcpAgent::CheckIocpCommand(OVERLAPPED* pOverlapped, DWORD dwBytes, ULONG_PTR ulCompKey)
{
	ASSERT(pOverlapped == nullptr);

	EnIocpAction action = IOCP_ACT_CONTINUE;
	CONNID dwConnID		= (CONNID)ulCompKey;

	switch(dwBytes)
	{
	case IOCP_CMD_SEND		: DoSend(dwConnID)			; break;
	case IOCP_CMD_DISCONNECT: ForceDisconnect(dwConnID)	; break;
	case IOCP_CMD_EXIT		: action = IOCP_ACT_BREAK	; break;
	default					: CheckError(FindSocketObj(dwConnID), SO_CLOSE, (int)dwBytes);
	}

	return action;
}

void CTcpAgent::ForceDisconnect(CONNID dwConnID)
{
	AddFreeSocketObj(FindSocketObj(dwConnID), SCF_CLOSE);
}

void CTcpAgent::HandleIo(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj, DWORD dwBytes, DWORD dwErrorCode)
{
	ASSERT(pBufferObj != nullptr);
	ASSERT(pSocketObj != nullptr);

	if(dwErrorCode != NO_ERROR)
	{
		HandleError(dwConnID, pSocketObj, pBufferObj, dwErrorCode);
		return;
	}

	if(dwBytes == 0 && pBufferObj->operation != SO_CONNECT)
	{
		AddFreeSocketObj(pSocketObj, SCF_CLOSE);
		AddFreeBufferObj(pBufferObj);
		return;
	}

	pBufferObj->buff.len = dwBytes;

	switch(pBufferObj->operation)
	{
	case SO_CONNECT:
		HandleConnect(dwConnID, pSocketObj, pBufferObj);
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

void CTcpAgent::HandleError(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj, DWORD dwErrorCode)
{
	CheckError(pSocketObj, pBufferObj->operation, dwErrorCode);
	AddFreeBufferObj(pBufferObj);
}

void CTcpAgent::HandleConnect(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	::SSO_UpdateConnectContext(pSocketObj->socket, 0);

	pSocketObj->SetConnected();

	if(TriggerFireConnect(pSocketObj) != HR_ERROR)
		DoReceive(pSocketObj, pBufferObj);
	else
	{
		AddFreeSocketObj(pSocketObj, SCF_NONE);
		AddFreeBufferObj(pBufferObj);
	}
}

void CTcpAgent::HandleSend(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj)
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

void CTcpAgent::HandleReceive(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj)
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

BOOL CTcpAgent::ContinueReceive(TSocketObj* pSocketObj, TBufferObj* pBufferObj, EnHandleResult& hr)
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

int CTcpAgent::DoReceive(TSocketObj* pSocketObj, TBufferObj* pBufferObj)
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

BOOL CTcpAgent::PauseReceive(CONNID dwConnID, BOOL bPause)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(!TSocketObj::IsValid(pSocketObj))
	{
		::SetLastError(ERROR_OBJECT_NOT_FOUND);
		return FALSE;
	}

	if(!pSocketObj->HasConnected())
	{
		::SetLastError(ERROR_INVALID_STATE);
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

DWORD CTcpAgent::CreateClientSocket(LPCTSTR lpszRemoteAddress, USHORT usPort, USHORT usLocalPort, SOCKET& soClient, HP_SOCKADDR& addr)
{
	if(!::GetSockAddrByHostName(lpszRemoteAddress, usPort, addr))
		return WSAEADDRNOTAVAIL;

	BOOL bBind = m_soAddr.IsSpecified();

	if(bBind && m_soAddr.family != addr.family)
		return WSAEAFNOSUPPORT;

	DWORD result = NO_ERROR;
	soClient	 = socket(addr.family, SOCK_STREAM, IPPROTO_TCP);

	if(soClient == INVALID_SOCKET)
		result = ::WSAGetLastError();
	else
	{
		BOOL bOnOff	= (m_dwKeepAliveTime > 0 && m_dwKeepAliveInterval > 0);
		ENSURE(::SSO_KeepAliveVals(soClient, bOnOff, m_dwKeepAliveTime, m_dwKeepAliveInterval) != SOCKET_ERROR);
		ENSURE(::SSO_ReuseAddress(soClient, m_bReuseAddress) != SOCKET_ERROR);

		if(usLocalPort == 0)
		{
			const HP_SOCKADDR& bindAddr = bBind ? m_soAddr : HP_SOCKADDR::AnyAddr(addr.family);

			if(::bind(soClient, bindAddr.Addr(), bindAddr.AddrSize()) == SOCKET_ERROR)
				result = ::WSAGetLastError();
		}
		else
		{
			HP_SOCKADDR bindAddr = bBind ? m_soAddr : HP_SOCKADDR::AnyAddr(addr.family);

			bindAddr.SetPort(usLocalPort);

			if(::bind(soClient, bindAddr.Addr(), bindAddr.AddrSize()) == SOCKET_ERROR)
				result = ::WSAGetLastError();
		}
	}

	return result;
}

DWORD CTcpAgent::PrepareConnect(CONNID& dwConnID, SOCKET soClient)
{
	if(!m_bfActiveSockets.AcquireLock(dwConnID))
		return ERROR_CONNECTION_COUNT_LIMIT;

	if(TRIGGER(FirePrepareConnect(dwConnID, soClient)) == HR_ERROR)
	{
		ENSURE(m_bfActiveSockets.ReleaseLock(dwConnID, nullptr));
		return ENSURE_ERROR_CANCELLED;
	}

	return NO_ERROR;
}

DWORD CTcpAgent::ConnectToServer(CONNID dwConnID, LPCTSTR lpszRemoteAddress, SOCKET soClient, const HP_SOCKADDR& addr, PVOID pExtra)
{
	TBufferObj* pBufferObj = GetFreeBufferObj();
	TSocketObj* pSocketObj = GetFreeSocketObj(dwConnID, soClient);

	AddClientSocketObj(dwConnID, pSocketObj, addr, lpszRemoteAddress, pExtra);

	DWORD result	= NO_ERROR;
	BOOL bNeedFree	= TRUE;

	if(m_bAsyncConnect)
	{
		ENSURE(::SSO_NoBlock(pSocketObj->socket) == NO_ERROR);

		if(::CreateIoCompletionPort((HANDLE)pSocketObj->socket, m_hCompletePort, (ULONG_PTR)pSocketObj, 0))
			result = ::PostConnect(m_pfnConnectEx, pSocketObj->socket, addr, pBufferObj);
		else
			result = ::GetLastError();
	}
	else
	{
		if(::connect(pSocketObj->socket, addr.Addr(), addr.AddrSize()) != SOCKET_ERROR)
		{
			ENSURE(::SSO_NoBlock(pSocketObj->socket) == NO_ERROR);

			if(::CreateIoCompletionPort((HANDLE)pSocketObj->socket, m_hCompletePort, (ULONG_PTR)pSocketObj, 0))
			{
				pSocketObj->SetConnected();

				if(TriggerFireConnect(pSocketObj) != HR_ERROR)
				{
					result		= DoReceive(pSocketObj, pBufferObj);
					bNeedFree	= FALSE;
				}
				else
					result = ENSURE_ERROR_CANCELLED;
			}
			else
				result = ::GetLastError();
		}
		else
			result = ::WSAGetLastError();
	}

	if(result != NO_ERROR)
	{
		if(bNeedFree)
		{
			AddFreeSocketObj(pSocketObj, SCF_NONE);
			AddFreeBufferObj(pBufferObj);
		}
	}

	return result;
}

BOOL CTcpAgent::Send(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset)
{
	ASSERT(pBuffer && iLength > 0);

	if(iOffset != 0) pBuffer += iOffset;

	WSABUF buffer;
	buffer.len = iLength;
	buffer.buf = (char*)pBuffer;

	return SendPackets(dwConnID, &buffer, 1);
}

BOOL CTcpAgent::DoSendPackets(CONNID dwConnID, const WSABUF pBuffers[], int iCount)
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

BOOL CTcpAgent::DoSendPackets(TSocketObj* pSocketObj, const WSABUF pBuffers[], int iCount)
{
	ASSERT(pSocketObj && pBuffers && iCount > 0);

	if(!pSocketObj->HasConnected())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

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

int CTcpAgent::SendInternal(TSocketObj* pSocketObj, const WSABUF pBuffers[], int iCount)
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

int CTcpAgent::SendPack(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength)
{
	return CatAndPost(pSocketObj, pBuffer, iLength);
}

int CTcpAgent::SendSafe(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength)
{
	return CatAndPost(pSocketObj, pBuffer, iLength);
}

int CTcpAgent::SendDirect(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength)
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

int CTcpAgent::CatAndPost(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength)
{
	int result = NO_ERROR;

	pSocketObj->sndBuff.Cat(pBuffer, iLength);
	pSocketObj->pending += iLength;

	if(pSocketObj->IsCanSend() && pSocketObj->IsSmooth() && !::PostIocpSend(m_hCompletePort, pSocketObj->connID))
		result = ::GetLastError();

	return result;
}

int CTcpAgent::DoSend(CONNID dwConnID)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
		return DoSend(pSocketObj);

	return ERROR_OBJECT_NOT_FOUND;
}

int CTcpAgent::DoSend(TSocketObj* pSocketObj)
{
	switch(m_enSendPolicy)
	{
	case SP_PACK:			return DoSendPack(pSocketObj);
	case SP_SAFE:			return DoSendSafe(pSocketObj);
	default: ASSERT(FALSE);	return ERROR_INVALID_INDEX;
	}
}

int CTcpAgent::DoSendPack(TSocketObj* pSocketObj)
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

int CTcpAgent::DoSendSafe(TSocketObj* pSocketObj)
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

int CTcpAgent::SendItem(TSocketObj* pSocketObj)
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

BOOL CTcpAgent::SendSmallFile(CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail)
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

void CTcpAgent::CheckError(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
{
	if(iErrorCode != WSAENOTSOCK && iErrorCode != ERROR_OPERATION_ABORTED)
		AddFreeSocketObj(pSocketObj, SCF_ERROR, enOperation, iErrorCode);
}
