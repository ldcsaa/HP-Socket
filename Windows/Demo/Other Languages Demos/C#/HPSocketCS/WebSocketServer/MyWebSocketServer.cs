using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using HPSocketCS;

namespace WebSocketDemo
{
    public class MyWebSocketServer
    {
        WebSocketServer wsServer = null;
        public void Run(string bindAddress, ushort port)
        {
            wsServer = new WebSocketServer();
            wsServer.IpAddress = bindAddress;
            wsServer.Port = port;

            // 指针形式的事件
            // wsServer.OnPointerWSMessageBody += new WebSocketEvent.OnPointerWSMessageBodyEventHandler(OnPointerDataBody);
            wsServer.OnWSMessageBody += new WebSocketEvent.OnWSMessageBodyEventHandler(OnWSMessageBody);
            wsServer.OnWSMessageComplete += new WebSocketEvent.OnWSMessageCompleteEventHandler(OnWSMessageComplete);
            wsServer.OnWSMessageHeader += new WebSocketEvent.OnWSMessageHeaderEventHandler(OnWSMessageHeader);
            
            if (!wsServer.Start())
            {
                throw new MyException(string.Format("启动失败,错误码:{0},错误信息:{1}", wsServer.ErrorCode, wsServer.ErrorMessage));
            }
        }

        HandleResult OnPointerDataBody(IntPtr connId, IntPtr data, int length)
        {
            // 指针形式的事件
            return HandleResult.Ok;
        }

        HandleResult OnWSMessageBody(IntPtr connId, byte[] data)
        {
            // 如果是文本,应该用utf8编码
            string str = Encoding.UTF8.GetString(data);
            Console.WriteLine("OnWSMessageBody() -> {0}", str);

            // 获取客户端的state
            var state = wsServer.GetWSMessageState(connId);
            if (state != null)
            {
                // 原样返回给客户端
                wsServer.SendWSMessage(connId, state, data);
            }

            return HandleResult.Ok;
        }

        HandleResult OnWSMessageComplete(IntPtr connId)
        {
            return HandleResult.Ok;
        }

        HandleResult OnWSMessageHeader(IntPtr connId, bool final, byte reserved, byte operationCode, byte[] mask, ulong bodyLength)
        {
            var state = wsServer.GetWSMessageState(connId);
            //WSOpcode.Close为客户端主动断开连接
            if (state != null && state.OperationCode == WSOpcode.Close)
            {
                wsServer.Disconnect(connId);
            }
            return HandleResult.Ok;
        }
    }
}
