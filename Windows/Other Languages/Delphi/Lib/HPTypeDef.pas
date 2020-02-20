unit HPTypeDef;

interface

uses
  Winapi.Windows;

const
  HPSocketDLL = 'HPSocket4C_U.dll';

  HP_VERSION_MAJOR=	5;	// 主版本号
  HP_VERSION_MINOR=5;	// 子版本号
  HP_VERSION_REVISE=1;	// 修正版本号
  HP_VERSION_BUILD=5;	// 构建编号

{/*****************************************************************************************************************************************************/
/**************************************************************** Base Type Definitions **************************************************************/
/*****************************************************************************************************************************************************/}

type
  {$Z4}

  { /************************************************************************
    名称：连接 ID 数据类型
    描述：定义连接 ID 的数据类型
    ************************************************************************/ }
  CONNID  = PLongWord;
  HP_CONNID = PLongWord;
  HP_CONNIDArray = array of HP_CONNID; { TODO : 这个数组变量用来获取所有连接id }
  
  {TODO:/************************************************************************
    名称：通信组件服务状态
    描述：应用程序可以通过通信组件的 GetState() 方法获取组件当前服务状态
    ************************************************************************ }
  En_HP_ServiceState = (
    SS_STARTING  = 0,    // 正在启动
    SS_STARTED   = 1,    // 已经启动
    SS_STOPPING  = 2,    // 正在停止
    SS_STOPPED   = 3     // 已经停止
  );

  { TODO:************************************************************************
    名称：Socket 操作类型
    描述：应用程序的 OnClose() 事件中通过该参数标识是哪种操作导致的错误
    ************************************************************************ }
  En_HP_SocketOperation = (
    SO_UNKNOWN = 0,  // Unknown
    SO_ACCEPT  = 1,  // Acccept
    SO_CONNECT = 2,  // Connnect
    SO_SEND    = 3,  // Send
    SO_RECEIVE = 4,  // Receive
    SO_CLOSE	 = 5   // Close
  );

  {TODO: ************************************************************************
    名称：事件通知处理结果
    描述：事件通知的返回值，不同的返回值会影响通信组件的后续行为
    ************************************************************************ }
  En_HP_HandleResult = (
    HR_OK     = 0, // 成功处理
    HR_IGNORE = 1, // 忽略处理
    HR_ERROR  = 2  // 错误处理
  );

  { TODO:/************************************************************************
    名称：数据抓取结果
    描述：数据抓取操作的返回值
    ************************************************************************/ }
  En_HP_FetchResult = (
    FR_OK               = 0,  // 成功
    FR_LENGTH_TOO_LONG  = 1,  // 抓取长度过大
    FR_DATA_NOT_FOUND   = 2   // 找不到 ConnID 对应的数据
  );

  { TODO:/************************************************************************
    名称：数据发送策略
    描述：Server 组件和 Agent 组件的数据发送策略

    * 打包模式（默认）	：尽量把多个发送操作的数据组合在一起发送，增加传输效率
    * 安全模式			    ：尽量把多个发送操作的数据组合在一起发送，并控制传输速度，避免缓冲区溢出
    * 直接模式			    ：对每一个发送操作都直接投递，适用于负载不高但要求实时性较高的场合
    ************************************************************************/ }
  En_HP_SendPolicy = (
    SP_PACK    = 0, // 打包模式（默认）
    SP_SAFE    = 1, // 安全模式
    SP_DIRECT  = 2 // 直接模式
  );

  {************************************************************************
   名称：OnSend 事件同步策略
   描述：Server 组件和 Agent 组件的 OnSend 事件同步策略

   * 不同步（默认）	：不同步 OnSend 事件，可能同时触发 OnReceive 和 OnClose 事件
   * 同步 OnClose	：只同步 OnClose 事件，可能同时触发 OnReceive 事件
   * 同步 OnReceive	：（只用于 TCP 组件）同步 OnReceive 和 OnClose 事件，不可能同时触发 OnReceive 或 OnClose 事件
   ************************************************************************}
  En_HP_OnSendSyncPolicy = (
	  OSSP_NONE			  = 0,	// 不同步（默认）
	  OSSP_CLOSE			= 1,	// 同步 OnClose
	  OSSP_RECEIVE		= 2	  // 同步 OnReceive（只用于 TCP 组件）
  );

  { TODO:************************************************************************
    名称：操作结果代码
    描述：Start() / Stop() 方法执行失败时，可通过 GetLastError() 获取错误代码
    ************************************************************************ }
  En_HP_SocketError = (
    SE_OK                     = 0, // 成功
    SE_ILLEGAL_STATE          = 1, // 当前状态不允许操作
    SE_INVALID_PARAM          = 2, // 非法参数
    SE_SOCKET_CREATE          = 3, // 创建 SOCKET 失败
    SE_SOCKET_BIND            = 4, // 绑定 SOCKET 失败
    SE_SOCKET_PREPARE         = 5, // 设置 SOCKET 失败
    SE_SOCKET_LISTEN          = 6, // 监听 SOCKET 失败
    SE_CP_CREATE              = 7, // 创建完成端口失败
    SE_WORKER_THREAD_CREATE   = 8, // 创建工作线程失败
    SE_DETECT_THREAD_CREATE   = 9, // 创建监测线程失败
    SE_SOCKE_ATTACH_TO_CP     = 10, // 绑定完成端口失败
    SE_CONNECT_SERVER         = 11, // 连接服务器失败
    SE_NETWORK                = 12, // 网络错误
    SE_DATA_PROC              = 13, // 数据处理错误
    SE_DATA_SEND              = 14, // 数据发送失败

    //***** SSL Socket 扩展操作结果代码 *****/
    SE_SSL_ENV_NOT_READY      = 101     // SSL 环境未就绪
  );

  {TODO:/************************************************************************
  名称：播送模式
  描述：UDP 组件的播送模式（组播或广播）
  ************************************************************************/}
  En_HP_CastMode = (
    CM_MULTICAST = 0, // 组播
    CM_BROADCAST = 1  // 广播
  );

  {************************************************************************
      名称：IP 地址类型
      描述：IP 地址类型枚举值
  ************************************************************************/}
  En_HP_IPAddrType =
  (
    IPT_ALL		= 0,		// 所有
    IPT_IPV4	= 1,		// IPv4
    IPT_IPV6	= 2		  // IPv6
  );


  {/************************************************************************
  名称：IP 地址条目结构体
  描述：IP 地址的地址簇/地址值结构体
  ************************************************************************/}
  HP_TIPAddr = packed record
    Ftype: En_HP_IPAddrType;
    address: pAnsiChar;
  end;

  LPTIPAddr= ^HP_TIPAddr;
  HP_LPTIPAddr= LPTIPAddr;

  {/************************************************************************
    名称：拒绝策略
    描述：调用被拒绝后的处理策略
   ************************************************************************/}
  En_HP_RejectedPolicy = (
    TRP_CALL_FAIL	= 0,	// 立刻返回失败
    TRP_WAIT_FOR	= 1,	// 等待（直到成功、超时或线程池关闭等原因导致失败）
    TRP_CALLER_RUN	= 2	// 调用者线程直接执行
  );

  {/************************************************************************
   名称：任务缓冲区类型
   描述：TSockeTask 对象创建和销毁时，根据不同类型的缓冲区类型作不同的处理
   ************************************************************************/}
  En_HP_TaskBufferType = (
	  TBT_COPY		= 0,	// 深拷贝
	  TBT_REFER		= 1,	// 浅拷贝
	  TBT_ATTACH		= 2	// 连接（不负责创建，但负责销毁）
  );

  {/************************************************************************
    名称：任务处理函数
    描述：任务处理入口函数
    参数：pvArg -- 自定义参数
    返回值：（无）
    ************************************************************************/}
    //typedef VOID (CALLBACK *Fn_TaskProc)(PVOID pvArg);
    //typedef Fn_TaskProc	HP_Fn_TaskProc;

  Fn_TaskProc  =procedure(pvArg: pointer);  stdcall;
  HP_Fn_TaskProc = Fn_TaskProc;

  LPTSocketTask = ^TSocketTask;

  {/************************************************************************
    名称：Socket 任务处理函数
    描述：Socket 任务处理入口函数
    参数：pTask -- Socket 任务结构体指针
    返回值：（无）
    ************************************************************************/}
  //typedef VOID (CALLBACK *Fn_SocketTaskProc)(TSocketTask* pTask);
  //typedef Fn_SocketTaskProc	HP_Fn_SocketTaskProc;
  Fn_SocketTaskProc  = procedure(pTask: LPTSocketTask);  stdcall;
  HP_Fn_SocketTaskProc = Fn_SocketTaskProc;


  {/************************************************************************
    名称：Socket 任务结构体
    描述：封装 Socket 任务相关数据结构
    ************************************************************************/}
  TSocketTask = record
  	fn: Fn_SocketTaskProc;			// 任务处理函数
	  sender: pointer	;		// 发起对象
	  connID: CONNID;		// 连接 ID
	  buf: PByte;		// 数据缓冲区
	  bufLen: Integer;		// 数据缓冲区长度
	  bufType: En_HP_TaskBufferType;	// 缓冲区类型
	  wparam: WPARAM;		// 自定义参数
	  lparam: LPARAM;		// 自定义参数
  end;
  HP_TSocketTask = TSocketTask;
  HP_LPTSocketTask=  ^HP_TSocketTask;

  {/************************************************************************
  名称：SSL 工作模式
  描述：标识 SSL 的工作模式，客户端模式或服务端模式
  ************************************************************************/}
  En_HP_SSLSessionMode   =
  (
    SSL_SM_CLIENT	= 0,	// 客户端模式
    SSL_SM_SERVER	= 1	  // 服务端模式
  );

  {/************************************************************************
  名称：SSL 验证模式
  描述：SSL 验证模式选项，SSL_VM_PEER 可以和后面两个选项组合一起
  ************************************************************************/}
  En_HP_SSLVerifyMode =
  (
    SSL_VM_NONE					        = $00,	// SSL_VERIFY_NONE
    SSL_VM_PEER					        = $01,	// SSL_VERIFY_PEER
    SSL_VM_FAIL_IF_NO_PEER_CERT	= $02,	// SSL_VERIFY_FAIL_IF_NO_PEER_CERT
    SSL_VM_CLIENT_ONCE			    = $04	  // SSL_VERIFY_CLIENT_ONCE
  );

