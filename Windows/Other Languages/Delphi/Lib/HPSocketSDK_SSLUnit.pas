unit HPSocketSDK_SSLUnit;

interface

uses
  Winapi.Windows, HPTypeDef, HPSocketSDKUnit;

type

{
/************************************************************************
名称：定义 SSL Socket 对象指针类型别名
描述：把 SSL Socket 对象指针定义为更直观的别名
************************************************************************/
}

  HP_SSLServer = HP_Object;
  HP_SSLAgent = HP_Object;
  HP_SSLClient = HP_Object;

  HP_SSLPullServer = HP_Object;
  HP_SSLPullAgent = HP_Object;
  HP_SSLPullClient = HP_Object;

  HP_SSLPackServer = HP_Object;
  HP_SSLPackAgent = HP_Object;
  HP_SSLPackClient = HP_Object;

  HP_HttpsServer = HP_Object;
  HP_HttpsAgent = HP_Object;
  HP_HttpsClient = HP_Object;
  HP_HttpsSyncClient = HP_Object;

//*****************************************************************************************************************************************************/
//******************************************************************** SSL Exports ********************************************************************/
//*****************************************************************************************************************************************************/


//********************************************************/
//************** HPSocket4C-SSL 对象创建函数 **************/

// 创建 HP_SSLServer 对象
function Create_HP_SSLServer(pListener: HP_TcpServerListener): HP_SSLServer; stdcall; external HPSocketDLL;
// 创建 HP_SSLAgent 对象
function Create_HP_SSLAgent(pListener: HP_TcpAgentListener): HP_SSLAgent; stdcall; external HPSocketDLL;
// 创建 HP_SSLClient 对象
function Create_HP_SSLClient(pListener: HP_TcpClientListener): HP_SSLClient; stdcall; external HPSocketDLL;
// 创建 HP_SSLPullServer 对象
function Create_HP_SSLPullServer(pListener: HP_TcpPullServerListener): HP_SSLPullServer; stdcall; external HPSocketDLL;
// 创建 HP_SSLPullAgent 对象
function Create_HP_SSLPullAgent(pListener: HP_TcpPullAgentListener): HP_SSLPullAgent; stdcall; external HPSocketDLL;
// 创建 HP_SSLPullClient 对象
function Create_HP_SSLPullClient(pListener: HP_TcpPullClientListener): HP_SSLPullClient; stdcall; external HPSocketDLL;
// 创建 HP_SSLPackServer 对象
function Create_HP_SSLPackServer(pListener: HP_TcpServerListener): HP_SSLPackServer; stdcall; external HPSocketDLL;
// 创建 HP_SSLPackAgent 对象
function Create_HP_SSLPackAgent(pListener: HP_TcpAgentListener): HP_SSLPackAgent;  stdcall; external HPSocketDLL;
// 创建 HP_SSLPackClient 对象
function Create_HP_SSLPackClient(pListener: HP_TcpClientListener): HP_SSLPackClient;  stdcall; external HPSocketDLL;

// 销毁 HP_SSLServer 对象
procedure Destroy_HP_SSLServer(pServer: HP_SSLServer); stdcall; external HPSocketDLL;
// 销毁 HP_SSLAgent 对象
procedure Destroy_HP_SSLAgent(pAgent: HP_SSLAgent); stdcall; external HPSocketDLL;
// 销毁 HP_SSLClient 对象
procedure Destroy_HP_SSLClient(pClient: HP_SSLClient); stdcall; external HPSocketDLL;
// 销毁 HP_SSLPullServer 对象
procedure Destroy_HP_SSLPullServer(pServer: HP_SSLPullServer); stdcall; external HPSocketDLL;
// 销毁 HP_SSLPullAgent 对象
procedure Destroy_HP_SSLPullAgent(pAgent: HP_SSLPullAgent); stdcall; external HPSocketDLL;
// 销毁 HP_SSLPullClient 对象
procedure Destroy_HP_SSLPullClient(pClient: HP_SSLPullClient); stdcall; external HPSocketDLL;
// 销毁 HP_SSLPackServer 对象
procedure Destroy_HP_SSLPackServer(pServer: HP_SSLPackServer); stdcall; external HPSocketDLL;
// 销毁 HP_SSLPackAgent 对象
procedure Destroy_HP_SSLPackAgent(pAgent: HP_SSLPackAgent); stdcall; external HPSocketDLL;
// 销毁 HP_SSLPackClient 对象
procedure Destroy_HP_SSLPackClient(pClient: HP_SSLPackClient); stdcall; external HPSocketDLL;

