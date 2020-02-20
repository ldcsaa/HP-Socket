unit HPSocketSDKUnit;

interface

uses
  Winapi.Windows, HPTypeDef;

type
  SOCKET = Pointer;

  PWSABUF = ^WSABUF;

  WSABUF = record
    len: Longint; { the length of the buffer }
    buf: PChar; { the pointer to the buffer }
  end { WSABUF };

  LPWSABUF = PWSABUF;

  WSABUFArray = array of WSABUF;

  PUShort = ^Word;

  { /************************************************************************
    名称：定义 Socket 对象指针类型别名
    描述：把 Socket 对象指针定义为更直观的别名
    ************************************************************************/ }
  HP_Object = PVOID;

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

  HP_UdpArqServer = HP_Object;

  HP_UdpArqClient = HP_Object;

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

  HP_UdpArqServerListener = HP_Object;

  HP_UdpArqClientListener = HP_Object;

  HP_Http = HP_Object;

  HP_HttpServer = HP_Object;

  HP_HttpAgent = HP_Object;

  HP_HttpClient = HP_Object;

  HP_HttpSyncClient = HP_Object;

  HP_HttpServerListener = HP_Object;

  HP_HttpAgentListener = HP_Object;

  HP_HttpClientListener = HP_Object;

  HP_ThreadPool = HP_Object;

  // *****************************************************************************************************************************************************/
  // ****************************************************************** TCP/UDP Exports ******************************************************************/
  // *****************************************************************************************************************************************************/

  // ****************************************************/
  // ***************** TCP/UDP 回调函数 ******************/

  // * Server 回调函数 */
  HP_FN_Server_OnPrepareListen = function(pSender: HP_Server; soListen: UINT_PTR): En_HP_HandleResult; stdcall;

  // 如果为 TCP 连接，pClient为 SOCKET 句柄；如果为 UDP 连接，pClient为 SOCKADDR_IN 指针；
  HP_FN_Server_OnAccept = function(pSender: HP_Server; dwConnID: HP_CONNID; pClient: UINT_PTR): En_HP_HandleResult; stdcall;

  HP_FN_Server_OnHandShake = function(pSender: HP_Server; dwConnID: HP_CONNID): En_HP_HandleResult; stdcall;

  HP_FN_Server_OnSend = function(pSender: HP_Server; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer): En_HP_HandleResult; stdcall;

  HP_FN_Server_OnReceive = function(pSender: HP_Server; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer): En_HP_HandleResult; stdcall;

  HP_FN_Server_OnPullReceive = function(pSender: HP_Server; dwConnID: HP_CONNID; iLength: Integer): En_HP_HandleResult; stdcall;

  HP_FN_Server_OnClose = function(pSender: HP_Server; dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer): En_HP_HandleResult; stdcall;

  HP_FN_Server_OnShutdown = function(pSender: HP_Server): En_HP_HandleResult; stdcall;

  // * Agent 回调函数 */
  HP_FN_Agent_OnPrepareConnect = function(pSender: HP_Agent; dwConnID: HP_CONNID; SOCKET: UINT_PTR): En_HP_HandleResult; stdcall;

  HP_FN_Agent_OnConnect = function(pSender: HP_Agent; dwConnID: HP_CONNID): En_HP_HandleResult; stdcall;

  HP_FN_Agent_OnHandShake = function(pSender: HP_Agent; dwConnID: HP_CONNID): En_HP_HandleResult; stdcall;

  HP_FN_Agent_OnSend = function(pSender: HP_Agent; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer): En_HP_HandleResult; stdcall;

  HP_FN_Agent_OnReceive = function(pSender: HP_Agent; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer): En_HP_HandleResult; stdcall;

  HP_FN_Agent_OnPullReceive = function(pSender: HP_Agent; dwConnID: HP_CONNID; iLength: Integer): En_HP_HandleResult; stdcall;

  HP_FN_Agent_OnClose = function(pSender: HP_Agent; dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer): En_HP_HandleResult; stdcall;

  HP_FN_Agent_OnShutdown = function(pSender: HP_Agent): En_HP_HandleResult; stdcall;

  // * Client 回调函数 */
  HP_FN_Client_OnPrepareConnect = function(pSender: HP_Client; dwConnID: HP_CONNID; SOCKET: UINT_PTR): En_HP_HandleResult; stdcall;

  HP_FN_Client_OnConnect = function(pSender: HP_Client; dwConnID: HP_CONNID): En_HP_HandleResult; stdcall;

  HP_FN_Client_OnHandShake = function(pSender: HP_Client; dwConnID: HP_CONNID): En_HP_HandleResult; stdcall;

  HP_FN_Client_OnSend = function(pSender: HP_Client; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer): En_HP_HandleResult; stdcall;

  HP_FN_Client_OnReceive = function(pSender: HP_Client; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer): En_HP_HandleResult; stdcall;

  HP_FN_Client_OnPullReceive = function(pSender: HP_Client; dwConnID: HP_CONNID; iLength: Integer): En_HP_HandleResult; stdcall;

  HP_FN_Client_OnClose = function(pSender: HP_Client; dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer): En_HP_HandleResult; stdcall;

  // ****************************************************/
  // *************** TCP/UDP 对象创建函数 ****************/

  // 创建 HP_TcpServer 对象
function Create_HP_TcpServer(pListener: HP_TcpServerListener): HP_TcpServer; stdcall; external HPSocketDLL;
// 创建 HP_TcpAgent 对象

function Create_HP_TcpAgent(pListener: HP_TcpAgentListener): HP_TcpAgent; stdcall; external HPSocketDLL;
// 创建 HP_TcpClient 对象

function Create_HP_TcpClient(pListener: HP_TcpClientListener): HP_TcpClient; stdcall; external HPSocketDLL;

// 创建 HP_TcpPullServer 对象
function Create_HP_TcpPullServer(pListener: HP_TcpPullServerListener): HP_TcpPullServer; stdcall; external HPSocketDLL;
// 创建 HP_TcpPullAgent 对象

function Create_HP_TcpPullAgent(pListener: HP_TcpPullAgentListener): HP_TcpPullAgent; stdcall; external HPSocketDLL;
// 创建 HP_TcpPullClient 对象

function Create_HP_TcpPullClient(pListener: HP_TcpPullClientListener): HP_TcpPullClient; stdcall; external HPSocketDLL;

// 创建 HP_TcpPackServer 对象
function Create_HP_TcpPackServer(pListener: HP_TcpServerListener): HP_TcpPackServer; stdcall; external HPSocketDLL;
// 创建 HP_TcpPackAgent 对象

function Create_HP_TcpPackAgent(pListener: HP_TcpAgentListener): HP_TcpPackAgent; stdcall; external HPSocketDLL;
// 创建 HP_TcpPackClient 对象

function Create_HP_TcpPackClient(pListener: HP_TcpClientListener): HP_TcpPackClient; stdcall; external HPSocketDLL;

// 创建 HP_UdpServer 对象
function Create_HP_UdpServer(pListener: HP_UdpServerListener): HP_UdpServer; stdcall; external HPSocketDLL;
// 创建 HP_UdpClient 对象

function Create_HP_UdpClient(pListener: HP_UdpClientListener): HP_UdpClient; stdcall; external HPSocketDLL;
// 创建 HP_UdpCast 对象

function Create_HP_UdpCast(pListener: HP_UdpCastListener): HP_UdpCast; stdcall; external HPSocketDLL;
// 创建 HP_UdpArqServer 对象: //;

function Create_HP_UdpArqServer(pListener: HP_UdpServerListener): HP_UdpArqServer; stdcall; external HPSocketDLL;
// 创建 HP_UdpArqClient 对象: //;

function Create_HP_UdpArqClient(pListener: HP_UdpClientListener): HP_UdpArqClient; stdcall; external HPSocketDLL;

// 销毁 HP_TcpServer 对象
procedure Destroy_HP_TcpServer(pServer: HP_TcpServer); stdcall; external HPSocketDLL;
// 销毁 HP_TcpAgent 对象

procedure Destroy_HP_TcpAgent(pAgent: HP_TcpAgent); stdcall; external HPSocketDLL;
// 销毁 HP_TcpClient 对象

procedure Destroy_HP_TcpClient(pClient: HP_TcpClient); stdcall; external HPSocketDLL;
// 销毁 HP_UdpArqServer 对象:

procedure Destroy_HP_UdpArqServer(pServer: HP_UdpArqServer); external HPSocketDLL;
// 销毁 HP_UdpArqClient 对象:

procedure Destroy_HP_UdpArqClient(pClient: HP_UdpArqClient); external HPSocketDLL;

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
function Create_HP_TcpServerListener(): HP_TcpServerListener; stdcall; external HPSocketDLL;
// 创建 HP_TcpAgentListener 对象

function Create_HP_TcpAgentListener(): HP_TcpAgentListener; stdcall; external HPSocketDLL;
// 创建 HP_TcpClientListener 对象

function Create_HP_TcpClientListener(): HP_TcpClientListener; stdcall; external HPSocketDLL;

// 创建 HP_TcpPullServerListener 对象
function Create_HP_TcpPullServerListener(): HP_TcpPullServerListener; stdcall; external HPSocketDLL;
// 创建 HP_TcpPullAgentListener 对象

function Create_HP_TcpPullAgentListener(): HP_TcpPullAgentListener; stdcall; external HPSocketDLL;
// 创建 HP_TcpPullClientListener 对象

function Create_HP_TcpPullClientListener(): HP_TcpPullClientListener; stdcall; external HPSocketDLL;

// 创建 HP_TcpPackServerListener 对象
function Create_HP_TcpPackServerListener(): HP_TcpPackServerListener; stdcall; external HPSocketDLL;
// 创建 HP_TcpPackAgentListener 对象

function Create_HP_TcpPackAgentListener(): HP_TcpPackAgentListener; stdcall; external HPSocketDLL;
// 创建 HP_TcpPackClientListener 对象

function Create_HP_TcpPackClientListener(): HP_TcpPackClientListener; stdcall; external HPSocketDLL;

// 创建 HP_UdpServerListener 对象
function Create_HP_UdpServerListener(): HP_UdpServerListener; stdcall; external HPSocketDLL;
// 创建 HP_UdpClientListener 对象

function Create_HP_UdpClientListener(): HP_UdpClientListener; stdcall; external HPSocketDLL;
// 创建 HP_UdpCastListener 对象

function Create_HP_UdpCastListener(): HP_UdpCastListener; stdcall; external HPSocketDLL;
// 创建 HP_UdpArqServerListener 对象

function Create_HP_UdpArqServerListener(): HP_UdpArqServerListener; stdcall; external HPSocketDLL;
// 创建 HP_UdpArqClientListener 对象

function Create_HP_UdpArqClientListener(): HP_UdpArqClientListener; stdcall; external HPSocketDLL;

// 销毁 HP_TcpServerListener 对象
procedure Destroy_HP_TcpServerListener(pListener: HP_TcpServerListener); stdcall; external HPSocketDLL;
// 销毁 HP_TcpAgentListener 对象

procedure Destroy_HP_TcpAgentListener(pListener: HP_TcpAgentListener); stdcall; external HPSocketDLL;
// 销毁 HP_TcpClientListener 对象

procedure Destroy_HP_TcpClientListener(pListener: HP_TcpClientListener); stdcall; external HPSocketDLL;
// 销毁 HP_UdpArqServerListener 对象

procedure Destroy_HP_UdpArqServerListener(pListener: HP_UdpArqServerListener); stdcall; external HPSocketDLL;
// 销毁 HP_UdpArqClientListener 对象

procedure Destroy_HP_UdpArqClientListener(pListener: HP_UdpArqClientListener); stdcall; external HPSocketDLL;

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

// **********************************************************************************/
// ***************************** Server 回调函数设置方法 *****************************/
procedure HP_Set_FN_Server_OnPrepareListen(pListener: HP_ServerListener; fn: HP_FN_Server_OnPrepareListen); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Server_OnAccept(pListener: HP_ServerListener; fn: HP_FN_Server_OnAccept); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Server_OnHandShake(pListener: HP_ServerListener; fn: HP_FN_Server_OnHandShake); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Server_OnSend(pListener: HP_ServerListener; fn: HP_FN_Server_OnSend); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Server_OnReceive(pListener: HP_ServerListener; fn: HP_FN_Server_OnReceive); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Server_OnPullReceive(pListener: HP_ServerListener; fn: HP_FN_Server_OnPullReceive); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Server_OnClose(pListener: HP_ServerListener; fn: HP_FN_Server_OnClose); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Server_OnShutdown(pListener: HP_ServerListener; fn: HP_FN_Server_OnShutdown); stdcall; external HPSocketDLL;

// **********************************************************************************/
// ****************************** Agent 回调函数设置方法 *****************************/
procedure HP_Set_FN_Agent_OnPrepareConnect(pListener: HP_AgentListener; fn: HP_FN_Agent_OnPrepareConnect); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Agent_OnConnect(pListener: HP_AgentListener; fn: HP_FN_Agent_OnConnect); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Agent_OnHandShake(pListener: HP_AgentListener; fn: HP_FN_Agent_OnHandShake); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Agent_OnSend(pListener: HP_AgentListener; fn: HP_FN_Agent_OnSend); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Agent_OnReceive(pListener: HP_AgentListener; fn: HP_FN_Agent_OnReceive); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Agent_OnPullReceive(pListener: HP_AgentListener; fn: HP_FN_Agent_OnPullReceive); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Agent_OnClose(pListener: HP_AgentListener; fn: HP_FN_Agent_OnClose); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Agent_OnShutdown(pListener: HP_AgentListener; fn: HP_FN_Agent_OnShutdown); stdcall; external HPSocketDLL;

// **********************************************************************************/
// ***************************** Client 回调函数设置方法 *****************************/
procedure HP_Set_FN_Client_OnPrepareConnect(pListener: HP_ClientListener; fn: HP_FN_Client_OnPrepareConnect); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Client_OnConnect(pListener: HP_ClientListener; fn: HP_FN_Client_OnConnect); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Client_OnHandShake(pListener: HP_ClientListener; fn: HP_FN_Client_OnHandShake); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Client_OnSend(pListener: HP_ClientListener; fn: HP_FN_Client_OnSend); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Client_OnReceive(pListener: HP_ClientListener; fn: HP_FN_Client_OnReceive); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Client_OnPullReceive(pListener: HP_ClientListener; fn: HP_FN_Client_OnPullReceive); stdcall; external HPSocketDLL;

procedure HP_Set_FN_Client_OnClose(pListener: HP_ClientListener; fn: HP_FN_Client_OnClose); stdcall; external HPSocketDLL;

// **************************************************************************/
// ***************************** Server 操作方法 *****************************/

{ //*
  * 名称：启动通信组件
  * 描述：启动服务端通信组件，启动完成后可开始接收客户端连接并收发数据
  *
  * 参数：		lpszBindAddress	-- 监听地址
  *			usPort			-- 监听端口
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 HP_Server_GetLastError() 获取错误代码
  */ }
function HP_Server_Start(pServer: HP_Server; lpszBindAddress: PChar; usPort: USHORT): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：关闭通信组件
  * 描述：关闭服务端通信组件，关闭完成后断开所有客户端连接并释放所有资源
  *
  * 参数：
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 HP_Server_GetLastError() 获取错误代码
  */ }
function HP_Server_Stop(pServer: HP_Server): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：发送数据
  * 描述：向指定连接发送数据
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pBuffer		-- 发送缓冲区
  *			iLength		-- 发送缓冲区长度
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
  */ }
function HP_Server_Send(pServer: HP_Server; dwConnID: HP_CONNID; const pBuffer: PBYTE; iLength: Integer): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：发送数据
  * 描述：向指定连接发送数据
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pBuffer		-- 发送缓冲区
  *			iLength		-- 发送缓冲区长度
  *			iOffset		-- 发送缓冲区指针偏移量
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
  */ }
function HP_Server_SendPart(pServer: HP_Server; dwConnID: HP_CONNID; const pBuffer: PBYTE; iLength: Integer; iOffset: Integer): BOOL; stdcall; external HPSocketDLL;

{ //*
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
  */ }
function HP_Server_SendPackets(pServer: HP_Server; dwConnID: HP_CONNID; const pBuffers: WSABUFArray; iCount: Integer): BOOL; stdcall; external HPSocketDLL;

{ /*
  * 名称：暂停/恢复接收
  * 描述：暂停/恢复某个连接的数据接收工作
  *
  * 参数：		dwConnID	-- 连接 ID
  *			      bPause		-- TRUE - 暂停, FALSE - 恢复
  * 返回值：	TRUE	-- 成功
  *			      FALSE	-- 失败
  */ }
function HP_Server_PauseReceive(pServer: HP_Server; dwConnID: HP_CONNID; bPause: BOOL): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：断开连接
  * 描述：断开与某个客户端的连接
  *
  * 参数：		dwConnID	-- 连接 ID
  *			bForce		-- 是否强制断开连接
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败
  */ }
function HP_Server_Disconnect(pServer: HP_Server; dwConnID: HP_CONNID; bForce: BOOL): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：断开超时连接
  * 描述：断开超过指定时长的连接
  *
  * 参数：		dwPeriod	-- 时长（毫秒）
  *			      bForce		-- 是否强制断开连接
  * 返回值：	TRUE	-- 成功
  *			      FALSE	-- 失败
  */ }
function HP_Server_DisconnectLongConnections(pServer: HP_Server; dwPeriod: DWORD; bForce: BOOL): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：断开静默连接
  * 描述：断开超过指定时长的静默连接
  *
  * 参数：		dwPeriod	-- 时长（毫秒）
  *			      bForce		-- 是否强制断开连接
  * 返回值：	TRUE	-- 成功
  *			      FALSE	-- 失败
  */ }
