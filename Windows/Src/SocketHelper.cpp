/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Author	: Bruce Liang
 * Website	: https://github.com/ldcsaa
 * Project	: https://github.com/ldcsaa/HP-Socket
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 44636872, 75375912
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
#include "Common/GeneralHelper.h"
#include "Common/SysHelper.h"
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

static const BYTE s_szUdpCloseNotify[] = {0xBE, 0xB6, 0x1F, 0xEB, 0xDA, 0x52, 0x46, 0xBA, 0x92, 0x33, 0x59, 0xDB, 0xBF, 0xE6, 0xC8, 0xE4};
static const int s_iUdpCloseNotifySize = ARRAY_SIZE(s_szUdpCloseNotify);

const hp_addr hp_addr::ANY_ADDR4(AF_INET, TRUE);
const hp_addr hp_addr::ANY_ADDR6(AF_INET6, TRUE);

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

	hints.ai_flags	= (AI_V4MAPPED | AI_ADDRCONFIG);
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
		iAddrLength	= HP_SOCKADDR::AddrMinStrLength(pSockAddr->family);
		lpszAddr	= new TCHAR[iAddrLength];

		ENSURE(sockaddr_IN_2_A(*vt[i], usFamily, lpszAddr, iAddrLength, usPort));

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

BOOL IsLittleEndian()
{
	static const USHORT _s_endian_test_value = 0x0102;
	static const BOOL _s_bLE = (*((BYTE*)&_s_endian_test_value) == 0x02);

	return _s_bLE;
}

USHORT HToLE16(USHORT value)
{
	return IsLittleEndian() ? value : ENDIAN_SWAP_16(value);
}

USHORT HToBE16(USHORT value)
{
	return IsLittleEndian() ? ENDIAN_SWAP_16(value) : value;
}

DWORD HToLE32(DWORD value)
{
	return IsLittleEndian() ? value : ENDIAN_SWAP_32(value);
}

DWORD HToBE32(DWORD value)
{
	return IsLittleEndian() ? ENDIAN_SWAP_32(value) : value;
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

BOOL PostIocpUnpause(HANDLE hIOCP, CONNID dwConnID)
{
	return PostIocpCommand(hIOCP, IOCP_CMD_UNPAUSE, dwConnID);
}

BOOL PostIocpTimeout(HANDLE hIOCP, CONNID dwConnID)
{
	return PostIocpCommand(hIOCP, IOCP_CMD_TIMEOUT, dwConnID);
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

int SSO_RecvTimeOut(SOCKET sock, int ms)
{
	return setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (CHAR*)&ms, sizeof(int));
}

int SSO_SendTimeOut(SOCKET sock, int ms)
{
	return setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (CHAR*)&ms, sizeof(int));
}

int SSO_ReuseAddress(SOCKET sock, EnReuseAddressPolicy opt)
{
	BOOL bSet	= TRUE;
	BOOL bUnSet	= FALSE;
	int rs		= NO_ERROR;

	if(opt == RAP_NONE)
	{
		rs  = setsockopt(sock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (CHAR*)&bSet, sizeof(BOOL));
		rs |= setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&bUnSet, sizeof(BOOL));
	}
	else if(opt == RAP_ADDR_ONLY)
	{
		rs  = setsockopt(sock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (CHAR*)&bUnSet, sizeof(BOOL));
		rs |= setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&bUnSet, sizeof(BOOL));
	}
	else if(opt == RAP_ADDR_AND_PORT)
	{
		rs  = setsockopt(sock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (CHAR*)&bUnSet, sizeof(BOOL));
		rs |= setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&bSet, sizeof(BOOL));
	}
	else
	{
		::SetLastError(ERROR_INVALID_PARAMETER);
		rs = -1;
	}

	return rs;
}

