/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.2.3
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 75375912
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
#include "../../Common/Src/WaitFor.h"

#include <malloc.h>
#include <process.h>

EnHandleResult CUdpServer::FireReceive(TUdpSocketObj* pSocketObj, const BYTE* pData, int iLength)
{
	if(m_enRecvPolicy == RP_SERIAL)
	{
		if(TUdpSocketObj::IsValid(pSocketObj))
		{
			CCriSecLock locallock(pSocketObj->csRecv);

			if(TUdpSocketObj::IsValid(pSocketObj))
			{
				return m_psoListener->OnReceive(pSocketObj->connID, pData, iLength);
			}
		}

		return (EnHandleResult)HR_CLOSED;
	}

	return m_psoListener->OnReceive(pSocketObj->connID, pData, iLength);
}

EnHandleResult CUdpServer::FireClose(TUdpSocketObj* pSocketObj)
{
	if(m_enRecvPolicy == RP_SERIAL)
	{
		CCriSecLock locallock(pSocketObj->csRecv);
		return m_psoListener->OnClose(pSocketObj->connID);
	}

	return m_psoListener->OnClose(pSocketObj->connID);
}

EnHandleResult CUdpServer::FireError(TUdpSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
{
	if(m_enRecvPolicy == RP_SERIAL)
	{
		CCriSecLock locallock(pSocketObj->csRecv);
		return m_psoListener->OnError(pSocketObj->connID, enOperation, iErrorCode);
	}

	return m_psoListener->OnError(pSocketObj->connID, enOperation, iErrorCode);
}

void CUdpServer::SetLastError(EnSocketError code, LPCSTR func, int ec)
{
	m_enLastError = code;

	TRACE("%s --> Error: %d, EC: %d\n", func, code, ec);
}

BOOL CUdpServer::Start(LPCTSTR pszBindAddress, USHORT usPort)
{
	if(!CheckParams() || !CheckStarting())
		return FALSE;

	if(CreateListenSocket(pszBindAddress, usPort))
		if(CreateCompletePort())
			if(CreateWorkerThreads())
				if(CreateDetectorThread())
					if(StartAccept())
					{
						m_enState = SS_STARTED;
						return TRUE;
					}

	Stop();

	return FALSE;
}

BOOL CUdpServer::CheckParams()
{
	m_itPool.SetItemCapacity((int)m_dwMaxDatagramSize);
	m_itPool.SetPoolSize((int)m_dwFreeBufferObjPool);
	m_itPool.SetPoolHold((int)m_dwFreeBufferObjHold);

	if(m_enSendPolicy >= SP_PACK && m_enSendPolicy <= SP_DIRECT)
		if(m_enRecvPolicy >= RP_SERIAL && m_enRecvPolicy <= RP_PARALLEL)
			if((int)m_dwWorkerThreadCount > 0 && m_dwWorkerThreadCount <= MAX_WORKER_THREAD_COUNT)
				if((int)m_dwFreeSocketObjLockTime >= 0 && m_dwFreeSocketObjLockTime <= MAXLONG)
					if((int)m_dwFreeSocketObjPool >= 0)
						if((int)m_dwFreeBufferObjPool >= 0)
							if((int)m_dwFreeSocketObjHold >= m_dwFreeSocketObjPool)
								if((int)m_dwFreeBufferObjHold >= m_dwFreeBufferObjPool)
									if((int)m_dwMaxDatagramSize > 0 && m_dwMaxDatagramSize <= MAXWORD)
										if((int)m_dwPostReceiveCount > 0)
											if((int)m_dwDetectAttempts >= 0 && m_dwDetectAttempts <= MAXLONG)
												if((int)m_dwDetectInterval >= 0&& m_dwDetectInterval <= MAXLONG)
													if((int)m_dwMaxShutdownWaitTime >= 0)
														return TRUE;

	SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
	return FALSE;
}

BOOL CUdpServer::CheckStarting()
{
	if(m_enState == SS_STOPED)
	{
		m_enState = SS_STARTING;
		::_ReadWriteBarrier();
	}
	else
	{
		SetLastError(SE_ILLEGAL_STATE, __FUNCTION__, ERROR_INVALID_OPERATION);
		return FALSE;
	}

	return TRUE;
}

BOOL CUdpServer::CheckStoping()
{
	if(m_enState == SS_STARTED || m_enState == SS_STARTING)
	{
		m_enState = SS_STOPING;
		::_ReadWriteBarrier();
	}
	else
	{
		SetLastError(SE_ILLEGAL_STATE, __FUNCTION__, ERROR_INVALID_OPERATION);
		return FALSE;
	}

	return TRUE;
}

BOOL CUdpServer::CreateListenSocket(LPCTSTR pszBindAddress, USHORT usPort)
{
	BOOL isOK = FALSE;

	m_soListen	= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(m_soListen != INVALID_SOCKET)
	{
		VERIFY(::SSO_UDP_ConnReset(m_soListen, FALSE) == NO_ERROR);

		SOCKADDR_IN addr;
		::sockaddr_A_2_IN(AF_INET, pszBindAddress, usPort, addr);

		if(::bind(m_soListen, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)) != SOCKET_ERROR)
		{
			if(FirePrepareListen(m_soListen) != HR_ERROR)
				isOK = TRUE;
			else
				SetLastError(SE_SOCKET_PREPARE, __FUNCTION__, ERROR_CANCELLED);
		}
		else
			SetLastError(SE_SOCKET_BIND, __FUNCTION__, ::WSAGetLastError());
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
			VERIFY(::PostIocpAccept(m_hCompletePort));
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

	FireServerShutdown();

	ReleaseFreeSocket();
	ReleaseFreeBuffer();

	CloseCompletePort();

	Reset();

	return TRUE;
}

void CUdpServer::Reset()
{
	m_phSocket.Reset();
	m_phBuffer.Reset();
	m_itPool.Clear();

	m_iRemainPostReceives	= 0;
	m_enState				= SS_STOPED;
}

void CUdpServer::CloseListenSocket()
{
	if(m_soListen != INVALID_SOCKET)
	{
		::ManualCloseSocket(m_soListen);
		m_soListen = INVALID_SOCKET;

		::WaitWithMessageLoop(150);
	}
}

void CUdpServer::DisconnectClientSocket()
{
	CReentrantReadLock locallock(m_csClientSocket);

	for(TUdpSocketObjPtrMapI it = m_mpClientSocket.begin(); it != m_mpClientSocket.end(); ++it)
		Disconnect(it->first);
}

void CUdpServer::ReleaseClientSocket()
{
	CReentrantWriteLock locallock(m_csClientSocket);

	for(TUdpSocketObjPtrMapI it = m_mpClientSocket.begin(); it != m_mpClientSocket.end(); ++it)
	{
		TUdpSocketObj* pSocketObj = it->second;
		DeleteSocketObj(pSocketObj);
	}

	m_mpClientAddr.clear();
	m_mpClientSocket.clear();
}

TUdpSocketObj*	CUdpServer::GetFreeSocketObj(CONNID dwConnID)
{
	TUdpSocketObj* pSocketObj = nullptr;

	if(m_lsFreeSocket.size() > 0)
	{
		CCriSecLock locallock(m_csFreeSocket);

		if(m_lsFreeSocket.size() > 0)
		{
			pSocketObj = m_lsFreeSocket.front();

			if(::GetTimeGap32(pSocketObj->freeTime) >= m_dwFreeSocketObjLockTime)
				m_lsFreeSocket.pop_front();
			else
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
	if(InvalidSocketObj(pSocketObj))
	{
		CloseClientSocketObj(pSocketObj, enFlag, enOperation, iErrorCode);
		TUdpSocketObj::Release(pSocketObj);

		{
			CReentrantWriteLock locallock(m_csClientSocket);

			m_mpClientAddr.erase(&pSocketObj->remoteAddr);
			m_mpClientSocket.erase(pSocketObj->connID);
		}

		{
			CCriSecLock locallock(m_csFreeSocket);
			m_lsFreeSocket.push_back(pSocketObj);
		}

		if(m_lsFreeSocket.size() > m_dwFreeSocketObjHold)
			CompressFreeSocket(m_dwFreeSocketObjPool);
	}
}

BOOL CUdpServer::InvalidSocketObj(TUdpSocketObj* pSocketObj)
{
	BOOL bDone = FALSE;

	if(m_enRecvPolicy == RP_SERIAL)
	{
		if(TUdpSocketObj::IsValid(pSocketObj))
		{
			CCriSecLock locallock(pSocketObj->csRecv);
			CCriSecLock locallock2(pSocketObj->csSend);

			if(TUdpSocketObj::IsValid(pSocketObj))
			{
				TUdpSocketObj::Invalid(pSocketObj);
				bDone = TRUE;
			}
		}
	}
	else
	{
		if(TUdpSocketObj::IsValid(pSocketObj))
		{
			CCriSecLock locallock(pSocketObj->csSend);

			if(TUdpSocketObj::IsValid(pSocketObj))
			{
				TUdpSocketObj::Invalid(pSocketObj);
				bDone = TRUE;
			}
		}
	}

	return bDone;
}

void CUdpServer::AddClientSocketObj(CONNID dwConnID, TUdpSocketObj* pSocketObj)
{
	ASSERT(FindSocketObj(dwConnID) == nullptr);

	pSocketObj->connTime = ::TimeGetTime();

	CReentrantWriteLock locallock(m_csClientSocket);

	m_mpClientSocket[dwConnID]				= pSocketObj;
	m_mpClientAddr[&pSocketObj->remoteAddr]	= dwConnID;
}

void CUdpServer::ReleaseFreeSocket()
{
	CompressFreeSocket(0, TRUE);
}

void CUdpServer::CompressFreeSocket(size_t size, BOOL bForce)
{
	CCriSecLock locallock(m_csFreeSocket);

	DWORD now = ::TimeGetTime();

	while(m_lsFreeSocket.size() > size)
	{
		TUdpSocketObj* pSocketObj = m_lsFreeSocket.front();

		if(bForce || (now - pSocketObj->freeTime) >= m_dwFreeSocketObjLockTime)
		{
			m_lsFreeSocket.pop_front();
			DeleteSocketObj(pSocketObj);
		}
		else
			break;
	}
}

TUdpSocketObj* CUdpServer::CreateSocketObj()
{
	TUdpSocketObj* pSocketObj = (TUdpSocketObj*)m_phSocket.Alloc(sizeof(TUdpSocketObj), HEAP_ZERO_MEMORY);
	ASSERT(pSocketObj);

	pSocketObj->TUdpSocketObj::TUdpSocketObj(m_itPool);
	
	return pSocketObj;
}

void CUdpServer::DeleteSocketObj(TUdpSocketObj* pSocketObj)
{
	ASSERT(pSocketObj);

	pSocketObj->TUdpSocketObj::~TUdpSocketObj();;
	m_phSocket.Free(pSocketObj);
}

TUdpBufferObj* CUdpServer::GetFreeBufferObj(int iLen)
{
	ASSERT(iLen >= -1 && iLen <= (int)m_dwMaxDatagramSize);

	TUdpBufferObj* pBufferObj = nullptr;

	if(m_lsFreeBuffer.size() > 0)
	{
		CCriSecLock locallock(m_csFreeBuffer);

		if(m_lsFreeBuffer.size() > 0)
		{
			pBufferObj = m_lsFreeBuffer.front();
			m_lsFreeBuffer.pop_front();
		}
	}

	if(!pBufferObj) pBufferObj = CreateBufferObj();

	if(iLen < 0) iLen	 = m_dwMaxDatagramSize;
	pBufferObj->addrLen	 = sizeof(SOCKADDR_IN);
	pBufferObj->buff.len = iLen;

	return pBufferObj;
}

void CUdpServer::AddFreeBufferObj(TUdpBufferObj* pBufferObj)
{
	if(m_lsFreeBuffer.size() < m_dwFreeBufferObjHold)
	{
		CCriSecLock locallock(m_csFreeBuffer);
		m_lsFreeBuffer.push_back(pBufferObj);
	}
	else
	{
		DeleteBufferObj(pBufferObj);
		CompressFreeBuffer(m_dwFreeBufferObjPool);
	}
}

void CUdpServer::ReleaseFreeBuffer()
{
	CompressFreeBuffer(0);
}

void CUdpServer::CompressFreeBuffer(size_t size)
{
	CCriSecLock locallock(m_csFreeBuffer);

	while(m_lsFreeBuffer.size() > size)
	{
		TUdpBufferObj* pBufferObj = m_lsFreeBuffer.front();

		m_lsFreeBuffer.pop_front();
		DeleteBufferObj(pBufferObj);
	}
}

TUdpBufferObj* CUdpServer::CreateBufferObj()
{
	TUdpBufferObj* pBufferObj	= (TUdpBufferObj*)m_phBuffer.Alloc(sizeof(TUdpBufferObj) + m_dwMaxDatagramSize, HEAP_ZERO_MEMORY);
	pBufferObj->buff.buf		= ((char*)pBufferObj) + sizeof(TUdpBufferObj);

	ASSERT(pBufferObj);
	return pBufferObj;
}

void CUdpServer::DeleteBufferObj(TUdpBufferObj* pBufferObj)
{
	ASSERT(pBufferObj);
	m_phBuffer.Free(pBufferObj);
}

TUdpSocketObj* CUdpServer::FindSocketObj(CONNID dwConnID)
{
	TUdpSocketObj* pSocketObj = nullptr;

	CReentrantReadLock locallock(m_csClientSocket);

	TUdpSocketObjPtrMapCI it = m_mpClientSocket.find(dwConnID);
	if(it != m_mpClientSocket.end())
		pSocketObj = it->second;

	return pSocketObj;
}

CONNID CUdpServer::FindConnectionID(SOCKADDR_IN* pAddr)
{
	CONNID dwConnID = 0;

	CReentrantReadLock locallock(m_csClientSocket);

	TSockAddrMapCI it = m_mpClientAddr.find(pAddr);
	if(it != m_mpClientAddr.end())
		dwConnID = it->second;

	return dwConnID;
}

void CUdpServer::CloseClientSocketObj(TUdpSocketObj* pSocketObj, EnSocketCloseFlag enFlag, EnSocketOperation enOperation, int iErrorCode)
{
	ASSERT(TUdpSocketObj::IsExist(pSocketObj));

	if(enFlag == SCF_CLOSE)
		FireClose(pSocketObj);
	else if(enFlag == SCF_ERROR)
		FireError(pSocketObj, enOperation, iErrorCode);
}

BOOL CUdpServer::GetListenAddress(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	return ::GetSocketLocalAddress(m_soListen, lpszAddress, iAddressLen, usPort);
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

	if(TUdpSocketObj::IsExist(pSocketObj))
	{
		pSocketObj->extra = pExtra;
		return TRUE;
	}

	return FALSE;
}

BOOL CUdpServer::GetConnectionExtra(CONNID dwConnID, PVOID* ppExtra)
{
	ASSERT(ppExtra != nullptr);

	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TUdpSocketObj::IsExist(pSocketObj))
	{
		*ppExtra = pSocketObj->extra;
		return TRUE;
	}

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
	return (DWORD)m_mpClientSocket.size();
}

BOOL CUdpServer::GetAllConnectionIDs(CONNID pIDs[], DWORD& dwCount)
{
	BOOL isOK	 = FALSE;
	DWORD dwSize = 0;

	{
		CReentrantReadLock locallock(m_csClientSocket);

		dwSize = (DWORD)m_mpClientSocket.size();

		if(pIDs != nullptr && dwSize <= dwCount)
		{
			TUdpSocketObjPtrMapCI it = m_mpClientSocket.begin();
			for(int i = 0; it != m_mpClientSocket.end(); ++it, ++i)
				pIDs[i] = it->first;

			isOK = TRUE;
		}
	}

	dwCount = dwSize;
	return isOK;
}

BOOL CUdpServer::GetConnectPeriod(CONNID dwConnID, DWORD& dwPeriod)
{
	BOOL isOK					= TRUE;
	TUdpSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TUdpSocketObj::IsValid(pSocketObj))
		dwPeriod = GetTimeGap32(pSocketObj->connTime);
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
	ulong_ptr_deque ls;

	{
		CReentrantReadLock locallock(m_csClientSocket);

		DWORD now = ::TimeGetTime();

		for(TUdpSocketObjPtrMapCI it = m_mpClientSocket.begin(); it != m_mpClientSocket.end(); ++it)
		{
			if(now - it->second->connTime >= dwPeriod)
				ls.push_back(it->first);
		}
	}
	
	for(ulong_ptr_deque::const_iterator it = ls.begin(); it != ls.end(); ++it)
		Disconnect(*it, bForce);

	return ls.size() > 0;
}

void CUdpServer::WaitForPostReceiveRelease()
{
	DWORD dwWait = 0;
	DWORD dwOrig = ::TimeGetTime();

	while(m_iRemainPostReceives > 0)
	{
		::WaitWithMessageLoop(100);
		dwWait = ::GetTimeGap32(dwOrig);
	}
}

void CUdpServer::WaitForClientSocketClose()
{
	DWORD dwWait = 0;
	DWORD dwOrig = ::TimeGetTime();

	while(m_mpClientSocket.size() > 0)
	{
		::WaitWithMessageLoop(100);
		dwWait = ::GetTimeGap32(dwOrig);
	}

	ASSERT(m_mpClientSocket.size() == 0);
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

		VERIFY(::WaitForMultipleObjects((DWORD)wait, pHandles, TRUE, INFINITE) == WAIT_OBJECT_0);

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
		VERIFY(::WaitForSingleObject(m_hDetector, INFINITE) == WAIT_OBJECT_0);

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

BOOL CUdpServer::DoAccept()
{
	TUdpBufferObj* pBufferObj = GetFreeBufferObj();
	return (DoReceive(pBufferObj) == NO_ERROR);
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

	return 0;
}

EnIocpAction CUdpServer::CheckIocpCommand(OVERLAPPED* pOverlapped, DWORD dwBytes, ULONG_PTR ulCompKey)
{
	ASSERT(pOverlapped == nullptr);

	EnIocpAction action = IOCP_ACT_CONTINUE;

	if(dwBytes == IOCP_CMD_SEND)
		DoSend((CONNID)ulCompKey);
	else if(dwBytes == IOCP_CMD_ACCEPT)
		DoAccept();
	else if(dwBytes == IOCP_CMD_DISCONNECT)
		ForceDisconnect((CONNID)ulCompKey);
	else if(dwBytes == IOCP_CMD_EXIT && ulCompKey == 0)
		action = IOCP_ACT_BREAK;
	else
		VERIFY(FALSE);

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
		VERIFY(FALSE);
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
			dwConnID	= ::GenerateConnectionID();
			pSocketObj	= GetFreeSocketObj(dwConnID);

			memcpy(&pSocketObj->remoteAddr, &pBufferObj->remoteAddr, sizeof(SOCKADDR_IN));
			AddClientSocketObj(dwConnID, pSocketObj);
		}
	}

	if(FireAccept(dwConnID, &pBufferObj->remoteAddr) == HR_ERROR)
	{
		AddFreeSocketObj(pSocketObj);
		dwConnID = 0;
	}

	return dwConnID;
}

