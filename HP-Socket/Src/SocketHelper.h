/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 4.3.1
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
 
#pragma once

#include <ws2tcpip.h>
#include <mswsock.h>
#include <malloc.h>

#include <atlfile.h>

#include "SocketInterface.h"
#include "../../Common/Src/WaitFor.h"
#include "../../Common/Src/bufferpool.h"
#include "../../Common/Src/RingBuffer.h"

/************************************************************************
名称：全局常量
描述：声明组件的公共全局常量
************************************************************************/

/* HP-Socket 版本号 */
#define HP_VERSION_MAJOR						4
#define HP_VERSION_MINOR						3
#define HP_VERSION_REVISE						1
#define HP_VERSION_BUILD						2

/* IOCP 最大工作线程数 */
#define MAX_WORKER_THREAD_COUNT					500
/* IOCP Socket 缓冲区最小值 */
#define MIN_SOCKET_BUFFER_SIZE					64
/* 小文件最大字节数 */
#define MAX_SMALL_FILE_SIZE						0x3FFFFF
/* 最大连接时长 */
#define MAX_CONNECTION_PERIOD					(MAXLONG / 2)

/* Server/Agent 默认最大连接数 */
#define DEFAULT_MAX_CONNECTION_COUNT			10000
/* Server/Agent 默认 IOCP 工作线程数 */
#define DEFAULT_WORKER_THREAD_COUNT				GetDefaultWorkerThreadCount()
/* Server/Agent 默认 Socket 缓存对象锁定时间 */
#define DEFAULT_FREE_SOCKETOBJ_LOCK_TIME		(10 * 1000)
/* Server/Agent 默认 Socket 缓存池大小 */
#define DEFAULT_FREE_SOCKETOBJ_POOL				150
/* Server/Agent 默认 Socket 缓存池回收阀值 */
#define DEFAULT_FREE_SOCKETOBJ_HOLD				600
/* Server/Agent 默认内存块缓存池大小 */
#define DEFAULT_FREE_BUFFEROBJ_POOL				300
/* Server/Agent 默认内存块缓存池回收阀值 */
#define DEFAULT_FREE_BUFFEROBJ_HOLD				1200
/* Client 默认内存块缓存池大小 */
#define DEFAULT_CLIENT_FREE_BUFFER_POOL_SIZE	10
/* Client 默认内存块缓存池回收阀值 */
#define DEFAULT_CLIENT_FREE_BUFFER_POOL_HOLD	40
/* Agent 默认绑定地址 */
#define  DEFAULT_BIND_ADDRESS					_T("0.0.0.0")

/* TCP 默认通信数据缓冲区大小 */
#define DEFAULT_TCP_SOCKET_BUFFER_SIZE			GetDefaultTcpSocketBufferSize()
/* TCP 默认心跳包间隔 */
#define DEFALUT_TCP_KEEPALIVE_TIME				(30 * 1000)
/* TCP 默认心跳确认包检测间隔 */
#define DEFALUT_TCP_KEEPALIVE_INTERVAL			(10 * 1000)
/* TCP Server 默认 Listen 队列大小 */
#define DEFAULT_TCP_SERVER_SOCKET_LISTEN_QUEUE	SOMAXCONN
/* TCP Server 默认预投递 Accept 数量 */
#define DEFAULT_TCP_SERVER_ACCEPT_SOCKET_COUNT	300

/* UDP 默认数据报文最大长度 */
#define DEFAULT_UDP_MAX_DATAGRAM_SIZE			1472
/* UDP 默认 Receive 预投递数量 */
#define DEFAULT_UDP_POST_RECEIVE_COUNT			300
/* UDP 默认监测包尝试次数 */
#define DEFAULT_UDP_DETECT_ATTEMPTS				3
/* UDP 默认监测包发送间隔 */
#define DEFAULT_UDP_DETECT_INTERVAL				20

