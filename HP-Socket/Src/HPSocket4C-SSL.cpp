/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.5.2
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

	#pragma comment(linker, "/EXPORT:HP_SSL_Initialize=_HP_SSL_Initialize@24")
	#pragma comment(linker, "/EXPORT:HP_SSL_Cleanup=_HP_SSL_Cleanup@0")
	#pragma comment(linker, "/EXPORT:HP_SSL_RemoveThreadLocalState=_HP_SSL_RemoveThreadLocalState@0")
	#pragma comment(linker, "/EXPORT:HP_SSL_IsValid=_HP_SSL_IsValid@0")

#endif

class C_HP_SSLServer : public C_HP_Object, public CSSLServer
{
public:
	C_HP_SSLServer(ITcpServerListener* pListener) : CSSLServer(pListener) {}
};

class C_HP_SSLPullServer : public C_HP_Object, public CSSLPullServer
{
public:
	C_HP_SSLPullServer(ITcpServerListener* pListener) : C_HP_Object(sizeof(IPullSocket)), CSSLPullServer(pListener) {}
};

class C_HP_SSLPackServer : public C_HP_Object, public CSSLPackServer
{
public:
	C_HP_SSLPackServer(ITcpServerListener* pListener) : C_HP_Object(sizeof(IPackSocket)), CSSLPackServer(pListener) {}
};

class C_HP_SSLAgent : public C_HP_Object, public CSSLAgent
{
public:
	C_HP_SSLAgent(ITcpAgentListener* pListener) : CSSLAgent(pListener) {}
};

class C_HP_SSLPullAgent : public C_HP_Object, public CSSLPullAgent
{
public:
	C_HP_SSLPullAgent(ITcpAgentListener* pListener) : C_HP_Object(sizeof(IPullSocket)), CSSLPullAgent(pListener) {}
};

class C_HP_SSLPackAgent : public C_HP_Object, public CSSLPackAgent
{
public:
	C_HP_SSLPackAgent(ITcpAgentListener* pListener) : C_HP_Object(sizeof(IPackSocket)), CSSLPackAgent(pListener) {}
};

class C_HP_SSLClient : public C_HP_Object, public CSSLClient
{
public:
	C_HP_SSLClient(ITcpClientListener* pListener) : CSSLClient(pListener) {}
};

class C_HP_SSLPullClient : public C_HP_Object, public CSSLPullClient
{
public:
	C_HP_SSLPullClient(ITcpClientListener* pListener) : C_HP_Object(sizeof(IPullClient)), CSSLPullClient(pListener) {}
};

class C_HP_SSLPackClient : public C_HP_Object, public CSSLPackClient
{
public:
	C_HP_SSLPackClient(ITcpClientListener* pListener) : C_HP_Object(sizeof(IPackClient)), CSSLPackClient(pListener) {}
};

/********************************************************/
/**************** HPSocket4C-SSL 导出函数 ****************/

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

/***************************************************************************************/
/************************************ SSL 初始化方法 ************************************/

HPSOCKET_API BOOL __stdcall HP_SSL_Initialize(En_HP_SSLSessionMode enSessionMode, int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath)
{
	return g_SSL.Initialize((EnSSLSessionMode)enSessionMode, iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath);
}

HPSOCKET_API void __stdcall HP_SSL_Cleanup()
{
	g_SSL.Cleanup();
}

HPSOCKET_API void __stdcall HP_SSL_RemoveThreadLocalState()
{
	g_SSL.RemoveThreadLocalState();
}

HPSOCKET_API BOOL __stdcall HP_SSL_IsValid()
{
	return g_SSL.IsValid();
}