int SSO_ExclusiveAddressUse(SOCKET sock, BOOL bExclusive)
{
	return setsockopt(sock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (CHAR*)&bExclusive, sizeof(BOOL));
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

int SSO_GetError(SOCKET sock)
{
	int e;
	socklen_t len = sizeof(e);

	if(IS_NO_ERROR(SSO_GetSocketOption(sock, SOL_SOCKET, SO_ERROR, &e, &len)))
		return e;

	return SOCKET_ERROR;
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

int IsUdpCloseNotify(const BYTE* pData, int iLength)
{
	return (iLength == s_iUdpCloseNotifySize								&&
			memcmp(pData, s_szUdpCloseNotify, s_iUdpCloseNotifySize) == 0)	;
}

int SendUdpCloseNotify(SOCKET sock)
{
	return send(sock, (LPCSTR)s_szUdpCloseNotify, s_iUdpCloseNotifySize, 0);
}

int SendUdpCloseNotify(SOCKET sock, const HP_SOCKADDR& remoteAddr)
{
	return sendto(sock, (LPCSTR)s_szUdpCloseNotify, s_iUdpCloseNotifySize, 0, remoteAddr.Addr(), remoteAddr.AddrSize());
}

int ManualCloseSocket(SOCKET sock, int iShutdownFlag, BOOL bGraceful)
{
	if(!bGraceful)
		SSO_Linger(sock, 1, 0);

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

	pBufferObj->ResetSendCounter();
	pSocketObj->Increment();

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

		if(result != WSA_IO_PENDING)
			pSocketObj->Decrement();
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

	pSocketObj->Increment();

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

		if(result != WSA_IO_PENDING)
			pSocketObj->Decrement();
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

	pBufferObj->ResetSendCounter();

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
	pBufferObj->operation	= SO_RECEIVE;
	pBufferObj->addrLen		= pBufferObj->remoteAddr.AddrSize();

	do 
	{
		DWORD dwFlag	= 0;
		DWORD dwBytes	= 0;

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

		if(!IS_UDP_RESET_ERROR(result))
			break;

		pBufferObj->ResetOV();

	} while(TRUE);

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
	int result		= NO_ERROR;
	DWORD dwFlag	= 0;
	DWORD dwBytes	= 0;

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

int NoBlockReceiveFrom(SOCKET sock, TUdpBufferObj* pBufferObj)
{
	int result = NoBlockReceiveFromNotCheck(sock, pBufferObj);

	if(result == WSAEWOULDBLOCK)
		result = NO_ERROR;

	return result;
}

int NoBlockReceiveFromNotCheck(SOCKET sock, TUdpBufferObj* pBufferObj)
{
	int result			= NO_ERROR;
	pBufferObj->addrLen	= pBufferObj->remoteAddr.AddrSize();

	do 
	{
		DWORD dwFlag	= 0;
		DWORD dwBytes	= 0;

		if(::WSARecvFrom(
							sock,
							&pBufferObj->buff,
							1,
							&dwBytes,
							&dwFlag,
							pBufferObj->remoteAddr.Addr(),
							&pBufferObj->addrLen,
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

	} while(IS_UDP_RESET_ERROR(result));


	return result;
}

BOOL SetMultiCastSocketOptions(SOCKET sock, const HP_SOCKADDR& bindAddr, const HP_SOCKADDR& castAddr, int iMCTtl, BOOL bMCLoop)
{
	if(castAddr.IsIPv4())
	{
		ENSURE(::SSO_SetSocketOption(sock, IPPROTO_IP, IP_MULTICAST_TTL, &iMCTtl, sizeof(iMCTtl)) != SOCKET_ERROR);
		ENSURE(::SSO_SetSocketOption(sock, IPPROTO_IP, IP_MULTICAST_LOOP, &bMCLoop, sizeof(bMCLoop)) != SOCKET_ERROR);

		ip_mreq mcast;
		::ZeroMemory(&mcast, sizeof(mcast));

		mcast.imr_multiaddr = castAddr.addr4.sin_addr;
		mcast.imr_interface = bindAddr.addr4.sin_addr;

		if(::SSO_SetSocketOption(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mcast, sizeof(mcast)) == SOCKET_ERROR)
			return FALSE;
	}
	else
	{
		ENSURE(::SSO_SetSocketOption(sock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &iMCTtl, sizeof(iMCTtl)) != SOCKET_ERROR);
		ENSURE(::SSO_SetSocketOption(sock, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &bMCLoop, sizeof(bMCLoop)) != SOCKET_ERROR);

		ipv6_mreq mcast;
		::ZeroMemory(&mcast, sizeof(mcast));

		mcast.ipv6mr_multiaddr = castAddr.addr6.sin6_addr;
		mcast.ipv6mr_interface = bindAddr.addr6.sin6_scope_id;

		if(::SSO_SetSocketOption(sock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mcast, sizeof(mcast)) == SOCKET_ERROR)
			return FALSE;
	}

	return TRUE;
}

int WaitForSocketWrite(SOCKET sock, DWORD dwTimeout)
{
	timeval tv = {(long)(dwTimeout / 1000), (long)((dwTimeout % 1000) * 1000)};

	fd_set wfds, efds;
	FD_ZERO(&wfds);
	FD_ZERO(&efds);
	FD_SET(sock, &wfds);
	FD_SET(sock, &efds);

	int rs = select(0, nullptr, &wfds, &efds, &tv);

	if(rs <= 0) return ((rs == 0) ? ERROR_TIMEOUT : ENSURE_ERROR(ERROR_CANT_WAIT));
	
	if(FD_ISSET(sock, &efds))
	{
		rs = SSO_GetError(sock);
		return ((rs != NO_ERROR && rs != SOCKET_ERROR) ? rs : ENSURE_ERROR(ERROR_CANT_WAIT));
	}

	VERIFY(FD_ISSET(sock, &wfds));

	return NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL SetCurrentWorkerThreadName()
{
	return SetWorkerThreadDefaultName(nullptr);
}

BOOL SetWorkerThreadDefaultName(HANDLE hThread)
{
	static volatile UINT _s_uiSeq = MAXUINT;

	return ::SetSequenceThreadName(hThread, DEFAULT_WORKER_THREAD_PREFIX, _s_uiSeq);
}

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
	case SE_GC_START:				return _T("Start GC Fail");

	case SE_SSL_ENV_NOT_READY:		return _T("SSL environment not ready");

	default: ASSERT(FALSE);			return _T("UNKNOWN ERROR");
	}
}

BOOL CodePageToUnicodeEx(int iCodePage, const char szSrc[], int iSrcLength, WCHAR szDest[], int& iDestLength)
{
	ASSERT(szSrc);

	int iSize = ::MultiByteToWideChar(iCodePage, 0, szSrc, iSrcLength, nullptr, 0);

	if(iSize == 0 || iSize > iDestLength || !szDest || iDestLength == 0)
	{
		iDestLength = iSize;
		return FALSE;
	}

	if(::MultiByteToWideChar(iCodePage, 0, szSrc, iSrcLength, szDest, iSize) != 0)
		iDestLength = iSize;
	else
		iDestLength = 0;

	return iDestLength != 0;
}

BOOL UnicodeToCodePageEx(int iCodePage, const WCHAR szSrc[], int iSrcLength, char szDest[], int& iDestLength)
{
	ASSERT(szSrc);

	int iSize = ::WideCharToMultiByte(iCodePage, 0, szSrc, iSrcLength, nullptr, 0, nullptr, nullptr);

	if(iSize == 0 || iSize > iDestLength || !szDest || iDestLength == 0)
	{
		iDestLength = iSize;
		return FALSE;
	}

	if(::WideCharToMultiByte(iCodePage, 0, szSrc, iSrcLength, szDest, iSize, nullptr, nullptr) != 0)
		iDestLength = iSize;
	else
		iDestLength = 0;

	return iDestLength != 0;
}

BOOL GbkToUnicodeEx(const char szSrc[], int iSrcLength, WCHAR szDest[], int& iDestLength)
{
	return CodePageToUnicodeEx(CP_ACP, szSrc, iSrcLength, szDest, iDestLength);
}

BOOL UnicodeToGbkEx(const WCHAR szSrc[], int iSrcLength, char szDest[], int& iDestLength)
{
	return UnicodeToCodePageEx(CP_ACP, szSrc, iSrcLength, szDest, iDestLength);
}

BOOL Utf8ToUnicodeEx(const char szSrc[], int iSrcLength, WCHAR szDest[], int& iDestLength)
{
	return CodePageToUnicodeEx(CP_UTF8, szSrc, iSrcLength, szDest, iDestLength);
}

BOOL UnicodeToUtf8Ex(const WCHAR szSrc[], int iSrcLength, char szDest[], int& iDestLength)
{
	return UnicodeToCodePageEx(CP_UTF8, szSrc, iSrcLength, szDest, iDestLength);
}

BOOL GbkToUtf8Ex(const char szSrc[], int iSrcLength, char szDest[], int& iDestLength)
{
	int iMiddleLength = 0;
	GbkToUnicodeEx(szSrc, iSrcLength, nullptr, iMiddleLength);

	if(iMiddleLength == 0)
	{
		iDestLength = 0;
		return FALSE;
	}

	unique_ptr<WCHAR[]> p(new WCHAR[iMiddleLength]);
	ENSURE(GbkToUnicodeEx(szSrc, iSrcLength, p.get(), iMiddleLength));

	return UnicodeToUtf8Ex(p.get(), iMiddleLength, szDest, iDestLength);
}

BOOL Utf8ToGbkEx(const char szSrc[], int iSrcLength, char szDest[], int& iDestLength)
{
	int iMiddleLength = 0;
	Utf8ToUnicodeEx(szSrc, iSrcLength, nullptr, iMiddleLength);

	if(iMiddleLength == 0)
	{
		iDestLength = 0;
		return FALSE;
	}

	unique_ptr<WCHAR[]> p(new WCHAR[iMiddleLength]);
	ENSURE(Utf8ToUnicodeEx(szSrc, iSrcLength, p.get(), iMiddleLength));

	return UnicodeToGbkEx(p.get(), iMiddleLength, szDest, iDestLength);
}

BOOL CodePageToUnicode(int iCodePage, const char szSrc[], WCHAR szDest[], int& iDestLength)
{
	return CodePageToUnicodeEx(iCodePage, szSrc, -1, szDest, iDestLength);
}

BOOL UnicodeToCodePage(int iCodePage, const WCHAR szSrc[], char szDest[], int& iDestLength)
{
	return UnicodeToCodePageEx(iCodePage, szSrc, -1, szDest, iDestLength);
}

BOOL GbkToUnicode(const char szSrc[], WCHAR szDest[], int& iDestLength)
{
	return GbkToUnicodeEx(szSrc, -1, szDest, iDestLength);
}

BOOL UnicodeToGbk(const WCHAR szSrc[], char szDest[], int& iDestLength)
{
	return UnicodeToGbkEx(szSrc, -1, szDest, iDestLength);
}

BOOL Utf8ToUnicode(const char szSrc[], WCHAR szDest[], int& iDestLength)
{
	return Utf8ToUnicodeEx(szSrc, -1, szDest, iDestLength);
}

BOOL UnicodeToUtf8(const WCHAR szSrc[], char szDest[], int& iDestLength)
{
	return UnicodeToUtf8Ex(szSrc, -1, szDest, iDestLength);
}

BOOL GbkToUtf8(const char szSrc[], char szDest[], int& iDestLength)
{
	return GbkToUtf8Ex(szSrc, -1, szDest, iDestLength);
}

BOOL Utf8ToGbk(const char szSrc[], char szDest[], int& iDestLength)
{
	return Utf8ToGbkEx(szSrc, -1, szDest, iDestLength);
}

DWORD GuessBase64EncodeBound(DWORD dwSrcLen)
{
	return 4 * ((dwSrcLen + 2) / 3);
}

DWORD GuessBase64DecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen)
{
	if(dwSrcLen < 2)
		return 0;

	if(lpszSrc[dwSrcLen - 2] == '=')
		dwSrcLen -= 2;
	else if(lpszSrc[dwSrcLen - 1] == '=')
			--dwSrcLen;

	DWORD dwMod = dwSrcLen % 4;
	DWORD dwAdd = dwMod == 2 ? 1 : (dwMod == 3 ? 2 : 0);

	return 3 * (dwSrcLen / 4) + dwAdd;
}

int Base64Encode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	static const BYTE CODES[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	DWORD dwRealLen = GuessBase64EncodeBound(dwSrcLen);

	if(lpszDest == nullptr || dwDestLen < dwRealLen)
	{
		dwDestLen = dwRealLen;
		return -5;
	}

	BYTE* p		= lpszDest;
	DWORD leven	= 3 * (dwSrcLen / 3);
	DWORD i		= 0;

	for (; i < leven; i += 3)
	{
		*p++ = CODES[lpszSrc[0] >> 2];
		*p++ = CODES[((lpszSrc[0] & 3) << 4) + (lpszSrc[1] >> 4)];
		*p++ = CODES[((lpszSrc[1] & 0xf) << 2) + (lpszSrc[2] >> 6)];
		*p++ = CODES[lpszSrc[2] & 0x3f];

		lpszSrc += 3;
	}

	if(i < dwSrcLen)
	{
		BYTE a = lpszSrc[0];
		BYTE b = (i + 1 < dwSrcLen) ? lpszSrc[1] : 0;

		*p++ = CODES[a >> 2];
		*p++ = CODES[((a & 3) << 4) + (b >> 4)];
		*p++ = (i + 1 < dwSrcLen) ? CODES[((b & 0xf) << 2)] : '=';
		*p++ = '=';
	}  

	ASSERT(dwRealLen == (DWORD)(p - lpszDest));

	if(dwDestLen > dwRealLen)
	{
		*p			= 0;
		dwDestLen	= dwRealLen;
	}

	return 0;  
}

int Base64Decode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	static const BYTE MAP[256]	=
	{ 
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 253, 255,
		255, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 253, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
		 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
		255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,
		  7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,
		 19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
		255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
		 37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
		 49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255
	};

	DWORD dwRealLen = GuessBase64DecodeBound(lpszSrc, dwSrcLen);

	if(lpszDest == nullptr || dwDestLen < dwRealLen)
	{
		dwDestLen = dwRealLen;
		return -5;
	}

	BYTE c;
	int g = 3;
	DWORD i, x, y, z;

	for(i = x = y = z = 0; i < dwSrcLen || x != 0;)
	{
		c = i < dwSrcLen ? MAP[lpszSrc[i++]] : 254;

		if(c == 255) {dwDestLen = 0; return -3;}
		else if(c == 254) {c = 0; g--;}
		else if(c == 253) continue;

		z = (z << 6) | c;

		if(++x == 4)
		{
			lpszDest[y++] = (BYTE)((z >> 16) & 255);
			if (g > 1) lpszDest[y++] = (BYTE)((z >> 8) & 255);
			if (g > 2) lpszDest[y++] = (BYTE)(z & 255);

			x = z = 0;
		}
	}

	BOOL isOK = (y == dwRealLen);

	if(!isOK)
		dwDestLen = 0;
	else
	{
		if(dwDestLen > dwRealLen)
		{
			lpszDest[dwRealLen]	= 0;
			dwDestLen			= dwRealLen;
		}
	}

	return isOK ? 0 : -3;
}

DWORD GuessUrlEncodeBound(const BYTE* lpszSrc, DWORD dwSrcLen)
{
	DWORD dwAdd = 0;

	for(DWORD i = 0; i < dwSrcLen; i++)
	{
		BYTE c	= lpszSrc[i];

		if(!(isalnum(c) || c == ' ' || c == '.' || c == '-' || c == '_' || c == '*'))
			dwAdd += 2;
	}

	return dwSrcLen + dwAdd;
}

DWORD GuessUrlDecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen)
{
	DWORD dwPercent = 0;

	for(DWORD i = 0; i < dwSrcLen; i++)
	{
		if(lpszSrc[i] == '%')
		{
			++dwPercent;
			i += 2;
		}
	}

	DWORD dwSub = dwPercent * 2;

	if(dwSrcLen < dwSub)
		return 0;

	return dwSrcLen - dwSub;
}

int UrlEncode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	BYTE c;
	DWORD j = 0;

	if(lpszDest == nullptr || dwDestLen == 0)
		goto ERROR_DEST_LEN;

	for(DWORD i = 0; i < dwSrcLen; i++)
	{
		if(j >= dwDestLen)
			goto ERROR_DEST_LEN;

		c = lpszSrc[i];

		if (isalnum(c) || c == '.' || c == '-' || c == '_' || c == '*')
			lpszDest[j++] = c;
		else if(c == ' ')
			lpszDest[j++] = '+';
		else
		{
			if(j + 3 >= dwDestLen)
				goto ERROR_DEST_LEN;

			lpszDest[j++] = '%';
			HEX_VALUE_TO_DOUBLE_CHAR(lpszDest + j, c);
			j += 2;
			
		}
	}

	if(dwDestLen > j)
	{
		lpszDest[j]	= 0;
		dwDestLen	= j;
	}

	return 0;

ERROR_DEST_LEN:
	dwDestLen = GuessUrlEncodeBound(lpszSrc, dwSrcLen);
	return -5;
}

