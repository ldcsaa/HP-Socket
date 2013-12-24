/*
 * Copyright Bruce Liang (ldcsaa@gmail.com)
 *
 * Version	: 3.0.1
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
#include "../GeneralHelper.h"
#include "SocketHelper.h"

#include <mstcpip.h>

#ifndef _WIN32_WCE
	#pragma comment(lib, "ws2_32")
#else
	#pragma comment(lib, "ws2")
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL IsIPAddress(LPCTSTR lpszAddress)
{
	if(!lpszAddress)
		return FALSE;

	int len = (int)lstrlen(lpszAddress) + 1;
	
	if(len < 7 || len > 15)
		return false;

	int iDotIndex = 0;
	int iDotCount = 0;

	for(int i = 0; i < len; ++i)
	{
		TCHAR c = lpszAddress[i];
		if((c < '0' || c > '9') && c != '.' && c != '\0')
			return FALSE;
		else if(c == '.' || c == '\0')
		{
			int iSubLen = i - iDotIndex;

			if(iSubLen < 1 || iSubLen > 3)
				return FALSE;

			TCHAR szSub[4] = {0};
			_tcsncpy(szSub, lpszAddress + iDotIndex, iSubLen);

			int iSub = _ttoi(szSub);

			if(iSub < 0 || iSub > 255)
				return FALSE;

			if(c == '.')
			{
				iDotIndex = i + 1;
				if(++iDotCount > 3)
					return FALSE;
			}
		}
	}

	return (iDotCount == 3);
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
		hostent* host = ::gethostbyname(CT2A(lpszHost));

		if(host)
			isOK = sockaddr_IN_2_IP(*(SOCKADDR_IN*)(*host->h_addr_list), lpszIP, iIPLen);
		else
			isOK = FALSE;
	}

	return isOK;
}

BOOL sockaddr_IN_2_IP(const SOCKADDR_IN& addr, LPTSTR lpszAddress, int& iAddressLen)
{
	BOOL isOK		= TRUE;
	char* lpszIP	= inet_ntoa(addr.sin_addr);
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

	return sockaddr_IN_2_IP(addr, lpszAddress, iAddressLen);
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

CONNID GenerateConnectionID(volatile CONNID& dwSeed)
{
	CONNID dwConnID	= ::InterlockedIncrement(&dwSeed);
	
	if(dwConnID == 0)
		dwConnID = ::InterlockedIncrement(&dwSeed);

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
		if(result == WSA_IO_PENDING)
			result = NO_ERROR;
	}

	return result;
}

int PostSend(TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	int result				= NO_ERROR;
	pBufferObj->operation	= SO_SEND;
	DWORD dwBytes;

	if(::WSASend(
					pSocketObj->socket,
					&pBufferObj->buff,
					1,
					&dwBytes,
					0,
					&pBufferObj->ov,
					nullptr
				) == SOCKET_ERROR)
	{
		result = ::WSAGetLastError();
		if(result == WSA_IO_PENDING)
			result = NO_ERROR;
	}

	return result;
}

int PostReceive(TSocketObj* pSocketObj, TBufferObj* pBufferObj)
{
	int result				= NO_ERROR;
	pBufferObj->operation	= SO_RECEIVE;
	DWORD dwBytes, dwFlag	= 0; 

	if(::WSARecv(
					pSocketObj->socket,
					&pBufferObj->buff,
					1,
					&dwBytes,
					&dwFlag,
					&pBufferObj->ov,
					nullptr
				) == SOCKET_ERROR)
	{
		result = ::WSAGetLastError();
		if(result == WSA_IO_PENDING)
			result = NO_ERROR;
	}

	return result;
}

int PostSendTo(SOCKET sock, TUdpBufferObj* pBufferObj)
{
	int result				= NO_ERROR;
	pBufferObj->operation	= SO_SEND;
	DWORD dwBytes;

	if(::WSASendTo	(
						sock,
						&pBufferObj->buff,
						1,
						&dwBytes,
						0,
						(sockaddr*)&pBufferObj->clientAddr,
						pBufferObj->addrLen,
						&pBufferObj->ov,
						nullptr
					) == SOCKET_ERROR)
	{
		result = ::WSAGetLastError();
		if(result == WSA_IO_PENDING)
			result = NO_ERROR;
	}

	return result;
}

int PostReceiveFrom(SOCKET sock, TUdpBufferObj* pBufferObj)
{
	int result				= NO_ERROR;
	pBufferObj->addrLen		= sizeof(SOCKADDR_IN);
	pBufferObj->operation	= SO_RECEIVE;
	DWORD dwBytes, dwFlag	= 0;

	::ZeroMemory(&pBufferObj->clientAddr, pBufferObj->addrLen);

	if(::WSARecvFrom(
						sock,
						&pBufferObj->buff,
						1,
						&dwBytes,
						&dwFlag,
						(sockaddr*)&pBufferObj->clientAddr,
						&pBufferObj->addrLen,
						&pBufferObj->ov,
						nullptr
					) == SOCKET_ERROR)
	{
		result = ::WSAGetLastError();
		if(result == WSA_IO_PENDING)
			result = NO_ERROR;
	}

	return result;
}
