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
#include "../../Common/Src/GeneralHelper.h"
#include "../../Common/Src/SysHelper.h"
#include "SocketHelper.h"

#include <mstcpip.h>

#ifndef _WIN32_WCE
	#pragma comment(lib, "ws2_32")
#else
	#pragma comment(lib, "ws2")
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////

const DWORD	MAX_WORKER_THREAD_COUNT					= 500;
const DWORD	MIN_SOCKET_BUFFER_SIZE					= 64;
const DWORD MAX_SMALL_FILE_SIZE						= 4096 * 1024;
const DWORD	DEFAULT_WORKER_THREAD_COUNT				= min((::SysGetNumberOfProcessors() * 2 + 2), MAX_WORKER_THREAD_COUNT);
const DWORD DEFAULT_FREE_SOCKETOBJ_LOCK_TIME		= 10 * 1000;
const DWORD	DEFAULT_FREE_SOCKETOBJ_POOL				= 150;
const DWORD	DEFAULT_FREE_SOCKETOBJ_HOLD				= 450;
const DWORD	DEFAULT_FREE_BUFFEROBJ_POOL				= 300;
const DWORD	DEFAULT_FREE_BUFFEROBJ_HOLD				= 900;
const DWORD	DEFAULT_MAX_SHUTDOWN_WAIT_TIME			= 15 * 1000;
const DWORD DEFAULT_CLIENT_FREE_BUFFER_POOL_SIZE	= 10;
const DWORD DEFAULT_CLIENT_FREE_BUFFER_POOL_HOLD	= 30;
const DWORD	DEFAULT_TCP_SOCKET_BUFFER_SIZE			= ::SysGetPageSize();
const DWORD	DEFALUT_TCP_KEEPALIVE_TIME				= 10 * 1000;
const DWORD	DEFALUT_TCP_KEEPALIVE_INTERVAL			= 5 * 1000;
const DWORD	DEFAULT_TCP_SERVER_SOCKET_LISTEN_QUEUE	= SOMAXCONN;
const DWORD	DEFAULT_TCP_SERVER_ACCEPT_SOCKET_COUNT	= 300;
const DWORD	DEFAULT_UDP_MAX_DATAGRAM_SIZE			= 1472;
const DWORD	DEFAULT_UDP_POST_RECEIVE_COUNT			= 300;
const DWORD DEFAULT_UDP_DETECT_ATTEMPTS				= 5;
const DWORD DEFAULT_UDP_DETECT_INTERVAL				= 12;
LPCTSTR DEFAULT_AGENT_BIND_ADDRESS					= _T("0.0.0.0");

///////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL IsIPAddress(LPCTSTR lpszAddress)
{
	if(!lpszAddress || lpszAddress[0] == '\0')
		return FALSE;

	return ::inet_addr(CT2A(lpszAddress)) != INADDR_NONE;
}

BOOL GetIPAddress(LPCTSTR lpszHost, LPTSTR lpszIP, int& iIPLen)
{
	BOOL isOK = TRUE;

	if(IsIPAddress(lpszHost))
	{
		int iHostLen = lstrlen(lpszHost);
		
		if(iHostLen > 0)
			++iHostLen;

		if(iHostLen > 0 && iIPLen >= iHostLen)
			lstrcpy(lpszIP, lpszHost);
		else
			isOK = FALSE;

		iIPLen = iHostLen;
	}
	else
	{
		IN_ADDR addr;

		if(GetOptimalIPByHostName(lpszHost, addr))
			isOK = IN_ADDR_2_IP(addr, lpszIP, iIPLen);
		else
			isOK = FALSE;
	}

	return isOK;
}

