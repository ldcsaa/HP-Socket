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
 
#pragma once

#include <winsock2.h>

/************************************************************************
���ƣ����� ID ��������
������Ӧ�ó�����԰� CONNID ����Ϊ������Ҫ�����ͣ��磺ULONG / ULONGLONG��
************************************************************************/
typedef ULONG_PTR	CONNID;

/*****************************************************************************************************/
/******************************************** �����ࡢ�ӿ� ********************************************/
/*****************************************************************************************************/

/************************************************************************
���ƣ�ͨ���������״̬
������Ӧ�ó������ͨ��ͨ������� GetState() ������ȡ�����ǰ����״̬
************************************************************************/
enum EnServiceState
{
	SS_STARTING	= 0,	// ��������
	SS_STARTED	= 1,	// �Ѿ�����
	SS_STOPPING	= 2,	// ����ֹͣ
	SS_STOPPED	= 3,	// �Ѿ�ֹͣ
};

/************************************************************************
���ƣ�Socket ��������
������Ӧ�ó���� OnClose() �¼���ͨ���ò�����ʶ�����ֲ������µĴ���
************************************************************************/
enum EnSocketOperation
{
	SO_UNKNOWN	= 0,	// Unknown
	SO_ACCEPT	= 1,	// Acccept
	SO_CONNECT	= 2,	// Connect
	SO_SEND		= 3,	// Send
	SO_RECEIVE	= 4,	// Receive
	SO_CLOSE	= 5,	// Close
};

/************************************************************************
���ƣ��¼�������
�������¼��ķ���ֵ����ͬ�ķ���ֵ��Ӱ��ͨ������ĺ�����Ϊ
************************************************************************/
enum EnHandleResult
{
	HR_OK		= 0,	// �ɹ�
	HR_IGNORE	= 1,	// ����
	HR_ERROR	= 2,	// ����
};

/************************************************************************
���ƣ�����ץȡ���
����������ץȡ�����ķ���ֵ
************************************************************************/
enum EnFetchResult
{
	FR_OK				= 0,	// �ɹ�
	FR_LENGTH_TOO_LONG	= 1,	// ץȡ���ȹ���
	FR_DATA_NOT_FOUND	= 2,	// �Ҳ��� ConnID ��Ӧ������
};

/************************************************************************
���ƣ����ݷ��Ͳ���
������Server ����� Agent ��������ݷ��Ͳ���

* ���ģʽ��Ĭ�ϣ�	�������Ѷ�����Ͳ��������������һ���ͣ����Ӵ���Ч��
* ��ȫģʽ			�������Ѷ�����Ͳ��������������һ���ͣ������ƴ����ٶȣ����⻺�������
* ֱ��ģʽ			����ÿһ�����Ͳ�����ֱ��Ͷ�ݣ������ڸ��ز��ߵ�Ҫ��ʵʱ�Խϸߵĳ���

************************************************************************/
enum EnSendPolicy
{
	SP_PACK				= 0,	// ���ģʽ��Ĭ�ϣ�
	SP_SAFE				= 1,	// ��ȫģʽ
	SP_DIRECT			= 2,	// ֱ��ģʽ
};

/************************************************************************
���ƣ������������
��������� Start() / Stop() ����ִ��ʧ��ʱ����ͨ�� GetLastError() ��ȡ�������
************************************************************************/
enum EnSocketError
{
	SE_OK						= NO_ERROR,	// �ɹ�
	SE_ILLEGAL_STATE			= 1,		// ��ǰ״̬���������
	SE_INVALID_PARAM			= 2,		// �Ƿ�����
	SE_SOCKET_CREATE			= 3,		// ���� SOCKET ʧ��
	SE_SOCKET_BIND				= 4,		// �� SOCKET ʧ��
	SE_SOCKET_PREPARE			= 5,		// ���� SOCKET ʧ��
	SE_SOCKET_LISTEN			= 6,		// ���� SOCKET ʧ��
	SE_CP_CREATE				= 7,		// ������ɶ˿�ʧ��
	SE_WORKER_THREAD_CREATE		= 8,		// ���������߳�ʧ��
	SE_DETECT_THREAD_CREATE		= 9,		// ��������߳�ʧ��
	SE_SOCKE_ATTACH_TO_CP		= 10,		// ����ɶ˿�ʧ��
	SE_CONNECT_SERVER			= 11,		// ���ӷ�����ʧ��
	SE_NETWORK					= 12,		// �������
	SE_DATA_PROC				= 13,		// ���ݴ������
	SE_DATA_SEND				= 14,		// ���ݷ���ʧ��
};

/************************************************************************
���ƣ�����ģʽ
������UDP ����Ĳ���ģʽ���鲥��㲥��
************************************************************************/
enum EnCastMode
{
	CM_MULTICAST	= 0,	// �鲥
	CM_BROADCAST	= 1,	// �㲥
};

/************************************************************************
���ƣ�Socket ���������ӿ�
�������������˺Ϳͻ��� Socket �������Ĺ�����Ϣ
************************************************************************/
class IComplexSocketListener
{
public:

	/*
	* ���ƣ��ѷ�������֪ͨ
	* �������ɹ��������ݺ�Socket ���������յ���֪ͨ
	*		
	* ������		dwConnID	-- ���� ID
	*			pData		-- �ѷ������ݻ�����
	*			iLength		-- �ѷ������ݳ���
	* ����ֵ��	HR_OK / HR_IGNORE	-- ����ִ��
	*			HR_ERROR			-- ��֪ͨ�������� HR_ERROR������ģʽ���������Դ���
	*/
	virtual EnHandleResult OnSend(CONNID dwConnID, const BYTE* pData, int iLength)					= 0;

	/*
	* ���ƣ����ݵ���֪ͨ��PUSH ģ�ͣ�
	* ���������� PUSH ģ�͵� Socket ͨ��������ɹ��������ݺ��� Socket ���������͸�֪ͨ
	*		
	* ������		dwConnID	-- ���� ID
	*			pData		-- �ѽ������ݻ�����
	*			iLength		-- �ѽ������ݳ���
	* ����ֵ��	HR_OK / HR_IGNORE	-- ����ִ��
	*			HR_ERROR			-- ���� OnClose() �¼�ʱ�䲢�ر�����
	*/
	virtual EnHandleResult OnReceive(CONNID dwConnID, const BYTE* pData, int iLength)				= 0;

	/*
	* ���ƣ����ݵ���֪ͨ��PULL ģ�ͣ�
	* ���������� PULL ģ�͵� Socket ͨ��������ɹ��������ݺ��� Socket ���������͸�֪ͨ
	*		
	* ������		dwConnID	-- ���� ID
	*			iLength		-- �ѽ������ݳ���
	* ����ֵ��	HR_OK / HR_IGNORE	-- ����ִ��
	*			HR_ERROR			-- ���� OnClose() �¼�ʱ�䲢�ر�����
	*/
	virtual EnHandleResult OnReceive(CONNID dwConnID, int iLength)									= 0;

	/*
	* ���ƣ�ͨ�Ŵ���֪ͨ
	* ������ͨ�ŷ��������Socket ���������յ���֪ͨ�����ر�����
	*		
	* ������		dwConnID	-- ���� ID
	*			enOperation	-- Socket ��������
	*			iErrorCode	-- �������
	* ����ֵ��	���Է���ֵ
	*/
	virtual EnHandleResult OnClose(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)	= 0;

