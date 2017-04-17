/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 4.2.1
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

 /******************************************************************************
Module:  HPSocket-SSL

Usage:
		方法一：
		--------------------------------------------------------------------------------------
		0. 应用程序包含 HPTypeDef.h / SocketInterface.h / HPSocket.h / HPSocket-SSL.h 头文件
		1. 启动通信前调用 SSL_Initialize() 函数初始化 SSL 全局环境参数，并确保方法返回成功
		2. 使用 HPSocket 组件执行通信（参考：HPSocket.h）
		3. 通信结束后调用 SSL_Cleanup() 函数清理 SSL 全局运行环境

		方法二：
		--------------------------------------------------------------------------------------
		0. 应用程序包含 HPSocket-SSL.h 头文件
		1. 创建一个全局唯一 CHPSSLInitializer 对象，通过该对象的构造函数初始化 SSL 全局环境参数
		2. 使用 HPSocket 组件执行通信（参考：HPSocket.h）
		3. 通信结束后通过 CHPSSLInitializer 对象的析构函数清理 SSL 全局运行环境

Release:
		<-- 动态链接库 -->
		1. x86/HPSocket-SSL.dll				- (32位/MBCS/Release)
		2. x86/HPSocket-SSL_D.dll			- (32位/MBCS/DeBug)
		3. x86/HPSocket-SSL_U.dll			- (32位/UNICODE/Release)
		4. x86/HPSocket-SSL_UD.dll			- (32位/UNICODE/DeBug)
		5. x64/HPSocket-SSL.dll				- (64位/MBCS/Release)
		6. x64/HPSocket-SSL_D.dll			- (64位/MBCS/DeBug)
		7. x64/HPSocket-SSL_U.dll			- (64位/UNICODE/Release)
		8. x64/HPSocket-SSL_UD.dll			- (64位/UNICODE/DeBug)

		<-- 静态链接库 -->
		!!注意!!：使用 HPSocket 静态库时，需要在工程属性中定义预处理宏 -> HPSOCKET_STATIC_LIB
		1. x86/static/HPSocket-SSL.lib		- (32位/MBCS/Release)
		2. x86/static/HPSocket-SSL_D.lib	- (32位/MBCS/DeBug)
		3. x86/static/HPSocket-SSL_U.lib	- (32位/UNICODE/Release)
		4. x86/static/HPSocket-SSL_UD.lib	- (32位/UNICODE/DeBug)
		5. x64/static/HPSocket-SSL.lib		- (64位/MBCS/Release)
		6. x64/static/HPSocket-SSL_D.lib	- (64位/MBCS/DeBug)
		7. x64/static/HPSocket-SSL_U.lib	- (64位/UNICODE/Release)
		8. x64/static/HPSocket-SSL_UD.lib	- (64位/UNICODE/DeBug)

******************************************************************************/

#pragma once

#ifndef _SSL_SUPPORT
	#define _SSL_SUPPORT
#endif

#include "HPSocket.h"

/*****************************************************************************************************************************************************/
/******************************************************************** SSL Exports ********************************************************************/
/*****************************************************************************************************************************************************/

/**************************************************/
/************** HPSocket-SSL 导出函数 **************/

// 创建 SSL ITcpServer 对象
HPSOCKET_API ITcpServer* HP_Create_SSLServer(ITcpServerListener* pListener);
// 创建 SSL ITcpAgent 对象
HPSOCKET_API ITcpAgent* HP_Create_SSLAgent(ITcpAgentListener* pListener);
// 创建 SSL ITcpClient 对象
HPSOCKET_API ITcpClient* HP_Create_SSLClient(ITcpClientListener* pListener);
// 创建 SSL ITcpPullServer 对象
HPSOCKET_API ITcpPullServer* HP_Create_SSLPullServer(ITcpServerListener* pListener);
// 创建 SSL ITcpPullAgent 对象
HPSOCKET_API ITcpPullAgent* HP_Create_SSLPullAgent(ITcpAgentListener* pListener);
// 创建 SSL ITcpPullClient 对象
HPSOCKET_API ITcpPullClient* HP_Create_SSLPullClient(ITcpClientListener* pListener);
// 创建 SSL ITcpPackServer 对象
HPSOCKET_API ITcpPackServer* HP_Create_SSLPackServer(ITcpServerListener* pListener);
// 创建 SSL ITcpPackAgent 对象
HPSOCKET_API ITcpPackAgent* HP_Create_SSLPackAgent(ITcpAgentListener* pListener);
// 创建 SSL ITcpPackClient 对象
HPSOCKET_API ITcpPackClient* HP_Create_SSLPackClient(ITcpClientListener* pListener);