function HP_Server_DisconnectSilenceConnections(pServer: HP_Server; dwPeriod: DWORD; bForce: BOOL): BOOL; stdcall; external HPSocketDLL;

// ******************************************************************************/
// ***************************** Server 属性访问方法 *****************************/

{ //*
  * 名称：设置连接的附加数据
  * 描述：是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序只身决定
  *
  * 参数：		dwConnID	-- 连接 ID
  *			      pv			-- 数据
  * 返回值：	TRUE	-- 成功
  *			      FALSE	-- 失败（无效的连接 ID）
  */ }
function HP_Server_SetConnectionExtra(pServer: HP_Server; dwConnID: HP_CONNID; pExtra: PVOID): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：获取连接的附加数据
  * 描述：是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序只身决定
  *
  * 参数：		dwConnID	-- 连接 ID
  *			      ppv			-- 数据指针
  * 返回值：	TRUE	-- 成功
  *			      FALSE	-- 失败（无效的连接 ID）
  */ }
function HP_Server_GetConnectionExtra(pServer: HP_Server; dwConnID: HP_CONNID; var ppExtra: PVOID): BOOL; stdcall; external HPSocketDLL;

// 检测是否为安全连接（SSL/HTTPS）
function HP_Server_IsSecure(pServer: HP_Server): BOOL; stdcall; external HPSocketDLL;

// * 检查通信组件是否已启动 */
function HP_Server_HasStarted(pServer: HP_Server): BOOL; stdcall; external HPSocketDLL;
// * 查看通信组件当前状态 */

function HP_Server_GetState(pServer: HP_Server): En_HP_ServiceState; stdcall; external HPSocketDLL;
// * 获取最近一次失败操作的错误代码 */

function HP_Server_GetLastError(pServer: HP_Server): En_HP_SocketError; stdcall; external HPSocketDLL;
// * 获取最近一次失败操作的错误描述 */

function HP_Server_GetLastErrorDesc(pServer: HP_Server): PChar; stdcall; external HPSocketDLL;
// * 获取连接中未发出数据的长度 */

function HP_Server_GetPendingDataLength(pServer: HP_Server; dwConnID: HP_CONNID; var piPending: Integer): BOOL; stdcall; external HPSocketDLL;
// * 获取连接的数据接收状态 */

function HP_Server_IsPauseReceive(pServer: HP_Server; dwConnID: HP_CONNID; var pbPaused: BOOL): BOOL; stdcall; external HPSocketDLL;
// * 检测是否有效连接 */

function HP_Server_IsConnected(pServer: HP_Server; dwConnID: HP_CONNID): BOOL; stdcall; external HPSocketDLL;
// * 获取客户端连接数 */

function HP_Server_GetConnectionCount(pServer: HP_Server): Word; stdcall; external HPSocketDLL;
// * 获取所有连接的 HP_CONNID */

function HP_Server_GetAllConnectionIDs(pServer: HP_Server; pIDs: HP_CONNIDArray; var pdwCount: DWORD): BOOL; stdcall; external HPSocketDLL;
// * 获取某个客户端连接时长（毫秒） */

function HP_Server_GetConnectPeriod(pServer: HP_Server; dwConnID: HP_CONNID; var pdwPeriod: DWORD): BOOL; stdcall; external HPSocketDLL;
// * 获取某个连接静默时间（毫秒） */

function HP_Server_GetSilencePeriod(pServer: HP_Server; dwConnID: HP_CONNID; var pdwPeriod: DWORD): BOOL; stdcall; external HPSocketDLL;
// * 获取监听 Socket 的地址信息 */

function HP_Server_GetListenAddress(pServer: HP_Server; lpszAddress: PChar; var piAddressLen: Integer; var pusPort: Word): BOOL; stdcall; external HPSocketDLL;
// * 获取某个连接的本地地址信息 */

function HP_Server_GetLocalAddress(pServer: HP_Server; dwConnID: HP_CONNID; lpszAddress: PChar; var piAddressLen: Integer; var pusPort: Word): BOOL; stdcall; external HPSocketDLL;
// * 获取某个连接的远程地址信息 */

function HP_Server_GetRemoteAddress(pServer: HP_Server; dwConnID: HP_CONNID; lpszAddress: PChar; var piAddressLen: Integer; var pusPort: Word): BOOL; stdcall; external HPSocketDLL;

// * 设置数据发送策略 */
procedure HP_Server_SetSendPolicy(pServer: HP_Server; enSendPolicy: En_HP_SendPolicy); stdcall; external HPSocketDLL;
// * 设置 OnSend 事件同步策略（默认：OSSP_NONE，不同步） */

procedure HP_Server_SetOnSendSyncPolicy(pServer: HP_Server; enSyncPolicy: En_HP_OnSendSyncPolicy); stdcall; external HPSocketDLL;
// * 设置 Socket 缓存对象锁定时间（毫秒，在锁定期间该 Socket 缓存对象不能被获取使用） */

procedure HP_Server_SetFreeSocketObjLockTime(pServer: HP_Server; dwFreeSocketObjLockTime: DWORD); stdcall; external HPSocketDLL;
// * 设置 Socket 缓存池大小（通常设置为平均并发连接数量的 1/3 - 1/2） */

procedure HP_Server_SetFreeSocketObjPool(pServer: HP_Server; dwFreeSocketObjPool: DWORD); stdcall; external HPSocketDLL;
// * 设置内存块缓存池大小（通常设置为 Socket 缓存池大小的 2 - 3 倍） */

procedure HP_Server_SetFreeBufferObjPool(pServer: HP_Server; dwFreeBufferObjPool: DWORD); stdcall; external HPSocketDLL;
// * 设置 Socket 缓存池回收阀值（通常设置为 Socket 缓存池大小的 3 倍） */

procedure HP_Server_SetFreeSocketObjHold(pServer: HP_Server; dwFreeSocketObjHold: DWORD); stdcall; external HPSocketDLL;
// * 设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍） */

procedure HP_Server_SetFreeBufferObjHold(pServer: HP_Server; dwFreeBufferObjHold: DWORD); stdcall; external HPSocketDLL;
// * 设置最大连接数（组件会根据设置值预分配内存，因此需要根据实际情况设置，不宜过大）*/

procedure HP_Server_SetMaxConnectionCount(pServer: HP_Server; dwMaxConnectionCount: DWORD); stdcall; external HPSocketDLL;
// * 设置工作线程数量（通常设置为 2 * CPU + 2） */

procedure HP_Server_SetWorkerThreadCount(pServer: HP_Server; dwWorkerThreadCount: DWORD); stdcall; external HPSocketDLL;
// * 设置是否标记静默时间（设置为 TRUE 时 DisconnectSilenceConnections() 和 GetSilencePeriod() 才有效，默认：TRUE） */

procedure HP_Server_SetMarkSilence(pServer: HP_Server; bMarkSilence: BOOL); stdcall; external HPSocketDLL;

// * 获取数据发送策略 */
function HP_Server_GetSendPolicy(pServer: HP_Server): En_HP_SendPolicy; stdcall; external HPSocketDLL;
// * 获取 OnSend 事件同步策略 */

function HP_Server_GetOnSendSyncPolicy(pServer: HP_Server): En_HP_OnSendSyncPolicy; stdcall; external HPSocketDLL;
// * 获取 Socket 缓存对象锁定时间 */

function HP_Server_GetFreeSocketObjLockTime(pServer: HP_Server): DWORD; stdcall; external HPSocketDLL;
// * 获取 Socket 缓存池大小 */

function HP_Server_GetFreeSocketObjPool(pServer: HP_Server): DWORD; stdcall; external HPSocketDLL;
// * 获取内存块缓存池大小 */

function HP_Server_GetFreeBufferObjPool(pServer: HP_Server): DWORD; stdcall; external HPSocketDLL;
// * 获取 Socket 缓存池回收阀值 */

function HP_Server_GetFreeSocketObjHold(pServer: HP_Server): DWORD; stdcall; external HPSocketDLL;
// * 获取内存块缓存池回收阀值 */

function HP_Server_GetFreeBufferObjHold(pServer: HP_Server): DWORD; stdcall; external HPSocketDLL;
// * 获取最大连接数 */

function HP_Server_GetMaxConnectionCount(pServer: HP_Server): DWORD; stdcall; external HPSocketDLL;
// * 获取工作线程数量 */

function HP_Server_GetWorkerThreadCount(pServer: HP_Server): DWORD; stdcall; external HPSocketDLL;
// * 检测是否标记静默时间 */

function HP_Server_IsMarkSilence(pServer: HP_Server): BOOL; stdcall; external HPSocketDLL;

// **********************************************************************************/
// ******************************* TCP Server 操作方法 *******************************/

{ //*
  * 名称：发送小文件
  * 描述：向指定连接发送 4096 KB 以下的小文件
  *
  * 参数：		dwConnID		-- 连接 ID
  *			lpszFileName	-- 文件路径
  *			pHead			-- 头部附加数据
  *			pTail			-- 尾部附加数据
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
  */ }
function HP_TcpServer_SendSmallFile(pServer: HP_Server; dwConnID: HP_CONNID; lpszFileName: PChar; const pHead: LPWSABUF; const pTail: LPWSABUF): BOOL; stdcall; external HPSocketDLL;

// **********************************************************************************/
// ***************************** TCP Server 属性访问方法 *****************************/

// * 设置监听 Socket 的等候队列大小（根据并发连接数量调整设置） */
procedure HP_TcpServer_SetSocketListenQueue(pServer: HP_TcpServer; dwSocketListenQueue: DWORD); stdcall; external HPSocketDLL;
// * 设置 Accept 预投递数量（根据负载调整设置，Accept 预投递数量越大则支持的并发连接请求越多） */

procedure HP_TcpServer_SetAcceptSocketCount(pServer: HP_TcpServer; dwAcceptSocketCount: DWORD); stdcall; external HPSocketDLL;
// * 设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为 1024 的倍数） */

procedure HP_TcpServer_SetSocketBufferSize(pServer: HP_TcpServer; dwSocketBufferSize: DWORD); stdcall; external HPSocketDLL;
// * 设置正常心跳包间隔（毫秒，0 则不发送心跳包，默认：30 * 1000） */

procedure HP_TcpServer_SetKeepAliveTime(pServer: HP_TcpServer; dwKeepAliveTime: DWORD); stdcall; external HPSocketDLL;
// * 设置异常心跳包间隔（毫秒，0 不发送心跳包，，默认：10 * 1000，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线） */

procedure HP_TcpServer_SetKeepAliveInterval(pServer: HP_TcpServer; dwKeepAliveInterval: DWORD); stdcall; external HPSocketDLL;

// * 获取 Accept 预投递数量 */
function HP_TcpServer_GetAcceptSocketCount(pServer: HP_TcpServer): DWORD; stdcall; external HPSocketDLL;
// * 获取通信数据缓冲区大小 */

function HP_TcpServer_GetSocketBufferSize(pServer: HP_TcpServer): DWORD; stdcall; external HPSocketDLL;
// * 获取监听 Socket 的等候队列大小 */

function HP_TcpServer_GetSocketListenQueue(pServer: HP_TcpServer): DWORD; stdcall; external HPSocketDLL;
// * 获取正常心跳包间隔 */

function HP_TcpServer_GetKeepAliveTime(pServer: HP_TcpServer): DWORD; stdcall; external HPSocketDLL;
// * 获取异常心跳包间隔 */

function HP_TcpServer_GetKeepAliveInterval(pServer: HP_TcpServer): DWORD; stdcall; external HPSocketDLL;

// **********************************************************************************/
// ***************************** UDP Server 属性访问方法 *****************************/

// * 设置数据报文最大长度（建议在局域网环境下不超过 1472 字节，在广域网环境下不超过 548 字节） */
procedure HP_UdpServer_SetMaxDatagramSize(pServer: HP_UdpServer; dwMaxDatagramSize: DWORD); stdcall; external HPSocketDLL;
// * 获取数据报文最大长度 */

function HP_UdpServer_GetMaxDatagramSize(pServer: HP_UdpServer): DWORD; stdcall; external HPSocketDLL;
// * 设置 Receive 预投递数量（根据负载调整设置，Receive 预投递数量越大则丢包概率越小） */

procedure HP_UdpServer_SetPostReceiveCount(pServer: HP_UdpServer; dwPostReceiveCount: DWORD); stdcall; external HPSocketDLL;
// * 获取 Receive 预投递数量 */

function HP_UdpServer_GetPostReceiveCount(pServer: HP_UdpServer): DWORD; stdcall; external HPSocketDLL;
// * 设置监测包尝试次数（0 则不发送监测跳包，如果超过最大尝试次数则认为已断线） */

procedure HP_UdpServer_SetDetectAttempts(pServer: HP_UdpServer; dwDetectAttempts: DWORD); stdcall; external HPSocketDLL;
// * 设置监测包发送间隔（秒，0 不发送监测包） */

procedure HP_UdpServer_SetDetectInterval(pServer: HP_UdpServer; dwDetectInterval: DWORD); stdcall; external HPSocketDLL;
// * 获取心跳检查次数 */

function HP_UdpServer_GetDetectAttempts(pServer: HP_UdpServer): DWORD; stdcall; external HPSocketDLL;
// * 获取心跳检查间隔 */

function HP_UdpServer_GetDetectInterval(pServer: HP_UdpServer): DWORD; stdcall; external HPSocketDLL;

// *************************** UDP ARQ Server 属性访问方法 ***************************/

// * 设置是否开启 nodelay 模式（默认：FALSE，不开启） */
procedure HP_UdpArqServer_SetNoDelay(pServer: HP_UdpArqServer; bNoDelay: Boolean); stdcall; external HPSocketDLL;
// * 设置是否关闭拥塞控制（默认：FALSE，不关闭） */

procedure HP_UdpArqServer_SetTurnoffCongestCtrl(pServer: HP_UdpArqServer; bTurnOff: Boolean); stdcall; external HPSocketDLL;
// * 设置数据刷新间隔（毫秒，默认：60） */

procedure HP_UdpArqServer_SetFlushInterval(pServer: HP_UdpArqServer; dwFlushInterval: DWORD); stdcall; external HPSocketDLL;
// * 设置快速重传 ACK 跨越次数（默认：0，关闭快速重传） */

procedure HP_UdpArqServer_SetResendByAcks(pServer: HP_UdpArqServer; dwResendByAcks: DWORD); stdcall; external HPSocketDLL;
// * 设置发送窗口大小（数据包数量，默认：128） */

procedure HP_UdpArqServer_SetSendWndSize(pServer: HP_UdpArqServer; dwSendWndSize: DWORD); stdcall; external HPSocketDLL;
// * 设置接收窗口大小（数据包数量，默认：512） */

procedure HP_UdpArqServer_SetRecvWndSize(pServer: HP_UdpArqServer; dwRecvWndSize: DWORD); stdcall; external HPSocketDLL;
// * 设置最小重传超时时间（毫秒，默认：30） */

procedure HP_UdpArqServer_SetMinRto(pServer: HP_UdpArqServer; dwMinRto: DWORD); stdcall; external HPSocketDLL;
// * 设置最大传输单元（默认：0，与 SetMaxDatagramSize(pServer:) 一致） */

procedure HP_UdpArqServer_SetMaxTransUnit(pServer: HP_UdpArqServer; dwMaxTransUnit: DWORD); stdcall; external HPSocketDLL;
// * 设置最大数据包大小（默认：4096） */

procedure HP_UdpArqServer_SetMaxMessageSize(pServer: HP_UdpArqServer; dwMaxMessageSize: DWORD); stdcall; external HPSocketDLL;
// * 设置握手超时时间（毫秒，默认：5000） */

procedure HP_UdpArqServer_SetHandShakeTimeout(pServer: HP_UdpArqServer; dwHandShakeTimeout: DWORD); stdcall; external HPSocketDLL;

// * 检测是否开启 nodelay 模式 */
function HP_UdpArqServer_IsNoDelay(pServer: HP_UdpArqServer): Boolean; stdcall; external HPSocketDLL;
// * 检测是否关闭拥塞控制 */

function HP_UdpArqServer_IsTurnoffCongestCtrl(pServer: HP_UdpArqServer): Boolean; stdcall; external HPSocketDLL;
// * 获取数据刷新间隔 */

function HP_UdpArqServer_GetFlushInterval(pServer: HP_UdpArqServer): DWORD; stdcall; external HPSocketDLL;
// * 获取快速重传 ACK 跨越次数 */

function HP_UdpArqServer_GetResendByAcks(pServer: HP_UdpArqServer): DWORD; stdcall; external HPSocketDLL;
// * 获取发送窗口大小 */

function HP_UdpArqServer_GetSendWndSize(pServer: HP_UdpArqServer): DWORD; stdcall; external HPSocketDLL;
// * 获取接收窗口大小 */

function HP_UdpArqServer_GetRecvWndSize(pServer: HP_UdpArqServer): DWORD; stdcall; external HPSocketDLL;
// * 获取最小重传超时时间 */

function HP_UdpArqServer_GetMinRto(pServer: HP_UdpArqServer): DWORD; stdcall; external HPSocketDLL;
// * 获取最大传输单元 */

function HP_UdpArqServer_GetMaxTransUnit(pServer: HP_UdpArqServer): DWORD; stdcall; external HPSocketDLL;
// * 获取最大数据包大小 */

function HP_UdpArqServer_GetMaxMessageSize(pServer: HP_UdpArqServer): DWORD; stdcall; external HPSocketDLL;
// * 获取握手超时时间 */

function HP_UdpArqServer_GetHandShakeTimeout(pServer: HP_UdpArqServer): DWORD; stdcall; external HPSocketDLL;