	/*
	* ���ƣ��ر�ͨ�����֪ͨ
	* ������ͨ������ر�ʱ��Socket ���������յ���֪ͨ
	*		
	* ������	
	* ����ֵ�����Է���ֵ
	*/
	virtual EnHandleResult OnShutdown()																= 0;

public:
	virtual ~IComplexSocketListener() {}
};

/************************************************************************
���ƣ������ Socket �������ӿ�
�������������� Socket �������������¼�
************************************************************************/
class IServerListener : public IComplexSocketListener
{
public:

	/*
	* ���ƣ�׼������֪ͨ
	* ������ͨ�ŷ�����������ʱ���ڼ��� Socket ������ɲ���ʼִ�м���ǰ��Socket ����
	*		�����յ���֪ͨ��������������֪ͨ��������ִ�� Socket ѡ�����õȶ��⹤��
	*		
	* ������		soListen	-- ���� Socket
	* ����ֵ��	HR_OK / HR_IGNORE	-- ����ִ��
	*			HR_ERROR			-- ��ֹ����ͨ�ŷ������
	*/
	virtual EnHandleResult OnPrepareListen(SOCKET soListen)				= 0;
};

/************************************************************************
���ƣ�TCP ����� Socket �������ӿ�
���������� TCP ����� Socket �������������¼�
************************************************************************/
class ITcpServerListener : public IServerListener
{
public:

	/*
	* ���ƣ���������֪ͨ
	* ���������յ��ͻ�����������ʱ��Socket ���������յ���֪ͨ��������������֪ͨ����
	*		����ִ�� Socket ѡ�����û�ܾ��ͻ������ӵȶ��⹤��
	*		
	* ������		dwConnID	-- ���� ID
	*			soClient	-- �ͻ��� Socket
	* ����ֵ��	HR_OK / HR_IGNORE	-- ��������
	*			HR_ERROR			-- �ܾ�����
	*/
	virtual EnHandleResult OnAccept(CONNID dwConnID, SOCKET soClient)	= 0;
};

/************************************************************************
���ƣ�PUSH ģ�ͷ���� Socket �������������
����������ĳЩ�¼���Ĭ�ϴ������������¼���
************************************************************************/
class CTcpServerListener : public ITcpServerListener
{
public:
	virtual EnHandleResult OnReceive(CONNID dwConnID, int iLength)					{return HR_IGNORE;}
	virtual EnHandleResult OnSend(CONNID dwConnID, const BYTE* pData, int iLength)	{return HR_IGNORE;}
	virtual EnHandleResult OnPrepareListen(SOCKET soListen)							{return HR_IGNORE;}
	virtual EnHandleResult OnAccept(CONNID dwConnID, SOCKET soClient)				{return HR_IGNORE;}
	virtual EnHandleResult OnShutdown()												{return HR_IGNORE;}
};

/************************************************************************
���ƣ�PULL ģ�ͷ���� Socket �������������
����������ĳЩ�¼���Ĭ�ϴ������������¼���
************************************************************************/
class CTcpPullServerListener : public CTcpServerListener
{
public:
	virtual EnHandleResult OnReceive(CONNID dwConnID, int iLength)						= 0;
	virtual EnHandleResult OnReceive(CONNID dwConnID, const BYTE* pData, int iLength)	{return HR_IGNORE;}
};

/************************************************************************
���ƣ�UDP ����� Socket �������ӿ�
���������� UDP ����� Socket �������������¼�
************************************************************************/
class IUdpServerListener : public IServerListener
{
public:

	/*
	* ���ƣ���������֪ͨ
	* ���������յ��ͻ�����������ʱ��Socket ���������յ���֪ͨ��������������֪ͨ����
	*		����ִ�� Socket ѡ�����û�ܾ��ͻ������ӵȶ��⹤��
	*		
	* ������		dwConnID	-- ���� ID
	*			soClient	-- �ͻ��� Socket
	* ����ֵ��	HR_OK / HR_IGNORE	-- ��������
	*			HR_ERROR			-- �ܾ�����
	*/
	virtual EnHandleResult OnAccept(CONNID dwConnID, const SOCKADDR_IN* pSockAddr)	= 0;
};

/************************************************************************
���ƣ�UDP ����� Socket �������������
����������ĳЩ�¼���Ĭ�ϴ������������¼���
************************************************************************/
class CUdpServerListener : public IUdpServerListener
{
public:
	virtual EnHandleResult OnReceive(CONNID dwConnID, int iLength)					{return HR_IGNORE;}
	virtual EnHandleResult OnSend(CONNID dwConnID, const BYTE* pData, int iLength)	{return HR_IGNORE;}
	virtual EnHandleResult OnPrepareListen(SOCKET soListen)							{return HR_IGNORE;}
	virtual EnHandleResult OnAccept(CONNID dwConnID, const SOCKADDR_IN* pSockAddr)	{return HR_IGNORE;}
	virtual EnHandleResult OnShutdown()												{return HR_IGNORE;}
};

/************************************************************************
���ƣ�ͨ�Ŵ��� Socket �������ӿ�
���������� ͨ�Ŵ��� Socket �������������¼�
************************************************************************/
class IAgentListener : public IComplexSocketListener
{
public:

	/*
	* ���ƣ�׼������֪ͨ
	* ������ͨ�ſͻ����������ʱ���ڿͻ��� Socket ������ɲ���ʼִ������ǰ��Socket ����
	*		�����յ���֪ͨ��������������֪ͨ��������ִ�� Socket ѡ�����õȶ��⹤��
	*		
	* ������		dwConnID	-- ���� ID
	*			socket		-- �ͻ��� Socket
	* ����ֵ��	HR_OK / HR_IGNORE	-- ����ִ��
	*			HR_ERROR			-- ��ֹ����ͨ�ſͻ������
	*/
	virtual EnHandleResult OnPrepareConnect(CONNID dwConnID, SOCKET socket)		= 0;

	/*
	* ���ƣ��������֪ͨ
	* �����������˳ɹ���������ʱ��Socket ���������յ���֪ͨ
	*		
	* ������		dwConnID	-- ���� ID
	* ����ֵ��	HR_OK / HR_IGNORE	-- ����ִ��
	*			HR_ERROR			-- ͬ�����ӣ���ֹ����ͨ�ſͻ������
	*								   �첽���ӣ��ر�����
	*/
	virtual EnHandleResult OnConnect(CONNID dwConnID)							= 0;
};

/************************************************************************
���ƣ�TCP ͨ�Ŵ��� Socket �������ӿ�
���������� TCP ͨ�Ŵ��� Socket �������������¼�
************************************************************************/
class ITcpAgentListener : public IAgentListener
{
};

/************************************************************************
���ƣ�PUSH ģ��ͨ�Ŵ��� Socket �������������
����������ĳЩ�¼���Ĭ�ϴ������������¼���
************************************************************************/
class CTcpAgentListener : public ITcpAgentListener
{
public:
	virtual EnHandleResult OnReceive(CONNID dwConnID, int iLength)						{return HR_IGNORE;}
	virtual EnHandleResult OnSend(CONNID dwConnID, const BYTE* pData, int iLength)		{return HR_IGNORE;}
	virtual EnHandleResult OnPrepareConnect(CONNID dwConnID, SOCKET socket)				{return HR_IGNORE;}
	virtual EnHandleResult OnConnect(CONNID dwConnID)									{return HR_IGNORE;}
	virtual EnHandleResult OnShutdown()													{return HR_IGNORE;}
};

