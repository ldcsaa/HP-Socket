/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.4.4
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
Module:  HPSocket for C DLL

Desc: ������ C ���������������ԣ��磺C / C# / Delphi �ȣ��ܷ����ʹ�� HPSocket

Usage:
		����һ��
		--------------------------------------------------------------------------------------
		0. ��C/C++ ���򣩰��� HPSocket4C.h ͷ�ļ�
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
		1. x86/HPSocket4C.dll		- (32λ/MBCS/Release)
		2. x86/HPSocket4C_D.dll		- (32λ/MBCS/DeBug)
		3. x86/HPSocket4C_U.dll		- (32λ/UNICODE/Release)
		4. x86/HPSocket4C_UD.dll	- (32λ/UNICODE/DeBug)
		5. x64/HPSocket4C.dll		- (64λ/MBCS/Release)
		6. x64/HPSocket4C_D.dll		- (64λ/MBCS/DeBug)
		7. x64/HPSocket4C_U.dll		- (64λ/UNICODE/Release)
		8. x64/HPSocket4C_UD.dll	- (64λ/UNICODE/DeBug)

******************************************************************************/

#pragma once

#include <winsock2.h>

/**************************************************/
/********* imports / exports HPSocket.dll *********/

#ifdef HPSOCKET_STATIC_LIB
	#define HPSOCKET_API EXTERN_C
#else
	#ifdef HPSOCKET_EXPORTS
		#define HPSOCKET_API EXTERN_C __declspec(dllexport)
	#else
		#define HPSOCKET_API EXTERN_C __declspec(dllimport)
	#endif
#endif

/************************************************************************
���ƣ����� ID ��������
�������������� ID ����������
************************************************************************/
typedef ULONG_PTR	HP_CONNID;

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
typedef HP_Object	HP_UdpAgent;
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
typedef HP_Object	HP_UdpServerListener;
typedef HP_Object	HP_UdpAgentListener;
typedef HP_Object	HP_UdpClientListener;
typedef HP_Object	HP_UdpCastListener;

/*****************************************************************************************************/
/******************************************** �����ࡢ�ӿ� ********************************************/
/*****************************************************************************************************/

/************************************************************************
���ƣ�ͨ���������״̬
������Ӧ�ó������ͨ��ͨ������� GetState() ������ȡ�����ǰ����״̬
************************************************************************/
enum En_HP_ServiceState
{
	HP_SS_STARTING	= 0,	// ��������
	HP_SS_STARTED	= 1,	// �Ѿ�����
	HP_SS_STOPPING	= 2,	// ����ֹͣ
	HP_SS_STOPPED	= 3,	// �Ѿ�ֹͣ
};

/************************************************************************
���ƣ�Socket ��������
������Ӧ�ó���� OnErrror() �¼���ͨ���ò�����ʶ�����ֲ������µĴ���
************************************************************************/
enum En_HP_SocketOperation
{
	HP_SO_UNKNOWN	= 0,	// Unknown
	HP_SO_ACCEPT	= 1,	// Acccept
	HP_SO_CONNECT	= 2,	// Connect
	HP_SO_SEND		= 3,	// Send
	HP_SO_RECEIVE	= 4,	// Receive
	HP_SO_CLOSE		= 5,	// Close
};

/************************************************************************
���ƣ��¼�֪ͨ������
�������¼�֪ͨ�ķ���ֵ����ͬ�ķ���ֵ��Ӱ��ͨ������ĺ�����Ϊ
************************************************************************/
enum En_HP_HandleResult
{
	HP_HR_OK		= 0,	// �ɹ�
	HP_HR_IGNORE	= 1,	// ����
	HP_HR_ERROR		= 2,	// ����
};

/************************************************************************
���ƣ�����ץȡ���
����������ץȡ�����ķ���ֵ
************************************************************************/
enum En_HP_FetchResult
{
	HP_FR_OK				= 0,	// �ɹ�
	HP_FR_LENGTH_TOO_LONG	= 1,	// ץȡ���ȹ���
	HP_FR_DATA_NOT_FOUND	= 2,	// �Ҳ��� ConnID ��Ӧ������
};

/************************************************************************
���ƣ����ݷ��Ͳ���
������Server ����� Agent ��������ݷ��Ͳ���

* ���ģʽ��Ĭ�ϣ�	�������Ѷ�����Ͳ��������������һ���ͣ����Ӵ���Ч��
* ��ȫģʽ			�������Ѷ�����Ͳ��������������һ���ͣ������ƴ����ٶȣ����⻺�������
* ֱ��ģʽ			����ÿһ�����Ͳ�����ֱ��Ͷ�ݣ������ڸ��ز��ߵ�Ҫ��ʵʱ�Խϸߵĳ���

************************************************************************/
enum En_HP_SendPolicy
{
	HP_SP_PACK				= 0,	// ���ģʽ��Ĭ�ϣ�
	HP_SP_SAFE				= 1,	// ��ȫģʽ
	HP_SP_DIRECT			= 2,	// ֱ��ģʽ
};

/************************************************************************
���ƣ������������
��������� Start() / Stop() ����ִ��ʧ��ʱ����ͨ�� GetLastError() ��ȡ�������
************************************************************************/
enum En_HP_SocketError
{
	HP_SE_OK					= NO_ERROR,	// �ɹ�
	HP_SE_ILLEGAL_STATE			= 1,		// ��ǰ״̬���������
	HP_SE_INVALID_PARAM			= 2,		// �Ƿ�����
	HP_SE_SOCKET_CREATE			= 3,		// ���� SOCKET ʧ��
	HP_SE_SOCKET_BIND			= 4,		// �� SOCKET ʧ��
	HP_SE_SOCKET_PREPARE		= 5,		// ���� SOCKET ʧ��
	HP_SE_SOCKET_LISTEN			= 6,		// ���� SOCKET ʧ��
	HP_SE_CP_CREATE				= 7,		// ������ɶ˿�ʧ��
	HP_SE_WORKER_THREAD_CREATE	= 8,		// ���������߳�ʧ��
	HP_SE_DETECT_THREAD_CREATE	= 9,		// ��������߳�ʧ��
	HP_SE_SOCKE_ATTACH_TO_CP	= 10,		// ����ɶ˿�ʧ��
	HP_SE_CONNECT_SERVER		= 11,		// ���ӷ�����ʧ��
	HP_SE_NETWORK				= 12,		// �������
	HP_SE_DATA_PROC				= 13,		// ���ݴ������
	HP_SE_DATA_SEND				= 14,		// ���ݷ���ʧ��
};

/************************************************************************
���ƣ�����ģʽ
������UDP ����Ĳ���ģʽ���鲥��㲥��
************************************************************************/
enum En_HP_CastMode
{
	HP_CM_MULTICAST	= 0,	// �鲥
	HP_CM_BROADCAST	= 1,	// �㲥
};

/****************************************************/
/************** HPSocket4C.dll �ص����� **************/

/* Server �ص����� */
typedef En_HP_HandleResult (__stdcall *HP_FN_Server_OnPrepareListen)	(UINT_PTR soListen);
// ���Ϊ TCP ���ӣ�pClientΪ SOCKET ��������Ϊ UDP ���ӣ�pClientΪ SOCKADDR_IN ָ�룻
typedef En_HP_HandleResult (__stdcall *HP_FN_Server_OnAccept)			(HP_CONNID dwConnID, UINT_PTR pClient);
typedef En_HP_HandleResult (__stdcall *HP_FN_Server_OnSend)				(HP_CONNID dwConnID, const BYTE* pData, int iLength);
typedef En_HP_HandleResult (__stdcall *HP_FN_Server_OnReceive)			(HP_CONNID dwConnID, const BYTE* pData, int iLength);
typedef En_HP_HandleResult (__stdcall *HP_FN_Server_OnPullReceive)		(HP_CONNID dwConnID, int iLength);
typedef En_HP_HandleResult (__stdcall *HP_FN_Server_OnClose)			(HP_CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode);
typedef En_HP_HandleResult (__stdcall *HP_FN_Server_OnShutdown)			();