// * 获取等待发送包数量 */
function HP_UdpArqServer_GetWaitingSendMessageCount(pServer: HP_UdpArqServer; dwConnID: HP_CONNID; var piCount: Integer): Boolean; stdcall; external HPSocketDLL;

// **************************************************************************/
// ***************************** Agent 操作方法 *****************************/

// *
{ * 名称：启动通信组件
  * 描述：启动通信代理组件，启动完成后可开始连接远程服务器
  *
  * 参数：		lpszBindAddress	-- 绑定地址（默认：nullptr，绑定 0.0.0.0）
  *			bAsyncConnect	-- 是否采用异步 Connect
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 HP_Agent_GetLastError() 获取错误代码
  */ }
function HP_Agent_Start(pAgent: HP_Agent; lpszBindAddress: PChar; bAsyncConnect: BOOL): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：关闭通信组件
  * 描述：关闭通信组件，关闭完成后断开所有连接并释放所有资源
  *
  * 参数：
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 HP_Agent_GetLastError() 获取错误代码
  */ }
function HP_Agent_Stop(pAgent: HP_Agent): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：连接服务器
  * 描述：连接服务器，连接成功后 IAgentListener 会接收到 OnConnect() / OnHandShake() 事件
  *
  * 参数：		lpszRemoteAddress	-- 服务端地址
  *			usPort				-- 服务端端口
  *			pdwConnID			-- 连接 ID（默认：nullptr，不获取连接 ID）
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过函数 SYS_GetLastError() 获取 Windows 错误代码
  */ }
function HP_Agent_Connect(pAgent: HP_Agent; lpszRemoteAddress: PChar; usPort: USHORT; pdwConnID: PInteger): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：连接服务器
  * 描述：连接服务器，连接成功后 IAgentListener 会接收到 OnConnect() / OnHandShake() 事件
  *
  * 参数：		lpszRemoteAddress	-- 服务端地址
  *			usPort				-- 服务端端口
  *			pdwConnID			-- 连接 ID（默认：nullptr，不获取连接 ID）
  *			pExtra				-- 连接附加数据（默认：nullptr）
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过函数 SYS_GetLastError() 获取 Windows 错误代码
  */ }
function HP_Agent_ConnectWithExtra(pAgent: HP_Agent; lpszRemoteAddress: PChar; usPort: USHORT;var pdwConnID: PInteger; pExtra: PVOID): BOOL; stdcall; external HPSocketDLL;

{ /*
  * 名称：连接服务器
  * 描述：连接服务器，连接成功后 IAgentListener 会接收到 OnConnect() / OnHandShake() 事件
  *
  * 参数：		lpszRemoteAddress	-- 服务端地址
  *			usPort				-- 服务端端口
  *			pdwConnID			-- 连接 ID（默认：nullptr，不获取连接 ID）
  *			usLocalPort			-- 本地端口（默认：0）
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过函数 SYS_GetLastError() 获取 Windows 错误代码
  */ }
function HP_Agent_ConnectWithLocalPort(pAgent: HP_Agent; lpszRemoteAddress: PChar; usPort: USHORT; pdwConnID: PInteger; usLocalPort: USHORT): BOOL; stdcall; external HPSocketDLL;
{/*
* 名称：连接服务器
* 描述：连接服务器，连接成功后 IAgentListener 会接收到 OnConnect() / OnHandShake() 事件
*
* 参数：		lpszRemoteAddress	-- 服务端地址
*			usPort				-- 服务端端口
*			pdwConnID			-- 连接 ID（默认：nullptr，不获取连接 ID）
*			lpszLocalAddress	-- 本地地址（默认：nullptr，使用 Start() 方法中绑定的地址）
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过函数 SYS_GetLastError() 获取 Windows 错误代码
*/ }
function HP_Agent_ConnectWithLocalAddress(pAgent: HP_Agent; lpszRemoteAddress: PChar; usPort: USHORT; pdwConnID: PInteger; lpszLocalAddress: PChar): Boolean; stdcall; external HPSocketDLL;

{ /*
  * 名称：连接服务器
  * 描述：连接服务器，连接成功后 IAgentListener 会接收到 OnConnect() / OnHandShake() 事件
  *
  * 参数：		lpszRemoteAddress	-- 服务端地址
  *			usPort				-- 服务端端口
  *			pdwConnID			-- 连接 ID（默认：nullptr，不获取连接 ID）
  *			pExtra				-- 连接附加数据（默认：nullptr）
  *			usLocalPort			-- 本地端口（默认：0）
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过函数 SYS_GetLastError() 获取 Windows 错误代码
  */ }
function HP_Agent_ConnectWithExtraAndLocalPort(pAgent: HP_Agent; lpszRemoteAddress: PChar; usPort: USHORT; pdwConnID: PInteger; pExtra: PVOID; usLocalPort: USHORT): BOOL; stdcall; external HPSocketDLL;
{/*
* 名称：连接服务器
* 描述：连接服务器，连接成功后 IAgentListener 会接收到 OnConnect() / OnHandShake() 事件
*
* 参数：		lpszRemoteAddress	-- 服务端地址
*			usPort				-- 服务端端口
*			pdwConnID			-- 连接 ID（默认：nullptr，不获取连接 ID）
*			pExtra				-- 连接附加数据（默认：nullptr）
*			usLocalPort			-- 本地端口（默认：0）
*			lpszLocalAddress	-- 本地地址（默认：nullptr，使用 Start() 方法中绑定的地址）
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过函数 SYS_GetLastError() 获取 Windows 错误代码
*/}
function HP_Agent_ConnectWithExtraAndLocalAddressPort(pAgent:HP_Agent ; lpszRemoteAddress: PChar; usPort: USHORT ; pdwConnID: PInteger;pExtra: PVOID ;usLocalPort: USHORT ; lpszLocalAddress: PChar ): Boolean ; stdcall; external HPSocketDLL;
{ //*
  * 名称：发送数据
  * 描述：向指定连接发送数据
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pBuffer		-- 发送缓冲区
  *			iLength		-- 发送缓冲区长度
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
  */ }
function HP_Agent_Send(pAgent: HP_Agent; dwConnID: HP_CONNID; const pBuffer: PBYTE; iLength: Integer): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：发送数据
  * 描述：向指定连接发送数据
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pBuffer		-- 发送缓冲区
  *			iLength		-- 发送缓冲区长度
  *			iOffset		-- 发送缓冲区指针偏移量
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
  */ }
function HP_Agent_SendPart(pAgent: HP_Agent; dwConnID: HP_CONNID; const pBuffer: PBYTE; iLength: Integer; iOffset: Integer): BOOL; stdcall; external HPSocketDLL;

{ //*
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
  */ }
function HP_Agent_SendPackets(pAgent: HP_Agent; dwConnID: HP_CONNID; const pBuffers: WSABUFArray; iCount: Integer): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：暂停/恢复接收
  * 描述：暂停/恢复某个连接的数据接收工作
  *
  * 参数：		dwConnID	-- 连接 ID
  *			bPause		-- TRUE - 暂停, FALSE - 恢复
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败
  */ }
function HP_Agent_PauseReceive(pAgent: HP_Agent; dwConnID: HP_CONNID; bPause: BOOL): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：断开连接
  * 描述：断开某个连接
  *
  * 参数：		dwConnID	-- 连接 ID
  *			bForce		-- 是否强制断开连接
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败
  */ }
function HP_Agent_Disconnect(pAgent: HP_Agent; dwConnID: HP_CONNID; bForce: BOOL): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：断开超时连接
  * 描述：断开超过指定时长的连接
  *
  * 参数：		dwPeriod	-- 时长（毫秒）
  *			bForce		-- 是否强制断开连接
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败
  */ }
function HP_Agent_DisconnectLongConnections(pAgent: HP_Agent; dwPeriod: DWORD; bForce: BOOL): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：断开静默连接
  * 描述：断开超过指定时长的静默连接
  *
  * 参数：		dwPeriod	-- 时长（毫秒）
  *			bForce		-- 是否强制断开连接
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败
  */ }
function HP_Agent_DisconnectSilenceConnections(pAgent: HP_Agent; dwPeriod: DWORD; bForce: BOOL): BOOL; stdcall; external HPSocketDLL;

// ******************************************************************************/
// ***************************** Agent 属性访问方法 *****************************/

{ //*
  * 名称：设置连接的附加数据
  * 描述：是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序只身决定
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pv			-- 数据
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败（无效的连接 ID）
  */ }
function HP_Agent_SetConnectionExtra(pAgent: HP_Agent; dwConnID: HP_CONNID; pExtra: Pointer): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：获取连接的附加数据
  * 描述：是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序只身决定
  *
  * 参数：		dwConnID	-- 连接 ID
  *			ppv			-- 数据指针
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败（无效的连接 ID）
  */ }
function HP_Agent_GetConnectionExtra(pAgent: HP_Agent; dwConnID: HP_CONNID; var ppExtra: PVOID): BOOL; stdcall; external HPSocketDLL;

// * 检测是否为安全连接（SSL/HTTPS） */
function HP_Agent_IsSecure(pAgent: HP_Agent): BOOL; stdcall; external HPSocketDLL;

// * 检查通信组件是否已启动 */
function HP_Agent_HasStarted(pAgent: HP_Agent): BOOL; stdcall; external HPSocketDLL;
// * 查看通信组件当前状态 */

function HP_Agent_GetState(pAgent: HP_Agent): En_HP_ServiceState; stdcall; external HPSocketDLL;
// * 获取连接数 */

function HP_Agent_GetConnectionCount(pAgent: HP_Agent): DWORD; stdcall; external HPSocketDLL;
// * 获取所有连接的 HP_CONNID */

function HP_Agent_GetAllConnectionIDs(pAgent: HP_Agent; pIDs: HP_CONNIDArray; var pdwCount: DWORD): BOOL; stdcall; external HPSocketDLL;
// * 获取某个连接时长（毫秒） */

function HP_Agent_GetConnectPeriod(pAgent: HP_Agent; dwConnID: HP_CONNID; var pdwPeriod: DWORD): BOOL; stdcall; external HPSocketDLL;
// * 获取某个连接静默时间（毫秒） */

function HP_Agent_GetSilencePeriod(pAgent: HP_Agent; dwConnID: HP_CONNID; var pdwPeriod: DWORD): BOOL; stdcall; external HPSocketDLL;
// * 获取某个连接的本地地址信息 */

function HP_Agent_GetLocalAddress(pAgent: HP_Agent; dwConnID: HP_CONNID; lpszAddress: PChar; var piAddressLen: Integer; var pusPort: USHORT): BOOL; stdcall; external HPSocketDLL;
// * 获取某个连接的远程地址信息 */

function HP_Agent_GetRemoteAddress(pAgent: HP_Agent; dwConnID: HP_CONNID; lpszAddress: PChar; var piAddressLen: Integer; var pusPort: USHORT): BOOL; stdcall; external HPSocketDLL;
// * 获取某个连接的远程主机信息 */

function HP_Agent_GetRemoteHost(pAgent: HP_Agent; dwConnID: HP_CONNID; lpszHost: PChar; var piHostLen: Integer; var Integer: USHORT): BOOL; stdcall; external HPSocketDLL;
// * 获取最近一次失败操作的错误代码 */

function HP_Agent_GetLastError(pAgent: HP_Agent): En_HP_SocketError; stdcall; external HPSocketDLL;
// * 获取最近一次失败操作的错误描述 */

function HP_Agent_GetLastErrorDesc(pAgent: HP_Agent): PChar; stdcall; external HPSocketDLL;
// * 获取连接中未发出数据的长度 */

function HP_Agent_GetPendingDataLength(pAgent: HP_Agent; dwConnID: HP_CONNID; var piPending: Integer): BOOL; stdcall; external HPSocketDLL;
// * 获取连接的数据接收状态 */

function HP_Agent_IsPauseReceive(pAgent: HP_Agent; dwConnID: HP_CONNID; var pbPaused: BOOL): BOOL; stdcall; external HPSocketDLL;
// * 检测是否有效连接 */

function HP_Agent_IsConnected(pAgent: HP_Agent; dwConnID: HP_CONNID): BOOL; stdcall; external HPSocketDLL;

// * 设置数据发送策略 */
procedure HP_Agent_SetSendPolicy(pAgent: HP_Agent; enSendPolicy: En_HP_SendPolicy); stdcall; external HPSocketDLL;
// * 设置 OnSend 事件同步策略（默认：OSSP_NONE，不同步） */

procedure HP_Agent_SetOnSendSyncPolicy(pAgent: HP_Agent; enSyncPolicy: En_HP_OnSendSyncPolicy); stdcall; external HPSocketDLL;
// * 设置 Socket 缓存对象锁定时间（毫秒，在锁定期间该 Socket 缓存对象不能被获取使用） */

procedure HP_Agent_SetFreeSocketObjLockTime(pAgent: HP_Agent; dwFreeSocketObjLockTime: DWORD); stdcall; external HPSocketDLL;
// * 设置 Socket 缓存池大小（通常设置为平均并发连接数量的 1/3 - 1/2） */

procedure HP_Agent_SetFreeSocketObjPool(pAgent: HP_Agent; dwFreeSocketObjPool: DWORD); stdcall; external HPSocketDLL;
// * 设置内存块缓存池大小（通常设置为 Socket 缓存池大小的 2 - 3 倍） */

procedure HP_Agent_SetFreeBufferObjPool(pAgent: HP_Agent; dwFreeBufferObjPool: DWORD); stdcall; external HPSocketDLL;
// * 设置 Socket 缓存池回收阀值（通常设置为 Socket 缓存池大小的 3 倍） */

procedure HP_Agent_SetFreeSocketObjHold(pAgent: HP_Agent; dwFreeSocketObjHold: DWORD); stdcall; external HPSocketDLL;
// * 设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍） */

procedure HP_Agent_SetFreeBufferObjHold(pAgent: HP_Agent; dwFreeBufferObjHold: DWORD); stdcall; external HPSocketDLL;
// * 设置最大连接数（组件会根据设置值预分配内存，因此需要根据实际情况设置，不宜过大）*/

procedure HP_Agent_SetMaxConnectionCount(pAgent: HP_Agent; dwMaxConnectionCount: DWORD); stdcall; external HPSocketDLL;
// * 设置工作线程数量（通常设置为 2 * CPU + 2） */

procedure HP_Agent_SetWorkerThreadCount(pAgent: HP_Agent; dwWorkerThreadCount: DWORD); stdcall; external HPSocketDLL;
// * 设置是否标记静默时间（设置为 TRUE 时 DisconnectSilenceConnections() 和 GetSilencePeriod() 才有效，默认：TRUE） */

procedure HP_Agent_SetMarkSilence(pAgent: HP_Agent; bMarkSilence: BOOL); stdcall; external HPSocketDLL;

// * 获取数据发送策略 */
function HP_Agent_GetSendPolicy(pAgent: HP_Agent): En_HP_SendPolicy; stdcall; external HPSocketDLL;
// * 获取 OnSend 事件同步策略 */

function HP_Agent_GetOnSendSyncPolicy(pAgent: HP_Agent): En_HP_OnSendSyncPolicy; stdcall; external HPSocketDLL;
// * 获取 Socket 缓存对象锁定时间 */

function HP_Agent_GetFreeSocketObjLockTime(pAgent: HP_Agent): DWORD; stdcall; external HPSocketDLL;
// * 获取 Socket 缓存池大小 */

function HP_Agent_GetFreeSocketObjPool(pAgent: HP_Agent): DWORD; stdcall; external HPSocketDLL;
// * 获取内存块缓存池大小 */

function HP_Agent_GetFreeBufferObjPool(pAgent: HP_Agent): DWORD; stdcall; external HPSocketDLL;
// * 获取 Socket 缓存池回收阀值 */

function HP_Agent_GetFreeSocketObjHold(pAgent: HP_Agent): DWORD; stdcall; external HPSocketDLL;
// * 获取内存块缓存池回收阀值 */

function HP_Agent_GetFreeBufferObjHold(pAgent: HP_Agent): DWORD; stdcall; external HPSocketDLL;
// * 获取最大连接数 */

function HP_Agent_GetMaxConnectionCount(pAgent: HP_Agent): DWORD; stdcall; external HPSocketDLL;
// * 获取工作线程数量 */

function HP_Agent_GetWorkerThreadCount(pAgent: HP_Agent): DWORD; stdcall; external HPSocketDLL;
// * 检测是否标记静默时间 */

function HP_Agent_IsMarkSilence(pAgent: HP_Agent): BOOL; stdcall; external HPSocketDLL;

// **********************************************************************************/
// ******************************* TCP Agent 操作方法 *******************************/

// *
{ * 名称：发送小文件
  * 描述：向指定连接发送 4096 KB 以下的小文件
  *
  * 参数：		dwConnID		-- 连接 ID
  *			lpszFileName	-- 文件路径
  *			pHead			-- 头部附加数据
  *			pTail			-- 尾部附加数据
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
  */ }
function HP_TcpAgent_SendSmallFile(pAgent: HP_Agent; dwConnID: HP_CONNID; lpszFileName: PChar; const pHead: LPWSABUF; const pTail: LPWSABUF): BOOL; stdcall; external HPSocketDLL;

// **********************************************************************************/
// ***************************** TCP Agent 属性访问方法 *****************************/

// * 设置是否启用地址重用机制（默认：不启用） */
procedure HP_TcpAgent_SetReuseAddress(pAgent: HP_TcpAgent; bReuseAddress: BOOL); stdcall; external HPSocketDLL;
// * 检测是否启用地址重用机制 */

