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

#include "SocketHelper.h"

#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////

const hp_addr hp_addr::ANY_ADDR4(AF_INET, TRUE);
const hp_addr hp_addr::ANY_ADDR6(AF_INET6, TRUE);

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

ADDRESS_FAMILY DetermineAddrFamily(LPCTSTR lpszAddress)
{
	if (!lpszAddress || lpszAddress[0] == 0)
		return AF_UNSPEC;

	if(::StrChr(lpszAddress, IPV6_ADDR_SEPARATOR_CHAR))
		return AF_INET6;

	TCHAR c;
	int arr[4];

	if(stscanf(lpszAddress, _T("%d.%d.%d.%d%c"), &arr[0], &arr[1], &arr[2], &arr[3], &c) != 4)
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

	return (::InetPton(addr.family, lpszAddress, addr.Addr()) == TRUE);
}

BOOL GetSockAddr(LPCTSTR lpszAddress, USHORT usPort, HP_SOCKADDR& addr)
{
	if(addr.family != AF_INET && addr.family != AF_INET6)
	{
		::WSASetLastError(EADDRNOTAVAIL);
		return FALSE;
	}

	if(addr.family == AF_INET6 && StrChr(lpszAddress, IPV6_ZONE_INDEX_CHAR))
		return GetSockAddrByHostNameDirectly(lpszAddress, usPort, addr);

	addr.ZeroAddr();

	int rs = ::InetPton(addr.family, lpszAddress, addr.SinAddr());

	if(rs != 1)
	{
		if(rs == 0) ::WSASetLastError(ERROR_INVALID_PARAMETER);

		return FALSE;
	}

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

	hints.ai_flags		= AI_ALL;
	hints.ai_family		= addr.family;
	hints.ai_socktype	= SOCK_STREAM;

	int rs = ::getaddrinfo(CT2A(lpszHost), nullptr, &hints, &pInfo);

	if(!IS_NO_ERROR(rs))
	{
		::WSASetLastError(ERROR_HOSTUNREACH);
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

	EXECUTE_RESTORE_ERROR(::freeaddrinfo(pInfo));

	if(isOK)
		addr.SetPort(usPort);
	else
		::WSASetLastError(EHOSTUNREACH);

	return isOK;
}

BOOL EnumHostIPAddresses(LPCTSTR lpszHost, EnIPAddrType enType, LPTIPAddr** lpppIPAddr, int& iIPAddrCount)
{
	*lpppIPAddr	 = nullptr;
	iIPAddrCount = 0;

	ADDRESS_FAMILY usFamily =	(enType		== IPT_ALL				?
								AF_UNSPEC	: (enType == IPT_IPV4	?
								AF_INET		: (enType == IPT_IPV6	?
								AF_INET6	: 0xFF)));

	if(usFamily == 0xFF)
	{
		::WSASetLastError(EAFNOSUPPORT);
		return FALSE;
	}

	vector<HP_PSOCKADDR> vt;

	ADDRESS_FAMILY usFamily2 = DetermineAddrFamily(lpszHost);

	if(usFamily2 != AF_UNSPEC)
	{
		if(usFamily != AF_UNSPEC && usFamily != usFamily2)
		{
			::WSASetLastError(EHOSTUNREACH);
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

	hints.ai_flags		= AI_ALL;
	hints.ai_family		= usFamily;
	hints.ai_socktype	= SOCK_STREAM;

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

	if(!isOK) ::WSASetLastError(EHOSTUNREACH);

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

	if(::InetNtop(addr.family, addr.SinAddr(), lpszAddress, iAddressLen))
	{
		iAddressLen	= (int)lstrlen(lpszAddress) + 1;
		isOK		= TRUE;
	}
	else
	{
		if(::WSAGetLastError() == ENOSPC)
			iAddressLen = HP_SOCKADDR::AddrMinStrLength(usFamily);
	}

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
	int result	 = bLocal ? getsockname(socket, addr.Addr(), (socklen_t*)&addr_len) : getpeername(socket, addr.Addr(), (socklen_t*)&addr_len);

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
	return (((ULONGLONG)ntohl((UINT)((value << 32) >> 32))) << 32) | ntohl((UINT)(value >> 32));
}

ULONGLONG HToN64(ULONGLONG value)
{
	return (((ULONGLONG)htonl((UINT)((value << 32) >> 32))) << 32) | htonl((UINT)(value >> 32));
}

HRESULT ReadSmallFile(LPCTSTR lpszFileName, CFile& file, CFileMapping& fmap, DWORD dwMaxFileSize)
{
	ASSERT(lpszFileName != nullptr);

	if(file.Open(lpszFileName, O_RDONLY))
	{
		SIZE_T dwSize;
		if(file.GetSize(dwSize))
		{
			if(dwSize > 0 && dwSize <= dwMaxFileSize)
			{
				if(fmap.Map(file, dwSize))
					return NO_ERROR;
			}
			else if(dwSize == 0)
				::SetLastError(ERROR_EMPTY);
			else
				::SetLastError(ERROR_FILE_TOO_LARGE);
		}
	}

	HRESULT rs = ::GetLastError();

	return (!IS_NO_ERROR(rs) ? rs : ERROR_UNKNOWN);
}

HRESULT MakeSmallFilePackage(LPCTSTR lpszFileName, CFile& file, CFileMapping& fmap, WSABUF szBuf[3], const LPWSABUF pHead, const LPWSABUF pTail)
{
	DWORD dwMaxFileSize = MAX_SMALL_FILE_SIZE - (pHead ? pHead->len : 0) - (pTail ? pTail->len : 0);
	ASSERT(dwMaxFileSize <= MAX_SMALL_FILE_SIZE);

	HRESULT hr = ReadSmallFile(lpszFileName, file, fmap, dwMaxFileSize);

	if(IS_NO_ERROR(hr))
	{
		szBuf[1].len = (UINT)fmap.Size();
		szBuf[1].buf = fmap;

		if(pHead) memcpy(&szBuf[0], pHead, sizeof(WSABUF));
		else	  memset(&szBuf[0], 0, sizeof(WSABUF));

		if(pTail) memcpy(&szBuf[2], pTail, sizeof(WSABUF));
		else	  memset(&szBuf[2], 0, sizeof(WSABUF));
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

int SSO_SetSocketOption(SOCKET sock, int level, int name, LPVOID val, int len)
{
	return setsockopt(sock, level, name, val, (socklen_t)len);
}

int SSO_GetSocketOption(SOCKET sock, int level, int name, LPVOID val, int* len)
{
	return getsockopt(sock, level, name, val, (socklen_t*)len);
}

int SSO_IoctlSocket(SOCKET sock, long cmd, PVOID arg)
{
	return ioctl(sock, cmd, arg);
}

int SSO_NoBlock(SOCKET sock, BOOL bNoBlock)
{
	return fcntl_SETFL(sock, O_NONBLOCK, bNoBlock) ? NO_ERROR : SOCKET_ERROR;
}

int SSO_NoDelay(SOCKET sock, BOOL bNoDelay)
{
	int val = bNoDelay ? 1 : 0;
	return setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(int));
}

int SSO_DontLinger(SOCKET sock, BOOL bDont)
{
	return SSO_Linger(sock, 0, 0);
}

int SSO_Linger(SOCKET sock, int l_onoff, int l_linger)
{
	linger ln = {l_onoff, l_linger};
	return setsockopt(sock, SOL_SOCKET, SO_LINGER, &ln, sizeof(linger));
}

int SSO_KeepAlive(SOCKET sock, BOOL bKeepAlive)
{
	int val = bKeepAlive ? 1 : 0;
	return setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(int));
}

int SSO_KeepAliveVals(SOCKET sock, BOOL bOnOff, DWORD dwIdle, DWORD dwInterval, DWORD dwCount)
{
	if(bOnOff)
	{
		dwIdle		/= 1000;
		dwInterval	/= 1000;

		if(dwIdle == 0 || dwInterval == 0 || dwCount == 0)
		{
			::WSASetLastError(EINVAL);
			return SOCKET_ERROR;
		}
	}

	BOOL isOK = IS_NO_ERROR(SSO_KeepAlive(sock, bOnOff));

	if(isOK && bOnOff)
	{
		isOK &= IS_NO_ERROR(setsockopt(sock, SOL_TCP, TCP_KEEPIDLE, &dwIdle, sizeof(DWORD)));
		isOK &= IS_NO_ERROR(setsockopt(sock, SOL_TCP, TCP_KEEPINTVL, &dwInterval, sizeof(DWORD)));
		isOK &= IS_NO_ERROR(setsockopt(sock, SOL_TCP, TCP_KEEPCNT, &dwCount, sizeof(DWORD)));
	}

	return isOK ? NO_ERROR : SOCKET_ERROR;
}

int SSO_ReuseAddress(SOCKET sock, BOOL bReuse)
{
	int val = bReuse ? 1 : 0;
	return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int));
}

int SSO_RecvBuffSize(SOCKET sock, int size)
{
	return setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof(int));
}

int SSO_SendBuffSize(SOCKET sock, int size)
{
	return setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof(int));
}

int SSO_RecvTimeout(SOCKET sock, int sec, int microsec)
{
	timeval tv = {sec, microsec};
	return setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(timeval));
}

int SSO_SendTimeout(SOCKET sock, int sec, int microsec)
{
	timeval tv = {sec, microsec};
	return setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(timeval));
}

int SSO_GetError(SOCKET sock)
{
	int e;
	socklen_t len = sizeof(e);

	if(IS_NO_ERROR(getsockopt(sock, SOL_SOCKET, SO_ERROR, &e, &len)))
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