/************************************************************************
���ƣ�PULL ͨ�Ŵ��� Socket �������������
����������ĳЩ�¼���Ĭ�ϴ������������¼���
************************************************************************/
class CTcpPullAgentListener : public CTcpAgentListener
{
public:
	virtual EnHandleResult OnReceive(CONNID dwConnID, int iLength)						= 0;
	virtual EnHandleResult OnReceive(CONNID dwConnID, const BYTE* pData, int iLength)	{return HR_IGNORE;}
};

/************************************************************************
���ƣ�UDP ͨ�Ŵ��� Socket �������ӿ�
���������� UDP ͨ�Ŵ��� Socket �������������¼�
************************************************************************/
class IUdpAgentListener : public IAgentListener
{
};

/************************************************************************
���ƣ�PUSH ģ��ͨ�Ŵ��� Socket �������������
����������ĳЩ�¼���Ĭ�ϴ������������¼���
************************************************************************/
class CUdpAgentListener : public IUdpAgentListener
{
public:
	virtual EnHandleResult OnReceive(CONNID dwConnID, int iLength)						{return HR_IGNORE;}
	virtual EnHandleResult OnSend(CONNID dwConnID, const BYTE* pData, int iLength)		{return HR_IGNORE;}
	virtual EnHandleResult OnPrepareConnect(CONNID dwConnID, SOCKET socket)				{return HR_IGNORE;}
	virtual EnHandleResult OnConnect(CONNID dwConnID)									{return HR_IGNORE;}
	virtual EnHandleResult OnShutdown()													{return HR_IGNORE;}
};


class IClient;

/************************************************************************
���ƣ��ͻ��� Socket �������ӿ�
����������ͻ��� Socket �������������¼�
************************************************************************/
class IClientListener
{
public:

	/*
	* ���ƣ�׼������֪ͨ
	* ������ͨ�ſͻ����������ʱ���ڿͻ��� Socket ������ɲ���ʼִ������ǰ��Socket ����
	*		�����յ���֪ͨ��������������֪ͨ��������ִ�� Socket ѡ�����õȶ��⹤��
	*		
	* ������		pClient		-- ���Ӷ���
	*			socket		-- �ͻ��� Socket
	* ����ֵ��	HR_OK / HR_IGNORE	-- ����ִ��
	*			HR_ERROR			-- ��ֹ����ͨ�ſͻ������
	*/
	virtual EnHandleResult OnPrepareConnect(IClient* pClient, SOCKET socket)						= 0;

	/*
	* ���ƣ��������֪ͨ
	* �����������˳ɹ���������ʱ��Socket ���������յ���֪ͨ
	*		
	* ������		pClient		-- ���Ӷ���
	* ����ֵ��	HR_OK / HR_IGNORE	-- ����ִ��
	*			HR_ERROR			-- ͬ�����ӣ���ֹ����ͨ�ſͻ������
	*								   �첽���ӣ��ر�����
	*/
	virtual EnHandleResult OnConnect(IClient* pClient)												= 0;

	/*
	* ���ƣ��ѷ�������֪ͨ
	* �������ɹ��������ݺ�Socket ���������յ���֪ͨ
	*		
	* ������		pClient		-- ���Ӷ���
	*			pData		-- �ѷ������ݻ�����
	*			iLength		-- �ѷ������ݳ���
	* ����ֵ��	HR_OK / HR_IGNORE	-- ����ִ��
	*			HR_ERROR			-- ��֪ͨ�������� HR_ERROR������ģʽ���������Դ���
	*/
	virtual EnHandleResult OnSend(IClient* pClient, const BYTE* pData, int iLength)					= 0;

	/*
	* ���ƣ����ݵ���֪ͨ��PUSH ģ�ͣ�
	* ���������� PUSH ģ�͵� Socket ͨ��������ɹ��������ݺ��� Socket ���������͸�֪ͨ
	*		
	* ������		pClient		-- ���Ӷ���
	*			pData		-- �ѽ������ݻ�����
	*			iLength		-- �ѽ������ݳ���
	* ����ֵ��	HR_OK / HR_IGNORE	-- ����ִ��
	*			HR_ERROR			-- ���� OnClose() �¼�ʱ�䲢�ر�����
	*/
	virtual EnHandleResult OnReceive(IClient* pClient, const BYTE* pData, int iLength)				= 0;

	/*
	* ���ƣ����ݵ���֪ͨ��PULL ģ�ͣ�
	* ���������� PULL ģ�͵� Socket ͨ��������ɹ��������ݺ��� Socket ���������͸�֪ͨ
	*		
	* ������		pClient		-- ���Ӷ���
	*			iLength		-- �ѽ������ݳ���
	* ����ֵ��	HR_OK / HR_IGNORE	-- ����ִ��
	*			HR_ERROR			-- ���� OnClose() �¼�ʱ�䲢�ر�����
	*/
	virtual EnHandleResult OnReceive(IClient* pClient, int iLength)									= 0;

	/*
	* ���ƣ�ͨ�Ŵ���֪ͨ
	* ������ͨ�ŷ��������Socket ���������յ���֪ͨ�����ر�����
	*		
	* ������		pClient		-- ���Ӷ���
	*			enOperation	-- Socket ��������
	*			iErrorCode	-- �������
	* ����ֵ��	���Է���ֵ
	*/
	virtual EnHandleResult OnClose(IClient* pClient, EnSocketOperation enOperation, int iErrorCode)	= 0;

public:
	virtual ~IClientListener() {}
};

/************************************************************************
���ƣ�TCP �ͻ��� Socket �������ӿ�
���������� TCP �ͻ��� Socket �������������¼�
************************************************************************/
class ITcpClientListener : public IClientListener
{
};

/************************************************************************
���ƣ�PUSH ģ�Ϳͻ��� Socket �������������
����������ĳЩ�¼���Ĭ�ϴ������������¼���
************************************************************************/
class CTcpClientListener : public ITcpClientListener
{
public:
	virtual EnHandleResult OnReceive(IClient* pClient, int iLength)						{return HR_IGNORE;}
	virtual EnHandleResult OnSend(IClient* pClient, const BYTE* pData, int iLength)		{return HR_IGNORE;}
	virtual EnHandleResult OnPrepareConnect(IClient* pClient, SOCKET socket)			{return HR_IGNORE;}
	virtual EnHandleResult OnConnect(IClient* pClient)									{return HR_IGNORE;}
};

/************************************************************************
���ƣ�PULL �ͻ��� Socket �������������
����������ĳЩ�¼���Ĭ�ϴ������������¼���
************************************************************************/
class CTcpPullClientListener : public CTcpClientListener
{
public:
	virtual EnHandleResult OnReceive(IClient* pClient, int iLength)						= 0;
	virtual EnHandleResult OnReceive(IClient* pClient, const BYTE* pData, int iLength)	{return HR_IGNORE;}
};

/************************************************************************
���ƣ�UDP �ͻ��� Socket �������ӿ�
���������� UDP �ͻ��� Socket �������������¼�
************************************************************************/
class IUdpClientListener : public IClientListener
{
};