// 销毁 SSL ITcpServer 对象
HPSOCKET_API void HP_Destroy_SSLServer(ITcpServer* pServer);
// 销毁 SSL ITcpAgent 对象
HPSOCKET_API void HP_Destroy_SSLAgent(ITcpAgent* pAgent);
// 销毁 SSL ITcpClient 对象
HPSOCKET_API void HP_Destroy_SSLClient(ITcpClient* pClient);
// 销毁 SSL ITcpPullServer 对象
HPSOCKET_API void HP_Destroy_SSLPullServer(ITcpPullServer* pServer);
// 销毁 SSL ITcpPullAgent 对象
HPSOCKET_API void HP_Destroy_SSLPullAgent(ITcpPullAgent* pAgent);
// 销毁 SSL ITcpPullClient 对象
HPSOCKET_API void HP_Destroy_SSLPullClient(ITcpPullClient* pClient);
// 销毁 SSL ITcpPackServer 对象
HPSOCKET_API void HP_Destroy_SSLPackServer(ITcpPackServer* pServer);
// 销毁 SSL ITcpPackAgent 对象
HPSOCKET_API void HP_Destroy_SSLPackAgent(ITcpPackAgent* pAgent);
// 销毁 SSL ITcpPackClient 对象
HPSOCKET_API void HP_Destroy_SSLPackClient(ITcpPackClient* pClient);

// SSL ITcpServer 对象创建器
struct SSLServer_Creator
{
	static ITcpServer* Create(ITcpServerListener* pListener)
	{
		return HP_Create_SSLServer(pListener);
	}

	static void Destroy(ITcpServer* pServer)
	{
		HP_Destroy_SSLServer(pServer);
	}
};

// SSL ITcpAgent 对象创建器
struct SSLAgent_Creator
{
	static ITcpAgent* Create(ITcpAgentListener* pListener)
	{
		return HP_Create_SSLAgent(pListener);
	}

	static void Destroy(ITcpAgent* pAgent)
	{
		HP_Destroy_SSLAgent(pAgent);
	}
};

// SSL ITcpClient 对象创建器
struct SSLClient_Creator
{
	static ITcpClient* Create(ITcpClientListener* pListener)
	{
		return HP_Create_SSLClient(pListener);
	}

	static void Destroy(ITcpClient* pClient)
	{
		HP_Destroy_SSLClient(pClient);
	}
};

// SSL ITcpPullServer 对象创建器
struct SSLPullServer_Creator
{
	static ITcpPullServer* Create(ITcpServerListener* pListener)
	{
		return HP_Create_SSLPullServer(pListener);
	}

	static void Destroy(ITcpPullServer* pServer)
	{
		HP_Destroy_SSLPullServer(pServer);
	}
};

// SSL ITcpPullAgent 对象创建器
struct SSLPullAgent_Creator
{
	static ITcpPullAgent* Create(ITcpAgentListener* pListener)
	{
		return HP_Create_SSLPullAgent(pListener);
	}

	static void Destroy(ITcpPullAgent* pAgent)
	{
		HP_Destroy_SSLPullAgent(pAgent);
	}
};

