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
    名称：定义 Socket 对象指针类型别名
    描述：把 Socket 对象指针定义为更直观的别名
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
//***************** TCP/UDP 回调函数 ******************/

//* Server 回调函数 */
HP_FN_Server_OnPrepareListen = function(pSender: HP_Server; soListen: Pointer):En_HP_HandleResult; stdcall;
// 如果为 TCP 连接，pClient为 SOCKET 句柄；如果为 UDP 连接，pClient为 SOCKADDR_IN 指针；
HP_FN_Server_OnAccept = function(pSender: HP_Server; dwConnID: HP_CONNID; pClient: Pointer):En_HP_HandleResult; stdcall;
HP_FN_Server_OnHandShake = function(pSender: HP_Server; dwConnID: HP_CONNID):En_HP_HandleResult; stdcall;
HP_FN_Server_OnSend = function(pSender: HP_Server; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Server_OnReceive = function(pSender: HP_Server; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Server_OnPullReceive = function(pSender: HP_Server; dwConnID: HP_CONNID; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Server_OnClose = function(pSender: HP_Server; dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer):En_HP_HandleResult; stdcall;
HP_FN_Server_OnShutdown = function(pSender: HP_Server):En_HP_HandleResult; stdcall;

//* Agent 回调函数 */
HP_FN_Agent_OnPrepareConnect = function(pSender: HP_Agent; dwConnID: HP_CONNID; socket: Pointer):En_HP_HandleResult; stdcall;
HP_FN_Agent_OnConnect = function(pSender: HP_Agent; dwConnID: HP_CONNID):En_HP_HandleResult; stdcall;
HP_FN_Agent_OnHandShake = function(pSender: HP_Agent; dwConnID: HP_CONNID):En_HP_HandleResult; stdcall;
HP_FN_Agent_OnSend = function(pSender: HP_Agent; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Agent_OnReceive = function(pSender: HP_Agent; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Agent_OnPullReceive = function(pSender: HP_Agent; dwConnID: HP_CONNID; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Agent_OnClose = function(pSender: HP_Agent; dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer):En_HP_HandleResult; stdcall;
HP_FN_Agent_OnShutdown = function(pSender: HP_Agent):En_HP_HandleResult; stdcall;

//* Client 回调函数 */
HP_FN_Client_OnPrepareConnect = function(pSender: HP_Client; dwConnID: HP_CONNID; socket: Pointer):En_HP_HandleResult; stdcall;
HP_FN_Client_OnConnect = function(pSender: HP_Client; dwConnID: HP_CONNID):En_HP_HandleResult; stdcall;
HP_FN_Client_OnHandShake = function(pSender: HP_Client; dwConnID: HP_CONNID):En_HP_HandleResult; stdcall;
HP_FN_Client_OnSend = function(pSender: HP_Client; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Client_OnReceive = function(pSender: HP_Client; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Client_OnPullReceive = function(pSender: HP_Client; dwConnID: HP_CONNID; iLength: Integer):En_HP_HandleResult; stdcall;
HP_FN_Client_OnClose = function(pSender: HP_Client; dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer):En_HP_HandleResult; stdcall;

//* HTTP 回调函数 */
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
//*************** TCP/UDP 对象创建函数 ****************/

// 创建 HP_TcpServer 对象
function Create_HP_TcpServer(pListener: HP_TcpServerListener):HP_TcpServer; stdcall; external HPSocketDLL;
// 创建 HP_TcpAgent 对象
function Create_HP_TcpAgent(pListener: HP_TcpAgentListener):HP_TcpAgent; stdcall; external HPSocketDLL;
// 创建 HP_TcpClient 对象
function Create_HP_TcpClient(pListener: HP_TcpClientListener):HP_TcpClient; stdcall; external HPSocketDLL;
// 创建 HP_TcpPullServer 对象
function Create_HP_TcpPullServer(pListener: HP_TcpPullServerListener):HP_TcpPullServer; stdcall; external HPSocketDLL;
// 创建 HP_TcpPullAgent 对象
function Create_HP_TcpPullAgent(pListener: HP_TcpPullAgentListener):HP_TcpPullAgent; stdcall; external HPSocketDLL;
// 创建 HP_TcpPullClient 对象
function Create_HP_TcpPullClient(pListener: HP_TcpPullClientListener):HP_TcpPullClient; stdcall; external HPSocketDLL;
// 创建 HP_TcpPackServer 对象
function Create_HP_TcpPackServer(pListener: HP_TcpServerListener):HP_TcpPackServer; stdcall; external HPSocketDLL;
// 创建 HP_TcpPackAgent 对象
function Create_HP_TcpPackAgent(pListener: HP_TcpAgentListener):HP_TcpPackAgent; stdcall; external HPSocketDLL;
// 创建 HP_TcpPackClient 对象
function Create_HP_TcpPackClient(pListener: HP_TcpClientListener):HP_TcpPackClient; stdcall; external HPSocketDLL;
// 创建 HP_UdpServer 对象
function Create_HP_UdpServer(pListener: HP_UdpServerListener):HP_UdpServer; stdcall; external HPSocketDLL;
// 创建 HP_UdpClient 对象
function Create_HP_UdpClient(pListener: HP_UdpClientListener):HP_UdpClient; stdcall; external HPSocketDLL;
// 创建 HP_UdpCast 对象
function Create_HP_UdpCast(pListener: HP_UdpCastListener):HP_UdpCast; stdcall; external HPSocketDLL;

// 销毁 HP_TcpServer 对象
procedure Destroy_HP_TcpServer(pServer: HP_TcpServer); stdcall; external HPSocketDLL;
// 销毁 HP_TcpAgent 对象
procedure Destroy_HP_TcpAgent(pAgent: HP_TcpAgent); stdcall; external HPSocketDLL;
// 销毁 HP_TcpClient 对象
procedure Destroy_HP_TcpClient(pClient: HP_TcpClient); stdcall; external HPSocketDLL;
// 销毁 HP_TcpPullServer 对象
procedure Destroy_HP_TcpPullServer(pServer: HP_TcpPullServer); stdcall; external HPSocketDLL;
// 销毁 HP_TcpPullAgent 对象
procedure Destroy_HP_TcpPullAgent(pAgent: HP_TcpPullAgent); stdcall; external HPSocketDLL;
// 销毁 HP_TcpPullClient 对象
procedure Destroy_HP_TcpPullClient(pClient: HP_TcpPullClient); stdcall; external HPSocketDLL;
// 销毁 HP_TcpPackServer 对象
procedure Destroy_HP_TcpPackServer(pServer: HP_TcpPackServer); stdcall; external HPSocketDLL;
// 销毁 HP_TcpPackAgent 对象
procedure Destroy_HP_TcpPackAgent(pAgent: HP_TcpPackAgent); stdcall; external HPSocketDLL;
// 销毁 HP_TcpPackClient 对象
procedure Destroy_HP_TcpPackClient(pClient: HP_TcpPackClient); stdcall; external HPSocketDLL;
// 销毁 HP_UdpServer 对象
procedure Destroy_HP_UdpServer(pServer: HP_UdpServer); stdcall; external HPSocketDLL;
// 销毁 HP_UdpClient 对象
procedure Destroy_HP_UdpClient(pClient: HP_UdpClient); stdcall; external HPSocketDLL;
// 销毁 HP_UdpCast 对象
procedure Destroy_HP_UdpCast(pCast: HP_UdpCast); stdcall; external HPSocketDLL;

// 创建 HP_TcpServerListener 对象
function Create_HP_TcpServerListener():HP_TcpServerListener; stdcall; external HPSocketDLL;
// 创建 HP_TcpAgentListener 对象
function Create_HP_TcpAgentListener():HP_TcpAgentListener; stdcall; external HPSocketDLL;
// 创建 HP_TcpClientListener 对象
function Create_HP_TcpClientListener():HP_TcpClientListener; stdcall; external HPSocketDLL;
// 创建 HP_TcpPullServerListener 对象
function Create_HP_TcpPullServerListener():HP_TcpPullServerListener; stdcall; external HPSocketDLL;
// 创建 HP_TcpPullAgentListener 对象
function Create_HP_TcpPullAgentListener():HP_TcpPullAgentListener; stdcall; external HPSocketDLL;
// 创建 HP_TcpPullClientListener 对象
function Create_HP_TcpPullClientListener():HP_TcpPullClientListener; stdcall; external HPSocketDLL;
// 创建 HP_TcpPackServerListener 对象
function Create_HP_TcpPackServerListener():HP_TcpPackServerListener; stdcall; external HPSocketDLL;
// 创建 HP_TcpPackAgentListener 对象
function Create_HP_TcpPackAgentListener():HP_TcpPackAgentListener; stdcall; external HPSocketDLL;
// 创建 HP_TcpPackClientListener 对象
function Create_HP_TcpPackClientListener():HP_TcpPackClientListener; stdcall; external HPSocketDLL;
// 创建 HP_UdpServerListener 对象
function Create_HP_UdpServerListener():HP_UdpServerListener; stdcall; external HPSocketDLL;
// 创建 HP_UdpClientListener 对象
function Create_HP_UdpClientListener():HP_UdpClientListener; stdcall; external HPSocketDLL;
// 创建 HP_UdpCastListener 对象
function Create_HP_UdpCastListener():HP_UdpCastListener; stdcall; external HPSocketDLL;

// 销毁 HP_TcpServerListener 对象
procedure Destroy_HP_TcpServerListener(pListener: HP_TcpServerListener); stdcall; external HPSocketDLL;
// 销毁 HP_TcpAgentListener 对象
procedure Destroy_HP_TcpAgentListener(pListener: HP_TcpAgentListener); stdcall; external HPSocketDLL;
// 销毁 HP_TcpClientListener 对象
procedure Destroy_HP_TcpClientListener(pListener: HP_TcpClientListener); stdcall; external HPSocketDLL;
// 销毁 HP_TcpPullServerListener 对象
procedure Destroy_HP_TcpPullServerListener(pListener: HP_TcpPullServerListener); stdcall; external HPSocketDLL;
// 销毁 HP_TcpPullAgentListener 对象
procedure Destroy_HP_TcpPullAgentListener(pListener: HP_TcpPullAgentListener); stdcall; external HPSocketDLL;
// 销毁 HP_TcpPullClientListener 对象
procedure Destroy_HP_TcpPullClientListener(pListener: HP_TcpPullClientListener); stdcall; external HPSocketDLL;
// 销毁 HP_TcpPackServerListener 对象
procedure Destroy_HP_TcpPackServerListener(pListener: HP_TcpPackServerListener); stdcall; external HPSocketDLL;
// 销毁 HP_TcpPackAgentListener 对象
procedure Destroy_HP_TcpPackAgentListener(pListener: HP_TcpPackAgentListener); stdcall; external HPSocketDLL;
// 销毁 HP_TcpPackClientListener 对象
procedure Destroy_HP_TcpPackClientListener(pListener: HP_TcpPackClientListener); stdcall; external HPSocketDLL;
// 销毁 HP_UdpServerListener 对象
procedure Destroy_HP_UdpServerListener(pListener: HP_UdpServerListener); stdcall; external HPSocketDLL;
// 销毁 HP_UdpClientListener 对象
procedure Destroy_HP_UdpClientListener(pListener: HP_UdpClientListener); stdcall; external HPSocketDLL;
// 销毁 HP_UdpCastListener 对象
procedure Destroy_HP_UdpCastListener(pListener: HP_UdpCastListener); stdcall; external HPSocketDLL;

//**********************************************************************************/
//***************************** Server 回调函数设置方法 *****************************/

procedure HP_Set_FN_Server_OnPrepareListen(pListener	: HP_ServerListener; fn: HP_FN_Server_OnPrepareListen); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnAccept(pListener			: HP_ServerListener; fn: HP_FN_Server_OnAccept); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnHandShake(pListener		: HP_ServerListener; fn: HP_FN_Server_OnHandShake); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnSend(pListener				: HP_ServerListener; fn: HP_FN_Server_OnSend); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnReceive(pListener			: HP_ServerListener; fn: HP_FN_Server_OnReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnPullReceive(pListener		: HP_ServerListener; fn: HP_FN_Server_OnPullReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnClose(pListener			: HP_ServerListener; fn: HP_FN_Server_OnClose); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnShutdown(pListener			: HP_ServerListener; fn: HP_FN_Server_OnShutdown); stdcall; external HPSocketDLL;

//**********************************************************************************/
//****************************** Agent 回调函数设置方法 *****************************/

procedure HP_Set_FN_Agent_OnPrepareConnect(pListener		: HP_AgentListener; fn: HP_FN_Agent_OnPrepareConnect); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnConnect(pListener			: HP_AgentListener; fn: HP_FN_Agent_OnConnect); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnHandShake(pListener			: HP_AgentListener; fn: HP_FN_Agent_OnHandShake); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnSend(pListener				: HP_AgentListener; fn: HP_FN_Agent_OnSend); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnReceive(pListener			: HP_AgentListener; fn: HP_FN_Agent_OnReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnPullReceive(pListener		: HP_AgentListener; fn: HP_FN_Agent_OnPullReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnClose(pListener				: HP_AgentListener; fn: HP_FN_Agent_OnClose); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnShutdown(pListener			: HP_AgentListener; fn: HP_FN_Agent_OnShutdown); stdcall; external HPSocketDLL;

//**********************************************************************************/
//***************************** Client 回调函数设置方法 *****************************/

procedure HP_Set_FN_Client_OnPrepareConnect(pListener	: HP_ClientListener; fn: HP_FN_Client_OnPrepareConnect); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnConnect(pListener			: HP_ClientListener; fn: HP_FN_Client_OnConnect); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnHandShake(pListener		: HP_ClientListener; fn: HP_FN_Client_OnHandShake); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnSend(pListener				: HP_ClientListener; fn: HP_FN_Client_OnSend); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnReceive(pListener			: HP_ClientListener; fn: HP_FN_Client_OnReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnPullReceive(pListener		: HP_ClientListener; fn: HP_FN_Client_OnPullReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnClose(pListener			: HP_ClientListener; fn: HP_FN_Client_OnClose); stdcall; external HPSocketDLL;

//**************************************************************************/
//***************************** Server 操作方法 *****************************/

{//*
* 名称：启动通信组件
* 描述：启动服务端通信组件，启动完成后可开始接收客户端连接并收发数据
*		
* 参数：		lpszBindAddress	-- 监听地址
*			usPort			-- 监听端口
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 HP_Server_GetLastError() 获取错误代码
*/}
function HP_Server_Start(pServer: HP_Server; lpszBindAddress: PChar; usPort: Word):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：关闭通信组件
* 描述：关闭服务端通信组件，关闭完成后断开所有客户端连接并释放所有资源
*		
* 参数：	
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 HP_Server_GetLastError() 获取错误代码
*/}
function HP_Server_Stop(pServer: HP_Server):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：发送数据
* 描述：向指定连接发送数据
*		
* 参数：		dwConnID	-- 连接 ID
*			pBuffer		-- 发送缓冲区
*			iLength		-- 发送缓冲区长度
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
*/}
function HP_Server_Send(pServer: HP_Server; dwConnID: HP_CONNID; const pBuffer: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：发送数据
* 描述：向指定连接发送数据
*		
* 参数：		dwConnID	-- 连接 ID
*			pBuffer		-- 发送缓冲区
*			iLength		-- 发送缓冲区长度
*			iOffset		-- 发送缓冲区指针偏移量
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
*/}
function HP_Server_SendPart(pServer: HP_Server; dwConnID: HP_CONNID; const pBuffer: Pointer; iLength: Integer; iOffset: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：发送多组数据
* 描述：向指定连接发送多组数据
*		TCP - 顺序发送所有数据包 
*		UDP - 把所有数据包组合成一个数据包发送（数据包的总长度不能大于设置的 UDP 包最大长度） 
*		
* 参数：		dwConnID	-- 连接 ID
*			pBuffers	-- 发送缓冲区数组
*			iCount		-- 发送缓冲区数目
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
*/}
function HP_Server_SendPackets(pServer: HP_Server; dwConnID: HP_CONNID; const pBuffers: WSABUFArray; iCount: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：断开连接
* 描述：断开与某个客户端的连接
*		
* 参数：		dwConnID	-- 连接 ID
*			bForce		-- 是否强制断开连接
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败
*/}
function HP_Server_Disconnect(pServer: HP_Server; dwConnID: HP_CONNID; bForce: BOOL):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：断开超时连接
* 描述：断开超过指定时长的连接
*		
* 参数：		dwPeriod	-- 时长（毫秒）
*			bForce		-- 是否强制断开连接
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败
*/}
function HP_Server_DisconnectLongConnections(pServer: HP_Server; dwPeriod: Word; bForce: BOOL):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：断开静默连接
* 描述：断开超过指定时长的静默连接
*		
* 参数：		dwPeriod	-- 时长（毫秒）
*			bForce		-- 是否强制断开连接
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败
*/}
function HP_Server_DisconnectSilenceConnections(pServer: HP_Server; dwPeriod: Word; bForce: BOOL):BOOL; stdcall; external HPSocketDLL;

//******************************************************************************/
//***************************** Server 属性访问方法 *****************************/

{//*
* 名称：设置连接的附加数据
* 描述：是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序只身决定
*		
* 参数：		dwConnID	-- 连接 ID
*			pv			-- 数据
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败（无效的连接 ID）
*/}
function HP_Server_SetConnectionExtra(pServer: HP_Server; dwConnID: HP_CONNID; pExtra: Pointer):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：获取连接的附加数据
* 描述：是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序只身决定
*		
* 参数：		dwConnID	-- 连接 ID
*			ppv			-- 数据指针
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败（无效的连接 ID）
*/}
function HP_Server_GetConnectionExtra(pServer: HP_Server; dwConnID: HP_CONNID; ppExtra: Pointer):BOOL; stdcall; external HPSocketDLL;

//* 检查通信组件是否已启动 */
function HP_Server_HasStarted(pServer: HP_Server):BOOL; stdcall; external HPSocketDLL;
//* 查看通信组件当前状态 */
function HP_Server_GetState(pServer: HP_Server):En_HP_ServiceState; stdcall; external HPSocketDLL;
//* 获取最近一次失败操作的错误代码 */
function HP_Server_GetLastError(pServer: HP_Server):En_HP_SocketError; stdcall; external HPSocketDLL;
//* 获取最近一次失败操作的错误描述 */
function HP_Server_GetLastErrorDesc(pServer: HP_Server):PChar; stdcall; external HPSocketDLL;
//* 获取连接中未发出数据的长度 */
function HP_Server_GetPendingDataLength(pServer: HP_Server; dwConnID: HP_CONNID; piPending: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 获取客户端连接数 */
function HP_Server_GetConnectionCount(pServer: HP_Server):Word; stdcall; external HPSocketDLL;
//* 获取所有连接的 HP_CONNID */
function HP_Server_GetAllConnectionIDs(pServer: HP_Server; pIDs: HP_CONNIDArray; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 获取某个客户端连接时长（毫秒） */
function HP_Server_GetConnectPeriod(pServer: HP_Server; dwConnID: HP_CONNID; pdwPeriod: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 获取某个连接静默时间（毫秒） */
function HP_Server_GetSilencePeriod(pServer: HP_Server; dwConnID: HP_CONNID; pdwPeriod: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 获取监听 Socket 的地址信息 */
function HP_Server_GetListenAddress(pServer: HP_Server; lpszAddress: PChar;var piAddressLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;
//* 获取某个连接的本地地址信息 */
function HP_Server_GetLocalAddress(pServer: HP_Server; dwConnID: HP_CONNID; lpszAddress: PChar;var piAddressLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;
//* 获取某个连接的远程地址信息 */
function HP_Server_GetRemoteAddress(pServer: HP_Server; dwConnID: HP_CONNID; lpszAddress: Pchar;var piAddressLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;

//* 设置数据发送策略 */
procedure HP_Server_SetSendPolicy(pServer: HP_Server; enSendPolicy: En_HP_SendPolicy); stdcall; external HPSocketDLL;
//* 设置 Socket 缓存对象锁定时间（毫秒，在锁定期间该 Socket 缓存对象不能被获取使用） */
procedure HP_Server_SetFreeSocketObjLockTime(pServer: HP_Server; dwFreeSocketObjLockTime: Word); stdcall; external HPSocketDLL;
//* 设置 Socket 缓存池大小（通常设置为平均并发连接数量的 1/3 - 1/2） */
procedure HP_Server_SetFreeSocketObjPool(pServer: HP_Server; dwFreeSocketObjPool: Word); stdcall; external HPSocketDLL;
//* 设置内存块缓存池大小（通常设置为 Socket 缓存池大小的 2 - 3 倍） */
procedure HP_Server_SetFreeBufferObjPool(pServer: HP_Server; dwFreeBufferObjPool: Word); stdcall; external HPSocketDLL;
//* 设置 Socket 缓存池回收阀值（通常设置为 Socket 缓存池大小的 3 倍） */
procedure HP_Server_SetFreeSocketObjHold(pServer: HP_Server; dwFreeSocketObjHold: Word); stdcall; external HPSocketDLL;
//* 设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍） */
procedure HP_Server_SetFreeBufferObjHold(pServer: HP_Server; dwFreeBufferObjHold: Word); stdcall; external HPSocketDLL;
//* 设置最大连接数（组件会根据设置值预分配内存，因此需要根据实际情况设置，不宜过大）*/
procedure HP_Server_SetMaxConnectionCount(pServer: HP_Server; dwMaxConnectionCount: Word); stdcall; external HPSocketDLL;
//* 设置工作线程数量（通常设置为 2 * CPU + 2） */
procedure HP_Server_SetWorkerThreadCount(pServer: HP_Server; dwWorkerThreadCount: Word); stdcall; external HPSocketDLL;
//* 设置是否标记静默时间（设置为 TRUE 时 DisconnectSilenceConnections() 和 GetSilencePeriod() 才有效，默认：TRUE） */
procedure HP_Server_SetMarkSilence(pServer: HP_Server; bMarkSilence: BOOL); stdcall; external HPSocketDLL;

//* 获取数据发送策略 */
function HP_Server_GetSendPolicy(pServer: HP_Server):En_HP_SendPolicy; stdcall; external HPSocketDLL;
//* 获取 Socket 缓存对象锁定时间 */
function HP_Server_GetFreeSocketObjLockTime(pServer: HP_Server):Word; stdcall; external HPSocketDLL;
//* 获取 Socket 缓存池大小 */
function HP_Server_GetFreeSocketObjPool(pServer: HP_Server):Word; stdcall; external HPSocketDLL;
//* 获取内存块缓存池大小 */
function HP_Server_GetFreeBufferObjPool(pServer: HP_Server):Word; stdcall; external HPSocketDLL;
//* 获取 Socket 缓存池回收阀值 */
function HP_Server_GetFreeSocketObjHold(pServer: HP_Server):Word; stdcall; external HPSocketDLL;
//* 获取内存块缓存池回收阀值 */
function HP_Server_GetFreeBufferObjHold(pServer: HP_Server):Word; stdcall; external HPSocketDLL;
//* 获取最大连接数 */
function HP_Server_GetMaxConnectionCount(pServer: HP_Server):Word; stdcall; external HPSocketDLL;
//* 获取工作线程数量 */
function HP_Server_GetWorkerThreadCount(pServer: HP_Server):Word; stdcall; external HPSocketDLL;
//* 检测是否标记静默时间 */
function HP_Server_IsMarkSilence(pServer: HP_Server):BOOL; stdcall; external HPSocketDLL;

//**********************************************************************************/
//******************************* TCP Server 操作方法 *******************************/

{//*
* 名称：发送小文件
* 描述：向指定连接发送 4096 KB 以下的小文件
*		
* 参数：		dwConnID		-- 连接 ID
*			lpszFileName	-- 文件路径
*			pHead			-- 头部附加数据
*			pTail			-- 尾部附加数据
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
*/}
function HP_TcpServer_SendSmallFile(pServer: HP_Server; dwConnID: HP_CONNID; lpszFileName: PChar; const pHead: LPWSABUF; const pTail: LPWSABUF):BOOL; stdcall; external HPSocketDLL;

//**********************************************************************************/
//***************************** TCP Server 属性访问方法 *****************************/

//* 设置监听 Socket 的等候队列大小（根据并发连接数量调整设置） */
procedure HP_TcpServer_SetSocketListenQueue(pServer: HP_TcpServer; dwSocketListenQueue: Word); stdcall; external HPSocketDLL;
//* 设置 Accept 预投递数量（根据负载调整设置，Accept 预投递数量越大则支持的并发连接请求越多） */
procedure HP_TcpServer_SetAcceptSocketCount(pServer: HP_TcpServer; dwAcceptSocketCount: Word); stdcall; external HPSocketDLL;
//* 设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为 1024 的倍数） */
procedure HP_TcpServer_SetSocketBufferSize(pServer: HP_TcpServer; dwSocketBufferSize: Word); stdcall; external HPSocketDLL;
//* 设置正常心跳包间隔（毫秒，0 则不发送心跳包，默认：30 * 1000） */
procedure HP_TcpServer_SetKeepAliveTime(pServer: HP_TcpServer; dwKeepAliveTime: Word); stdcall; external HPSocketDLL;
//* 设置异常心跳包间隔（毫秒，0 不发送心跳包，，默认：10 * 1000，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线） */
procedure HP_TcpServer_SetKeepAliveInterval(pServer: HP_TcpServer; dwKeepAliveInterval: Word); stdcall; external HPSocketDLL;

//* 获取 Accept 预投递数量 */
function HP_TcpServer_GetAcceptSocketCount(pServer: HP_TcpServer):Word; stdcall; external HPSocketDLL;
//* 获取通信数据缓冲区大小 */
function HP_TcpServer_GetSocketBufferSize(pServer: HP_TcpServer):Word; stdcall; external HPSocketDLL;
//* 获取监听 Socket 的等候队列大小 */
function HP_TcpServer_GetSocketListenQueue(pServer: HP_TcpServer):Word; stdcall; external HPSocketDLL;
//* 获取正常心跳包间隔 */
function HP_TcpServer_GetKeepAliveTime(pServer: HP_TcpServer):Word; stdcall; external HPSocketDLL;
//* 获取异常心跳包间隔 */
function HP_TcpServer_GetKeepAliveInterval(pServer: HP_TcpServer):Word; stdcall; external HPSocketDLL;

//**********************************************************************************/
//***************************** UDP Server 属性访问方法 *****************************/

//* 设置数据报文最大长度（建议在局域网环境下不超过 1472 字节，在广域网环境下不超过 548 字节） */
procedure HP_UdpServer_SetMaxDatagramSize(pServer: HP_UdpServer; dwMaxDatagramSize: Word); stdcall; external HPSocketDLL;
//* 获取数据报文最大长度 */
function HP_UdpServer_GetMaxDatagramSize(pServer: HP_UdpServer):Word; stdcall; external HPSocketDLL;

//* 设置 Receive 预投递数量（根据负载调整设置，Receive 预投递数量越大则丢包概率越小） */
procedure HP_UdpServer_SetPostReceiveCount(pServer: HP_UdpServer; dwPostReceiveCount: Word); stdcall; external HPSocketDLL;
//* 获取 Receive 预投递数量 */
function HP_UdpServer_GetPostReceiveCount(pServer: HP_UdpServer):Word; stdcall; external HPSocketDLL;

//* 设置监测包尝试次数（0 则不发送监测跳包，如果超过最大尝试次数则认为已断线） */
procedure HP_UdpServer_SetDetectAttempts(pServer: HP_UdpServer; dwDetectAttempts: Word); stdcall; external HPSocketDLL;
//* 设置监测包发送间隔（秒，0 不发送监测包） */
procedure HP_UdpServer_SetDetectInterval(pServer: HP_UdpServer; dwDetectInterval: Word); stdcall; external HPSocketDLL;
//* 获取心跳检查次数 */
function HP_UdpServer_GetDetectAttempts(pServer: HP_UdpServer):Word; stdcall; external HPSocketDLL;
//* 获取心跳检查间隔 */
function HP_UdpServer_GetDetectInterval(pServer: HP_UdpServer):Word; stdcall; external HPSocketDLL;

//**************************************************************************/
//***************************** Agent 操作方法 *****************************/

//*
{* 名称：启动通信组件
* 描述：启动通信代理组件，启动完成后可开始连接远程服务器
*		
* 参数：		lpszBindAddress	-- 绑定地址（默认：nullptr，绑定 0.0.0.0）
*			bAsyncConnect	-- 是否采用异步 Connect
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 HP_Agent_GetLastError() 获取错误代码
*/}
function HP_Agent_Start(pAgent: HP_Agent; lpszBindAddress: PChar; bAsyncConnect: BOOL):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：关闭通信组件
* 描述：关闭通信组件，关闭完成后断开所有连接并释放所有资源
*		
* 参数：	
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 HP_Agent_GetLastError() 获取错误代码
*/}
function HP_Agent_Stop(pAgent: HP_Agent):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：连接服务器
* 描述：连接服务器，连接成功后 IAgentListener 会接收到 OnConnect() / OnHandShake() 事件
*		
* 参数：		lpszRemoteAddress	-- 服务端地址
*			usPort				-- 服务端端口
*			pdwConnID			-- 连接 ID（默认：nullptr，不获取连接 ID）
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过函数 SYS_GetLastError() 获取 Windows 错误代码
*/}
function HP_Agent_Connect(pAgent: HP_Agent; lpszRemoteAddress: PChar; usPort: Word; pdwConnID: PInteger):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：发送数据
* 描述：向指定连接发送数据
*		
* 参数：		dwConnID	-- 连接 ID
*			pBuffer		-- 发送缓冲区
*			iLength		-- 发送缓冲区长度
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
*/}
function HP_Agent_Send(pAgent: HP_Agent; dwConnID: HP_CONNID; const pBuffer: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：发送数据
* 描述：向指定连接发送数据
*		
* 参数：		dwConnID	-- 连接 ID
*			pBuffer		-- 发送缓冲区
*			iLength		-- 发送缓冲区长度
*			iOffset		-- 发送缓冲区指针偏移量
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
*/}
function HP_Agent_SendPart(pAgent: HP_Agent; dwConnID: HP_CONNID; const pBuffer: Pointer; iLength: Integer; iOffset: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：发送多组数据
* 描述：向指定连接发送多组数据
*		TCP - 顺序发送所有数据包 
*		UDP - 把所有数据包组合成一个数据包发送（数据包的总长度不能大于设置的 UDP 包最大长度） 
*		
* 参数：		dwConnID	-- 连接 ID
*			pBuffers	-- 发送缓冲区数组
*			iCount		-- 发送缓冲区数目
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
*/}
function HP_Agent_SendPackets(pAgent: HP_Agent; dwConnID: HP_CONNID; const pBuffers: WSABUFArray; iCount: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：断开连接
* 描述：断开某个连接
*		
* 参数：		dwConnID	-- 连接 ID
*			bForce		-- 是否强制断开连接
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败
*/}
function HP_Agent_Disconnect(pAgent: HP_Agent; dwConnID: HP_CONNID; bForce: BOOL):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：断开超时连接
* 描述：断开超过指定时长的连接
*		
* 参数：		dwPeriod	-- 时长（毫秒）
*			bForce		-- 是否强制断开连接
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败
*/}
function HP_Agent_DisconnectLongConnections(pAgent: HP_Agent; dwPeriod: Word; bForce: BOOL):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：断开静默连接
* 描述：断开超过指定时长的静默连接
*		
* 参数：		dwPeriod	-- 时长（毫秒）
*			bForce		-- 是否强制断开连接
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败
*/}
function HP_Agent_DisconnectSilenceConnections(pAgent: HP_Agent; dwPeriod: Word; bForce: BOOL):BOOL; stdcall; external HPSocketDLL;

//******************************************************************************/
//***************************** Agent 属性访问方法 *****************************/

{//*
* 名称：设置连接的附加数据
* 描述：是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序只身决定
*		
* 参数：		dwConnID	-- 连接 ID
*			pv			-- 数据
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败（无效的连接 ID）
*/}
function HP_Agent_SetConnectionExtra(pAgent: HP_Agent; dwConnID: HP_CONNID; pExtra: Pointer):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：获取连接的附加数据
* 描述：是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序只身决定
*		
* 参数：		dwConnID	-- 连接 ID
*			ppv			-- 数据指针
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败（无效的连接 ID）
*/}
function HP_Agent_GetConnectionExtra(pAgent: HP_Agent; dwConnID: HP_CONNID; ppExtra: Pointer):BOOL; stdcall; external HPSocketDLL;

//* 检查通信组件是否已启动 */
function HP_Agent_HasStarted(pAgent: HP_Agent):BOOL; stdcall; external HPSocketDLL;
//* 查看通信组件当前状态 */
function HP_Agent_GetState(pAgent: HP_Agent):En_HP_ServiceState; stdcall; external HPSocketDLL;
//* 获取连接数 */
function HP_Agent_GetConnectionCount(pAgent: HP_Agent):Word; stdcall; external HPSocketDLL;
//* 获取所有连接的 HP_CONNID */
function HP_Agent_GetAllConnectionIDs(pAgent: HP_Agent; pIDs: HP_CONNIDArray; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 获取某个连接时长（毫秒） */
function HP_Agent_GetConnectPeriod(pAgent: HP_Agent; dwConnID: HP_CONNID; pdwPeriod: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 获取某个连接静默时间（毫秒） */
function HP_Agent_GetSilencePeriod(pAgent: HP_Agent; dwConnID: HP_CONNID; pdwPeriod: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 获取某个连接的本地地址信息 */
function HP_Agent_GetLocalAddress(pAgent: HP_Agent; dwConnID: HP_CONNID; lpszAddress: PChar;var piAddressLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;
//* 获取某个连接的远程地址信息 */
function HP_Agent_GetRemoteAddress(pAgent: HP_Agent; dwConnID: HP_CONNID; lpszAddress: PChar;var piAddressLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;
//* 获取某个连接的远程主机信息 */
function HP_Agent_GetRemoteHost(pAgent: HP_Agent; dwConnID: HP_CONNID; lpszHost: PChar;var piHostLen: Integer;var Integer: Word):BOOL; stdcall; external HPSocketDLL;
//* 获取最近一次失败操作的错误代码 */
function HP_Agent_GetLastError(pAgent: HP_Agent):En_HP_SocketError; stdcall; external HPSocketDLL;
//* 获取最近一次失败操作的错误描述 */
function HP_Agent_GetLastErrorDesc(pAgent: HP_Agent):PChar; stdcall; external HPSocketDLL;
//* 获取连接中未发出数据的长度 */
function HP_Agent_GetPendingDataLength(pAgent: HP_Agent; dwConnID: HP_CONNID; piPending: Pointer):BOOL; stdcall; external HPSocketDLL;

//* 设置数据发送策略 */
procedure HP_Agent_SetSendPolicy(pAgent: HP_Agent; enSendPolicy: En_HP_SendPolicy); stdcall; external HPSocketDLL;
//* 设置 Socket 缓存对象锁定时间（毫秒，在锁定期间该 Socket 缓存对象不能被获取使用） */
procedure HP_Agent_SetFreeSocketObjLockTime(pAgent: HP_Agent; dwFreeSocketObjLockTime: Word); stdcall; external HPSocketDLL;
//* 设置 Socket 缓存池大小（通常设置为平均并发连接数量的 1/3 - 1/2） */
procedure HP_Agent_SetFreeSocketObjPool(pAgent: HP_Agent; dwFreeSocketObjPool: Word); stdcall; external HPSocketDLL;
//* 设置内存块缓存池大小（通常设置为 Socket 缓存池大小的 2 - 3 倍） */
procedure HP_Agent_SetFreeBufferObjPool(pAgent: HP_Agent; dwFreeBufferObjPool: Word); stdcall; external HPSocketDLL;
//* 设置 Socket 缓存池回收阀值（通常设置为 Socket 缓存池大小的 3 倍） */
procedure HP_Agent_SetFreeSocketObjHold(pAgent: HP_Agent; dwFreeSocketObjHold: Word); stdcall; external HPSocketDLL;
//* 设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍） */
procedure HP_Agent_SetFreeBufferObjHold(pAgent: HP_Agent; dwFreeBufferObjHold: Word); stdcall; external HPSocketDLL;
//* 设置最大连接数（组件会根据设置值预分配内存，因此需要根据实际情况设置，不宜过大）*/
procedure HP_Agent_SetMaxConnectionCount(pAgent: HP_Agent; dwMaxConnectionCount: Word); stdcall; external HPSocketDLL;
//* 设置工作线程数量（通常设置为 2 * CPU + 2） */
procedure HP_Agent_SetWorkerThreadCount(pAgent: HP_Agent; dwWorkerThreadCount: Word); stdcall; external HPSocketDLL;
//* 设置是否标记静默时间（设置为 TRUE 时 DisconnectSilenceConnections() 和 GetSilencePeriod() 才有效，默认：TRUE） */
procedure HP_Agent_SetMarkSilence(pAgent: HP_Agent; bMarkSilence: BOOL); stdcall; external HPSocketDLL;

//* 获取数据发送策略 */
function HP_Agent_GetSendPolicy(pAgent: HP_Agent):En_HP_SendPolicy; stdcall; external HPSocketDLL;
//* 获取 Socket 缓存对象锁定时间 */
function HP_Agent_GetFreeSocketObjLockTime(pAgent: HP_Agent):Word; stdcall; external HPSocketDLL;
//* 获取 Socket 缓存池大小 */
function HP_Agent_GetFreeSocketObjPool(pAgent: HP_Agent):Word; stdcall; external HPSocketDLL;
//* 获取内存块缓存池大小 */
function HP_Agent_GetFreeBufferObjPool(pAgent: HP_Agent):Word; stdcall; external HPSocketDLL;
//* 获取 Socket 缓存池回收阀值 */
function HP_Agent_GetFreeSocketObjHold(pAgent: HP_Agent):Word; stdcall; external HPSocketDLL;
//* 获取内存块缓存池回收阀值 */
function HP_Agent_GetFreeBufferObjHold(pAgent: HP_Agent):Word; stdcall; external HPSocketDLL;
//* 获取最大连接数 */
function HP_Agent_GetMaxConnectionCount(pAgent: HP_Agent):Word; stdcall; external HPSocketDLL;
//* 获取工作线程数量 */
function HP_Agent_GetWorkerThreadCount(pAgent: HP_Agent):Word; stdcall; external HPSocketDLL;
//* 检测是否标记静默时间 */
function HP_Agent_IsMarkSilence(pAgent: HP_Agent):BOOL; stdcall; external HPSocketDLL;

//**********************************************************************************/
//******************************* TCP Agent 操作方法 *******************************/

//*
{* 名称：发送小文件
* 描述：向指定连接发送 4096 KB 以下的小文件
*		
* 参数：		dwConnID		-- 连接 ID
*			lpszFileName	-- 文件路径
*			pHead			-- 头部附加数据
*			pTail			-- 尾部附加数据
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
*/}
function HP_TcpAgent_SendSmallFile(pAgent: HP_Agent; dwConnID: HP_CONNID; lpszFileName: PChar; const pHead: LPWSABUF; const pTail: LPWSABUF):BOOL; stdcall; external HPSocketDLL;

//**********************************************************************************/
//***************************** TCP Agent 属性访问方法 *****************************/

//* 设置是否启用地址重用机制（默认：不启用） */
procedure HP_TcpAgent_SetReuseAddress(pAgent: HP_TcpAgent; bReuseAddress: BOOL); stdcall; external HPSocketDLL;
//* 检测是否启用地址重用机制 */
function HP_TcpAgent_IsReuseAddress(pAgent: HP_TcpAgent):BOOL; stdcall; external HPSocketDLL;

//* 设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为 1024 的倍数） */
procedure HP_TcpAgent_SetSocketBufferSize(pAgent: HP_TcpAgent; dwSocketBufferSize: Word); stdcall; external HPSocketDLL;
//* 设置正常心跳包间隔（毫秒，0 则不发送心跳包，默认：30 * 1000） */
procedure HP_TcpAgent_SetKeepAliveTime(pAgent: HP_TcpAgent; dwKeepAliveTime: Word); stdcall; external HPSocketDLL;
//* 设置异常心跳包间隔（毫秒，0 不发送心跳包，，默认：10 * 1000，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线） */
procedure HP_TcpAgent_SetKeepAliveInterval(pAgent: HP_TcpAgent; dwKeepAliveInterval: Word); stdcall; external HPSocketDLL;

//* 获取通信数据缓冲区大小 */
function HP_TcpAgent_GetSocketBufferSize(pAgent: HP_TcpAgent):Word; stdcall; external HPSocketDLL;
//* 获取正常心跳包间隔 */
function HP_TcpAgent_GetKeepAliveTime(pAgent: HP_TcpAgent):Word; stdcall; external HPSocketDLL;
//* 获取异常心跳包间隔 */
function HP_TcpAgent_GetKeepAliveInterval(pAgent: HP_TcpAgent):Word; stdcall; external HPSocketDLL;

//******************************************************************************/
//***************************** Client 组件操作方法 *****************************/

//*
{* 名称：启动通信组件
* 描述：启动客户端通信组件并连接服务端，启动完成后可开始收发数据
*		
* 参数：		lpszRemoteAddress	-- 服务端地址
*			usPort				-- 服务端端口
*			bAsyncConnect		-- 是否采用异步 Connect
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 HP_Client_GetLastError() 获取错误代码
*/}
function HP_Client_Start(pClient: HP_Client; lpszRemoteAddress: PChar; usPort: Word; bAsyncConnect: BOOL):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：启动通信组件（并指定绑定地址）
* 描述：启动客户端通信组件并连接服务端，启动完成后可开始收发数据
*		
* 参数：		lpszRemoteAddress	-- 服务端地址
*			usPort				-- 服务端端口
*			bAsyncConnect		-- 是否采用异步 Connect
*			lpszBindAddress		-- 绑定地址（默认：nullptr，TcpClient/UdpClient -> 不执行绑定操作，UdpCast 绑定 -> 0.0.0.0）
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 HP_Client_GetLastError() 获取错误代码
*/}
function HP_Client_StartWithBindAddress(pClient: HP_Client; lpszRemoteAddress: PChar; usPort: Word; bAsyncConnect: BOOL; lpszBindAddress: PChar):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：关闭通信组件
* 描述：关闭客户端通信组件，关闭完成后断开与服务端的连接并释放所有资源
*		
* 参数：	
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 HP_Client_GetLastError() 获取错误代码
*/}
function HP_Client_Stop(pClient: HP_Client):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：发送数据
* 描述：向服务端发送数据
*		
* 参数：		pBuffer		-- 发送缓冲区
*			iLength		-- 发送缓冲区长度
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
*/}
function HP_Client_Send(pClient: HP_Client; const pBuffer: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：发送数据
* 描述：向服务端发送数据
*		
* 参数：		pBuffer		-- 发送缓冲区
*			iLength		-- 发送缓冲区长度
*			iOffset		-- 发送缓冲区指针偏移量
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
*/ }
function HP_Client_SendPart(pClient: HP_Client; const pBuffer: Pointer; iLength: Integer; iOffset: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：发送多组数据
* 描述：向服务端发送多组数据
*		TCP - 顺序发送所有数据包 
*		UDP - 把所有数据包组合成一个数据包发送（数据包的总长度不能大于设置的 UDP 包最大长度） 
*		
* 参数：		pBuffers	-- 发送缓冲区数组
*			iCount		-- 发送缓冲区数目
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
*/}
function HP_Client_SendPackets(pClient: HP_Client; const pBuffers: WSABUFArray; iCount: Integer):BOOL; stdcall; external HPSocketDLL;

//******************************************************************************/
//***************************** Client 属性访问方法 *****************************/

//* 设置连接的附加数据 */
procedure HP_Client_SetExtra(pClient: HP_Client; pExtra: Pointer); stdcall; external HPSocketDLL;
//* 获取连接的附加数据 */
function HP_Client_GetExtra(pClient: HP_Client): Pointer ; stdcall; external HPSocketDLL;

//* 检查通信组件是否已启动 */
function HP_Client_HasStarted(pClient: HP_Client):BOOL; stdcall; external HPSocketDLL;
//* 查看通信组件当前状态 */
function HP_Client_GetState(pClient: HP_Client):En_HP_ServiceState; stdcall; external HPSocketDLL;
//* 获取最近一次失败操作的错误代码 */
function HP_Client_GetLastError(pClient: HP_Client):En_HP_SocketError; stdcall; external HPSocketDLL;
//* 获取最近一次失败操作的错误描述 */
function HP_Client_GetLastErrorDesc(pClient: HP_Client):PChar; stdcall; external HPSocketDLL;
//* 获取该组件对象的连接 ID */
function HP_Client_GetConnectionID(pClient: HP_Client):HP_CONNID; stdcall; external HPSocketDLL;
//* 获取 Client Socket 的地址信息 */
function HP_Client_GetLocalAddress(pClient: HP_Client; lpszAddress: PChar;var piAddressLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;
//* 获取连接的远程主机信息 */
function HP_Client_GetRemoteHost(pClient: HP_Client; lpszHost: PChar;var piHostLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;
//* 获取连接中未发出数据的长度 */
function HP_Client_GetPendingDataLength(pClient: HP_Client;var piPending: Integer):BOOL; stdcall; external HPSocketDLL;
//* 设置内存块缓存池大小（通常设置为 -> PUSH 模型：5 - 10；PULL 模型：10 - 20 ） */
procedure HP_Client_SetFreeBufferPoolSize(pClient: HP_Client; dwFreeBufferPoolSize: Word); stdcall; external HPSocketDLL;
//* 设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍） */
procedure HP_Client_SetFreeBufferPoolHold(pClient: HP_Client; dwFreeBufferPoolHold: Word); stdcall; external HPSocketDLL;
//* 获取内存块缓存池大小 */
function HP_Client_GetFreeBufferPoolSize(pClient: HP_Client):Word; stdcall; external HPSocketDLL;
//* 获取内存块缓存池回收阀值 */
function HP_Client_GetFreeBufferPoolHold(pClient: HP_Client):Word; stdcall; external HPSocketDLL;

//**********************************************************************************/
//******************************* TCP Client 操作方法 *******************************/

//*
{* 名称：发送小文件
* 描述：向服务端发送 4096 KB 以下的小文件
*		
* 参数：		lpszFileName	-- 文件路径
*			pHead			-- 头部附加数据
*			pTail			-- 尾部附加数据
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
*/}
function HP_TcpClient_SendSmallFile(pClient: HP_Client; lpszFileName: PChar; const pHead: LPWSABUF; const pTail: LPWSABUF):BOOL; stdcall; external HPSocketDLL;

//**********************************************************************************/
//***************************** TCP Client 属性访问方法 *****************************/

//* 设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为：(N * 1024) - sizeof(TBufferObj)） */
procedure HP_TcpClient_SetSocketBufferSize(pClient: HP_TcpClient; dwSocketBufferSize: Word); stdcall; external HPSocketDLL;
//* 设置正常心跳包间隔（毫秒，0 则不发送心跳包，默认：30 * 1000） */
procedure HP_TcpClient_SetKeepAliveTime(pClient: HP_TcpClient; dwKeepAliveTime: Word); stdcall; external HPSocketDLL;
//* 设置异常心跳包间隔（毫秒，0 不发送心跳包，，默认：10 * 1000，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线） */
procedure HP_TcpClient_SetKeepAliveInterval(pClient: HP_TcpClient; dwKeepAliveInterval: Word); stdcall; external HPSocketDLL;

//* 获取通信数据缓冲区大小 */
function HP_TcpClient_GetSocketBufferSize(pClient: HP_TcpClient):Word; stdcall; external HPSocketDLL;
//* 获取正常心跳包间隔 */
function HP_TcpClient_GetKeepAliveTime(pClient: HP_TcpClient):Word; stdcall; external HPSocketDLL;
//* 获取异常心跳包间隔 */
function HP_TcpClient_GetKeepAliveInterval(pClient: HP_TcpClient):Word; stdcall; external HPSocketDLL;

//**********************************************************************************/
//***************************** UDP Client 属性访问方法 *****************************/

//* 设置数据报文最大长度（建议在局域网环境下不超过 1472 字节，在广域网环境下不超过 548 字节） */
procedure HP_UdpClient_SetMaxDatagramSize(pClient: HP_UdpClient; dwMaxDatagramSize: Word); stdcall; external HPSocketDLL;
//* 获取数据报文最大长度 */
function HP_UdpClient_GetMaxDatagramSize(pClient: HP_UdpClient):Word; stdcall; external HPSocketDLL;

//* 设置监测包尝试次数（0 则不发送监测跳包，如果超过最大尝试次数则认为已断线） */
procedure HP_UdpClient_SetDetectAttempts(pClient: HP_UdpClient; dwDetectAttempts: Word); stdcall; external HPSocketDLL;
//* 设置监测包发送间隔（秒，0 不发送监测包） */
procedure HP_UdpClient_SetDetectInterval(pClient: HP_UdpClient; dwDetectInterval: Word); stdcall; external HPSocketDLL;
//* 获取心跳检查次数 */
function HP_UdpClient_GetDetectAttempts(pClient: HP_UdpClient):Word; stdcall; external HPSocketDLL;
//* 获取心跳检查间隔 */
function HP_UdpClient_GetDetectInterval(pClient: HP_UdpClient):Word; stdcall; external HPSocketDLL;

//**********************************************************************************/
//****************************** UDP Cast 属性访问方法 ******************************/

//* 设置数据报文最大长度（建议在局域网环境下不超过 1472 字节，在广域网环境下不超过 548 字节） */
procedure HP_UdpCast_SetMaxDatagramSize(pCast: HP_UdpCast; dwMaxDatagramSize: Word); stdcall; external HPSocketDLL;
//* 获取数据报文最大长度 */
function HP_UdpCast_GetMaxDatagramSize(pCast: HP_UdpCast):Word; stdcall; external HPSocketDLL;
//* 获取当前数据报的远程地址信息（通常在 OnReceive 事件中调用） */
function HP_UdpCast_GetRemoteAddress(pCast: HP_UdpCast; lpszAddress:array of PChar; piAddressLen: Pointer; pusPort: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 设置是否启用地址重用机制（默认：不启用） */
procedure HP_UdpCast_SetReuseAddress(pCast: HP_UdpCast; bReuseAddress: BOOL); stdcall; external HPSocketDLL;
//* 检测是否启用地址重用机制 */
function HP_UdpCast_IsReuseAddress(pCast: HP_UdpCast):BOOL; stdcall; external HPSocketDLL;
//* 设置传播模式（组播或广播） */
procedure HP_UdpCast_SetCastMode(pCast: HP_UdpCast; enCastMode: En_HP_CastMode); stdcall; external HPSocketDLL;
//* 获取传播模式 */
function HP_UdpCast_GetCastMode(pCast: HP_UdpCast):En_HP_CastMode; stdcall; external HPSocketDLL;
//* 设置组播报文的 TTL（0 - 255） */
procedure HP_UdpCast_SetMultiCastTtl(pCast: HP_UdpCast; iMCTtl: Integer); stdcall; external HPSocketDLL;
//* 获取组播报文的 TTL */
function HP_UdpCast_GetMultiCastTtl(pCast: HP_UdpCast):Integer; stdcall; external HPSocketDLL;
//* 设置是否启用组播环路（TRUE or FALSE） */
procedure HP_UdpCast_SetMultiCastLoop(pCast: HP_UdpCast; bMCLoop: BOOL); stdcall; external HPSocketDLL;
//* 检测是否启用组播环路 */
function HP_UdpCast_IsMultiCastLoop(pCast: HP_UdpCast):BOOL; stdcall; external HPSocketDLL;

//***************************************************************************************/
//***************************** TCP Pull Server 组件操作方法 *****************************/

//*
{* 名称：抓取数据
* 描述：用户通过该方法从 Socket 组件中抓取数据
*		
* 参数：		dwConnID	-- 连接 ID
*			pData		-- 抓取缓冲区
*			iLength		-- 抓取数据长度
* 返回值：	En_HP_FetchResult
*/}
function HP_TcpPullServer_Fetch(pServer: HP_TcpPullServer; dwConnID: HP_CONNID; pData: Pointer; iLength: Integer):En_HP_FetchResult; stdcall; external HPSocketDLL;

{//*
* 名称：窥探数据（不会移除缓冲区数据）
* 描述：用户通过该方法从 Socket 组件中窥探数据
*		
* 参数：		dwConnID	-- 连接 ID
*			pData		-- 窥探缓冲区
*			iLength		-- 窥探数据长度
* 返回值：	En_HP_FetchResult
*/}
function HP_TcpPullServer_Peek(pServer: HP_TcpPullServer; dwConnID: HP_CONNID; pData: Pointer; iLength: Integer):En_HP_FetchResult; stdcall; external HPSocketDLL;

//***************************************************************************************/
//***************************** TCP Pull Server 属性访问方法 *****************************/

//***************************************************************************************/
//***************************** TCP Pull Agent 组件操作方法 *****************************/

{//*
* 名称：抓取数据
* 描述：用户通过该方法从 Socket 组件中抓取数据
*		
* 参数：		dwConnID	-- 连接 ID
*			pData		-- 抓取缓冲区
*			iLength		-- 抓取数据长度
* 返回值：	En_HP_FetchResult
*/}
function HP_TcpPullAgent_Fetch(pAgent: HP_TcpPullAgent; dwConnID: HP_CONNID; pData: Pointer; iLength: Integer):En_HP_FetchResult; stdcall; external HPSocketDLL;

{//*
* 名称：窥探数据（不会移除缓冲区数据）
* 描述：用户通过该方法从 Socket 组件中窥探数据
*		
* 参数：		dwConnID	-- 连接 ID
*			pData		-- 窥探缓冲区
*			iLength		-- 窥探数据长度
* 返回值：	En_HP_FetchResult
*/}
function HP_TcpPullAgent_Peek(pAgent: HP_TcpPullAgent; dwConnID: HP_CONNID; pData: Pointer; iLength: Integer):En_HP_FetchResult; stdcall; external HPSocketDLL;

//***************************************************************************************/
//***************************** TCP Pull Agent 属性访问方法 *****************************/

//***************************************************************************************/
//***************************** TCP Pull Client 组件操作方法 *****************************/

{//*
* 名称：抓取数据
* 描述：用户通过该方法从 Socket 组件中抓取数据
*		
* 参数：		dwConnID	-- 连接 ID
*			pData		-- 抓取缓冲区
*			iLength		-- 抓取数据长度
* 返回值：	En_HP_FetchResult
*/}
function HP_TcpPullClient_Fetch(pClient: HP_TcpPullClient; pData: Pointer; iLength: Integer):En_HP_FetchResult; stdcall; external HPSocketDLL;

{//*
* 名称：窥探数据（不会移除缓冲区数据）
* 描述：用户通过该方法从 Socket 组件中窥探数据
*		
* 参数：		dwConnID	-- 连接 ID
*			pData		-- 窥探缓冲区
*			iLength		-- 窥探数据长度
* 返回值：	En_HP_FetchResult
*/}
function HP_TcpPullClient_Peek(pClient: HP_TcpPullClient; pData: Pointer; iLength: Integer):En_HP_FetchResult; stdcall; external HPSocketDLL;

//***************************************************************************************/
//***************************** TCP Pull Client 属性访问方法 *****************************/

//***************************************************************************************/
//***************************** TCP Pack Server 组件操作方法 *****************************/

//***************************************************************************************/
//***************************** TCP Pack Server 属性访问方法 *****************************/

//* 设置数据包最大长度（有效数据包最大长度不能超过 4194303/0x3FFFFF 字节，默认：262144/0x40000） */
procedure HP_TcpPackServer_SetMaxPackSize(pServer: HP_TcpPackServer; dwMaxPackSize: Word); stdcall; external HPSocketDLL;
//* 设置包头标识（有效包头标识取值范围 0 ~ 1023/0x3FF，当包头标识为 0 时不校验包头，默认：0） */
procedure HP_TcpPackServer_SetPackHeaderFlag(pServer: HP_TcpPackServer; usPackHeaderFlag: Word); stdcall; external HPSocketDLL;

//* 获取数据包最大长度 */
function HP_TcpPackServer_GetMaxPackSize(pServer: HP_TcpPackServer):Word; stdcall; external HPSocketDLL;
//* 获取包头标识 */
function HP_TcpPackServer_GetPackHeaderFlag(pServer: HP_TcpPackServer):Word; stdcall; external HPSocketDLL;

//***************************************************************************************/
//***************************** TCP Pack Agent 组件操作方法 *****************************/

//***************************************************************************************/
//***************************** TCP Pack Agent 属性访问方法 *****************************/

//* 设置数据包最大长度（有效数据包最大长度不能超过 4194303/0x3FFFFF 字节，默认：262144/0x40000） */
procedure HP_TcpPackAgent_SetMaxPackSize(pAgent: HP_TcpPackAgent; dwMaxPackSize: Word); stdcall; external HPSocketDLL;
//* 设置包头标识（有效包头标识取值范围 0 ~ 1023/0x3FF，当包头标识为 0 时不校验包头，默认：0） */
procedure HP_TcpPackAgent_SetPackHeaderFlag(pAgent: HP_TcpPackAgent; usPackHeaderFlag: Word); stdcall; external HPSocketDLL;

//* 获取数据包最大长度 */
function HP_TcpPackAgent_GetMaxPackSize(pAgent: HP_TcpPackAgent):Word; stdcall; external HPSocketDLL;
//* 获取包头标识 */
function HP_TcpPackAgent_GetPackHeaderFlag(pAgent: HP_TcpPackAgent):Word; stdcall; external HPSocketDLL;

//***************************************************************************************/
//***************************** TCP Pack Client 组件操作方法 *****************************/

//***************************************************************************************/
//***************************** TCP Pack Client 属性访问方法 *****************************/

//* 设置数据包最大长度（有效数据包最大长度不能超过 4194303/0x3FFFFF 字节，默认：262144/0x40000） */
procedure HP_TcpPackClient_SetMaxPackSize(pClient: HP_TcpPackClient; dwMaxPackSize: Word); stdcall; external HPSocketDLL;
//* 设置包头标识（有效包头标识取值范围 0 ~ 1023/0x3FF，当包头标识为 0 时不校验包头，默认：0） */
procedure HP_TcpPackClient_SetPackHeaderFlag(pClient: HP_TcpPackClient; usPackHeaderFlag: Word); stdcall; external HPSocketDLL;

//* 获取数据包最大长度 */
function HP_TcpPackClient_GetMaxPackSize(pClient: HP_TcpPackClient):Word; stdcall; external HPSocketDLL;
//* 获取包头标识 */
function HP_TcpPackClient_GetPackHeaderFlag(pClient: HP_TcpPackClient):Word; stdcall; external HPSocketDLL;

//*****************************************************************************************************************************************************/
//******************************************************************** HTTP Exports *******************************************************************/
//*****************************************************************************************************************************************************/

//****************************************************/
//******************* HTTP 回调函数 *******************/

type
  {/* HTTP Server 回调函数 */ }
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

  {/* HTTP Agent 回调函数 */ }
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

  {/* HTTP Client 回调函数 */ }
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
//***************** HTTP 对象创建函数 *****************/

// 创建 HP_HttpServer 对象
function Create_HP_HttpServer(pListener: HP_HttpServerListener):HP_HttpServer; stdcall; external HPSocketDLL;
// 创建 HP_HttpAgent 对象
function Create_HP_HttpAgent(pListener: HP_HttpAgentListener):HP_HttpAgent; stdcall; external HPSocketDLL;
// 创建 HP_HttpClient 对象
function Create_HP_HttpClient(pListener: HP_HttpClientListener):HP_HttpClient; stdcall; external HPSocketDLL;
// 创建 HP_HttpClient 对象
function Create_HP_HttpSyncClient(pListener: HP_HttpClientListener):HP_HttpSyncClient; stdcall; external HPSocketDLL;

// 销毁 HP_HttpServer 对象
procedure Destroy_HP_HttpServer(pServer: HP_HttpServer); stdcall; external HPSocketDLL;
// 销毁 HP_HttpAgent 对象
procedure Destroy_HP_HttpAgent(pAgent: HP_HttpAgent); stdcall; external HPSocketDLL;
// 销毁 HP_HttpClient 对象
procedure Destroy_HP_HttpClient(pClient: HP_HttpClient); stdcall; external HPSocketDLL;
// 销毁 HP_HttpSyncClient 对象
procedure Destroy_HP_HttpSyncClient(pClient: HP_HttpSyncClient); stdcall; external HPSocketDLL;

// 创建 HP_HttpServerListener 对象
function Create_HP_HttpServerListener():HP_HttpServerListener; stdcall; external HPSocketDLL;
// 创建 HP_HttpAgentListener 对象
function Create_HP_HttpAgentListener():HP_HttpAgentListener; stdcall; external HPSocketDLL;
// 创建 HP_HttpClientListener 对象
function Create_HP_HttpClientListener():HP_HttpClientListener; stdcall; external HPSocketDLL;

// 销毁 HP_HttpServerListener 对象
procedure Destroy_HP_HttpServerListener(pListener: HP_HttpServerListener); stdcall; external HPSocketDLL;
// 销毁 HP_HttpAgentListener 对象
procedure Destroy_HP_HttpAgentListener(pListener: HP_HttpAgentListener); stdcall; external HPSocketDLL;
// 销毁 HP_HttpClientListener 对象
procedure Destroy_HP_HttpClientListener(pListener: HP_HttpClientListener); stdcall; external HPSocketDLL;

//**********************************************************************************/
//*************************** HTTP Server 回调函数设置方法 **************************/

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
//**************************** HTTP Agent 回调函数设置方法 **************************/

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
//*************************** HTTP Client 回调函数设置方法 **************************/

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
//***************************** Server 操作方法 *****************************/

//*
{* 名称：回复请求
* 描述：向客户端回复 HTTP 请求
*		
* 参数：		dwConnID		-- 连接 ID
*			usStatusCode	-- HTTP 状态码
*			lpszDesc		-- HTTP 状态描述
*			lpHeaders		-- 回复请求头
*			iHeaderCount	-- 回复请求头数量
*			pData			-- 回复请求体
*			iLength			-- 回复请求体长度
* 返回值：	TRUE			-- 成功
*			FALSE			-- 失败
*/}
function HP_HttpServer_SendResponse(pServer: HP_HttpServer; dwConnID: HP_CONNID; usStatusCode: Word; lpszDesc: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pData: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：发送本地文件
* 描述：向指定连接发送 4096 KB 以下的小文件
*		
* 参数：		dwConnID		-- 连接 ID
*			lpszFileName	-- 文件路径
*			usStatusCode	-- HTTP 状态码
*			lpszDesc		-- HTTP 状态描述
*			lpHeaders		-- 回复请求头
*			iHeaderCount	-- 回复请求头数量
* 返回值：	TRUE			-- 成功
*			FALSE			-- 失败
*/ }
function HP_HttpServer_SendLocalFile(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszFileName: PChar; usStatusCode: Word; lpszDesc: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
{/*
* 名称：发送 WebSocket 消息
* 描述：向对端端发送 WebSocket 消息
*		
* 参数：		dwConnID		-- 连接 ID
*			bFinal			-- 是否结束帧
*			iReserved		-- RSV1/RSV2/RSV3 各 1 位
*			iOperationCode	-- 操作码：0x0 - 0xF
*			lpszMask		-- 掩码（nullptr 或 4 字节掩码，如果为 nullptr 则没有掩码）
*			pData			-- 消息体数据缓冲区
*			iLength			-- 消息体数据长度
*			ullBodyLen		-- 消息总长度
* 								ullBodyLen = 0		 -> 消息总长度为 iLength
* 								ullBodyLen = iLength -> 消息总长度为 ullBodyLen
* 								ullBodyLen > iLength -> 消息总长度为 ullBodyLen，后续消息体长度为 ullBOdyLen - iLength，后续消息体通过底层方法 Send() / SendPackets() 发送
* 								ullBodyLen < iLength -> 错误参数，发送失败
* 返回值：	TRUE			-- 成功
*			FALSE			-- 失败
*/}
function HP_HttpServer_SendWSMessage(pServer: HP_HttpServer; dwConnID: HP_CONNID; bFinal: BOOL; iReserved: PChar; iOperationCode: PChar; const lpszMask: PChar; pData: PChar; iLength: Integer; ullBodyLen: LongWord):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：释放连接
* 描述：把连接放入释放队列，等待某个时间（通过 SetReleaseDelay() 设置）关闭连接
*		
* 参数：		dwConnID		-- 连接 ID
* 返回值：	TRUE			-- 成功
*			FALSE			-- 失败
*/}
function HP_HttpServer_Release(pServer: HP_HttpServer; dwConnID: HP_CONNID):BOOL; stdcall; external HPSocketDLL;

//******************************************************************************/
//***************************** Server 属性访问方法 *****************************/

//* 设置连接释放延时（默认：3000 毫秒） */
procedure HP_HttpServer_SetReleaseDelay(pServer: HP_HttpServer; dwReleaseDelay: Word); stdcall; external HPSocketDLL;
//* 获取连接释放延时 */
function HP_HttpServer_GetReleaseDelay(pServer: HP_HttpServer):Word; stdcall; external HPSocketDLL;
//* 获取请求行 URL 域掩码（URL 域参考：EnHttpUrlField） */
function HP_HttpServer_GetUrlFieldSet(pServer: HP_HttpServer; dwConnID: HP_CONNID):Word; stdcall; external HPSocketDLL;
//* 获取某个 URL 域值 */
function HP_HttpServer_GetUrlField(pServer: HP_HttpServer; dwConnID: HP_CONNID; enField: En_HP_HttpUrlField):PChar; stdcall; external HPSocketDLL;
//* 获取请求方法 */
function HP_HttpServer_GetMethod(pServer: HP_HttpServer; dwConnID: HP_CONNID):PChar; stdcall; external HPSocketDLL;

//* 设置本地协议版本 */
procedure HP_HttpServer_SetLocalVersion(pServer: HP_HttpServer; usVersion: En_HP_HttpVersion); stdcall; external HPSocketDLL;
//* 获取本地协议版本 */
function HP_HttpServer_GetLocalVersion(pServer: HP_HttpServer):En_HP_HttpVersion; stdcall; external HPSocketDLL;

//* 检查是否升级协议 */
function HP_HttpServer_IsUpgrade(pServer: HP_HttpServer; dwConnID: HP_CONNID):BOOL; stdcall; external HPSocketDLL;
//* 检查是否有 Keep-Alive 标识 */
function HP_HttpServer_IsKeepAlive(pServer: HP_HttpServer; dwConnID: HP_CONNID):BOOL; stdcall; external HPSocketDLL;
//* 获取协议版本 */
function HP_HttpServer_GetVersion(pServer: HP_HttpServer; dwConnID: HP_CONNID):Word; stdcall; external HPSocketDLL;
//* 获取主机 */
function HP_HttpServer_GetHost(pServer: HP_HttpServer; dwConnID: HP_CONNID):PChar; stdcall; external HPSocketDLL;
//* 获取内容长度 */
function HP_HttpServer_GetContentLength(pServer: HP_HttpServer; dwConnID: HP_CONNID): Longint ; stdcall; external HPSocketDLL;
//* 获取内容类型 */
function HP_HttpServer_GetContentType(pServer: HP_HttpServer; dwConnID: HP_CONNID):PChar; stdcall; external HPSocketDLL;
//* 获取内容编码 */
function HP_HttpServer_GetContentEncoding(pServer: HP_HttpServer; dwConnID: HP_CONNID):PChar; stdcall; external HPSocketDLL;
//* 获取传输编码 */
function HP_HttpServer_GetTransferEncoding(pServer: HP_HttpServer; dwConnID: HP_CONNID):PChar; stdcall; external HPSocketDLL;
//* 获取协议升级类型 */
function HP_HttpServer_GetUpgradeType(pServer: HP_HttpServer; dwConnID: HP_CONNID):En_HP_HttpUpgradeType; stdcall; external HPSocketDLL;
//* 获取解析错误代码 */
function HP_HttpServer_GetParseErrorCode(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszErrorDesc: PChar):Word; stdcall; external HPSocketDLL;

//* 获取某个请求头（单值） */
function HP_HttpServer_GetHeader(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszName: PChar; lpszValue: PChar):BOOL; stdcall; external HPSocketDLL;
//* 获取某个请求头（多值） */
function HP_HttpServer_GetHeaders(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszName: PChar; lpszValue:array of PChar; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 获取所有请求头 */
function HP_HttpServer_GetAllHeaders(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpHeaders: TNVPair; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 获取所有请求头名称 */
function HP_HttpServer_GetAllHeaderNames(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszName:array of PChar; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;

//* 获取 Cookie */
function HP_HttpServer_GetCookie(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszName: PChar; lpszValue: PChar):BOOL; stdcall; external HPSocketDLL;
//* 获取所有 Cookie */
function HP_HttpServer_GetAllCookies(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpCookies: TNVPair; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 获取当前 WebSocket 消息状态，传入 nullptr 则不获取相应字段 */
function HP_HttpServer_GetWSMessageState(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpbFinal: BOOL; lpiReserved: PChar; lpiOperationCode: PChar; lpszMask: PChar; lpullBodyLen: LongWord; lpullBodyRemain: LongWord):BOOL; stdcall; external HPSocketDLL;

//**************************************************************************/
//***************************** Agent 操作方法 ******************************/

//*
{* 名称：发送请求
* 描述：向服务端发送 HTTP 请求
*		
* 参数：		dwConnID		-- 连接 ID
*			lpszMethod		-- 请求方法
*			lpszPath		-- 请求路径
*			lpHeaders		-- 请求头
*			iHeaderCount	-- 请求头数量
*			pBody			-- 请求体
*			iLength			-- 请求体长度
* 返回值：	TRUE			-- 成功
*			FALSE			-- 失败
*/}
function HP_HttpAgent_SendRequest(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszMethod: PChar; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pData: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：发送本地文件
* 描述：向指定连接发送 4096 KB 以下的小文件
*		
* 参数：		dwConnID		-- 连接 ID
*			lpszFileName	-- 文件路径
*			lpszMethod		-- 请求方法
*			lpszPath		-- 请求路径
*			lpHeaders		-- 请求头
*			iHeaderCount	-- 请求头数量
* 返回值：	TRUE			-- 成功
*			FALSE			-- 失败
*/}
function HP_HttpAgent_SendLocalFile(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszFileName: PChar; lpszMethod: PChar; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;

//* 发送 POST 请求 */
function HP_HttpAgent_SendPost(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pBody: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;
//* 发送 PUT 请求 */
function HP_HttpAgent_SendPut(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pBody: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;
//* 发送 PATCH 请求 */
function HP_HttpAgent_SendPatch(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pBody: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;
//* 发送 GET 请求 */
function HP_HttpAgent_SendGet(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* 发送 DELETE 请求 */
function HP_HttpAgent_SendDelete(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* 发送 HEAD 请求 */
function HP_HttpAgent_SendHead(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* 发送 TRACE 请求 */
function HP_HttpAgent_SendTrace(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* 发送 OPTIONS 请求 */
function HP_HttpAgent_SendOptions(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* 发送 CONNECT 请求 */
function HP_HttpAgent_SendConnect(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszHost: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
{/*
* 名称：发送 WebSocket 消息
* 描述：向对端端发送 WebSocket 消息
*		
* 参数：		dwConnID		-- 连接 ID
*			bFinal			-- 是否结束帧
*			iReserved		-- RSV1/RSV2/RSV3 各 1 位
*			iOperationCode	-- 操作码：0x0 - 0xF
*			lpszMask		-- 掩码（nullptr 或 4 字节掩码，如果为 nullptr 则没有掩码）
*			pData			-- 消息体数据缓冲区
*			iLength			-- 消息体数据长度
*			ullBodyLen		-- 消息总长度
* 								ullBodyLen = 0		 -> 消息总长度为 iLength
* 								ullBodyLen = iLength -> 消息总长度为 ullBodyLen
* 								ullBodyLen > iLength -> 消息总长度为 ullBodyLen，后续消息体长度为 ullBOdyLen - iLength，后续消息体通过底层方法 Send() / SendPackets() 发送
* 								ullBodyLen < iLength -> 错误参数，发送失败
* 返回值：	TRUE			-- 成功
*			FALSE			-- 失败
*/}
function HP_HttpAgent_SendWSMessage(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; bFinal: BOOL; iReserved: PChar; iOperationCode: PChar; const lpszMask: Byte; pData: PChar; iLength: Integer; ullBodyLen: Longword):BOOL; stdcall; external HPSocketDLL;

//******************************************************************************/
//***************************** Agent 属性访问方法 ******************************/

//* 获取 HTTP 状态码 */
function HP_HttpAgent_GetStatusCode(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):Word; stdcall; external HPSocketDLL;

//* 设置本地协议版本 */
procedure HP_HttpAgent_SetLocalVersion(pAgent: HP_HttpAgent; usVersion: En_HP_HttpVersion); stdcall; external HPSocketDLL;
//* 获取本地协议版本 */
function HP_HttpAgent_GetLocalVersion(pAgent: HP_HttpAgent):En_HP_HttpVersion; stdcall; external HPSocketDLL;

//* 检查是否升级协议 */
function HP_HttpAgent_IsUpgrade(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):BOOL; stdcall; external HPSocketDLL;
//* 检查是否有 Keep-Alive 标识 */
function HP_HttpAgent_IsKeepAlive(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):BOOL; stdcall; external HPSocketDLL;
//* 获取协议版本 */
function HP_HttpAgent_GetVersion(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):Word; stdcall; external HPSocketDLL;
//* 获取内容长度 */
function HP_HttpAgent_GetContentLength(pAgent: HP_HttpAgent; dwConnID: HP_CONNID): Longint ; stdcall; external HPSocketDLL;
//* 获取内容类型 */
function HP_HttpAgent_GetContentType(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):PChar; stdcall; external HPSocketDLL;

//* 获取协议升级类型 */
function HP_HttpAgent_GetUpgradeType(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):En_HP_HttpUpgradeType; stdcall; external HPSocketDLL;
//* 获取解析错误代码 */
function HP_HttpAgent_GetParseErrorCode(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszErrorDesc: PChar):Word; stdcall; external HPSocketDLL;
//* 获取内容编码 */
function HP_HttpAgent_GetContentEncoding(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):PChar; stdcall; external HPSocketDLL;
//* 获取传输编码 */
function HP_HttpAgent_GetTransferEncoding(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):PChar; stdcall; external HPSocketDLL;
//* 获取某个请求头（单值） */
function HP_HttpAgent_GetHeader(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszName: PChar; lpszValue: PChar):BOOL; stdcall; external HPSocketDLL;
//* 获取某个请求头（多值） */
function HP_HttpAgent_GetHeaders(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszName: PChar; lpszValue:array of PChar; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 获取所有请求头 */
function HP_HttpAgent_GetAllHeaders(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpHeaders: TNVPair; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 获取所有请求头名称 */
function HP_HttpAgent_GetAllHeaderNames(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszName:array of PChar; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;

//* 获取 Cookie */
function HP_HttpAgent_GetCookie(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszName: PChar; lpszValue: PChar):BOOL; stdcall; external HPSocketDLL;
//* 获取所有 Cookie */
function HP_HttpAgent_GetAllCookies(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpCookies: TNVPair; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 添加 Cookie */
function HP_HttpAgent_AddCookie(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszName: PChar; lpszValue: PChar; bRelpace: BOOL):BOOL; stdcall; external HPSocketDLL;
//* 删除 Cookie */
function HP_HttpAgent_DeleteCookie(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszName: PChar):BOOL; stdcall; external HPSocketDLL;
//* 删除所有 Cookie */
function HP_HttpAgent_DeleteAllCookies(pAgent: HP_HttpAgent; dwConnID: HP_CONNID):BOOL; stdcall; external HPSocketDLL;
 //* 获取当前 WebSocket 消息状态，传入 nullptr 则不获取相应字段 */
function HP_HttpAgent_GetWSMessageState(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpbFinal: BOOL; lpiReserved: PChar; lpiOperationCode: PChar; lpszMask: PChar; lpullBodyLen: Longword; lpullBodyRemain: Longword):BOOL; stdcall; external HPSocketDLL;


//**************************************************************************/
//***************************** Client 操作方法 *****************************/

//*
{* 名称：发送请求
* 描述：向服务端发送 HTTP 请求
*		
* 参数：		lpszMethod		-- 请求方法
*			lpszPath		-- 请求路径
*			lpHeaders		-- 请求头
*			iHeaderCount	-- 请求头数量
*			pBody			-- 请求体
*			iLength			-- 请求体长度
* 返回值：	TRUE			-- 成功
*			FALSE			-- 失败
*/}
function HP_HttpClient_SendRequest(pClient: HP_HttpClient; lpszMethod: PChar; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pBody: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;

{//*
* 名称：发送本地文件
* 描述：向指定连接发送 4096 KB 以下的小文件
*		
* 参数：		dwConnID		-- 连接 ID
*			lpszFileName	-- 文件路径
*			lpszMethod		-- 请求方法
*			lpszPath		-- 请求路径
*			lpHeaders		-- 请求头
*			iHeaderCount	-- 请求头数量
* 返回值：	TRUE			-- 成功
*			FALSE			-- 失败
*/}
function HP_HttpClient_SendLocalFile(pClient: HP_HttpClient; lpszFileName: PChar; lpszMethod: PChar; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;

//* 发送 POST 请求 */
function HP_HttpClient_SendPost(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pBody: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;
//* 发送 PUT 请求 */
function HP_HttpClient_SendPut(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pBody: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;
//* 发送 PATCH 请求 */
function HP_HttpClient_SendPatch(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pBody: Pointer; iLength: Integer):BOOL; stdcall; external HPSocketDLL;
//* 发送 GET 请求 */
function HP_HttpClient_SendGet(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* 发送 DELETE 请求 */
function HP_HttpClient_SendDelete(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* 发送 HEAD 请求 */
function HP_HttpClient_SendHead(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* 发送 TRACE 请求 */
function HP_HttpClient_SendTrace(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* 发送 OPTIONS 请求 */
function HP_HttpClient_SendOptions(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* 发送 CONNECT 请求 */
function HP_HttpClient_SendConnect(pClient: HP_HttpClient; lpszHost: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer):BOOL; stdcall; external HPSocketDLL;
{/*
* 名称：发送 WebSocket 消息
* 描述：向对端端发送 WebSocket 消息
*		
* 参数：		bFinal			-- 是否结束帧
*			iReserved		-- RSV1/RSV2/RSV3 各 1 位
*			iOperationCode	-- 操作码：0x0 - 0xF
*			lpszMask		-- 掩码（nullptr 或 4 字节掩码，如果为 nullptr 则没有掩码）
*			pData			-- 消息体数据缓冲区
*			iLength			-- 消息体数据长度
*			ullBodyLen		-- 消息总长度
* 								ullBodyLen = 0		 -> 消息总长度为 iLength
* 								ullBodyLen = iLength -> 消息总长度为 ullBodyLen
* 								ullBodyLen > iLength -> 消息总长度为 ullBodyLen，后续消息体长度为 ullBOdyLen - iLength，后续消息体通过底层方法 Send() / SendPackets() 发送
* 								ullBodyLen < iLength -> 错误参数，发送失败
* 返回值：	TRUE			-- 成功
*			FALSE			-- 失败
*/}
function HP_HttpClient_SendWSMessage(pClient: HP_HttpClient; bFinal: BOOL; iReserved: PChar; iOperationCode: PChar; const lpszMask: Byte; pData: PChar; iLength: Integer; ullBodyLen: LongWord):BOOL; stdcall; external HPSocketDLL;

//******************************************************************************/
//***************************** Client 属性访问方法 *****************************/

//* 获取 HTTP 状态码 */
function HP_HttpClient_GetStatusCode(pClient: HP_HttpClient):Word; stdcall; external HPSocketDLL;

//* 设置本地协议版本 */
procedure HP_HttpClient_SetLocalVersion(pClient: HP_HttpClient; usVersion: En_HP_HttpVersion); stdcall; external HPSocketDLL;
//* 获取本地协议版本 */
function HP_HttpClient_GetLocalVersion(pClient: HP_HttpClient):En_HP_HttpVersion; stdcall; external HPSocketDLL;

//* 检查是否升级协议 */
function HP_HttpClient_IsUpgrade(pClient: HP_HttpClient):BOOL; stdcall; external HPSocketDLL;
//* 检查是否有 Keep-Alive 标识 */
function HP_HttpClient_IsKeepAlive(pClient: HP_HttpClient):BOOL; stdcall; external HPSocketDLL;
//* 获取协议版本 */
function HP_HttpClient_GetVersion(pClient: HP_HttpClient):Word; stdcall; external HPSocketDLL;
//* 获取内容长度 */
function HP_HttpClient_GetContentLength(pClient: HP_HttpClient): Longint ; stdcall; external HPSocketDLL;
//* 获取内容类型 */
function HP_HttpClient_GetContentType(pClient: HP_HttpClient):PChar; stdcall; external HPSocketDLL;
//* 获取内容编码 */
function HP_HttpClient_GetContentEncoding(pClient: HP_HttpClient):PChar; stdcall; external HPSocketDLL;
//* 获取传输编码 */
function HP_HttpClient_GetTransferEncoding(pClient: HP_HttpClient):PChar; stdcall; external HPSocketDLL;
//* 获取协议升级类型 */
function HP_HttpClient_GetUpgradeType(pClient: HP_HttpClient):En_HP_HttpUpgradeType; stdcall; external HPSocketDLL;
//* 获取解析错误代码 */
function HP_HttpClient_GetParseErrorCode(pClient: HP_HttpClient; lpszErrorDesc: PChar):Word; stdcall; external HPSocketDLL;

//* 获取某个请求头（单值） */
function HP_HttpClient_GetHeader(pClient: HP_HttpClient; lpszName: PChar; lpszValue: PChar):BOOL; stdcall; external HPSocketDLL;
//* 获取某个请求头（多值） */
function HP_HttpClient_GetHeaders(pClient: HP_HttpClient; lpszName: PChar; lpszValue:array of PChar; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 获取所有请求头 */
function HP_HttpClient_GetAllHeaders(pClient: HP_HttpClient; lpHeaders: TNVPair; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 获取所有请求头名称 */
function HP_HttpClient_GetAllHeaderNames(pClient: HP_HttpClient; lpszName:array of PChar; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;

//* 获取 Cookie */
function HP_HttpClient_GetCookie(pClient: HP_HttpClient; lpszName: PChar; lpszValue: PChar):BOOL; stdcall; external HPSocketDLL;
//* 获取所有 Cookie */
function HP_HttpClient_GetAllCookies(pClient: HP_HttpClient; lpCookies: TNVPair; pdwCount: Pointer):BOOL; stdcall; external HPSocketDLL;
//* 添加 Cookie */
function HP_HttpClient_AddCookie(pClient: HP_HttpClient; lpszName: PChar; lpszValue: PChar; bRelpace: BOOL):BOOL; stdcall; external HPSocketDLL;
//* 删除 Cookie */
function HP_HttpClient_DeleteCookie(pClient: HP_HttpClient; lpszName: PChar):BOOL; stdcall; external HPSocketDLL;
//* 删除所有 Cookie */
function HP_HttpClient_DeleteAllCookies(pClient: HP_HttpClient):BOOL; stdcall; external HPSocketDLL;
//* 获取当前 WebSocket 消息状态，传入 nullptr 则不获取相应字段 */
function HP_HttpClient_GetWSMessageState(pClient: HP_HttpClient; lpbFinal: BOOL; lpiReserved: PChar; lpiOperationCode: PChar; lpszMask: PChar; lpullBodyLen: LongWord; lpullBodyRemain: LongWord):BOOL; stdcall; external HPSocketDLL;

{/**************************************************************************/
/************************ HTTP Sync Client 操作方法 ************************/}
{/*
* 名称：发送 URL 请求
* 描述：向服务端发送 HTTP URL 请求
*		
* 参数：		lpszMethod		-- 请求方法
*			lpszUrl			-- 请求 URL
*			lpHeaders		-- 请求头
*			iHeaderCount	-- 请求头数量
*			pBody			-- 请求体
*			iLength			-- 请求体长度
*			bForceReconnect	-- 强制重新连接（默认：FALSE，当请求 URL 的主机和端口与现有连接一致时，重用现有连接）
* 返回值：	TRUE			-- 成功
*			FALSE			-- 失败
*/}
function HP_HttpSyncClient_OpenUrl(pClient: HP_HttpSyncClient; lpszMethod: PChar; lpszUrl: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer; const pBody: PChar; iLength: Integer; bForceReconnect: BOOL):BOOL; stdcall; external HPSocketDLL;
{/*
* 名称：清除请求结果
* 描述：清除上一次请求的响应头和响应体等结果信息（该方法会在每次发送请求前自动调用）
*
* 参数：		
* 返回值：	TRUE			-- 成功
*			FALSE			-- 失败
*/}
function HP_HttpSyncClient_CleanupRequestResult(pClient: HP_HttpSyncClient):BOOL; stdcall; external HPSocketDLL;

{/******************************************************************************/
/************************ HTTP Sync Client 属性访问方法 ************************/}

//* 设置连接超时（毫秒，0：系统默认超时，默认：5000） */
procedure HP_HttpSyncClient_SetConnectTimeout(pClient: HP_HttpSyncClient; dwConnectTimeout: LongWord); stdcall; external HPSocketDLL;
//* 设置请求超时（毫秒，0：无限等待，默认：10000） */                                                     
procedure HP_HttpSyncClient_SetRequestTimeout(pClient: HP_HttpSyncClient; dwRequestTimeout: LongWord); stdcall; external HPSocketDLL;

//* 获取连接超时 */
function HP_HttpSyncClient_GetConnectTimeout(pClient: HP_HttpSyncClient):LongWord; stdcall; external HPSocketDLL;
//* 获取请求超时 */
function HP_HttpSyncClient_GetRequestTimeout(pClient: HP_HttpSyncClient):LongWord; stdcall; external HPSocketDLL;

//* 获取响应体 */
function HP_HttpSyncClient_GetResponseBody(pClient: HP_HttpSyncClient; lpszBod: PChar; piLength: Pointer):BOOL; stdcall; external HPSocketDLL;

//*****************************************************************************************************************************************************/
//*************************************************************** Global Function Exports *************************************************************/
//*****************************************************************************************************************************************************/
// 获取 HPSocket 版本号（4 个字节分别为：主版本号，子版本号，修正版本号，构建编号）
function HP_GetHPSocketVersion():LongWord; stdcall; external HPSocketDLL;

//* 获取错误描述文本 */
function HP_GetSocketErrorDesc(enCode: En_HP_SocketError):PChar; stdcall; external HPSocketDLL;
//* 调用系统的 GetLastError() 方法获取系统错误代码 */
function SYS_GetLastError():Word; stdcall; external HPSocketDLL;
// 调用系统的 WSAGetLastError() 方法获取系统错误代码
function SYS_WSAGetLastError():Integer; stdcall; external HPSocketDLL;
// 调用系统的 setsockopt()
function SYS_SetSocketOption(sock: SOCKET; level: Integer; name: Integer; val: Pointer; len: Integer):Integer; stdcall; external HPSocketDLL;
// 调用系统的 getsockopt()
function SYS_GetSocketOption(sock: SOCKET; level: Integer; name: Integer; val: Pointer; len: Pointer):Integer; stdcall; external HPSocketDLL;
// 调用系统的 ioctlsocket()
function SYS_IoctlSocket(sock: SOCKET; cmd: Longint; arg: Longword):Integer; stdcall; external HPSocketDLL;
// 调用系统的 WSAIoctl()
function SYS_WSAIoctl(sock: SOCKET; dwIoControlCode: Word; lpvInBuffer: Pointer; cbInBuffer: Word; lpvOutBuffer: Pointer; cbOutBuffer: Word; lpcbBytesReturned: LPDWORD):Integer; stdcall; external HPSocketDLL;

// 设置 socket 选项：IPPROTO_TCP -> TCP_NODELAY
function SYS_SSO_NoDelay(sock: SOCKET; bNoDelay: BOOL):Integer; stdcall; external HPSocketDLL;
// 设置 socket 选项：SOL_SOCKET -> SO_DONTLINGER
function SYS_SSO_DontLinger(sock: SOCKET; bDont: BOOL):Integer; stdcall; external HPSocketDLL;
// 设置 socket 选项：SOL_SOCKET -> SO_LINGER
function SYS_SSO_Linger(sock: SOCKET; l_onoff: Word; l_linger: Word):Integer; stdcall; external HPSocketDLL;
// 设置 socket 选项：SOL_SOCKET -> SO_RCVBUF
function SYS_SSO_RecvBuffSize(sock: SOCKET; size: Integer):Integer; stdcall; external HPSocketDLL;
// 设置 socket 选项：SOL_SOCKET -> SO_SNDBUF
function SYS_SSO_SendBuffSize(sock: SOCKET; size: Integer):Integer; stdcall; external HPSocketDLL;
// 设置 socket 选项：SOL_SOCKET -> SO_REUSEADDR
function SYS_SSO_ReuseAddress(sock: SOCKET; bReuse: BOOL):Integer; stdcall; external HPSocketDLL;

// 获取 SOCKET 本地地址信息
function SYS_GetSocketLocalAddress(socket: SOCKET; lpszAddress: PChar;var piAddressLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;
// 获取 SOCKET 远程地址信息
function SYS_GetSocketRemoteAddress(socket: SOCKET; lpszAddress: PChar;var piAddressLen: Integer;var pusPort: Word):BOOL; stdcall; external HPSocketDLL;

//* 枚举主机 IP 地址 */
function SYS_EnumHostIPAddresses(lpszHost: PChar; enType: En_HP_IPAddrType;var lpppIPAddr : HP_TIPAddr;var piIPAddrCount: Integer):BOOL; stdcall; external HPSocketDLL;
//* 释放 HP_LPTIPAddr* */
function SYS_FreeHostIPAddresses(var lppIPAddr : HP_TIPAddr):BOOL; stdcall; external HPSocketDLL;

//* 检查字符串是否符合 IP 地址格式 */
function SYS_IsIPAddress(lpszAddress: PChar ;var penType: En_HP_IPAddrType):BOOL; stdcall; external HPSocketDLL;
//* 通过主机名获取 IP 地址 */
function SYS_GetIPAddress(lpszHost: PChar; lpszIP: PChar;var piIPLenth: Integer; var penType: En_HP_IPAddrType):BOOL; stdcall; external HPSocketDLL;


//* 64 位网络字节序转主机字节序 */
function SYS_NToH64(value: Longword):Longword; stdcall; external HPSocketDLL;
//* 64 位主机字节序转网络字节序 */
function SYS_HToN64(value: Longword):Longword; stdcall; external HPSocketDLL;

//===================================下面是编码函数
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

// 普通压缩
function SYS_Compress(const lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord):Integer; stdcall; external HPSocketDLL;
// 高级压缩（默认值：iLevel -> -1，iMethod -> 8，iWindowBits -> 15，iMemLevel -> 8，iStrategy -> 0）
function SYS_CompressEx(const lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord; iLevel: Integer; iMethod: Integer; iWindowBits: Integer; iMemLevel: Integer; iStrategy: Integer):Integer; stdcall; external HPSocketDLL;
// 普通解压
function SYS_Uncompress(const lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord):Integer; stdcall; external HPSocketDLL;
// 高级解压（默认值：iWindowBits -> 15）
function SYS_UncompressEx(const lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord; iWindowBits: Integer):Integer; stdcall; external HPSocketDLL;
// 推测压缩结果长度
function SYS_GuessCompressBound(dwSrcLen: LongWord; bGZip: BooL):LongWord; stdcall; external HPSocketDLL;

// Gzip 压缩
function SYS_GZipCompress(const lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord):Integer; stdcall; external HPSocketDLL;
// Gzip 解压
function SYS_GZipUncompress(const lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord):Integer; stdcall; external HPSocketDLL;
// 推测 Gzip 解压结果长度（如果返回 0 或不合理值则说明输入内容并非有效的 Gzip 格式）
function SYS_GZipGuessUncompressBound(const lpszSrc: PChar; dwSrcLen: LongWord):LongWord; stdcall; external HPSocketDLL;

// 计算 Base64 编码后长度
function SYS_GuessBase64EncodeBound(dwSrcLen: LongWord):LongWord; stdcall; external HPSocketDLL;
// 计算 Base64 解码后长度
function SYS_GuessBase64DecodeBound(const lpszSrc: PChar; dwSrcLen: LongWord):LongWord; stdcall; external HPSocketDLL;
// Base64 编码（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
function SYS_Base64Encode(const lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord):Integer; stdcall; external HPSocketDLL;
// Base64 解码（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
function SYS_Base64Decode(const lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord):Integer; stdcall; external HPSocketDLL;

// 计算 URL 编码后长度
function SYS_GuessUrlEncodeBound(const lpszSrc: PChar; dwSrcLen: LongWord):LongWord; stdcall; external HPSocketDLL;
// 计算 URL 解码后长度
function SYS_GuessUrlDecodeBound(const lpszSrc: PChar; dwSrcLen: LongWord):LongWord; stdcall; external HPSocketDLL;
// URL 编码（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
function SYS_UrlEncode(lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord):Integer; stdcall; external HPSocketDLL;
// URL 解码（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
function SYS_UrlDecode(lpszSrc: PChar; dwSrcLen: LongWord; lpszDest: PChar; pdwDestLen: LongWord):Integer; stdcall; external HPSocketDLL;



implementation


initialization
  IsMultiThread := True;
end.