void CUdpServer::HandleSend(CONNID dwConnID, TUdpBufferObj* pBufferObj)
{
	BOOL bNeedFree = FALSE;

	switch(m_enSendPolicy)
	{
	case SP_PACK:
		{
			TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

			if(TUdpSocketObj::IsValid(pSocketObj))
			{
				long sndCount = ::InterlockedDecrement(&pSocketObj->sndCount);

				TriggerFireSend(dwConnID, pBufferObj);
				if(sndCount == 0) DoSendPack(pSocketObj);
			}
			else
				bNeedFree = TRUE;
		}

		break;
	case SP_SAFE:
		{
			long sndCount				= -1;
			TUdpSocketObj* pSocketObj	= FindSocketObj(dwConnID);

			if(TUdpSocketObj::IsValid(pSocketObj))
			{
				sndCount = ::InterlockedDecrement(&pSocketObj->sndCount);

				if(sndCount == 0 && !pSocketObj->smooth)
				{
					CCriSecLock locallock(pSocketObj->csSend);

					if((sndCount = pSocketObj->sndCount) == 0)
						pSocketObj->smooth = TRUE;
				}

				TriggerFireSend(dwConnID, pBufferObj);
				if(sndCount == 0) DoSendSafe(pSocketObj);
			}
			else
				bNeedFree = TRUE;
		}

		break;
	case SP_DIRECT:
		{
			TriggerFireSend(dwConnID, pBufferObj);
		}

		break;
	default:
		ASSERT(FALSE);
	}

	if(bNeedFree) AddFreeBufferObj(pBufferObj);
}