/* Agent �ص����� */
typedef En_HP_HandleResult (__stdcall *HP_FN_Agent_OnPrepareConnect)	(HP_CONNID dwConnID, UINT_PTR socket);
typedef En_HP_HandleResult (__stdcall *HP_FN_Agent_OnConnect)			(HP_CONNID dwConnID);
typedef En_HP_HandleResult (__stdcall *HP_FN_Agent_OnSend)				(HP_CONNID dwConnID, const BYTE* pData, int iLength);
typedef En_HP_HandleResult (__stdcall *HP_FN_Agent_OnReceive)			(HP_CONNID dwConnID, const BYTE* pData, int iLength);
typedef En_HP_HandleResult (__stdcall *HP_FN_Agent_OnPullReceive)		(HP_CONNID dwConnID, int iLength);
typedef En_HP_HandleResult (__stdcall *HP_FN_Agent_OnClose)				(HP_CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode);
typedef En_HP_HandleResult (__stdcall *HP_FN_Agent_OnShutdown)			();

/* Client �ص����� */
typedef En_HP_HandleResult (__stdcall *HP_FN_Client_OnPrepareConnect)	(HP_Client pClient, UINT_PTR socket);
typedef En_HP_HandleResult (__stdcall *HP_FN_Client_OnConnect)			(HP_Client pClient);
typedef En_HP_HandleResult (__stdcall *HP_FN_Client_OnSend)				(HP_Client pClient, const BYTE* pData, int iLength);
typedef En_HP_HandleResult (__stdcall *HP_FN_Client_OnReceive)			(HP_Client pClient, const BYTE* pData, int iLength);
typedef En_HP_HandleResult (__stdcall *HP_FN_Client_OnPullReceive)		(HP_Client pClient, int iLength);
typedef En_HP_HandleResult (__stdcall *HP_FN_Client_OnClose)			(HP_Client pClient, En_HP_SocketOperation enOperation, int iErrorCode);

/****************************************************/
/************** HPSocket4C.dll �������� **************/

// ���� HP_TcpServer ����
HPSOCKET_API HP_TcpServer __stdcall Create_HP_TcpServer(HP_TcpServerListener pListener);
// ���� HP_TcpAgent ����
HPSOCKET_API HP_TcpAgent __stdcall Create_HP_TcpAgent(HP_TcpAgentListener pListener);
// ���� HP_TcpClient ����
HPSOCKET_API HP_TcpClient __stdcall Create_HP_TcpClient(HP_TcpClientListener pListener);
// ���� HP_TcpPullServer ����
HPSOCKET_API HP_TcpPullServer __stdcall Create_HP_TcpPullServer(HP_TcpPullServerListener pListener);
// ���� HP_TcpPullAgent ����
HPSOCKET_API HP_TcpPullAgent __stdcall Create_HP_TcpPullAgent(HP_TcpPullAgentListener pListener);
// ���� HP_TcpPullClient ����
HPSOCKET_API HP_TcpPullClient __stdcall Create_HP_TcpPullClient(HP_TcpPullClientListener pListener);
// ���� HP_TcpPackServer ����
HPSOCKET_API HP_TcpPackServer __stdcall Create_HP_TcpPackServer(HP_TcpServerListener pListener);
// ���� HP_TcpPackAgent ����
HPSOCKET_API HP_TcpPackAgent __stdcall Create_HP_TcpPackAgent(HP_TcpAgentListener pListener);
// ���� HP_TcpPackClient ����
HPSOCKET_API HP_TcpPackClient __stdcall Create_HP_TcpPackClient(HP_TcpClientListener pListener);
// ���� HP_UdpServer ����
HPSOCKET_API HP_UdpServer __stdcall Create_HP_UdpServer(HP_UdpServerListener pListener);
// ���� HP_UdpAgent ����
HPSOCKET_API HP_UdpAgent __stdcall Create_HP_UdpAgent(HP_UdpAgentListener pListener);
// ���� HP_UdpClient ����
HPSOCKET_API HP_UdpClient __stdcall Create_HP_UdpClient(HP_UdpClientListener pListener);
// ���� HP_UdpCast ����
HPSOCKET_API HP_UdpCast __stdcall Create_HP_UdpCast(HP_UdpCastListener pListener);

// ���� HP_TcpServer ����
HPSOCKET_API void __stdcall Destroy_HP_TcpServer(HP_TcpServer pServer);
// ���� HP_TcpAgent ����
HPSOCKET_API void __stdcall Destroy_HP_TcpAgent(HP_TcpAgent pAgent);
// ���� HP_TcpClient ����
HPSOCKET_API void __stdcall Destroy_HP_TcpClient(HP_TcpClient pClient);
// ���� HP_TcpPullServer ����
HPSOCKET_API void __stdcall Destroy_HP_TcpPullServer(HP_TcpPullServer pServer);
// ���� HP_TcpPullAgent ����
HPSOCKET_API void __stdcall Destroy_HP_TcpPullAgent(HP_TcpPullAgent pAgent);
// ���� HP_TcpPullClient ����
HPSOCKET_API void __stdcall Destroy_HP_TcpPullClient(HP_TcpPullClient pClient);
// ���� HP_TcpPackServer ����
HPSOCKET_API void __stdcall Destroy_HP_TcpPackServer(HP_TcpPackServer pServer);
// ���� HP_TcpPackAgent ����
HPSOCKET_API void __stdcall Destroy_HP_TcpPackAgent(HP_TcpPackAgent pAgent);
// ���� HP_TcpPackClient ����
HPSOCKET_API void __stdcall Destroy_HP_TcpPackClient(HP_TcpPackClient pClient);
// ���� HP_UdpServer ����
HPSOCKET_API void __stdcall Destroy_HP_UdpServer(HP_UdpServer pServer);
// ���� HP_UdpAgent ����
HPSOCKET_API void __stdcall Destroy_HP_UdpAgent(HP_UdpAgent pAgent);
// ���� HP_UdpClient ����
HPSOCKET_API void __stdcall Destroy_HP_UdpClient(HP_UdpClient pClient);
// ���� HP_UdpCast ����
HPSOCKET_API void __stdcall Destroy_HP_UdpCast(HP_UdpCast pCast);

// ���� HP_TcpServerListener ����
HPSOCKET_API HP_TcpServerListener __stdcall Create_HP_TcpServerListener();
// ���� HP_TcpAgentListener ����
HPSOCKET_API HP_TcpAgentListener __stdcall Create_HP_TcpAgentListener();
// ���� HP_TcpClientListener ����
HPSOCKET_API HP_TcpClientListener __stdcall Create_HP_TcpClientListener();
// ���� HP_TcpPullServerListener ����
HPSOCKET_API HP_TcpPullServerListener __stdcall Create_HP_TcpPullServerListener();
// ���� HP_TcpPullAgentListener ����
HPSOCKET_API HP_TcpPullAgentListener __stdcall Create_HP_TcpPullAgentListener();
// ���� HP_TcpPullClientListener ����
HPSOCKET_API HP_TcpPullClientListener __stdcall Create_HP_TcpPullClientListener();
// ���� HP_UdpServerListener ����
HPSOCKET_API HP_UdpServerListener __stdcall Create_HP_UdpServerListener();
// ���� HP_UdpAgentListener ����
HPSOCKET_API HP_TcpAgentListener __stdcall Create_HP_UdpAgentListener();
// ���� HP_UdpClientListener ����
HPSOCKET_API HP_UdpClientListener __stdcall Create_HP_UdpClientListener();
// ���� HP_UdpCastListener ����
HPSOCKET_API HP_UdpCastListener __stdcall Create_HP_UdpCastListener();

