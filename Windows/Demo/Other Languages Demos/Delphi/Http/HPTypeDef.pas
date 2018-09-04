unit HPTypeDef;

interface


type
   {******************************************** 公共常量 ********************************************/}
  { /************************************************************************
    名称：连接 ID 数据类型
    描述：定义连接 ID 的数据类型
    ************************************************************************/ }
  HP_CONNID = Word;
  PHP_CONNID = ^HP_CONNID;
  HP_CONNIDArray = array of HP_CONNID; { TODO : 这个数组变量用来获取所有连接id }
  
  {TODO:/************************************************************************
    名称：通信组件服务状态
    描述：应用程序可以通过通信组件的 GetState() 方法获取组件当前服务状态
    ************************************************************************ }
  En_HP_ServiceState = (
    SS_STARTING = 0, // 正在启动
    SS_STARTED = 1, // 已经启动
    SS_STOPPING = 2, // 正在停止
    SS_STOPPED = 3 // 已经停止
    );

  { TODO:************************************************************************
    名称：Socket 操作类型
    描述：应用程序的 OnErrror() 事件中通过该参数标识是哪种操作导致的错误
    ************************************************************************ }
  En_HP_SocketOperation = (
    SO_UNKNOWN = 0, // Unknown
    SO_ACCEPT = 1, // Acccept
    SO_CONNECT = 2, // Connnect
    SO_SEND = 3, // Send
    SO_RECEIVE = 4, // Receive
    SO_CLOSE		= 5 // Close
    );

  {TODO: ************************************************************************
    名称：事件通知处理结果
    描述：事件通知的返回值，不同的返回值会影响通信组件的后续行为
    ************************************************************************ }
  En_HP_HandleResult = (
    HR_OK = 0, // 成功处理
    HR_IGNORE = 1, // 忽略处理
    HR_ERROR = 2 // 错误处理
    );

  { TODO:/************************************************************************
    名称：数据抓取结果
    描述：数据抓取操作的返回值
    ************************************************************************/ }
  En_HP_FetchResult = (
    FR_OK = 0, // 成功
    FR_LENGTH_TOO_LONG = 1, // 抓取长度过大
    FR_DATA_NOT_FOUND = 2 // 找不到 ConnID 对应的数据
    );

  { TODO:/************************************************************************
    名称：数据发送策略
    描述：Server 组件和 Agent 组件的数据发送策略

    * 打包模式（默认）	：尽量把多个发送操作的数据组合在一起发送，增加传输效率
    * 安全模式			：尽量把多个发送操作的数据组合在一起发送，并控制传输速度，避免缓冲区溢出
    * 直接模式			：对每一个发送操作都直接投递，适用于负载不高但要求实时性较高的场合

    ************************************************************************/ }
  En_HP_SendPolicy = (
    SP_PACK = 0, // 打包模式（默认）
    SP_SAFE = 1, // 安全模式
    SP_DIRECT = 2 // 直接模式
    );

  { TODO:************************************************************************
    名称：操作结果代码
    描述：Start() / Stop() 方法执行失败时，可通过 GetLastError() 获取错误代码
    ************************************************************************ }
  En_HP_SocketError = (
    SE_OK = 0, // 成功
    SE_ILLEGAL_STATE = 1, // 当前状态不允许操作
    SE_INVALID_PARAM = 2, // 非法参数
    SE_SOCKET_CREATE = 3, // 创建 SOCKET 失败
    SE_SOCKET_BIND = 4, // 绑定 SOCKET 失败
    SE_SOCKET_PREPARE = 5, // 设置 SOCKET 失败
    SE_SOCKET_LISTEN = 6, // 监听 SOCKET 失败
    SE_CP_CREATE = 7, // 创建完成端口失败
    SE_WORKER_THREAD_CREATE = 8, // 创建工作线程失败
    SE_DETECT_THREAD_CREATE = 9, // 创建监测线程失败
    SE_SOCKE_ATTACH_TO_CP = 10, // 绑定完成端口失败
    SE_CONNECT_SERVER = 11, // 连接服务器失败
    SE_NETWORK = 12, // 网络错误
    SE_DATA_PROC = 13, // 数据处理错误
    SE_DATA_SEND = 14, // 数据发送失败

    //***** SSL Socket 扩展操作结果代码 *****/
    SE_SSL_ENV_NOT_READY = 101     // SSL 环境未就绪
    );

  {TODO:/************************************************************************
  名称：播送模式
  描述：UDP 组件的播送模式（组播或广播）
  ************************************************************************/}
  En_HP_CastMode = (
    CM_MULTICAST = 0, // 组播
    CM_BROADCAST = 1 // 广播
    );

    {/************************************************************************
名称：IP 地址类型
描述：IP 地址类型枚举值
************************************************************************/}
  En_HP_IPAddrType = (
	IPT_ALL		= 0,		// 所有
	IPT_IPV4	= 1,		// IPv4
	IPT_IPV6	= 2		// IPv6
  );

 {/************************************************************************
名称：IP 地址条目结构体
描述：IP 地址的地址簇/地址值结构体
************************************************************************/}
  HP_TIPAddr  = packed record
  addr :  En_HP_IPAddrType;
	address : PChar;
  end;
  
  LPIPAddr = ^HP_TIPAddr;
  HP_LPTIPAddr = ^HP_TIPAddr;



