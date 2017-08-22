/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 5.0.1
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
Module:  HPSocket-SSL

Usage:
		������
		--------------------------------------------------------------------------------------
		0. Ӧ�ó������ HPTypeDef.h / SocketInterface.h / HPSocket.h / HPSocket-SSL.h ͷ�ļ�
		1. ÿ������ͨ��ǰ���� SetupSSLContext() ������ʼ�� SSL ������������ȷ���������سɹ�
		2. ʹ�� HPSocket ���ִ��ͨ�ţ��ο���HPSocket.h��

Release:
		<-- ��̬���ӿ� -->
		1. x86/HPSocket-SSL.dll				- (32λ/MBCS/Release)
		2. x86/HPSocket-SSL_D.dll			- (32λ/MBCS/DeBug)
		3. x86/HPSocket-SSL_U.dll			- (32λ/UNICODE/Release)
		4. x86/HPSocket-SSL_UD.dll			- (32λ/UNICODE/DeBug)
		5. x64/HPSocket-SSL.dll				- (64λ/MBCS/Release)
		6. x64/HPSocket-SSL_D.dll			- (64λ/MBCS/DeBug)
		7. x64/HPSocket-SSL_U.dll			- (64λ/UNICODE/Release)
		8. x64/HPSocket-SSL_UD.dll			- (64λ/UNICODE/DeBug)

		<-- ��̬���ӿ� -->
		!!ע��!!��ʹ�� HPSocket ��̬��ʱ����Ҫ�ڹ��������ж���Ԥ����� -> HPSOCKET_STATIC_LIB
		1. x86/static/HPSocket-SSL.lib		- (32λ/MBCS/Release)
		2. x86/static/HPSocket-SSL_D.lib	- (32λ/MBCS/DeBug)
		3. x86/static/HPSocket-SSL_U.lib	- (32λ/UNICODE/Release)
		4. x86/static/HPSocket-SSL_UD.lib	- (32λ/UNICODE/DeBug)
		5. x64/static/HPSocket-SSL.lib		- (64λ/MBCS/Release)
		6. x64/static/HPSocket-SSL_D.lib	- (64λ/MBCS/DeBug)
		7. x64/static/HPSocket-SSL_U.lib	- (64λ/UNICODE/Release)
		8. x64/static/HPSocket-SSL_UD.lib	- (64λ/UNICODE/DeBug)

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
/************** HPSocket-SSL �������� **************/

// ���� SSL ITcpServer ����
HPSOCKET_API ITcpServer* HP_Create_SSLServer(ITcpServerListener* pListener);
// ���� SSL ITcpAgent ����
HPSOCKET_API ITcpAgent* HP_Create_SSLAgent(ITcpAgentListener* pListener);
// ���� SSL ITcpClient ����
HPSOCKET_API ITcpClient* HP_Create_SSLClient(ITcpClientListener* pListener);
// ���� SSL ITcpPullServer ����
HPSOCKET_API ITcpPullServer* HP_Create_SSLPullServer(ITcpServerListener* pListener);
// ���� SSL ITcpPullAgent ����
HPSOCKET_API ITcpPullAgent* HP_Create_SSLPullAgent(ITcpAgentListener* pListener);
// ���� SSL ITcpPullClient ����
HPSOCKET_API ITcpPullClient* HP_Create_SSLPullClient(ITcpClientListener* pListener);
// ���� SSL ITcpPackServer ����
HPSOCKET_API ITcpPackServer* HP_Create_SSLPackServer(ITcpServerListener* pListener);
// ���� SSL ITcpPackAgent ����
HPSOCKET_API ITcpPackAgent* HP_Create_SSLPackAgent(ITcpAgentListener* pListener);
// ���� SSL ITcpPackClient ����
HPSOCKET_API ITcpPackClient* HP_Create_SSLPackClient(ITcpClientListener* pListener);

// ���� SSL ITcpServer ����
HPSOCKET_API void HP_Destroy_SSLServer(ITcpServer* pServer);
// ���� SSL ITcpAgent ����
HPSOCKET_API void HP_Destroy_SSLAgent(ITcpAgent* pAgent);
// ���� SSL ITcpClient ����
HPSOCKET_API void HP_Destroy_SSLClient(ITcpClient* pClient);
// ���� SSL ITcpPullServer ����
HPSOCKET_API void HP_Destroy_SSLPullServer(ITcpPullServer* pServer);
// ���� SSL ITcpPullAgent ����
HPSOCKET_API void HP_Destroy_SSLPullAgent(ITcpPullAgent* pAgent);
// ���� SSL ITcpPullClient ����
HPSOCKET_API void HP_Destroy_SSLPullClient(ITcpPullClient* pClient);
// ���� SSL ITcpPackServer ����
HPSOCKET_API void HP_Destroy_SSLPackServer(ITcpPackServer* pServer);
// ���� SSL ITcpPackAgent ����
HPSOCKET_API void HP_Destroy_SSLPackAgent(ITcpPackAgent* pAgent);
// ���� SSL ITcpPackClient ����
HPSOCKET_API void HP_Destroy_SSLPackClient(ITcpPackClient* pClient);

// SSL ITcpServer ���󴴽���
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

// SSL ITcpAgent ���󴴽���
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

// SSL ITcpClient ���󴴽���
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

