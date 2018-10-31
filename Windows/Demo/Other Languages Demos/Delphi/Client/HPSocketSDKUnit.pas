{:
����: ����~��*_^��~
ʱ��:   2017-11-20
Email:  ztcjn@qq.com
ע��:   ����в�������ĵط����뼰ʱ�������ң�лл��
���ܣ�Hp-Socket delphi��Ԫ������Ҫ�����иĳ��ࡣ
}
unit HPSocketSDKUnit;

interface

uses
  Windows, HPTypeDef, SysUtils,DateUtils,StrUtils;

const
  HPSocketDLL = 'HPSocket4C.dll';

type
{$Z4}
  SOCKET = Pointer;

  WSABUF = Record
    len: LongInt; { the length of the buffer }
    buf: PChar; { the pointer to the buffer }
  end { WSABUF };

  PWSABUF = ^WSABUF;
  LPWSABUF = PWSABUF;

  WSABUFArray = array of WSABUF;

  PInteger = ^Integer;
  PUShort = ^Word;

  { /************************************************************************
    ���ƣ����� Socket ����ָ�����ͱ���
    �������� Socket ����ָ�붨��Ϊ��ֱ�۵ı���
    ************************************************************************/ }
  HP_Object = longint;

  HP_Server = HP_Object;
  HP_Agent = HP_Object;
  HP_Client = HP_Object;
  HP_TcpServer = HP_Object;
  HP_TcpAgent = HP_Object;
  HP_TcpClient = HP_Object;
  HP_PullSocket = HP_Object;
  HP_PullClient = HP_Object;
  HP_TcpPullServer = HP_Object;
  HP_TcpPullAgent = HP_Object;
  HP_TcpPullClient = HP_Object;
  HP_PackSocket = HP_Object;
  HP_PackClient = HP_Object;
  HP_TcpPackServer = HP_Object;
  HP_TcpPackAgent = HP_Object;
  HP_TcpPackClient = HP_Object;
  HP_UdpServer = HP_Object;
  HP_UdpClient = HP_Object;
  HP_UdpCast = HP_Object;

  HP_Listener = HP_Object;
  HP_ServerListener = HP_Object;
  HP_AgentListener = HP_Object;
  HP_ClientListener = HP_Object;
  HP_TcpServerListener = HP_Object;
  HP_TcpAgentListener = HP_Object;
  HP_TcpClientListener = HP_Object;
  HP_PullSocketListener = HP_Object;
  HP_PullClientListener = HP_Object;
  HP_TcpPullServerListener = HP_Object;
  HP_TcpPullAgentListener = HP_Object;
  HP_TcpPullClientListener = HP_Object;
  HP_PackSocketListener = HP_Object;
  HP_PackClientListener = HP_Object;
  HP_TcpPackServerListener = HP_Object;
  HP_TcpPackAgentListener = HP_Object;
  HP_TcpPackClientListener = HP_Object;
  HP_UdpServerListener = HP_Object;
  HP_UdpClientListener = HP_Object;
  HP_UdpCastListener = HP_Object;

  HP_Http = HP_Object;
  HP_HttpServer = HP_Object;
  HP_HttpAgent = HP_Object;
  HP_HttpClient = HP_Object;
  HP_HttpSyncClient = HP_Object;

  HP_HttpServerListener = HP_Object;
  HP_HttpAgentListener = HP_Object;
  HP_HttpClientListener = HP_Object;

//*****************************************************************************************************************************************************/
//****************************************************************** TCP/UDP Exports ******************************************************************/
//*****************************************************************************************************************************************************/

//****************************************************/
//***************** TCP/UDP �ص����� ******************/

