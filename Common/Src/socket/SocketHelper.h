/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.1.3
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
 
#pragma once

#include <ws2tcpip.h>
#include <mswsock.h>
#include <malloc.h>

#include "SocketInterface.h"
#include"../WaitFor.h"

/************************************************************************
名称：Windows Socket 组件初始化类
描述：自动加载和卸载 Windows Socket 组件
************************************************************************/
class CInitSocket
{
public:
	CInitSocket(LPWSADATA lpWSAData = nullptr, BYTE minorVersion = 2, BYTE majorVersion = 2)
	{
		LPWSADATA lpTemp = lpWSAData;
		if(!lpTemp)
			lpTemp	= (LPWSADATA)_alloca(sizeof(WSADATA));

		m_iResult	= ::WSAStartup(MAKEWORD(minorVersion, majorVersion), lpTemp);
	}

	~CInitSocket()
	{
		if(IsValid())
			::WSACleanup();
	}

	int		GetResult()	{return m_iResult;}
	BOOL	IsValid()	{return m_iResult == 0;}

private:
	int		m_iResult;
};

enum EnSocketCloseFlag
{
	SCF_NONE	= 0,
	SCF_CLOSE	= 1,
	SCF_ERROR	= 2
};

/* 数据缓冲区基础结构 */
struct TBufferObjBase
{
	OVERLAPPED			ov;
	WSABUF				buff;
	EnSocketOperation	operation;
};

/* 数据缓冲区结构 */
struct TBufferObj : public TBufferObjBase
{
	SOCKET client;
};

/* UDP 数据缓冲区结构 */
struct TUdpBufferObj : public TBufferObjBase
{
	SOCKADDR_IN	clientAddr;
	int			addrLen;
};

/* 数据缓冲区结构链表 */
typedef list<TBufferObj*>		TBufferObjPtrList;

/* Udp 数据缓冲区结构链表 */
typedef list<TUdpBufferObj*>	TUdpBufferObjPtrList;

/* Socket 缓冲区基础结构 */
struct TSocketObjBase
{
	CONNID		connID;
	SOCKADDR_IN	clientAddr;
	PVOID		extra;
	BOOL		valid;

	union
	{
		DWORD	freeTime;
		DWORD	connTime;
	};

	static BOOL IsExist(TSocketObjBase* pSocketObj)
		{return pSocketObj != nullptr;}

	static BOOL IsValid(TSocketObjBase* pSocketObj)
		{return IsExist(pSocketObj) && pSocketObj->valid == TRUE;}

	static void Invalid(TSocketObjBase* pSocketObj)
		{ASSERT(IsExist(pSocketObj)); pSocketObj->valid = FALSE;}

	void Reset(CONNID dwConnID)
	{
		connID	= dwConnID;
		valid	= TRUE;
		extra	= nullptr;
	}
};

/* 数据缓冲区结构 */
struct TSocketObj : public TSocketObjBase
{
	SOCKET		socket;
	CCriSec		crisec;

	void Reset(CONNID dwConnID, SOCKET soClient)
	{
		__super::Reset(dwConnID);
		socket = soClient;
	}
};

/* UDP 数据缓冲区结构 */
struct TUdpSocketObj : public TSocketObjBase
{
	volatile DWORD	detectFails;

	void Reset(CONNID dwConnID)
	{
		__super::Reset(dwConnID);
		detectFails = 0;
	}
};

/* 数据缓冲区结构链表 */
typedef list<TSocketObj*>					TSocketObjPtrList;
/* 数据缓冲区结构哈希表 */
typedef hash_map<CONNID, TSocketObj*>		TSocketObjPtrMap;
/* 数据缓冲区结构哈希表迭代器 */
typedef TSocketObjPtrMap::iterator			TSocketObjPtrMapI;
/* 数据缓冲区结构哈希表 const 迭代器 */
typedef TSocketObjPtrMap::const_iterator	TSocketObjPtrMapCI;

/* UDP 数据缓冲区结构链表 */
typedef list<TUdpSocketObj*>				TUdpSocketObjPtrList;
/* UDP 数据缓冲区结构哈希表 */
typedef hash_map<CONNID, TUdpSocketObj*>	TUdpSocketObjPtrMap;
/* UDP 数据缓冲区结构哈希表迭代器 */
typedef TUdpSocketObjPtrMap::iterator		TUdpSocketObjPtrMapI;
/* UDP 数据缓冲区结构哈希表 const 迭代器 */
typedef TUdpSocketObjPtrMap::const_iterator	TUdpSocketObjPtrMapCI;

/* SOCKADDR_IN 比较器 */
template<size_t bk_size = 4, size_t min_bks = 8>
struct sockaddr_hash_func
{
	enum
	{
		bucket_size	= bk_size,
		min_buckets	= min_bks,
	};

	//HASH_MAP函数。
	size_t operator() (const SOCKADDR_IN* pA) const
	{
		return	(	(pA->sin_family << 16) | ntohs(pA->sin_port)) ^
				(	(pA->sin_addr.s_net << 24)	| 
					(pA->sin_addr.s_host << 16)	| 
					(pA->sin_addr.s_lh << 8)	| 
					pA->sin_addr.s_impno		);
	}

