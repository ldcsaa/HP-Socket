using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace HPSocketCS
{
    public class HttpAgent : TcpAgent
    {
        /// <summary>
        /// 【可选】开始解析
        /// </summary>
        public event HttpAgentEvent.OnMessageBeginEventHandler OnMessageBegin;
        /// <summary>
        /// 【可选】请求头通知
        /// </summary>
        public event HttpAgentEvent.OnHeaderEventHandler OnHeader;
        /// <summary>
        /// 【可选】Chunked 报文头通知
        /// </summary>
        public event HttpAgentEvent.OnChunkHeaderEventHandler OnChunkHeader;
        /// <summary>
        /// 【可选】Chunked 报文结束通知
        /// </summary>
        public event HttpAgentEvent.OnChunkCompleteEventHandler OnChunkComplete;
        /// <summary>
        /// 【可选】升级协议通知
        /// </summary>
        public event HttpAgentEvent.OnUpgradeEventHandler OnUpgrade;
        /// <summary>
        /// 【可选】状态行解析完成（仅用于 HTTP 客户端）
        /// </summary>
        public event HttpAgentEvent.OnStatusLineEventHandler OnStatusLine;
        /// <summary>
        /// 【必须】请求头完成通知
        /// </summary>
        public event HttpAgentEvent.OnHeadersCompleteEventHandler OnHeadersComplete;
        /// <summary>
        /// 【必须】请求体报文通知
        /// </summary>
        public event HttpAgentEvent.OnBodyEventHandler OnBody;
        /// <summary>
        /// 【必须】请求体报文通知
        /// </summary>
        public event HttpAgentEvent.OnPointerDataBodyEventHandler OnPointerDataBody;
        /// <summary>
        /// 【必须】完成解析通知
        /// </summary>
        public event HttpAgentEvent.OnMessageCompleteEventHandler OnMessageComplete;
        /// <summary>
        /// 【必须】解析错误通知
        /// </summary>
        public event HttpAgentEvent.OnParseErrorEventHandler OnParseError;

        /// <summary>
        /// 创建socket监听&服务组件
        /// </summary>
        /// <returns></returns>
        protected override bool CreateListener()
        {
            if (IsCreate == true || pListener != IntPtr.Zero || pAgent != IntPtr.Zero)
            {
                return false;
            }

            pListener = HttpSdk.Create_HP_HttpAgentListener();
            if (pListener == IntPtr.Zero)
            {
                return false;
            }
            pAgent = HttpSdk.Create_HP_HttpAgent(pListener);
            if (pAgent == IntPtr.Zero)
            {
                return false;
            }

            IsCreate = true;

            return true;
        }

        /// <summary>
        /// 终止服务并释放资源
        /// </summary>
        public override void Destroy()
        {
            Stop();

            if (pAgent != IntPtr.Zero)
            {
                HttpSdk.Destroy_HP_HttpAgent(pAgent);
                pAgent = IntPtr.Zero;
            }
            if (pListener != IntPtr.Zero)
            {
                HttpSdk.Destroy_HP_HttpAgentListener(pListener);
                pListener = IntPtr.Zero;
            }

            IsCreate = false;
        }


        HttpSdk.OnMessageBegin _OnMessageBegin;
        HttpSdk.OnHeader _OnHeader;
        HttpSdk.OnChunkHeader _OnChunkHeader;
        HttpSdk.OnChunkComplete _OnChunkComplete;
        HttpSdk.OnUpgrade _OnUpgrade;
        HttpSdk.OnStatusLine _OnStatusLine;
        HttpSdk.OnHeadersComplete _OnHeadersComplete;
        HttpSdk.OnBody _OnBody;
        HttpSdk.OnMessageComplete _OnMessageComplete;
        HttpSdk.OnParseError _OnParseError;

        ///////////////////////////////////////////////////////////////////////////////////////


        protected override void SetCallback()
        {
            // 设置tcpagent的callback
            _OnPrepareConnect = new Sdk.OnPrepareConnect(SDK_OnPrepareConnect);
            _OnConnect = new Sdk.OnConnect(SDK_OnConnect);
            _OnSend = new Sdk.OnSend(SDK_OnSend);
            _OnReceive = new Sdk.OnReceive(SDK_OnReceive);
            _OnClose = new Sdk.OnClose(SDK_OnClose);
            _OnShutdown = new Sdk.OnShutdown(SDK_OnShutdown);
            _OnHandShake = new Sdk.OnHandShake(SDK_OnHandShake);

            // 设置 Socket 监听器回调函数
            HttpSdk.HP_Set_FN_HttpAgent_OnPrepareConnect(pListener, _OnPrepareConnect);
            HttpSdk.HP_Set_FN_HttpAgent_OnConnect(pListener, _OnConnect);
            HttpSdk.HP_Set_FN_HttpAgent_OnSend(pListener, _OnSend);
            HttpSdk.HP_Set_FN_HttpAgent_OnReceive(pListener, _OnReceive);
            HttpSdk.HP_Set_FN_HttpAgent_OnClose(pListener, _OnClose);
            HttpSdk.HP_Set_FN_HttpAgent_OnShutdown(pListener, _OnShutdown);
            HttpSdk.HP_Set_FN_HttpAgent_OnHandShake(pListener, _OnHandShake);


            // 设置自身的callback
            _OnMessageBegin = new HttpSdk.OnMessageBegin(SDK_OnMessageBegin);
            _OnHeader = new HttpSdk.OnHeader(SDK_OnHeader);
            _OnChunkHeader = new HttpSdk.OnChunkHeader(SDK_OnChunkHeader);
            _OnChunkComplete = new HttpSdk.OnChunkComplete(SDK_OnChunkComplete);
            _OnUpgrade = new HttpSdk.OnUpgrade(SDK_OnUpgrade);
            _OnStatusLine = new HttpSdk.OnStatusLine(SDK_OnStatusLine);
            _OnHeadersComplete = new HttpSdk.OnHeadersComplete(SDK_OnHeadersComplete);
            _OnBody = new HttpSdk.OnBody(SDK_OnBody);
            _OnMessageComplete = new HttpSdk.OnMessageComplete(SDK_OnMessageComplete);
            _OnParseError = new HttpSdk.OnParseError(SDK_OnParseError);

            HttpSdk.HP_Set_FN_HttpAgent_OnMessageBegin(pListener, _OnMessageBegin);
            HttpSdk.HP_Set_FN_HttpAgent_OnStatusLine(pListener, _OnStatusLine);
            HttpSdk.HP_Set_FN_HttpAgent_OnHeader(pListener, _OnHeader);
            HttpSdk.HP_Set_FN_HttpAgent_OnHeadersComplete(pListener, _OnHeadersComplete);
            HttpSdk.HP_Set_FN_HttpAgent_OnBody(pListener, _OnBody);
            HttpSdk.HP_Set_FN_HttpAgent_OnChunkHeader(pListener, _OnChunkHeader);
            HttpSdk.HP_Set_FN_HttpAgent_OnChunkComplete(pListener, _OnChunkComplete);
            HttpSdk.HP_Set_FN_HttpAgent_OnMessageComplete(pListener, _OnMessageComplete);
            HttpSdk.HP_Set_FN_HttpAgent_OnUpgrade(pListener, _OnUpgrade);
            HttpSdk.HP_Set_FN_HttpAgent_OnParseError(pListener, _OnParseError);
        }

        protected HttpParseResult SDK_OnParseError(IntPtr pSender, IntPtr connId, int iErrorCode, string lpszErrorDesc)
        {
            if (OnParseError != null)
            {
                return OnParseError(connId, iErrorCode, lpszErrorDesc);
            }
            return HttpParseResult.Ok;
        }

        protected HttpParseResult SDK_OnMessageComplete(IntPtr pSender, IntPtr connId)
        {
            if (OnMessageComplete != null)
            {
                return OnMessageComplete(connId);
            }
            return HttpParseResult.Ok;
        }

        protected HttpParseResult SDK_OnBody(IntPtr pSender, IntPtr connId, IntPtr pData, int iLength)
        {
            if (OnPointerDataBody != null)
            {
                return OnPointerDataBody(connId, pData, iLength);
            }
            else if (OnBody != null)
            {
                byte[] bytes = new byte[iLength];
                Marshal.Copy(pData, bytes, 0, iLength);
                return OnBody(connId, bytes);
            }

            return HttpParseResult.Ok;
        }

        private HttpParseResultEx SDK_OnHeadersComplete(IntPtr pSender, IntPtr connId)
        {
            if (OnHeadersComplete != null)
            {
                return OnHeadersComplete(connId);
            }
            return HttpParseResultEx.Ok;
        }

        protected HttpParseResult SDK_OnStatusLine(IntPtr pSender, IntPtr connId, ushort usStatusCode, string lpszDesc)
        {
            if (OnStatusLine != null)
            {
                return OnStatusLine(connId, usStatusCode, lpszDesc);
            }
            return HttpParseResult.Ok;
        }

        protected HttpParseResult SDK_OnUpgrade(IntPtr pSender, IntPtr connId, HttpUpgradeType enUpgradeType)
        {
            if (OnUpgrade != null)
            {
                return OnUpgrade(connId, enUpgradeType);
            }
            return HttpParseResult.Ok;
        }

        protected HttpParseResult SDK_OnChunkComplete(IntPtr pSender, IntPtr connId)
        {
            if (OnChunkComplete != null)
            {
                return OnChunkComplete(connId);
            }
            return HttpParseResult.Ok;
        }

        protected HttpParseResult SDK_OnChunkHeader(IntPtr pSender, IntPtr connId, int iLength)
        {
            if (OnChunkHeader != null)
            {
                return OnChunkHeader(connId, iLength);
            }
            return HttpParseResult.Ok;
        }

        protected HttpParseResult SDK_OnHeader(IntPtr pSender, IntPtr connId, string lpszName, string lpszValue)
        {
            if (OnHeader != null)
            {
                return OnHeader(connId, lpszName, lpszValue);
            }
            return HttpParseResult.Ok;
        }

        protected HttpParseResult SDK_OnMessageBegin(IntPtr pSender, IntPtr connId)
        {
            if (OnMessageBegin != null)
            {
                return OnMessageBegin(connId);
            }
            return HttpParseResult.Ok;
        }


        /**************************************************************************/
        /***************************** Agent 操作方法 ******************************/

        /// <summary>
        /// 发送请求
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="method">请求方法</param>
        /// <param name="path">请求路径</param>
        /// <param name="headers">请求头</param>
        /// <param name="body">请求体</param>
        /// <returns></returns>
        public bool SendRequest(IntPtr connId, HttpMethod method, string path, THeader[] headers, byte[] body, int bodyLength)
        {
            int headersLength = headers == null ? 0 : headers.Length;
            return HttpSdk.HP_HttpAgent_SendRequest(pAgent, connId, method.ToString(), path, headers, headersLength, body, bodyLength);
        }

        /// <summary>
        /// 名称：发送本地文件
        /// 描述：向指定连接发送 4096 KB 以下的小文件
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="fileName"></param>
        /// <param name="method"></param>
        /// <param name="path"></param>
        /// <param name="headers"></param>
        /// <returns></returns>
        public bool SendLocalFile(IntPtr connId, string fileName, HttpMethod method, string path, THeader[] headers)
        {
            int headersLength = headers == null ? 0 : headers.Length;
            return HttpSdk.HP_HttpAgent_SendLocalFile(pAgent, connId, fileName, method.ToString(), path, headers, headersLength);
        }

        /// <summary>
        /// 发送 POST 请求
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="path"></param>
        /// <param name="headers"></param>
        /// <param name="body"></param>
        /// <returns></returns>
        public bool SendPost(IntPtr connId, string path, THeader[] headers, string body)
        {
            int bodyLength = body == null ? 0 : body.Length;
            int headersLength = headers == null ? 0 : headers.Length;
            return HttpSdk.HP_HttpAgent_SendPost(pAgent, connId, path, headers, headersLength, body, bodyLength);
        }

        /// <summary>
        /// 发送 PUT 请求
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="path"></param>
        /// <param name="headers"></param>
        /// <param name="body"></param>
        /// <returns></returns>
        public bool SendPut(IntPtr connId, string path, THeader[] headers, string body)
        {
            int bodyLength = body == null ? 0 : body.Length;
            int headersLength = headers == null ? 0 : headers.Length;
            return HttpSdk.HP_HttpAgent_SendPut(pAgent, connId, path, headers, headersLength, body, bodyLength);
        }

        /// <summary>
        /// 发送 PATCH 请求
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="path"></param>
        /// <param name="headers"></param>
        /// <param name="body"></param>
        /// <returns></returns>
        public bool SendPatch(IntPtr connId, string path, THeader[] headers, string body)
        {
            int bodyLength = body == null ? 0 : body.Length;
            int headersLength = headers == null ? 0 : headers.Length;
            return HttpSdk.HP_HttpAgent_SendPatch(pAgent, connId, path, headers, headersLength, body, bodyLength);
        }

        /// <summary>
        /// 发送 GET 请求
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="path"></param>
        /// <param name="headers"></param>
        /// <returns></returns>
        public bool SendGet(IntPtr connId, string path, THeader[] headers)
        {
            int headersLength = headers == null ? 0 : headers.Length;
            return HttpSdk.HP_HttpAgent_SendGet(pAgent, connId, path, headers, headersLength);
        }

        /// <summary>
        /// 发送 DELETE 请求
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="path"></param>
        /// <param name="headers"></param>
        /// <returns></returns>
        public bool SendDelete(IntPtr connId, string path, THeader[] headers)
        {
            return HttpSdk.HP_HttpAgent_SendDelete(pAgent, connId, path, headers, headers.Length);
        }

        /// <summary>
        /// 发送 HEAD 请求
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="path"></param>
        /// <param name="headers"></param>
        /// <returns></returns>
        public bool SendHead(IntPtr connId, string path, THeader[] headers)
        {
            int headersLength = headers == null ? 0 : headers.Length;
            return HttpSdk.HP_HttpAgent_SendHead(pAgent, connId, path, headers, headersLength);
        }

        /// <summary>
        /// 发送 TRACE 请求
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="path"></param>
        /// <param name="headers"></param>
        /// <returns></returns>
        public bool SendTrace(IntPtr connId, string path, THeader[] headers)
        {
            int headersLength = headers == null ? 0 : headers.Length;
            return HttpSdk.HP_HttpAgent_SendTrace(pAgent, connId, path, headers, headersLength);
        }

        /// <summary>
        /// 发送 OPTIONS 请求
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="path"></param>
        /// <param name="headers"></param>
        /// <returns></returns>
        public bool SendOptions(IntPtr connId, string path, THeader[] headers)
        {
            int headersLength = headers == null ? 0 : headers.Length;
            return HttpSdk.HP_HttpAgent_SendOptions(pAgent, connId, path, headers, headersLength);
        }

        /// <summary>
        /// 发送 CONNECT 请求
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="path"></param>
        /// <param name="headers"></param>
        /// <returns></returns>
        public bool SendConnect(IntPtr connId, string path, THeader[] headers)
        {
            int headersLength = headers == null ? 0 : headers.Length;
            return HttpSdk.HP_HttpAgent_SendConnect(pAgent, connId, path, headers, headersLength);
        }

        /******************************************************************************/
        /***************************** Agent 属性访问方法 ******************************/

        /// <summary>
        /// 获取或设置是否使用 Cookie
        /// </summary>
        public bool UseCookie
        {
            get
            {
                return HttpSdk.HP_HttpAgent_IsUseCookie(pAgent);
            }
            set
            {
                HttpSdk.HP_HttpAgent_SetUseCookie(pAgent, value);
            }
        }

        /// <summary>
        /// 获取 HTTP 状态码
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public HttpStatusCode GetStatusCode(IntPtr connId)
        {
            // 有没有可能获取到未定义的状态码???
            return (HttpStatusCode)HttpSdk.HP_HttpAgent_GetStatusCode(pAgent, connId);
        }

        /// <summary>
        /// 获取或设置本地协议版本
        /// </summary>
        public HttpVersion LocalVersion
        {
            get
            {
                return HttpSdk.HP_HttpAgent_GetLocalVersion(pAgent);
            }
            set
            {
                HttpSdk.HP_HttpAgent_SetLocalVersion(pAgent, value);
            }
        }

        /// <summary>
        /// 检查是否升级协议
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public bool IsUpgrade(IntPtr connId)
        {
            return HttpSdk.HP_HttpAgent_IsUpgrade(pAgent, connId);
        }

        /// <summary>
        /// 检查是否有 Keep-Alive 标识
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public bool IsKeepAlive(IntPtr connId)
        {
            return HttpSdk.HP_HttpAgent_IsKeepAlive(pAgent, connId);
        }

        /// <summary>
        /// 获取协议版本
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public HttpVersion GetVersion(IntPtr connId)
        {
            return (HttpVersion)HttpSdk.HP_HttpAgent_GetVersion(pAgent, connId);
        }

        /// <summary>
        /// 获取内容长度
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public long GetContentLength(IntPtr connId)
        {
            return HttpSdk.HP_HttpAgent_GetContentLength(pAgent, connId);
        }


        /// <summary>
        /// 获取内容类型
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public string GetContentType(IntPtr connId)
        {
            IntPtr ptr = HttpSdk.HP_HttpAgent_GetContentType(pAgent, connId);
            return Common.PtrToAnsiString(ptr);
        }

        /// <summary>
        /// 获取内容类型
        /// </summary>
        /// <returns></returns>
        public string GetContentEncoding(IntPtr connId)
        {
            IntPtr ptr = HttpSdk.HP_HttpAgent_GetContentEncoding(pAgent, connId);
            return Common.PtrToAnsiString(ptr);
        }

        /// <summary>
        /// 获取传输编码
        /// </summary>
        /// <returns></returns>
        public string GetTransferEncoding(IntPtr connId)
        {
            IntPtr ptr = HttpSdk.HP_HttpAgent_GetTransferEncoding(pAgent, connId);
            return Common.PtrToAnsiString(ptr);
        }

        /// <summary>
        /// 获取协议升级类型
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public HttpUpgradeType GetUpgradeType(IntPtr connId)
        {
            return HttpSdk.HP_HttpAgent_GetUpgradeType(pAgent, connId);
        }

        /// <summary>
        /// 获取解析错误代码
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public ErrorInfo GetParseErrorInfo(IntPtr connId)
        {
            IntPtr ptr = IntPtr.Zero;
            var error = new ErrorInfo();
            error.ErrorCode = HttpSdk.HP_HttpAgent_GetParseErrorCode(pAgent, connId, ref ptr);
            if (ptr != IntPtr.Zero)
            {
                try
                {
                    error.ErrorMsg = Common.PtrToAnsiString(ptr); ;
                }
                catch (Exception)
                {
                }
            }
            return error;
        }

        /// <summary>
        /// 获取某个请求头（单值）
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="key"></param>
        /// <returns></returns>
        public string GetHeader(IntPtr connId, string key)
        {
            IntPtr ptr = IntPtr.Zero;
            if (HttpSdk.HP_HttpAgent_GetHeader(pAgent, connId, key, ref ptr))
            {
                return Common.PtrToAnsiString(ptr);
            }
            return "";
        }

        /// <summary>
        /// 获取某个请求头（多值）
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="key"></param>
        /// <returns></returns>
        public List<string> GetHeaders(IntPtr connId, string key)
        {
            var list = new List<string>();
            try
            {
                IntPtr[] arr = null;
                uint count = 0;
                HttpSdk.HP_HttpAgent_GetHeaders(pAgent, connId, key, arr, ref count);
                if (count > 0)
                {
                    arr = new IntPtr[count];
                    if (HttpSdk.HP_HttpAgent_GetHeaders(pAgent, connId, key, arr, ref count) && count > 0)
                    {
                        foreach (var item in arr)
                        {
                            list.Add(Common.PtrToAnsiString(item));
                        }
                    }
                }
            }
            catch (Exception)
            {
            }
            return list;
        }

        /// <summary>
        /// 获取所有请求头
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public List<THeader> GetAllHeaders(IntPtr connId)
        {
            var list = new List<THeader>();
            try
            {
                uint count = 0;
                IntPtr arrPtr = IntPtr.Zero;
                HttpSdk.HP_HttpAgent_GetAllHeaders(pAgent, connId, arrPtr, ref count);
                if (count > 0)
                {
                    var ptrArr = new THeaderForIntPtr[count];
                    arrPtr = Marshal.UnsafeAddrOfPinnedArrayElement(ptrArr, 0);
                    if (HttpSdk.HP_HttpAgent_GetAllHeaders(pAgent, connId, arrPtr, ref count))
                    {
                        foreach (var item in ptrArr)
                        {
                            list.Add(new THeader()
                            {
                                Name = Common.PtrToAnsiString(item.Name),
                                Value = Common.PtrToAnsiString(item.Value),
                            });
                        }
                    }
                }
            }
            catch (Exception)
            {
            }

            return list;
        }


        /// <summary>
        /// 获取所有请求头名称
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public List<string> GetAllHeaderNames(IntPtr connId)
        {
            var list = new List<string>();
            try
            {
                IntPtr[] arr = null;
                uint count = 0;
                HttpSdk.HP_HttpAgent_GetAllHeaderNames(pAgent, connId, arr, ref count);
                if (count > 0)
                {
                    arr = new IntPtr[count];
                    if (HttpSdk.HP_HttpAgent_GetAllHeaderNames(pAgent, connId, arr, ref count) && count > 0)
                    {
                        foreach (var item in arr)
                        {
                            list.Add(Common.PtrToAnsiString(item));
                        }
                    }
                }
            }
            catch (Exception)
            {
            }
            return list;
        }

        /// <summary>
        /// 获取Cookie
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="key"></param>
        /// <returns></returns>
        public string GetCookie(IntPtr connId, string key)
        {
            IntPtr ptr = IntPtr.Zero;
            if (HttpSdk.HP_HttpAgent_GetCookie(pAgent, connId, key, ref ptr))
            {
                return Common.PtrToAnsiString(ptr);
            }
            return "";
        }

        /// <summary>
        /// 获取所有 Cookie
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="lpCookies"></param>
        /// <param name="pdwCount"></param>
        /// <returns></returns>
        public List<TCookie> GetAllCookies(IntPtr connId)
        {
            var list = new List<TCookie>();
            try
            {
                uint count = 0;
                IntPtr arrPtr = IntPtr.Zero;
                HttpSdk.HP_HttpAgent_GetAllCookies(pAgent, connId, arrPtr, ref count);
                if (count > 0)
                {
                    var ptrArr = new THeaderForIntPtr[count];
                    arrPtr = Marshal.UnsafeAddrOfPinnedArrayElement(ptrArr, 0);
                    if (HttpSdk.HP_HttpAgent_GetAllCookies(pAgent, connId, arrPtr, ref count))
                    {
                        foreach (var item in ptrArr)
                        {
                            list.Add(new TCookie()
                            {
                                Name = Common.PtrToAnsiString(item.Name),
                                Value = Common.PtrToAnsiString(item.Value)
                            });
                        }
                    }
                }
            }
            catch (Exception)
            {
            }

            return list;
        }
    }
}