//*****************************************************************************************************************************************************/
//*************************************************************** Global Function Exports *************************************************************/
//*****************************************************************************************************************************************************/

//***************************************************************************************/
//************************************ SSL 初始化方法 ************************************/

{/*
* 名称：SNI 默认回调函数
* 描述：HP_SSLServer_SetupSSLContext 方法中如果不指定 SNI 回调函数则使用此 SNI 默认回调函数
*
* 参数：		lpszServerName	-- 请求域名
*		       	pContext		    -- SSL Context 对象
*
* 返回值：SNI 主机证书对应的索引
*/}
function HP_SSL_DefaultServerNameCallback(lpszServerName: PChar; pContext: PVOID): Integer; stdcall; external HPSocketDLL;
{
* 名称：清理线程局部环境 SSL 资源
* 描述：任何一个操作 SSL 的线程，通信结束时都需要清理线程局部环境 SSL 资源
*		1、主线程和 HP-Socket 工作线程在通信结束时会自动清理线程局部环境 SSL 资源。因此，一般情况下不必手工调用本方法
*		2、特殊情况下，当自定义线程参与 HP-Socket 通信操作并检查到 SSL 内存泄漏时，需在每次通信结束时自定义线程调用本方法
*
* 参数：		dwThreadID	-- 线程 ID（0：当前线程）
*
* 返回值：无
}
procedure HP_SSL_RemoveThreadLocalState(dwThreadID: LongWord); stdcall; external HPSocketDLL;

{/*
* 名称：初始化通信组件 SSL 环境参数
* 描述：SSL 环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
*
* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
*			lpszPemCertFile			-- 证书文件
*			lpszPemKeyFile			-- 私钥文件
*			lpszKeyPasswod			-- 私钥密码（没有密码则为空）
*			lpszCAPemCertFileOrPath	-- CA 证书文件或目录（单向验证或客户端可选）
*			fnServerNameCallback	-- SNI 回调函数指针（可选）
*
* 返回值：	TRUE	-- 成功
*			      FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/ }
function HP_SSLServer_SetupSSLContext(pServer: HP_SSLServer; iVerifyMode: integer = Integer(SSL_VM_NONE) ; //* SSL_VM_NONE */
    lpszPemCertFile: PChar=nil;            //* nullptr */
    lpszPemKeyFile: PChar=nil;             //* nullptr */
    lpszKeyPasswod: PChar=nil;            //* nullptr */
    lpszCAPemCertFileOrPath: PChar= nil;    //* nullptr */
    fnServerNameCallback: HP_Fn_SNI_ServerNameCallback = nil  //* nullptr */
  ): Bool; stdcall; external HPSocketDLL;

{/*
* 名称：初始化通信组件 SSL 环境参数（通过内存加载证书）
* 描述：SSL 环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
*
* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
*			      lpszPemCert				-- 证书内容
*			      lpszPemKey				-- 私钥内容
*		        lpszKeyPasswod			-- 私钥密码（没有密码则为空）
*			      lpszCAPemCert			-- CA 证书内容（单向验证或客户端可选）
*			      fnServerNameCallback	-- SNI 回调函数指针（可选，如果为 nullptr 则使用 SNI 默认回调函数）
*
* 返回值：	TRUE	-- 成功
*			      FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/}
function HP_SSLServer_SetupSSLContextByMemory(pServer: HP_SSLServer; iVerifyMode: integer = Integer(SSL_VM_NONE) ; //* SSL_VM_NONE */
    lpszPemCert: PChar=nil;             //* nullptr */
    lpszPemKey: PChar=nil;              //* nullptr */
    lpszKeyPasswod: PChar=nil;          //* nullptr */
    lpszCAPemCert: PChar=nil;           //* nullptr */
    fnServerNameCallback: HP_Fn_SNI_ServerNameCallback = nil  //* nullptr */
  ): Bool; stdcall; external HPSocketDLL;