// ���� HP_TcpServerListener ����
HPSOCKET_API void __stdcall Destroy_HP_TcpServerListener(HP_TcpServerListener pListener);
// ���� HP_TcpAgentListener ����
HPSOCKET_API void __stdcall Destroy_HP_TcpAgentListener(HP_TcpAgentListener pListener);
// ���� HP_TcpClientListener ����
HPSOCKET_API void __stdcall Destroy_HP_TcpClientListener(HP_TcpClientListener pListener);
// ���� HP_TcpPullServerListener ����
HPSOCKET_API void __stdcall Destroy_HP_TcpPullServerListener(HP_TcpPullServerListener pListener);
// ���� HP_TcpPullAgentListener ����
HPSOCKET_API void __stdcall Destroy_HP_TcpPullAgentListener(HP_TcpPullAgentListener pListener);
// ���� HP_TcpPullClientListener ����
HPSOCKET_API void __stdcall Destroy_HP_TcpPullClientListener(HP_TcpPullClientListener pListener);
// ���� HP_UdpServerListener ����
HPSOCKET_API void __stdcall Destroy_HP_UdpServerListener(HP_UdpServerListener pListener);
// ���� HP_UdpAgentListener ����
HPSOCKET_API void __stdcall Destroy_HP_UdpAgentListener(HP_UdpAgentListener pListener);
// ���� HP_UdpClientListener ����
HPSOCKET_API void __stdcall Destroy_HP_UdpClientListener(HP_UdpClientListener pListener);
// ���� HP_UdpCastListener ����
HPSOCKET_API void __stdcall Destroy_HP_UdpCastListener(HP_UdpCastListener pListener);

/**********************************************************************************/
/***************************** Server �ص��������÷��� *****************************/

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnPrepareListen(HP_ServerListener pListener	, HP_FN_Server_OnPrepareListen fn);
HPSOCKET_API void __stdcall HP_Set_FN_Server_OnAccept(HP_ServerListener pListener			, HP_FN_Server_OnAccept fn);
HPSOCKET_API void __stdcall HP_Set_FN_Server_OnSend(HP_ServerListener pListener				, HP_FN_Server_OnSend fn);
HPSOCKET_API void __stdcall HP_Set_FN_Server_OnReceive(HP_ServerListener pListener			, HP_FN_Server_OnReceive fn);
HPSOCKET_API void __stdcall HP_Set_FN_Server_OnPullReceive(HP_ServerListener pListener		, HP_FN_Server_OnPullReceive fn);
HPSOCKET_API void __stdcall HP_Set_FN_Server_OnClose(HP_ServerListener pListener			, HP_FN_Server_OnClose fn);
HPSOCKET_API void __stdcall HP_Set_FN_Server_OnShutdown(HP_ServerListener pListener			, HP_FN_Server_OnShutdown fn);

/**********************************************************************************/
/****************************** Agent �ص��������÷��� *****************************/

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnPrepareConnect(HP_AgentListener pListener		, HP_FN_Agent_OnPrepareConnect fn);
HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnConnect(HP_AgentListener pListener			, HP_FN_Agent_OnConnect fn);
HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnSend(HP_AgentListener pListener				, HP_FN_Agent_OnSend fn);
HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnReceive(HP_AgentListener pListener			, HP_FN_Agent_OnReceive fn);
HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnPullReceive(HP_AgentListener pListener		, HP_FN_Agent_OnPullReceive fn);
HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnClose(HP_AgentListener pListener				, HP_FN_Agent_OnClose fn);
HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnShutdown(HP_AgentListener pListener			, HP_FN_Agent_OnShutdown fn);

/**********************************************************************************/
/***************************** Client �ص��������÷��� *****************************/

HPSOCKET_API void __stdcall HP_Set_FN_Client_OnPrepareConnect(HP_ClientListener pListener	, HP_FN_Client_OnPrepareConnect fn);
HPSOCKET_API void __stdcall HP_Set_FN_Client_OnConnect(HP_ClientListener pListener			, HP_FN_Client_OnConnect fn);
HPSOCKET_API void __stdcall HP_Set_FN_Client_OnSend(HP_ClientListener pListener				, HP_FN_Client_OnSend fn);
HPSOCKET_API void __stdcall HP_Set_FN_Client_OnReceive(HP_ClientListener pListener			, HP_FN_Client_OnReceive fn);
HPSOCKET_API void __stdcall HP_Set_FN_Client_OnPullReceive(HP_ClientListener pListener		, HP_FN_Client_OnPullReceive fn);
HPSOCKET_API void __stdcall HP_Set_FN_Client_OnClose(HP_ClientListener pListener			, HP_FN_Client_OnClose fn);

/**************************************************************************/
/***************************** Server �������� *****************************/

/*
* ���ƣ�����ͨ�����
* ���������������ͨ�������������ɺ�ɿ�ʼ���տͻ������Ӳ��շ�����
*		
* ������		pszBindAddress	-- ������ַ
*			usPort			-- �����˿�
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __stdcall HP_Server_Start(HP_Server pServer, LPCTSTR pszBindAddress, USHORT usPort);

/*
* ���ƣ��ر�ͨ�����
* �������رշ����ͨ��������ر���ɺ�Ͽ����пͻ������Ӳ��ͷ�������Դ
*		
* ������	
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __stdcall HP_Server_Stop(HP_Server pServer);

/*
* ���ƣ���������
* ��������ָ�����ӷ�������
*		
* ������		dwConnID	-- ���� ID
*			pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� Windows API ���� ::GetLastError() ��ȡ Windows �������
*/
HPSOCKET_API BOOL __stdcall HP_Server_Send(HP_Server pServer, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength);

/*
* ���ƣ���������
* ��������ָ�����ӷ�������
*		
* ������		dwConnID	-- ���� ID
*			pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
*			iOffset		-- ���ͻ�����ָ��ƫ����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� Windows API ���� ::GetLastError() ��ȡ Windows �������
*/
HPSOCKET_API BOOL __stdcall HP_Server_SendPart(HP_Server pServer, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset);

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
*			FALSE	-- ʧ�ܣ���ͨ�� Windows API ���� ::GetLastError() ��ȡ Windows �������
*/
HPSOCKET_API BOOL __stdcall HP_Server_SendPackets(HP_Server pServer, HP_CONNID dwConnID, const WSABUF pBuffers[], int iCount);

/*
* ���ƣ��Ͽ�����
* �������Ͽ���ĳ���ͻ��˵�����
*		
* ������		dwConnID	-- ���� ID
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __stdcall HP_Server_Disconnect(HP_Server pServer, HP_CONNID dwConnID, BOOL bForce);

/*
* ���ƣ��Ͽ���ʱ����
* �������Ͽ�����ָ��ʱ��������
*		
* ������		dwPeriod	-- ʱ�������룩
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __stdcall HP_Server_DisconnectLongConnections(HP_Server pServer, DWORD dwPeriod, BOOL bForce);

/*
* ���ƣ��Ͽ���Ĭ����
* �������Ͽ�����ָ��ʱ���ľ�Ĭ����
*		
* ������		dwPeriod	-- ʱ�������룩
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __stdcall HP_Server_DisconnectSilenceConnections(HP_Server pServer, DWORD dwPeriod, BOOL bForce);

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
HPSOCKET_API BOOL __stdcall HP_Server_SetConnectionExtra(HP_Server pServer, HP_CONNID dwConnID, PVOID pExtra);

/*
* ���ƣ���ȡ���ӵĸ�������
* �������Ƿ�Ϊ���Ӱ󶨸������ݻ��߰�ʲô�������ݣ�����Ӧ�ó���ֻ�����
*		
* ������		dwConnID	-- ���� ID
*			ppv			-- ����ָ��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���Ч������ ID��
*/
HPSOCKET_API BOOL __stdcall HP_Server_GetConnectionExtra(HP_Server pServer, HP_CONNID dwConnID, PVOID* ppExtra);

/* ���ͨ������Ƿ������� */
HPSOCKET_API BOOL __stdcall HP_Server_HasStarted(HP_Server pServer);
/* �鿴ͨ�������ǰ״̬ */
HPSOCKET_API En_HP_ServiceState __stdcall HP_Server_GetState(HP_Server pServer);
/* ��ȡ���һ��ʧ�ܲ����Ĵ������ */
HPSOCKET_API En_HP_SocketError __stdcall HP_Server_GetLastError(HP_Server pServer);
/* ��ȡ���һ��ʧ�ܲ����Ĵ������� */
HPSOCKET_API LPCTSTR __stdcall HP_Server_GetLastErrorDesc(HP_Server pServer);
/* ��ȡ������δ�������ݵĳ��� */
HPSOCKET_API BOOL __stdcall HP_Server_GetPendingDataLength(HP_Server pServer, HP_CONNID dwConnID, int* piPending);
/* ��ȡ�ͻ��������� */
HPSOCKET_API DWORD __stdcall HP_Server_GetConnectionCount(HP_Server pServer);
/* ��ȡ�������ӵ� CONNID */
HPSOCKET_API BOOL __stdcall HP_Server_GetAllConnectionIDs(HP_Server pServer, HP_CONNID pIDs[], DWORD* pdwCount);
/* ��ȡĳ���ͻ�������ʱ�������룩 */
HPSOCKET_API BOOL __stdcall HP_Server_GetConnectPeriod(HP_Server pServer, HP_CONNID dwConnID, DWORD* pdwPeriod);
/* ��ȡĳ�����Ӿ�Ĭʱ�䣨���룩 */
HPSOCKET_API BOOL __stdcall HP_Server_GetSilencePeriod(HP_Server pServer, HP_CONNID dwConnID, DWORD* pdwPeriod);
/* ��ȡ���� Socket �ĵ�ַ��Ϣ */
HPSOCKET_API BOOL __stdcall HP_Server_GetListenAddress(HP_Server pServer, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort);
/* ��ȡĳ�����ӵ�Զ�̵�ַ��Ϣ */
HPSOCKET_API BOOL __stdcall HP_Server_GetRemoteAddress(HP_Server pServer, HP_CONNID dwConnID, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort);

