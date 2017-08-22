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
Module:  HPSocket-SSL for C

Desc: ������ C ���������������ԣ��磺C / C# / Delphi �ȣ��ܷ����ʹ�� HPSocket SSL

Usage:
		������
		--------------------------------------------------------------------------------------
		0. ��C/C++ ���򣩰��� HPTypeDef.h / HPSocket4C.h / HPSocket4C-SSL.h ͷ�ļ�
		1. ÿ������ͨ��ǰ���� HP_SSL<Xxx>_SetupSSLContext() ������ʼ�� SSL ������������ȷ���������سɹ�
		2. ʹ�� HPSocket SSL ���ִ��ͨ�ţ��ο���HPSocket4C.h��

Release:
		<-- ��̬���ӿ� -->
		1. x86/HPSocket4C-SSL.dll			- (32λ/MBCS/Release)
		2. x86/HPSocket4C-SSL_D.dll			- (32λ/MBCS/DeBug)
		3. x86/HPSocket4C-SSL_U.dll			- (32λ/UNICODE/Release)
		4. x86/HPSocket4C-SSL_UD.dll		- (32λ/UNICODE/DeBug)
		5. x64/HPSocket4C-SSL.dll			- (64λ/MBCS/Release)
		6. x64/HPSocket4C-SSL_D.dll			- (64λ/MBCS/DeBug)
		7. x64/HPSocket4C-SSL_U.dll			- (64λ/UNICODE/Release)
		8. x64/HPSocket4C-SSL_UD.dll		- (64λ/UNICODE/DeBug)

		<-- ��̬���ӿ� -->
		!!ע��!!��ʹ�� HPSocket ��̬��ʱ����Ҫ�ڹ��������ж���Ԥ����� -> HPSOCKET_STATIC_LIB
		1. x86/static/HPSocket4C-SSL.lib	- (32λ/MBCS/Release)
		2. x86/static/HPSocket4C-SSL_D.lib	- (32λ/MBCS/DeBug)
		3. x86/static/HPSocket4C-SSL_U.lib	- (32λ/UNICODE/Release)
		4. x86/static/HPSocket4C-SSL_UD.lib	- (32λ/UNICODE/DeBug)
		5. x64/static/HPSocket4C-SSL.lib	- (64λ/MBCS/Release)
		6. x64/static/HPSocket4C-SSL_D.lib	- (64λ/MBCS/DeBug)
		7. x64/static/HPSocket4C-SSL_U.lib	- (64λ/UNICODE/Release)
		8. x64/static/HPSocket4C-SSL_UD.lib	- (64λ/UNICODE/DeBug)

******************************************************************************/

#pragma once

#ifndef _SSL_SUPPORT
	#define _SSL_SUPPORT
#endif

#include "HPSocket4C.h"

/************************************************************************
���ƣ����� SSL Socket ����ָ�����ͱ���
�������� SSL Socket ����ָ�붨��Ϊ��ֱ�۵ı���
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
/************** HPSocket4C-SSL ���󴴽����� **************/

// ���� HP_SSLServer ����
HPSOCKET_API HP_SSLServer __stdcall Create_HP_SSLServer(HP_TcpServerListener pListener);
// ���� HP_SSLAgent ����
HPSOCKET_API HP_SSLAgent __stdcall Create_HP_SSLAgent(HP_TcpAgentListener pListener);
// ���� HP_SSLClient ����
HPSOCKET_API HP_SSLClient __stdcall Create_HP_SSLClient(HP_TcpClientListener pListener);
// ���� HP_SSLPullServer ����
HPSOCKET_API HP_SSLPullServer __stdcall Create_HP_SSLPullServer(HP_TcpPullServerListener pListener);
// ���� HP_SSLPullAgent ����
HPSOCKET_API HP_SSLPullAgent __stdcall Create_HP_SSLPullAgent(HP_TcpPullAgentListener pListener);
// ���� HP_SSLPullClient ����
HPSOCKET_API HP_SSLPullClient __stdcall Create_HP_SSLPullClient(HP_TcpPullClientListener pListener);
// ���� HP_SSLPackServer ����
HPSOCKET_API HP_SSLPackServer __stdcall Create_HP_SSLPackServer(HP_TcpServerListener pListener);
// ���� HP_SSLPackAgent ����
HPSOCKET_API HP_SSLPackAgent __stdcall Create_HP_SSLPackAgent(HP_TcpAgentListener pListener);
// ���� HP_SSLPackClient ����
HPSOCKET_API HP_SSLPackClient __stdcall Create_HP_SSLPackClient(HP_TcpClientListener pListener);