/************************************************************************
���ƣ�UDP �ͻ��� Socket �������������
����������ĳЩ�¼���Ĭ�ϴ������������¼���
************************************************************************/
class CUdpClientListener : public IUdpClientListener
{
public:
	virtual EnHandleResult OnReceive(IClient* pClient, int iLength)						{return HR_IGNORE;}
	virtual EnHandleResult OnSend(IClient* pClient, const BYTE* pData, int iLength)		{return HR_IGNORE;}
	virtual EnHandleResult OnPrepareConnect(IClient* pClient, SOCKET socket)			{return HR_IGNORE;}
	virtual EnHandleResult OnConnect(IClient* pClient)									{return HR_IGNORE;}
};

/************************************************************************
���ƣ�UDP ���� Socket �������ӿ�
���������� UDP ���� Socket �������������¼�
************************************************************************/
class IUdpCastListener : public IClientListener
{
};

/************************************************************************
���ƣ�UDP ���� Socket �������������
����������ĳЩ�¼���Ĭ�ϴ������������¼���
************************************************************************/
class CUdpCastListener : public IUdpCastListener
{
public:
	virtual EnHandleResult OnReceive(IClient* pClient, int iLength)						{return HR_IGNORE;}
	virtual EnHandleResult OnSend(IClient* pClient, const BYTE* pData, int iLength)		{return HR_IGNORE;}
	virtual EnHandleResult OnPrepareConnect(IClient* pClient, SOCKET socket)			{return HR_IGNORE;}
	virtual EnHandleResult OnConnect(IClient* pClient)									{return HR_IGNORE;}
};

/************************************************************************
���ƣ����� Socket ����ӿ�
���������帴�� Socket ��������в������������Է��ʷ��������� Socket ���ͬʱ������ Socket ����
************************************************************************/
class IComplexSocket
{
public:

	/***********************************************************************/
	/***************************** ����������� *****************************/

	/*
	* ���ƣ��ر�ͨ�����
	* �������ر�ͨ��������ر���ɺ�Ͽ��������Ӳ��ͷ�������Դ
	*		
	* ������	
	* ����ֵ��	TRUE	-- �ɹ�
	*			FALSE	-- ʧ�ܣ���ͨ�� GetLastError() ��ȡ�������
	*/
	virtual BOOL Stop	()																		= 0;

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
	virtual BOOL Send	(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset = 0)	= 0;

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
	virtual BOOL SendPackets(CONNID dwConnID, const WSABUF pBuffers[], int iCount)	= 0;

	/*
	* ���ƣ��Ͽ�����
	* �������Ͽ�ĳ������
	*		
	* ������		dwConnID	-- ���� ID
	*			bForce		-- �Ƿ�ǿ�ƶϿ�����
	* ����ֵ��	TRUE	-- �ɹ�
	*			FALSE	-- ʧ��
	*/
	virtual BOOL Disconnect(CONNID dwConnID, BOOL bForce = TRUE)					= 0;

	/*
	* ���ƣ��Ͽ���ʱ����
	* �������Ͽ�����ָ��ʱ��������
	*		
	* ������		dwPeriod	-- ʱ�������룩
	*			bForce		-- �Ƿ�ǿ�ƶϿ�����
	* ����ֵ��	TRUE	-- �ɹ�
	*			FALSE	-- ʧ��
	*/
	virtual BOOL DisconnectLongConnections(DWORD dwPeriod, BOOL bForce = TRUE)		= 0;

	/*
	* ���ƣ��Ͽ���Ĭ����
	* �������Ͽ�����ָ��ʱ���ľ�Ĭ����
	*		
	* ������		dwPeriod	-- ʱ�������룩
	*			bForce		-- �Ƿ�ǿ�ƶϿ�����
	* ����ֵ��	TRUE	-- �ɹ�
	*			FALSE	-- ʧ��
	*/
	virtual BOOL DisconnectSilenceConnections(DWORD dwPeriod, BOOL bForce = TRUE)	= 0;

public:

	/***********************************************************************/
	/***************************** ���Է��ʷ��� *****************************/

	/*
	* ���ƣ��������ӵĸ�������
	* �������Ƿ�Ϊ���Ӱ󶨸������ݻ��߰�ʲô�������ݣ�����Ӧ�ó����������
	*		
	* ������		dwConnID	-- ���� ID
	*			pv			-- ����
	* ����ֵ��	TRUE	-- �ɹ�
	*			FALSE	-- ʧ�ܣ���Ч������ ID��
	*/
	virtual BOOL SetConnectionExtra		(CONNID dwConnID, PVOID pExtra)			= 0;

	/*
	* ���ƣ���ȡ���ӵĸ�������
	* �������Ƿ�Ϊ���Ӱ󶨸������ݻ��߰�ʲô�������ݣ�����Ӧ�ó����������
	*		
	* ������		dwConnID	-- ���� ID
	*			ppv			-- ����ָ��
	* ����ֵ��	TRUE	-- �ɹ�
	*			FALSE	-- ʧ�ܣ���Ч������ ID��
	*/
	virtual BOOL GetConnectionExtra			(CONNID dwConnID, PVOID* ppExtra)	= 0;

	/* ���ͨ������Ƿ������� */
	virtual BOOL HasStarted				()									= 0;
	/* �鿴ͨ�������ǰ״̬ */
	virtual EnServiceState GetState		()									= 0;
	/* ��ȡ������ */
	virtual DWORD GetConnectionCount	()									= 0;
	/* ��ȡ�������ӵ� CONNID */
	virtual BOOL GetAllConnectionIDs	(CONNID pIDs[], DWORD& dwCount)		= 0;
	/* ��ȡĳ������ʱ�������룩 */
	virtual BOOL GetConnectPeriod		(CONNID dwConnID, DWORD& dwPeriod)	= 0;
	/* ��ȡĳ�����Ӿ�Ĭʱ�䣨���룩 */
	virtual BOOL GetSilencePeriod		(CONNID dwConnID, DWORD& dwPeriod)	= 0;
	/* ��ȡĳ�����ӵ�Զ�̵�ַ��Ϣ */
	virtual BOOL GetRemoteAddress		(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)	= 0;
	/* ��ȡ���һ��ʧ�ܲ����Ĵ������ */
	virtual EnSocketError GetLastError	()									= 0;
	/* ��ȡ���һ��ʧ�ܲ����Ĵ������� */
	virtual LPCTSTR GetLastErrorDesc	()									= 0;
	/* ��ȡ������δ�������ݵĳ��� */
	virtual BOOL GetPendingDataLength	(CONNID dwConnID, int& iPending)	= 0;