{/*
* 名称：绑定 SNI 主机域名
* 描述：SSL 服务端在 AddSSLContext() 成功后可以调用本方法绑定主机域名到 SNI 主机证书
*
* 参数：		lpszServerName		-- 主机域名
*			      iContextIndex		  -- SNI 主机证书对应的索引
*
* 返回值：	TRUE	-- 成功
*			      FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/}
function HP_SSLServer_BindSSLServerName(pServer: HP_SSLServer; lpszServerName: PChar; iContextIndex: LongWord) : Bool; stdcall; external HPSocketDLL;

{/*
* 名称：增加 SNI 主机证书
* 描述：SSL 服务端在 SetupSSLContext() 成功后可以调用本方法增加多个 SNI 主机证书
*
* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
*			lpszPemCertFile			-- 证书文件
*			lpszPemKeyFile			-- 私钥文件
*			lpszKeyPasswod			-- 私钥密码（没有密码则为空）
*			lpszCAPemCertFileOrPath	-- CA 证书文件或目录（单向验证可选）
*
* 返回值：	正数		-- 成功，并返回 SNI 主机证书对应的索引，该索引用于在 SNI 回调函数中定位 SNI 主机
*			      负数		-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/}
function HP_SSLServer_AddSSLContext(pServer: HP_SSLServer; iVerifyMode: LongWord; lpszPemCertFile: PChar;
        lpszPemKeyFile: PChar;
        lpszKeyPasswod: PChar; //* nullptr */,
        lpszCAPemCertFileOrPath: PChar //* nullptr */
  ): Integer; stdcall; external HPSocketDLL;

{/*
* 名称：增加 SNI 主机证书（通过内存加载证书）
* 描述：SSL 服务端在 SetupSSLContext() 成功后可以调用本方法增加多个 SNI 主机证书
*
* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
*			lpszPemCert				-- 证书内容
*			lpszPemKey				-- 私钥内容
*			lpszKeyPasswod			-- 私钥密码（没有密码则为空）
*			lpszCAPemCert			-- CA 证书内容（单向验证可选）
*
* 返回值：	正数		-- 成功，并返回 SNI 主机证书对应的索引，该索引用于在 SNI 回调函数中定位 SNI 主机
*			负数		-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/}
function  HP_SSLServer_AddSSLContextByMemory(pServer: HP_SSLServer; iVerifyMode: LongWord;
        lpszPemCert: PChar;
        lpszPemKey: PChar;
        lpszKeyPasswod: PChar;     //* nullptr */,
        lpszCAPemCert: PChar       //* nullptr */
  ): Integer; stdcall; external HPSocketDLL;
{/*
* 名称：清理通信组件 SSL 运行环境
* 描述：清理通信组件 SSL 运行环境，回收 SSL 相关内存
*		1、通信组件析构时会自动调用本方法
*		2、当要重新设置通信组件 SSL 环境参数时，需要先调用本方法清理原先的环境参数
*
* 参数：	无
*
* 返回值：无
*/}
procedure HP_SSLServer_CleanupSSLContext(pServer: HP_SSLServer); stdcall; external HPSocketDLL;

{/*
* 名称：初始化通信组件 SSL 环境参数
* 描述：SSL 环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
*
* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
*			lpszPemCertFile			-- 证书文件（客户端可选）
*			lpszPemKeyFile			-- 私钥文件（客户端可选）
*			lpszKeyPasswod			-- 私钥密码（没有密码则为空）
*			lpszCAPemCertFileOrPath	-- CA 证书文件或目录（单向验证或客户端可选）
*
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/}
function HP_SSLAgent_SetupSSLContext(pAgent: HP_SSLAgent; iVerifyMode: LongWord; //* SSL_VM_NONE */,
      lpszPemCertFile: PChar;          //* nullptr */,
      lpszPemKeyFile: PChar;           //* nullptr */,
      lpszKeyPasswod: PChar;           //* nullptr */,
      lpszCAPemCertFileOrPath: PChar   //* nullptr */
   ): Bool; stdcall; external HPSocketDLL;