/* �������ݷ��Ͳ��� */
HPSOCKET_API void __stdcall HP_Server_SetSendPolicy(HP_Server pServer, En_HP_SendPolicy enSendPolicy);
/* ���� Socket �����������ʱ�䣨���룬�������ڼ�� Socket ��������ܱ���ȡʹ�ã� */
HPSOCKET_API void __stdcall HP_Server_SetFreeSocketObjLockTime(HP_Server pServer, DWORD dwFreeSocketObjLockTime);
/* ���� Socket ����ش�С��ͨ������Ϊƽ���������������� 1/3 - 1/2�� */
HPSOCKET_API void __stdcall HP_Server_SetFreeSocketObjPool(HP_Server pServer, DWORD dwFreeSocketObjPool);
/* �����ڴ�黺��ش�С��ͨ������Ϊ Socket ����ش�С�� 2 - 3 ���� */
HPSOCKET_API void __stdcall HP_Server_SetFreeBufferObjPool(HP_Server pServer, DWORD dwFreeBufferObjPool);
/* ���� Socket ����ػ��շ�ֵ��ͨ������Ϊ Socket ����ش�С�� 3 ���� */
HPSOCKET_API void __stdcall HP_Server_SetFreeSocketObjHold(HP_Server pServer, DWORD dwFreeSocketObjHold);
/* �����ڴ�黺��ػ��շ�ֵ��ͨ������Ϊ�ڴ�黺��ش�С�� 3 ���� */
HPSOCKET_API void __stdcall HP_Server_SetFreeBufferObjHold(HP_Server pServer, DWORD dwFreeBufferObjHold);
/* ���ù����߳�������ͨ������Ϊ 2 * CPU + 2�� */
HPSOCKET_API void __stdcall HP_Server_SetWorkerThreadCount(HP_Server pServer, DWORD dwWorkerThreadCount);
/* �����Ƿ��Ǿ�Ĭʱ�䣨����Ϊ TRUE ʱ DisconnectSilenceConnections() �� GetSilencePeriod() ����Ч��Ĭ�ϣ�FALSE�� */
HPSOCKET_API void __stdcall HP_Server_SetMarkSilence(HP_Server pServer, BOOL bMarkSilence);

/* ��ȡ���ݷ��Ͳ��� */
HPSOCKET_API En_HP_SendPolicy __stdcall HP_Server_GetSendPolicy(HP_Server pServer);
/* ��ȡ Socket �����������ʱ�� */
HPSOCKET_API DWORD __stdcall HP_Server_GetFreeSocketObjLockTime(HP_Server pServer);
/* ��ȡ Socket ����ش�С */
HPSOCKET_API DWORD __stdcall HP_Server_GetFreeSocketObjPool(HP_Server pServer);
/* ��ȡ�ڴ�黺��ش�С */
HPSOCKET_API DWORD __stdcall HP_Server_GetFreeBufferObjPool(HP_Server pServer);
/* ��ȡ Socket ����ػ��շ�ֵ */
HPSOCKET_API DWORD __stdcall HP_Server_GetFreeSocketObjHold(HP_Server pServer);
/* ��ȡ�ڴ�黺��ػ��շ�ֵ */
HPSOCKET_API DWORD __stdcall HP_Server_GetFreeBufferObjHold(HP_Server pServer);
/* ��ȡ�����߳����� */
HPSOCKET_API DWORD __stdcall HP_Server_GetWorkerThreadCount(HP_Server pServer);
/* ����Ƿ��Ǿ�Ĭʱ�� */
HPSOCKET_API BOOL __stdcall HP_Server_IsMarkSilence(HP_Server pServer);

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
*			FALSE	-- ʧ�ܣ���ͨ�� Windows API ���� ::GetLastError() ��ȡ Windows �������
*/
HPSOCKET_API BOOL __stdcall HP_TcpServer_SendSmallFile(HP_Server pServer, HP_CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail);

/**********************************************************************************/
/***************************** TCP Server ���Է��ʷ��� *****************************/

/* ���ü��� Socket �ĵȺ���д�С�����ݲ������������������ã� */
HPSOCKET_API void __stdcall HP_TcpServer_SetSocketListenQueue(HP_TcpServer pServer, DWORD dwSocketListenQueue);
/* ���� Accept ԤͶ�����������ݸ��ص������ã�Accept ԤͶ������Խ����֧�ֵĲ�����������Խ�ࣩ */
HPSOCKET_API void __stdcall HP_TcpServer_SetAcceptSocketCount(HP_TcpServer pServer, DWORD dwAcceptSocketCount);
/* ����ͨ�����ݻ�������С������ƽ��ͨ�����ݰ���С�������ã�ͨ������Ϊ 1024 �ı����� */
HPSOCKET_API void __stdcall HP_TcpServer_SetSocketBufferSize(HP_TcpServer pServer, DWORD dwSocketBufferSize);
/* ����������������������룬0 �򲻷�����������Ĭ�ϣ�30 * 1000�� */
HPSOCKET_API void __stdcall HP_TcpServer_SetKeepAliveTime(HP_TcpServer pServer, DWORD dwKeepAliveTime);
/* �����쳣��������������룬0 ����������������Ĭ�ϣ�10 * 1000������������ɴ� [Ĭ�ϣ�WinXP 5 ��, Win7 10 ��] ��ⲻ������ȷ�ϰ�����Ϊ�Ѷ��ߣ� */
HPSOCKET_API void __stdcall HP_TcpServer_SetKeepAliveInterval(HP_TcpServer pServer, DWORD dwKeepAliveInterval);

/* ��ȡ Accept ԤͶ������ */
HPSOCKET_API DWORD __stdcall HP_TcpServer_GetAcceptSocketCount(HP_TcpServer pServer);
/* ��ȡͨ�����ݻ�������С */
HPSOCKET_API DWORD __stdcall HP_TcpServer_GetSocketBufferSize(HP_TcpServer pServer);
/* ��ȡ���� Socket �ĵȺ���д�С */
HPSOCKET_API DWORD __stdcall HP_TcpServer_GetSocketListenQueue(HP_TcpServer pServer);
/* ��ȡ������������� */
HPSOCKET_API DWORD __stdcall HP_TcpServer_GetKeepAliveTime(HP_TcpServer pServer);
/* ��ȡ�쳣��������� */
HPSOCKET_API DWORD __stdcall HP_TcpServer_GetKeepAliveInterval(HP_TcpServer pServer);

/**********************************************************************************/
/***************************** UDP Server ���Է��ʷ��� *****************************/

/* �������ݱ�����󳤶ȣ������ھ����������²����� 1472 �ֽڣ��ڹ����������²����� 548 �ֽڣ� */
HPSOCKET_API void __stdcall HP_UdpServer_SetMaxDatagramSize(HP_UdpServer pServer, DWORD dwMaxDatagramSize);
/* ��ȡ���ݱ�����󳤶� */
HPSOCKET_API DWORD __stdcall HP_UdpServer_GetMaxDatagramSize(HP_UdpServer pServer);

