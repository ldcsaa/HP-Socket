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

#pragma once

#include "HPTypeDef.h"
#include "SocketInterface.h"
#include "common/StringT.h"
#include "common/SysHelper.h"
#include "common/BufferPtr.h"
#include "common/BufferPool.h"
#include "common/RingBuffer.h"
#include "common/FileHelper.h"

#include <netdb.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#ifdef _ZLIB_SUPPORT
#include <zlib.h>
#endif

using ADDRESS_FAMILY	= sa_family_t;
using IN_ADDR			= in_addr;
using IN6_ADDR			= in6_addr;
using SOCKADDR			= sockaddr;
using SOCKADDR_IN		= sockaddr_in;
using SOCKADDR_IN6		= sockaddr_in6;

/************************************************************************
名称：全局常量
描述：声明组件的公共全局常量
************************************************************************/

/* Socket 缓冲区最小值 */
#define MIN_SOCKET_BUFFER_SIZE					8
/* 小文件最大字节数 */
#define MAX_SMALL_FILE_SIZE						0x3FFFFF
/* 最大连接时长 */
#define MAX_CONNECTION_PERIOD					(MAXINT / 2)
/* 处理接收事件时最大读取次数 */
#define MAX_CONTINUE_READS						30
/* 处理发送事件时最大写入次数 */
#define MAX_CONTINUE_WRITES						50

/* 默认工作队列等待的最大描述符事件数量 */
#define DEFAULT_WORKER_MAX_EVENT_COUNT			CIODispatcher::DEF_WORKER_MAX_EVENTS

/* Server/Agent 默认最大连接数 */
#define DEFAULT_MAX_CONNECTION_COUNT			10000
/* Server/Agent 默认 Socket 缓存对象锁定时间 */
#define DEFAULT_FREE_SOCKETOBJ_LOCK_TIME		(30 * 1000)
/* Server/Agent 默认 Socket 缓存池大小 */
#define DEFAULT_FREE_SOCKETOBJ_POOL				600
/* Server/Agent 默认 Socket 缓存池回收阀值 */
#define DEFAULT_FREE_SOCKETOBJ_HOLD				600
/* Server/Agent 默认内存块缓存池大小 */
#define DEFAULT_FREE_BUFFEROBJ_POOL				1200
/* Server/Agent 默认内存块缓存池回收阀值 */
#define DEFAULT_FREE_BUFFEROBJ_HOLD				1200
/* Client 默认内存块缓存池大小 */
#define DEFAULT_CLIENT_FREE_BUFFER_POOL_SIZE	60
/* Client 默认内存块缓存池回收阀值 */
#define DEFAULT_CLIENT_FREE_BUFFER_POOL_HOLD	60
/* IPv4 默认绑定地址 */
#define  DEFAULT_IPV4_BIND_ADDRESS				_T("0.0.0.0")
/* IPv6 默认绑定地址 */
#define  DEFAULT_IPV6_BIND_ADDRESS				_T("::")

/* TCP 默认通信数据缓冲区大小 */
#define DEFAULT_TCP_SOCKET_BUFFER_SIZE			DEFAULT_BUFFER_SIZE
/* TCP 默认心跳包间隔 */
#define DEFALUT_TCP_KEEPALIVE_TIME				(60 * 1000)
/* TCP 默认心跳确认包检测间隔 */
#define DEFALUT_TCP_KEEPALIVE_INTERVAL			(20 * 1000)
/* TCP Server 默认 Listen 队列大小 */
#define DEFAULT_TCP_SERVER_SOCKET_LISTEN_QUEUE	SOMAXCONN

/* UDP 默认数据报文最大长度 */
#define DEFAULT_UDP_MAX_DATAGRAM_SIZE			1472
/* UDP 默认 Receive 预投递数量 */
#define DEFAULT_UDP_POST_RECEIVE_COUNT			DEFAULT_WORKER_MAX_EVENT_COUNT
/* UDP 默认监测包尝试次数 */
#define DEFAULT_UDP_DETECT_ATTEMPTS				3
/* UDP 默认监测包发送间隔 */
#define DEFAULT_UDP_DETECT_INTERVAL				60

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