// ���� HP_SSLServer ����
HPSOCKET_API void __stdcall Destroy_HP_SSLServer(HP_SSLServer pServer);
// ���� HP_SSLAgent ����
HPSOCKET_API void __stdcall Destroy_HP_SSLAgent(HP_SSLAgent pAgent);
// ���� HP_SSLClient ����
HPSOCKET_API void __stdcall Destroy_HP_SSLClient(HP_SSLClient pClient);
// ���� HP_SSLPullServer ����
HPSOCKET_API void __stdcall Destroy_HP_SSLPullServer(HP_SSLPullServer pServer);
// ���� HP_SSLPullAgent ����
HPSOCKET_API void __stdcall Destroy_HP_SSLPullAgent(HP_SSLPullAgent pAgent);
// ���� HP_SSLPullClient ����
HPSOCKET_API void __stdcall Destroy_HP_SSLPullClient(HP_SSLPullClient pClient);
// ���� HP_SSLPackServer ����
HPSOCKET_API void __stdcall Destroy_HP_SSLPackServer(HP_SSLPackServer pServer);
// ���� HP_SSLPackAgent ����
HPSOCKET_API void __stdcall Destroy_HP_SSLPackAgent(HP_SSLPackAgent pAgent);
// ���� HP_SSLPackClient ����
HPSOCKET_API void __stdcall Destroy_HP_SSLPackClient(HP_SSLPackClient pClient);

/*****************************************************************************************************************************************************/
/******************************************************************** HTTPS Exports ******************************************************************/
/*****************************************************************************************************************************************************/

/****************************************************/
/**************** HTTPS ���󴴽����� *****************/

// ���� HP_HttpsServer ����
HPSOCKET_API HP_HttpsServer __stdcall Create_HP_HttpsServer(HP_HttpServerListener pListener);
// ���� HP_HttpsAgent ����
HPSOCKET_API HP_HttpsAgent __stdcall Create_HP_HttpsAgent(HP_HttpAgentListener pListener);
// ���� HP_HttpsClient ����
HPSOCKET_API HP_HttpsClient __stdcall Create_HP_HttpsClient(HP_HttpClientListener pListener);
// ���� HP_HttpSyncClient ����
HPSOCKET_API HP_HttpsSyncClient __stdcall Create_HP_HttpsSyncClient(HP_HttpClientListener pListener);

// ���� HP_HttpsServer ����
HPSOCKET_API void __stdcall Destroy_HP_HttpsServer(HP_HttpsServer pServer);
// ���� HP_HttpsAgent ����
HPSOCKET_API void __stdcall Destroy_HP_HttpsAgent(HP_HttpsAgent pAgent);
// ���� HP_HttpsClient ����
HPSOCKET_API void __stdcall Destroy_HP_HttpsClient(HP_HttpsClient pClient);
// ���� HP_HttpSyncClient ����
HPSOCKET_API void __stdcall Destroy_HP_HttpsSyncClient(HP_HttpsSyncClient pClient);

/*****************************************************************************************************************************************************/
/*************************************************************** Global Function Exports *************************************************************/
/*****************************************************************************************************************************************************/

/***************************************************************************************/
/************************************ SSL ��ʼ������ ************************************/

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

/*
* ���ƣ���ʼ��ͨ����� SSL ��������
* ������SSL �������������� SSL ͨ���������ǰ��ɳ�ʼ������������ʧ��
*		
* ������		iVerifyMode				-- SSL ��֤ģʽ���ο� EnSSLVerifyMode��
*			lpszPemCertFile			-- ֤���ļ�
*			lpszPemKeyFile			-- ˽Կ�ļ�
*			lpszKeyPasswod			-- ˽Կ���루û��������Ϊ�գ�
*			lpszCAPemCertFileOrPath	-- CA ֤���ļ���Ŀ¼��������֤��ͻ��˿�ѡ��
*			fnServerNameCallback	-- SNI �ص�����ָ�루��ѡ��
*
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡʧ��ԭ��
*/
HPSOCKET_API BOOL __stdcall HP_SSLServer_SetupSSLContext(HP_SSLServer pServer, int iVerifyMode /* SSL_VM_NONE */, LPCTSTR lpszPemCertFile /* nullptr */, LPCTSTR lpszPemKeyFile /* nullptr */, LPCTSTR lpszKeyPasswod /* nullptr */, LPCTSTR lpszCAPemCertFileOrPath /* nullptr */, HP_Fn_SNI_ServerNameCallback fnServerNameCallback /* nullptr */);