/* ���� Receive ԤͶ�����������ݸ��ص������ã�Receive ԤͶ������Խ���򶪰�����ԽС�� */
HPSOCKET_API void __stdcall HP_UdpServer_SetPostReceiveCount(HP_UdpServer pServer, DWORD dwPostReceiveCount);
/* ��ȡ Receive ԤͶ������ */
HPSOCKET_API DWORD __stdcall HP_UdpServer_GetPostReceiveCount(HP_UdpServer pServer);

/* ���ü������Դ�����0 �򲻷��ͼ�������������������Դ�������Ϊ�Ѷ��ߣ� */
HPSOCKET_API void __stdcall HP_UdpServer_SetDetectAttempts(HP_UdpServer pServer, DWORD dwDetectAttempts);
/* ���ü������ͼ�����룬0 �����ͼ����� */
HPSOCKET_API void __stdcall HP_UdpServer_SetDetectInterval(HP_UdpServer pServer, DWORD dwDetectInterval);
/* ��ȡ���������� */
HPSOCKET_API DWORD __stdcall HP_UdpServer_GetDetectAttempts(HP_UdpServer pServer);
/* ��ȡ��������� */
HPSOCKET_API DWORD __stdcall HP_UdpServer_GetDetectInterval(HP_UdpServer pServer);

/**************************************************************************/
/***************************** Agent �������� *****************************/

/*
* ���ƣ�����ͨ�����
* ����������ͨ�Ŵ��������������ɺ�ɿ�ʼ����Զ�̷�����
*		
* ������		pszBindAddress	-- ������ַ
*			bAsyncConnect	-- �Ƿ�����첽 Connect
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __stdcall HP_Agent_Start(HP_Agent pAgent, LPCTSTR pszBindAddress, BOOL bAsyncConnect);

/*
* ���ƣ��ر�ͨ�����
* �������ر�ͨ��������ر���ɺ�Ͽ��������Ӳ��ͷ�������Դ
*		
* ������	
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __stdcall HP_Agent_Stop(HP_Agent pAgent);

/*
* ���ƣ����ӷ�����
* ���������ӷ����������ӳɹ��� IAgentListener ����յ� OnConnect() �¼�
*		
* ������		pszRemoteAddress	-- ����˵�ַ
*			usPort				-- ����˶˿�
*			pdwConnID			-- ���� ID��Ĭ�ϣ�nullptr������ȡ���� ID��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ������ SYS_GetLastError() ��ȡ Windows �������
*/
HPSOCKET_API BOOL __stdcall HP_Agent_Connect(HP_Agent pAgent, LPCTSTR pszRemoteAddress, USHORT usPort, HP_CONNID* pdwConnID);

/*
* ���ƣ���������
* ��������ָ�����ӷ�������
*		
* ������		dwConnID	-- ���� ID
*			pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� Windows API ���� ::GetLastError() ��ȡ Windows �������
*/
HPSOCKET_API BOOL __stdcall HP_Agent_Send(HP_Agent pAgent, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength);

/*
* ���ƣ���������
* ��������ָ�����ӷ�������
*		
* ������		dwConnID	-- ���� ID
*			pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
*			iOffset		-- ���ͻ�����ָ��ƫ����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� Windows API ���� ::GetLastError() ��ȡ Windows �������
*/
HPSOCKET_API BOOL __stdcall HP_Agent_SendPart(HP_Agent pAgent, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset);

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
*			FALSE	-- ʧ�ܣ���ͨ�� Windows API ���� ::GetLastError() ��ȡ Windows �������
*/
HPSOCKET_API BOOL __stdcall HP_Agent_SendPackets(HP_Agent pAgent, HP_CONNID dwConnID, const WSABUF pBuffers[], int iCount);

/*
* ���ƣ��Ͽ�����
* �������Ͽ�ĳ������
*		
* ������		dwConnID	-- ���� ID
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __stdcall HP_Agent_Disconnect(HP_Agent pAgent, HP_CONNID dwConnID, BOOL bForce);

/*
* ���ƣ��Ͽ���ʱ����
* �������Ͽ�����ָ��ʱ��������
*		
* ������		dwPeriod	-- ʱ�������룩
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __stdcall HP_Agent_DisconnectLongConnections(HP_Agent pAgent, DWORD dwPeriod, BOOL bForce);

/*
* ���ƣ��Ͽ���Ĭ����
* �������Ͽ�����ָ��ʱ���ľ�Ĭ����
*		
* ������		dwPeriod	-- ʱ�������룩
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __stdcall HP_Agent_DisconnectSilenceConnections(HP_Agent pAgent, DWORD dwPeriod, BOOL bForce);

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
HPSOCKET_API BOOL __stdcall HP_Agent_SetConnectionExtra(HP_Agent pAgent, HP_CONNID dwConnID, PVOID pExtra);

/*
* ���ƣ���ȡ���ӵĸ�������
* �������Ƿ�Ϊ���Ӱ󶨸������ݻ��߰�ʲô�������ݣ�����Ӧ�ó���ֻ�����
*		
* ������		dwConnID	-- ���� ID
*			ppv			-- ����ָ��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���Ч������ ID��
*/
HPSOCKET_API BOOL __stdcall HP_Agent_GetConnectionExtra(HP_Agent pAgent, HP_CONNID dwConnID, PVOID* ppExtra);

/* ���ͨ������Ƿ������� */
HPSOCKET_API BOOL __stdcall HP_Agent_HasStarted(HP_Agent pAgent);
/* �鿴ͨ�������ǰ״̬ */
HPSOCKET_API En_HP_ServiceState __stdcall HP_Agent_GetState(HP_Agent pAgent);
/* ��ȡ������ */
HPSOCKET_API DWORD __stdcall HP_Agent_GetConnectionCount(HP_Agent pAgent);
/* ��ȡ�������ӵ� CONNID */
HPSOCKET_API BOOL __stdcall HP_Agent_GetAllConnectionIDs(HP_Agent pAgent, HP_CONNID pIDs[], DWORD* pdwCount);
/* ��ȡĳ������ʱ�������룩 */
HPSOCKET_API BOOL __stdcall HP_Agent_GetConnectPeriod(HP_Agent pAgent, HP_CONNID dwConnID, DWORD* pdwPeriod);
/* ��ȡĳ�����Ӿ�Ĭʱ�䣨���룩 */
HPSOCKET_API BOOL __stdcall HP_Agent_GetSilencePeriod(HP_Agent pAgent, HP_CONNID dwConnID, DWORD* pdwPeriod);
/* ��ȡĳ�����ӵı��ص�ַ��Ϣ */
HPSOCKET_API BOOL __stdcall HP_Agent_GetLocalAddress(HP_Agent pAgent, HP_CONNID dwConnID, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort);
/* ��ȡĳ�����ӵ�Զ�̵�ַ��Ϣ */
HPSOCKET_API BOOL __stdcall HP_Agent_GetRemoteAddress(HP_Agent pAgent, HP_CONNID dwConnID, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort);
/* ��ȡ���һ��ʧ�ܲ����Ĵ������ */
HPSOCKET_API En_HP_SocketError __stdcall HP_Agent_GetLastError(HP_Agent pAgent);
/* ��ȡ���һ��ʧ�ܲ����Ĵ������� */
HPSOCKET_API LPCTSTR __stdcall HP_Agent_GetLastErrorDesc(HP_Agent pAgent);
/* ��ȡ������δ�������ݵĳ��� */
HPSOCKET_API BOOL __stdcall HP_Agent_GetPendingDataLength(HP_Agent pAgent, HP_CONNID dwConnID, int* piPending);