	/* �������ݷ��Ͳ��� */
	virtual void SetSendPolicy				(EnSendPolicy enSendPolicy)		= 0;
	/* ���� Socket �����������ʱ�䣨���룬�������ڼ�� Socket ��������ܱ���ȡʹ�ã� */
	virtual void SetFreeSocketObjLockTime	(DWORD dwFreeSocketObjLockTime)	= 0;
	/* ���� Socket ����ش�С��ͨ������Ϊƽ���������������� 1/3 - 1/2�� */
	virtual void SetFreeSocketObjPool		(DWORD dwFreeSocketObjPool)		= 0;
	/* �����ڴ�黺��ش�С��ͨ������Ϊ Socket ����ش�С�� 2 - 3 ���� */
	virtual void SetFreeBufferObjPool		(DWORD dwFreeBufferObjPool)		= 0;
	/* ���� Socket ����ػ��շ�ֵ��ͨ������Ϊ Socket ����ش�С�� 3 ���� */
	virtual void SetFreeSocketObjHold		(DWORD dwFreeSocketObjHold)		= 0;
	/* �����ڴ�黺��ػ��շ�ֵ��ͨ������Ϊ�ڴ�黺��ش�С�� 3 ���� */
	virtual void SetFreeBufferObjHold		(DWORD dwFreeBufferObjHold)		= 0;
	/* ���ù����߳�������ͨ������Ϊ 2 * CPU + 2�� */
	virtual void SetWorkerThreadCount		(DWORD dwWorkerThreadCount)		= 0;
	/* �����Ƿ��Ǿ�Ĭʱ�䣨����Ϊ TRUE ʱ DisconnectSilenceConnections() �� GetSilencePeriod() ����Ч��Ĭ�ϣ�FALSE�� */
	virtual void SetMarkSilence				(BOOL bMarkSilence)				= 0;

	/* ��ȡ���ݷ��Ͳ��� */
	virtual EnSendPolicy GetSendPolicy		()	= 0;
	/* ��ȡ Socket �����������ʱ�� */
	virtual DWORD GetFreeSocketObjLockTime	()	= 0;
	/* ��ȡ Socket ����ش�С */
	virtual DWORD GetFreeSocketObjPool		()	= 0;
	/* ��ȡ�ڴ�黺��ش�С */
	virtual DWORD GetFreeBufferObjPool		()	= 0;
	/* ��ȡ Socket ����ػ��շ�ֵ */
	virtual DWORD GetFreeSocketObjHold		()	= 0;
	/* ��ȡ�ڴ�黺��ػ��շ�ֵ */
	virtual DWORD GetFreeBufferObjHold		()	= 0;
	/* ��ȡ�����߳����� */
	virtual DWORD GetWorkerThreadCount		()	= 0;
	/* ����Ƿ��Ǿ�Ĭʱ�� */
	virtual BOOL IsMarkSilence				()	= 0;

public:
	virtual ~IComplexSocket() {}
};

/************************************************************************
���ƣ�ͨ�ŷ��������ӿ�
����������ͨ�ŷ������������в������������Է��ʷ���
************************************************************************/
class IServer : public IComplexSocket
{
public:

	/***********************************************************************/
	/***************************** ����������� *****************************/

	/*
	* ���ƣ�����ͨ�����
	* ���������������ͨ�������������ɺ�ɿ�ʼ���տͻ������Ӳ��շ�����
	*		
	* ������		pszBindAddress	-- ������ַ
	*			usPort			-- �����˿�
	* ����ֵ��	TRUE	-- �ɹ�
	*			FALSE	-- ʧ�ܣ���ͨ�� GetLastError() ��ȡ�������
	*/
	virtual BOOL Start	(LPCTSTR pszBindAddress, USHORT usPort)								= 0;

public:

	/***********************************************************************/
	/***************************** ���Է��ʷ��� *****************************/

	/* ��ȡ���� Socket �ĵ�ַ��Ϣ */
	virtual BOOL GetListenAddress(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)	= 0;
};

/************************************************************************
���ƣ�TCP ͨ�ŷ��������ӿ�
���������� TCP ͨ�ŷ������������в������������Է��ʷ���
************************************************************************/
class ITcpServer : public IServer
{
public:

	/***********************************************************************/
	/***************************** ����������� *****************************/

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
	virtual BOOL SendSmallFile	(CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr)	= 0;

public:

	/***********************************************************************/
	/***************************** ���Է��ʷ��� *****************************/

	/* ���� Accept ԤͶ�����������ݸ��ص������ã�Accept ԤͶ������Խ����֧�ֵĲ�����������Խ�ࣩ */
	virtual void SetAcceptSocketCount	(DWORD dwAcceptSocketCount)		= 0;
	/* ����ͨ�����ݻ�������С������ƽ��ͨ�����ݰ���С�������ã�ͨ������Ϊ 1024 �ı����� */
	virtual void SetSocketBufferSize	(DWORD dwSocketBufferSize)		= 0;
	/* ���ü��� Socket �ĵȺ���д�С�����ݲ������������������ã� */
	virtual void SetSocketListenQueue	(DWORD dwSocketListenQueue)		= 0;
	/* ����������������������룬0 �򲻷�����������Ĭ�ϣ�30 * 1000�� */
	virtual void SetKeepAliveTime		(DWORD dwKeepAliveTime)			= 0;
	/* �����쳣��������������룬0 ����������������Ĭ�ϣ�10 * 1000������������ɴ� [Ĭ�ϣ�WinXP 5 ��, Win7 10 ��] ��ⲻ������ȷ�ϰ�����Ϊ�Ѷ��ߣ� */
	virtual void SetKeepAliveInterval	(DWORD dwKeepAliveInterval)		= 0;

	/* ��ȡ Accept ԤͶ������ */
	virtual DWORD GetAcceptSocketCount	()	= 0;
	/* ��ȡͨ�����ݻ�������С */
	virtual DWORD GetSocketBufferSize	()	= 0;
	/* ��ȡ���� Socket �ĵȺ���д�С */
	virtual DWORD GetSocketListenQueue	()	= 0;
	/* ��ȡ������������� */
	virtual DWORD GetKeepAliveTime		()	= 0;
	/* ��ȡ�쳣��������� */
	virtual DWORD GetKeepAliveInterval	()	= 0;
};

/************************************************************************
���ƣ�UDP ͨ�ŷ��������ӿ�
���������� UDP ͨ�ŷ������������в������������Է��ʷ���
************************************************************************/
class IUdpServer : public IServer
{
public:

	/***********************************************************************/
	/***************************** ����������� *****************************/

public:

	/***********************************************************************/
	/***************************** ���Է��ʷ��� *****************************/

	/* �������ݱ�����󳤶ȣ������ھ����������²����� 1472 �ֽڣ��ڹ����������²����� 548 �ֽڣ� */
	virtual void SetMaxDatagramSize		(DWORD dwMaxDatagramSize)	= 0;
	/* ��ȡ���ݱ�����󳤶� */
	virtual DWORD GetMaxDatagramSize	()							= 0;

	/* ���� Receive ԤͶ�����������ݸ��ص������ã�Receive ԤͶ������Խ���򶪰�����ԽС�� */
	virtual void SetPostReceiveCount	(DWORD dwPostReceiveCount)	= 0;
	/* ��ȡ Receive ԤͶ������ */
	virtual DWORD GetPostReceiveCount	()							= 0;

	/* ���ü������Դ�����0 �򲻷��ͼ�������������������Դ�������Ϊ�Ѷ��ߣ� */
	virtual void SetDetectAttempts		(DWORD dwDetectAttempts)	= 0;
	/* ���ü������ͼ�����룬0 �����ͼ����� */
	virtual void SetDetectInterval		(DWORD dwDetectInterval)	= 0;
	/* ��ȡ���������� */
	virtual DWORD GetDetectAttempts		()							= 0;
	/* ��ȡ��������� */
	virtual DWORD GetDetectInterval		()							= 0;
};

/************************************************************************
���ƣ�ͨ�Ŵ�������ӿ�
����������ͨ�Ŵ�����������в������������Է��ʷ������������������һ��ͬʱ���Ӷ���������Ŀͻ������
************************************************************************/
class IAgent : public IComplexSocket
{
public:

