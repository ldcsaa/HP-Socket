using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HPSocketCS;

namespace HttpService
{
    public class HttpService
    {
        HttpServer httpServer = null;
        public void Run(string bindAddress, ushort port)
        {
            httpServer = new HttpServer();
            httpServer.IpAddress = bindAddress;
            httpServer.Port = port;

            // 设置http服务器事件
            httpServer.OnPrepareListen += new TcpServerEvent.OnPrepareListenEventHandler(OnPrepareListen);
            httpServer.OnAccept += new TcpServerEvent.OnAcceptEventHandler(OnAccept);
            httpServer.OnSend += new TcpServerEvent.OnSendEventHandler(OnSend);
            httpServer.OnPointerDataReceive += new TcpServerEvent.OnPointerDataReceiveEventHandler(OnPointerDataReceive);
            httpServer.OnClose += new TcpServerEvent.OnCloseEventHandler(OnClose);
            httpServer.OnShutdown += new TcpServerEvent.OnShutdownEventHandler(OnShutdown);
            httpServer.OnHandShake += new TcpServerEvent.OnHandShakeEventHandler(OnHandShake);


            httpServer.OnMessageBegin += new HttpEvent.OnMessageBeginEventHandler(OnMessageBegin);
            httpServer.OnHeader += new HttpEvent.OnHeaderEventHandler(OnHeader);
            httpServer.OnChunkHeader += new HttpEvent.OnChunkHeaderEventHandler(OnChunkHeader);
            httpServer.OnChunkComplete += new HttpEvent.OnChunkCompleteEventHandler(OnChunkComplete);
            httpServer.OnUpgrade += new HttpEvent.OnUpgradeEventHandler(OnUpgrade);
            httpServer.OnRequestLine += new HttpServerEvent.OnRequestLineEventHandler(OnRequestLine);
            httpServer.OnHeadersComplete += new HttpEvent.OnHeadersCompleteEventHandler(OnHeadersComplete);
            httpServer.OnPointerDataBody += new HttpEvent.OnPointerDataBodyEventHandler(OnPointerDataBody);
            httpServer.OnMessageComplete += new HttpEvent.OnMessageCompleteEventHandler(OnMessageComplete);
            httpServer.OnParseError += new HttpEvent.OnParseErrorEventHandler(OnParseError);

            if (!httpServer.Start())
            {
                throw new MyException(string.Format("启动失败,错误码:{0},错误信息:{1}", httpServer.ErrorCode, httpServer.ErrorMessage));
            }

        }