/* �������ݷ��Ͳ��� */
HPSOCKET_API void __stdcall HP_Agent_SetSendPolicy(HP_Agent pAgent, En_HP_SendPolicy enSendPolicy);
/* ���� Socket �����������ʱ�䣨���룬�������ڼ�� Socket ��������ܱ���ȡʹ�ã� */
HPSOCKET_API void __stdcall HP_Agent_SetFreeSocketObjLockTime(HP_Agent pAgent, DWORD dwFreeSocketObjLockTime);
/* ���� Socket ����ش�С��ͨ������Ϊƽ���������������� 1/3 - 1/2�� */
HPSOCKET_API void __stdcall HP_Agent_SetFreeSocketObjPool(HP_Agent pAgent, DWORD dwFreeSocketObjPool);
/* �����ڴ�黺��ش�С��ͨ������Ϊ Socket ����ش�С�� 2 - 3 ���� */
HPSOCKET_API void __stdcall HP_Agent_SetFreeBufferObjPool(HP_Agent pAgent, DWORD dwFreeBufferObjPool);
/* ���� Socket ����ػ��շ�ֵ��ͨ������Ϊ Socket ����ش�С�� 3 ���� */
HPSOCKET_API void __stdcall HP_Agent_SetFreeSocketObjHold(HP_Agent pAgent, DWORD dwFreeSocketObjHold);
/* �����ڴ�黺��ػ��շ�ֵ��ͨ������Ϊ�ڴ�黺��ش�С�� 3 ���� */
HPSOCKET_API void __stdcall HP_Agent_SetFreeBufferObjHold(HP_Agent pAgent, DWORD dwFreeBufferObjHold);
/* ���ù����߳�������ͨ������Ϊ 2 * CPU + 2�� */
HPSOCKET_API void __stdcall HP_Agent_SetWorkerThreadCount(HP_Agent pAgent, DWORD dwWorkerThreadCount);
/* �����Ƿ��Ǿ�Ĭʱ�䣨����Ϊ TRUE ʱ DisconnectSilenceConnections() �� GetSilencePeriod() ����Ч��Ĭ�ϣ�FALSE�� */
HPSOCKET_API void __stdcall HP_Agent_SetMarkSilence(HP_Agent pAgent, BOOL bMarkSilence);

/* ��ȡ���ݷ��Ͳ��� */
HPSOCKET_API En_HP_SendPolicy __stdcall HP_Agent_GetSendPolicy(HP_Agent pAgent);
/* ��ȡ Socket �����������ʱ�� */
HPSOCKET_API DWORD __stdcall HP_Agent_GetFreeSocketObjLockTime(HP_Agent pAgent);
/* ��ȡ Socket ����ش�С */
HPSOCKET_API DWORD __stdcall HP_Agent_GetFreeSocketObjPool(HP_Agent pAgent);
/* ��ȡ�ڴ�黺��ش�С */
HPSOCKET_API DWORD __stdcall HP_Agent_GetFreeBufferObjPool(HP_Agent pAgent);
/* ��ȡ Socket ����ػ��շ�ֵ */
HPSOCKET_API DWORD __stdcall HP_Agent_GetFreeSocketObjHold(HP_Agent pAgent);
/* ��ȡ�ڴ�黺��ػ��շ�ֵ */
HPSOCKET_API DWORD __stdcall HP_Agent_GetFreeBufferObjHold(HP_Agent pAgent);
/* ��ȡ�����߳����� */
HPSOCKET_API DWORD __stdcall HP_Agent_GetWorkerThreadCount(HP_Agent pAgent);
/* ����Ƿ��Ǿ�Ĭʱ�� */
HPSOCKET_API BOOL __stdcall HP_Agent_IsMarkSilence(HP_Agent pAgent);

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
*			FALSE	-- ʧ�ܣ���ͨ�� Windows API ���� ::GetLastError() ��ȡ Windows �������
*/
HPSOCKET_API BOOL __stdcall HP_TcpAgent_SendSmallFile(HP_Agent pAgent, HP_CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail);

/**********************************************************************************/
/***************************** TCP Agent ���Է��ʷ��� *****************************/

/* �����Ƿ����õ�ַ���û��ƣ�Ĭ�ϣ������ã� */
HPSOCKET_API void __stdcall HP_TcpAgent_SetReuseAddress(HP_TcpAgent pAgent, BOOL bReuseAddress);
/* ����Ƿ����õ�ַ���û��� */
HPSOCKET_API BOOL __stdcall HP_TcpAgent_IsReuseAddress(HP_TcpAgent pAgent);

/* ����ͨ�����ݻ�������С������ƽ��ͨ�����ݰ���С�������ã�ͨ������Ϊ 1024 �ı����� */
HPSOCKET_API void __stdcall HP_TcpAgent_SetSocketBufferSize(HP_TcpAgent pAgent, DWORD dwSocketBufferSize);
/* ����������������������룬0 �򲻷�����������Ĭ�ϣ�30 * 1000�� */
HPSOCKET_API void __stdcall HP_TcpAgent_SetKeepAliveTime(HP_TcpAgent pAgent, DWORD dwKeepAliveTime);
/* �����쳣��������������룬0 ����������������Ĭ�ϣ�10 * 1000������������ɴ� [Ĭ�ϣ�WinXP 5 ��, Win7 10 ��] ��ⲻ������ȷ�ϰ�����Ϊ�Ѷ��ߣ� */
HPSOCKET_API void __stdcall HP_TcpAgent_SetKeepAliveInterval(HP_TcpAgent pAgent, DWORD dwKeepAliveInterval);

/* ��ȡͨ�����ݻ�������С */
HPSOCKET_API DWORD __stdcall HP_TcpAgent_GetSocketBufferSize(HP_TcpAgent pAgent);
/* ��ȡ������������� */
HPSOCKET_API DWORD __stdcall HP_TcpAgent_GetKeepAliveTime(HP_TcpAgent pAgent);
/* ��ȡ�쳣��������� */
HPSOCKET_API DWORD __stdcall HP_TcpAgent_GetKeepAliveInterval(HP_TcpAgent pAgent);

/**********************************************************************************/
/***************************** UDP Agent ���Է��ʷ��� *****************************/

/* �������ݱ�����󳤶ȣ������ھ����������²����� 1472 �ֽڣ��ڹ����������²����� 548 �ֽڣ� */
HPSOCKET_API void __stdcall HP_UdpAgent_SetMaxDatagramSize(HP_UdpAgent pAgent, DWORD dwMaxDatagramSize);
/* ���ü������Դ�����0 �򲻷��ͼ�������������������Դ�������Ϊ�Ѷ��ߣ� */
HPSOCKET_API void __stdcall HP_UdpAgent_SetDetectAttempts(HP_UdpAgent pAgent, DWORD dwDetectAttempts);
/* ���ü������ͼ�����룬0 �����ͼ����� */
HPSOCKET_API void __stdcall HP_UdpAgent_SetDetectInterval(HP_UdpAgent pAgent, DWORD dwDetectInterval);
/* ��ȡ���ݱ�����󳤶� */
HPSOCKET_API DWORD __stdcall HP_UdpAgent_GetMaxDatagramSize(HP_UdpAgent pAgent);
/* ��ȡ���������� */
HPSOCKET_API DWORD __stdcall HP_UdpAgent_GetDetectAttempts(HP_UdpAgent pAgent);
/* ��ȡ��������� */
HPSOCKET_API DWORD __stdcall HP_UdpAgent_GetDetectInterval(HP_UdpAgent pAgent);

/******************************************************************************/
/***************************** Client ����������� *****************************/

/*
* ���ƣ�����ͨ�����
* �����������ͻ���ͨ����������ӷ���ˣ�������ɺ�ɿ�ʼ�շ�����
*		
* ������		pszRemoteAddress	-- ����˵�ַ
*			usPort				-- ����˶˿�
*			bAsyncConnect		-- �Ƿ�����첽 Connect
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __stdcall HP_Client_Start(HP_Client pClient, LPCTSTR pszRemoteAddress, USHORT usPort, BOOL bAsyncConnect);

/*
* ���ƣ��ر�ͨ�����
* �������رտͻ���ͨ��������ر���ɺ�Ͽ������˵����Ӳ��ͷ�������Դ
*		
* ������	
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __stdcall HP_Client_Stop(HP_Client pClient);

/*
* ���ƣ���������
* �����������˷�������
*		
* ������		pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� Windows API ���� ::GetLastError() ��ȡ Windows �������
*/
HPSOCKET_API BOOL __stdcall HP_Client_Send(HP_Client pClient, const BYTE* pBuffer, int iLength);

/*
* ���ƣ���������
* �����������˷�������
*		
* ������		pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
*			iOffset		-- ���ͻ�����ָ��ƫ����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� Windows API ���� ::GetLastError() ��ȡ Windows �������
*/
HPSOCKET_API BOOL __stdcall HP_Client_SendPart(HP_Client pClient, const BYTE* pBuffer, int iLength, int iOffset);

