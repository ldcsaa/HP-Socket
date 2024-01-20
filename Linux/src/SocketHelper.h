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

#pragma once

#include "../include/hpsocket/HPTypeDef.h"
#include "../include/hpsocket/SocketInterface.h"
#include "common/StringT.h"
#include "common/SysHelper.h"
#include "common/BufferPtr.h"
#include "common/BufferPool.h"
#include "common/RingBuffer.h"
#include "common/FileHelper.h"
#include "InternalDef.h"

#include <netdb.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#ifdef _ZLIB_SUPPORT
#include <zlib.h>
#endif

#ifdef _BROTLI_SUPPORT
#include <brotli/decode.h>
#include <brotli/encode.h>
#endif

using ADDRESS_FAMILY	= sa_family_t;
using IN_ADDR			= in_addr;
using IN6_ADDR			= in6_addr;
using SOCKADDR			= sockaddr;
using SOCKADDR_IN		= sockaddr_in;
using SOCKADDR_IN6		= sockaddr_in6;

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

	inline int EffectAddrSize() const
	{
		return EffectAddrSize(family);
	}

	inline static int EffectAddrSize(ADDRESS_FAMILY f)
	{
		return (f == AF_INET) ? offsetof(SOCKADDR_IN, sin_zero) : sizeof(SOCKADDR_IN6);
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
			return INET_ADDRSTRLEN;

		return INET6_ADDRSTRLEN;
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

		size_t _Val		  = 2166136261U;
		const int size	  = EffectAddrSize();
		const BYTE* pAddr = (const BYTE*)Addr();

		for(int i = 0; i < size; i++)
			_Val = 16777619U * _Val ^ (size_t)pAddr[i];

		return (_Val);
	}

	bool EqualTo(const hp_sockaddr& other) const
	{
		ASSERT(IsSpecified() && other.IsSpecified());

		return EqualMemory(this, &other, EffectAddrSize());
	}

	hp_sockaddr(ADDRESS_FAMILY f = AF_UNSPEC, BOOL bZeroAddr = FALSE)
	{
		family = f;

		if(bZeroAddr) ZeroAddr();
	}

} HP_SOCKADDR, *HP_PSOCKADDR;

typedef struct hp_scope_host
{
	LPCTSTR addr;
	LPCTSTR name;

	BOOL bNeedFree;

	hp_scope_host(LPCTSTR lpszOriginAddress)
	{
		ASSERT(lpszOriginAddress != nullptr);

		LPCTSTR lpszFind = ::StrChr(lpszOriginAddress, HOST_SEPARATOR_CHAR);

		if(lpszFind == nullptr)
		{
			addr		= lpszOriginAddress;
			name		= lpszOriginAddress;
			bNeedFree	= FALSE;
		}
		else
		{
			int i			= (int)(lpszFind - lpszOriginAddress);
			int iSize		= (int)lstrlen(lpszOriginAddress) + 1;
			LPTSTR lpszCopy	= new TCHAR[iSize];

			::memcpy((PVOID)lpszCopy, (PVOID)lpszOriginAddress, iSize * sizeof(TCHAR));

			lpszCopy[i]	= 0;
			addr		= lpszCopy;
			name		= lpszCopy + i + 1;
			bNeedFree	= TRUE;

			if(::IsStrEmpty(name))
				name = addr;
		}
	}

	~hp_scope_host()
	{
		if(bNeedFree)
			delete[] addr;
	}

} HP_SCOPE_HOST, *HP_PSCOPE_HOST;

struct TNodeBufferObj : public TItem
{
	using __super = TItem;

	HP_SOCKADDR	remoteAddr;

public:
	void Reset(int first = 0, int last = 0)
	{
		__super::Reset(first, last);
		remoteAddr.Reset();
	}

public:
	static TNodeBufferObj* Construct(CPrivateHeap& heap,
									int		capacity	= DEFAULT_ITEM_CAPACITY,
									BYTE*	pData		= nullptr,
									int		length		= 0)
	{
		return ::ConstructItemT((TNodeBufferObj*)(nullptr), heap, capacity, pData, length);
	}

	static void Destruct(TNodeBufferObj* pBufferObj)
	{
		::DestructItemT(pBufferObj);
	}

	TNodeBufferObj(CPrivateHeap& hp, BYTE* pHead, int cap = DEFAULT_ITEM_CAPACITY, BYTE* pData = nullptr, int length = 0)
	: TItem(hp, pHead, cap, pData, length)
	{

	}

