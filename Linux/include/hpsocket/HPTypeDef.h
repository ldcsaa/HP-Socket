/*
* Copyright: JessMA Open Source (ldcsaa@gmail.com)
*
* Author	: Bruce Liang
* Website	: https://github.com/ldcsaa
* Project	: https://github.com/ldcsaa/HP-Socket
* Blog		: http://www.cnblogs.com/ldcsaa
* Wiki		: http://www.oschina.net/p/hp-socket
* QQ Group	: 44636872, 75375912
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

#include "GlobalDef.h"

/* HP-Socket 版本号 */
#define HP_VERSION_MAJOR	5	// 主版本号
#define HP_VERSION_MINOR	8	// 子版本号
#define HP_VERSION_REVISE	3	// 修正版本号
#define HP_VERSION_BUILD	2	// 构建编号

//#define _UDP_DISABLED			// 禁用 UDP
//#define _SSL_DISABLED			// 禁用 SSL
//#define _HTTP_DISABLED		// 禁用 HTTP
//#define _ZLIB_DISABLED		// 禁用 ZLIB
//#define _ICONV_DISABLED		// 禁用 ICONV
//#define _BROTLI_DISABLED		// 禁用 BROTLI

/* 是否启用 UDP，如果定义了 _UDP_DISABLED 则禁用（默认：启用） */
#if !defined(_UDP_DISABLED)
	#ifndef _UDP_SUPPORT
		#define _UDP_SUPPORT
	#endif
#endif

/* 是否启用 SSL，如果定义了 _SSL_DISABLED 则禁用（默认：启用） */
#if !defined(_SSL_DISABLED)
	#ifndef _SSL_SUPPORT
		#define _SSL_SUPPORT
	#endif
#endif

/* 是否启用 HTTP，如果定义了 _HTTP_DISABLED 则禁用（默认：启用） */
#if !defined(_HTTP_DISABLED)
	#ifndef _HTTP_SUPPORT
		#define _HTTP_SUPPORT
	#endif
#endif


/* 是否启用 ZLIB，如果定义了 _ZLIB_DISABLED 则禁用（默认：启用） */
#if !defined(_ZLIB_DISABLED)
	#ifndef _ZLIB_SUPPORT
		#define _ZLIB_SUPPORT
	#endif
#endif

/* 是否启用 BROTLI，如果定义了 _BROTLI_DISABLED 则禁用（默认：启用） */
#if !defined(_BROTLI_DISABLED)
	#ifndef _BROTLI_SUPPORT
		#define _BROTLI_SUPPORT
	#endif
#endif

/* 是否启用 ICONV，如果定义了 _ICONV_DISABLED 则禁用（默认：启用） */
#if !defined(_ICONV_DISABLED)
	#ifndef _ICONV_SUPPORT
		#define _ICONV_SUPPORT
	#endif
#endif

#define HPSOCKET_API	EXTERN_C __attribute__ ((__visibility__("default")))

#define __HP_CALL

/************************************************************************
名称：连接 ID 数据类型
描述：应用程序可以把 CONNID 定义为自身需要的类型（如：ULONG / ULONGLONG）
************************************************************************/
typedef ULID	CONNID, HP_CONNID;

/************************************************************************
名称：通信组件服务状态
描述：应用程序可以通过通信组件的 GetState() 方法获取组件当前服务状态
************************************************************************/
typedef enum EnServiceState
{
	SS_STARTING	= 0,	// 正在启动
	SS_STARTED	= 1,	// 已经启动
	SS_STOPPING	= 2,	// 正在停止
	SS_STOPPED	= 3,	// 已经停止
} En_HP_ServiceState;

/************************************************************************
名称：Socket 操作类型
描述：应用程序的 OnClose() 事件中通过该参数标识是哪种操作导致的错误
************************************************************************/
typedef enum EnSocketOperation
{
	SO_UNKNOWN	= 0,	// Unknown
	SO_ACCEPT	= 1,	// Acccept
	SO_CONNECT	= 2,	// Connect
	SO_SEND		= 3,	// Send
	SO_RECEIVE	= 4,	// Receive
	SO_CLOSE	= 5,	// Close
} En_HP_SocketOperation;

/************************************************************************
名称：事件处理结果
描述：事件的返回值，不同的返回值会影响通信组件的后续行为
************************************************************************/
typedef enum EnHandleResult
{
	HR_OK		= 0,	// 成功
	HR_IGNORE	= 1,	// 忽略
	HR_ERROR	= 2,	// 错误
} En_HP_HandleResult;