	//比较函数。
	bool operator() (const SOCKADDR_IN* pA, const SOCKADDR_IN* pB) const
	{
		return memcmp(pA, pB, offsetof(SOCKADDR_IN, sin_zero)) < 0;
	}
};

/* 地址-连接 ID 哈希表 */
typedef hash_map<SOCKADDR_IN*, CONNID, sockaddr_hash_func<>>	TSockAddrMap;
/* 地址-连接 ID 哈希表迭代器 */
typedef TSockAddrMap::iterator									TSockAddrMapI;
/* 地址-连接 ID 哈希表 const 迭代器 */
typedef TSockAddrMap::const_iterator							TSockAddrMapCI;

/*****************************************************************************************************/
/******************************************** 公共帮助方法 ********************************************/
/*****************************************************************************************************/

/* 检查字符串是否符合 IP 地址格式 */
BOOL IsIPAddress(LPCTSTR lpszAddress);
/* 通过主机名获取 IP 地址 */
BOOL GetIPAddress(LPCTSTR lpszHost, LPTSTR lpszIP, int& iIPLenth);
/* 通过主机名获取最优的 IP 地址 */
BOOL GetOptimalIPByHostName(LPCTSTR lpszHost, IN_ADDR& addr);
/* 获取 SOCKADDR_IN 结构的 IP 地址 */
BOOL sockaddr_IN_2_IP(const IN_ADDR& addr, LPTSTR lpszAddress, int& iAddressLen);
/* 把 SOCKADDR_IN 结构转换为地址数据 */
BOOL sockaddr_IN_2_A(const SOCKADDR_IN& addr, ADDRESS_FAMILY& usFamily, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort);
/* 把地址数据转换为 SOCKADDR_IN 结构 */
BOOL sockaddr_A_2_IN(ADDRESS_FAMILY usFamily, LPCTSTR pszAddress, USHORT usPort, SOCKADDR_IN& addr);
/* 获取 Socket 的本地或远程地址信息 */
BOOL GetSocketAddress(SOCKET socket, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort, BOOL bLocal = TRUE);
/* 获取 Socket 的本地地址信息 */
BOOL GetSocketLocalAddress(SOCKET socket, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort);
/* 获取 Socket 的远程地址信息 */
BOOL GetSocketRemoteAddress(SOCKET socket, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort);

/* 获取 Socket 的某个扩展函数的指针 */
PVOID GetExtensionFuncPtr					(SOCKET sock, GUID guid);
/* 获取 AcceptEx 扩展函数指针 */
LPFN_ACCEPTEX Get_AcceptEx_FuncPtr			(SOCKET sock);
/* 获取 GetAcceptExSockaddrs 扩展函数指针 */
LPFN_GETACCEPTEXSOCKADDRS Get_GetAcceptExSockaddrs_FuncPtr(SOCKET sock);
/* 获取 ConnectEx 扩展函数指针 */
LPFN_CONNECTEX Get_ConnectEx_FuncPtr		(SOCKET sock);
/* 获取 TransmitFile 扩展函数指针 */
LPFN_TRANSMITFILE Get_TransmitFile_FuncPtr	(SOCKET sock);
/* 获取 DisconnectEx 扩展函数指针 */
LPFN_DISCONNECTEX Get_DisconnectEx_FuncPtr	(SOCKET sock);

/************************************************************************
名称：setsockopt() 帮助方法
描述：简化常用的 setsockopt() 调用
************************************************************************/

int SSO_UpdateAcceptContext	(SOCKET soClient, SOCKET soBind);
int SSO_UpdateConnectContext(SOCKET soClient, int iOption);
int SSO_NoDelay				(SOCKET sock, BOOL bNoDelay = TRUE);
int SSO_DontLinger			(SOCKET sock, BOOL bDont = TRUE);
int SSO_Linger				(SOCKET sock, USHORT l_onoff, USHORT l_linger);
int SSO_KeepAlive			(SOCKET sock, BOOL bKeepAlive = TRUE);
int SSO_KeepAliveVals		(SOCKET sock, u_long onoff, u_long time, u_long interval);
int SSO_RecvBuffSize		(SOCKET sock, int size);
int SSO_SendBuffSize		(SOCKET sock, int size);
int SSO_ReuseAddress		(SOCKET sock, BOOL bReuse = TRUE);
int SSO_UDP_ConnReset		(SOCKET sock, BOOL bNewBehavior = TRUE);

/************************************************************************
名称：Socket 操作方法
描述：Socket 操作包装方法
************************************************************************/

/* 生成 Connection ID */
CONNID GenerateConnectionID	(volatile CONNID& dwSeed);
/* 关闭 Socket */
int ManualCloseSocket		(SOCKET sock, int iShutdownFlag = 0xFF, BOOL bGraceful = TRUE, BOOL bReuseAddress = FALSE);
/* 投递 AccceptEx() */
int PostAccept				(LPFN_ACCEPTEX pfnAcceptEx, SOCKET soListen, SOCKET soClient, TBufferObj* pBufferObj);
/* 投递 WSASend() */
int PostSend				(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* 投递 WSARecv() */
int PostReceive				(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* 投递 WSASendTo() */
int PostSendTo				(SOCKET sock, TUdpBufferObj* pBufferObj);
/* 投递 WSARecvFrom() */
int PostReceiveFrom			(SOCKET sock, TUdpBufferObj* pBufferObj);
