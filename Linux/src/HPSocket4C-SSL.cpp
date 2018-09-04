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
 
#include "HPSocket4C-SSL.h"

#ifdef _SSL_SUPPORT

#include "SocketObject4C.h"
#include "TcpServer.h"
#include "TcpClient.h"
#include "TcpAgent.h"
#include "TcpPullServer.h"
#include "TcpPullClient.h"
#include "TcpPullAgent.h"
#include "TcpPackServer.h"
#include "TcpPackClient.h"
#include "TcpPackAgent.h"

#ifdef _HTTP_SUPPORT
#include "HttpServer.h"
#include "HttpAgent.h"
#include "HttpClient.h"
#endif

/*****************************************************************************************************************************************************/
/******************************************************************** SSL Exports ********************************************************************/
/*****************************************************************************************************************************************************/

typedef C_HP_ObjectT<CSSLServer, ITcpServerListener>							C_HP_SSLServer;
typedef C_HP_ObjectT<CSSLPullServer, ITcpServerListener, sizeof(IPullSocket)>	C_HP_SSLPullServer;
typedef C_HP_ObjectT<CSSLPackServer, ITcpServerListener, sizeof(IPackSocket)>	C_HP_SSLPackServer;

typedef C_HP_ObjectT<CSSLAgent, ITcpAgentListener>								C_HP_SSLAgent;
typedef C_HP_ObjectT<CSSLPullAgent, ITcpAgentListener, sizeof(IPullSocket)>		C_HP_SSLPullAgent;
typedef C_HP_ObjectT<CSSLPackAgent, ITcpAgentListener, sizeof(IPackSocket)>		C_HP_SSLPackAgent;

typedef C_HP_ObjectT<CSSLClient, ITcpClientListener>							C_HP_SSLClient;
typedef C_HP_ObjectT<CSSLPullClient, ITcpClientListener, sizeof(IPullClient)>	C_HP_SSLPullClient;
typedef C_HP_ObjectT<CSSLPackClient, ITcpClientListener, sizeof(IPackClient)>	C_HP_SSLPackClient;

/********************************************************/
/************** HPSocket4C-SSL 对象创建函数 **************/

HPSOCKET_API HP_SSLServer __HP_CALL Create_HP_SSLServer(HP_TcpServerListener pListener)
{
	return (HP_SSLServer)(new C_HP_SSLServer((ITcpServerListener*)pListener));
}

HPSOCKET_API HP_SSLAgent __HP_CALL Create_HP_SSLAgent(HP_TcpAgentListener pListener)
{
	return (HP_SSLAgent)(new C_HP_SSLAgent((ITcpAgentListener*)pListener));
}

HPSOCKET_API HP_SSLClient __HP_CALL Create_HP_SSLClient(HP_TcpClientListener pListener)
{
	return (HP_SSLClient)(new C_HP_SSLClient((ITcpClientListener*)pListener));
}

HPSOCKET_API HP_SSLPullServer __HP_CALL Create_HP_SSLPullServer(HP_TcpPullServerListener pListener)
{
	return (HP_SSLPullServer)(new C_HP_SSLPullServer((ITcpServerListener*)pListener));
}

HPSOCKET_API HP_SSLPullAgent __HP_CALL Create_HP_SSLPullAgent(HP_TcpPullAgentListener pListener)
{
	return (HP_SSLPullAgent)(new C_HP_SSLPullAgent((ITcpAgentListener*)pListener));
}

HPSOCKET_API HP_SSLPullClient __HP_CALL Create_HP_SSLPullClient(HP_TcpPullClientListener pListener)
{
	return (HP_SSLPullClient)(new C_HP_SSLPullClient((ITcpClientListener*)pListener));
}

HPSOCKET_API HP_SSLPackServer __HP_CALL Create_HP_SSLPackServer(HP_TcpServerListener pListener)
{
	return (HP_SSLPackServer)(new C_HP_SSLPackServer((ITcpServerListener*)pListener));
}

HPSOCKET_API HP_SSLPackAgent __HP_CALL Create_HP_SSLPackAgent(HP_TcpAgentListener pListener)
{
	return (HP_SSLPackAgent)(new C_HP_SSLPackAgent((ITcpAgentListener*)pListener));
}

HPSOCKET_API HP_SSLPackClient __HP_CALL Create_HP_SSLPackClient(HP_TcpClientListener pListener)
{
	return (HP_SSLPackClient)(new C_HP_SSLPackClient((ITcpClientListener*)pListener));
}

HPSOCKET_API void __HP_CALL Destroy_HP_SSLServer(HP_SSLServer pServer)
{
	delete (C_HP_SSLServer*)pServer;
}

HPSOCKET_API void __HP_CALL Destroy_HP_SSLAgent(HP_SSLAgent pAgent)
{
	delete (C_HP_SSLAgent*)pAgent;
}

HPSOCKET_API void __HP_CALL Destroy_HP_SSLClient(HP_SSLClient pClient)
{
	delete (C_HP_SSLClient*)pClient;
}

HPSOCKET_API void __HP_CALL Destroy_HP_SSLPullServer(HP_SSLPullServer pServer)
{
	delete (C_HP_SSLPullServer*)pServer;
}

HPSOCKET_API void __HP_CALL Destroy_HP_SSLPullAgent(HP_SSLPullAgent pAgent)
{
	delete (C_HP_SSLPullAgent*)pAgent;
}

HPSOCKET_API void __HP_CALL Destroy_HP_SSLPullClient(HP_SSLPullClient pClient)
{
	delete (C_HP_SSLPullClient*)pClient;
}

