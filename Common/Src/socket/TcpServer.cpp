/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.2.1
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
#include "TcpServer.h"
#include "../WaitFor.h"
#include "../FuncHelper.h"

#include <malloc.h>
#include <process.h>

const DWORD	CTcpServer::MAX_WORKER_THREAD_COUNT			= 500;
const DWORD	CTcpServer::MIN_SOCKET_BUFFER_SIZE			= 64;
const DWORD	CTcpServer::DEFAULT_WORKER_THREAD_COUNT		= min((::GetCpuCount() * 2 + 2), MAX_WORKER_THREAD_COUNT);
const DWORD	CTcpServer::DEFAULT_SOCKET_LISTEN_QUEUE		= 300;
const DWORD	CTcpServer::DEFAULT_ACCEPT_SOCKET_COUNT		= DEFAULT_SOCKET_LISTEN_QUEUE;
const DWORD	CTcpServer::DEFAULT_SOCKET_BUFFER_SIZE		= 4 * 1024 - sizeof(TBufferObj);
const DWORD CTcpServer::DEFAULT_FREE_SOCKETOBJ_LOCK_TIME= 3 * 1000;
const DWORD	CTcpServer::DEFAULT_FREE_SOCKETOBJ_POOL		= 150;
const DWORD	CTcpServer::DEFAULT_FREE_SOCKETOBJ_HOLD		= 450;
const DWORD	CTcpServer::DEFAULT_FREE_BUFFEROBJ_POOL		= 300;
const DWORD	CTcpServer::DEFAULT_FREE_BUFFEROBJ_HOLD		= 900;
const DWORD	CTcpServer::DEFALUT_KEEPALIVE_TIME			= 5 * 1000;
const DWORD	CTcpServer::DEFALUT_KEEPALIVE_INTERVAL		= 3 * 1000;
const DWORD	CTcpServer::DEFAULT_MAX_SHUTDOWN_WAIT_TIME	= 15 * 1000;

void CTcpServer::SetLastError(EnSocketError code, LPCSTR func, int ec)
{
	m_enLastError = code;

	TRACE("%s --> Error: %d, EC: %d\n", func, code, ec);
}

BOOL CTcpServer::Start(LPCTSTR pszBindAddress, USHORT usPort)
{
	if(!CheckParams() || !CheckStarting())
		return FALSE;

	if(CreateListenSocket(pszBindAddress, usPort))
		if(CreateCompletePort())
			if(CreateWorkerThreads())
				if(StartAccept())
				{
					m_enState = SS_STARTED;
					return TRUE;
				}

	Stop();

	return FALSE;
}

BOOL CTcpServer::CheckParams()
{
	m_itPool.SetItemCapacity((int)m_dwSocketBufferSize);
	m_itPool.SetPoolSize((int)m_dwFreeBufferObjPool);
	m_itPool.SetPoolHold((int)m_dwFreeBufferObjHold);

	if((int)m_dwWorkerThreadCount > 0 && m_dwWorkerThreadCount <= MAX_WORKER_THREAD_COUNT)
		if((int)m_dwAcceptSocketCount > 0)
			if((int)m_dwSocketBufferSize >= MIN_SOCKET_BUFFER_SIZE)
				if((int)m_dwSocketListenQueue > 0)
					if((int)m_dwFreeSocketObjLockTime >= 0 && m_dwFreeSocketObjLockTime <= MAXLONG)
						if((int)m_dwFreeSocketObjPool >= 0)
							if((int)m_dwFreeBufferObjPool >= 0)
								if((int)m_dwFreeSocketObjHold >= m_dwFreeSocketObjPool)
									if((int)m_dwFreeBufferObjHold >= m_dwFreeBufferObjPool)
										if((int)m_dwKeepAliveTime >= 0)
											if((int)m_dwKeepAliveInterval >= 0)
												if((int)m_dwMaxShutdownWaitTime >= 0)
													return TRUE;

	SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
	return FALSE;
}

BOOL CTcpServer::CheckStarting()
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

BOOL CTcpServer::CheckStoping()
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