void CUdpServer::TriggerFireSend(CONNID dwConnID, TUdpBufferObj* pBufferObj)
{
	if(FireSend(dwConnID, (BYTE*)pBufferObj->buff.buf, pBufferObj->buff.len) == HR_ERROR)
	{
		TRACE("<S-CNNID: %Iu> OnSend() event should not return 'HR_ERROR' !!\n", dwConnID);
		ASSERT(FALSE);
	}

	AddFreeBufferObj(pBufferObj);
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
			if(FireReceive(pSocketObj, (BYTE*)pBufferObj->buff.buf, pBufferObj->buff.len) == HR_ERROR)
			{
				TRACE("<S-CNNID: %Iu> OnReceive() event return 'HR_ERROR', connection will be closed !\n", dwConnID);
				AddFreeSocketObj(pSocketObj, SCF_ERROR, SO_RECEIVE, ERROR_CANCELLED);
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
		VERIFY(!HasStarted());

		::InterlockedDecrement(&m_iRemainPostReceives);
		ASSERT(m_iRemainPostReceives >= 0);

		AddFreeBufferObj(pBufferObj);
	}

	return result;
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
			result = SendInternal(dwConnID, itPtr->Ptr(), iLength);
		else
			result = ERROR_INCORRECT_SIZE;
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
	BOOL isPostSend = !TUdpSocketObj::IsPending(pSocketObj);
	return CatAndPost(pSocketObj, pBuffer, iLength, isPostSend);
}