int UrlDecode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	char c;
	DWORD j = 0;

	if(lpszDest == nullptr || dwDestLen == 0)
		goto ERROR_DEST_LEN;

	for(DWORD i = 0; i < dwSrcLen; i++)
	{
		if(j >= dwDestLen)
			goto ERROR_DEST_LEN;

		c = lpszSrc[i];

		if(c == '+')
			lpszDest[j++] = ' ';
		else if(c != '%')
			lpszDest[j++] = c;
		else
		{
			if(i + 2 >= dwSrcLen)
				goto ERROR_SRC_DATA;

			lpszDest[j++] = HEX_DOUBLE_CHAR_TO_VALUE(lpszSrc + i + 1);
			i += 2;
		}
	}

	if(dwDestLen > j)
	{
		lpszDest[j]	= 0;
		dwDestLen	= j;
	}

	return 0;

ERROR_SRC_DATA:
	dwDestLen = 0;
	return -3;

ERROR_DEST_LEN:
	dwDestLen = GuessUrlDecodeBound(lpszSrc, dwSrcLen);
	return -5;
}

void DestroyCompressor(IHPCompressor* pCompressor)
{
	delete pCompressor;
}

void DestroyDecompressor(IHPDecompressor* pDecompressor)
{
	delete pDecompressor;
}