{/*
* 名称：初始化通信组件 SSL 环境参数（通过内存加载证书）
* 描述：SSL 环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
*
* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
*			      lpszPemCert				-- 证书内容
*			      lpszPemKey				-- 私钥内容
*			      lpszKeyPasswod			-- 私钥密码（没有密码则为空）
*			      lpszCAPemCert			-- CA 证书内容（单向验证或客户端可选）
*
* 返回值：	TRUE	-- 成功
*			      FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/}
function HP_SSLAgent_SetupSSLContextByMemory(pAgent: HP_SSLAgent; iVerifyMode: LongWord; //* SSL_VM_NONE */,
      lpszPemCert: PChar;               //* nullptr */,
      lpszPemKey: PChar;                //* nullptr */,
      lpszKeyPasswod: PChar;            //* nullptr */,
      lpszCAPemCert: PChar             //* nullptr */
   ):Bool; stdcall; external HPSocketDLL;
{/*
* 名称：清理通信组件 SSL 运行环境
* 描述：清理通信组件 SSL 运行环境，回收 SSL 相关内存
*		1、通信组件析构时会自动调用本方法
*		2、当要重新设置通信组件 SSL 环境参数时，需要先调用本方法清理原先的环境参数
*
* 参数：	无
*
* 返回值：无
*/ }
procedure HP_SSLAgent_CleanupSSLContext(pAgent: HP_SSLAgent); stdcall; external HPSocketDLL;

{/*
* 名称：初始化通信组件 SSL 环境参数
* 描述：SSL 环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
*
* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
*			lpszPemCertFile			-- 证书文件（客户端可选）
*			lpszPemKeyFile			-- 私钥文件（客户端可选）
*			lpszKeyPasswod			-- 私钥密码（没有密码则为空）
*			lpszCAPemCertFileOrPath	-- CA 证书文件或目录（单向验证或客户端可选）
*
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/}
function HP_SSLClient_SetupSSLContext(pClient: HP_SSLClient; iVerifyMode: LongWord; //* SSL_VM_NONE */,
     lpszPemCertFile: PChar;         //* nullptr */,
     lpszPemKeyFile: PChar;          //* nullptr */,
     lpszKeyPasswod: PChar;          //* nullptr */,
     lpszCAPemCertFileOrPath: PChar  //* nullptr */
  ): Bool; stdcall; external HPSocketDLL;

{/*
* 名称：初始化通信组件 SSL 环境参数（通过内存加载证书）
* 描述：SSL 环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
*
* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
*			      lpszPemCert				-- 证书内容
*			      lpszPemKey				-- 私钥内容
*			      lpszKeyPasswod			-- 私钥密码（没有密码则为空）
*			      lpszCAPemCert			-- CA 证书内容（单向验证或客户端可选）
*
* 返回值：	TRUE	-- 成功
*			      FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/}
function HP_SSLClient_SetupSSLContextByMemory(pClient: HP_SSLClient; iVerifyMode: LongWord; //* SSL_VM_NONE */,
     lpszPemCert: PChar;              //* nullptr */,
     lpszPemKey: PChar;               //* nullptr */,
     lpszKeyPasswod: PChar;           //* nullptr */,
     lpszCAPemCert: PChar             //* nullptr */
   ): Bool; stdcall; external HPSocketDLL;

{/*
* 名称：清理通信组件 SSL 运行环境
* 描述：清理通信组件 SSL 运行环境，回收 SSL 相关内存
*		1、通信组件析构时会自动调用本方法
*		2、当要重新设置通信组件 SSL 环境参数时，需要先调用本方法清理原先的环境参数
*
* 参数：	无
*
* 返回值：无
*/}
procedure HP_SSLClient_CleanupSSLContext(pClient: HP_SSLClient); stdcall; external HPSocketDLL;

{/***************************************************************************************/
/************************************* SSL 操作方法 ************************************/}