HPSOCKET_API void __HP_CALL Destroy_HP_SSLPackServer(HP_SSLPackServer pServer)
{
	delete (C_HP_SSLPackServer*)pServer;
}

HPSOCKET_API void __HP_CALL Destroy_HP_SSLPackAgent(HP_SSLPackAgent pAgent)
{
	delete (C_HP_SSLPackAgent*)pAgent;
}

HPSOCKET_API void __HP_CALL Destroy_HP_SSLPackClient(HP_SSLPackClient pClient)
{
	delete (C_HP_SSLPackClient*)pClient;
}

/*****************************************************************************************************************************************************/
/*************************************************************** Global Function Exports *************************************************************/
/*****************************************************************************************************************************************************/

HPSOCKET_API void __HP_CALL HP_SSL_RemoveThreadLocalState(THR_ID dwThreadID)
{
	CSSLContext::RemoveThreadLocalState(dwThreadID);
}

HPSOCKET_API BOOL __HP_CALL HP_SSLServer_SetupSSLContext(HP_SSLServer pServer, int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath, HP_Fn_SNI_ServerNameCallback fnServerNameCallback)
{
	return C_HP_Object::ToSecond<ITcpServer>(pServer)->SetupSSLContext(iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath, fnServerNameCallback);
}

HPSOCKET_API int __HP_CALL HP_SSLServer_AddSSLContext(HP_SSLServer pServer, int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath)
{
	return C_HP_Object::ToSecond<ITcpServer>(pServer)->AddSSLContext(iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath);
}

HPSOCKET_API void __HP_CALL HP_SSLServer_CleanupSSLContext(HP_SSLServer pServer)
{
	C_HP_Object::ToSecond<ITcpServer>(pServer)->CleanupSSLContext();
}

HPSOCKET_API BOOL __HP_CALL HP_SSLAgent_SetupSSLContext(HP_SSLAgent pAgent, int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath)
{
	return C_HP_Object::ToSecond<ITcpAgent>(pAgent)->SetupSSLContext(iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath);
}

HPSOCKET_API void __HP_CALL HP_SSLAgent_CleanupSSLContext(HP_SSLAgent pAgent)
{
	C_HP_Object::ToSecond<ITcpAgent>(pAgent)->CleanupSSLContext();
}

HPSOCKET_API BOOL __HP_CALL HP_SSLClient_SetupSSLContext(HP_SSLClient pClient, int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath)
{
	return C_HP_Object::ToSecond<ITcpClient>(pClient)->SetupSSLContext(iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath);
}

HPSOCKET_API void __HP_CALL HP_SSLClient_CleanupSSLContext(HP_SSLClient pClient)
{
	C_HP_Object::ToSecond<ITcpClient>(pClient)->CleanupSSLContext();
}

/*****************************************************************************************************************************************************/
/******************************************************************** HTTPS Exports ******************************************************************/
/*****************************************************************************************************************************************************/

#ifdef _HTTP_SUPPORT

typedef C_HP_ObjectT<CHttpsServer, IHttpServerListener, sizeof(IComplexHttpResponder)>		C_HP_HttpsServer;
typedef C_HP_ObjectT<CHttpsAgent, IHttpAgentListener, sizeof(IComplexHttpRequester)>		C_HP_HttpsAgent;
typedef C_HP_ObjectT<CHttpsClient, IHttpClientListener, sizeof(IHttpRequester)>				C_HP_HttpsClient;
typedef C_HP_ObjectT<CHttpsSyncClient, IHttpClientListener, sizeof(IHttpSyncRequester)>		C_HP_HttpsSyncClient;

/****************************************************/
/**************** HTTPS 对象创建函数 *****************/

HPSOCKET_API HP_HttpsServer __HP_CALL Create_HP_HttpsServer(HP_HttpServerListener pListener)
{
	return (HP_HttpsServer)(new C_HP_HttpsServer((IHttpServerListener*)pListener));
}

HPSOCKET_API HP_HttpsAgent __HP_CALL Create_HP_HttpsAgent(HP_HttpAgentListener pListener)
{
	return (HP_HttpsAgent)(new C_HP_HttpsAgent((IHttpAgentListener*)pListener));
}

HPSOCKET_API HP_HttpsClient __HP_CALL Create_HP_HttpsClient(HP_HttpClientListener pListener)
{
	return (HP_HttpsClient)(new C_HP_HttpsClient((IHttpClientListener*)pListener));
}

HPSOCKET_API HP_HttpsSyncClient __HP_CALL Create_HP_HttpsSyncClient(HP_HttpClientListener pListener)
{
	return (HP_HttpsSyncClient)(new C_HP_HttpsSyncClient((IHttpClientListener*)pListener));
}

HPSOCKET_API void __HP_CALL Destroy_HP_HttpsServer(HP_HttpsServer pServer)
{
	delete (C_HP_HttpsServer*)pServer;
}

HPSOCKET_API void __HP_CALL Destroy_HP_HttpsAgent(HP_HttpsAgent pAgent)
{
	delete (C_HP_HttpsAgent*)pAgent;
}

HPSOCKET_API void __HP_CALL Destroy_HP_HttpsClient(HP_HttpsClient pClient)
{
	delete (C_HP_HttpsClient*)pClient;
}

HPSOCKET_API void __HP_CALL Destroy_HP_HttpsSyncClient(HP_HttpsSyncClient pClient)
{
	delete (C_HP_HttpsSyncClient*)pClient;
}

#endif

#endif