#ifdef _ZLIB_SUPPORT

CHPZLibCompressor::CHPZLibCompressor(Fn_CompressDataCallback fnCallback, int iWindowBits, int iLevel, int iMethod, int iMemLevel, int iStrategy, DWORD dwBuffSize)
: m_fnCallback	(fnCallback)
, m_dwBuffSize	(dwBuffSize)
, m_bValid		(FALSE)
{
	ASSERT(m_fnCallback != nullptr);

	::ZeroObject(m_Stream);

	m_bValid = (::deflateInit2(&m_Stream, iLevel, iMethod, iWindowBits, iMemLevel, iStrategy) == Z_OK);
}
CHPZLibCompressor::~CHPZLibCompressor()
{
	if(m_bValid) ::deflateEnd(&m_Stream);
}

BOOL CHPZLibCompressor::Reset()
{
	return (m_bValid = (::deflateReset(&m_Stream) == Z_OK));
}

BOOL CHPZLibCompressor::Process(const BYTE* pData, int iLength, BOOL bLast, PVOID pContext)
{
	return ProcessEx(pData, iLength, bLast, FALSE, pContext);
}

BOOL CHPZLibCompressor::ProcessEx(const BYTE* pData, int iLength, BOOL bLast, BOOL bFlush, PVOID pContext)
{
	ASSERT(IsValid() && iLength > 0);

	if(!IsValid())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	unique_ptr<BYTE[]> szBuff(new BYTE[m_dwBuffSize]);

	m_Stream.next_in	= (z_const Bytef*)pData;
	m_Stream.avail_in	= iLength;
	
	BOOL isOK	= TRUE;
	int rs		= Z_OK;
	int flush	= bLast ? Z_FINISH : (bFlush ? Z_SYNC_FLUSH : Z_NO_FLUSH);

	while(m_Stream.avail_in > 0)
	{
		do
		{
			m_Stream.next_out  = szBuff.get();
			m_Stream.avail_out = m_dwBuffSize;

			rs = ::deflate(&m_Stream, flush);

			if(rs == Z_STREAM_ERROR)
			{
				::SetLastError(ERROR_INVALID_DATA);
				isOK = FALSE;
				
				goto ZLIB_COMPRESS_END;
			}

			int iRead = (int)(m_dwBuffSize - m_Stream.avail_out);

			if(iRead == 0)
				break;

			if(!m_fnCallback(szBuff.get(), iRead, pContext))
			{
				::SetLastError(ERROR_CANCELLED);
				isOK = FALSE;

				goto ZLIB_COMPRESS_END;
			}
		} while(m_Stream.avail_out == 0);
	}

ZLIB_COMPRESS_END:

	ASSERT(!isOK || (rs == Z_OK && !bLast) || (rs == Z_STREAM_END && bLast));

	if(!isOK || bLast) Reset();

	return isOK;
}