int CUdpServer::SendSafe(TUdpSocketObj* pSocketObj, const BYTE* pBuffer, int iLength)
{
	BOOL isPostSend = !TUdpSocketObj::IsPending(pSocketObj) && TUdpSocketObj::IsSmooth(pSocketObj);
	return CatAndPost(pSocketObj, pBuffer, iLength, isPostSend);
}

int CUdpServer::SendDirect(TUdpSocketObj* pSocketObj, const BYTE* pBuffer, int iLength)
{
	TUdpBufferObj* pBufferObj = GetFreeBufferObj(iLength);

	memcpy(&pBufferObj->remoteAddr, &pSocketObj->remoteAddr, sizeof(SOCKADDR_IN));
	memcpy(pBufferObj->buff.buf, pBuffer, iLength);

	int result = ::PostSendTo(m_soListen, pBufferObj);

	if(result != NO_ERROR)
	{
		VERIFY(!HasStarted());
		AddFreeBufferObj(pBufferObj);
	}

	return result;
}

int CUdpServer::CatAndPost(TUdpSocketObj* pSocketObj, const BYTE* pBuffer, int iLength, BOOL isPostSend)
{
	int result	 = NO_ERROR;
	TItem* pItem = m_itPool.PickFreeItem();

	pItem->Cat(pBuffer, iLength);
	pSocketObj->sndBuff.PushBack(pItem);
	pSocketObj->pending += iLength;

	if(isPostSend && !::PostIocpSend(m_hCompletePort, pSocketObj->connID))
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
	int result	= NO_ERROR;

	if(TUdpSocketObj::IsPending(pSocketObj))
	{
		CCriSecLock locallock(pSocketObj->csSend);

		if(TUdpSocketObj::IsValid(pSocketObj))
			result = SendItem(pSocketObj);
	}


	if(!IOCP_SUCCESS(result))
		VERIFY(!HasStarted());

	return result;
}

