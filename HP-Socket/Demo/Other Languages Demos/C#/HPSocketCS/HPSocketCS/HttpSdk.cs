using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;

namespace HPSocketCS
{
    public static class ____LinqEx
    {
        public static byte LOBYTE(this HttpVersion version)
        {
            return (byte)((ushort)version & 0xff);
        }

        public static byte HIBYTE(this HttpVersion version)
        {
            return (byte)((ushort)version >> 8);//高8位
        }

        public static string ToString(this HttpMethod method)
        {
            var fi = method.GetType().GetField(method.ToString());
            var attributes = (DescriptionAttribute[])fi.GetCustomAttributes(typeof(DescriptionAttribute), false);
            return (attributes.Length > 0) ? attributes[0].Description : method.ToString();
        }
    }


    public enum WSOpcode : byte
    {
        Cont = 0x0,
        Text = 0x1,
        Binary = 0x2,
        Close = 0x8,
        Ping = 0x9,
        Pong = 0xa,
    }

    public enum WSReserved : byte
    {
        Off = 0x0,
        On = 0x1
    }

    public class WSMessageState
    {
        public bool Final { get; set; }
        public WSReserved Reserved { get; set; }
        public WSOpcode OperationCode { get; set; }
        public byte[] Mask { get; set; }
        public ulong BodyLen { get; set; }
        public ulong BodyRemain { get; set; }
    }


    /// <summary>
    /// http版本号
    /// </summary>
    public enum HttpVersion
    {
        /// <summary>
        /// http 1.0
        /// </summary>
        V1_0 = 1,
        /// <summary>
        /// http 1.1
        /// </summary>
        v1_1 = 257,
    }
    /// <summary>
    /// URL 域
    /// HTTP 请求行中 URL 段位的域定义
    /// </summary>
    public enum HttpUrlField
    {
        Schema = 0,
        Host = 1,
        Port = 2,
        Path = 3,
        QueryString = 4,
        Fragment = 5,
        UserInfo = 6,
        /// <summary>
        /// Field Count
        /// </summary>
        Max = 7,
    }
    /// <summary>
    /// HTTP 解析结果标识
    /// 指示 HTTP 解析器是否继续执行解析操作
    /// </summary>
    public enum HttpParseResult
    {
        /// <summary>
        /// 终止解析，断开连接
        /// </summary>
        Error = -1,
        /// <summary>
        /// 继续解析
        /// </summary>
        Ok = 0,
    }

    /// <summary>
    /// HTTP 解析结果标识
    /// 指示 HTTP 解析器是否继续执行解析操作
    /// </summary>
    public enum HttpParseResultEx
    {
        /// <summary>
        /// 终止解析，断开连接
        /// </summary>
        Error = -1,
        /// <summary>
        /// 继续解析
        /// </summary>
        Ok = 0,
        /// <summary>
        /// 跳过当前请求 BODY
        /// </summary>
        SkipBody = 1,
        /// <summary>
        /// 升级协议
        /// </summary>
        Upgrade = 2,
    }
    public enum HttpUpgradeType
    {
        /// <summary>
        /// 没有升级
        /// </summary>
        None = 0,
        /// <summary>
        /// WebSocket
        /// </summary>
        WebSocket = 1,
        /// <summary>
        /// HTTP 隧道
        /// </summary>
        HttpTunnel = 2,
        /// <summary>
        /// 未知类型
        /// </summary>
        Unknown = -1,
    }

    public enum HttpStatusCode
    {
        Continue = 100,
        SwitchingProtocols = 101,
        Processing = 102,

        Ok = 200,
        Created = 201,
        Accepted = 202,
        NonAuthoritativeInformation = 203,
        NoContent = 204,
        ResetContent = 205,
        PartialContent = 206,
        MultiStatus = 207,

        MultipleChoices = 300,
        MovedPermanently = 301,
        MovedTemporarily = 302,
        SeeOther = 303,
        NotModified = 304,
        UseProxy = 305,
        SwitchProxy = 306,
        TemporaryRedirect = 307,

        BadRequest = 400,
        Unauthorized = 401,
        PaymentRequired = 402,
        Forbidden = 403,
        NotFound = 404,
        MethodNotAllowed = 405,
        NotAcceptable = 406,
        ProxyAuthenticationRequired = 407,
        RequestTimeout = 408,
        Conflict = 409,
        Gone = 410,
        LengthRequired = 411,
        PreconditionFailed = 412,
        RequestEntityTooLarge = 413,
        RequestUriTooLong = 414,
        UnsupportedMediaType = 415,
        RequestedRangeNotSatisfiable = 416,
        ExpectationFailed = 417,
        UnprocessableEntity = 422,
        Locked = 423,
        FailedDependency = 424,
        UnorderedCollection = 435,
        UpgradeRequired = 426,
        RetryWith = 449,

        InternalServerError = 500,
        NotImplemented = 501,
        BadGateway = 502,
        ServiceUnavailable = 503,
        GatewayTimeout = 504,
        HttpVersionNotSupported = 505,
        VariantAlsoNegotiates = 506,
        InsufficientStorage = 507,
        BandwidthLimitExceeded = 509,
        NotExtended = 510,