{/************************************************************************
名称：SNI 服务名称回调函数
描述：根据服务器名称选择 SSL 证书
参数：
		lpszServerName -- 服务器名称（域名）
返回值：
		0	 -- 成功，使用默认 SSL 证书
		正数	 -- 成功，使用返回值对应的 SNI 主机证书
		负数	 -- 失败，中断 SSL 握手

************************************************************************/}
  Fn_SNI_ServerNameCallback  =function(lpszServerName: PChar; pContext: PVOID): Integer;  stdcall;
  HP_Fn_SNI_ServerNameCallback = Fn_SNI_ServerNameCallback;


{/************************************************************************
名称：HTTP 版本
描述：低字节：主版本号，高字节：次版本号
************************************************************************/}
  En_HP_HttpVersion = (
    HV_1_0 = 1,
    HV_1_1 = 257
  );

{/************************************************************************
名称：URL 域
描述：HTTP 请求行中 URL 段位的域定义
************************************************************************/ }
  En_HP_HttpUrlField = (
    HUF_SCHEMA		= 0,	// Schema
    HUF_HOST		  = 1,	// Host
    HUF_PORT		  = 2,	// Port
    HUF_PATH		  = 3,	// Path
    HUF_QUERY		  = 4,	// Query String
    HUF_FRAGMENT	= 5,	// Fragment
    HUF_USERINFO	= 6,	// User Info
    HUF_MAX			  = 7	  // (Field Count)
  );