BOOL GetOptimalIPByHostName(LPCTSTR lpszHost, IN_ADDR& addr)
{
	addr.s_addr		= 0;
	hostent* host	= ::gethostbyname(CT2A(lpszHost));

	if(host)
	{
		IN_ADDR inAddr;
		ULONG addrs[3]  = {0};
		char** pptr		= nullptr;

		if(host->h_addrtype == AF_INET)
		{
			for(pptr = host->h_addr_list; *pptr != nullptr; ++pptr)
			{
				inAddr.s_addr	= *(ULONG*)*pptr;
				UCHAR a			= inAddr.s_net;
				UCHAR b			= inAddr.s_host;

				if(addrs[0] == 0 && a == 127)
				{
					addrs[0] = inAddr.s_addr;
					break;
				}
				else if	(	addrs[1] == 0							&& 
							(
								(a == 10)							||
								(a == 172 && b >= 16 && b <= 31)	||
								(a == 192 && b == 168)
							)
						)
					addrs[1] = inAddr.s_addr;
				else if(addrs[2] == 0)
					addrs[2] = inAddr.s_addr;
			}

			for(int i = 0; i < 3; i++)
			{
				if(addrs[i] != 0)
				{
					addr.s_addr = addrs[i];
					break;
				}
			}
		}
	}

	return addr.s_addr != 0;
}

BOOL IN_ADDR_2_IP(const IN_ADDR& addr, LPTSTR lpszAddress, int& iAddressLen)
{
	BOOL isOK		= TRUE;
	char* lpszIP	= inet_ntoa(addr);
	int iIPLen		= (int)strlen(lpszIP);

	if(iIPLen > 0)
		++iIPLen;

	if(iIPLen > 0 && iAddressLen >= iIPLen)
		lstrcpy(lpszAddress, CA2T(lpszIP));
	else
		isOK = FALSE;

	iAddressLen = iIPLen;

	return isOK;
}

BOOL sockaddr_IN_2_A(const SOCKADDR_IN& addr, ADDRESS_FAMILY& usFamily, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort)
{
	usFamily = addr.sin_family;
	usPort	 = ntohs(addr.sin_port);

	return IN_ADDR_2_IP(addr.sin_addr, lpszAddress, iAddressLen);
}

BOOL sockaddr_A_2_IN(ADDRESS_FAMILY usFamily, LPCTSTR pszAddress, USHORT usPort, SOCKADDR_IN& addr)
{
	addr.sin_family			= usFamily;
	addr.sin_port			= htons(usPort);
	addr.sin_addr.s_addr	= inet_addr(CT2A(pszAddress));

	return addr.sin_addr.s_addr != INADDR_NONE;
}

BOOL GetSocketAddress(SOCKET socket, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort, BOOL bLocal)
{
	sockaddr addr;

	int addr_len = sizeof(addr);
	int result	 = bLocal ? getsockname(socket, &addr, &addr_len) : getpeername(socket, &addr, &addr_len);

	if(result == NO_ERROR)
	{
		ADDRESS_FAMILY usFamily;
		return sockaddr_IN_2_A((sockaddr_in&)addr, usFamily, lpszAddress, iAddressLen, usPort);
	}

	return FALSE;
}

BOOL GetSocketLocalAddress(SOCKET socket, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort)
{
	return GetSocketAddress(socket, lpszAddress, iAddressLen, usPort, TRUE);
}

BOOL GetSocketRemoteAddress(SOCKET socket, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort)
{
	return GetSocketAddress(socket, lpszAddress, iAddressLen, usPort, FALSE);
}

PVOID GetExtensionFuncPtr(SOCKET sock, GUID guid)
{
	DWORD dwBytes;
	PVOID pfn = nullptr;

	::WSAIoctl	(
					sock,
					SIO_GET_EXTENSION_FUNCTION_POINTER,
					&guid,
					sizeof(guid),
					&pfn,
					sizeof(pfn),
					&dwBytes,
					nullptr,
					nullptr
				);

	return pfn;
}

LPFN_ACCEPTEX Get_AcceptEx_FuncPtr(SOCKET sock)
{
	GUID guid = WSAID_ACCEPTEX;
	return (LPFN_ACCEPTEX)GetExtensionFuncPtr(sock, guid);
}

LPFN_GETACCEPTEXSOCKADDRS Get_GetAcceptExSockaddrs_FuncPtr(SOCKET sock)
{
	GUID guid = WSAID_GETACCEPTEXSOCKADDRS;
	return (LPFN_GETACCEPTEXSOCKADDRS)GetExtensionFuncPtr(sock, guid);
}

LPFN_CONNECTEX Get_ConnectEx_FuncPtr(SOCKET sock)
{
	GUID guid = WSAID_CONNECTEX;
	return (LPFN_CONNECTEX)GetExtensionFuncPtr(sock, guid);
}

