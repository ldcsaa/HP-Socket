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

#include "HPTypeDef.h"

/*****************************************************************************************************************************************************/
/***************************************************************** TCP/UDP Interfaces ****************************************************************/
/*****************************************************************************************************************************************************/

/************************************************************************
名称：双接口模版类
描述：定义双接口转换方法
************************************************************************/

#if defined(__arm__) && defined(__GNUC__) && !(defined(__clang__) || defined(__llvm__))

#define __DUAL_VPTR_GAP__	sizeof(PVOID)

class __IFakeDualInterface__
{
public:
	virtual ~__IFakeDualInterface__() {}
};

template<class F, class S> class DualInterface : public F, private __IFakeDualInterface__, public S

#else

#define __DUAL_VPTR_GAP__	0

template<class F, class S> class DualInterface : public F, public S

#endif

{
public:

	/* this 转换为 F* */
	inline static F* ToF(DualInterface* pThis)
	{
		return (F*)(pThis);
	}

	/* F* 转换为 this */
	inline static DualInterface* FromF(F* pF)
	{
		return (DualInterface*)(pF);
	}

	/* this 转换为 S* */
	inline static S* ToS(DualInterface* pThis)
	{
		return (S*)(F2S(ToF(pThis)));
	}

	/* S* 转换为 this */
	inline static DualInterface* FromS(S* pS)
	{
		return FromF(S2F(pS));
	}

	/* S* 转换为 F* */
	inline static F* S2F(S* pS)
	{
		return (F*)((char*)pS - (sizeof(F) + __DUAL_VPTR_GAP__));
	}

	/* F* 转换为 S* */
	inline static S* F2S(F* pF)
	{
		return (S*)((char*)pF + (sizeof(F) + __DUAL_VPTR_GAP__));
	}

public:
	virtual ~DualInterface() = default;
};

/************************************************************************
名称：复合 Socket 组件接口
描述：定义复合 Socket 组件的所有操作方法和属性访问方法，复合 Socket 组件同时管理多个 Socket 连接
************************************************************************/
class IComplexSocket
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

	/*
	* 名称：关闭通信组件
	* 描述：关闭通信组件，关闭完成后断开所有连接并释放所有资源
	*		
	* 参数：	
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 GetLastError() 获取错误代码
	*/
	virtual BOOL Stop	()																		= 0;

	/*
	* 名称：发送数据
	* 描述：向指定连接发送数据
	*		
	* 参数：		dwConnID	-- 连接 ID
	*			pBuffer		-- 发送缓冲区
	*			iLength		-- 发送缓冲区长度
	*			iOffset		-- 发送缓冲区指针偏移量
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL Send	(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset = 0)	= 0;

	/*
	* 名称：发送多组数据
	* 描述：向指定连接发送多组数据
	*		TCP - 顺序发送所有数据包 
	*		UDP - 把所有数据包组合成一个数据包发送（数据包的总长度不能大于设置的 UDP 包最大长度） 
	*		
	* 参数：		dwConnID	-- 连接 ID
	*			pBuffers	-- 发送缓冲区数组
	*			iCount		-- 发送缓冲区数目
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL SendPackets(CONNID dwConnID, const WSABUF pBuffers[], int iCount)	= 0;

	/*
	* 名称：暂停/恢复接收
	* 描述：暂停/恢复某个连接的数据接收工作
	*		
	* 参数：		dwConnID	-- 连接 ID
	*			bPause		-- TRUE - 暂停, FALSE - 恢复
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败
	*/
	virtual BOOL PauseReceive(CONNID dwConnID, BOOL bPause = TRUE)					= 0;

	/*
	* 名称：断开连接
	* 描述：断开某个连接
	*		
	* 参数：		dwConnID	-- 连接 ID
	*			bForce		-- 是否强制断开连接
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败
	*/
	virtual BOOL Disconnect(CONNID dwConnID, BOOL bForce = TRUE)					= 0;

	/*
	* 名称：断开超时连接
	* 描述：断开超过指定时长的连接
	*		
	* 参数：		dwPeriod	-- 时长（毫秒）
	*			bForce		-- 是否强制断开连接
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败
	*/
	virtual BOOL DisconnectLongConnections(DWORD dwPeriod, BOOL bForce = TRUE)		= 0;

	/*
	* 名称：断开静默连接
	* 描述：断开超过指定时长的静默连接
	*		
	* 参数：		dwPeriod	-- 时长（毫秒）
	*			bForce		-- 是否强制断开连接
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败
	*/
	virtual BOOL DisconnectSilenceConnections(DWORD dwPeriod, BOOL bForce = TRUE)	= 0;

	/*
	* 名称：等待
	* 描述：等待通信组件停止运行
	*		
	* 参数：		dwMilliseconds	-- 超时时间（毫秒，默认：-1，永不超时）
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL Wait(DWORD dwMilliseconds = INFINITE)								= 0;

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/*
	* 名称：设置连接的附加数据
	* 描述：是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序自身决定
	*		
	* 参数：		dwConnID	-- 连接 ID
	*			pv			-- 数据
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败（无效的连接 ID）
	*/
	virtual BOOL SetConnectionExtra		(CONNID dwConnID, PVOID pExtra)			= 0;

	/*
	* 名称：获取连接的附加数据
	* 描述：是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序自身决定
	*		
	* 参数：		dwConnID	-- 连接 ID
	*			ppv			-- 数据指针
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败（无效的连接 ID）
	*/
	virtual BOOL GetConnectionExtra		(CONNID dwConnID, PVOID* ppExtra)		= 0;

	/* 检测是否为安全连接（SSL/HTTPS） */
	virtual BOOL IsSecure				()										= 0;
	/* 检查通信组件是否已启动 */
	virtual BOOL HasStarted				()										= 0;
	/* 查看通信组件当前状态 */
	virtual EnServiceState GetState		()										= 0;
	/* 获取连接数 */
	virtual DWORD GetConnectionCount	()										= 0;
	/* 获取所有连接的 CONNID */
	virtual BOOL GetAllConnectionIDs	(CONNID pIDs[], DWORD& dwCount)			= 0;
	/* 获取某个连接时长（毫秒） */
	virtual BOOL GetConnectPeriod		(CONNID dwConnID, DWORD& dwPeriod)		= 0;
	/* 获取某个连接静默时间（毫秒） */
	virtual BOOL GetSilencePeriod		(CONNID dwConnID, DWORD& dwPeriod)		= 0;
	/* 获取某个连接的本地地址信息 */
	virtual BOOL GetLocalAddress		(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)	= 0;
	/* 获取某个连接的远程地址信息 */
	virtual BOOL GetRemoteAddress		(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)	= 0;
	/* 获取最近一次失败操作的错误代码 */
	virtual EnSocketError GetLastError	()										= 0;
	/* 获取最近一次失败操作的错误描述 */
	virtual LPCTSTR GetLastErrorDesc	()										= 0;
	/* 获取连接中未发出数据的长度 */
	virtual BOOL GetPendingDataLength	(CONNID dwConnID, int& iPending)		= 0;
	/* 获取连接的数据接收状态 */
	virtual BOOL IsPauseReceive			(CONNID dwConnID, BOOL& bPaused)		= 0;
	/* 检测是否有效连接 */
	virtual BOOL IsConnected			(CONNID dwConnID)						= 0;

	/* 设置地址重用选项 */
	virtual void SetReuseAddressPolicy(EnReuseAddressPolicy enReusePolicy)		= 0;
	/* 设置数据发送策略（对 Linux 平台组件无效） */
	virtual void SetSendPolicy				(EnSendPolicy enSendPolicy)			= 0;
	/* 设置 OnSend 事件同步策略（对 Linux 平台组件无效） */
	virtual void SetOnSendSyncPolicy		(EnOnSendSyncPolicy enSyncPolicy)	= 0;
	/* 设置最大连接数（组件会根据设置值预分配内存，因此需要根据实际情况设置，不宜过大）*/
	virtual void SetMaxConnectionCount		(DWORD dwMaxConnectionCount)		= 0;
	/* 设置 Socket 缓存对象锁定时间（毫秒，在锁定期间该 Socket 缓存对象不能被获取使用） */
	virtual void SetFreeSocketObjLockTime	(DWORD dwFreeSocketObjLockTime)		= 0;
	/* 设置 Socket 缓存池大小（通常设置为平均并发连接数的 1/3 - 1/2） */
	virtual void SetFreeSocketObjPool		(DWORD dwFreeSocketObjPool)			= 0;
	/* 设置内存块缓存池大小（通常设置为 Socket 缓存池大小的 2 - 3 倍） */
	virtual void SetFreeBufferObjPool		(DWORD dwFreeBufferObjPool)			= 0;
	/* 设置 Socket 缓存池回收阀值（通常设置为 Socket 缓存池大小的 3 倍） */
	virtual void SetFreeSocketObjHold		(DWORD dwFreeSocketObjHold)			= 0;
	/* 设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍） */
	virtual void SetFreeBufferObjHold		(DWORD dwFreeBufferObjHold)			= 0;
	/* 设置工作线程数量（通常设置为 2 * CPU + 2） */
	virtual void SetWorkerThreadCount		(DWORD dwWorkerThreadCount)			= 0;
	/* 设置是否标记静默时间（设置为 TRUE 时 DisconnectSilenceConnections() 和 GetSilencePeriod() 才有效，默认：TRUE） */
	virtual void SetMarkSilence				(BOOL bMarkSilence)					= 0;

	/* 获取地址重用选项 */
	virtual EnReuseAddressPolicy GetReuseAddressPolicy	()	= 0;
	/* 获取数据发送策略（对 Linux 平台组件无效） */
	virtual EnSendPolicy GetSendPolicy					()	= 0;
	/* 获取 OnSend 事件同步策略（对 Linux 平台组件无效） */
	virtual EnOnSendSyncPolicy GetOnSendSyncPolicy		()	= 0;
	/* 获取最大连接数 */
	virtual DWORD GetMaxConnectionCount					()	= 0;
	/* 获取 Socket 缓存对象锁定时间 */
	virtual DWORD GetFreeSocketObjLockTime				()	= 0;
	/* 获取 Socket 缓存池大小 */
	virtual DWORD GetFreeSocketObjPool					()	= 0;
	/* 获取内存块缓存池大小 */
	virtual DWORD GetFreeBufferObjPool					()	= 0;
	/* 获取 Socket 缓存池回收阀值 */
	virtual DWORD GetFreeSocketObjHold					()	= 0;
	/* 获取内存块缓存池回收阀值 */
	virtual DWORD GetFreeBufferObjHold					()	= 0;
	/* 获取工作线程数量 */
	virtual DWORD GetWorkerThreadCount					()	= 0;
	/* 检测是否标记静默时间 */
	virtual BOOL IsMarkSilence							()	= 0;

public:
	virtual ~IComplexSocket() = default;
};

/************************************************************************
名称：通信服务端组件接口
描述：定义通信服务端组件的所有操作方法和属性访问方法
************************************************************************/
class IServer : public IComplexSocket
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

	/*
	* 名称：启动通信组件
	* 描述：启动服务端通信组件，启动完成后可开始接收客户端连接并收发数据
	*		
	* 参数：		lpszBindAddress	-- 监听地址
	*			usPort			-- 监听端口
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 GetLastError() 获取错误代码
	*/
	virtual BOOL Start	(LPCTSTR lpszBindAddress, USHORT usPort)							= 0;

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 获取监听 Socket 的地址信息 */
	virtual BOOL GetListenAddress(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)	= 0;
};