int CUdpServer::DoSendSafe(TUdpSocketObj* pSocketObj)
{
	int result = NO_ERROR;

	if(TUdpSocketObj::IsPending(pSocketObj) && TUdpSocketObj::IsSmooth(pSocketObj))
	{
		CCriSecLock locallock(pSocketObj->csSend);

		if(TUdpSocketObj::IsPending(pSocketObj) && TUdpSocketObj::IsSmooth(pSocketObj))
		{
			pSocketObj->smooth = FALSE;

			result = SendItem(pSocketObj);

			if(result == NO_ERROR)
				pSocketObj->smooth = TRUE;
		}
	}

	if(!IOCP_SUCCESS(result))
		VERIFY(!HasStarted());

	return result;
}

int CUdpServer::SendItem(TUdpSocketObj* pSocketObj)
{
	int result = NO_ERROR;

	while(pSocketObj->sndBuff.Size() > 0)
	{
		::InterlockedIncrement(&pSocketObj->sndCount);

		TItemPtr itPtr(m_itPool, pSocketObj->sndBuff.PopFront());

		int iBufferSize = itPtr->Size();
		ASSERT(iBufferSize > 0 && iBufferSize <= (int)m_dwMaxDatagramSize);

		pSocketObj->pending		 -= iBufferSize;
		TUdpBufferObj* pBufferObj = GetFreeBufferObj(iBufferSize);

		memcpy(&pBufferObj->remoteAddr, &pSocketObj->remoteAddr, sizeof(SOCKADDR_IN));
		memcpy(pBufferObj->buff.buf, itPtr->Ptr(), iBufferSize);

		int result = ::PostSendToNotCheck(m_soListen, pBufferObj);

		if(result != NO_ERROR)
		{
			if(result != WSA_IO_PENDING)
				AddFreeBufferObj(pBufferObj);;

			break;
		}
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
		int rc = sendto(m_soListen, nullptr, 0, 0, (sockaddr*)&pSocketObj->remoteAddr, sizeof(SOCKADDR_IN));

		if(rc == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
			isOK = FALSE;

		if(isOK)
			TRACE("<S-CNNID: %Iu> send 0 bytes (detect package)\n", dwConnID);
		else
			VERIFY(!HasStarted());
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
	CReentrantReadLock locallock(m_csClientSocket);

	for(TUdpSocketObjPtrMapCI it = m_mpClientSocket.begin(); it != m_mpClientSocket.end(); ++it)
	{
		if(it->second->detectFails >= m_dwDetectAttempts)
			::PostIocpDisconnect(m_hCompletePort, it->first);
		else
			::InterlockedIncrement(&it->second->detectFails);
	}
}
