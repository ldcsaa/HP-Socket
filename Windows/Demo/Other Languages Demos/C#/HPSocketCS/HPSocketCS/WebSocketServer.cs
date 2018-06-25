using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Security.Cryptography;

namespace HPSocketCS
{
    public class WebSocketServer : HttpServer
    {
        public event WebSocketEvent.OnWSMessageHeaderEventHandler OnWSMessageHeader;
        public event WebSocketEvent.OnWSMessageBodyEventHandler OnWSMessageBody;
        public event WebSocketEvent.OnPointerWSMessageBodyEventHandler OnPointerWSMessageBody;
        public event WebSocketEvent.OnWSMessageCompleteEventHandler OnWSMessageComplete;

        HttpSdk.OnWSMessageHeader _OnWSMessageHeader;
        HttpSdk.OnWSMessageBody _OnWSMessageBody;
        HttpSdk.OnWSMessageComplete _OnWSMessageComplete;

        protected override void SetCallback()
        {
            base.SetCallback();

            // 设置websocket的callback
            _OnWSMessageHeader = new HttpSdk.OnWSMessageHeader(SDK_OnWSMessageHeader);
            _OnWSMessageBody = new HttpSdk.OnWSMessageBody(SDK_OnWSMessageBody);
            _OnWSMessageComplete = new HttpSdk.OnWSMessageComplete(SDK_OnWSMessageComplete);

            HttpSdk.HP_Set_FN_HttpServer_OnWSMessageHeader(pListener, _OnWSMessageHeader);
            HttpSdk.HP_Set_FN_HttpServer_OnWSMessageBody(pListener, _OnWSMessageBody);
            HttpSdk.HP_Set_FN_HttpServer_OnWSMessageComplete(pListener, _OnWSMessageComplete);
            
        }

        protected virtual HandleResult SDK_OnWSMessageHeader(IntPtr pSender, IntPtr dwConnID, bool bFinal, byte iReserved, byte iOperationCode, byte[] lpszMask, ulong ullBodyLen)
        {
            if (OnWSMessageHeader != null)
            {
                return OnWSMessageHeader(dwConnID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen);
            }
            return HandleResult.Ok;
        }

        protected virtual HandleResult SDK_OnWSMessageBody(IntPtr pSender, IntPtr dwConnID, IntPtr pData, int length)
        {
            if (OnPointerWSMessageBody != null)
            {
                return OnPointerWSMessageBody(dwConnID, pData, length);
            }
            else if (OnWSMessageBody != null)
            {
                byte[] bytes = new byte[length];
                Marshal.Copy(pData, bytes, 0, length);
                return OnWSMessageBody(dwConnID, bytes);
            }
            return HandleResult.Ok;
        }

        protected virtual HandleResult SDK_OnWSMessageComplete(IntPtr pSender, IntPtr connId)
        {
            if (OnWSMessageComplete != null)
            {
                return OnWSMessageComplete(connId);
            }
            return HandleResult.Ok;
        }


        protected override HttpParseResult SDK_OnUpgrade(IntPtr pSender, IntPtr connId, HttpUpgradeType upgradeType)
        {
            if (upgradeType == HttpUpgradeType.HttpTunnel)
            {
                SendResponse(connId, HttpStatusCode.Ok, "Connection Established", null, null, 0);
            }
            else if (upgradeType == HttpUpgradeType.WebSocket)
            {
                THeader[] headers =
                {
                    new THeader() { Name= "Connection", Value = "Upgrade" },
                    new THeader() { Name= "Upgrade", Value = "WebSocket" },
                    new THeader(),
                    new THeader(),
                };

                var keyName = "Sec-WebSocket-Key";
                var secWebSocketKey = GetHeader(connId, keyName);
                if (string.IsNullOrEmpty(secWebSocketKey))
                {
                    return HttpParseResult.Error;
                }


                SHA1 sha1 = new SHA1CryptoServiceProvider();
                byte[] bytes_sha1_in = Encoding.UTF8.GetBytes(secWebSocketKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
                byte[] bytes_sha1_out = sha1.ComputeHash(bytes_sha1_in);
                string str_sha1_out = Convert.ToBase64String(bytes_sha1_out);

                headers[2].Name = "Sec-WebSocket-Accept";
                headers[2].Value = str_sha1_out;


                var secWebSocketProtocol = GetHeader(connId, "Sec-WebSocket-Protocol");
                if (!string.IsNullOrEmpty(secWebSocketProtocol))
                {
                    var arr = secWebSocketProtocol.Split(new[] { ',', ' ' });
                    if (arr.Length > 0)
                    {
                        headers[3].Name = "Sec-WebSocket-Protocol";
                        headers[3].Value = arr[0];
                    }

                }

                SendResponse(connId, HttpStatusCode.SwitchingProtocols, null, headers, null, 0);
            }
            return HttpParseResult.Ok;
        }

        public bool SendMessage(IntPtr connId, WSMessageState state, IntPtr pData, int length)
        {
            return HttpSdk.HP_HttpServer_SendWSMessage(pServer, connId, state.Final, state.Reserved, state.OperationCode, state.Mask, pData, length, 0);
        }

        public bool SendWSMessage(IntPtr connId, WSMessageState state, byte[] data)
        {
            state.Mask = null;
            int bodyLen = data == null ? 0 : data.Length;
            return HttpSdk.HP_HttpServer_SendWSMessage(pServer, connId, state.Final, state.Reserved, state.OperationCode, state.Mask, data, bodyLen, 0);
        }

        public WSMessageState GetWSMessageState(IntPtr connId)
        {
            bool final = false;
            WSReserved reserved =  WSReserved.Off;
            WSOpcode operationCode = WSOpcode.Cont;
            IntPtr maskPtr = IntPtr.Zero;
            ulong bodyLen = 0;
            ulong bodyRemain = 0;
            bool ret = HttpSdk.HP_HttpServer_GetWSMessageState(pServer, connId, ref final, ref reserved, ref operationCode, ref maskPtr, ref bodyLen, ref bodyRemain);
            if (ret)
            {
                WSMessageState state = new WSMessageState()
                {
                    Final = final,
                    Reserved = reserved,
                    OperationCode = operationCode,

                    BodyLen = bodyLen,
                    BodyRemain = bodyRemain,
                };

                if (maskPtr != IntPtr.Zero)
                {
                    state.Mask = new byte[4];
                    Marshal.Copy(maskPtr, state.Mask, 0, state.Mask.Length);
                }

                return state;
            }

            return null;
        }

    }

}