        UnparseableResponseHeaders = 600,

    }
    /// <summary>
    /// HttpMethod
    /// </summary>
    public enum HttpMethod
    {
        [Description("GET")]
        Get,
        [Description("POST")]
        Post,
        [Description("PUT")]
        Put,
        [Description("PATCH")]
        Patch,
        [Description("DELETE")]
        Delete,
        [Description("HEAD")]
        Head,
        [Description("TRACE")]
        Trace,
        [Description("OPTIONS")]
        Options,
        [Description("CONNECT")]
        Connect,
    }

    /// <summary>
    /// Name/Value 结构体
    /// 字符串名值对结构体
    /// </summary>
    public struct TNVPair
    {
        [MarshalAs(UnmanagedType.LPStr)]
        public string Name;

        [MarshalAs(UnmanagedType.LPStr)]
        public string Value;
    }
    public struct TCookie
    {
        [MarshalAs(UnmanagedType.LPStr)]
        public string Name;

        [MarshalAs(UnmanagedType.LPStr)]
        public string Value;
    }

    public struct THeader
    {
        [MarshalAs(UnmanagedType.LPStr)]
        public string Name;

        [MarshalAs(UnmanagedType.LPStr)]
        public string Value;
    }

    public struct THeaderForIntPtr
    {
        public IntPtr Name;
        public IntPtr Value;
    }

    /// <summary>
    /// http事件
    /// </summary>
    public class HttpEvent
    {
        public delegate HttpParseResult OnMessageBeginEventHandler(IntPtr connId);
        public delegate HttpParseResult OnHeaderEventHandler(IntPtr connId, string name, string value);
        public delegate HttpParseResultEx OnHeadersCompleteEventHandler(IntPtr connId);
        public delegate HttpParseResult OnPointerDataBodyEventHandler(IntPtr connId, IntPtr pData, int length);
        public delegate HttpParseResult OnBodyEventHandler(IntPtr connId, byte[] bytes);

        public delegate HttpParseResult OnChunkHeaderEventHandler(IntPtr connId, int length);
        public delegate HttpParseResult OnChunkCompleteEventHandler(IntPtr connId);
        public delegate HttpParseResult OnMessageCompleteEventHandler(IntPtr connId);
        public delegate HttpParseResult OnUpgradeEventHandler(IntPtr connId, HttpUpgradeType upgradeType);
        public delegate HttpParseResult OnParseErrorEventHandler(IntPtr connId, int errorCode, string errorDesc);
    }
    /// <summary>
    /// http server 事件
    /// </summary>
    public class HttpServerEvent : HttpEvent
    {
        public delegate HttpParseResult OnRequestLineEventHandler(IntPtr connId, string method, string url);
    }
    /// <summary>
    /// http cilent 事件
    /// </summary>
    public class HttpClientEvent : HttpEvent
    {
        public delegate HttpParseResult OnStatusLineEventHandler(IntPtr connId, ushort statusCode, string desc);
    }

    /// <summary>
    /// http agent 事件
    /// </summary>
    public class HttpAgentEvent : HttpClientEvent
    {

    }

    /// <summary>
    /// websocket 事件
    /// </summary>
    public class WebSocketEvent : HttpEvent
    {
        public delegate HandleResult OnWSMessageHeaderEventHandler(IntPtr connId, bool final, byte reserved, byte operationCode, byte[] mask, ulong bodyLength);
        public delegate HandleResult OnPointerWSMessageBodyEventHandler(IntPtr connId, IntPtr pData, int length);
        public delegate HandleResult OnWSMessageBodyEventHandler(IntPtr connId, byte[] data);
        public delegate HandleResult OnWSMessageCompleteEventHandler(IntPtr connId);
    }


    public class HttpSdk
    {
        /// <summary>
        /// HPSocket的文件路径
        /// </summary>
        public const string HPSOCKET_HTTP_DLL_PATH = Sdk.HPSOCKET_DLL_PATH;

        /****************************************************/
        /******************* HTTP 回调函数 *******************/
        /* Agent & Server & Clent */
        public delegate HttpParseResult OnMessageBegin(IntPtr pSender, IntPtr connId);
        public delegate HttpParseResult OnHeader(IntPtr pSender, IntPtr connId, string lpszName, string lpszValue);
        public delegate HttpParseResultEx OnHeadersComplete(IntPtr pSender, IntPtr connId);
        public delegate HttpParseResult OnBody(IntPtr pSender, IntPtr connId, IntPtr pData, int iLength);
        public delegate HttpParseResult OnChunkHeader(IntPtr pSender, IntPtr connId, int iLength);
        public delegate HttpParseResult OnChunkComplete(IntPtr pSender, IntPtr connId);
        public delegate HttpParseResult OnMessageComplete(IntPtr pSender, IntPtr connId);
        public delegate HttpParseResult OnUpgrade(IntPtr pSender, IntPtr connId, HttpUpgradeType enUpgradeType);
        public delegate HttpParseResult OnParseError(IntPtr pSender, IntPtr connId, int iErrorCode, string lpszErrorDesc);

