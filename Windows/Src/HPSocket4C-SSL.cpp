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
#include "HPSocket4C-SSL.h"
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

#include "HttpServer.h"
#include "HttpAgent.h"
#include "HttpClient.h"

#if !defined(_WIN64) && !defined(HPSOCKET_STATIC_LIB)

	#pragma comment(linker, "/EXPORT:Create_HP_SSLAgent=_Create_HP_SSLAgent@4")
	#pragma comment(linker, "/EXPORT:Create_HP_SSLClient=_Create_HP_SSLClient@4")
	#pragma comment(linker, "/EXPORT:Create_HP_SSLPullAgent=_Create_HP_SSLPullAgent@4")
	#pragma comment(linker, "/EXPORT:Create_HP_SSLPullClient=_Create_HP_SSLPullClient@4")
	#pragma comment(linker, "/EXPORT:Create_HP_SSLPullServer=_Create_HP_SSLPullServer@4")
	#pragma comment(linker, "/EXPORT:Create_HP_SSLPackAgent=_Create_HP_SSLPackAgent@4")
	#pragma comment(linker, "/EXPORT:Create_HP_SSLPackClient=_Create_HP_SSLPackClient@4")
	#pragma comment(linker, "/EXPORT:Create_HP_SSLPackServer=_Create_HP_SSLPackServer@4")
	#pragma comment(linker, "/EXPORT:Create_HP_SSLServer=_Create_HP_SSLServer@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_SSLAgent=_Destroy_HP_SSLAgent@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_SSLClient=_Destroy_HP_SSLClient@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_SSLPullAgent=_Destroy_HP_SSLPullAgent@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_SSLPullClient=_Destroy_HP_SSLPullClient@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_SSLPullServer=_Destroy_HP_SSLPullServer@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_SSLPackAgent=_Destroy_HP_SSLPackAgent@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_SSLPackClient=_Destroy_HP_SSLPackClient@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_SSLPackServer=_Destroy_HP_SSLPackServer@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_SSLServer=_Destroy_HP_SSLServer@4")

	#pragma comment(linker, "/EXPORT:Create_HP_HttpsAgent=_Create_HP_HttpsAgent@4")
	#pragma comment(linker, "/EXPORT:Create_HP_HttpsClient=_Create_HP_HttpsClient@4")
	#pragma comment(linker, "/EXPORT:Create_HP_HttpsServer=_Create_HP_HttpsServer@4")
	#pragma comment(linker, "/EXPORT:Create_HP_HttpsSyncClient=_Create_HP_HttpsSyncClient@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_HttpsAgent=_Destroy_HP_HttpsAgent@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_HttpsClient=_Destroy_HP_HttpsClient@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_HttpsServer=_Destroy_HP_HttpsServer@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_HttpsSyncClient=_Destroy_HP_HttpsSyncClient@4")

	#pragma comment(linker, "/EXPORT:HP_SSL_RemoveThreadLocalState=_HP_SSL_RemoveThreadLocalState@4")
	#pragma comment(linker, "/EXPORT:HP_SSLAgent_SetupSSLContext=_HP_SSLAgent_SetupSSLContext@24")
	#pragma comment(linker, "/EXPORT:HP_SSLAgent_CleanupSSLContext=_HP_SSLAgent_CleanupSSLContext@4")
	#pragma comment(linker, "/EXPORT:HP_SSLClient_SetupSSLContext=_HP_SSLClient_SetupSSLContext@24")
	#pragma comment(linker, "/EXPORT:HP_SSLClient_CleanupSSLContext=_HP_SSLClient_CleanupSSLContext@4")
	#pragma comment(linker, "/EXPORT:HP_SSLServer_SetupSSLContext=_HP_SSLServer_SetupSSLContext@28")
	#pragma comment(linker, "/EXPORT:HP_SSLServer_AddSSLContext=_HP_SSLServer_AddSSLContext@24")
	#pragma comment(linker, "/EXPORT:HP_SSLServer_CleanupSSLContext=_HP_SSLServer_CleanupSSLContext@4")

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

HPSOCKET_API HP_SSLServer __stdcall Create_HP_SSLServer(HP_TcpServerListener pListener)
{
	return (HP_SSLServer)(new C_HP_SSLServer((ITcpServerListener*)pListener));
}

