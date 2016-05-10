//                            _ooOoo_
//                           o8888888o
//                           88" . "88
//                           (| -_- |)
//                            O\ = /O
//                        ____/`---'\____
//                      .   ' \\| |// `.
//                       / \\||| : |||// \
//                     / _||||| -:- |||||- \
//                       | | \\\ - /// | |
//                     | \_| ''\---/'' | |
//                      \ .-\__ `-` ___/-. /
//                   ___`. .' /--.--\ `. . __
//                ."" '< `.___\_<|>_/___.' >'"".
//               | | : `- \`.;`\ _ /`;.`/ - ` : | |
//                 \ \ `-. \_ __\ /__ _/ .-` / /
//         ======`-.____`-.___\_____/___.-`____.-'======
//                            `=---='
//
//         .............................................
//                  佛祖镇楼                  BUG辟易
//          佛曰:
//                  写字楼里写字间，写字间里程序员；
//                  程序人员写程序，又拿程序换酒钱。
//                  酒醒只在网上坐，酒醉还来网下眠；
//                  酒醉酒醒日复日，网上网下年复年。
//                  但愿老死电脑间，不愿鞠躬老板前；
//                  奔驰宝马贵者趣，公交自行程序员。
//                  别人笑我忒疯癫，我笑自己命太贱；
//                  不见满街漂亮妹，哪个归得程序员？
unit HPSocketSDKUnit;

interface

uses
  Windows, SysUtils,DateUtils,StrUtils;

const
  HPSocketDLL = 'HPSocket4C_U.dll';

type
{$Z4}
  SOCKET = Pointer;

  WSABUF = Record
    len: ULONG; { the length of the buffer }
    buf: PChar; { the pointer to the buffer }
  end { WSABUF };

  PWSABUF = ^WSABUF;
  LPWSABUF = PWSABUF;

  WSABUFArray = array of WSABUF;

  PInteger = ^Integer;
  PUShort = ^Word;
  { /************************************************************************
    名称：连接 ID 数据类型
    描述：定义连接 ID 的数据类型
    ************************************************************************/ }
  HP_CONNID = DWORD;
  PHP_CONNID = ^HP_CONNID;
  HP_CONNIDArray = array of HP_CONNID; { TODO : 这个数组变量用来获取所有连接id }
  { /************************************************************************
    名称：定义 Socket 对象指针类型别名
    描述：把 Socket 对象指针定义为更直观的别名
    ************************************************************************/ }
  HP_Object = longint;

  HP_Server = HP_Object;
  HP_Agent = HP_Object;
  HP_Client = HP_Object;
  HP_TcpServer = HP_Object;
  HP_HttpServer = HP_Object;
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
  HP_HttpServerListener = HP_Object;
  HP_TcpAgentListener = HP_Object;
  HP_TcpClientListener = HP_Object;
  HP_PullSocketListener = HP_Object;
  HP_PullClientListener = HP_Object;
  HP_TcpPullServerListener = HP_Object;
  HP_TcpPullAgentListener = HP_Object;
  HP_TcpPullClientListener = HP_Object;
  HP_UdpServerListener = HP_Object;
  HP_UdpClientListener = HP_Object;
  HP_UdpCastListener = HP_Object;

  {******************************************** 公共类、接口 ********************************************/}

  {TODO:/************************************************************************
    名称：通信组件服务状态
    描述：应用程序可以通过通信组件的 GetState() 方法获取组件当前服务状态
    ************************************************************************ }
  En_HP_ServiceState = (
    HP_SS_STARTING = 0, // 正在启动
    HP_SS_STARTED = 1, // 已经启动
    HP_SS_STOPING = 2, // 正在停止
    HP_SS_STOPED = 3 // 已经停止
    );

  { TODO:************************************************************************
    名称：Socket 操作类型
    描述：应用程序的 OnErrror() 事件中通过该参数标识是哪种操作导致的错误
    ************************************************************************ }
  En_HP_SocketOperation = (
    HP_SO_UNKNOWN = 0, // Unknown
    HP_SO_ACCEPT = 1, // Acccept
    HP_SO_CONNECT = 2, // Connnect
    HP_SO_SEND = 3, // Send
    HP_SO_RECEIVE = 4, // Receive
    HP_SO_CLOSE		= 5 // Close
    );

  {TODO: ************************************************************************
    名称：事件通知处理结果
    描述：事件通知的返回值，不同的返回值会影响通信组件的后续行为
    ************************************************************************ }
  En_HP_HandleResult = (
    HP_HR_OK = 0, // 成功处理
    HP_HR_IGNORE = 1, // 忽略处理
    HP_HR_ERROR = 2 // 错误处理
    );

  { TODO:/************************************************************************
    名称：数据抓取结果
    描述：数据抓取操作的返回值
    ************************************************************************/ }
  En_HP_FetchResult = (
    HP_FR_OK = 0, // 成功
    HP_FR_LENGTH_TOO_LONG = 1, // 抓取长度过大
    HP_FR_DATA_NOT_FOUND = 2 // 找不到 ConnID 对应的数据
    );

  { TODO:/************************************************************************
    名称：数据发送策略
    描述：Server 组件和 Agent 组件的数据发送策略

    * 打包模式（默认）	：尽量把多个发送操作的数据组合在一起发送，增加传输效率
    * 安全模式			：尽量把多个发送操作的数据组合在一起发送，并控制传输速度，避免缓冲区溢出
    * 直接模式			：对每一个发送操作都直接投递，适用于负载不高但要求实时性较高的场合

    ************************************************************************/ }
  En_HP_SendPolicy = (
    HP_SP_PACK = 0, // 打包模式（默认）
    HP_SP_SAFE = 1, // 安全模式
    HP_SP_DIRECT = 2 // 直接模式
    );

  { TODO:************************************************************************
    名称：操作结果代码
    描述：Start() / Stop() 方法执行失败时，可通过 GetLastError() 获取错误代码
    ************************************************************************ }
  En_HP_SocketError = (
    HP_SE_OK = 0, // 成功
    HP_SE_ILLEGAL_STATE = 1, // 当前状态不允许操作
    HP_SE_INVALID_PARAM = 2, // 非法参数
    HP_SE_SOCKET_CREATE = 3, // 创建 SOCKET 失败
    HP_SE_SOCKET_BIND = 4, // 绑定 SOCKET 失败
    HP_SE_SOCKET_PREPARE = 5, // 设置 SOCKET 失败
    HP_SE_SOCKET_LISTEN = 6, // 监听 SOCKET 失败
    HP_SE_CP_CREATE = 7, // 创建完成端口失败
    HP_SE_WORKER_THREAD_CREATE = 8, // 创建工作线程失败
    HP_SE_DETECT_THREAD_CREATE = 9, // 创建监测线程失败
    HP_SE_SOCKE_ATTACH_TO_CP = 10, // 绑定完成端口失败
    HP_SE_CONNECT_SERVER = 11, // 连接服务器失败
    HP_SE_NETWORK = 12, // 网络错误
    HP_SE_DATA_PROC = 13, // 数据处理错误
    HP_SE_DATA_SEND = 14, // 数据发送失败

    //***** SSL Socket 扩展操作结果代码 *****/
    HP_SE_SSL_ENV_NOT_READY = 101     // SSL 环境未就绪
    );

  {TODO:
    /************************************************************************
  名称：播送模式
  描述：UDP 组件的播送模式（组播或广播）
  ************************************************************************/}
  En_HP_CastMode = (
    HP_CM_MULTICAST = 0, // 组播
    HP_CM_BROADCAST = 1 // 广播
    );


   { /************** HPSocket4C.dll 回调函数 **************/ }
  {TODO: /* Tcp服务端回调函数 */ }
  HP_FN_Server_OnPrepareListen = function(soListen: Pointer): En_HP_HandleResult; stdcall;
  // 如果为 TCP 连接，pClient为 SOCKET 句柄；如果为 UDP 连接，pClient为 SOCKADDR_IN 指针；
  HP_FN_Server_OnAccept = function(dwConnID: HP_CONNID; pClient: Pointer): En_HP_HandleResult; stdcall;
  HP_FN_Server_OnHandShake = function(dwConnID: HP_CONNID): En_HP_HandleResult; stdcall;
  HP_FN_Server_OnSend = function(dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer): En_HP_HandleResult; stdcall;
  HP_FN_Server_OnReceive = function(dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer): En_HP_HandleResult; stdcall;
  HP_FN_Server_OnPullReceive = function(dwConnID: HP_CONNID; iLength: Integer): En_HP_HandleResult; stdcall;
  HP_FN_Server_OnClose = function(dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer): En_HP_HandleResult; stdcall;
  HP_FN_Server_OnShutdown = function(): En_HP_HandleResult; stdcall;

  {TODO: /* Agent 回调函数 */ }
  HP_FN_Agent_OnPrepareConnect = function(dwConnID: HP_CONNID; SOCKET: Pointer): En_HP_HandleResult; stdcall;
  HP_FN_Agent_OnConnect = function(dwConnID: HP_CONNID): En_HP_HandleResult; stdcall;
  HP_FN_Agent_OnHandShake = function(dwConnID: HP_CONNID): En_HP_HandleResult; stdcall;
  HP_FN_Agent_OnSend = function(dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer): En_HP_HandleResult; stdcall;
  HP_FN_Agent_OnReceive = function(dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer): En_HP_HandleResult; stdcall;
  HP_FN_Agent_OnPullReceive = function(dwConnID: HP_CONNID; iLength: Integer): En_HP_HandleResult; stdcall;
  HP_FN_Agent_OnClose = function(dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer): En_HP_HandleResult; stdcall;
  HP_FN_Agent_OnShutdown = function(): En_HP_HandleResult; stdcall;

   {TODO: /* Client 回调函数 */ }
  HP_FN_Client_OnPrepareConnect = function(pClient: HP_Client; SOCKET: Pointer): En_HP_HandleResult; stdcall;
  HP_FN_Client_OnConnect = function(pClient: HP_Client): En_HP_HandleResult; stdcall;
  HP_FN_Client_OnHandShake = function(pClient: HP_Client): En_HP_HandleResult; stdcall;
  HP_FN_Client_OnSend = function(pClient: HP_Client; const pData: Pointer; iLength: Integer): En_HP_HandleResult; stdcall;
  HP_FN_Client_OnReceive = function(pClient: HP_Client; const pData: Pointer; iLength: Integer): En_HP_HandleResult; stdcall;
  HP_FN_Client_OnPullReceive = function(pClient: HP_Client; iLength: Integer): En_HP_HandleResult; stdcall;
  HP_FN_Client_OnClose = function(pClient: HP_Client; enOperation: En_HP_SocketOperation; iLength: Integer): En_HP_HandleResult; stdcall;

  { /****************************************************/
    /************** HPSocket4C.dll 导出函数 **************/ }
  {TODO: 创建 HP_TcpServer 对象 }