/* TCP Pack 包长度位数 */
#define TCP_PACK_LENGTH_BITS					22
/* TCP Pack 包长度掩码 */
#define TCP_PACK_LENGTH_MASK					0x3FFFFF
/* TCP Pack 包最大长度硬限制 */
#define TCP_PACK_MAX_SIZE_LIMIT					0x3FFFFF
/* TCP Pack 包默认最大长度 */
#define TCP_PACK_DEFAULT_MAX_SIZE				0x040000
/* TCP Pack 包头标识值硬限制 */
#define TCP_PACK_HEADER_FLAG_LIMIT				0x0003FF
/* TCP Pack 包头默认标识值 */
#define TCP_PACK_DEFAULT_HEADER_FLAG			0x000000

DWORD GetDefaultWorkerThreadCount();
DWORD GetDefaultTcpSocketBufferSize();

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

		m_iResult	= ::WSAStartup(MAKEWORD(majorVersion, minorVersion), lpTemp);
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

/* Server 组件和 Agent 组件内部使用的事件处理结果常量 */

// 连接已关闭
#define HR_CLOSED	0xFF

/* 关闭连接标识 */
enum EnSocketCloseFlag
{
	SCF_NONE	= 0,	// 不触发事件
	SCF_CLOSE	= 1,	// 触发 正常关闭 OnClose 事件
	SCF_ERROR	= 2		// 触发 异常关闭 OnClose 事件
};

/* 数据缓冲区基础结构 */
template<class T> struct TBufferObjBase
{
	WSAOVERLAPPED		ov;
	EnSocketOperation	operation;
	WSABUF				buff;

	int					capacity;
	CPrivateHeap&		heap;

	T* next;
	T* last;

	static T* Construct(CPrivateHeap& heap, DWORD dwCapacity)
	{
		T* pBufferObj = (T*)heap.Alloc(sizeof(T) + dwCapacity);
		ASSERT(pBufferObj);

		ZeroMemory(pBufferObj, sizeof(T));
		
		pBufferObj->TBufferObjBase::TBufferObjBase(heap, dwCapacity);
		pBufferObj->buff.buf = ((char*)pBufferObj) + sizeof(T);

		return pBufferObj;
	}

	static void Destruct(T* pBufferObj)
	{
		ASSERT(pBufferObj);
		pBufferObj->heap.Free(pBufferObj);
	}

	TBufferObjBase(CPrivateHeap& hp, DWORD dwCapacity)
	: heap(hp)
	, capacity((int)dwCapacity)
	{
		ASSERT(capacity > 0);
	}

	int Cat(const BYTE* pData, int length)
	{
		ASSERT(pData != nullptr && length > 0);

		int cat = min(Remain(), length);

		if(cat > 0)
		{
			memcpy(buff.buf + buff.len, pData, cat);
			buff.len += cat;
		}

		return cat;
	}

	void Reset()	{buff.len = 0;}
	int Remain()	{return capacity - buff.len;}
	BOOL IsFull()	{return buff.len == capacity;}
};

/* 数据缓冲区结构 */
struct TBufferObj : public TBufferObjBase<TBufferObj>
{
	SOCKET client;
};

/* UDP 数据缓冲区结构 */
struct TUdpBufferObj : public TBufferObjBase<TUdpBufferObj>
{
	SOCKADDR_IN	remoteAddr;
	int			addrLen;
};