{/************************************************************************
名称：HTTP 解析结果标识
描述：指示 HTTP 解析器是否继续执行解析操作
************************************************************************/}
  En_HP_HttpParseResult = (

    HPR_OK	      = 0,			// 继续解析
    HPR_SKIP_BODY	= 1,	    // 跳过当前请求 BODY（仅用于 OnHeadersComplete 事件）
    HPR_UPGRADE		= 2,	    // 升级协议（仅用于 OnHeadersComplete 事件）
    HPR_ERROR		  = -1	    // 终止解析，断开连接
  );

{/************************************************************************
名称：HTTP 协议升级类型
描述：标识 HTTP 升级为哪种协议
************************************************************************/ }
  En_HP_HttpUpgradeType = (

    HUT_NONE		    = 0,	// 没有升级
    HUT_WEB_SOCKET	= 1,	// WebSocket
    HUT_HTTP_TUNNEL	= 2,	// HTTP 隧道
    HUT_UNKNOWN		  = -1	// 未知类型
  );

{/************************************************************************
名称：HTTP 状态码
描述：HTTP 标准状态码
************************************************************************/}
  En_HP_HttpStatusCode = (

    HSC_CONTINUE						            = 100,
    HSC_SWITCHING_PROTOCOLS				      = 101,
    HSC_PROCESSING						          = 102,

    HSC_OK								              = 200,
    HSC_CREATED							            = 201,
    HSC_ACCEPTED						            = 202,
    HSC_NON_AUTHORITATIVE_INFORMATION	  = 203,
    HSC_NO_CONTENT						          = 204,
    HSC_RESET_CONTENT					          = 205,
    HSC_PARTIAL_CONTENT					        = 206,
    HSC_MULTI_STATUS				            = 207,
    HSC_ALREADY_REPORTED				        = 208,
    HSC_IM_USED							            = 226,

    HSC_MULTIPLE_CHOICES				        = 300,
    HSC_MOVED_PERMANENTLY				        = 301,
    HSC_MOVED_TEMPORARILY				        = 302,
    HSC_SEE_OTHER						            = 303,
    HSC_NOT_MODIFIED					          = 304,
    HSC_USE_PROXY						            = 305,
    HSC_SWITCH_PROXY					          = 306,
    HSC_TEMPORARY_REDIRECT				      = 307,
    HSC_PERMANENT_REDIRECT				      = 308,

    HSC_BAD_REQUEST						          = 400,
    HSC_UNAUTHORIZED					          = 401,
    HSC_PAYMENT_REQUIRED				        = 402,
    HSC_FORBIDDEN						            = 403,
    HSC_NOT_FOUND						            = 404,
    HSC_METHOD_NOT_ALLOWED				      = 405,
    HSC_NOT_ACCEPTABLE					        = 406,
    HSC_PROXY_AUTHENTICATION_REQUIRED	  = 407,
    HSC_REQUEST_TIMEOUT					        = 408,
    HSC_CONFLICT						            = 409,
    HSC_GONE							              = 410,
    HSC_LENGTH_REQUIRED					        = 411,
    HSC_PRECONDITION_FAILED				      = 412,
    HSC_REQUEST_ENTITY_TOO_LARGE		    = 413,
    HSC_REQUEST_URI_TOO_LONG			      = 414,
    HSC_UNSUPPORTED_MEDIA_TYPE			    = 415,
    HSC_REQUESTED_RANGE_NOT_SATISFIABLE	= 416,
    HSC_EXPECTATION_FAILED				      = 417,
    HSC_MISDIRECTED_REQUEST				      = 421,
    HSC_UNPROCESSABLE_ENTITY			      = 422,
    HSC_LOCKED							            = 423,
    HSC_FAILED_DEPENDENCY				        = 424,
    HSC_UNORDERED_COLLECTION			      = 425,
    HSC_UPGRADE_REQUIRED				        = 426,
    HSC_PRECONDITION_REQUIRED			      = 428,
    HSC_TOO_MANY_REQUESTS				        = 429,
    HSC_REQUEST_HEADER_FIELDS_TOO_LARGE	= 431,
    HSC_UNAVAILABLE_FOR_LEGAL_REASONS	  = 451,
    HSC_RETRY_WITH						          = 449,

    HSC_INTERNAL_SERVER_ERROR			      = 500,
    HSC_NOT_IMPLEMENTED					        = 501,
    HSC_BAD_GATEWAY						          = 502,
    HSC_SERVICE_UNAVAILABLE				      = 503,
    HSC_GATEWAY_TIMEOUT					        = 504,
    HSC_HTTP_VERSION_NOT_SUPPORTED		  = 505,
    HSC_VARIANT_ALSO_NEGOTIATES			    = 506,
    HSC_INSUFFICIENT_STORAGE			      = 507,
    HSC_LOOP_DETECTED					          = 508,
    HSC_BANDWIDTH_LIMIT_EXCEEDED		    = 509,
    HSC_NOT_EXTENDED					          = 510,
    HSC_NETWORK_AUTHENTICATION_REQUIRED	= 511,

    HSC_UNPARSEABLE_RESPONSE_HEADERS	  = 600
  );