function HP_TcpAgent_IsReuseAddress(pAgent: HP_TcpAgent): BOOL; stdcall; external HPSocketDLL;
// * 设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为 1024 的倍数） */

procedure HP_TcpAgent_SetSocketBufferSize(pAgent: HP_TcpAgent; dwSocketBufferSize: DWORD); stdcall; external HPSocketDLL;
// * 设置正常心跳包间隔（毫秒，0 则不发送心跳包，默认：30 * 1000） */

procedure HP_TcpAgent_SetKeepAliveTime(pAgent: HP_TcpAgent; dwKeepAliveTime: DWORD); stdcall; external HPSocketDLL;
// * 设置异常心跳包间隔（毫秒，0 不发送心跳包，，默认：10 * 1000，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线） */

procedure HP_TcpAgent_SetKeepAliveInterval(pAgent: HP_TcpAgent; dwKeepAliveInterval: DWORD); stdcall; external HPSocketDLL;
// * 获取通信数据缓冲区大小 */

function HP_TcpAgent_GetSocketBufferSize(pAgent: HP_TcpAgent): DWORD; stdcall; external HPSocketDLL;
// * 获取正常心跳包间隔 */

function HP_TcpAgent_GetKeepAliveTime(pAgent: HP_TcpAgent): DWORD; stdcall; external HPSocketDLL;
// * 获取异常心跳包间隔 */

function HP_TcpAgent_GetKeepAliveInterval(pAgent: HP_TcpAgent): DWORD; stdcall; external HPSocketDLL;

// ******************************************************************************/
// ***************************** Client 组件操作方法 *****************************/

// *
{ * 名称：启动通信组件
  * 描述：启动客户端通信组件并连接服务端，启动完成后可开始收发数据
  *
  * 参数：		lpszRemoteAddress	-- 服务端地址
  *			usPort				-- 服务端端口
  *			bAsyncConnect		-- 是否采用异步 Connect
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 HP_Client_GetLastError() 获取错误代码
  */ }
function HP_Client_Start(pClient: HP_Client; lpszRemoteAddress: PChar; usPort: USHORT; bAsyncConnect: BOOL): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：启动通信组件（并指定绑定地址）
  * 描述：启动客户端通信组件并连接服务端，启动完成后可开始收发数据
  *
  * 参数：		lpszRemoteAddress	-- 服务端地址
  *			usPort				-- 服务端端口
  *			bAsyncConnect		-- 是否采用异步 Connect
  *			lpszBindAddress		-- 绑定地址（默认：nullptr，TcpClient/UdpClient -> 不执行绑定操作，UdpCast 绑定 -> 0.0.0.0）
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 HP_Client_GetLastError() 获取错误代码
  */ }
function HP_Client_StartWithBindAddress(pClient: HP_Client; lpszRemoteAddress: PChar; usPort: USHORT; bAsyncConnect: BOOL; lpszBindAddress: PChar): BOOL; stdcall; external HPSocketDLL;

{ /*
  * 名称：启动通信组件（并指定绑定地址）
  * 描述：启动客户端通信组件并连接服务端，启动完成后可开始收发数据
  *
  * 参数：		lpszRemoteAddress	-- 服务端地址
  *			usPort				-- 服务端端口
  *			bAsyncConnect		-- 是否采用异步 Connect
  *			lpszBindAddress		-- 绑定地址（默认：nullptr，TcpClient/UdpClient -> 不执行绑定操作，UdpCast 绑定 -> 任意地址）
  *			usLocalPort			-- 本地端口（默认：0）
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 HP_Client_GetLastError() 获取错误代码
  */ }
function HP_Client_StartWithBindAddressAndLocalPort(pClient: HP_Client; lpszRemoteAddress: PChar; usPort: USHORT; bAsyncConnect: BOOL; lpszBindAddress: PChar; usLocalPort: USHORT): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：关闭通信组件
  * 描述：关闭客户端通信组件，关闭完成后断开与服务端的连接并释放所有资源
  *
  * 参数：
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 HP_Client_GetLastError() 获取错误代码
  */ }
function HP_Client_Stop(pClient: HP_Client): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：发送数据
  * 描述：向服务端发送数据
  *
  * 参数：		pBuffer		-- 发送缓冲区
  *			iLength		-- 发送缓冲区长度
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
  */ }
function HP_Client_Send(pClient: HP_Client; const pBuffer: PBYTE; iLength: Integer): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：发送数据
  * 描述：向服务端发送数据
  *
  * 参数：		pBuffer		-- 发送缓冲区
  *			iLength		-- 发送缓冲区长度
  *			iOffset		-- 发送缓冲区指针偏移量
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
  */ }
function HP_Client_SendPart(pClient: HP_Client; const pBuffer: PBYTE; iLength: Integer; iOffset: Integer): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：发送多组数据
  * 描述：向服务端发送多组数据
  *		TCP - 顺序发送所有数据包
  *		UDP - 把所有数据包组合成一个数据包发送（数据包的总长度不能大于设置的 UDP 包最大长度）
  *
  * 参数：		pBuffers	-- 发送缓冲区数组
  *			iCount		-- 发送缓冲区数目
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
  */ }
function HP_Client_SendPackets(pClient: HP_Client; const pBuffers: WSABUFArray; iCount: Integer): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：暂停/恢复接收
  * 描述：暂停/恢复某个连接的数据接收工作
  *
  *			bPause	-- TRUE - 暂停, FALSE - 恢复
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败
  */ }
function HP_Client_PauseReceive(pClient: HP_Client; bPause: BOOL): BOOL; stdcall; external HPSocketDLL;

// ******************************************************************************/
// ***************************** Client 属性访问方法 *****************************/

// * 设置连接的附加数据 */
procedure HP_Client_SetExtra(pClient: HP_Client; pExtra: PVOID); stdcall; external HPSocketDLL;
// * 获取连接的附加数据 */

function HP_Client_GetExtra(pClient: HP_Client): PVOID; stdcall; external HPSocketDLL;

// * 检测是否为安全连接（SSL/HTTPS） */
function HP_Client_IsSecure(pClient: HP_Client): BOOL; stdcall; external HPSocketDLL;

// * 检查通信组件是否已启动 */
function HP_Client_HasStarted(pClient: HP_Client): BOOL; stdcall; external HPSocketDLL;
// * 查看通信组件当前状态 */

function HP_Client_GetState(pClient: HP_Client): En_HP_ServiceState; stdcall; external HPSocketDLL;
// * 获取最近一次失败操作的错误代码 */

function HP_Client_GetLastError(pClient: HP_Client): En_HP_SocketError; stdcall; external HPSocketDLL;
// * 获取最近一次失败操作的错误描述 */

function HP_Client_GetLastErrorDesc(pClient: HP_Client): PChar; stdcall; external HPSocketDLL;
// * 获取该组件对象的连接 ID */

function HP_Client_GetConnectionID(pClient: HP_Client): HP_CONNID; stdcall; external HPSocketDLL;
// * 获取 Client Socket 的地址信息 */

function HP_Client_GetLocalAddress(pClient: HP_Client; lpszAddress: PChar; var piAddressLen: Integer; var pusPort: USHORT): BOOL; stdcall; external HPSocketDLL;
// * 获取连接的远程主机信息 */

function HP_Client_GetRemoteHost(pClient: HP_Client; lpszHost: PChar; var piHostLen: Integer; var pusPort: USHORT): BOOL; stdcall; external HPSocketDLL;
// * 获取连接中未发出数据的长度 */

function HP_Client_GetPendingDataLength(pClient: HP_Client; var piPending: Integer): BOOL; stdcall; external HPSocketDLL;
// * 获取连接的数据接收状态 */

function HP_Client_IsPauseReceive(pClient: HP_Client; var pbPaused: BOOL): BOOL; stdcall; external HPSocketDLL;
// * 检测是否有效连接 */

function HP_Client_IsConnected(pClient: HP_Client): BOOL; stdcall; external HPSocketDLL;
// * 设置内存块缓存池大小（通常设置为 -> PUSH 模型：5 - 10；PULL 模型：10 - 20 ） */

procedure HP_Client_SetFreeBufferPoolSize(pClient: HP_Client; dwFreeBufferPoolSize: DWORD); stdcall; external HPSocketDLL;
// * 设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍） */

procedure HP_Client_SetFreeBufferPoolHold(pClient: HP_Client; dwFreeBufferPoolHold: DWORD); stdcall; external HPSocketDLL;
// * 获取内存块缓存池大小 */

function HP_Client_GetFreeBufferPoolSize(pClient: HP_Client): DWORD; stdcall; external HPSocketDLL;
// * 获取内存块缓存池回收阀值 */

function HP_Client_GetFreeBufferPoolHold(pClient: HP_Client): DWORD; stdcall; external HPSocketDLL;

// **********************************************************************************/
// ******************************* TCP Client 操作方法 *******************************/

// *
{ * 名称：发送小文件
  * 描述：向服务端发送 4096 KB 以下的小文件
  *
  * 参数：		lpszFileName	-- 文件路径
  *			pHead			-- 头部附加数据
  *			pTail			-- 尾部附加数据
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取 Windows 错误代码
  */ }
function HP_TcpClient_SendSmallFile(pClient: HP_Client; lpszFileName: PChar; const pHead: LPWSABUF; const pTail: LPWSABUF): BOOL; stdcall; external HPSocketDLL;

// **********************************************************************************/
// ***************************** TCP Client 属性访问方法 *****************************/

// * 设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为：(N * 1024) - sizeof(TBufferObj)） */
procedure HP_TcpClient_SetSocketBufferSize(pClient: HP_TcpClient; dwSocketBufferSize: DWORD); stdcall; external HPSocketDLL;
// * 设置正常心跳包间隔（毫秒，0 则不发送心跳包，默认：30 * 1000） */

procedure HP_TcpClient_SetKeepAliveTime(pClient: HP_TcpClient; dwKeepAliveTime: DWORD); stdcall; external HPSocketDLL;
// * 设置异常心跳包间隔（毫秒，0 不发送心跳包，，默认：10 * 1000，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线） */

procedure HP_TcpClient_SetKeepAliveInterval(pClient: HP_TcpClient; dwKeepAliveInterval: DWORD); stdcall; external HPSocketDLL;

// * 获取通信数据缓冲区大小 */
function HP_TcpClient_GetSocketBufferSize(pClient: HP_TcpClient): DWORD; stdcall; external HPSocketDLL;
// * 获取正常心跳包间隔 */

function HP_TcpClient_GetKeepAliveTime(pClient: HP_TcpClient): DWORD; stdcall; external HPSocketDLL;
// * 获取异常心跳包间隔 */

function HP_TcpClient_GetKeepAliveInterval(pClient: HP_TcpClient): DWORD; stdcall; external HPSocketDLL;

// **********************************************************************************/
// ***************************** UDP Client 属性访问方法 *****************************/

// * 设置数据报文最大长度（建议在局域网环境下不超过 1472 字节，在广域网环境下不超过 548 字节） */
procedure HP_UdpClient_SetMaxDatagramSize(pClient: HP_UdpClient; dwMaxDatagramSize: DWORD); stdcall; external HPSocketDLL;
// * 获取数据报文最大长度 */

function HP_UdpClient_GetMaxDatagramSize(pClient: HP_UdpClient): DWORD; stdcall; external HPSocketDLL;
// * 设置监测包尝试次数（0 则不发送监测跳包，如果超过最大尝试次数则认为已断线） */

procedure HP_UdpClient_SetDetectAttempts(pClient: HP_UdpClient; dwDetectAttempts: DWORD); stdcall; external HPSocketDLL;
// * 设置监测包发送间隔（秒，0 不发送监测包） */

procedure HP_UdpClient_SetDetectInterval(pClient: HP_UdpClient; dwDetectInterval: DWORD); stdcall; external HPSocketDLL;
// * 获取心跳检查次数 */

function HP_UdpClient_GetDetectAttempts(pClient: HP_UdpClient): DWORD; stdcall; external HPSocketDLL;
// * 获取心跳检查间隔 */

function HP_UdpClient_GetDetectInterval(pClient: HP_UdpClient): DWORD; stdcall; external HPSocketDLL;

// **********************************************************************************/
// *************************** UDP ARQ Client 属性访问方法 ***************************/

// * 设置是否开启 nodelay 模式（默认：FALSE，不开启） */
procedure HP_UdpArqClient_SetNoDelay(pClient: HP_UdpArqClient; bNoDelay: Boolean); stdcall; external HPSocketDLL;
// * 设置是否关闭拥塞控制（默认：FALSE，不关闭） */

procedure HP_UdpArqClient_SetTurnoffCongestCtrl(pClient: HP_UdpArqClient; bTurnOff: Boolean); stdcall; external HPSocketDLL;
// * 设置数据刷新间隔（毫秒，默认：60） */

procedure HP_UdpArqClient_SetFlushInterval(pClient: HP_UdpArqClient; dwFlushInterval: DWORD); stdcall; external HPSocketDLL;
// * 设置快速重传 ACK 跨越次数（默认：0，关闭快速重传） */

procedure HP_UdpArqClient_SetResendByAcks(pClient: HP_UdpArqClient; dwResendByAcks: DWORD); stdcall; external HPSocketDLL;
// * 设置发送窗口大小（数据包数量，默认：128） */

procedure HP_UdpArqClient_SetSendWndSize(pClient: HP_UdpArqClient; dwSendWndSize: DWORD); stdcall; external HPSocketDLL;
// * 设置接收窗口大小（数据包数量，默认：512） */

procedure HP_UdpArqClient_SetRecvWndSize(pClient: HP_UdpArqClient; dwRecvWndSize: DWORD); stdcall; external HPSocketDLL;
// * 设置最小重传超时时间（毫秒，默认：30） */

procedure HP_UdpArqClient_SetMinRto(pClient: HP_UdpArqClient; dwMinRto: DWORD); stdcall; external HPSocketDLL;
// * 设置最大传输单元（默认：0，与 SetMaxDatagramSize(pClient:) 一致） */

procedure HP_UdpArqClient_SetMaxTransUnit(pClient: HP_UdpArqClient; dwMaxTransUnit: DWORD); stdcall; external HPSocketDLL;
// * 设置最大数据包大小（默认：4096） */

procedure HP_UdpArqClient_SetMaxMessageSize(pClient: HP_UdpArqClient; dwMaxMessageSize: DWORD); stdcall; external HPSocketDLL;
// * 设置握手超时时间（毫秒，默认：5000） */

procedure HP_UdpArqClient_SetHandShakeTimeout(pClient: HP_UdpArqClient; dwHandShakeTimeout: DWORD); stdcall; external HPSocketDLL;

// * 检测是否开启 nodelay 模式 */
function HP_UdpArqClient_IsNoDelay(pClient: HP_UdpArqClient): Boolean; stdcall; external HPSocketDLL;
// * 检测是否关闭拥塞控制 */

function HP_UdpArqClient_IsTurnoffCongestCtrl(pClient: HP_UdpArqClient): Boolean; stdcall; external HPSocketDLL;
// * 获取数据刷新间隔 */

function HP_UdpArqClient_GetFlushInterval(pClient: HP_UdpArqClient): DWORD; stdcall; external HPSocketDLL;
// * 获取快速重传 ACK 跨越次数 */

function HP_UdpArqClient_GetResendByAcks(pClient: HP_UdpArqClient): DWORD; stdcall; external HPSocketDLL;
// * 获取发送窗口大小 */

function HP_UdpArqClient_GetSendWndSize(pClient: HP_UdpArqClient): DWORD; stdcall; external HPSocketDLL;
// * 获取接收窗口大小 */

function HP_UdpArqClient_GetRecvWndSize(pClient: HP_UdpArqClient): DWORD; stdcall; external HPSocketDLL;
// * 获取最小重传超时时间 */

function HP_UdpArqClient_GetMinRto(pClient: HP_UdpArqClient): DWORD; stdcall; external HPSocketDLL;
// * 获取最大传输单元 */

function HP_UdpArqClient_GetMaxTransUnit(pClient: HP_UdpArqClient): DWORD; stdcall; external HPSocketDLL;
// * 获取最大数据包大小 */

function HP_UdpArqClient_GetMaxMessageSize(pClient: HP_UdpArqClient): DWORD; stdcall; external HPSocketDLL;
// * 获取握手超时时间 */

function HP_UdpArqClient_GetHandShakeTimeout(pClient: HP_UdpArqClient): DWORD; stdcall; external HPSocketDLL;

// * 获取等待发送包数量 */
function HP_UdpArqClient_GetWaitingSendMessageCount(pClient: HP_UdpArqClient; var piCount: Integer): Boolean; stdcall; external HPSocketDLL;

// **********************************************************************************/
// ****************************** UDP Cast 属性访问方法 ******************************/

// * 设置数据报文最大长度（建议在局域网环境下不超过 1472 字节，在广域网环境下不超过 548 字节） */
procedure HP_UdpCast_SetMaxDatagramSize(pCast: HP_UdpCast; dwMaxDatagramSize: DWORD); stdcall; external HPSocketDLL;
// * 获取数据报文最大长度 */

function HP_UdpCast_GetMaxDatagramSize(pCast: HP_UdpCast): DWORD; stdcall; external HPSocketDLL;
// * 获取当前数据报的远程地址信息（通常在 OnReceive 事件中调用） */

function HP_UdpCast_GetRemoteAddress(pCast: HP_UdpCast; lpszAddress: PChar; var piAddressLen: Integer; var pusPort: USHORT): BOOL; stdcall; external HPSocketDLL;
// * 设置是否启用地址重用机制（默认：不启用） */

procedure HP_UdpCast_SetReuseAddress(pCast: HP_UdpCast; bReuseAddress: BOOL); stdcall; external HPSocketDLL;
// * 检测是否启用地址重用机制 */

