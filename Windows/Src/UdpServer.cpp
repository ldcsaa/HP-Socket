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
#include "UdpServer.h"
#include "../Common/Src/WaitFor.h"

#include <malloc.h>
#include <process.h>

const CInitSocket CUdpServer::sm_wsSocket;

EnHandleResult CUdpServer::TriggerFireAccept(TUdpSocketObj* pSocketObj)
{
	EnHandleResult rs = TRIGGER(FireAccept(pSocketObj));
	pSocketObj->csRecv.WriteDone();

	return rs;
}

EnHandleResult CUdpServer::TriggerFireReceive(TUdpSocketObj* pSocketObj, TUdpBufferObj* pBufferObj)
{
	EnHandleResult rs = (EnHandleResult)HR_CLOSED;

	if(TUdpSocketObj::IsValid(pSocketObj))
	{
		CReentrantReadLock locallock(pSocketObj->csRecv);

		if(TUdpSocketObj::IsValid(pSocketObj))
		{
			rs = TRIGGER(FireReceive(pSocketObj, (BYTE*)pBufferObj->buff.buf, pBufferObj->buff.len));
		}
	}

	return rs;
}

EnHandleResult CUdpServer::TriggerFireSend(TUdpSocketObj* pSocketObj, TUdpBufferObj* pBufferObj)
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

