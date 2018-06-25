using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HPSocketCS;

namespace HttpServerDemo
{
    public class MyHttpsServer
    {
        HttpsServer httpsServer = null;
        public void Run(string bindAddress, ushort port)
        {
            httpsServer = new HttpsServer(SSLVerifyMode.None, "./ssl-cert/server.cer", "./ssl-cert/server.key", "123456", "./ssl-cert/ca.crt", null);
            // 需要自己初始化ssl环境
            httpsServer.Initialize();

            httpsServer.IpAddress = bindAddress;
            httpsServer.Port = port;

            // 设置http服务器事件
            httpsServer.OnMessageBegin += new HttpEvent.OnMessageBeginEventHandler(OnMessageBegin);
            httpsServer.OnHeader += new HttpEvent.OnHeaderEventHandler(OnHeader);
            httpsServer.OnChunkHeader += new HttpEvent.OnChunkHeaderEventHandler(OnChunkHeader);
            httpsServer.OnChunkComplete += new HttpEvent.OnChunkCompleteEventHandler(OnChunkComplete);
            httpsServer.OnUpgrade += new HttpEvent.OnUpgradeEventHandler(OnUpgrade);
            httpsServer.OnRequestLine += new HttpServerEvent.OnRequestLineEventHandler(OnRequestLine);
            httpsServer.OnHeadersComplete += new HttpEvent.OnHeadersCompleteEventHandler(OnHeadersComplete);
            httpsServer.OnPointerDataBody += new HttpEvent.OnPointerDataBodyEventHandler(OnPointerDataBody);
            httpsServer.OnMessageComplete += new HttpEvent.OnMessageCompleteEventHandler(OnMessageComplete);
            httpsServer.OnParseError += new HttpEvent.OnParseErrorEventHandler(OnParseError);



            if (!httpsServer.Start())
            {
                throw new MyException(string.Format("启动失败,错误码:{0},错误信息:{1}", httpsServer.ErrorCode, httpsServer.ErrorMessage));
            }
        }

        public void Exit()
        {
            httpsServer.Stop();
            // 需要自己反初始化ssl环境
            httpsServer.Uninitialize();
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

            HttpUrlField urlFieldSet = httpsServer.GetUrlFieldSet(connId);

            result.AppendFormat("{0}[URL Fields]\r\n", sep1);

            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "SCHEMA", httpsServer.GetUrlField(connId, HttpUrlField.Schema));
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "HOST", httpsServer.GetUrlField(connId, HttpUrlField.Host));
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "PORT", httpsServer.GetUrlField(connId, HttpUrlField.Port));
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "PATH", httpsServer.GetUrlField(connId, HttpUrlField.Path));
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "QUERY", httpsServer.GetUrlField(connId, HttpUrlField.QueryString));
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "FRAGMENT", httpsServer.GetUrlField(connId, HttpUrlField.Fragment));
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "USERINFO", httpsServer.GetUrlField(connId, HttpUrlField.UserInfo));


            var headers = httpsServer.GetAllHeaders(connId);
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

            var cookies = httpsServer.GetAllCookies(connId);
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
            var httpVersion = httpsServer.GetVersion(connId);
            var upgType = httpsServer.GetUpgradeType(connId);

            result.AppendFormat("{0}[Basic Info]\r\n", sep1);
            result.AppendFormat("{0}{1}: {2}.{3}\r\n", sep2, "Version", httpVersion.LOBYTE(), httpVersion.HIBYTE());
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "Method", httpsServer.GetMethod(connId));
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "IsUpgrade", upgType != HttpUpgradeType.None);
            if (upgType != HttpUpgradeType.None)
            {
                result.AppendFormat("{0}{1}: {2}\r\n", sep2, "UpgradeType", upgType);
            }

            var keepAlive = httpsServer.IsKeepAlive(connId);
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "IsKeepAlive", keepAlive);
            if (withContentLength)
            {
                result.AppendFormat("{0}{1}: {2}\r\n", sep2, "ContentLength", httpsServer.GetContentLength(connId));

            }
            result.AppendFormat("{0}{1}: {2}\r\n", sep2, "ContentType", httpsServer.GetContentType(connId));

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

            if (httpsServer.IsUpgrade(connId))
            {
                return HttpParseResult.Ok;
            }

            var arr = httpsServer.GetAllCookies(connId);

            bool isSkipBody = httpsServer.GetMethod(connId) == "HEAD";
            
            uint sequence = 0;
            if (uint.TryParse(httpsServer.GetCookie(connId, "requestSequence"), out sequence))
            {
                sequence++;
            }

            string cookie = string.Format("requestSequence={0}; path=/", sequence);

            string body = GetHeaderSummary(connId, "    ", 0, false);
            if (isSkipBody)
            {
                body = "";
            }

            // 这里的utf8 对应Content-Type里的charset, 能保证中文不乱码
            var bytes = Encoding.UTF8.GetBytes(body);
            THeader[] headers =
            {
                // 这里请参照http协议
                new THeader() { Name= "Content-Type", Value = "text/plain;charset=utf-8" },
                new THeader() { Name= "Content-Length", Value = bytes.Length.ToString() },
                new THeader() { Name= "Set-Cookie", Value = cookie},
            };
            
            // 回复请求
            httpsServer.SendResponse(connId, HttpStatusCode.Ok, "Http Server OK", headers, bytes, bytes.Length);

            // 不是长连接就断开
            if (httpsServer.IsKeepAlive(connId) == false)
            {
                httpsServer.Release(connId);
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
        
    }
}