// SSL ITcpPullClient 对象创建器
struct SSLPullClient_Creator
{
	static ITcpPullClient* Create(ITcpClientListener* pListener)
	{
		return HP_Create_SSLPullClient(pListener);
	}

	static void Destroy(ITcpPullClient* pClient)
	{
		HP_Destroy_SSLPullClient(pClient);
	}
};

// SSL ITcpPackServer 对象创建器
struct SSLPackServer_Creator
{
	static ITcpPackServer* Create(ITcpServerListener* pListener)
	{
		return HP_Create_SSLPackServer(pListener);
	}

	static void Destroy(ITcpPackServer* pServer)
	{
		HP_Destroy_SSLPackServer(pServer);
	}
};

// SSL ITcpPackAgent 对象创建器
struct SSLPackAgent_Creator
{
	static ITcpPackAgent* Create(ITcpAgentListener* pListener)
	{
		return HP_Create_SSLPackAgent(pListener);
	}

	static void Destroy(ITcpPackAgent* pAgent)
	{
		HP_Destroy_SSLPackAgent(pAgent);
	}
};

// SSL ITcpPackClient 对象创建器
struct SSLPackClient_Creator
{
	static ITcpPackClient* Create(ITcpClientListener* pListener)
	{
		return HP_Create_SSLPackClient(pListener);
	}

	static void Destroy(ITcpPackClient* pClient)
	{
		HP_Destroy_SSLPackClient(pClient);
	}
};

// SSL ITcpServer 对象智能指针
typedef CHPSocketPtr<ITcpServer, ITcpServerListener, SSLServer_Creator>			CSSLServerPtr;
// SSL ITcpAgent 对象智能指针
typedef CHPSocketPtr<ITcpAgent, ITcpAgentListener, SSLAgent_Creator>			CSSLAgentPtr;
// SSL ITcpClient 对象智能指针
typedef CHPSocketPtr<ITcpClient, ITcpClientListener, SSLClient_Creator>			CSSLClientPtr;
// SSL ITcpPullServer 对象智能指针
typedef CHPSocketPtr<ITcpPullServer, ITcpServerListener, SSLPullServer_Creator>	CSSLPullServerPtr;
// SSL ITcpPullAgent 对象智能指针
typedef CHPSocketPtr<ITcpPullAgent, ITcpAgentListener, SSLPullAgent_Creator>	CSSLPullAgentPtr;
// SSL ITcpPullClient 对象智能指针
typedef CHPSocketPtr<ITcpPullClient, ITcpClientListener, SSLPullClient_Creator>	CSSLPullClientPtr;
// SSL ITcpPackServer 对象智能指针
typedef CHPSocketPtr<ITcpPackServer, ITcpServerListener, SSLPackServer_Creator>	CSSLPackServerPtr;
// SSL ITcpPackAgent 对象智能指针
typedef CHPSocketPtr<ITcpPackAgent, ITcpAgentListener, SSLPackAgent_Creator>	CSSLPackAgentPtr;
// SSL ITcpPackClient 对象智能指针
typedef CHPSocketPtr<ITcpPackClient, ITcpClientListener, SSLPackClient_Creator>	CSSLPackClientPtr;

/*****************************************************************************************************************************************************/
/******************************************************************** HTTPS Exports ******************************************************************/
/*****************************************************************************************************************************************************/

// 创建 IHttpServer 对象
HPSOCKET_API IHttpServer* HP_Create_HttpsServer(IHttpServerListener* pListener);
// 创建 IHttpAgent 对象
HPSOCKET_API IHttpAgent* HP_Create_HttpsAgent(IHttpAgentListener* pListener);
// 创建 IHttpClient 对象
HPSOCKET_API IHttpClient* HP_Create_HttpsClient(IHttpClientListener* pListener);
// 创建 IHttpSyncClient 对象
HPSOCKET_API IHttpSyncClient* HP_Create_HttpsSyncClient(IHttpClientListener* pListener);