LPFN_TRANSMITFILE Get_TransmitFile_FuncPtr(SOCKET sock)
{
	GUID guid = WSAID_TRANSMITFILE;
	return (LPFN_TRANSMITFILE)GetExtensionFuncPtr(sock, guid);
}

LPFN_DISCONNECTEX Get_DisconnectEx_FuncPtr	(SOCKET sock)
{
	GUID guid = WSAID_DISCONNECTEX;
	return (LPFN_DISCONNECTEX)GetExtensionFuncPtr(sock, guid);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL PostIocpCommand(HANDLE hIOCP, EnIocpCommand enCmd, ULONG_PTR ulParam)
{
	return ::PostQueuedCompletionStatus(hIOCP, enCmd, ulParam, nullptr);
}

BOOL PostIocpExit(HANDLE hIOCP)
{
	return ::PostQueuedCompletionStatus(hIOCP, IOCP_CMD_EXIT, 0, nullptr);
}

BOOL PostIocpAccept(HANDLE hIOCP)
{
	return ::PostQueuedCompletionStatus(hIOCP, IOCP_CMD_ACCEPT, 0, nullptr);
}

BOOL PostIocpDisconnect(HANDLE hIOCP, CONNID dwConnID)
{
	return ::PostQueuedCompletionStatus(hIOCP, IOCP_CMD_DISCONNECT, dwConnID, nullptr);
}

BOOL PostIocpSend(HANDLE hIOCP, CONNID dwConnID)
{
	return ::PostQueuedCompletionStatus(hIOCP, IOCP_CMD_SEND, dwConnID, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

int SSO_SetSocketOption(SOCKET sock, int level, int name, LPVOID val, int len)
{
	return setsockopt(sock, level, name, (CHAR*)val, len);
}

int SSO_GetSocketOption(SOCKET sock, int level, int name, LPVOID val, int* len)
{
	return getsockopt(sock, level, name, (CHAR*)val, len);
}

int SSO_IoctlSocket(SOCKET sock, long cmd, u_long* arg)
{
	return ioctlsocket(sock, cmd, arg);
}

int SSO_WSAIoctl(SOCKET sock, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer, LPDWORD lpcbBytesReturned)
{
	return ::WSAIoctl(sock, dwIoControlCode, lpvInBuffer, cbInBuffer, lpvOutBuffer, cbOutBuffer, lpcbBytesReturned, nullptr, nullptr);
}

int SSO_UpdateAcceptContext(SOCKET soClient, SOCKET soBind)
{
	return setsockopt(soClient, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (CHAR*)&soBind, sizeof(SOCKET));
}

int SSO_UpdateConnectContext(SOCKET soClient, int iOption)
{
	return setsockopt(soClient, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, (CHAR*)&iOption, sizeof(int));
}

int SSO_NoDelay(SOCKET sock, BOOL bNoDelay)
{
	return setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (CHAR*)&bNoDelay, sizeof(BOOL));
}

int SSO_DontLinger(SOCKET sock, BOOL bDont)
{
	return setsockopt(sock, SOL_SOCKET, SO_DONTLINGER, (CHAR*)&bDont, sizeof(BOOL));
}

int SSO_Linger(SOCKET sock, USHORT l_onoff, USHORT l_linger)
{
	linger ln = {l_onoff, l_linger};
	return setsockopt(sock, SOL_SOCKET, SO_LINGER, (CHAR*)&ln, sizeof(linger));
}

int SSO_KeepAlive(SOCKET sock, BOOL bKeepAlive)
{
	return setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (CHAR*)&bKeepAlive, sizeof(BOOL));
}

int SSO_KeepAliveVals(SOCKET sock, u_long onoff, u_long time, u_long interval)
{
	int result		 = NO_ERROR;
	tcp_keepalive in = {onoff, time, interval};
	DWORD dwBytes;

	if(::WSAIoctl	(
						sock, 
						SIO_KEEPALIVE_VALS, 
						(LPVOID)&in, 
						sizeof(in), 
						nullptr, 
						0, 
						&dwBytes, 
						nullptr, 
						nullptr
					) == SOCKET_ERROR)
	{
		result = ::WSAGetLastError();
		if(result == WSAEWOULDBLOCK)
			result = NO_ERROR;
	}

	return result;
}

int SSO_RecvBuffSize(SOCKET sock, int size)
{
	return setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (CHAR*)&size, sizeof(int));
}