/************************************************************************
名称：数据抓取结果
描述：数据抓取操作的返回值
************************************************************************/
typedef enum EnFetchResult
{
	FR_OK				= 0,	// 成功
	FR_LENGTH_TOO_LONG	= 1,	// 抓取长度过大
	FR_DATA_NOT_FOUND	= 2,	// 找不到 ConnID 对应的数据
} En_HP_FetchResult;

/************************************************************************
名称：数据发送策略
描述：Server 组件和 Agent 组件的数据发送策略

* 打包发送策略（默认）	：尽量把多个发送操作的数据组合在一起发送，增加传输效率
* 安全发送策略			：尽量把多个发送操作的数据组合在一起发送，并控制传输速度，避免缓冲区溢出
* 直接发送策略			：对每一个发送操作都直接投递，适用于负载不高但要求实时性较高的场合
************************************************************************/
typedef enum EnSendPolicy
{
	SP_PACK				= 0,	// 打包模式（默认）
	SP_SAFE				= 1,	// 安全模式
	SP_DIRECT			= 2,	// 直接模式
} En_HP_SendPolicy;

/************************************************************************
名称：OnSend 事件同步策略
描述：Server 组件和 Agent 组件的 OnSend 事件同步策略

* 不同步（默认）	：不同步 OnSend 事件，可能同时触发 OnReceive 和 OnClose 事件
* 同步 OnClose	：只同步 OnClose 事件，可能同时触发 OnReceive 事件
* 同步 OnReceive	：（只用于 TCP 组件）同步 OnReceive 和 OnClose 事件，不可能同时触发 OnReceive 或 OnClose 事件
************************************************************************/
typedef enum EnOnSendSyncPolicy
{
	OSSP_NONE			= 0,	// 不同步（默认）
	OSSP_CLOSE			= 1,	// 同步 OnClose
	OSSP_RECEIVE		= 2,	// 同步 OnReceive（只用于 TCP 组件）	
} En_HP_OnSendSyncPolicy;

/************************************************************************
名称：地址重用选项
描述：通信组件底层 socket 的地址重用选项
************************************************************************/
typedef enum EnReuseAddressPolicy
{
	RAP_NONE			= 0,	// 不重用
	RAP_ADDR_ONLY		= 1,	// 仅重用地址
	RAP_ADDR_AND_PORT	= 2,	// 重用地址和端口
} En_HP_ReuseAddressPolicy;

/************************************************************************
名称：操作结果代码
描述：组件 Start() / Stop() 方法执行失败时，可通过 GetLastError() 获取错误代码
************************************************************************/
typedef enum EnSocketError
{
	SE_OK						= NO_ERROR,	// 成功
	SE_ILLEGAL_STATE			= 1,		// 当前状态不允许操作
	SE_INVALID_PARAM			= 2,		// 非法参数
	SE_SOCKET_CREATE			= 3,		// 创建 SOCKET 失败
	SE_SOCKET_BIND				= 4,		// 绑定 SOCKET 失败
	SE_SOCKET_PREPARE			= 5,		// 设置 SOCKET 失败
	SE_SOCKET_LISTEN			= 6,		// 监听 SOCKET 失败
	SE_CP_CREATE				= 7,		// 创建完成端口失败
	SE_WORKER_THREAD_CREATE		= 8,		// 创建工作线程失败
	SE_DETECT_THREAD_CREATE		= 9,		// 创建监测线程失败
	SE_SOCKE_ATTACH_TO_CP		= 10,		// 绑定完成端口失败
	SE_CONNECT_SERVER			= 11,		// 连接服务器失败
	SE_NETWORK					= 12,		// 网络错误
	SE_DATA_PROC				= 13,		// 数据处理错误
	SE_DATA_SEND				= 14,		// 数据发送失败

	/***** SSL Socket 扩展操作结果代码 *****/
	SE_SSL_ENV_NOT_READY		= 101,		// SSL 环境未就绪
} En_HP_SocketError;

/************************************************************************
名称：播送模式
描述：UDP 组件的播送模式（组播或广播）
************************************************************************/
typedef enum EnCastMode
{
	CM_UNICAST		= -1,	// 单播
	CM_MULTICAST	= 0,	// 组播
	CM_BROADCAST	= 1,	// 广播
} En_HP_CastMode;