#define PORT_SEPARATOR_CHAR						':'
#define IPV6_ADDR_BEGIN_CHAR					'['
#define IPV6_ADDR_END_CHAR						']'
#define IPV4_ADDR_SEPARATOR_CHAR				'.'
#define IPV6_ADDR_SEPARATOR_CHAR				':'
#define IPV6_ZONE_INDEX_CHAR					'%'

#define INVALID_SOCKET							INVALID_FD
#define SOCKET_ERROR							HAS_ERROR
#define WSASetLastError							SetLastError
#define WSAGetLastError							GetLastError
#define InetPton								inet_pton
#define InetNtop								inet_ntop
#define closesocket								close

typedef struct hp_addr
{
	ADDRESS_FAMILY family;

	union
	{
		ULONG_PTR	addr;
		IN_ADDR		addr4;
		IN6_ADDR	addr6;
	};

	static const hp_addr ANY_ADDR4;
	static const hp_addr ANY_ADDR6;

	inline int AddrSize() const
	{
		return AddrSize(family);
	}

	inline static int AddrSize(ADDRESS_FAMILY f)
	{
		if(f == AF_INET)
			return sizeof(IN_ADDR);

		return sizeof(IN6_ADDR);
	}

	inline static const hp_addr& AnyAddr(ADDRESS_FAMILY f)
	{
		if(f == AF_INET)
			return ANY_ADDR4;

		return ANY_ADDR6;
	}

	inline const ULONG_PTR* Addr()	const	{return &addr;}
	inline ULONG_PTR* Addr()				{return &addr;}

	inline BOOL IsIPv4()			const	{return family == AF_INET;}
	inline BOOL IsIPv6()			const	{return family == AF_INET6;}
	inline BOOL IsSpecified()		const	{return IsIPv4() || IsIPv6();}
	inline void ZeroAddr()					{::ZeroMemory(&addr6, sizeof(addr6));}
	inline void Reset()						{::ZeroMemory(this, sizeof(*this));}

	inline hp_addr& Copy(hp_addr& other) const
	{
		if(this != &other)
			memcpy(&other, this, offsetof(hp_addr, addr) + AddrSize());

		return other;
	}

	hp_addr(ADDRESS_FAMILY f = AF_UNSPEC, BOOL bZeroAddr = FALSE)
	{
		family = f;

		if(bZeroAddr) ZeroAddr();
	}

} HP_ADDR, *HP_PADDR;

typedef struct hp_sockaddr
{
	union
	{
		ADDRESS_FAMILY	family;
		SOCKADDR		addr;
		SOCKADDR_IN		addr4;
		SOCKADDR_IN6	addr6;
	};

	inline int AddrSize() const
	{
		return AddrSize(family);
	}

	inline static int AddrSize(ADDRESS_FAMILY f)
	{
		if(f == AF_INET)
			return sizeof(SOCKADDR_IN);

		return sizeof(SOCKADDR_IN6);
	}

	inline static const hp_sockaddr& AnyAddr(ADDRESS_FAMILY f)
	{
		static const hp_sockaddr s_any_addr4(AF_INET, TRUE);
		static const hp_sockaddr s_any_addr6(AF_INET6, TRUE);

		if(f == AF_INET)
			return s_any_addr4;

		return s_any_addr6;
	}

	inline static int AddrMinStrLength(ADDRESS_FAMILY f)
	{
		if(f == AF_INET)
			return 16;

		return 46;
	}

	inline BOOL IsIPv4()			const	{return family == AF_INET;}
	inline BOOL IsIPv6()			const	{return family == AF_INET6;}
	inline BOOL IsSpecified()		const	{return IsIPv4() || IsIPv6();}
	inline USHORT Port()			const	{return ntohs(addr4.sin_port);}
	inline void SetPort(USHORT usPort)		{addr4.sin_port = htons(usPort);}
	inline void* SinAddr()			const	{return IsIPv4() ? (void*)&addr4.sin_addr : (void*)&addr6.sin6_addr;}
	inline void* SinAddr()					{return IsIPv4() ? (void*)&addr4.sin_addr : (void*)&addr6.sin6_addr;}

	inline const SOCKADDR* Addr()	const	{return &addr;}
	inline SOCKADDR* Addr()					{return &addr;}
	inline void ZeroAddr()					{::ZeroMemory(((char*)this) + sizeof(family), sizeof(*this) - sizeof(family));}
	inline void Reset()						{::ZeroMemory(this, sizeof(*this));}

	inline hp_sockaddr& Copy(hp_sockaddr& other) const
	{
		if(this != &other)
			memcpy(&other, this, AddrSize());

		return other;
	}

	size_t Hash() const
	{
		ASSERT(IsSpecified());

		if(IsIPv4())
			return ((addr4.sin_family << 16) | addr4.sin_port) ^ addr4.sin_addr.s_addr;
		else
		{
			ULONG* p = (ULONG*)(((char*)this) + offsetof(SOCKADDR_IN6, sin6_addr));
			return ((addr6.sin6_family << 16) | addr6.sin6_port) ^ addr6.sin6_flowinfo ^ p[0] ^ p[1] ^ p[2] ^ p[3] ^ p[4];
		}
	}

	bool EqualTo(const hp_sockaddr& other) const
	{
		ASSERT(IsSpecified() && other.IsSpecified());

		if(IsIPv4())
			return EqualMemory(this, &other, offsetof(SOCKADDR_IN, sin_zero));
		else
			return EqualMemory(this, &other, sizeof(addr6));
	}

	hp_sockaddr(ADDRESS_FAMILY f = AF_UNSPEC, BOOL bZeroAddr = FALSE)
	{
		family = f;

		if(bZeroAddr) ZeroAddr();
	}

} HP_SOCKADDR, *HP_PSOCKADDR;

