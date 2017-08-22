/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 5.0.1
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
#include "../../Common/Src/GeneralHelper.h"
#include "../../Common/Src/SysHelper.h"
#include "SocketHelper.h"

#include <mstcpip.h>
#pragma comment(lib, "ws2_32")

#if !defined(stscanf_s)
	#ifdef _UNICODE
		#define stscanf_s	swscanf_s
	#else
		#define stscanf_s	sscanf_s
	#endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD GetDefaultWorkerThreadCount()
{
	static DWORD s_dwtc = min((::SysGetNumberOfProcessors() * 2 + 2), MAX_WORKER_THREAD_COUNT);
	return s_dwtc;
}

DWORD GetDefaultTcpSocketBufferSize()
{
	static DWORD s_dtsbs = ::SysGetPageSize();
	return s_dtsbs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

ADDRESS_FAMILY DetermineAddrFamily(LPCTSTR lpszAddress)
{
	if (!lpszAddress || lpszAddress[0] == 0)
		return AF_UNSPEC;

	if(::StrChr(lpszAddress, IPV6_ADDR_SEPARATOR_CHAR))
		return AF_INET6;

	TCHAR c;
	int arr[4];

	if(stscanf_s(lpszAddress, _T("%d.%d.%d.%d%c"), &arr[0], &arr[1], &arr[2], &arr[3], &c, 1) != 4)
		return AF_UNSPEC;

	for(int i = 0; i < 4; i++)
	{
		if(arr[i] < 0 || arr[i] > 255)
			return AF_UNSPEC;
	}

	return AF_INET;
}

BOOL GetInAddr(LPCTSTR lpszAddress, HP_ADDR& addr)
{
	addr.family = DetermineAddrFamily(lpszAddress);

	if (addr.family == AF_UNSPEC)
		return FALSE;

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	return (::InetPton(addr.family, lpszAddress, addr.Addr()) == TRUE);
#else
	HP_SOCKADDR sockAddr(addr.family);

	if(!GetSockAddr(lpszAddress, 0, sockAddr))
		return FALSE;

	memcpy(addr.Addr(), sockAddr.SinAddr(), addr.AddrSize());

	return TRUE;
#endif
}

BOOL GetSockAddr(LPCTSTR lpszAddress, USHORT usPort, HP_SOCKADDR& addr)
{
	if(addr.family != AF_INET && addr.family != AF_INET6)
	{
		::WSASetLastError(WSAEADDRNOTAVAIL);
		return FALSE;
	}

	int iSize = addr.AddrSize();

	if(::WSAStringToAddress((LPTSTR)lpszAddress, addr.family, nullptr, addr.Addr(), &iSize) != NO_ERROR)
		return FALSE;

	if(usPort != 0)
		addr.SetPort(usPort);

	return TRUE;
}

BOOL IsIPAddress(LPCTSTR lpszAddress, EnIPAddrType* penType)
{
	HP_ADDR addr;

	BOOL isOK = GetInAddr(lpszAddress, addr);

	if(isOK && penType)
		*penType = addr.IsIPv4() ? IPT_IPV4 : IPT_IPV6;

	return isOK;
}

BOOL GetIPAddress(LPCTSTR lpszHost, LPTSTR lpszIP, int& iIPLen, EnIPAddrType& enType)
{
	BOOL isOK = TRUE;

	HP_SOCKADDR addr;

	if(!GetSockAddrByHostName(lpszHost, 0, addr))
		return FALSE;

	enType = addr.IsIPv4() ? IPT_IPV4 : IPT_IPV6;

	USHORT usPort;
	ADDRESS_FAMILY usFamily;
	return sockaddr_IN_2_A(addr, usFamily, lpszIP, iIPLen, usPort);
}

BOOL GetSockAddrByHostName(LPCTSTR lpszHost, USHORT usPort, HP_SOCKADDR& addr)
{
	addr.family = DetermineAddrFamily(lpszHost);

	if(addr.family != AF_UNSPEC)
		return GetSockAddr(lpszHost, usPort, addr);

	return GetSockAddrByHostNameDirectly(lpszHost, usPort, addr);

}

BOOL GetSockAddrByHostNameDirectly(LPCTSTR lpszHost, USHORT usPort, HP_SOCKADDR& addr)
{
	addr.ZeroAddr();

	addrinfo* pInfo	= nullptr;
	addrinfo hints	= {0};

	hints.ai_flags	= AI_ALL;
	hints.ai_family	= addr.family;

	int rs = ::getaddrinfo(CT2A(lpszHost), nullptr, &hints, &pInfo);

	if(rs != NO_ERROR)
	{
		::WSASetLastError(rs);
		return FALSE;
	}

	BOOL isOK = FALSE;

	for(addrinfo* pCur = pInfo; pCur != nullptr; pCur = pCur->ai_next)
	{
		if(pCur->ai_family == AF_INET || pCur->ai_family == AF_INET6)
		{
			memcpy(addr.Addr(), pCur->ai_addr, pCur->ai_addrlen);
			isOK = TRUE;

			break;
		}
	}

	::freeaddrinfo(pInfo);

	if(isOK)
		addr.SetPort(usPort);
	else
		::WSASetLastError(WSAHOST_NOT_FOUND);

	return isOK;
}

BOOL EnumHostIPAddresses(LPCTSTR lpszHost, EnIPAddrType enType, LPTIPAddr** lpppIPAddr, int& iIPAddrCount)
{
	*lpppIPAddr	 = nullptr;
	iIPAddrCount = 0;

	ADDRESS_FAMILY usFamily =	(enType == IPT_ALL					?
								AF_UNSPEC	: (enType == IPT_IPV4	?
								AF_INET		: (enType == IPT_IPV6	?
								AF_INET6	: 0xFF)));
	
	if(usFamily == 0xFF)
	{
		::WSASetLastError(WSAEAFNOSUPPORT);
		return FALSE;
	}

	vector<HP_PSOCKADDR> vt;

	ADDRESS_FAMILY usFamily2 = DetermineAddrFamily(lpszHost);

	if(usFamily2 != AF_UNSPEC)
	{
		if(usFamily != AF_UNSPEC && usFamily != usFamily2)
		{
			::WSASetLastError(WSAHOST_NOT_FOUND);
			return FALSE;
		}

		HP_SOCKADDR addr(usFamily2);

		if(!GetSockAddr(lpszHost, 0, addr))
			return FALSE;

		vt.emplace_back(&addr);

		return RetrieveSockAddrIPAddresses(vt, lpppIPAddr, iIPAddrCount);
	}

	addrinfo* pInfo	= nullptr;
	addrinfo hints	= {0};

	hints.ai_flags	= AI_ALL;
	hints.ai_family	= usFamily;

	int rs = ::getaddrinfo(CT2A(lpszHost), nullptr, &hints, &pInfo);

	if(rs != NO_ERROR)
	{
		::WSASetLastError(rs);
		return FALSE;
	}

	for(addrinfo* pCur = pInfo; pCur != nullptr; pCur = pCur->ai_next)
	{
		if(pCur->ai_family == AF_INET || pCur->ai_family == AF_INET6)
			vt.emplace_back((HP_PSOCKADDR)pCur->ai_addr);
	}

	BOOL isOK = RetrieveSockAddrIPAddresses(vt, lpppIPAddr, iIPAddrCount);

	::freeaddrinfo(pInfo);

	if(!isOK) ::WSASetLastError(WSAHOST_NOT_FOUND);

	return isOK;
}

BOOL RetrieveSockAddrIPAddresses(const vector<HP_PSOCKADDR>& vt, LPTIPAddr** lpppIPAddr, int& iIPAddrCount)
{
	iIPAddrCount = (int)vt.size();

	if(iIPAddrCount == 0) return FALSE;

	HP_PSOCKADDR	pSockAddr;
	ADDRESS_FAMILY	usFamily;
	USHORT			usPort;
	int				iAddrLength;
	LPTSTR			lpszAddr;
	LPTIPAddr		lpItem;

	(*lpppIPAddr) = new LPTIPAddr[iIPAddrCount + 1];
	(*lpppIPAddr)[iIPAddrCount] = nullptr;

	for(int i = 0; i < iIPAddrCount; i++)
	{
		pSockAddr	= vt[i];
		iAddrLength	= HP_SOCKADDR::AddrMinStrLength(pSockAddr->family) + 6;
		lpszAddr	= new TCHAR[iAddrLength];

		VERIFY(sockaddr_IN_2_A(*vt[i], usFamily, lpszAddr, iAddrLength, usPort));

		lpItem			= new TIPAddr;
		lpItem->type	= pSockAddr->IsIPv4() ? IPT_IPV4 : IPT_IPV6;
		lpItem->address	= lpszAddr;

		(*lpppIPAddr)[i] = lpItem;
	}

	return TRUE;
}

BOOL FreeHostIPAddresses(LPTIPAddr* lppIPAddr)
{
	if(!lppIPAddr) return FALSE;

	LPTIPAddr p;
	LPTIPAddr* lppCur = lppIPAddr;

	while((p = *lppCur++) != nullptr)
	{
		delete[] p->address;
		delete p;
	}

	delete[] lppIPAddr;

	return TRUE;
}

BOOL sockaddr_IN_2_A(const HP_SOCKADDR& addr, ADDRESS_FAMILY& usFamily, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort)
{
	BOOL isOK	= FALSE;

	usFamily	= addr.family;
	usPort		= addr.Port();

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	if(::InetNtop(addr.family, addr.SinAddr(), lpszAddress, iAddressLen))
	{
		iAddressLen	= lstrlen(lpszAddress) + 1;
		isOK		= TRUE;
	}
	else
	{
		if(::WSAGetLastError() == ERROR_INVALID_PARAMETER)
			iAddressLen = HP_SOCKADDR::AddrMinStrLength(usFamily);
	}
#else
	if(::WSAAddressToString((LPSOCKADDR)addr.Addr(), addr.AddrSize(), nullptr, lpszAddress, (LPDWORD)&iAddressLen) == NO_ERROR)
	{
		LPTSTR lpszEnd	= nullptr;
		BOOL bIPv6		= addr.IsIPv6();
		BOOL bHasPort	= (addr.Port() != 0);

		if(!bIPv6)
		{
			if(bHasPort)
				lpszEnd = ::StrChr(lpszAddress, PORT_SEPARATOR_CHAR);
		}
		else
		{
			if(bHasPort)
			{
				static const TCHAR s_szBrk[] = {IPV6_ADDR_END_CHAR, IPV6_ZONE_INDEX_CHAR, 0};

				ASSERT(lpszAddress[0] == IPV6_ADDR_BEGIN_CHAR);
				lpszEnd = ::StrPBrk(lpszAddress, s_szBrk);
			}
			else
			{
				lpszEnd = ::StrChr(lpszAddress, IPV6_ZONE_INDEX_CHAR);
			}
		}

		ASSERT(!bHasPort || lpszEnd);

		if(lpszEnd)
		{
			lpszEnd[0]	= 0;
			iAddressLen	= lpszEnd - lpszAddress;
		}

		if(bIPv6 && bHasPort)
			memcpy(lpszAddress, (lpszAddress + 1), iAddressLen * sizeof(TCHAR));
		else
			++iAddressLen;

		isOK = TRUE;
	}
#endif

	return isOK;
}

BOOL sockaddr_A_2_IN(LPCTSTR lpszAddress, USHORT usPort, HP_SOCKADDR& addr)
{
	addr.family = DetermineAddrFamily(lpszAddress);
	return GetSockAddr(lpszAddress, usPort, addr);
}

BOOL GetSocketAddress(SOCKET socket, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort, BOOL bLocal)
{
	HP_SOCKADDR addr;

	int addr_len = addr.AddrSize();
	int result	 = bLocal ? getsockname(socket, addr.Addr(), &addr_len) : getpeername(socket, addr.Addr(), &addr_len);

	if(result != NO_ERROR)
		return FALSE;

	ADDRESS_FAMILY usFamily;
	return sockaddr_IN_2_A(addr, usFamily, lpszAddress, iAddressLen, usPort);
}

BOOL GetSocketLocalAddress(SOCKET socket, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort)
{
	return GetSocketAddress(socket, lpszAddress, iAddressLen, usPort, TRUE);
}

BOOL GetSocketRemoteAddress(SOCKET socket, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort)
{
	return GetSocketAddress(socket, lpszAddress, iAddressLen, usPort, FALSE);
}

ULONGLONG NToH64(ULONGLONG value)
{
	return (((ULONGLONG)ntohl((u_long)((value << 32) >> 32))) << 32) | ntohl((u_long)(value >> 32));
}

ULONGLONG HToN64(ULONGLONG value)
{
	return (((ULONGLONG)htonl((u_long)((value << 32) >> 32))) << 32) | htonl((u_long)(value >> 32));
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

HRESULT ReadSmallFile(LPCTSTR lpszFileName, CAtlFile& file, CAtlFileMapping<>& fmap, DWORD dwMaxFileSize)
{
	ASSERT(lpszFileName != nullptr);

	HRESULT hr = file.Create(lpszFileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);

	if(SUCCEEDED(hr))
	{
		ULONGLONG ullLen;
		hr = file.GetSize(ullLen);

		if(SUCCEEDED(hr))
		{
			if(ullLen > 0 && ullLen <= dwMaxFileSize)
				hr = fmap.MapFile(file);
			else if(ullLen == 0)
				hr = HRESULT_FROM_WIN32(ERROR_FILE_INVALID);
			else
				hr = HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE);
		}
	}

	return hr;
}

HRESULT MakeSmallFilePackage(LPCTSTR lpszFileName, CAtlFile& file, CAtlFileMapping<>& fmap, WSABUF szBuf[3], const LPWSABUF pHead, const LPWSABUF pTail)
{
	DWORD dwMaxFileSize = MAX_SMALL_FILE_SIZE - (pHead ? pHead->len : 0) - (pTail ? pTail->len : 0);
	ASSERT(dwMaxFileSize <= MAX_SMALL_FILE_SIZE);

	HRESULT hr = ReadSmallFile(lpszFileName, file, fmap, dwMaxFileSize);

	if(SUCCEEDED(hr))
	{
		szBuf[1].len = (ULONG)fmap.GetMappingSize();
		szBuf[1].buf = fmap;

		if(pHead) memcpy(&szBuf[0], pHead, sizeof(WSABUF));
		else	  memset(&szBuf[0], 0, sizeof(WSABUF));

		if(pTail) memcpy(&szBuf[2], pTail, sizeof(WSABUF));
		else	  memset(&szBuf[2], 0, sizeof(WSABUF));
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL PostIocpCommand(HANDLE hIOCP, EnIocpCommand enCmd, ULONG_PTR ulParam)
{
	return ::PostQueuedCompletionStatus(hIOCP, enCmd, ulParam, nullptr);
}

BOOL PostIocpExit(HANDLE hIOCP)
{
	return PostIocpCommand(hIOCP, IOCP_CMD_EXIT, 0);
}

BOOL PostIocpAccept(HANDLE hIOCP)
{
	return PostIocpCommand(hIOCP, IOCP_CMD_ACCEPT, 0);
}

BOOL PostIocpDisconnect(HANDLE hIOCP, CONNID dwConnID)
{
	return PostIocpCommand(hIOCP, IOCP_CMD_DISCONNECT, dwConnID);
}

BOOL PostIocpSend(HANDLE hIOCP, CONNID dwConnID)
{
	return PostIocpCommand(hIOCP, IOCP_CMD_SEND, dwConnID);
}

BOOL PostIocpClose(HANDLE hIOCP, CONNID dwConnID, int iErrorCode)
{
	return PostIocpCommand(hIOCP, (EnIocpCommand)iErrorCode, dwConnID);
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

int SSO_NoBlock(SOCKET sock, BOOL bNoBlock)
{
	return ioctlsocket(sock, FIONBIO, (ULONG*)&bNoBlock);
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

int PostAccept(LPFN_ACCEPTEX pfnAcceptEx, SOCKET soListen, SOCKET soClient, TBufferObj* pBufferObj, ADDRESS_FAMILY usFamily)
{
	int result = PostAcceptNotCheck(pfnAcceptEx, soListen, soClient, pBufferObj, usFamily);

	if(result == WSA_IO_PENDING)
		result = NO_ERROR;

	return result;
}

int PostAcceptNotCheck(LPFN_ACCEPTEX pfnAcceptEx, SOCKET soListen, SOCKET soClient, TBufferObj* pBufferObj, ADDRESS_FAMILY usFamily)
{
	int result				= NO_ERROR;
	pBufferObj->client		= soClient;
	pBufferObj->operation	= SO_ACCEPT;
	int iAddrLen			= HP_SOCKADDR::AddrSize(usFamily) + 16;

	if(!pfnAcceptEx	(
						soListen,
						pBufferObj->client,
						pBufferObj->buff.buf,
						0,
						iAddrLen,
						iAddrLen,
						nullptr,
						&pBufferObj->ov
					)
	)
	{
		result = ::WSAGetLastError();
	}

	return result;
}

int PostConnect(LPFN_CONNECTEX pfnConnectEx, SOCKET soClient, const HP_SOCKADDR& sockAddr, TBufferObj* pBufferObj)
{
	int result = PostConnectNotCheck(pfnConnectEx, soClient, sockAddr, pBufferObj);

	if(result == WSA_IO_PENDING)
		result = NO_ERROR;

	return result;
}

int PostConnectNotCheck(LPFN_CONNECTEX pfnConnectEx, SOCKET soClient, const HP_SOCKADDR& sockAddr, TBufferObj* pBufferObj)
{
	int result				= NO_ERROR;
	pBufferObj->client		= soClient;
	pBufferObj->operation	= SO_CONNECT;

	if(!pfnConnectEx	(
							soClient,
							sockAddr.Addr(),
							sockAddr.AddrSize(),
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
	pBufferObj->addrLen		= pBufferObj->remoteAddr.AddrSize();

	if(::WSASendTo	(
						sock,
						&pBufferObj->buff,
						1,
						&dwBytes,
						0,
						pBufferObj->remoteAddr.Addr(),
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
	pBufferObj->addrLen		= pBufferObj->remoteAddr.AddrSize();

	pBufferObj->remoteAddr.ZeroAddr();

	if(::WSARecvFrom(
						sock,
						&pBufferObj->buff,
						1,
						&dwBytes,
						&dwFlag,
						pBufferObj->remoteAddr.Addr(),
						&pBufferObj->addrLen,
						&pBufferObj->ov,
						nullptr
					) == SOCKET_ERROR)
	{
		result = ::WSAGetLastError();
	}

	return result;
}

int NoBlockReceive(TBufferObj* pBufferObj)
{
	int result = NoBlockReceiveNotCheck(pBufferObj);

	if(result == WSAEWOULDBLOCK)
		result = NO_ERROR;

	return result;
}

int NoBlockReceiveNotCheck(TBufferObj* pBufferObj)
{
	int result				= NO_ERROR;
	DWORD dwFlag			= 0;
	DWORD dwBytes			= 0;

	if(::WSARecv(
					pBufferObj->client,
					&pBufferObj->buff,
					1,
					&dwBytes,
					&dwFlag,
					nullptr,
					nullptr
				) == SOCKET_ERROR)
	{
		result = ::WSAGetLastError();
	}
	else
	{
		if(dwBytes > 0)
			pBufferObj->buff.len = dwBytes;
		else
			result = WSAEDISCON;
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
	case SE_DATA_SEND:				return _T("Send Data Fail");

	case SE_SSL_ENV_NOT_READY:		return _T("SSL environment not ready");

	default: ASSERT(FALSE);			return _T("UNKNOWN ERROR");
	}
}

DWORD GetHPSocketVersion()
{
	static DWORD s_dwVersion = (HP_VERSION_MAJOR << 24) | (HP_VERSION_MINOR << 16) | (HP_VERSION_REVISE << 8) | HP_VERSION_BUILD;

	return s_dwVersion;
}