/************************************************************************
名称：TCP 通信服务端组件接口
描述：定义 TCP 通信服务端组件的所有操作方法和属性访问方法
************************************************************************/
class ITcpServer : public IServer
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

	/*
	* 名称：发送小文件
	* 描述：向指定连接发送 4096 KB 以下的小文件
	*		
	* 参数：		dwConnID		-- 连接 ID
	*			lpszFileName	-- 文件路径
	*			pHead			-- 头部附加数据
	*			pTail			-- 尾部附加数据
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL SendSmallFile(CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr)	= 0;

#ifdef _SSL_SUPPORT
	/*
	* 名称：初始化通信组件 SSL 环境参数
	* 描述：SSL 环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
	*		
	* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
	*			lpszPemCertFile			-- 证书文件
	*			lpszPemKeyFile			-- 私钥文件
	*			lpszKeyPassword			-- 私钥密码（没有密码则为空）
	*			lpszCAPemCertFileOrPath	-- CA 证书文件或目录（单向验证或客户端可选）
	*			fnServerNameCallback	-- SNI 回调函数指针（可选）
	*
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual BOOL SetupSSLContext(int iVerifyMode = SSL_VM_NONE, LPCTSTR lpszPemCertFile = nullptr, LPCTSTR lpszPemKeyFile = nullptr, LPCTSTR lpszKeyPassword = nullptr, LPCTSTR lpszCAPemCertFileOrPath = nullptr, Fn_SNI_ServerNameCallback fnServerNameCallback = nullptr)	= 0;

	/*
	* 名称：初始化通信组件 SSL 环境参数（通过内存加载证书）
	* 描述：SSL 环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
	*		
	* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
	*			lpszPemCert				-- 证书内容
	*			lpszPemKey				-- 私钥内容
	*			lpszKeyPassword			-- 私钥密码（没有密码则为空）
	*			lpszCAPemCert			-- CA 证书内容（单向验证或客户端可选）
	*			fnServerNameCallback	-- SNI 回调函数指针（可选，如果为 nullptr 则使用 SNI 默认回调函数）
	*
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual BOOL SetupSSLContextByMemory(int iVerifyMode = SSL_VM_NONE, LPCSTR lpszPemCert = nullptr, LPCSTR lpszPemKey = nullptr, LPCSTR lpszKeyPassword = nullptr, LPCSTR lpszCAPemCert = nullptr, Fn_SNI_ServerNameCallback fnServerNameCallback = nullptr)				= 0;

	/*
	* 名称：增加 SNI 主机证书
	* 描述：SSL 服务端在 SetupSSLContext() 成功后可以调用本方法增加多个 SNI 主机证书
	*		
	* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
	*			lpszPemCertFile			-- 证书文件
	*			lpszPemKeyFile			-- 私钥文件
	*			lpszKeyPassword			-- 私钥密码（没有密码则为空）
	*			lpszCAPemCertFileOrPath	-- CA 证书文件或目录（单向验证可选）
	*
	* 返回值：	正数		-- 成功，并返回 SNI 主机证书对应的索引，该索引用于在 SNI 回调函数中定位 SNI 主机
	*			负数		-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual int AddSSLContext(int iVerifyMode = SSL_VM_NONE, LPCTSTR lpszPemCertFile = nullptr, LPCTSTR lpszPemKeyFile = nullptr, LPCTSTR lpszKeyPassword = nullptr, LPCTSTR lpszCAPemCertFileOrPath = nullptr)																= 0;

	/*
	* 名称：增加 SNI 主机证书（通过内存加载证书）
	* 描述：SSL 服务端在 SetupSSLContext() 成功后可以调用本方法增加多个 SNI 主机证书
	*		
	* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
	*			lpszPemCert				-- 证书内容
	*			lpszPemKey				-- 私钥内容
	*			lpszKeyPassword			-- 私钥密码（没有密码则为空）
	*			lpszCAPemCert			-- CA 证书内容（单向验证可选）
	*
	* 返回值：	正数		-- 成功，并返回 SNI 主机证书对应的索引，该索引用于在 SNI 回调函数中定位 SNI 主机
	*			负数		-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual int AddSSLContextByMemory(int iVerifyMode = SSL_VM_NONE, LPCSTR lpszPemCert = nullptr, LPCSTR lpszPemKey = nullptr, LPCSTR lpszKeyPassword = nullptr, LPCSTR lpszCAPemCert = nullptr)																			= 0;

	/*
	* 名称：绑定 SNI 主机域名
	* 描述：SSL 服务端在 AddSSLContext() 成功后可以调用本方法绑定主机域名到 SNI 主机证书
	*		
	* 参数：		lpszServerName		-- 主机域名
	*			iContextIndex		-- SNI 主机证书对应的索引
	*
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual BOOL BindSSLServerName(LPCTSTR lpszServerName, int iContextIndex)	= 0;

	/*
	* 名称：清理通信组件 SSL 运行环境
	* 描述：清理通信组件 SSL 运行环境，回收 SSL 相关内存
	*		1、通信组件析构时会自动调用本方法
	*		2、当要重新设置通信组件 SSL 环境参数时，需要先调用本方法清理原先的环境参数
	*		
	* 参数：	无
	* 
	* 返回值：无
	*/
	virtual void CleanupSSLContext()											= 0;

	/*
	* 名称：启动 SSL 握手
	* 描述：当通信组件设置为非自动握手时，需要调用本方法启动 SSL 握手
	*		
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual BOOL StartSSLHandShake(CONNID dwConnID)								= 0;

#endif

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 设置 EPOLL 等待事件的最大数量 */
	virtual void SetAcceptSocketCount	(DWORD dwAcceptSocketCount)		= 0;
	/* 设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为 1024 的倍数） */
	virtual void SetSocketBufferSize	(DWORD dwSocketBufferSize)		= 0;
	/* 设置监听 Socket 的等候队列大小（根据并发连接数量调整设置） */
	virtual void SetSocketListenQueue	(DWORD dwSocketListenQueue)		= 0;
	/* 设置正常心跳包间隔（毫秒，0 则不发送心跳包，默认：60 * 1000） */
	virtual void SetKeepAliveTime		(DWORD dwKeepAliveTime)			= 0;
	/* 设置异常心跳包间隔（毫秒，0 不发送心跳包，，默认：20 * 1000，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线） */
	virtual void SetKeepAliveInterval	(DWORD dwKeepAliveInterval)		= 0;

	/* 获取 EPOLL 等待事件的最大数量 */
	virtual DWORD GetAcceptSocketCount	()	= 0;
	/* 获取通信数据缓冲区大小 */
	virtual DWORD GetSocketBufferSize	()	= 0;
	/* 获取监听 Socket 的等候队列大小 */
	virtual DWORD GetSocketListenQueue	()	= 0;
	/* 获取正常心跳包间隔 */
	virtual DWORD GetKeepAliveTime		()	= 0;
	/* 获取异常心跳包间隔 */
	virtual DWORD GetKeepAliveInterval	()	= 0;

#ifdef _SSL_SUPPORT
	/* 设置通信组件握手方式（默认：TRUE，自动握手） */
	virtual void SetSSLAutoHandShake(BOOL bAutoHandShake)				= 0;
	/* 获取通信组件握手方式 */
	virtual BOOL IsSSLAutoHandShake()									= 0;

	/* 设置 SSL 加密算法列表 */
	virtual void SetSSLCipherList(LPCTSTR lpszCipherList)				= 0;
	/* 获取 SSL 加密算法列表 */
	virtual LPCTSTR GetSSLCipherList()									= 0;

	/*
	* 名称：获取 SSL Session 信息
	* 描述：获取指定类型的 SSL Session 信息（输出类型参考：EnSSLSessionInfo）
	*		
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual BOOL GetSSLSessionInfo(CONNID dwConnID, EnSSLSessionInfo enInfo, LPVOID* lppInfo)	= 0;
#endif

};

#ifdef _UDP_SUPPORT

/************************************************************************
名称：UDP 通信服务端组件接口
描述：定义 UDP 通信服务端组件的所有操作方法和属性访问方法
************************************************************************/
class IUdpServer : public IServer
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 设置数据报文最大长度（建议在局域网环境下不超过 1472 字节，在广域网环境下不超过 548 字节） */
	virtual void SetMaxDatagramSize		(DWORD dwMaxDatagramSize)	= 0;
	/* 获取数据报文最大长度 */
	virtual DWORD GetMaxDatagramSize	()							= 0;

	/* 设置 Receive 预投递数量（根据负载调整设置，Receive 预投递数量越大则丢包概率越小） */
	virtual void SetPostReceiveCount	(DWORD dwPostReceiveCount)	= 0;
	/* 获取 Receive 预投递数量 */
	virtual DWORD GetPostReceiveCount	()							= 0;

	/* 设置监测包尝试次数（0 则不发送监测跳包，如果超过最大尝试次数则认为已断线） */
	virtual void SetDetectAttempts		(DWORD dwDetectAttempts)	= 0;
	/* 设置监测包发送间隔（毫秒，0 不发送监测包） */
	virtual void SetDetectInterval		(DWORD dwDetectInterval)	= 0;
	/* 获取心跳检查次数 */
	virtual DWORD GetDetectAttempts		()							= 0;
	/* 获取心跳检查间隔 */
	virtual DWORD GetDetectInterval		()							= 0;
};

/************************************************************************
名称：Server/Agent ARQ 模型组件接口
描述：定义 Server/Agent 组件的 ARQ 模型组件的所有操作方法
************************************************************************/
class IArqSocket
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 设置是否开启 nodelay 模式（默认：FALSE，不开启） */
	virtual void SetNoDelay				(BOOL bNoDelay)				= 0;
	/* 设置是否关闭拥塞控制（默认：FALSE，不关闭） */
	virtual void SetTurnoffCongestCtrl	(BOOL bTurnOff)				= 0;
	/* 设置数据刷新间隔（毫秒，默认：60） */
	virtual void SetFlushInterval		(DWORD dwFlushInterval)		= 0;
	/* 设置快速重传 ACK 跨越次数（默认：0，关闭快速重传） */
	virtual void SetResendByAcks		(DWORD dwResendByAcks)		= 0;
	/* 设置发送窗口大小（数据包数量，默认：128） */
	virtual void SetSendWndSize			(DWORD dwSendWndSize)		= 0;
	/* 设置接收窗口大小（数据包数量，默认：512） */
	virtual void SetRecvWndSize			(DWORD dwRecvWndSize)		= 0;
	/* 设置最小重传超时时间（毫秒，默认：30） */
	virtual void SetMinRto				(DWORD dwMinRto)			= 0;
	/* 设置快速握手次数限制（默认：5，如果为 0 则不限制） */
	virtual void SetFastLimit			(DWORD dwFastLimit)			= 0;
	/* 设置最大传输单元（默认：0，与 SetMaxDatagramSize() 一致） */
	virtual void SetMaxTransUnit		(DWORD dwMaxTransUnit)		= 0;
	/* 设置最大数据包大小（默认：4096） */
	virtual void SetMaxMessageSize		(DWORD dwMaxMessageSize)	= 0;
	/* 设置握手超时时间（毫秒，默认：5000） */
	virtual void SetHandShakeTimeout	(DWORD dwHandShakeTimeout)	= 0;

	/* 检测是否开启 nodelay 模式 */
	virtual BOOL IsNoDelay				()							= 0;
	/* 检测是否关闭拥塞控制 */
	virtual BOOL IsTurnoffCongestCtrl	()							= 0;
	/* 获取数据刷新间隔 */
	virtual DWORD GetFlushInterval		()							= 0;
	/* 获取快速重传 ACK 跨越次数 */
	virtual DWORD GetResendByAcks		()							= 0;
	/* 获取发送窗口大小 */
	virtual DWORD GetSendWndSize		()							= 0;
	/* 获取接收窗口大小 */
	virtual DWORD GetRecvWndSize		()							= 0;
	/* 获取最小重传超时时间 */
	virtual DWORD GetMinRto				()							= 0;
	/* 获取快速握手次数限制 */
	virtual DWORD GetFastLimit			()							= 0;
	/* 获取最大传输单元 */
	virtual DWORD GetMaxTransUnit		()							= 0;
	/* 获取最大数据包大小 */
	virtual DWORD GetMaxMessageSize		()							= 0;
	/* 获取握手超时时间 */
	virtual DWORD GetHandShakeTimeout	()							= 0;

	/* 获取等待发送包数量 */
	virtual BOOL GetWaitingSendMessageCount	(CONNID dwConnID, int& iCount)	= 0;

public:
	virtual ~IArqSocket() = default;
};

/************************************************************************
名称：UDP ARQ 通信服务端组件接口
描述：继承了 ARQ 和 Server 接口
************************************************************************/
typedef	DualInterface<IArqSocket, IUdpServer>	IUdpArqServer;

#endif

/************************************************************************
名称：通信代理组件接口
描述：定义通信代理组件的所有操作方法和属性访问方法，代理组件本质是一个同时连接多个服务器的客户端组件
************************************************************************/
class IAgent : public IComplexSocket
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

	/*
	* 名称：启动通信组件
	* 描述：启动通信代理组件，启动完成后可开始连接远程服务器
	*		
	* 参数：		lpszBindAddress	-- 绑定地址（默认：nullptr，绑定任意地址）
	*			bAsyncConnect	-- 是否采用异步 Connect
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 GetLastError() 获取错误代码
	*/
	virtual BOOL Start	(LPCTSTR lpszBindAddress = nullptr, BOOL bAsyncConnect = TRUE)				= 0;

	/*
	* 名称：连接服务器
	* 描述：连接服务器，连接成功后 IAgentListener 会接收到 OnConnect() / OnHandShake() 事件
	*		
	* 参数：		lpszRemoteAddress	-- 服务端地址
	*			usPort				-- 服务端端口
	*			pdwConnID			-- 连接 ID（默认：nullptr，不获取连接 ID）
	*			pExtra				-- 连接附加数据（默认：nullptr）
	*			usLocalPort			-- 本地端口（默认：0）
	*			lpszLocalAddress	-- 本地地址（默认：nullptr，使用 Start() 方法中绑定的地址）
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL Connect(LPCTSTR lpszRemoteAddress, USHORT usPort, CONNID* pdwConnID = nullptr, PVOID pExtra = nullptr, USHORT usLocalPort = 0, LPCTSTR lpszLocalAddress = nullptr)	= 0;


public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 获取某个连接的远程主机信息 */
	virtual BOOL GetRemoteHost	(CONNID dwConnID, TCHAR lpszHost[], int& iHostLen, USHORT& usPort)	= 0;

};

/************************************************************************
名称：TCP 通信代理组件接口
描述：定义 TCP 通信代理组件的所有操作方法和属性访问方法
************************************************************************/
class ITcpAgent : public IAgent
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

	/*
	* 名称：发送小文件
	* 描述：向指定连接发送 4096 KB 以下的小文件
	*		
	* 参数：		dwConnID		-- 连接 ID
	*			lpszFileName	-- 文件路径
	*			pHead			-- 头部附加数据
	*			pTail			-- 尾部附加数据
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL SendSmallFile(CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr)	= 0;

#ifdef _SSL_SUPPORT
	/*
	* 名称：初始化通信组件 SSL 环境参数
	* 描述：SSL 环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
	*		
	* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
	*			lpszPemCertFile			-- 证书文件（客户端可选）
	*			lpszPemKeyFile			-- 私钥文件（客户端可选）
	*			lpszKeyPassword			-- 私钥密码（没有密码则为空）
	*			lpszCAPemCertFileOrPath	-- CA 证书文件或目录（单向验证或客户端可选）
	*
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual BOOL SetupSSLContext(int iVerifyMode = SSL_VM_NONE, LPCTSTR lpszPemCertFile = nullptr, LPCTSTR lpszPemKeyFile = nullptr, LPCTSTR lpszKeyPassword = nullptr, LPCTSTR lpszCAPemCertFileOrPath = nullptr)	= 0;

	/*
	* 名称：初始化通信组件 SSL 环境参数（通过内存加载证书）
	* 描述：SSL 环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
	*		
	* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
	*			lpszPemCert				-- 证书内容
	*			lpszPemKey				-- 私钥内容
	*			lpszKeyPassword			-- 私钥密码（没有密码则为空）
	*			lpszCAPemCert			-- CA 证书内容（单向验证或客户端可选）
	*
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual BOOL SetupSSLContextByMemory(int iVerifyMode = SSL_VM_NONE, LPCSTR lpszPemCert = nullptr, LPCSTR lpszPemKey = nullptr, LPCSTR lpszKeyPassword = nullptr, LPCSTR lpszCAPemCert = nullptr)					= 0;

	/*
	* 名称：清理通信组件 SSL 运行环境
	* 描述：清理通信组件 SSL 运行环境，回收 SSL 相关内存
	*		1、通信组件析构时会自动调用本方法
	*		2、当要重新设置通信组件 SSL 环境参数时，需要先调用本方法清理原先的环境参数
	*		
	* 参数：	无
	* 
	* 返回值：无
	*/
	virtual void CleanupSSLContext()						= 0;

	/*
	* 名称：启动 SSL 握手
	* 描述：当通信组件设置为非自动握手时，需要调用本方法启动 SSL 握手
	*		
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual BOOL StartSSLHandShake(CONNID dwConnID)			= 0;

#endif

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为 1024 的倍数） */
	virtual void SetSocketBufferSize	(DWORD dwSocketBufferSize)		= 0;
	/* 设置正常心跳包间隔（毫秒，0 则不发送心跳包，默认：60 * 1000） */
	virtual void SetKeepAliveTime		(DWORD dwKeepAliveTime)			= 0;
	/* 设置异常心跳包间隔（毫秒，0 不发送心跳包，，默认：20 * 1000，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线） */
	virtual void SetKeepAliveInterval	(DWORD dwKeepAliveInterval)		= 0;

	/* 获取通信数据缓冲区大小 */
	virtual DWORD GetSocketBufferSize	()	= 0;
	/* 获取正常心跳包间隔 */
	virtual DWORD GetKeepAliveTime		()	= 0;
	/* 获取异常心跳包间隔 */
	virtual DWORD GetKeepAliveInterval	()	= 0;