/* Server 组件和 Agent 组件内部使用的事件处理结果常量 */

// 连接已关闭
#define HR_CLOSED	0xFF

/* 命令类型 */
enum EnDispCmdType
{
	DISP_CMD_SEND		= 0x01,	// 发送数据
	DISP_CMD_RECEIVE	= 0x02,	// 接收数据
	DISP_CMD_UNPAUSE	= 0x03,	// 恢复接收数据
	DISP_CMD_DISCONNECT	= 0x04	// 断开连接
};

/* 关闭连接标识 */
enum EnSocketCloseFlag
{
	SCF_NONE	= 0,	// 不触发事件
	SCF_CLOSE	= 1,	// 触发 正常关闭 OnClose 事件
	SCF_ERROR	= 2		// 触发 异常关闭 OnClose 事件
};

/* 数据缓冲节点 */
typedef TItem			TBufferObj;
/* 数据缓冲区对象池 */
typedef CItemPool		CBufferObjPool;
/* 数据缓冲区链表模板 */
typedef TItemListExV	TBufferObjList;

/* 线程 ID - 接收缓冲区哈希表 */
typedef unordered_map<THR_ID, CBufferPtr*>	TReceiveBufferMap;
/* 线程 ID - 接收缓冲区哈希表迭代器 */
typedef TReceiveBufferMap::iterator			TReceiveBufferMapI;
/* 线程 ID - 接收缓冲区哈希表 const 迭代器 */
typedef TReceiveBufferMap::const_iterator	TReceiveBufferMapCI;

/* Socket 缓冲区基础结构 */
struct TSocketObjBase
{
	CPrivateHeap& heap;

	CONNID		connID;
	HP_SOCKADDR	remoteAddr;
	PVOID		extra;
	PVOID		reserved;
	PVOID		reserved2;
	BOOL		valid;
	DWORD		activeTime;

	union
	{
		DWORD	freeTime;
		DWORD	connTime;
	};

	volatile BOOL connected;
	volatile BOOL paused;

	TSocketObjBase(CPrivateHeap& hp) : heap(hp) {}

	static BOOL IsExist(TSocketObjBase* pSocketObj)
		{return pSocketObj != nullptr;}

	static BOOL IsValid(TSocketObjBase* pSocketObj)
		{return pSocketObj != nullptr && pSocketObj->valid;}

	static void Invalid(TSocketObjBase* pSocketObj)
		{ASSERT(IsExist(pSocketObj)); pSocketObj->valid = FALSE;}