function Create_HP_TcpServer(pListener: HP_TcpServerListener): HP_TcpServer; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_HttpServer 对象 }
function Create_HP_HttpServer(pListener: HP_HttpServerListener): HP_HttpServer; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_TcpAgent 对象 }
function Create_HP_TcpAgent(pListener: HP_TcpAgentListener): HP_TcpAgent; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_TcpClient 对象 }
function Create_HP_TcpClient(pListener: HP_TcpClientListener): HP_TcpClient; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_TcpPullServer 对象 }
function Create_HP_TcpPullServer(pListener: HP_TcpPullServerListener): HP_TcpPullServer; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_TcpPullAgent 对象 }
function Create_HP_TcpPullAgent(pListener: HP_TcpPullAgentListener): HP_TcpPullAgent; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_TcpPullClient 对象}
function Create_HP_TcpPullClient(pListener: HP_TcpPullClientListener): HP_TcpPullClient; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_TcpPackServer 对象 }
function Create_HP_TcpPackServer(pListener: HP_TcpServerListener): HP_TcpPackServer; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_TcpPackAgent 对象  }
function Create_HP_TcpPackAgent(pListener: HP_TcpAgentListener): HP_TcpPackAgent; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_TcpPackClient 对象}
function Create_HP_TcpPackClient(pListener: HP_TcpClientListener): HP_TcpPackClient; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_UdpServer 对象  }
function Create_HP_UdpServer(pListener: HP_UdpServerListener): HP_UdpServer; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_UdpClient 对象 }
function Create_HP_UdpClient(pListener: HP_UdpClientListener): HP_UdpClient; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_UdpCast 对象 }
function Create_HP_UdpCast(pListener: HP_UdpCastListener): HP_UdpCast; stdcall; external HPSocketDLL;

  {TODO: 销毁 HP_TcpServer 对象  }
procedure Destroy_HP_TcpServer(pServer: HP_TcpServer); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_HttpServer 对象  }
procedure Destroy_HP_HttpServer(pServer: HP_HttpServer); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_TcpAgent 对象  }
procedure Destroy_HP_TcpAgent(pAgent: HP_TcpAgent); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_TcpClient 对象  }
procedure Destroy_HP_TcpClient(pClient: HP_TcpClient); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_TcpPullServer 对象  }
procedure Destroy_HP_TcpPullServer(pServer: HP_TcpPullServer); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_TcpPullAgent 对象   }
procedure Destroy_HP_TcpPullAgent(pAgent: HP_TcpPullAgent); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_TcpPullClient 对象   }
procedure Destroy_HP_TcpPullClient(pClient: HP_TcpPullClient); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_TcpPackServer 对象 }
procedure Destroy_HP_TcpPackServer(pServer: HP_TcpPackServer); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_TcpPackAgent 对象  }
procedure Destroy_HP_TcpPackAgent(pAgent: HP_TcpPackAgent); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_TcpPackClient 对象 }
procedure Destroy_HP_TcpPackClient(pClient: HP_TcpPackClient); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_UdpServer 对象    }
procedure Destroy_HP_UdpServer(pServer: HP_UdpServer); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_UdpClient 对象    }
procedure Destroy_HP_UdpClient(pClient: HP_UdpClient); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_UdpCast 对象    }
procedure Destroy_HP_UdpCast(pCast: HP_UdpCast); stdcall; external HPSocketDLL;

  {TODO: 创建 HP_TcpServerListener and HP_TcpPackServerListener 对象  }
function Create_HP_TcpServerListener(): HP_TcpServerListener; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_HttpServerListener 对象  }
function Create_HP_HttpServerListener(): HP_HttpServerListener; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_TcpAgentListener 对象   }
function Create_HP_TcpAgentListener(): HP_TcpAgentListener; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_TcpClientListener 对象   }
function Create_HP_TcpClientListener(): HP_TcpClientListener; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_TcpPullServerListener 对象 }
function Create_HP_TcpPullServerListener(): HP_TcpPullServerListener; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_TcpPullAgentListener 对象  }
function Create_HP_TcpPullAgentListener(): HP_TcpPullAgentListener; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_TcpPullClientListener 对象   }
function Create_HP_TcpPullClientListener(): HP_TcpPullClientListener; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_UdpServerListener 对象     }
function Create_HP_UdpServerListener(): HP_UdpServerListener; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_UdpClientListener 对象   }
function Create_HP_UdpClientListener(): HP_UdpClientListener; stdcall; external HPSocketDLL;
  {TODO: 创建 HP_UdpCastListener 对象      }
function Create_HP_UdpCastListener(): HP_UdpCastListener; stdcall; external HPSocketDLL;

  {TODO: 销毁 HP_TcpServerListener and HP_TcpPackServerListener对象 }
procedure Destroy_HP_TcpServerListener(pListener: HP_TcpServerListener); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_HttpServerListener 对象 }
procedure Destroy_HP_HttpServerListener(pListener: HP_HttpServerListener); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_TcpAgentListener 对象 }
procedure Destroy_HP_TcpAgentListener(pListener: HP_TcpAgentListener); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_TcpClientListener 对象  }
procedure Destroy_HP_TcpClientListener(pListener: HP_TcpClientListener); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_TcpPullServerListener 对象  }
procedure Destroy_HP_TcpPullServerListener(pListener: HP_TcpPullServerListener); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_TcpPullAgentListener 对象  }
procedure Destroy_HP_TcpPullAgentListener(pListener: HP_TcpPullAgentListener); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_TcpPullClientListener 对象  }
procedure Destroy_HP_TcpPullClientListener(pListener: HP_TcpPullClientListener); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_UdpServerListener 对象    }
procedure Destroy_HP_UdpServerListener(pListener: HP_UdpServerListener); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_UdpClientListener 对象   }
procedure Destroy_HP_UdpClientListener(pListener: HP_UdpClientListener); stdcall; external HPSocketDLL;
  {TODO: 销毁 HP_UdpCastListener 对象    }
procedure Destroy_HP_UdpCastListener(pListener: HP_UdpCastListener); stdcall; external HPSocketDLL;