{/*
* 名称：启动 SSL 握手
* 描述：当通信组件设置为非自动握手时，需要调用本方法启动 SSL 握手
*
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/}
function HP_SSLServer_StartSSLHandShake(pServer: HP_SSLServer; dwConnID: HP_CONNID): BOOL; stdcall; external HPSocketDLL;

//* 设置通信组件握手方式（默认：TRUE，自动握手） */
procedure HP_SSLServer_SetSSLAutoHandShake(pServer: HP_SSLServer;bAutoHandShake: BOOL); stdcall; external HPSocketDLL;
//* 获取通信组件握手方式 */
function HP_SSLServer_IsSSLAutoHandShake(pServer: HP_SSLServer ): BOOL; stdcall; external HPSocketDLL;
{/*
* 名称：获取 SSL Session 信息
* 描述：获取指定类型的 SSL Session 信息（输出类型参考：En_HP_SSLSessionInfo）
*
* 返回值：	TRUE	-- 成功
*			      FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/}
function HP_SSLServer_GetSSLSessionInfo(pServer: HP_SSLServer;
     dwConnID: HP_CONNID;
     enInfo: En_HP_SSLSessionInfo;
     lppInfo: PLPVOID
   ): Bool; stdcall; external HPSocketDLL;
{/*
* 名称：启动 SSL 握手
* 描述：当通信组件设置为非自动握手时，需要调用本方法启动 SSL 握手
*
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/}
function HP_SSLAgent_StartSSLHandShake(pAgent: HP_SSLAgent; dwConnID: HP_CONNID): BOOL; stdcall; external HPSocketDLL;

//* 设置通信组件握手方式（默认：TRUE，自动握手） */
procedure HP_SSLAgent_SetSSLAutoHandShake(pAgent: HP_SSLAgent; bAutoHandShake: BOOL); stdcall; external HPSocketDLL;
//* 获取通信组件握手方式 */
function HP_SSLAgent_IsSSLAutoHandShake(pAgent: HP_SSLAgent ): BOOL; stdcall; external HPSocketDLL;

{/*
* 名称：获取 SSL Session 信息
* 描述：获取指定类型的 SSL Session 信息（输出类型参考：En_HP_SSLSessionInfo）
*
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/}
function HP_SSLAgent_GetSSLSessionInfo(pAgent: HP_SSLAgent;
     dwConnID: HP_CONNID;
     enInfo: En_HP_SSLSessionInfo;
     lppInfo: PLPVOID
   ): Bool; stdcall; external HPSocketDLL;
{/*
* 名称：启动 SSL 握手
* 描述：当通信组件设置为非自动握手时，需要调用本方法启动 SSL 握手
*
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/}
function HP_SSLClient_StartSSLHandShake(pClient: HP_SSLClient): BOOL; stdcall; external HPSocketDLL;

//* 设置通信组件握手方式（默认：TRUE，自动握手） */
procedure HP_SSLClient_SetSSLAutoHandShake(pClient: HP_SSLClient; bAutoHandShake: BOOL); stdcall; external HPSocketDLL;
//* 获取通信组件握手方式 */
function HP_SSLClient_IsSSLAutoHandShake(pClient: HP_SSLClient): BOOL; stdcall; external HPSocketDLL;

{/*
* 名称：获取 SSL Session 信息
* 描述：获取指定类型的 SSL Session 信息（输出类型参考：En_HP_SSLSessionInfo）
*
* 返回值：	TRUE	-- 成功
*			FALSE	-- 失败，可通过 SYS_GetLastError() 获取失败原因
*/}
function HP_SSLClient_GetSSLSessionInfo(pClient: HP_SSLClient;
     enInfo: En_HP_SSLSessionInfo;
     lppInfo: PLPVOID
   ): Bool; stdcall; external HPSocketDLL;
{/*****************************************************************************************************************************************************/
/******************************************************************** HTTPS Exports ******************************************************************/
/*****************************************************************************************************************************************************/

#ifdef _HTTP_SUPPORT
 }
{/****************************************************/
/**************** HTTPS 对象创建函数 *****************/}

// 创建 HP_HttpsServer 对象
function Create_HP_HttpsServer(pListener: HP_HttpServerListener): HP_HttpsServer; stdcall; external HPSocketDLL;
// 创建 HP_HttpsAgent 对象
function Create_HP_HttpsAgent(pListener: HP_HttpAgentListener): HP_HttpsAgent; stdcall; external HPSocketDLL;
// 创建 HP_HttpsClient 对象
function Create_HP_HttpsClient(pListener: HP_HttpClientListener): HP_HttpsClient; stdcall; external HPSocketDLL;
// 创建 HP_HttpsSyncClient 对象
function Create_HP_HttpsSyncClient(pListener: HP_HttpClientListener): HP_HttpsSyncClient; stdcall; external HPSocketDLL;

// 销毁 HP_HttpsServer 对象
procedure Destroy_HP_HttpsServer(pServer: HP_HttpsServer); stdcall; external HPSocketDLL;
// 销毁 HP_HttpsAgent 对象
procedure Destroy_HP_HttpsAgent(pAgent: HP_HttpsAgent); stdcall; external HPSocketDLL;
// 销毁 HP_HttpsClient 对象
procedure Destroy_HP_HttpsClient(pClient: HP_HttpsClient); stdcall; external HPSocketDLL;
// 销毁 HP_HttpsSyncClient 对象
procedure Destroy_HP_HttpsSyncClient(pClient: HP_HttpsSyncClient); stdcall; external HPSocketDLL;

implementation

initialization
  IsMultiThread := True;

end.