	~TNodeBufferObj()
	{
	}

	DECLARE_NO_COPY_CLASS(TNodeBufferObj)
};

typedef TItemPtrT<TNodeBufferObj>					TNodeBufferObjPtr;
typedef CNodePoolT<TNodeBufferObj>					CNodeBufferObjPool;
typedef CCASQueue<TNodeBufferObj>					CNodeRecvQueue;
typedef TItemListExT<TNodeBufferObj, volatile int>	TNodeBufferObjList;

typedef unique_ptr<CCriSec[]>						CNodeCriSecs;
typedef unique_ptr<TNodeBufferObjList[]>			TNodeBufferObjLists;

/* Server 组件和 Agent 组件内部使用的事件处理结果常量 */

// 连接已关闭
#define HR_CLOSED		0xFF

/* 命令类型 */
enum EnDispCmdType
{
	DISP_CMD_SEND			= 0x01,	// 发送数据
	DISP_CMD_RECEIVE		= 0x02,	// 接收数据
	DISP_CMD_UNPAUSE		= 0x03,	// 恢复接收数据
	DISP_CMD_DISCONNECT		= 0x04,	// 断开连接
	DISP_CMD_TIMEOUT		= 0x05,	// 保活超时
};

/* 关闭连接标识 */
enum EnSocketCloseFlag
{
	SCF_NONE			= 0,	// 不触发事件
	SCF_CLOSE			= 1,	// 触发 正常关闭 OnClose 事件
	SCF_ERROR			= 2		// 触发 异常关闭 OnClose 事件
};

/* 监听 Socket 数组智能指针 */
typedef unique_ptr<SOCKET[]>	ListenSocketsPtr;

/* 数据缓冲节点 */
typedef TItem					TBufferObj;
/* 数据缓冲节点智能指针 */
typedef TItemPtr				TBufferObjPtr;
/* 数据缓冲区对象池 */
typedef CItemPool				CBufferObjPool;
/* 数据缓冲区链表模板 */
typedef TItemListExV			TBufferObjList;

/* 接收缓冲区数组智能指针 */
typedef unique_ptr<CBufferPtr[]>			CReceiveBuffersPtr;

/* 线程 ID - 接收缓冲区哈希表 */
typedef unordered_map<THR_ID, CBufferPtr*>	TReceiveBufferMap;
/* 线程 ID - 接收缓冲区哈希表迭代器 */
typedef TReceiveBufferMap::iterator			TReceiveBufferMapI;
/* 线程 ID - 接收缓冲区哈希表 const 迭代器 */
typedef TReceiveBufferMap::const_iterator	TReceiveBufferMapCI;

/* Socket 缓冲区基础结构 */
struct TSocketObjBase : public CSafeCounter
{
	CPrivateHeap&		heap;
	CReentrantCriSec	csSend;
	TBufferObjList		sndBuff;


	CONNID		connID;
	HP_SOCKADDR	remoteAddr;
	PVOID		extra;
	PVOID		reserved;
	PVOID		reserved2;
	DWORD		activeTime;

	union
	{
		DWORD	freeTime;
		DWORD	connTime;
	};

	volatile BOOL valid;
	volatile BOOL connected;
	volatile BOOL paused;

	TSocketObjBase(CPrivateHeap& hp, CBufferObjPool& bfPool) : heap(hp), sndBuff(bfPool) {}

	static BOOL IsExist(TSocketObjBase* pSocketObj)
		{return pSocketObj != nullptr;}

	static BOOL IsValid(TSocketObjBase* pSocketObj)
		{return (IsExist(pSocketObj) && pSocketObj->valid == TRUE);}

	static void Invalid(TSocketObjBase* pSocketObj)
		{ASSERT(IsExist(pSocketObj)); pSocketObj->valid = FALSE;}

	static void Release(TSocketObjBase* pSocketObj)
	{
		ASSERT(IsExist(pSocketObj));
		
		pSocketObj->freeTime = ::TimeGetTime();
		pSocketObj->sndBuff.Release();
	}

	static BOOL InvalidSocketObj(TSocketObjBase* pSocketObj)
	{
		BOOL bDone = FALSE;

		if(TSocketObjBase::IsValid(pSocketObj))
		{
			pSocketObj->SetConnected(FALSE);

			CReentrantCriSecLock locallock(pSocketObj->csSend);

			if(TSocketObjBase::IsValid(pSocketObj))
			{
				TSocketObjBase::Invalid(pSocketObj);
				bDone = TRUE;
			}
		}

		return bDone;
	}