BOOL CTcpServer::CreateListenSocket(LPCTSTR pszBindAddress, USHORT usPort)
{
	BOOL isOK = FALSE;

	m_soListen	= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(m_soListen != INVALID_SOCKET)
	{
		SOCKADDR_IN addr;
		::sockaddr_A_2_IN(AF_INET, pszBindAddress, usPort, addr);

		if(::bind(m_soListen, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)) != SOCKET_ERROR)
		{
			if(FirePrepareListen(m_soListen) != HR_ERROR)
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
				SetLastError(SE_SOCKET_PREPARE, __FUNCTION__, ERROR_FUNCTION_FAILED);
		}
		else
			SetLastError(SE_SOCKET_BIND, __FUNCTION__, ::WSAGetLastError());
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
	BOOL isOK = FALSE;

	if(::CreateIoCompletionPort((HANDLE)m_soListen, m_hCompletePort, m_soListen, 0))
	{
		isOK = TRUE;

		for(DWORD i = 0; i < m_dwAcceptSocketCount; i++)
			::PostIocpAccept(m_hCompletePort);
	}
	else
		SetLastError(SE_SOCKE_ATTACH_TO_CP, __FUNCTION__, ::GetLastError());

	return isOK;
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

	FireServerShutdown();

	ReleaseFreeSocket();
	ReleaseFreeBuffer();

	CloseCompletePort();

	Reset();

	return TRUE;
}

void CTcpServer::Reset()
{
	m_phSocket.Reset();
	m_phBuffer.Reset();
	m_itPool.Clear();

	m_pfnAcceptEx				= nullptr;
	m_pfnGetAcceptExSockaddrs	= nullptr;
	m_pfnDisconnectEx			= nullptr;
	m_enState					= SS_STOPED;
}

void CTcpServer::CloseListenSocket()
{
	if(m_soListen != INVALID_SOCKET)
	{
		::ManualCloseSocket(m_soListen);
		m_soListen = INVALID_SOCKET;
	}
}

void CTcpServer::DisconnectClientSocket()
{
	CReentrantReadLock locallock(m_csClientSocket);

	for(TSocketObjPtrMapI it = m_mpClientSocket.begin(); it != m_mpClientSocket.end(); ++it)
		Disconnect(it->first);
}

void CTcpServer::ReleaseClientSocket()
{
	CReentrantWriteLock locallock(m_csClientSocket);

	for(TSocketObjPtrMapI it = m_mpClientSocket.begin(); it != m_mpClientSocket.end(); ++it)
	{
		TSocketObj* pSocketObj = it->second;

		CloseClientSocketObj(pSocketObj);
		DeleteSocketObj(pSocketObj);
	}

	m_mpClientSocket.clear();
}

TSocketObj*	CTcpServer::GetFreeSocketObj(CONNID dwConnID, SOCKET soClient)
{
	TSocketObj* pSocketObj = nullptr;

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
	pSocketObj->Reset(dwConnID, soClient);

	return pSocketObj;
}

void CTcpServer::AddFreeSocketObj(CONNID dwConnID, EnSocketCloseFlag enFlag, EnSocketOperation enOperation, int iErrorCode)
{
	BOOL bDone				= FALSE;
	TSocketObj* pSocketObj	= FindSocketObj(dwConnID);;

	if(TSocketObj::IsValid(pSocketObj))
	{
		CReentrantWriteLock locallock(m_csClientSocket);

		if(TSocketObj::IsValid(pSocketObj))
		{
			TSocketObj::Invalid(pSocketObj);
			bDone = TRUE;
		}
	}

	if(bDone)
	{
		CloseClientSocketObj(pSocketObj, enFlag, enOperation, iErrorCode);
		TSocketObj::Release(pSocketObj);

		{
			CReentrantWriteLock locallock(m_csClientSocket);
			m_mpClientSocket.erase(dwConnID);
		}

		{
			CCriSecLock locallock(m_csFreeSocket);
			m_lsFreeSocket.push_back(pSocketObj);
		}

		if(m_lsFreeSocket.size() > m_dwFreeSocketObjHold)
			CompressFreeSocket(m_dwFreeSocketObjPool);
	}
}

