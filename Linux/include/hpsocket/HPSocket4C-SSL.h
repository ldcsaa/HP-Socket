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

#include "HPSocket4C.h"

#ifdef _SSL_SUPPORT

/************************************************************************
名称：定义 SSL Socket 对象指针类型别名
描述：把 SSL Socket 对象指针定义为更直观的别名
************************************************************************/

typedef HP_Object	HP_SSLServer;
typedef HP_Object	HP_SSLAgent;
typedef HP_Object	HP_SSLClient;
typedef HP_Object	HP_SSLPullServer;
typedef HP_Object	HP_SSLPullAgent;
typedef HP_Object	HP_SSLPullClient;
typedef HP_Object	HP_SSLPackServer;
typedef HP_Object	HP_SSLPackAgent;
typedef HP_Object	HP_SSLPackClient;

typedef HP_Object	HP_HttpsServer;
typedef HP_Object	HP_HttpsAgent;
typedef HP_Object	HP_HttpsClient;
typedef HP_Object	HP_HttpsSyncClient;

/*****************************************************************************************************************************************************/
/******************************************************************** SSL Exports ********************************************************************/
/*****************************************************************************************************************************************************/

/********************************************************/
/************** HPSocket4C-SSL 对象创建函数 **************/

// 创建 HP_SSLServer 对象
HPSOCKET_API HP_SSLServer __HP_CALL Create_HP_SSLServer(HP_TcpServerListener pListener);
// 创建 HP_SSLAgent 对象
HPSOCKET_API HP_SSLAgent __HP_CALL Create_HP_SSLAgent(HP_TcpAgentListener pListener);
// 创建 HP_SSLClient 对象
HPSOCKET_API HP_SSLClient __HP_CALL Create_HP_SSLClient(HP_TcpClientListener pListener);
// 创建 HP_SSLPullServer 对象
HPSOCKET_API HP_SSLPullServer __HP_CALL Create_HP_SSLPullServer(HP_TcpPullServerListener pListener);
// 创建 HP_SSLPullAgent 对象
HPSOCKET_API HP_SSLPullAgent __HP_CALL Create_HP_SSLPullAgent(HP_TcpPullAgentListener pListener);
// 创建 HP_SSLPullClient 对象
HPSOCKET_API HP_SSLPullClient __HP_CALL Create_HP_SSLPullClient(HP_TcpPullClientListener pListener);
// 创建 HP_SSLPackServer 对象
HPSOCKET_API HP_SSLPackServer __HP_CALL Create_HP_SSLPackServer(HP_TcpServerListener pListener);
// 创建 HP_SSLPackAgent 对象
HPSOCKET_API HP_SSLPackAgent __HP_CALL Create_HP_SSLPackAgent(HP_TcpAgentListener pListener);
// 创建 HP_SSLPackClient 对象
HPSOCKET_API HP_SSLPackClient __HP_CALL Create_HP_SSLPackClient(HP_TcpClientListener pListener);

// 销毁 HP_SSLServer 对象
HPSOCKET_API void __HP_CALL Destroy_HP_SSLServer(HP_SSLServer pServer);
// 销毁 HP_SSLAgent 对象
HPSOCKET_API void __HP_CALL Destroy_HP_SSLAgent(HP_SSLAgent pAgent);
// 销毁 HP_SSLClient 对象
HPSOCKET_API void __HP_CALL Destroy_HP_SSLClient(HP_SSLClient pClient);
// 销毁 HP_SSLPullServer 对象
HPSOCKET_API void __HP_CALL Destroy_HP_SSLPullServer(HP_SSLPullServer pServer);
// 销毁 HP_SSLPullAgent 对象
HPSOCKET_API void __HP_CALL Destroy_HP_SSLPullAgent(HP_SSLPullAgent pAgent);
// 销毁 HP_SSLPullClient 对象
HPSOCKET_API void __HP_CALL Destroy_HP_SSLPullClient(HP_SSLPullClient pClient);
// 销毁 HP_SSLPackServer 对象
HPSOCKET_API void __HP_CALL Destroy_HP_SSLPackServer(HP_SSLPackServer pServer);
// 销毁 HP_SSLPackAgent 对象
HPSOCKET_API void __HP_CALL Destroy_HP_SSLPackAgent(HP_SSLPackAgent pAgent);
// 销毁 HP_SSLPackClient 对象
HPSOCKET_API void __HP_CALL Destroy_HP_SSLPackClient(HP_SSLPackClient pClient);

