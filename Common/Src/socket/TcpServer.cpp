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
#include "TcpServer.h"
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

const DWORD	CTcpServer::MAX_WORKER_THREAD_COUNT			= 500;
const DWORD	CTcpServer::MIN_SOCKET_BUFFER_SIZE			= 64;
const DWORD	CTcpServer::DEFAULT_WORKER_THREAD_COUNT		= min((::GetCpuCount() * 2 + 2), MAX_WORKER_THREAD_COUNT);
const DWORD	CTcpServer::DEFAULT_ACCEPT_SOCKET_COUNT		= 1 * DEFAULT_WORKER_THREAD_COUNT;
const DWORD	CTcpServer::DEFAULT_SOCKET_BUFFER_SIZE		= 4 * 1024 - sizeof(TBufferObj);
const DWORD	CTcpServer::DEFAULT_SOCKET_LISTEN_QUEUE		= 30;
const DWORD CTcpServer::DEFAULT_FREE_SOCKETOBJ_LOCK_TIME= 3 * 1000;
const DWORD	CTcpServer::DEFAULT_FREE_SOCKETOBJ_POOL		= 100;
const DWORD	CTcpServer::DEFAULT_FREE_BUFFEROBJ_POOL		= 200;
const DWORD	CTcpServer::DEFAULT_FREE_SOCKETOBJ_HOLD		= 300;
const DWORD	CTcpServer::DEFAULT_FREE_BUFFEROBJ_HOLD		= 600;
const DWORD	CTcpServer::DEFALUT_KEEPALIVE_TIME			= 5 * 1000;
const DWORD	CTcpServer::DEFALUT_KEEPALIVE_INTERVAL		= 3 * 1000;
const DWORD	CTcpServer::DEFAULT_MAX_SHUTDOWN_WAIT_TIME	= 15 * 1000;

void CTcpServer::SetLastError(EnServerError code, LPCSTR func, int ec)
{
	m_enLastError = code;

	TRACE3("%s --> Error: %d, EC: %d\n", func, code, ec);
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
			if(FirePrepareListen(m_soListen) != ISocketListener::HR_ERROR)
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
			::PostQueuedCompletionStatus(m_hCompletePort, IOCP_SI_ACCEPT, 0, nullptr);
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

	m_dwConnID					= 0;
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
	CReentrantWriteLock locallock(m_csClientSocket);

	for(TSocketObjPtrMapI it = m_mpClientSocket.begin(); it != m_mpClientSocket.end(); ++it)
		Disconnect(it->first);
}

void CTcpServer::ReleaseClientSocket()
{
	CReentrantWriteLock locallock(m_csClientSocket);

	for(TSocketObjPtrMapI it = m_mpClientSocket.begin(); it != m_mpClientSocket.end(); ++it)
	{
		TSocketObj* pSocketObj = it->second;

		CloseSocketObj(pSocketObj);
		DeleteSocketObj(pSocketObj);
	}

	m_mpClientSocket.clear();
}

TSocketObj*	CTcpServer::GetFreeSocketObj()
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

	if(!pSocketObj) pSocketObj	= CreateSocketObj();
	pSocketObj->extra			= nullptr;

	return pSocketObj;
}

void CTcpServer::AddFreeSocketObj(CONNID dwConnID)
{
	BOOL bDone = FALSE;
	TSocketObj* pSocketObj = nullptr;

	{
		CReentrantWriteLock locallock(m_csClientSocket);
		pSocketObj = FindSocketObj(dwConnID);

		if(pSocketObj != nullptr)
		{
			m_mpClientSocket.erase(dwConnID);
			bDone = TRUE;
		}
	}

	if(bDone)
	{
		CloseSocketObj(pSocketObj);

		{
			CCriSecLock locallock(m_csFreeSocket);

			pSocketObj->freeTime = ::TimeGetTime();
			m_lsFreeSocket.push_back(pSocketObj);
		}

		if(m_lsFreeSocket.size() > m_dwFreeSocketObjHold)
			CompressFreeSocket(m_dwFreeSocketObjPool);
	}
}

void CTcpServer::AddClientSocketObj(CONNID dwConnID, TSocketObj* pSocketObj)
{
	CReentrantWriteLock locallock(m_csClientSocket);

	ASSERT(FindSocketObj(dwConnID) == nullptr);

	pSocketObj->connTime		= ::TimeGetTime();
	m_mpClientSocket[dwConnID]  = pSocketObj;
}