/************************************************************************
名称：IP 地址类型
描述：IP 地址类型枚举值
************************************************************************/
typedef enum EnIPAddrType
{
	IPT_ALL		= 0,		// 所有
	IPT_IPV4	= 1,		// IPv4
	IPT_IPV6	= 2,		// IPv6
} En_HP_IPAddrType;

/************************************************************************
名称：IP 地址条目结构体
描述：IP 地址的地址簇/地址值结构体
************************************************************************/
typedef struct TIPAddr
{
	EnIPAddrType type;
	LPCTSTR		 address;
} *LPTIPAddr, HP_TIPAddr, *HP_LPTIPAddr;

/************************************************************************
名称：缓冲区结构体
描述：数据缓冲区
************************************************************************/
typedef struct _WSABUF
{
	UINT	len;
	LPBYTE	buf;
} WSABUF, *PWSABUF, *LPWSABUF;

/************************************************************************
名称：拒绝策略
描述：调用被拒绝后的处理策略
************************************************************************/
typedef enum EnRejectedPolicy
{
	TRP_CALL_FAIL	= 0,	// 立刻返回失败
	TRP_WAIT_FOR	= 1,	// 等待（直到成功、超时或线程池关闭等原因导致失败）
	TRP_CALLER_RUN	= 2,	// 调用者线程直接执行
} En_HP_RejectedPolicy;

/************************************************************************
名称：任务缓冲区类型
描述：TSockeTask 对象创建和销毁时，根据不同类型的缓冲区类型作不同的处理
************************************************************************/
typedef enum EnTaskBufferType
{
	TBT_COPY	= 0,	// 深拷贝
	TBT_REFER	= 1,	// 浅拷贝
	TBT_ATTACH	= 2,	// 附属（不负责创建，但负责销毁）
} En_HP_TaskBufferType;

/************************************************************************
名称：任务处理函数
描述：任务处理入口函数
参数：pvArg -- 自定义参数
返回值：（无）
************************************************************************/
typedef VOID (__HP_CALL *Fn_TaskProc)(PVOID pvArg);
typedef Fn_TaskProc	HP_Fn_TaskProc;

struct TSocketTask;

/************************************************************************
名称：Socket 任务处理函数
描述：Socket 任务处理入口函数
参数：pTask -- Socket 任务结构体指针
返回值：（无）
************************************************************************/
typedef VOID (__HP_CALL *Fn_SocketTaskProc)(TSocketTask* pTask);
typedef Fn_SocketTaskProc	HP_Fn_SocketTaskProc;

/************************************************************************
名称：Socket 任务结构体
描述：封装 Socket 任务相关数据结构
************************************************************************/
typedef struct TSocketTask
{
	Fn_SocketTaskProc	fn;			// 任务处理函数
	PVOID				sender;		// 发起对象
	CONNID				connID;		// 连接 ID
	LPCBYTE				buf;		// 数据缓冲区
	INT					bufLen;		// 数据缓冲区长度
	EnTaskBufferType	bufType;	// 缓冲区类型
	WPARAM				wparam;		// 自定义参数
	LPARAM				lparam;		// 自定义参数
} *LPTSocketTask, HP_TSocketTask, *HP_LPTSocketTask;

/************************************************************************
名称：获取 HPSocket 版本号
描述：版本号（4 个字节分别为：主版本号，子版本号，修正版本号，构建编号）
************************************************************************/
inline DWORD GetHPSocketVersion()
{
	return (HP_VERSION_MAJOR << 24) | (HP_VERSION_MINOR << 16) | (HP_VERSION_REVISE << 8) | HP_VERSION_BUILD;
}

/*****************************************************************************************************************************************************/
/**************************************************************** SSL Type Definitions ***************************************************************/
/*****************************************************************************************************************************************************/

#ifdef _SSL_SUPPORT

/************************************************************************
名称：SSL 工作模式
描述：标识 SSL 的工作模式，客户端模式或服务端模式
************************************************************************/
typedef enum EnSSLSessionMode
{
	SSL_SM_CLIENT	= 0,	// 客户端模式
	SSL_SM_SERVER	= 1,	// 服务端模式
} En_HP_SSLSessionMode;