        /* Server */
        public delegate HttpParseResult OnRequestLine(IntPtr pSender, IntPtr connId, string lpszMethod, string lpszUrl);

        /* Agent & Client */
        public delegate HttpParseResult OnStatusLine(IntPtr pSender, IntPtr connId, ushort usStatusCode, string lpszDesc);

        /* WebSocket */
        public delegate HandleResult OnWSMessageHeader(IntPtr pSender, IntPtr dwConnID, bool bFinal, byte iReserved, byte iOperationCode, byte[] lpszMask, ulong ullBodyLen);
        public delegate HandleResult OnWSMessageBody(IntPtr pSender, IntPtr dwConnID, IntPtr pData, int length);
        public delegate HandleResult OnWSMessageComplete(IntPtr pSender, IntPtr dwConnID);


        /****************************************************/
        /***************** HTTP 对象创建函数 *****************/


        /// <summary>
        /// 创建 HP_HttpServer 对象
        /// </summary>
        /// <param name="pListener"></param>
        /// <returns></returns>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr Create_HP_HttpServer(IntPtr pListener);
        /// <summary>
        ///  创建 HP_HttpAgent 对象
        /// </summary>
        /// <param name="pListener"></param>
        /// <returns></returns>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr Create_HP_HttpAgent(IntPtr pListener);
        /// <summary>
        ///  创建 HP_HttpClient 对象
        /// </summary>
        /// <param name="pListener"></param>
        /// <returns></returns>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr Create_HP_HttpClient(IntPtr pListener);

        /// <summary>
        /// 创建 HP_HttpSyncClient 对象
        /// </summary>
        /// <param name="pListener"></param>
        /// <returns></returns>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr Create_HP_HttpSyncClient(IntPtr pListener);


        /// <summary>
        ///  销毁 HP_HttpServer 对象
        /// </summary>
        /// <param name="pServer"></param>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void Destroy_HP_HttpServer(IntPtr pServer);
        /// <summary>
        ///  销毁 HP_HttpAgent 对象
        /// </summary>
        /// <param name="pAgent"></param>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void Destroy_HP_HttpAgent(IntPtr pAgent);
        /// <summary>
        ///  销毁 HP_HttpClient 对象
        /// </summary>
        /// <param name="pClient"></param>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void Destroy_HP_HttpClient(IntPtr pClient);

        /// <summary>
        /// 销毁 HP_HttpSyncClient 对象
        /// </summary>
        /// <param name="pClient"></param>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void Destroy_HP_HttpSyncClient(IntPtr pClient);

        /// <summary>
        ///  创建 HP_HttpServerListener 对象
        /// </summary>
        /// <returns></returns>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr Create_HP_HttpServerListener();
        /// <summary>
        ///  创建 HP_HttpAgentListener 对象
        /// </summary>
        /// <returns></returns>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr Create_HP_HttpAgentListener();
        /// <summary>
        /// 创建 HP_HttpClientListener 对象
        /// </summary>
        /// <returns></returns>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr Create_HP_HttpClientListener();

        /// <summary>
        ///  销毁 HP_HttpServerListener 对象
        /// </summary>
        /// <param name="pListener"></param>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void Destroy_HP_HttpServerListener(IntPtr pListener);
        /// <summary>
        ///  销毁 HP_HttpAgentListener 对象
        /// </summary>
        /// <param name="pListener"></param>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void Destroy_HP_HttpAgentListener(IntPtr pListener);
        /// <summary>
        ///  销毁 HP_HttpClientListener 对象
        /// </summary>
        /// <param name="pListener"></param>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void Destroy_HP_HttpClientListener(IntPtr pListener);


