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
 
#include "stdafx.h"
#include "HPSocket-SSL.h"
#include "TcpServer.h"
#include "TcpAgent.h"
#include "TcpClient.h"
#include "TcpPullServer.h"
#include "TcpPullClient.h"
#include "TcpPullAgent.h"
#include "TcpPackServer.h"
#include "TcpPackClient.h"
#include "TcpPackAgent.h"

#include "HttpServer.h"
#include "HttpAgent.h"
#include "HttpClient.h"

/*****************************************************************************************************************************************************/
/******************************************************************** SSL Exports ********************************************************************/
/*****************************************************************************************************************************************************/

HPSOCKET_API ITcpServer* HP_Create_SSLServer(ITcpServerListener* pListener)
{
	return new CSSLServer(pListener);
}

HPSOCKET_API ITcpAgent* HP_Create_SSLAgent(ITcpAgentListener* pListener)
{
	return new CSSLAgent(pListener);
}

HPSOCKET_API ITcpClient* HP_Create_SSLClient(ITcpClientListener* pListener)
{
	return new CSSLClient(pListener);
}

HPSOCKET_API ITcpPullServer* HP_Create_SSLPullServer(ITcpServerListener* pListener)
{
	return (ITcpPullServer*)(new CSSLPullServer(pListener));
}

HPSOCKET_API ITcpPullAgent* HP_Create_SSLPullAgent(ITcpAgentListener* pListener)
{
	return (ITcpPullAgent*)(new CSSLPullAgent(pListener));
}

HPSOCKET_API ITcpPullClient* HP_Create_SSLPullClient(ITcpClientListener* pListener)
{
	return (ITcpPullClient*)(new CSSLPullClient(pListener));
}

HPSOCKET_API ITcpPackServer* HP_Create_SSLPackServer(ITcpServerListener* pListener)
{
	return (ITcpPackServer*)(new CSSLPackServer(pListener));
}

HPSOCKET_API ITcpPackAgent* HP_Create_SSLPackAgent(ITcpAgentListener* pListener)
{
	return (ITcpPackAgent*)(new CSSLPackAgent(pListener));
}

HPSOCKET_API ITcpPackClient* HP_Create_SSLPackClient(ITcpClientListener* pListener)
{
	return (ITcpPackClient*)(new CSSLPackClient(pListener));
}

HPSOCKET_API void HP_Destroy_SSLServer(ITcpServer* pServer)
{
	delete pServer;
}

HPSOCKET_API void HP_Destroy_SSLAgent(ITcpAgent* pAgent)
{
	delete pAgent;
}

HPSOCKET_API void HP_Destroy_SSLClient(ITcpClient* pClient)
{
	delete pClient;
}

HPSOCKET_API void HP_Destroy_SSLPullServer(ITcpPullServer* pServer)
{
	delete pServer;
}

HPSOCKET_API void HP_Destroy_SSLPullAgent(ITcpPullAgent* pAgent)
{
	delete pAgent;
}

HPSOCKET_API void HP_Destroy_SSLPullClient(ITcpPullClient* pClient)
{
	delete pClient;
}

HPSOCKET_API void HP_Destroy_SSLPackServer(ITcpPackServer* pServer)
{
	delete pServer;
}

HPSOCKET_API void HP_Destroy_SSLPackAgent(ITcpPackAgent* pAgent)
{
	delete pAgent;
}

HPSOCKET_API void HP_Destroy_SSLPackClient(ITcpPackClient* pClient)
{
	delete pClient;
}

/*****************************************************************************************************************************************************/
/******************************************************************** HTTPS Exports ******************************************************************/
/*****************************************************************************************************************************************************/

HPSOCKET_API IHttpServer* HP_Create_HttpsServer(IHttpServerListener* pListener)
{
	return (IHttpServer*)(new CHttpsServer(pListener));
}

HPSOCKET_API IHttpAgent* HP_Create_HttpsAgent(IHttpAgentListener* pListener)
{
	return (IHttpAgent*)(new CHttpsAgent(pListener));
}

HPSOCKET_API IHttpClient* HP_Create_HttpsClient(IHttpClientListener* pListener)
{
	return (IHttpClient*)(new CHttpsClient(pListener));
}

HPSOCKET_API IHttpSyncClient* HP_Create_HttpsSyncClient(IHttpClientListener* pListener)
{
	return (IHttpSyncClient*)(new CHttpsSyncClient(pListener));
}

HPSOCKET_API void HP_Destroy_HttpsServer(IHttpServer* pServer)
{
	delete pServer;
}

HPSOCKET_API void HP_Destroy_HttpsAgent(IHttpAgent* pAgent)
{
	delete pAgent;
}

HPSOCKET_API void HP_Destroy_HttpsClient(IHttpClient* pClient)
{
	delete pClient;
}

HPSOCKET_API void HP_Destroy_HttpsSyncClient(IHttpSyncClient* pClient)
{
	delete pClient;
}

/*****************************************************************************************************************************************************/
/*************************************************************** Global Function Exports *************************************************************/
/*****************************************************************************************************************************************************/

HPSOCKET_API void __stdcall HP_SSL_RemoveThreadLocalState(DWORD dwThreadID)
{
	CSSLContext::RemoveThreadLocalState(dwThreadID);
}