/*
* ���ƣ����Ͷ�������
* �����������˷��Ͷ�������
*		TCP - ˳�����������ݰ� 
*		UDP - ���������ݰ���ϳ�һ�����ݰ����ͣ����ݰ����ܳ��Ȳ��ܴ������õ� UDP ����󳤶ȣ� 
*		
* ������		pBuffers	-- ���ͻ���������
*			iCount		-- ���ͻ�������Ŀ
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� Windows API ���� ::GetLastError() ��ȡ Windows �������
*/
HPSOCKET_API BOOL __stdcall HP_Client_SendPackets(HP_Client pClient, const WSABUF pBuffers[], int iCount);

/******************************************************************************/
/***************************** Client ���Է��ʷ��� *****************************/

/* �������ӵĸ������� */
HPSOCKET_API void __stdcall HP_Client_SetExtra(HP_Client pClient, PVOID pExtra);
/* ��ȡ���ӵĸ������� */
HPSOCKET_API PVOID __stdcall HP_Client_GetExtra(HP_Client pClient);

/* ���ͨ������Ƿ������� */
HPSOCKET_API BOOL __stdcall HP_Client_HasStarted(HP_Client pClient);
/* �鿴ͨ�������ǰ״̬ */
HPSOCKET_API En_HP_ServiceState	__stdcall HP_Client_GetState(HP_Client pClient);
/* ��ȡ���һ��ʧ�ܲ����Ĵ������ */
HPSOCKET_API En_HP_SocketError	__stdcall HP_Client_GetLastError(HP_Client pClient);
/* ��ȡ���һ��ʧ�ܲ����Ĵ������� */
HPSOCKET_API LPCTSTR __stdcall HP_Client_GetLastErrorDesc(HP_Client pClient);
/* ��ȡ�������������� ID */
HPSOCKET_API HP_CONNID __stdcall HP_Client_GetConnectionID(HP_Client pClient);
/* ��ȡ Client Socket �ĵ�ַ��Ϣ */
HPSOCKET_API BOOL __stdcall HP_Client_GetLocalAddress(HP_Client pClient, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort);
/* ��ȡ������δ�������ݵĳ��� */
HPSOCKET_API BOOL __stdcall HP_Client_GetPendingDataLength(HP_Client pClient, int* piPending);
/* �����ڴ�黺��ش�С��ͨ������Ϊ -> PUSH ģ�ͣ�5 - 10��PULL ģ�ͣ�10 - 20 �� */
HPSOCKET_API void __stdcall HP_Client_SetFreeBufferPoolSize(HP_Client pClient, DWORD dwFreeBufferPoolSize);
/* �����ڴ�黺��ػ��շ�ֵ��ͨ������Ϊ�ڴ�黺��ش�С�� 3 ���� */
HPSOCKET_API void __stdcall HP_Client_SetFreeBufferPoolHold(HP_Client pClient, DWORD dwFreeBufferPoolHold);
/* ��ȡ�ڴ�黺��ش�С */
HPSOCKET_API DWORD __stdcall HP_Client_GetFreeBufferPoolSize(HP_Client pClient);
/* ��ȡ�ڴ�黺��ػ��շ�ֵ */
HPSOCKET_API DWORD __stdcall HP_Client_GetFreeBufferPoolHold(HP_Client pClient);

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
*			FALSE	-- ʧ�ܣ���ͨ�� Windows API ���� ::GetLastError() ��ȡ Windows �������
*/
HPSOCKET_API BOOL __stdcall HP_TcpClient_SendSmallFile(HP_Client pClient, LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail);

/**********************************************************************************/
/***************************** TCP Client ���Է��ʷ��� *****************************/

/* ����ͨ�����ݻ�������С������ƽ��ͨ�����ݰ���С�������ã�ͨ������Ϊ��(N * 1024) - sizeof(TBufferObj)�� */
HPSOCKET_API void __stdcall HP_TcpClient_SetSocketBufferSize(HP_TcpClient pClient, DWORD dwSocketBufferSize);
/* ����������������������룬0 �򲻷�����������Ĭ�ϣ�30 * 1000�� */
HPSOCKET_API void __stdcall HP_TcpClient_SetKeepAliveTime(HP_TcpClient pClient, DWORD dwKeepAliveTime);
/* �����쳣��������������룬0 ����������������Ĭ�ϣ�10 * 1000������������ɴ� [Ĭ�ϣ�WinXP 5 ��, Win7 10 ��] ��ⲻ������ȷ�ϰ�����Ϊ�Ѷ��ߣ� */
HPSOCKET_API void __stdcall HP_TcpClient_SetKeepAliveInterval(HP_TcpClient pClient, DWORD dwKeepAliveInterval);

/* ��ȡͨ�����ݻ�������С */
HPSOCKET_API DWORD __stdcall HP_TcpClient_GetSocketBufferSize(HP_TcpClient pClient);
/* ��ȡ������������� */
HPSOCKET_API DWORD __stdcall HP_TcpClient_GetKeepAliveTime(HP_TcpClient pClient);
/* ��ȡ�쳣��������� */
HPSOCKET_API DWORD __stdcall HP_TcpClient_GetKeepAliveInterval(HP_TcpClient pClient);

/**********************************************************************************/
/***************************** UDP Client ���Է��ʷ��� *****************************/

/* �������ݱ�����󳤶ȣ������ھ����������²����� 1472 �ֽڣ��ڹ����������²����� 548 �ֽڣ� */
HPSOCKET_API void __stdcall HP_UdpClient_SetMaxDatagramSize(HP_UdpClient pClient, DWORD dwMaxDatagramSize);
/* ��ȡ���ݱ�����󳤶� */
HPSOCKET_API DWORD __stdcall HP_UdpClient_GetMaxDatagramSize(HP_UdpClient pClient);

/* ���ü������Դ�����0 �򲻷��ͼ�������������������Դ�������Ϊ�Ѷ��ߣ� */
HPSOCKET_API void __stdcall HP_UdpClient_SetDetectAttempts(HP_UdpClient pClient, DWORD dwDetectAttempts);
/* ���ü������ͼ�����룬0 �����ͼ����� */
HPSOCKET_API void __stdcall HP_UdpClient_SetDetectInterval(HP_UdpClient pClient, DWORD dwDetectInterval);
/* ��ȡ���������� */
HPSOCKET_API DWORD __stdcall HP_UdpClient_GetDetectAttempts(HP_UdpClient pClient);
/* ��ȡ��������� */
HPSOCKET_API DWORD __stdcall HP_UdpClient_GetDetectInterval(HP_UdpClient pClient);

/**********************************************************************************/
/****************************** UDP Cast ���Է��ʷ��� ******************************/

/* �������ݱ�����󳤶ȣ������ھ����������²����� 1472 �ֽڣ��ڹ����������²����� 548 �ֽڣ� */
HPSOCKET_API void __stdcall HP_UdpCast_SetMaxDatagramSize(HP_UdpCast pCast, DWORD dwMaxDatagramSize);
/* ��ȡ���ݱ�����󳤶� */
HPSOCKET_API DWORD __stdcall HP_UdpCast_GetMaxDatagramSize(HP_UdpCast pCast);
/* ��ȡ��ǰ���ݱ���Զ�̵�ַ��Ϣ��ͨ���� OnReceive �¼��е��ã� */
HPSOCKET_API BOOL __stdcall HP_UdpCast_GetRemoteAddress(HP_UdpCast pCast, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort);
/* ���ð󶨵�ַ */
HPSOCKET_API void __stdcall HP_UdpCast_SetBindAdddress(HP_UdpCast pCast, LPCTSTR pszBindAddress);
/* ��ȡ�󶨵�ַ */
HPSOCKET_API LPCTSTR __stdcall HP_UdpCast_GetBindAdddress(HP_UdpCast pCast);
/* �����Ƿ����õ�ַ���û��ƣ�Ĭ�ϣ������ã� */
HPSOCKET_API void __stdcall HP_UdpCast_SetReuseAddress(HP_UdpCast pCast, BOOL bReuseAddress);
/* ����Ƿ����õ�ַ���û��� */
HPSOCKET_API BOOL __stdcall HP_UdpCast_IsReuseAddress(HP_UdpCast pCast);
/* ���ô���ģʽ���鲥��㲥�� */
HPSOCKET_API void __stdcall HP_UdpCast_SetCastMode(HP_UdpCast pCast, En_HP_CastMode enCastMode);
/* ��ȡ����ģʽ */
HPSOCKET_API En_HP_CastMode __stdcall HP_UdpCast_GetCastMode(HP_UdpCast pCast);
/* �����鲥���ĵ� TTL��0 - 255�� */
HPSOCKET_API void __stdcall HP_UdpCast_SetMultiCastTtl(HP_UdpCast pCast, int iMCTtl);
/* ��ȡ�鲥���ĵ� TTL */
HPSOCKET_API int __stdcall HP_UdpCast_GetMultiCastTtl(HP_UdpCast pCast);
/* �����Ƿ������鲥��·��TRUE or FALSE�� */
HPSOCKET_API void __stdcall HP_UdpCast_SetMultiCastLoop(HP_UdpCast pCast, BOOL bMCLoop);
/* ����Ƿ������鲥��· */
HPSOCKET_API BOOL __stdcall HP_UdpCast_IsMultiCastLoop(HP_UdpCast pCast);

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
HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullServer_Fetch(HP_TcpPullServer pServer, HP_CONNID dwConnID, BYTE* pData, int iLength);