/* 数据缓冲区链表模板 */
template<class T> struct TBufferObjListT : public TSimpleList<T>
{
public:
	int Cat(const BYTE* pData, int length)
	{
		ASSERT(pData != nullptr && length > 0);

		int remain = length;

		while(remain > 0)
		{
			T* pItem = Back();

			if(pItem == nullptr || pItem->IsFull())
				pItem = PushBack(bfPool.PickFreeItem());

			int cat  = pItem->Cat(pData, remain);

			pData	+= cat;
			remain	-= cat;
		}

		return length;
	}

	T* PushTail(const BYTE* pData, int length)
	{
		ASSERT(pData != nullptr && length > 0 && length <= (int)bfPool.GetItemCapacity());

		T* pItem = PushBack(bfPool.PickFreeItem());
		pItem->Cat(pData, length);

		return pItem;
	}

	void Release()
	{
		bfPool.PutFreeItem(*this);
	}

public:
	TBufferObjListT(CNodePoolT<T>& pool) : bfPool(pool)
	{
	}

private:
	CNodePoolT<T>& bfPool;
};

/* 数据缓冲区对象池 */
typedef CNodePoolT<TBufferObj>			CBufferObjPool;
/* UDP 数据缓冲区对象池 */
typedef CNodePoolT<TUdpBufferObj>		CUdpBufferObjPool;
/* 数据缓冲区链表模板 */
typedef TBufferObjListT<TBufferObj>		TBufferObjList;
/* UDP 数据缓冲区链表模板 */
typedef TBufferObjListT<TUdpBufferObj>	TUdpBufferObjList;

/* 数据缓冲区结构链表 */
typedef CRingPool<TBufferObj>		TBufferObjPtrList;

/* Udp 数据缓冲区结构链表 */
typedef CRingPool<TUdpBufferObj>	TUdpBufferObjPtrList;

/* Socket 缓冲区基础结构 */
struct TSocketObjBase
{
	static const long DEF_SNDBUFF_SIZE = 8192;

	CONNID		connID;
	SOCKADDR_IN	remoteAddr;
	PVOID		extra;
	PVOID		reserved;
	PVOID		reserved2;
	BOOL		valid;

	union
	{
		DWORD	freeTime;
		DWORD	connTime;
	};

	DWORD		activeTime;

	CCriSec		csSend;

	long			sndBuffSize;
	volatile BOOL	smooth;
	volatile long	pending;
	volatile long	sndCount;

	CReentrantSpinGuard csRecv;

	static BOOL IsExist(TSocketObjBase* pSocketObj)
		{return pSocketObj != nullptr;}

	static BOOL IsValid(TSocketObjBase* pSocketObj)
		{return pSocketObj != nullptr && pSocketObj->valid;}

	static void Invalid(TSocketObjBase* pSocketObj)
		{ASSERT(IsExist(pSocketObj)); pSocketObj->valid = FALSE;}

	static void Release(TSocketObjBase* pSocketObj)
		{ASSERT(IsExist(pSocketObj)); pSocketObj->freeTime = ::TimeGetTime();}

	long Pending()		{return pending;}
	BOOL IsPending()	{return pending > 0;}
	BOOL IsCanSend()	{return sndCount <= sndBuffSize;}
	BOOL IsSmooth()		{return smooth;}
	void TurnOnSmooth()	{smooth = TRUE;}

	BOOL TurnOffSmooth()
		{return ::InterlockedCompareExchange((volatile long*)&smooth, FALSE, TRUE) == TRUE;}
	
	BOOL ResetSndBuffSize(SOCKET socket)
	{
		int len = (int)(sizeof(sndBuffSize));
		return getsockopt(socket, SOL_SOCKET, SO_SNDBUF, (CHAR*)&sndBuffSize, &len) != 0;
	}

	void Reset(CONNID dwConnID)
	{
		connID		= dwConnID;
		valid		= TRUE;
		smooth		= TRUE;
		pending		= 0;
		sndCount	= 0;
		sndBuffSize	= DEF_SNDBUFF_SIZE;
		extra		= nullptr;
		reserved	= nullptr;
		reserved2	= nullptr;
	}
};

/* 数据缓冲区结构 */
struct TSocketObj : public TSocketObjBase
{
	SOCKET			socket;
	CStringA		host;
	TBufferObjList	sndBuff;
	
	TSocketObj(CBufferObjPool& bfPool)
	: sndBuff(bfPool)
	{

	}