function HP_UdpCast_IsReuseAddress(pCast: HP_UdpCast): BOOL; stdcall; external HPSocketDLL;
// * 设置传播模式（组播或广播） */

procedure HP_UdpCast_SetCastMode(pCast: HP_UdpCast; enCastMode: En_HP_CastMode); stdcall; external HPSocketDLL;
// * 获取传播模式 */

function HP_UdpCast_GetCastMode(pCast: HP_UdpCast): En_HP_CastMode; stdcall; external HPSocketDLL;
// * 设置组播报文的 TTL（0 - 255） */

procedure HP_UdpCast_SetMultiCastTtl(pCast: HP_UdpCast; iMCTtl: Integer); stdcall; external HPSocketDLL;
// * 获取组播报文的 TTL */

function HP_UdpCast_GetMultiCastTtl(pCast: HP_UdpCast): Integer; stdcall; external HPSocketDLL;
// * 设置是否启用组播环路（TRUE or FALSE） */

procedure HP_UdpCast_SetMultiCastLoop(pCast: HP_UdpCast; bMCLoop: BOOL); stdcall; external HPSocketDLL;
// * 检测是否启用组播环路 */

function HP_UdpCast_IsMultiCastLoop(pCast: HP_UdpCast): BOOL; stdcall; external HPSocketDLL;

// ***************************************************************************************/
// ***************************** TCP Pull Server 组件操作方法 *****************************/

// *
{ * 名称：抓取数据
  * 描述：用户通过该方法从 Socket 组件中抓取数据
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pData		-- 抓取缓冲区
  *			iLength		-- 抓取数据长度
  * 返回值：	En_HP_FetchResult
  */ }
function HP_TcpPullServer_Fetch(pServer: HP_TcpPullServer; dwConnID: HP_CONNID; pData: PBYTE; iLength: Integer): En_HP_FetchResult; stdcall; external HPSocketDLL;

{ //*
  * 名称：窥探数据（不会移除缓冲区数据）
  * 描述：用户通过该方法从 Socket 组件中窥探数据
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pData		-- 窥探缓冲区
  *			iLength		-- 窥探数据长度
  * 返回值：	En_HP_FetchResult
  */ }
function HP_TcpPullServer_Peek(pServer: HP_TcpPullServer; dwConnID: HP_CONNID; pData: PBYTE; iLength: Integer): En_HP_FetchResult; stdcall; external HPSocketDLL;

// ***************************************************************************************/
// ***************************** TCP Pull Server 属性访问方法 *****************************/

// ***************************************************************************************/
// ***************************** TCP Pull Agent 组件操作方法 *****************************/

{ //*
  * 名称：抓取数据
  * 描述：用户通过该方法从 Socket 组件中抓取数据
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pData		-- 抓取缓冲区
  *			iLength		-- 抓取数据长度
  * 返回值：	En_HP_FetchResult
  */ }
function HP_TcpPullAgent_Fetch(pAgent: HP_TcpPullAgent; dwConnID: HP_CONNID; pData: PBYTE; iLength: Integer): En_HP_FetchResult; stdcall; external HPSocketDLL;

{ //*
  * 名称：窥探数据（不会移除缓冲区数据）
  * 描述：用户通过该方法从 Socket 组件中窥探数据
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pData		-- 窥探缓冲区
  *			iLength		-- 窥探数据长度
  * 返回值：	En_HP_FetchResult
  */ }
function HP_TcpPullAgent_Peek(pAgent: HP_TcpPullAgent; dwConnID: HP_CONNID; pData: PBYTE; iLength: Integer): En_HP_FetchResult; stdcall; external HPSocketDLL;

// ***************************************************************************************/
// ***************************** TCP Pull Agent 属性访问方法 *****************************/

// ***************************************************************************************/
// ***************************** TCP Pull Client 组件操作方法 *****************************/

{ //*
  * 名称：抓取数据
  * 描述：用户通过该方法从 Socket 组件中抓取数据
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pData		-- 抓取缓冲区
  *			iLength		-- 抓取数据长度
  * 返回值：	En_HP_FetchResult
  */ }
function HP_TcpPullClient_Fetch(pClient: HP_TcpPullClient; pData: PBYTE; iLength: Integer): En_HP_FetchResult; stdcall; external HPSocketDLL;

{ //*
  * 名称：窥探数据（不会移除缓冲区数据）
  * 描述：用户通过该方法从 Socket 组件中窥探数据
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pData		-- 窥探缓冲区
  *			iLength		-- 窥探数据长度
  * 返回值：	En_HP_FetchResult
  */ }
function HP_TcpPullClient_Peek(pClient: HP_TcpPullClient; pData: PBYTE; iLength: Integer): En_HP_FetchResult; stdcall; external HPSocketDLL;

// ***************************************************************************************/
// ***************************** TCP Pull Client 属性访问方法 *****************************/

// ***************************************************************************************/
// ***************************** TCP Pack Server 组件操作方法 *****************************/

// ***************************************************************************************/
// ***************************** TCP Pack Server 属性访问方法 *****************************/

// * 设置数据包最大长度（有效数据包最大长度不能超过 4194303/0x3FFFFF 字节，默认：262144/0x40000） */
procedure HP_TcpPackServer_SetMaxPackSize(pServer: HP_TcpPackServer; dwMaxPackSize: DWORD); stdcall; external HPSocketDLL;
// * 设置包头标识（有效包头标识取值范围 0 ~ 1023/0x3FF，当包头标识为 0 时不校验包头，默认：0） */

procedure HP_TcpPackServer_SetPackHeaderFlag(pServer: HP_TcpPackServer; usPackHeaderFlag: DWORD); stdcall; external HPSocketDLL;

// * 获取数据包最大长度 */
function HP_TcpPackServer_GetMaxPackSize(pServer: HP_TcpPackServer): DWORD; stdcall; external HPSocketDLL;
// * 获取包头标识 */

function HP_TcpPackServer_GetPackHeaderFlag(pServer: HP_TcpPackServer): DWORD; stdcall; external HPSocketDLL;

// ***************************************************************************************/
// ***************************** TCP Pack Agent 组件操作方法 *****************************/

// ***************************************************************************************/
// ***************************** TCP Pack Agent 属性访问方法 *****************************/

// * 设置数据包最大长度（有效数据包最大长度不能超过 4194303/0x3FFFFF 字节，默认：262144/0x40000） */
procedure HP_TcpPackAgent_SetMaxPackSize(pAgent: HP_TcpPackAgent; dwMaxPackSize: DWORD); stdcall; external HPSocketDLL;
// * 设置包头标识（有效包头标识取值范围 0 ~ 1023/0x3FF，当包头标识为 0 时不校验包头，默认：0） */

procedure HP_TcpPackAgent_SetPackHeaderFlag(pAgent: HP_TcpPackAgent; usPackHeaderFlag: DWORD); stdcall; external HPSocketDLL;

// * 获取数据包最大长度 */
function HP_TcpPackAgent_GetMaxPackSize(pAgent: HP_TcpPackAgent): DWORD; stdcall; external HPSocketDLL;
// * 获取包头标识 */

function HP_TcpPackAgent_GetPackHeaderFlag(pAgent: HP_TcpPackAgent): DWORD; stdcall; external HPSocketDLL;

// ***************************************************************************************/
// ***************************** TCP Pack Client 组件操作方法 *****************************/

// ***************************************************************************************/
// ***************************** TCP Pack Client 属性访问方法 *****************************/

// * 设置数据包最大长度（有效数据包最大长度不能超过 4194303/0x3FFFFF 字节，默认：262144/0x40000） */
procedure HP_TcpPackClient_SetMaxPackSize(pClient: HP_TcpPackClient; dwMaxPackSize: DWORD); stdcall; external HPSocketDLL;
// * 设置包头标识（有效包头标识取值范围 0 ~ 1023/0x3FF，当包头标识为 0 时不校验包头，默认：0） */

procedure HP_TcpPackClient_SetPackHeaderFlag(pClient: HP_TcpPackClient; usPackHeaderFlag: DWORD); stdcall; external HPSocketDLL;

// * 获取数据包最大长度 */
function HP_TcpPackClient_GetMaxPackSize(pClient: HP_TcpPackClient): DWORD; stdcall; external HPSocketDLL;
// * 获取包头标识 */

function HP_TcpPackClient_GetPackHeaderFlag(pClient: HP_TcpPackClient): DWORD; stdcall; external HPSocketDLL;

// *************************************************************************/
// *************** Global Function Exports *********************************/
// *************************************************************************/
// 获取 HPSocket 版本号（4 个字节分别为：主版本号，子版本号，修正版本号，构建编号）
function HP_GetHPSocketVersion(): LongWord; stdcall; external HPSocketDLL;

// * 获取错误描述文本 */
function HP_GetSocketErrorDesc(enCode: En_HP_SocketError): PChar; stdcall; external HPSocketDLL;
// * 调用系统的 GetLastError() 方法获取系统错误代码 */

function SYS_GetLastError(): DWORD; stdcall; external HPSocketDLL;
// 调用系统的 WSAGetLastError() 方法获取系统错误代码

function SYS_WSAGetLastError(): Integer; stdcall; external HPSocketDLL;
// 调用系统的 setsockopt()

function SYS_SetSocketOption(sock: SOCKET; level: Integer; name: Integer; val: Pointer; len: Integer): Integer; stdcall; external HPSocketDLL;
// 调用系统的 getsockopt()

function SYS_GetSocketOption(sock: SOCKET; level: Integer; name: Integer; val: Pointer; var len: Pointer): Integer; stdcall; external HPSocketDLL;
// 调用系统的 ioctlsocket()

function SYS_IoctlSocket(sock: SOCKET; cmd: Longint; var arg: LongWord): Integer; stdcall; external HPSocketDLL;
// 调用系统的 WSAIoctl()

function SYS_WSAIoctl(sock: SOCKET; dwIoControlCode: DWORD; lpvInBuffer: Pointer; cbInBuffer: DWORD; lpvOutBuffer: Pointer; cbOutBuffer: DWORD; lpcbBytesReturned: LPDWORD): Integer; stdcall; external HPSocketDLL;

// 设置 socket 选项：IPPROTO_TCP -> TCP_NODELAY
function SYS_SSO_NoDelay(sock: SOCKET; bNoDelay: BOOL): Integer; stdcall; external HPSocketDLL;
// 设置 socket 选项：SOL_SOCKET -> SO_DONTLINGER

function SYS_SSO_DontLinger(sock: SOCKET; bDont: BOOL): Integer; stdcall; external HPSocketDLL;
// 设置 socket 选项：SOL_SOCKET -> SO_LINGER

function SYS_SSO_Linger(sock: SOCKET; l_onoff: USHORT; l_linger: USHORT): Integer; stdcall; external HPSocketDLL;
// 设置 socket 选项：SOL_SOCKET -> SO_RCVBUF

function SYS_SSO_RecvBuffSize(sock: SOCKET; SIZE: Integer): Integer; stdcall; external HPSocketDLL;
// 设置 socket 选项：SOL_SOCKET -> SO_SNDBUF

function SYS_SSO_SendBuffSize(sock: SOCKET; SIZE: Integer): Integer; stdcall; external HPSocketDLL;
// 设置 socket 选项：SOL_SOCKET -> SO_REUSEADDR

function SYS_SSO_ReuseAddress(sock: SOCKET; bReuse: BOOL): Integer; stdcall; external HPSocketDLL;
// 设置 socket 选项：SOL_SOCKET -> SO_EXCLUSIVEADDRUSE

function SYS_SSO_ExclusiveAddressUse(sock: SOCKET; bExclusive: BOOL): Integer; stdcall; external HPSocketDLL;

// 获取 SOCKET 本地地址信息
function SYS_GetSocketLocalAddress(SOCKET: SOCKET; lpszAddress: array of Char; var piAddressLen: Integer; var pusPort: USHORT): BOOL; stdcall; external HPSocketDLL;
// 获取 SOCKET 远程地址信息

function SYS_GetSocketRemoteAddress(SOCKET: SOCKET; lpszAddress: array of Char; var piAddressLen: Integer; var pusPort: USHORT): BOOL; stdcall; external HPSocketDLL;

// * 枚举主机 IP 地址 */
function SYS_EnumHostIPAddresses(lpszHost: PChar; enType: En_HP_IPAddrType; var lpppIPAddr: HP_LPTIPAddr; var piIPAddrCount: Integer): BOOL; stdcall; external HPSocketDLL;
// * 释放 HP_LPTIPAddr* */

function SYS_FreeHostIPAddresses(var lppIPAddr: HP_LPTIPAddr): BOOL; stdcall; external HPSocketDLL;
// * 检查字符串是否符合 IP 地址格式 */

function SYS_IsIPAddress(lpszAddress: PChar; var penType: En_HP_IPAddrType): BOOL; stdcall; external HPSocketDLL;
// * 通过主机名获取 IP 地址 */

function SYS_GetIPAddress(lpszHost: PChar; lpszIP: array of Char; var piIPLenth: Integer; var penType: En_HP_IPAddrType): BOOL; stdcall; external HPSocketDLL;

// * 64 位网络字节序转主机字节序 */
function SYS_NToH64(value: LongWord): LongWord; stdcall; external HPSocketDLL;
// * 64 位主机字节序转网络字节序 */

function SYS_HToN64(value: LongWord): LongWord; stdcall; external HPSocketDLL;

// * 分配内存 */
function SYS_Malloc(SIZE: Integer): PBYTE; stdcall; external HPSocketDLL;
// * 重新分配内存 */

function SYS_Realloc(p: PBYTE; SIZE: Integer): PBYTE; stdcall; external HPSocketDLL;
// * 释放内存 */

procedure SYS_Free(p: PBYTE); stdcall; external HPSocketDLL;

// ===================================下面是编码函数
// CP_XXX -> UNICODE
function SYS_CodePageToUnicode(iCodePage: Integer; const szSrc: PAnsiChar; szDest: PWideChar; var piDestLength: Integer): BOOL; stdcall; external HPSocketDLL;
// UNICODE -> CP_XXX

function SYS_UnicodeToCodePage(iCodePage: Integer; const szSrc: PWideChar; szDest: PAnsiChar; var piDestLength: Integer): BOOL; stdcall; external HPSocketDLL;
// GBK -> UNICODE

function SYS_GbkToUnicode(const szSrc: PAnsiChar; szDest: PWideChar; var piDestLength: Integer): BOOL; stdcall; external HPSocketDLL;
// UNICODE -> GBK

function SYS_UnicodeToGbk(const szSrc: PWideChar; szDest: PAnsiChar; var piDestLength: Integer): BOOL; stdcall; external HPSocketDLL;
// UTF8 -> UNICODE

function SYS_Utf8ToUnicode(const szSrc: PAnsiChar; szDest: PWideChar; var piDestLength: Integer): BOOL; stdcall; external HPSocketDLL;
// UNICODE -> UTF8

function SYS_UnicodeToUtf8(const szSrc: PWideChar; szDest: PAnsiChar; var piDestLength: Integer): BOOL; stdcall; external HPSocketDLL;
// GBK -> UTF8

function SYS_GbkToUtf8(const szSrc: PAnsiChar; szDest: PAnsiChar; var piDestLength: Integer): BOOL; stdcall; external HPSocketDLL;
// UTF8 -> GBK

function SYS_Utf8ToGbk(const szSrc: PAnsiChar; szDest: PAnsiChar; var piDestLength: Integer): BOOL; stdcall; external HPSocketDLL;

// 普通压缩
function SYS_Compress(const lpszSrc: PChar; dwSrcLen: LongWord; var lpszDest: PChar; var pdwDestLen: LongWord): Integer; stdcall; external HPSocketDLL;
// 高级压缩（默认值：iLevel -> -1，iMethod -> 8，iWindowBits -> 15，iMemLevel -> 8，iStrategy -> 0）

function SYS_CompressEx(const lpszSrc: PChar; dwSrcLen: LongWord; var lpszDest: PChar; var pdwDestLen: LongWord; iLevel: Integer; iMethod: Integer; iWindowBits: Integer; iMemLevel: Integer; iStrategy: Integer): Integer; stdcall; external HPSocketDLL;
// 普通解压

function SYS_Uncompress(const lpszSrc: PChar; dwSrcLen: LongWord; var lpszDest: PChar; var pdwDestLen: LongWord): Integer; stdcall; external HPSocketDLL;
// 高级解压（默认值：iWindowBits -> 15）

function SYS_UncompressEx(const lpszSrc: PChar; dwSrcLen: LongWord; var lpszDest: PChar; var pdwDestLen: LongWord; iWindowBits: Integer): Integer; stdcall; external HPSocketDLL;
// 推测压缩结果长度

function SYS_GuessCompressBound(dwSrcLen: LongWord; bGZip: BOOL): LongWord; stdcall; external HPSocketDLL;

// Gzip 压缩
function SYS_GZipCompress(const lpszSrc: PChar; dwSrcLen: LongWord; var lpszDest: PChar; var pdwDestLen: LongWord): Integer; stdcall; external HPSocketDLL;
// Gzip 解压

function SYS_GZipUncompress(const lpszSrc: PChar; dwSrcLen: LongWord; var lpszDest: PChar; var pdwDestLen: LongWord): Integer; stdcall; external HPSocketDLL;
// 推测 Gzip 解压结果长度（如果返回 0 或不合理值则说明输入内容并非有效的 Gzip 格式）

function SYS_GZipGuessUncompressBound(const lpszSrc: PChar; dwSrcLen: LongWord): LongWord; stdcall; external HPSocketDLL;

// 计算 Base64 编码后长度
function SYS_GuessBase64EncodeBound(dwSrcLen: LongWord): LongWord; stdcall; external HPSocketDLL;
// 计算 Base64 解码后长度