//* Server �ص����� */
HP_FN_Server_OnPrepareListen = function(pSender: HP_Server; soListen: Pointer):En_HP_HandleResult; stdcall;
// ���Ϊ TCP ���ӣ�pClientΪ SOCKET ��������Ϊ UDP ���ӣ�pClientΪ SOCKADDR_IN ָ�룻
HP_FN_Server_OnAccept = function(pSender: HP_Server; dwConnID: HP_CONNID; pClient: Pointer):En_HP_HandleResult; stdcall;
HP_FN_Server_OnHandShake = function(pSender: HP_Server; dwConnID: HP_CONNID):En_HP_HandleResult; stdcall;
HP_FN_Server_OnSend = function(pSender: HP_Server; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Server_OnReceive = function(pSender: HP_Server; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Server_OnPullReceive = function(pSender: HP_Server; dwConnID: HP_CONNID; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Server_OnClose = function(pSender: HP_Server; dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer):En_HP_HandleResult; stdcall;
HP_FN_Server_OnShutdown = function(pSender: HP_Server):En_HP_HandleResult; stdcall;

//* Agent �ص����� */
HP_FN_Agent_OnPrepareConnect = function(pSender: HP_Agent; dwConnID: HP_CONNID; socket: Pointer):En_HP_HandleResult; stdcall;
HP_FN_Agent_OnConnect = function(pSender: HP_Agent; dwConnID: HP_CONNID):En_HP_HandleResult; stdcall;
HP_FN_Agent_OnHandShake = function(pSender: HP_Agent; dwConnID: HP_CONNID):En_HP_HandleResult; stdcall;
HP_FN_Agent_OnSend = function(pSender: HP_Agent; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Agent_OnReceive = function(pSender: HP_Agent; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Agent_OnPullReceive = function(pSender: HP_Agent; dwConnID: HP_CONNID; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Agent_OnClose = function(pSender: HP_Agent; dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer):En_HP_HandleResult; stdcall;
HP_FN_Agent_OnShutdown = function(pSender: HP_Agent):En_HP_HandleResult; stdcall;

//* Client �ص����� */
HP_FN_Client_OnPrepareConnect = function(pSender: HP_Client; dwConnID: HP_CONNID; socket: Pointer):En_HP_HandleResult; stdcall;
HP_FN_Client_OnConnect = function(pSender: HP_Client; dwConnID: HP_CONNID):En_HP_HandleResult; stdcall;
HP_FN_Client_OnHandShake = function(pSender: HP_Client; dwConnID: HP_CONNID):En_HP_HandleResult; stdcall;
HP_FN_Client_OnSend = function(pSender: HP_Client; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Client_OnReceive = function(pSender: HP_Client; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Client_OnPullReceive = function(pSender: HP_Client; dwConnID: HP_CONNID; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Client_OnClose = function(pSender: HP_Client; dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer):En_HP_HandleResult; stdcall;

//* HTTP �ص����� */
HP_FN_Http_OnMessageBegin = function(pSender: HP_Http; dwConnID: HP_CONNID):En_HP_HttpParseResult; stdcall;
HP_FN_Http_OnRequestLine = function(pSender: HP_Http; dwConnID: HP_CONNID; lpszMethod: PChar; lpszUrl: PChar):En_HP_HttpParseResult; stdcall;
HP_FN_Http_OnStatusLine = function(pSender: HP_Http; dwConnID: HP_CONNID; usStatusCode: Word; lpszDesc: PChar):En_HP_HttpParseResult; stdcall;
HP_FN_Http_OnHeader = function(pSender: HP_Http; dwConnID: HP_CONNID; lpszName: PChar; lpszValue: PChar):En_HP_HttpParseResult; stdcall;
HP_FN_Http_OnHeadersComplete = function(pSender: HP_Http; dwConnID: HP_CONNID):En_HP_HttpParseResult; stdcall;
HP_FN_Http_OnBody = function(pSender: HP_Http; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer):En_HP_HttpParseResult; stdcall;
HP_FN_Http_OnChunkHeader = function(pSender: HP_Http; dwConnID: HP_CONNID; iLength: Integer):En_HP_HttpParseResult; stdcall;
HP_FN_Http_OnChunkComplete = function(pSender: HP_Http; dwConnID: HP_CONNID):En_HP_HttpParseResult; stdcall;
HP_FN_Http_OnMessageComplete = function(pSender: HP_Http; dwConnID: HP_CONNID):En_HP_HttpParseResult; stdcall;
HP_FN_Http_OnUpgrade = function(pSender: HP_Http; dwConnID: HP_CONNID; enUpgradeType: En_HP_HttpUpgradeType):En_HP_HttpParseResult; stdcall;
HP_FN_Http_OnParseError = function(pSender: HP_Http; dwConnID: HP_CONNID; iErrorCode: Integer; lpszErrorDesc: PChar):En_HP_HttpParseResult; stdcall;

HP_FN_Http_OnWSMessageHeader = function(pSender: HP_Http; dwConnID: HP_CONNID; bFinal: BOOL; iReserved: PChar; iOperationCode: PChar; const lpszMask: PChar; ullBodyLen: LongWord):En_HP_HandleResult; stdcall;
HP_FN_Http_OnWSMessageBody = function(pSender: HP_Http; dwConnID: HP_CONNID; const pData: PChar; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Http_OnWSMessageComplete = function(pSender: HP_Http; dwConnID: HP_CONNID):En_HP_HandleResult; stdcall;

//****************************************************/
//*************** TCP/UDP ���󴴽����� ****************/

// ���� HP_TcpServer ����
function Create_HP_TcpServer(pListener: HP_TcpServerListener):HP_TcpServer; stdcall; external HPSocketDLL;
// ���� HP_TcpAgent ����
function Create_HP_TcpAgent(pListener: HP_TcpAgentListener):HP_TcpAgent; stdcall; external HPSocketDLL;
// ���� HP_TcpClient ����
function Create_HP_TcpClient(pListener: HP_TcpClientListener):HP_TcpClient; stdcall; external HPSocketDLL;
// ���� HP_TcpPullServer ����
function Create_HP_TcpPullServer(pListener: HP_TcpPullServerListener):HP_TcpPullServer; stdcall; external HPSocketDLL;
// ���� HP_TcpPullAgent ����
function Create_HP_TcpPullAgent(pListener: HP_TcpPullAgentListener):HP_TcpPullAgent; stdcall; external HPSocketDLL;
// ���� HP_TcpPullClient ����
function Create_HP_TcpPullClient(pListener: HP_TcpPullClientListener):HP_TcpPullClient; stdcall; external HPSocketDLL;
// ���� HP_TcpPackServer ����
function Create_HP_TcpPackServer(pListener: HP_TcpServerListener):HP_TcpPackServer; stdcall; external HPSocketDLL;
// ���� HP_TcpPackAgent ����
function Create_HP_TcpPackAgent(pListener: HP_TcpAgentListener):HP_TcpPackAgent; stdcall; external HPSocketDLL;
// ���� HP_TcpPackClient ����
function Create_HP_TcpPackClient(pListener: HP_TcpClientListener):HP_TcpPackClient; stdcall; external HPSocketDLL;
// ���� HP_UdpServer ����
function Create_HP_UdpServer(pListener: HP_UdpServerListener):HP_UdpServer; stdcall; external HPSocketDLL;
// ���� HP_UdpClient ����
function Create_HP_UdpClient(pListener: HP_UdpClientListener):HP_UdpClient; stdcall; external HPSocketDLL;
// ���� HP_UdpCast ����
function Create_HP_UdpCast(pListener: HP_UdpCastListener):HP_UdpCast; stdcall; external HPSocketDLL;

// ���� HP_TcpServer ����
procedure Destroy_HP_TcpServer(pServer: HP_TcpServer); stdcall; external HPSocketDLL;
// ���� HP_TcpAgent ����
procedure Destroy_HP_TcpAgent(pAgent: HP_TcpAgent); stdcall; external HPSocketDLL;
// ���� HP_TcpClient ����
procedure Destroy_HP_TcpClient(pClient: HP_TcpClient); stdcall; external HPSocketDLL;
// ���� HP_TcpPullServer ����
procedure Destroy_HP_TcpPullServer(pServer: HP_TcpPullServer); stdcall; external HPSocketDLL;
// ���� HP_TcpPullAgent ����
procedure Destroy_HP_TcpPullAgent(pAgent: HP_TcpPullAgent); stdcall; external HPSocketDLL;
// ���� HP_TcpPullClient ����
procedure Destroy_HP_TcpPullClient(pClient: HP_TcpPullClient); stdcall; external HPSocketDLL;
// ���� HP_TcpPackServer ����
procedure Destroy_HP_TcpPackServer(pServer: HP_TcpPackServer); stdcall; external HPSocketDLL;
// ���� HP_TcpPackAgent ����
procedure Destroy_HP_TcpPackAgent(pAgent: HP_TcpPackAgent); stdcall; external HPSocketDLL;
// ���� HP_TcpPackClient ����
procedure Destroy_HP_TcpPackClient(pClient: HP_TcpPackClient); stdcall; external HPSocketDLL;
// ���� HP_UdpServer ����
procedure Destroy_HP_UdpServer(pServer: HP_UdpServer); stdcall; external HPSocketDLL;
// ���� HP_UdpClient ����
procedure Destroy_HP_UdpClient(pClient: HP_UdpClient); stdcall; external HPSocketDLL;
// ���� HP_UdpCast ����
procedure Destroy_HP_UdpCast(pCast: HP_UdpCast); stdcall; external HPSocketDLL;

// ���� HP_TcpServerListener ����
function Create_HP_TcpServerListener():HP_TcpServerListener; stdcall; external HPSocketDLL;
// ���� HP_TcpAgentListener ����
function Create_HP_TcpAgentListener():HP_TcpAgentListener; stdcall; external HPSocketDLL;
// ���� HP_TcpClientListener ����
function Create_HP_TcpClientListener():HP_TcpClientListener; stdcall; external HPSocketDLL;
// ���� HP_TcpPullServerListener ����
function Create_HP_TcpPullServerListener():HP_TcpPullServerListener; stdcall; external HPSocketDLL;
// ���� HP_TcpPullAgentListener ����
function Create_HP_TcpPullAgentListener():HP_TcpPullAgentListener; stdcall; external HPSocketDLL;
// ���� HP_TcpPullClientListener ����
function Create_HP_TcpPullClientListener():HP_TcpPullClientListener; stdcall; external HPSocketDLL;
// ���� HP_TcpPackServerListener ����
function Create_HP_TcpPackServerListener():HP_TcpPackServerListener; stdcall; external HPSocketDLL;
// ���� HP_TcpPackAgentListener ����
function Create_HP_TcpPackAgentListener():HP_TcpPackAgentListener; stdcall; external HPSocketDLL;
// ���� HP_TcpPackClientListener ����
function Create_HP_TcpPackClientListener():HP_TcpPackClientListener; stdcall; external HPSocketDLL;
// ���� HP_UdpServerListener ����
function Create_HP_UdpServerListener():HP_UdpServerListener; stdcall; external HPSocketDLL;
// ���� HP_UdpClientListener ����
function Create_HP_UdpClientListener():HP_UdpClientListener; stdcall; external HPSocketDLL;
// ���� HP_UdpCastListener ����
function Create_HP_UdpCastListener():HP_UdpCastListener; stdcall; external HPSocketDLL;

// ���� HP_TcpServerListener ����
procedure Destroy_HP_TcpServerListener(pListener: HP_TcpServerListener); stdcall; external HPSocketDLL;
// ���� HP_TcpAgentListener ����
procedure Destroy_HP_TcpAgentListener(pListener: HP_TcpAgentListener); stdcall; external HPSocketDLL;
// ���� HP_TcpClientListener ����
procedure Destroy_HP_TcpClientListener(pListener: HP_TcpClientListener); stdcall; external HPSocketDLL;
// ���� HP_TcpPullServerListener ����
procedure Destroy_HP_TcpPullServerListener(pListener: HP_TcpPullServerListener); stdcall; external HPSocketDLL;
// ���� HP_TcpPullAgentListener ����
procedure Destroy_HP_TcpPullAgentListener(pListener: HP_TcpPullAgentListener); stdcall; external HPSocketDLL;
// ���� HP_TcpPullClientListener ����
procedure Destroy_HP_TcpPullClientListener(pListener: HP_TcpPullClientListener); stdcall; external HPSocketDLL;
// ���� HP_TcpPackServerListener ����
procedure Destroy_HP_TcpPackServerListener(pListener: HP_TcpPackServerListener); stdcall; external HPSocketDLL;
// ���� HP_TcpPackAgentListener ����
procedure Destroy_HP_TcpPackAgentListener(pListener: HP_TcpPackAgentListener); stdcall; external HPSocketDLL;
// ���� HP_TcpPackClientListener ����
procedure Destroy_HP_TcpPackClientListener(pListener: HP_TcpPackClientListener); stdcall; external HPSocketDLL;
// ���� HP_UdpServerListener ����
procedure Destroy_HP_UdpServerListener(pListener: HP_UdpServerListener); stdcall; external HPSocketDLL;
// ���� HP_UdpClientListener ����
procedure Destroy_HP_UdpClientListener(pListener: HP_UdpClientListener); stdcall; external HPSocketDLL;
// ���� HP_UdpCastListener ����
procedure Destroy_HP_UdpCastListener(pListener: HP_UdpCastListener); stdcall; external HPSocketDLL;

//**********************************************************************************/
//***************************** Server �ص��������÷��� *****************************/

procedure HP_Set_FN_Server_OnPrepareListen(pListener	: HP_ServerListener; fn: HP_FN_Server_OnPrepareListen); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnAccept(pListener			: HP_ServerListener; fn: HP_FN_Server_OnAccept); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnHandShake(pListener		: HP_ServerListener; fn: HP_FN_Server_OnHandShake); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnSend(pListener				: HP_ServerListener; fn: HP_FN_Server_OnSend); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnReceive(pListener			: HP_ServerListener; fn: HP_FN_Server_OnReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnPullReceive(pListener		: HP_ServerListener; fn: HP_FN_Server_OnPullReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnClose(pListener			: HP_ServerListener; fn: HP_FN_Server_OnClose); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnShutdown(pListener			: HP_ServerListener; fn: HP_FN_Server_OnShutdown); stdcall; external HPSocketDLL;

//**********************************************************************************/
//****************************** Agent �ص��������÷��� *****************************/

procedure HP_Set_FN_Agent_OnPrepareConnect(pListener		: HP_AgentListener; fn: HP_FN_Agent_OnPrepareConnect); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnConnect(pListener			: HP_AgentListener; fn: HP_FN_Agent_OnConnect); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnHandShake(pListener			: HP_AgentListener; fn: HP_FN_Agent_OnHandShake); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnSend(pListener				: HP_AgentListener; fn: HP_FN_Agent_OnSend); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnReceive(pListener			: HP_AgentListener; fn: HP_FN_Agent_OnReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnPullReceive(pListener		: HP_AgentListener; fn: HP_FN_Agent_OnPullReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnClose(pListener				: HP_AgentListener; fn: HP_FN_Agent_OnClose); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnShutdown(pListener			: HP_AgentListener; fn: HP_FN_Agent_OnShutdown); stdcall; external HPSocketDLL;

//**********************************************************************************/
//***************************** Client �ص��������÷��� *****************************/

procedure HP_Set_FN_Client_OnPrepareConnect(pListener	: HP_ClientListener; fn: HP_FN_Client_OnPrepareConnect); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnConnect(pListener			: HP_ClientListener; fn: HP_FN_Client_OnConnect); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnHandShake(pListener		: HP_ClientListener; fn: HP_FN_Client_OnHandShake); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnSend(pListener				: HP_ClientListener; fn: HP_FN_Client_OnSend); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnReceive(pListener			: HP_ClientListener; fn: HP_FN_Client_OnReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnPullReceive(pListener		: HP_ClientListener; fn: HP_FN_Client_OnPullReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnClose(pListener			: HP_ClientListener; fn: HP_FN_Client_OnClose); stdcall; external HPSocketDLL;

//**************************************************************************/
//***************************** Server �������� *****************************/

{//*
* ���ƣ�����ͨ�����
* ���������������ͨ�������������ɺ�ɿ�ʼ���տͻ������Ӳ��շ�����
*		
* ������		lpszBindAddress	-- ������ַ
*			usPort			-- �����˿�
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� HP_Server_GetLastError() ��ȡ�������
*/}
function HP_Server_Start(pServer: HP_Server; lpszBindAddress: PChar; usPort: Word):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ��ر�ͨ�����
* �������رշ����ͨ��������ر���ɺ�Ͽ����пͻ������Ӳ��ͷ�������Դ
*		
* ������	
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� HP_Server_GetLastError() ��ȡ�������
*/}
function HP_Server_Stop(pServer: HP_Server):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ���������
* ��������ָ�����ӷ�������
*		
* ������		dwConnID	-- ���� ID
*			pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡ Windows �������
*/}
function HP_Server_Send(pServer: HP_Server; dwConnID: HP_CONNID; const pBuffer: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ���������
* ��������ָ�����ӷ�������
*		
* ������		dwConnID	-- ���� ID
*			pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
*			iOffset		-- ���ͻ�����ָ��ƫ����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡ Windows �������
*/}
function HP_Server_SendPart(pServer: HP_Server; dwConnID: HP_CONNID; const pBuffer: Pointer; iLength: Integer; iOffset: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ����Ͷ�������
* ��������ָ�����ӷ��Ͷ�������
*		TCP - ˳�����������ݰ� 
*		UDP - ���������ݰ���ϳ�һ�����ݰ����ͣ����ݰ����ܳ��Ȳ��ܴ������õ� UDP ����󳤶ȣ� 
*		
* ������		dwConnID	-- ���� ID
*			pBuffers	-- ���ͻ���������
*			iCount		-- ���ͻ�������Ŀ
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡ Windows �������
*/}
function HP_Server_SendPackets(pServer: HP_Server; dwConnID: HP_CONNID; const pBuffers: WSABUFArray; iCount: Integer):BOOL; stdcall; external HPSocketDLL;

{/*
* ���ƣ���ͣ/�ָ�����
* ��������ͣ/�ָ�ĳ�����ӵ����ݽ��չ���
*
* ������		dwConnID	-- ���� ID
*			bPause		-- TRUE - ��ͣ, FALSE - �ָ�
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/}
function HP_Server_PauseReceive(pServer: HP_Server; dwConnID: HP_CONNID; bPause: BOOL):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ��Ͽ�����
* �������Ͽ���ĳ���ͻ��˵�����
*		
* ������		dwConnID	-- ���� ID
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/}
function HP_Server_Disconnect(pServer: HP_Server; dwConnID: HP_CONNID; bForce: BOOL):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ��Ͽ���ʱ����
* �������Ͽ�����ָ��ʱ��������
*		
* ������		dwPeriod	-- ʱ�������룩
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/}
function HP_Server_DisconnectLongConnections(pServer: HP_Server; dwPeriod: Word; bForce: BOOL):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ��Ͽ���Ĭ����
* �������Ͽ�����ָ��ʱ���ľ�Ĭ����
*		
* ������		dwPeriod	-- ʱ�������룩
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/}
function HP_Server_DisconnectSilenceConnections(pServer: HP_Server; dwPeriod: Word; bForce: BOOL):BOOL; stdcall; external HPSocketDLL;

//******************************************************************************/
//***************************** Server ���Է��ʷ��� *****************************/

{//*
* ���ƣ��������ӵĸ�������
* �������Ƿ�Ϊ���Ӱ󶨸������ݻ��߰�ʲô�������ݣ�����Ӧ�ó���ֻ�����
*		
* ������		dwConnID	-- ���� ID
*			pv			-- ����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���Ч������ ID��
*/}
function HP_Server_SetConnectionExtra(pServer: HP_Server; dwConnID: HP_CONNID; pExtra: Pointer):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ���ȡ���ӵĸ�������
* �������Ƿ�Ϊ���Ӱ󶨸������ݻ��߰�ʲô�������ݣ�����Ӧ�ó���ֻ�����
*		
* ������		dwConnID	-- ���� ID
*			ppv			-- ����ָ��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���Ч������ ID��
*/}
function HP_Server_GetConnectionExtra(pServer: HP_Server; dwConnID: HP_CONNID; ppExtra: Pointer):BOOL; stdcall; external HPSocketDLL;

//* ���ͨ������Ƿ������� */
function HP_Server_HasStarted(pServer: HP_Server):BOOL; stdcall; external HPSocketDLL;
//* �鿴ͨ�������ǰ״̬ */
function HP_Server_GetState(pServer: HP_Server):En_HP_ServiceState; stdcall; external HPSocketDLL;
//* ��ȡ���һ��ʧ�ܲ����Ĵ������ */
function HP_Server_GetLastError(pServer: HP_Server):En_HP_SocketError; stdcall; external HPSocketDLL;
//* ��ȡ���һ��ʧ�ܲ����Ĵ������� */
function HP_Server_GetLastErrorDesc(pServer: HP_Server):PChar; stdcall; external HPSocketDLL;
//* ��ȡ������δ�������ݵĳ��� */
function HP_Server_GetPendingDataLength(pServer: HP_Server; dwConnID: HP_CONNID; piPending: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ���ӵ����ݽ���״̬ */
function HP_Server_IsPauseReceive(pServer: HP_Server; dwConnID: HP_CONNID;var pbPaused: PBOOL):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ�ͻ��������� */
function HP_Server_GetConnectionCount(pServer: HP_Server):Word; stdcall; external HPSocketDLL;
//* ��ȡ�������ӵ� HP_CONNID */
function HP_Server_GetAllConnectionIDs(pServer: HP_Server; pIDs: HP_CONNIDArray; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��ȡĳ���ͻ�������ʱ�������룩 */
function HP_Server_GetConnectPeriod(pServer: HP_Server; dwConnID: HP_CONNID; pdwPeriod: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��ȡĳ�����Ӿ�Ĭʱ�䣨���룩 */
function HP_Server_GetSilencePeriod(pServer: HP_Server; dwConnID: HP_CONNID; pdwPeriod: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ���� Socket �ĵ�ַ��Ϣ */
function HP_Server_GetListenAddress(pServer: HP_Server; lpszAddress: PChar;var piAddressLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;
//* ��ȡĳ�����ӵı��ص�ַ��Ϣ */
function HP_Server_GetLocalAddress(pServer: HP_Server; dwConnID: HP_CONNID; lpszAddress: PChar;var piAddressLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;
//* ��ȡĳ�����ӵ�Զ�̵�ַ��Ϣ */
function HP_Server_GetRemoteAddress(pServer: HP_Server; dwConnID: HP_CONNID; lpszAddress: Pchar;var piAddressLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;

//* �������ݷ��Ͳ��� */
procedure HP_Server_SetSendPolicy(pServer: HP_Server; enSendPolicy: En_HP_SendPolicy); stdcall; external HPSocketDLL;
//* ���� Socket �����������ʱ�䣨���룬�������ڼ�� Socket ��������ܱ���ȡʹ�ã� */
procedure HP_Server_SetFreeSocketObjLockTime(pServer: HP_Server; dwFreeSocketObjLockTime: Word); stdcall; external HPSocketDLL;
//* ���� Socket ����ش�С��ͨ������Ϊƽ���������������� 1/3 - 1/2�� */
procedure HP_Server_SetFreeSocketObjPool(pServer: HP_Server; dwFreeSocketObjPool: Word); stdcall; external HPSocketDLL;
//* �����ڴ�黺��ش�С��ͨ������Ϊ Socket ����ش�С�� 2 - 3 ���� */
procedure HP_Server_SetFreeBufferObjPool(pServer: HP_Server; dwFreeBufferObjPool: Word); stdcall; external HPSocketDLL;
//* ���� Socket ����ػ��շ�ֵ��ͨ������Ϊ Socket ����ش�С�� 3 ���� */
procedure HP_Server_SetFreeSocketObjHold(pServer: HP_Server; dwFreeSocketObjHold: Word); stdcall; external HPSocketDLL;
//* �����ڴ�黺��ػ��շ�ֵ��ͨ������Ϊ�ڴ�黺��ش�С�� 3 ���� */
procedure HP_Server_SetFreeBufferObjHold(pServer: HP_Server; dwFreeBufferObjHold: Word); stdcall; external HPSocketDLL;
//* ���������������������������ֵԤ�����ڴ棬�����Ҫ����ʵ��������ã����˹���*/
procedure HP_Server_SetMaxConnectionCount(pServer: HP_Server; dwMaxConnectionCount: Word); stdcall; external HPSocketDLL;
//* ���ù����߳�������ͨ������Ϊ 2 * CPU + 2�� */
procedure HP_Server_SetWorkerThreadCount(pServer: HP_Server; dwWorkerThreadCount: Word); stdcall; external HPSocketDLL;
//* �����Ƿ��Ǿ�Ĭʱ�䣨����Ϊ TRUE ʱ DisconnectSilenceConnections() �� GetSilencePeriod() ����Ч��Ĭ�ϣ�TRUE�� */
procedure HP_Server_SetMarkSilence(pServer: HP_Server; bMarkSilence: BOOL); stdcall; external HPSocketDLL;

//* ��ȡ���ݷ��Ͳ��� */
function HP_Server_GetSendPolicy(pServer: HP_Server):En_HP_SendPolicy; stdcall; external HPSocketDLL;
//* ��ȡ Socket �����������ʱ�� */
function HP_Server_GetFreeSocketObjLockTime(pServer: HP_Server):Word; stdcall; external HPSocketDLL;
//* ��ȡ Socket ����ش�С */
function HP_Server_GetFreeSocketObjPool(pServer: HP_Server):Word; stdcall; external HPSocketDLL;
//* ��ȡ�ڴ�黺��ش�С */
function HP_Server_GetFreeBufferObjPool(pServer: HP_Server):Word; stdcall; external HPSocketDLL;
//* ��ȡ Socket ����ػ��շ�ֵ */
function HP_Server_GetFreeSocketObjHold(pServer: HP_Server):Word; stdcall; external HPSocketDLL;
//* ��ȡ�ڴ�黺��ػ��շ�ֵ */
function HP_Server_GetFreeBufferObjHold(pServer: HP_Server):Word; stdcall; external HPSocketDLL;
//* ��ȡ��������� */
function HP_Server_GetMaxConnectionCount(pServer: HP_Server):Word; stdcall; external HPSocketDLL;
//* ��ȡ�����߳����� */
function HP_Server_GetWorkerThreadCount(pServer: HP_Server):Word; stdcall; external HPSocketDLL;
//* ����Ƿ��Ǿ�Ĭʱ�� */
function HP_Server_IsMarkSilence(pServer: HP_Server):BOOL; stdcall; external HPSocketDLL;

//**********************************************************************************/
//******************************* TCP Server �������� *******************************/

{//*
* ���ƣ�����С�ļ�
* ��������ָ�����ӷ��� 4096 KB ���µ�С�ļ�
*		
* ������		dwConnID		-- ���� ID
*			lpszFileName	-- �ļ�·��
*			pHead			-- ͷ����������
*			pTail			-- β����������
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡ Windows �������
*/}
function HP_TcpServer_SendSmallFile(pServer: HP_Server; dwConnID: HP_CONNID; lpszFileName: PChar; const pHead: LPWSABUF; const pTail: LPWSABUF):BOOL; stdcall; external HPSocketDLL;

//**********************************************************************************/
//***************************** TCP Server ���Է��ʷ��� *****************************/

//* ���ü��� Socket �ĵȺ���д�С�����ݲ������������������ã� */
procedure HP_TcpServer_SetSocketListenQueue(pServer: HP_TcpServer; dwSocketListenQueue: Word); stdcall; external HPSocketDLL;
//* ���� Accept ԤͶ�����������ݸ��ص������ã�Accept ԤͶ������Խ����֧�ֵĲ�����������Խ�ࣩ */
procedure HP_TcpServer_SetAcceptSocketCount(pServer: HP_TcpServer; dwAcceptSocketCount: Word); stdcall; external HPSocketDLL;
//* ����ͨ�����ݻ�������С������ƽ��ͨ�����ݰ���С�������ã�ͨ������Ϊ 1024 �ı����� */
procedure HP_TcpServer_SetSocketBufferSize(pServer: HP_TcpServer; dwSocketBufferSize: Word); stdcall; external HPSocketDLL;
//* ����������������������룬0 �򲻷�����������Ĭ�ϣ�30 * 1000�� */
procedure HP_TcpServer_SetKeepAliveTime(pServer: HP_TcpServer; dwKeepAliveTime: Word); stdcall; external HPSocketDLL;
//* �����쳣��������������룬0 ����������������Ĭ�ϣ�10 * 1000������������ɴ� [Ĭ�ϣ�WinXP 5 ��, Win7 10 ��] ��ⲻ������ȷ�ϰ�����Ϊ�Ѷ��ߣ� */
procedure HP_TcpServer_SetKeepAliveInterval(pServer: HP_TcpServer; dwKeepAliveInterval: Word); stdcall; external HPSocketDLL;

//* ��ȡ Accept ԤͶ������ */
function HP_TcpServer_GetAcceptSocketCount(pServer: HP_TcpServer):Word; stdcall; external HPSocketDLL;
//* ��ȡͨ�����ݻ�������С */
function HP_TcpServer_GetSocketBufferSize(pServer: HP_TcpServer):Word; stdcall; external HPSocketDLL;
//* ��ȡ���� Socket �ĵȺ���д�С */
function HP_TcpServer_GetSocketListenQueue(pServer: HP_TcpServer):Word; stdcall; external HPSocketDLL;
//* ��ȡ������������� */
function HP_TcpServer_GetKeepAliveTime(pServer: HP_TcpServer):Word; stdcall; external HPSocketDLL;
//* ��ȡ�쳣��������� */
function HP_TcpServer_GetKeepAliveInterval(pServer: HP_TcpServer):Word; stdcall; external HPSocketDLL;

//**********************************************************************************/
//***************************** UDP Server ���Է��ʷ��� *****************************/

//* �������ݱ�����󳤶ȣ������ھ����������²����� 1472 �ֽڣ��ڹ����������²����� 548 �ֽڣ� */
procedure HP_UdpServer_SetMaxDatagramSize(pServer: HP_UdpServer; dwMaxDatagramSize: Word); stdcall; external HPSocketDLL;
//* ��ȡ���ݱ�����󳤶� */
function HP_UdpServer_GetMaxDatagramSize(pServer: HP_UdpServer):Word; stdcall; external HPSocketDLL;

//* ���� Receive ԤͶ�����������ݸ��ص������ã�Receive ԤͶ������Խ���򶪰�����ԽС�� */
procedure HP_UdpServer_SetPostReceiveCount(pServer: HP_UdpServer; dwPostReceiveCount: Word); stdcall; external HPSocketDLL;
//* ��ȡ Receive ԤͶ������ */
function HP_UdpServer_GetPostReceiveCount(pServer: HP_UdpServer):Word; stdcall; external HPSocketDLL;

//* ���ü������Դ�����0 �򲻷��ͼ�������������������Դ�������Ϊ�Ѷ��ߣ� */
procedure HP_UdpServer_SetDetectAttempts(pServer: HP_UdpServer; dwDetectAttempts: Word); stdcall; external HPSocketDLL;
//* ���ü������ͼ�����룬0 �����ͼ����� */
procedure HP_UdpServer_SetDetectInterval(pServer: HP_UdpServer; dwDetectInterval: Word); stdcall; external HPSocketDLL;
//* ��ȡ���������� */
function HP_UdpServer_GetDetectAttempts(pServer: HP_UdpServer):Word; stdcall; external HPSocketDLL;
//* ��ȡ��������� */
function HP_UdpServer_GetDetectInterval(pServer: HP_UdpServer):Word; stdcall; external HPSocketDLL;

//**************************************************************************/
//***************************** Agent �������� *****************************/

//*
{* ���ƣ�����ͨ�����
* ����������ͨ�Ŵ��������������ɺ�ɿ�ʼ����Զ�̷�����
*		
* ������		lpszBindAddress	-- �󶨵�ַ��Ĭ�ϣ�nullptr���� 0.0.0.0��
*			bAsyncConnect	-- �Ƿ�����첽 Connect
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� HP_Agent_GetLastError() ��ȡ�������
*/}
function HP_Agent_Start(pAgent: HP_Agent; lpszBindAddress: PChar; bAsyncConnect: BOOL):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ��ر�ͨ�����
* �������ر�ͨ��������ر���ɺ�Ͽ��������Ӳ��ͷ�������Դ
*		
* ������	
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� HP_Agent_GetLastError() ��ȡ�������
*/}
function HP_Agent_Stop(pAgent: HP_Agent):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ����ӷ�����
* ���������ӷ����������ӳɹ��� IAgentListener ����յ� OnConnect() / OnHandShake() �¼�
*		
* ������		lpszRemoteAddress	-- ����˵�ַ
*			usPort				-- ����˶˿�
*			pdwConnID			-- ���� ID��Ĭ�ϣ�nullptr������ȡ���� ID��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ������ SYS_GetLastError() ��ȡ Windows �������
*/}
function HP_Agent_Connect(pAgent: HP_Agent; lpszRemoteAddress: PChar; usPort: Word; pdwConnID: PInteger):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ����ӷ�����
* ���������ӷ����������ӳɹ��� IAgentListener ����յ� OnConnect() / OnHandShake() �¼�
*		
* ������		lpszRemoteAddress	-- ����˵�ַ
*			usPort				-- ����˶˿�
*			pdwConnID			-- ���� ID��Ĭ�ϣ�nullptr������ȡ���� ID��
*			pExtra				-- ���Ӹ������ݣ�Ĭ�ϣ�nullptr��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ������ SYS_GetLastError() ��ȡ Windows �������
*/}
function HP_Agent_ConnectWithExtra(pAgent: HP_Agent; lpszRemoteAddress: PChar; usPort: Word; pdwConnID: PInteger; pExtra: Pointer): BOOL; stdcall; external HPSocketDLL;


{//*
* ���ƣ���������
* ��������ָ�����ӷ�������
*		
* ������		dwConnID	-- ���� ID
*			pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡ Windows �������
*/}
function HP_Agent_Send(pAgent: HP_Agent; dwConnID: HP_CONNID; const pBuffer: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ���������
* ��������ָ�����ӷ�������
*		
* ������		dwConnID	-- ���� ID
*			pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
*			iOffset		-- ���ͻ�����ָ��ƫ����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡ Windows �������
*/}
function HP_Agent_SendPart(pAgent: HP_Agent; dwConnID: HP_CONNID; const pBuffer: Pointer; iLength: Integer; iOffset: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ����Ͷ�������
* ��������ָ�����ӷ��Ͷ�������
*		TCP - ˳�����������ݰ� 
*		UDP - ���������ݰ���ϳ�һ�����ݰ����ͣ����ݰ����ܳ��Ȳ��ܴ������õ� UDP ����󳤶ȣ� 
*		
* ������		dwConnID	-- ���� ID
*			pBuffers	-- ���ͻ���������
*			iCount		-- ���ͻ�������Ŀ
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡ Windows �������
*/}
function HP_Agent_SendPackets(pAgent: HP_Agent; dwConnID: HP_CONNID; const pBuffers: WSABUFArray; iCount: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ���ͣ/�ָ�����
* ��������ͣ/�ָ�ĳ�����ӵ����ݽ��չ���
*		
* ������		dwConnID	-- ���� ID
*			bPause		-- TRUE - ��ͣ, FALSE - �ָ�
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/ }
function HP_Agent_PauseReceive(pAgent: HP_Agent; dwConnID: HP_CONNID; bPause: BOOL):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ��Ͽ�����
* �������Ͽ�ĳ������
*		
* ������		dwConnID	-- ���� ID
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/}
function HP_Agent_Disconnect(pAgent: HP_Agent; dwConnID: HP_CONNID; bForce: BOOL):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ��Ͽ���ʱ����
* �������Ͽ�����ָ��ʱ��������
*		
* ������		dwPeriod	-- ʱ�������룩
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/}
function HP_Agent_DisconnectLongConnections(pAgent: HP_Agent; dwPeriod: Word; bForce: BOOL):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ��Ͽ���Ĭ����
* �������Ͽ�����ָ��ʱ���ľ�Ĭ����
*		
* ������		dwPeriod	-- ʱ�������룩
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/}
function HP_Agent_DisconnectSilenceConnections(pAgent: HP_Agent; dwPeriod: Word; bForce: BOOL):BOOL; stdcall; external HPSocketDLL;

//******************************************************************************/
//***************************** Agent ���Է��ʷ��� *****************************/

{//*
* ���ƣ��������ӵĸ�������
* �������Ƿ�Ϊ���Ӱ󶨸������ݻ��߰�ʲô�������ݣ�����Ӧ�ó���ֻ�����
*		
* ������		dwConnID	-- ���� ID
*			pv			-- ����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���Ч������ ID��
*/}
function HP_Agent_SetConnectionExtra(pAgent: HP_Agent; dwConnID: HP_CONNID; pExtra: Pointer):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ���ȡ���ӵĸ�������
* �������Ƿ�Ϊ���Ӱ󶨸������ݻ��߰�ʲô�������ݣ�����Ӧ�ó���ֻ�����
*		
* ������		dwConnID	-- ���� ID
*			ppv			-- ����ָ��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���Ч������ ID��
*/}
function HP_Agent_GetConnectionExtra(pAgent: HP_Agent; dwConnID: HP_CONNID; ppExtra: Pointer):BOOL; stdcall; external HPSocketDLL;

//* ���ͨ������Ƿ������� */
function HP_Agent_HasStarted(pAgent: HP_Agent):BOOL; stdcall; external HPSocketDLL;
//* �鿴ͨ�������ǰ״̬ */
function HP_Agent_GetState(pAgent: HP_Agent):En_HP_ServiceState; stdcall; external HPSocketDLL;
//* ��ȡ������ */
function HP_Agent_GetConnectionCount(pAgent: HP_Agent):Word; stdcall; external HPSocketDLL;
//* ��ȡ�������ӵ� HP_CONNID */
function HP_Agent_GetAllConnectionIDs(pAgent: HP_Agent; pIDs: HP_CONNIDArray; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��ȡĳ������ʱ�������룩 */
function HP_Agent_GetConnectPeriod(pAgent: HP_Agent; dwConnID: HP_CONNID; pdwPeriod: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��ȡĳ�����Ӿ�Ĭʱ�䣨���룩 */
function HP_Agent_GetSilencePeriod(pAgent: HP_Agent; dwConnID: HP_CONNID; pdwPeriod: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��ȡĳ�����ӵı��ص�ַ��Ϣ */
function HP_Agent_GetLocalAddress(pAgent: HP_Agent; dwConnID: HP_CONNID; lpszAddress: PChar;var piAddressLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;
//* ��ȡĳ�����ӵ�Զ�̵�ַ��Ϣ */
function HP_Agent_GetRemoteAddress(pAgent: HP_Agent; dwConnID: HP_CONNID; lpszAddress: PChar;var piAddressLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;
//* ��ȡĳ�����ӵ�Զ��������Ϣ */
function HP_Agent_GetRemoteHost(pAgent: HP_Agent; dwConnID: HP_CONNID; lpszHost: PChar;var piHostLen: Integer;var Integer: Word):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ���һ��ʧ�ܲ����Ĵ������ */
function HP_Agent_GetLastError(pAgent: HP_Agent):En_HP_SocketError; stdcall; external HPSocketDLL;
//* ��ȡ���һ��ʧ�ܲ����Ĵ������� */
function HP_Agent_GetLastErrorDesc(pAgent: HP_Agent):PChar; stdcall; external HPSocketDLL;
//* ��ȡ������δ�������ݵĳ��� */
function HP_Agent_GetPendingDataLength(pAgent: HP_Agent; dwConnID: HP_CONNID; piPending: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ���ӵ����ݽ���״̬ */
function HP_Agent_IsPauseReceive(pAgent: HP_Agent; dwConnID: HP_CONNID;var pbPaused: PBOOL):BOOL; stdcall; external HPSocketDLL;

//* �������ݷ��Ͳ��� */
procedure HP_Agent_SetSendPolicy(pAgent: HP_Agent; enSendPolicy: En_HP_SendPolicy); stdcall; external HPSocketDLL;
//* ���� Socket �����������ʱ�䣨���룬�������ڼ�� Socket ��������ܱ���ȡʹ�ã� */
procedure HP_Agent_SetFreeSocketObjLockTime(pAgent: HP_Agent; dwFreeSocketObjLockTime: Word); stdcall; external HPSocketDLL;
//* ���� Socket ����ش�С��ͨ������Ϊƽ���������������� 1/3 - 1/2�� */
procedure HP_Agent_SetFreeSocketObjPool(pAgent: HP_Agent; dwFreeSocketObjPool: Word); stdcall; external HPSocketDLL;
//* �����ڴ�黺��ش�С��ͨ������Ϊ Socket ����ش�С�� 2 - 3 ���� */
procedure HP_Agent_SetFreeBufferObjPool(pAgent: HP_Agent; dwFreeBufferObjPool: Word); stdcall; external HPSocketDLL;
//* ���� Socket ����ػ��շ�ֵ��ͨ������Ϊ Socket ����ش�С�� 3 ���� */
procedure HP_Agent_SetFreeSocketObjHold(pAgent: HP_Agent; dwFreeSocketObjHold: Word); stdcall; external HPSocketDLL;
//* �����ڴ�黺��ػ��շ�ֵ��ͨ������Ϊ�ڴ�黺��ش�С�� 3 ���� */
procedure HP_Agent_SetFreeBufferObjHold(pAgent: HP_Agent; dwFreeBufferObjHold: Word); stdcall; external HPSocketDLL;
//* ���������������������������ֵԤ�����ڴ棬�����Ҫ����ʵ��������ã����˹���*/
procedure HP_Agent_SetMaxConnectionCount(pAgent: HP_Agent; dwMaxConnectionCount: Word); stdcall; external HPSocketDLL;
//* ���ù����߳�������ͨ������Ϊ 2 * CPU + 2�� */
procedure HP_Agent_SetWorkerThreadCount(pAgent: HP_Agent; dwWorkerThreadCount: Word); stdcall; external HPSocketDLL;
//* �����Ƿ��Ǿ�Ĭʱ�䣨����Ϊ TRUE ʱ DisconnectSilenceConnections() �� GetSilencePeriod() ����Ч��Ĭ�ϣ�TRUE�� */
procedure HP_Agent_SetMarkSilence(pAgent: HP_Agent; bMarkSilence: BOOL); stdcall; external HPSocketDLL;

//* ��ȡ���ݷ��Ͳ��� */
function HP_Agent_GetSendPolicy(pAgent: HP_Agent):En_HP_SendPolicy; stdcall; external HPSocketDLL;
//* ��ȡ Socket �����������ʱ�� */
function HP_Agent_GetFreeSocketObjLockTime(pAgent: HP_Agent):Word; stdcall; external HPSocketDLL;
//* ��ȡ Socket ����ش�С */
function HP_Agent_GetFreeSocketObjPool(pAgent: HP_Agent):Word; stdcall; external HPSocketDLL;
//* ��ȡ�ڴ�黺��ش�С */
function HP_Agent_GetFreeBufferObjPool(pAgent: HP_Agent):Word; stdcall; external HPSocketDLL;
//* ��ȡ Socket ����ػ��շ�ֵ */
function HP_Agent_GetFreeSocketObjHold(pAgent: HP_Agent):Word; stdcall; external HPSocketDLL;
//* ��ȡ�ڴ�黺��ػ��շ�ֵ */
function HP_Agent_GetFreeBufferObjHold(pAgent: HP_Agent):Word; stdcall; external HPSocketDLL;
//* ��ȡ��������� */
function HP_Agent_GetMaxConnectionCount(pAgent: HP_Agent):Word; stdcall; external HPSocketDLL;
//* ��ȡ�����߳����� */
function HP_Agent_GetWorkerThreadCount(pAgent: HP_Agent):Word; stdcall; external HPSocketDLL;
//* ����Ƿ��Ǿ�Ĭʱ�� */
function HP_Agent_IsMarkSilence(pAgent: HP_Agent):BOOL; stdcall; external HPSocketDLL;

//**********************************************************************************/
//******************************* TCP Agent �������� *******************************/

//*
{* ���ƣ�����С�ļ�
* ��������ָ�����ӷ��� 4096 KB ���µ�С�ļ�
*		
* ������		dwConnID		-- ���� ID
*			lpszFileName	-- �ļ�·��
*			pHead			-- ͷ����������
*			pTail			-- β����������
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡ Windows �������
*/}
function HP_TcpAgent_SendSmallFile(pAgent: HP_Agent; dwConnID: HP_CONNID; lpszFileName: PChar; const pHead: LPWSABUF; const pTail: LPWSABUF):BOOL; stdcall; external HPSocketDLL;

//**********************************************************************************/
//***************************** TCP Agent ���Է��ʷ��� *****************************/

//* �����Ƿ����õ�ַ���û��ƣ�Ĭ�ϣ������ã� */
procedure HP_TcpAgent_SetReuseAddress(pAgent: HP_TcpAgent; bReuseAddress: BOOL); stdcall; external HPSocketDLL;
//* ����Ƿ����õ�ַ���û��� */
function HP_TcpAgent_IsReuseAddress(pAgent: HP_TcpAgent):BOOL; stdcall; external HPSocketDLL;

//* ����ͨ�����ݻ�������С������ƽ��ͨ�����ݰ���С�������ã�ͨ������Ϊ 1024 �ı����� */
procedure HP_TcpAgent_SetSocketBufferSize(pAgent: HP_TcpAgent; dwSocketBufferSize: Word); stdcall; external HPSocketDLL;
//* ����������������������룬0 �򲻷�����������Ĭ�ϣ�30 * 1000�� */
procedure HP_TcpAgent_SetKeepAliveTime(pAgent: HP_TcpAgent; dwKeepAliveTime: Word); stdcall; external HPSocketDLL;
//* �����쳣��������������룬0 ����������������Ĭ�ϣ�10 * 1000������������ɴ� [Ĭ�ϣ�WinXP 5 ��, Win7 10 ��] ��ⲻ������ȷ�ϰ�����Ϊ�Ѷ��ߣ� */
procedure HP_TcpAgent_SetKeepAliveInterval(pAgent: HP_TcpAgent; dwKeepAliveInterval: Word); stdcall; external HPSocketDLL;

//* ��ȡͨ�����ݻ�������С */
function HP_TcpAgent_GetSocketBufferSize(pAgent: HP_TcpAgent):Word; stdcall; external HPSocketDLL;
//* ��ȡ������������� */
function HP_TcpAgent_GetKeepAliveTime(pAgent: HP_TcpAgent):Word; stdcall; external HPSocketDLL;
//* ��ȡ�쳣��������� */
function HP_TcpAgent_GetKeepAliveInterval(pAgent: HP_TcpAgent):Word; stdcall; external HPSocketDLL;

//******************************************************************************/
//***************************** Client ����������� *****************************/

//*
{* ���ƣ�����ͨ�����
* �����������ͻ���ͨ����������ӷ���ˣ�������ɺ�ɿ�ʼ�շ�����
*		
* ������		lpszRemoteAddress	-- ����˵�ַ
*			usPort				-- ����˶˿�
*			bAsyncConnect		-- �Ƿ�����첽 Connect
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� HP_Client_GetLastError() ��ȡ�������
*/}
function HP_Client_Start(pClient: HP_Client; lpszRemoteAddress: PChar; usPort: Word; bAsyncConnect: BOOL):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ�����ͨ���������ָ���󶨵�ַ��
* �����������ͻ���ͨ����������ӷ���ˣ�������ɺ�ɿ�ʼ�շ�����
*		
* ������		lpszRemoteAddress	-- ����˵�ַ
*			usPort				-- ����˶˿�
*			bAsyncConnect		-- �Ƿ�����첽 Connect
*			lpszBindAddress		-- �󶨵�ַ��Ĭ�ϣ�nullptr��TcpClient/UdpClient -> ��ִ�а󶨲�����UdpCast �� -> 0.0.0.0��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� HP_Client_GetLastError() ��ȡ�������
*/}
function HP_Client_StartWithBindAddress(pClient: HP_Client; lpszRemoteAddress: PChar; usPort: Word; bAsyncConnect: BOOL; lpszBindAddress: PChar):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ��ر�ͨ�����
* �������رտͻ���ͨ��������ر���ɺ�Ͽ������˵����Ӳ��ͷ�������Դ
*		
* ������	
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� HP_Client_GetLastError() ��ȡ�������
*/}
function HP_Client_Stop(pClient: HP_Client):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ���������
* �����������˷�������
*		
* ������		pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡ Windows �������
*/}
function HP_Client_Send(pClient: HP_Client; const pBuffer: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ���������
* �����������˷�������
*		
* ������		pBuffer		-- ���ͻ�����
*			iLength		-- ���ͻ���������
*			iOffset		-- ���ͻ�����ָ��ƫ����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡ Windows �������
*/ }
function HP_Client_SendPart(pClient: HP_Client; const pBuffer: Pointer; iLength: Integer; iOffset: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ����Ͷ�������
* �����������˷��Ͷ�������
*		TCP - ˳�����������ݰ� 
*		UDP - ���������ݰ���ϳ�һ�����ݰ����ͣ����ݰ����ܳ��Ȳ��ܴ������õ� UDP ����󳤶ȣ� 
*		
* ������		pBuffers	-- ���ͻ���������
*			iCount		-- ���ͻ�������Ŀ
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡ Windows �������
*/}
function HP_Client_SendPackets(pClient: HP_Client; const pBuffers: WSABUFArray; iCount: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ���ͣ/�ָ�����
* ��������ͣ/�ָ�ĳ�����ӵ����ݽ��չ���
*		
*			bPause	-- TRUE - ��ͣ, FALSE - �ָ�
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/}
function HP_Client_PauseReceive(pClient: HP_Client; bPause: BOOL):BOOL; stdcall; external HPSocketDLL;

//******************************************************************************/
//***************************** Client ���Է��ʷ��� *****************************/

//* �������ӵĸ������� */
procedure HP_Client_SetExtra(pClient: HP_Client; pExtra: Pointer); stdcall; external HPSocketDLL;
//* ��ȡ���ӵĸ������� */
function HP_Client_GetExtra(pClient: HP_Client): Pointer ; stdcall; external HPSocketDLL;

//* ���ͨ������Ƿ������� */
function HP_Client_HasStarted(pClient: HP_Client):BOOL; stdcall; external HPSocketDLL;
//* �鿴ͨ�������ǰ״̬ */
function HP_Client_GetState(pClient: HP_Client):En_HP_ServiceState; stdcall; external HPSocketDLL;
//* ��ȡ���һ��ʧ�ܲ����Ĵ������ */
function HP_Client_GetLastError(pClient: HP_Client):En_HP_SocketError; stdcall; external HPSocketDLL;
//* ��ȡ���һ��ʧ�ܲ����Ĵ������� */
function HP_Client_GetLastErrorDesc(pClient: HP_Client):PChar; stdcall; external HPSocketDLL;
//* ��ȡ�������������� ID */
function HP_Client_GetConnectionID(pClient: HP_Client):HP_CONNID; stdcall; external HPSocketDLL;
//* ��ȡ Client Socket �ĵ�ַ��Ϣ */
function HP_Client_GetLocalAddress(pClient: HP_Client; lpszAddress: PChar;var piAddressLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ���ӵ�Զ��������Ϣ */
function HP_Client_GetRemoteHost(pClient: HP_Client; lpszHost: PChar;var piHostLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ������δ�������ݵĳ��� */
function HP_Client_GetPendingDataLength(pClient: HP_Client;var piPending: Integer):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ���ӵ����ݽ���״̬ */
function HP_Client_IsPauseReceive(pClient: HP_Client;var pbPaused: PBOOL):BOOL; stdcall; external HPSocketDLL;
//* �����ڴ�黺��ش�С��ͨ������Ϊ -> PUSH ģ�ͣ�5 - 10��PULL ģ�ͣ�10 - 20 �� */
procedure HP_Client_SetFreeBufferPoolSize(pClient: HP_Client; dwFreeBufferPoolSize: Word); stdcall; external HPSocketDLL;
//* �����ڴ�黺��ػ��շ�ֵ��ͨ������Ϊ�ڴ�黺��ش�С�� 3 ���� */
procedure HP_Client_SetFreeBufferPoolHold(pClient: HP_Client; dwFreeBufferPoolHold: Word); stdcall; external HPSocketDLL;
//* ��ȡ�ڴ�黺��ش�С */
function HP_Client_GetFreeBufferPoolSize(pClient: HP_Client):Word; stdcall; external HPSocketDLL;
//* ��ȡ�ڴ�黺��ػ��շ�ֵ */
function HP_Client_GetFreeBufferPoolHold(pClient: HP_Client):Word; stdcall; external HPSocketDLL;

//**********************************************************************************/
//******************************* TCP Client �������� *******************************/

//*
{* ���ƣ�����С�ļ�
* �����������˷��� 4096 KB ���µ�С�ļ�
*		
* ������		lpszFileName	-- �ļ�·��
*			pHead			-- ͷ����������
*			pTail			-- β����������
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� SYS_GetLastError() ��ȡ Windows �������
*/}
function HP_TcpClient_SendSmallFile(pClient: HP_Client; lpszFileName: PChar; const pHead: LPWSABUF; const pTail: LPWSABUF):BOOL; stdcall; external HPSocketDLL;

//**********************************************************************************/
//***************************** TCP Client ���Է��ʷ��� *****************************/

//* ����ͨ�����ݻ�������С������ƽ��ͨ�����ݰ���С�������ã�ͨ������Ϊ��(N * 1024) - sizeof(TBufferObj)�� */
procedure HP_TcpClient_SetSocketBufferSize(pClient: HP_TcpClient; dwSocketBufferSize: Word); stdcall; external HPSocketDLL;
//* ����������������������룬0 �򲻷�����������Ĭ�ϣ�30 * 1000�� */
procedure HP_TcpClient_SetKeepAliveTime(pClient: HP_TcpClient; dwKeepAliveTime: Word); stdcall; external HPSocketDLL;
//* �����쳣��������������룬0 ����������������Ĭ�ϣ�10 * 1000������������ɴ� [Ĭ�ϣ�WinXP 5 ��, Win7 10 ��] ��ⲻ������ȷ�ϰ�����Ϊ�Ѷ��ߣ� */
procedure HP_TcpClient_SetKeepAliveInterval(pClient: HP_TcpClient; dwKeepAliveInterval: Word); stdcall; external HPSocketDLL;

//* ��ȡͨ�����ݻ�������С */
function HP_TcpClient_GetSocketBufferSize(pClient: HP_TcpClient):Word; stdcall; external HPSocketDLL;
//* ��ȡ������������� */
function HP_TcpClient_GetKeepAliveTime(pClient: HP_TcpClient):Word; stdcall; external HPSocketDLL;
//* ��ȡ�쳣��������� */
function HP_TcpClient_GetKeepAliveInterval(pClient: HP_TcpClient):Word; stdcall; external HPSocketDLL;

//**********************************************************************************/
//***************************** UDP Client ���Է��ʷ��� *****************************/

//* �������ݱ�����󳤶ȣ������ھ����������²����� 1472 �ֽڣ��ڹ����������²����� 548 �ֽڣ� */
procedure HP_UdpClient_SetMaxDatagramSize(pClient: HP_UdpClient; dwMaxDatagramSize: Word); stdcall; external HPSocketDLL;
//* ��ȡ���ݱ�����󳤶� */
function HP_UdpClient_GetMaxDatagramSize(pClient: HP_UdpClient):Word; stdcall; external HPSocketDLL;

//* ���ü������Դ�����0 �򲻷��ͼ�������������������Դ�������Ϊ�Ѷ��ߣ� */
procedure HP_UdpClient_SetDetectAttempts(pClient: HP_UdpClient; dwDetectAttempts: Word); stdcall; external HPSocketDLL;
//* ���ü������ͼ�����룬0 �����ͼ����� */
procedure HP_UdpClient_SetDetectInterval(pClient: HP_UdpClient; dwDetectInterval: Word); stdcall; external HPSocketDLL;
//* ��ȡ���������� */
function HP_UdpClient_GetDetectAttempts(pClient: HP_UdpClient):Word; stdcall; external HPSocketDLL;
//* ��ȡ��������� */
function HP_UdpClient_GetDetectInterval(pClient: HP_UdpClient):Word; stdcall; external HPSocketDLL;

//**********************************************************************************/
//****************************** UDP Cast ���Է��ʷ��� ******************************/

//* �������ݱ�����󳤶ȣ������ھ����������²����� 1472 �ֽڣ��ڹ����������²����� 548 �ֽڣ� */
procedure HP_UdpCast_SetMaxDatagramSize(pCast: HP_UdpCast; dwMaxDatagramSize: Word); stdcall; external HPSocketDLL;
//* ��ȡ���ݱ�����󳤶� */
function HP_UdpCast_GetMaxDatagramSize(pCast: HP_UdpCast):Word; stdcall; external HPSocketDLL;
//* ��ȡ��ǰ���ݱ���Զ�̵�ַ��Ϣ��ͨ���� OnReceive �¼��е��ã� */
function HP_UdpCast_GetRemoteAddress(pCast: HP_UdpCast; lpszAddress:array of PChar; piAddressLen: Pointer; pusPort: Pointer):BOOL; stdcall; external HPSocketDLL;
//* �����Ƿ����õ�ַ���û��ƣ�Ĭ�ϣ������ã� */
procedure HP_UdpCast_SetReuseAddress(pCast: HP_UdpCast; bReuseAddress: BOOL); stdcall; external HPSocketDLL;
//* ����Ƿ����õ�ַ���û��� */
function HP_UdpCast_IsReuseAddress(pCast: HP_UdpCast):BOOL; stdcall; external HPSocketDLL;
//* ���ô���ģʽ���鲥��㲥�� */
procedure HP_UdpCast_SetCastMode(pCast: HP_UdpCast; enCastMode: En_HP_CastMode); stdcall; external HPSocketDLL;
//* ��ȡ����ģʽ */
function HP_UdpCast_GetCastMode(pCast: HP_UdpCast):En_HP_CastMode; stdcall; external HPSocketDLL;
//* �����鲥���ĵ� TTL��0 - 255�� */
procedure HP_UdpCast_SetMultiCastTtl(pCast: HP_UdpCast; iMCTtl: Integer); stdcall; external HPSocketDLL;
//* ��ȡ�鲥���ĵ� TTL */
function HP_UdpCast_GetMultiCastTtl(pCast: HP_UdpCast):Integer; stdcall; external HPSocketDLL;
//* �����Ƿ������鲥��·��TRUE or FALSE�� */
procedure HP_UdpCast_SetMultiCastLoop(pCast: HP_UdpCast; bMCLoop: BOOL); stdcall; external HPSocketDLL;
//* ����Ƿ������鲥��· */
function HP_UdpCast_IsMultiCastLoop(pCast: HP_UdpCast):BOOL; stdcall; external HPSocketDLL;

//***************************************************************************************/
//***************************** TCP Pull Server ����������� *****************************/

//*
{* ���ƣ�ץȡ����
* �������û�ͨ���÷����� Socket �����ץȡ����
*		
* ������		dwConnID	-- ���� ID
*			pData		-- ץȡ������
*			iLength		-- ץȡ���ݳ���
* ����ֵ��	En_HP_FetchResult
*/}
function HP_TcpPullServer_Fetch(pServer: HP_TcpPullServer; dwConnID: HP_CONNID; pData: Pointer; iLength: Integer):En_HP_FetchResult; stdcall; external HPSocketDLL;

{//*
* ���ƣ���̽���ݣ������Ƴ����������ݣ�
* �������û�ͨ���÷����� Socket ����п�̽����
*		
* ������		dwConnID	-- ���� ID
*			pData		-- ��̽������
*			iLength		-- ��̽���ݳ���
* ����ֵ��	En_HP_FetchResult
*/}
function HP_TcpPullServer_Peek(pServer: HP_TcpPullServer; dwConnID: HP_CONNID; pData: Pointer; iLength: Integer):En_HP_FetchResult; stdcall; external HPSocketDLL;

//***************************************************************************************/
//***************************** TCP Pull Server ���Է��ʷ��� *****************************/

//***************************************************************************************/
//***************************** TCP Pull Agent ����������� *****************************/

{//*
* ���ƣ�ץȡ����
* �������û�ͨ���÷����� Socket �����ץȡ����
*		
* ������		dwConnID	-- ���� ID
*			pData		-- ץȡ������
*			iLength		-- ץȡ���ݳ���
* ����ֵ��	En_HP_FetchResult
*/}
function HP_TcpPullAgent_Fetch(pAgent: HP_TcpPullAgent; dwConnID: HP_CONNID; pData: Pointer; iLength: Integer):En_HP_FetchResult; stdcall; external HPSocketDLL;

{//*
* ���ƣ���̽���ݣ������Ƴ����������ݣ�
* �������û�ͨ���÷����� Socket ����п�̽����
*		
* ������		dwConnID	-- ���� ID
*			pData		-- ��̽������
*			iLength		-- ��̽���ݳ���
* ����ֵ��	En_HP_FetchResult
*/}
function HP_TcpPullAgent_Peek(pAgent: HP_TcpPullAgent; dwConnID: HP_CONNID; pData: Pointer; iLength: Integer):En_HP_FetchResult; stdcall; external HPSocketDLL;

//***************************************************************************************/
//***************************** TCP Pull Agent ���Է��ʷ��� *****************************/

//***************************************************************************************/
//***************************** TCP Pull Client ����������� *****************************/

{//*
* ���ƣ�ץȡ����
* �������û�ͨ���÷����� Socket �����ץȡ����
*		
* ������		dwConnID	-- ���� ID
*			pData		-- ץȡ������
*			iLength		-- ץȡ���ݳ���
* ����ֵ��	En_HP_FetchResult
*/}
function HP_TcpPullClient_Fetch(pClient: HP_TcpPullClient; pData: Pointer; iLength: Integer):En_HP_FetchResult; stdcall; external HPSocketDLL;

{//*
* ���ƣ���̽���ݣ������Ƴ����������ݣ�
* �������û�ͨ���÷����� Socket ����п�̽����
*		
* ������		dwConnID	-- ���� ID
*			pData		-- ��̽������
*			iLength		-- ��̽���ݳ���
* ����ֵ��	En_HP_FetchResult
*/}
function HP_TcpPullClient_Peek(pClient: HP_TcpPullClient; pData: Pointer; iLength: Integer):En_HP_FetchResult; stdcall; external HPSocketDLL;

//***************************************************************************************/
//***************************** TCP Pull Client ���Է��ʷ��� *****************************/

//***************************************************************************************/
//***************************** TCP Pack Server ����������� *****************************/

//***************************************************************************************/
//***************************** TCP Pack Server ���Է��ʷ��� *****************************/

//* �������ݰ���󳤶ȣ���Ч���ݰ���󳤶Ȳ��ܳ��� 4194303/0x3FFFFF �ֽڣ�Ĭ�ϣ�262144/0x40000�� */
procedure HP_TcpPackServer_SetMaxPackSize(pServer: HP_TcpPackServer; dwMaxPackSize: Word); stdcall; external HPSocketDLL;
//* ���ð�ͷ��ʶ����Ч��ͷ��ʶȡֵ��Χ 0 ~ 1023/0x3FF������ͷ��ʶΪ 0 ʱ��У���ͷ��Ĭ�ϣ�0�� */
procedure HP_TcpPackServer_SetPackHeaderFlag(pServer: HP_TcpPackServer; usPackHeaderFlag: Word); stdcall; external HPSocketDLL;

//* ��ȡ���ݰ���󳤶� */
function HP_TcpPackServer_GetMaxPackSize(pServer: HP_TcpPackServer):Word; stdcall; external HPSocketDLL;
//* ��ȡ��ͷ��ʶ */
function HP_TcpPackServer_GetPackHeaderFlag(pServer: HP_TcpPackServer):Word; stdcall; external HPSocketDLL;

//***************************************************************************************/
//***************************** TCP Pack Agent ����������� *****************************/

//***************************************************************************************/
//***************************** TCP Pack Agent ���Է��ʷ��� *****************************/

//* �������ݰ���󳤶ȣ���Ч���ݰ���󳤶Ȳ��ܳ��� 4194303/0x3FFFFF �ֽڣ�Ĭ�ϣ�262144/0x40000�� */
procedure HP_TcpPackAgent_SetMaxPackSize(pAgent: HP_TcpPackAgent; dwMaxPackSize: Word); stdcall; external HPSocketDLL;
//* ���ð�ͷ��ʶ����Ч��ͷ��ʶȡֵ��Χ 0 ~ 1023/0x3FF������ͷ��ʶΪ 0 ʱ��У���ͷ��Ĭ�ϣ�0�� */
procedure HP_TcpPackAgent_SetPackHeaderFlag(pAgent: HP_TcpPackAgent; usPackHeaderFlag: Word); stdcall; external HPSocketDLL;

//* ��ȡ���ݰ���󳤶� */
function HP_TcpPackAgent_GetMaxPackSize(pAgent: HP_TcpPackAgent):Word; stdcall; external HPSocketDLL;
//* ��ȡ��ͷ��ʶ */
function HP_TcpPackAgent_GetPackHeaderFlag(pAgent: HP_TcpPackAgent):Word; stdcall; external HPSocketDLL;

//***************************************************************************************/
//***************************** TCP Pack Client ����������� *****************************/

//***************************************************************************************/
//***************************** TCP Pack Client ���Է��ʷ��� *****************************/

//* �������ݰ���󳤶ȣ���Ч���ݰ���󳤶Ȳ��ܳ��� 4194303/0x3FFFFF �ֽڣ�Ĭ�ϣ�262144/0x40000�� */
procedure HP_TcpPackClient_SetMaxPackSize(pClient: HP_TcpPackClient; dwMaxPackSize: Word); stdcall; external HPSocketDLL;
//* ���ð�ͷ��ʶ����Ч��ͷ��ʶȡֵ��Χ 0 ~ 1023/0x3FF������ͷ��ʶΪ 0 ʱ��У���ͷ��Ĭ�ϣ�0�� */
procedure HP_TcpPackClient_SetPackHeaderFlag(pClient: HP_TcpPackClient; usPackHeaderFlag: Word); stdcall; external HPSocketDLL;

//* ��ȡ���ݰ���󳤶� */
function HP_TcpPackClient_GetMaxPackSize(pClient: HP_TcpPackClient):Word; stdcall; external HPSocketDLL;
//* ��ȡ��ͷ��ʶ */
function HP_TcpPackClient_GetPackHeaderFlag(pClient: HP_TcpPackClient):Word; stdcall; external HPSocketDLL;

//*****************************************************************************************************************************************************/
//******************************************************************** HTTP Exports *******************************************************************/
//*****************************************************************************************************************************************************/

//****************************************************/
//******************* HTTP �ص����� *******************/

type
  {/* HTTP Server �ص����� */ }
  HP_FN_HttpServer_OnMessageBegin =	 HP_FN_Http_OnMessageBegin;
  HP_FN_HttpServer_OnRequestLine =	 HP_FN_Http_OnRequestLine;
  HP_FN_HttpServer_OnHeader =			 HP_FN_Http_OnHeader;
  HP_FN_HttpServer_OnHeadersComplete = HP_FN_Http_OnHeadersComplete;
  HP_FN_HttpServer_OnBody =			 HP_FN_Http_OnBody;
  HP_FN_HttpServer_OnChunkHeader =	 HP_FN_Http_OnChunkHeader;
  HP_FN_HttpServer_OnChunkComplete =	 HP_FN_Http_OnChunkComplete;
  HP_FN_HttpServer_OnMessageComplete = HP_FN_Http_OnMessageComplete;
  HP_FN_HttpServer_OnUpgrade =		 HP_FN_Http_OnUpgrade;
  HP_FN_HttpServer_OnParseError =		 HP_FN_Http_OnParseError;

  HP_FN_HttpServer_OnWSMessageHeader = HP_FN_Http_OnWSMessageHeader;
  HP_FN_HttpServer_OnWSMessageBody = HP_FN_Http_OnWSMessageBody;
  HP_FN_HttpServer_OnWSMessageComplete = HP_FN_Http_OnWSMessageComplete;

  HP_FN_HttpServer_OnPrepareListen =	 HP_FN_Server_OnPrepareListen;
  HP_FN_HttpServer_OnAccept =			 HP_FN_Server_OnAccept;
  HP_FN_HttpServer_OnHandShake =	     HP_FN_Server_OnHandShake;
  HP_FN_HttpServer_OnReceive =		 HP_FN_Server_OnReceive;
  HP_FN_HttpServer_OnSend =			 HP_FN_Server_OnSend;
  HP_FN_HttpServer_OnClose =			 HP_FN_Server_OnClose;
  HP_FN_HttpServer_OnShutdown	=		 HP_FN_Server_OnShutdown;

  {/* HTTP Agent �ص����� */ }
  HP_FN_HttpAgent_OnMessageBegin =	   HP_FN_Http_OnMessageBegin;
  HP_FN_HttpAgent_OnStatusLine =		   HP_FN_Http_OnStatusLine;
  HP_FN_HttpAgent_OnHeader =			     HP_FN_Http_OnHeader;
  HP_FN_HttpAgent_OnHeadersComplete =  HP_FN_Http_OnHeadersComplete;
  HP_FN_HttpAgent_OnBody =			       HP_FN_Http_OnBody;
  HP_FN_HttpAgent_OnChunkHeader =		   HP_FN_Http_OnChunkHeader;
  HP_FN_HttpAgent_OnChunkComplete =    HP_FN_Http_OnChunkComplete;
  HP_FN_HttpAgent_OnMessageComplete =  HP_FN_Http_OnMessageComplete;
  HP_FN_HttpAgent_OnUpgrade =			     HP_FN_Http_OnUpgrade;
  HP_FN_HttpAgent_OnParseError =		   HP_FN_Http_OnParseError;

  HP_FN_HttpAgent_OnWSMessageHeader =  HP_FN_Http_OnWSMessageHeader;
  HP_FN_HttpAgent_OnWSMessageBody =    HP_FN_Http_OnWSMessageBody;
  HP_FN_HttpAgent_OnWSMessageComplete = HP_FN_Http_OnWSMessageComplete;

  HP_FN_HttpAgent_OnPrepareConnect =   HP_FN_Agent_OnPrepareConnect;
  HP_FN_HttpAgent_OnConnect =			     HP_FN_Agent_OnConnect;
  HP_FN_HttpAgent_OnHandShake =		     HP_FN_Agent_OnHandShake;
  HP_FN_HttpAgent_OnReceive =			     HP_FN_Agent_OnReceive;
  HP_FN_HttpAgent_OnSend =		         HP_FN_Agent_OnSend;
  HP_FN_HttpAgent_OnClose =			       HP_FN_Agent_OnClose;
  HP_FN_HttpAgent_OnShutdown =		     HP_FN_Agent_OnShutdown;

  {/* HTTP Client �ص����� */ }
  HP_FN_HttpClient_OnMessageBegin =    HP_FN_Http_OnMessageBegin;
  HP_FN_HttpClient_OnStatusLine =      HP_FN_Http_OnStatusLine;
  HP_FN_HttpClient_OnHeader =          HP_FN_Http_OnHeader;
  HP_FN_HttpClient_OnHeadersComplete = HP_FN_Http_OnHeadersComplete;
  HP_FN_HttpClient_OnBody =            HP_FN_Http_OnBody;
  HP_FN_HttpClient_OnChunkHeader =     HP_FN_Http_OnChunkHeader;
  HP_FN_HttpClient_OnChunkComplete =   HP_FN_Http_OnChunkComplete;
  HP_FN_HttpClient_OnMessageComplete = HP_FN_Http_OnMessageComplete;
  HP_FN_HttpClient_OnUpgrade =         HP_FN_Http_OnUpgrade;
  HP_FN_HttpClient_OnParseError =      HP_FN_Http_OnParseError;

  HP_FN_HttpClient_OnWSMessageHeader = HP_FN_Http_OnWSMessageHeader;
  HP_FN_HttpClient_OnWSMessageBody =   HP_FN_Http_OnWSMessageBody;
  HP_FN_HttpClient_OnWSMessageComplete = HP_FN_Http_OnWSMessageComplete;

  HP_FN_HttpClient_OnPrepareConnect =  HP_FN_Client_OnPrepareConnect;
  HP_FN_HttpClient_OnConnect =         HP_FN_Client_OnConnect;
  HP_FN_HttpClient_OnHandShake =       HP_FN_Client_OnHandShake;
  HP_FN_HttpClient_OnReceive =         HP_FN_Client_OnReceive;
  HP_FN_HttpClient_OnSend =            HP_FN_Client_OnSend;
  HP_FN_HttpClient_OnClose =	         HP_FN_Client_OnClose;

//****************************************************/
//***************** HTTP ���󴴽����� *****************/

// ���� HP_HttpServer ����
function Create_HP_HttpServer(pListener: HP_HttpServerListener):HP_HttpServer; stdcall; external HPSocketDLL;
// ���� HP_HttpAgent ����
function Create_HP_HttpAgent(pListener: HP_HttpAgentListener):HP_HttpAgent; stdcall; external HPSocketDLL;
// ���� HP_HttpClient ����
function Create_HP_HttpClient(pListener: HP_HttpClientListener):HP_HttpClient; stdcall; external HPSocketDLL;
// ���� HP_HttpClient ����
function Create_HP_HttpSyncClient(pListener: HP_HttpClientListener):HP_HttpSyncClient; stdcall; external HPSocketDLL;

// ���� HP_HttpServer ����
procedure Destroy_HP_HttpServer(pServer: HP_HttpServer); stdcall; external HPSocketDLL;
// ���� HP_HttpAgent ����
procedure Destroy_HP_HttpAgent(pAgent: HP_HttpAgent); stdcall; external HPSocketDLL;
// ���� HP_HttpClient ����
procedure Destroy_HP_HttpClient(pClient: HP_HttpClient); stdcall; external HPSocketDLL;
// ���� HP_HttpSyncClient ����
procedure Destroy_HP_HttpSyncClient(pClient: HP_HttpSyncClient); stdcall; external HPSocketDLL;

// ���� HP_HttpServerListener ����
function Create_HP_HttpServerListener():HP_HttpServerListener; stdcall; external HPSocketDLL;
// ���� HP_HttpAgentListener ����
function Create_HP_HttpAgentListener():HP_HttpAgentListener; stdcall; external HPSocketDLL;
// ���� HP_HttpClientListener ����
function Create_HP_HttpClientListener():HP_HttpClientListener; stdcall; external HPSocketDLL;

// ���� HP_HttpServerListener ����
procedure Destroy_HP_HttpServerListener(pListener: HP_HttpServerListener); stdcall; external HPSocketDLL;
// ���� HP_HttpAgentListener ����
procedure Destroy_HP_HttpAgentListener(pListener: HP_HttpAgentListener); stdcall; external HPSocketDLL;
// ���� HP_HttpClientListener ����
procedure Destroy_HP_HttpClientListener(pListener: HP_HttpClientListener); stdcall; external HPSocketDLL;

//**********************************************************************************/
//*************************** HTTP Server �ص��������÷��� **************************/

procedure HP_Set_FN_HttpServer_OnMessageBegin(pListener		: HP_HttpServerListener; fn: HP_FN_HttpServer_OnMessageBegin); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnRequestLine(pListener		: HP_HttpServerListener; fn: HP_FN_HttpServer_OnRequestLine); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnHeader(pListener			: HP_HttpServerListener; fn: HP_FN_HttpServer_OnHeader); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnHeadersComplete(pListener	: HP_HttpServerListener; fn: HP_FN_HttpServer_OnHeadersComplete); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnBody(pListener				: HP_HttpServerListener; fn: HP_FN_HttpServer_OnBody); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnChunkHeader(pListener		: HP_HttpServerListener; fn: HP_FN_HttpServer_OnChunkHeader); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnChunkComplete(pListener	: HP_HttpServerListener; fn: HP_FN_HttpServer_OnChunkComplete); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnMessageComplete(pListener	: HP_HttpServerListener; fn: HP_FN_HttpServer_OnMessageComplete); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnUpgrade(pListener			: HP_HttpServerListener; fn: HP_FN_HttpServer_OnUpgrade); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnParseError(pListener		: HP_HttpServerListener; fn: HP_FN_HttpServer_OnParseError); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnWSMessageHeader(pListener		: HP_HttpServerListener; fn: HP_FN_HttpServer_OnWSMessageHeader); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnWSMessageBody(pListener		: HP_HttpServerListener; fn: HP_FN_HttpServer_OnWSMessageBody); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnWSMessageComplete(pListener		: HP_HttpServerListener; fn: HP_FN_HttpServer_OnWSMessageComplete); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnPrepareListen(pListener	: HP_HttpServerListener; fn: HP_FN_HttpServer_OnPrepareListen); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnAccept(pListener			: HP_HttpServerListener; fn: HP_FN_HttpServer_OnAccept); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnHandShake(pListener		: HP_HttpServerListener; fn: HP_FN_HttpServer_OnHandShake); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnReceive(pListener			: HP_HttpServerListener; fn: HP_FN_HttpServer_OnReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnSend(pListener				: HP_HttpServerListener; fn: HP_FN_HttpServer_OnSend); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnClose(pListener			: HP_HttpServerListener; fn: HP_FN_HttpServer_OnClose); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpServer_OnShutdown(pListener			: HP_HttpServerListener; fn: HP_FN_HttpServer_OnShutdown); stdcall; external HPSocketDLL;

//**********************************************************************************/
//**************************** HTTP Agent �ص��������÷��� **************************/

procedure HP_Set_FN_HttpAgent_OnMessageBegin(pListener		: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnMessageBegin); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnStatusLine(pListener			: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnStatusLine); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnHeader(pListener				: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnHeader); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnHeadersComplete(pListener	: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnHeadersComplete); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnBody(pListener				: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnBody); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnChunkHeader(pListener		: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnChunkHeader); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnChunkComplete(pListener		: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnChunkComplete); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnMessageComplete(pListener	: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnMessageComplete); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnUpgrade(pListener			: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnUpgrade); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnParseError(pListener			: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnParseError); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnWSMessageHeader(pListener			: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnWSMessageHeader); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnWSMessageBody(pListener			: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnWSMessageBody); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnWSMessageComplete(pListener			: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnWSMessageComplete); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnPrepareConnect(pListener		: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnPrepareConnect); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnConnect(pListener			: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnConnect); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnHandShake(pListener			: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnHandShake); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnReceive(pListener			: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnSend(pListener				: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnSend); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnClose(pListener				: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnClose); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpAgent_OnShutdown(pListener			: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnShutdown); stdcall; external HPSocketDLL;

//**********************************************************************************/
//*************************** HTTP Client �ص��������÷��� **************************/

procedure HP_Set_FN_HttpClient_OnMessageBegin(pListener		: HP_HttpClientListener; fn: HP_FN_HttpClient_OnMessageBegin); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpClient_OnStatusLine(pListener		: HP_HttpClientListener; fn: HP_FN_HttpClient_OnStatusLine); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpClient_OnHeader(pListener			: HP_HttpClientListener; fn: HP_FN_HttpClient_OnHeader); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpClient_OnHeadersComplete(pListener	: HP_HttpClientListener; fn: HP_FN_HttpClient_OnHeadersComplete); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpClient_OnBody(pListener				: HP_HttpClientListener; fn: HP_FN_HttpClient_OnBody); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpClient_OnChunkHeader(pListener		: HP_HttpClientListener; fn: HP_FN_HttpClient_OnChunkHeader); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpClient_OnChunkComplete(pListener	: HP_HttpClientListener; fn: HP_FN_HttpClient_OnChunkComplete); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpClient_OnMessageComplete(pListener	: HP_HttpClientListener; fn: HP_FN_HttpClient_OnMessageComplete); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpClient_OnUpgrade(pListener			: HP_HttpClientListener; fn: HP_FN_HttpClient_OnUpgrade); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpClient_OnParseError(pListener		: HP_HttpClientListener; fn: HP_FN_HttpClient_OnParseError); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnWSMessageHeader(pListener		: HP_HttpClientListener;	fn: HP_FN_HttpClient_OnWSMessageHeader); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpClient_OnWSMessageBody(pListener		: HP_HttpClientListener; fn: HP_FN_HttpClient_OnWSMessageBody); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpClient_OnWSMessageComplete(pListener		: HP_HttpClientListener; fn: HP_FN_HttpClient_OnWSMessageComplete); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnPrepareConnect(pListener	: HP_HttpClientListener; fn: HP_FN_HttpClient_OnPrepareConnect); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpClient_OnConnect(pListener			: HP_HttpClientListener; fn: HP_FN_HttpClient_OnConnect); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpClient_OnHandShake(pListener		: HP_HttpClientListener; fn: HP_FN_HttpClient_OnHandShake); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpClient_OnReceive(pListener			: HP_HttpClientListener; fn: HP_FN_HttpClient_OnReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpClient_OnSend(pListener				: HP_HttpClientListener; fn: HP_FN_HttpClient_OnSend); stdcall; external HPSocketDLL;
procedure HP_Set_FN_HttpClient_OnClose(pListener			: HP_HttpClientListener; fn: HP_FN_HttpClient_OnClose); stdcall; external HPSocketDLL;

//**************************************************************************/
//***************************** Server �������� *****************************/

//*
{* ���ƣ��ظ�����
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
*/}
function HP_HttpServer_SendResponse(pServer: HP_HttpServer; dwConnID: HP_CONNID; usStatusCode: Word; lpszDesc: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pData: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
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
*/ }
function HP_HttpServer_SendLocalFile(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszFileName: PChar; usStatusCode: Word; lpszDesc: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
{/*
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
*/}
function HP_HttpServer_SendWSMessage(pServer: HP_HttpServer; dwConnID: HP_CONNID; bFinal: BOOL; iReserved: PChar; iOperationCode: PChar; const lpszMask: PChar; pData: PChar; iLength: Integer; ullBodyLen: LongWord):BOOL; stdcall; external HPSocketDLL;

{//*
* ���ƣ��ͷ�����
* �����������ӷ����ͷŶ��У��ȴ�ĳ��ʱ�䣨ͨ�� SetReleaseDelay() ���ã��ر�����
*		
* ������		dwConnID		-- ���� ID
* ����ֵ��	TRUE			-- �ɹ�
*			FALSE			-- ʧ��
*/}
function HP_HttpServer_Release(pServer: HP_HttpServer; dwConnID: HP_CONNID):BOOL; stdcall; external HPSocketDLL;

//******************************************************************************/
//***************************** Server ���Է��ʷ��� *****************************/

//* ���������ͷ���ʱ��Ĭ�ϣ�3000 ���룩 */
procedure HP_HttpServer_SetReleaseDelay(pServer: HP_HttpServer; dwReleaseDelay: Word); stdcall; external HPSocketDLL;
//* ��ȡ�����ͷ���ʱ */
function HP_HttpServer_GetReleaseDelay(pServer: HP_HttpServer):Word; stdcall; external HPSocketDLL;
//* ��ȡ������ URL �����루URL ��ο���EnHttpUrlField�� */
function HP_HttpServer_GetUrlFieldSet(pServer: HP_HttpServer; dwConnID: HP_CONNID):Word; stdcall; external HPSocketDLL;
//* ��ȡĳ�� URL ��ֵ */
function HP_HttpServer_GetUrlField(pServer: HP_HttpServer; dwConnID: HP_CONNID; enField: En_HP_HttpUrlField):PChar; stdcall; external HPSocketDLL;
//* ��ȡ���󷽷� */
function HP_HttpServer_GetMethod(pServer: HP_HttpServer; dwConnID: HP_CONNID):PChar; stdcall; external HPSocketDLL;

//* ���ñ���Э��汾 */
procedure HP_HttpServer_SetLocalVersion(pServer: HP_HttpServer; usVersion: En_HP_HttpVersion); stdcall; external HPSocketDLL;
//* ��ȡ����Э��汾 */
function HP_HttpServer_GetLocalVersion(pServer: HP_HttpServer):En_HP_HttpVersion; stdcall; external HPSocketDLL;

//* ����Ƿ�����Э�� */
function HP_HttpServer_IsUpgrade(pServer: HP_HttpServer; dwConnID: HP_CONNID):BOOL; stdcall; external HPSocketDLL;
//* ����Ƿ��� Keep-Alive ��ʶ */
function HP_HttpServer_IsKeepAlive(pServer: HP_HttpServer; dwConnID: HP_CONNID):BOOL; stdcall; external HPSocketDLL;
//* ��ȡЭ��汾 */
function HP_HttpServer_GetVersion(pServer: HP_HttpServer; dwConnID: HP_CONNID):Word; stdcall; external HPSocketDLL;
//* ��ȡ���� */
function HP_HttpServer_GetHost(pServer: HP_HttpServer; dwConnID: HP_CONNID):PChar; stdcall; external HPSocketDLL;
//* ��ȡ���ݳ��� */
function HP_HttpServer_GetContentLength(pServer: HP_HttpServer; dwConnID: HP_CONNID): Longint ; stdcall; external HPSocketDLL;
//* ��ȡ�������� */
function HP_HttpServer_GetContentType(pServer: HP_HttpServer; dwConnID: HP_CONNID):PChar; stdcall; external HPSocketDLL;
//* ��ȡ���ݱ��� */
function HP_HttpServer_GetContentEncoding(pServer: HP_HttpServer; dwConnID: HP_CONNID):PChar; stdcall; external HPSocketDLL;
//* ��ȡ������� */
function HP_HttpServer_GetTransferEncoding(pServer: HP_HttpServer; dwConnID: HP_CONNID):PChar; stdcall; external HPSocketDLL;
//* ��ȡЭ���������� */
function HP_HttpServer_GetUpgradeType(pServer: HP_HttpServer; dwConnID: HP_CONNID):En_HP_HttpUpgradeType; stdcall; external HPSocketDLL;
//* ��ȡ����������� */
function HP_HttpServer_GetParseErrorCode(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszErrorDesc: PChar):Word; stdcall; external HPSocketDLL;

//* ��ȡĳ������ͷ����ֵ�� */
function HP_HttpServer_GetHeader(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszName: PChar; lpszValue: PChar):BOOL; stdcall; external HPSocketDLL;
//* ��ȡĳ������ͷ����ֵ�� */
function HP_HttpServer_GetHeaders(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszName: PChar; lpszValue:array of PChar; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ��������ͷ */
function HP_HttpServer_GetAllHeaders(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpHeaders: TNVPair; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ��������ͷ���� */
function HP_HttpServer_GetAllHeaderNames(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszName:array of PChar; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;

//* ��ȡ Cookie */
function HP_HttpServer_GetCookie(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszName: PChar; lpszValue: PChar):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ���� Cookie */
function HP_HttpServer_GetAllCookies(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpCookies: TNVPair; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ��ǰ WebSocket ��Ϣ״̬������ nullptr �򲻻�ȡ��Ӧ�ֶ� */
function HP_HttpServer_GetWSMessageState(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpbFinal: BOOL; lpiReserved: PChar; lpiOperationCode: PChar; lpszMask: PChar; lpullBodyLen: LongWord; lpullBodyRemain: LongWord):BOOL; stdcall; external HPSocketDLL;

//**************************************************************************/
//***************************** Agent �������� ******************************/

//*
{* ���ƣ���������
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
*/}
function HP_HttpAgent_SendRequest(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszMethod: PChar; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pData: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
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
*/}
function HP_HttpAgent_SendLocalFile(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszFileName: PChar; lpszMethod: PChar; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;

//* ���� POST ���� */
function HP_HttpAgent_SendPost(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pBody: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;
//* ���� PUT ���� */
function HP_HttpAgent_SendPut(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pBody: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;
//* ���� PATCH ���� */
function HP_HttpAgent_SendPatch(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pBody: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;
//* ���� GET ���� */
function HP_HttpAgent_SendGet(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* ���� DELETE ���� */
function HP_HttpAgent_SendDelete(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* ���� HEAD ���� */
function HP_HttpAgent_SendHead(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* ���� TRACE ���� */
function HP_HttpAgent_SendTrace(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* ���� OPTIONS ���� */
function HP_HttpAgent_SendOptions(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* ���� CONNECT ���� */
function HP_HttpAgent_SendConnect(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszHost: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
{/*
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
*/}
function HP_HttpAgent_SendWSMessage(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; bFinal: BOOL; iReserved: PChar; iOperationCode: PChar; const lpszMask: Byte; pData: PChar; iLength: Integer; ullBodyLen: Longword):BOOL; stdcall; external HPSocketDLL;

//******************************************************************************/
//***************************** Agent ���Է��ʷ��� ******************************/

//* ��ȡ HTTP ״̬�� */
function HP_HttpAgent_GetStatusCode(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):Word; stdcall; external HPSocketDLL;

//* ���ñ���Э��汾 */
procedure HP_HttpAgent_SetLocalVersion(pAgent: HP_HttpAgent; usVersion: En_HP_HttpVersion); stdcall; external HPSocketDLL;
//* ��ȡ����Э��汾 */
function HP_HttpAgent_GetLocalVersion(pAgent: HP_HttpAgent):En_HP_HttpVersion; stdcall; external HPSocketDLL;

//* ����Ƿ�����Э�� */
function HP_HttpAgent_IsUpgrade(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):BOOL; stdcall; external HPSocketDLL;
//* ����Ƿ��� Keep-Alive ��ʶ */
function HP_HttpAgent_IsKeepAlive(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):BOOL; stdcall; external HPSocketDLL;
//* ��ȡЭ��汾 */
function HP_HttpAgent_GetVersion(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):Word; stdcall; external HPSocketDLL;
//* ��ȡ���ݳ��� */
function HP_HttpAgent_GetContentLength(pAgent: HP_HttpAgent; dwConnID: HP_CONNID): Longint ; stdcall; external HPSocketDLL;
//* ��ȡ�������� */
function HP_HttpAgent_GetContentType(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):PChar; stdcall; external HPSocketDLL;

//* ��ȡЭ���������� */
function HP_HttpAgent_GetUpgradeType(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):En_HP_HttpUpgradeType; stdcall; external HPSocketDLL;
//* ��ȡ����������� */
function HP_HttpAgent_GetParseErrorCode(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszErrorDesc: PChar):Word; stdcall; external HPSocketDLL;
//* ��ȡ���ݱ��� */
function HP_HttpAgent_GetContentEncoding(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):PChar; stdcall; external HPSocketDLL;
//* ��ȡ������� */
function HP_HttpAgent_GetTransferEncoding(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):PChar; stdcall; external HPSocketDLL;
//* ��ȡĳ������ͷ����ֵ�� */
function HP_HttpAgent_GetHeader(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszName: PChar; lpszValue: PChar):BOOL; stdcall; external HPSocketDLL;
//* ��ȡĳ������ͷ����ֵ�� */
function HP_HttpAgent_GetHeaders(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszName: PChar; lpszValue:array of PChar; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ��������ͷ */
function HP_HttpAgent_GetAllHeaders(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpHeaders: TNVPair; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ��������ͷ���� */
function HP_HttpAgent_GetAllHeaderNames(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszName:array of PChar; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;

//* ��ȡ Cookie */
function HP_HttpAgent_GetCookie(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszName: PChar; lpszValue: PChar):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ���� Cookie */
function HP_HttpAgent_GetAllCookies(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpCookies: TNVPair; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��� Cookie */
function HP_HttpAgent_AddCookie(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszName: PChar; lpszValue: PChar; bRelpace: BOOL):BOOL; stdcall; external HPSocketDLL;
//* ɾ�� Cookie */
function HP_HttpAgent_DeleteCookie(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszName: PChar):BOOL; stdcall; external HPSocketDLL;
//* ɾ������ Cookie */
function HP_HttpAgent_DeleteAllCookies(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):BOOL; stdcall; external HPSocketDLL;
 //* ��ȡ��ǰ WebSocket ��Ϣ״̬������ nullptr �򲻻�ȡ��Ӧ�ֶ� */
function HP_HttpAgent_GetWSMessageState(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpbFinal: BOOL; lpiReserved: PChar; lpiOperationCode: PChar; lpszMask: PChar; lpullBodyLen: Longword; lpullBodyRemain: Longword):BOOL; stdcall; external HPSocketDLL;


//**************************************************************************/
//***************************** Client �������� *****************************/

//*
{* ���ƣ���������
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
*/}
function HP_HttpClient_SendRequest(pClient: HP_HttpClient; lpszMethod: PChar; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pBody: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
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
*/}
function HP_HttpClient_SendLocalFile(pClient: HP_HttpClient; lpszFileName: PChar; lpszMethod: PChar; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;

//* ���� POST ���� */
function HP_HttpClient_SendPost(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pBody: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;
//* ���� PUT ���� */
function HP_HttpClient_SendPut(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pBody: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;
//* ���� PATCH ���� */
function HP_HttpClient_SendPatch(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pBody: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;
//* ���� GET ���� */
function HP_HttpClient_SendGet(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* ���� DELETE ���� */
function HP_HttpClient_SendDelete(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* ���� HEAD ���� */
function HP_HttpClient_SendHead(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* ���� TRACE ���� */
function HP_HttpClient_SendTrace(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* ���� OPTIONS ���� */
function HP_HttpClient_SendOptions(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* ���� CONNECT ���� */
function HP_HttpClient_SendConnect(pClient: HP_HttpClient; lpszHost: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
{/*
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
*/}
function HP_HttpClient_SendWSMessage(pClient: HP_HttpClient; bFinal: BOOL; iReserved: PChar; iOperationCode: PChar; const lpszMask: Byte; pData: PChar; iLength: Integer; ullBodyLen: LongWord):BOOL; stdcall; external HPSocketDLL;

//******************************************************************************/
//***************************** Client ���Է��ʷ��� *****************************/

//* ��ȡ HTTP ״̬�� */
function HP_HttpClient_GetStatusCode(pClient: HP_HttpClient):Word; stdcall; external HPSocketDLL;

//* ���ñ���Э��汾 */
procedure HP_HttpClient_SetLocalVersion(pClient: HP_HttpClient; usVersion: En_HP_HttpVersion); stdcall; external HPSocketDLL;
//* ��ȡ����Э��汾 */
function HP_HttpClient_GetLocalVersion(pClient: HP_HttpClient):En_HP_HttpVersion; stdcall; external HPSocketDLL;

//* ����Ƿ�����Э�� */
function HP_HttpClient_IsUpgrade(pClient: HP_HttpClient):BOOL; stdcall; external HPSocketDLL;
//* ����Ƿ��� Keep-Alive ��ʶ */
function HP_HttpClient_IsKeepAlive(pClient: HP_HttpClient):BOOL; stdcall; external HPSocketDLL;
//* ��ȡЭ��汾 */
function HP_HttpClient_GetVersion(pClient: HP_HttpClient):Word; stdcall; external HPSocketDLL;
//* ��ȡ���ݳ��� */
function HP_HttpClient_GetContentLength(pClient: HP_HttpClient): Longint ; stdcall; external HPSocketDLL;
//* ��ȡ�������� */
function HP_HttpClient_GetContentType(pClient: HP_HttpClient):PChar; stdcall; external HPSocketDLL;
//* ��ȡ���ݱ��� */
function HP_HttpClient_GetContentEncoding(pClient: HP_HttpClient):PChar; stdcall; external HPSocketDLL;
//* ��ȡ������� */
function HP_HttpClient_GetTransferEncoding(pClient: HP_HttpClient):PChar; stdcall; external HPSocketDLL;
//* ��ȡЭ���������� */
function HP_HttpClient_GetUpgradeType(pClient: HP_HttpClient):En_HP_HttpUpgradeType; stdcall; external HPSocketDLL;
//* ��ȡ����������� */
function HP_HttpClient_GetParseErrorCode(pClient: HP_HttpClient; lpszErrorDesc: PChar):Word; stdcall; external HPSocketDLL;

//* ��ȡĳ������ͷ����ֵ�� */
function HP_HttpClient_GetHeader(pClient: HP_HttpClient; lpszName: PChar; lpszValue: PChar):BOOL; stdcall; external HPSocketDLL;
//* ��ȡĳ������ͷ����ֵ�� */
function HP_HttpClient_GetHeaders(pClient: HP_HttpClient; lpszName: PChar; lpszValue:array of PChar; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ��������ͷ */
function HP_HttpClient_GetAllHeaders(pClient: HP_HttpClient; lpHeaders: TNVPair; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ��������ͷ���� */
function HP_HttpClient_GetAllHeaderNames(pClient: HP_HttpClient; lpszName:array of PChar; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;

//* ��ȡ Cookie */
function HP_HttpClient_GetCookie(pClient: HP_HttpClient; lpszName: PChar; lpszValue: PChar):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ���� Cookie */
function HP_HttpClient_GetAllCookies(pClient: HP_HttpClient; lpCookies: TNVPair; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* ��� Cookie */
function HP_HttpClient_AddCookie(pClient: HP_HttpClient; lpszName: PChar; lpszValue: PChar; bRelpace: BOOL):BOOL; stdcall; external HPSocketDLL;
//* ɾ�� Cookie */
function HP_HttpClient_DeleteCookie(pClient: HP_HttpClient; lpszName: PChar):BOOL; stdcall; external HPSocketDLL;
//* ɾ������ Cookie */
function HP_HttpClient_DeleteAllCookies(pClient: HP_HttpClient):BOOL; stdcall; external HPSocketDLL;
//* ��ȡ��ǰ WebSocket ��Ϣ״̬������ nullptr �򲻻�ȡ��Ӧ�ֶ� */
function HP_HttpClient_GetWSMessageState(pClient: HP_HttpClient; lpbFinal: BOOL; lpiReserved: PChar; lpiOperationCode: PChar; lpszMask: PChar; lpullBodyLen: LongWord; lpullBodyRemain: LongWord):BOOL; stdcall; external HPSocketDLL;

{/**************************************************************************/
/************************ HTTP Sync Client �������� ************************/}
{/*
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
*/}
function HP_HttpSyncClient_OpenUrl(pClient: HP_HttpSyncClient; lpszMethod: PChar; lpszUrl: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pBody: PChar; iLength: Integer; bForceReconnect: BOOL):BOOL; stdcall; external HPSocketDLL;
{/*
* ���ƣ����������
* �����������һ���������Ӧͷ����Ӧ��Ƚ����Ϣ���÷�������ÿ�η�������ǰ�Զ����ã�
*
* ������		
* ����ֵ��	TRUE			-- �ɹ�
*			FALSE			-- ʧ��
*/}
function HP_HttpSyncClient_CleanupRequestResult(pClient: HP_HttpSyncClient):BOOL; stdcall; external HPSocketDLL;

{/******************************************************************************/
/************************ HTTP Sync Client ���Է��ʷ��� ************************/}

//* �������ӳ�ʱ�����룬0��ϵͳĬ�ϳ�ʱ��Ĭ�ϣ�5000�� */
procedure HP_HttpSyncClient_SetConnectTimeout(pClient: HP_HttpSyncClient; dwConnectTimeout: LongWord); stdcall; external HPSocketDLL;
//* ��������ʱ�����룬0�����޵ȴ���Ĭ�ϣ�10000�� */                                                     
procedure HP_HttpSyncClient_SetRequestTimeout(pClient: HP_HttpSyncClient; dwRequestTimeout: LongWord); stdcall; external HPSocketDLL;

//* ��ȡ���ӳ�ʱ */
function HP_HttpSyncClient_GetConnectTimeout(pClient: HP_HttpSyncClient):LongWord; stdcall; external HPSocketDLL;
//* ��ȡ����ʱ */
function HP_HttpSyncClient_GetRequestTimeout(pClient: HP_HttpSyncClient):LongWord; stdcall; external HPSocketDLL;

//* ��ȡ��Ӧ�� */
function HP_HttpSyncClient_GetResponseBody(pClient: HP_HttpSyncClient; lpszBod: PChar; piLength: Pointer):BOOL; stdcall; external HPSocketDLL;

//*****************************************************************************************************************************************************/
//*************************************************************** Global Function Exports *************************************************************/
//*****************************************************************************************************************************************************/
// ��ȡ HPSocket �汾�ţ�4 ���ֽڷֱ�Ϊ�����汾�ţ��Ӱ汾�ţ������汾�ţ�������ţ�
function HP_GetHPSocketVersion():LongWord; stdcall; external HPSocketDLL;

//* ��ȡ���������ı� */
function HP_GetSocketErrorDesc(enCode: En_HP_SocketError):PChar; stdcall; external HPSocketDLL;
//* ����ϵͳ�� GetLastError() ������ȡϵͳ������� */
function SYS_GetLastError():Word; stdcall; external HPSocketDLL;
// ����ϵͳ�� WSAGetLastError() ������ȡϵͳ�������
function SYS_WSAGetLastError():Integer; stdcall; external HPSocketDLL;
// ����ϵͳ�� setsockopt()
function SYS_SetSocketOption(sock: SOCKET; level: Integer; name: Integer; val: Pointer; len: Integer):Integer; stdcall; external HPSocketDLL;
// ����ϵͳ�� getsockopt()
function SYS_GetSocketOption(sock: SOCKET; level: Integer; name: Integer; val: Pointer; len: Pointer):Integer; stdcall; external HPSocketDLL;
// ����ϵͳ�� ioctlsocket()
function SYS_IoctlSocket(sock: SOCKET; cmd: Longint; arg: Longword):Integer; stdcall; external HPSocketDLL;
// ����ϵͳ�� WSAIoctl()
function SYS_WSAIoctl(sock: SOCKET; dwIoControlCode: Word; lpvInBuffer: Pointer; cbInBuffer: Word; lpvOutBuffer: Pointer; cbOutBuffer: Word; lpcbBytesReturned: LPDWORD):Integer; stdcall; external HPSocketDLL;

// ���� socket ѡ�IPPROTO_TCP -> TCP_NODELAY
function SYS_SSO_NoDelay(sock: SOCKET; bNoDelay: BOOL):Integer; stdcall; external HPSocketDLL;
// ���� socket ѡ�SOL_SOCKET -> SO_DONTLINGER
function SYS_SSO_DontLinger(sock: SOCKET; bDont: BOOL):Integer; stdcall; external HPSocketDLL;
// ���� socket ѡ�SOL_SOCKET -> SO_LINGER
function SYS_SSO_Linger(sock: SOCKET; l_onoff: Word; l_linger: Word):Integer; stdcall; external HPSocketDLL;
// ���� socket ѡ�SOL_SOCKET -> SO_RCVBUF
function SYS_SSO_RecvBuffSize(sock: SOCKET; size: Integer):Integer; stdcall; external HPSocketDLL;
// ���� socket ѡ�SOL_SOCKET -> SO_SNDBUF
function SYS_SSO_SendBuffSize(sock: SOCKET; size: Integer):Integer; stdcall; external HPSocketDLL;
// ���� socket ѡ�SOL_SOCKET -> SO_REUSEADDR
function SYS_SSO_ReuseAddress(sock: SOCKET; bReuse: BOOL):Integer; stdcall; external HPSocketDLL;

// ��ȡ SOCKET ���ص�ַ��Ϣ
function SYS_GetSocketLocalAddress(socket: SOCKET; lpszAddress: PChar;var piAddressLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;
// ��ȡ SOCKET Զ�̵�ַ��Ϣ
function SYS_GetSocketRemoteAddress(socket: SOCKET; lpszAddress: PChar;var piAddressLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;

//* ö������ IP ��ַ */
function SYS_EnumHostIPAddresses(lpszHost: PChar; enType: En_HP_IPAddrType;var lpppIPAddr : HP_TIPAddr;var piIPAddrCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* �ͷ� HP_LPTIPAddr* */
function SYS_FreeHostIPAddresses(var lppIPAddr : HP_TIPAddr):BOOL; stdcall; external HPSocketDLL;

//* ����ַ����Ƿ���� IP ��ַ��ʽ */
function SYS_IsIPAddress(lpszAddress: PChar ;var penType: En_HP_IPAddrType):BOOL; stdcall; external HPSocketDLL;
//* ͨ����������ȡ IP ��ַ */
function SYS_GetIPAddress(lpszHost: PChar; lpszIP: PChar;var piIPLenth: Integer; var penType: En_HP_IPAddrType):BOOL; stdcall; external HPSocketDLL;


//* 64 λ�����ֽ���ת�����ֽ��� */
function SYS_NToH64(value: Longword):Longword; stdcall; external HPSocketDLL;
//* 64 λ�����ֽ���ת�����ֽ��� */
function SYS_HToN64(value: Longword):Longword; stdcall; external HPSocketDLL;

//===================================�����Ǳ��뺯��
// CP_XXX -> UNICODE
function SYS_CodePageToUnicode(iCodePage: Integer; const szSrc: array of PChar; szDest: array of WideString;  piDestLength: Pointer):BOOL; stdcall; external HPSocketDLL;
// UNICODE -> CP_XXX
function SYS_UnicodeToCodePage(iCodePage: Integer; const szSrc: array of WideString; szDest: array of PChar; piDestLength: Pointer):BOOL; stdcall; external HPSocketDLL;
// GBK -> UNICODE
function SYS_GbkToUnicode(const szSrc: array of PChar; szDest: array of WideString; piDestLength: Pointer):BOOL; stdcall; external HPSocketDLL;
// UNICODE -> GBK
function SYS_UnicodeToGbk(const szSrc: array of WideString; szDest: array of PChar; piDestLength: Pointer):BOOL; stdcall; external HPSocketDLL;
// UTF8 -> UNICODE
function SYS_Utf8ToUnicode(const szSrc: array of PChar; szDest: array of WideString; piDestLength: Pointer):BOOL; stdcall; external HPSocketDLL;
// UNICODE -> UTF8
function SYS_UnicodeToUtf8(const szSrc: array of WideString; szDest: array of PChar; piDestLength: Pointer):BOOL; stdcall; external HPSocketDLL;
// GBK -> UTF8
function SYS_GbkToUtf8(const szSrc: array of PChar; szDest: array of WideString; piDestLength: Pointer):BOOL; stdcall; external HPSocketDLL;
// UTF8 -> GBK
function SYS_Utf8ToGbk(const szSrc: array of PChar; szDest: array of PChar; piDestLength: Pointer):BOOL; stdcall; external HPSocketDLL;

// ��ͨѹ��
function SYS_Compress(const lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord):Integer; stdcall; external HPSocketDLL;
// �߼�ѹ����Ĭ��ֵ��iLevel -> -1��iMethod -> 8��iWindowBits -> 15��iMemLevel -> 8��iStrategy -> 0��
function SYS_CompressEx(const lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord; iLevel: Integer; iMethod: Integer; iWindowBits: Integer; iMemLevel: Integer; iStrategy: Integer):Integer; stdcall; external HPSocketDLL;
// ��ͨ��ѹ
function SYS_Uncompress(const lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord):Integer; stdcall; external HPSocketDLL;
// �߼���ѹ��Ĭ��ֵ��iWindowBits -> 15��
function SYS_UncompressEx(const lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord; iWindowBits: Integer):Integer; stdcall; external HPSocketDLL;
// �Ʋ�ѹ���������
function SYS_GuessCompressBound(dwSrcLen: LongWord; bGZip: BooL):LongWord; stdcall; external HPSocketDLL;

// Gzip ѹ��
function SYS_GZipCompress(const lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord):Integer; stdcall; external HPSocketDLL;
// Gzip ��ѹ
function SYS_GZipUncompress(const lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord):Integer; stdcall; external HPSocketDLL;
// �Ʋ� Gzip ��ѹ������ȣ�������� 0 �򲻺���ֵ��˵���������ݲ�����Ч�� Gzip ��ʽ��
function SYS_GZipGuessUncompressBound(const lpszSrc: PChar; dwSrcLen: LongWord):LongWord; stdcall; external HPSocketDLL;

// ���� Base64 ����󳤶�
function SYS_GuessBase64EncodeBound(dwSrcLen: LongWord):LongWord; stdcall; external HPSocketDLL;
// ���� Base64 ����󳤶�
function SYS_GuessBase64DecodeBound(const lpszSrc: PChar; dwSrcLen: LongWord):LongWord; stdcall; external HPSocketDLL;
// Base64 ���루����ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
function SYS_Base64Encode(const lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord):Integer; stdcall; external HPSocketDLL;
// Base64 ���루����ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
function SYS_Base64Decode(const lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord):Integer; stdcall; external HPSocketDLL;

// ���� URL ����󳤶�
function SYS_GuessUrlEncodeBound(const lpszSrc: PChar; dwSrcLen: LongWord):LongWord; stdcall; external HPSocketDLL;
// ���� URL ����󳤶�
function SYS_GuessUrlDecodeBound(const lpszSrc: PChar; dwSrcLen: LongWord):LongWord; stdcall; external HPSocketDLL;
// URL ���루����ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
function SYS_UrlEncode(lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord):Integer; stdcall; external HPSocketDLL;
// URL ���루����ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
function SYS_UrlDecode(lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord):Integer; stdcall; external HPSocketDLL;



implementation


initialization
  IsMultiThread := True;
end.