/************************************************************************
名称：SSL 验证模式
描述：SSL 验证模式选项，SSL_VM_PEER 可以和后面两个选项组合一起
************************************************************************/
typedef enum EnSSLVerifyMode
{
	SSL_VM_NONE					= 0x00,	// SSL_VERIFY_NONE
	SSL_VM_PEER					= 0x01,	// SSL_VERIFY_PEER
	SSL_VM_FAIL_IF_NO_PEER_CERT	= 0x02,	// SSL_VERIFY_FAIL_IF_NO_PEER_CERT
	SSL_VM_CLIENT_ONCE			= 0x04,	// SSL_VERIFY_CLIENT_ONCE
} En_HP_SSLVerifyMode;

/************************************************************************
名称：SSL Session 信息类型
描述：用于 GetSSLSessionInfo()，标识输出的 Session 信息类型
************************************************************************/
typedef enum EnSSLSessionInfo
{
	SSL_SSI_MIN					= 0,	// 
	SSL_SSI_CTX					= 0,	// SSL CTX				（输出类型：SSL_CTX*）
	SSL_SSI_CTX_METHOD			= 1,	// SSL CTX Mehtod		（输出类型：SSL_METHOD*）
	SSL_SSI_CTX_CIPHERS			= 2,	// SSL CTX Ciphers		（输出类型：STACK_OF(SSL_CIPHER)*）
	SSL_SSI_CTX_CERT_STORE		= 3,	// SSL CTX Cert Store	（输出类型：X509_STORE*）
	SSL_SSI_SERVER_NAME_TYPE	= 4,	// Server Name Type		（输出类型：int）
	SSL_SSI_SERVER_NAME			= 5,	// Server Name			（输出类型：LPCSTR）
	SSL_SSI_VERSION				= 6,	// SSL Version			（输出类型：LPCSTR）
	SSL_SSI_METHOD				= 7,	// SSL Method			（输出类型：SSL_METHOD*）
	SSL_SSI_CERT				= 8,	// SSL Cert				（输出类型：X509*）
	SSL_SSI_PKEY				= 9,	// SSL Private Key		（输出类型：EVP_PKEY*）
	SSL_SSI_CURRENT_CIPHER		= 10,	// SSL Current Cipher	（输出类型：SSL_CIPHER*）
	SSL_SSI_CIPHERS				= 11,	// SSL Available Ciphers（输出类型：STACK_OF(SSL_CIPHER)*）
	SSL_SSI_CLIENT_CIPHERS		= 12,	// SSL Client Ciphers	（输出类型：STACK_OF(SSL_CIPHER)*）
	SSL_SSI_PEER_CERT			= 13,	// SSL Peer Cert		（输出类型：X509*）
	SSL_SSI_PEER_CERT_CHAIN		= 14,	// SSL Peer Cert Chain	（输出类型：STACK_OF(X509)*）
	SSL_SSI_VERIFIED_CHAIN		= 15,	// SSL Verified Chain	（输出类型：STACK_OF(X509)*）
	SSL_SSI_MAX					= 15,	// 
} En_HP_SSLSessionInfo;

/************************************************************************
名称：SNI 服务名称回调函数
描述：根据服务器名称选择 SSL 证书
参数：	
		lpszServerName -- 服务器名称（域名）

返回值：
		0	 -- 成功，使用默认 SSL 证书索引
		正数	 -- 成功，使用返回值对应的 SNI 主机证书索引
		负数	 -- 失败，中断 SSL 握手

************************************************************************/
typedef int (__HP_CALL *Fn_SNI_ServerNameCallback)(LPCTSTR lpszServerName, PVOID pContext);
typedef Fn_SNI_ServerNameCallback	HP_Fn_SNI_ServerNameCallback;

#endif

/*****************************************************************************************************************************************************/
/**************************************************************** HTTP Type Definitions **************************************************************/
/*****************************************************************************************************************************************************/

#ifdef _HTTP_SUPPORT

/************************************************************************
名称：HTTP 版本
描述：低字节：主版本号，高字节：次版本号
************************************************************************/

typedef enum EnHttpVersion
{
	HV_1_0	= MAKEWORD(1, 0),	// HTTP/1.0
	HV_1_1	= MAKEWORD(1, 1)	// HTTP/1.1
} En_HP_HttpVersion;