// 销毁 IHttpServer 对象
HPSOCKET_API void HP_Destroy_HttpsServer(IHttpServer* pServer);
// 销毁 IHttpAgent 对象
HPSOCKET_API void HP_Destroy_HttpsAgent(IHttpAgent* pAgent);
// 销毁 IHttpClient 对象
HPSOCKET_API void HP_Destroy_HttpsClient(IHttpClient* pClient);
// 销毁 IHttpSyncClient 对象
HPSOCKET_API void HP_Destroy_HttpsSyncClient(IHttpSyncClient* pClient);

// IHttpServer 对象创建器
struct HttpsServer_Creator
{
	static IHttpServer* Create(IHttpServerListener* pListener)
	{
		return HP_Create_HttpsServer(pListener);
	}

	static void Destroy(IHttpServer* pServer)
	{
		HP_Destroy_HttpsServer(pServer);
	}
};

// IHttpAgent 对象创建器
struct HttpsAgent_Creator
{
	static IHttpAgent* Create(IHttpAgentListener* pListener)
	{
		return HP_Create_HttpsAgent(pListener);
	}

	static void Destroy(IHttpAgent* pAgent)
	{
		HP_Destroy_HttpsAgent(pAgent);
	}
};

// IHttpClient 对象创建器
struct HttpsClient_Creator
{
	static IHttpClient* Create(IHttpClientListener* pListener)
	{
		return HP_Create_HttpsClient(pListener);
	}

	static void Destroy(IHttpClient* pClient)
	{
		HP_Destroy_HttpsClient(pClient);
	}
};

// IHttpSyncClient 对象创建器
struct HttpsSyncClient_Creator
{
	static IHttpSyncClient* Create(IHttpClientListener* pListener)
	{
		return HP_Create_HttpsSyncClient(pListener);
	}

	static void Destroy(IHttpSyncClient* pClient)
	{
		HP_Destroy_HttpsSyncClient(pClient);
	}
};

// IHttpServer 对象智能指针
typedef CHPSocketPtr<IHttpServer, IHttpServerListener, HttpsServer_Creator>		CHttpsServerPtr;
// IHttpAgent 对象智能指针
typedef CHPSocketPtr<IHttpAgent, IHttpAgentListener, HttpsAgent_Creator>		CHttpsAgentPtr;
// IHttpClient 对象智能指针
typedef CHPSocketPtr<IHttpClient, IHttpClientListener, HttpsClient_Creator>		CHttpsClientPtr;
// IHttpSyncClient 对象智能指针
typedef CHPSocketPtr<IHttpSyncClient, BOOL, HttpsSyncClient_Creator>			CHttpsSyncClientPtr;

/*****************************************************************************************************************************************************/
/*************************************************************** Global Function Exports *************************************************************/
/*****************************************************************************************************************************************************/

/*
* 名称：初始化 SSL 全局环境参数
* 描述：SSL 全局环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
*		
* 参数：		enSessionMode			-- SSL 工作模式（参考 EnSSLSessionMode）
*			iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
*			lpszPemCertFile			-- 证书文件（客户端可选）
*			lpszPemKeyFile			-- 私钥文件（客户端可选）
*			lpszKeyPasswod			-- 私钥密码（没有密码则为空）
*			lpszCAPemCertFileOrPath	-- CA 证书文件或目录（单向验证或客户端可选）
*			fnServerNameCallback	-- SNI 回调函数指针（可选，只用于服务端）
*
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/
HPSOCKET_API BOOL HP_SSL_Initialize(EnSSLSessionMode enSessionMode, int iVerifyMode = SSL_VM_NONE, LPCTSTR lpszPemCertFile = nullptr, LPCTSTR lpszPemKeyFile = nullptr, LPCTSTR lpszKeyPasswod = nullptr, LPCTSTR lpszCAPemCertFileOrPath = nullptr, Fn_SNI_ServerNameCallback fnServerNameCallback = nullptr);

/*
* 名称：增加 SNI 主机证书（只用于服务端）
* 描述：SSL 服务端在 Initialize() 成功后可以调用本方法增加多个 SNI 主机证书
*		
* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
*			lpszPemCertFile			-- 证书文件
*			lpszPemKeyFile			-- 私钥文件
*			lpszKeyPasswod			-- 私钥密码（没有密码则为空）
*			lpszCAPemCertFileOrPath	-- CA 证书文件或目录（单向验证可选）
*
* 返回值：	正数		-- 成功，并返回 SNI 主机证书对应的索引，该索引用于在 SNI 回调函数中定位 SNI 主机
*			负数		-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/
HPSOCKET_API int HP_SSL_AddServerContext(int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod = nullptr, LPCTSTR lpszCAPemCertFileOrPath = nullptr);

/*
* 名称：清理 SSL 全局运行环境
* 描述：清理 SSL 全局运行环境，回收 SSL 相关内存
*		1、应用程序退出时会自动调用本方法
*		2、当要重新设置 SSL 全局环境参数时，需要先调用本方法清理原先的环境参数
*		
* 参数：	无
* 
* 返回值：无
*/
HPSOCKET_API void HP_SSL_Cleanup();