EnHandleResult CUdpServer::TriggerFireClose(TUdpSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
{
	CReentrantWriteLock locallock(pSocketObj->csRecv);
	return FireClose(pSocketObj, enOperation, iErrorCode);
}

void CUdpServer::SetLastError(EnSocketError code, LPCSTR func, int ec)
{
	m_enLastError = code;
	::SetLastError(ec);

	TRACE("%s --> Error: %d, EC: %d\n", func, code, ec);
}

BOOL CUdpServer::Start(LPCTSTR lpszBindAddress, USHORT usPort)
{
	if(!CheckParams() || !CheckStarting())
		return FALSE;

	PrepareStart();

	if(CreateListenSocket(lpszBindAddress, usPort))
		if(CreateCompletePort())
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

	if(!lpszBindAddress || lpszBindAddress[0] == 0)
		lpszBindAddress = DEFAULT_IPV4_BIND_ADDRESS;

	HP_SOCKADDR addr;

	if(::sockaddr_A_2_IN(lpszBindAddress, usPort, addr))
	{
		m_usFamily = addr.family;
		m_soListen = socket(m_usFamily, SOCK_DGRAM, IPPROTO_UDP);

		if(m_soListen != INVALID_SOCKET)
		{
			ENSURE(::SSO_UDP_ConnReset(m_soListen, FALSE) == NO_ERROR);

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

BOOL CUdpServer::CreateCompletePort()
{
	m_hCompletePort	= ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
	if(m_hCompletePort == nullptr)
		SetLastError(SE_CP_CREATE, __FUNCTION__, ::GetLastError());

	return (m_hCompletePort != nullptr);
}

BOOL CUdpServer::CreateWorkerThreads()
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

BOOL CUdpServer::CreateDetectorThread()
{
	BOOL isOK = TRUE;

	if(NeedDetectorThread())
	{
		m_hDetector = (HANDLE)_beginthreadex(nullptr, 0, DetecotrThreadProc, (LPVOID)this, 0, nullptr);
		isOK = m_hDetector != nullptr;
	}

	return isOK;
}

BOOL CUdpServer::StartAccept()
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

BOOL CUdpServer::Stop()
{
	if(!CheckStoping())
		return FALSE;

	CloseListenSocket();

	WaitForDetectorThreadEnd();
	WaitForPostReceiveRelease();

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

void CUdpServer::Reset()
{
	m_phSocket.Reset();

	m_iRemainPostReceives	= 0;
	m_enState				= SS_STOPPED;
	m_usFamily				= AF_UNSPEC;
}

void CUdpServer::CloseListenSocket()
{
	if(m_soListen != INVALID_SOCKET)
	{
		::ManualCloseSocket(m_soListen);
		m_soListen = INVALID_SOCKET;

		::WaitWithMessageLoop(100);
	}
}

void CUdpServer::DisconnectClientSocket()
{
	DWORD size					= 0;
	unique_ptr<CONNID[]> ids	= m_bfActiveSockets.GetAllElementIndexes(size);

	for(DWORD i = 0; i < size; i++)
		Disconnect(ids[i]);
}

void CUdpServer::ReleaseClientSocket()
{
	ENSURE(m_bfActiveSockets.IsEmpty());
	m_bfActiveSockets.Reset();

	CWriteLock locallock(m_csClientSocket);
	m_mpClientAddr.clear();
}

TUdpSocketObj* CUdpServer::GetFreeSocketObj(CONNID dwConnID)
{
	DWORD dwIndex;
	TUdpSocketObj* pSocketObj = nullptr;

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
	pSocketObj->Reset(dwConnID);

	return pSocketObj;
}

void CUdpServer::AddFreeSocketObj(CONNID dwConnID, EnSocketCloseFlag enFlag, EnSocketOperation enOperation, int iErrorCode)
{
	AddFreeSocketObj(FindSocketObj(dwConnID), enFlag, enOperation, iErrorCode);
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

	ENSURE(m_bfActiveSockets.ReleaseLock(dwConnID, pSocketObj));

	CWriteLock locallock(m_csClientSocket);
	m_mpClientAddr[&pSocketObj->remoteAddr]	= dwConnID;
}

void CUdpServer::ReleaseFreeSocket()
{
	TUdpSocketObj* pSocketObj = nullptr;

	while(m_lsFreeSocket.TryGet(&pSocketObj))
		DeleteSocketObj(pSocketObj);

	ENSURE(m_lsFreeSocket.IsEmpty());
	m_lsFreeSocket.Reset();

	ReleaseGCSocketObj(TRUE);
	ENSURE(m_lsGCSocket.IsEmpty());
}

TUdpSocketObj* CUdpServer::CreateSocketObj()
{
	return TUdpSocketObj::Construct(m_phSocket, m_bfObjPool);
}

void CUdpServer::DeleteSocketObj(TUdpSocketObj* pSocketObj)
{
	TUdpSocketObj::Destruct(pSocketObj);
}

TUdpBufferObj* CUdpServer::GetFreeBufferObj(int iLen)
{
	ASSERT(iLen >= -1 && iLen <= (int)m_dwMaxDatagramSize);

	TUdpBufferObj* pBufferObj		= m_bfObjPool.PickFreeItem();;
	if(iLen < 0) iLen				= m_dwMaxDatagramSize;
	pBufferObj->buff.len			= iLen;
	pBufferObj->remoteAddr.family	= m_usFamily;
	pBufferObj->addrLen				= pBufferObj->remoteAddr.AddrSize();

	return pBufferObj;
}

void CUdpServer::AddFreeBufferObj(TUdpBufferObj* pBufferObj)
{
	m_bfObjPool.PutFreeItem(pBufferObj);
}

void CUdpServer::ReleaseFreeBuffer()
{
	m_bfObjPool.Clear();
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
		TriggerFireClose(pSocketObj, SO_CLOSE, SE_OK);
	else if(enFlag == SCF_ERROR)
		TriggerFireClose(pSocketObj, enOperation, iErrorCode);
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
	BOOL isOK					= FALSE;
	TUdpSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TUdpSocketObj::IsValid(pSocketObj))
		isOK = ::PostIocpDisconnect(m_hCompletePort, dwConnID);

	return isOK;
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

void CUdpServer::WaitForPostReceiveRelease()
{
	while(m_iRemainPostReceives > 0)
		::WaitWithMessageLoop(50);
}

void CUdpServer::WaitForClientSocketClose()
{
	while(m_bfActiveSockets.Elements() > 0)
		::WaitWithMessageLoop(50);
}

void CUdpServer::WaitForWorkerThreadEnd()
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

void CUdpServer::WaitForDetectorThreadEnd()
{
	if(m_hDetector != nullptr)
	{
		m_evDetector.Set();
		ENSURE(::WaitForSingleObject(m_hDetector, INFINITE) == WAIT_OBJECT_0);
		m_evDetector.Reset();

		::CloseHandle(m_hDetector);
		m_hDetector = nullptr;
	}
}

void CUdpServer::CloseCompletePort()
{
	if(m_hCompletePort != nullptr)
	{
		::CloseHandle(m_hCompletePort);
		m_hCompletePort = nullptr;
	}
}

UINT WINAPI CUdpServer::WorkerThreadProc(LPVOID pv)
{
	CUdpServer* pServer	= (CUdpServer*)pv;

	while(TRUE)
	{
		DWORD dwErrorCode = NO_ERROR;
		DWORD dwBytes;
		ULONG_PTR  ulCompKey;
		OVERLAPPED* pOverlapped;
		
		BOOL result = ::GetQueuedCompletionStatus
												(
													pServer->m_hCompletePort,
													&dwBytes,
													&ulCompKey,
													&pOverlapped,
													INFINITE
												);

		if(pOverlapped == nullptr)
		{
			EnIocpAction action = pServer->CheckIocpCommand(pOverlapped, dwBytes, ulCompKey);

			if(action == IOCP_ACT_CONTINUE)
				continue;
			else if(action == IOCP_ACT_BREAK)
				break;
		}

		TUdpBufferObj* pBufferObj	= CONTAINING_RECORD(pOverlapped, TUdpBufferObj, ov);
		CONNID dwConnID				= pServer->FindConnectionID(&pBufferObj->remoteAddr);

		if (!result)
		{
			DWORD dwFlag	= 0;
			DWORD dwSysCode = ::GetLastError();

			if(pServer->HasStarted())
			{
				result = ::WSAGetOverlappedResult((SOCKET)ulCompKey, &pBufferObj->ov, &dwBytes, FALSE, &dwFlag);

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

		pServer->HandleIo(dwConnID, pBufferObj, dwBytes, dwErrorCode);
	}

	pServer->OnWorkerThreadEnd(::GetCurrentThreadId());

	return 0;
}

EnIocpAction CUdpServer::CheckIocpCommand(OVERLAPPED* pOverlapped, DWORD dwBytes, ULONG_PTR ulCompKey)
{
	ASSERT(pOverlapped == nullptr);

	EnIocpAction action = IOCP_ACT_CONTINUE;

	if(dwBytes == IOCP_CMD_SEND)
		DoSend((CONNID)ulCompKey);
	else if(dwBytes == IOCP_CMD_ACCEPT)
		DoReceive(GetFreeBufferObj());
	else if(dwBytes == IOCP_CMD_DISCONNECT)
		ForceDisconnect((CONNID)ulCompKey);
	else if(dwBytes == IOCP_CMD_EXIT && ulCompKey == 0)
		action = IOCP_ACT_BREAK;
	else
		ENSURE(FALSE);

	return action;
}

void CUdpServer::ForceDisconnect(CONNID dwConnID)
{
	AddFreeSocketObj(dwConnID, SCF_CLOSE);
}

void CUdpServer::HandleIo(CONNID dwConnID, TUdpBufferObj* pBufferObj, DWORD dwBytes, DWORD dwErrorCode)
{
	ASSERT(pBufferObj != nullptr);

	if(dwErrorCode != NO_ERROR)
	{
		HandleError(dwConnID, pBufferObj, dwErrorCode);
		return;
	}

	if(dwBytes == 0)
	{
		HandleZeroBytes(dwConnID, pBufferObj);
		return;
	}

	pBufferObj->buff.len = dwBytes;

	switch(pBufferObj->operation)
	{
	case SO_SEND:
		HandleSend(dwConnID, pBufferObj);
		break;
	case SO_RECEIVE:
		HandleReceive(dwConnID, pBufferObj);
		break;
	default:
		ASSERT(FALSE);
	}
}

void CUdpServer::HandleError(CONNID dwConnID, TUdpBufferObj* pBufferObj, DWORD dwErrorCode)
{
	if(dwConnID != 0)
		AddFreeSocketObj(dwConnID, SCF_ERROR, pBufferObj->operation, dwErrorCode);
	
	if(pBufferObj->operation == SO_RECEIVE)
		DoReceive(pBufferObj);
	else
		AddFreeBufferObj(pBufferObj);
}

void CUdpServer::HandleZeroBytes(CONNID dwConnID, TUdpBufferObj* pBufferObj)
{
	if(pBufferObj->operation == SO_RECEIVE)
	{
		if(dwConnID == 0)
			dwConnID = HandleAccept(pBufferObj);
		
		if(dwConnID != 0)
		{
			TRACE("<S-CNNID: %Iu> recv 0 bytes (detect package)\n", dwConnID);

			TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

			if(TUdpSocketObj::IsValid(pSocketObj))
			{
				pSocketObj->detectFails = 0;
				SendDetectPackage(dwConnID, pSocketObj);
			}			
		}

		DoReceive(pBufferObj);
	}
	else
		ENSURE(FALSE);
}

CONNID CUdpServer::HandleAccept(TUdpBufferObj* pBufferObj)
{
	CONNID dwConnID				= 0;
	TUdpSocketObj* pSocketObj	= nullptr;

	{
		CCriSecLock locallock(m_csAccept);

		dwConnID = FindConnectionID(&pBufferObj->remoteAddr);

		if(dwConnID != 0)
			return dwConnID;
		else
		{
			if(!HasStarted() || !m_bfActiveSockets.AcquireLock(dwConnID))
				return 0;

			pSocketObj = GetFreeSocketObj(dwConnID);
			pSocketObj->csRecv.WaitToWrite();
		}
	}

	AddClientSocketObj(dwConnID, pSocketObj, pBufferObj->remoteAddr);

	if(TriggerFireAccept(pSocketObj) == HR_ERROR)
	{
		AddFreeSocketObj(pSocketObj);
		return 0;
	}

	return dwConnID;
}

void CUdpServer::HandleSend(CONNID dwConnID, TUdpBufferObj* pBufferObj)
{
	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(!TUdpSocketObj::IsValid(pSocketObj))
	{
		AddFreeBufferObj(pBufferObj);
		return;
	}

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

void CUdpServer::HandleReceive(CONNID dwConnID, TUdpBufferObj* pBufferObj)
{
	if(dwConnID == 0)
		dwConnID = HandleAccept(pBufferObj);

	if(dwConnID != 0)
	{
		TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

		if(TUdpSocketObj::IsValid(pSocketObj))
		{
			if(m_bMarkSilence) pSocketObj->activeTime = ::TimeGetTime();
			if(TriggerFireReceive(pSocketObj, pBufferObj) == HR_ERROR)
			{
				TRACE("<S-CNNID: %Iu> OnReceive() event return 'HR_ERROR', connection will be closed !\n", dwConnID);
				AddFreeSocketObj(pSocketObj, SCF_ERROR, SO_RECEIVE, ENSURE_ERROR_CANCELLED);
			}
		}
	}

	DoReceive(pBufferObj);
}

int CUdpServer::DoReceive(TUdpBufferObj* pBufferObj)
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

BOOL CUdpServer::PauseReceive(CONNID dwConnID, BOOL bPause)
{
	::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

BOOL CUdpServer::Send(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset)
{
	int result = NO_ERROR;

	ASSERT(pBuffer && iLength > 0 && iLength <= (int)m_dwMaxDatagramSize);

	if(pBuffer && iLength > 0 && iLength <= (int)m_dwMaxDatagramSize)
	{
		if(iOffset != 0) pBuffer += iOffset;
		result = SendInternal(dwConnID, pBuffer, iLength);
	}
	else
		result = ERROR_INVALID_PARAMETER;

	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);
}

BOOL CUdpServer::SendPackets(CONNID dwConnID, const WSABUF pBuffers[], int iCount)
{
	int result = NO_ERROR;

	ASSERT(pBuffers && iCount > 0);

	if(pBuffers && iCount > 0)
	{
		int iLength = 0;
		int iMaxLen = (int)m_dwMaxDatagramSize;

		TUdpBufferObj* pBufferObj = m_bfObjPool.PickFreeItem();

		for(int i = 0; i < iCount; i++)
		{
			int iBufLen = pBuffers[i].len;

			if(iBufLen > 0)
			{
				BYTE* pBuffer = (BYTE*)pBuffers[i].buf;
				ASSERT(pBuffer);

				iLength += iBufLen;

				if(iLength <= iMaxLen)
					pBufferObj->Cat(pBuffer, iBufLen);
				else
					break;
			}
		}

		if(iLength > 0 && iLength <= iMaxLen)
			result = SendInternal(dwConnID, (const BYTE*)pBufferObj->buff.buf, pBufferObj->buff.len);
		else
			result = ERROR_INCORRECT_SIZE;

		m_bfObjPool.PutFreeItem(pBufferObj);
	}
	else
		result = ERROR_INVALID_PARAMETER;

	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);
}

int CUdpServer::SendInternal(CONNID dwConnID, const BYTE* pBuffer, int iLength)
{
	int result					= NO_ERROR;
	TUdpSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TUdpSocketObj::IsValid(pSocketObj))
	{
		CCriSecLock locallock(pSocketObj->csSend);

		if(TUdpSocketObj::IsValid(pSocketObj))
		{
			switch(m_enSendPolicy)
			{
			case SP_PACK:	result = SendPack(pSocketObj, pBuffer, iLength);	break;
			case SP_SAFE:	result = SendSafe(pSocketObj, pBuffer, iLength);	break;
			case SP_DIRECT:	result = SendDirect(pSocketObj, pBuffer, iLength);	break;
			default: ASSERT(FALSE);	result = ERROR_INVALID_INDEX;				break;
			}
		}
		else
			result = ERROR_OBJECT_NOT_FOUND;
	}
	else
		result = ERROR_OBJECT_NOT_FOUND;

	return result;
}

int CUdpServer::SendPack(TUdpSocketObj* pSocketObj, const BYTE* pBuffer, int iLength)
{
	return CatAndPost(pSocketObj, pBuffer, iLength);
}

int CUdpServer::SendSafe(TUdpSocketObj* pSocketObj, const BYTE* pBuffer, int iLength)
{
	return CatAndPost(pSocketObj, pBuffer, iLength);
}

int CUdpServer::SendDirect(TUdpSocketObj* pSocketObj, const BYTE* pBuffer, int iLength)
{
	TUdpBufferObj* pBufferObj = GetFreeBufferObj(iLength);

	pSocketObj->remoteAddr.Copy(pBufferObj->remoteAddr);
	memcpy(pBufferObj->buff.buf, pBuffer, iLength);

	int result		= ::PostSendTo(m_soListen, pBufferObj);
	LONG sndCounter	= pBufferObj->ReleaseSendCounter();

	if(sndCounter == 0 || result != NO_ERROR)
	{
		AddFreeBufferObj(pBufferObj);
		
		if(result != NO_ERROR)
			ENSURE(!HasStarted());
	}

	return result;
}

int CUdpServer::CatAndPost(TUdpSocketObj* pSocketObj, const BYTE* pBuffer, int iLength)
{
	int result = NO_ERROR;

	pSocketObj->sndBuff.PushTail(pBuffer, iLength);
	pSocketObj->pending += iLength;

	if(pSocketObj->IsCanSend() && pSocketObj->IsSmooth() && !::PostIocpSend(m_hCompletePort, pSocketObj->connID))
		result = ::GetLastError();

	return result;
}

int CUdpServer::DoSend(CONNID dwConnID)
{
	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TUdpSocketObj::IsValid(pSocketObj))
		return DoSend(pSocketObj);

	return ERROR_OBJECT_NOT_FOUND;
}

int CUdpServer::DoSend(TUdpSocketObj* pSocketObj)
{
	switch(m_enSendPolicy)
	{
	case SP_PACK:			return DoSendPack(pSocketObj);
	case SP_SAFE:			return DoSendSafe(pSocketObj);
	default: ASSERT(FALSE);	return ERROR_INVALID_INDEX;
	}
}

int CUdpServer::DoSendPack(TUdpSocketObj* pSocketObj)
{
	if(!pSocketObj->IsCanSend())
		return NO_ERROR;

	int result = NO_ERROR;

	if(pSocketObj->IsPending() && pSocketObj->TurnOffSmooth())
	{
		{
			CCriSecLock locallock(pSocketObj->csSend);

			if(!TUdpSocketObj::IsValid(pSocketObj))
				return ERROR_OBJECT_NOT_FOUND;

			if(pSocketObj->IsPending())
				result = SendItem(pSocketObj);

			pSocketObj->TurnOnSmooth();
		}

		if(result == WSA_IO_PENDING && pSocketObj->IsSmooth())
			::PostIocpSend(m_hCompletePort, pSocketObj->connID);
	}

	if(!IOCP_SUCCESS(result))
		ENSURE(!HasStarted());

	return result;
}

int CUdpServer::DoSendSafe(TUdpSocketObj* pSocketObj)
{
	if(pSocketObj->sndCount == 0 && !pSocketObj->IsSmooth())
	{
		CCriSecLock locallock(pSocketObj->csSend);

		if(!TUdpSocketObj::IsValid(pSocketObj))
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

		if(!TUdpSocketObj::IsValid(pSocketObj))
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
		ENSURE(!HasStarted());

	return result;
}

int CUdpServer::SendItem(TUdpSocketObj* pSocketObj)
{
	int result = NO_ERROR;

	while(pSocketObj->sndBuff.Size() > 0)
	{
		TUdpBufferObj* pBufferObj	= pSocketObj->sndBuff.PopFront();
		int iBufferSize				= pBufferObj->buff.len;

		ASSERT(iBufferSize > 0 && iBufferSize <= (int)m_dwMaxDatagramSize);

		pSocketObj->pending		   -= iBufferSize;
		::InterlockedExchangeAdd(&pSocketObj->sndCount, iBufferSize);

		memcpy(&pBufferObj->remoteAddr, &pSocketObj->remoteAddr, sizeof(SOCKADDR_IN));

		result			= ::PostSendToNotCheck(m_soListen, pBufferObj);
		LONG sndCounter	= pBufferObj->ReleaseSendCounter();

		if(sndCounter == 0 || !IOCP_SUCCESS(result))
			AddFreeBufferObj(pBufferObj);

		if(result != NO_ERROR)
			break;
	}

	return result;
}

BOOL CUdpServer::SendDetectPackage(CONNID dwConnID, TUdpSocketObj* pSocketObj)
{
	BOOL isOK = TRUE;

	if(!HasStarted())
		isOK = FALSE;
	else
	{
		int rc = sendto(m_soListen, nullptr, 0, 0, pSocketObj->remoteAddr.Addr(), pSocketObj->remoteAddr.AddrSize());

		if(rc == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
			isOK = FALSE;

		if(isOK)
			TRACE("<S-CNNID: %Iu> send 0 bytes (detect package)\n", dwConnID);
		else
			ENSURE(!HasStarted());
	}

	return isOK;
}

UINT WINAPI CUdpServer::DetecotrThreadProc(LPVOID pv)
{
	TRACE("---------------> Server Detecotr Thread 0x%08X started <---------------\n", ::GetCurrentThreadId());

	CUdpServer* pServer	= (CUdpServer*)pv;

	ASSERT(pServer->NeedDetectorThread());

	while(pServer->HasStarted())
	{
		DWORD retval = ::WaitForSingleObject(pServer->m_evDetector, pServer->m_dwDetectInterval * 1000L);

		if(retval == WAIT_TIMEOUT)
			pServer->DetectConnections();
		else if(retval == WAIT_OBJECT_0)
			break;
		else
			ASSERT(FALSE);
	}

	TRACE("---------------> Server Detecotr Thread 0x%08X stoped <---------------\n", ::GetCurrentThreadId());

	return 0;
}

void CUdpServer::DetectConnections()
{
	DWORD size					= 0;
	unique_ptr<CONNID[]> ids	= m_bfActiveSockets.GetAllElementIndexes(size);

	for(DWORD i = 0; i < size; i++)
	{
		CONNID connID				= ids[i];
		TUdpSocketObj* pSocketObj	= FindSocketObj(connID);

		if(pSocketObj)
		{
			if(pSocketObj->detectFails >= m_dwDetectAttempts)
				::PostIocpDisconnect(m_hCompletePort, connID);
			else
				::InterlockedIncrement(&pSocketObj->detectFails);
		}
	}
}