int SSO_SendBuffSize(SOCKET sock, int size)
{
	return setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (CHAR*)&size, sizeof(int));
}

int SSO_ReuseAddress(SOCKET sock, BOOL bReuse)
{
	return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&bReuse, sizeof(BOOL));
}

int SSO_UDP_ConnReset(SOCKET sock, BOOL bNewBehavior)
{
	int result = NO_ERROR;
	DWORD dwBytes;

	if(::WSAIoctl	(
						sock, 
						SIO_UDP_CONNRESET, 
						(LPVOID)&bNewBehavior, 
						sizeof(bNewBehavior), 
						nullptr, 
						0, 
						&dwBytes, 
						nullptr, 
						nullptr
					) == SOCKET_ERROR)
	{
		result = ::WSAGetLastError();
		if(result == WSAEWOULDBLOCK)
			result = NO_ERROR;
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

CONNID GenerateConnectionID()
{
	static volatile CONNID s_dwConnID = 0;

	CONNID dwConnID	= ::InterlockedIncrement(&s_dwConnID);
	
	if(dwConnID == 0)
		dwConnID = ::InterlockedIncrement(&s_dwConnID);

	return dwConnID;
}

int ManualCloseSocket(SOCKET sock, int iShutdownFlag, BOOL bGraceful, BOOL bReuseAddress)
{
	if(!bGraceful)
		SSO_Linger(sock, 1, 0);

	if(bReuseAddress)
		SSO_ReuseAddress(sock, bReuseAddress);

	if(iShutdownFlag != 0xFF)
		shutdown(sock, iShutdownFlag);

	return closesocket(sock);
}

int PostAccept(LPFN_ACCEPTEX pfnAcceptEx, SOCKET soListen, SOCKET soClient, TBufferObj* pBufferObj)
{
	int result = PostAcceptNotCheck(pfnAcceptEx, soListen, soClient, pBufferObj);

	if(result == WSA_IO_PENDING)
		result = NO_ERROR;

	return result;
}

int PostAcceptNotCheck(LPFN_ACCEPTEX pfnAcceptEx, SOCKET soListen, SOCKET soClient, TBufferObj* pBufferObj)
{
	int result				= NO_ERROR;
	pBufferObj->client		= soClient;
	pBufferObj->operation	= SO_ACCEPT;

	if(!pfnAcceptEx	(
						soListen,
						pBufferObj->client,
						pBufferObj->buff.buf,
						0,
						sizeof(SOCKADDR_IN) + 16,
						sizeof(SOCKADDR_IN) + 16,
						nullptr,
						&pBufferObj->ov
					)
	)
	{
		result = ::WSAGetLastError();
	}

	return result;
}

int PostConnect(LPFN_CONNECTEX pfnConnectEx, SOCKET soClient, SOCKADDR_IN& soAddrIN, TBufferObj* pBufferObj)
{
	int result = PostConnectNotCheck(pfnConnectEx, soClient, soAddrIN, pBufferObj);

	if(result == WSA_IO_PENDING)
		result = NO_ERROR;

	return result;
}

int PostConnectNotCheck(LPFN_CONNECTEX pfnConnectEx, SOCKET soClient, SOCKADDR_IN& soAddrIN, TBufferObj* pBufferObj)
{
	int result				= NO_ERROR;
	pBufferObj->client		= soClient;
	pBufferObj->operation	= SO_CONNECT;

	if(!pfnConnectEx	(
							soClient,
							(SOCKADDR*)&soAddrIN,
							sizeof(SOCKADDR_IN),
							nullptr,
							0,
							nullptr,
							&pBufferObj->ov
						)
	)
	{
		result = ::WSAGetLastError();
	}

	return result;
}

int PostSend(TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	int result = PostSendNotCheck(pSocketObj, pBufferObj);

	if(result == WSA_IO_PENDING)
		result = NO_ERROR;

	return result;
}

int PostSendNotCheck(TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	int result				= NO_ERROR;
	DWORD dwBytes			= 0;
	pBufferObj->client		= pSocketObj->socket;
	pBufferObj->operation	= SO_SEND;

	if(::WSASend(
					pBufferObj->client,
					&pBufferObj->buff,
					1,
					&dwBytes,
					0,
					&pBufferObj->ov,
					nullptr
				) == SOCKET_ERROR)
	{
		result = ::WSAGetLastError();
	}

	return result;
}

int PostReceive(TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	int result = PostReceiveNotCheck(pSocketObj, pBufferObj);

	if(result == WSA_IO_PENDING)
		result = NO_ERROR;

	return result;
}

int PostReceiveNotCheck(TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	int result				= NO_ERROR;
	DWORD dwFlag			= 0; 
	DWORD dwBytes			= 0; 
	pBufferObj->client		= pSocketObj->socket;
	pBufferObj->operation	= SO_RECEIVE;

	if(::WSARecv(
					pBufferObj->client,
					&pBufferObj->buff,
					1,
					&dwBytes,
					&dwFlag,
					&pBufferObj->ov,
					nullptr
				) == SOCKET_ERROR)
	{
		result = ::WSAGetLastError();
	}

	return result;
}

int PostSendTo(SOCKET sock, TUdpBufferObj* pBufferObj)
{
	int result = PostSendToNotCheck(sock, pBufferObj);

	if(result == WSA_IO_PENDING)
		result = NO_ERROR;

	return result;
}

int PostSendToNotCheck(SOCKET sock, TUdpBufferObj* pBufferObj)
{
	int result				= NO_ERROR;
	DWORD dwBytes			= 0;
	pBufferObj->operation	= SO_SEND;
	pBufferObj->addrLen		= sizeof(SOCKADDR_IN);

	if(::WSASendTo	(
						sock,
						&pBufferObj->buff,
						1,
						&dwBytes,
						0,
						(sockaddr*)&pBufferObj->remoteAddr,
						pBufferObj->addrLen,
						&pBufferObj->ov,
						nullptr
					) == SOCKET_ERROR)
	{
		result = ::WSAGetLastError();
	}

	return result;
}

int PostReceiveFrom(SOCKET sock, TUdpBufferObj* pBufferObj)
{
	int result = PostReceiveFromNotCheck(sock, pBufferObj);

	if(result == WSA_IO_PENDING)
		result = NO_ERROR;

	return result;
}

int PostReceiveFromNotCheck(SOCKET sock, TUdpBufferObj* pBufferObj)
{
	int result				= NO_ERROR;
	DWORD dwFlag			= 0;
	DWORD dwBytes			= 0;
	pBufferObj->operation	= SO_RECEIVE;
	pBufferObj->addrLen		= sizeof(SOCKADDR_IN);

	::ZeroMemory(&pBufferObj->remoteAddr, pBufferObj->addrLen);

	if(::WSARecvFrom(
						sock,
						&pBufferObj->buff,
						1,
						&dwBytes,
						&dwFlag,
						(sockaddr*)&pBufferObj->remoteAddr,
						&pBufferObj->addrLen,
						&pBufferObj->ov,
						nullptr
					) == SOCKET_ERROR)
	{
		result = ::WSAGetLastError();
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

LPCTSTR GetSocketErrorDesc(EnSocketError enCode)
{
	switch(enCode)
	{
	case SE_OK:						return _T("SUCCESS");
	case SE_ILLEGAL_STATE:			return _T("Illegal State");
	case SE_INVALID_PARAM:			return _T("Invalid Parameter");
	case SE_SOCKET_CREATE:			return _T("Create SOCKET Fail");
	case SE_SOCKET_BIND:			return _T("Bind SOCKET Fail");
	case SE_SOCKET_PREPARE:			return _T("Prepare SOCKET Fail");
	case SE_SOCKET_LISTEN:			return _T("Listen SOCKET Fail");
	case SE_CP_CREATE:				return _T("Create IOCP Fail");
	case SE_WORKER_THREAD_CREATE:	return _T("Create Worker Thread Fail");
	case SE_DETECT_THREAD_CREATE:	return _T("Create Detector Thread Fail");
	case SE_SOCKE_ATTACH_TO_CP:		return _T("Attach SOCKET to IOCP Fail");
	case SE_CONNECT_SERVER:			return _T("Connect to Server Fail");
	case SE_NETWORK:				return _T("Network Error");
	case SE_DATA_PROC:				return _T("Process Data Error");
	case SE_DATA_SEND:				return _T("Send Data Error");
	default: ASSERT(FALSE);			return _T("UNKNOWN ERROR");
	}
}