{TODO: ***************************** Server 回调函数设置方法 ***************************** }
procedure HP_Set_FN_Server_OnPrepareListen(pListener: HP_TcpServerListener; fn: HP_FN_Server_OnPrepareListen); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnAccept(pListener: HP_TcpServerListener; fn: HP_FN_Server_OnAccept); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnHandShake(pListener: HP_TcpServerListener; fn: HP_FN_Server_OnHandShake); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnSend(pListener: HP_TcpServerListener; fn: HP_FN_Server_OnSend); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnReceive(pListener: HP_TcpServerListener; fn: HP_FN_Server_OnReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnPullReceive(pListener: HP_TcpServerListener; fn: HP_FN_Server_OnPullReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnClose(pListener: HP_TcpServerListener; fn: HP_FN_Server_OnClose); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Server_OnShutdown(pListener: HP_TcpServerListener; fn: HP_FN_Server_OnShutdown); stdcall; external HPSocketDLL;



{TODO:****************************** Agent 回调函数设置方法 ***************************** }
procedure HP_Set_FN_Agent_OnPrepareConnect(pListener: HP_AgentListener; fn: HP_FN_Agent_OnPrepareConnect); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnConnect(pListener: HP_AgentListener; fn: HP_FN_Agent_OnConnect); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnHandShake(pListener: HP_AgentListener; fn: HP_FN_Agent_OnHandShake); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnSend(pListener: HP_AgentListener; fn: HP_FN_Agent_OnSend); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnReceive(pListener: HP_AgentListener; fn: HP_FN_Agent_OnReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnPullReceive(pListener: HP_AgentListener; fn: HP_FN_Agent_OnPullReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnClose(pListener: HP_AgentListener; fn: HP_FN_Agent_OnClose); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Agent_OnAgentShutdown(pListener: HP_AgentListener; fn: HP_FN_Agent_OnShutdown); stdcall; external HPSocketDLL;

{TODO:***************************** Client 回调函数设置方法 ***************************** }
procedure HP_Set_FN_Client_OnPrepareConnect(pListener: HP_TcpClientListener; fn: HP_FN_Client_OnPrepareConnect); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnConnect(pListener: HP_TcpClientListener; fn: HP_FN_Client_OnConnect); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnHandShake(pListener: HP_TcpClientListener; fn: HP_FN_Client_OnHandShake); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnSend(pListener: HP_TcpClientListener; fn: HP_FN_Client_OnSend); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnReceive(pListener: HP_TcpClientListener; fn: HP_FN_Client_OnReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnPullReceive(pListener: HP_TcpClientListener; fn: HP_FN_Client_OnPullReceive); stdcall; external HPSocketDLL;
procedure HP_Set_FN_Client_OnClose(pListener: HP_TcpClientListener; fn: HP_FN_Client_OnClose); stdcall; external HPSocketDLL;

{ /***************************** Server 操作方法 *****************************/ }
{ TODO:
  * 名称：启动通信组件
  * 描述：启动服务端通信组件，启动完成后可开始接收客户端连接并收发数据
  *
  * 参数：		pszBindAddress	-- 监听地址
  *		      	usPort			-- 监听端口
  * 返回值：	TRUE	-- 成功
  *			      FALSE	-- 失败，可通过 GetLastError() 获取错误代码 }
function HP_Server_Start(pServer: HP_Server; pszBindAddress: WideString; usPort: Word): BOOL; stdcall; external HPSocketDLL;

{ TODO:
  * 名称：关闭通信组件
  * 描述：关闭服务端通信组件，关闭完成后断开所有客户端连接并释放所有资源
  *
  * 参数：
  * 返回值：  TRUE	-- 成功
  *			      FALSE	-- 失败，可通过 GetLastError() 获取错误代码 }
function HP_Server_Stop(pServer: HP_Server): BOOL; stdcall; external HPSocketDLL;

{ TODO:
  * 名称：发送数据
  * 描述：用户通过该方法向指定客户端发送数据
  *
  * 参数：dwConnID	-- 连接 ID
  *			  pBuffer		-- 发送数据缓冲区
  *		  	iLength		-- 发送数据长度
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败 }
   { Private declarations }

function HP_Server_Send(pServer: HP_Server; dwConnID: HP_CONNID; const pBuffer: Pointer; iLength: Integer): BOOL;
stdcall; external HPSocketDLL;


{ TODO:
  * 名称：发送数据
  * 描述：向指定连接发送数据
  *
  * 参数：dwConnID	-- 连接 ID
  *		  	pBuffer		-- 发送缓冲区
  *			  iLength		-- 发送缓冲区长度
  *			  iOffset		-- 发送缓冲区指针偏移量
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码 }
function HP_Server_SendPart(pServer: HP_Server; dwConnID: HP_CONNID; const pBuffer: Pointer; iLength: Integer; iOffset: Integer): BOOL;
stdcall; external HPSocketDLL;

{ TODO:
  * 名称：发送多组数据
  * 描述：向指定连接发送多组数据
  *		TCP - 顺序发送所有数据包
  *		UDP - 把所有数据包组合成一个数据包发送（数据包的总长度不能大于设置的 UDP 包最大长度）
  *
  * 参数：	dwConnID	-- 连接 ID
  *			    pBuffers	-- 发送缓冲区数组
  *			    iCount		-- 发送缓冲区数目
  * 返回值：  TRUE	-- 成功
  *		    	  FALSE	-- 失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码 }
function HP_Server_SendPackets(pServer: HP_Server; dwConnID: HP_CONNID; const pBuffers: WSABUFArray; iCount: Integer): BOOL;
stdcall; external HPSocketDLL;

{ TODO:
  * 名称：断开连接
  * 描述：断开与某个客户端的连接
  *
  * 参数：	dwConnID	-- 连接 ID
  *		    	bForce		-- 是否强制断开连接
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败 }
function HP_Server_Disconnect(pServer: HP_Server; dwConnID: HP_CONNID; bForce: BOOL): BOOL;
stdcall; external HPSocketDLL;

{ TODO:
  * 名称：断开超时连接
  * 描述：断开超过指定时长的连接
  *
  * 参数：  dwPeriod	-- 时长（毫秒）
  *		    	bForce		-- 是否强制断开连接
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败 }
function HP_Server_DisconnectLongConnections(pServer: HP_Server; dwPeriod: LongWord; bForce: BOOL): BOOL;
stdcall; external HPSocketDLL;

{  TODO:
  * 名称：断开静默连接
  * 描述：断开超过指定时长的静默连接
  *
  * 参数：dwPeriod	-- 时长（毫秒）
  *		  	bForce		-- 是否强制断开连接
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败 }
function HP_Server_DisconnectSilenceConnections(pServer: HP_Server; dwPeriod: LongWord; bForce: BOOL): BOOL;
stdcall; external HPSocketDLL;

{***************************** Server 属性访问方法 *****************************/ }
{ TODO:
  * 名称：设置连接的附加数据
  * 描述：是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序只身决定
  *
  * 参数：  dwConnID	-- 连接 ID
  *		    	pv			-- 数据
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败（无效的连接 ID） }
function HP_Server_SetConnectionExtra(pServer: HP_Server; dwConnID: HP_CONNID; pExtra: Pointer): BOOL;
stdcall; external HPSocketDLL;

{ TODO:
  * 名称：获取连接的附加数据
  * 描述：是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序只身决定
  *
  * 参数： dwConnID	-- 连接 ID
  *			   ppv			-- 数据指针
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败（无效的连接 ID） }
function HP_Server_GetConnectionExtra(pServer: HP_Server; dwConnID: HP_CONNID; ppExtra: Pointer): BOOL;
stdcall; external HPSocketDLL;

{ TODO: 检查通信组件是否已启动 */ }
function HP_Server_HasStarted(pServer: HP_Server): BOOL; stdcall; external HPSocketDLL;
{ TODO: 查看通信组件当前状态 */ }
function HP_Server_GetState(pServer: HP_Server): En_HP_ServiceState; stdcall; external HPSocketDLL;
{ TODO: 获取最近一次失败操作的错误代码 */ }
function HP_Server_GetLastError(pServer: HP_Server): En_HP_SocketError; stdcall; external HPSocketDLL;
{ TODO: 获取最近一次失败操作的错误描述 */ }
function HP_Server_GetLastErrorDesc(pServer: HP_Server): PWideChar; stdcall; external HPSocketDLL;
{ TODO: 获取连接中未发出数据的长度 */ }
function HP_Server_GetPendingDataLength(pServer: HP_Server; dwConnID: HP_CONNID; piPending: PInteger): BOOL; stdcall; external HPSocketDLL;
{ TODO: 获取客户端连接数 */ }
function HP_Server_GetConnectionCount(pServer: HP_Server): LongInt; stdcall; external HPSocketDLL;
{ TODO: 获取所有连接的 CONNID */ }
function HP_Server_GetAllConnectionIDs(pServer: HP_Server; pIDs: HP_CONNIDArray; pdwCount: PLongint): BOOL; stdcall; external HPSocketDLL;
{ TODO: 获取某个客户端连接时长（毫秒） */ }
function HP_Server_GetConnectPeriod(pServer: HP_Server; dwConnID: HP_CONNID; pdwPeriod: PLongint): BOOL; stdcall; external HPSocketDLL;
{ TODO: 获取某个连接静默时间（毫秒） */  }
function HP_Server_GetSilencePeriod(pServer: HP_Server; dwConnID: HP_CONNID; pdwPeriod: PLongint): BOOL; stdcall; external HPSocketDLL;
{ TODO: 获取监听 Socket 的地址信息 */ }
function HP_Server_GetListenAddress(pServer: HP_Server; lpszAddress: PWideChar; piAddressLen: PInteger; pusPort: PUShort): BOOL; stdcall; external HPSocketDLL;
{ TODO: 获取某个连接的远程地址信息 */ }
function HP_Server_GetRemoteAddress(pServer: HP_Server; dwConnID: HP_CONNID; lpszAddress: PWideChar; piAddressLen: PInteger; pusPort: PUShort): BOOL; stdcall; external HPSocketDLL;

{ TODO: 设置数据发送策略 */ }
procedure HP_Server_SetSendPolicy(pServer: HP_Server; enSendPolicy: En_HP_SendPolicy); stdcall; external HPSocketDLL;
{ TODO: 设置 Socket 缓存对象锁定时间（毫秒，在锁定期间该 Socket 缓存对象不能被获取使用） */ }
procedure HP_Server_SetFreeSocketObjLockTime(pServer: HP_Server; dwFreeSocketObjLockTime: LongInt); stdcall; external HPSocketDLL;
{ TODO: 设置 Socket 缓存池大小（通常设置为平均并发连接数量的 1/3 - 1/2） */ }
procedure HP_Server_SetFreeSocketObjPool(pServer: HP_Server; dwFreeSocketObjPool: LongInt); stdcall; external HPSocketDLL;
{ TODO: 设置内存块缓存池大小（通常设置为 Socket 缓存池大小的 2 - 3 倍） */ }
procedure HP_Server_SetFreeBufferObjPool(pServer: HP_Server; dwFreeBufferObjPool: LongInt); stdcall; external HPSocketDLL;
{ TODO: 设置 Socket 缓存池回收阀值（通常设置为 Socket 缓存池大小的 3 倍） */ }
procedure HP_Server_SetFreeSocketObjHold(pServer: HP_Server; dwFreeSocketObjHold: LongInt); stdcall; external HPSocketDLL;
{ TODO: 设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍） */ }
procedure HP_Server_SetFreeBufferObjHold(pServer: HP_Server; dwFreeBufferObjHold: LongInt); stdcall; external HPSocketDLL;
{ TODO: 设置工作线程数量（通常设置为 2 * CPU + 2） */ }
procedure HP_Server_SetWorkerThreadCount(pServer: HP_Server; dwWorkerThreadCount: LongInt); stdcall; external HPSocketDLL;
{ TODO: 设置是否标记静默时间（设置为 TRUE 时 DisconnectSilenceConnections() 和 GetSilencePeriod() 才有效，默认：FALSE） */ }
procedure HP_Server_SetMarkSilence(pServer: HP_Server; bMarkSilence: BOOL); stdcall; external HPSocketDLL;

{ TODO: 获取数据发送策略 */ }
function HP_Server_GetSendPolicy(pServer: HP_Server): En_HP_SendPolicy; stdcall; external HPSocketDLL;
{ TODO: 获取 Socket 缓存对象锁定时间 */ }
function HP_Server_GetFreeSocketObjLockTime(pServer: HP_Server): LongInt; stdcall; external HPSocketDLL;
{ TODO: 获取 Socket 缓存池大小 */ }
function HP_Server_GetFreeSocketObjPool(pServer: HP_Server): LongInt; stdcall; external HPSocketDLL;
{ TODO: 获取内存块缓存池大小 */ }
function HP_Server_GetFreeBufferObjPool(pServer: HP_Server): LongInt; stdcall; external HPSocketDLL;
{ TODO: 获取 Socket 缓存池回收阀值 */ }
function HP_Server_GetFreeSocketObjHold(pServer: HP_Server): LongInt; stdcall; external HPSocketDLL;
{ TODO: 获取内存块缓存池回收阀值 */ }
function HP_Server_GetFreeBufferObjHold(pServer: HP_Server): LongInt; stdcall; external HPSocketDLL;
{ TODO: 获取工作线程数量 */ }
function HP_Server_GetWorkerThreadCount(pServer: HP_Server): LongInt; stdcall; external HPSocketDLL;
{ TODO: 检测是否标记静默时间 */ }
function HP_Server_IsMarkSilence(pServer: HP_Server): BOOL; stdcall; external HPSocketDLL;

{ /******************************* TCP Server 操作方法 *******************************/ }
{ TODO:
  * 名称：发送小文件
  * 描述：向指定连接发送 4096 KB 以下的小文件
  *
  * 参数：		dwConnID		-- 连接 ID
  *			lpszFileName	-- 文件路径
  *			pHead			-- 头部附加数据
  *			pTail			-- 尾部附加数据
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码
  */ }
function HP_TcpServer_SendSmallFile(pServer: HP_Server; dwConnID: HP_CONNID;
  lpszFileName: PWideChar; const pHead: LPWSABUF; const pTail: LPWSABUF): BOOL;
stdcall; external HPSocketDLL;

{ /***************************** TCP Server 属性访问方法 *****************************/ }

{ TODO: 设置监听 Socket 的等候队列大小（根据并发连接数量调整设置） */ }
procedure HP_TcpServer_SetSocketListenQueue(pServer: HP_TcpServer; dwSocketListenQueue: LongInt); stdcall; external HPSocketDLL;
{ TODO: 设置 Accept 预投递数量（根据负载调整设置，Accept 预投递数量越大则支持的并发连接请求越多） */ }
procedure HP_TcpServer_SetAcceptSocketCount(pServer: HP_TcpServer; dwAcceptSocketCount: LongInt); stdcall; external HPSocketDLL;
{ TODO: 设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为 1024 的倍数） */ }
procedure HP_TcpServer_SetSocketBufferSize(pServer: HP_TcpServer; dwSocketBufferSize: LongInt); stdcall; external HPSocketDLL;
{ TODO: 设置心跳包间隔（毫秒，0 则不发送心跳包） */ }
procedure HP_TcpServer_SetKeepAliveTime(pServer: HP_TcpServer; dwKeepAliveTime: LongInt); stdcall; external HPSocketDLL;
{ TODO: 设置心跳确认包检测间隔（毫秒，0 不发送心跳包，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线） */ }
procedure HP_TcpServer_SetKeepAliveInterval(pServer: HP_TcpServer; dwKeepAliveInterval: LongInt); stdcall; external HPSocketDLL;

{ TODO: 获取 Accept 预投递数量 */ }
function HP_TcpServer_GetAcceptSocketCount(pServer: HP_TcpServer): LongInt; stdcall; external HPSocketDLL;
{ TODO: 获取通信数据缓冲区大小 */ }
function HP_TcpServer_GetSocketBufferSize(pServer: HP_TcpServer): LongInt; stdcall; external HPSocketDLL;
{ TODO: 获取监听 Socket 的等候队列大小 */ }
function HP_TcpServer_GetSocketListenQueue(pServer: HP_TcpServer): LongInt; stdcall; external HPSocketDLL;
{ TODO: 获取心跳检查次数 */ }
function HP_TcpServer_GetKeepAliveTime(pServer: HP_TcpServer): LongInt; stdcall; external HPSocketDLL;
{ TODO: 获取心跳检查间隔 */ }
function HP_TcpServer_GetKeepAliveInterval(pServer: HP_TcpServer): LongInt; stdcall; external HPSocketDLL;

{ /***************************** UDP Server 属性访问方法 *****************************/ }

{ TODO: 设置数据报文最大长度（建议在局域网环境下不超过 1472 字节，在广域网环境下不超过 548 字节） */ }
procedure HP_UdpServer_SetMaxDatagramSize(pServer: HP_UdpServer; dwMaxDatagramSize: LongInt); stdcall; external HPSocketDLL;
{ TODO: 获取数据报文最大长度 */ }
function HP_UdpServer_GetMaxDatagramSize(pServer: HP_UdpServer): LongInt; stdcall; external HPSocketDLL;

{ TODO: 设置 Receive 预投递数量（根据负载调整设置，Receive 预投递数量越大则丢包概率越小） */ }
procedure HP_UdpServer_SetPostReceiveCount(pServer: HP_UdpServer; dwPostReceiveCount: LongInt); stdcall; external HPSocketDLL;
{ TODO: 获取 Receive 预投递数量 */ }
function HP_UdpServer_GetPostReceiveCount(pServer: HP_UdpServer): LongInt; stdcall; external HPSocketDLL;

{ TODO: 设置监测包尝试次数（0 则不发送监测跳包，如果超过最大尝试次数则认为已断线） */ }
procedure HP_UdpServer_SetDetectAttempts(pServer: HP_UdpServer; dwDetectAttempts: LongInt); stdcall; external HPSocketDLL;
{ TODO: 设置监测包发送间隔（秒，0 不发送监测包） */ }
procedure HP_UdpServer_SetDetectInterval(pServer: HP_UdpServer; dwDetectInterval: LongInt); stdcall; external HPSocketDLL;
{ TODO: 获取心跳检查次数 */ }
function HP_UdpServer_GetDetectAttempts(pServer: HP_UdpServer): LongInt; stdcall; external HPSocketDLL;
{ TODO: 获取心跳检查间隔 */ }
function HP_UdpServer_GetDetectInterval(pServer: HP_UdpServer): LongInt; stdcall; external HPSocketDLL;

{ ***************************** Agent 操作方法 ***************************** }

{ TODO:
  * 名称：启动通信组件
  * 描述：启动通信代理组件，启动完成后可开始连接远程服务器
  *
  * 参数：	pszBindAddress	-- 监听地址
  *			    bAsyncConnect	-- 是否采用异步 Connect
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 GetLastError() 获取错误代码
  */ }
function HP_Agent_Start(pAgent: HP_Agent; pszBindAddress: PWideChar; bAsyncConnect: BOOL): BOOL; stdcall; external HPSocketDLL;

{ TODO:
  * 名称：关闭通信组件
  * 描述：关闭通信组件，关闭完成后断开所有连接并释放所有资源
  *
  * 参数：
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 GetLastError() 获取错误代码
  */ }
function HP_Agent_Stop(pAgent: HP_Agent): BOOL; stdcall; external HPSocketDLL;

{ TODO:
  * 名称：连接服务器
  * 描述：连接服务器，连接成功后 IAgentListener 会接收到 OnConnect() 事件
  *
  * 参数：		pszRemoteAddress	-- 服务端地址
  *			usPort				-- 服务端端口
  *			pdwConnID			-- 连接 ID（默认：nullptr，不获取连接 ID）
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过函数 SYS_GetLastError() 获取 Windows 错误代码
  */ }
function HP_Agent_Connect(pAgent: HP_Agent; pszRemoteAddress: PWideChar; usPort: Word; pdwConnID: PHP_CONNID): BOOL;
stdcall; external HPSocketDLL;

{ TODO:
  * 名称：发送数据
  * 描述：向指定连接发送数据
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pBuffer		-- 发送缓冲区
  *			iLength		-- 发送缓冲区长度
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码
  */ }
function HP_Agent_Send(pAgent: HP_Agent; dwConnID: HP_CONNID; const pBuffer: Pointer; iLength: Integer): BOOL;
stdcall; external HPSocketDLL;

{ TODO:
  * 名称：发送数据
  * 描述：向指定连接发送数据
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pBuffer		-- 发送缓冲区
  *			iLength		-- 发送缓冲区长度
  *			iOffset		-- 发送缓冲区指针偏移量
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码
  */ }
function HP_Agent_SendPart(pAgent: HP_Agent; dwConnID: HP_CONNID; const pBuffer: Pointer; iLength: Integer; iOffset: Integer): BOOL;
stdcall; external HPSocketDLL;

{ TODO:
  * 名称：发送多组数据
  * 描述：向指定连接发送多组数据
  *		TCP - 顺序发送所有数据包
  *		UDP - 把所有数据包组合成一个数据包发送（数据包的总长度不能大于设置的 UDP 包最大长度）
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pBuffers	-- 发送缓冲区数组
  *			iCount		-- 发送缓冲区数目
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码
  */ }
function HP_Agent_SendPackets(pAgent: HP_Agent; dwConnID: HP_CONNID; const pBuffers: WSABUFArray; iCount: Integer): BOOL;
stdcall; external HPSocketDLL;

{ TODO:
  * 名称：断开连接
  * 描述：断开某个连接
  *
  * 参数：		dwConnID	-- 连接 ID
  *			bForce		-- 是否强制断开连接
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败
  */ }
function HP_Agent_Disconnect(pAgent: HP_Agent; dwConnID: HP_CONNID; bForce: BOOL): BOOL;
stdcall; external HPSocketDLL;

{ TODO:
  * 名称：断开超时连接
  * 描述：断开超过指定时长的连接
  *
  * 参数：		dwPeriod	-- 时长（毫秒）
  *			bForce		-- 是否强制断开连接
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败
  */ }
function HP_Agent_DisconnectLongConnections(pAgent: HP_Agent; dwPeriod: LongInt; bForce: BOOL): BOOL;
stdcall; external HPSocketDLL;

{ TODO:
  * 名称：断开静默连接
  * 描述：断开超过指定时长的静默连接
  *
  * 参数：		dwPeriod	-- 时长（毫秒）
  *			bForce		-- 是否强制断开连接
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败
  */ }
function HP_Agent_DisconnectSilenceConnections(pAgent: HP_Agent; dwPeriod: LongInt; bForce: BOOL): BOOL;
stdcall; external HPSocketDLL;

{ /******************************************************************************/
  /***************************** Agent 属性访问方法 *****************************/ }

{ TODO
  * 名称：设置连接的附加数据
  * 描述：是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序只身决定
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pv			-- 数据
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败（无效的连接 ID）
  */ }
function HP_Agent_SetConnectionExtra(pAgent: HP_Agent; dwConnID: HP_CONNID; pExtra: Pointer): BOOL;
stdcall; external HPSocketDLL;

{ TODO
  * 名称：获取连接的附加数据
  * 描述：是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序只身决定
  *
  * 参数：		dwConnID	-- 连接 ID
  *			ppv			-- 数据指针
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败（无效的连接 ID）
  */ }
function HP_Agent_GetConnectionExtra(pAgent: HP_Agent; dwConnID: HP_CONNID;
  ppExtra: Pointer): BOOL; stdcall; external HPSocketDLL;

{TODO 检查通信组件是否已启动 */ }
function HP_Agent_HasStarted(pAgent: HP_Agent): BOOL; stdcall; external HPSocketDLL;
{TODO 查看通信组件当前状态 */ }
function HP_Agent_GetState(pAgent: HP_Agent): En_HP_ServiceState; stdcall; external HPSocketDLL;
{TODO 获取连接数 */ }
function HP_Agent_GetConnectionCount(pAgent: HP_Agent): LongInt; stdcall; external HPSocketDLL;
{ TODO 获取所有连接的 CONNID */ }
function HP_Agent_GetAllConnectionIDs(pAgent: HP_Agent; pIDs: HP_CONNIDArray; pdwCount: PLongint): BOOL; stdcall; external HPSocketDLL;
{TODO 获取某个连接时长（毫秒） */ }
function HP_Agent_GetConnectPeriod(pAgent: HP_Agent; dwConnID: HP_CONNID; pdwPeriod: PLongint): BOOL; stdcall; external HPSocketDLL;
{TODO 获取某个连接静默时间（毫秒） */ }
function HP_Agent_GetSilencePeriod(pAgent: HP_Agent; dwConnID: HP_CONNID; pdwPeriod: PLongint): BOOL; stdcall; external HPSocketDLL;
{TODO 获取某个连接的本地地址信息 */ }
function HP_Agent_GetLocalAddress(pAgent: HP_Agent; dwConnID: HP_CONNID; lpszAddress: PWideChar; piAddressLen: PInteger; pusPort: PUShort): BOOL; stdcall; external HPSocketDLL;
{TODO 获取某个连接的远程地址信息 */ }
function HP_Agent_GetRemoteAddress(pAgent: HP_Agent; dwConnID: HP_CONNID; lpszAddress: PWideChar; piAddressLen: PInteger; pusPort: PUShort): BOOL; stdcall; external HPSocketDLL;
{TODO 获取最近一次失败操作的错误代码 */ }
function HP_Agent_GetLastError(pAgent: HP_Agent): En_HP_SocketError; stdcall; external HPSocketDLL;
{TODO 获取最近一次失败操作的错误描述 */ }
function HP_Agent_GetLastErrorDesc(pAgent: HP_Agent): PWideChar; stdcall; external HPSocketDLL;
{TODO 获取连接中未发出数据的长度 */ }
function HP_Agent_GetPendingDataLength(pAgent: HP_Agent; dwConnID: HP_CONNID; piPending: PInteger): BOOL; stdcall; external HPSocketDLL;

{TODO 设置数据发送策略 */ }
procedure HP_Agent_SetSendPolicy(pAgent: HP_Agent; enSendPolicy: En_HP_SendPolicy); stdcall; external HPSocketDLL;
{TODO 设置 Socket 缓存对象锁定时间（毫秒，在锁定期间该 Socket 缓存对象不能被获取使用） */ }
procedure HP_Agent_SetFreeSocketObjLockTime(pAgent: HP_Agent; dwFreeSocketObjLockTime: LongInt); stdcall; external HPSocketDLL;
{TODO 设置 Socket 缓存池大小（通常设置为平均并发连接数量的 1/3 - 1/2） */ }
procedure HP_Agent_SetFreeSocketObjPool(pAgent: HP_Agent; dwFreeSocketObjPool: LongInt); stdcall; external HPSocketDLL;
{TODO 设置内存块缓存池大小（通常设置为 Socket 缓存池大小的 2 - 3 倍） */ }
procedure HP_Agent_SetFreeBufferObjPool(pAgent: HP_Agent; dwFreeBufferObjPool: LongInt); stdcall; external HPSocketDLL;
{TODO 设置 Socket 缓存池回收阀值（通常设置为 Socket 缓存池大小的 3 倍） */ }
procedure HP_Agent_SetFreeSocketObjHold(pAgent: HP_Agent; dwFreeSocketObjHold: LongInt); stdcall; external HPSocketDLL;
{TODO 设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍） */ }
procedure HP_Agent_SetFreeBufferObjHold(pAgent: HP_Agent; dwFreeBufferObjHold: LongInt); stdcall; external HPSocketDLL;
{TODO 设置工作线程数量（通常设置为 2 * CPU + 2） */ }
procedure HP_Agent_SetWorkerThreadCount(pAgent: HP_Agent; dwWorkerThreadCount: LongInt); stdcall; external HPSocketDLL;
{TODO 设置是否标记静默时间（设置为 TRUE 时 DisconnectSilenceConnections() 和 GetSilencePeriod() 才有效，默认：FALSE） */ }
procedure HP_Agent_SetMarkSilence(pAgent: HP_Agent; bMarkSilence: BOOL); stdcall; external HPSocketDLL;

{TODO 获取数据发送策略 */ }
function HP_Agent_GetSendPolicy(pAgent: HP_Agent): En_HP_SendPolicy; stdcall; external HPSocketDLL;
{TODO 获取 Socket 缓存对象锁定时间 */ }
function HP_Agent_GetFreeSocketObjLockTime(pAgent: HP_Agent): LongInt; stdcall; external HPSocketDLL;
{TODO 获取 Socket 缓存池大小 */ }
function HP_Agent_GetFreeSocketObjPool(pAgent: HP_Agent): LongInt; stdcall; external HPSocketDLL;
{TODO 获取内存块缓存池大小 */ }
function HP_Agent_GetFreeBufferObjPool(HpAgent: HP_Agent): LongInt; stdcall; external HPSocketDLL;
{TODO 获取 Socket 缓存池回收阀值 */ }
function HP_Agent_GetFreeSocketObjHold(pAgent: HP_Agent): LongInt; stdcall; external HPSocketDLL;
{TODO 获取内存块缓存池回收阀值 */ }
function HP_Agent_GetFreeBufferObjHold(pAgent: HP_Agent): LongInt; stdcall; external HPSocketDLL;
{TODO 获取工作线程数量 */ }
function HP_Agent_GetWorkerThreadCount(pAgent: HP_Agent): LongInt; stdcall; external HPSocketDLL;
{TODO 检测是否标记静默时间 */ }
function HP_Agent_IsMarkSilence(pAgent: HP_Agent): BOOL; stdcall; external HPSocketDLL;

{ ******************************* TCP Agent 操作方法 ******************************* }

{TODO
  * 名称：发送小文件
  * 描述：向指定连接发送 4096 KB 以下的小文件
  *
  * 参数：		dwConnID		-- 连接 ID
  *			lpszFileName	-- 文件路径
  *			pHead			-- 头部附加数据
  *			pTail			-- 尾部附加数据
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码
  */ }
function HP_TcpAgent_SendSmallFile(pAgent: HP_Agent; dwConnID: HP_CONNID; lpszFileName: PWideChar; const pHead: LPWSABUF; const pTail: LPWSABUF): BOOL;
stdcall; external HPSocketDLL;

{ ***************************** TCP Agent 属性访问方法 *****************************/ }

{ TODO 设置是否启用地址重用机制（默认：不启用） */ }
procedure HP_TcpAgent_SetReuseAddress(pAgent: HP_TcpAgent; bReuseAddress: BOOL); stdcall; external HPSocketDLL;
{ TODO 检测是否启用地址重用机制 */ }
function HP_TcpAgent_IsReuseAddress(pAgent: HP_TcpAgent): BOOL; stdcall; external HPSocketDLL;

{ TODO 设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为 1024 的倍数） */ }
procedure HP_TcpAgent_SetSocketBufferSize(pAgent: HP_TcpAgent; dwSocketBufferSize: LongInt); stdcall; external HPSocketDLL;
{ TODO 设置心跳包间隔（毫秒，0 则不发送心跳包） */ }
procedure HP_TcpAgent_SetKeepAliveTime(pAgent: HP_TcpAgent; dwKeepAliveTime: LongInt); stdcall; external HPSocketDLL;
{ TODO 设置心跳确认包检测间隔（毫秒，0 不发送心跳包，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线） */ }
procedure HP_TcpAgent_SetKeepAliveInterval(pAgent: HP_TcpAgent; dwKeepAliveInterval: LongInt); stdcall; external HPSocketDLL;

{ TODO 获取通信数据缓冲区大小 */ }
function HP_TcpAgent_GetSocketBufferSize(pAgent: HP_TcpAgent): LongInt; stdcall; external HPSocketDLL;
{ TODO 获取心跳检查次数 */ }
function HP_TcpAgent_GetKeepAliveTime(pAgent: HP_TcpAgent): LongInt; stdcall; external HPSocketDLL;
{ TODO 获取心跳检查间隔 */ }
function HP_TcpAgent_GetKeepAliveInterval(pAgent: HP_TcpAgent): LongInt; stdcall; external HPSocketDLL;


{ /***************************** Client 组件操作方法 *****************************/ }

{ TODO
  * 名称：启动通信组件
  * 描述：启动客户端通信组件并连接服务端，启动完成后可开始收发数据
  *
  * 参数：		pszRemoteAddress	-- 服务端地址
  *			usPort				-- 服务端端口
  *			bAsyncConnect		-- 是否采用异步 Connect
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 GetLastError() 获取错误代码
  */ }
function HP_Client_Start(pClient: HP_Client; pszRemoteAddress: PWideChar; usPort: Word; bAsyncConnect: BOOL): BOOL;
stdcall; external HPSocketDLL;

{ TODO
  * 名称：关闭通信组件
  * 描述：关闭客户端通信组件，关闭完成后断开与服务端的连接并释放所有资源
  *
  * 参数：
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 GetLastError() 获取错误代码
  */ }
function HP_Client_Stop(pClient: HP_Client): BOOL; stdcall; external HPSocketDLL;

{ TODO
  * 名称：发送数据
  * 描述：向服务端发送数据
  *
  * 参数：		pBuffer		-- 发送缓冲区
  *			iLength		-- 发送缓冲区长度
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码
  */ }
function HP_Client_Send(pClient: HP_Client; const pBuffer: Pointer; iLength: Integer): BOOL; stdcall; external HPSocketDLL;

{ TODO
  * 名称：发送数据
  * 描述：向服务端发送数据
  *
  * 参数：		pBuffer		-- 发送缓冲区
  *			iLength		-- 发送缓冲区长度
  *			iOffset		-- 发送缓冲区指针偏移量
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码
  */ }
function HP_Client_SendPart(pClient: HP_Client; const pBuffer: Pointer; iLength: Integer; iOffset: Integer): BOOL;
stdcall; external HPSocketDLL;

{ TODO
  * 名称：发送多组数据
  * 描述：向服务端发送多组数据
  *		TCP - 顺序发送所有数据包
  *		UDP - 把所有数据包组合成一个数据包发送（数据包的总长度不能大于设置的 UDP 包最大长度）
  *
  * 参数：		pBuffers	-- 发送缓冲区数组
  *			iCount		-- 发送缓冲区数目
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码
  */ }
function HP_Client_SendPackets(pClient: HP_Client; const pBuffers: WSABUFArray; iCount: Integer): BOOL; stdcall; external HPSocketDLL;

{ /******************************************************************************/
  /***************************** Client 属性访问方法 *****************************/ }

{ TODO 设置连接的附加数据 */ }
procedure HP_Client_SetExtra(pClient: HP_Client; pExtra: Pointer); stdcall; external HPSocketDLL;
{ TODO 获取连接的附加数据 */ }
function HP_Client_GetExtra(pClient: HP_Client): Pointer; stdcall; external HPSocketDLL;

{ TODO 检查通信组件是否已启动 */ }
function HP_Client_HasStarted(pClient: HP_Client): BOOL; stdcall; external HPSocketDLL;
{ TODO 查看通信组件当前状态 */ }
function HP_Client_GetState(pClient: HP_Client): En_HP_ServiceState; stdcall; external HPSocketDLL;
{ TODO 获取最近一次失败操作的错误代码 */ }
function HP_Client_GetLastError(pClient: HP_Client): En_HP_SocketError; stdcall; external HPSocketDLL;
{ TODO 获取最近一次失败操作的错误描述 */ }
function HP_Client_GetLastErrorDesc(pClient: HP_Client): PWideChar; stdcall; external HPSocketDLL;
{ TODO 获取该组件对象的连接 ID */ }
function HP_Client_GetConnectionID(pClient: HP_Client): HP_CONNID; stdcall; external HPSocketDLL;
{ TODO 获取 Client Socket 的地址信息 */ }
function HP_Client_GetLocalAddress(pClient: HP_Client; lpszAddress: PWideChar; piAddressLen: PInteger; pusPort: PUShort): BOOL;
stdcall; external HPSocketDLL;
{ TODO 获取连接中未发出数据的长度 */ }
function HP_Client_GetPendingDataLength(pClient: HP_Client; piPending: PInteger): BOOL; stdcall; external HPSocketDLL;
{ TODO 设置内存块缓存池大小（通常设置为 -> PUSH 模型：5 - 10；PULL 模型：10 - 20 ） */ }
procedure HP_Client_SetFreeBufferPoolSize(pClient: HP_Client; dwFreeBufferPoolSize: LongInt); stdcall; external HPSocketDLL;
{ TODO 设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍） */ }
procedure HP_Client_SetFreeBufferPoolHold(pClient: HP_Client; dwFreeBufferPoolHold: LongInt); stdcall; external HPSocketDLL;
{ TODO 获取内存块缓存池大小 */ }
function HP_Client_GetFreeBufferPoolSize(pClient: HP_Client): LongInt; stdcall; external HPSocketDLL;
{ TODO 获取内存块缓存池回收阀值 */ }
function HP_Client_GetFreeBufferPoolHold(pClient: HP_Client): LongInt; stdcall; external HPSocketDLL;

{ /**********************************************************************************/
  /******************************* TCP Client 操作方法 *******************************/ }

{ /*
  * 名称：发送小文件
  * 描述：向服务端发送 4096 KB 以下的小文件
  *
  * 参数：		lpszFileName	-- 文件路径
  *			pHead			-- 头部附加数据
  *			pTail			-- 尾部附加数据
  * 返回值：	TRUE	-- 成功
  *			FALSE	-- 失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码
  */ }
function HP_TcpClient_SendSmallFile(pClient: HP_Client; lpszFileName: PWideChar; const pHead: LPWSABUF; const pTail: LPWSABUF): BOOL;
stdcall; external HPSocketDLL;

{ /**********************************************************************************/
  /***************************** TCP Client 属性访问方法 *****************************/ }

{ TODO 设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为：(N * 1024) - sizeof(TBufferObj)） */ }
procedure HP_TcpClient_SetSocketBufferSize(pClient: HP_TcpClient; dwSocketBufferSize: LongInt); stdcall; external HPSocketDLL;
{ TODO 设置心跳包间隔（毫秒，0 则不发送心跳包） */ }
procedure HP_TcpClient_SetKeepAliveTime(pClient: HP_TcpClient; dwKeepAliveTime: LongInt); stdcall; external HPSocketDLL;
{ TODO 设置心跳确认包检测间隔（毫秒，0 不发送心跳包，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线） */ }
procedure HP_TcpClient_SetKeepAliveInterval(pClient: HP_TcpClient; dwKeepAliveInterval: LongInt); stdcall; external HPSocketDLL;

{ TODO 获取通信数据缓冲区大小 */ }
function HP_TcpClient_GetSocketBufferSize(pClient: HP_TcpClient): LongInt; stdcall; external HPSocketDLL;
{ TODO 获取心跳检查次数 */ }
function HP_TcpClient_GetKeepAliveTime(pClient: HP_TcpClient): LongInt; stdcall; external HPSocketDLL;
{ TODO 获取心跳检查间隔 */ }
function HP_TcpClient_GetKeepAliveInterval(pClient: HP_TcpClient): LongInt; stdcall; external HPSocketDLL;

{***************************** UDP Client 属性访问方法 *****************************/ }

{ TODO 设置数据报文最大长度（建议在局域网环境下不超过 1472 字节，在广域网环境下不超过 548 字节） */ }
procedure HP_UdpClient_SetMaxDatagramSize(pClient: HP_UdpClient; dwMaxDatagramSize: LongInt); stdcall; external HPSocketDLL;
{ TODO 获取数据报文最大长度 */ }
function HP_UdpClient_GetMaxDatagramSize(pClient: HP_UdpClient): LongInt; stdcall; external HPSocketDLL;

{ TODO 设置监测包尝试次数（0 则不发送监测跳包，如果超过最大尝试次数则认为已断线） */ }
procedure HP_UdpClient_SetDetectAttempts(pClient: HP_UdpClient; dwDetectAttempts: LongInt); stdcall; external HPSocketDLL;
{ TODO 设置监测包发送间隔（秒，0 不发送监测包） */ }
procedure HP_UdpClient_SetDetectInterval(pClient: HP_UdpClient; dwDetectInterval: LongInt); stdcall; external HPSocketDLL;
{ TODO 获取心跳检查次数 */ }
function HP_UdpClient_GetDetectAttempts(pClient: HP_UdpClient): LongInt; stdcall; external HPSocketDLL;
{ TODO 获取心跳检查间隔 */ }
function HP_UdpClient_GetDetectInterval(pClient: HP_UdpClient): LongInt; stdcall; external HPSocketDLL;

{ ***************************** UDP Cast 属性访问方法 *****************************/ }
{TODO 设置数据报文最大长度（建议在局域网环境下不超过 1472 字节，在广域网环境下不超过 548 字节） */}
procedure HP_UdpCast_SetMaxDatagramSize(pCast: HP_UdpCast; dwMaxDatagramSize: LongInt); stdcall; external HPSocketdll;
{TODO 获取数据报文最大长度 */ }
function HP_UdpCast_GetMaxDatagramSize(pCast: HP_UdpCast): LongInt; stdcall; external HPSocketdll;
{TODO 获取当前数据报的远程地址信息（通常在 OnReceive 事件中调用） */ }
function HP_UdpCast_GetRemoteAddress(pCast: HP_UdpCast; lpszAddress: PWideChar; piAddresslen: PInteger; pusPort: PUShort): BOOL;
stdcall; external HPSocketDLL;
{TODO 设置绑定地址 */ }
procedure HP_UdpCast_SetBindAdddress(pCast: HP_UdpCast; pszBindAddress: PWideChar); stdcall; external HPSocketDll;
{TODO 获取绑定地址 */  }
function HP_UdpCast_GetBindAdddress(pCast: HP_UdpCast): WideChar; stdcall external HPSocketDll;
{TODO 设置是否启用地址重用机制（默认：不启用） */  }
procedure HP_UdpCast_SetReuseAddress(pCast: HP_UdpCast; bReuseAddress: BOOL); stdcall; external HPSocketDll;
{TODO 检测是否启用地址重用机制 */  }
function HP_UdpCast_IsReuseAddress(pCast: HP_UdpCast): BOOL; stdcall; external HPSocketDll;
{TODO 设置传播模式（组播或广播） */  }
procedure HP_UdpCast_SetCastMode(pCast: HP_UdpCast; enCastMode: En_HP_CastMode); stdcall; external HPSocketDll;
{TODO 获取传播模式 */  }
function HP_UdpCast_GetCastMode(pCast: HP_UdpCast): En_HP_CastMode; stdcall; external HPSocketDll;
{TODO 设置组播报文的 TTL（0 - 255） */    }
procedure HP_UdpCast_SetMultiCastTtl(PChar: HP_UdpCast; iMCTtl: LongInt); stdcall; external HPSocketDll;
{TODO 获取组播报文的 TTL */ }
function HP_UdpCast_GetMultiCastTtl(PChar: HP_UdpCast): LongInt; stdcall; external HPSocketDll;
{TODO 设置是否启用组播环路（TRUE or FALSE） */   }
procedure HP_UdpCast_SetMultiCastLoop(PChar: HP_UdpCast; bMCLoop: BOOL); stdcall; external HPSocketDll;
{TODO 检测是否启用组播环路 */ }
function HP_UdpCast_IsMultiCastLoop(PChar: HP_UdpCast): BOOL; stdcall; external HPSocketDll;

{ **************************** TCP Pull Server 组件操作方法 *****************************/ }

{TODO
  * 名称：抓取数据
  * 描述：用户通过该方法从 Socket 组件中抓取数据
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pBuffer		-- 数据抓取缓冲区
  *			iLength		-- 抓取数据长度
  * 返回值：	En_HP_FetchResult
  */ }
function HP_TcpPullServer_Fetch(pServer: HP_TcpPullServer; dwConnID: HP_CONNID; pData: Pointer; iLength: Integer): En_HP_FetchResult;
stdcall; external HPSocketDLL;

{  TODO
* 名称：窥探数据（不会移除缓冲区数据）
* 描述：用户通过该方法从 Socket 组件中窥探数据
*
* 参数：		dwConnID	-- 连接 ID
*			pData		-- 窥探缓冲区
*			iLength		-- 窥探数据长度
* 返回值：	En_HP_FetchResult
*/ }
function HP_TcpPullServer_Peek(pServer: HP_TcpPullServer; dwConnID: HP_CONNID; pData: Pointer; iLength: Integer): En_HP_FetchResult;
stdcall; external HPSocketDLL;

{ /***************************************************************************************/
  /***************************** TCP Pull Server 属性访问方法 *****************************/ }

{ /***************************************************************************************/
  /***************************** TCP Pull Agent 组件操作方法 *****************************/ }

{ TODO
  * 名称：抓取数据
  * 描述：用户通过该方法从 Socket 组件中抓取数据
  *
  * 参数：		dwConnID	-- 连接 ID
  *			pBuffer		-- 数据抓取缓冲区
  *			iLength		-- 抓取数据长度
  * 返回值：	En_HP_FetchResult
  */ }
function HP_TcpPullAgent_Fetch(pAgent: HP_TcpPullAgent; dwConnID: HP_CONNID; pBuffer: Pointer; iLength: Integer): En_HP_FetchResult;
stdcall; external HPSocketDLL;

{TODO* 名称：窥探数据（不会移除缓冲区数据）
* 描述：用户通过该方法从 Socket 组件中窥探数据
*
* 参数：		dwConnID	-- 连接 ID
*			pData		-- 窥探缓冲区
*			iLength		-- 窥探数据长度
* 返回值：	En_HP_FetchResult   }
function HP_TcpPullAgent_Peek(pAgent: HP_TcpPullAgent; dwConnID: HP_CONNID; pBuffer: Pointer; iLength: Integer): En_HP_FetchResult;
stdcall; external HPSocketDLL;

{ ***************************** TCP Pull Client 组件操作方法 *****************************/ }
{ TODO
* 名称：抓取数据
* 描述：用户通过该方法从 Socket 组件中抓取数据
*
* 参数：	dwConnID	-- 连接 ID
*			    pData		-- 抓取缓冲区
*		    	iLength		-- 抓取数据长度
* 返回值：	En_HP_FetchResult
  */ }
function HP_TcpPullClient_Fetch(pClient: HP_TcpPullClient; pData: Pointer; iLength: Integer): En_HP_FetchResult;
stdcall; external HPSocketDLL;

{  TODO
* 名称：窥探数据（不会移除缓冲区数据）
* 描述：用户通过该方法从 Socket 组件中窥探数据
*
* 参数：		dwConnID	-- 连接 ID
*			pData		-- 窥探缓冲区
*			iLength		-- 窥探数据长度
* 返回值：	En_HP_FetchResult
*/ }
function HP_TcpPullClient_Peek(pClient: HP_TcpPullClient; pData: Pointer; iLength: Integer): En_HP_FetchResult;
stdcall; external HPSocketDLL;
{/***************************** TCP Pack Server 属性访问方法 *****************************/ }

{TODO/* 设置数据包最大长度（有效数据包最大长度不能超过 524287/0x7FFFF 字节，默认：262144/0x40000） */}
procedure HP_TcpPackServer_SetMaxPackSize(pServer: HP_TcpPackServer ; dwMaxPackSize: LongWord); stdcall; external HPSocketDLL;
{TODO 设置包头标识（有效包头标识取值范围 0 ~ 8191/0x1FFF，当包头标识为 0 时不校验包头，默认：0） */ }
procedure HP_TcpPackServer_SetPackHeaderFlag(pServer: HP_TcpPackServer ; usPackHeaderFlag: Word); stdcall; external HPSocketDLL;

{TODO 获取数据包最大长度 */ }
function HP_TcpPackServer_GetMaxPackSize(pServer: HP_TcpPackServer): LongWord; stdcall; external HPSocketDLL;
{TODO 获取包头标识 */     }
function HP_TcpPackServer_GetPackHeaderFlag(pServer: HP_TcpPackServer): Word; stdcall; external HPSocketDLL;

//***************************** TCP Pack Agent 属性访问方法 *****************************/

{TODO 设置数据包最大长度（有效数据包最大长度不能超过 524287/0x7FFFF 字节，默认：262144/0x40000） */}
procedure HP_TcpPackAgent_SetMaxPackSize(pAgent: HP_TcpPackAgent; dwMaxPackSize: LongWord); stdcall; external HPSocketDLL;
{TODO 设置包头标识（有效包头标识取值范围 0 ~ 8191/0x1FFF，当包头标识为 0 时不校验包头，默认：0） */ }
procedure HP_TcpPackAgent_SetPackHeaderFlag(pAgent: HP_TcpPackAgent; usPackHeaderFlag: Word); stdcall; external HPSocketDLL;

{TODO 获取数据包最大长度 */ }
function HP_TcpPackAgent_GetMaxPackSize(pAgent: HP_TcpPackAgent): LongWord; stdcall; external HPSocketDLL;
{TODO 获取包头标识 */ }
function HP_TcpPackAgent_GetPackHeaderFlag(pAgent: HP_TcpPackAgent): Word; stdcall; external HPSocketDLL;

//***************************** TCP Pack Client 属性访问方法 *****************************/

{TODO  设置数据包最大长度（有效数据包最大长度不能超过 524287/0x7FFFF 字节，默认：262144/0x40000） */  }
procedure HP_TcpPackClient_SetMaxPackSize(pClient: HP_TcpPackAgent; dwMaxPackSize: LongWord); stdcall; external HPSocketDLL;
{TODO  设置包头标识（有效包头标识取值范围 0 ~ 8191/0x1FFF，当包头标识为 0 时不校验包头，默认：0） */ }
procedure HP_TcpPackClient_SetPackHeaderFlag(pClient: HP_TcpPackAgent; usPackHeaderFlag: Word); stdcall; external HPSocketDLL;

{TODO  获取数据包最大长度 */ }
function HP_TcpPackClient_GetMaxPackSize(pClient: HP_TcpPackAgent): LongWord; stdcall; external HPSocketDLL;
{TODO  获取包头标识 */ }
function HP_TcpPackClient_GetPackHeaderFlag(pClient: HP_TcpPackAgent): Word; stdcall; external HPSocketDLL;

{ ************************************** 其它方法 ***************************************/ }
{ /* 获取错误描述文本 */ }
function HP_GetSocketErrorDesc(enCode: En_HP_SocketError): PWideChar; stdcall; external HPSocketDLL;
{ /* 调用系统的 ::GetLastError() 方法获取系统错误代码 */ }
function SYS_GetLastError(): LongInt; stdcall; external HPSocketDLL;
// 调用系统的 ::WSAGetLastError() 方法获取通信错误代码
function SYS_WSAGetLastError(): Integer; stdcall; external HPSocketDLL;
// 调用系统的 setsockopt()
function SYS_SetSocketOption(sock: SOCKET; level: Integer; name: Integer; val: pointer; len: Integer): Integer; stdcall; external HPSocketDLL;
// 调用系统的 getsockopt()
function SYS_GetSocketOption(sock: SOCKET; level: Integer; name: Integer; val: Pointer; len: PInteger): Integer; stdcall; external HPSocketDLL;
// 调用系统的 ioctlsocket()
function SYS_IoctlSocket(sock: SOCKET; cmd: LongInt; arg: PULONG): Integer; stdcall; external HPSocketDLL;
// 调用系统的 ::WSAIoctl()
function SYS_WSAIoctl(sock: SOCKET; dwIoControlCode: LongInt; lpvInBuffer: pointer; cbInBuffer: LongInt; lpvOutBuffer: pointer; cbOutBuffer: LongInt; lpcbBytesReturned: LPDWORD): Integer; stdcall; external HPSocketDLL;


implementation


initialization
  IsMultiThread := True;
end.