HPSOCKET_API HP_SSLAgent __stdcall Create_HP_SSLAgent(HP_TcpAgentListener pListener)
{
	return (HP_SSLAgent)(new C_HP_SSLAgent((ITcpAgentListener*)pListener));
}

HPSOCKET_API HP_SSLClient __stdcall Create_HP_SSLClient(HP_TcpClientListener pListener)
{
	return (HP_SSLClient)(new C_HP_SSLClient((ITcpClientListener*)pListener));
}

HPSOCKET_API HP_SSLPullServer __stdcall Create_HP_SSLPullServer(HP_TcpPullServerListener pListener)
{
	return (HP_SSLPullServer)(new C_HP_SSLPullServer((ITcpServerListener*)pListener));
}

HPSOCKET_API HP_SSLPullAgent __stdcall Create_HP_SSLPullAgent(HP_TcpPullAgentListener pListener)
{
	return (HP_SSLPullAgent)(new C_HP_SSLPullAgent((ITcpAgentListener*)pListener));
}

HPSOCKET_API HP_SSLPullClient __stdcall Create_HP_SSLPullClient(HP_TcpPullClientListener pListener)
{
	return (HP_SSLPullClient)(new C_HP_SSLPullClient((ITcpClientListener*)pListener));
}

HPSOCKET_API HP_SSLPackServer __stdcall Create_HP_SSLPackServer(HP_TcpServerListener pListener)
{
	return (HP_SSLPackServer)(new C_HP_SSLPackServer((ITcpServerListener*)pListener));
}

HPSOCKET_API HP_SSLPackAgent __stdcall Create_HP_SSLPackAgent(HP_TcpAgentListener pListener)
{
	return (HP_SSLPackAgent)(new C_HP_SSLPackAgent((ITcpAgentListener*)pListener));
}

HPSOCKET_API HP_SSLPackClient __stdcall Create_HP_SSLPackClient(HP_TcpClientListener pListener)
{
	return (HP_SSLPackClient)(new C_HP_SSLPackClient((ITcpClientListener*)pListener));
}

HPSOCKET_API void __stdcall Destroy_HP_SSLServer(HP_SSLServer pServer)
{
	delete (C_HP_SSLServer*)pServer;
}

HPSOCKET_API void __stdcall Destroy_HP_SSLAgent(HP_SSLAgent pAgent)
{
	delete (C_HP_SSLAgent*)pAgent;
}

HPSOCKET_API void __stdcall Destroy_HP_SSLClient(HP_SSLClient pClient)
{
	delete (C_HP_SSLClient*)pClient;
}

HPSOCKET_API void __stdcall Destroy_HP_SSLPullServer(HP_SSLPullServer pServer)
{
	delete (C_HP_SSLPullServer*)pServer;
}

HPSOCKET_API void __stdcall Destroy_HP_SSLPullAgent(HP_SSLPullAgent pAgent)
{
	delete (C_HP_SSLPullAgent*)pAgent;
}

HPSOCKET_API void __stdcall Destroy_HP_SSLPullClient(HP_SSLPullClient pClient)
{
	delete (C_HP_SSLPullClient*)pClient;
}

HPSOCKET_API void __stdcall Destroy_HP_SSLPackServer(HP_SSLPackServer pServer)
{
	delete (C_HP_SSLPackServer*)pServer;
}

HPSOCKET_API void __stdcall Destroy_HP_SSLPackAgent(HP_SSLPackAgent pAgent)
{
	delete (C_HP_SSLPackAgent*)pAgent;
}

HPSOCKET_API void __stdcall Destroy_HP_SSLPackClient(HP_SSLPackClient pClient)
{
	delete (C_HP_SSLPackClient*)pClient;
}

/*****************************************************************************************************************************************************/
/******************************************************************** HTTPS Exports ******************************************************************/
/*****************************************************************************************************************************************************/

typedef C_HP_ObjectT<CHttpsServer, IHttpServerListener, sizeof(IComplexHttpResponder)>		C_HP_HttpsServer;
typedef C_HP_ObjectT<CHttpsAgent, IHttpAgentListener, sizeof(IComplexHttpRequester)>		C_HP_HttpsAgent;
typedef C_HP_ObjectT<CHttpsClient, IHttpClientListener, sizeof(IHttpRequester)>				C_HP_HttpsClient;
typedef C_HP_ObjectT<CHttpsSyncClient, IHttpClientListener, sizeof(IHttpSyncRequester)>		C_HP_HttpsSyncClient;

