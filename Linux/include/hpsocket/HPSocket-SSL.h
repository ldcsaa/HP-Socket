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

#include "HPSocket.h"

#ifdef _SSL_SUPPORT

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
/*************************************************************** Global Function Exports *************************************************************/
/*****************************************************************************************************************************************************/

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
HPSOCKET_API void HP_SSL_RemoveThreadLocalState(THR_ID dwThreadID);

/*****************************************************************************************************************************************************/
/******************************************************************** HTTPS Exports ******************************************************************/
/*****************************************************************************************************************************************************/

#ifdef _HTTP_SUPPORT

// 创建 IHttpsServer 对象
HPSOCKET_API IHttpServer* HP_Create_HttpsServer(IHttpServerListener* pListener);
// 创建 IHttpsAgent 对象
HPSOCKET_API IHttpAgent* HP_Create_HttpsAgent(IHttpAgentListener* pListener);
// 创建 IHttpsClient 对象
HPSOCKET_API IHttpClient* HP_Create_HttpsClient(IHttpClientListener* pListener);
// 创建 IHttpsSyncClient 对象
HPSOCKET_API IHttpSyncClient* HP_Create_HttpsSyncClient(IHttpClientListener* pListener);

// 销毁 IHttpsServer 对象
HPSOCKET_API void HP_Destroy_HttpsServer(IHttpServer* pServer);
// 销毁 IHttpsAgent 对象
HPSOCKET_API void HP_Destroy_HttpsAgent(IHttpAgent* pAgent);
// 销毁 IHttpsClient 对象
HPSOCKET_API void HP_Destroy_HttpsClient(IHttpClient* pClient);
// 销毁 IHttpsSyncClient 对象
HPSOCKET_API void HP_Destroy_HttpsSyncClient(IHttpSyncClient* pClient);

// IHttpsServer 对象创建器
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

// IHttpsAgent 对象创建器
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

// IHttpsClient 对象创建器
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

// IHttpsSyncClient 对象创建器
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

// IHttpsServer 对象智能指针
typedef CHPSocketPtr<IHttpServer, IHttpServerListener, HttpsServer_Creator>		CHttpsServerPtr;
// IHttpsAgent 对象智能指针
typedef CHPSocketPtr<IHttpAgent, IHttpAgentListener, HttpsAgent_Creator>		CHttpsAgentPtr;
// IHttpsClient 对象智能指针
typedef CHPSocketPtr<IHttpClient, IHttpClientListener, HttpsClient_Creator>		CHttpsClientPtr;
// IHttpsSyncClient 对象智能指针
typedef CHPSocketPtr<IHttpSyncClient, IHttpClientListener, HttpsSyncClient_Creator>			CHttpsSyncClientPtr;

#endif

#endif