// SSL ITcpPullServer ���󴴽���
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

// SSL ITcpPullAgent ���󴴽���
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

// SSL ITcpPullClient ���󴴽���
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

// SSL ITcpPackServer ���󴴽���
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

// SSL ITcpPackAgent ���󴴽���
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

// SSL ITcpPackClient ���󴴽���
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

// SSL ITcpServer ��������ָ��
typedef CHPSocketPtr<ITcpServer, ITcpServerListener, SSLServer_Creator>			CSSLServerPtr;
// SSL ITcpAgent ��������ָ��
typedef CHPSocketPtr<ITcpAgent, ITcpAgentListener, SSLAgent_Creator>			CSSLAgentPtr;
// SSL ITcpClient ��������ָ��
typedef CHPSocketPtr<ITcpClient, ITcpClientListener, SSLClient_Creator>			CSSLClientPtr;
// SSL ITcpPullServer ��������ָ��
typedef CHPSocketPtr<ITcpPullServer, ITcpServerListener, SSLPullServer_Creator>	CSSLPullServerPtr;
// SSL ITcpPullAgent ��������ָ��
typedef CHPSocketPtr<ITcpPullAgent, ITcpAgentListener, SSLPullAgent_Creator>	CSSLPullAgentPtr;
// SSL ITcpPullClient ��������ָ��
typedef CHPSocketPtr<ITcpPullClient, ITcpClientListener, SSLPullClient_Creator>	CSSLPullClientPtr;
// SSL ITcpPackServer ��������ָ��
typedef CHPSocketPtr<ITcpPackServer, ITcpServerListener, SSLPackServer_Creator>	CSSLPackServerPtr;
// SSL ITcpPackAgent ��������ָ��
typedef CHPSocketPtr<ITcpPackAgent, ITcpAgentListener, SSLPackAgent_Creator>	CSSLPackAgentPtr;
// SSL ITcpPackClient ��������ָ��
typedef CHPSocketPtr<ITcpPackClient, ITcpClientListener, SSLPackClient_Creator>	CSSLPackClientPtr;

/*****************************************************************************************************************************************************/
/******************************************************************** HTTPS Exports ******************************************************************/
/*****************************************************************************************************************************************************/

// ���� IHttpServer ����
HPSOCKET_API IHttpServer* HP_Create_HttpsServer(IHttpServerListener* pListener);
// ���� IHttpAgent ����
HPSOCKET_API IHttpAgent* HP_Create_HttpsAgent(IHttpAgentListener* pListener);
// ���� IHttpClient ����
HPSOCKET_API IHttpClient* HP_Create_HttpsClient(IHttpClientListener* pListener);
// ���� IHttpSyncClient ����
HPSOCKET_API IHttpSyncClient* HP_Create_HttpsSyncClient(IHttpClientListener* pListener);

// ���� IHttpServer ����
HPSOCKET_API void HP_Destroy_HttpsServer(IHttpServer* pServer);
// ���� IHttpAgent ����
HPSOCKET_API void HP_Destroy_HttpsAgent(IHttpAgent* pAgent);
// ���� IHttpClient ����
HPSOCKET_API void HP_Destroy_HttpsClient(IHttpClient* pClient);
// ���� IHttpSyncClient ����
HPSOCKET_API void HP_Destroy_HttpsSyncClient(IHttpSyncClient* pClient);

// IHttpServer ���󴴽���
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

// IHttpAgent ���󴴽���
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

// IHttpClient ���󴴽���
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

// IHttpSyncClient ���󴴽���
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

// IHttpServer ��������ָ��
typedef CHPSocketPtr<IHttpServer, IHttpServerListener, HttpsServer_Creator>		CHttpsServerPtr;
// IHttpAgent ��������ָ��
typedef CHPSocketPtr<IHttpAgent, IHttpAgentListener, HttpsAgent_Creator>		CHttpsAgentPtr;
// IHttpClient ��������ָ��
typedef CHPSocketPtr<IHttpClient, IHttpClientListener, HttpsClient_Creator>		CHttpsClientPtr;
// IHttpSyncClient ��������ָ��
typedef CHPSocketPtr<IHttpSyncClient, BOOL, HttpsSyncClient_Creator>			CHttpsSyncClientPtr;

/*****************************************************************************************************************************************************/
/*************************************************************** Global Function Exports *************************************************************/
/*****************************************************************************************************************************************************/

/*
* ���ƣ������ֲ߳̾����� SSL ��Դ
* �������κ�һ������ SSL ���̣߳�ͨ�Ž���ʱ����Ҫ�����ֲ߳̾����� SSL ��Դ
*		1�����̺߳� HP-Socket �����߳���ͨ�Ž���ʱ���Զ������ֲ߳̾����� SSL ��Դ����ˣ�һ������²����ֹ����ñ�����
*		2����������£����Զ����̲߳��� HP-Socket ͨ�Ų�������鵽 SSL �ڴ�й©ʱ������ÿ��ͨ�Ž���ʱ�Զ����̵߳��ñ�����
*		
* ������		dwThreadID	-- �߳� ID��0����ǰ�̣߳�
* 
* ����ֵ����
*/
HPSOCKET_API void __stdcall HP_SSL_RemoveThreadLocalState(DWORD dwThreadID);