	static void Release(TSocketObjBase* pSocketObj)
		{ASSERT(IsExist(pSocketObj)); pSocketObj->freeTime = ::TimeGetTime();}

	BOOL IsPaused		()	const	{return paused;}

	DWORD GetConnTime	()	const	{return connTime;}
	DWORD GetFreeTime	()	const	{return freeTime;}
	DWORD GetActiveTime	()	const	{return activeTime;}

	BOOL HasConnected()							{return connected;}
	void SetConnected(BOOL bConnected = TRUE)	{connected = bConnected;}

	void Reset(CONNID dwConnID)
	{
		connID		= dwConnID;
		connected	= FALSE;
		valid		= TRUE;
		paused		= FALSE;
		extra		= nullptr;
		reserved	= nullptr;
		reserved2	= nullptr;
	}
};

/* 数据缓冲区结构 */
struct TSocketObj : public TSocketObjBase
{
	using __super = TSocketObjBase;

	CReentrantCriSec	csIo;
	CReentrantCriSec	csSend;

	SOCKET				socket;
	TBufferObjList		sndBuff;

	static TSocketObj* Construct(CPrivateHeap& hp, CBufferObjPool& bfPool)
	{
		TSocketObj* pSocketObj = (TSocketObj*)hp.Alloc(sizeof(TSocketObj));
		ASSERT(pSocketObj);

		return new (pSocketObj) TSocketObj(hp, bfPool);
	}

	static void Destruct(TSocketObj* pSocketObj)
	{
		ASSERT(pSocketObj);

		CPrivateHeap& heap = pSocketObj->heap;
		pSocketObj->TSocketObj::~TSocketObj();
		heap.Free(pSocketObj);
	}
	
	TSocketObj(CPrivateHeap& hp, CBufferObjPool& bfPool)
	: __super(hp), sndBuff(bfPool)
	{

	}

	static void Release(TSocketObj* pSocketObj)
	{
		__super::Release(pSocketObj);
		pSocketObj->sndBuff.Release();
	}

	int Pending()		{return sndBuff.Length();}
	BOOL IsPending()	{return Pending() > 0;}

	static BOOL InvalidSocketObj(TSocketObj* pSocketObj)
	{
		BOOL bDone = FALSE;

		if(TSocketObjBase::IsValid(pSocketObj))
		{
			pSocketObj->SetConnected(FALSE);

			CReentrantCriSecLock locallock(pSocketObj->csIo);
			CReentrantCriSecLock locallock2(pSocketObj->csSend);

			if(TSocketObjBase::IsValid(pSocketObj))
			{
				TSocketObjBase::Invalid(pSocketObj);
				bDone = TRUE;
			}
		}

		return bDone;
	}

	void Reset(CONNID dwConnID, SOCKET soClient)
	{
		__super::Reset(dwConnID);
		
		socket = soClient;
	}
};

/* Agent 数据缓冲区结构 */
struct TAgentSocketObj : public TSocketObj
{
	using __super = TSocketObj;

	CStringA host;
	
	static TAgentSocketObj* Construct(CPrivateHeap& hp, CBufferObjPool& bfPool)
	{
		TAgentSocketObj* pSocketObj = (TAgentSocketObj*)hp.Alloc(sizeof(TAgentSocketObj));
		ASSERT(pSocketObj);

		return new (pSocketObj) TAgentSocketObj(hp, bfPool);
	}

	static void Destruct(TAgentSocketObj* pSocketObj)
	{
		ASSERT(pSocketObj);

		CPrivateHeap& heap = pSocketObj->heap;
		pSocketObj->TAgentSocketObj::~TAgentSocketObj();
		heap.Free(pSocketObj);
	}
	
	TAgentSocketObj(CPrivateHeap& hp, CBufferObjPool& bfPool)
	: __super(hp, bfPool)
	{

	}

	void Reset(CONNID dwConnID, SOCKET soClient)
	{
		__super::Reset(dwConnID, soClient);

		host.Empty();
	}

	BOOL GetRemoteHost(LPCSTR* lpszHost, USHORT* pusPort = nullptr)
	{
		*lpszHost = host;

		if(pusPort)
			*pusPort = remoteAddr.Port();

		return (!host.IsEmpty());
	}
};