	static void Release(TSocketObj* pSocketObj)
	{
		__super::Release(pSocketObj);

		pSocketObj->sndBuff.Release();
	}

	void Reset(CONNID dwConnID, SOCKET soClient)
	{
		__super::Reset(dwConnID);
		
		host.Empty();

		socket = soClient;
	}

	BOOL GetRemoteHost(LPCSTR* lpszHost, USHORT* pusPort = nullptr)
	{
		*lpszHost = host;

		if(pusPort)
			*pusPort = ntohs(remoteAddr.sin_port);

		return (*lpszHost != nullptr && (*lpszHost)[0] != 0);
	}
};

/* UDP 数据缓冲区结构 */
struct TUdpSocketObj : public TSocketObjBase
{
	TUdpBufferObjList	sndBuff;
	volatile DWORD		detectFails;

	TUdpSocketObj(CUdpBufferObjPool& bfPool)
	: sndBuff(bfPool)
	{

	}

	static void Release(TUdpSocketObj* pSocketObj)
	{
		__super::Release(pSocketObj);

		pSocketObj->sndBuff.Release();
	}

	void Reset(CONNID dwConnID)
	{
		__super::Reset(dwConnID);
		detectFails = 0;
	}
};

/* 有效 TSocketObj 缓存 */
typedef CRingCache2<TSocketObj, CONNID, true>		TSocketObjPtrPool;
/* 失效 TSocketObj 缓存 */
typedef CRingPool<TSocketObj>						TSocketObjPtrList;
/* 失效 TSocketObj 垃圾回收结构链表 */
typedef CCASQueue<TSocketObj>						TSocketObjPtrQueue;

/* 有效 TUdpSocketObj 缓存 */
typedef CRingCache2<TUdpSocketObj, CONNID, true>	TUdpSocketObjPtrPool;
/* 失效 TUdpSocketObj 缓存 */
typedef CRingPool<TUdpSocketObj>					TUdpSocketObjPtrList;
/* 失效 TUdpSocketObj 垃圾回收结构链表 */
typedef CCASQueue<TUdpSocketObj>					TUdpSocketObjPtrQueue;

/* SOCKADDR_IN 比较器 */
struct sockaddr_func
{
	struct hash
	{
		size_t operator() (const SOCKADDR_IN* pA) const
		{
			return ((pA->sin_family << 16) | pA->sin_port) ^ pA->sin_addr.s_addr;
		}
	};

	struct equal_to
	{
		bool operator () (const SOCKADDR_IN* pA, const SOCKADDR_IN* pB) const
		{
			return memcmp(pA, pB, offsetof(SOCKADDR_IN, sin_zero)) == 0;
		}
	};

};

/* 地址-连接 ID 哈希表 */
typedef unordered_map<SOCKADDR_IN*, CONNID, sockaddr_func::hash, sockaddr_func::equal_to>
										TSockAddrMap;
/* 地址-连接 ID 哈希表迭代器 */
typedef TSockAddrMap::iterator			TSockAddrMapI;
/* 地址-连接 ID 哈希表 const 迭代器 */
typedef TSockAddrMap::const_iterator	TSockAddrMapCI;

/* IClient 组件关闭上下文 */
struct TClientCloseContext
{
	BOOL bFireOnClose;
	EnSocketOperation enOperation;
	int iErrorCode;

	TClientCloseContext(BOOL bFire = TRUE, EnSocketOperation enOp = SO_CLOSE, int iCode = SE_OK)
	{
		Reset(bFire, enOp, iCode);
	}

	void Reset(BOOL bFire = TRUE, EnSocketOperation enOp = SO_CLOSE, int iCode = SE_OK)
	{
		bFireOnClose = bFire;
		enOperation	 = enOp;
		iErrorCode	 = iCode;
	}

};

/*****************************************************************************************************/
/******************************************** 公共帮助方法 ********************************************/
/*****************************************************************************************************/