CHPZLibDecompressor::CHPZLibDecompressor(Fn_DecompressDataCallback fnCallback, int iWindowBits, DWORD dwBuffSize)
: m_fnCallback	(fnCallback)
, m_dwBuffSize	(dwBuffSize)
, m_bValid		(FALSE)
{
	ASSERT(m_fnCallback != nullptr);

	::ZeroObject(m_Stream);

	m_bValid = (::inflateInit2(&m_Stream, iWindowBits) == Z_OK);
}
CHPZLibDecompressor::~CHPZLibDecompressor()
{
	if(m_bValid) ::inflateEnd(&m_Stream);
}

BOOL CHPZLibDecompressor::Reset()
{
	return (m_bValid = (::inflateReset(&m_Stream) == Z_OK));
}

BOOL CHPZLibDecompressor::Process(const BYTE* pData, int iLength, PVOID pContext)
{
	ASSERT(IsValid() && iLength > 0);

	if(!IsValid())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	unique_ptr<BYTE[]> szBuff(new BYTE[m_dwBuffSize]);

	m_Stream.next_in	= (z_const Bytef*)pData;
	m_Stream.avail_in	= iLength;

	BOOL isOK	= TRUE;
	int rs		= Z_OK;

	while(m_Stream.avail_in > 0)
	{
		do
		{
			m_Stream.next_out  = szBuff.get();
			m_Stream.avail_out = m_dwBuffSize;

			rs = ::inflate(&m_Stream, Z_NO_FLUSH);

			if(rs != Z_OK && rs != Z_STREAM_END)
			{
				::SetLastError(ERROR_INVALID_DATA);
				isOK = FALSE;

				goto ZLIB_DECOMPRESS_END;
			}

			int iRead = (int)(m_dwBuffSize - m_Stream.avail_out);

			if(iRead == 0)
				break;

			if(!m_fnCallback(szBuff.get(), iRead, pContext))
			{
				::SetLastError(ERROR_CANCELLED);
				isOK = FALSE;

				goto ZLIB_DECOMPRESS_END;
			}
		} while(m_Stream.avail_out == 0);

		if(rs == Z_STREAM_END)
			break;
	}

ZLIB_DECOMPRESS_END:

	ASSERT(!isOK || rs == Z_OK || rs == Z_STREAM_END);

	if(!isOK || rs == Z_STREAM_END) Reset();

	return isOK;
}