void CTcpServer::AddClientSocketObj(CONNID dwConnID, TSocketObj* pSocketObj)
{
	ASSERT(FindSocketObj(dwConnID) == nullptr);

	pSocketObj->connTime = ::TimeGetTime();

	CReentrantWriteLock locallock(m_csClientSocket);
	m_mpClientSocket[dwConnID] = pSocketObj;
}

void CTcpServer::ReleaseFreeSocket()
{
	CompressFreeSocket(0, TRUE);
}

void CTcpServer::CompressFreeSocket(size_t size, BOOL bForce)
{
	CCriSecLock locallock(m_csFreeSocket);

	DWORD now = ::TimeGetTime();

	while(m_lsFreeSocket.size() > size)
	{
		TSocketObj* pSocketObj = m_lsFreeSocket.front();

		if(bForce || (now - pSocketObj->freeTime) >= m_dwFreeSocketObjLockTime)
		{
			m_lsFreeSocket.pop_front();
			DeleteSocketObj(pSocketObj);
		}
		else
			break;
	}
}

TSocketObj* CTcpServer::CreateSocketObj()
{
	TSocketObj* pSocketObj = (TSocketObj*)m_phSocket.Alloc(sizeof(TSocketObj), HEAP_ZERO_MEMORY);
	ASSERT(pSocketObj);

	pSocketObj->TSocketObj::TSocketObj(m_itPool);
	
	return pSocketObj;
}

void CTcpServer::DeleteSocketObj(TSocketObj* pSocketObj)
{
	ASSERT(pSocketObj);

	pSocketObj->TSocketObj::~TSocketObj();
	m_phSocket.Free(pSocketObj);
}

TBufferObj*	CTcpServer::GetFreeBufferObj(int iLen)
{
	ASSERT(iLen >= 0 && iLen <= (int)m_dwSocketBufferSize);

	TBufferObj* pBufferObj = nullptr;

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

	if(iLen <= 0) iLen	 = m_dwSocketBufferSize;
	pBufferObj->buff.len = iLen;

	return pBufferObj;
}

void CTcpServer::AddFreeBufferObj(TBufferObj* pBufferObj)
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

void CTcpServer::ReleaseFreeBuffer()
{
	CompressFreeBuffer(0);
}

void CTcpServer::CompressFreeBuffer(size_t size)
{
	CCriSecLock locallock(m_csFreeBuffer);

	while(m_lsFreeBuffer.size() > size)
	{
		TBufferObj* pBufferObj = m_lsFreeBuffer.front();

		m_lsFreeBuffer.pop_front();
		DeleteBufferObj(pBufferObj);
	}
}

TBufferObj* CTcpServer::CreateBufferObj()
{
	TBufferObj* pBufferObj	= (TBufferObj*)m_phBuffer.Alloc(sizeof(TBufferObj) + m_dwSocketBufferSize, HEAP_ZERO_MEMORY);
	pBufferObj->buff.buf	= ((char*)pBufferObj) + sizeof(TBufferObj);

	ASSERT(pBufferObj);
	return pBufferObj;
}

void CTcpServer::DeleteBufferObj(TBufferObj* pBufferObj)
{
	ASSERT(pBufferObj);
	m_phBuffer.Free(pBufferObj);
}

TSocketObj* CTcpServer::FindSocketObj(CONNID dwConnID)
{
	TSocketObj* pSocketObj = nullptr;

	CReentrantReadLock locallock(m_csClientSocket);

	TSocketObjPtrMapCI it = m_mpClientSocket.find(dwConnID);
	if(it != m_mpClientSocket.end())
		pSocketObj = it->second;

	return pSocketObj;
}

void CTcpServer::CloseClientSocketObj(TSocketObj* pSocketObj, EnSocketCloseFlag enFlag, EnSocketOperation enOperation, int iErrorCode, int iShutdownFlag)
{
	ASSERT(TSocketObj::IsExist(pSocketObj));

	if(enFlag == SCF_CLOSE)
		FireClose(pSocketObj->connID);
	else if(enFlag == SCF_ERROR)
		FireError(pSocketObj->connID, enOperation, iErrorCode);

	::ManualCloseSocket(pSocketObj->socket, iShutdownFlag);
}

