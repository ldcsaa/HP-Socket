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

#include <ws2tcpip.h>
#include <mswsock.h>
#include <malloc.h>

#include <atlfile.h>

#include "../Include/HPSocket/SocketInterface.h"
#include "Common/WaitFor.h"
#include "Common/SysHelper.h"
#include "Common/FuncHelper.h"
#include "Common/BufferPool.h"
#include "Common/RingBuffer.h"
#include "InternalDef.h"

#ifdef _ZLIB_SUPPORT
#include <zutil.h>
#endif

#ifdef _BROTLI_SUPPORT
#pragma warning(push)
#pragma warning(disable: 4005)
#include <brotli/decode.h>
#include <brotli/encode.h>
#pragma warning(pop)
#endif

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
			lpTemp	= CreateLocalObject(WSADATA);

		m_iResult	= ::WSAStartup(MAKEWORD(majorVersion, minorVersion), lpTemp);
	}

	~CInitSocket()
	{
		if(IsValid())
			::WSACleanup();
	}

	int	 GetResult() const {return m_iResult;}
	BOOL IsValid()	 const {return m_iResult == 0;}

private:
	int m_iResult;
};

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

/* Server 组件和 Agent 组件内部使用的事件处理结果常量 */

// 连接已关闭
#define HR_CLOSED	0xFF

/* 关闭连接标识 */
enum EnSocketCloseFlag
{
	SCF_NONE		= 0,	// 不触发事件
	SCF_CLOSE		= 1,	// 触发 正常关闭 OnClose 事件
	SCF_ERROR		= 2		// 触发 异常关闭 OnClose 事件
};

/* 数据缓冲区基础结构 */
template<class T> struct TBufferObjBase
{
	WSAOVERLAPPED		ov;
	CPrivateHeap&		heap;

	EnSocketOperation	operation;
	WSABUF				buff;

	int					capacity;
	volatile LONG		sndCounter;

	T* next;
	T* last;

	static T* Construct(CPrivateHeap& heap, DWORD dwCapacity)
	{
		T* pBufferObj = (T*)heap.Alloc(sizeof(T) + dwCapacity);
		ASSERT(pBufferObj);

		pBufferObj->TBufferObjBase::TBufferObjBase(heap, dwCapacity);
		pBufferObj->buff.buf = ((char*)pBufferObj) + sizeof(T);

		return pBufferObj;
	}

	static void Destruct(T* pBufferObj)
	{
		ASSERT(pBufferObj);
		pBufferObj->heap.Free(pBufferObj);
	}

	void ResetSendCounter()
	{
		sndCounter = 2;
	}

	LONG ReleaseSendCounter()
	{
		return ::InterlockedDecrement(&sndCounter);
	}

	TBufferObjBase(CPrivateHeap& hp, DWORD dwCapacity)
	: heap(hp)
	, capacity((int)dwCapacity)
	{
		ASSERT(capacity > 0);
	}

	int Cat(const BYTE* pData, int length)
	{
		ASSERT(pData != nullptr && length >= 0);

		int cat = min(Remain(), length);

		if(cat > 0)
		{
			memcpy(buff.buf + buff.len, pData, cat);
			buff.len += cat;
		}

		return cat;
	}

	void ResetOV()	{::ZeroMemory(&ov, sizeof(ov));}
	void Reset()	{ResetOV(); buff.len = 0;}
	int Remain()	{return capacity - buff.len;}
	BOOL IsFull()	{return Remain() == 0;}
};

/* 数据缓冲区结构 */
struct TBufferObj : public TBufferObjBase<TBufferObj>
{
	SOCKET client;
};

/* UDP 数据缓冲区结构 */
struct TUdpBufferObj : public TBufferObjBase<TUdpBufferObj>
{
	HP_SOCKADDR	remoteAddr;
	int			addrLen;
};