	DWORD GetConnTime	()	const	{return connTime;}
	DWORD GetFreeTime	()	const	{return freeTime;}
	DWORD GetActiveTime	()	const	{return activeTime;}
	BOOL IsPaused		()	const	{return paused;}

	int Pending			()	const	{return sndBuff.Length();}
	BOOL IsPending		()	const	{return Pending() > 0;}

	BOOL HasConnected()							{return connected == TRUE;}
	BOOL IsConnecting()							{return connected == CST_CONNECTING;}
	void SetConnected(BOOL bConnected = TRUE)	{connected = bConnected;}

	void Reset(CONNID dwConnID)
	{
		ResetCount();

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

	SOCKET socket;

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
	: __super(hp, bfPool)
	{

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
	using __super = TSocketObjBase;

	int		index;
	PVOID	pHolder;
	FD		fdTimer;

	volatile DWORD	detectFails;

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
	: __super(hp, bfPool)
	{

	}

	void Reset(CONNID dwConnID)
	{
		__super::Reset(dwConnID);

		index		= -1;
		detectFails	= 0;
		fdTimer		= INVALID_FD;
		pHolder		= nullptr;
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
	BOOL bNotify;

	TClientCloseContext(BOOL bFire = TRUE, EnSocketOperation enOp = SO_CLOSE, int iCode = SE_OK, BOOL bNtf = TRUE)
	{
		Reset(bFire, enOp, iCode, bNtf);
	}

	void Reset(BOOL bFire = TRUE, EnSocketOperation enOp = SO_CLOSE, int iCode = SE_OK, BOOL bNtf = TRUE)
	{
		bFireOnClose = bFire;
		enOperation	 = enOp;
		iErrorCode	 = iCode;
		bNotify		 = bNtf;
	}

};

/*****************************************************************************************************/
/******************************************** 公共帮助方法 ********************************************/
/*****************************************************************************************************/

/* 默认工作线程前缀 */
#define DEFAULT_WORKER_THREAD_PREFIX	"hp-worker-"

/* 设置当前工作线程名称 */
BOOL SetCurrentWorkerThreadName();
/* 设置工作线程默认名称 */
BOOL SetWorkerThreadDefaultName(THR_ID tid);

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
/* 设置组播选项 */
BOOL SetMultiCastSocketOptions(SOCKET sock, const HP_SOCKADDR& bindAddr, const HP_SOCKADDR& castAddr, int iMCTtl, BOOL bMCLoop);

/* 64 位网络字节序转主机字节序 */
ULONGLONG NToH64(ULONGLONG value);
/* 64 位主机字节序转网络字节序 */
ULONGLONG HToN64(ULONGLONG value);

/* 短整型高低字节交换 */
#define ENDIAN_SWAP_16(A)	((USHORT)((((USHORT)(A) & 0xff00) >> 8) | (((USHORT)(A) & 0x00ff) << 8)))
/* 长整型高低字节交换 */
#define ENDIAN_SWAP_32(A)	((((DWORD)(A) & 0xff000000) >> 24) | \
							(((DWORD)(A) & 0x00ff0000) >>  8)  | \
							(((DWORD)(A) & 0x0000ff00) <<  8)  | \
							(((DWORD)(A) & 0x000000ff) << 24)	 )

/* 检查是否小端字节序 */
BOOL IsLittleEndian();
/* 短整型主机字节序转小端字节序 */
USHORT HToLE16(USHORT value);
/* 短整型主机字节序转大端字节序 */
USHORT HToBE16(USHORT value);
/* 长整型主机字节序转小端字节序 */
DWORD HToLE32(DWORD value);
/* 长整型主机字节序转大端字节序 */
DWORD HToBE32(DWORD value);

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
int SSO_ReuseAddress		(SOCKET sock, EnReuseAddressPolicy opt);
int SSO_RecvBuffSize		(SOCKET sock, int size);
int SSO_SendBuffSize		(SOCKET sock, int size);
int SSO_RecvTimeOut			(SOCKET sock, int ms);
int SSO_SendTimeOut			(SOCKET sock, int ms);
int SSO_GetError			(SOCKET sock);

/* 生成 Connection ID */
CONNID GenerateConnectionID();
/* 检测 UDP 连接关闭通知 */
int IsUdpCloseNotify(const BYTE* pData, int iLength);
/* 发送 UDP 连接关闭通知 */
int SendUdpCloseNotify(SOCKET sock);
/* 发送 UDP 连接关闭通知 */
int SendUdpCloseNotify(SOCKET sock, const HP_SOCKADDR& remoteAddr);
/* 关闭 Socket */
int ManualCloseSocket(SOCKET sock, int iShutdownFlag = 0xFF, BOOL bGraceful = TRUE);

#ifdef _ICONV_SUPPORT

#define CHARSET_GBK				"GBK"
#define CHARSET_UTF_8			"UTF-8"
#define CHARSET_UTF_16LE		"UTF-16LE"
#define CHARSET_UTF_32LE		"UTF-32LE"
#define CHARSET_UTF_16BE		"UTF-16BE"
#define CHARSET_UTF_32BE		"UTF-32BE"

// 系统 UNICODE 字符集
#define SYSTEM_CHARSET_UNICODE	( (sizeof(WCHAR) == 4)										   \
									? (IsLittleEndian() ? CHARSET_UTF_32LE : CHARSET_UTF_32BE) \
									: (IsLittleEndian() ? CHARSET_UTF_16LE : CHARSET_UTF_16BE) )

// Charset A -> Charset B
BOOL CharsetConvert(LPCSTR lpszFromCharset, LPCSTR lpszToCharset, LPCSTR lpszInBuf, int iInBufLen, LPSTR lpszOutBuf, int& iOutBufLen);

// GBK -> UNICODE
BOOL GbkToUnicodeEx(const char szSrc[], int iSrcLength, WCHAR szDest[], int& iDestLength);
// UNICODE -> GBK
BOOL UnicodeToGbkEx(const WCHAR szSrc[], int iSrcLength, char szDest[], int& iDestLength);
// UTF8 -> UNICODE
BOOL Utf8ToUnicodeEx(const char szSrc[], int iSrcLength, WCHAR szDest[], int& iDestLength);
// UNICODE -> UTF8
BOOL UnicodeToUtf8Ex(const WCHAR szSrc[], int iSrcLength, char szDest[], int& iDestLength);
// GBK -> UTF8
BOOL GbkToUtf8Ex(const char szSrc[], int iSrcLength, char szDest[], int& iDestLength);
// UTF8 -> GBK
BOOL Utf8ToGbkEx(const char szSrc[], int iSrcLength, char szDest[], int& iDestLength);

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

/* 销毁压缩器对象 */
void DestroyCompressor(IHPCompressor* pCompressor);
/* 销毁解压器对象 */
void DestroyDecompressor(IHPDecompressor* pDecompressor);

#ifdef _ZLIB_SUPPORT

/* ZLib 压缩器 */
class CHPZLibCompressor : public IHPCompressor
{
public:
	virtual BOOL Process(const BYTE* pData, int iLength, BOOL bLast, PVOID pContext = nullptr);
	virtual BOOL ProcessEx(const BYTE* pData, int iLength, BOOL bLast, BOOL bFlush = FALSE, PVOID pContext = nullptr);
	virtual BOOL IsValid() {return m_bValid;}
	virtual BOOL Reset();

public:
	CHPZLibCompressor(Fn_CompressDataCallback fnCallback, int iWindowBits = MAX_WBITS, int iLevel = Z_DEFAULT_COMPRESSION, int iMethod = Z_DEFLATED, int iMemLevel = MAX_MEM_LEVEL, int iStrategy = Z_DEFAULT_STRATEGY, DWORD dwBuffSize = DEFAULT_COMPRESS_BUFFER_SIZE);
	virtual ~CHPZLibCompressor();

private:
	Fn_CompressDataCallback m_fnCallback;
	z_stream m_Stream;
	BOOL m_bValid;
	DWORD m_dwBuffSize;
};

/* ZLib 解压器 */
class CHPZLibDecompressor : public IHPDecompressor
{
public:
	virtual BOOL Process(const BYTE* pData, int iLength, PVOID pContext = nullptr);
	virtual BOOL IsValid() {return m_bValid;}
	virtual BOOL Reset();

public:
	CHPZLibDecompressor(Fn_DecompressDataCallback fnCallback, int iWindowBits = MAX_WBITS, DWORD dwBuffSize = DEFAULT_COMPRESS_BUFFER_SIZE);
	virtual ~CHPZLibDecompressor();

private:
	Fn_DecompressDataCallback m_fnCallback;
	z_stream m_Stream;
	BOOL m_bValid;
	DWORD m_dwBuffSize;
};

/* 创建 ZLib 压缩器对象 */
IHPCompressor* CreateZLibCompressor(Fn_CompressDataCallback fnCallback, int iWindowBits = MAX_WBITS, int iLevel = Z_DEFAULT_COMPRESSION, int iMethod = Z_DEFLATED, int iMemLevel = MAX_MEM_LEVEL, int iStrategy = Z_DEFAULT_STRATEGY, DWORD dwBuffSize = DEFAULT_COMPRESS_BUFFER_SIZE);
/* 创建 GZip 压缩器对象 */
IHPCompressor* CreateGZipCompressor(Fn_CompressDataCallback fnCallback, int iLevel = Z_DEFAULT_COMPRESSION, int iMethod = Z_DEFLATED, int iMemLevel = MAX_MEM_LEVEL, int iStrategy = Z_DEFAULT_STRATEGY, DWORD dwBuffSize = DEFAULT_COMPRESS_BUFFER_SIZE);
/* 创建 ZLib 解压器对象 */
IHPDecompressor* CreateZLibDecompressor(Fn_DecompressDataCallback fnCallback, int iWindowBits = MAX_WBITS, DWORD dwBuffSize = DEFAULT_COMPRESS_BUFFER_SIZE);
/* 创建 GZip 解压器对象 */
IHPDecompressor* CreateGZipDecompressor(Fn_DecompressDataCallback fnCallback, DWORD dwBuffSize = DEFAULT_COMPRESS_BUFFER_SIZE);

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

#ifdef _BROTLI_SUPPORT

/* Brotli 压缩器 */
class CHPBrotliCompressor : public IHPCompressor
{
public:
	virtual BOOL Process(const BYTE* pData, int iLength, BOOL bLast, PVOID pContext = nullptr);
	virtual BOOL ProcessEx(const BYTE* pData, int iLength, BOOL bLast, BOOL bFlush = FALSE, PVOID pContext = nullptr);
	virtual BOOL IsValid() {return m_bValid;}
	virtual BOOL Reset();

public:
	CHPBrotliCompressor(Fn_CompressDataCallback fnCallback, int iQuality = BROTLI_DEFAULT_QUALITY, int iWindow = BROTLI_DEFAULT_WINDOW, int iMode = BROTLI_DEFAULT_MODE, DWORD dwBuffSize = DEFAULT_COMPRESS_BUFFER_SIZE);
	virtual ~CHPBrotliCompressor();

private:
	Fn_CompressDataCallback m_fnCallback;
	BrotliEncoderState* m_pState;
	BOOL m_bValid;