BOOL CTcpServer::GetListenAddress(LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	return ::GetSocketLocalAddress(m_soListen, lpszAddress, iAddressLen, usPort);
}

BOOL CTcpServer::GetRemoteAddress(CONNID dwConnID, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort)
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

	if(TSocketObj::IsExist(pSocketObj))
	{
		pSocketObj->extra = pExtra;
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpServer::GetConnectionExtra(CONNID dwConnID, PVOID* ppExtra)
{
	ASSERT(ppExtra != nullptr);

	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsExist(pSocketObj))
	{
		*ppExtra = pSocketObj->extra;
		return TRUE;
	}

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
	return (DWORD)m_mpClientSocket.size();
}

BOOL CTcpServer::GetConnectPeriod(CONNID dwConnID, DWORD& dwPeriod)
{
	BOOL isOK				= TRUE;
	TSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(TSocketObj::IsExist(pSocketObj))
		dwPeriod = GetTimeGap32(pSocketObj->connTime);
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
	ulong_ptr_deque ls;

	{
		CReentrantReadLock locallock(m_csClientSocket);

		DWORD now = ::TimeGetTime();

		for(TSocketObjPtrMapCI it = m_mpClientSocket.begin(); it != m_mpClientSocket.end(); ++it)
		{
			if(now - it->second->connTime >= dwPeriod)
				ls.push_back(it->first);
		}
	}
	
	for(ulong_ptr_deque::const_iterator it = ls.begin(); it != ls.end(); ++it)
		Disconnect(*it, bForce);

	return ls.size() > 0;
}

void CTcpServer::WaitForClientSocketClose()
{
	DWORD dwWait = 0;
	DWORD dwOrig = ::TimeGetTime();

	while(m_mpClientSocket.size() > 0 && dwWait < m_dwMaxShutdownWaitTime)
	{
		::WaitWithMessageLoop(100);
		dwWait = ::GetTimeGap32(dwOrig);
	}
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

		VERIFY(::WaitForMultipleObjects((DWORD)wait, pHandles, TRUE, INFINITE) == WAIT_OBJECT_0);

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
		SOCKET		soClient	= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		TBufferObj*	pBufferObj	= GetFreeBufferObj();

		ASSERT(soClient != INVALID_SOCKET);

		isOK = (::PostAccept(m_pfnAcceptEx, m_soListen, soClient, pBufferObj) == NO_ERROR);

		if(!isOK)
		{
			VERIFY(!HasStarted());

			::ManualCloseSocket(soClient);
			AddFreeBufferObj(pBufferObj);
		}
	}

	return isOK;
}

UINT WINAPI CTcpServer::WorkerThreadProc(LPVOID pv)
{
	CTcpServer* pServer	= (CTcpServer*)pv;

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
				SOCKET sock	= pBufferObj->operation != SO_ACCEPT ? pSocketObj->socket : (SOCKET)pSocketObj;
				result		= ::WSAGetOverlappedResult(sock, &pBufferObj->ov, &dwBytes, FALSE, &dwFlag);

				if (!result)
				{
					dwErrorCode = ::WSAGetLastError();
					TRACE("GetQueuedCompletionStatus error (<S-CNNID: %Iu> SYS: %d, SOCK: %d, FLAG: %d)\n", dwConnID, dwSysCode, dwErrorCode, dwFlag);
				}
			}
			else
				dwErrorCode = dwSysCode;
		}

		pServer->HandleIo(dwConnID, pSocketObj, pBufferObj, dwBytes, dwErrorCode);
	}

	return 0;
}