#ifdef _SSL_SUPPORT
	/* 设置通信组件握手方式（默认：TRUE，自动握手） */
	virtual void SetSSLAutoHandShake(BOOL bAutoHandShake)				= 0;
	/* 获取通信组件握手方式 */
	virtual BOOL IsSSLAutoHandShake()									= 0;

	/* 设置 SSL 加密算法列表 */
	virtual void SetSSLCipherList(LPCTSTR lpszCipherList)				= 0;
	/* 获取 SSL 加密算法列表 */
	virtual LPCTSTR GetSSLCipherList()									= 0;

	/*
	* 名称：获取 SSL Session 信息
	* 描述：获取指定类型的 SSL Session 信息（输出类型参考：EnSSLSessionInfo）
	*		
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual BOOL GetSSLSessionInfo(CONNID dwConnID, EnSSLSessionInfo enInfo, LPVOID* lppInfo)	= 0;
#endif

};

/************************************************************************
名称：通信客户端组件接口
描述：定义通信客户端组件的所有操作方法和属性访问方法
************************************************************************/
class IClient
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

	/*
	* 名称：启动通信组件
	* 描述：启动客户端通信组件并连接服务端，启动完成后可开始收发数据
	*		
	* 参数：		lpszRemoteAddress	-- 服务端地址
	*			usPort				-- 服务端端口
	*			bAsyncConnect		-- 是否采用异步 Connect
	*			lpszBindAddress		-- 绑定地址（默认：nullptr，TcpClient/UdpClient -> 不执行绑定操作，UdpCast 绑定 -> 任意地址）
	*			usLocalPort			-- 本地端口（默认：0）
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 GetLastError() 获取错误代码
	*/
	virtual BOOL Start	(LPCTSTR lpszRemoteAddress, USHORT usPort, BOOL bAsyncConnect = TRUE, LPCTSTR lpszBindAddress = nullptr, USHORT usLocalPort = 0)	= 0;

	/*
	* 名称：关闭通信组件
	* 描述：关闭客户端通信组件，关闭完成后断开与服务端的连接并释放所有资源
	*		
	* 参数：	
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 GetLastError() 获取错误代码
	*/
	virtual BOOL Stop	()																		= 0;

	/*
	* 名称：发送数据
	* 描述：向服务端发送数据
	*		
	* 参数：		pBuffer		-- 发送缓冲区
	*			iLength		-- 发送缓冲区长度
	*			iOffset		-- 发送缓冲区指针偏移量
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL Send	(const BYTE* pBuffer, int iLength, int iOffset = 0)						= 0;

	/*
	* 名称：发送多组数据
	* 描述：向服务端发送多组数据
	*		TCP - 顺序发送所有数据包 
	*		UDP - 把所有数据包组合成一个数据包发送（数据包的总长度不能大于设置的 UDP 包最大长度） 
	*		
	* 参数：		pBuffers	-- 发送缓冲区数组
	*			iCount		-- 发送缓冲区数目
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL SendPackets(const WSABUF pBuffers[], int iCount)								= 0;

	/*
	* 名称：暂停/恢复接收
	* 描述：暂停/恢复某个连接的数据接收工作
	*		
	*			bPause	-- TRUE - 暂停, FALSE - 恢复
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败
	*/
	virtual BOOL PauseReceive(BOOL bPause = TRUE)												= 0;

	/*
	* 名称：等待
	* 描述：等待通信组件停止运行
	*		
	* 参数：		dwMilliseconds	-- 超时时间（毫秒，默认：-1，永不超时）
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL Wait(DWORD dwMilliseconds = INFINITE)											= 0;

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 设置连接的附加数据 */
	virtual void SetExtra					(PVOID pExtra)										= 0;

	/* 获取连接的附加数据 */
	virtual PVOID GetExtra					()													= 0;

	/* 检测是否为安全连接（SSL/HTTPS） */
	virtual BOOL IsSecure					()													= 0;
	/* 检查通信组件是否已启动 */
	virtual BOOL HasStarted					()													= 0;
	/* 查看通信组件当前状态 */
	virtual EnServiceState	GetState		()													= 0;
	/* 获取最近一次失败操作的错误代码 */
	virtual EnSocketError	GetLastError	()													= 0;
	/* 获取最近一次失败操作的错误描述 */
	virtual LPCTSTR			GetLastErrorDesc()													= 0;
	/* 获取该组件对象的连接 ID */
	virtual CONNID			GetConnectionID	()													= 0;
	/* 获取 Client Socket 的地址信息 */
	virtual BOOL GetLocalAddress		(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)	= 0;
	/* 获取连接的远程主机信息 */
	virtual BOOL GetRemoteHost			(TCHAR lpszHost[], int& iHostLen, USHORT& usPort)		= 0;
	/* 获取连接中未发出数据的长度 */
	virtual BOOL GetPendingDataLength	(int& iPending)											= 0;
	/* 获取连接的数据接收状态 */
	virtual BOOL IsPauseReceive			(BOOL& bPaused)											= 0;
	/* 检测是否有效连接 */
	virtual BOOL IsConnected			()														= 0;

	/* 设置地址重用选项 */
	virtual void SetReuseAddressPolicy(EnReuseAddressPolicy enReusePolicy)						= 0;
	/* 设置内存块缓存池大小（通常设置为 -> PUSH 模型：5 - 10；PULL 模型：10 - 20 ） */
	virtual void SetFreeBufferPoolSize		(DWORD dwFreeBufferPoolSize)						= 0;
	/* 设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍） */
	virtual void SetFreeBufferPoolHold		(DWORD dwFreeBufferPoolHold)						= 0;

	/* 获取地址重用选项 */
	virtual EnReuseAddressPolicy GetReuseAddressPolicy	()										= 0;
	/* 获取内存块缓存池大小 */
	virtual DWORD GetFreeBufferPoolSize					()										= 0;
	/* 获取内存块缓存池回收阀值 */
	virtual DWORD GetFreeBufferPoolHold					()										= 0;

public:
	virtual ~IClient() = default;
};

/************************************************************************
名称：TCP 通信客户端组件接口
描述：定义 TCP 通信客户端组件的所有操作方法和属性访问方法
************************************************************************/
class ITcpClient : public IClient
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

	/*
	* 名称：发送小文件
	* 描述：向服务端发送 4096 KB 以下的小文件
	*		
	* 参数：		lpszFileName	-- 文件路径
	*			pHead			-- 头部附加数据
	*			pTail			-- 尾部附加数据
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL SendSmallFile(LPCTSTR lpszFileName, const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr)	= 0;

#ifdef _SSL_SUPPORT
	/*
	* 名称：初始化通信组件 SSL 环境参数
	* 描述：SSL 环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
	*		
	* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
	*			lpszPemCertFile			-- 证书文件（客户端可选）
	*			lpszPemKeyFile			-- 私钥文件（客户端可选）
	*			lpszKeyPassword			-- 私钥密码（没有密码则为空）
	*			lpszCAPemCertFileOrPath	-- CA 证书文件或目录（单向验证或客户端可选）
	*
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual BOOL SetupSSLContext(int iVerifyMode = SSL_VM_NONE, LPCTSTR lpszPemCertFile = nullptr, LPCTSTR lpszPemKeyFile = nullptr, LPCTSTR lpszKeyPassword = nullptr, LPCTSTR lpszCAPemCertFileOrPath = nullptr)	= 0;

	/*
	* 名称：初始化通信组件 SSL 环境参数（通过内存加载证书）
	* 描述：SSL 环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
	*		
	* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
	*			lpszPemCert				-- 证书内容
	*			lpszPemKey				-- 私钥内容
	*			lpszKeyPassword			-- 私钥密码（没有密码则为空）
	*			lpszCAPemCert			-- CA 证书内容（单向验证或客户端可选）
	*
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual BOOL SetupSSLContextByMemory(int iVerifyMode = SSL_VM_NONE, LPCSTR lpszPemCert = nullptr, LPCSTR lpszPemKey = nullptr, LPCSTR lpszKeyPassword = nullptr, LPCSTR lpszCAPemCert = nullptr)					= 0;

	/*
	* 名称：清理通信组件 SSL 运行环境
	* 描述：清理通信组件 SSL 运行环境，回收 SSL 相关内存
	*		1、通信组件析构时会自动调用本方法
	*		2、当要重新设置通信组件 SSL 环境参数时，需要先调用本方法清理原先的环境参数
	*		
	* 参数：	无
	* 
	* 返回值：无
	*/
	virtual void CleanupSSLContext()	= 0;

	/*
	* 名称：启动 SSL 握手
	* 描述：当通信组件设置为非自动握手时，需要调用本方法启动 SSL 握手
	*		
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual BOOL StartSSLHandShake()	= 0;

#endif

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为：(N * 1024) - sizeof(TBufferObj)） */
	virtual void SetSocketBufferSize	(DWORD dwSocketBufferSize)	= 0;
	/* 设置正常心跳包间隔（毫秒，0 则不发送心跳包，默认：60 * 1000） */
	virtual void SetKeepAliveTime		(DWORD dwKeepAliveTime)		= 0;
	/* 设置异常心跳包间隔（毫秒，0 不发送心跳包，，默认：20 * 1000，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线） */
	virtual void SetKeepAliveInterval	(DWORD dwKeepAliveInterval)	= 0;

	/* 获取通信数据缓冲区大小 */
	virtual DWORD GetSocketBufferSize	()	= 0;
	/* 获取正常心跳包间隔 */
	virtual DWORD GetKeepAliveTime		()	= 0;
	/* 获取异常心跳包间隔 */
	virtual DWORD GetKeepAliveInterval	()	= 0;

#ifdef _SSL_SUPPORT
	/* 设置通信组件握手方式（默认：TRUE，自动握手） */
	virtual void SetSSLAutoHandShake(BOOL bAutoHandShake)	= 0;
	/* 获取通信组件握手方式 */
	virtual BOOL IsSSLAutoHandShake()						= 0;

	/* 设置 SSL 加密算法列表 */
	virtual void SetSSLCipherList(LPCTSTR lpszCipherList)	= 0;
	/* 获取 SSL 加密算法列表 */
	virtual LPCTSTR GetSSLCipherList()						= 0;

	/*
	* 名称：获取 SSL Session 信息
	* 描述：获取指定类型的 SSL Session 信息（输出类型参考：EnSSLSessionInfo）
	*		
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual BOOL GetSSLSessionInfo(EnSSLSessionInfo enInfo, LPVOID* lppInfo)	= 0;
#endif

};

#ifdef _UDP_SUPPORT

/************************************************************************
名称：UDP 通信客户端组件接口
描述：定义 UDP 通信客户端组件的所有操作方法和属性访问方法
************************************************************************/
class IUdpClient : public IClient
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 设置数据报文最大长度（建议在局域网环境下不超过 1472 字节，在广域网环境下不超过 548 字节） */
	virtual void SetMaxDatagramSize	(DWORD dwMaxDatagramSize)	= 0;
	/* 获取数据报文最大长度 */
	virtual DWORD GetMaxDatagramSize()							= 0;

	/* 设置监测包尝试次数（0 则不发送监测跳包，如果超过最大尝试次数则认为已断线） */
	virtual void SetDetectAttempts	(DWORD dwDetectAttempts)	= 0;
	/* 设置监测包发送间隔（毫秒，0 不发送监测包） */
	virtual void SetDetectInterval	(DWORD dwDetectInterval)	= 0;
	/* 获取心跳检查次数 */
	virtual DWORD GetDetectAttempts	()							= 0;
	/* 获取心跳检查间隔 */
	virtual DWORD GetDetectInterval	()							= 0;
};

/************************************************************************
名称：UDP 传播组件接口
描述：定义 UDP 传播（组播或广播）组件的所有操作方法和属性访问方法
************************************************************************/
class IUdpCast : public IClient
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 设置数据报文最大长度（建议在局域网环境下不超过 1472 字节，在广域网环境下不超过 548 字节） */
	virtual void SetMaxDatagramSize	(DWORD dwMaxDatagramSize)		= 0;
	/* 获取数据报文最大长度 */
	virtual DWORD GetMaxDatagramSize()								= 0;

	/* 设置传播模式（组播或广播） */
	virtual void SetCastMode		(EnCastMode enCastMode)			= 0;
	/* 获取传播模式 */
	virtual EnCastMode GetCastMode	()								= 0;

	/* 设置组播报文的 TTL（0 - 255） */
	virtual void SetMultiCastTtl	(int iMCTtl)					= 0;
	/* 获取组播报文的 TTL */
	virtual int GetMultiCastTtl		()								= 0;

	/* 设置是否启用组播环路（TRUE or FALSE） */
	virtual void SetMultiCastLoop	(BOOL bMCLoop)					= 0;
	/* 检测是否启用组播环路 */
	virtual BOOL IsMultiCastLoop	()								= 0;

	/* 获取当前数据报的远程地址信息（通常在 OnReceive 事件中调用） */
	virtual BOOL GetRemoteAddress	(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)	= 0;
};