	int m_iQuality;
	int m_iWindow;
	int m_iMode;
	DWORD m_dwBuffSize;
};

/* Brotli 解压器 */
class CHPBrotliDecompressor : public IHPDecompressor
{
public:
	virtual BOOL Process(const BYTE* pData, int iLength, PVOID pContext = nullptr);
	virtual BOOL IsValid() {return m_bValid;}
	virtual BOOL Reset();

public:
	CHPBrotliDecompressor(Fn_DecompressDataCallback fnCallback, DWORD dwBuffSize = DEFAULT_COMPRESS_BUFFER_SIZE);
	virtual ~CHPBrotliDecompressor();

private:
	Fn_DecompressDataCallback m_fnCallback;
	BrotliDecoderState* m_pState;
	BOOL m_bValid;
	DWORD m_dwBuffSize;
};

/* 创建 Brotli 压缩器对象 */
IHPCompressor* CreateBrotliCompressor(Fn_CompressDataCallback fnCallback, int iQuality = BROTLI_DEFAULT_QUALITY, int iWindow = BROTLI_DEFAULT_WINDOW, int iMode = BROTLI_DEFAULT_MODE, DWORD dwBuffSize = DEFAULT_COMPRESS_BUFFER_SIZE);
/* 创建 Brotli 解压器对象 */
IHPDecompressor* CreateBrotliDecompressor(Fn_DecompressDataCallback fnCallback, DWORD dwBuffSize = DEFAULT_COMPRESS_BUFFER_SIZE);

// Brotli 压缩（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int BrotliCompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// Brotli 高级压缩（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int BrotliCompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iQuality = BROTLI_DEFAULT_QUALITY, int iWindow = BROTLI_DEFAULT_WINDOW, int iMode = BROTLI_DEFAULT_MODE);
// Brotli 解压（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int BrotliUncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// Brotli 推测压缩结果长度
DWORD BrotliGuessCompressBound(DWORD dwSrcLen);

#endif