/************************************************************************
名称：URL 域
描述：HTTP 请求行中 URL 段位的域定义
************************************************************************/
typedef enum EnHttpUrlField
{ 
	HUF_SCHEMA		= 0,	// Schema
	HUF_HOST		= 1,	// Host
	HUF_PORT		= 2,	// Port
	HUF_PATH		= 3,	// Path
	HUF_QUERY		= 4,	// Query String
	HUF_FRAGMENT	= 5,	// Fragment
	HUF_USERINFO	= 6,	// User Info
	HUF_MAX			= 7,	// (Field Count)
} En_HP_HttpUrlField;

/************************************************************************
名称：HTTP 解析结果标识
描述：指示 HTTP 解析器是否继续执行解析操作
************************************************************************/
typedef enum EnHttpParseResult
{
	HPR_OK			= 0,	// 解析成功
	HPR_SKIP_BODY	= 1,	// 跳过当前请求 BODY（仅用于 OnHeadersComplete 事件）
	HPR_UPGRADE		= 2,	// 升级协议（仅用于 OnHeadersComplete 事件）
	HPR_ERROR		= -1,	// 解析错误，终止解析，断开连接
} En_HP_HttpParseResult;

/************************************************************************
名称：HTTP 协议升级类型
描述：标识 HTTP 升级为哪种协议
************************************************************************/
typedef enum EnHttpUpgradeType
{
	HUT_NONE		= 0,	// 没有升级
	HUT_WEB_SOCKET	= 1,	// WebSocket
	HUT_HTTP_TUNNEL	= 2,	// HTTP 隧道
	HUT_UNKNOWN		= -1,	// 未知类型
} En_HP_HttpUpgradeType;

/************************************************************************
名称：HTTP 状态码
描述：HTTP 标准状态码
************************************************************************/
typedef enum EnHttpStatusCode
{ 
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
	HSC_ALREADY_REPORTED				= 208,
	HSC_IM_USED							= 226,

	HSC_MULTIPLE_CHOICES				= 300,
	HSC_MOVED_PERMANENTLY				= 301,
	HSC_MOVED_TEMPORARILY				= 302,
	HSC_SEE_OTHER						= 303,
	HSC_NOT_MODIFIED					= 304,
	HSC_USE_PROXY						= 305,
	HSC_SWITCH_PROXY					= 306,
	HSC_TEMPORARY_REDIRECT				= 307,
	HSC_PERMANENT_REDIRECT				= 308,

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
	HSC_MISDIRECTED_REQUEST				= 421,
	HSC_UNPROCESSABLE_ENTITY			= 422,
	HSC_LOCKED							= 423,
	HSC_FAILED_DEPENDENCY				= 424,
	HSC_UNORDERED_COLLECTION			= 425,
	HSC_UPGRADE_REQUIRED				= 426,
	HSC_PRECONDITION_REQUIRED			= 428,
	HSC_TOO_MANY_REQUESTS				= 429,
	HSC_REQUEST_HEADER_FIELDS_TOO_LARGE	= 431,
	HSC_UNAVAILABLE_FOR_LEGAL_REASONS	= 451,
	HSC_RETRY_WITH						= 449,

	HSC_INTERNAL_SERVER_ERROR			= 500,
	HSC_NOT_IMPLEMENTED					= 501,
	HSC_BAD_GATEWAY						= 502,
	HSC_SERVICE_UNAVAILABLE				= 503,
	HSC_GATEWAY_TIMEOUT					= 504,
	HSC_HTTP_VERSION_NOT_SUPPORTED		= 505,
	HSC_VARIANT_ALSO_NEGOTIATES			= 506,
	HSC_INSUFFICIENT_STORAGE			= 507,
	HSC_LOOP_DETECTED					= 508,
	HSC_BANDWIDTH_LIMIT_EXCEEDED		= 509,
	HSC_NOT_EXTENDED					= 510,
	HSC_NETWORK_AUTHENTICATION_REQUIRED	= 511,

	HSC_UNPARSEABLE_RESPONSE_HEADERS	= 600
} En_HP_HttpStatusCode;

/************************************************************************
名称：Name/Value 结构体
描述：字符串名值对结构体
************************************************************************/
typedef struct TNVPair
{ 
	LPCSTR name;
	LPCSTR value;
}	HP_TNVPair,
TParam, HP_TParam, *LPPARAM, *HP_LPPARAM,
THeader, HP_THeader, *LPHEADER, *HP_LPHEADER,
TCookie, HP_TCookie, *LPCOOKIE, *HP_LPCOOKIE;

#endif
