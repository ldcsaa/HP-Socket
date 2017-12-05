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

/******************************************************************************
Module:  HPSocket-SSL for C

Desc: 导出纯 C 函数，让其它语言（如：C / C# / Delphi 等）能方便地使用 HPSocket SSL

Usage:
		方法：
		--------------------------------------------------------------------------------------
		0. （C/C++ 程序）包含 HPTypeDef.h / HPSocket4C.h / HPSocket4C-SSL.h 头文件
		1. 每次启动通信前调用 HP_SSL<Xxx>_SetupSSLContext() 函数初始化 SSL 环境参数，并确保方法返回成功
		2. 使用 HPSocket SSL 组件执行通信（参考：HPSocket4C.h）

Release:
		<-- 动态链接库 -->
		1. x86/HPSocket4C-SSL.dll			- (32位/MBCS/Release)
		2. x86/HPSocket4C-SSL_D.dll			- (32位/MBCS/DeBug)
		3. x86/HPSocket4C-SSL_U.dll			- (32位/UNICODE/Release)
		4. x86/HPSocket4C-SSL_UD.dll		- (32位/UNICODE/DeBug)
		5. x64/HPSocket4C-SSL.dll			- (64位/MBCS/Release)
		6. x64/HPSocket4C-SSL_D.dll			- (64位/MBCS/DeBug)
		7. x64/HPSocket4C-SSL_U.dll			- (64位/UNICODE/Release)
		8. x64/HPSocket4C-SSL_UD.dll		- (64位/UNICODE/DeBug)

		<-- 静态链接库 -->
		!!注意!!：使用 HPSocket 静态库时，需要在工程属性中定义预处理宏 -> HPSOCKET_STATIC_LIB
		1. x86/static/HPSocket4C-SSL.lib	- (32位/MBCS/Release)
		2. x86/static/HPSocket4C-SSL_D.lib	- (32位/MBCS/DeBug)
		3. x86/static/HPSocket4C-SSL_U.lib	- (32位/UNICODE/Release)
		4. x86/static/HPSocket4C-SSL_UD.lib	- (32位/UNICODE/DeBug)
		5. x64/static/HPSocket4C-SSL.lib	- (64位/MBCS/Release)
		6. x64/static/HPSocket4C-SSL_D.lib	- (64位/MBCS/DeBug)
		7. x64/static/HPSocket4C-SSL_U.lib	- (64位/UNICODE/Release)
		8. x64/static/HPSocket4C-SSL_UD.lib	- (64位/UNICODE/DeBug)

******************************************************************************/

#pragma once

#ifndef _SSL_SUPPORT
	#define _SSL_SUPPORT
#endif

#include "HPSocket4C.h"

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
HPSOCKET_API HP_SSLServer __stdcall Create_HP_SSLServer(HP_TcpServerListener pListener);
// 创建 HP_SSLAgent 对象
HPSOCKET_API HP_SSLAgent __stdcall Create_HP_SSLAgent(HP_TcpAgentListener pListener);
// 创建 HP_SSLClient 对象
HPSOCKET_API HP_SSLClient __stdcall Create_HP_SSLClient(HP_TcpClientListener pListener);
// 创建 HP_SSLPullServer 对象
HPSOCKET_API HP_SSLPullServer __stdcall Create_HP_SSLPullServer(HP_TcpPullServerListener pListener);
// 创建 HP_SSLPullAgent 对象
HPSOCKET_API HP_SSLPullAgent __stdcall Create_HP_SSLPullAgent(HP_TcpPullAgentListener pListener);
// 创建 HP_SSLPullClient 对象
HPSOCKET_API HP_SSLPullClient __stdcall Create_HP_SSLPullClient(HP_TcpPullClientListener pListener);
// 创建 HP_SSLPackServer 对象
HPSOCKET_API HP_SSLPackServer __stdcall Create_HP_SSLPackServer(HP_TcpServerListener pListener);
// 创建 HP_SSLPackAgent 对象
HPSOCKET_API HP_SSLPackAgent __stdcall Create_HP_SSLPackAgent(HP_TcpAgentListener pListener);
// 创建 HP_SSLPackClient 对象
HPSOCKET_API HP_SSLPackClient __stdcall Create_HP_SSLPackClient(HP_TcpClientListener pListener);