function SYS_GuessBase64DecodeBound(const lpszSrc: PChar; dwSrcLen: LongWord): LongWord; stdcall; external HPSocketDLL;
// Base64 编码（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）

function SYS_Base64Encode(const lpszSrc: PChar; dwSrcLen: LongWord; var lpszDest: PChar; var pdwDestLen: LongWord): Integer; stdcall; external HPSocketDLL;
// Base64 解码（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）

function SYS_Base64Decode(const lpszSrc: PChar; dwSrcLen: LongWord; var lpszDest: PChar; var pdwDestLen: LongWord): Integer; stdcall; external HPSocketDLL;

// 计算 URL 编码后长度
function SYS_GuessUrlEncodeBound(const lpszSrc: PChar; dwSrcLen: LongWord): LongWord; stdcall; external HPSocketDLL;
// 计算 URL 解码后长度

function SYS_GuessUrlDecodeBound(const lpszSrc: PChar; dwSrcLen: LongWord): LongWord; stdcall; external HPSocketDLL;
// URL 编码（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）

function SYS_UrlEncode(lpszSrc: PChar; dwSrcLen: LongWord; var lpszDest: PChar; var pdwDestLen: LongWord): Integer; stdcall; external HPSocketDLL;
// URL 解码（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）

function SYS_UrlDecode(lpszSrc: PChar; dwSrcLen: LongWord; var lpszDest: PChar; var pdwDestLen: LongWord): Integer; stdcall; external HPSocketDLL;


// *****************************************************************************************************************************************************/
// ******************************************************************** HTTP Exports *******************************************************************/
// *****************************************************************************************************************************************************/

// ****************************************************/
// ******************* HTTP 回调函数 *******************/

type
  // * HTTP 回调函数 */
  HP_FN_Http_OnMessageBegin = function(pSender: HP_Http; dwConnID: HP_CONNID): En_HP_HttpParseResult; stdcall;

  HP_FN_Http_OnRequestLine = function(pSender: HP_Http; dwConnID: HP_CONNID; lpszMethod: PChar; lpszUrl: PChar): En_HP_HttpParseResult; stdcall;

  HP_FN_Http_OnStatusLine = function(pSender: HP_Http; dwConnID: HP_CONNID; usStatusCode: USHORT; lpszDesc: PChar): En_HP_HttpParseResult; stdcall;

  HP_FN_Http_OnHeader = function(pSender: HP_Http; dwConnID: HP_CONNID; lpszName: PChar; lpszValue: PChar): En_HP_HttpParseResult; stdcall;

  HP_FN_Http_OnHeadersComplete = function(pSender: HP_Http; dwConnID: HP_CONNID): En_HP_HttpParseResult; stdcall;

  HP_FN_Http_OnBody = function(pSender: HP_Http; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer): En_HP_HttpParseResult; stdcall;

  HP_FN_Http_OnChunkHeader = function(pSender: HP_Http; dwConnID: HP_CONNID; iLength: Integer): En_HP_HttpParseResult; stdcall;

  HP_FN_Http_OnChunkComplete = function(pSender: HP_Http; dwConnID: HP_CONNID): En_HP_HttpParseResult; stdcall;

  HP_FN_Http_OnMessageComplete = function(pSender: HP_Http; dwConnID: HP_CONNID): En_HP_HttpParseResult; stdcall;

  HP_FN_Http_OnUpgrade = function(pSender: HP_Http; dwConnID: HP_CONNID; enUpgradeType: En_HP_HttpUpgradeType): En_HP_HttpParseResult; stdcall;

  HP_FN_Http_OnParseError = function(pSender: HP_Http; dwConnID: HP_CONNID; iErrorCode: Integer; lpszErrorDesc: PChar): En_HP_HttpParseResult; stdcall;

  HP_FN_Http_OnWSMessageHeader = function(pSender: HP_Http; dwConnID: HP_CONNID; bFinal: BOOL; iReserved: byte; iOperationCode: byte; const lpszMask: array of byte; ullBodyLen: LongWord): En_HP_HandleResult; stdcall;

  HP_FN_Http_OnWSMessageBody = function(pSender: HP_Http; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer): En_HP_HandleResult; stdcall;

  HP_FN_Http_OnWSMessageComplete = function(pSender: HP_Http; dwConnID: HP_CONNID): En_HP_HandleResult; stdcall;

  { /* HTTP Server 回调函数 */ }
  HP_FN_HttpServer_OnMessageBegin = HP_FN_Http_OnMessageBegin;

  HP_FN_HttpServer_OnRequestLine = HP_FN_Http_OnRequestLine;

  HP_FN_HttpServer_OnHeader = HP_FN_Http_OnHeader;

  HP_FN_HttpServer_OnHeadersComplete = HP_FN_Http_OnHeadersComplete;

  HP_FN_HttpServer_OnBody = HP_FN_Http_OnBody;

  HP_FN_HttpServer_OnChunkHeader = HP_FN_Http_OnChunkHeader;

  HP_FN_HttpServer_OnChunkComplete = HP_FN_Http_OnChunkComplete;

  HP_FN_HttpServer_OnMessageComplete = HP_FN_Http_OnMessageComplete;

  HP_FN_HttpServer_OnUpgrade = HP_FN_Http_OnUpgrade;

  HP_FN_HttpServer_OnParseError = HP_FN_Http_OnParseError;

  HP_FN_HttpServer_OnWSMessageHeader = HP_FN_Http_OnWSMessageHeader;

  HP_FN_HttpServer_OnWSMessageBody = HP_FN_Http_OnWSMessageBody;

  HP_FN_HttpServer_OnWSMessageComplete = HP_FN_Http_OnWSMessageComplete;

  HP_FN_HttpServer_OnPrepareListen = HP_FN_Server_OnPrepareListen;

  HP_FN_HttpServer_OnAccept = HP_FN_Server_OnAccept;

  HP_FN_HttpServer_OnHandShake = HP_FN_Server_OnHandShake;

  HP_FN_HttpServer_OnReceive = HP_FN_Server_OnReceive;

  HP_FN_HttpServer_OnSend = HP_FN_Server_OnSend;

  HP_FN_HttpServer_OnClose = HP_FN_Server_OnClose;

  HP_FN_HttpServer_OnShutdown = HP_FN_Server_OnShutdown;

  { /* HTTP Agent 回调函数 */ }
  HP_FN_HttpAgent_OnMessageBegin = HP_FN_Http_OnMessageBegin;

  HP_FN_HttpAgent_OnStatusLine = HP_FN_Http_OnStatusLine;

  HP_FN_HttpAgent_OnHeader = HP_FN_Http_OnHeader;

  HP_FN_HttpAgent_OnHeadersComplete = HP_FN_Http_OnHeadersComplete;

  HP_FN_HttpAgent_OnBody = HP_FN_Http_OnBody;

  HP_FN_HttpAgent_OnChunkHeader = HP_FN_Http_OnChunkHeader;

  HP_FN_HttpAgent_OnChunkComplete = HP_FN_Http_OnChunkComplete;

  HP_FN_HttpAgent_OnMessageComplete = HP_FN_Http_OnMessageComplete;

  HP_FN_HttpAgent_OnUpgrade = HP_FN_Http_OnUpgrade;

  HP_FN_HttpAgent_OnParseError = HP_FN_Http_OnParseError;

  HP_FN_HttpAgent_OnWSMessageHeader = HP_FN_Http_OnWSMessageHeader;

  HP_FN_HttpAgent_OnWSMessageBody = HP_FN_Http_OnWSMessageBody;

  HP_FN_HttpAgent_OnWSMessageComplete = HP_FN_Http_OnWSMessageComplete;

  HP_FN_HttpAgent_OnPrepareConnect = HP_FN_Agent_OnPrepareConnect;

  HP_FN_HttpAgent_OnConnect = HP_FN_Agent_OnConnect;

  HP_FN_HttpAgent_OnHandShake = HP_FN_Agent_OnHandShake;

  HP_FN_HttpAgent_OnReceive = HP_FN_Agent_OnReceive;

  HP_FN_HttpAgent_OnSend = HP_FN_Agent_OnSend;

  HP_FN_HttpAgent_OnClose = HP_FN_Agent_OnClose;

  HP_FN_HttpAgent_OnShutdown = HP_FN_Agent_OnShutdown;

  { /* HTTP Client 回调函数 */ }
  HP_FN_HttpClient_OnMessageBegin = HP_FN_Http_OnMessageBegin;

  HP_FN_HttpClient_OnStatusLine = HP_FN_Http_OnStatusLine;

  HP_FN_HttpClient_OnHeader = HP_FN_Http_OnHeader;

  HP_FN_HttpClient_OnHeadersComplete = HP_FN_Http_OnHeadersComplete;

  HP_FN_HttpClient_OnBody = HP_FN_Http_OnBody;

  HP_FN_HttpClient_OnChunkHeader = HP_FN_Http_OnChunkHeader;

  HP_FN_HttpClient_OnChunkComplete = HP_FN_Http_OnChunkComplete;

  HP_FN_HttpClient_OnMessageComplete = HP_FN_Http_OnMessageComplete;

  HP_FN_HttpClient_OnUpgrade = HP_FN_Http_OnUpgrade;

  HP_FN_HttpClient_OnParseError = HP_FN_Http_OnParseError;

  HP_FN_HttpClient_OnWSMessageHeader = HP_FN_Http_OnWSMessageHeader;

  HP_FN_HttpClient_OnWSMessageBody = HP_FN_Http_OnWSMessageBody;

  HP_FN_HttpClient_OnWSMessageComplete = HP_FN_Http_OnWSMessageComplete;

  HP_FN_HttpClient_OnPrepareConnect = HP_FN_Client_OnPrepareConnect;

  HP_FN_HttpClient_OnConnect = HP_FN_Client_OnConnect;

  HP_FN_HttpClient_OnHandShake = HP_FN_Client_OnHandShake;

  HP_FN_HttpClient_OnReceive = HP_FN_Client_OnReceive;

  HP_FN_HttpClient_OnSend = HP_FN_Client_OnSend;

  HP_FN_HttpClient_OnClose = HP_FN_Client_OnClose;

  // ****************************************************/
  // ***************** HTTP 对象创建函数 *****************/

  // 创建 HP_HttpServer 对象
function Create_HP_HttpServer(pListener: HP_HttpServerListener): HP_HttpServer; stdcall; external HPSocketDLL;
// 创建 HP_HttpAgent 对象

function Create_HP_HttpAgent(pListener: HP_HttpAgentListener): HP_HttpAgent; stdcall; external HPSocketDLL;
// 创建 HP_HttpClient 对象

function Create_HP_HttpClient(pListener: HP_HttpClientListener): HP_HttpClient; stdcall; external HPSocketDLL;
// 创建 HP_HttpClient 对象

function Create_HP_HttpSyncClient(pListener: HP_HttpClientListener): HP_HttpSyncClient; stdcall; external HPSocketDLL;

// 销毁 HP_HttpServer 对象
procedure Destroy_HP_HttpServer(pServer: HP_HttpServer); stdcall; external HPSocketDLL;
// 销毁 HP_HttpAgent 对象

procedure Destroy_HP_HttpAgent(pAgent: HP_HttpAgent); stdcall; external HPSocketDLL;
// 销毁 HP_HttpClient 对象

procedure Destroy_HP_HttpClient(pClient: HP_HttpClient); stdcall; external HPSocketDLL;
// 销毁 HP_HttpSyncClient 对象

procedure Destroy_HP_HttpSyncClient(pClient: HP_HttpSyncClient); stdcall; external HPSocketDLL;

// 创建 HP_HttpServerListener 对象
function Create_HP_HttpServerListener(): HP_HttpServerListener; stdcall; external HPSocketDLL;
// 创建 HP_HttpAgentListener 对象

function Create_HP_HttpAgentListener(): HP_HttpAgentListener; stdcall; external HPSocketDLL;
// 创建 HP_HttpClientListener 对象

function Create_HP_HttpClientListener(): HP_HttpClientListener; stdcall; external HPSocketDLL;

// 销毁 HP_HttpServerListener 对象
procedure Destroy_HP_HttpServerListener(pListener: HP_HttpServerListener); stdcall; external HPSocketDLL;
// 销毁 HP_HttpAgentListener 对象

procedure Destroy_HP_HttpAgentListener(pListener: HP_HttpAgentListener); stdcall; external HPSocketDLL;
// 销毁 HP_HttpClientListener 对象

procedure Destroy_HP_HttpClientListener(pListener: HP_HttpClientListener); stdcall; external HPSocketDLL;

// **********************************************************************************/
// *************************** HTTP Server 回调函数设置方法 **************************/

procedure HP_Set_FN_HttpServer_OnMessageBegin(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnMessageBegin); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnRequestLine(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnRequestLine); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnHeader(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnHeader); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnHeadersComplete(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnHeadersComplete); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnBody(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnBody); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnChunkHeader(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnChunkHeader); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnChunkComplete(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnChunkComplete); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnMessageComplete(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnMessageComplete); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnUpgrade(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnUpgrade); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnParseError(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnParseError); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnWSMessageHeader(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnWSMessageHeader); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnWSMessageBody(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnWSMessageBody); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnWSMessageComplete(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnWSMessageComplete); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnPrepareListen(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnPrepareListen); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnAccept(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnAccept); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnHandShake(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnHandShake); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnReceive(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnReceive); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnSend(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnSend); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnClose(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnClose); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpServer_OnShutdown(pListener: HP_HttpServerListener; fn: HP_FN_HttpServer_OnShutdown); stdcall; external HPSocketDLL;

// **********************************************************************************/
// **************************** HTTP Agent 回调函数设置方法 **************************/

procedure HP_Set_FN_HttpAgent_OnMessageBegin(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnMessageBegin); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnStatusLine(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnStatusLine); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnHeader(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnHeader); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnHeadersComplete(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnHeadersComplete); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnBody(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnBody); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnChunkHeader(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnChunkHeader); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnChunkComplete(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnChunkComplete); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnMessageComplete(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnMessageComplete); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnUpgrade(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnUpgrade); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnParseError(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnParseError); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnWSMessageHeader(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnWSMessageHeader); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnWSMessageBody(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnWSMessageBody); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnWSMessageComplete(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnWSMessageComplete); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnPrepareConnect(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnPrepareConnect); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnConnect(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnConnect); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnHandShake(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnHandShake); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnReceive(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnReceive); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnSend(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnSend); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnClose(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnClose); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpAgent_OnShutdown(pListener: HP_HttpAgentListener; fn: HP_FN_HttpAgent_OnShutdown); stdcall; external HPSocketDLL;

// **********************************************************************************/
// *************************** HTTP Client 回调函数设置方法 **************************/

procedure HP_Set_FN_HttpClient_OnMessageBegin(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnMessageBegin); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnStatusLine(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnStatusLine); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnHeader(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnHeader); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnHeadersComplete(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnHeadersComplete); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnBody(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnBody); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnChunkHeader(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnChunkHeader); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnChunkComplete(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnChunkComplete); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnMessageComplete(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnMessageComplete); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnUpgrade(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnUpgrade); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnParseError(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnParseError); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnWSMessageHeader(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnWSMessageHeader); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnWSMessageBody(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnWSMessageBody); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnWSMessageComplete(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnWSMessageComplete); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnPrepareConnect(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnPrepareConnect); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnConnect(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnConnect); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnHandShake(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnHandShake); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnReceive(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnReceive); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnSend(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnSend); stdcall; external HPSocketDLL;

procedure HP_Set_FN_HttpClient_OnClose(pListener: HP_HttpClientListener; fn: HP_FN_HttpClient_OnClose); stdcall; external HPSocketDLL;

// **************************************************************************/
// ********************** HTTP  Server 操作方法 *****************************/

// *
{ * 名称：回复请求
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
  */ }
function HP_HttpServer_SendResponse(pServer: HP_HttpServer; dwConnID: HP_CONNID; usStatusCode: USHORT; lpszDesc: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer; const pData: PBYTE; iLength: Integer): BOOL; stdcall; external HPSocketDLL;