EnIocpAction CTcpServer::CheckIocpCommand(OVERLAPPED* pOverlapped, DWORD dwBytes, ULONG_PTR ulCompKey)
{
	EnIocpAction action = IOCP_ACT_GOON;

	if(pOverlapped == nullptr)
	{
		if(dwBytes == IOCP_CMD_SEND)
		{
			DoSend((CONNID)ulCompKey);
			action = IOCP_ACT_CONTINUE;
		}
		else if(dwBytes == IOCP_CMD_ACCEPT)
		{
			DoAccept();
			action = IOCP_ACT_CONTINUE;
		}
		else if(dwBytes == IOCP_CMD_DISCONNECT)
		{
			ForceDisconnect((CONNID)ulCompKey);
			action = IOCP_ACT_CONTINUE;
		}
		else if(dwBytes == IOCP_CMD_EXIT && ulCompKey == 0)
			action = IOCP_ACT_BREAK;
		else
			VERIFY(FALSE);
	}

	return action;
}

void CTcpServer::ForceDisconnect(CONNID dwConnID)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
		AddFreeSocketObj(dwConnID, SCF_CLOSE);
}

void CTcpServer::HandleIo(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj, DWORD dwBytes, DWORD dwErrorCode)
{
	ASSERT(pBufferObj != nullptr);
	ASSERT(pSocketObj != nullptr);

	if(dwErrorCode != NO_ERROR)
	{
		HandleError(dwConnID, pBufferObj, dwErrorCode);
		return;
	}

	if(dwBytes == 0 && pBufferObj->operation != SO_ACCEPT)
	{
		AddFreeSocketObj(dwConnID, SCF_CLOSE);
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

void CTcpServer::HandleError(CONNID dwConnID, TBufferObj* pBufferObj, DWORD dwErrorCode)
{
	if(pBufferObj->operation != SO_ACCEPT)
		CheckError(dwConnID, pBufferObj->operation, dwErrorCode);
	else
	{
		::ManualCloseSocket(pBufferObj->client);
		::PostIocpAccept(m_hCompletePort);
	}

	AddFreeBufferObj(pBufferObj);
}

void CTcpServer::HandleAccept(SOCKET soListen, TBufferObj* pBufferObj)
{
	::PostIocpAccept(m_hCompletePort);

	int iLocalSockaddrLen;
	int iRemoteSockaddrLen;
	SOCKADDR* pLocalSockAddr;
	SOCKADDR* pRemoteSockAddr;

	m_pfnGetAcceptExSockaddrs
							(
								pBufferObj->buff.buf,
								0,
								sizeof(SOCKADDR_IN) + 16,
								sizeof(SOCKADDR_IN) + 16,
								(SOCKADDR **)&pLocalSockAddr,
								&iLocalSockaddrLen,
								(SOCKADDR **)&pRemoteSockAddr,
								&iRemoteSockaddrLen
							);

	SOCKET socket			= pBufferObj->client;
	CONNID dwConnID			= ::GenerateConnectionID();
	TSocketObj* pSocketObj	= GetFreeSocketObj(dwConnID, socket);

	memcpy(&pSocketObj->remoteAddr, pRemoteSockAddr, sizeof(SOCKADDR_IN));
	AddClientSocketObj(dwConnID, pSocketObj);

	VERIFY(::SSO_UpdateAcceptContext(socket, soListen) == NO_ERROR);

	BOOL bOnOff	= (m_dwKeepAliveTime > 0 && m_dwKeepAliveInterval > 0);
	VERIFY(::SSO_KeepAliveVals(socket, bOnOff, m_dwKeepAliveTime, m_dwKeepAliveInterval) == NO_ERROR);

	if(FireAccept(dwConnID, socket) != HR_ERROR)
	{
		VERIFY(::CreateIoCompletionPort((HANDLE)socket, m_hCompletePort, (ULONG_PTR)pSocketObj, 0));
		DoReceive(dwConnID, pSocketObj, pBufferObj);
	}
	else
	{
		AddFreeSocketObj(dwConnID, SCF_NONE);
		AddFreeBufferObj(pBufferObj);
	}
}

void CTcpServer::HandleSend(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	if(FireSend(dwConnID, (BYTE*)pBufferObj->buff.buf, pBufferObj->buff.len) == HR_ERROR)
	{
		TRACE("<S-CNNID: %Iu> OnSend() event should not return 'HR_ERROR' !!\n", dwConnID);
		ASSERT(FALSE);
	}

	AddFreeBufferObj(pBufferObj);

	if(TSocketObj::IsValid(pSocketObj))
	{
		long sndCount = ::InterlockedDecrement(&pSocketObj->sndCount);
		if(sndCount == 0) DoSend(pSocketObj);
	}
}

void CTcpServer::HandleReceive(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	if(FireReceive(dwConnID, (BYTE*)pBufferObj->buff.buf, pBufferObj->buff.len) != HR_ERROR)
		DoReceive(dwConnID, pSocketObj, pBufferObj);
	else
	{
		TRACE("<S-CNNID: %Iu> OnReceive() event return 'HR_ERROR', connection will be closed !\n", dwConnID);
		AddFreeSocketObj(dwConnID, SCF_ERROR, SO_RECEIVE, ERROR_FUNCTION_FAILED);
		AddFreeBufferObj(pBufferObj);
	}
}

int CTcpServer::DoReceive(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	pBufferObj->buff.len = m_dwSocketBufferSize;
	int result			 = ::PostReceive(pSocketObj, pBufferObj);

	if(result != NO_ERROR)
	{
		CheckError(dwConnID, SO_RECEIVE, result);
		AddFreeBufferObj(pBufferObj);
	}

	return result;
}

BOOL CTcpServer::Send(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset)
{
	ASSERT(pBuffer && iLength > 0);

	if(iOffset != 0) pBuffer += iOffset;

	int result				= NO_ERROR;
	TSocketObj* pSocketObj	= FindSocketObj(dwConnID);

	if(!TSocketObj::IsValid(pSocketObj))
		result = ERROR_OBJECT_NOT_FOUND;
	else
	{
		CCriSecLock locallock(pSocketObj->crisec);

		if(!TSocketObj::IsValid(pSocketObj))
			result = ERROR_OBJECT_NOT_FOUND;
		else
		{
			BOOL isPending = TSocketObj::IsPending(pSocketObj);
			pSocketObj->sndBuff.Cat(pBuffer, iLength);
			pSocketObj->pending += iLength;

			if(!isPending && !::PostIocpSend(m_hCompletePort, dwConnID))
				result = ::GetLastError();
		}
	}
	
	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);
}

BOOL CTcpServer::DoSend(CONNID dwConnID)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);
	return DoSend(pSocketObj);
}