	/***********************************************************************/
	/***************************** ����������� *****************************/

	/*
	* ���ƣ�����ͨ�����
	* ����������ͨ�Ŵ��������������ɺ�ɿ�ʼ����Զ�̷�����
	*		
	* ������		pszBindAddress	-- ������ַ
	*			bAsyncConnect	-- �Ƿ�����첽 Connect
	* ����ֵ��	TRUE	-- �ɹ�
	*			FALSE	-- ʧ�ܣ���ͨ�� GetLastError() ��ȡ�������
	*/
	virtual BOOL Start	(LPCTSTR pszBindAddress = nullptr, BOOL bAsyncConnect = TRUE)						= 0;

	/*
	* ���ƣ����ӷ�����
	* ���������ӷ����������ӳɹ��� IAgentListener ����յ� OnConnect() �¼�
	*		
	* ������		pszRemoteAddress	-- ����˵�ַ
	*			usPort				-- ����˶˿�
	*			pdwConnID			-- ���� ID��Ĭ�ϣ�nullptr������ȡ���� ID��
	* ����ֵ��	TRUE	-- �ɹ�
	*			FALSE	-- ʧ�ܣ���ͨ�� Windows API ���� ::GetLastError() ��ȡ Windows �������
	*/
	virtual BOOL Connect(LPCTSTR pszRemoteAddress, USHORT usPort, CONNID* pdwConnID = nullptr)				= 0;

public:

	/***********************************************************************/
	/***************************** ���Է��ʷ��� *****************************/

	/* ��ȡĳ�����ӵı��ص�ַ��Ϣ */
	virtual BOOL GetLocalAddress(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)	= 0;
};

/************************************************************************
���ƣ�TCP ͨ�Ŵ�������ӿ�
���������� TCP ͨ�Ŵ�����������в������������Է��ʷ���
************************************************************************/
class ITcpAgent : public IAgent
{
public:

	/***********************************************************************/
	/***************************** ����������� *****************************/

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
	virtual BOOL SendSmallFile	(CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr)	= 0;

public:

	/***********************************************************************/
	/***************************** ���Է��ʷ��� *****************************/

	/* �����Ƿ����õ�ַ���û��ƣ�Ĭ�ϣ������ã� */
	virtual void SetReuseAddress		(BOOL bReuseAddress)			= 0;
	/* ����Ƿ����õ�ַ���û��� */
	virtual BOOL IsReuseAddress			()								= 0;

	/* ����ͨ�����ݻ�������С������ƽ��ͨ�����ݰ���С�������ã�ͨ������Ϊ 1024 �ı����� */
	virtual void SetSocketBufferSize	(DWORD dwSocketBufferSize)		= 0;
	/* ����������������������룬0 �򲻷�����������Ĭ�ϣ�30 * 1000�� */
	virtual void SetKeepAliveTime		(DWORD dwKeepAliveTime)			= 0;
	/* �����쳣��������������룬0 ����������������Ĭ�ϣ�10 * 1000������������ɴ� [Ĭ�ϣ�WinXP 5 ��, Win7 10 ��] ��ⲻ������ȷ�ϰ�����Ϊ�Ѷ��ߣ� */
	virtual void SetKeepAliveInterval	(DWORD dwKeepAliveInterval)		= 0;

	/* ��ȡͨ�����ݻ�������С */
	virtual DWORD GetSocketBufferSize	()	= 0;
	/* ��ȡ������������� */
	virtual DWORD GetKeepAliveTime		()	= 0;
	/* ��ȡ�쳣��������� */
	virtual DWORD GetKeepAliveInterval	()	= 0;
};

/************************************************************************
���ƣ�UDP ͨ�Ŵ�������ӿ�
���������� UDP ͨ�Ŵ�����������в������������Է��ʷ���
************************************************************************/
class IUdpAgent : public IAgent
{
public:

	/***********************************************************************/
	/***************************** ����������� *****************************/

public:

	/***********************************************************************/
	/***************************** ���Է��ʷ��� *****************************/

	/* �������ݱ�����󳤶ȣ������ھ����������²����� 1472 �ֽڣ��ڹ����������²����� 548 �ֽڣ� */
	virtual void SetMaxDatagramSize	(DWORD dwMaxDatagramSize)	= 0;
	/* ��ȡ���ݱ�����󳤶� */
	virtual DWORD GetMaxDatagramSize()							= 0;

	/* ���ü������Դ�����0 �򲻷��ͼ�������������������Դ�������Ϊ�Ѷ��ߣ� */
	virtual void SetDetectAttempts	(DWORD dwDetectAttempts)	= 0;
	/* ���ü������ͼ�����룬0 �����ͼ����� */
	virtual void SetDetectInterval	(DWORD dwDetectInterval)	= 0;
	/* ��ȡ���������� */
	virtual DWORD GetDetectAttempts	()							= 0;
	/* ��ȡ��������� */
	virtual DWORD GetDetectInterval	()							= 0;
};

/************************************************************************
���ƣ�ͨ�ſͻ�������ӿ�
����������ͨ�ſͻ�����������в������������Է��ʷ���
************************************************************************/
class IClient
{
public:

	/***********************************************************************/
	/***************************** ����������� *****************************/

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
	virtual BOOL Start	(LPCTSTR pszRemoteAddress, USHORT usPort, BOOL bAsyncConnect = FALSE)	= 0;

	/*
	* ���ƣ��ر�ͨ�����
	* �������رտͻ���ͨ��������ر���ɺ�Ͽ������˵����Ӳ��ͷ�������Դ
	*		
	* ������	
	* ����ֵ��	TRUE	-- �ɹ�
	*			FALSE	-- ʧ�ܣ���ͨ�� GetLastError() ��ȡ�������
	*/
	virtual BOOL Stop	()																		= 0;

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
	virtual BOOL Send	(const BYTE* pBuffer, int iLength, int iOffset = 0)						= 0;

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
	virtual BOOL SendPackets(const WSABUF pBuffers[], int iCount)								= 0;

public:

	/***********************************************************************/
	/***************************** ���Է��ʷ��� *****************************/

	/* �������ӵĸ������� */
	virtual void SetExtra					(PVOID pExtra)										= 0;

	/* ��ȡ���ӵĸ������� */
	virtual PVOID GetExtra					()													= 0;

	/* ���ͨ������Ƿ������� */
	virtual BOOL HasStarted					()													= 0;
	/* �鿴ͨ�������ǰ״̬ */
	virtual EnServiceState	GetState		()													= 0;
	/* ��ȡ���һ��ʧ�ܲ����Ĵ������ */
	virtual EnSocketError	GetLastError	()													= 0;
	/* ��ȡ���һ��ʧ�ܲ����Ĵ������� */
	virtual LPCTSTR			GetLastErrorDesc()													= 0;
	/* ��ȡ�������������� ID */
	virtual CONNID			GetConnectionID	()													= 0;
	/* ��ȡ Client Socket �ĵ�ַ��Ϣ */
	virtual BOOL		GetLocalAddress	(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)	= 0;
	/* ��ȡ������δ�������ݵĳ��� */
	virtual BOOL GetPendingDataLength	(int& iPending)											= 0;