/************************************************************************
名称：UDP 节点组件接口
描述：定义 UDP 节点组件的所有操作方法和属性访问方法
************************************************************************/
class IUdpNode
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

	/*
	* 名称：启动通信组件
	* 描述：启动 UDP 节点通信组件，启动完成后可开始收发数据
	*		
	* 参数：		lpszBindAddress		-- 绑定地址（默认：nullptr，绑定任意地址）
	*			usPort				-- 本地端口（默认：0）
	*			enCastMode			-- 传播模式（默认：CM_UNICAST）
	*			lpszCastAddress		-- 传播地址（默认：nullptr，当 enCaseMode 为 CM_MULTICAST 或 CM_BROADCAST 时有效）
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 GetLastError() 获取错误代码
	*/
	virtual BOOL Start(LPCTSTR lpszBindAddress = nullptr, USHORT usPort = 0, EnCastMode enCastMode = CM_UNICAST, LPCTSTR lpszCastAddress = nullptr)	= 0;

	/*
	* 名称：关闭通信组件
	* 描述：关闭 UDP 节点通信组件，关闭完成后释放所有资源
	*		
	* 参数：	
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 GetLastError() 获取错误代码
	*/
	virtual BOOL Stop()																										= 0;

	/*
	* 名称：发送数据
	* 描述：向指定地址发送数据
	*		
	* 参数：		lpszRemoteAddress	-- 远程地址
	*			usRemotePort		-- 远程端口
	*			pBuffer				-- 发送缓冲区
	*			iLength				-- 发送缓冲区长度
	*			iOffset				-- 发送缓冲区指针偏移量
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL Send(LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pBuffer, int iLength, int iOffset = 0)	= 0;

	/*
	* 名称：发送多组数据
	* 描述：向指定地址发送多组数据，把所有数据包组合成一个数据包发送（数据包的总长度不能大于设置的 UDP 包最大长度） 
	*		
	* 参数：		lpszRemoteAddress	-- 远程地址
	*			usRemotePort		-- 远程端口
	*			pBuffers			-- 发送缓冲区数组
	*			iCount				-- 发送缓冲区数目
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL SendPackets(LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const WSABUF pBuffers[], int iCount)			= 0;

	/*
	* 名称：发送数据
	* 描述：向传播地址发送数据
	*		
	* 参数：		pBuffer		-- 发送缓冲区
	*			iLength		-- 发送缓冲区长度
	*			iOffset		-- 发送缓冲区指针偏移量
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL SendCast(const BYTE* pBuffer, int iLength, int iOffset = 0)												= 0;

	/*
	* 名称：发送多组数据
	* 描述：向传播地址发送多组数据，把所有数据包组合成一个数据包发送（数据包的总长度不能大于设置的 UDP 包最大长度） 
	*		
	* 参数：		pBuffers	-- 发送缓冲区数组
	*			iCount		-- 发送缓冲区数目
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL SendCastPackets(const WSABUF pBuffers[], int iCount)														= 0;

	/*
	* 名称：等待
	* 描述：等待通信组件停止运行
	*		
	* 参数：		dwMilliseconds	-- 超时时间（毫秒，默认：-1，永不超时）
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL Wait(DWORD dwMilliseconds = INFINITE)																		= 0;

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 设置附加数据 */
	virtual void SetExtra					(PVOID pExtra)										= 0;

	/* 获取附加数据 */
	virtual PVOID GetExtra					()													= 0;

	/* 检查通信组件是否已启动 */
	virtual BOOL HasStarted					()													= 0;
	/* 查看通信组件当前状态 */
	virtual EnServiceState GetState			()													= 0;
	/* 获取最近一次失败操作的错误代码 */
	virtual EnSocketError GetLastError		()													= 0;
	/* 获取最近一次失败操作的错误描述 */
	virtual LPCTSTR GetLastErrorDesc		()													= 0;
	/* 获取本节点地址 */
	virtual BOOL GetLocalAddress		(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)	= 0;
	/* 获取本节点传播地址 */
	virtual BOOL GetCastAddress			(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)	= 0;
	/* 获取传播模式 */
	virtual EnCastMode GetCastMode		()														= 0;
	/* 获取未发出数据的长度 */
	virtual BOOL GetPendingDataLength	(int& iPending)											= 0;

	/* 设置数据报文最大长度（建议在局域网环境下不超过 1432 字节，在广域网环境下不超过 548 字节） */
	virtual void SetMaxDatagramSize	(DWORD dwMaxDatagramSize)	= 0;
	/* 获取数据报文最大长度 */
	virtual DWORD GetMaxDatagramSize()							= 0;

	/* 设置组播报文的 TTL（0 - 255） */
	virtual void SetMultiCastTtl	(int iMCTtl)				= 0;
	/* 获取组播报文的 TTL */
	virtual int GetMultiCastTtl		()							= 0;

	/* 设置是否启用组播环路（TRUE or FALSE） */
	virtual void SetMultiCastLoop	(BOOL bMCLoop)				= 0;
	/* 检测是否启用组播环路 */
	virtual BOOL IsMultiCastLoop	()							= 0;

	/* 设置地址重用选项 */
	virtual void SetReuseAddressPolicy(EnReuseAddressPolicy enReusePolicy)	= 0;
	/* 设置工作线程数量（通常设置为 2 * CPU + 2） */
	virtual void SetWorkerThreadCount	(DWORD dwWorkerThreadCount)			= 0;
	/* 设置 Receive 预投递数量（根据负载调整设置，Receive 预投递数量越大则丢包概率越小） */
	virtual void SetPostReceiveCount	(DWORD dwPostReceiveCount)			= 0;
	/* 设置内存块缓存池大小 */
	virtual void SetFreeBufferPoolSize	(DWORD dwFreeBufferPoolSize)		= 0;
	/* 设置内存块缓存池回收阀值 */
	virtual void SetFreeBufferPoolHold	(DWORD dwFreeBufferPoolHold)		= 0;

	/* 获取地址重用选项 */
	virtual EnReuseAddressPolicy GetReuseAddressPolicy()					= 0;
	/* 获取工作线程数量 */
	virtual DWORD GetWorkerThreadCount	()									= 0;
	/* 获取 Receive 预投递数量 */
	virtual DWORD GetPostReceiveCount	()									= 0;
	/* 获取内存块缓存池大小 */
	virtual DWORD GetFreeBufferPoolSize	()									= 0;
	/* 获取内存块缓存池回收阀值 */	
	virtual DWORD GetFreeBufferPoolHold	()									= 0;

public:
	virtual ~IUdpNode() = default;
};

/************************************************************************
名称：Client ARQ 模型组件接口
描述：定义 Client 组件的 ARQ 模型组件的所有操作方法
************************************************************************/
class IArqClient
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 设置是否开启 nodelay 模式（默认：FALSE，不开启） */
	virtual void SetNoDelay				(BOOL bNoDelay)				= 0;
	/* 设置是否关闭拥塞控制（默认：FALSE，不关闭） */
	virtual void SetTurnoffCongestCtrl	(BOOL bTurnOff)				= 0;
	/* 设置数据刷新间隔（毫秒，默认：60） */
	virtual void SetFlushInterval		(DWORD dwFlushInterval)		= 0;
	/* 设置快速重传 ACK 跨越次数（默认：0，关闭快速重传） */
	virtual void SetResendByAcks		(DWORD dwResendByAcks)		= 0;
	/* 设置发送窗口大小（数据包数量，默认：128） */
	virtual void SetSendWndSize			(DWORD dwSendWndSize)		= 0;
	/* 设置接收窗口大小（数据包数量，默认：512） */
	virtual void SetRecvWndSize			(DWORD dwRecvWndSize)		= 0;
	/* 设置最小重传超时时间（毫秒，默认：30） */
	virtual void SetMinRto				(DWORD dwMinRto)			= 0;
	/* 设置快速握手次数限制（默认：5，如果为 0 则不限制） */
	virtual void SetFastLimit			(DWORD dwFastLimit)			= 0;
	/* 设置最大传输单元（默认：0，与 SetMaxDatagramSize() 一致） */
	virtual void SetMaxTransUnit		(DWORD dwMaxTransUnit)		= 0;
	/* 设置最大数据包大小（默认：4096） */
	virtual void SetMaxMessageSize		(DWORD dwMaxMessageSize)	= 0;
	/* 设置握手超时时间（毫秒，默认：5000） */
	virtual void SetHandShakeTimeout	(DWORD dwHandShakeTimeout)	= 0;

	/* 检测是否开启 nodelay 模式 */
	virtual BOOL IsNoDelay				()							= 0;
	/* 检测是否关闭拥塞控制 */
	virtual BOOL IsTurnoffCongestCtrl	()							= 0;
	/* 获取数据刷新间隔 */
	virtual DWORD GetFlushInterval		()							= 0;
	/* 获取快速重传 ACK 跨越次数 */
	virtual DWORD GetResendByAcks		()							= 0;
	/* 获取发送窗口大小 */
	virtual DWORD GetSendWndSize		()							= 0;
	/* 获取接收窗口大小 */
	virtual DWORD GetRecvWndSize		()							= 0;
	/* 获取最小重传超时时间 */
	virtual DWORD GetMinRto				()							= 0;
	/* 获取快速握手次数限制 */
	virtual DWORD GetFastLimit			()							= 0;
	/* 获取最大传输单元 */
	virtual DWORD GetMaxTransUnit		()							= 0;
	/* 获取最大数据包大小 */
	virtual DWORD GetMaxMessageSize		()							= 0;
	/* 获取握手超时时间 */
	virtual DWORD GetHandShakeTimeout	()							= 0;

	/* 获取等待发送包数量 */
	virtual BOOL GetWaitingSendMessageCount	(int& iCount)			= 0;

public:
	virtual ~IArqClient() = default;
};

/************************************************************************
名称：UDP ARQ 通信客户端组件接口
描述：继承了 ARQ 和 Client 接口
************************************************************************/
typedef	DualInterface<IArqClient, IUdpClient>	IUdpArqClient;

#endif

/************************************************************************
名称：Server/Agent PULL 模型组件接口
描述：定义 Server/Agent 组件的 PULL 模型组件的所有操作方法
************************************************************************/
class IPullSocket
{
public:

	/*
	* 名称：抓取数据
	* 描述：用户通过该方法从 Socket 组件中抓取数据
	*		
	* 参数：		dwConnID	-- 连接 ID
	*			pData		-- 抓取缓冲区
	*			iLength		-- 抓取数据长度
	* 返回值：	EnFetchResult
	*/
	virtual EnFetchResult Fetch	(CONNID dwConnID, BYTE* pData, int iLength)	= 0;

	/*
	* 名称：窥探数据（不会移除缓冲区数据）
	* 描述：用户通过该方法从 Socket 组件中窥探数据
	*		
	* 参数：		dwConnID	-- 连接 ID
	*			pData		-- 窥探缓冲区
	*			iLength		-- 窥探数据长度
	* 返回值：	EnFetchResult
	*/
	virtual EnFetchResult Peek	(CONNID dwConnID, BYTE* pData, int iLength)	= 0;

public:
	virtual ~IPullSocket() = default;
};

/************************************************************************
名称：Client PULL 模型组件接口
描述：定义 Client 组件的 PULL 模型组件的所有操作方法
************************************************************************/
class IPullClient
{
public:

	/*
	* 名称：抓取数据
	* 描述：用户通过该方法从 Socket 组件中抓取数据
	*		
	* 参数：		pData		-- 抓取缓冲区
	*			iLength		-- 抓取数据长度
	* 返回值：	EnFetchResult
	*/
	virtual EnFetchResult Fetch	(BYTE* pData, int iLength)	= 0;

	/*
	* 名称：窥探数据（不会移除缓冲区数据）
	* 描述：用户通过该方法从 Socket 组件中窥探数据
	*		
	* 参数：		pData		-- 窥探缓冲区
	*			iLength		-- 窥探数据长度
	* 返回值：	EnFetchResult
	*/
	virtual EnFetchResult Peek	(BYTE* pData, int iLength)	= 0;

public:
	virtual ~IPullClient() = default;
};

/************************************************************************
名称：TCP PULL 模型组件接口
描述：继承了 PULL 和 Socket 接口
************************************************************************/
typedef	DualInterface<IPullSocket, ITcpServer>	ITcpPullServer;
typedef	DualInterface<IPullSocket, ITcpAgent>	ITcpPullAgent;
typedef	DualInterface<IPullClient, ITcpClient>	ITcpPullClient;

/************************************************************************
名称：Server/Agent PACK 模型组件接口
描述：定义 Server/Agent 组件的 PACK 模型组件的所有操作方法
************************************************************************/
class IPackSocket
{
public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 设置数据包最大长度（有效数据包最大长度不能超过 4194303/0x3FFFFF 字节，默认：262144/0x40000） */
	virtual void SetMaxPackSize		(DWORD dwMaxPackSize)			= 0;
	/* 设置包头标识（有效包头标识取值范围 0 ~ 1023/0x3FF，当包头标识为 0 时不校验包头，默认：0） */
	virtual void SetPackHeaderFlag	(USHORT usPackHeaderFlag)		= 0;

	/* 获取数据包最大长度 */
	virtual DWORD GetMaxPackSize	()								= 0;
	/* 获取包头标识 */
	virtual USHORT GetPackHeaderFlag()								= 0;

public:
	virtual ~IPackSocket() = default;
};

/************************************************************************
名称：Client PACK 模型组件接口
描述：定义 Client 组件的 PACK 模型组件的所有操作方法
************************************************************************/
class IPackClient
{
public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 设置数据包最大长度（有效数据包最大长度不能超过 4194303/0x3FFFFF 字节，默认：262144/0x40000） */
	virtual void SetMaxPackSize		(DWORD dwMaxPackSize)			= 0;
	/* 设置包头标识（有效包头标识取值范围 0 ~ 1023/0x3FF，当包头标识为 0 时不校验包头，默认：0） */
	virtual void SetPackHeaderFlag	(USHORT usPackHeaderFlag)		= 0;

	/* 获取数据包最大长度 */
	virtual DWORD GetMaxPackSize	()								= 0;
	/* 获取包头标识 */
	virtual USHORT GetPackHeaderFlag()								= 0;

public:
	virtual ~IPackClient() = default;
};

/************************************************************************
名称：TCP PACK 模型组件接口
描述：继承了 PACK 和 Socket 接口
************************************************************************/
typedef	DualInterface<IPackSocket, ITcpServer>	ITcpPackServer;
typedef	DualInterface<IPackSocket, ITcpAgent>	ITcpPackAgent;
typedef	DualInterface<IPackClient, ITcpClient>	ITcpPackClient;

/************************************************************************
名称：Socket 监听器基接口
描述：定义组件监听器的公共方法
************************************************************************/
template<class T> class ISocketListenerT
{
public:

	/*
	* 名称：握手完成通知
	* 描述：连接完成握手时，Socket 监听器将收到该通知，监听器接收到该通知后才能开始
	*		数据收发操作
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	* 返回值：	HR_OK / HR_IGNORE	-- 继续执行
	*			HR_ERROR			-- 引发 OnClose() 事件并关闭连接
	*/
	virtual EnHandleResult OnHandShake(T* pSender, CONNID dwConnID)												= 0;

	/*
	* 名称：已发送数据通知
	* 描述：成功发送数据后，Socket 监听器将收到该通知
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	*			pData		-- 已发送数据缓冲区
	*			iLength		-- 已发送数据长度
	* 返回值：	HR_OK / HR_IGNORE	-- 继续执行
	*			HR_ERROR			-- 该通知不允许返回 HR_ERROR（调试模式下引发断言错误）
	*/
	virtual EnHandleResult OnSend(T* pSender, CONNID dwConnID, const BYTE* pData, int iLength)					= 0;

	/*
	* 名称：数据到达通知（PUSH 模型）
	* 描述：对于 PUSH 模型的 Socket 通信组件，成功接收数据后将向 Socket 监听器发送该通知
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	*			pData		-- 已接收数据缓冲区
	*			iLength		-- 已接收数据长度
	* 返回值：	HR_OK / HR_IGNORE	-- 继续执行
	*			HR_ERROR			-- 引发 OnClose() 事件并关闭连接
	*/
	virtual EnHandleResult OnReceive(T* pSender, CONNID dwConnID, const BYTE* pData, int iLength)				= 0;

	/*
	* 名称：数据到达通知（PULL 模型）
	* 描述：对于 PULL 模型的 Socket 通信组件，成功接收数据后将向 Socket 监听器发送该通知
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	*			iLength		-- 已接收数据长度
	* 返回值：	HR_OK / HR_IGNORE	-- 继续执行
	*			HR_ERROR			-- 引发 OnClose() 事件并关闭连接
	*/
	virtual EnHandleResult OnReceive(T* pSender, CONNID dwConnID, int iLength)									= 0;

	/*
	* 名称：通信错误通知
	* 描述：通信发生错误后，Socket 监听器将收到该通知，并关闭连接
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	*			enOperation	-- Socket 操作类型
	*			iErrorCode	-- 错误代码
	* 返回值：	忽略返回值
	*/
	virtual EnHandleResult OnClose(T* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)	= 0;

public:
	virtual ~ISocketListenerT() = default;
};