/* UDP 数据缓冲区结构 */
struct TUdpSocketObj : public TSocketObjBase
{
	using __super		= TSocketObjBase;
	using CRecvQueue	= CCASQueue<TItem>;

	CBufferObjPool&		itPool;

	CSimpleRWLock		lcIo;
	CReentrantCriSec	csSend;

	TBufferObjList		sndBuff;
	CRecvQueue			recvQueue;

	volatile DWORD		detectFails;

	static TUdpSocketObj* Construct(CPrivateHeap& hp, CBufferObjPool& bfPool)
	{
		TUdpSocketObj* pSocketObj = (TUdpSocketObj*)hp.Alloc(sizeof(TUdpSocketObj));
		ASSERT(pSocketObj);

		return new (pSocketObj) TUdpSocketObj(hp, bfPool);
	}

	static void Destruct(TUdpSocketObj* pSocketObj)
	{
		ASSERT(pSocketObj);

		CPrivateHeap& heap = pSocketObj->heap;
		pSocketObj->TUdpSocketObj::~TUdpSocketObj();
		heap.Free(pSocketObj);
	}
	
	TUdpSocketObj(CPrivateHeap& hp, CBufferObjPool& bfPool)
	: __super(hp), sndBuff(bfPool), itPool(bfPool)
	{

	}

	~TUdpSocketObj()
	{
		ClearRecvQueue();
	}

	static void Release(TUdpSocketObj* pSocketObj)
	{
		__super::Release(pSocketObj);
		pSocketObj->sndBuff.Release();
	}

	int Pending()		{return sndBuff.Length();}
	BOOL IsPending()	{return Pending() > 0;}
	BOOL HasRecvData()	{return !recvQueue.IsEmpty();}

	static BOOL InvalidSocketObj(TUdpSocketObj* pSocketObj)
	{
		BOOL bDone = FALSE;

		if(TSocketObjBase::IsValid(pSocketObj))
		{
			pSocketObj->SetConnected(FALSE);

			CWriteLock			 locallock(pSocketObj->lcIo);
			CReentrantCriSecLock locallock2(pSocketObj->csSend);

			if(TSocketObjBase::IsValid(pSocketObj))
			{
				TSocketObjBase::Invalid(pSocketObj);
				bDone = TRUE;
			}
		}

		return bDone;
	}

	void Reset(CONNID dwConnID)
	{
		__super::Reset(dwConnID);

		detectFails = 0;
	}

	void ClearRecvQueue()
	{
		TItem* pItem = nullptr;

		while(recvQueue.PopFront(&pItem))
			itPool.PutFreeItem(pItem);

		VERIFY(recvQueue.IsEmpty());
	}
};

/* 有效 TSocketObj 缓存 */
typedef CRingCache2<TSocketObj, CONNID, true>		TSocketObjPtrPool;
/* 失效 TSocketObj 缓存 */
typedef CRingPool<TSocketObj>						TSocketObjPtrList;
/* 失效 TSocketObj 垃圾回收结构链表 */
typedef CCASQueue<TSocketObj>						TSocketObjPtrQueue;

/* 有效 TSocketObj 缓存 */
typedef CRingCache2<TAgentSocketObj, CONNID, true>	TAgentSocketObjPtrPool;
/* 失效 TSocketObj 缓存 */
typedef CRingPool<TAgentSocketObj>					TAgentSocketObjPtrList;
/* 失效 TSocketObj 垃圾回收结构链表 */
typedef CCASQueue<TAgentSocketObj>					TAgentSocketObjPtrQueue;

/* 有效 TUdpSocketObj 缓存 */
typedef CRingCache2<TUdpSocketObj, CONNID, true>	TUdpSocketObjPtrPool;
/* 失效 TUdpSocketObj 缓存 */
typedef CRingPool<TUdpSocketObj>					TUdpSocketObjPtrList;
/* 失效 TUdpSocketObj 垃圾回收结构链表 */
typedef CCASQueue<TUdpSocketObj>					TUdpSocketObjPtrQueue;

/* HP_SOCKADDR 比较器 */
struct hp_sockaddr_func
{
	struct hash
	{
		size_t operator() (const HP_SOCKADDR* pA) const
		{
			return pA->Hash();
		}
	};