/*****************************************************************************************************************************************************/
/*************************************************************** Global Function Exports *************************************************************/
/*****************************************************************************************************************************************************/

/***************************************************************************************/
/************************************ SSL 初始化方法 ************************************/

/*
* 名称：清理线程局部环境 SSL 资源
* 描述：任何一个操作 SSL 的线程，通信结束时都需要清理线程局部环境 SSL 资源
*		1、主线程和 HP-Socket 工作线程在通信结束时会自动清理线程局部环境 SSL 资源。因此，一般情况下不必手工调用本方法
*		2、特殊情况下，当自定义线程参与 HP-Socket 通信操作并检查到 SSL 内存泄漏时，需在每次通信结束时自定义线程调用本方法
*		
* 参数：		dwThreadID	-- 线程 ID（0：当前线程）
* 
* 返回值：无
*/
HPSOCKET_API void __HP_CALL HP_SSL_RemoveThreadLocalState(THR_ID dwThreadID);

/*
* 名称：初始化通信组件 SSL 环境参数
* 描述：SSL 环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
*		
* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
*			lpszPemCertFile			-- 证书文件
*			lpszPemKeyFile			-- 私钥文件
*			lpszKeyPasswod			-- 私钥密码（没有密码则为空）
*			lpszCAPemCertFileOrPath	-- CA 证书文件或目录（单向验证或客户端可选）
*			fnServerNameCallback	-- SNI 回调函数指针（可选）
*
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/
HPSOCKET_API BOOL __HP_CALL HP_SSLServer_SetupSSLContext(HP_SSLServer pServer, int iVerifyMode /* SSL_VM_NONE */, LPCTSTR lpszPemCertFile /* nullptr */, LPCTSTR lpszPemKeyFile /* nullptr */, LPCTSTR lpszKeyPasswod /* nullptr */, LPCTSTR lpszCAPemCertFileOrPath /* nullptr */, HP_Fn_SNI_ServerNameCallback fnServerNameCallback /* nullptr */);

/*
* 名称：增加 SNI 主机证书
* 描述：SSL 服务端在 SetupSSLContext() 成功后可以调用本方法增加多个 SNI 主机证书
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
HPSOCKET_API int __HP_CALL HP_SSLServer_AddSSLContext(HP_SSLServer pServer, int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod /* nullptr */, LPCTSTR lpszCAPemCertFileOrPath /* nullptr */);

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
HPSOCKET_API void __HP_CALL HP_SSLServer_CleanupSSLContext(HP_SSLServer pServer);

/*
* 名称：初始化通信组件 SSL 环境参数
* 描述：SSL 环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
*		
* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
*			lpszPemCertFile			-- 证书文件（客户端可选）
*			lpszPemKeyFile			-- 私钥文件（客户端可选）
*			lpszKeyPasswod			-- 私钥密码（没有密码则为空）
*			lpszCAPemCertFileOrPath	-- CA 证书文件或目录（单向验证或客户端可选）
*
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/
HPSOCKET_API BOOL __HP_CALL HP_SSLAgent_SetupSSLContext(HP_SSLAgent pAgent, int iVerifyMode /* SSL_VM_NONE */, LPCTSTR lpszPemCertFile /* nullptr */, LPCTSTR lpszPemKeyFile /* nullptr */, LPCTSTR lpszKeyPasswod /* nullptr */, LPCTSTR lpszCAPemCertFileOrPath /* nullptr */);

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
HPSOCKET_API void __HP_CALL HP_SSLAgent_CleanupSSLContext(HP_SSLAgent pAgent);

/*
* 名称：初始化通信组件 SSL 环境参数
* 描述：SSL 环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
*		
* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
*			lpszPemCertFile			-- 证书文件（客户端可选）
*			lpszPemKeyFile			-- 私钥文件（客户端可选）
*			lpszKeyPasswod			-- 私钥密码（没有密码则为空）
*			lpszCAPemCertFileOrPath	-- CA 证书文件或目录（单向验证或客户端可选）
*
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/
HPSOCKET_API BOOL __HP_CALL HP_SSLClient_SetupSSLContext(HP_SSLClient pClient, int iVerifyMode /* SSL_VM_NONE */, LPCTSTR lpszPemCertFile /* nullptr */, LPCTSTR lpszPemKeyFile /* nullptr */, LPCTSTR lpszKeyPasswod /* nullptr */, LPCTSTR lpszCAPemCertFileOrPath /* nullptr */);

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
HPSOCKET_API void __HP_CALL HP_SSLClient_CleanupSSLContext(HP_SSLClient pClient);