template<class T> class IComplexSocketListenerT : public ISocketListenerT<T>
{
public:

	/*
	* 名称：关闭通信组件通知
	* 描述：通信组件关闭时，Socket 监听器将收到该通知
	*		
	* 参数：		pSender		-- 事件源对象
	* 返回值：忽略返回值
	*/
	virtual EnHandleResult OnShutdown(T* pSender)																= 0;

};

/************************************************************************
名称：服务端 Socket 监听器接口
描述：定义服务端 Socket 监听器的所有事件
************************************************************************/
template<class T> class IServerListenerT : public IComplexSocketListenerT<T>
{
public:

	/*
	* 名称：准备监听通知
	* 描述：通信服务端组件启动时，在监听 Socket 创建完成并开始执行监听前，Socket 监听
	*		器将收到该通知，监听器可以在通知处理方法中执行 Socket 选项设置等额外工作
	*		
	* 参数：		pSender		-- 事件源对象
	*			soListen	-- 监听 Socket
	* 返回值：	HR_OK / HR_IGNORE	-- 继续执行
	*			HR_ERROR			-- 终止启动通信服务组件
	*/
	virtual EnHandleResult OnPrepareListen(T* pSender, SOCKET soListen)						= 0;

	/*
	* 名称：接收连接通知
	* 描述：接收到客户端连接请求时，Socket 监听器将收到该通知，监听器可以在通知处理方
	*		法中执行 Socket 选项设置或拒绝客户端连接等额外工作
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	*			soClient	-- TCP: 客户端 Socket 句柄，UDP: 客户端 Socket SOCKADDR 指针
	* 返回值：	HR_OK / HR_IGNORE	-- 接受连接
	*			HR_ERROR			-- 拒绝连接
	*/
	virtual EnHandleResult OnAccept(T* pSender, CONNID dwConnID, UINT_PTR soClient)			= 0;
};

/************************************************************************
名称：TCP 服务端 Socket 监听器接口
描述：定义 TCP 服务端 Socket 监听器的所有事件
************************************************************************/
class ITcpServerListener : public IServerListenerT<ITcpServer>
{
public:

};

/************************************************************************
名称：PUSH 模型服务端 Socket 监听器抽象基类
描述：定义某些事件的默认处理方法（忽略事件）
************************************************************************/
class CTcpServerListener : public ITcpServerListener
{
public:
	virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen)							override {return HR_IGNORE;}
	virtual EnHandleResult OnAccept(ITcpServer* pSender, CONNID dwConnID, UINT_PTR soClient)				override {return HR_IGNORE;}
	virtual EnHandleResult OnHandShake(ITcpServer* pSender, CONNID dwConnID)								override {return HR_IGNORE;}
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength)						override {return HR_IGNORE;}
	virtual EnHandleResult OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)		override {return HR_IGNORE;}
	virtual EnHandleResult OnShutdown(ITcpServer* pSender)													override {return HR_IGNORE;}
};

/************************************************************************
名称：PULL 模型服务端 Socket 监听器抽象基类
描述：定义某些事件的默认处理方法（忽略事件）
************************************************************************/
class CTcpPullServerListener : public CTcpServerListener
{
public:
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength)						override = 0;
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)	override {return HR_IGNORE;}
};

#ifdef _UDP_SUPPORT

/************************************************************************
名称：UDP 服务端 Socket 监听器接口
描述：定义 UDP 服务端 Socket 监听器的所有事件
************************************************************************/
class IUdpServerListener : public IServerListenerT<IUdpServer>
{
public:

};

/************************************************************************
名称：UDP 服务端 Socket 监听器抽象基类
描述：定义某些事件的默认处理方法（忽略事件）
************************************************************************/
class CUdpServerListener : public IUdpServerListener
{
public:
	virtual EnHandleResult OnPrepareListen(IUdpServer* pSender, SOCKET soListen)						override {return HR_IGNORE;}
	virtual EnHandleResult OnAccept(IUdpServer* pSender, CONNID dwConnID, UINT_PTR pSockAddr)			override {return HR_IGNORE;}
	virtual EnHandleResult OnHandShake(IUdpServer* pSender, CONNID dwConnID)							override {return HR_IGNORE;}
	virtual EnHandleResult OnReceive(IUdpServer* pSender, CONNID dwConnID, int iLength)					override {return HR_IGNORE;}
	virtual EnHandleResult OnSend(IUdpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)	override {return HR_IGNORE;}
	virtual EnHandleResult OnShutdown(IUdpServer* pSender)												override {return HR_IGNORE;}
};

#endif

/************************************************************************
名称：通信代理 Socket 监听器接口
描述：定义 通信代理 Socket 监听器的所有事件
************************************************************************/
template<class T> class IAgentListenerT : public IComplexSocketListenerT<T>
{
public:

	/*
	* 名称：准备连接通知
	* 描述：通信客户端组件启动时，在客户端 Socket 创建完成并开始执行连接前，Socket 监听
	*		器将收到该通知，监听器可以在通知处理方法中执行 Socket 选项设置等额外工作
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	*			socket		-- 客户端 Socket
	* 返回值：	HR_OK / HR_IGNORE	-- 继续执行
	*			HR_ERROR			-- 终止启动通信客户端组件
	*/
	virtual EnHandleResult OnPrepareConnect(T* pSender, CONNID dwConnID, SOCKET socket)		= 0;

	/*
	* 名称：连接完成通知
	* 描述：与服务端成功建立连接时，Socket 监听器将收到该通知
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	* 返回值：	HR_OK / HR_IGNORE	-- 继续执行
	*			HR_ERROR			-- 同步连接：终止启动通信客户端组件
	*								   异步连接：关闭连接
	*/
	virtual EnHandleResult OnConnect(T* pSender, CONNID dwConnID)							= 0;
};

/************************************************************************
名称：TCP 通信代理 Socket 监听器接口
描述：定义 TCP 通信代理 Socket 监听器的所有事件
************************************************************************/
class ITcpAgentListener : public IAgentListenerT<ITcpAgent>
{
public:

};

/************************************************************************
名称：PUSH 模型通信代理 Socket 监听器抽象基类
描述：定义某些事件的默认处理方法（忽略事件）
************************************************************************/
class CTcpAgentListener : public ITcpAgentListener
{
public:
	virtual EnHandleResult OnPrepareConnect(ITcpAgent* pSender, CONNID dwConnID, SOCKET socket)				override {return HR_IGNORE;}
	virtual EnHandleResult OnConnect(ITcpAgent* pSender, CONNID dwConnID)									override {return HR_IGNORE;}
	virtual EnHandleResult OnHandShake(ITcpAgent* pSender, CONNID dwConnID)									override {return HR_IGNORE;}
	virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, int iLength)						override {return HR_IGNORE;}
	virtual EnHandleResult OnSend(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)		override {return HR_IGNORE;}
	virtual EnHandleResult OnShutdown(ITcpAgent* pSender)													override {return HR_IGNORE;}
};

/************************************************************************
名称：PULL 通信代理 Socket 监听器抽象基类
描述：定义某些事件的默认处理方法（忽略事件）
************************************************************************/
class CTcpPullAgentListener : public CTcpAgentListener
{
public:
	virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, int iLength)						override = 0;
	virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)	override {return HR_IGNORE;}
};

/************************************************************************
名称：客户端 Socket 监听器接口
描述：定义客户端 Socket 监听器的所有事件
************************************************************************/

template<class T> class IClientListenerT : public ISocketListenerT<T>
{
public:
	
	/*
	* 名称：准备连接通知
	* 描述：通信客户端组件启动时，在客户端 Socket 创建完成并开始执行连接前，Socket 监听
	*		器将收到该通知，监听器可以在通知处理方法中执行 Socket 选项设置等额外工作
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	*			socket		-- 客户端 Socket
	* 返回值：	HR_OK / HR_IGNORE	-- 继续执行
	*			HR_ERROR			-- 终止启动通信客户端组件
	*/
	virtual EnHandleResult OnPrepareConnect(T* pSender, CONNID dwConnID, SOCKET socket)						= 0;

	/*
	* 名称：连接完成通知
	* 描述：与服务端成功建立连接时，Socket 监听器将收到该通知
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	* 返回值：	HR_OK / HR_IGNORE	-- 继续执行
	*			HR_ERROR			-- 同步连接：终止启动通信客户端组件
	*								   异步连接：关闭连接
	*/
	virtual EnHandleResult OnConnect(T* pSender, CONNID dwConnID)											= 0;
};

/************************************************************************
名称：TCP 客户端 Socket 监听器接口
描述：定义 TCP 客户端 Socket 监听器的所有事件
************************************************************************/
class ITcpClientListener : public IClientListenerT<ITcpClient>
{
public:

};

/************************************************************************
名称：PUSH 模型客户端 Socket 监听器抽象基类
描述：定义某些事件的默认处理方法（忽略事件）
************************************************************************/
class CTcpClientListener : public ITcpClientListener
{
public:
	virtual EnHandleResult OnPrepareConnect(ITcpClient* pSender, CONNID dwConnID, SOCKET socket)			override {return HR_IGNORE;}
	virtual EnHandleResult OnConnect(ITcpClient* pSender, CONNID dwConnID)									override {return HR_IGNORE;}
	virtual EnHandleResult OnHandShake(ITcpClient* pSender, CONNID dwConnID)								override {return HR_IGNORE;}
	virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, int iLength)						override {return HR_IGNORE;}
	virtual EnHandleResult OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)		override {return HR_IGNORE;}
};

/************************************************************************
名称：PULL 客户端 Socket 监听器抽象基类
描述：定义某些事件的默认处理方法（忽略事件）
************************************************************************/
class CTcpPullClientListener : public CTcpClientListener
{
public:
	virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, int iLength)						= 0;
	virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)	{return HR_IGNORE;}
};

#ifdef _UDP_SUPPORT

/************************************************************************
名称：UDP 客户端 Socket 监听器接口
描述：定义 UDP 客户端 Socket 监听器的所有事件
************************************************************************/
class IUdpClientListener : public IClientListenerT<IUdpClient>
{
public:

};

/************************************************************************
名称：UDP 户端 Socket 监听器抽象基类
描述：定义某些事件的默认处理方法（忽略事件）
************************************************************************/
class CUdpClientListener : public IUdpClientListener
{
public:
	virtual EnHandleResult OnPrepareConnect(IUdpClient* pSender, CONNID dwConnID, SOCKET socket)			override {return HR_IGNORE;}
	virtual EnHandleResult OnConnect(IUdpClient* pSender, CONNID dwConnID)									override {return HR_IGNORE;}
	virtual EnHandleResult OnHandShake(IUdpClient* pSender, CONNID dwConnID)								override {return HR_IGNORE;}
	virtual EnHandleResult OnReceive(IUdpClient* pSender, CONNID dwConnID, int iLength)						override {return HR_IGNORE;}
	virtual EnHandleResult OnSend(IUdpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)		override {return HR_IGNORE;}
};

/************************************************************************
名称：UDP 传播 Socket 监听器接口
描述：定义 UDP 传播 Socket 监听器的所有事件
************************************************************************/
class IUdpCastListener : public IClientListenerT<IUdpCast>
{
public:

};

/************************************************************************
名称：UDP 传播 Socket 监听器抽象基类
描述：定义某些事件的默认处理方法（忽略事件）
************************************************************************/
class CUdpCastListener : public IUdpCastListener
{
public:
	virtual EnHandleResult OnPrepareConnect(IUdpCast* pSender, CONNID dwConnID, SOCKET socket)				override {return HR_IGNORE;}
	virtual EnHandleResult OnConnect(IUdpCast* pSender, CONNID dwConnID)									override {return HR_IGNORE;}
	virtual EnHandleResult OnHandShake(IUdpCast* pSender, CONNID dwConnID)									override {return HR_IGNORE;}
	virtual EnHandleResult OnReceive(IUdpCast* pSender, CONNID dwConnID, int iLength)						override {return HR_IGNORE;}
	virtual EnHandleResult OnSend(IUdpCast* pSender, CONNID dwConnID, const BYTE* pData, int iLength)		override {return HR_IGNORE;}
};

/************************************************************************
名称：UDP 节点 Socket 监听器接口
描述：定义 UDP 节点 Socket 监听器的所有事件
************************************************************************/
class IUdpNodeListener
{
public:

	/*
	* 名称：准备监听通知
	* 描述：通信组件启动时，在监听 Socket 创建完成并开始执行监听前，Socket 监听器
	*		将收到该通知，监听器可以在通知处理方法中执行 Socket 选项设置等额外工作
	*		
	* 参数：		pSender		-- 事件源对象
	*			soListen	-- 监听 Socket
	* 返回值：	HR_OK / HR_IGNORE	-- 继续执行
	*			HR_ERROR			-- 终止启动通信服务组件
	*/
	virtual EnHandleResult OnPrepareListen(IUdpNode* pSender, SOCKET soListen)															= 0;

	/*
	* 名称：已发送数据通知
	* 描述：成功发送数据后，Socket 监听器将收到该通知
	*		
	* 参数：		pSender				-- 事件源对象
	*			lpszRemoteAddress	-- 远程地址
	*			usRemotePort		-- 远程端口
	*			pData				-- 已发送数据缓冲区
	*			iLength				-- 已发送数据长度
	* 返回值：	忽略返回值
	*/
	virtual EnHandleResult OnSend(IUdpNode* pSender, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pData, int iLength)	= 0;

	/*
	* 名称：数据到达通知（PUSH 模型）
	* 描述：成功接收数据后，Socket 监听器将收到该通知
	*		
	* 参数：		pSender				-- 事件源对象
	*			lpszRemoteAddress	-- 远程地址
	*			usRemotePort		-- 远程端口
	*			pData				-- 已发送数据缓冲区
	*			iLength				-- 已发送数据长度
	* 返回值：	忽略返回值
	*/
	virtual EnHandleResult OnReceive(IUdpNode* pSender, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pData, int iLength)	= 0;

	/*
	* 名称：通信错误通知
	* 描述：通信发生错误后，Socket 监听器将收到该通知
	*		
	* 参数：		pSender				-- 事件源对象
	*			lpszRemoteAddress	-- 远程地址
	*			usRemotePort		-- 远程端口
	*			enOperation			-- Socket 操作类型
	*			iErrorCode			-- 错误代码
	*			pData				-- 本次事件关联的数据缓冲区
	*			iLength				-- 本次事件关联的数据长度
	* 返回值：	忽略返回值
	*/
	virtual EnHandleResult OnError(IUdpNode* pSender, EnSocketOperation enOperation, int iErrorCode, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pData, int iLength)	= 0;