	/* �����ڴ�黺��ش�С��ͨ������Ϊ -> PUSH ģ�ͣ�5 - 10��PULL ģ�ͣ�10 - 20 �� */
	virtual void SetFreeBufferPoolSize		(DWORD dwFreeBufferPoolSize)						= 0;
	/* �����ڴ�黺��ػ��շ�ֵ��ͨ������Ϊ�ڴ�黺��ش�С�� 3 ���� */
	virtual void SetFreeBufferPoolHold		(DWORD dwFreeBufferPoolHold)						= 0;

	/* ��ȡ�ڴ�黺��ش�С */
	virtual DWORD GetFreeBufferPoolSize		()													= 0;
	/* ��ȡ�ڴ�黺��ػ��շ�ֵ */
	virtual DWORD GetFreeBufferPoolHold		()													= 0;

public:
	virtual ~IClient() {}
};

/************************************************************************
���ƣ�TCP ͨ�ſͻ�������ӿ�
���������� TCP ͨ�ſͻ�����������в������������Է��ʷ���
************************************************************************/
class ITcpClient : public IClient
{
public:

	/***********************************************************************/
	/***************************** ����������� *****************************/

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
	virtual BOOL SendSmallFile	(LPCTSTR lpszFileName, const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr)	= 0;

public:

	/***********************************************************************/
	/***************************** ���Է��ʷ��� *****************************/
	
	/* ����ͨ�����ݻ�������С������ƽ��ͨ�����ݰ���С�������ã�ͨ������Ϊ��(N * 1024) - sizeof(TBufferObj)�� */
	virtual void SetSocketBufferSize	(DWORD dwSocketBufferSize)	= 0;
	/* ����������������������룬0 �򲻷�����������Ĭ�ϣ�30 * 1000�� */
	virtual void SetKeepAliveTime		(DWORD dwKeepAliveTime)		= 0;
	/* �����쳣��������������룬0 ����������������Ĭ�ϣ�10 * 1000������������ɴ� [Ĭ�ϣ�WinXP 5 ��, Win7 10 ��] ��ⲻ������ȷ�ϰ�����Ϊ�Ѷ��ߣ� */
	virtual void SetKeepAliveInterval	(DWORD dwKeepAliveInterval)	= 0;

	/* ��ȡͨ�����ݻ�������С */
	virtual DWORD GetSocketBufferSize	()	= 0;
	/* ��ȡ������������� */
	virtual DWORD GetKeepAliveTime		()	= 0;
	/* ��ȡ�쳣��������� */
	virtual DWORD GetKeepAliveInterval	()	= 0;
};

/************************************************************************
���ƣ�UDP ͨ�ſͻ�������ӿ�
���������� UDP ͨ�ſͻ�����������в������������Է��ʷ���
************************************************************************/
class IUdpClient : public IClient
{
public:

	/***********************************************************************/
	/***************************** ����������� *****************************/

public:

	/***********************************************************************/
	/***************************** ���Է��ʷ��� *****************************/

	/* �������ݱ�����󳤶ȣ������ھ����������²����� 1472 �ֽڣ��ڹ����������²����� 548 �ֽڣ� */
	virtual void SetMaxDatagramSize	(DWORD dwMaxDatagramSize)	= 0;
	/* ��ȡ���ݱ�����󳤶� */
	virtual DWORD GetMaxDatagramSize()							= 0;

	/* ���ü������Դ�����0 �򲻷��ͼ�������������������Դ�������Ϊ�Ѷ��ߣ� */
	virtual void SetDetectAttempts	(DWORD dwDetectAttempts)	= 0;
	/* ���ü������ͼ�����룬0 �����ͼ����� */
	virtual void SetDetectInterval	(DWORD dwDetectInterval)	= 0;
	/* ��ȡ���������� */
	virtual DWORD GetDetectAttempts	()							= 0;
	/* ��ȡ��������� */
	virtual DWORD GetDetectInterval	()							= 0;
};

/************************************************************************
���ƣ�UDP ��������ӿ�
���������� UDP �������鲥��㲥����������в������������Է��ʷ���
************************************************************************/
class IUdpCast : public IClient
{
public:

	/***********************************************************************/
	/***************************** ����������� *****************************/

public:

	/***********************************************************************/
	/***************************** ���Է��ʷ��� *****************************/

	/* ���ð󶨵�ַ */
	virtual void SetBindAdddress	(LPCTSTR pszBindAddress)		= 0;
	/* ��ȡ�󶨵�ַ */
	virtual LPCTSTR GetBindAdddress	()								= 0;

	/* �������ݱ�����󳤶ȣ������ھ����������²����� 1472 �ֽڣ��ڹ����������²����� 548 �ֽڣ� */
	virtual void SetMaxDatagramSize	(DWORD dwMaxDatagramSize)		= 0;
	/* ��ȡ���ݱ�����󳤶� */
	virtual DWORD GetMaxDatagramSize()								= 0;

	/* �����Ƿ����õ�ַ���û��ƣ�Ĭ�ϣ������ã� */
	virtual void SetReuseAddress	(BOOL bReuseAddress)			= 0;
	/* ����Ƿ����õ�ַ���û��� */
	virtual BOOL IsReuseAddress		()								= 0;

	/* ���ô���ģʽ���鲥��㲥�� */
	virtual void SetCastMode		(EnCastMode enCastMode)			= 0;
	/* ��ȡ����ģʽ */
	virtual EnCastMode GetCastMode	()								= 0;

	/* �����鲥���ĵ� TTL��0 - 255�� */
	virtual void SetMultiCastTtl	(int iMCTtl)					= 0;
	/* ��ȡ�鲥���ĵ� TTL */
	virtual int GetMultiCastTtl		()								= 0;

	/* �����Ƿ������鲥��·��TRUE or FALSE�� */
	virtual void SetMultiCastLoop	(BOOL bMCLoop)					= 0;
	/* ����Ƿ������鲥��· */
	virtual BOOL IsMultiCastLoop	()								= 0;

	/* ��ȡ��ǰ���ݱ���Զ�̵�ַ��Ϣ��ͨ���� OnReceive �¼��е��ã� */
	virtual BOOL GetRemoteAddress	(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)	= 0;
};

/************************************************************************
���ƣ�Server/Agent PULL ģ������ӿ�
���������� Server/Agent ����� PULL ģ����������в�������
************************************************************************/
class IPullSocket
{
public:

	/*
	* ���ƣ�ץȡ����
	* �������û�ͨ���÷����� Socket �����ץȡ����
	*		
	* ������		dwConnID	-- ���� ID
	*			pData		-- ץȡ������
	*			iLength		-- ץȡ���ݳ���
	* ����ֵ��	EnFetchResult
	*/
	virtual EnFetchResult Fetch	(CONNID dwConnID, BYTE* pData, int iLength)	= 0;

	/*
	* ���ƣ���̽���ݣ������Ƴ����������ݣ�
	* �������û�ͨ���÷����� Socket ����п�̽����
	*		
	* ������		dwConnID	-- ���� ID
	*			pData		-- ��̽������
	*			iLength		-- ��̽���ݳ���
	* ����ֵ��	EnFetchResult
	*/
	virtual EnFetchResult Peek	(CONNID dwConnID, BYTE* pData, int iLength)	= 0;

public:
	virtual ~IPullSocket() {}
};

/************************************************************************
���ƣ�Client PULL ģ������ӿ�
���������� Client ����� PULL ģ����������в�������
************************************************************************/
class IPullClient
{
public:

	/*
	* ���ƣ�ץȡ����
	* �������û�ͨ���÷����� Socket �����ץȡ����
	*		
	* ������		pData		-- ץȡ������
	*			iLength		-- ץȡ���ݳ���
	* ����ֵ��	EnFetchResult
	*/
	virtual EnFetchResult Fetch	(BYTE* pData, int iLength)	= 0;

	/*
	* ���ƣ���̽���ݣ������Ƴ����������ݣ�
	* �������û�ͨ���÷����� Socket ����п�̽����
	*		
	* ������		pData		-- ��̽������
	*			iLength		-- ��̽���ݳ���
	* ����ֵ��	EnFetchResult
	*/
	virtual EnFetchResult Peek	(BYTE* pData, int iLength)	= 0;

public:
	virtual ~IPullClient() {}
};

/************************************************************************
���ƣ�TCP Server PULL ģ������ӿ�
�������̳��� ITcpServer �� IPullSocket
************************************************************************/
class ITcpPullServer : public IPullSocket, public ITcpServer
{
public:
	/* IServer* ת��Ϊ IPullSocket* */
	inline static IPullSocket* ToPull(IServer* pServer)
	{
		return (IPullSocket*)((char*)pServer - sizeof(IPullSocket));
	}

	/* IPullSocket* ת��Ϊ ITcpServer* */
	inline static ITcpServer* ToServer(IPullSocket* pPullSocket)
	{
		return (ITcpServer*)((char*)pPullSocket + sizeof(IPullSocket));
	}
};

/************************************************************************
���ƣ�TCP Agent PULL ģ������ӿ�
�������̳��� ITcpAgent �� IPullSocket
************************************************************************/
class ITcpPullAgent : public IPullSocket, public ITcpAgent
{
public:
	/* IAgent* ת��Ϊ IPullSocket* */
	inline static IPullSocket* ToPull(IAgent* pAgent)
	{
		return (IPullSocket*)((char*)pAgent - sizeof(IPullSocket));
	}

	/* IPullSocket* ת��Ϊ ITcpAgent* */
	inline static ITcpAgent* ToAgent(IPullSocket* pPullSocket)
	{
		return (ITcpAgent*)((char*)pPullSocket + sizeof(IPullSocket));
	}
};

/************************************************************************
���ƣ�TCP Client PULL ģ������ӿ�
�������̳��� ITcpClient �� IPullClient
************************************************************************/
class ITcpPullClient : public IPullClient, public ITcpClient
{
public:
	/* IClient* ת��Ϊ IPullClient* */
	inline static IPullClient* ToPull(IClient* pClient)
	{
		return (IPullClient*)((char*)pClient - sizeof(IPullClient));
	}

	/* IPullClient* ת��Ϊ ITcpClient* */
	inline static ITcpClient* ToClient(IPullClient* pPullClient)
	{
		return (ITcpClient*)((char*)pPullClient + sizeof(IPullClient));
	}
};

/************************************************************************
���ƣ�Server/Agent PACK ģ������ӿ�
���������� Server/Agent ����� PACK ģ����������в�������
************************************************************************/
class IPackSocket
{
public:

	/***********************************************************************/
	/***************************** ���Է��ʷ��� *****************************/

	/* �������ݰ���󳤶ȣ���Ч���ݰ���󳤶Ȳ��ܳ��� 524287/0x7FFFF �ֽڣ�Ĭ�ϣ�262144/0x40000�� */
	virtual void SetMaxPackSize		(DWORD dwMaxPackSize)			= 0;
	/* ���ð�ͷ��ʶ����Ч��ͷ��ʶȡֵ��Χ 0 ~ 8191/0x1FFF������ͷ��ʶΪ 0 ʱ��У���ͷ��Ĭ�ϣ�0�� */
	virtual void SetPackHeaderFlag	(USHORT usPackHeaderFlag)		= 0;

	/* ��ȡ���ݰ���󳤶� */
	virtual DWORD GetMaxPackSize	()								= 0;
	/* ��ȡ��ͷ��ʶ */
	virtual USHORT GetPackHeaderFlag()								= 0;

public:
	virtual ~IPackSocket() {}
};

/************************************************************************
���ƣ�Client PACK ģ������ӿ�
���������� Client ����� PACK ģ����������в�������
************************************************************************/
class IPackClient
{
public:

	/***********************************************************************/
	/***************************** ���Է��ʷ��� *****************************/

	/* �������ݰ���󳤶ȣ���Ч���ݰ���󳤶Ȳ��ܳ��� 524287/0x7FFFF �ֽڣ�Ĭ�ϣ�262144/0x40000�� */
	virtual void SetMaxPackSize		(DWORD dwMaxPackSize)			= 0;
	/* ���ð�ͷ��ʶ����Ч��ͷ��ʶȡֵ��Χ 0 ~ 8191/0x1FFF������ͷ��ʶΪ 0 ʱ��У���ͷ��Ĭ�ϣ�0�� */
	virtual void SetPackHeaderFlag	(USHORT usPackHeaderFlag)		= 0;

	/* ��ȡ���ݰ���󳤶� */
	virtual DWORD GetMaxPackSize	()								= 0;
	/* ��ȡ��ͷ��ʶ */
	virtual USHORT GetPackHeaderFlag()								= 0;

public:
	virtual ~IPackClient() {}
};

/************************************************************************
���ƣ�TCP Server PACK ģ������ӿ�
�������̳��� ITcpServer �� IPackSocket
************************************************************************/
class ITcpPackServer : public IPackSocket, public ITcpServer
{
public:
	/* IServer* ת��Ϊ IPackSocket* */
	inline static IPackSocket* ToPack(IServer* pServer)
	{
		return (IPackSocket*)((char*)pServer - sizeof(IPackSocket));
	}

	/* IPackSocket* ת��Ϊ ITcpServer* */
	inline static ITcpServer* ToServer(IPackSocket* pPackSocket)
	{
		return (ITcpServer*)((char*)pPackSocket + sizeof(IPackSocket));
	}
};

/************************************************************************
���ƣ�TCP Agent PACK ģ������ӿ�
�������̳��� ITcpAgent �� IPackSocket
************************************************************************/
class ITcpPackAgent : public IPackSocket, public ITcpAgent
{
public:
	/* IAgent* ת��Ϊ IPackSocket* */
	inline static IPackSocket* ToPack(IAgent* pAgent)
	{
		return (IPackSocket*)((char*)pAgent - sizeof(IPackSocket));
	}

	/* IPackSocket* ת��Ϊ ITcpAgent* */
	inline static ITcpAgent* ToAgent(IPackSocket* pPackSocket)
	{
		return (ITcpAgent*)((char*)pPackSocket + sizeof(IPackSocket));
	}
};

/************************************************************************
���ƣ�TCP Client PACK ģ������ӿ�
�������̳��� ITcpClient �� IPackClient
************************************************************************/
class ITcpPackClient : public IPackClient, public ITcpClient
{
public:
	/* IClient* ת��Ϊ IPackClient* */
	inline static IPackClient* ToPack(IClient* pClient)
	{
		return (IPackClient*)((char*)pClient - sizeof(IPackClient));
	}

	/* IPackClient* ת��Ϊ ITcpClient* */
	inline static ITcpClient* ToClient(IPackClient* pPackClient)
	{
		return (ITcpClient*)((char*)pPackClient + sizeof(IPackClient));
	}
};
