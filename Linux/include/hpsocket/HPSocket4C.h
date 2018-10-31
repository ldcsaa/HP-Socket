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
Module:  HPSocket for C

Desc: ������ C ���������������ԣ��磺C / C# / Java �ȣ��ܷ����ʹ�� HPSocket

Usage:
		����һ��
		--------------------------------------------------------------------------------------
		0. ��C/C++ ���򣩰��� HPTypeDef.h / HPSocket4C.h ͷ�ļ�
		1. ���� ::Create_HP_XxxListener() ������������������
		2. ���� ::Create_HP_Xxx(pListener) �������� HPSocket ����
		3. ���� ::HP_Set_FN_Xxx_OnYyy(pListener, ...) �������ü������Ļص�����
		4. ������ص����������� HPSocket ����
		5. ...... ......
		6. ���� ::Destroy_HP_Xxx(pSocket) �������� HPSocket ����
		7. ���� ::Destroy_HP_XxxListener(pListener) �������ټ���������

		��������
		--------------------------------------------------------------------------------------
		1. Ӧ�ó������Ҫ�õ��ĵ���������װ���ض����Եİ�װ����
		2. ͨ����װ���װ�����������ķ�ʽʹ�� HPSocket

Release:
		<-- ��̬���ӿ� -->
		1. x86/libhpsocket4c.so			- (32λ/MBCS/Release)
		2. x86/libhpsocket4c_d.so		- (32λ/MBCS/DeBug)
		3. x64/libhpsocket4c.so			- (64λ/MBCS/Release)
		4. x64/libhpsocket4c_d.so		- (64λ/MBCS/DeBug)

		<-- ��̬���ӿ� -->
		!!ע��!!��ʹ�� HPSocket ��̬��ʱ����Ҫ�ڹ��������ж���Ԥ����� -> HPSOCKET_STATIC_LIB
		1. x86/static/libhpsocket4c.a	- (32λ/MBCS/Release)
		2. x86/static/libhpsocket4c_d.a	- (32λ/MBCS/DeBug)
		3. x64/static/libhpsocket4c.a	- (64λ/MBCS/Release)
		4. x64/static/libhpsocket4c_d.a	- (64λ/MBCS/DeBug)

******************************************************************************/

#pragma once

#include "HPTypeDef.h"

/************************************************************************
���ƣ����� Socket ����ָ�����ͱ���
�������� Socket ����ָ�붨��Ϊ��ֱ�۵ı���
************************************************************************/

typedef PVOID		HP_Object;

typedef HP_Object	HP_Server;
typedef HP_Object	HP_Agent;
typedef HP_Object	HP_Client;
typedef HP_Object	HP_TcpServer;
typedef HP_Object	HP_TcpAgent;
typedef HP_Object	HP_TcpClient;
typedef HP_Object	HP_PullSocket;
typedef HP_Object	HP_PullClient;
typedef HP_Object	HP_TcpPullServer;
typedef HP_Object	HP_TcpPullAgent;
typedef HP_Object	HP_TcpPullClient;
typedef HP_Object	HP_PackSocket;
typedef HP_Object	HP_PackClient;
typedef HP_Object	HP_TcpPackServer;
typedef HP_Object	HP_TcpPackAgent;
typedef HP_Object	HP_TcpPackClient;
typedef HP_Object	HP_UdpServer;
typedef HP_Object	HP_UdpClient;
typedef HP_Object	HP_UdpCast;

typedef HP_Object	HP_Listener;
typedef HP_Object	HP_ServerListener;
typedef HP_Object	HP_AgentListener;
typedef HP_Object	HP_ClientListener;
typedef HP_Object	HP_TcpServerListener;
typedef HP_Object	HP_TcpAgentListener;
typedef HP_Object	HP_TcpClientListener;
typedef HP_Object	HP_PullSocketListener;
typedef HP_Object	HP_PullClientListener;
typedef HP_Object	HP_TcpPullServerListener;
typedef HP_Object	HP_TcpPullAgentListener;
typedef HP_Object	HP_TcpPullClientListener;
typedef HP_Object	HP_PackSocketListener;
typedef HP_Object	HP_PackClientListener;
typedef HP_Object	HP_TcpPackServerListener;
typedef HP_Object	HP_TcpPackAgentListener;
typedef HP_Object	HP_TcpPackClientListener;
typedef HP_Object	HP_UdpServerListener;
typedef HP_Object	HP_UdpClientListener;
typedef HP_Object	HP_UdpCastListener;

typedef HP_Object	HP_Http;
typedef HP_Object	HP_HttpServer;
typedef HP_Object	HP_HttpAgent;
typedef HP_Object	HP_HttpClient;
typedef HP_Object	HP_HttpSyncClient;

typedef HP_Object	HP_HttpServerListener;
typedef HP_Object	HP_HttpAgentListener;
typedef HP_Object	HP_HttpClientListener;

typedef HP_Object	HP_ThreadPool;

/*****************************************************************************************************************************************************/
/****************************************************************** TCP/UDP Exports ******************************************************************/
/*****************************************************************************************************************************************************/

/****************************************************/
/***************** TCP/UDP �ص����� ******************/

/* Server �ص����� */
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Server_OnPrepareListen)	(HP_Server pSender, UINT_PTR soListen);
// ���Ϊ TCP ���ӣ�pClientΪ SOCKET ��������Ϊ UDP ���ӣ�pClientΪ SOCKADDR ָ�룻
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Server_OnAccept)			(HP_Server pSender, HP_CONNID dwConnID, UINT_PTR pClient);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Server_OnHandShake)		(HP_Server pSender, HP_CONNID dwConnID);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Server_OnSend)				(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Server_OnReceive)			(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Server_OnPullReceive)		(HP_Server pSender, HP_CONNID dwConnID, int iLength);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Server_OnClose)			(HP_Server pSender, HP_CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Server_OnShutdown)			(HP_Server pSender);

/* Agent �ص����� */
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Agent_OnPrepareConnect)	(HP_Agent pSender, HP_CONNID dwConnID, UINT_PTR socket);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Agent_OnConnect)			(HP_Agent pSender, HP_CONNID dwConnID);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Agent_OnHandShake)			(HP_Agent pSender, HP_CONNID dwConnID);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Agent_OnSend)				(HP_Agent pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Agent_OnReceive)			(HP_Agent pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Agent_OnPullReceive)		(HP_Agent pSender, HP_CONNID dwConnID, int iLength);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Agent_OnClose)				(HP_Agent pSender, HP_CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Agent_OnShutdown)			(HP_Agent pSender);

/* Client �ص����� */
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Client_OnPrepareConnect)	(HP_Client pSender, HP_CONNID dwConnID, UINT_PTR socket);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Client_OnConnect)			(HP_Client pSender, HP_CONNID dwConnID);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Client_OnHandShake)		(HP_Client pSender, HP_CONNID dwConnID);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Client_OnSend)				(HP_Client pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Client_OnReceive)			(HP_Client pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Client_OnPullReceive)		(HP_Client pSender, HP_CONNID dwConnID, int iLength);
typedef En_HP_HandleResult (__HP_CALL *HP_FN_Client_OnClose)			(HP_Client pSender, HP_CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode);

/****************************************************/
/*************** TCP/UDP ���󴴽����� ****************/

// ���� HP_TcpServer ����
HPSOCKET_API HP_TcpServer __HP_CALL Create_HP_TcpServer(HP_TcpServerListener pListener);
// ���� HP_TcpAgent ����
HPSOCKET_API HP_TcpAgent __HP_CALL Create_HP_TcpAgent(HP_TcpAgentListener pListener);
// ���� HP_TcpClient ����
HPSOCKET_API HP_TcpClient __HP_CALL Create_HP_TcpClient(HP_TcpClientListener pListener);
// ���� HP_TcpPullServer ����
HPSOCKET_API HP_TcpPullServer __HP_CALL Create_HP_TcpPullServer(HP_TcpPullServerListener pListener);
// ���� HP_TcpPullAgent ����
HPSOCKET_API HP_TcpPullAgent __HP_CALL Create_HP_TcpPullAgent(HP_TcpPullAgentListener pListener);
// ���� HP_TcpPullClient ����
HPSOCKET_API HP_TcpPullClient __HP_CALL Create_HP_TcpPullClient(HP_TcpPullClientListener pListener);
// ���� HP_TcpPackServer ����
HPSOCKET_API HP_TcpPackServer __HP_CALL Create_HP_TcpPackServer(HP_TcpServerListener pListener);
// ���� HP_TcpPackAgent ����
HPSOCKET_API HP_TcpPackAgent __HP_CALL Create_HP_TcpPackAgent(HP_TcpAgentListener pListener);
// ���� HP_TcpPackClient ����
HPSOCKET_API HP_TcpPackClient __HP_CALL Create_HP_TcpPackClient(HP_TcpClientListener pListener);
// ���� HP_UdpServer ����
HPSOCKET_API HP_UdpServer __HP_CALL Create_HP_UdpServer(HP_UdpServerListener pListener);
// ���� HP_UdpClient ����
HPSOCKET_API HP_UdpClient __HP_CALL Create_HP_UdpClient(HP_UdpClientListener pListener);
// ���� HP_UdpCast ����
HPSOCKET_API HP_UdpCast __HP_CALL Create_HP_UdpCast(HP_UdpCastListener pListener);

// ���� HP_TcpServer ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpServer(HP_TcpServer pServer);
// ���� HP_TcpAgent ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpAgent(HP_TcpAgent pAgent);
// ���� HP_TcpClient ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpClient(HP_TcpClient pClient);
// ���� HP_TcpPullServer ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpPullServer(HP_TcpPullServer pServer);
// ���� HP_TcpPullAgent ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpPullAgent(HP_TcpPullAgent pAgent);
// ���� HP_TcpPullClient ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpPullClient(HP_TcpPullClient pClient);
// ���� HP_TcpPackServer ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpPackServer(HP_TcpPackServer pServer);
// ���� HP_TcpPackAgent ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpPackAgent(HP_TcpPackAgent pAgent);
// ���� HP_TcpPackClient ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpPackClient(HP_TcpPackClient pClient);
// ���� HP_UdpServer ����
HPSOCKET_API void __HP_CALL Destroy_HP_UdpServer(HP_UdpServer pServer);
// ���� HP_UdpClient ����
HPSOCKET_API void __HP_CALL Destroy_HP_UdpClient(HP_UdpClient pClient);
// ���� HP_UdpCast ����
HPSOCKET_API void __HP_CALL Destroy_HP_UdpCast(HP_UdpCast pCast);

// ���� HP_TcpServerListener ����
HPSOCKET_API HP_TcpServerListener __HP_CALL Create_HP_TcpServerListener();
// ���� HP_TcpAgentListener ����
HPSOCKET_API HP_TcpAgentListener __HP_CALL Create_HP_TcpAgentListener();
// ���� HP_TcpClientListener ����
HPSOCKET_API HP_TcpClientListener __HP_CALL Create_HP_TcpClientListener();
// ���� HP_TcpPullServerListener ����
HPSOCKET_API HP_TcpPullServerListener __HP_CALL Create_HP_TcpPullServerListener();
// ���� HP_TcpPullAgentListener ����
HPSOCKET_API HP_TcpPullAgentListener __HP_CALL Create_HP_TcpPullAgentListener();
// ���� HP_TcpPullClientListener ����
HPSOCKET_API HP_TcpPullClientListener __HP_CALL Create_HP_TcpPullClientListener();
// ���� HP_TcpPackServerListener ����
HPSOCKET_API HP_TcpPackServerListener __HP_CALL Create_HP_TcpPackServerListener();
// ���� HP_TcpPackAgentListener ����
HPSOCKET_API HP_TcpPackAgentListener __HP_CALL Create_HP_TcpPackAgentListener();
// ���� HP_TcpPackClientListener ����
HPSOCKET_API HP_TcpPackClientListener __HP_CALL Create_HP_TcpPackClientListener();
// ���� HP_UdpServerListener ����
HPSOCKET_API HP_UdpServerListener __HP_CALL Create_HP_UdpServerListener();
// ���� HP_UdpClientListener ����
HPSOCKET_API HP_UdpClientListener __HP_CALL Create_HP_UdpClientListener();
// ���� HP_UdpCastListener ����
HPSOCKET_API HP_UdpCastListener __HP_CALL Create_HP_UdpCastListener();

// ���� HP_TcpServerListener ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpServerListener(HP_TcpServerListener pListener);
// ���� HP_TcpAgentListener ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpAgentListener(HP_TcpAgentListener pListener);
// ���� HP_TcpClientListener ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpClientListener(HP_TcpClientListener pListener);
// ���� HP_TcpPullServerListener ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpPullServerListener(HP_TcpPullServerListener pListener);
// ���� HP_TcpPullAgentListener ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpPullAgentListener(HP_TcpPullAgentListener pListener);
// ���� HP_TcpPullClientListener ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpPullClientListener(HP_TcpPullClientListener pListener);
// ���� HP_TcpPackServerListener ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpPackServerListener(HP_TcpPackServerListener pListener);
// ���� HP_TcpPackAgentListener ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpPackAgentListener(HP_TcpPackAgentListener pListener);
// ���� HP_TcpPackClientListener ����
HPSOCKET_API void __HP_CALL Destroy_HP_TcpPackClientListener(HP_TcpPackClientListener pListener);
// ���� HP_UdpServerListener ����
HPSOCKET_API void __HP_CALL Destroy_HP_UdpServerListener(HP_UdpServerListener pListener);
// ���� HP_UdpClientListener ����
HPSOCKET_API void __HP_CALL Destroy_HP_UdpClientListener(HP_UdpClientListener pListener);
// ���� HP_UdpCastListener ����
HPSOCKET_API void __HP_CALL Destroy_HP_UdpCastListener(HP_UdpCastListener pListener);

/**********************************************************************************/
/***************************** Server �ص��������÷��� *****************************/