/***************************************************************************************/
/************************************* SSL 操作方法 ************************************/

/*
* 名称：启动 SSL 握手
* 描述：当通信组件设置为非自动握手时，需要调用本方法启动 SSL 握手
*
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/
HPSOCKET_API BOOL __HP_CALL HP_SSLServer_StartSSLHandShake(HP_SSLServer pServer, HP_CONNID dwConnID);

/* 设置通信组件握手方式（默认：TRUE，自动握手） */
HPSOCKET_API void __HP_CALL HP_SSLServer_SetSSLAutoHandShake(HP_SSLServer pServer, BOOL bAutoHandShake);
/* 获取通信组件握手方式 */
HPSOCKET_API BOOL __HP_CALL HP_SSLServer_IsSSLAutoHandShake(HP_SSLServer pServer);

/*
* 名称：启动 SSL 握手
* 描述：当通信组件设置为非自动握手时，需要调用本方法启动 SSL 握手
*
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/
HPSOCKET_API BOOL __HP_CALL HP_SSLAgent_StartSSLHandShake(HP_SSLAgent pAgent, HP_CONNID dwConnID);

/* 设置通信组件握手方式（默认：TRUE，自动握手） */
HPSOCKET_API void __HP_CALL HP_SSLAgent_SetSSLAutoHandShake(HP_SSLAgent pAgent, BOOL bAutoHandShake);
/* 获取通信组件握手方式 */
HPSOCKET_API BOOL __HP_CALL HP_SSLAgent_IsSSLAutoHandShake(HP_SSLAgent pAgent);

/*
* 名称：启动 SSL 握手
* 描述：当通信组件设置为非自动握手时，需要调用本方法启动 SSL 握手
*
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/
HPSOCKET_API BOOL __HP_CALL HP_SSLClient_StartSSLHandShake(HP_SSLClient pClient);

/* 设置通信组件握手方式（默认：TRUE，自动握手） */
HPSOCKET_API void __HP_CALL HP_SSLClient_SetSSLAutoHandShake(HP_SSLClient pClient, BOOL bAutoHandShake);
/* 获取通信组件握手方式 */
HPSOCKET_API BOOL __HP_CALL HP_SSLClient_IsSSLAutoHandShake(HP_SSLClient pClient);

/*****************************************************************************************************************************************************/
/******************************************************************** HTTPS Exports ******************************************************************/
/*****************************************************************************************************************************************************/

#ifdef _HTTP_SUPPORT

/****************************************************/
/**************** HTTPS 对象创建函数 *****************/

// 创建 HP_HttpsServer 对象
HPSOCKET_API HP_HttpsServer __HP_CALL Create_HP_HttpsServer(HP_HttpServerListener pListener);
// 创建 HP_HttpsAgent 对象
HPSOCKET_API HP_HttpsAgent __HP_CALL Create_HP_HttpsAgent(HP_HttpAgentListener pListener);
// 创建 HP_HttpsClient 对象
HPSOCKET_API HP_HttpsClient __HP_CALL Create_HP_HttpsClient(HP_HttpClientListener pListener);
// 创建 HP_HttpsSyncClient 对象
HPSOCKET_API HP_HttpsSyncClient __HP_CALL Create_HP_HttpsSyncClient(HP_HttpClientListener pListener);

// 销毁 HP_HttpsServer 对象
HPSOCKET_API void __HP_CALL Destroy_HP_HttpsServer(HP_HttpsServer pServer);
// 销毁 HP_HttpsAgent 对象
HPSOCKET_API void __HP_CALL Destroy_HP_HttpsAgent(HP_HttpsAgent pAgent);
// 销毁 HP_HttpsClient 对象
HPSOCKET_API void __HP_CALL Destroy_HP_HttpsClient(HP_HttpsClient pClient);
// 销毁 HP_HttpsSyncClient 对象
HPSOCKET_API void __HP_CALL Destroy_HP_HttpsSyncClient(HP_HttpsSyncClient pClient);

#endif

#endif