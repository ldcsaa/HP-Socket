/*
 * Copyright Bruce Liang (ldcsaa@gmail.com)
 *
 * Version	: 3.1.1
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * WeiBo	: http://weibo.com/u/1402935851
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
#include "../WaitFor.h"
#include "../FuncHelper.h"

#include <malloc.h>
#include <process.h>

#define IOCP_SI_EXIT		0x00000000
#define IOCP_SI_ACCEPT		0xFFFFFFF1
#define IOCP_SI_DISCONNECT	0xFFFFFFF2

#define IOCP_SI_RS_GONO		0
#define IOCP_SI_RS_CONTINUE	1
#define IOCP_SI_RS_BREAK	2

const DWORD	CUdpServer::MAX_WORKER_THREAD_COUNT			= 500;
const DWORD	CUdpServer::DEFAULT_WORKER_THREAD_COUNT		= min((::GetCpuCount() * 2 + 2), MAX_WORKER_THREAD_COUNT);
const DWORD CUdpServer::DEFAULT_FREE_SOCKETOBJ_LOCK_TIME= 3 * 1000;
const DWORD	CUdpServer::DEFAULT_FREE_SOCKETOBJ_POOL		= 100;
const DWORD	CUdpServer::DEFAULT_FREE_BUFFEROBJ_POOL		= 200;
const DWORD	CUdpServer::DEFAULT_FREE_SOCKETOBJ_HOLD		= 300;
const DWORD	CUdpServer::DEFAULT_FREE_BUFFEROBJ_HOLD		= 600;
const DWORD	CUdpServer::DEFAULT_MAX_DATAGRAM_SIZE		= 1472;
const DWORD CUdpServer::DEFAULT_DETECT_ATTEMPTS			= 3;
const DWORD CUdpServer::DEFAULT_DETECT_INTERVAL			= 10;
const DWORD	CUdpServer::DEFAULT_MAX_SHUTDOWN_WAIT_TIME	= 15 * 1000;

void CUdpServer::SetLastError(EnServerError code, LPCSTR func, int ec)
{
	m_enLastError = code;

	TRACE3("%s --> Error: %d, EC: %d\n", func, code, ec);
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
	if((int)m_dwWorkerThreadCount > 0 && m_dwWorkerThreadCount <= MAX_WORKER_THREAD_COUNT)
		if((int)m_dwFreeSocketObjLockTime >= 0 && m_dwFreeSocketObjLockTime <= MAXLONG)
			if((int)m_dwFreeSocketObjPool >= 0)
				if((int)m_dwFreeBufferObjPool >= 0)
					if((int)m_dwFreeSocketObjHold >= m_dwFreeSocketObjPool)
						if((int)m_dwFreeBufferObjHold >= m_dwFreeBufferObjPool)
							if((int)m_dwMaxDatagramSize > 0 && m_dwMaxDatagramSize <= MAXWORD)
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
	if(m_enState == SS_STARTED || m_enState == SS_STARTING)
		m_enState = SS_STOPING;
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
			if(FirePrepareListen(m_soListen) != ISocketListener::HR_ERROR)
				isOK = TRUE;
			else
				SetLastError(SE_SOCKET_PREPARE, __FUNCTION__, ERROR_FUNCTION_FAILED);
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
	BOOL isOK = FALSE;

	if(::CreateIoCompletionPort((HANDLE)m_soListen, m_hCompletePort, m_soListen, 0))
	{
		isOK = TRUE;

		for(DWORD i = 0; i < m_dwWorkerThreadCount; i++)
			::PostQueuedCompletionStatus(m_hCompletePort, IOCP_SI_ACCEPT, 0, nullptr);
	}
	else
		SetLastError(SE_SOCKE_ATTACH_TO_CP, __FUNCTION__, ::GetLastError());

	return isOK;
}

BOOL CUdpServer::Stop()
{
	if(!CheckStoping())
		return FALSE;

	CloseListenSocket();

	WaitForDetectorThreadEnd();

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

	m_dwConnID	= 0;
	m_enState	= SS_STOPED;
}

void CUdpServer::CloseListenSocket()
{
	if(m_soListen != INVALID_SOCKET)
	{
		::ManualCloseSocket(m_soListen);
		m_soListen = INVALID_SOCKET;
	}
}

void CUdpServer::DisconnectClientSocket()
{
	CReentrantWriteLock locallock(m_csClientSocket);

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

TUdpSocketObj*	CUdpServer::GetFreeSocketObj()
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

	if(!pSocketObj) pSocketObj	= CreateSocketObj();
	pSocketObj->extra			= nullptr;

	return pSocketObj;
}

void CUdpServer::AddFreeSocketObj(CONNID dwConnID)
{
	BOOL bDone = FALSE;
	TUdpSocketObj* pSocketObj = nullptr;

	{
		CReentrantWriteLock locallock(m_csClientSocket);

		pSocketObj = FindSocketObj(dwConnID);
		if(TUdpSocketObj::IsValid(pSocketObj))
		{
			m_mpClientAddr.erase(&pSocketObj->clientAddr);
			m_mpClientSocket.erase(dwConnID);
			bDone = TRUE;
		}
	}

	if(bDone)
	{
		{
			CCriSecLock locallock(m_csFreeSocket);

			pSocketObj->freeTime = ::TimeGetTime();
			m_lsFreeSocket.push_back(pSocketObj);
		}

		if(m_lsFreeSocket.size() > m_dwFreeSocketObjHold)
			CompressFreeSocket(m_dwFreeSocketObjPool);
	}
}

void CUdpServer::AddClientSocketObj(CONNID dwConnID, TUdpSocketObj* pSocketObj)
{
	CReentrantWriteLock locallock(m_csClientSocket);

	ASSERT(FindSocketObj(dwConnID) == nullptr);

	pSocketObj->connTime						= ::TimeGetTime();
	m_mpClientSocket[dwConnID]					= pSocketObj;
	m_mpClientAddr	[&pSocketObj->clientAddr]	= dwConnID;
}

void CUdpServer::ReleaseFreeSocket()
{
	CompressFreeSocket(0, TRUE);
}

void CUdpServer::CompressFreeSocket(size_t size, BOOL bForce)
{
	CCriSecLock locallock(m_csFreeSocket);

	while(m_lsFreeSocket.size() > size)
	{
		TUdpSocketObj* pSocketObj = m_lsFreeSocket.front();

		if(bForce || ::GetTimeGap32(pSocketObj->freeTime) >= m_dwFreeSocketObjLockTime)
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

	pSocketObj->TUdpSocketObj::TUdpSocketObj();
	
	return pSocketObj;
}

void CUdpServer::DeleteSocketObj(TUdpSocketObj* pSocketObj)
{
	ASSERT(pSocketObj);

	pSocketObj->TUdpSocketObj::~TUdpSocketObj();;
	m_phSocket.Free(pSocketObj);
}

TUdpBufferObj*	CUdpServer::GetFreeBufferObj(int iLen)
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
	pBufferObj->buff.len = iLen;

	return pBufferObj;
}

void CUdpServer::AddFreeBufferObj(TUdpBufferObj* pBufferObj)
{
	{
		CCriSecLock locallock(m_csFreeBuffer);
		m_lsFreeBuffer.push_back(pBufferObj);
	}

	if(m_lsFreeBuffer.size() > m_dwFreeBufferObjHold)
		CompressFreeBuffer(m_dwFreeBufferObjPool);
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

BOOL CUdpServer::GetListenAddress(LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	return ::GetSocketLocalAddress(m_soListen, lpszAddress, iAddressLen, usPort);
}

BOOL CUdpServer::GetClientAddress(CONNID dwConnID, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TUdpSocketObj::IsValid(pSocketObj))
	{
		ADDRESS_FAMILY usFamily;
		return ::sockaddr_IN_2_A(pSocketObj->clientAddr, usFamily, lpszAddress, iAddressLen, usPort);
	}

	return FALSE;
}

BOOL CUdpServer::SetConnectionExtra(CONNID dwConnID, PVOID pExtra)
{
	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TUdpSocketObj::IsValid(pSocketObj))
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

	if(TUdpSocketObj::IsValid(pSocketObj))
	{
		*ppExtra = pSocketObj->extra;
		return TRUE;
	}

	return FALSE;
}

DWORD CUdpServer::GetConnectionCount()
{
	return (DWORD)m_mpClientSocket.size();
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
		isOK = ::PostQueuedCompletionStatus(m_hCompletePort, IOCP_SI_DISCONNECT, dwConnID, nullptr);

	return isOK;
}

BOOL CUdpServer::DisconnectLongConnections(DWORD dwPeriod, BOOL bForce)
{
	ulong_ptr_list ls;

	{
		CReentrantReadLock locallock(m_csClientSocket);

		size_t size = m_mpClientSocket.size();

		if(size > 0)
		{
			for(TUdpSocketObjPtrMapCI it = m_mpClientSocket.begin(); it != m_mpClientSocket.end(); ++it)
			{
				if(::GetTimeGap32(it->second->connTime) >= dwPeriod)
					ls.push_back(it->first);
			}
		}
	}
	
	size_t size = ls.size();

	if(size > 0)
	{
		for(ulong_ptr_list::const_iterator it = ls.begin(); it != ls.end(); ++it)
			Disconnect(*it, bForce);
	}

	return size > 0;
}

void CUdpServer::WaitForClientSocketClose()
{
	DWORD dwWait = 0;
	DWORD dwOrig = ::TimeGetTime();

	while(m_mpClientSocket.size() > 0 && dwWait < m_dwMaxShutdownWaitTime)
	{
		::WaitWithMessageLoop(100);
		dwWait = ::GetTimeGap32(dwOrig);
	}
}

void CUdpServer::WaitForWorkerThreadEnd()
{
	int count = (int)m_vtWorkerThreads.size();

	for(int i = 0; i < count; i++)
		::PostQueuedCompletionStatus(m_hCompletePort, IOCP_SI_EXIT, 0, nullptr);

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

void CUdpServer::TerminateWorkerThread()
{
	size_t count = m_vtWorkerThreads.size();

	for(size_t i = 0; i < count; i++)
	{
		HANDLE hThread = m_vtWorkerThreads[i];
		::TerminateThread(hThread, 1);
		::CloseHandle(hThread);
	}

	m_vtWorkerThreads.clear();
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
	return DoReceive(pBufferObj);
}

UINT WINAPI CUdpServer::WorkerThreadProc(LPVOID pv)
{
	CUdpServer* pServer	= (CUdpServer*)pv;

	while (TRUE)
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
			int indative = pServer->CheckSpecialIndative(pOverlapped, dwBytes, ulCompKey);

			if(indative == IOCP_SI_RS_CONTINUE)
				continue;
			else if(indative == IOCP_SI_RS_BREAK)
				break;
		}

		TUdpBufferObj* pBufferObj	= CONTAINING_RECORD(pOverlapped, TUdpBufferObj, ov);
		CONNID dwConnID				= pServer->FindConnectionID(&pBufferObj->clientAddr);

		if (!result)
		{
			DWORD dwFlag	= 0;
			DWORD dwSysCode = ::GetLastError();

			if(pServer->HasStarted())
			{
				result		= ::WSAGetOverlappedResult((SOCKET)ulCompKey, &pBufferObj->ov, &dwBytes, FALSE, &dwFlag);

				if (!result)
				{
					dwErrorCode = ::WSAGetLastError();
					TRACE("GetQueuedCompletionStatus error (<S-CNNID: %Iu> SYS: %d, SOCK: %d, FLAG: %d)\n", dwConnID, dwSysCode, dwErrorCode, dwFlag);
				}
			}
			else
				dwErrorCode = dwSysCode;
		}

		pServer->HandleIo(dwConnID, pBufferObj, dwBytes, dwErrorCode);
	}

	return 0;
}

int CUdpServer::CheckSpecialIndative(OVERLAPPED* pOverlapped, DWORD dwBytes, ULONG_PTR ulCompKey)
{
	int indative = IOCP_SI_RS_GONO;

	if(pOverlapped == nullptr)
	{
		if(dwBytes == IOCP_SI_ACCEPT)
		{
			DoAccept();
			indative = IOCP_SI_RS_CONTINUE;
		}
		else if(dwBytes == IOCP_SI_DISCONNECT)
		{
			ForceDisconnect((CONNID)ulCompKey);
			indative = IOCP_SI_RS_CONTINUE;
		}
		else if(dwBytes == IOCP_SI_EXIT && ulCompKey == 0)
			indative = IOCP_SI_RS_BREAK;
		else
			VERIFY(FALSE);
	}

	return indative;
}

void CUdpServer::ForceDisconnect(CONNID dwConnID)
{
	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TUdpSocketObj::IsValid(pSocketObj))
	{
		FireClose(dwConnID);
		AddFreeSocketObj(dwConnID);
	}
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
	{
		FireError(dwConnID, pBufferObj->operation, dwErrorCode);
		AddFreeSocketObj(dwConnID);
	}
	
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
			TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

			if(TUdpSocketObj::IsValid(pSocketObj))
			{
				pSocketObj->detectFails = 0;
				DoSend(pSocketObj, nullptr, 0);
			}
			
			TRACE("<S-CNNID: %Iu> recv 0 bytes (detect package)\n", dwConnID);
		}

		DoReceive(pBufferObj);
	}
	else
	{
		ASSERT(dwConnID != 0);
		TRACE("<S-CNNID: %Iu> send 0 bytes (detect package)\n", dwConnID);
	}
}

CONNID CUdpServer::HandleAccept(TUdpBufferObj* pBufferObj)
{
	CONNID dwConnID = 0;

	{
		CCriSecLock locallock(m_csAccept);

		dwConnID = FindConnectionID(&pBufferObj->clientAddr);

		if(dwConnID == 0)
		{
			dwConnID					= ::GenerateConnectionID(m_dwConnID);
			TUdpSocketObj* pSocketObj	= GetFreeSocketObj();
			pSocketObj->connID			= dwConnID;
			pSocketObj->detectFails		= 0;

			memcpy(&pSocketObj->clientAddr, &pBufferObj->clientAddr, pBufferObj->addrLen);
			AddClientSocketObj(dwConnID, pSocketObj);
		}
		else
			return dwConnID;
	}

	if(FireAccept(dwConnID, &pBufferObj->clientAddr) == ISocketListener::HR_ERROR)
	{
		AddFreeSocketObj(dwConnID);
		dwConnID = 0;
	}

	return dwConnID;
}

void CUdpServer::HandleSend(CONNID dwConnID, TUdpBufferObj* pBufferObj)
{
	if(FireSend(dwConnID, (BYTE*)pBufferObj->buff.buf, pBufferObj->buff.len) == ISocketListener::HR_ERROR)
	{
		TRACE1("<S-CNNID: %Iu> OnSend() event should not return 'HR_ERROR' !!\n", dwConnID);
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
		if(FireReceive(dwConnID, (BYTE*)pBufferObj->buff.buf, pBufferObj->buff.len) == ISocketListener::HR_ERROR)
		{
			TRACE1("<S-CNNID: %Iu> OnReceive() event return 'HR_ERROR', connection will be closed !\n", dwConnID);
			FireError(dwConnID, SO_RECEIVE, ERROR_FUNCTION_FAILED);
			AddFreeSocketObj(dwConnID);
		}
	}

	DoReceive(pBufferObj);
}

BOOL CUdpServer::DoReceive(TUdpBufferObj* pBufferObj)
{
	BOOL isOK = FALSE;

	if(HasStarted())
	{
		pBufferObj->buff.len = m_dwMaxDatagramSize;
		isOK = (::PostReceiveFrom(m_soListen, pBufferObj) == NO_ERROR);
	}

	if(!isOK)
	{
		VERIFY(!HasStarted());
		AddFreeBufferObj(pBufferObj);
	}

	return isOK;
}

BOOL CUdpServer::Send(CONNID dwConnID, const BYTE* pBuffer, int iLength)
{
	ASSERT(pBuffer && iLength > 0 && iLength <= (int)m_dwMaxDatagramSize);

	if(!pBuffer || iLength <= 0 || iLength > (int)m_dwMaxDatagramSize)
		return FALSE;

	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(pSocketObj == nullptr)
		return FALSE;

	DoSend(pSocketObj, pBuffer, iLength);
	
	return TRUE;
}

BOOL CUdpServer::DoSend(TUdpSocketObj* pSocketObj, const BYTE* pBuffer, int iLen)
{
	BOOL isOK = FALSE;

	if(HasStarted())
	{
		TUdpBufferObj* pBufferObj	= GetFreeBufferObj(iLen);
		pBufferObj->addrLen			= sizeof(SOCKADDR_IN);

		memcpy(&pBufferObj->clientAddr, &pSocketObj->clientAddr, pBufferObj->addrLen);
		memcpy(pBufferObj->buff.buf, pBuffer, iLen);

		isOK = (::PostSendTo(m_soListen, pBufferObj) == NO_ERROR);

		if(!isOK)
		{
			VERIFY(!HasStarted());
			AddFreeBufferObj(pBufferObj);
		}
	}

	return isOK;
}

UINT WINAPI CUdpServer::DetecotrThreadProc(LPVOID pv)
{
	TRACE1("---------------> Server Detecotr Thread 0x%08X started <---------------\n", ::GetCurrentThreadId());

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

	TRACE1("---------------> Server Detecotr Thread 0x%08X stoped <---------------\n", ::GetCurrentThreadId());

	return 0;
}

void CUdpServer::DetectConnections()
{
	CReentrantReadLock locallock(m_csClientSocket);

	for(TUdpSocketObjPtrMapCI it = m_mpClientSocket.begin(); it != m_mpClientSocket.end(); ++it)
	{
		if(it->second->detectFails++ >= m_dwDetectAttempts)
			::PostQueuedCompletionStatus(m_hCompletePort, IOCP_SI_DISCONNECT, it->first, nullptr);
	}

}

LPCTSTR CUdpServer::GetLastErrorDesc()
{
	switch(m_enLastError)
	{
	case SE_OK:						return _T("成功");
	case SE_ILLEGAL_STATE:			return _T("当前状态不允许操作");
	case SE_INVALID_PARAM:			return _T("非法参数");
	case SE_SOCKET_CREATE:			return _T("创建监听 SOCKET 失败");
	case SE_SOCKET_BIND:			return _T("绑定监听地址失败");
	case SE_SOCKET_PREPARE:			return _T("设置监听 SOCKET 失败");
	case SE_SOCKET_LISTEN:			return _T("启动监听失败");
	case SE_CP_CREATE:				return _T("创建完成端口失败");
	case SE_WORKER_THREAD_CREATE:	return _T("创建工作线程失败");
	case SE_DETECT_THREAD_CREATE:	return _T("创建监测线程失败");
	case SE_SOCKE_ATTACH_TO_CP:		return _T("监听 SOCKET 绑定到完成端口失败");
	default: ASSERT(FALSE);			return _T("");
	}
}