HPSOCKET_API void __HP_CALL HP_Set_FN_Server_OnPrepareListen(HP_ServerListener pListener	, HP_FN_Server_OnPrepareListen fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Server_OnAccept(HP_ServerListener pListener			, HP_FN_Server_OnAccept fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Server_OnHandShake(HP_ServerListener pListener		, HP_FN_Server_OnHandShake fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Server_OnSend(HP_ServerListener pListener				, HP_FN_Server_OnSend fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Server_OnReceive(HP_ServerListener pListener			, HP_FN_Server_OnReceive fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Server_OnPullReceive(HP_ServerListener pListener		, HP_FN_Server_OnPullReceive fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Server_OnClose(HP_ServerListener pListener			, HP_FN_Server_OnClose fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Server_OnShutdown(HP_ServerListener pListener			, HP_FN_Server_OnShutdown fn);

/**********************************************************************************/
/****************************** Agent �ص��������÷��� *****************************/

HPSOCKET_API void __HP_CALL HP_Set_FN_Agent_OnPrepareConnect(HP_AgentListener pListener		, HP_FN_Agent_OnPrepareConnect fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Agent_OnConnect(HP_AgentListener pListener			, HP_FN_Agent_OnConnect fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Agent_OnHandShake(HP_AgentListener pListener			, HP_FN_Agent_OnHandShake fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Agent_OnSend(HP_AgentListener pListener				, HP_FN_Agent_OnSend fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Agent_OnReceive(HP_AgentListener pListener			, HP_FN_Agent_OnReceive fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Agent_OnPullReceive(HP_AgentListener pListener		, HP_FN_Agent_OnPullReceive fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Agent_OnClose(HP_AgentListener pListener				, HP_FN_Agent_OnClose fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Agent_OnShutdown(HP_AgentListener pListener			, HP_FN_Agent_OnShutdown fn);

/**********************************************************************************/
/***************************** Client �ص��������÷��� *****************************/

HPSOCKET_API void __HP_CALL HP_Set_FN_Client_OnPrepareConnect(HP_ClientListener pListener	, HP_FN_Client_OnPrepareConnect fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Client_OnConnect(HP_ClientListener pListener			, HP_FN_Client_OnConnect fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Client_OnHandShake(HP_ClientListener pListener		, HP_FN_Client_OnHandShake fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Client_OnSend(HP_ClientListener pListener				, HP_FN_Client_OnSend fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Client_OnReceive(HP_ClientListener pListener			, HP_FN_Client_OnReceive fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Client_OnPullReceive(HP_ClientListener pListener		, HP_FN_Client_OnPullReceive fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_Client_OnClose(HP_ClientListener pListener			, HP_FN_Client_OnClose fn);

/**************************************************************************/
/***************************** Server �������� *****************************/

/*
* ���ƣ�����ͨ�����
* ���������������ͨ�������������ɺ�ɿ�ʼ���տͻ������Ӳ��շ�����
*		
* ������		lpszBindAddress	-- ������ַ
*			usPort			-- �����˿�
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� HP_Server_GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Server_Start(HP_Server pServer, LPCTSTR lpszBindAddress, USHORT usPort);

/*
* ���ƣ��ر�ͨ�����
* �������رշ����ͨ��������ر���ɺ�Ͽ����пͻ������Ӳ��ͷ�������Դ
*		
* ������	
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� HP_Server_GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Server_Stop(HP_Server pServer);

/*
* ���ƣ���������
* ��������ָ�����ӷ�������
*		
* ������		dwConnID	-- ���� ID
*			pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Server_Send(HP_Server pServer, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength);

/*
* ���ƣ���������
* ��������ָ�����ӷ�������
*		
* ������		dwConnID	-- ���� ID
*			pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
*			iOffset		-- ���ͻ�����ָ��ƫ����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Server_SendPart(HP_Server pServer, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset);

/*
* ���ƣ����Ͷ�������
* ��������ָ�����ӷ��Ͷ�������
*		TCP - ˳�����������ݰ� 
*		UDP - ���������ݰ���ϳ�һ�����ݰ����ͣ����ݰ����ܳ��Ȳ��ܴ������õ� UDP ����󳤶ȣ� 
*		
* ������		dwConnID	-- ���� ID
*			pBuffers	-- ���ͻ���������
*			iCount		-- ���ͻ�������Ŀ
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Server_SendPackets(HP_Server pServer, HP_CONNID dwConnID, const WSABUF pBuffers[], int iCount);

/*
* ���ƣ���ͣ/�ָ�����
* ��������ͣ/�ָ�ĳ�����ӵ����ݽ��չ���
*		
* ������		dwConnID	-- ���� ID
*			bPause		-- TRUE - ��ͣ, FALSE - �ָ�
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_Server_PauseReceive(HP_Server pServer, HP_CONNID dwConnID, BOOL bPause);

/*
* ���ƣ��Ͽ�����
* �������Ͽ���ĳ���ͻ��˵�����
*		
* ������		dwConnID	-- ���� ID
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_Server_Disconnect(HP_Server pServer, HP_CONNID dwConnID, BOOL bForce);

/*
* ���ƣ��Ͽ���ʱ����
* �������Ͽ�����ָ��ʱ��������
*		
* ������		dwPeriod	-- ʱ�������룩
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_Server_DisconnectLongConnections(HP_Server pServer, DWORD dwPeriod, BOOL bForce);

/*
* ���ƣ��Ͽ���Ĭ����
* �������Ͽ�����ָ��ʱ���ľ�Ĭ����
*		
* ������		dwPeriod	-- ʱ�������룩
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_Server_DisconnectSilenceConnections(HP_Server pServer, DWORD dwPeriod, BOOL bForce);

/******************************************************************************/
/***************************** Server ���Է��ʷ��� *****************************/

/*
* ���ƣ��������ӵĸ�������
* �������Ƿ�Ϊ���Ӱ󶨸������ݻ��߰�ʲô�������ݣ�����Ӧ�ó���ֻ�����
*		
* ������		dwConnID	-- ���� ID
*			pv			-- ����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���Ч������ ID��
*/
HPSOCKET_API BOOL __HP_CALL HP_Server_SetConnectionExtra(HP_Server pServer, HP_CONNID dwConnID, PVOID pExtra);

/*
* ���ƣ���ȡ���ӵĸ�������
* �������Ƿ�Ϊ���Ӱ󶨸������ݻ��߰�ʲô�������ݣ�����Ӧ�ó���ֻ�����
*		
* ������		dwConnID	-- ���� ID
*			ppv			-- ����ָ��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���Ч������ ID��
*/
HPSOCKET_API BOOL __HP_CALL HP_Server_GetConnectionExtra(HP_Server pServer, HP_CONNID dwConnID, PVOID* ppExtra);

/* ����Ƿ�Ϊ��ȫ���ӣ�SSL/HTTPS�� */
HPSOCKET_API BOOL __HP_CALL HP_Server_IsSecure(HP_Server pServer);
/* ���ͨ������Ƿ������� */
HPSOCKET_API BOOL __HP_CALL HP_Server_HasStarted(HP_Server pServer);
/* �鿴ͨ�������ǰ״̬ */
HPSOCKET_API En_HP_ServiceState __HP_CALL HP_Server_GetState(HP_Server pServer);
/* ��ȡ���һ��ʧ�ܲ����Ĵ������ */
HPSOCKET_API En_HP_SocketError __HP_CALL HP_Server_GetLastError(HP_Server pServer);
/* ��ȡ���һ��ʧ�ܲ����Ĵ������� */
HPSOCKET_API LPCTSTR __HP_CALL HP_Server_GetLastErrorDesc(HP_Server pServer);
/* ��ȡ������δ�������ݵĳ��� */
HPSOCKET_API BOOL __HP_CALL HP_Server_GetPendingDataLength(HP_Server pServer, HP_CONNID dwConnID, int* piPending);
/* ��ȡ���ӵ����ݽ���״̬ */
HPSOCKET_API BOOL __HP_CALL HP_Server_IsPauseReceive(HP_Server pServer, HP_CONNID dwConnID, BOOL* pbPaused);
/* ����Ƿ���Ч���� */
HPSOCKET_API BOOL __HP_CALL HP_Server_IsConnected(HP_Server pServer, HP_CONNID dwConnID);
/* ��ȡ�ͻ��������� */
HPSOCKET_API DWORD __HP_CALL HP_Server_GetConnectionCount(HP_Server pServer);
/* ��ȡ�������ӵ� HP_CONNID */
HPSOCKET_API BOOL __HP_CALL HP_Server_GetAllConnectionIDs(HP_Server pServer, HP_CONNID pIDs[], DWORD* pdwCount);
/* ��ȡĳ���ͻ�������ʱ�������룩 */
HPSOCKET_API BOOL __HP_CALL HP_Server_GetConnectPeriod(HP_Server pServer, HP_CONNID dwConnID, DWORD* pdwPeriod);
/* ��ȡĳ�����Ӿ�Ĭʱ�䣨���룩 */
HPSOCKET_API BOOL __HP_CALL HP_Server_GetSilencePeriod(HP_Server pServer, HP_CONNID dwConnID, DWORD* pdwPeriod);
/* ��ȡ���� Socket �ĵ�ַ��Ϣ */
HPSOCKET_API BOOL __HP_CALL HP_Server_GetListenAddress(HP_Server pServer, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort);
/* ��ȡĳ�����ӵı��ص�ַ��Ϣ */
HPSOCKET_API BOOL __HP_CALL HP_Server_GetLocalAddress(HP_Server pServer, HP_CONNID dwConnID, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort);
/* ��ȡĳ�����ӵ�Զ�̵�ַ��Ϣ */
HPSOCKET_API BOOL __HP_CALL HP_Server_GetRemoteAddress(HP_Server pServer, HP_CONNID dwConnID, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort);

/* �������ݷ��Ͳ��� ���� Linux ƽ̨�����Ч�� */
HPSOCKET_API void __HP_CALL HP_Server_SetSendPolicy(HP_Server pServer, En_HP_SendPolicy enSendPolicy);
/* ���� Socket �����������ʱ�䣨���룬�������ڼ�� Socket ��������ܱ���ȡʹ�ã� */
HPSOCKET_API void __HP_CALL HP_Server_SetFreeSocketObjLockTime(HP_Server pServer, DWORD dwFreeSocketObjLockTime);
/* ���� Socket ����ش�С��ͨ������Ϊƽ���������������� 1/3 - 1/2�� */
HPSOCKET_API void __HP_CALL HP_Server_SetFreeSocketObjPool(HP_Server pServer, DWORD dwFreeSocketObjPool);
/* �����ڴ�黺��ش�С��ͨ������Ϊ Socket ����ش�С�� 2 - 3 ���� */
HPSOCKET_API void __HP_CALL HP_Server_SetFreeBufferObjPool(HP_Server pServer, DWORD dwFreeBufferObjPool);
/* ���� Socket ����ػ��շ�ֵ��ͨ������Ϊ Socket ����ش�С�� 3 ���� */
HPSOCKET_API void __HP_CALL HP_Server_SetFreeSocketObjHold(HP_Server pServer, DWORD dwFreeSocketObjHold);
/* �����ڴ�黺��ػ��շ�ֵ��ͨ������Ϊ�ڴ�黺��ش�С�� 3 ���� */
HPSOCKET_API void __HP_CALL HP_Server_SetFreeBufferObjHold(HP_Server pServer, DWORD dwFreeBufferObjHold);
/* ���������������������������ֵԤ�����ڴ棬�����Ҫ����ʵ��������ã����˹���*/
HPSOCKET_API void __HP_CALL HP_Server_SetMaxConnectionCount(HP_Server pServer, DWORD dwMaxConnectionCount);
/* ���ù����߳�������ͨ������Ϊ 2 * CPU + 2�� */
HPSOCKET_API void __HP_CALL HP_Server_SetWorkerThreadCount(HP_Server pServer, DWORD dwWorkerThreadCount);
/* �����Ƿ��Ǿ�Ĭʱ�䣨����Ϊ TRUE ʱ DisconnectSilenceConnections() �� GetSilencePeriod() ����Ч��Ĭ�ϣ�TRUE�� */
HPSOCKET_API void __HP_CALL HP_Server_SetMarkSilence(HP_Server pServer, BOOL bMarkSilence);

/* ��ȡ���ݷ��Ͳ��� ���� Linux ƽ̨�����Ч�� */
HPSOCKET_API En_HP_SendPolicy __HP_CALL HP_Server_GetSendPolicy(HP_Server pServer);
/* ��ȡ Socket �����������ʱ�� */
HPSOCKET_API DWORD __HP_CALL HP_Server_GetFreeSocketObjLockTime(HP_Server pServer);
/* ��ȡ Socket ����ش�С */
HPSOCKET_API DWORD __HP_CALL HP_Server_GetFreeSocketObjPool(HP_Server pServer);
/* ��ȡ�ڴ�黺��ش�С */
HPSOCKET_API DWORD __HP_CALL HP_Server_GetFreeBufferObjPool(HP_Server pServer);
/* ��ȡ Socket ����ػ��շ�ֵ */
HPSOCKET_API DWORD __HP_CALL HP_Server_GetFreeSocketObjHold(HP_Server pServer);
/* ��ȡ�ڴ�黺��ػ��շ�ֵ */
HPSOCKET_API DWORD __HP_CALL HP_Server_GetFreeBufferObjHold(HP_Server pServer);
/* ��ȡ��������� */
HPSOCKET_API DWORD __HP_CALL HP_Server_GetMaxConnectionCount(HP_Server pServer);
/* ��ȡ�����߳����� */
HPSOCKET_API DWORD __HP_CALL HP_Server_GetWorkerThreadCount(HP_Server pServer);
/* ����Ƿ��Ǿ�Ĭʱ�� */
HPSOCKET_API BOOL __HP_CALL HP_Server_IsMarkSilence(HP_Server pServer);

/**********************************************************************************/
/******************************* TCP Server �������� *******************************/

/*
* ���ƣ�����С�ļ�
* ��������ָ�����ӷ��� 4096 KB ���µ�С�ļ�
*		
* ������		dwConnID		-- ���� ID
*			lpszFileName	-- �ļ�·��
*			pHead			-- ͷ����������
*			pTail			-- β����������
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_TcpServer_SendSmallFile(HP_Server pServer, HP_CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail);

/**********************************************************************************/
/***************************** TCP Server ���Է��ʷ��� *****************************/

/* ���ü��� Socket �ĵȺ���д�С�����ݲ������������������ã� */
HPSOCKET_API void __HP_CALL HP_TcpServer_SetSocketListenQueue(HP_TcpServer pServer, DWORD dwSocketListenQueue);
/* ���� EPOLL �ȴ��¼���������� */
HPSOCKET_API void __HP_CALL HP_TcpServer_SetAcceptSocketCount(HP_TcpServer pServer, DWORD dwAcceptSocketCount);
/* ����ͨ�����ݻ�������С������ƽ��ͨ�����ݰ���С�������ã�ͨ������Ϊ 1024 �ı����� */
HPSOCKET_API void __HP_CALL HP_TcpServer_SetSocketBufferSize(HP_TcpServer pServer, DWORD dwSocketBufferSize);
/* ����������������������룬0 �򲻷�����������Ĭ�ϣ�60 * 1000�� */
HPSOCKET_API void __HP_CALL HP_TcpServer_SetKeepAliveTime(HP_TcpServer pServer, DWORD dwKeepAliveTime);
/* �����쳣��������������룬0 ����������������Ĭ�ϣ�20 * 1000������������ɴ� [Ĭ�ϣ�WinXP 5 ��, Win7 10 ��] ��ⲻ������ȷ�ϰ�����Ϊ�Ѷ��ߣ� */
HPSOCKET_API void __HP_CALL HP_TcpServer_SetKeepAliveInterval(HP_TcpServer pServer, DWORD dwKeepAliveInterval);

/* ��ȡ EPOLL �ȴ��¼���������� */
HPSOCKET_API DWORD __HP_CALL HP_TcpServer_GetAcceptSocketCount(HP_TcpServer pServer);
/* ��ȡͨ�����ݻ�������С */
HPSOCKET_API DWORD __HP_CALL HP_TcpServer_GetSocketBufferSize(HP_TcpServer pServer);
/* ��ȡ���� Socket �ĵȺ���д�С */
HPSOCKET_API DWORD __HP_CALL HP_TcpServer_GetSocketListenQueue(HP_TcpServer pServer);
/* ��ȡ������������� */
HPSOCKET_API DWORD __HP_CALL HP_TcpServer_GetKeepAliveTime(HP_TcpServer pServer);
/* ��ȡ�쳣��������� */
HPSOCKET_API DWORD __HP_CALL HP_TcpServer_GetKeepAliveInterval(HP_TcpServer pServer);

/**********************************************************************************/
/***************************** UDP Server ���Է��ʷ��� *****************************/

/* �������ݱ�����󳤶ȣ������ھ����������²����� 1472 �ֽڣ��ڹ����������²����� 548 �ֽڣ� */
HPSOCKET_API void __HP_CALL HP_UdpServer_SetMaxDatagramSize(HP_UdpServer pServer, DWORD dwMaxDatagramSize);
/* ��ȡ���ݱ�����󳤶� */
HPSOCKET_API DWORD __HP_CALL HP_UdpServer_GetMaxDatagramSize(HP_UdpServer pServer);

/* ���� Receive ԤͶ�����������ݸ��ص������ã�Receive ԤͶ������Խ���򶪰�����ԽС�� */
HPSOCKET_API void __HP_CALL HP_UdpServer_SetPostReceiveCount(HP_UdpServer pServer, DWORD dwPostReceiveCount);
/* ��ȡ Receive ԤͶ������ */
HPSOCKET_API DWORD __HP_CALL HP_UdpServer_GetPostReceiveCount(HP_UdpServer pServer);

/* ���ü������Դ�����0 �򲻷��ͼ�������������������Դ�������Ϊ�Ѷ��ߣ� */
HPSOCKET_API void __HP_CALL HP_UdpServer_SetDetectAttempts(HP_UdpServer pServer, DWORD dwDetectAttempts);
/* ���ü������ͼ�����룬0 �����ͼ����� */
HPSOCKET_API void __HP_CALL HP_UdpServer_SetDetectInterval(HP_UdpServer pServer, DWORD dwDetectInterval);
/* ��ȡ���������� */
HPSOCKET_API DWORD __HP_CALL HP_UdpServer_GetDetectAttempts(HP_UdpServer pServer);
/* ��ȡ��������� */
HPSOCKET_API DWORD __HP_CALL HP_UdpServer_GetDetectInterval(HP_UdpServer pServer);

/**************************************************************************/
/***************************** Agent �������� *****************************/

/*
* ���ƣ�����ͨ�����
* ����������ͨ�Ŵ��������������ɺ�ɿ�ʼ����Զ�̷�����
*		
* ������		lpszBindAddress	-- �󶨵�ַ��Ĭ�ϣ�nullptr���������ַ��
*			bAsyncConnect	-- �Ƿ�����첽 Connect
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� HP_Agent_GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Agent_Start(HP_Agent pAgent, LPCTSTR lpszBindAddress, BOOL bAsyncConnect);

/*
* ���ƣ��ر�ͨ�����
* �������ر�ͨ��������ر���ɺ�Ͽ��������Ӳ��ͷ�������Դ
*		
* ������	
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� HP_Agent_GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Agent_Stop(HP_Agent pAgent);

/*
* ���ƣ����ӷ�����
* ���������ӷ����������ӳɹ��� IAgentListener ����յ� OnConnect() / OnHandShake() �¼�
*		
* ������		lpszRemoteAddress	-- ����˵�ַ
*			usPort				-- ����˶˿�
*			pdwConnID			-- ���� ID��Ĭ�ϣ�nullptr������ȡ���� ID��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ������ SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Agent_Connect(HP_Agent pAgent, LPCTSTR lpszRemoteAddress, USHORT usPort, HP_CONNID* pdwConnID);

/*
* ���ƣ����ӷ�����
* ���������ӷ����������ӳɹ��� IAgentListener ����յ� OnConnect() / OnHandShake() �¼�
*		
* ������		lpszRemoteAddress	-- ����˵�ַ
*			usPort				-- ����˶˿�
*			pdwConnID			-- ���� ID��Ĭ�ϣ�nullptr������ȡ���� ID��
*			pExtra				-- ���Ӹ������ݣ�Ĭ�ϣ�nullptr��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ������ SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Agent_ConnectWithExtra(HP_Agent pAgent, LPCTSTR lpszRemoteAddress, USHORT usPort, HP_CONNID* pdwConnID, PVOID pExtra);

/*
* ���ƣ����ӷ�����
* ���������ӷ����������ӳɹ��� IAgentListener ����յ� OnConnect() / OnHandShake() �¼�
*
* ������		lpszRemoteAddress	-- ����˵�ַ
*			usPort				-- ����˶˿�
*			pdwConnID			-- ���� ID��Ĭ�ϣ�nullptr������ȡ���� ID��
*			usLocalPort			-- ���ض˿ڣ�Ĭ�ϣ�0��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ������ SYS_GetLastError() ��ȡ Windows �������
*/
HPSOCKET_API BOOL __HP_CALL HP_Agent_ConnectWithLocalPort(HP_Agent pAgent, LPCTSTR lpszRemoteAddress, USHORT usPort, HP_CONNID* pdwConnID, USHORT usLocalPort);

/*
* ���ƣ����ӷ�����
* ���������ӷ����������ӳɹ��� IAgentListener ����յ� OnConnect() / OnHandShake() �¼�
*
* ������		lpszRemoteAddress	-- ����˵�ַ
*			usPort				-- ����˶˿�
*			pdwConnID			-- ���� ID��Ĭ�ϣ�nullptr������ȡ���� ID��
*			pExtra				-- ���Ӹ������ݣ�Ĭ�ϣ�nullptr��
*			usLocalPort			-- ���ض˿ڣ�Ĭ�ϣ�0��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ������ SYS_GetLastError() ��ȡ Windows �������
*/
HPSOCKET_API BOOL __HP_CALL HP_Agent_ConnectWithExtraAndLocalPort(HP_Agent pAgent, LPCTSTR lpszRemoteAddress, USHORT usPort, HP_CONNID* pdwConnID, PVOID pExtra, USHORT usLocalPort);

/*
* ���ƣ���������
* ��������ָ�����ӷ�������
*		
* ������		dwConnID	-- ���� ID
*			pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Agent_Send(HP_Agent pAgent, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength);

/*
* ���ƣ���������
* ��������ָ�����ӷ�������
*		
* ������		dwConnID	-- ���� ID
*			pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
*			iOffset		-- ���ͻ�����ָ��ƫ����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Agent_SendPart(HP_Agent pAgent, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset);

/*
* ���ƣ����Ͷ�������
* ��������ָ�����ӷ��Ͷ�������
*		TCP - ˳�����������ݰ� 
*		UDP - ���������ݰ���ϳ�һ�����ݰ����ͣ����ݰ����ܳ��Ȳ��ܴ������õ� UDP ����󳤶ȣ� 
*		
* ������		dwConnID	-- ���� ID
*			pBuffers	-- ���ͻ���������
*			iCount		-- ���ͻ�������Ŀ
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Agent_SendPackets(HP_Agent pAgent, HP_CONNID dwConnID, const WSABUF pBuffers[], int iCount);

/*
* ���ƣ���ͣ/�ָ�����
* ��������ͣ/�ָ�ĳ�����ӵ����ݽ��չ���
*		
* ������		dwConnID	-- ���� ID
*			bPause		-- TRUE - ��ͣ, FALSE - �ָ�
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_Agent_PauseReceive(HP_Agent pAgent, HP_CONNID dwConnID, BOOL bPause);

/*
* ���ƣ��Ͽ�����
* �������Ͽ�ĳ������
*		
* ������		dwConnID	-- ���� ID
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_Agent_Disconnect(HP_Agent pAgent, HP_CONNID dwConnID, BOOL bForce);

/*
* ���ƣ��Ͽ���ʱ����
* �������Ͽ�����ָ��ʱ��������
*		
* ������		dwPeriod	-- ʱ�������룩
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_Agent_DisconnectLongConnections(HP_Agent pAgent, DWORD dwPeriod, BOOL bForce);

/*
* ���ƣ��Ͽ���Ĭ����
* �������Ͽ�����ָ��ʱ���ľ�Ĭ����
*		
* ������		dwPeriod	-- ʱ�������룩
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_Agent_DisconnectSilenceConnections(HP_Agent pAgent, DWORD dwPeriod, BOOL bForce);

/******************************************************************************/
/***************************** Agent ���Է��ʷ��� *****************************/

/*
* ���ƣ��������ӵĸ�������
* �������Ƿ�Ϊ���Ӱ󶨸������ݻ��߰�ʲô�������ݣ�����Ӧ�ó���ֻ�����
*		
* ������		dwConnID	-- ���� ID
*			pv			-- ����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���Ч������ ID��
*/
HPSOCKET_API BOOL __HP_CALL HP_Agent_SetConnectionExtra(HP_Agent pAgent, HP_CONNID dwConnID, PVOID pExtra);

/*
* ���ƣ���ȡ���ӵĸ�������
* �������Ƿ�Ϊ���Ӱ󶨸������ݻ��߰�ʲô�������ݣ�����Ӧ�ó���ֻ�����
*		
* ������		dwConnID	-- ���� ID
*			ppv			-- ����ָ��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���Ч������ ID��
*/
HPSOCKET_API BOOL __HP_CALL HP_Agent_GetConnectionExtra(HP_Agent pAgent, HP_CONNID dwConnID, PVOID* ppExtra);

/* ����Ƿ�Ϊ��ȫ���ӣ�SSL/HTTPS�� */
HPSOCKET_API BOOL __HP_CALL HP_Agent_IsSecure(HP_Agent pAgent);
/* ���ͨ������Ƿ������� */
HPSOCKET_API BOOL __HP_CALL HP_Agent_HasStarted(HP_Agent pAgent);
/* �鿴ͨ�������ǰ״̬ */
HPSOCKET_API En_HP_ServiceState __HP_CALL HP_Agent_GetState(HP_Agent pAgent);
/* ��ȡ������ */
HPSOCKET_API DWORD __HP_CALL HP_Agent_GetConnectionCount(HP_Agent pAgent);
/* ��ȡ�������ӵ� HP_CONNID */
HPSOCKET_API BOOL __HP_CALL HP_Agent_GetAllConnectionIDs(HP_Agent pAgent, HP_CONNID pIDs[], DWORD* pdwCount);
/* ��ȡĳ������ʱ�������룩 */
HPSOCKET_API BOOL __HP_CALL HP_Agent_GetConnectPeriod(HP_Agent pAgent, HP_CONNID dwConnID, DWORD* pdwPeriod);
/* ��ȡĳ�����Ӿ�Ĭʱ�䣨���룩 */
HPSOCKET_API BOOL __HP_CALL HP_Agent_GetSilencePeriod(HP_Agent pAgent, HP_CONNID dwConnID, DWORD* pdwPeriod);
/* ��ȡĳ�����ӵı��ص�ַ��Ϣ */
HPSOCKET_API BOOL __HP_CALL HP_Agent_GetLocalAddress(HP_Agent pAgent, HP_CONNID dwConnID, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort);
/* ��ȡĳ�����ӵ�Զ�̵�ַ��Ϣ */
HPSOCKET_API BOOL __HP_CALL HP_Agent_GetRemoteAddress(HP_Agent pAgent, HP_CONNID dwConnID, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort);
/* ��ȡĳ�����ӵ�Զ��������Ϣ */
HPSOCKET_API BOOL __HP_CALL HP_Agent_GetRemoteHost(HP_Agent pAgent, HP_CONNID dwConnID, TCHAR lpszHost[], int* piHostLen, USHORT* pusPort);
/* ��ȡ���һ��ʧ�ܲ����Ĵ������ */
HPSOCKET_API En_HP_SocketError __HP_CALL HP_Agent_GetLastError(HP_Agent pAgent);
/* ��ȡ���һ��ʧ�ܲ����Ĵ������� */
HPSOCKET_API LPCTSTR __HP_CALL HP_Agent_GetLastErrorDesc(HP_Agent pAgent);
/* ��ȡ������δ�������ݵĳ��� */
HPSOCKET_API BOOL __HP_CALL HP_Agent_GetPendingDataLength(HP_Agent pAgent, HP_CONNID dwConnID, int* piPending);
/* ��ȡ���ӵ����ݽ���״̬ */
HPSOCKET_API BOOL __HP_CALL HP_Agent_IsPauseReceive(HP_Agent pAgent, HP_CONNID dwConnID, BOOL* pbPaused);
/* ����Ƿ���Ч���� */
HPSOCKET_API BOOL __HP_CALL HP_Agent_IsConnected(HP_Agent pAgent, HP_CONNID dwConnID);

/* �������ݷ��Ͳ��� ���� Linux ƽ̨�����Ч�� */
HPSOCKET_API void __HP_CALL HP_Agent_SetSendPolicy(HP_Agent pAgent, En_HP_SendPolicy enSendPolicy);
/* ���� Socket �����������ʱ�䣨���룬�������ڼ�� Socket ��������ܱ���ȡʹ�ã� */
HPSOCKET_API void __HP_CALL HP_Agent_SetFreeSocketObjLockTime(HP_Agent pAgent, DWORD dwFreeSocketObjLockTime);
/* ���� Socket ����ش�С��ͨ������Ϊƽ���������������� 1/3 - 1/2�� */
HPSOCKET_API void __HP_CALL HP_Agent_SetFreeSocketObjPool(HP_Agent pAgent, DWORD dwFreeSocketObjPool);
/* �����ڴ�黺��ش�С��ͨ������Ϊ Socket ����ش�С�� 2 - 3 ���� */
HPSOCKET_API void __HP_CALL HP_Agent_SetFreeBufferObjPool(HP_Agent pAgent, DWORD dwFreeBufferObjPool);
/* ���� Socket ����ػ��շ�ֵ��ͨ������Ϊ Socket ����ش�С�� 3 ���� */
HPSOCKET_API void __HP_CALL HP_Agent_SetFreeSocketObjHold(HP_Agent pAgent, DWORD dwFreeSocketObjHold);
/* �����ڴ�黺��ػ��շ�ֵ��ͨ������Ϊ�ڴ�黺��ش�С�� 3 ���� */
HPSOCKET_API void __HP_CALL HP_Agent_SetFreeBufferObjHold(HP_Agent pAgent, DWORD dwFreeBufferObjHold);
/* ���������������������������ֵԤ�����ڴ棬�����Ҫ����ʵ��������ã����˹���*/
HPSOCKET_API void __HP_CALL HP_Agent_SetMaxConnectionCount(HP_Agent pAgent, DWORD dwMaxConnectionCount);
/* ���ù����߳�������ͨ������Ϊ 2 * CPU + 2�� */
HPSOCKET_API void __HP_CALL HP_Agent_SetWorkerThreadCount(HP_Agent pAgent, DWORD dwWorkerThreadCount);
/* �����Ƿ��Ǿ�Ĭʱ�䣨����Ϊ TRUE ʱ DisconnectSilenceConnections() �� GetSilencePeriod() ����Ч��Ĭ�ϣ�TRUE�� */
HPSOCKET_API void __HP_CALL HP_Agent_SetMarkSilence(HP_Agent pAgent, BOOL bMarkSilence);

/* ��ȡ���ݷ��Ͳ��� ���� Linux ƽ̨�����Ч�� */
HPSOCKET_API En_HP_SendPolicy __HP_CALL HP_Agent_GetSendPolicy(HP_Agent pAgent);
/* ��ȡ Socket �����������ʱ�� */
HPSOCKET_API DWORD __HP_CALL HP_Agent_GetFreeSocketObjLockTime(HP_Agent pAgent);
/* ��ȡ Socket ����ش�С */
HPSOCKET_API DWORD __HP_CALL HP_Agent_GetFreeSocketObjPool(HP_Agent pAgent);
/* ��ȡ�ڴ�黺��ش�С */
HPSOCKET_API DWORD __HP_CALL HP_Agent_GetFreeBufferObjPool(HP_Agent pAgent);
/* ��ȡ Socket ����ػ��շ�ֵ */
HPSOCKET_API DWORD __HP_CALL HP_Agent_GetFreeSocketObjHold(HP_Agent pAgent);
/* ��ȡ�ڴ�黺��ػ��շ�ֵ */
HPSOCKET_API DWORD __HP_CALL HP_Agent_GetFreeBufferObjHold(HP_Agent pAgent);
/* ��ȡ��������� */
HPSOCKET_API DWORD __HP_CALL HP_Agent_GetMaxConnectionCount(HP_Agent pAgent);
/* ��ȡ�����߳����� */
HPSOCKET_API DWORD __HP_CALL HP_Agent_GetWorkerThreadCount(HP_Agent pAgent);
/* ����Ƿ��Ǿ�Ĭʱ�� */
HPSOCKET_API BOOL __HP_CALL HP_Agent_IsMarkSilence(HP_Agent pAgent);

/**********************************************************************************/
/******************************* TCP Agent �������� *******************************/

/*
* ���ƣ�����С�ļ�
* ��������ָ�����ӷ��� 4096 KB ���µ�С�ļ�
*		
* ������		dwConnID		-- ���� ID
*			lpszFileName	-- �ļ�·��
*			pHead			-- ͷ����������
*			pTail			-- β����������
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_TcpAgent_SendSmallFile(HP_Agent pAgent, HP_CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail);

/**********************************************************************************/
/***************************** TCP Agent ���Է��ʷ��� *****************************/

/* �����Ƿ����õ�ַ���û��ƣ�Ĭ�ϣ������ã� */
HPSOCKET_API void __HP_CALL HP_TcpAgent_SetReuseAddress(HP_TcpAgent pAgent, BOOL bReuseAddress);
/* ����Ƿ����õ�ַ���û��� */
HPSOCKET_API BOOL __HP_CALL HP_TcpAgent_IsReuseAddress(HP_TcpAgent pAgent);

/* ����ͨ�����ݻ�������С������ƽ��ͨ�����ݰ���С�������ã�ͨ������Ϊ 1024 �ı����� */
HPSOCKET_API void __HP_CALL HP_TcpAgent_SetSocketBufferSize(HP_TcpAgent pAgent, DWORD dwSocketBufferSize);
/* ����������������������룬0 �򲻷�����������Ĭ�ϣ�60 * 1000�� */
HPSOCKET_API void __HP_CALL HP_TcpAgent_SetKeepAliveTime(HP_TcpAgent pAgent, DWORD dwKeepAliveTime);
/* �����쳣��������������룬0 ����������������Ĭ�ϣ�20 * 1000������������ɴ� [Ĭ�ϣ�WinXP 5 ��, Win7 10 ��] ��ⲻ������ȷ�ϰ�����Ϊ�Ѷ��ߣ� */
HPSOCKET_API void __HP_CALL HP_TcpAgent_SetKeepAliveInterval(HP_TcpAgent pAgent, DWORD dwKeepAliveInterval);

/* ��ȡͨ�����ݻ�������С */
HPSOCKET_API DWORD __HP_CALL HP_TcpAgent_GetSocketBufferSize(HP_TcpAgent pAgent);
/* ��ȡ������������� */
HPSOCKET_API DWORD __HP_CALL HP_TcpAgent_GetKeepAliveTime(HP_TcpAgent pAgent);
/* ��ȡ�쳣��������� */
HPSOCKET_API DWORD __HP_CALL HP_TcpAgent_GetKeepAliveInterval(HP_TcpAgent pAgent);

/******************************************************************************/
/***************************** Client ����������� *****************************/

/*
* ���ƣ�����ͨ�����
* �����������ͻ���ͨ����������ӷ���ˣ�������ɺ�ɿ�ʼ�շ�����
*		
* ������		lpszRemoteAddress	-- ����˵�ַ
*			usPort				-- ����˶˿�
*			bAsyncConnect		-- �Ƿ�����첽 Connect
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� HP_Client_GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Client_Start(HP_Client pClient, LPCTSTR lpszRemoteAddress, USHORT usPort, BOOL bAsyncConnect);

/*
* ���ƣ�����ͨ���������ָ���󶨵�ַ��
* �����������ͻ���ͨ����������ӷ���ˣ�������ɺ�ɿ�ʼ�շ�����
*		
* ������		lpszRemoteAddress	-- ����˵�ַ
*			usPort				-- ����˶˿�
*			bAsyncConnect		-- �Ƿ�����첽 Connect
*			lpszBindAddress		-- �󶨵�ַ��Ĭ�ϣ�nullptr��TcpClient/UdpClient -> ��ִ�а󶨲�����UdpCast �� -> �����ַ��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� HP_Client_GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Client_StartWithBindAddress(HP_Client pClient, LPCTSTR lpszRemoteAddress, USHORT usPort, BOOL bAsyncConnect, LPCTSTR lpszBindAddress);

/*
* ���ƣ�����ͨ���������ָ���󶨵�ַ��
* �����������ͻ���ͨ����������ӷ���ˣ�������ɺ�ɿ�ʼ�շ�����
*		
* ������		lpszRemoteAddress	-- ����˵�ַ
*			usPort				-- ����˶˿�
*			bAsyncConnect		-- �Ƿ�����첽 Connect
*			lpszBindAddress		-- �󶨵�ַ��Ĭ�ϣ�nullptr��TcpClient/UdpClient -> ��ִ�а󶨲�����UdpCast �� -> �����ַ��
*			usLocalPort			-- ���ض˿ڣ�Ĭ�ϣ�0��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� HP_Client_GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Client_StartWithBindAddressAndLocalPort(HP_Client pClient, LPCTSTR lpszRemoteAddress, USHORT usPort, BOOL bAsyncConnect, LPCTSTR lpszBindAddress, USHORT usLocalPort);

/*
* ���ƣ��ر�ͨ�����
* �������رտͻ���ͨ��������ر���ɺ�Ͽ������˵����Ӳ��ͷ�������Դ
*		
* ������	
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� HP_Client_GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Client_Stop(HP_Client pClient);

/*
* ���ƣ���������
* �����������˷�������
*		
* ������		pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Client_Send(HP_Client pClient, const BYTE* pBuffer, int iLength);

/*
* ���ƣ���������
* �����������˷�������
*		
* ������		pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
*			iOffset		-- ���ͻ�����ָ��ƫ����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Client_SendPart(HP_Client pClient, const BYTE* pBuffer, int iLength, int iOffset);

/*
* ���ƣ����Ͷ�������
* �����������˷��Ͷ�������
*		TCP - ˳�����������ݰ� 
*		UDP - ���������ݰ���ϳ�һ�����ݰ����ͣ����ݰ����ܳ��Ȳ��ܴ������õ� UDP ����󳤶ȣ� 
*		
* ������		pBuffers	-- ���ͻ���������
*			iCount		-- ���ͻ�������Ŀ
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_Client_SendPackets(HP_Client pClient, const WSABUF pBuffers[], int iCount);

/*
* ���ƣ���ͣ/�ָ�����
* ��������ͣ/�ָ�ĳ�����ӵ����ݽ��չ���
*		
*			bPause	-- TRUE - ��ͣ, FALSE - �ָ�
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_Client_PauseReceive(HP_Client pClient, BOOL bPause);

/******************************************************************************/
/***************************** Client ���Է��ʷ��� *****************************/

/* �������ӵĸ������� */
HPSOCKET_API void __HP_CALL HP_Client_SetExtra(HP_Client pClient, PVOID pExtra);
/* ��ȡ���ӵĸ������� */
HPSOCKET_API PVOID __HP_CALL HP_Client_GetExtra(HP_Client pClient);

/* ����Ƿ�Ϊ��ȫ���ӣ�SSL/HTTPS�� */
HPSOCKET_API BOOL __HP_CALL HP_Client_IsSecure(HP_Client pClient);
/* ���ͨ������Ƿ������� */
HPSOCKET_API BOOL __HP_CALL HP_Client_HasStarted(HP_Client pClient);
/* �鿴ͨ�������ǰ״̬ */
HPSOCKET_API En_HP_ServiceState	__HP_CALL HP_Client_GetState(HP_Client pClient);
/* ��ȡ���һ��ʧ�ܲ����Ĵ������ */
HPSOCKET_API En_HP_SocketError	__HP_CALL HP_Client_GetLastError(HP_Client pClient);
/* ��ȡ���һ��ʧ�ܲ����Ĵ������� */
HPSOCKET_API LPCTSTR __HP_CALL HP_Client_GetLastErrorDesc(HP_Client pClient);
/* ��ȡ�������������� ID */
HPSOCKET_API HP_CONNID __HP_CALL HP_Client_GetConnectionID(HP_Client pClient);
/* ��ȡ Client Socket �ĵ�ַ��Ϣ */
HPSOCKET_API BOOL __HP_CALL HP_Client_GetLocalAddress(HP_Client pClient, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort);
/* ��ȡ���ӵ�Զ��������Ϣ */
HPSOCKET_API BOOL __HP_CALL HP_Client_GetRemoteHost(HP_Client pClient, TCHAR lpszHost[], int* piHostLen, USHORT* pusPort);
/* ��ȡ������δ�������ݵĳ��� */
HPSOCKET_API BOOL __HP_CALL HP_Client_GetPendingDataLength(HP_Client pClient, int* piPending);
/* ��ȡ���ӵ����ݽ���״̬ */
HPSOCKET_API BOOL __HP_CALL HP_Client_IsPauseReceive(HP_Client pClient, BOOL* pbPaused);
/* ����Ƿ���Ч���� */
HPSOCKET_API BOOL __HP_CALL HP_Client_IsConnected(HP_Client pClient);
/* �����ڴ�黺��ش�С��ͨ������Ϊ -> PUSH ģ�ͣ�5 - 10��PULL ģ�ͣ�10 - 20 �� */
HPSOCKET_API void __HP_CALL HP_Client_SetFreeBufferPoolSize(HP_Client pClient, DWORD dwFreeBufferPoolSize);
/* �����ڴ�黺��ػ��շ�ֵ��ͨ������Ϊ�ڴ�黺��ش�С�� 3 ���� */
HPSOCKET_API void __HP_CALL HP_Client_SetFreeBufferPoolHold(HP_Client pClient, DWORD dwFreeBufferPoolHold);
/* ��ȡ�ڴ�黺��ش�С */
HPSOCKET_API DWORD __HP_CALL HP_Client_GetFreeBufferPoolSize(HP_Client pClient);
/* ��ȡ�ڴ�黺��ػ��շ�ֵ */
HPSOCKET_API DWORD __HP_CALL HP_Client_GetFreeBufferPoolHold(HP_Client pClient);

/**********************************************************************************/
/******************************* TCP Client �������� *******************************/

/*
* ���ƣ�����С�ļ�
* �����������˷��� 4096 KB ���µ�С�ļ�
*		
* ������		lpszFileName	-- �ļ�·��
*			pHead			-- ͷ����������
*			pTail			-- β����������
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_TcpClient_SendSmallFile(HP_Client pClient, LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail);

/**********************************************************************************/
/***************************** TCP Client ���Է��ʷ��� *****************************/

/* ����ͨ�����ݻ�������С������ƽ��ͨ�����ݰ���С�������ã�ͨ������Ϊ��(N * 1024) - sizeof(TBufferObj)�� */
HPSOCKET_API void __HP_CALL HP_TcpClient_SetSocketBufferSize(HP_TcpClient pClient, DWORD dwSocketBufferSize);
/* ����������������������룬0 �򲻷�����������Ĭ�ϣ�60 * 1000�� */
HPSOCKET_API void __HP_CALL HP_TcpClient_SetKeepAliveTime(HP_TcpClient pClient, DWORD dwKeepAliveTime);
/* �����쳣��������������룬0 ����������������Ĭ�ϣ�20 * 1000������������ɴ� [Ĭ�ϣ�WinXP 5 ��, Win7 10 ��] ��ⲻ������ȷ�ϰ�����Ϊ�Ѷ��ߣ� */
HPSOCKET_API void __HP_CALL HP_TcpClient_SetKeepAliveInterval(HP_TcpClient pClient, DWORD dwKeepAliveInterval);

/* ��ȡͨ�����ݻ�������С */
HPSOCKET_API DWORD __HP_CALL HP_TcpClient_GetSocketBufferSize(HP_TcpClient pClient);
/* ��ȡ������������� */
HPSOCKET_API DWORD __HP_CALL HP_TcpClient_GetKeepAliveTime(HP_TcpClient pClient);
/* ��ȡ�쳣��������� */
HPSOCKET_API DWORD __HP_CALL HP_TcpClient_GetKeepAliveInterval(HP_TcpClient pClient);

/**********************************************************************************/
/***************************** UDP Client ���Է��ʷ��� *****************************/

/* �������ݱ�����󳤶ȣ������ھ����������²����� 1472 �ֽڣ��ڹ����������²����� 548 �ֽڣ� */
HPSOCKET_API void __HP_CALL HP_UdpClient_SetMaxDatagramSize(HP_UdpClient pClient, DWORD dwMaxDatagramSize);
/* ��ȡ���ݱ�����󳤶� */
HPSOCKET_API DWORD __HP_CALL HP_UdpClient_GetMaxDatagramSize(HP_UdpClient pClient);

/* ���ü������Դ�����0 �򲻷��ͼ�������������������Դ�������Ϊ�Ѷ��ߣ� */
HPSOCKET_API void __HP_CALL HP_UdpClient_SetDetectAttempts(HP_UdpClient pClient, DWORD dwDetectAttempts);
/* ���ü������ͼ�����룬0 �����ͼ����� */
HPSOCKET_API void __HP_CALL HP_UdpClient_SetDetectInterval(HP_UdpClient pClient, DWORD dwDetectInterval);
/* ��ȡ���������� */
HPSOCKET_API DWORD __HP_CALL HP_UdpClient_GetDetectAttempts(HP_UdpClient pClient);
/* ��ȡ��������� */
HPSOCKET_API DWORD __HP_CALL HP_UdpClient_GetDetectInterval(HP_UdpClient pClient);

/**********************************************************************************/
/****************************** UDP Cast ���Է��ʷ��� ******************************/

/* �������ݱ�����󳤶ȣ������ھ����������²����� 1472 �ֽڣ��ڹ����������²����� 548 �ֽڣ� */
HPSOCKET_API void __HP_CALL HP_UdpCast_SetMaxDatagramSize(HP_UdpCast pCast, DWORD dwMaxDatagramSize);
/* ��ȡ���ݱ�����󳤶� */
HPSOCKET_API DWORD __HP_CALL HP_UdpCast_GetMaxDatagramSize(HP_UdpCast pCast);
/* ��ȡ��ǰ���ݱ���Զ�̵�ַ��Ϣ��ͨ���� OnReceive �¼��е��ã� */
HPSOCKET_API BOOL __HP_CALL HP_UdpCast_GetRemoteAddress(HP_UdpCast pCast, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort);
/* �����Ƿ����õ�ַ���û��ƣ�Ĭ�ϣ������ã� */
HPSOCKET_API void __HP_CALL HP_UdpCast_SetReuseAddress(HP_UdpCast pCast, BOOL bReuseAddress);
/* ����Ƿ����õ�ַ���û��� */
HPSOCKET_API BOOL __HP_CALL HP_UdpCast_IsReuseAddress(HP_UdpCast pCast);
/* ���ô���ģʽ���鲥��㲥�� */
HPSOCKET_API void __HP_CALL HP_UdpCast_SetCastMode(HP_UdpCast pCast, En_HP_CastMode enCastMode);
/* ��ȡ����ģʽ */
HPSOCKET_API En_HP_CastMode __HP_CALL HP_UdpCast_GetCastMode(HP_UdpCast pCast);
/* �����鲥���ĵ� TTL��0 - 255�� */
HPSOCKET_API void __HP_CALL HP_UdpCast_SetMultiCastTtl(HP_UdpCast pCast, int iMCTtl);
/* ��ȡ�鲥���ĵ� TTL */
HPSOCKET_API int __HP_CALL HP_UdpCast_GetMultiCastTtl(HP_UdpCast pCast);
/* �����Ƿ������鲥��·��TRUE or FALSE�� */
HPSOCKET_API void __HP_CALL HP_UdpCast_SetMultiCastLoop(HP_UdpCast pCast, BOOL bMCLoop);
/* ����Ƿ������鲥��· */
HPSOCKET_API BOOL __HP_CALL HP_UdpCast_IsMultiCastLoop(HP_UdpCast pCast);

/***************************************************************************************/
/***************************** TCP Pull Server ����������� *****************************/

/*
* ���ƣ�ץȡ����
* �������û�ͨ���÷����� Socket �����ץȡ����
*		
* ������		dwConnID	-- ���� ID
*			pData		-- ץȡ������
*			iLength		-- ץȡ���ݳ���
* ����ֵ��	En_HP_FetchResult
*/
HPSOCKET_API En_HP_FetchResult __HP_CALL HP_TcpPullServer_Fetch(HP_TcpPullServer pServer, HP_CONNID dwConnID, BYTE* pData, int iLength);

/*
* ���ƣ���̽���ݣ������Ƴ����������ݣ�
* �������û�ͨ���÷����� Socket ����п�̽����
*		
* ������		dwConnID	-- ���� ID
*			pData		-- ��̽������
*			iLength		-- ��̽���ݳ���
* ����ֵ��	En_HP_FetchResult
*/
HPSOCKET_API En_HP_FetchResult __HP_CALL HP_TcpPullServer_Peek(HP_TcpPullServer pServer, HP_CONNID dwConnID, BYTE* pData, int iLength);

/***************************************************************************************/
/***************************** TCP Pull Server ���Է��ʷ��� *****************************/

/***************************************************************************************/
/***************************** TCP Pull Agent ����������� *****************************/

/*
* ���ƣ�ץȡ����
* �������û�ͨ���÷����� Socket �����ץȡ����
*		
* ������		dwConnID	-- ���� ID
*			pData		-- ץȡ������
*			iLength		-- ץȡ���ݳ���
* ����ֵ��	En_HP_FetchResult
*/
HPSOCKET_API En_HP_FetchResult __HP_CALL HP_TcpPullAgent_Fetch(HP_TcpPullAgent pAgent, HP_CONNID dwConnID, BYTE* pData, int iLength);

/*
* ���ƣ���̽���ݣ������Ƴ����������ݣ�
* �������û�ͨ���÷����� Socket ����п�̽����
*		
* ������		dwConnID	-- ���� ID
*			pData		-- ��̽������
*			iLength		-- ��̽���ݳ���
* ����ֵ��	En_HP_FetchResult
*/
HPSOCKET_API En_HP_FetchResult __HP_CALL HP_TcpPullAgent_Peek(HP_TcpPullAgent pAgent, HP_CONNID dwConnID, BYTE* pData, int iLength);

/***************************************************************************************/
/***************************** TCP Pull Agent ���Է��ʷ��� *****************************/

/***************************************************************************************/
/***************************** TCP Pull Client ����������� *****************************/

/*
* ���ƣ�ץȡ����
* �������û�ͨ���÷����� Socket �����ץȡ����
*		
* ������		dwConnID	-- ���� ID
*			pData		-- ץȡ������
*			iLength		-- ץȡ���ݳ���
* ����ֵ��	En_HP_FetchResult
*/
HPSOCKET_API En_HP_FetchResult __HP_CALL HP_TcpPullClient_Fetch(HP_TcpPullClient pClient, BYTE* pData, int iLength);

/*
* ���ƣ���̽���ݣ������Ƴ����������ݣ�
* �������û�ͨ���÷����� Socket ����п�̽����
*		
* ������		dwConnID	-- ���� ID
*			pData		-- ��̽������
*			iLength		-- ��̽���ݳ���
* ����ֵ��	En_HP_FetchResult
*/
HPSOCKET_API En_HP_FetchResult __HP_CALL HP_TcpPullClient_Peek(HP_TcpPullClient pClient, BYTE* pData, int iLength);

/***************************************************************************************/
/***************************** TCP Pull Client ���Է��ʷ��� *****************************/

/***************************************************************************************/
/***************************** TCP Pack Server ����������� *****************************/

/***************************************************************************************/
/***************************** TCP Pack Server ���Է��ʷ��� *****************************/

/* �������ݰ���󳤶ȣ���Ч���ݰ���󳤶Ȳ��ܳ��� 4194303/0x3FFFFF �ֽڣ�Ĭ�ϣ�262144/0x40000�� */
HPSOCKET_API void __HP_CALL HP_TcpPackServer_SetMaxPackSize(HP_TcpPackServer pServer, DWORD dwMaxPackSize);
/* ���ð�ͷ��ʶ����Ч��ͷ��ʶȡֵ��Χ 0 ~ 1023/0x3FF������ͷ��ʶΪ 0 ʱ��У���ͷ��Ĭ�ϣ�0�� */
HPSOCKET_API void __HP_CALL HP_TcpPackServer_SetPackHeaderFlag(HP_TcpPackServer pServer, USHORT usPackHeaderFlag);

/* ��ȡ���ݰ���󳤶� */
HPSOCKET_API DWORD __HP_CALL HP_TcpPackServer_GetMaxPackSize(HP_TcpPackServer pServer);
/* ��ȡ��ͷ��ʶ */
HPSOCKET_API USHORT __HP_CALL HP_TcpPackServer_GetPackHeaderFlag(HP_TcpPackServer pServer);

/***************************************************************************************/
/***************************** TCP Pack Agent ����������� *****************************/

/***************************************************************************************/
/***************************** TCP Pack Agent ���Է��ʷ��� *****************************/

/* �������ݰ���󳤶ȣ���Ч���ݰ���󳤶Ȳ��ܳ��� 4194303/0x3FFFFF �ֽڣ�Ĭ�ϣ�262144/0x40000�� */
HPSOCKET_API void __HP_CALL HP_TcpPackAgent_SetMaxPackSize(HP_TcpPackAgent pAgent, DWORD dwMaxPackSize);
/* ���ð�ͷ��ʶ����Ч��ͷ��ʶȡֵ��Χ 0 ~ 1023/0x3FF������ͷ��ʶΪ 0 ʱ��У���ͷ��Ĭ�ϣ�0�� */
HPSOCKET_API void __HP_CALL HP_TcpPackAgent_SetPackHeaderFlag(HP_TcpPackAgent pAgent, USHORT usPackHeaderFlag);

/* ��ȡ���ݰ���󳤶� */
HPSOCKET_API DWORD __HP_CALL HP_TcpPackAgent_GetMaxPackSize(HP_TcpPackAgent pAgent);
/* ��ȡ��ͷ��ʶ */
HPSOCKET_API USHORT __HP_CALL HP_TcpPackAgent_GetPackHeaderFlag(HP_TcpPackAgent pAgent);

/***************************************************************************************/
/***************************** TCP Pack Client ����������� *****************************/

/***************************************************************************************/
/***************************** TCP Pack Client ���Է��ʷ��� *****************************/

/* �������ݰ���󳤶ȣ���Ч���ݰ���󳤶Ȳ��ܳ��� 4194303/0x3FFFFF �ֽڣ�Ĭ�ϣ�262144/0x40000�� */
HPSOCKET_API void __HP_CALL HP_TcpPackClient_SetMaxPackSize(HP_TcpPackClient pClient, DWORD dwMaxPackSize);
/* ���ð�ͷ��ʶ����Ч��ͷ��ʶȡֵ��Χ 0 ~ 1023/0x3FF������ͷ��ʶΪ 0 ʱ��У���ͷ��Ĭ�ϣ�0�� */
HPSOCKET_API void __HP_CALL HP_TcpPackClient_SetPackHeaderFlag(HP_TcpPackClient pClient, USHORT usPackHeaderFlag);

/* ��ȡ���ݰ���󳤶� */
HPSOCKET_API DWORD __HP_CALL HP_TcpPackClient_GetMaxPackSize(HP_TcpPackClient pClient);
/* ��ȡ��ͷ��ʶ */
HPSOCKET_API USHORT __HP_CALL HP_TcpPackClient_GetPackHeaderFlag(HP_TcpPackClient pClient);

/*****************************************************************************************************************************************************/
/*************************************************************** Global Function Exports *************************************************************/
/*****************************************************************************************************************************************************/

// ��ȡ HPSocket �汾�ţ�4 ���ֽڷֱ�Ϊ�����汾�ţ��Ӱ汾�ţ������汾�ţ�������ţ�
HPSOCKET_API DWORD __HP_CALL HP_GetHPSocketVersion();

/* ��ȡ���������ı� */
HPSOCKET_API LPCTSTR __HP_CALL HP_GetSocketErrorDesc(En_HP_SocketError enCode);
// ����ϵͳ�� errno ������ȡϵͳ�������
HPSOCKET_API DWORD __HP_CALL SYS_GetLastError();
// ����ϵͳ�� strerror() ������ȡϵͳ�����������
HPSOCKET_API LPCSTR __HP_CALL SYS_GetLastErrorStr();
// ����ϵͳ�� setsockopt()
HPSOCKET_API int __HP_CALL SYS_SetSocketOption(SOCKET sock, int level, int name, LPVOID val, int len);
// ����ϵͳ�� getsockopt()
HPSOCKET_API int __HP_CALL SYS_GetSocketOption(SOCKET sock, int level, int name, LPVOID val, int* len);
// ����ϵͳ�� ioctlsocket()
HPSOCKET_API int __HP_CALL SYS_IoctlSocket(SOCKET sock, long cmd, ULONG* arg);

// ����ϵͳ�� fcntl() ���� F_SETFL ����
HPSOCKET_API BOOL __HP_CALL SYS_fcntl_SETFL(FD fd, INT fl, BOOL bSet);

// ���� FD ѡ�O_NONBLOCK
HPSOCKET_API int __HP_CALL SYS_SSO_NoBlock(SOCKET sock, BOOL bNoBlock);
// ���� socket ѡ�IPPROTO_TCP -> TCP_NODELAY
HPSOCKET_API int __HP_CALL SYS_SSO_NoDelay(SOCKET sock, BOOL bNoDelay);
// ���� socket ѡ�SOL_SOCKET -> SO_DONTLINGER
HPSOCKET_API int __HP_CALL SYS_SSO_DontLinger(SOCKET sock, BOOL bDont);
// ���� socket ѡ�SOL_SOCKET -> SO_LINGER
HPSOCKET_API int __HP_CALL SYS_SSO_Linger(SOCKET sock, USHORT l_onoff, USHORT l_linger);
// ���� socket ѡ�SOL_SOCKET -> SO_RCVBUF
HPSOCKET_API int __HP_CALL SYS_SSO_RecvBuffSize(SOCKET sock, int size);
// ���� socket ѡ�SOL_SOCKET -> SO_SNDBUF
HPSOCKET_API int __HP_CALL SYS_SSO_SendBuffSize(SOCKET sock, int size);
// ���� socket ѡ�SOL_SOCKET -> SO_REUSEADDR
HPSOCKET_API int __HP_CALL SYS_SSO_ReuseAddress(SOCKET sock, BOOL bReuse);

// ��ȡ SOCKET ���ص�ַ��Ϣ
HPSOCKET_API BOOL __HP_CALL SYS_GetSocketLocalAddress(SOCKET socket, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort);
// ��ȡ SOCKET Զ�̵�ַ��Ϣ
HPSOCKET_API BOOL __HP_CALL SYS_GetSocketRemoteAddress(SOCKET socket, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort);

/* ö������ IP ��ַ */
HPSOCKET_API BOOL __HP_CALL SYS_EnumHostIPAddresses(LPCTSTR lpszHost, En_HP_IPAddrType enType, HP_LPTIPAddr** lpppIPAddr, int* piIPAddrCount);
/* �ͷ� HP_LPTIPAddr* */
HPSOCKET_API BOOL __HP_CALL SYS_FreeHostIPAddresses(HP_LPTIPAddr* lppIPAddr);
/* ����ַ����Ƿ���� IP ��ַ��ʽ */
HPSOCKET_API BOOL __HP_CALL SYS_IsIPAddress(LPCTSTR lpszAddress, En_HP_IPAddrType* penType);
/* ͨ����������ȡ IP ��ַ */
HPSOCKET_API BOOL __HP_CALL SYS_GetIPAddress(LPCTSTR lpszHost, TCHAR lpszIP[], int* piIPLenth, En_HP_IPAddrType* penType);

/* 64 λ�����ֽ���ת�����ֽ��� */
HPSOCKET_API ULONGLONG __HP_CALL SYS_NToH64(ULONGLONG value);
/* 64 λ�����ֽ���ת�����ֽ��� */
HPSOCKET_API ULONGLONG __HP_CALL SYS_HToN64(ULONGLONG value);

/* �����ڴ� */
HPSOCKET_API LPBYTE __HP_CALL SYS_Malloc(int size);
/* ���·����ڴ� */
HPSOCKET_API LPBYTE __HP_CALL SYS_Realloc(LPBYTE p, int size);
/* �ͷ��ڴ� */
HPSOCKET_API VOID __HP_CALL SYS_Free(LPBYTE p);

#ifdef _ICONV_SUPPORT

// Charset A -> Charset B
HPSOCKET_API BOOL __HP_CALL SYS_CharsetConvert(LPCSTR lpszFromCharset, LPCSTR lpszToCharset, LPCSTR lpszInBuf, int iInBufLen, LPSTR lpszOutBuf, int* piOutBufLen);
// GBK -> UNICODE
HPSOCKET_API BOOL __HP_CALL SYS_GbkToUnicode(const char szSrc[], WCHAR szDest[], int* piDestLength);
// UNICODE -> GBK
HPSOCKET_API BOOL __HP_CALL SYS_UnicodeToGbk(const WCHAR szSrc[], char szDest[], int* piDestLength);
// UTF8 -> UNICODE
HPSOCKET_API BOOL __HP_CALL SYS_Utf8ToUnicode(const char szSrc[], WCHAR szDest[], int* piDestLength);
// UNICODE -> UTF8
HPSOCKET_API BOOL __HP_CALL SYS_UnicodeToUtf8(const WCHAR szSrc[], char szDest[], int* piDestLength);
// GBK -> UTF8
HPSOCKET_API BOOL __HP_CALL SYS_GbkToUtf8(const char szSrc[], char szDest[], int* piDestLength);
// UTF8 -> GBK
HPSOCKET_API BOOL __HP_CALL SYS_Utf8ToGbk(const char szSrc[], char szDest[], int* piDestLength);

#endif

#ifdef _ZLIB_SUPPORT

// ��ͨѹ��
HPSOCKET_API int __HP_CALL SYS_Compress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen);
// �߼�ѹ����Ĭ��ֵ��iLevel -> -1��iMethod -> 8��iWindowBits -> 15��iMemLevel -> 8��iStrategy -> 0��
HPSOCKET_API int __HP_CALL SYS_CompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen, int iLevel, int iMethod, int iWindowBits, int iMemLevel, int iStrategy);
// ��ͨ��ѹ
HPSOCKET_API int __HP_CALL SYS_Uncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen);
// �߼���ѹ��Ĭ��ֵ��iWindowBits -> 15��
HPSOCKET_API int __HP_CALL SYS_UncompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen, int iWindowBits);
// �Ʋ�ѹ���������
HPSOCKET_API DWORD __HP_CALL SYS_GuessCompressBound(DWORD dwSrcLen, BOOL bGZip);

// Gzip ѹ��
HPSOCKET_API int __HP_CALL SYS_GZipCompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen);
// Gzip ��ѹ
HPSOCKET_API int __HP_CALL SYS_GZipUncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen);
// �Ʋ� Gzip ��ѹ������ȣ�������� 0 �򲻺���ֵ��˵���������ݲ�����Ч�� Gzip ��ʽ��
HPSOCKET_API DWORD __HP_CALL SYS_GZipGuessUncompressBound(const BYTE* lpszSrc, DWORD dwSrcLen);

#endif

// ���� Base64 ����󳤶�
HPSOCKET_API DWORD __HP_CALL SYS_GuessBase64EncodeBound(DWORD dwSrcLen);
// ���� Base64 ����󳤶�
HPSOCKET_API DWORD __HP_CALL SYS_GuessBase64DecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen);
// Base64 ���루����ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
HPSOCKET_API int __HP_CALL SYS_Base64Encode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen);
// Base64 ���루����ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
HPSOCKET_API int __HP_CALL SYS_Base64Decode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen);

// ���� URL ����󳤶�
HPSOCKET_API DWORD __HP_CALL SYS_GuessUrlEncodeBound(const BYTE* lpszSrc, DWORD dwSrcLen);
// ���� URL ����󳤶�
HPSOCKET_API DWORD __HP_CALL SYS_GuessUrlDecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen);
// URL ���루����ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
HPSOCKET_API int __HP_CALL SYS_UrlEncode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen);
// URL ���루����ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
HPSOCKET_API int __HP_CALL SYS_UrlDecode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen);

/*****************************************************************************************************************************************************/
/******************************************************************** HTTP Exports *******************************************************************/
/*****************************************************************************************************************************************************/

#ifdef _HTTP_SUPPORT

/****************************************************/
/******************* HTTP �ص����� *******************/

/* HTTP �ص����� */
typedef En_HP_HttpParseResult (__HP_CALL *HP_FN_Http_OnMessageBegin)	(HP_Http pSender, HP_CONNID dwConnID);
typedef En_HP_HttpParseResult (__HP_CALL *HP_FN_Http_OnRequestLine)		(HP_Http pSender, HP_CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl);
typedef En_HP_HttpParseResult (__HP_CALL *HP_FN_Http_OnStatusLine)		(HP_Http pSender, HP_CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc);
typedef En_HP_HttpParseResult (__HP_CALL *HP_FN_Http_OnHeader)			(HP_Http pSender, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue);
typedef En_HP_HttpParseResult (__HP_CALL *HP_FN_Http_OnHeadersComplete)	(HP_Http pSender, HP_CONNID dwConnID);
typedef En_HP_HttpParseResult (__HP_CALL *HP_FN_Http_OnBody)			(HP_Http pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
typedef En_HP_HttpParseResult (__HP_CALL *HP_FN_Http_OnChunkHeader)		(HP_Http pSender, HP_CONNID dwConnID, int iLength);
typedef En_HP_HttpParseResult (__HP_CALL *HP_FN_Http_OnChunkComplete)	(HP_Http pSender, HP_CONNID dwConnID);
typedef En_HP_HttpParseResult (__HP_CALL *HP_FN_Http_OnMessageComplete)	(HP_Http pSender, HP_CONNID dwConnID);
typedef En_HP_HttpParseResult (__HP_CALL *HP_FN_Http_OnUpgrade)			(HP_Http pSender, HP_CONNID dwConnID, En_HP_HttpUpgradeType enUpgradeType);
typedef En_HP_HttpParseResult (__HP_CALL *HP_FN_Http_OnParseError)		(HP_Http pSender, HP_CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc);

typedef En_HP_HandleResult	 (__HP_CALL *HP_FN_Http_OnWSMessageHeader)	(HP_Http pSender, HP_CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen);
typedef En_HP_HandleResult	 (__HP_CALL *HP_FN_Http_OnWSMessageBody)	(HP_Http pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
typedef En_HP_HandleResult	 (__HP_CALL *HP_FN_Http_OnWSMessageComplete)(HP_Http pSender, HP_CONNID dwConnID);

/* HTTP Server �ص����� */
typedef HP_FN_Http_OnMessageBegin			HP_FN_HttpServer_OnMessageBegin;
typedef HP_FN_Http_OnRequestLine			HP_FN_HttpServer_OnRequestLine;
typedef HP_FN_Http_OnHeader					HP_FN_HttpServer_OnHeader;
typedef HP_FN_Http_OnHeadersComplete		HP_FN_HttpServer_OnHeadersComplete;
typedef HP_FN_Http_OnBody					HP_FN_HttpServer_OnBody;
typedef HP_FN_Http_OnChunkHeader			HP_FN_HttpServer_OnChunkHeader;
typedef HP_FN_Http_OnChunkComplete			HP_FN_HttpServer_OnChunkComplete;
typedef HP_FN_Http_OnMessageComplete		HP_FN_HttpServer_OnMessageComplete;
typedef HP_FN_Http_OnUpgrade				HP_FN_HttpServer_OnUpgrade;
typedef HP_FN_Http_OnParseError				HP_FN_HttpServer_OnParseError;

typedef HP_FN_Http_OnWSMessageHeader		HP_FN_HttpServer_OnWSMessageHeader;
typedef HP_FN_Http_OnWSMessageBody			HP_FN_HttpServer_OnWSMessageBody;
typedef HP_FN_Http_OnWSMessageComplete		HP_FN_HttpServer_OnWSMessageComplete;

typedef HP_FN_Server_OnPrepareListen		HP_FN_HttpServer_OnPrepareListen;
typedef HP_FN_Server_OnAccept				HP_FN_HttpServer_OnAccept;
typedef HP_FN_Server_OnHandShake			HP_FN_HttpServer_OnHandShake;
typedef HP_FN_Server_OnReceive				HP_FN_HttpServer_OnReceive;
typedef HP_FN_Server_OnSend					HP_FN_HttpServer_OnSend;
typedef HP_FN_Server_OnClose				HP_FN_HttpServer_OnClose;
typedef HP_FN_Server_OnShutdown				HP_FN_HttpServer_OnShutdown;

/* HTTP Agent �ص����� */
typedef HP_FN_Http_OnMessageBegin			HP_FN_HttpAgent_OnMessageBegin;
typedef HP_FN_Http_OnStatusLine				HP_FN_HttpAgent_OnStatusLine;
typedef HP_FN_Http_OnHeader					HP_FN_HttpAgent_OnHeader;
typedef HP_FN_Http_OnHeadersComplete		HP_FN_HttpAgent_OnHeadersComplete;
typedef HP_FN_Http_OnBody					HP_FN_HttpAgent_OnBody;
typedef HP_FN_Http_OnChunkHeader			HP_FN_HttpAgent_OnChunkHeader;
typedef HP_FN_Http_OnChunkComplete			HP_FN_HttpAgent_OnChunkComplete;
typedef HP_FN_Http_OnMessageComplete		HP_FN_HttpAgent_OnMessageComplete;
typedef HP_FN_Http_OnUpgrade				HP_FN_HttpAgent_OnUpgrade;
typedef HP_FN_Http_OnParseError				HP_FN_HttpAgent_OnParseError;

typedef HP_FN_Http_OnWSMessageHeader		HP_FN_HttpAgent_OnWSMessageHeader;
typedef HP_FN_Http_OnWSMessageBody			HP_FN_HttpAgent_OnWSMessageBody;
typedef HP_FN_Http_OnWSMessageComplete		HP_FN_HttpAgent_OnWSMessageComplete;

typedef HP_FN_Agent_OnPrepareConnect		HP_FN_HttpAgent_OnPrepareConnect;
typedef HP_FN_Agent_OnConnect				HP_FN_HttpAgent_OnConnect;
typedef HP_FN_Agent_OnHandShake				HP_FN_HttpAgent_OnHandShake;
typedef HP_FN_Agent_OnReceive				HP_FN_HttpAgent_OnReceive;
typedef HP_FN_Agent_OnSend					HP_FN_HttpAgent_OnSend;
typedef HP_FN_Agent_OnClose					HP_FN_HttpAgent_OnClose;
typedef HP_FN_Agent_OnShutdown				HP_FN_HttpAgent_OnShutdown;

/* HTTP Client �ص����� */
typedef HP_FN_Http_OnMessageBegin			HP_FN_HttpClient_OnMessageBegin;
typedef HP_FN_Http_OnStatusLine				HP_FN_HttpClient_OnStatusLine;
typedef HP_FN_Http_OnHeader					HP_FN_HttpClient_OnHeader;
typedef HP_FN_Http_OnHeadersComplete		HP_FN_HttpClient_OnHeadersComplete;
typedef HP_FN_Http_OnBody					HP_FN_HttpClient_OnBody;
typedef HP_FN_Http_OnChunkHeader			HP_FN_HttpClient_OnChunkHeader;
typedef HP_FN_Http_OnChunkComplete			HP_FN_HttpClient_OnChunkComplete;
typedef HP_FN_Http_OnMessageComplete		HP_FN_HttpClient_OnMessageComplete;
typedef HP_FN_Http_OnUpgrade				HP_FN_HttpClient_OnUpgrade;
typedef HP_FN_Http_OnParseError				HP_FN_HttpClient_OnParseError;

typedef HP_FN_Http_OnWSMessageHeader		HP_FN_HttpClient_OnWSMessageHeader;
typedef HP_FN_Http_OnWSMessageBody			HP_FN_HttpClient_OnWSMessageBody;
typedef HP_FN_Http_OnWSMessageComplete		HP_FN_HttpClient_OnWSMessageComplete;

typedef HP_FN_Client_OnPrepareConnect		HP_FN_HttpClient_OnPrepareConnect;
typedef HP_FN_Client_OnConnect				HP_FN_HttpClient_OnConnect;
typedef HP_FN_Client_OnHandShake			HP_FN_HttpClient_OnHandShake;
typedef HP_FN_Client_OnReceive				HP_FN_HttpClient_OnReceive;
typedef HP_FN_Client_OnSend					HP_FN_HttpClient_OnSend;
typedef HP_FN_Client_OnClose				HP_FN_HttpClient_OnClose;

/****************************************************/
/***************** HTTP ���󴴽����� *****************/

// ���� HP_HttpServer ����
HPSOCKET_API HP_HttpServer __HP_CALL Create_HP_HttpServer(HP_HttpServerListener pListener);
// ���� HP_HttpAgent ����
HPSOCKET_API HP_HttpAgent __HP_CALL Create_HP_HttpAgent(HP_HttpAgentListener pListener);
// ���� HP_HttpClient ����
HPSOCKET_API HP_HttpClient __HP_CALL Create_HP_HttpClient(HP_HttpClientListener pListener);
// ���� HP_HttpSyncClient ����
HPSOCKET_API HP_HttpSyncClient __HP_CALL Create_HP_HttpSyncClient(HP_HttpClientListener pListener);

// ���� HP_HttpServer ����
HPSOCKET_API void __HP_CALL Destroy_HP_HttpServer(HP_HttpServer pServer);
// ���� HP_HttpAgent ����
HPSOCKET_API void __HP_CALL Destroy_HP_HttpAgent(HP_HttpAgent pAgent);
// ���� HP_HttpClient ����
HPSOCKET_API void __HP_CALL Destroy_HP_HttpClient(HP_HttpClient pClient);
// ���� HP_HttpSyncClient ����
HPSOCKET_API void __HP_CALL Destroy_HP_HttpSyncClient(HP_HttpSyncClient pClient);

// ���� HP_HttpServerListener ����
HPSOCKET_API HP_HttpServerListener __HP_CALL Create_HP_HttpServerListener();
// ���� HP_HttpAgentListener ����
HPSOCKET_API HP_HttpAgentListener __HP_CALL Create_HP_HttpAgentListener();
// ���� HP_HttpClientListener ����
HPSOCKET_API HP_HttpClientListener __HP_CALL Create_HP_HttpClientListener();

// ���� HP_HttpServerListener ����
HPSOCKET_API void __HP_CALL Destroy_HP_HttpServerListener(HP_HttpServerListener pListener);
// ���� HP_HttpAgentListener ����
HPSOCKET_API void __HP_CALL Destroy_HP_HttpAgentListener(HP_HttpAgentListener pListener);
// ���� HP_HttpClientListener ����
HPSOCKET_API void __HP_CALL Destroy_HP_HttpClientListener(HP_HttpClientListener pListener);

/**********************************************************************************/
/*************************** HTTP Server �ص��������÷��� **************************/

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnMessageBegin(HP_HttpServerListener pListener		, HP_FN_HttpServer_OnMessageBegin fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnRequestLine(HP_HttpServerListener pListener		, HP_FN_HttpServer_OnRequestLine fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnHeader(HP_HttpServerListener pListener			, HP_FN_HttpServer_OnHeader fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnHeadersComplete(HP_HttpServerListener pListener	, HP_FN_HttpServer_OnHeadersComplete fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnBody(HP_HttpServerListener pListener				, HP_FN_HttpServer_OnBody fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnChunkHeader(HP_HttpServerListener pListener		, HP_FN_HttpServer_OnChunkHeader fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnChunkComplete(HP_HttpServerListener pListener	, HP_FN_HttpServer_OnChunkComplete fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnMessageComplete(HP_HttpServerListener pListener	, HP_FN_HttpServer_OnMessageComplete fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnUpgrade(HP_HttpServerListener pListener			, HP_FN_HttpServer_OnUpgrade fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnParseError(HP_HttpServerListener pListener		, HP_FN_HttpServer_OnParseError fn);

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnWSMessageHeader(HP_HttpServerListener pListener	, HP_FN_HttpServer_OnWSMessageHeader fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnWSMessageBody(HP_HttpServerListener pListener	, HP_FN_HttpServer_OnWSMessageBody fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnWSMessageComplete(HP_HttpServerListener pListener, HP_FN_HttpServer_OnWSMessageComplete fn);

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnPrepareListen(HP_HttpServerListener pListener	, HP_FN_HttpServer_OnPrepareListen fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnAccept(HP_HttpServerListener pListener			, HP_FN_HttpServer_OnAccept fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnHandShake(HP_HttpServerListener pListener		, HP_FN_HttpServer_OnHandShake fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnReceive(HP_HttpServerListener pListener			, HP_FN_HttpServer_OnReceive fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnSend(HP_HttpServerListener pListener				, HP_FN_HttpServer_OnSend fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnClose(HP_HttpServerListener pListener			, HP_FN_HttpServer_OnClose fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnShutdown(HP_HttpServerListener pListener			, HP_FN_HttpServer_OnShutdown fn);

/**********************************************************************************/
/**************************** HTTP Agent �ص��������÷��� **************************/

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnMessageBegin(HP_HttpAgentListener pListener		, HP_FN_HttpAgent_OnMessageBegin fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnStatusLine(HP_HttpAgentListener pListener			, HP_FN_HttpAgent_OnStatusLine fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnHeader(HP_HttpAgentListener pListener				, HP_FN_HttpAgent_OnHeader fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnHeadersComplete(HP_HttpAgentListener pListener	, HP_FN_HttpAgent_OnHeadersComplete fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnBody(HP_HttpAgentListener pListener				, HP_FN_HttpAgent_OnBody fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnChunkHeader(HP_HttpAgentListener pListener		, HP_FN_HttpAgent_OnChunkHeader fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnChunkComplete(HP_HttpAgentListener pListener		, HP_FN_HttpAgent_OnChunkComplete fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnMessageComplete(HP_HttpAgentListener pListener	, HP_FN_HttpAgent_OnMessageComplete fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnUpgrade(HP_HttpAgentListener pListener			, HP_FN_HttpAgent_OnUpgrade fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnParseError(HP_HttpAgentListener pListener			, HP_FN_HttpAgent_OnParseError fn);

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnWSMessageHeader(HP_HttpAgentListener pListener	, HP_FN_HttpAgent_OnWSMessageHeader fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnWSMessageBody(HP_HttpAgentListener pListener		, HP_FN_HttpAgent_OnWSMessageBody fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnWSMessageComplete(HP_HttpAgentListener pListener	, HP_FN_HttpAgent_OnWSMessageComplete fn);

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnPrepareConnect(HP_HttpAgentListener pListener		, HP_FN_HttpAgent_OnPrepareConnect fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnConnect(HP_HttpAgentListener pListener			, HP_FN_HttpAgent_OnConnect fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnHandShake(HP_HttpAgentListener pListener			, HP_FN_HttpAgent_OnHandShake fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnReceive(HP_HttpAgentListener pListener			, HP_FN_HttpAgent_OnReceive fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnSend(HP_HttpAgentListener pListener				, HP_FN_HttpAgent_OnSend fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnClose(HP_HttpAgentListener pListener				, HP_FN_HttpAgent_OnClose fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnShutdown(HP_HttpAgentListener pListener			, HP_FN_HttpAgent_OnShutdown fn);

/**********************************************************************************/
/*************************** HTTP Client �ص��������÷��� **************************/

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnMessageBegin(HP_HttpClientListener pListener		, HP_FN_HttpClient_OnMessageBegin fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnStatusLine(HP_HttpClientListener pListener		, HP_FN_HttpClient_OnStatusLine fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnHeader(HP_HttpClientListener pListener			, HP_FN_HttpClient_OnHeader fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnHeadersComplete(HP_HttpClientListener pListener	, HP_FN_HttpClient_OnHeadersComplete fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnBody(HP_HttpClientListener pListener				, HP_FN_HttpClient_OnBody fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnChunkHeader(HP_HttpClientListener pListener		, HP_FN_HttpClient_OnChunkHeader fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnChunkComplete(HP_HttpClientListener pListener	, HP_FN_HttpClient_OnChunkComplete fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnMessageComplete(HP_HttpClientListener pListener	, HP_FN_HttpClient_OnMessageComplete fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnUpgrade(HP_HttpClientListener pListener			, HP_FN_HttpClient_OnUpgrade fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnParseError(HP_HttpClientListener pListener		, HP_FN_HttpClient_OnParseError fn);

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnWSMessageHeader(HP_HttpClientListener pListener	, HP_FN_HttpClient_OnWSMessageHeader fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnWSMessageBody(HP_HttpClientListener pListener	, HP_FN_HttpClient_OnWSMessageBody fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnWSMessageComplete(HP_HttpClientListener pListener, HP_FN_HttpClient_OnWSMessageComplete fn);

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnPrepareConnect(HP_HttpClientListener pListener	, HP_FN_HttpClient_OnPrepareConnect fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnConnect(HP_HttpClientListener pListener			, HP_FN_HttpClient_OnConnect fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnHandShake(HP_HttpClientListener pListener		, HP_FN_HttpClient_OnHandShake fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnReceive(HP_HttpClientListener pListener			, HP_FN_HttpClient_OnReceive fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnSend(HP_HttpClientListener pListener				, HP_FN_HttpClient_OnSend fn);
HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnClose(HP_HttpClientListener pListener			, HP_FN_HttpClient_OnClose fn);

/**************************************************************************/
/*************************** HTTP Server �������� **************************/

/*
* ���ƣ��ظ�����
* ��������ͻ��˻ظ� HTTP ����
*		
* ������		dwConnID		-- ���� ID
*			usStatusCode	-- HTTP ״̬��
*			lpszDesc		-- HTTP ״̬����
*			lpHeaders		-- �ظ�����ͷ
*			iHeaderCount	-- �ظ�����ͷ����
*			pData			-- �ظ�������
*			iLength			-- �ظ������峤��
* ����ֵ��	TRUE			-- �ɹ�
*			FALSE			-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_HttpServer_SendResponse(HP_HttpServer pServer, HP_CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pData, int iLength);

/*
* ���ƣ����ͱ����ļ�
* ��������ָ�����ӷ��� 4096 KB ���µ�С�ļ�
*		
* ������		dwConnID		-- ���� ID
*			lpszFileName	-- �ļ�·��
*			usStatusCode	-- HTTP ״̬��
*			lpszDesc		-- HTTP ״̬����
*			lpHeaders		-- �ظ�����ͷ
*			iHeaderCount	-- �ظ�����ͷ����
* ����ֵ��	TRUE			-- �ɹ�
*			FALSE			-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_HttpServer_SendLocalFile(HP_HttpServer pServer, HP_CONNID dwConnID, LPCSTR lpszFileName, USHORT usStatusCode, LPCSTR lpszDesc, const HP_THeader lpHeaders[], int iHeaderCount);

/*
* ���ƣ����� WebSocket ��Ϣ
* ��������Զ˶˷��� WebSocket ��Ϣ
*		
* ������		dwConnID		-- ���� ID
*			bFinal			-- �Ƿ����֡
*			iReserved		-- RSV1/RSV2/RSV3 �� 1 λ
*			iOperationCode	-- �����룺0x0 - 0xF
*			lpszMask		-- ���루nullptr �� 4 �ֽ����룬���Ϊ nullptr ��û�����룩
*			pData			-- ��Ϣ�����ݻ�����
*			iLength			-- ��Ϣ�����ݳ���
*			ullBodyLen		-- ��Ϣ�ܳ���
* 								ullBodyLen = 0		 -> ��Ϣ�ܳ���Ϊ iLength
* 								ullBodyLen = iLength -> ��Ϣ�ܳ���Ϊ ullBodyLen
* 								ullBodyLen > iLength -> ��Ϣ�ܳ���Ϊ ullBodyLen��������Ϣ�峤��Ϊ ullBOdyLen - iLength��������Ϣ��ͨ���ײ㷽�� Send() / SendPackets() ����
* 								ullBodyLen < iLength -> �������������ʧ��
* ����ֵ��	TRUE			-- �ɹ�
*			FALSE			-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_HttpServer_SendWSMessage(HP_HttpServer pServer, HP_CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], BYTE* pData, int iLength, ULONGLONG ullBodyLen);

/*
* ���ƣ��ͷ�����
* �����������ӷ����ͷŶ��У��ȴ�ĳ��ʱ�䣨ͨ�� SetReleaseDelay() ���ã��ر�����
*		
* ������		dwConnID		-- ���� ID
* ����ֵ��	TRUE			-- �ɹ�
*			FALSE			-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_HttpServer_Release(HP_HttpServer pServer, HP_CONNID dwConnID);

/******************************************************************************/
/*************************** HTTP Server ���Է��ʷ��� **************************/

/* ���������ͷ���ʱ��Ĭ�ϣ�3000 ���룩 */
HPSOCKET_API void __HP_CALL HP_HttpServer_SetReleaseDelay(HP_HttpServer pServer, DWORD dwReleaseDelay);
/* ��ȡ�����ͷ���ʱ */
HPSOCKET_API DWORD __HP_CALL HP_HttpServer_GetReleaseDelay(HP_HttpServer pServer);
/* ��ȡ������ URL �����루URL ��ο���EnHttpUrlField�� */
HPSOCKET_API USHORT __HP_CALL HP_HttpServer_GetUrlFieldSet(HP_HttpServer pServer, HP_CONNID dwConnID);
/* ��ȡĳ�� URL ��ֵ */
HPSOCKET_API LPCSTR __HP_CALL HP_HttpServer_GetUrlField(HP_HttpServer pServer, HP_CONNID dwConnID, En_HP_HttpUrlField enField);
/* ��ȡ���󷽷� */
HPSOCKET_API LPCSTR __HP_CALL HP_HttpServer_GetMethod(HP_HttpServer pServer, HP_CONNID dwConnID);

/* ���ñ���Э��汾 */
HPSOCKET_API void __HP_CALL HP_HttpServer_SetLocalVersion(HP_HttpServer pServer, En_HP_HttpVersion usVersion);
/* ��ȡ����Э��汾 */
HPSOCKET_API En_HP_HttpVersion __HP_CALL HP_HttpServer_GetLocalVersion(HP_HttpServer pServer);

/* ����Ƿ�����Э�� */
HPSOCKET_API BOOL __HP_CALL HP_HttpServer_IsUpgrade(HP_HttpServer pServer, HP_CONNID dwConnID);
/* ����Ƿ��� Keep-Alive ��ʶ */
HPSOCKET_API BOOL __HP_CALL HP_HttpServer_IsKeepAlive(HP_HttpServer pServer, HP_CONNID dwConnID);
/* ��ȡЭ��汾 */
HPSOCKET_API USHORT __HP_CALL HP_HttpServer_GetVersion(HP_HttpServer pServer, HP_CONNID dwConnID);
/* ��ȡ���� */
HPSOCKET_API LPCSTR __HP_CALL HP_HttpServer_GetHost(HP_HttpServer pServer, HP_CONNID dwConnID);
/* ��ȡ���ݳ��� */
HPSOCKET_API ULONGLONG __HP_CALL HP_HttpServer_GetContentLength(HP_HttpServer pServer, HP_CONNID dwConnID);
/* ��ȡ�������� */
HPSOCKET_API LPCSTR __HP_CALL HP_HttpServer_GetContentType(HP_HttpServer pServer, HP_CONNID dwConnID);
/* ��ȡ���ݱ��� */
HPSOCKET_API LPCSTR __HP_CALL HP_HttpServer_GetContentEncoding(HP_HttpServer pServer, HP_CONNID dwConnID);
/* ��ȡ������� */
HPSOCKET_API LPCSTR __HP_CALL HP_HttpServer_GetTransferEncoding(HP_HttpServer pServer, HP_CONNID dwConnID);
/* ��ȡЭ���������� */
HPSOCKET_API En_HP_HttpUpgradeType __HP_CALL HP_HttpServer_GetUpgradeType(HP_HttpServer pServer, HP_CONNID dwConnID);
/* ��ȡ����������� */
HPSOCKET_API USHORT __HP_CALL HP_HttpServer_GetParseErrorCode(HP_HttpServer pServer, HP_CONNID dwConnID, LPCSTR* lpszErrorDesc);

/* ��ȡĳ������ͷ����ֵ�� */
HPSOCKET_API BOOL __HP_CALL HP_HttpServer_GetHeader(HP_HttpServer pServer, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue);
/* ��ȡĳ������ͷ����ֵ�� */
HPSOCKET_API BOOL __HP_CALL HP_HttpServer_GetHeaders(HP_HttpServer pServer, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue[], DWORD* pdwCount);
/* ��ȡ��������ͷ */
HPSOCKET_API BOOL __HP_CALL HP_HttpServer_GetAllHeaders(HP_HttpServer pServer, HP_CONNID dwConnID, HP_THeader lpHeaders[], DWORD* pdwCount);
/* ��ȡ��������ͷ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpServer_GetAllHeaderNames(HP_HttpServer pServer, HP_CONNID dwConnID, LPCSTR lpszName[], DWORD* pdwCount);

/* ��ȡ Cookie */
HPSOCKET_API BOOL __HP_CALL HP_HttpServer_GetCookie(HP_HttpServer pServer, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue);
/* ��ȡ���� Cookie */
HPSOCKET_API BOOL __HP_CALL HP_HttpServer_GetAllCookies(HP_HttpServer pServer, HP_CONNID dwConnID, HP_TCookie lpCookies[], DWORD* pdwCount);

/* ��ȡ��ǰ WebSocket ��Ϣ״̬������ nullptr �򲻻�ȡ��Ӧ�ֶ� */
HPSOCKET_API BOOL __HP_CALL HP_HttpServer_GetWSMessageState(HP_HttpServer pServer, HP_CONNID dwConnID, BOOL* lpbFinal, BYTE* lpiReserved, BYTE* lpiOperationCode, LPCBYTE* lpszMask, ULONGLONG* lpullBodyLen, ULONGLONG* lpullBodyRemain);

/**************************************************************************/
/*************************** HTTP Agent �������� ***************************/

/*
* ���ƣ���������
* �����������˷��� HTTP ����
*		
* ������		dwConnID		-- ���� ID
*			lpszMethod		-- ���󷽷�
*			lpszPath		-- ����·��
*			lpHeaders		-- ����ͷ
*			iHeaderCount	-- ����ͷ����
*			pBody			-- ������
*			iLength			-- �����峤��
* ����ֵ��	TRUE			-- �ɹ�
*			FALSE			-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendRequest(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pData, int iLength);

/*
* ���ƣ����ͱ����ļ�
* ��������ָ�����ӷ��� 4096 KB ���µ�С�ļ�
*		
* ������		dwConnID		-- ���� ID
*			lpszFileName	-- �ļ�·��
*			lpszMethod		-- ���󷽷�
*			lpszPath		-- ����·��
*			lpHeaders		-- ����ͷ
*			iHeaderCount	-- ����ͷ����
* ����ֵ��	TRUE			-- �ɹ�
*			FALSE			-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendLocalFile(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszFileName, LPCSTR lpszMethod, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount);

/* ���� POST ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendPost(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength);
/* ���� PUT ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendPut(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength);
/* ���� PATCH ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendPatch(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength);
/* ���� GET ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendGet(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount);
/* ���� DELETE ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendDelete(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount);
/* ���� HEAD ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendHead(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount);
/* ���� TRACE ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendTrace(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount);
/* ���� OPTIONS ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendOptions(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount);
/* ���� CONNECT ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendConnect(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszHost, const HP_THeader lpHeaders[], int iHeaderCount);

/*
* ���ƣ����� WebSocket ��Ϣ
* ��������Զ˶˷��� WebSocket ��Ϣ
*		
* ������		dwConnID		-- ���� ID
*			bFinal			-- �Ƿ����֡
*			iReserved		-- RSV1/RSV2/RSV3 �� 1 λ
*			iOperationCode	-- �����룺0x0 - 0xF
*			lpszMask		-- ���루nullptr �� 4 �ֽ����룬���Ϊ nullptr ��û�����룩
*			pData			-- ��Ϣ�����ݻ�����
*			iLength			-- ��Ϣ�����ݳ���
*			ullBodyLen		-- ��Ϣ�ܳ���
* 								ullBodyLen = 0		 -> ��Ϣ�ܳ���Ϊ iLength
* 								ullBodyLen = iLength -> ��Ϣ�ܳ���Ϊ ullBodyLen
* 								ullBodyLen > iLength -> ��Ϣ�ܳ���Ϊ ullBodyLen��������Ϣ�峤��Ϊ ullBOdyLen - iLength��������Ϣ��ͨ���ײ㷽�� Send() / SendPackets() ����
* 								ullBodyLen < iLength -> �������������ʧ��
* ����ֵ��	TRUE			-- �ɹ�
*			FALSE			-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendWSMessage(HP_HttpAgent pAgent, HP_CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], BYTE* pData, int iLength, ULONGLONG ullBodyLen);

/******************************************************************************/
/*************************** HTTP Agent ���Է��ʷ��� ***************************/

/* ��ȡ HTTP ״̬�� */
HPSOCKET_API USHORT __HP_CALL HP_HttpAgent_GetStatusCode(HP_HttpAgent pAgent, HP_CONNID dwConnID);

/* ���ñ���Э��汾 */
HPSOCKET_API void __HP_CALL HP_HttpAgent_SetLocalVersion(HP_HttpAgent pAgent, En_HP_HttpVersion usVersion);
/* ��ȡ����Э��汾 */
HPSOCKET_API En_HP_HttpVersion __HP_CALL HP_HttpAgent_GetLocalVersion(HP_HttpAgent pAgent);

/* ����Ƿ�����Э�� */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_IsUpgrade(HP_HttpAgent pAgent, HP_CONNID dwConnID);
/* ����Ƿ��� Keep-Alive ��ʶ */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_IsKeepAlive(HP_HttpAgent pAgent, HP_CONNID dwConnID);
/* ��ȡЭ��汾 */
HPSOCKET_API USHORT __HP_CALL HP_HttpAgent_GetVersion(HP_HttpAgent pAgent, HP_CONNID dwConnID);
/* ��ȡ���ݳ��� */
HPSOCKET_API ULONGLONG __HP_CALL HP_HttpAgent_GetContentLength(HP_HttpAgent pAgent, HP_CONNID dwConnID);
/* ��ȡ�������� */
HPSOCKET_API LPCSTR __HP_CALL HP_HttpAgent_GetContentType(HP_HttpAgent pAgent, HP_CONNID dwConnID);
/* ��ȡ���ݱ��� */
HPSOCKET_API LPCSTR __HP_CALL HP_HttpAgent_GetContentEncoding(HP_HttpAgent pAgent, HP_CONNID dwConnID);
/* ��ȡ������� */
HPSOCKET_API LPCSTR __HP_CALL HP_HttpAgent_GetTransferEncoding(HP_HttpAgent pAgent, HP_CONNID dwConnID);
/* ��ȡЭ���������� */
HPSOCKET_API En_HP_HttpUpgradeType __HP_CALL HP_HttpAgent_GetUpgradeType(HP_HttpAgent pAgent, HP_CONNID dwConnID);
/* ��ȡ����������� */
HPSOCKET_API USHORT __HP_CALL HP_HttpAgent_GetParseErrorCode(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR* lpszErrorDesc);

/* ��ȡĳ������ͷ����ֵ�� */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_GetHeader(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue);
/* ��ȡĳ������ͷ����ֵ�� */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_GetHeaders(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue[], DWORD* pdwCount);
/* ��ȡ��������ͷ */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_GetAllHeaders(HP_HttpAgent pAgent, HP_CONNID dwConnID, HP_THeader lpHeaders[], DWORD* pdwCount);
/* ��ȡ��������ͷ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_GetAllHeaderNames(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszName[], DWORD* pdwCount);

/* �����Ƿ�ʹ�� Cookie */
HPSOCKET_API void __HP_CALL HP_HttpAgent_SetUseCookie(HP_HttpAgent pAgent, BOOL bUseCookie);
/* ����Ƿ�ʹ�� Cookie */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_IsUseCookie(HP_HttpAgent pAgent);
/* ��ȡ Cookie */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_GetCookie(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue);
/* ��ȡ���� Cookie */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_GetAllCookies(HP_HttpAgent pAgent, HP_CONNID dwConnID, HP_TCookie lpCookies[], DWORD* pdwCount);

/* ��ȡ��ǰ WebSocket ��Ϣ״̬������ nullptr �򲻻�ȡ��Ӧ�ֶ� */
HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_GetWSMessageState(HP_HttpAgent pAgent, HP_CONNID dwConnID, BOOL* lpbFinal, BYTE* lpiReserved, BYTE* lpiOperationCode, LPCBYTE* lpszMask, ULONGLONG* lpullBodyLen, ULONGLONG* lpullBodyRemain);

/**************************************************************************/
/*************************** HTTP Client �������� **************************/

/*
* ���ƣ���������
* �����������˷��� HTTP ����
*		
* ������		lpszMethod		-- ���󷽷�
*			lpszPath		-- ����·��
*			lpHeaders		-- ����ͷ
*			iHeaderCount	-- ����ͷ����
*			pBody			-- ������
*			iLength			-- �����峤��
* ����ֵ��	TRUE			-- �ɹ�
*			FALSE			-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendRequest(HP_HttpClient pClient, LPCSTR lpszMethod, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength);

/*
* ���ƣ����ͱ����ļ�
* ��������ָ�����ӷ��� 4096 KB ���µ�С�ļ�
*		
* ������		dwConnID		-- ���� ID
*			lpszFileName	-- �ļ�·��
*			lpszMethod		-- ���󷽷�
*			lpszPath		-- ����·��
*			lpHeaders		-- ����ͷ
*			iHeaderCount	-- ����ͷ����
* ����ֵ��	TRUE			-- �ɹ�
*			FALSE			-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendLocalFile(HP_HttpClient pClient, LPCSTR lpszFileName, LPCSTR lpszMethod, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount);

/* ���� POST ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendPost(HP_HttpClient pClient, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength);
/* ���� PUT ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendPut(HP_HttpClient pClient, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength);
/* ���� PATCH ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendPatch(HP_HttpClient pClient, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength);
/* ���� GET ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendGet(HP_HttpClient pClient, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount);
/* ���� DELETE ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendDelete(HP_HttpClient pClient, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount);
/* ���� HEAD ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendHead(HP_HttpClient pClient, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount);
/* ���� TRACE ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendTrace(HP_HttpClient pClient, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount);
/* ���� OPTIONS ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendOptions(HP_HttpClient pClient, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount);
/* ���� CONNECT ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendConnect(HP_HttpClient pClient, LPCSTR lpszHost, const HP_THeader lpHeaders[], int iHeaderCount);

/*
* ���ƣ����� WebSocket ��Ϣ
* ��������Զ˶˷��� WebSocket ��Ϣ
*		
* ������		bFinal			-- �Ƿ����֡
*			iReserved		-- RSV1/RSV2/RSV3 �� 1 λ
*			iOperationCode	-- �����룺0x0 - 0xF
*			lpszMask		-- ���루nullptr �� 4 �ֽ����룬���Ϊ nullptr ��û�����룩
*			pData			-- ��Ϣ�����ݻ�����
*			iLength			-- ��Ϣ�����ݳ���
*			ullBodyLen		-- ��Ϣ�ܳ���
* 								ullBodyLen = 0		 -> ��Ϣ�ܳ���Ϊ iLength
* 								ullBodyLen = iLength -> ��Ϣ�ܳ���Ϊ ullBodyLen
* 								ullBodyLen > iLength -> ��Ϣ�ܳ���Ϊ ullBodyLen��������Ϣ�峤��Ϊ ullBOdyLen - iLength��������Ϣ��ͨ���ײ㷽�� Send() / SendPackets() ����
* 								ullBodyLen < iLength -> �������������ʧ��
* ����ֵ��	TRUE			-- �ɹ�
*			FALSE			-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendWSMessage(HP_HttpClient pClient, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], BYTE* pData, int iLength, ULONGLONG ullBodyLen);

/******************************************************************************/
/*************************** HTTP Client ���Է��ʷ��� **************************/

/* ��ȡ HTTP ״̬�� */
HPSOCKET_API USHORT __HP_CALL HP_HttpClient_GetStatusCode(HP_HttpClient pClient);

/* ���ñ���Э��汾 */
HPSOCKET_API void __HP_CALL HP_HttpClient_SetLocalVersion(HP_HttpClient pClient, En_HP_HttpVersion usVersion);
/* ��ȡ����Э��汾 */
HPSOCKET_API En_HP_HttpVersion __HP_CALL HP_HttpClient_GetLocalVersion(HP_HttpClient pClient);

/* ����Ƿ�����Э�� */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_IsUpgrade(HP_HttpClient pClient);
/* ����Ƿ��� Keep-Alive ��ʶ */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_IsKeepAlive(HP_HttpClient pClient);
/* ��ȡЭ��汾 */
HPSOCKET_API USHORT __HP_CALL HP_HttpClient_GetVersion(HP_HttpClient pClient);
/* ��ȡ���ݳ��� */
HPSOCKET_API ULONGLONG __HP_CALL HP_HttpClient_GetContentLength(HP_HttpClient pClient);
/* ��ȡ�������� */
HPSOCKET_API LPCSTR __HP_CALL HP_HttpClient_GetContentType(HP_HttpClient pClient);
/* ��ȡ���ݱ��� */
HPSOCKET_API LPCSTR __HP_CALL HP_HttpClient_GetContentEncoding(HP_HttpClient pClient);
/* ��ȡ������� */
HPSOCKET_API LPCSTR __HP_CALL HP_HttpClient_GetTransferEncoding(HP_HttpClient pClient);
/* ��ȡЭ���������� */
HPSOCKET_API En_HP_HttpUpgradeType __HP_CALL HP_HttpClient_GetUpgradeType(HP_HttpClient pClient);
/* ��ȡ����������� */
HPSOCKET_API USHORT __HP_CALL HP_HttpClient_GetParseErrorCode(HP_HttpClient pClient, LPCSTR* lpszErrorDesc);

/* ��ȡĳ������ͷ����ֵ�� */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_GetHeader(HP_HttpClient pClient, LPCSTR lpszName, LPCSTR* lpszValue);
/* ��ȡĳ������ͷ����ֵ�� */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_GetHeaders(HP_HttpClient pClient, LPCSTR lpszName, LPCSTR lpszValue[], DWORD* pdwCount);
/* ��ȡ��������ͷ */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_GetAllHeaders(HP_HttpClient pClient, HP_THeader lpHeaders[], DWORD* pdwCount);
/* ��ȡ��������ͷ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_GetAllHeaderNames(HP_HttpClient pClient, LPCSTR lpszName[], DWORD* pdwCount);

/* �����Ƿ�ʹ�� Cookie */
HPSOCKET_API void __HP_CALL HP_HttpClient_SetUseCookie(HP_HttpClient pClient, BOOL bUseCookie);
/* ����Ƿ�ʹ�� Cookie */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_IsUseCookie(HP_HttpClient pClient);
/* ��ȡ Cookie */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_GetCookie(HP_HttpClient pClient, LPCSTR lpszName, LPCSTR* lpszValue);
/* ��ȡ���� Cookie */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_GetAllCookies(HP_HttpClient pClient, HP_TCookie lpCookies[], DWORD* pdwCount);

/* ��ȡ��ǰ WebSocket ��Ϣ״̬������ nullptr �򲻻�ȡ��Ӧ�ֶ� */
HPSOCKET_API BOOL __HP_CALL HP_HttpClient_GetWSMessageState(HP_HttpClient pClient, BOOL* lpbFinal, BYTE* lpiReserved, BYTE* lpiOperationCode, LPCBYTE* lpszMask, ULONGLONG* lpullBodyLen, ULONGLONG* lpullBodyRemain);

/**************************************************************************/
/************************ HTTP Sync Client �������� ************************/

/*
* ���ƣ����� URL ����
* �����������˷��� HTTP URL ����
*		
* ������		lpszMethod		-- ���󷽷�
*			lpszUrl			-- ���� URL
*			lpHeaders		-- ����ͷ
*			iHeaderCount	-- ����ͷ����
*			pBody			-- ������
*			iLength			-- �����峤��
*			bForceReconnect	-- ǿ���������ӣ�Ĭ�ϣ�FALSE�������� URL �������Ͷ˿�����������һ��ʱ�������������ӣ�
* ����ֵ��	TRUE			-- �ɹ�
*			FALSE			-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_HttpSyncClient_OpenUrl(HP_HttpSyncClient pClient, LPCSTR lpszMethod, LPCSTR lpszUrl, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength, BOOL bForceReconnect);

/*
* ���ƣ����������
* �����������һ���������Ӧͷ����Ӧ��Ƚ����Ϣ���÷�������ÿ�η�������ǰ�Զ����ã�
*
* ������		
* ����ֵ��	TRUE			-- �ɹ�
*			FALSE			-- ʧ��
*/
HPSOCKET_API BOOL __HP_CALL HP_HttpSyncClient_CleanupRequestResult(HP_HttpSyncClient pClient);

/******************************************************************************/
/************************ HTTP Sync Client ���Է��ʷ��� ************************/

/* �������ӳ�ʱ�����룬0��ϵͳĬ�ϳ�ʱ��Ĭ�ϣ�5000�� */
HPSOCKET_API void __HP_CALL HP_HttpSyncClient_SetConnectTimeout(HP_HttpSyncClient pClient, DWORD dwConnectTimeout);
/* ��������ʱ�����룬0�����޵ȴ���Ĭ�ϣ�10000�� */
HPSOCKET_API void __HP_CALL HP_HttpSyncClient_SetRequestTimeout(HP_HttpSyncClient pClient, DWORD dwRequestTimeout);

/* ��ȡ���ӳ�ʱ */
HPSOCKET_API DWORD __HP_CALL HP_HttpSyncClient_GetConnectTimeout(HP_HttpSyncClient pClient);
/* ��ȡ����ʱ */
HPSOCKET_API DWORD __HP_CALL HP_HttpSyncClient_GetRequestTimeout(HP_HttpSyncClient pClient);

/* ��ȡ��Ӧ�� */
HPSOCKET_API BOOL __HP_CALL HP_HttpSyncClient_GetResponseBody(HP_HttpSyncClient pClient, LPCBYTE* lpszBody, int* piLength);

/**************************************************************************/
/*************************** HTTP Cookie ������ **************************/

/* ���ļ����� Cookie */
HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_MGR_LoadFromFile(LPCSTR lpszFile, BOOL bKeepExists /*= TRUE*/);
/* ���� Cookie ���ļ� */
HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_MGR_SaveToFile(LPCSTR lpszFile, BOOL bKeepExists /*= TRUE*/);
/* ���� Cookie */
HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_MGR_ClearCookies(LPCSTR lpszDomain /*= nullptr*/, LPCSTR lpszPath /*= nullptr*/);
/* ������� Cookie */
HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_MGR_RemoveExpiredCookies(LPCSTR lpszDomain /*= nullptr*/, LPCSTR lpszPath /*= nullptr*/);
/* ���� Cookie */
HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_MGR_SetCookie(LPCSTR lpszName, LPCSTR lpszValue, LPCSTR lpszDomain, LPCSTR lpszPath, int iMaxAge /*= -1*/, BOOL bHttpOnly /*= FALSE*/, BOOL bSecure /*= FALSE*/, int enSameSite /*= 0*/, BOOL bOnlyUpdateValueIfExists /*= TRUE*/);
/* ɾ�� Cookie */
HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_MGR_DeleteCookie(LPCSTR lpszDomain, LPCSTR lpszPath, LPCSTR lpszName);
/* �����Ƿ���������� Cookie */
HPSOCKET_API void __HP_CALL HP_HttpCookie_MGR_SetEnableThirdPartyCookie(BOOL bEnableThirdPartyCookie /*= TRUE*/);
/* ����Ƿ���������� Cookie */
HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_MGR_IsEnableThirdPartyCookie();

/* Cookie expires �ַ���ת��Ϊ���� */
HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_HLP_ParseExpires(LPCSTR lpszExpires, __time64_t* ptmExpires);
/* ����ת��Ϊ Cookie expires �ַ��� */
HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_HLP_MakeExpiresStr(char lpszBuff[], int* piBuffLen, __time64_t tmExpires);
/* ���� Cookie �ַ��� */
HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_HLP_ToString(char lpszBuff[], int* piBuffLen, LPCSTR lpszName, LPCSTR lpszValue, LPCSTR lpszDomain, LPCSTR lpszPath, int iMaxAge /*= -1*/, BOOL bHttpOnly /*= FALSE*/, BOOL bSecure /*= FALSE*/, int enSameSite /*= 0*/);
/* ��ȡ��ǰ UTC ʱ�� */
HPSOCKET_API __time64_t __HP_CALL HP_HttpCookie_HLP_CurrentUTCTime();
/* Max-Age -> expires */
HPSOCKET_API __time64_t __HP_CALL HP_HttpCookie_HLP_MaxAgeToExpires(int iMaxAge);
/* expires -> Max-Age */
HPSOCKET_API int __HP_CALL HP_HttpCookie_HLP_ExpiresToMaxAge(__time64_t tmExpires);

/*****************************************************************************************************************************************************/
/************************************************************* HTTP Global Function Exports **********************************************************/
/*****************************************************************************************************************************************************/

#endif

/*****************************************************************************************************************************************************/
/**************************************************************** Thread Pool Exports ****************************************************************/
/*****************************************************************************************************************************************************/

/****************************************************/
/******************* ���󴴽����� ********************/

// ���� IHPThreadPool ����
HPSOCKET_API HP_ThreadPool __HP_CALL Create_HP_ThreadPool();
// ���� IHPThreadPool ����
HPSOCKET_API void __HP_CALL Destroy_HP_ThreadPool(HP_ThreadPool pThreadPool);

/*
* ���ƣ����� TSocketTask ����
* ����������������󣬸ö����������� HP_Destroy_SocketTaskObj() ����
*		
* ������		fnTaskProc	-- ��������
*			pSender		-- �������
*			dwConnID	-- ���� ID
*			pBuffer		-- ���ݻ�����
*			iBuffLen	-- ���ݻ���������
*			enBuffType	-- ���ݻ��������ͣ�Ĭ�ϣ�TBT_COPY��
*							TBT_COPY	���������pBuffer ���Ƶ� TSocketTask ���󡣴˺� TSocketTask ������ pBuffer �������κι���
*											-> ������ pBuffer ����� pBuffer �������ڲ��ܿصĳ���
*							TBT_REFER	����ǳ������pBuffer �����Ƶ� TSocketTask ������ȷ�� TSocketTask �������������� pBuffer ������Ч
*											-> ������ pBuffer �ϴ�� pBuffer �����ã����� pBuffer ���������ܿصĳ���
*							TBT_ATTACH	����������ִ��ǳ�������� TSocketTask ������� pBuffer ������Ȩ���������ͷ� pBuffer�������λ���������
*											-> ע�⣺pBuffer ������ SYS_Malloc() �����������ʹ�ñ����ͣ�������ܻᷢ���ڴ���ʴ���
*			wParam		-- �Զ������
*			lParam		-- �Զ������
* ����ֵ��	HP_LPTSocketTask
*/
HPSOCKET_API HP_LPTSocketTask __HP_CALL Create_HP_SocketTaskObj(Fn_SocketTaskProc fnTaskProc, PVOID pSender, HP_CONNID dwConnID, LPCBYTE pBuffer, INT iBuffLen, En_HP_TaskBufferType enBuffType /*= TBT_COPY*/, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/);

// ���� TSocketTask ����
HPSOCKET_API void __HP_CALL Destroy_HP_SocketTaskObj(HP_LPTSocketTask pTask);

/***********************************************************************/
/***************************** ����������� *****************************/

/*
* ���ƣ������̳߳����
* ������
*		
* ������		dwThreadCount		-- �߳���������Ĭ�ϣ�0��
*									>0 -> dwThreadCount
*									=0 -> (CPU���� * 2 + 2)
*									<0 -> (CPU���� * (-dwThreadCount))
*			dwMaxQueueSize		-- ����������������Ĭ�ϣ�0�������ƣ�
*			enRejectedPolicy	-- ����ܾ��������
*									TRP_CALL_FAIL��Ĭ�ϣ�	�����̷���ʧ��
*									TRP_WAIT_FOR			���ȴ���ֱ���ɹ�����ʱ���̳߳عرյ�ԭ����ʧ�ܣ�
*									TRP_CALLER_RUN			���������߳�ֱ��ִ��
*			dwStackSize			-- �̶߳�ջ�ռ��С��Ĭ�ϣ�0 -> ����ϵͳĬ�ϣ�
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_ThreadPool_Start(HP_ThreadPool pThreadPool, DWORD dwThreadCount /*= 0*/, DWORD dwMaxQueueSize /*= 0*/, En_HP_RejectedPolicy enRejectedPolicy /*= TRP_CALL_FAIL*/, DWORD dwStackSize /*= 0*/);

/*
* ���ƣ��ر��̳߳����
* �������ڹ涨ʱ���ڹر��̳߳��������������߳������ȴ�ʱ����δ�������رգ��᳢��ǿ�ƹرգ���������ºܿ��ܻ����ϵͳ��Դй©
*		
* ������		dwMaxWait	-- ���ȴ�ʱ�䣨���룬Ĭ�ϣ�INFINITE��һֱ�ȴ���
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_ThreadPool_Stop(HP_ThreadPool pThreadPool, DWORD dwMaxWait /*= INFINITE*/);

/*
* ���ƣ��ύ����
* ���������̳߳��ύ�첽����
*		
* ������		fnTaskProc	-- ��������
*			pvArg		-- �������
*			dwMaxWait	-- �����ύ���ȴ�ʱ�䣨���� TRP_WAIT_FOR �����̳߳���Ч��Ĭ�ϣ�INFINITE��һֱ�ȴ���
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*							���У������� ERROR_DESTINATION_ELEMENT_FULL ��ʾ�����������
*/
HPSOCKET_API BOOL __HP_CALL HP_ThreadPool_Submit(HP_ThreadPool pThreadPool, HP_Fn_TaskProc fnTaskProc, PVOID pvArg, DWORD dwMaxWait /*= INFINITE*/);

/*
* ���ƣ��ύ Socket ����
* ���������̳߳��ύ�첽 Socket ����
*		
* ������		pTask		-- �������
*			dwMaxWait	-- �����ύ���ȴ�ʱ�䣨���� TRP_WAIT_FOR �����̳߳���Ч��Ĭ�ϣ�INFINITE��һֱ�ȴ���
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*							���У������� ERROR_DESTINATION_ELEMENT_FULL ��ʾ�����������
*							ע�⣺����ύʧ�ܣ���Ҫ�ֹ����� Destroy_HP_SocketTaskObj() ���� TSocketTask ����
*/
HPSOCKET_API BOOL __HP_CALL HP_ThreadPool_Submit_Task(HP_ThreadPool pThreadPool, HP_LPTSocketTask pTask, DWORD dwMaxWait /*= INFINITE*/);

/*
* ���ƣ������̳߳ش�С
* ���������ӻ�����̳߳صĹ����߳�����
*		
* ������		dwNewThreadCount	-- �߳�����
*									>0 -> dwNewThreadCount
*									=0 -> (CPU���� * 2 + 2)
*									<0 -> (CPU���� * (-dwNewThreadCount))
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡϵͳ�������
*/
HPSOCKET_API BOOL __HP_CALL HP_ThreadPool_AdjustThreadCount(HP_ThreadPool pThreadPool, DWORD dwNewThreadCount);

/***********************************************************************/
/***************************** ���Է��ʷ��� *****************************/

/* ����̳߳�����Ƿ������� */
HPSOCKET_API BOOL __HP_CALL HP_ThreadPool_HasStarted(HP_ThreadPool pThreadPool);
/* �鿴�̳߳������ǰ״̬ */
HPSOCKET_API EnServiceState	__HP_CALL HP_ThreadPool_GetState(HP_ThreadPool pThreadPool);
/* ��ȡ��ǰ������д�С */
HPSOCKET_API DWORD __HP_CALL HP_ThreadPool_GetQueueSize(HP_ThreadPool pThreadPool);
/* ��ȡ�����߳����� */
HPSOCKET_API DWORD __HP_CALL HP_ThreadPool_GetThreadCount(HP_ThreadPool pThreadPool);
/* ��ȡ�������������� */
HPSOCKET_API DWORD __HP_CALL HP_ThreadPool_GetMaxQueueSize(HP_ThreadPool pThreadPool);
/* ��ȡ����ܾ�������� */
HPSOCKET_API EnRejectedPolicy __HP_CALL HP_ThreadPool_GetRejectedPolicy(HP_ThreadPool pThreadPool);