	/*
	* 名称：关闭通信组件通知
	* 描述：通信组件关闭时，Socket 监听器将收到该通知
	*		
	* 参数：		pSender		-- 事件源对象
	* 返回值：	忽略返回值
	*/
	virtual EnHandleResult OnShutdown(IUdpNode* pSender)																				= 0;

public:
	virtual ~IUdpNodeListener() = default;
};

/************************************************************************
名称：UDP 节点 Socket 监听器抽象基类
描述：定义某些事件的默认处理方法（忽略事件）
************************************************************************/
class CUdpNodeListener : public IUdpNodeListener
{
public:
	virtual EnHandleResult OnPrepareListen(IUdpNode* pSender, SOCKET soListen)															override {return HR_IGNORE;}
	virtual EnHandleResult OnSend(IUdpNode* pSender, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pData, int iLength)	override {return HR_IGNORE;}
	virtual EnHandleResult OnShutdown(IUdpNode* pSender)																				override {return HR_IGNORE;}
};

#endif

/*****************************************************************************************************************************************************/
/****************************************************************** HTTP Interfaces ******************************************************************/
/*****************************************************************************************************************************************************/

#ifdef _HTTP_SUPPORT

/************************************************************************
名称：复合 Http 组件接口
描述：定义复合 Http 组件的所有操作方法和属性访问方法，复合 Http 组件同时管理多个 Http 连接
************************************************************************/
class IComplexHttp
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

	/*
	* 名称：启动 HTTP 通信
	* 描述：当通信组件设置为非自动启动 HTTP 通信时，需要调用本方法启动 HTTP 通信
	*		
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual BOOL StartHttp(CONNID dwConnID)												= 0;

	/*
	* 名称：发送 Chunked 数据分片
	* 描述：向对端发送 Chunked 数据分片
	*		
	* 参数：		dwConnID		-- 连接 ID
	*			pData			-- Chunked 数据分片
	*			iLength			-- 数据分片长度（为 0 表示结束分片）
	*			lpszExtensions	-- 扩展属性（默认：nullptr）
	* 返回值：	TRUE			-- 成功
	*			FALSE			-- 失败
	*/
	virtual BOOL SendChunkData(CONNID dwConnID, const BYTE* pData = nullptr, int iLength = 0, LPCSTR lpszExtensions = nullptr)	= 0;

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 设置本地协议版本 */
	virtual void SetLocalVersion(EnHttpVersion usVersion)								= 0;
	/* 获取本地协议版本 */
	virtual EnHttpVersion GetLocalVersion()												= 0;

	/* 检查是否升级协议 */
	virtual BOOL IsUpgrade(CONNID dwConnID)												= 0;
	/* 检查是否有 Keep-Alive 标识 */
	virtual BOOL IsKeepAlive(CONNID dwConnID)											= 0;
	/* 获取协议版本 */
	virtual USHORT GetVersion(CONNID dwConnID)											= 0;
	/* 获取内容长度 */
	virtual ULONGLONG GetContentLength(CONNID dwConnID)									= 0;
	/* 获取内容类型 */
	virtual LPCSTR GetContentType(CONNID dwConnID)										= 0;
	/* 获取内容编码 */
	virtual LPCSTR GetContentEncoding(CONNID dwConnID)									= 0;
	/* 获取传输编码 */
	virtual LPCSTR GetTransferEncoding(CONNID dwConnID)									= 0;
	/* 获取协议升级类型 */
	virtual EnHttpUpgradeType GetUpgradeType(CONNID dwConnID)							= 0;
	/* 获取解析错误代码 */
	virtual USHORT GetParseErrorCode(CONNID dwConnID, LPCSTR* lpszErrorDesc = nullptr)	= 0;

	/* 获取某个请求头（单值） */
	virtual BOOL GetHeader(CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue)						= 0;
	/* 获取某个请求头（多值） */
	virtual BOOL GetHeaders(CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue[], DWORD& dwCount)	= 0;
	/* 获取所有请求头 */
	virtual BOOL GetAllHeaders(CONNID dwConnID, THeader lpHeaders[], DWORD& dwCount)				= 0;
	/* 获取所有请求头名称 */
	virtual BOOL GetAllHeaderNames(CONNID dwConnID, LPCSTR lpszName[], DWORD& dwCount)				= 0;

	/* 获取 Cookie */
	virtual BOOL GetCookie(CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue)						= 0;
	/* 获取所有 Cookie */
	virtual BOOL GetAllCookies(CONNID dwConnID, TCookie lpCookies[], DWORD& dwCount)				= 0;

	/*
	// !! maybe implemented in future !! //

	virtual BOOL GetParam(CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue)						= 0;
	virtual BOOL GetParams(CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue[], DWORD& dwCount)	= 0;
	virtual BOOL GetAllParams(CONNID dwConnID, LPPARAM lpszParam[], DWORD& dwCount)					= 0;
	virtual BOOL GetAllParamNames(CONNID dwConnID, LPCSTR lpszName[], DWORD& dwCount)				= 0;
	*/

	/* 获取当前 WebSocket 消息状态，传入 nullptr 则不获取相应字段 */
	virtual BOOL GetWSMessageState(CONNID dwConnID, BOOL* lpbFinal, BYTE* lpiReserved, BYTE* lpiOperationCode, LPCBYTE* lpszMask, ULONGLONG* lpullBodyLen, ULONGLONG* lpullBodyRemain)	= 0;

	/* 设置 HTTP 启动方式（默认：TRUE，自动启动） */
	virtual void SetHttpAutoStart(BOOL bAutoStart)													= 0;
	/* 获取 HTTP 启动方式 */
	virtual BOOL IsHttpAutoStart()																	= 0;

public:
	virtual ~IComplexHttp() = default;
};

/************************************************************************
名称：复合 Http 请求者组件接口
描述：定义复合 Http 请求者组件的所有操作方法和属性访问方法
************************************************************************/
class IComplexHttpRequester : public IComplexHttp
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

	/*
	* 名称：发送 WebSocket 消息
	* 描述：向对端端发送 WebSocket 消息
	*		
	* 参数：		dwConnID		-- 连接 ID
	*			bFinal			-- 是否结束帧
	*			iReserved		-- RSV1/RSV2/RSV3 各 1 位
	*			iOperationCode	-- 操作码：0x0 - 0xF
	*			lpszMask		-- 掩码（nullptr 或 4 字节掩码，如果为 nullptr 则没有掩码）
	*			pData			-- 消息体数据缓冲区
	*			iLength			-- 消息体数据长度
	*			ullBodyLen		-- 消息总长度
	* 								ullBodyLen = 0		 -> 消息总长度为 iLength
	* 								ullBodyLen = iLength -> 消息总长度为 ullBodyLen
	* 								ullBodyLen > iLength -> 消息总长度为 ullBodyLen，后续消息体长度为 ullBOdyLen - iLength，后续消息体通过底层方法 Send() / SendPackets() 发送
	* 								ullBodyLen < iLength -> 错误参数，发送失败
	* 返回值：	TRUE			-- 成功
	*			FALSE			-- 失败
	*/
	virtual BOOL SendWSMessage(CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], const BYTE* pData = nullptr, int iLength = 0, ULONGLONG ullBodyLen = 0)	= 0;

	/*
	* 名称：发送请求
	* 描述：向服务端发送 HTTP 请求
	*		
	* 参数：		dwConnID		-- 连接 ID
	*			lpszMethod		-- 请求方法
	*			lpszPath		-- 请求路径
	*			lpHeaders		-- 请求头
	*			iHeaderCount	-- 请求头数量
	*			pBody			-- 请求体
	*			iLength			-- 请求体长度
	* 返回值：	TRUE			-- 成功
	*			FALSE			-- 失败
	*/
	virtual BOOL SendRequest(CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0, const BYTE* pBody = nullptr, int iLength = 0)	= 0;

	/*
	* 名称：发送本地文件
	* 描述：向指定连接发送 4096 KB 以下的小文件
	*		
	* 参数：		dwConnID		-- 连接 ID
	*			lpszFileName	-- 文件路径
	*			lpszMethod		-- 请求方法
	*			lpszPath		-- 请求路径
	*			lpHeaders		-- 请求头
	*			iHeaderCount	-- 请求头数量
	* 返回值：	TRUE			-- 成功
	*			FALSE			-- 失败
	*/
	virtual BOOL SendLocalFile(CONNID dwConnID, LPCSTR lpszFileName, LPCSTR lpszMethod, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)							= 0;

	/* 发送 POST 请求 */
	virtual BOOL SendPost(CONNID dwConnID, LPCSTR lpszPath, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)													= 0;
	/* 发送 PUT 请求 */
	virtual BOOL SendPut(CONNID dwConnID, LPCSTR lpszPath, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)														= 0;
	/* 发送 PATCH 请求 */
	virtual BOOL SendPatch(CONNID dwConnID, LPCSTR lpszPath, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)													= 0;
	/* 发送 GET 请求 */
	virtual BOOL SendGet(CONNID dwConnID, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)																		= 0;
	/* 发送 DELETE 请求 */
	virtual BOOL SendDelete(CONNID dwConnID, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)																	= 0;
	/* 发送 HEAD 请求 */
	virtual BOOL SendHead(CONNID dwConnID, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)																		= 0;
	/* 发送 TRACE 请求 */
	virtual BOOL SendTrace(CONNID dwConnID, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)																		= 0;
	/* 发送 OPTIONS 请求 */
	virtual BOOL SendOptions(CONNID dwConnID, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)																	= 0;
	/* 发送 CONNECT 请求 */
	virtual BOOL SendConnect(CONNID dwConnID, LPCSTR lpszHost, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)																	= 0;

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 获取 HTTP 状态码 */
	virtual USHORT GetStatusCode(CONNID dwConnID)						= 0;

	/* 设置是否使用 Cookie（默认：TRUE） */
	virtual void SetUseCookie(BOOL bUseCookie)							= 0;
	/* 检查是否使用 Cookie */
	virtual BOOL IsUseCookie()											= 0;
};

/************************************************************************
名称：复合 Http 响应者组件接口
描述：定义复合 Http 响应者组件的所有操作方法和属性访问方法
************************************************************************/
class IComplexHttpResponder : public IComplexHttp
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

	/*
	* 名称：发送 WebSocket 消息
	* 描述：向对端端发送 WebSocket 消息
	*		
	* 参数：		dwConnID		-- 连接 ID
	*			bFinal			-- 是否结束帧
	*			iReserved		-- RSV1/RSV2/RSV3 各 1 位
	*			iOperationCode	-- 操作码：0x0 - 0xF
	*			pData			-- 消息体数据缓冲区
	*			iLength			-- 消息体数据长度
	*			ullBodyLen		-- 消息总长度
	* 								ullBodyLen = 0		 -> 消息总长度为 iLength
	* 								ullBodyLen = iLength -> 消息总长度为 ullBodyLen
	* 								ullBodyLen > iLength -> 消息总长度为 ullBodyLen，后续消息体长度为 ullBOdyLen - iLength，后续消息体通过底层方法 Send() / SendPackets() 发送
	* 								ullBodyLen < iLength -> 错误参数，发送失败
	* 返回值：	TRUE			-- 成功
	*			FALSE			-- 失败
	*/
	virtual BOOL SendWSMessage(CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE* pData, int iLength = 0, ULONGLONG ullBodyLen = 0)	= 0;

	/*
	* 名称：回复请求
	* 描述：向客户端回复 HTTP 请求
	*		
	* 参数：		dwConnID		-- 连接 ID
	*			usStatusCode	-- HTTP 状态码
	*			lpszDesc		-- HTTP 状态描述
	*			lpHeaders		-- 回复请求头
	*			iHeaderCount	-- 回复请求头数量
	*			pData			-- 回复请求体
	*			iLength			-- 回复请求体长度
	* 返回值：	TRUE			-- 成功
	*			FALSE			-- 失败
	*/
	virtual BOOL SendResponse(CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc = nullptr, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0, const BYTE* pData = nullptr, int iLength = 0)	= 0;

	/*
	* 名称：发送本地文件
	* 描述：向指定连接发送 4096 KB 以下的小文件
	*		
	* 参数：		dwConnID		-- 连接 ID
	*			lpszFileName	-- 文件路径
	*			usStatusCode	-- HTTP 状态码
	*			lpszDesc		-- HTTP 状态描述
	*			lpHeaders		-- 回复请求头
	*			iHeaderCount	-- 回复请求头数量
	* 返回值：	TRUE			-- 成功
	*			FALSE			-- 失败
	*/
	virtual BOOL SendLocalFile(CONNID dwConnID, LPCSTR lpszFileName, USHORT usStatusCode = HSC_OK, LPCSTR lpszDesc = nullptr, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)				= 0;

	/*
	* 名称：释放连接
	* 描述：把连接放入释放队列，等待某个时间（通过 SetReleaseDelay() 设置）关闭连接
	*		
	* 参数：		dwConnID		-- 连接 ID
	* 返回值：	TRUE			-- 成功
	*			FALSE			-- 失败
	*/
	virtual BOOL Release(CONNID dwConnID)								= 0;

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 获取主机 */
	virtual LPCSTR GetHost(CONNID dwConnID)								= 0;

	/* 设置连接释放延时（默认：3000 毫秒） */
	virtual void SetReleaseDelay(DWORD dwReleaseDelay)					= 0;
	/* 获取连接释放延时 */
	virtual DWORD GetReleaseDelay()										= 0;

	/* 获取请求行 URL 域掩码（URL 域参考：EnHttpUrlField） */
	virtual USHORT GetUrlFieldSet(CONNID dwConnID)						= 0;
	/* 获取某个 URL 域值 */
	virtual LPCSTR GetUrlField(CONNID dwConnID, EnHttpUrlField enField)	= 0;
	/* 获取请求方法 */
	virtual LPCSTR GetMethod(CONNID dwConnID)							= 0;
};