{/************************************************************************
名称：HTTP 版本
描述：低字节：主版本号，高字节：次版本号
************************************************************************/}
  En_HP_HttpVersion = (
    HV_1_0 = 0,
    HV_1_1 = 1
   );

{/************************************************************************
名称：URL 域
描述：HTTP 请求行中 URL 段位的域定义
************************************************************************/ }
  En_HP_HttpUrlField = (

    HUF_SCHEMA		= 0,	// Schema
    HUF_HOST		= 1,	// Host
    HUF_PORT		= 2,	// Port
    HUF_PATH		= 3,	// Path
    HUF_QUERY		= 4,	// Query String
    HUF_FRAGMENT	= 5,	// Fragment
    HUF_USERINFO	= 6,	// User Info
    HUF_MAX			= 7	// (Field Count)
  );

{/************************************************************************
名称：HTTP 解析结果标识
描述：指示 HTTP 解析器是否继续执行解析操作
************************************************************************/}
  En_HP_HttpParseResult = (

    HPR_OK	= 0,			// 继续解析
    HPR_SKIP_BODY	= 1,	// 跳过当前请求 BODY（仅用于 OnHeadersComplete 事件）
    HPR_UPGRADE		= 2,	// 升级协议（仅用于 OnHeadersComplete 事件）
    HPR_ERROR		= -1	// 终止解析，断开连接
  ) ;

{/************************************************************************
名称：HTTP 协议升级类型
描述：标识 HTTP 升级为哪种协议
************************************************************************/ }
  En_HP_HttpUpgradeType = (

    HUT_NONE		= 0,	// 没有升级
    HUT_WEB_SOCKET	= 1,	// WebSocket
    HUT_HTTP_TUNNEL	= 2,	// HTTP 隧道
    HUT_UNKNOWN		= -1	// 未知类型
  );

{/************************************************************************
名称：HTTP 状态码
描述：HTTP 标准状态码
************************************************************************/}
  En_HP_HttpStatusCode = (

    HSC_CONTINUE						= 100,
    HSC_SWITCHING_PROTOCOLS				= 101,
    HSC_PROCESSING						= 102,

    HSC_OK								= 200,
    HSC_CREATED							= 201,
    HSC_ACCEPTED						= 202,
    HSC_NON_AUTHORITATIVE_INFORMATION	= 203,
    HSC_NO_CONTENT						= 204,
    HSC_RESET_CONTENT					= 205,
    HSC_PARTIAL_CONTENT					= 206,
    HSC_MULTI_STATUS					= 207,

    HSC_MULTIPLE_CHOICES				= 300,
    HSC_MOVED_PERMANENTLY				= 301,
    HSC_MOVED_TEMPORARILY				= 302,
    HSC_SEE_OTHER						= 303,
    HSC_NOT_MODIFIED					= 304,
    HSC_USE_PROXY						= 305,
    HSC_SWITCH_PROXY					= 306,
    HSC_TEMPORARY_REDIRECT				= 307,

    HSC_BAD_REQUEST						= 400,
    HSC_UNAUTHORIZED					= 401,
    HSC_PAYMENT_REQUIRED				= 402,
    HSC_FORBIDDEN						= 403,
    HSC_NOT_FOUND						= 404,
    HSC_METHOD_NOT_ALLOWED				= 405,
    HSC_NOT_ACCEPTABLE					= 406,
    HSC_PROXY_AUTHENTICATION_REQUIRED	= 407,
    HSC_REQUEST_TIMEOUT					= 408,
    HSC_CONFLICT						= 409,
    HSC_GONE							= 410,
    HSC_LENGTH_REQUIRED					= 411,
    HSC_PRECONDITION_FAILED				= 412,
    HSC_REQUEST_ENTITY_TOO_LARGE		= 413,
    HSC_REQUEST_URI_TOO_LONG			= 414,
    HSC_UNSUPPORTED_MEDIA_TYPE			= 415,
    HSC_REQUESTED_RANGE_NOT_SATISFIABLE	= 416,
    HSC_EXPECTATION_FAILED				= 417,
    HSC_UNPROCESSABLE_ENTITY			= 422,
    HSC_LOCKED							= 423,
    HSC_FAILED_DEPENDENCY				= 424,
    HSC_UNORDERED_COLLECTION			= 425,
    HSC_UPGRADE_REQUIRED				= 426,
    HSC_RETRY_WITH						= 449,

    HSC_INTERNAL_SERVER_ERROR			= 500,
    HSC_NOT_IMPLEMENTED					= 501,
    HSC_BAD_GATEWAY						= 502,
    HSC_SERVICE_UNAVAILABLE				= 503,
    HSC_GATEWAY_TIMEOUT					= 504,
    HSC_HTTP_VERSION_NOT_SUPPORTED		= 505,
    HSC_VARIANT_ALSO_NEGOTIATES			= 506,
    HSC_INSUFFICIENT_STORAGE			= 507,
    HSC_BANDWIDTH_LIMIT_EXCEEDED		= 509,
    HSC_NOT_EXTENDED					= 510,

    HSC_UNPARSEABLE_RESPONSE_HEADERS	= 600
  );

{/************************************************************************
名称：Name/Value 结构体
描述：字符串名值对结构体
************************************************************************/}
  TNVPair = array of array of PChar;
  
implementation

end.