/*
* 名称：清理线程局部环境 SSL 资源
* 描述：任何一个操作 SSL 的线程，在通信结束时都需要清理线程局部环境 SSL 资源
*		1、主线程和 HP-Socket 工作线程在通信结束时会自动清理线程局部环境 SSL 资源。因此，一般情况下不必手工调用本方法
*		2、特殊情况下，当自定义线程参与 HP-Socket 通信操作并检查到 SSL 内存泄漏时，需在每次通信结束时自定义线程调用本方法
*		
* 参数：		dwThreadID	-- 线程 ID（0：当前线程）
* 
* 返回值：无
*/
HPSOCKET_API void HP_SSL_RemoveThreadLocalState(DWORD dwThreadID = 0);

/* 检查 SSL 全局运行环境是否初始化完成 */
HPSOCKET_API BOOL HP_SSL_IsValid();

class CHPSSLInitializer
{
public:
	CHPSSLInitializer(EnSSLSessionMode enSessionMode, int iVerifyMode = SSL_VM_NONE, LPCTSTR lpszPemCertFile = nullptr, LPCTSTR lpszPemKeyFile = nullptr, LPCTSTR lpszKeyPasswod = nullptr, LPCTSTR lpszCAPemCertFileOrPath = nullptr, Fn_SNI_ServerNameCallback fnServerNameCallback = nullptr)
	{
		HP_SSL_Initialize(enSessionMode, iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath, fnServerNameCallback);
	}

	~CHPSSLInitializer()
	{
		HP_SSL_Cleanup();
	}

	BOOL Reset(EnSSLSessionMode enSessionMode, int iVerifyMode = SSL_VM_NONE, LPCTSTR lpszPemCertFile = nullptr, LPCTSTR lpszPemKeyFile = nullptr, LPCTSTR lpszKeyPasswod = nullptr, LPCTSTR lpszCAPemCertFileOrPath = nullptr, Fn_SNI_ServerNameCallback fnServerNameCallback = nullptr)
	{
		HP_SSL_Cleanup();
		return HP_SSL_Initialize(enSessionMode, iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath, fnServerNameCallback);
	}

	int AddServerContext(int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod = nullptr, LPCTSTR lpszCAPemCertFileOrPath = nullptr)
	{
		return HP_SSL_AddServerContext(iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath);
	}

	void RemoveThreadLocalState(DWORD dwThreadID = 0)
	{
		HP_SSL_RemoveThreadLocalState(dwThreadID);
	}

	BOOL IsValid()
	{
		return HP_SSL_IsValid();
	}

	DWORD GetErrorCode()
	{
		return SYS_GetLastError();
	}

private:
	CHPSSLInitializer(const CHPSSLInitializer&);
	CHPSSLInitializer& operator = (const CHPSSLInitializer&);
};