// 获取 HPSocket 版本号（4 个字节分别为：主版本号，子版本号，修正版本号，构建编号）
DWORD GetHPSocketVersion();

/* 获取错误描述文本 */
LPCTSTR GetSocketErrorDesc(EnSocketError enCode);
/* IPv4 字符串地址转换为整数 */
ULONG GetIPv4InAddr(LPCTSTR lpszAddress);
/* 检查字符串是否符合 IP 地址格式 */
BOOL IsIPAddress(LPCTSTR lpszAddress);
/* 通过主机名获取 IP 地址 */
BOOL GetIPAddress(LPCTSTR lpszHost, __out LPTSTR lpszIP, __inout int& iIPLenth);
/* 通过主机名获取最优的 IP 地址 */
BOOL GetOptimalIPByHostName(LPCTSTR lpszHost, __out IN_ADDR& addr);
/* 获取 IN_ADDR 结构的 IP 地址 */
BOOL IN_ADDR_2_IP(const IN_ADDR& addr, __out LPTSTR lpszAddress, __inout int& iAddressLen);
/* 把 SOCKADDR_IN 结构转换为地址数据 */
BOOL sockaddr_IN_2_A(const SOCKADDR_IN& addr, __out ADDRESS_FAMILY& usFamily, __out LPTSTR lpszAddress, __inout int& iAddressLen, __out USHORT& usPort);
/* 把地址数据转换为 SOCKADDR_IN 结构 */
BOOL sockaddr_A_2_IN(ADDRESS_FAMILY usFamily, LPCTSTR lpszAddress, USHORT usPort, __out SOCKADDR_IN& addr);
/* 获取 Socket 的本地或远程地址信息 */
BOOL GetSocketAddress(SOCKET socket, __out LPTSTR lpszAddress, __inout int& iAddressLen, __out USHORT& usPort, BOOL bLocal = TRUE);
/* 获取 Socket 的本地地址信息 */
BOOL GetSocketLocalAddress(SOCKET socket, __out LPTSTR lpszAddress, __inout int& iAddressLen, __out USHORT& usPort);
/* 获取 Socket 的远程地址信息 */
BOOL GetSocketRemoteAddress(SOCKET socket, __out LPTSTR lpszAddress, __inout int& iAddressLen, __out USHORT& usPort);

/* 64 位网络字节序转主机字节序 */
ULONGLONG NToH64(ULONGLONG value);
/* 64 位主机字节序转网络字节序 */
ULONGLONG HToN64(ULONGLONG value);

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

HRESULT ReadSmallFile(LPCTSTR lpszFileName, CAtlFile& file, CAtlFileMapping<>& fmap, DWORD dwMaxFileSize = MAX_SMALL_FILE_SIZE);
HRESULT MakeSmallFilePackage(LPCTSTR lpszFileName, CAtlFile& file, CAtlFileMapping<>& fmap, WSABUF szBuf[3], const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr);

/************************************************************************
名称：IOCP 指令投递帮助方法
描述：简化 IOCP 指令投递
************************************************************************/

/* IOCP 命令 */
enum EnIocpCommand
{
	IOCP_CMD_EXIT		= 0x00000000,	// 退出程序
	IOCP_CMD_ACCEPT		= 0xFFFFFFF1,	// 接受连接
	IOCP_CMD_DISCONNECT	= 0xFFFFFFF2,	// 断开连接
	IOCP_CMD_SEND		= 0xFFFFFFF3	// 发送数据
};

/* IOCP 命令处理动作 */
enum EnIocpAction
{
	IOCP_ACT_GOON		= 0,	// 继续执行
	IOCP_ACT_CONTINUE	= 1,	// 重新执行
	IOCP_ACT_BREAK		= 2		// 中断执行
};