	struct equal_to
	{
		bool operator () (const HP_SOCKADDR* pA, const HP_SOCKADDR* pB) const
		{
			return pA->EqualTo(*pB);
		}
	};

};

/* 地址-连接 ID 哈希表 */
typedef unordered_map<const HP_SOCKADDR*, CONNID, hp_sockaddr_func::hash, hp_sockaddr_func::equal_to>
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

/* 获取错误描述文本 */
LPCTSTR GetSocketErrorDesc(EnSocketError enCode);
/* 确定地址簇 */
ADDRESS_FAMILY DetermineAddrFamily(LPCTSTR lpszAddress);
/* 地址字符串地址转换为 HP_ADDR */
BOOL GetInAddr(LPCTSTR lpszAddress, HP_ADDR& addr);
/* 地址字符串地址转换为 HP_SOCKADDR */
BOOL GetSockAddr(LPCTSTR lpszAddress, USHORT usPort, HP_SOCKADDR& addr);
/* 检查字符串是否符合 IP 地址格式 */
BOOL IsIPAddress(LPCTSTR lpszAddress, EnIPAddrType* penType = nullptr);
/* 通过主机名获取 IP 地址 */
BOOL GetIPAddress(LPCTSTR lpszHost, LPTSTR lpszIP, int& iIPLenth, EnIPAddrType& enType);
/* 通过主机名获取 HP_SOCKADDR */
BOOL GetSockAddrByHostName(LPCTSTR lpszHost, USHORT usPort, HP_SOCKADDR& addr);
/* 通过主机名获取 HP_SOCKADDR */
BOOL GetSockAddrByHostNameDirectly(LPCTSTR lpszHost, USHORT usPort, HP_SOCKADDR &addr);
/* 枚举主机 IP 地址 */
BOOL EnumHostIPAddresses(LPCTSTR lpszHost, EnIPAddrType enType, LPTIPAddr** lpppIPAddr, int& iIPAddrCount);
/* 填充 LPTIPAddr* */
BOOL RetrieveSockAddrIPAddresses(const vector<HP_PSOCKADDR>& vt, LPTIPAddr** lpppIPAddr, int& iIPAddrCount);
/* 释放 LPTIPAddr* */
BOOL FreeHostIPAddresses(LPTIPAddr* lppIPAddr);
/* 把 HP_SOCKADDR 结构转换为地址字符串 */
BOOL sockaddr_IN_2_A(const HP_SOCKADDR& addr, ADDRESS_FAMILY& usFamily, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort);
/* 把地址字符串转换为 HP_SOCKADDR 结构 */
BOOL sockaddr_A_2_IN(LPCTSTR lpszAddress, USHORT usPort, HP_SOCKADDR& addr);
/* 获取 Socket 的本地或远程地址信息 */
BOOL GetSocketAddress(SOCKET socket, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort, BOOL bLocal = TRUE);
/* 获取 Socket 的本地地址信息 */
BOOL GetSocketLocalAddress(SOCKET socket, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort);
/* 获取 Socket 的远程地址信息 */
BOOL GetSocketRemoteAddress(SOCKET socket, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort);

/* 64 位网络字节序转主机字节序 */
ULONGLONG NToH64(ULONGLONG value);
/* 64 位主机字节序转网络字节序 */
ULONGLONG HToN64(ULONGLONG value);

HRESULT ReadSmallFile(LPCTSTR lpszFileName, CFile& file, CFileMapping& fmap, DWORD dwMaxFileSize = MAX_SMALL_FILE_SIZE);
HRESULT MakeSmallFilePackage(LPCTSTR lpszFileName, CFile& file, CFileMapping& fmap, WSABUF szBuf[3], const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr);

/************************************************************************
名称：setsockopt() 帮助方法
描述：简化常用的 setsockopt() 调用
************************************************************************/

int SSO_SetSocketOption		(SOCKET sock, int level, int name, LPVOID val, int len);
int SSO_GetSocketOption		(SOCKET sock, int level, int name, LPVOID val, int* len);
int SSO_IoctlSocket			(SOCKET sock, long cmd, PVOID arg);