{ //*
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
function HP_HttpServer_SendLocalFile(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszFileName: PChar; usStatusCode: USHORT; lpszDesc: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer): BOOL; stdcall; external HPSocketDLL;

{ /*
  * 名称：发送 Chunked 数据分片
  * 描述：向对端发送 Chunked 数据分片
  *		
  * 参数：		dwConnID		-- 连接 ID
  *			pData			-- Chunked 数据分片
  *			iLength			-- 数据分片长度（为 0 表示结束分片）
  *			lpszExtensions	-- 扩展属性（默认：nullptr）
  * 返回值：	TRUE			-- 成功
  *			FALSE			-- 失败
  */ }
 function HP_HttpServer_SendChunkData(pServer: HP_HttpServer; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer; lpszExtensions: PChar): BOOL; stdcall; external HPSocketDLL;

{ /*
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
  */ }
function HP_HttpServer_SendWSMessage(pServer: HP_HttpServer; dwConnID: HP_CONNID; bFinal: BOOL; iReserved: byte; iOperationCode: byte; const pData: PBYTE; iLength: Integer; ullBodyLen: LongWord): BOOL; stdcall; external HPSocketDLL;

{ //*
  * 名称：释放连接
  * 描述：把连接放入释放队列，等待某个时间（通过 SetReleaseDelay() 设置）关闭连接
  *
  * 参数：		dwConnID		-- 连接 ID
  * 返回值：	TRUE			-- 成功
  *			FALSE			-- 失败
  */ }
function HP_HttpServer_Release(pServer: HP_HttpServer; dwConnID: HP_CONNID): BOOL; stdcall; external HPSocketDLL;

{ /*
  * 名称：启动 HTTP 通信
  * 描述：当通信组件设置为非自动启动 HTTP 通信时，需要调用本方法启动 HTTP 通信
  *
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
  */ }
function HP_HttpServer_StartHttp(pServer: HP_HttpServer; dwConnID: HP_CONNID): BOOL; stdcall; external HPSocketDLL;


// ******************************************************************************/
// ***************************** Server 属性访问方法 *****************************/

// * 设置连接释放延时（默认：3000 毫秒） */
procedure HP_HttpServer_SetReleaseDelay(pServer: HP_HttpServer; dwReleaseDelay: DWORD); stdcall; external HPSocketDLL;
// * 获取连接释放延时 */

function HP_HttpServer_GetReleaseDelay(pServer: HP_HttpServer): DWORD; stdcall; external HPSocketDLL;
// * 获取请求行 URL 域掩码（URL 域参考：EnHttpUrlField） */

function HP_HttpServer_GetUrlFieldSet(pServer: HP_HttpServer; dwConnID: HP_CONNID): DWORD; stdcall; external HPSocketDLL;
// * 获取某个 URL 域值 */

function HP_HttpServer_GetUrlField(pServer: HP_HttpServer; dwConnID: HP_CONNID; enField: En_HP_HttpUrlField): PChar; stdcall; external HPSocketDLL;
// * 获取请求方法 */

function HP_HttpServer_GetMethod(pServer: HP_HttpServer; dwConnID: HP_CONNID): PChar; stdcall; external HPSocketDLL;

// * 设置本地协议版本 */
procedure HP_HttpServer_SetLocalVersion(pServer: HP_HttpServer; usVersion: En_HP_HttpVersion); stdcall; external HPSocketDLL;
// * 获取本地协议版本 */

function HP_HttpServer_GetLocalVersion(pServer: HP_HttpServer): En_HP_HttpVersion; stdcall; external HPSocketDLL;

// * 检查是否升级协议 */
function HP_HttpServer_IsUpgrade(pServer: HP_HttpServer; dwConnID: HP_CONNID): BOOL; stdcall; external HPSocketDLL;
// * 检查是否有 Keep-Alive 标识 */

function HP_HttpServer_IsKeepAlive(pServer: HP_HttpServer; dwConnID: HP_CONNID): BOOL; stdcall; external HPSocketDLL;
// * 获取协议版本 */

function HP_HttpServer_GetVersion(pServer: HP_HttpServer; dwConnID: HP_CONNID): USHORT; stdcall; external HPSocketDLL;
// * 获取主机 */

function HP_HttpServer_GetHost(pServer: HP_HttpServer; dwConnID: HP_CONNID): PChar; stdcall; external HPSocketDLL;
// * 获取内容长度 */

function HP_HttpServer_GetContentLength(pServer: HP_HttpServer; dwConnID: HP_CONNID): LongWord; stdcall; external HPSocketDLL;
// * 获取内容类型 */

function HP_HttpServer_GetContentType(pServer: HP_HttpServer; dwConnID: HP_CONNID): PChar; stdcall; external HPSocketDLL;
// * 获取内容编码 */

function HP_HttpServer_GetContentEncoding(pServer: HP_HttpServer; dwConnID: HP_CONNID): PChar; stdcall; external HPSocketDLL;
// * 获取传输编码 */

function HP_HttpServer_GetTransferEncoding(pServer: HP_HttpServer; dwConnID: HP_CONNID): PChar; stdcall; external HPSocketDLL;
// * 获取协议升级类型 */

function HP_HttpServer_GetUpgradeType(pServer: HP_HttpServer; dwConnID: HP_CONNID): En_HP_HttpUpgradeType; stdcall; external HPSocketDLL;
// * 获取解析错误代码 */

function HP_HttpServer_GetParseErrorCode(pServer: HP_HttpServer; dwConnID: HP_CONNID; var lpszErrorDesc: PChar): USHORT; stdcall; external HPSocketDLL;

// * 获取某个请求头（单值） */
function HP_HttpServer_GetHeader(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszName: PChar; var lpszValue: PChar): BOOL; stdcall; external HPSocketDLL;
// * 获取某个请求头（多值） */

function HP_HttpServer_GetHeaders(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszName: PChar; lpszValue: array of PChar; var pdwCount: DWORD): BOOL; stdcall; external HPSocketDLL;
// * 获取所有请求头 */

function HP_HttpServer_GetAllHeaders(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpHeaders: HP_THeaderAry; var pdwCount: DWORD): BOOL; stdcall; external HPSocketDLL;
// * 获取所有请求头名称 */

function HP_HttpServer_GetAllHeaderNames(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszName: array of PChar; var pdwCount: DWORD): BOOL; stdcall; external HPSocketDLL;

// * 获取 Cookie */
function HP_HttpServer_GetCookie(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpszName: PChar; var lpszValue: PChar): BOOL; stdcall; external HPSocketDLL;
// * 获取所有 Cookie */

function HP_HttpServer_GetAllCookies(pServer: HP_HttpServer; dwConnID: HP_CONNID; lpCookies: HP_TCookieAry; var pdwCount: Pointer): BOOL; stdcall; external HPSocketDLL;
// * 获取当前 WebSocket 消息状态，传入 nullptr 则不获取相应字段 */

function HP_HttpServer_GetWSMessageState(pServer: HP_HttpServer; dwConnID: HP_CONNID; var lpbFinal: BOOL; var lpiReserved: byte; var lpiOperationCode: byte; var lpszMask: PBYTE; var lpullBodyLen: LongWord; var lpullBodyRemain: LongWord): BOOL; stdcall; external HPSocketDLL;

// * 设置 HTTP 启动方式（默认：TRUE，自动启动） */
procedure HP_HttpServer_SetHttpAutoStart(pServer: HP_HttpServer; bAutoStart: BOOL); stdcall; external HPSocketDLL;
// * 获取 HTTP 启动方式 */

function HP_HttpServer_IsHttpAutoStart(pServer: HP_HttpServer): BOOL; stdcall; external HPSocketDLL;

// **************************************************************************/
// ********************** HTTP  Agent 操作方法 ******************************/

// *
{ * 名称：发送请求
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
  */ }
function HP_HttpAgent_SendRequest(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszMethod: PChar; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer; const pData: PBYTE; iLength: Integer): BOOL; stdcall; external HPSocketDLL;

{ //*
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
  */ }
function HP_HttpAgent_SendLocalFile(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszFileName: PChar; lpszMethod: PChar; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer): BOOL; stdcall; external HPSocketDLL;

{ /*
  * 名称：发送 Chunked 数据分片
  * 描述：向对端发送 Chunked 数据分片
  *		
  * 参数：		dwConnID		-- 连接 ID
  *			pData			-- Chunked 数据分片
  *			iLength			-- 数据分片长度（为 0 表示结束分片）
  *			lpszExtensions	-- 扩展属性（默认：nullptr）
  * 返回值：	TRUE			-- 成功
  *			FALSE			-- 失败
  */ }
 function HP_HttpAgent_SendChunkData(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer; lpszExtensions: PChar): BOOL; stdcall; external HPSocketDLL;

// * 发送 POST 请求 */
function HP_HttpAgent_SendPost(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer; const pBody: PBYTE; iLength: Integer): BOOL; stdcall; external HPSocketDLL;
// * 发送 PUT 请求 */

function HP_HttpAgent_SendPut(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer; const pBody: PBYTE; iLength: Integer): BOOL; stdcall; external HPSocketDLL;
// * 发送 PATCH 请求 */

function HP_HttpAgent_SendPatch(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer; const pBody: PBYTE; iLength: Integer): BOOL; stdcall; external HPSocketDLL;
// * 发送 GET 请求 */

function HP_HttpAgent_SendGet(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer): BOOL; stdcall; external HPSocketDLL;
// * 发送 DELETE 请求 */

function HP_HttpAgent_SendDelete(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer): BOOL; stdcall; external HPSocketDLL;
// * 发送 HEAD 请求 */

function HP_HttpAgent_SendHead(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer): BOOL; stdcall; external HPSocketDLL;
// * 发送 TRACE 请求 */

function HP_HttpAgent_SendTrace(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer): BOOL; stdcall; external HPSocketDLL;
// * 发送 OPTIONS 请求 */

function HP_HttpAgent_SendOptions(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer): BOOL; stdcall; external HPSocketDLL;
// * 发送 CONNECT 请求 */

function HP_HttpAgent_SendConnect(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszHost: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer): BOOL; stdcall; external HPSocketDLL;
{ /*
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
  */ }

function HP_HttpAgent_SendWSMessage(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; bFinal: BOOL; iReserved: byte; iOperationCode: byte; const lpszMask: array of byte; const pData: PBYTE; iLength: Integer; ullBodyLen: LongWord): BOOL; stdcall; external HPSocketDLL;

{ /*
  * 名称：启动 HTTP 通信
  * 描述：当通信组件设置为非自动启动 HTTP 通信时，需要调用本方法启动 HTTP 通信
  *
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
  */ }
function HP_HttpAgent_StartHttp(pAgent: HP_HttpAgent; dwConnID: HP_CONNID): BOOL; stdcall; external HPSocketDLL;

// ******************************************************************************/
// ********************** HTTP  Agent 属性访问方法 ******************************/

// * 获取 HTTP 状态码 */
function HP_HttpAgent_GetStatusCode(pAgent: HP_HttpAgent; dwConnID: HP_CONNID): USHORT; stdcall; external HPSocketDLL;

// * 设置本地协议版本 */
procedure HP_HttpAgent_SetLocalVersion(pAgent: HP_HttpAgent; usVersion: En_HP_HttpVersion); stdcall; external HPSocketDLL;
// * 获取本地协议版本 */

function HP_HttpAgent_GetLocalVersion(pAgent: HP_HttpAgent): En_HP_HttpVersion; stdcall; external HPSocketDLL;

// * 检查是否升级协议 */
function HP_HttpAgent_IsUpgrade(pAgent: HP_HttpAgent; dwConnID: HP_CONNID): BOOL; stdcall; external HPSocketDLL;
// * 检查是否有 Keep-Alive 标识 */

function HP_HttpAgent_IsKeepAlive(pAgent: HP_HttpAgent; dwConnID: HP_CONNID): BOOL; stdcall; external HPSocketDLL;
// * 获取协议版本 */

function HP_HttpAgent_GetVersion(pAgent: HP_HttpAgent; dwConnID: HP_CONNID): USHORT; stdcall; external HPSocketDLL;
// * 获取内容长度 */

function HP_HttpAgent_GetContentLength(pAgent: HP_HttpAgent; dwConnID: HP_CONNID): LongWord; stdcall; external HPSocketDLL;
// * 获取内容类型 */

function HP_HttpAgent_GetContentType(pAgent: HP_HttpAgent; dwConnID: HP_CONNID): PChar; stdcall; external HPSocketDLL;
// * 获取内容编码 */

function HP_HttpAgent_GetContentEncoding(pAgent: HP_HttpAgent; dwConnID: HP_CONNID): PChar; stdcall; external HPSocketDLL;
// * 获取传输编码 */

function HP_HttpAgent_GetTransferEncoding(pAgent: HP_HttpAgent; dwConnID: HP_CONNID): PChar; stdcall; external HPSocketDLL;

// * 获取协议升级类型 */
function HP_HttpAgent_GetUpgradeType(pAgent: HP_HttpAgent; dwConnID: HP_CONNID): En_HP_HttpUpgradeType; stdcall; external HPSocketDLL;
// * 获取解析错误代码 */

function HP_HttpAgent_GetParseErrorCode(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; var lpszErrorDesc: PChar): USHORT; stdcall; external HPSocketDLL;

// * 获取某个请求头（单值） */
function HP_HttpAgent_GetHeader(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszName: PChar; var lpszValue: PChar): BOOL; stdcall; external HPSocketDLL;
// * 获取某个请求头（多值） */

function HP_HttpAgent_GetHeaders(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszName: PChar; lpszValue: array of PChar; var pdwCount: DWORD): BOOL; stdcall; external HPSocketDLL;
// * 获取所有请求头 */

function HP_HttpAgent_GetAllHeaders(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpHeaders: HP_THeaderAry; var pdwCount: DWORD): BOOL; stdcall; external HPSocketDLL;
// * 获取所有请求头名称 */

function HP_HttpAgent_GetAllHeaderNames(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszName: array of PChar; var pdwCount: DWORD): BOOL; stdcall; external HPSocketDLL;

// * 设置是否使用 Cookie */
procedure HP_HttpAgent_SetUseCookie(pAgent: HP_HttpAgent; bUseCookie: BOOL); stdcall; external HPSocketDLL;
// * 检查是否使用 Cookie */

function HP_HttpAgent_IsUseCookie(pAgent: HP_HttpAgent): BOOL; stdcall; external HPSocketDLL;
// * 获取 Cookie */

function HP_HttpAgent_GetCookie(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpszName: PChar; var lpszValue: PChar): BOOL; stdcall; external HPSocketDLL;
// * 获取所有 Cookie */

function HP_HttpAgent_GetAllCookies(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; lpCookies: HP_TCookieAry; var pdwCount: DWORD): BOOL; stdcall; external HPSocketDLL;

// * 获取当前 WebSocket 消息状态，传入 nullptr 则不获取相应字段 */

function HP_HttpAgent_GetWSMessageState(pAgent: HP_HttpAgent; dwConnID: HP_CONNID; var lpbFinal: BOOL; var lpiReserved: byte; var lpiOperationCode: byte; var lpszMask: PBYTE; var lpullBodyLen: LongWord; var lpullBodyRemain: LongWord): BOOL; stdcall; external HPSocketDLL;

// * 设置 HTTP 启动方式（默认：TRUE，自动启动） */
procedure HP_HttpAgent_SetHttpAutoStart(pAgent: HP_HttpAgent; bAutoStart: BOOL); stdcall; external HPSocketDLL;
// * 获取 HTTP 启动方式 */

function HP_HttpAgent_IsHttpAutoStart(pAgent: HP_HttpAgent): BOOL; stdcall; external HPSocketDLL;

// **************************************************************************/
// *********************** HTTP Client 操作方法 *****************************/

// *
{ * 名称：发送请求
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
  */ }
function HP_HttpClient_SendRequest(pClient: HP_HttpClient; lpszMethod: PChar; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer; const pBody: PBYTE; iLength: Integer): BOOL; stdcall; external HPSocketDLL;

{ //*
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
  */ }
function HP_HttpClient_SendLocalFile(pClient: HP_HttpClient; lpszFileName: PChar; lpszMethod: PChar; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer): BOOL; stdcall; external HPSocketDLL;

{ /*
  * 名称：发送 Chunked 数据分片
  * 描述：向对端发送 Chunked 数据分片
  *		
  * 参数：		dwConnID		-- 连接 ID
  *			pData			-- Chunked 数据分片
  *			iLength			-- 数据分片长度（为 0 表示结束分片）
  *			lpszExtensions	-- 扩展属性（默认：nullptr）
  * 返回值：	TRUE			-- 成功
  *			FALSE			-- 失败
  */ }
 function HP_HttpClient_SendChunkData(pClient: HP_HttpClient; const pData: PBYTE; iLength: Integer; lpszExtensions: PChar): BOOL; stdcall; external HPSocketDLL;

// * 发送 POST 请求 */
function HP_HttpClient_SendPost(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer; const pBody: PBYTE; iLength: Integer): BOOL; stdcall; external HPSocketDLL;
// * 发送 PUT 请求 */

function HP_HttpClient_SendPut(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer; const pBody: PBYTE; iLength: Integer): BOOL; stdcall; external HPSocketDLL;
// * 发送 PATCH 请求 */

function HP_HttpClient_SendPatch(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer; const pBody: PBYTE; iLength: Integer): BOOL; stdcall; external HPSocketDLL;
// * 发送 GET 请求 */

function HP_HttpClient_SendGet(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: TNVPair; iHeaderCount: Integer): BOOL; stdcall; external HPSocketDLL;
// * 发送 DELETE 请求 */

function HP_HttpClient_SendDelete(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer): BOOL; stdcall; external HPSocketDLL;
// * 发送 HEAD 请求 */

function HP_HttpClient_SendHead(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer): BOOL; stdcall; external HPSocketDLL;
// * 发送 TRACE 请求 */

function HP_HttpClient_SendTrace(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer): BOOL; stdcall; external HPSocketDLL;
// * 发送 OPTIONS 请求 */

function HP_HttpClient_SendOptions(pClient: HP_HttpClient; lpszPath: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer): BOOL; stdcall; external HPSocketDLL;
// * 发送 CONNECT 请求 */

function HP_HttpClient_SendConnect(pClient: HP_HttpClient; lpszHost: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer): BOOL; stdcall; external HPSocketDLL;
{ /*
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
  */ }

function HP_HttpClient_SendWSMessage(pClient: HP_HttpClient; bFinal: BOOL; iReserved: byte; iOperationCode: byte; const lpszMask: array of byte; const pData: PBYTE; iLength: Integer; ullBodyLen: LongWord): BOOL; stdcall; external HPSocketDLL;

{ /*
  * 名称：启动 HTTP 通信
  * 描述：当通信组件设置为非自动启动 HTTP 通信时，需要调用本方法启动 HTTP 通信
  *
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
  */ }
function HP_HttpClient_StartHttp(pClient: HP_HttpClient): BOOL; stdcall; external HPSocketDLL;

// ******************************************************************************/
// ********************** HTTP Client 属性访问方法 *****************************/

// * 获取 HTTP 状态码 */
function HP_HttpClient_GetStatusCode(pClient: HP_HttpClient): USHORT; stdcall; external HPSocketDLL;

// * 设置本地协议版本 */
procedure HP_HttpClient_SetLocalVersion(pClient: HP_HttpClient; usVersion: En_HP_HttpVersion); stdcall; external HPSocketDLL;
// * 获取本地协议版本 */

function HP_HttpClient_GetLocalVersion(pClient: HP_HttpClient): En_HP_HttpVersion; stdcall; external HPSocketDLL;

// * 检查是否升级协议 */
function HP_HttpClient_IsUpgrade(pClient: HP_HttpClient): BOOL; stdcall; external HPSocketDLL;
// * 检查是否有 Keep-Alive 标识 */

function HP_HttpClient_IsKeepAlive(pClient: HP_HttpClient): BOOL; stdcall; external HPSocketDLL;
// * 获取协议版本 */

function HP_HttpClient_GetVersion(pClient: HP_HttpClient): USHORT; stdcall; external HPSocketDLL;
// * 获取内容长度 */

function HP_HttpClient_GetContentLength(pClient: HP_HttpClient): LongWord; stdcall; external HPSocketDLL;
// * 获取内容类型 */

function HP_HttpClient_GetContentType(pClient: HP_HttpClient): PChar; stdcall; external HPSocketDLL;
// * 获取内容编码 */

function HP_HttpClient_GetContentEncoding(pClient: HP_HttpClient): PChar; stdcall; external HPSocketDLL;
// * 获取传输编码 */

function HP_HttpClient_GetTransferEncoding(pClient: HP_HttpClient): PChar; stdcall; external HPSocketDLL;
// * 获取协议升级类型 */

function HP_HttpClient_GetUpgradeType(pClient: HP_HttpClient): En_HP_HttpUpgradeType; stdcall; external HPSocketDLL;
// * 获取解析错误代码 */

function HP_HttpClient_GetParseErrorCode(pClient: HP_HttpClient; var lpszErrorDesc: PChar): USHORT; stdcall; external HPSocketDLL;

// * 获取某个请求头（单值） */
function HP_HttpClient_GetHeader(pClient: HP_HttpClient; lpszName: PChar; var lpszValue: PChar): BOOL; stdcall; external HPSocketDLL;
// * 获取某个请求头（多值） */

function HP_HttpClient_GetHeaders(pClient: HP_HttpClient; lpszName: PChar; lpszValue: array of PChar; var pdwCount: DWORD): BOOL; stdcall; external HPSocketDLL;
// * 获取所有请求头 */

function HP_HttpClient_GetAllHeaders(pClient: HP_HttpClient; lpHeaders: HP_THeaderAry; var pdwCount: DWORD): BOOL; stdcall; external HPSocketDLL;
// * 获取所有请求头名称 */

function HP_HttpClient_GetAllHeaderNames(pClient: HP_HttpClient; lpszName: array of PChar; var pdwCount: DWORD): BOOL; stdcall; external HPSocketDLL;

// * 设置是否使用 Cookie */
procedure HP_HttpClient_SetUseCookie(pClient: HP_HttpClient; bUseCookie: BOOL); stdcall; external HPSocketDLL;
// * 检查是否使用 Cookie */

function HP_HttpClient_IsUseCookie(pClient: HP_HttpClient): BOOL; stdcall; external HPSocketDLL;
// * 获取 Cookie */

function HP_HttpClient_GetCookie(pClient: HP_HttpClient; lpszName: PChar; var lpszValue: PChar): BOOL; stdcall; external HPSocketDLL;
// * 获取所有 Cookie */

function HP_HttpClient_GetAllCookies(pClient: HP_HttpClient; lpCookies: HP_TCookieAry; var pdwCount: DWORD): BOOL; stdcall; external HPSocketDLL;

// * 获取当前 WebSocket 消息状态，传入 nullptr 则不获取相应字段 */
function HP_HttpClient_GetWSMessageState(pClient: HP_HttpClient; var lpbFinal: BOOL; var lpiReserved: byte; var lpiOperationCode: byte; var lpszMask: PBYTE; var lpullBodyLen: LongWord; var lpullBodyRemain: LongWord): BOOL; stdcall; external HPSocketDLL;

// * 设置 HTTP 启动方式（默认：TRUE，自动启动） */
procedure HP_HttpClient_SetHttpAutoStart(pClient: HP_HttpClient; bAutoStart: BOOL); stdcall; external HPSocketDLL;
// * 获取 HTTP 启动方式 */

function HP_HttpClient_IsHttpAutoStart(pClient: HP_HttpClient): BOOL; stdcall; external HPSocketDLL;

{ /**************************************************************************/
  /************************ HTTP Sync Client 操作方法 ************************/ }
{ /*
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
  */ }
function HP_HttpSyncClient_OpenUrl(pClient: HP_HttpSyncClient; lpszMethod: PChar; lpszUrl: PChar; const lpHeaders: HP_THeaderAry; iHeaderCount: Integer; const pBody: PBYTE; iLength: Integer; bForceReconnect: BOOL): BOOL; stdcall; external HPSocketDLL;
{ /*
  * 名称：清除请求结果
  * 描述：清除上一次请求的响应头和响应体等结果信息（该方法会在每次发送请求前自动调用）
  *
  * 参数：
  * 返回值：	TRUE			-- 成功
  *			FALSE			-- 失败
  */ }

function HP_HttpSyncClient_CleanupRequestResult(pClient: HP_HttpSyncClient): BOOL; stdcall; external HPSocketDLL;

{ /******************************************************************************/
  /************************ HTTP Sync Client 属性访问方法 ************************/ }

// * 设置连接超时（毫秒，0：系统默认超时，默认：5000） */
procedure HP_HttpSyncClient_SetConnectTimeout(pClient: HP_HttpSyncClient; dwConnectTimeout: DWORD); stdcall; external HPSocketDLL;
// * 设置请求超时（毫秒，0：无限等待，默认：10000） */

procedure HP_HttpSyncClient_SetRequestTimeout(pClient: HP_HttpSyncClient; dwRequestTimeout: DWORD); stdcall; external HPSocketDLL;

// * 获取连接超时 */
function HP_HttpSyncClient_GetConnectTimeout(pClient: HP_HttpSyncClient): DWORD; stdcall; external HPSocketDLL;
// * 获取请求超时 */

function HP_HttpSyncClient_GetRequestTimeout(pClient: HP_HttpSyncClient): DWORD; stdcall; external HPSocketDLL;

// * 获取响应体 */
function HP_HttpSyncClient_GetResponseBody(pClient: HP_HttpSyncClient; var lpszBody: PBYTE; piLength: Pointer): BOOL; stdcall; external HPSocketDLL;

// **************************************************************************/
// *************************** HTTP Cookie 管理方法 **************************/

// * 从文件加载 Cookie */
function HP_HttpCookie_MGR_LoadFromFile(lpszFile: PChar; bKeepExists: BOOL = True): BOOL; stdcall; external HPSocketDLL;
// * 保存 Cookie 到文件 */

function HP_HttpCookie_MGR_SaveToFile(lpszFile: PChar; bKeepExists: BOOL = True): BOOL; stdcall; external HPSocketDLL;
// * 清理 Cookie */

function HP_HttpCookie_MGR_ClearCookies(lpszDomain: PChar = nil; lpszPath: PChar = nil): BOOL; stdcall; external HPSocketDLL;
// * 清理过期 Cookie */

function HP_HttpCookie_MGR_RemoveExpiredCookies(lpszDomain: PChar = nil; lpszPath: PChar = nil): BOOL; stdcall; external HPSocketDLL;
// * 设置 Cookie */

function HP_HttpCookie_MGR_SetCookie(lpszName: PChar; lpszValue: PChar; lpszDomain: PChar; lpszPath: PChar; iMaxAge: Integer = -1; bHttpOnly: BOOL = False; bSecure: BOOL = False; enSameSite: Integer = 0; bOnlyUpdateValueIfExists: BOOL = True): BOOL; stdcall; external HPSocketDLL;
// * 删除 Cookie */

function HP_HttpCookie_MGR_DeleteCookie(lpszDomain: PChar; lpszPath: PChar; lpszName: PChar): BOOL; stdcall; external HPSocketDLL;
// * 设置是否允许第三方 Cookie */

function HP_HttpCookie_MGR_SetEnableThirdPartyCookie(bEnableThirdPartyCookie: BOOL = True): BOOL; stdcall; external HPSocketDLL;
// * 检查是否允许第三方 Cookie */

function HP_HttpCookie_MGR_IsEnableThirdPartyCookie(): BOOL; stdcall; external HPSocketDLL;

// * Cookie expires 字符串转换为整数 */
function HP_HttpCookie_HLP_ParseExpires(lpszExpires: PChar; var ptmExpires: LongWord): BOOL; stdcall; external HPSocketDLL;
// * 整数转换为 Cookie expires 字符串 */

function HP_HttpCookie_HLP_MakeExpiresStr(lpszBuff: array of Char; var piBuffLen: PInteger; tmExpires: Integer): Boolean; stdcall; external HPSocketDLL;
// * 生成 Cookie 字符串 */

function HP_HttpCookie_HLP_ToString(lpszBuff: array of Char; var piBuffLen: Integer; lpszName: PChar; lpszValue: PChar; lpszDomain: PChar; lpszPath: PChar; iMaxAge: Integer = -1; bHttpOnly: BOOL = False; bSecure: BOOL = False; enSameSite: Integer = 0): BOOL; stdcall; external HPSocketDLL;

// * 获取当前 UTC 时间 */
function HP_HttpCookie_HLP_CurrentUTCTime(): Int64; stdcall; external HPSocketDLL;
// * Max-Age -> expires */

function HP_HttpCookie_HLP_MaxAgeToExpires(iMaxAge: Integer): Int64; stdcall; external HPSocketDLL;
// * expires -> Max-Age */

function HP_HttpCookie_HLP_ExpiresToMaxAge(tmExpires: Int64): Integer; stdcall; external HPSocketDLL;

{ /*****************************************************************************************************************************************************/
  /**************************************************************** Thread Pool Exports ****************************************************************/
  /*****************************************************************************************************************************************************/ }

{ /****************************************************/
  /******************* 对象创建函数 ********************/ }

// 创建 IHPThreadPool 对象
function Create_HP_ThreadPool(): HP_ThreadPool; stdcall; external HPSocketDLL;
// 销毁 IHPThreadPool 对象

procedure Destroy_HP_ThreadPool(pThreadPool: HP_ThreadPool); stdcall; external HPSocketDLL;

{ /*
  * 名称：创建 TSocketTask 对象
  * 描述：创建任务对象，该对象最终需由 HP_Destroy_SocketTaskObj() 销毁
  *
  * 参数：		fnTaskProc	-- 任务处理函数
  *			pSender		-- 发起对象
  *			dwConnID	-- 连接 ID
  *			pBuffer		-- 数据缓冲区
  *			iBuffLen	-- 数据缓冲区长度
  *			enBuffType	-- 数据缓冲区类型（默认：TBT_COPY）
  *							TBT_COPY	：（深拷贝）pBuffer 复制到 TSocketTask 对象。此后 TSocketTask 对象与 pBuffer 不再有任何关联
  *											-> 适用于 pBuffer 不大或 pBuffer 生命周期不受控的场景
  *							TBT_REFER	：（浅拷贝）pBuffer 不复制到 TSocketTask 对象，需确保 TSocketTask 对象生命周期内 pBuffer 必须有效
  *											-> 适用于 pBuffer 较大或 pBuffer 可重用，并且 pBuffer 生命周期受控的场景
  *							TBT_ATTACH	：（附属）执行浅拷贝，但 TSocketTask 对象会获得 pBuffer 的所有权，并负责释放 pBuffer，避免多次缓冲区拷贝
  *											-> 注意：pBuffer 必须由 SYS_Malloc() 函数分配才能使用本类型，否则可能会发生内存访问错误
  *			wParam		-- 自定义参数
  *			lParam		-- 自定义参数
  * 返回值：	HP_LPTSocketTask
  */ }
function Create_HP_SocketTaskObj(fnTaskProc: Fn_SocketTaskProc; pSender: PVOID; dwConnID: HP_CONNID; pBuffer: PBYTE; iBuffLen: Integer; enBuffType: En_HP_TaskBufferType = TBT_COPY; wParam: wParam = 0; lParam: lParam = 0): HP_LPTSocketTask; stdcall; external HPSocketDLL;

// 销毁 TSocketTask 对象
procedure Destroy_HP_SocketTaskObj(pTask: HP_LPTSocketTask); stdcall; external HPSocketDLL;

{ /***********************************************************************/
  /***************************** 组件操作方法 *****************************/ }

{ /*
  * 名称：启动线程池组件
  * 描述：
  *
  * 参数：		dwThreadCount		-- 线程数量，（默认：0）
  *									>0 -> dwThreadCount
  *									=0 -> (CPU核数 * 2 + 2)
  *									<0 -> (CPU核数 * (-dwThreadCount))
  *			dwMaxQueueSize		-- 任务队列最大容量（默认：0，不限制）
  *			enRejectedPolicy	-- 任务拒绝处理策略
  *									TRP_CALL_FAIL（默认）	：立刻返回失败
  *									TRP_WAIT_FOR			：等待（直到成功、超时或线程池关闭等原因导致失败）
  *									TRP_CALLER_RUN			：调用者线程直接执行
  *			dwStackSize			-- 线程堆栈空间大小（默认：0 -> 操作系统默认）
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取系统错误代码
  */ }
function HP_ThreadPool_Start(pThreadPool: HP_ThreadPool; dwThreadCount: DWORD = 0; dwMaxQueueSize: DWORD = 0; enRejectedPolicy: En_HP_RejectedPolicy = TRP_CALL_FAIL; dwStackSize: DWORD = 0): BOOL; stdcall; external HPSocketDLL;

{ /*
  * 名称：关闭线程池组件
  * 描述：在规定时间内关闭线程池组件，如果工作线程在最大等待时间内未能正常关闭，会尝试强制关闭，这种情况下很可能会造成系统资源泄漏
  *
  * 参数：		dwMaxWait	-- 最大等待时间（毫秒，默认：INFINITE，一直等待）
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取系统错误代码
  */ }
function HP_ThreadPool_Stop(pThreadPool: HP_ThreadPool; dwMaxWait: DWORD = INFINITE): BOOL; stdcall; external HPSocketDLL;

{ /*
  * 名称：提交任务
  * 描述：向线程池提交异步任务
  *
  * 参数：		fnTaskProc	-- 任务处理函数
  *			pvArg		-- 任务参数
  *			dwMaxWait	-- 任务提交最大等待时间（仅对 TRP_WAIT_FOR 类型线程池生效，默认：INFINITE，一直等待）
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取系统错误代码
  *							其中，错误码 ERROR_DESTINATION_ELEMENT_FULL 表示任务队列已满
  */ }
function HP_ThreadPool_Submit(pThreadPool: HP_ThreadPool; fnTaskProc: HP_Fn_TaskProc; pvArg: PVOID; dwMaxWait: DWORD = INFINITE): BOOL; stdcall; external HPSocketDLL;

{ /*
  * 名称：提交 Socket 任务
  * 描述：向线程池提交异步 Socket 任务
  *
  * 参数：		pTask		-- 任务参数
  *			dwMaxWait	-- 任务提交最大等待时间（仅对 TRP_WAIT_FOR 类型线程池生效，默认：INFINITE，一直等待）
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取系统错误代码
  *							其中，错误码 ERROR_DESTINATION_ELEMENT_FULL 表示任务队列已满
  *							注意：如果提交失败，需要手工调用 Destroy_HP_SocketTaskObj() 销毁 TSocketTask 对象
  */ }
function HP_ThreadPool_Submit_Task(pThreadPool: HP_ThreadPool; pTask: HP_LPTSocketTask; dwMaxWait: DWORD = INFINITE): BOOL; stdcall; external HPSocketDLL;

{ /*
  * 名称：调整线程池大小
  * 描述：增加或减少线程池的工作线程数量
  *
  * 参数：		dwNewThreadCount	-- 线程数量
  *									>0 -> dwNewThreadCount
  *									=0 -> (CPU核数 * 2 + 2)
  *									<0 -> (CPU核数 * (-dwNewThreadCount))
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 SYS_GetLastError() 获取系统错误代码
  */ }
function HP_ThreadPool_AdjustThreadCount(pThreadPool: HP_ThreadPool; dwNewThreadCount: DWORD): BOOL; stdcall; external HPSocketDLL;

{ /***********************************************************************/
  /***************************** 属性访问方法 *****************************/ }

// * 检查线程池组件是否已启动 */
function HP_ThreadPool_HasStarted(pThreadPool: HP_ThreadPool): BOOL; stdcall; external HPSocketDLL;
// * 查看线程池组件当前状态 */

function HP_ThreadPool_GetState(pThreadPool: HP_ThreadPool): En_HP_ServiceState; stdcall; external HPSocketDLL;
// * 获取当前任务队列大小 */

function HP_ThreadPool_GetQueueSize(pThreadPool: HP_ThreadPool): DWORD; stdcall; external HPSocketDLL;
// * 获取工作线程数量 */

function HP_ThreadPool_GetThreadCount(pThreadPool: HP_ThreadPool): DWORD; stdcall; external HPSocketDLL;
// * 获取任务队列最大容量 */

function HP_ThreadPool_GetMaxQueueSize(pThreadPool: HP_ThreadPool): DWORD; stdcall; external HPSocketDLL;
// * 获取任务拒绝处理策略 */

function HP_ThreadPool_GetRejectedPolicy(pThreadPool: HP_ThreadPool): En_HP_RejectedPolicy; stdcall; external HPSocketDLL;

implementation

initialization
  IsMultiThread := True;

end.