IHPCompressor* CreateZLibCompressor(Fn_CompressDataCallback fnCallback, int iWindowBits, int iLevel, int iMethod, int iMemLevel, int iStrategy, DWORD dwBuffSize)
{
	return new CHPZLibCompressor(fnCallback, iWindowBits, iLevel, iMethod, iMemLevel, iStrategy, dwBuffSize);
}

IHPCompressor* CreateGZipCompressor(Fn_CompressDataCallback fnCallback, int iLevel, int iMethod, int iMemLevel, int iStrategy, DWORD dwBuffSize)
{
	return new CHPZLibCompressor(fnCallback, MAX_WBITS + 16, iLevel, iMethod, iMemLevel, iStrategy, dwBuffSize);
}

IHPDecompressor* CreateZLibDecompressor(Fn_DecompressDataCallback fnCallback, int iWindowBits, DWORD dwBuffSize)
{
	return new CHPZLibDecompressor(fnCallback, iWindowBits, dwBuffSize);
}

IHPDecompressor* CreateGZipDecompressor(Fn_DecompressDataCallback fnCallback, DWORD dwBuffSize)
{
	return new CHPZLibDecompressor(fnCallback, MAX_WBITS + 32, dwBuffSize);
}

int Compress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	return CompressEx(lpszSrc, dwSrcLen, lpszDest, dwDestLen);
}

int CompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iLevel, int iMethod, int iWindowBits, int iMemLevel, int iStrategy)
{
	z_stream stream;

	stream.next_in	 = (z_const Bytef*)lpszSrc;
	stream.avail_in	 = dwSrcLen;
	stream.next_out	 = lpszDest;
	stream.avail_out = dwDestLen;
	stream.zalloc	 = nullptr;
	stream.zfree	 = nullptr;
	stream.opaque	 = nullptr;

	int err = ::deflateInit2(&stream, iLevel, iMethod, iWindowBits, iMemLevel, iStrategy);

	if(err != Z_OK) return err;

	err = ::deflate(&stream, Z_FINISH);

	if(err != Z_STREAM_END)
	{
		::deflateEnd(&stream);
		return err == Z_OK ? Z_BUF_ERROR : err;
	}

	if(dwDestLen > stream.total_out)
	{
		lpszDest[stream.total_out]	= 0;
		dwDestLen					= stream.total_out;
	}

	return ::deflateEnd(&stream);
}

int Uncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	return UncompressEx(lpszSrc, dwSrcLen, lpszDest, dwDestLen);
}

int UncompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iWindowBits)
{
	z_stream stream;

	stream.next_in	 = (z_const Bytef*)lpszSrc;
	stream.avail_in	 = dwSrcLen;
	stream.next_out	 = lpszDest;
	stream.avail_out = dwDestLen;
	stream.zalloc	 = nullptr;
	stream.zfree	 = nullptr;

	int err = ::inflateInit2(&stream, iWindowBits);

	if(err != Z_OK) return err;

	err = ::inflate(&stream, Z_FINISH);

	if(err != Z_STREAM_END)
	{
		::inflateEnd(&stream);
		return (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0)) ? Z_DATA_ERROR : err;
	}

	if(dwDestLen > stream.total_out)
	{
		lpszDest[stream.total_out]	= 0;
		dwDestLen					= stream.total_out;
	}

	return inflateEnd(&stream);
}

DWORD GuessCompressBound(DWORD dwSrcLen, BOOL bGZip)
{
	DWORD dwBound = ::compressBound(dwSrcLen);

	if(bGZip) dwBound += 16;

	return dwBound;
}

int GZipCompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	return CompressEx(lpszSrc, dwSrcLen, lpszDest, dwDestLen, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS + 16);
}

int GZipUncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	return UncompressEx(lpszSrc, dwSrcLen, lpszDest, dwDestLen, MAX_WBITS + 32);
}

DWORD GZipGuessUncompressBound(const BYTE* lpszSrc, DWORD dwSrcLen)
{
	if(dwSrcLen < 20 || *(USHORT*)lpszSrc != 0x8B1F)
		return 0;

	return *(DWORD*)(lpszSrc + dwSrcLen - 4);
}

#endif

#ifdef _BROTLI_SUPPORT

CHPBrotliCompressor::CHPBrotliCompressor(Fn_CompressDataCallback fnCallback, int iQuality, int iWindow, int iMode, DWORD dwBuffSize)
: m_fnCallback	(fnCallback)
, m_iQuality	(iQuality)
, m_iWindow		(iWindow)
, m_iMode		(iMode)
, m_dwBuffSize	(dwBuffSize)
, m_bValid		(FALSE)
{
	ASSERT(m_fnCallback != nullptr);

	Reset();
}

CHPBrotliCompressor::~CHPBrotliCompressor()
{
	if(m_bValid) ::BrotliEncoderDestroyInstance(m_pState);
}

BOOL CHPBrotliCompressor::Reset()
{
	if(m_bValid) ::BrotliEncoderDestroyInstance(m_pState);
	m_pState =   ::BrotliEncoderCreateInstance(nullptr, nullptr, nullptr);

	if(m_pState != nullptr)
	{
		::BrotliEncoderSetParameter(m_pState, BROTLI_PARAM_QUALITY	, (UINT)m_iQuality);
		::BrotliEncoderSetParameter(m_pState, BROTLI_PARAM_LGWIN	, (UINT)m_iWindow);
		::BrotliEncoderSetParameter(m_pState, BROTLI_PARAM_MODE		, (UINT)m_iMode);

		if (m_iWindow > BROTLI_MAX_WINDOW_BITS)
			::BrotliEncoderSetParameter(m_pState, BROTLI_PARAM_LARGE_WINDOW, BROTLI_TRUE);
	}

	return (m_bValid = (m_pState != nullptr));
}

BOOL CHPBrotliCompressor::Process(const BYTE* pData, int iLength, BOOL bLast, PVOID pContext)
{
	return ProcessEx(pData, iLength, bLast, FALSE, pContext);
}

BOOL CHPBrotliCompressor::ProcessEx(const BYTE* pData, int iLength, BOOL bLast, BOOL bFlush, PVOID pContext)
{
	ASSERT(IsValid() && iLength > 0);

	if(!IsValid())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	unique_ptr<BYTE[]> szBuff(new BYTE[m_dwBuffSize]);

	const BYTE* pNextInData	= pData;
	size_t iAvlInLen		= (SIZE_T)iLength;
	BYTE* pNextOutData		= nullptr;
	size_t iAvlOutLen		= 0;

	BOOL isOK				  = TRUE;
	BrotliEncoderOperation op = bLast ? BROTLI_OPERATION_FINISH : (bFlush ? BROTLI_OPERATION_FLUSH : BROTLI_OPERATION_PROCESS);

	while(iAvlInLen > 0)
	{
		do
		{
			pNextOutData = szBuff.get();
			iAvlOutLen	 = m_dwBuffSize;

			if(!::BrotliEncoderCompressStream(m_pState, op, &iAvlInLen, &pNextInData, &iAvlOutLen, &pNextOutData, nullptr))
			{
				::SetLastError(ERROR_INVALID_DATA);
				isOK = FALSE;

				goto BROTLI_COMPRESS_END;
			}

			int iRead = (int)(m_dwBuffSize - iAvlOutLen);

			if(iRead == 0)
				break;

			if(!m_fnCallback(szBuff.get(), iRead, pContext))
			{
				::SetLastError(ERROR_CANCELLED);
				isOK = FALSE;
				
				goto BROTLI_COMPRESS_END;
			}
		} while (iAvlOutLen == 0);
	}

BROTLI_COMPRESS_END:

	if(!isOK || bLast) Reset();

	return isOK;
}