int SSO_NoBlock				(SOCKET sock, BOOL bNoBlock = TRUE);
int SSO_NoDelay				(SOCKET sock, BOOL bNoDelay = TRUE);
int SSO_DontLinger			(SOCKET sock, BOOL bDont = TRUE);
int SSO_Linger				(SOCKET sock, int l_onoff, int l_linger);
int SSO_KeepAlive			(SOCKET sock, BOOL bKeepAlive = TRUE);
int SSO_KeepAliveVals		(SOCKET sock, BOOL bOnOff, DWORD dwIdle, DWORD dwInterval, DWORD dwCount = 5);
int SSO_ReuseAddress		(SOCKET sock, BOOL bReuse = TRUE);
int SSO_RecvBuffSize		(SOCKET sock, int size);
int SSO_SendBuffSize		(SOCKET sock, int size);
int SSO_RecvTimeout			(SOCKET sock, int sec, int microsec = 0);
int SSO_SendTimeout			(SOCKET sock, int sec, int microsec = 0);
int SSO_GetError			(SOCKET sock);

/* 生成 Connection ID */
CONNID GenerateConnectionID();
/* 关闭 Socket */
int ManualCloseSocket(SOCKET sock, int iShutdownFlag = 0xFF, BOOL bGraceful = TRUE);

#ifdef _ICONV_SUPPORT

#define CHARSET_GBK			"GBK"
#define CHARSET_UTF_8		"UTF-8"
#define CHARSET_UTF_16LE	"UTF-16LE"
#define CHARSET_UTF_32LE	"UTF-32LE"

// Charset A -> Charset B
BOOL CharsetConvert(LPCSTR lpszFromCharset, LPCSTR lpszToCharset, LPCSTR lpszInBuf, int iInBufLen, LPSTR lpszOutBuf, int& iOutBufLen);

// GBK -> UNICODE
BOOL GbkToUnicode(const char szSrc[], WCHAR szDest[], int& iDestLength);
// UNICODE -> GBK
BOOL UnicodeToGbk(const WCHAR szSrc[], char szDest[], int& iDestLength);
// UTF8 -> UNICODE
BOOL Utf8ToUnicode(const char szSrc[], WCHAR szDest[], int& iDestLength);
// UNICODE -> UTF8
BOOL UnicodeToUtf8(const WCHAR szSrc[], char szDest[], int& iDestLength);
// GBK -> UTF8
BOOL GbkToUtf8(const char szSrc[], char szDest[], int& iDestLength);
// UTF8 -> GBK
BOOL Utf8ToGbk(const char szSrc[], char szDest[], int& iDestLength);

#endif

#ifdef _ZLIB_SUPPORT

// 普通压缩（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int Compress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// 高级压缩（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int CompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iLevel = Z_DEFAULT_COMPRESSION, int iMethod = Z_DEFLATED, int iWindowBits = MAX_WBITS, int iMemLevel = MAX_MEM_LEVEL, int iStrategy = Z_DEFAULT_STRATEGY);
// 普通解压（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int Uncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// 高级解压（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int UncompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iWindowBits = MAX_WBITS);
// 推测压缩结果长度
DWORD GuessCompressBound(DWORD dwSrcLen, BOOL bGZip = FALSE);

// Gzip 压缩（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int GZipCompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// Gzip 解压（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int GZipUncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// 推测 Gzip 解压结果长度（如果返回 0 或不合理值则说明输入内容并非有效的 Gzip 格式）
DWORD GZipGuessUncompressBound(const BYTE* lpszSrc, DWORD dwSrcLen);

#endif

// 计算 Base64 编码后长度
DWORD GuessBase64EncodeBound(DWORD dwSrcLen);
// 计算 Base64 解码后长度
DWORD GuessBase64DecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen);
// Base64 编码（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int Base64Encode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// Base64 解码（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int Base64Decode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);

// 计算 URL 编码后长度
DWORD GuessUrlEncodeBound(const BYTE* lpszSrc, DWORD dwSrcLen);
// 计算 URL 解码后长度
DWORD GuessUrlDecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen);
// URL 编码（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int UrlEncode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// URL 解码（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int UrlDecode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