/*
* ���ƣ����� SNI ����֤��
* ������SSL ������� SetupSSLContext() �ɹ�����Ե��ñ��������Ӷ�� SNI ����֤��
*		
* ������		iVerifyMode				-- SSL ��֤ģʽ���ο� EnSSLVerifyMode��
*			lpszPemCertFile			-- ֤���ļ�
*			lpszPemKeyFile			-- ˽Կ�ļ�
*			lpszKeyPasswod			-- ˽Կ���루û��������Ϊ�գ�
*			lpszCAPemCertFileOrPath	-- CA ֤���ļ���Ŀ¼��������֤��ѡ��
*
* ����ֵ��	����		-- �ɹ��������� SNI ����֤���Ӧ�������������������� SNI �ص������ж�λ SNI ����
*			����		-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡʧ��ԭ��
*/
HPSOCKET_API int __stdcall HP_SSLServer_AddSSLContext(HP_SSLServer pServer, int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod /* nullptr */, LPCTSTR lpszCAPemCertFileOrPath /* nullptr */);

/*
* ���ƣ�����ͨ����� SSL ���л���
* ����������ͨ����� SSL ���л��������� SSL ����ڴ�
*		1��ͨ���������ʱ���Զ����ñ�����
*		2����Ҫ��������ͨ����� SSL ��������ʱ����Ҫ�ȵ��ñ���������ԭ�ȵĻ�������
*		
* ������	��
* 
* ����ֵ����
*/
HPSOCKET_API void __stdcall HP_SSLServer_CleanupSSLContext(HP_SSLServer pServer);

/*
* ���ƣ���ʼ��ͨ����� SSL ��������
* ������SSL �������������� SSL ͨ���������ǰ��ɳ�ʼ������������ʧ��
*		
* ������		iVerifyMode				-- SSL ��֤ģʽ���ο� EnSSLVerifyMode��
*			lpszPemCertFile			-- ֤���ļ����ͻ��˿�ѡ��
*			lpszPemKeyFile			-- ˽Կ�ļ����ͻ��˿�ѡ��
*			lpszKeyPasswod			-- ˽Կ���루û��������Ϊ�գ�
*			lpszCAPemCertFileOrPath	-- CA ֤���ļ���Ŀ¼��������֤��ͻ��˿�ѡ��
*
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡʧ��ԭ��
*/
HPSOCKET_API BOOL __stdcall HP_SSLAgent_SetupSSLContext(HP_SSLAgent pAgent, int iVerifyMode /* SSL_VM_NONE */, LPCTSTR lpszPemCertFile /* nullptr */, LPCTSTR lpszPemKeyFile /* nullptr */, LPCTSTR lpszKeyPasswod /* nullptr */, LPCTSTR lpszCAPemCertFileOrPath /* nullptr */);

/*
* ���ƣ�����ͨ����� SSL ���л���
* ����������ͨ����� SSL ���л��������� SSL ����ڴ�
*		1��ͨ���������ʱ���Զ����ñ�����
*		2����Ҫ��������ͨ����� SSL ��������ʱ����Ҫ�ȵ��ñ���������ԭ�ȵĻ�������
*		
* ������	��
* 
* ����ֵ����
*/
HPSOCKET_API void __stdcall HP_SSLAgent_CleanupSSLContext(HP_SSLAgent pAgent);

/*
* ���ƣ���ʼ��ͨ����� SSL ��������
* ������SSL �������������� SSL ͨ���������ǰ��ɳ�ʼ������������ʧ��
*		
* ������		iVerifyMode				-- SSL ��֤ģʽ���ο� EnSSLVerifyMode��
*			lpszPemCertFile			-- ֤���ļ����ͻ��˿�ѡ��
*			lpszPemKeyFile			-- ˽Կ�ļ����ͻ��˿�ѡ��
*			lpszKeyPasswod			-- ˽Կ���루û��������Ϊ�գ�
*			lpszCAPemCertFileOrPath	-- CA ֤���ļ���Ŀ¼��������֤��ͻ��˿�ѡ��
*
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡʧ��ԭ��
*/
HPSOCKET_API BOOL __stdcall HP_SSLClient_SetupSSLContext(HP_SSLClient pClient, int iVerifyMode /* SSL_VM_NONE */, LPCTSTR lpszPemCertFile /* nullptr */, LPCTSTR lpszPemKeyFile /* nullptr */, LPCTSTR lpszKeyPasswod /* nullptr */, LPCTSTR lpszCAPemCertFileOrPath /* nullptr */);

/*
* ���ƣ�����ͨ����� SSL ���л���
* ����������ͨ����� SSL ���л��������� SSL ����ڴ�
*		1��ͨ���������ʱ���Զ����ñ�����
*		2����Ҫ��������ͨ����� SSL ��������ʱ����Ҫ�ȵ��ñ���������ԭ�ȵĻ�������
*		
* ������	��
* 
* ����ֵ����
*/
HPSOCKET_API void __stdcall HP_SSLClient_CleanupSSLContext(HP_SSLClient pClient);