/* 数据缓冲区链表模板 */
template<class T> struct TBufferObjListT : public TSimpleList<T>
{
public:
	int Cat(const BYTE* pData, int length)
	{
		ASSERT(pData != nullptr && length >= 0);

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
		ASSERT(pData != nullptr && length >= 0 && length <= (int)bfPool.GetItemCapacity());

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

/* TBufferObj 智能指针 */
typedef TItemPtrT<TBufferObj>			TBufferObjPtr;
/* TUdpBufferObj 智能指针 */
typedef TItemPtrT<TUdpBufferObj>		TUdpBufferObjPtr;

/* Socket 缓冲区基础结构 */
struct TSocketObjBase : public CSafeCounter
{
	CPrivateHeap& heap;

	CONNID		connID;
	HP_SOCKADDR	remoteAddr;
	PVOID		extra;
	PVOID		reserved;
	PVOID		reserved2;

	union
	{
		DWORD	freeTime;
		DWORD	connTime;
	};

	DWORD		activeTime;

	volatile BOOL	valid;
	volatile BOOL	smooth;
	volatile long	pending;
	volatile long	sndCount;

	volatile BOOL	connected;
	volatile BOOL	paused;
	volatile BOOL	recving;

	TSocketObjBase(CPrivateHeap& hp) : heap(hp) {}

	static BOOL IsExist(TSocketObjBase* pSocketObj)
		{return pSocketObj != nullptr;}

	static BOOL IsValid(TSocketObjBase* pSocketObj)
		{return (IsExist(pSocketObj) && pSocketObj->valid == TRUE);}

	static void Invalid(TSocketObjBase* pSocketObj)
		{ASSERT(IsExist(pSocketObj)); pSocketObj->valid = FALSE;}

	static void Release(TSocketObjBase* pSocketObj)
		{ASSERT(IsExist(pSocketObj)); pSocketObj->freeTime = ::TimeGetTime();}

	DWORD GetConnTime	()	const	{return connTime;}
	DWORD GetFreeTime	()	const	{return freeTime;}
	DWORD GetActiveTime	()	const	{return activeTime;}
	BOOL IsPaused		()	const	{return paused;}

	long Pending()		{return pending;}
	BOOL IsPending()	{return pending > 0;}
	BOOL IsSmooth()		{return smooth;}
	void TurnOnSmooth()	{smooth = TRUE;}

	BOOL TurnOffSmooth()
		{return ::InterlockedCompareExchange((volatile long*)&smooth, FALSE, TRUE) == TRUE;}
	
	BOOL HasConnected()							{return connected;}
	void SetConnected(BOOL bConnected = TRUE)	{connected = bConnected;}

	void Reset(CONNID dwConnID)
	{
		ResetCount();

		connID		= dwConnID;
		connected	= FALSE;
		valid		= TRUE;
		smooth		= TRUE;
		paused		= FALSE;
		recving		= FALSE;
		pending		= 0;
		sndCount	= 0;
		extra		= nullptr;
		reserved	= nullptr;
		reserved2	= nullptr;
	}
};

/* 数据缓冲区结构 */
struct TSocketObj : public TSocketObjBase
{
	CCriSec			csRecv;
	CCriSec			csSend;
	CSpinGuard		sgPause;

	SOCKET			socket;
	CStringA		host;
	TBufferObjList	sndBuff;

	BOOL IsCanSend() {return sndCount <= GetSendBufferSize();}

	long GetSendBufferSize()
	{
		long lSize;
		int len	= (int)(sizeof(lSize));
		int rs	= getsockopt(socket, SOL_SOCKET, SO_SNDBUF, (CHAR*)&lSize, &len);

		if(rs == SOCKET_ERROR || lSize <= 0)
			lSize = DEFAULT_SOCKET_SNDBUFF_SIZE;

		return lSize;
	}

	static TSocketObj* Construct(CPrivateHeap& hp, CBufferObjPool& bfPool)
	{
		TSocketObj* pSocketObj = (TSocketObj*)hp.Alloc(sizeof(TSocketObj));
		ASSERT(pSocketObj);

		pSocketObj->TSocketObj::TSocketObj(hp, bfPool);

		return pSocketObj;
	}

	static void Destruct(TSocketObj* pSocketObj)
	{
		ASSERT(pSocketObj);

		CPrivateHeap& heap = pSocketObj->heap;
		pSocketObj->TSocketObj::~TSocketObj();
		heap.Free(pSocketObj);
	}
	
	TSocketObj(CPrivateHeap& hp, CBufferObjPool& bfPool)
	: TSocketObjBase(hp), sndBuff(bfPool)
	{

	}

	static BOOL InvalidSocketObj(TSocketObj* pSocketObj)
	{
		BOOL bDone = FALSE;

		if(TSocketObj::IsValid(pSocketObj))
		{
			pSocketObj->SetConnected(FALSE);

			CCriSecLock locallock(pSocketObj->csRecv);
			CCriSecLock locallock2(pSocketObj->csSend);

			if(TSocketObjBase::IsValid(pSocketObj))
			{
				TSocketObjBase::Invalid(pSocketObj);
				bDone = TRUE;
			}
		}

		return bDone;
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
			*pusPort = remoteAddr.Port();

		return (*lpszHost != nullptr && (*lpszHost)[0] != 0);
	}
};

/* UDP 数据缓冲区结构 */
struct TUdpSocketObj : public TSocketObjBase
{
	PVOID				pHolder;
	HANDLE				hTimer;

	CRWLock				csRecv;
	CCriSec				csSend;

	TUdpBufferObjList	sndBuff;
	volatile DWORD		detectFails;

	BOOL IsCanSend			() {return sndCount <= GetSendBufferSize();}
	long GetSendBufferSize	() {return (4 * DEFAULT_SOCKET_SNDBUFF_SIZE);}

	static TUdpSocketObj* Construct(CPrivateHeap& hp, CUdpBufferObjPool& bfPool)
	{
		TUdpSocketObj* pSocketObj = (TUdpSocketObj*)hp.Alloc(sizeof(TUdpSocketObj));
		ASSERT(pSocketObj);

		pSocketObj->TUdpSocketObj::TUdpSocketObj(hp, bfPool);

		return pSocketObj;
	}

	static void Destruct(TUdpSocketObj* pSocketObj)
	{
		ASSERT(pSocketObj);

		CPrivateHeap& heap = pSocketObj->heap;
		pSocketObj->TUdpSocketObj::~TUdpSocketObj();
		heap.Free(pSocketObj);
	}
	
	TUdpSocketObj(CPrivateHeap& hp, CUdpBufferObjPool& bfPool)
	: TSocketObjBase(hp), sndBuff(bfPool)
	{

	}

	static BOOL InvalidSocketObj(TUdpSocketObj* pSocketObj)
	{
		BOOL bDone = FALSE;

		if(TUdpSocketObj::IsValid(pSocketObj))
		{
			pSocketObj->SetConnected(FALSE);

			CReentrantWriteLock	locallock(pSocketObj->csRecv);
			CCriSecLock			locallock2(pSocketObj->csSend);

			if(TSocketObjBase::IsValid(pSocketObj))
			{
				TSocketObjBase::Invalid(pSocketObj);
				bDone = TRUE;
			}
		}

		return bDone;
	}

	static void Release(TUdpSocketObj* pSocketObj)
	{
		__super::Release(pSocketObj);

		pSocketObj->sndBuff.Release();
	}

	void Reset(CONNID dwConnID)
	{
		__super::Reset(dwConnID);

		pHolder		= nullptr;
		hTimer		= nullptr;
		detectFails	= 0;
	}
};

/* 虚拟 Timer Queue */
class _CFakeTimerQueue
{
public:
	_CFakeTimerQueue()
	{

	}

	~_CFakeTimerQueue()
	{

	}

	HANDLE CreateTimer(WAITORTIMERCALLBACK fnCallback, PVOID lpParam, DWORD dwPeriod, DWORD dwDueTime = INFINITE, ULONG ulFlags = WT_EXECUTEDEFAULT)
	{
		return INVALID_HANDLE_VALUE;
	}

	BOOL ChangeTimer(HANDLE hTimer, DWORD dwPeriod, DWORD dwDueTime = INFINITE)
	{
		return TRUE;
	}

	BOOL DeleteTimer(HANDLE hTimer, HANDLE hCompletionEvent = INVALID_HANDLE_VALUE)
	{
		return TRUE;
	}

	BOOL Reset()
	{
		return TRUE;
	}

	BOOL IsValid()					{ return TRUE; }

	HANDLE GetHandle()				{ return INVALID_HANDLE_VALUE; }
	const HANDLE GetHandle() const	{ return INVALID_HANDLE_VALUE; }

	operator HANDLE()				{ return INVALID_HANDLE_VALUE; }
	operator const HANDLE() const	{ return INVALID_HANDLE_VALUE; }

private:
	_CFakeTimerQueue(const _CFakeTimerQueue&);
	_CFakeTimerQueue operator = (const _CFakeTimerQueue&);
};

/* 垃圾回收 Timer Queue */
#ifdef USE_EXTERNAL_GC
typedef CTimerQueue			CGCTimerQueue;
#else
typedef _CFakeTimerQueue	CGCTimerQueue;
#endif


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
#define DEFAULT_WORKER_THREAD_PREFIX	_T("hp-worker-")

/* 设置当前工作线程名称 */
BOOL SetCurrentWorkerThreadName();
/* 设置工作线程默认名称 */
BOOL SetWorkerThreadDefaultName(HANDLE hThread);

/* 获取错误描述文本 */
LPCTSTR GetSocketErrorDesc(EnSocketError enCode);
/* 确定地址簇 */
ADDRESS_FAMILY DetermineAddrFamily(LPCTSTR lpszAddress);
/* 地址字符串地址转换为 HP_ADDR */
BOOL GetInAddr(LPCTSTR lpszAddress, __out HP_ADDR& addr);
/* 地址字符串地址转换为 HP_SOCKADDR */
BOOL GetSockAddr(LPCTSTR lpszAddress, USHORT usPort, __inout HP_SOCKADDR& addr);
/* 检查字符串是否符合 IP 地址格式 */
BOOL IsIPAddress(LPCTSTR lpszAddress, __out EnIPAddrType* penType = nullptr);
/* 通过主机名获取 IP 地址 */
BOOL GetIPAddress(LPCTSTR lpszHost, __out LPTSTR lpszIP, __inout int& iIPLenth, __out EnIPAddrType& enType);
/* 通过主机名获取 HP_SOCKADDR */
BOOL GetSockAddrByHostName(LPCTSTR lpszHost, USHORT usPort, __out HP_SOCKADDR& addr);
/* 通过主机名获取 HP_SOCKADDR */
BOOL GetSockAddrByHostNameDirectly(LPCTSTR lpszHost, USHORT usPort, HP_SOCKADDR &addr);
/* 枚举主机 IP 地址 */
BOOL EnumHostIPAddresses(LPCTSTR lpszHost, EnIPAddrType enType, __out LPTIPAddr** lpppIPAddr, __out int& iIPAddrCount);
/* 填充 LPTIPAddr* */
BOOL RetrieveSockAddrIPAddresses(const vector<HP_PSOCKADDR>& vt, __out LPTIPAddr** lpppIPAddr, __out int& iIPAddrCount);
/* 释放 LPTIPAddr* */
BOOL FreeHostIPAddresses(LPTIPAddr* lppIPAddr);
/* 把 HP_SOCKADDR 结构转换为地址字符串 */
BOOL sockaddr_IN_2_A(const HP_SOCKADDR& addr, __out ADDRESS_FAMILY& usFamily, __out LPTSTR lpszAddress, __inout int& iAddressLen, __out USHORT& usPort);
/* 把地址字符串转换为 HP_SOCKADDR 结构 */
BOOL sockaddr_A_2_IN(LPCTSTR lpszAddress, USHORT usPort, __out HP_SOCKADDR& addr);
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
	IOCP_CMD_SEND		= 0xFFFFFFF3,	// 发送数据
	IOCP_CMD_UNPAUSE	= 0xFFFFFFF4,	// 取消暂停
	IOCP_CMD_TIMEOUT	= 0xFFFFFFF5	// 保活超时
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
BOOL PostIocpUnpause(HANDLE hIOCP, CONNID dwConnID);
BOOL PostIocpTimeout(HANDLE hIOCP, CONNID dwConnID);
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
int SSO_NoBlock				(SOCKET sock, BOOL bNoBlock = TRUE);
int SSO_NoDelay				(SOCKET sock, BOOL bNoDelay = TRUE);
int SSO_DontLinger			(SOCKET sock, BOOL bDont = TRUE);
int SSO_Linger				(SOCKET sock, USHORT l_onoff, USHORT l_linger);
int SSO_KeepAlive			(SOCKET sock, BOOL bKeepAlive = TRUE);
int SSO_KeepAliveVals		(SOCKET sock, u_long onoff, u_long time, u_long interval);
int SSO_RecvBuffSize		(SOCKET sock, int size);
int SSO_SendBuffSize		(SOCKET sock, int size);
int SSO_RecvTimeOut			(SOCKET sock, int ms);
int SSO_SendTimeOut			(SOCKET sock, int ms);
int SSO_ReuseAddress		(SOCKET sock, EnReuseAddressPolicy opt);
int SSO_ExclusiveAddressUse	(SOCKET sock, BOOL bExclusive = TRUE);
int SSO_UDP_ConnReset		(SOCKET sock, BOOL bNewBehavior = TRUE);
int SSO_GetError			(SOCKET sock);

/************************************************************************
名称：Socket 操作方法
描述：Socket 操作包装方法
************************************************************************/

/* 检测 IOCP 操作返回值：NO_ERROR 则返回 TRUE */
#define IOCP_NO_ERROR(rs)		((rs) == NO_ERROR)
/* 检测 IOCP 操作返回值：WSA_IO_PENDING 则返回 TRUE */
#define IOCP_PENDING(rs)		((rs) == WSA_IO_PENDING)
/* 检测 IOCP 操作返回值：NO_ERROR 或 WSA_IO_PENDING 则返回 TRUE */
#define IOCP_SUCCESS(rs)		(IOCP_NO_ERROR(rs) || IOCP_PENDING(rs))

/* 检查是否 UDP RESET 错误 */
#define IS_UDP_RESET_ERROR(rs)	((rs) == WSAENETRESET || (rs) == WSAECONNRESET)

/* 生成 Connection ID */
CONNID GenerateConnectionID	();
/* 检测 UDP 连接关闭通知 */
int IsUdpCloseNotify		(const BYTE* pData, int iLength);
/* 发送 UDP 连接关闭通知 */
int SendUdpCloseNotify		(SOCKET sock);
/* 发送 UDP 连接关闭通知 */
int SendUdpCloseNotify		(SOCKET sock, const HP_SOCKADDR& remoteAddr);
/* 关闭 Socket */
int ManualCloseSocket		(SOCKET sock, int iShutdownFlag = 0xFF, BOOL bGraceful = TRUE);
/* 投递 AccceptEx()，并把 WSA_IO_PENDING 转换为 NO_ERROR */
int PostAccept				(LPFN_ACCEPTEX pfnAcceptEx, SOCKET soListen, SOCKET soClient, TBufferObj* pBufferObj, ADDRESS_FAMILY usFamily);
/* 投递 AccceptEx() */
int PostAcceptNotCheck		(LPFN_ACCEPTEX pfnAcceptEx, SOCKET soListen, SOCKET soClient, TBufferObj* pBufferObj, ADDRESS_FAMILY usFamily);
/* 投递 ConnectEx()，并把 WSA_IO_PENDING 转换为 NO_ERROR */
int PostConnect				(LPFN_CONNECTEX pfnConnectEx, SOCKET soClient, const HP_SOCKADDR& sockAddr, TBufferObj* pBufferObj);
/* 投递 ConnectEx() */
int PostConnectNotCheck		(LPFN_CONNECTEX pfnConnectEx, SOCKET soClient, const HP_SOCKADDR& sockAddr, TBufferObj* pBufferObj);
/* 投递 WSASend()，并把 WSA_IO_PENDING 转换为 NO_ERROR */
int PostSend				(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* 投递 WSASend() */
int PostSendNotCheck		(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* 投递 WSARecv()，并把 WSA_IO_PENDING 转换为 NO_ERROR */
int PostReceive				(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* 投递 WSARecv() */
int PostReceiveNotCheck		(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* 投递 WSASendTo()，并把 WSA_IO_PENDING 转换为 NO_ERROR */
int PostSendTo				(SOCKET sock, TUdpBufferObj* pBufferObj);
/* 投递 WSASendTo() */
int PostSendToNotCheck		(SOCKET sock, TUdpBufferObj* pBufferObj);
/* 投递 WSARecvFrom()，并把 WSA_IO_PENDING 转换为 NO_ERROR */
int PostReceiveFrom			(SOCKET sock, TUdpBufferObj* pBufferObj);
/* 投递 WSARecvFrom() */
int PostReceiveFromNotCheck	(SOCKET sock, TUdpBufferObj* pBufferObj);
/* 执行非阻塞 WSARecv()，并把 WSAEWOULDBLOCK 转换为 NO_ERROR */
int NoBlockReceive(TBufferObj* pBufferObj);
/* 执行非阻塞 WSARecv() */
int NoBlockReceiveNotCheck(TBufferObj* pBufferObj);
/* 执行非阻塞 WSARecvFrom()，并把 WSAEWOULDBLOCK 转换为 NO_ERROR */
int NoBlockReceiveFrom(SOCKET sock, TUdpBufferObj* pBufferObj);
/* 执行非阻塞 WSARecvFrom() */
int NoBlockReceiveFromNotCheck(SOCKET sock, TUdpBufferObj* pBufferObj);
/* 设置组播选项 */
BOOL SetMultiCastSocketOptions(SOCKET sock, const HP_SOCKADDR& bindAddr, const HP_SOCKADDR& castAddr, int iMCTtl, BOOL bMCLoop);
/* 等待连接 */
int WaitForSocketWrite(SOCKET sock, DWORD dwTimeout);

// CP_XXX -> UNICODE
BOOL CodePageToUnicodeEx(int iCodePage, const char szSrc[], int iSrcLength, WCHAR szDest[], int& iDestLength);
// UNICODE -> CP_XXX
BOOL UnicodeToCodePageEx(int iCodePage, const WCHAR szSrc[], int iSrcLength, char szDest[], int& iDestLength);
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

// CP_XXX -> UNICODE
BOOL CodePageToUnicode(int iCodePage, const char szSrc[], WCHAR szDest[], int& iDestLength);
// UNICODE -> CP_XXX
BOOL UnicodeToCodePage(int iCodePage, const WCHAR szSrc[], char szDest[], int& iDestLength);
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
	CHPZLibCompressor(Fn_CompressDataCallback fnCallback, int iWindowBits = DEF_WBITS, int iLevel = Z_DEFAULT_COMPRESSION, int iMethod = Z_DEFLATED, int iMemLevel = DEF_MEM_LEVEL, int iStrategy = Z_DEFAULT_STRATEGY, DWORD dwBuffSize = DEFAULT_COMPRESS_BUFFER_SIZE);
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
	CHPZLibDecompressor(Fn_DecompressDataCallback fnCallback, int iWindowBits = DEF_WBITS, DWORD dwBuffSize = DEFAULT_COMPRESS_BUFFER_SIZE);
	virtual ~CHPZLibDecompressor();

private:
	Fn_DecompressDataCallback m_fnCallback;
	z_stream m_Stream;
	BOOL m_bValid;
	DWORD m_dwBuffSize;
};

/* 创建 ZLib 压缩器对象 */
IHPCompressor* CreateZLibCompressor(Fn_CompressDataCallback fnCallback, int iWindowBits = DEF_WBITS, int iLevel = Z_DEFAULT_COMPRESSION, int iMethod = Z_DEFLATED, int iMemLevel = DEF_MEM_LEVEL, int iStrategy = Z_DEFAULT_STRATEGY, DWORD dwBuffSize = DEFAULT_COMPRESS_BUFFER_SIZE);
/* 创建 GZip 压缩器对象 */
IHPCompressor* CreateGZipCompressor(Fn_CompressDataCallback fnCallback, int iLevel = Z_DEFAULT_COMPRESSION, int iMethod = Z_DEFLATED, int iMemLevel = DEF_MEM_LEVEL, int iStrategy = Z_DEFAULT_STRATEGY, DWORD dwBuffSize = DEFAULT_COMPRESS_BUFFER_SIZE);
/* 创建 ZLib 解压器对象 */
IHPDecompressor* CreateZLibDecompressor(Fn_DecompressDataCallback fnCallback, int iWindowBits = DEF_WBITS, DWORD dwBuffSize = DEFAULT_COMPRESS_BUFFER_SIZE);
/* 创建 GZip 解压器对象 */
IHPDecompressor* CreateGZipDecompressor(Fn_DecompressDataCallback fnCallback, DWORD dwBuffSize = DEFAULT_COMPRESS_BUFFER_SIZE);

// 普通压缩（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int Compress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// 高级压缩（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int CompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iLevel = Z_DEFAULT_COMPRESSION, int iMethod = Z_DEFLATED, int iWindowBits = DEF_WBITS, int iMemLevel = DEF_MEM_LEVEL, int iStrategy = Z_DEFAULT_STRATEGY);
// 普通解压（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int Uncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// 高级解压（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
int UncompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iWindowBits = DEF_WBITS);
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