BOOL PostIocpCommand(HANDLE hIOCP, EnIocpCommand enCmd, ULONG_PTR ulParam);
BOOL PostIocpExit(HANDLE hIOCP);
BOOL PostIocpAccept(HANDLE hIOCP);
BOOL PostIocpDisconnect(HANDLE hIOCP, CONNID dwConnID);
BOOL PostIocpSend(HANDLE hIOCP, CONNID dwConnID);
BOOL PostIocpClose(HANDLE hIOCP, CONNID dwConnID, int iErrorCode);

/************************************************************************
名称：setsockopt() 帮助方法
描述：简化常用的 setsockopt() 调用
************************************************************************/

int SSO_SetSocketOption		(SOCKET sock, int level, int name, LPVOID val, int len);
int SSO_GetSocketOption		(SOCKET sock, int level, int name, LPVOID val, int* len);
int SSO_IoctlSocket			(SOCKET sock, long cmd, u_long* arg);
int SSO_WSAIoctl			(SOCKET sock, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer, LPDWORD lpcbBytesReturned);

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

/* 检测 IOCP 操作返回值：NO_ERROR 则返回 TRUE */
#define IOCP_NO_ERROR(result)	(result == NO_ERROR)
/* 检测 IOCP 操作返回值：WSA_IO_PENDING 则返回 TRUE */
#define IOCP_PENDING(result)	(result == WSA_IO_PENDING)
/* 检测 IOCP 操作返回值：NO_ERROR 或 WSA_IO_PENDING 则返回 TRUE */
#define IOCP_SUCCESS(result)	(IOCP_NO_ERROR(result) || IOCP_PENDING(result))

/* 生成 Connection ID */
CONNID GenerateConnectionID	();
/* 关闭 Socket */
int ManualCloseSocket		(SOCKET sock, int iShutdownFlag = 0xFF, BOOL bGraceful = TRUE, BOOL bReuseAddress = FALSE);
/* 投递 AccceptEx()，并把 WSA_IO_PENDING 转换为 NO_ERROR */
int PostAccept				(LPFN_ACCEPTEX pfnAcceptEx, SOCKET soListen, SOCKET soClient, TBufferObj* pBufferObj);
/* 投递 AccceptEx() */
int PostAcceptNotCheck		(LPFN_ACCEPTEX pfnAcceptEx, SOCKET soListen, SOCKET soClient, TBufferObj* pBufferObj);
/* 投递 ConnectEx()，并把 WSA_IO_PENDING 转换为 NO_ERROR */
int PostConnect				(LPFN_CONNECTEX pfnConnectEx, SOCKET soClient, SOCKADDR_IN& soAddrIN, TBufferObj* pBufferObj);
/* 投递 ConnectEx() */
int PostConnectNotCheck		(LPFN_CONNECTEX pfnConnectEx, SOCKET soClient, SOCKADDR_IN& soAddrIN, TBufferObj* pBufferObj);
/* 投递 WSASend()，并把 WSA_IO_PENDING 转换为 NO_ERROR */
int PostSend				(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* 投递 WSASend() */
int PostSendNotCheck		(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* 投递 WSARecv() ，并把 WSA_IO_PENDING 转换为 NO_ERROR*/
int PostReceive				(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* 投递 WSARecv() */
int PostReceiveNotCheck		(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* 投递 WSASendTo() ，并把 WSA_IO_PENDING 转换为 NO_ERROR*/
int PostSendTo				(SOCKET sock, TUdpBufferObj* pBufferObj);
/* 投递 WSASendTo() */
int PostSendToNotCheck		(SOCKET sock, TUdpBufferObj* pBufferObj);
/* 投递 WSARecvFrom() ，并把 WSA_IO_PENDING 转换为 NO_ERROR*/
int PostReceiveFrom			(SOCKET sock, TUdpBufferObj* pBufferObj);
/* 投递 WSARecvFrom() */
int PostReceiveFromNotCheck	(SOCKET sock, TUdpBufferObj* pBufferObj);