        private string GetHeaderSummary(IntPtr connId, string sep, int sepCount, bool withContentLength)
        {
            
            string sep1 = "";

            for (int i = 0; i < sepCount; i++)
            {
                sep1 += sep;
            }


            string sep2 = sep1;
            sep2 += sep;


            StringBuilder result = new StringBuilder();

            HttpUrlField urlFieldSet = httpServer.GetUrlFieldSet(connId);

            result.AppendFormat("{0}[URL Fields]\r\n", sep1);

            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "SCHEMA", httpServer.GetUrlField(connId, HttpUrlField.Schema));
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "HOST", httpServer.GetUrlField(connId, HttpUrlField.Host));
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "PORT", httpServer.GetUrlField(connId, HttpUrlField.Port));
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "PATH", httpServer.GetUrlField(connId, HttpUrlField.Path));
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "QUERY", httpServer.GetUrlField(connId, HttpUrlField.QueryString));
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "FRAGMENT", httpServer.GetUrlField(connId, HttpUrlField.Fragment));
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "USERINFO", httpServer.GetUrlField(connId, HttpUrlField.UserInfo));


            var headers = httpServer.GetAllHeaders(connId);
            result.AppendFormat("{0}[Request Headers]\r\n", sep1);

            if (headers.Count == 0)
            {
                result.AppendFormat("{0}(no header)\r\n", sep2);
            }
            else
            {
                foreach (var item in headers)
                {
                    result.AppendFormat("{0}{1}: {2}\r\n", sep2, item.Name, item.Value);
                }
            }

            var cookies = httpServer.GetAllCookies(connId);
            result.AppendFormat("{0}[Cookies]\r\n", sep1);

            if (cookies.Count == 0)
            {
                result.AppendFormat("{0}(no cookie)\r\n", sep2);
            }
            else
            {
                foreach (var item in cookies)
                {
                    result.AppendFormat("{0}{1}: {2}\r\n", sep2, item.Name, item.Value);
                }
            }
            var httpVersion = httpServer.GetVersion(connId);
            var upgType = httpServer.GetUpgradeType(connId);

            result.AppendFormat("{0}[Basic Info]\r\n", sep1);
            result.AppendFormat("{0}{1}: {2}.{3}\r\n", sep2, "Version", httpVersion.LOBYTE(), httpVersion.HIBYTE());
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "Method", httpServer.GetMethod(connId));
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "IsUpgrade", upgType != HttpUpgradeType.None);
            if (upgType != HttpUpgradeType.None)
            {
                result.AppendFormat("{0}{1}: {2}\r\n", sep2, "UpgradeType", upgType);
            }

            var keepAlive = httpServer.IsKeepAlive(connId);
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "IsKeepAlive", keepAlive);
            if (withContentLength)
            {
                result.AppendFormat("{0}{1}: {2}\r\n", sep2, "ContentLength", httpServer.GetContentLength(connId));

            }
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "ContentType", httpServer.GetContentType(connId));

            return result.ToString();
        }


        private HttpParseResult OnMessageBegin(IntPtr connId)
        {
            Console.WriteLine("OnMessageBegin({0})", connId);
            return HttpParseResult.Ok;
        }

        private HttpParseResult OnRequestLine(IntPtr connId, string method, string url)
        {
            Console.WriteLine("OnRequestLine({0}, {1}, {2})", connId, method, url);
            if (url == "/favicon.ico")
            {
                //触发OnParseError(connId, 2, "the on_url callback failed")
                return HttpParseResult.Error;
            }
            return HttpParseResult.Ok;
        }

        private HttpParseResult OnHeader(IntPtr connId, string name, string value)
        {
            Console.WriteLine("OnHeader({0}, {1}, {2})", connId, name, value);
            return HttpParseResult.Ok;
        }

        private HttpParseResultEx OnHeadersComplete(IntPtr connId)
        {
            string summary = GetHeaderSummary(connId, "    ", 0, true);
            Console.WriteLine("OnHeadersComplete({0})\r\n====================\r\n{1}\r\n====================", connId, summary);
            return HttpParseResultEx.Ok;
        }

        private HttpParseResult OnPointerDataBody(IntPtr connId, IntPtr pData, int length)
        {
            Console.WriteLine("OnBody({0}, {1})", connId, length);
            return HttpParseResult.Ok;
        }

        private HttpParseResult OnChunkHeader(IntPtr connId, int length)
        {
            Console.WriteLine("OnChunkHeader({0}, {1})", connId, length);
            return HttpParseResult.Ok;
        }

        private HttpParseResult OnChunkComplete(IntPtr connId)
        {
            Console.WriteLine("OnChunkComplete({0})", connId);
            return HttpParseResult.Ok;
        }

        private HttpParseResult OnMessageComplete(IntPtr connId)
        {

            if (httpServer.IsUpgrade(connId))
            {
                return HttpParseResult.Ok;
            }

            var arr = httpServer.GetAllCookies(connId);

            bool isSkipBody = httpServer.GetMethod(connId) == "HEAD";
            uint sequence = 0;
            if (uint.TryParse(httpServer.GetCookie(connId, "requestSequence"), out sequence))
            {
                sequence++;
            }

            string cookie = string.Format("requestSequence={0}; path=/", sequence);

            string body = GetHeaderSummary(connId, "    ", 0, false);

            THeader[] headers =
            {
                new THeader() { Name= "Content-Type", Value = "text/plain" },
                new THeader() { Name= "Content-Length", Value = body.Length.ToString() },
                new THeader() { Name= "Set-Cookie", Value = cookie},
            };

            if (isSkipBody)
            {
                body = "";
            }

            // 回复请求
            httpServer.SendResponse(connId, HttpStatusCode.Ok, "Http Server OK", headers, body);

            // 不是长连接就断开
            if (httpServer.IsKeepAlive(connId) == false)
            {
                httpServer.Release(connId);
            }

            return HttpParseResult.Ok;
        }

        private HttpParseResult OnUpgrade(IntPtr connId, HttpUpgradeType upgradeType)
        {
            Console.WriteLine("OnUpgrade({0}, {1})", connId, upgradeType);
            return HttpParseResult.Ok;
        }
        private HttpParseResult OnParseError(IntPtr connId, int errorCode, string errorDesc)
        {
            Console.WriteLine("OnParseError({0}, {1}, {2})", connId, errorCode, errorDesc);
            return HttpParseResult.Ok;
        }

        /**************************************************************************/

        private HandleResult OnPrepareListen(IntPtr soListen)
        {
            return HandleResult.Ok;
        }

        private HandleResult OnAccept(IntPtr connId, IntPtr pClient)
        {
            return HandleResult.Ok;
        }

        private HandleResult OnHandShake(IntPtr connId)
        {
            return HandleResult.Ok;
        }

        private HandleResult OnSend(IntPtr connId, byte[] bytes)
        {
            return HandleResult.Ok;
        }

        private HandleResult OnPointerDataReceive(IntPtr connId, IntPtr pData, int length)
        {
            return HandleResult.Ok;
        }

        private HandleResult OnClose(IntPtr connId, SocketOperation enOperation, int errorCode)
        {
            return HandleResult.Ok;
        }

        private HandleResult OnShutdown()
        {
            return HandleResult.Ok;
        }
    }
}
