using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace HPSocketCS
{
    public class HttpServer : TcpServer
    {
        /// <summary>
        /// 【可选】开始解析
        /// </summary>
        public event HttpServerEvent.OnMessageBeginEventHandler OnMessageBegin;
        /// <summary>
        /// 【可选】请求头通知
        /// </summary>
        public event HttpServerEvent.OnHeaderEventHandler OnHeader;
        /// <summary>
        /// 【可选】Chunked 报文头通知
        /// </summary>
        public event HttpServerEvent.OnChunkHeaderEventHandler OnChunkHeader;
        /// <summary>
        /// 【可选】Chunked 报文结束通知
        /// </summary>
        public event HttpServerEvent.OnChunkCompleteEventHandler OnChunkComplete;
        /// <summary>
        /// 【可选】升级协议通知
        /// </summary>
        public event HttpServerEvent.OnUpgradeEventHandler OnUpgrade;
        /// <summary>
        /// 【可选】请求行解析完成（仅用于 HTTP 服务端）
        /// </summary>
        public event HttpServerEvent.OnRequestLineEventHandler OnRequestLine;
        /// <summary>
        /// 【必须】请求头完成通知
        /// </summary>
        public event HttpServerEvent.OnHeadersCompleteEventHandler OnHeadersComplete;
        /// <summary>
        /// 【必须】请求体报文通知
        /// </summary>
        public event HttpServerEvent.OnBodyEventHandler OnBody;
        /// <summary>
        /// 【必须】请求体报文通知
        /// </summary>
        public event HttpServerEvent.OnPointerDataBodyEventHandler OnPointerDataBody;
        /// <summary>
        /// 【必须】完成解析通知
        /// </summary>
        public event HttpServerEvent.OnMessageCompleteEventHandler OnMessageComplete;
        /// <summary>
        /// 【必须】解析错误通知
        /// </summary>
        public event HttpServerEvent.OnParseErrorEventHandler OnParseError;


        /// <summary>
        /// 创建socket监听&服务组件
        /// </summary>
        /// <returns></returns>
        protected override bool CreateListener()
        {
            if (IsCreate == true || pListener != IntPtr.Zero || pServer != IntPtr.Zero)
            {
                return false;
            }

            pListener = HttpSdk.Create_HP_HttpServerListener();
            if (pListener == IntPtr.Zero)
            {
                return false;
            }
            pServer = HttpSdk.Create_HP_HttpServer(pListener);
            if (pServer == IntPtr.Zero)
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

            if (pServer != IntPtr.Zero)
            {
                HttpSdk.Destroy_HP_HttpServer(pServer);
                pServer = IntPtr.Zero;
            }
            if (pListener != IntPtr.Zero)
            {
                HttpSdk.Destroy_HP_HttpServerListener(pListener);
                pListener = IntPtr.Zero;
            }

            IsCreate = false;
        }


        HttpSdk.OnMessageBegin _OnMessageBegin;
        HttpSdk.OnHeader _OnHeader;
        HttpSdk.OnChunkHeader _OnChunkHeader;
        HttpSdk.OnChunkComplete _OnChunkComplete;
        HttpSdk.OnUpgrade _OnUpgrade;
        HttpSdk.OnRequestLine _OnRequestLine;
        HttpSdk.OnHeadersComplete _OnHeadersComplete;
        HttpSdk.OnBody _OnBody;
        HttpSdk.OnMessageComplete _OnMessageComplete;
        HttpSdk.OnParseError _OnParseError;



        protected override void SetCallback()
        {
            // 设置tcpserver的callback
            _OnPrepareListen = new Sdk.OnPrepareListen(SDK_OnPrepareListen);
            _OnAccept = new Sdk.OnAccept(SDK_OnAccept);
            _OnSend = new Sdk.OnSend(SDK_OnSend);
            _OnReceive = new Sdk.OnReceive(SDK_OnReceive);
            _OnClose = new Sdk.OnClose(SDK_OnClose);
            _OnShutdown = new Sdk.OnShutdown(SDK_OnShutdown);
            _OnHandShake = new Sdk.OnHandShake(SDK_OnHandShake);

            HttpSdk.HP_Set_FN_HttpServer_OnPrepareListen(pListener, _OnPrepareListen);
            HttpSdk.HP_Set_FN_HttpServer_OnAccept(pListener, _OnAccept);
            HttpSdk.HP_Set_FN_HttpServer_OnSend(pListener, _OnSend);
            HttpSdk.HP_Set_FN_HttpServer_OnReceive(pListener, _OnReceive);
            HttpSdk.HP_Set_FN_HttpServer_OnClose(pListener, _OnClose);
            HttpSdk.HP_Set_FN_HttpServer_OnShutdown(pListener, _OnShutdown);
            HttpSdk.HP_Set_FN_HttpServer_OnHandShake(pListener, _OnHandShake);


            // 设置自身的callback
            _OnMessageBegin = new HttpSdk.OnMessageBegin(SDK_OnMessageBegin);
            _OnHeader = new HttpSdk.OnHeader(SDK_OnHeader);
            _OnChunkHeader = new HttpSdk.OnChunkHeader(SDK_OnChunkHeader);
            _OnChunkComplete = new HttpSdk.OnChunkComplete(SDK_OnChunkComplete);
            _OnUpgrade = new HttpSdk.OnUpgrade(SDK_OnUpgrade);
            _OnRequestLine = new HttpSdk.OnRequestLine(SDK_OnRequestLine);
            _OnHeadersComplete = new HttpSdk.OnHeadersComplete(SDK_OnHeadersComplete);
            _OnBody = new HttpSdk.OnBody(SDK_OnBody);
            _OnMessageComplete = new HttpSdk.OnMessageComplete(SDK_OnMessageComplete);
            _OnParseError = new HttpSdk.OnParseError(SDK_OnParseError);

            HttpSdk.HP_Set_FN_HttpServer_OnMessageBegin(pListener, _OnMessageBegin);
            HttpSdk.HP_Set_FN_HttpServer_OnRequestLine(pListener, _OnRequestLine);
            HttpSdk.HP_Set_FN_HttpServer_OnHeader(pListener, _OnHeader);
            HttpSdk.HP_Set_FN_HttpServer_OnHeadersComplete(pListener, _OnHeadersComplete);
            HttpSdk.HP_Set_FN_HttpServer_OnBody(pListener, _OnBody);
            HttpSdk.HP_Set_FN_HttpServer_OnChunkHeader(pListener, _OnChunkHeader);
            HttpSdk.HP_Set_FN_HttpServer_OnChunkComplete(pListener, _OnChunkComplete);
            HttpSdk.HP_Set_FN_HttpServer_OnMessageComplete(pListener, _OnMessageComplete);
            HttpSdk.HP_Set_FN_HttpServer_OnUpgrade(pListener, _OnUpgrade);
            HttpSdk.HP_Set_FN_HttpServer_OnParseError(pListener, _OnParseError);
        }

        protected virtual HttpParseResult SDK_OnParseError(IntPtr pSender, IntPtr connId, int iErrorCode, string lpszErrorDesc)
        {
            if (OnParseError != null)
            {
                return OnParseError(connId, iErrorCode, lpszErrorDesc);
            }
            return HttpParseResult.Ok;
        }

        protected virtual HttpParseResult SDK_OnMessageComplete(IntPtr pSender, IntPtr connId)
        {
            if (OnMessageComplete != null)
            {
                return OnMessageComplete(connId);
            }
            return HttpParseResult.Ok;
        }

        protected virtual HttpParseResult SDK_OnBody(IntPtr pSender, IntPtr connId, IntPtr pData, int iLength)
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

        protected virtual HttpParseResultEx SDK_OnHeadersComplete(IntPtr pSender, IntPtr connId)
        {
            if (OnHeadersComplete != null)
            {
                return OnHeadersComplete(connId);
            }
            return HttpParseResultEx.Ok;
        }

        protected virtual HttpParseResult SDK_OnRequestLine(IntPtr pSender, IntPtr connId, string lpszMethod, string lpszUrl)
        {
            if (OnRequestLine != null)
            {
                return OnRequestLine(connId, lpszMethod, lpszUrl);
            }
            return HttpParseResult.Ok;
        }

        protected virtual HttpParseResult SDK_OnUpgrade(IntPtr pSender, IntPtr connId, HttpUpgradeType enUpgradeType)
        {
            if (OnUpgrade != null)
            {
                return OnUpgrade(connId, enUpgradeType);
            }
            return HttpParseResult.Ok;
        }

        protected virtual HttpParseResult SDK_OnChunkComplete(IntPtr pSender, IntPtr connId)
        {
            if (OnChunkComplete != null)
            {
                return OnChunkComplete(connId);
            }
            return HttpParseResult.Ok;
        }

        protected virtual HttpParseResult SDK_OnChunkHeader(IntPtr pSender, IntPtr connId, int iLength)
        {
            if (OnChunkHeader != null)
            {
                return OnChunkHeader(connId, iLength);
            }
            return HttpParseResult.Ok;
        }

        protected virtual HttpParseResult SDK_OnHeader(IntPtr pSender, IntPtr connId, string lpszName, string lpszValue)
        {
            if (OnHeader != null)
            {
                return OnHeader(connId, lpszName, lpszValue);
            }
            return HttpParseResult.Ok;
        }

        protected virtual HttpParseResult SDK_OnMessageBegin(IntPtr pSender, IntPtr connId)
        {
            if (OnMessageBegin != null)
            {
                return OnMessageBegin(connId);
            }
            return HttpParseResult.Ok;
        }

        /**************************************************************************/
        /***************************** Server 操作方法 *****************************/
        
        /// <summary>
        /// 名称：回复请求
        /// 描述：向客户端回复 HTTP 请求
        /// </summary>
        /// <param name="connId">连接 ID</param>
        /// <param name="httpStatusCode">HTTP 状态码</param>
        /// <param name="desc">HTTP 状态描述</param>
        /// <param name="headers">回复请求头</param>
        /// <param name="body">回复请求体</param>
        /// <returns></returns>
        public bool SendResponse(IntPtr connId, HttpStatusCode httpStatusCode, string desc, THeader[] headers, byte[] body, int bodyLength)
        {
            int headersLength = headers == null ? 0 : headers.Length;
            return HttpSdk.HP_HttpServer_SendResponse(pServer, connId, (ushort)httpStatusCode, desc, headers, headersLength, body, bodyLength);
        }
        

        /// <summary>
        /// 名称：发送本地文件
        /// 描述：向指定连接发送 4096 KB 以下的小文件
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="fileName"></param>
        /// <param name="httpStatusCode"></param>
        /// <param name="desc"></param>
        /// <param name="headers"></param>
        /// <returns></returns>
        public bool SendLocalFile(IntPtr connId, string fileName, HttpStatusCode httpStatusCode, string desc, THeader[] headers)
        {
            int headersLength = headers == null ? 0 : headers.Length;
            return HttpSdk.HP_HttpServer_SendLocalFile(pServer, connId, fileName, (ushort)httpStatusCode, desc, headers, headersLength);
        }

        /// <summary>
        /// 名称：释放连接
        /// 描述：把连接放入释放队列，等待某个时间（通过 SetReleaseDelay() 设置）关闭连接
        /// </summary>
        /// <param name="connId">连接 ID</param>
        /// <returns></returns>
        public bool Release(IntPtr connId)
        {
            return HttpSdk.HP_HttpServer_Release(pServer, connId);
        }


        /******************************************************************************/
        /***************************** Server 属性访问方法 *****************************/

        /// <summary>
        /// 获取或设置连接释放延时（默认：3000 毫秒）
        /// </summary>
        public uint ReleaseDelay
        {
            get
            {
                return HttpSdk.HP_HttpServer_GetReleaseDelay(pServer);
            }
            set
            {
                HttpSdk.HP_HttpServer_SetReleaseDelay(pServer, value);
            }
        }


        /// <summary>
        /// 获取请求行 URL 域掩码（URL 域参考：HttpUrlField）
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public HttpUrlField GetUrlFieldSet(IntPtr connId)
        {
            return (HttpUrlField)HttpSdk.HP_HttpServer_GetUrlFieldSet(pServer, connId);
        }

        /// <summary>
        /// 获取某个 URL 域值
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="enField"></param>
        /// <returns></returns>
        public string GetUrlField(IntPtr connId, HttpUrlField enField)
        {
            IntPtr ptr = HttpSdk.HP_HttpServer_GetUrlField(pServer, connId, enField);
            return Common.PtrToAnsiString(ptr);
        }

        /// <summary>
        /// 获取请求方法
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public string GetMethod(IntPtr connId)
        {
            IntPtr ptr = HttpSdk.HP_HttpServer_GetMethod(pServer, connId);
            return Common.PtrToAnsiString(ptr);
        }

        /// <summary>
        /// 获取或设置本地协议版本
        /// </summary>
        public HttpVersion LocalVersion
        {
            get
            {
                return HttpSdk.HP_HttpServer_GetLocalVersion(pServer);
            }
            set
            {
                HttpSdk.HP_HttpServer_SetLocalVersion(pServer, value);
            }
        }


        /// <summary>
        /// 检查是否升级协议
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public bool IsUpgrade(IntPtr connId)
        {
            return HttpSdk.HP_HttpServer_IsUpgrade(pServer, connId);
        }

        /// <summary>
        /// 检查是否有 Keep-Alive 标识
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public bool IsKeepAlive(IntPtr connId)
        {
            return HttpSdk.HP_HttpServer_IsKeepAlive(pServer, connId);
        }

        /// <summary>
        /// 获取协议版本
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public HttpVersion GetVersion(IntPtr connId)
        {
            return (HttpVersion)HttpSdk.HP_HttpServer_GetVersion(pServer, connId);
        }

        /// <summary>
        /// 获取主机
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public string GetHost(IntPtr connId)
        {
            IntPtr ptr = HttpSdk.HP_HttpServer_GetHost(pServer, connId);
            return Common.PtrToAnsiString(ptr);
        }

        /// <summary>
        /// 获取内容长度
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public long GetContentLength(IntPtr connId)
        {
            return HttpSdk.HP_HttpServer_GetContentLength(pServer, connId);
        }


        /// <summary>
        /// 获取内容类型
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public string GetContentType(IntPtr connId)
        {
            IntPtr ptr = HttpSdk.HP_HttpServer_GetContentType(pServer, connId);
            return Common.PtrToAnsiString(ptr);
        }

        /// <summary>
        /// 获取内容类型
        /// </summary>
        /// <returns></returns>
        public string GetContentEncoding(IntPtr connId)
        {
            IntPtr ptr = HttpSdk.HP_HttpServer_GetContentEncoding(pServer, connId);
            return Common.PtrToAnsiString(ptr);
        }

        /// <summary>
        /// 获取传输编码
        /// </summary>
        /// <returns></returns>
        public string GetTransferEncoding(IntPtr connId)
        {
            IntPtr ptr = HttpSdk.HP_HttpServer_GetTransferEncoding(pServer, connId);
            return Common.PtrToAnsiString(ptr);
        }

        /// <summary>
        /// 获取协议升级类型
        /// </summary>
        /// <param name="connId"></param>
        /// <returns></returns>
        public HttpUpgradeType GetUpgradeType(IntPtr connId)
        {
            return HttpSdk.HP_HttpServer_GetUpgradeType(pServer, connId);
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
            error.ErrorCode = HttpSdk.HP_HttpServer_GetParseErrorCode(pServer, connId, ref ptr);
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
            if (HttpSdk.HP_HttpServer_GetHeader(pServer, connId, key, ref ptr))
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
                HttpSdk.HP_HttpServer_GetHeaders(pServer, connId, key, arr, ref count);
                if (count > 0)
                {
                    arr = new IntPtr[count];
                    if (HttpSdk.HP_HttpServer_GetHeaders(pServer, connId, key, arr, ref count) && count > 0)
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
                HttpSdk.HP_HttpServer_GetAllHeaders(pServer, connId, arrPtr, ref count);
                if (count > 0)
                {
                    var ptrArr = new THeaderForIntPtr[count];
                    arrPtr = Marshal.UnsafeAddrOfPinnedArrayElement(ptrArr, 0);
                    if (HttpSdk.HP_HttpServer_GetAllHeaders(pServer, connId, arrPtr, ref count))
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
                HttpSdk.HP_HttpServer_GetAllHeaderNames(pServer, connId, arr, ref count);
                if (count > 0)
                {
                    arr = new IntPtr[count];
                    if (HttpSdk.HP_HttpServer_GetAllHeaderNames(pServer, connId, arr, ref count) && count > 0)
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
            if (HttpSdk.HP_HttpServer_GetCookie(pServer, connId, key, ref ptr))
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
                HttpSdk.HP_HttpServer_GetAllCookies(pServer, connId, arrPtr, ref count);
                if (count > 0)
                {
                    var ptrArr = new THeaderForIntPtr[count];
                    arrPtr = Marshal.UnsafeAddrOfPinnedArrayElement(ptrArr, 0);
                    if (HttpSdk.HP_HttpServer_GetAllCookies(pServer, connId, arrPtr, ref count))
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