BOOL CTcpServer::DoSend(TSocketObj* pSocketObj)
{
	BOOL isOK	= TRUE;
	int result	= NO_ERROR;

	if(TSocketObj::IsPending(pSocketObj))
	{
		CCriSecLock locallock(pSocketObj->crisec);

		TItemList& sndBuff = pSocketObj->sndBuff;

		while(sndBuff.Size() > 0)
		{
			TItemPtr itPtr(m_itPool, sndBuff.PopFront());

			int iBufferSize = itPtr->Size();
			ASSERT(iBufferSize > 0 && iBufferSize <= (int)m_dwSocketBufferSize);

			TBufferObj* pBufferObj = GetFreeBufferObj(iBufferSize);
			memcpy(pBufferObj->buff.buf, itPtr->Ptr(), iBufferSize);

			::InterlockedIncrement(&pSocketObj->sndCount);
			result = ::PostSendNotCheck(pSocketObj, pBufferObj);
			pSocketObj->pending -= iBufferSize;

			if(result != NO_ERROR)
			{
				if(result != WSA_IO_PENDING)
				{
					AddFreeBufferObj(pBufferObj);
					isOK = FALSE;
				}

				break;
			}
		}
	}

	if(!isOK) CheckError(pSocketObj->connID, SO_SEND, result);
	return isOK;
}

void CTcpServer::CheckError(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	if(iErrorCode != WSAENOTSOCK && iErrorCode != ERROR_OPERATION_ABORTED)
		AddFreeSocketObj(dwConnID, SCF_ERROR, enOperation, iErrorCode);
}