CHPBrotliDecompressor::CHPBrotliDecompressor(Fn_DecompressDataCallback fnCallback, DWORD dwBuffSize)
: m_fnCallback	(fnCallback)
, m_dwBuffSize	(dwBuffSize)
, m_bValid		(FALSE)
{
	ASSERT(m_fnCallback != nullptr);

	Reset();
}

CHPBrotliDecompressor::~CHPBrotliDecompressor()
{
	if(m_bValid) ::BrotliDecoderDestroyInstance(m_pState);
}

BOOL CHPBrotliDecompressor::Reset()
{
	if(m_bValid) ::BrotliDecoderDestroyInstance(m_pState);
	m_pState =   ::BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);

	return (m_bValid = (m_pState != nullptr));
}

BOOL CHPBrotliDecompressor::Process(const BYTE* pData, int iLength, PVOID pContext)
{
	ASSERT(IsValid() && iLength > 0);

	if(!IsValid())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	unique_ptr<BYTE[]> szBuff(new BYTE[m_dwBuffSize]);

	const BYTE* pNextInData	= pData;
	size_t iAvlInLen		= (SIZE_T)iLength;
	BYTE* pNextOutData		= nullptr;
	size_t iAvlOutLen		= 0;

	BOOL isOK				= TRUE;
	BrotliDecoderResult rs	= BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;

	do
	{
		do
		{
			pNextOutData = szBuff.get();
			iAvlOutLen	 = m_dwBuffSize;

			rs = ::BrotliDecoderDecompressStream(m_pState, &iAvlInLen, &pNextInData, &iAvlOutLen, &pNextOutData, nullptr);

			if(rs == BROTLI_DECODER_RESULT_ERROR)
			{
				::SetLastError(ERROR_INVALID_DATA);
				isOK = FALSE;

				goto BROTLI_DECOMPRESS_END;
			}

			int iRead = (int)(m_dwBuffSize - iAvlOutLen);

			if(iRead == 0)
				break;

			if(!m_fnCallback(szBuff.get(), iRead, pContext))
			{
				::SetLastError(ERROR_CANCELLED);
				isOK = FALSE;

				goto BROTLI_DECOMPRESS_END;
			}
		} while (iAvlOutLen == 0);

		if(rs == BROTLI_DECODER_RESULT_SUCCESS)
			break;

	} while(rs == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT);

BROTLI_DECOMPRESS_END:

	if(!isOK || rs == BROTLI_DECODER_RESULT_SUCCESS) Reset();

	return isOK;
}

IHPCompressor* CreateBrotliCompressor(Fn_CompressDataCallback fnCallback, int iQuality, int iWindow, int iMode, DWORD dwBuffSize)
{
	return new CHPBrotliCompressor(fnCallback, iQuality, iWindow, iMode, dwBuffSize);
}

IHPDecompressor* CreateBrotliDecompressor(Fn_DecompressDataCallback fnCallback, DWORD dwBuffSize)
{
	return new CHPBrotliDecompressor(fnCallback, dwBuffSize);
}

int BrotliCompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	return BrotliCompressEx(lpszSrc, dwSrcLen, lpszDest, dwDestLen, BROTLI_DEFAULT_QUALITY, BROTLI_DEFAULT_WINDOW, BROTLI_DEFAULT_MODE);
}

int BrotliCompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iQuality, int iWindow, int iMode)
{
	size_t stDestLen = (size_t)dwDestLen;
	int rs = ::BrotliEncoderCompress(iQuality, iWindow, (BrotliEncoderMode)iMode, (size_t)dwSrcLen, lpszSrc, &stDestLen, lpszDest);
	dwDestLen = (DWORD)stDestLen;

	return (rs == 1) ? 0 : ((rs == 3) ? -5 : -3);
}

int BrotliUncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	size_t stDestLen = (size_t)dwDestLen;
	BrotliDecoderResult rs = ::BrotliDecoderDecompress((size_t)dwSrcLen, lpszSrc, &stDestLen, lpszDest);
	dwDestLen = (DWORD)stDestLen;

	return (rs == BROTLI_DECODER_RESULT_SUCCESS) ? 0 : ((rs == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) ? -5 : -3);
}

DWORD BrotliGuessCompressBound(DWORD dwSrcLen)
{
	return (DWORD)::BrotliEncoderMaxCompressedSize((size_t)dwSrcLen);
}

#endif