/************************************************************************
名称：简单 HTTP 组件接口
描述：定义 简单 HTTP 组件的所有操作方法和属性访问方法
************************************************************************/
class IHttp
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

	/*
	* 名称：发送 WebSocket 消息
	* 描述：向对端端发送 WebSocket 消息
	*		
	* 参数：		bFinal			-- 是否结束帧
	*			iReserved		-- RSV1/RSV2/RSV3 各 1 位
	*			iOperationCode	-- 操作码：0x0 - 0xF
	*			lpszMask		-- 掩码（nullptr 或 4 字节掩码，如果为 nullptr 则没有掩码）
	*			pData			-- 消息体数据缓冲区
	*			iLength			-- 消息体数据长度
	*			ullBodyLen		-- 消息总长度
	* 								ullBodyLen = 0		 -> 消息总长度为 iLength
	* 								ullBodyLen = iLength -> 消息总长度为 ullBodyLen
	* 								ullBodyLen > iLength -> 消息总长度为 ullBodyLen，后续消息体长度为 ullBOdyLen - iLength，后续消息体通过底层方法 Send() / SendPackets() 发送
	* 								ullBodyLen < iLength -> 错误参数，发送失败
	* 返回值：	TRUE			-- 成功
	*			FALSE			-- 失败
	*/
	virtual BOOL SendWSMessage(BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], const BYTE* pData = nullptr, int iLength = 0, ULONGLONG ullBodyLen = 0)	= 0;

	/*
	* 名称：启动 HTTP 通信
	* 描述：当通信组件设置为非自动启动 HTTP 通信时，需要调用本方法启动 HTTP 通信
	*		
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
	*/
	virtual BOOL StartHttp()											= 0;

	/*
	* 名称：发送 Chunked 数据分片
	* 描述：向对端发送 Chunked 数据分片
	*		
	* 参数：		pData			-- Chunked 数据分片
	*			iLength			-- 数据分片长度（为 0 表示结束分片）
	*			lpszExtensions	-- 扩展属性（默认：nullptr）
	* 返回值：	TRUE			-- 成功
	*			FALSE			-- 失败
	*/
	virtual BOOL SendChunkData(const BYTE* pData = nullptr, int iLength = 0, LPCSTR lpszExtensions = nullptr)	= 0;

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 设置本地协议版本 */
	virtual void SetLocalVersion(EnHttpVersion usVersion)				= 0;
	/* 获取本地协议版本 */
	virtual EnHttpVersion GetLocalVersion()								= 0;

	/* 检查是否升级协议 */
	virtual BOOL IsUpgrade()											= 0;
	/* 检查是否有 Keep-Alive 标识 */
	virtual BOOL IsKeepAlive()											= 0;
	/* 获取协议版本 */
	virtual USHORT GetVersion()											= 0;
	/* 获取内容长度 */
	virtual ULONGLONG GetContentLength()								= 0;
	/* 获取内容类型 */
	virtual LPCSTR GetContentType()										= 0;
	/* 获取内容编码 */
	virtual LPCSTR GetContentEncoding()									= 0;
	/* 获取传输编码 */
	virtual LPCSTR GetTransferEncoding()								= 0;
	/* 获取协议升级类型 */
	virtual EnHttpUpgradeType GetUpgradeType()							= 0;
	/* 获取解析错误代码 */
	virtual USHORT GetParseErrorCode(LPCSTR* lpszErrorDesc = nullptr)	= 0;

	/* 获取 HTTP 状态码 */
	virtual USHORT GetStatusCode()										= 0;

	/* 获取某个请求头（单值） */
	virtual BOOL GetHeader(LPCSTR lpszName, LPCSTR* lpszValue)						= 0;
	/* 获取某个请求头（多值） */
	virtual BOOL GetHeaders(LPCSTR lpszName, LPCSTR lpszValue[], DWORD& dwCount)	= 0;
	/* 获取所有请求头 */
	virtual BOOL GetAllHeaders(THeader lpHeaders[], DWORD& dwCount)					= 0;
	/* 获取所有请求头名称 */
	virtual BOOL GetAllHeaderNames(LPCSTR lpszName[], DWORD& dwCount)				= 0;

	/* 获取 Cookie */
	virtual BOOL GetCookie(LPCSTR lpszName, LPCSTR* lpszValue)						= 0;
	/* 获取所有 Cookie */
	virtual BOOL GetAllCookies(TCookie lpCookies[], DWORD& dwCount)					= 0;

	/*
	// !! maybe implemented in future !! //

	virtual BOOL GetParam(LPCSTR lpszName, LPCSTR* lpszValue)						= 0;
	virtual BOOL GetParams(LPCSTR lpszName, LPCSTR lpszValue[], DWORD& dwCount)		= 0;
	virtual BOOL GetAllParams(LPPARAM lpszParam[], DWORD& dwCount)					= 0;
	virtual BOOL GetAllParamNames(LPCSTR lpszName[], DWORD& dwCount)				= 0;
	*/

	/* 获取当前 WebSocket 消息状态，传入 nullptr 则不获取相应字段 */
	virtual BOOL GetWSMessageState(BOOL* lpbFinal, BYTE* lpiReserved, BYTE* lpiOperationCode, LPCBYTE* lpszMask, ULONGLONG* lpullBodyLen, ULONGLONG* lpullBodyRemain)	= 0;

	/* 设置 HTTP 启动方式（默认：TRUE，自动启动） */
	virtual void SetHttpAutoStart(BOOL bAutoStart)									= 0;
	/* 获取 HTTP 启动方式 */
	virtual BOOL IsHttpAutoStart()													= 0;

public:
	virtual ~IHttp() = default;
};

/************************************************************************
名称：简单 Http 请求者组件接口
描述：定义简单 Http 请求者组件的所有操作方法和属性访问方法
************************************************************************/
class IHttpRequester : public IHttp
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

	/*
	* 名称：发送请求
	* 描述：向服务端发送 HTTP 请求
	*		
	* 参数：		lpszMethod		-- 请求方法
	*			lpszPath		-- 请求路径
	*			lpHeaders		-- 请求头
	*			iHeaderCount	-- 请求头数量
	*			pBody			-- 请求体
	*			iLength			-- 请求体长度
	* 返回值：	TRUE			-- 成功
	*			FALSE			-- 失败
	*/
	virtual BOOL SendRequest(LPCSTR lpszMethod, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0, const BYTE* pBody = nullptr, int iLength = 0)	= 0;

	/*
	* 名称：发送本地文件
	* 描述：向指定连接发送 4096 KB 以下的小文件
	*		
	* 参数：		dwConnID		-- 连接 ID
	*			lpszFileName	-- 文件路径
	*			lpszMethod		-- 请求方法
	*			lpszPath		-- 请求路径
	*			lpHeaders		-- 请求头
	*			iHeaderCount	-- 请求头数量
	* 返回值：	TRUE			-- 成功
	*			FALSE			-- 失败
	*/
	virtual BOOL SendLocalFile(LPCSTR lpszFileName, LPCSTR lpszMethod, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)							= 0;

	/* 发送 POST 请求 */
	virtual BOOL SendPost(LPCSTR lpszPath, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)														= 0;
	/* 发送 PUT 请求 */
	virtual BOOL SendPut(LPCSTR lpszPath, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)														= 0;
	/* 发送 PATCH 请求 */
	virtual BOOL SendPatch(LPCSTR lpszPath, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)													= 0;
	/* 发送 GET 请求 */
	virtual BOOL SendGet(LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)																		= 0;
	/* 发送 DELETE 请求 */
	virtual BOOL SendDelete(LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)																		= 0;
	/* 发送 HEAD 请求 */
	virtual BOOL SendHead(LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)																		= 0;
	/* 发送 TRACE 请求 */
	virtual BOOL SendTrace(LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)																		= 0;
	/* 发送 OPTIONS 请求 */
	virtual BOOL SendOptions(LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)																	= 0;
	/* 发送 CONNECT 请求 */
	virtual BOOL SendConnect(LPCSTR lpszHost, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)																	= 0;

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 设置是否使用 Cookie（默认：TRUE） */
	virtual void SetUseCookie(BOOL bUseCookie)								= 0;
	/* 检查是否使用 Cookie */
	virtual BOOL IsUseCookie()												= 0;
};

/************************************************************************
名称：简单 Http 同步请求者组件接口
描述：定义简单 Http 同步请求者组件的所有操作方法和属性访问方法
************************************************************************/
class IHttpSyncRequester : public IHttpRequester
{
public:

	/*
	* 名称：发送 URL 请求
	* 描述：向服务端发送 HTTP URL 请求
	*		
	* 参数：		lpszMethod		-- 请求方法
	*			lpszUrl			-- 请求 URL
	*			lpHeaders		-- 请求头
	*			iHeaderCount	-- 请求头数量
	*			pBody			-- 请求体
	*			iLength			-- 请求体长度
	*			bForceReconnect	-- 强制重新连接（默认：FALSE，当请求 URL 的主机和端口与现有连接一致时，重用现有连接）
	* 返回值：	TRUE			-- 成功
	*			FALSE			-- 失败
	*/
	virtual BOOL OpenUrl(LPCSTR lpszMethod, LPCSTR lpszUrl, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0, const BYTE* pBody = nullptr, int iLength = 0, BOOL bForceReconnect = FALSE)	= 0;

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

	/*
	* 名称：清除请求结果
	* 描述：清除上一次请求的响应头和响应体等结果信息（该方法会在每次发送请求前自动调用）
	*
	* 参数：		
	* 返回值：	TRUE			-- 成功
	*			FALSE			-- 失败
	*/
	virtual BOOL CleanupRequestResult	()									= 0;

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 设置连接超时（毫秒，0：系统默认超时，默认：5000） */
	virtual void SetConnectTimeout		(DWORD dwConnectTimeout)			= 0;
	/* 设置请求超时（毫秒，0：无限等待，默认：10000） */
	virtual void SetRequestTimeout		(DWORD dwRequestTimeout)			= 0;

	/* 获取连接超时 */
	virtual DWORD GetConnectTimeout		()									= 0;
	/* 获取请求超时 */
	virtual DWORD GetRequestTimeout		()									= 0;

	/* 获取响应体 */
	virtual BOOL GetResponseBody		(LPCBYTE* lpszBody, int* iLength)	= 0;
};


/************************************************************************
名称：HTTP 组件接口
描述：继承了 HTTP 和 Socket 接口
************************************************************************/
typedef DualInterface<IComplexHttpResponder, ITcpServer>	IHttpServer;
typedef DualInterface<IComplexHttpRequester, ITcpAgent>		IHttpAgent;
typedef DualInterface<IHttpRequester, ITcpClient>			IHttpClient;
typedef DualInterface<IHttpSyncRequester, ITcpClient>		IHttpSyncClient;

/************************************************************************
名称：IComplexHttp 组件监听器基接口
描述：定义 IComplexHttp 组件监听器的所有事件
************************************************************************/
template<class T> class IHttpListenerT
{
public:

	/*
	* 名称：开始解析通知
	* 描述：开始解析 HTTP 报文时，向监听器发送该通知
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	* 返回值：	HPR_OK		-- 继续执行
	*			HPR_ERROR	-- 引发 OnParserError() 和 OnClose() 事件并关闭连接
	*/
	virtual EnHttpParseResult OnMessageBegin(T* pSender, CONNID dwConnID)										= 0;

	/*
	* 名称：请求行解析完成通知（仅用于 HTTP 服务端）
	* 描述：请求行解析完成后，向监听器发送该通知
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	*			lpszMethod	-- 请求方法名
	*			lpszUrl		-- 请求行中的 URL 域
	* 返回值：	HPR_OK		-- 继续执行
	*			HPR_ERROR	-- 引发 OnParserError() 和 OnClose() 事件并关闭连接
	*/
	virtual EnHttpParseResult OnRequestLine(T* pSender, CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)		= 0;

	/*
	* 名称：状态行解析完成通知（仅用于 HTTP 客户端）
	* 描述：状态行解析完成后，向监听器发送该通知
	*		
	* 参数：		pSender			-- 事件源对象
	*			dwConnID		-- 连接 ID
	*			usStatusCode	-- HTTP 状态码
	*			lpszDesc		-- 状态描述
	* 返回值：	HPR_OK			-- 继续执行
	*			HPR_ERROR		-- 引发 OnParserError() 和 OnClose() 事件并关闭连接
	*/
	virtual EnHttpParseResult OnStatusLine(T* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)	= 0;

	/*
	* 名称：请求头通知
	* 描述：每当解析完成一个请求头后，向监听器发送该通知
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	*			lpszName	-- 请求头名称
	*			lpszValue	-- 请求头值
	* 返回值：	HPR_OK		-- 继续执行
	*			HPR_ERROR	-- 引发 OnParserError() 和 OnClose() 事件并关闭连接
	*/
	virtual EnHttpParseResult OnHeader(T* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)			= 0;

	/*
	* 名称：请求头完成通知
	* 描述：解析完成所有请求头后，向监听器发送该通知
	*		
	* 参数：		pSender			-- 事件源对象
	*			dwConnID		-- 连接 ID
	* 返回值：	HPR_OK			-- 继续执行
	*			HPR_SKIP_BODY	-- 跳过当前请求的 HTTP BODY
	*			HPR_UPGRADE		-- 升级协议
	*			HPR_ERROR		-- 引发 OnParserError() 和 OnClose() 事件并关闭连接
	*/
	virtual EnHttpParseResult OnHeadersComplete(T* pSender, CONNID dwConnID)									= 0;

	/*
	* 名称：BODY 报文通知
	* 描述：每当接收到 HTTP BODY 报文，向监听器发送该通知
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	*			pData		-- 数据缓冲区
	*			iLength		-- 数据长度
	* 返回值：	HPR_OK		-- 继续执行
	*			HPR_ERROR	-- 引发 OnParserError() 和 OnClose() 事件并关闭连接
	*/
	virtual EnHttpParseResult OnBody(T* pSender, CONNID dwConnID, const BYTE* pData, int iLength)				= 0;

	/*
	* 名称：Chunked 报文头通知
	* 描述：每当解析出一个 Chunked 报文头，向监听器发送该通知
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	*			iLength		-- Chunked 报文体数据长度
	* 返回值：	HPR_OK		-- 继续执行
	*			HPR_ERROR	-- 引发 OnParserError() 和 OnClose() 事件并关闭连接
	*/
	virtual EnHttpParseResult OnChunkHeader(T* pSender, CONNID dwConnID, int iLength)							= 0;

	/*
	* 名称：Chunked 报文结束通知
	* 描述：每当解析完一个 Chunked 报文，向监听器发送该通知
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	* 返回值：	HPR_OK		-- 继续执行
	*			HPR_ERROR	-- 引发 OnParserError() 和 OnClose() 事件并关闭连接
	*/
	virtual EnHttpParseResult OnChunkComplete(T* pSender, CONNID dwConnID)										= 0;

	/*
	* 名称：完成解析通知
	* 描述：每当解析完成一个完整 HTTP 报文，向监听器发送该通知
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	* 返回值：	HPR_OK		-- 继续执行
	*			HPR_ERROR	-- 引发 OnParserError() 和 OnClose() 事件并关闭连接
	*/
	virtual EnHttpParseResult OnMessageComplete(T* pSender, CONNID dwConnID)									= 0;

	/*
	* 名称：升级协议通知
	* 描述：当需要升级协议时，向监听器发送该通知
	*		
	* 参数：		pSender			-- 事件源对象
	*			dwConnID		-- 连接 ID
	*			enUpgradeType	-- 协议类型
	* 返回值：	HPR_OK			-- 继续执行
	*			HPR_ERROR		-- 引发 OnClose() 事件并关闭连接
	*/
	virtual EnHttpParseResult OnUpgrade(T* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)			= 0;

	/*
	* 名称：解析错误通知
	* 描述：当解析 HTTP 报文错误时，向监听器发送该通知
	*		
	* 参数：		pSender			-- 事件源对象
	*			dwConnID		-- 连接 ID
	*			iErrorCode		-- 错误代码
	*			lpszErrorDesc	-- 错误描述
	* 返回值：	HPR_OK			-- 继续执行
	*			HPR_ERROR		-- 引发 OnClose() 事件并关闭连接
	*/
	virtual EnHttpParseResult OnParseError(T* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)	= 0;

	/*
	* 名称：WebSocket 数据包头通知
	* 描述：当解析 WebSocket 数据包头时，向监听器发送该通知
	*		
	* 参数：		pSender			-- 事件源对象
	*			dwConnID		-- 连接 ID
	*			bFinal			-- 是否结束帧
	*			iReserved		-- RSV1/RSV2/RSV3 各 1 位
	*			iOperationCode	-- 操作码：0x0 - 0xF
	*			lpszMask		-- 掩码（nullptr 或 4 字节掩码，如果为 nullptr 则没有掩码）
	*			ullBodyLen		-- 消息体长度
	* 返回值：	HR_OK / HR_IGNORE	-- 继续执行
	*			HR_ERROR			-- 引发 OnClose() 事件并关闭连接
	*/
	virtual EnHandleResult OnWSMessageHeader(T* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)	= 0;

	/*
	* 名称：WebSocket 数据包体通知
	* 描述：当接收到 WebSocket 数据包体时，向监听器发送该通知
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	*			pData		-- 消息体数据缓冲区
	*			iLength		-- 消息体数据长度
	* 返回值：	HR_OK / HR_IGNORE	-- 继续执行
	*			HR_ERROR			-- 引发 OnClose() 事件并关闭连接
	*/
	virtual EnHandleResult OnWSMessageBody(T* pSender, CONNID dwConnID, const BYTE* pData, int iLength)			= 0;

	/*
	* 名称：WebSocket 数据包完成通知
	* 描述：当完整接收一个 WebSocket 数据包时，向监听器发送该通知
	*		
	* 参数：		pSender		-- 事件源对象
	*			dwConnID	-- 连接 ID
	* 返回值：	HR_OK / HR_IGNORE	-- 继续执行
	*			HR_ERROR			-- 引发 OnClose() 事件并关闭连接
	*/
	virtual EnHandleResult OnWSMessageComplete(T* pSender, CONNID dwConnID)										= 0;

public:
	virtual ~IHttpListenerT() = default;
};