        /**********************************************************************************/
        /*************************** HTTP Server 回调函数设置方法 **************************/

        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnMessageBegin(IntPtr pListener, OnMessageBegin fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnRequestLine(IntPtr pListener, OnRequestLine fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnHeader(IntPtr pListener, OnHeader fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnHeadersComplete(IntPtr pListener, OnHeadersComplete fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnBody(IntPtr pListener, OnBody fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnChunkHeader(IntPtr pListener, OnChunkHeader fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnChunkComplete(IntPtr pListener, OnChunkComplete fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnMessageComplete(IntPtr pListener, OnMessageComplete fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnUpgrade(IntPtr pListener, OnUpgrade fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnParseError(IntPtr pListener, OnParseError fn);

        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnWSMessageHeader(IntPtr pListener, OnWSMessageHeader fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnWSMessageBody(IntPtr pListener, OnWSMessageBody fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnWSMessageComplete(IntPtr pListener, OnWSMessageComplete fn);


        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnPrepareListen(IntPtr pListener, Sdk.OnPrepareListen fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnAccept(IntPtr pListener, Sdk.OnAccept fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnHandShake(IntPtr pListener, Sdk.OnHandShake fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnReceive(IntPtr pListener, Sdk.OnReceive fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnSend(IntPtr pListener, Sdk.OnSend fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnClose(IntPtr pListener, Sdk.OnClose fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpServer_OnShutdown(IntPtr pListener, Sdk.OnShutdown fn);

        /**********************************************************************************/
        /**************************** HTTP Agent 回调函数设置方法 **************************/

        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnMessageBegin(IntPtr pListener, OnMessageBegin fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnStatusLine(IntPtr pListener, OnStatusLine fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnHeader(IntPtr pListener, OnHeader fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnHeadersComplete(IntPtr pListener, OnHeadersComplete fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnBody(IntPtr pListener, OnBody fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnChunkHeader(IntPtr pListener, OnChunkHeader fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnChunkComplete(IntPtr pListener, OnChunkComplete fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnMessageComplete(IntPtr pListener, OnMessageComplete fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnUpgrade(IntPtr pListener, OnUpgrade fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnParseError(IntPtr pListener, OnParseError fn);

        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnWSMessageHeader(IntPtr pListener, OnWSMessageHeader fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnWSMessageBody(IntPtr pListener, OnWSMessageBody fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnWSMessageComplete(IntPtr pListener, OnWSMessageComplete fn);


        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnPrepareConnect(IntPtr pListener, Sdk.OnPrepareConnect fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnConnect(IntPtr pListener, Sdk.OnConnect fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnHandShake(IntPtr pListener, Sdk.OnHandShake fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnReceive(IntPtr pListener, Sdk.OnReceive fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnSend(IntPtr pListener, Sdk.OnSend fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnClose(IntPtr pListener, Sdk.OnClose fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpAgent_OnShutdown(IntPtr pListener, Sdk.OnShutdown fn);

        /**********************************************************************************/
        /*************************** HTTP Client 回调函数设置方法 **************************/

        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnMessageBegin(IntPtr pListener, OnMessageBegin fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnStatusLine(IntPtr pListener, OnStatusLine fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnHeader(IntPtr pListener, OnHeader fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnHeadersComplete(IntPtr pListener, OnHeadersComplete fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnBody(IntPtr pListener, OnBody fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnChunkHeader(IntPtr pListener, OnChunkHeader fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnChunkComplete(IntPtr pListener, OnChunkComplete fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnMessageComplete(IntPtr pListener, OnMessageComplete fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnUpgrade(IntPtr pListener, OnUpgrade fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnParseError(IntPtr pListener, OnParseError fn);

        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnWSMessageHeader(IntPtr pListener, OnWSMessageHeader fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnWSMessageBody(IntPtr pListener, OnWSMessageBody fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnWSMessageComplete(IntPtr pListener, OnWSMessageComplete fn);


        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnPrepareConnect(IntPtr pListener, Sdk.OnPrepareConnect fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnConnect(IntPtr pListener, Sdk.OnConnect fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnHandShake(IntPtr pListener, Sdk.OnHandShake fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnReceive(IntPtr pListener, Sdk.OnReceive fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnSend(IntPtr pListener, Sdk.OnSend fn);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_Set_FN_HttpClient_OnClose(IntPtr pListener, Sdk.OnClose fn);



        /**************************************************************************/
        /***************************** Server 操作方法 *****************************/
        /*
               * 名称：回复请求
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
               */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpServer_SendResponse(IntPtr pServer, IntPtr dwConnID, ushort usStatusCode, string lpszDesc, THeader[] lpHeaders, int iHeaderCount, byte[] pData, int iLength);

        /*
        /*
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
        */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpServer_SendLocalFile(IntPtr pServer, IntPtr dwConnID, string lpszFileName, ushort usStatusCode, string lpszDesc, THeader[] lpHeaders, int iHeaderCount);

        /*
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
        */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpServer_SendWSMessage(IntPtr pServer, IntPtr dwConnID, bool bFinal, WSReserved iReserved, WSOpcode iOperationCode, byte[] lpszMask, IntPtr pData, int iLength, ulong ullBodyLen);
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpServer_SendWSMessage(IntPtr pServer, IntPtr dwConnID, bool bFinal, WSReserved iReserved, WSOpcode iOperationCode, byte[] lpszMask, byte[] pdata, int iLength, ulong ullBodyLen);


        /*
        * 名称：释放连接
        * 描述：把连接放入释放队列，等待某个时间（通过 SetReleaseDelay() 设置）关闭连接
        *
        * 参数：		dwConnID		-- 连接 ID
        * 返回值：	TRUE			-- 成功
        *			FALSE			-- 失败
        */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern bool HP_HttpServer_Release(IntPtr pServer, IntPtr dwConnID);

        /******************************************************************************/
        /***************************** Server 属性访问方法 *****************************/

        /* 设置连接释放延时（默认：3000 毫秒） */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_HttpServer_SetReleaseDelay(IntPtr pServer, uint dwReleaseDelay);
        /* 获取连接释放延时 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern uint HP_HttpServer_GetReleaseDelay(IntPtr pServer);
        /* 获取请求行 URL 域掩码（URL 域参考：EnHttpUrlField） */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern ushort HP_HttpServer_GetUrlFieldSet(IntPtr pServer, IntPtr dwConnID);
        /* 获取某个 URL 域值 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr HP_HttpServer_GetUrlField(IntPtr pServer, IntPtr dwConnID, HttpUrlField enField);
        /* 获取请求方法 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr HP_HttpServer_GetMethod(IntPtr pServer, IntPtr dwConnID);

        /* 设置本地协议版本 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_HttpServer_SetLocalVersion(IntPtr pServer, HttpVersion usVersion);
        /* 获取本地协议版本 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern HttpVersion HP_HttpServer_GetLocalVersion(IntPtr pServer);

        /* 检查是否升级协议 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern bool HP_HttpServer_IsUpgrade(IntPtr pServer, IntPtr dwConnID);
        /* 检查是否有 Keep-Alive 标识 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern bool HP_HttpServer_IsKeepAlive(IntPtr pServer, IntPtr dwConnID);
        /* 获取协议版本 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern ushort HP_HttpServer_GetVersion(IntPtr pServer, IntPtr dwConnID);
        /* 获取主机 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr HP_HttpServer_GetHost(IntPtr pServer, IntPtr dwConnID);
        /* 获取内容长度 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern long HP_HttpServer_GetContentLength(IntPtr pServer, IntPtr dwConnID);
        /* 获取内容类型 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr HP_HttpServer_GetContentType(IntPtr pServer, IntPtr dwConnID);
        /* 获取内容编码 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr HP_HttpServer_GetContentEncoding(IntPtr pServer, IntPtr dwConnID);
        /* 获取传输编码 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr HP_HttpServer_GetTransferEncoding(IntPtr pServer, IntPtr dwConnID);
        /* 获取协议升级类型 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern HttpUpgradeType HP_HttpServer_GetUpgradeType(IntPtr pServer, IntPtr dwConnID);
        /* 获取解析错误代码 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern ushort HP_HttpServer_GetParseErrorCode(IntPtr pServer, IntPtr dwConnID, ref IntPtr lpszErrorDesc);

        /* 获取某个请求头（单值） */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpServer_GetHeader(IntPtr pServer, IntPtr dwConnID, string lpszName, ref IntPtr lpszValue);
        /* 获取某个请求头（多值） */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpServer_GetHeaders(IntPtr pServer, IntPtr dwConnID, string lpszName, IntPtr[] lpszValue, ref uint pdwCount);
        /* 获取所有请求头 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern bool HP_HttpServer_GetAllHeaders(IntPtr pServer, IntPtr dwConnID, IntPtr lpHeaders, ref uint pdwCount);
        /* 获取所有请求头名称 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern bool HP_HttpServer_GetAllHeaderNames(IntPtr pServer, IntPtr dwConnID, IntPtr[] lpszName, ref uint pdwCount);
        /* 获取 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpServer_GetCookie(IntPtr pServer, IntPtr dwConnID, string lpszName, ref IntPtr lpszValue);
        /* 获取所有 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern bool HP_HttpServer_GetAllCookies(IntPtr pServer, IntPtr dwConnID, IntPtr lpCookies, ref uint pdwCount);

        /* 获取当前 WebSocket 消息状态，传入 nullptr 则不获取相应字段 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpServer_GetWSMessageState(IntPtr pServer, IntPtr dwConnID, ref bool lpbFinal, ref WSReserved lpiReserved, ref WSOpcode lpiOperationCode, ref IntPtr lpszMask, ref ulong lpullBodyLen, ref ulong lpullBodyRemain);


        /**************************************************************************/
        /***************************** Agent 操作方法 ******************************/

        /*
        * 名称：发送请求
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
        */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpAgent_SendRequest(IntPtr pAgent, IntPtr dwConnID, string lpszMethod, string lpszPath, THeader[] lpHeaders, int iHeaderCount, byte[] pData, int iLength);

        /*
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
        */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpAgent_SendLocalFile(IntPtr pAgent, IntPtr dwConnID, string lpszFileName, string lpszMethod, string lpszPath, THeader[] lpHeaders, int iHeaderCount);

        /* 发送 POST 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpAgent_SendPost(IntPtr pAgent, IntPtr dwConnID, string lpszPath, THeader[] lpHeaders, int iHeaderCount, string pBody, int iLength);
        /* 发送 PUT 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpAgent_SendPut(IntPtr pAgent, IntPtr dwConnID, string lpszPath, THeader[] lpHeaders, int iHeaderCount, string pBody, int iLength);
        /* 发送 PATCH 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpAgent_SendPatch(IntPtr pAgent, IntPtr dwConnID, string lpszPath, THeader[] lpHeaders, int iHeaderCount, string pBody, int iLength);
        /* 发送 GET 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpAgent_SendGet(IntPtr pAgent, IntPtr dwConnID, string lpszPath, THeader[] lpHeaders, int iHeaderCount);
        /* 发送 DELETE 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpAgent_SendDelete(IntPtr pAgent, IntPtr dwConnID, string lpszPath, THeader[] lpHeaders, int iHeaderCount);
        /* 发送 HEAD 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpAgent_SendHead(IntPtr pAgent, IntPtr dwConnID, string lpszPath, THeader[] lpHeaders, int iHeaderCount);
        /* 发送 TRACE 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpAgent_SendTrace(IntPtr pAgent, IntPtr dwConnID, string lpszPath, THeader[] lpHeaders, int iHeaderCount);
        /* 发送 OPTIONS 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpAgent_SendOptions(IntPtr pAgent, IntPtr dwConnID, string lpszPath, THeader[] lpHeaders, int iHeaderCount);
        /* 发送 CONNECT 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpAgent_SendConnect(IntPtr pAgent, IntPtr dwConnID, string lpszHost, THeader[] lpHeaders, int iHeaderCount);

        /*
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
        */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpAgent_SendWSMessage(IntPtr pAgent, IntPtr dwConnID, bool bFinal, byte iReserved, byte iOperationCode, byte[] lpszMask, IntPtr pData, int iLength, ulong ullBodyLen);


        /******************************************************************************/
        /***************************** Agent 属性访问方法 ******************************/

        /* 获取 HTTP 状态码 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern ushort HP_HttpAgent_GetStatusCode(IntPtr pAgent, IntPtr dwConnID);

        /* 设置本地协议版本 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_HttpAgent_SetLocalVersion(IntPtr pAgent, HttpVersion usVersion);
        /* 获取本地协议版本 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern HttpVersion HP_HttpAgent_GetLocalVersion(IntPtr pAgent);

        /* 检查是否升级协议 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern bool HP_HttpAgent_IsUpgrade(IntPtr pAgent, IntPtr dwConnID);
        /* 检查是否有 Keep-Alive 标识 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern bool HP_HttpAgent_IsKeepAlive(IntPtr pAgent, IntPtr dwConnID);
        /* 获取协议版本 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern ushort HP_HttpAgent_GetVersion(IntPtr pAgent, IntPtr dwConnID);
        /* 获取内容长度 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern long HP_HttpAgent_GetContentLength(IntPtr pAgent, IntPtr dwConnID);
        /* 获取内容类型 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr HP_HttpAgent_GetContentType(IntPtr pAgent, IntPtr dwConnID);
        /* 获取内容编码 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr HP_HttpAgent_GetContentEncoding(IntPtr pAgent, IntPtr dwConnID);
        /* 获取传输编码 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr HP_HttpAgent_GetTransferEncoding(IntPtr pAgent, IntPtr dwConnID);
        /* 获取协议升级类型 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern HttpUpgradeType HP_HttpAgent_GetUpgradeType(IntPtr pAgent, IntPtr dwConnID);
        /* 获取解析错误代码 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern ushort HP_HttpAgent_GetParseErrorCode(IntPtr pAgent, IntPtr dwConnID, ref IntPtr lpszErrorDesc);

        /* 获取某个请求头（单值） */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpAgent_GetHeader(IntPtr pAgent, IntPtr dwConnID, string lpszName, ref IntPtr lpszValue);
        /* 获取某个请求头（多值） */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpAgent_GetHeaders(IntPtr pAgent, IntPtr dwConnID, string lpszName, IntPtr[] lpszValue, ref uint pdwCount);
        /* 获取所有请求头 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern bool HP_HttpAgent_GetAllHeaders(IntPtr pAgent, IntPtr dwConnID, IntPtr lpHeaders, ref uint pdwCount);
        /* 获取所有请求头名称 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpAgent_GetAllHeaderNames(IntPtr pAgent, IntPtr dwConnID, IntPtr[] lpszName, ref uint pdwCount);

        /* 设置是否使用 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_HttpAgent_SetUseCookie(IntPtr pAgent, bool bUseCookie);
        /* 检查是否使用 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern bool HP_HttpAgent_IsUseCookie(IntPtr pAgent);

        /* 获取 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpAgent_GetCookie(IntPtr pAgent, IntPtr dwConnID, string lpszName, ref IntPtr lpszValue);
        /* 获取所有 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern bool HP_HttpAgent_GetAllCookies(IntPtr pAgent, IntPtr dwConnID, IntPtr lpCookies, ref uint pdwCount);
        /* 获取当前 WebSocket 消息状态，传入 nullptr 则不获取相应字段 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern bool HP_HttpAgent_GetWSMessageState(IntPtr pAgent, IntPtr dwConnID, ref bool lpbFinal, ref byte lpiReserved, ref byte lpiOperationCode, ref IntPtr lpszMask, ref ulong lpullBodyLen, ref ulong lpullBodyRemain);




        /**************************************************************************/
        /***************************** Client 操作方法 *****************************/

        /*
        * 名称：发送请求
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
        */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_SendRequest(IntPtr pClient, string lpszMethod, string lpszPath, THeader[] lpHeaders, int iHeaderCount, byte[] pBody, int iLength);

        /*
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
        */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_SendLocalFile(IntPtr pClient, string lpszFileName, string lpszMethod, string lpszPath, THeader[] lpHeaders, int iHeaderCount);

        /* 发送 POST 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_SendPost(IntPtr pClient, string lpszPath, THeader[] lpHeaders, int iHeaderCount, string pBody, int iLength);
        /* 发送 PUT 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_SendPut(IntPtr pClient, string lpszPath, THeader[] lpHeaders, int iHeaderCount, string pBody, int iLength);
        /* 发送 PATCH 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_SendPatch(IntPtr pClient, string lpszPath, THeader[] lpHeaders, int iHeaderCount, string pBody, int iLength);
        /* 发送 GET 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_SendGet(IntPtr pClient, string lpszPath, THeader[] lpHeaders, int iHeaderCount);
        /* 发送 DELETE 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_SendDelete(IntPtr pClient, string lpszPath, THeader[] lpHeaders, int iHeaderCount);
        /* 发送 HEAD 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_SendHead(IntPtr pClient, string lpszPath, THeader[] lpHeaders, int iHeaderCount);
        /* 发送 TRACE 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_SendTrace(IntPtr pClient, string lpszPath, THeader[] lpHeaders, int iHeaderCount);
        /* 发送 OPTIONS 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_SendOptions(IntPtr pClient, string lpszPath, THeader[] lpHeaders, int iHeaderCount);
        /* 发送 CONNECT 请求 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_SendConnect(IntPtr pClient, string lpszHost, THeader[] lpHeaders, int iHeaderCount);

        /*
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
        */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_SendWSMessage(IntPtr pClient, bool bFinal, byte iReserved, byte iOperationCode, byte[] lpszMask, IntPtr pData, int iLength, ulong ullBodyLen);

        /******************************************************************************/
        /***************************** Client 属性访问方法 *****************************/

        /* 获取 HTTP 状态码 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern ushort HP_HttpClient_GetStatusCode(IntPtr pClient);

        /* 设置本地协议版本 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern void HP_HttpClient_SetLocalVersion(IntPtr pClient, HttpVersion usVersion);
        /* 获取本地协议版本 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern HttpVersion HP_HttpClient_GetLocalVersion(IntPtr pClient);

        /* 检查是否升级协议 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_IsUpgrade(IntPtr pClient);
        /* 检查是否有 Keep-Alive 标识 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_IsKeepAlive(IntPtr pClient);
        /* 获取协议版本 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern ushort HP_HttpClient_GetVersion(IntPtr pClient);
        /* 获取内容长度 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern long HP_HttpClient_GetContentLength(IntPtr pClient);
        /* 获取内容类型 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern IntPtr HP_HttpClient_GetContentType(IntPtr pClient);
        /* 获取内容编码 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr HP_HttpClient_GetContentEncoding(IntPtr pClient);
        /* 获取传输编码 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern IntPtr HP_HttpClient_GetTransferEncoding(IntPtr pClient);
        /* 获取协议升级类型 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern HttpUpgradeType HP_HttpClient_GetUpgradeType(IntPtr pClient);
        /* 获取解析错误代码 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern ushort HP_HttpClient_GetParseErrorCode(IntPtr pClient, ref IntPtr lpszErrorDesc);

        /* 获取某个请求头（单值） */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_GetHeader(IntPtr pClient, string lpszName, ref IntPtr lpszValue);
        /* 获取某个请求头（多值） */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_GetHeaders(IntPtr pClient, string lpszName, IntPtr[] lpszValue, ref uint pdwCount);
        /* 获取所有请求头 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_GetAllHeaders(IntPtr pClient, IntPtr lpHeaders, ref uint pdwCount);
        /* 获取所有请求头名称 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_GetAllHeaderNames(IntPtr pClient, IntPtr[] lpszName, ref uint pdwCount);

        /* 设置是否使用 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_HttpClient_SetUseCookie(IntPtr pClient, bool bUseCookie);
        /* 检查是否使用 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern bool HP_HttpClient_IsUseCookie(IntPtr pClient);
        /* 获取 Cookie */

        /* 获取 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_GetCookie(IntPtr pClient, string lpszName, ref IntPtr lpszValue);
        /* 获取所有 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpClient_GetAllCookies(IntPtr pClient, IntPtr lpCookies, ref uint pdwCount);

        /* 获取当前 WebSocket 消息状态，传入 nullptr 则不获取相应字段 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern bool HP_HttpClient_GetWSMessageState(IntPtr pClient, ref bool lpbFinal, ref byte lpiReserved, ref byte lpiOperationCode, ref IntPtr lpszMask, ref ulong lpullBodyLen, ref ulong lpullBodyRemain);


        /**************************************************************************/
        /************************ HTTP Sync Client 操作方法 ************************/
        /// <summary>
        /// 名称：发送 URL 请求
        /// 描述：向服务端发送 HTTP URL 请求
        /// </summary>
        /// <param name="pClient"></param>
        /// <param name="lpszMethod">请求方法</param>
        /// <param name="lpszUrl">请求 URL</param>
        /// <param name="lpHeaders">请求头</param>
        /// <param name="iHeaderCount">请求头数量</param>
        /// <param name="pBody">请求体</param>
        /// <param name="iLength">请求体长度</param>
        /// <param name="bForceReconnect">强制重新连接（默认：FALSE，当请求 URL 的主机和端口与现有连接一致时，重用现有连接）</param>
        /// <returns></returns>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpSyncClient_OpenUrl(IntPtr pClient, string lpszMethod, string lpszUrl, THeader[] lpHeaders, int iHeaderCount, string pData, int iLength, bool bForceReconnect);


        /// <summary>
        /// 名称：清除请求结果
        /// 描述：清除上一次请求的响应头和响应体等结果信息（该方法会在每次发送请求前自动调用）
        /// </summary>
        /// <param name="pClient"></param>
        /// <returns></returns>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern bool HP_HttpSyncClient_CleanupRequestResult(IntPtr pClient);

        /******************************************************************************/
        /************************ HTTP Sync Client 属性访问方法 ************************/
        /// <summary>
        /// 设置连接超时（毫秒，0：系统默认超时，默认：5000）
        /// </summary>
        /// <param name="pClient"></param>
        /// <param name="dwConnectTimeout"></param>
        /// <returns></returns>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_HttpSyncClient_SetConnectTimeout(IntPtr pClient, uint dwConnectTimeout);
        /// <summary>
        /// 设置请求超时（毫秒，0：无限等待，默认：10000）
        /// </summary>
        /// <param name="pClient"></param>
        /// <param name="dwConnectTimeout"></param>
        /// <returns></returns>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_HttpSyncClient_SetRequestTimeout(IntPtr pClient, uint dwRequestTimeout);
        /// <summary>
        /// 获取连接超时
        /// </summary>
        /// <param name="pClient"></param>
        /// <param name="dwConnectTimeout"></param>
        /// <returns></returns>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern uint HP_HttpSyncClient_GetConnectTimeout(IntPtr pClient);
        /// <summary>
        /// 获取请求超时
        /// </summary>
        /// <param name="pClient"></param>
        /// <param name="dwConnectTimeout"></param>
        /// <returns></returns>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern uint HP_HttpSyncClient_GetRequestTimeout(IntPtr pClient);

        /// <summary>
        /// 获取响应体
        /// </summary>
        /// <param name="pClient"></param>
        /// <param name="dwConnectTimeout"></param>
        /// <returns></returns>
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern uint HP_HttpSyncClient_GetResponseBody(IntPtr pClient, ref IntPtr lpszBody, ref int iLength);

        /**************************************************************************/
        /*************************** HTTP Cookie 管理方法 **************************/
        /* 从文件加载 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpCookie_MGR_LoadFromFile(string lpszFile, bool bKeepExists /*= TRUE*/);
        /* 保存 Cookie 到文件 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpCookie_MGR_SaveToFile(string lpszFile, bool bKeepExists /*= TRUE*/);
        /* 清理 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpCookie_MGR_ClearCookies(string lpszDomain /*= nullptr*/, string lpszPath /*= nullptr*/);
        /* 清理过期 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpCookie_MGR_RemoveExpiredCookies(string lpszDomain /*= nullptr*/, string lpszPath /*= nullptr*/);
        /* 设置 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpCookie_MGR_SetCookie(string lpszName, string lpszValue, string lpszDomain, string lpszPath, int iMaxAge /*= -1*/, bool bHttpOnly /*= FALSE*/, bool bSecure /*= FALSE*/, int enSameSite /*= 0*/, bool bOnlyUpdateValueIfExists /*= TRUE*/);
        /* 删除 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpCookie_MGR_DeleteCookie(string lpszDomain, string lpszPath, string lpszName);
        /* 设置是否允许第三方 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern void HP_HttpCookie_MGR_SetEnableThirdPartyCookie(bool bEnableThirdPartyCookie /*= TRUE*/);
        /* 检查是否允许第三方 Cookie */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern bool HP_HttpCookie_MGR_IsEnableThirdPartyCookie();

        /* Cookie expires 字符串转换为整数 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpCookie_HLP_ParseExpires(string lpszExpires, ref ulong ptmExpires);
        /* 整数转换为 Cookie expires 字符串 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpCookie_HLP_MakeExpiresStr([MarshalAs(UnmanagedType.LPStr)] StringBuilder lpszBuff, ref int piBuffLen, ulong tmExpires);
        /* 生成 Cookie 字符串 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool HP_HttpCookie_HLP_ToString([MarshalAs(UnmanagedType.LPStr)] StringBuilder lpszBuff, ref int piBuffLen, string lpszName, string lpszValue, string lpszDomain, string lpszPath, int iMaxAge /*= -1*/, bool bHttpOnly /*= FALSE*/, bool bSecure /*= FALSE*/, int enSameSite /*= 0*/);
        /* 获取当前 UTC 时间 */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern ulong HP_HttpCookie_HLP_CurrentUTCTime();
        /* Max-Age -> expires */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern ulong HP_HttpCookie_HLP_MaxAgeToExpires(int iMaxAge);
        /* expires -> Max-Age */
        [DllImport(HPSOCKET_HTTP_DLL_PATH, SetLastError = true)]
        public static extern int HP_HttpCookie_HLP_ExpiresToMaxAge(ulong tmExpires);
    }
}