/****************************************************/
/**************** HTTPS 对象创建函数 *****************/

HPSOCKET_API HP_HttpsServer __stdcall Create_HP_HttpsServer(HP_HttpServerListener pListener)
{
	return (HP_HttpsServer)(new C_HP_HttpsServer((IHttpServerListener*)pListener));
}

HPSOCKET_API HP_HttpsAgent __stdcall Create_HP_HttpsAgent(HP_HttpAgentListener pListener)
{
	return (HP_HttpsAgent)(new C_HP_HttpsAgent((IHttpAgentListener*)pListener));
}

HPSOCKET_API HP_HttpsClient __stdcall Create_HP_HttpsClient(HP_HttpClientListener pListener)
{
	return (HP_HttpsClient)(new C_HP_HttpsClient((IHttpClientListener*)pListener));
}

HPSOCKET_API HP_HttpsSyncClient __stdcall Create_HP_HttpsSyncClient(HP_HttpClientListener pListener)
{
	return (HP_HttpsSyncClient)(new C_HP_HttpsSyncClient((IHttpClientListener*)pListener));
}

HPSOCKET_API void __stdcall Destroy_HP_HttpsServer(HP_HttpsServer pServer)
{
	delete (C_HP_HttpsServer*)pServer;
}

HPSOCKET_API void __stdcall Destroy_HP_HttpsAgent(HP_HttpsAgent pAgent)
{
	delete (C_HP_HttpsAgent*)pAgent;
}

HPSOCKET_API void __stdcall Destroy_HP_HttpsClient(HP_HttpsClient pClient)
{
	delete (C_HP_HttpsClient*)pClient;
}

HPSOCKET_API void __stdcall Destroy_HP_HttpsSyncClient(HP_HttpsSyncClient pClient)
{
	delete (C_HP_HttpsSyncClient*)pClient;
}

/*****************************************************************************************************************************************************/
/*************************************************************** Global Function Exports *************************************************************/
/*****************************************************************************************************************************************************/

HPSOCKET_API void __stdcall HP_SSL_RemoveThreadLocalState(DWORD dwThreadID)
{
	CSSLContext::RemoveThreadLocalState(dwThreadID);
}

HPSOCKET_API BOOL __stdcall HP_SSLServer_SetupSSLContext(HP_SSLServer pServer, int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath, HP_Fn_SNI_ServerNameCallback fnServerNameCallback)
{
	return C_HP_Object::ToSecond<ITcpServer>(pServer)->SetupSSLContext(iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath, fnServerNameCallback);
}

HPSOCKET_API int __stdcall HP_SSLServer_AddSSLContext(HP_SSLServer pServer, int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath)
{
	return C_HP_Object::ToSecond<ITcpServer>(pServer)->AddSSLContext(iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath);
}

HPSOCKET_API void __stdcall HP_SSLServer_CleanupSSLContext(HP_SSLServer pServer)
{
	C_HP_Object::ToSecond<ITcpServer>(pServer)->CleanupSSLContext();
}

HPSOCKET_API BOOL __stdcall HP_SSLAgent_SetupSSLContext(HP_SSLAgent pAgent, int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath)
{
	return C_HP_Object::ToSecond<ITcpAgent>(pAgent)->SetupSSLContext(iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath);
}

HPSOCKET_API void __stdcall HP_SSLAgent_CleanupSSLContext(HP_SSLAgent pAgent)
{
	C_HP_Object::ToSecond<ITcpAgent>(pAgent)->CleanupSSLContext();
}

HPSOCKET_API BOOL __stdcall HP_SSLClient_SetupSSLContext(HP_SSLClient pClient, int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath)
{
	return C_HP_Object::ToSecond<ITcpClient>(pClient)->SetupSSLContext(iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath);
}

HPSOCKET_API void __stdcall HP_SSLClient_CleanupSSLContext(HP_SSLClient pClient)
{
	C_HP_Object::ToSecond<ITcpClient>(pClient)->CleanupSSLContext();
}