{/************************************************************************
名称：SSL Session 信息类型
描述：用于 GetSSLSessionInfo()，标识输出的 Session 信息类型
************************************************************************/}
  En_HP_SSLSessionInfo = (
    SSL_SSI_MIN				           	= 0,	       //
    SSL_SSI_CTX				          	= 0,	       // SSL CTX				            （输出类型：SSL_CTX*）
    SSL_SSI_CTX_METHOD			      = 1,	       // SSL CTX Mehtod		        （输出类型：SSL_METHOD*）
    SSL_SSI_CTX_CIPHERS			      = 2,	       // SSL CTX Ciphers		        （输出类型：STACK_OF(SSL_CIPHER)*）
    SSL_SSI_CTX_CERT_STORE		    = 3,         // SSL CTX Cert Store	      （输出类型：X509_STORE*）
    SSL_SSI_SERVER_NAME_TYPE	    = 4,         // Server Name Type		      （输出类型：int）
    SSL_SSI_SERVER_NAME		      	= 5,	       // Server Name			          （输出类型：LPCSTR）
    SSL_SSI_VERSION				        = 6,	       // SSL Version			          （输出类型：LPCSTR）
    SSL_SSI_METHOD				        = 7,	       // SSL Method			          （输出类型：SSL_METHOD*）
    SSL_SSI_CERT				          = 8,	       // SSL Cert				          （输出类型：X509*）
    SSL_SSI_PKEY				          = 9,	       // SSL Private Key		        （输出类型：EVP_PKEY*）
    SSL_SSI_CURRENT_CIPHER		    = 10,	       // SSL Current Cipher	      （输出类型：SSL_CIPHER*）
    SSL_SSI_CIPHERS				        = 11,	       // SSL Available Ciphers     （输出类型：STACK_OF(SSL_CIPHER)*）
    SSL_SSI_CLIENT_CIPHERS		    = 12,	       // SSL Client Ciphers	      （输出类型：STACK_OF(SSL_CIPHER)*）
    SSL_SSI_PEER_CERT			        = 13,	       // SSL Peer Cert		          （输出类型：X509*）
    SSL_SSI_PEER_CERT_CHAIN		    = 14,	       // SSL Peer Cert Chain	      （输出类型：STACK_OF(X509)*）
    SSL_SSI_VERIFIED_CHAIN		    = 15,	       // SSL Verified Chain	      （输出类型：STACK_OF(X509)*）
    SSL_SSI_MAX					          = 15	       //
  );