// 销毁 HP_SSLServer 对象
HPSOCKET_API void __stdcall Destroy_HP_SSLServer(HP_SSLServer pServer);
// 销毁 HP_SSLAgent 对象
HPSOCKET_API void __stdcall Destroy_HP_SSLAgent(HP_SSLAgent pAgent);
// 销毁 HP_SSLClient 对象
HPSOCKET_API void __stdcall Destroy_HP_SSLClient(HP_SSLClient pClient);
// 销毁 HP_SSLPullServer 对象
HPSOCKET_API void __stdcall Destroy_HP_SSLPullServer(HP_SSLPullServer pServer);
// 销毁 HP_SSLPullAgent 对象
HPSOCKET_API void __stdcall Destroy_HP_SSLPullAgent(HP_SSLPullAgent pAgent);
// 销毁 HP_SSLPullClient 对象
HPSOCKET_API void __stdcall Destroy_HP_SSLPullClient(HP_SSLPullClient pClient);
// 销毁 HP_SSLPackServer 对象
HPSOCKET_API void __stdcall Destroy_HP_SSLPackServer(HP_SSLPackServer pServer);
// 销毁 HP_SSLPackAgent 对象
HPSOCKET_API void __stdcall Destroy_HP_SSLPackAgent(HP_SSLPackAgent pAgent);
// 销毁 HP_SSLPackClient 对象
HPSOCKET_API void __stdcall Destroy_HP_SSLPackClient(HP_SSLPackClient pClient);

/*****************************************************************************************************************************************************/
/******************************************************************** HTTPS Exports ******************************************************************/
/*****************************************************************************************************************************************************/

/****************************************************/
/**************** HTTPS 对象创建函数 *****************/

// 创建 HP_HttpsServer 对象
HPSOCKET_API HP_HttpsServer __stdcall Create_HP_HttpsServer(HP_HttpServerListener pListener);
// 创建 HP_HttpsAgent 对象
HPSOCKET_API HP_HttpsAgent __stdcall Create_HP_HttpsAgent(HP_HttpAgentListener pListener);
// 创建 HP_HttpsClient 对象
HPSOCKET_API HP_HttpsClient __stdcall Create_HP_HttpsClient(HP_HttpClientListener pListener);
// 创建 HP_HttpSyncClient 对象
HPSOCKET_API HP_HttpsSyncClient __stdcall Create_HP_HttpsSyncClient(HP_HttpClientListener pListener);

// 销毁 HP_HttpsServer 对象
HPSOCKET_API void __stdcall Destroy_HP_HttpsServer(HP_HttpsServer pServer);
// 销毁 HP_HttpsAgent 对象
HPSOCKET_API void __stdcall Destroy_HP_HttpsAgent(HP_HttpsAgent pAgent);
// 销毁 HP_HttpsClient 对象
HPSOCKET_API void __stdcall Destroy_HP_HttpsClient(HP_HttpsClient pClient);
// 销毁 HP_HttpSyncClient 对象
HPSOCKET_API void __stdcall Destroy_HP_HttpsSyncClient(HP_HttpsSyncClient pClient);

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
HPSOCKET_API void __stdcall HP_SSL_RemoveThreadLocalState(DWORD dwThreadID);

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
HPSOCKET_API BOOL __stdcall HP_SSLServer_SetupSSLContext(HP_SSLServer pServer, int iVerifyMode /* SSL_VM_NONE */, LPCTSTR lpszPemCertFile /* nullptr */, LPCTSTR lpszPemKeyFile /* nullptr */, LPCTSTR lpszKeyPasswod /* nullptr */, LPCTSTR lpszCAPemCertFileOrPath /* nullptr */, HP_Fn_SNI_ServerNameCallback fnServerNameCallback /* nullptr */);

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
HPSOCKET_API int __stdcall HP_SSLServer_AddSSLContext(HP_SSLServer pServer, int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod /* nullptr */, LPCTSTR lpszCAPemCertFileOrPath /* nullptr */);

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
HPSOCKET_API void __stdcall HP_SSLServer_CleanupSSLContext(HP_SSLServer pServer);

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
HPSOCKET_API BOOL __stdcall HP_SSLAgent_SetupSSLContext(HP_SSLAgent pAgent, int iVerifyMode /* SSL_VM_NONE */, LPCTSTR lpszPemCertFile /* nullptr */, LPCTSTR lpszPemKeyFile /* nullptr */, LPCTSTR lpszKeyPasswod /* nullptr */, LPCTSTR lpszCAPemCertFileOrPath /* nullptr */);

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
HPSOCKET_API void __stdcall HP_SSLAgent_CleanupSSLContext(HP_SSLAgent pAgent);

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
HPSOCKET_API BOOL __stdcall HP_SSLClient_SetupSSLContext(HP_SSLClient pClient, int iVerifyMode /* SSL_VM_NONE */, LPCTSTR lpszPemCertFile /* nullptr */, LPCTSTR lpszPemKeyFile /* nullptr */, LPCTSTR lpszKeyPasswod /* nullptr */, LPCTSTR lpszCAPemCertFileOrPath /* nullptr */);

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
HPSOCKET_API void __stdcall HP_SSLClient_CleanupSSLContext(HP_SSLClient pClient);