/*
* ���ƣ���̽���ݣ������Ƴ����������ݣ�
* �������û�ͨ���÷����� Socket ����п�̽����
*		
* ������		dwConnID	-- ���� ID
*			pData		-- ��̽������
*			iLength		-- ��̽���ݳ���
* ����ֵ��	En_HP_FetchResult
*/
HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullServer_Peek(HP_TcpPullServer pServer, HP_CONNID dwConnID, BYTE* pData, int iLength);

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
HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullAgent_Fetch(HP_TcpPullAgent pAgent, HP_CONNID dwConnID, BYTE* pData, int iLength);

/*
* ���ƣ���̽���ݣ������Ƴ����������ݣ�
* �������û�ͨ���÷����� Socket ����п�̽����
*		
* ������		dwConnID	-- ���� ID
*			pData		-- ��̽������
*			iLength		-- ��̽���ݳ���
* ����ֵ��	En_HP_FetchResult
*/
HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullAgent_Peek(HP_TcpPullAgent pAgent, HP_CONNID dwConnID, BYTE* pData, int iLength);

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
HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullClient_Fetch(HP_TcpPullClient pClient, BYTE* pData, int iLength);

/*
* ���ƣ���̽���ݣ������Ƴ����������ݣ�
* �������û�ͨ���÷����� Socket ����п�̽����
*		
* ������		dwConnID	-- ���� ID
*			pData		-- ��̽������
*			iLength		-- ��̽���ݳ���
* ����ֵ��	En_HP_FetchResult
*/
HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullClient_Peek(HP_TcpPullClient pClient, BYTE* pData, int iLength);

/***************************************************************************************/
/***************************** TCP Pull Client ���Է��ʷ��� *****************************/

/***************************************************************************************/
/***************************** TCP Pack Server ����������� *****************************/

/***************************************************************************************/
/***************************** TCP Pack Server ���Է��ʷ��� *****************************/

/* �������ݰ���󳤶ȣ���Ч���ݰ���󳤶Ȳ��ܳ��� 524287/0x7FFFF �ֽڣ�Ĭ�ϣ�262144/0x40000�� */
HPSOCKET_API void __stdcall HP_TcpPackServer_SetMaxPackSize(HP_TcpPackServer pServer, DWORD dwMaxPackSize);
/* ���ð�ͷ��ʶ����Ч��ͷ��ʶȡֵ��Χ 0 ~ 8191/0x1FFF������ͷ��ʶΪ 0 ʱ��У���ͷ��Ĭ�ϣ�0�� */
HPSOCKET_API void __stdcall HP_TcpPackServer_SetPackHeaderFlag(HP_TcpPackServer pServer, USHORT usPackHeaderFlag);

/* ��ȡ���ݰ���󳤶� */
HPSOCKET_API DWORD __stdcall HP_TcpPackServer_GetMaxPackSize(HP_TcpPackServer pServer);
/* ��ȡ��ͷ��ʶ */
HPSOCKET_API USHORT __stdcall HP_TcpPackServer_GetPackHeaderFlag(HP_TcpPackServer pServer);

/***************************************************************************************/
/***************************** TCP Pack Agent ����������� *****************************/

/***************************************************************************************/
/***************************** TCP Pack Agent ���Է��ʷ��� *****************************/

/* �������ݰ���󳤶ȣ���Ч���ݰ���󳤶Ȳ��ܳ��� 524287/0x7FFFF �ֽڣ�Ĭ�ϣ�262144/0x40000�� */
HPSOCKET_API void __stdcall HP_TcpPackAgent_SetMaxPackSize(HP_TcpPackAgent pAgent, DWORD dwMaxPackSize);
/* ���ð�ͷ��ʶ����Ч��ͷ��ʶȡֵ��Χ 0 ~ 8191/0x1FFF������ͷ��ʶΪ 0 ʱ��У���ͷ��Ĭ�ϣ�0�� */
HPSOCKET_API void __stdcall HP_TcpPackAgent_SetPackHeaderFlag(HP_TcpPackAgent pAgent, USHORT usPackHeaderFlag);

/* ��ȡ���ݰ���󳤶� */
HPSOCKET_API DWORD __stdcall HP_TcpPackAgent_GetMaxPackSize(HP_TcpPackAgent pAgent);
/* ��ȡ��ͷ��ʶ */
HPSOCKET_API USHORT __stdcall HP_TcpPackAgent_GetPackHeaderFlag(HP_TcpPackAgent pAgent);

/***************************************************************************************/
/***************************** TCP Pack Client ����������� *****************************/

/***************************************************************************************/
/***************************** TCP Pack Client ���Է��ʷ��� *****************************/

/* �������ݰ���󳤶ȣ���Ч���ݰ���󳤶Ȳ��ܳ��� 524287/0x7FFFF �ֽڣ�Ĭ�ϣ�262144/0x40000�� */
HPSOCKET_API void __stdcall HP_TcpPackClient_SetMaxPackSize(HP_TcpPackClient pClient, DWORD dwMaxPackSize);
/* ���ð�ͷ��ʶ����Ч��ͷ��ʶȡֵ��Χ 0 ~ 8191/0x1FFF������ͷ��ʶΪ 0 ʱ��У���ͷ��Ĭ�ϣ�0�� */
HPSOCKET_API void __stdcall HP_TcpPackClient_SetPackHeaderFlag(HP_TcpPackClient pClient, USHORT usPackHeaderFlag);

/* ��ȡ���ݰ���󳤶� */
HPSOCKET_API DWORD __stdcall HP_TcpPackClient_GetMaxPackSize(HP_TcpPackClient pClient);
/* ��ȡ��ͷ��ʶ */
HPSOCKET_API USHORT __stdcall HP_TcpPackClient_GetPackHeaderFlag(HP_TcpPackClient pClient);

/***************************************************************************************/
/*************************************** �������� ***************************************/

/* ��ȡ���������ı� */
HPSOCKET_API LPCTSTR __stdcall HP_GetSocketErrorDesc(En_HP_SocketError enCode);
/* ����ϵͳ�� ::GetLastError() ������ȡϵͳ������� */
HPSOCKET_API DWORD __stdcall SYS_GetLastError();
// ����ϵͳ�� ::WSAGetLastError() ������ȡͨ�Ŵ������
HPSOCKET_API int __stdcall SYS_WSAGetLastError();
// ����ϵͳ�� setsockopt()
HPSOCKET_API int __stdcall SYS_SetSocketOption(SOCKET sock, int level, int name, LPVOID val, int len);
// ����ϵͳ�� getsockopt()
HPSOCKET_API int __stdcall SYS_GetSocketOption(SOCKET sock, int level, int name, LPVOID val, int* len);
// ����ϵͳ�� ioctlsocket()
HPSOCKET_API int __stdcall SYS_IoctlSocket(SOCKET sock, long cmd, u_long* arg);
// ����ϵͳ�� ::WSAIoctl()
HPSOCKET_API int __stdcall SYS_WSAIoctl(SOCKET sock, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer, LPDWORD lpcbBytesReturned);