/************************************************************************
名称：IHttpServer 组件端监听器接口
描述：定义 IHttpServer 监听器的所有事件
************************************************************************/
class IHttpServerListener : public IHttpListenerT<IHttpServer>, public ITcpServerListener
{
public:

};

/************************************************************************
名称：IHttpAgent 组件端监听器接口
描述：定义 IHttpAgent 监听器的所有事件
************************************************************************/
class IHttpAgentListener : public IHttpListenerT<IHttpAgent>, public ITcpAgentListener
{
public:

};

/************************************************************************
名称：IHttpClient 组件端监听器接口
描述：定义 IHttpClient 监听器的所有事件
************************************************************************/
class IHttpClientListener : public IHttpListenerT<IHttpClient>, public ITcpClientListener
{
public:

};

/************************************************************************
名称：IHttpServerListener 监听器抽象基类
描述：定义某些事件的默认处理方法（忽略事件）
************************************************************************/
class CHttpServerListener : public IHttpServerListener
{
public:
	virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen)										override {return HR_IGNORE;}
	virtual EnHandleResult OnAccept(ITcpServer* pSender, CONNID dwConnID, UINT_PTR soClient)							override {return HR_IGNORE;}
	virtual EnHandleResult OnHandShake(ITcpServer* pSender, CONNID dwConnID)											override {return HR_IGNORE;}
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength)									override {return HR_IGNORE;}
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)				override {return HR_IGNORE;}
	virtual EnHandleResult OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)					override {return HR_IGNORE;}
	virtual EnHandleResult OnShutdown(ITcpServer* pSender)																override {return HR_IGNORE;}

	virtual EnHttpParseResult OnMessageBegin(IHttpServer* pSender, CONNID dwConnID)										override {return HPR_OK;}
	virtual EnHttpParseResult OnRequestLine(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)	override {return HPR_OK;}
	virtual EnHttpParseResult OnStatusLine(IHttpServer* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)	override {return HPR_OK;}
	virtual EnHttpParseResult OnHeader(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)		override {return HPR_OK;}
	virtual EnHttpParseResult OnChunkHeader(IHttpServer* pSender, CONNID dwConnID, int iLength)							override {return HPR_OK;}
	virtual EnHttpParseResult OnChunkComplete(IHttpServer* pSender, CONNID dwConnID)									override {return HPR_OK;}
	virtual EnHttpParseResult OnUpgrade(IHttpServer* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)			override {return HPR_OK;}

	virtual EnHandleResult OnWSMessageHeader(IHttpServer* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)	override {return HR_IGNORE;}
	virtual EnHandleResult OnWSMessageBody(IHttpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)		override {return HR_IGNORE;}
	virtual EnHandleResult OnWSMessageComplete(IHttpServer* pSender, CONNID dwConnID)									override {return HR_IGNORE;}
};

/************************************************************************
名称：IHttpAgentListener 监听器抽象基类
描述：定义某些事件的默认处理方法（忽略事件）
************************************************************************/
class CHttpAgentListener : public IHttpAgentListener
{
public:
	virtual EnHandleResult OnPrepareConnect(ITcpAgent* pSender, CONNID dwConnID, SOCKET socket)							override {return HR_IGNORE;}
	virtual EnHandleResult OnConnect(ITcpAgent* pSender, CONNID dwConnID)												override {return HR_IGNORE;}
	virtual EnHandleResult OnHandShake(ITcpAgent* pSender, CONNID dwConnID)												override {return HR_IGNORE;}
	virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, int iLength)									override {return HR_IGNORE;}
	virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)				override {return HR_IGNORE;}
	virtual EnHandleResult OnSend(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)					override {return HR_IGNORE;}
	virtual EnHandleResult OnShutdown(ITcpAgent* pSender)																override {return HR_IGNORE;}

	virtual EnHttpParseResult OnMessageBegin(IHttpAgent* pSender, CONNID dwConnID)										override {return HPR_OK;}
	virtual EnHttpParseResult OnRequestLine(IHttpAgent* pSender, CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)	override {return HPR_OK;}
	virtual EnHttpParseResult OnStatusLine(IHttpAgent* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)	override {return HPR_OK;}
	virtual EnHttpParseResult OnHeader(IHttpAgent* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)			override {return HPR_OK;}
	virtual EnHttpParseResult OnChunkHeader(IHttpAgent* pSender, CONNID dwConnID, int iLength)							override {return HPR_OK;}
	virtual EnHttpParseResult OnChunkComplete(IHttpAgent* pSender, CONNID dwConnID)										override {return HPR_OK;}
	virtual EnHttpParseResult OnUpgrade(IHttpAgent* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)			override {return HPR_OK;}

	virtual EnHandleResult OnWSMessageHeader(IHttpAgent* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)	override {return HR_IGNORE;}
	virtual EnHandleResult OnWSMessageBody(IHttpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)		override {return HR_IGNORE;}
	virtual EnHandleResult OnWSMessageComplete(IHttpAgent* pSender, CONNID dwConnID)									override {return HR_IGNORE;}
};

/************************************************************************
名称：IHttpClientListener 监听器抽象基类
描述：定义某些事件的默认处理方法（忽略事件）
************************************************************************/

class CHttpClientListener : public IHttpClientListener
{
public:
	virtual EnHandleResult OnPrepareConnect(ITcpClient* pSender, CONNID dwConnID, SOCKET socket)						override {return HR_IGNORE;}
	virtual EnHandleResult OnConnect(ITcpClient* pSender, CONNID dwConnID)												override {return HR_IGNORE;}
	virtual EnHandleResult OnHandShake(ITcpClient* pSender, CONNID dwConnID)											override {return HR_IGNORE;}
	virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, int iLength)									override {return HR_IGNORE;}
	virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)				override {return HR_IGNORE;}
	virtual EnHandleResult OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)					override {return HR_IGNORE;}

	virtual EnHttpParseResult OnMessageBegin(IHttpClient* pSender, CONNID dwConnID)										override {return HPR_OK;}
	virtual EnHttpParseResult OnRequestLine(IHttpClient* pSender, CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)	override {return HPR_OK;}
	virtual EnHttpParseResult OnStatusLine(IHttpClient* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)	override {return HPR_OK;}
	virtual EnHttpParseResult OnHeader(IHttpClient* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)		override {return HPR_OK;}
	virtual EnHttpParseResult OnChunkHeader(IHttpClient* pSender, CONNID dwConnID, int iLength)							override {return HPR_OK;}
	virtual EnHttpParseResult OnChunkComplete(IHttpClient* pSender, CONNID dwConnID)									override {return HPR_OK;}
	virtual EnHttpParseResult OnUpgrade(IHttpClient* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)			override {return HPR_OK;}

	virtual EnHandleResult OnWSMessageHeader(IHttpClient* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)	override {return HR_IGNORE;}
	virtual EnHandleResult OnWSMessageBody(IHttpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)		override {return HR_IGNORE;}
	virtual EnHandleResult OnWSMessageComplete(IHttpClient* pSender, CONNID dwConnID)									override {return HR_IGNORE;}
};

/************************************************************************
名称：IHttpClientListener 监听器抽象基类
描述：定义某些事件的默认处理方法（忽略事件）
************************************************************************/

class CHttpSyncClientListener : public CHttpClientListener
{
public:
	virtual EnHandleResult OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)	override {return HR_IGNORE;}

	virtual EnHttpParseResult OnHeadersComplete(IHttpClient* pSender, CONNID dwConnID)									override {return HPR_OK;}
	virtual EnHttpParseResult OnBody(IHttpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)				override {return HPR_OK;}
	virtual EnHttpParseResult OnMessageComplete(IHttpClient* pSender, CONNID dwConnID)									override {return HPR_OK;}
	virtual EnHttpParseResult OnParseError(IHttpClient* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)	override {return HPR_OK;}

};

#endif

/*****************************************************************************************************************************************************/
/************************************************************** Thread Pool Interfaces ***************************************************************/
/*****************************************************************************************************************************************************/

/************************************************************************
名称：线程池组件接口
描述：定义线程池组件的所有操作方法和属性访问方法
************************************************************************/
class IHPThreadPool
{
public:

	/***********************************************************************/
	/***************************** 组件操作方法 *****************************/

	/*
	* 名称：启动线程池组件
	* 描述：
	*		
	* 参数：		dwThreadCount		-- 线程数量，（默认：0）
	*									>0 -> dwThreadCount
	*									=0 -> (CPU核数 * 2 + 2)
	*									<0 -> (CPU核数 * (-dwThreadCount))
	*			dwMaxQueueSize		-- 任务队列最大容量（默认：0，不限制）
	*			enRejectedPolicy	-- 任务拒绝处理策略
	*									TRP_CALL_FAIL（默认）	：立刻返回失败
	*									TRP_WAIT_FOR			：等待（直到成功、超时或线程池关闭等原因导致失败）
	*									TRP_CALLER_RUN			：调用者线程直接执行
	*			dwStackSize			-- 线程堆栈空间大小（默认：0 -> 操作系统默认）
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL Start	(DWORD dwThreadCount = 0, DWORD dwMaxQueueSize = 0, EnRejectedPolicy enRejectedPolicy = TRP_CALL_FAIL, DWORD dwStackSize = 0)	= 0;

	/*
	* 名称：关闭线程池组件
	* 描述：在规定时间内关闭线程池组件，如果工作线程在最大等待时间内未能正常关闭，会尝试强制关闭，这种情况下很可能会造成系统资源泄漏
	*		
	* 参数：		dwMaxWait	-- 最大等待时间（毫秒，默认：INFINITE，一直等待）
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL Stop	(DWORD dwMaxWait = INFINITE)										= 0;

	/*
	* 名称：提交任务
	* 描述：向线程池提交异步任务
	*		
	* 参数：		fnTaskProc	-- 任务处理函数
	*			pvArg		-- 任务参数
	*			dwMaxWait	-- 任务提交最大等待时间（仅对 TRP_WAIT_FOR 类型线程池生效，默认：INFINITE，一直等待）
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*							其中，错误码 ERROR_DESTINATION_ELEMENT_FULL 表示任务队列已满
	*/
	virtual BOOL Submit	(Fn_TaskProc fnTaskProc, PVOID pvArg, DWORD dwMaxWait = INFINITE)	= 0;

	/*
	* 名称：提交 Socket 任务
	* 描述：向线程池提交异步 Socket 任务
	*		
	* 参数：		pTask		-- 任务参数
	*			dwMaxWait	-- 任务提交最大等待时间（仅对 TRP_WAIT_FOR 类型线程池生效，默认：INFINITE，一直等待）
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*							其中，错误码 ERROR_DESTINATION_ELEMENT_FULL 表示任务队列已满
	*							注意：如果提交失败，需要手工调用 Destroy_HP_SocketTaskObj() 销毁 TSocketTask 对象
	*/
	virtual BOOL Submit	(LPTSocketTask pTask, DWORD dwMaxWait = INFINITE)					= 0;

	/*
	* 名称：调整线程池大小
	* 描述：增加或减少线程池的工作线程数量
	*		
	* 参数：		dwNewThreadCount	-- 线程数量
	*									>0 -> dwNewThreadCount
	*									=0 -> (CPU核数 * 2 + 2)
	*									<0 -> (CPU核数 * (-dwNewThreadCount))
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL AdjustThreadCount(DWORD dwNewThreadCount)									= 0;

	/*
	* 名称：等待
	* 描述：等待线程池组件停止运行
	*		
	* 参数：		dwMilliseconds	-- 超时时间（毫秒，默认：-1，永不超时）
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取错误代码
	*/
	virtual BOOL Wait(DWORD dwMilliseconds = INFINITE)										= 0;

public:

	/***********************************************************************/
	/***************************** 属性访问方法 *****************************/

	/* 检查线程池组件是否已启动 */
	virtual BOOL HasStarted						()	= 0;
	/* 查看线程池组件当前状态 */
	virtual EnServiceState	GetState			()	= 0;
	/* 获取当前任务等待队列大小 */
	virtual DWORD GetQueueSize					()	= 0;
	/* 获取当前正在执行的任务数量 */
	virtual DWORD GetTaskCount					()	= 0;
	/* 获取工作线程数量 */
	virtual DWORD GetThreadCount				()	= 0;
	/* 获取任务队列最大容量 */
	virtual DWORD GetMaxQueueSize				()	= 0;
	/* 获取任务拒绝处理策略 */
	virtual EnRejectedPolicy GetRejectedPolicy	()	= 0;

public:
	virtual ~IHPThreadPool() = default;
};