void CTcpServer::ReleaseFreeSocket()
{
	CompressFreeSocket(0, TRUE);
}

void CTcpServer::CompressFreeSocket(size_t size, BOOL bForce)
{
	CCriSecLock locallock(m_csFreeSocket);

	while(m_lsFreeSocket.size() > size)
	{
		TSocketObj* pSocketObj = m_lsFreeSocket.front();

		if(bForce || ::GetTimeGap32(pSocketObj->freeTime) >= m_dwFreeSocketObjLockTime)
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

	pSocketObj->TSocketObj::TSocketObj();
	
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
	{
		CCriSecLock locallock(m_csFreeBuffer);
		m_lsFreeBuffer.push_back(pBufferObj);
	}

	if(m_lsFreeBuffer.size() > m_dwFreeBufferObjHold)
		CompressFreeBuffer(m_dwFreeBufferObjPool);
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

void CTcpServer::CloseSocketObj(TSocketObj* pSocketObj, int iShutdownFlag)
{
	if(TSocketObj::IsValid(pSocketObj))
	{
		CCriSecLock locallock(pSocketObj->crisec);

		if(pSocketObj->socket != INVALID_SOCKET)
		{
			::ManualCloseSocket(pSocketObj->socket, iShutdownFlag);
			pSocketObj->socket = INVALID_SOCKET;
		}
	}
}

BOOL CTcpServer::GetListenAddress(LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	return ::GetSocketLocalAddress(m_soListen, lpszAddress, iAddressLen, usPort);
}

BOOL CTcpServer::GetClientAddress(CONNID dwConnID, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
	{
		ADDRESS_FAMILY usFamily;
		return ::sockaddr_IN_2_A(pSocketObj->clientAddr, usFamily, lpszAddress, iAddressLen, usPort);
	}

	return FALSE;
}

BOOL CTcpServer::SetConnectionExtra(CONNID dwConnID, PVOID pExtra)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
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

	if(TSocketObj::IsValid(pSocketObj))
	{
		*ppExtra = pSocketObj->extra;
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

	if(TSocketObj::IsValid(pSocketObj))
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
			isOK = ::PostQueuedCompletionStatus(m_hCompletePort, IOCP_SI_DISCONNECT, dwConnID, nullptr);
		else
			isOK = m_pfnDisconnectEx(pSocketObj->socket, nullptr, 0, 0);
	}

	return isOK;
}

BOOL CTcpServer::DisconnectLongConnections(DWORD dwPeriod, BOOL bForce)
{
	ulong_ptr_list ls;

	{
		CReentrantReadLock locallock(m_csClientSocket);

		size_t size = m_mpClientSocket.size();

		if(size > 0)
		{
			for(TSocketObjPtrMapCI it = m_mpClientSocket.begin(); it != m_mpClientSocket.end(); ++it)
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

void CTcpServer::TerminateWorkerThread()
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

	while (TRUE)
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
			int indative = pServer->CheckSpecialIndative(pOverlapped, dwBytes, pSocketObj);

			if(indative == IOCP_SI_RS_CONTINUE)
				continue;
			else if(indative == IOCP_SI_RS_BREAK)
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

int CTcpServer::CheckSpecialIndative(OVERLAPPED* pOverlapped, DWORD dwBytes, TSocketObj* pSocketObj)
{
	int indative = 0;

	if(pOverlapped == nullptr)
	{
		if(dwBytes == IOCP_SI_ACCEPT)
		{
			DoAccept();
			indative = IOCP_SI_RS_CONTINUE;
		}
		else if(dwBytes == IOCP_SI_DISCONNECT)
		{
			ForceDisconnect((CONNID)pSocketObj);
			indative = IOCP_SI_RS_CONTINUE;
		}
		else if(dwBytes == IOCP_SI_EXIT && pSocketObj == nullptr)
			indative = IOCP_SI_RS_BREAK;
		else
			VERIFY(FALSE);
	}

	return indative;
}

void CTcpServer::ForceDisconnect(CONNID dwConnID)
{
	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(TSocketObj::IsValid(pSocketObj))
	{
		FireClose(dwConnID);
		AddFreeSocketObj(dwConnID);
	}
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
		FireClose(dwConnID);
		AddFreeSocketObj(dwConnID);
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
		::PostQueuedCompletionStatus(m_hCompletePort, IOCP_SI_ACCEPT, 0, nullptr);
	}

	AddFreeBufferObj(pBufferObj);
}

void CTcpServer::HandleAccept(SOCKET soListen, TBufferObj* pBufferObj)
{
	::PostQueuedCompletionStatus(m_hCompletePort, IOCP_SI_ACCEPT, 0, nullptr);

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
	CONNID dwConnID			= ::GenerateConnectionID(m_dwConnID);
	TSocketObj* pSocketObj	= GetFreeSocketObj();
	pSocketObj->socket		= socket;
	pSocketObj->connID		= dwConnID;

	memcpy(&pSocketObj->clientAddr, pRemoteSockAddr, sizeof(SOCKADDR_IN));
	AddClientSocketObj(dwConnID, pSocketObj);

	VERIFY(::SSO_UpdateAcceptContext(socket, soListen) == NO_ERROR);

	BOOL bOnOff	= (m_dwKeepAliveTime > 0 && m_dwKeepAliveInterval > 0);
	VERIFY(::SSO_KeepAliveVals(socket, bOnOff, m_dwKeepAliveTime, m_dwKeepAliveInterval) == NO_ERROR);

	if(FireAccept(dwConnID, socket) != ISocketListener::HR_ERROR)
	{
		VERIFY(::CreateIoCompletionPort((HANDLE)socket, m_hCompletePort, (ULONG_PTR)pSocketObj, 0));
		DoReceive(dwConnID, pSocketObj, pBufferObj);
	}
	else
	{
		AddFreeSocketObj(dwConnID);
		AddFreeBufferObj(pBufferObj);
	}
}

void CTcpServer::HandleSend(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	if(FireSend(dwConnID, (BYTE*)pBufferObj->buff.buf, pBufferObj->buff.len) == ISocketListener::HR_ERROR)
	{
		TRACE1("<S-CNNID: %Iu> OnSend() event should not return 'HR_ERROR' !!\n", dwConnID);
		ASSERT(FALSE);
	}

	AddFreeBufferObj(pBufferObj);
}

void CTcpServer::HandleReceive(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	if(FireReceive(dwConnID, (BYTE*)pBufferObj->buff.buf, pBufferObj->buff.len) != ISocketListener::HR_ERROR)
		DoReceive(dwConnID, pSocketObj, pBufferObj);
	else
	{
		TRACE1("<S-CNNID: %Iu> OnReceive() event return 'HR_ERROR', connection will be closed !\n", dwConnID);
		FireError(dwConnID, SO_RECEIVE, ERROR_FUNCTION_FAILED);
		AddFreeSocketObj(dwConnID);
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

BOOL CTcpServer::Send(CONNID dwConnID, const BYTE* pBuffer, int iLength)
{
	ASSERT(pBuffer && iLength > 0);

	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(pSocketObj == nullptr)
		return FALSE;

	int result = DoSend(dwConnID, pSocketObj, pBuffer, iLength);

	if(result != NO_ERROR)
		CheckError(dwConnID, SO_SEND, result);

	return (result == NO_ERROR);
}

int CTcpServer::DoSend(CONNID dwConnID, TSocketObj* pSocketObj, const BYTE* pBuffer, int iLen)
{
	int result	= NO_ERROR;
	int iRemain	= iLen;

	CCriSecLock locallock(pSocketObj->crisec);

	while(iRemain > 0)
	{
		int iBufferSize = min(iRemain, (int)m_dwSocketBufferSize);
		TBufferObj* pBufferObj = GetFreeBufferObj(iBufferSize);
		memcpy(pBufferObj->buff.buf, pBuffer, iBufferSize);

		result = ::PostSend(pSocketObj, pBufferObj);

		if(result != NO_ERROR)
		{
			AddFreeBufferObj(pBufferObj);
			break;
		}

		iRemain -= iBufferSize;
		pBuffer += iBufferSize;
	}

	return result;
}

void CTcpServer::CheckError(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	if(iErrorCode != WSAENOTSOCK && iErrorCode != ERROR_OPERATION_ABORTED)
	{
		FireError(dwConnID, enOperation, iErrorCode);
		AddFreeSocketObj(dwConnID);
	}
}

LPCTSTR CTcpServer::GetLastErrorDesc()
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
	case SE_SOCKE_ATTACH_TO_CP:		return _T("监听 SOCKET 绑定到完成端口失败");
	default: ASSERT(FALSE);			return _T("");
	}
}