{/************************************************************************
名称：Name/Value 结构体
描述：字符串名值对结构体
************************************************************************/}
  //WSABUF = Record
  //  name: PAnsiChar; { the length of the buffer }
  //  value: PAnsiChar; { the pointer to the buffer }
 // end { WSABUF };
  //TNVPair = array of WSABUF;

  TNVPair = record
	  name:  pAnsichar;
	  value: pAnsichar;
  end;

  HP_TNVPair= TNVPair;

  LPPARAM=    ^TParam;
  TParam = TNVPair;
	HP_LPPARAM= ^HP_TParam;
  HP_TParam=  TNVPair;

  LPHEADER=     ^THeader;
  THeader =  TNVPair;
  HP_LPHEADER = ^HP_THeader;
  HP_THeader=   THeader;

  LPCOOKIE=   ^TCookie;
  TCookie =  TNVPair;
  HP_LPCOOKIE = ^HP_TCookie;
  HP_TCookie=   TCookie;



  HP_THeaderAry = array of HP_THeader;


  HP_TCookieAry= array of HP_TCookie;

  PLPVOID = ^LPVOID;

{/************************************************************************
名称：获取 HPSocket 版本号
描述：版本号（4 个字节分别为：主版本号，子版本号，修正版本号，构建编号）
************************************************************************/}
function GetHPSocketVersion(): int64;

implementation

{/************************************************************************
名称：获取 HPSocket 版本号
描述：版本号（4 个字节分别为：主版本号，子版本号，修正版本号，构建编号）
************************************************************************/}
function GetHPSocketVersion(): int64;
begin
	result:= (HP_VERSION_MAJOR shl 24) or
           (HP_VERSION_MINOR shl 16) or
           (HP_VERSION_REVISE shl 8) or
           HP_VERSION_BUILD;
end;

end.
