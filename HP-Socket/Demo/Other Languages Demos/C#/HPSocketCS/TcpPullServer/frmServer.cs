using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using HPSocketCS;

namespace TcpPullServer
{
    public enum AppState
    {
        Starting, Started, Stoping, Stoped, Error
    }

    public partial class frmServer : Form
    {
        private AppState appState = AppState.Stoped;

        private delegate void ShowMsg(string msg);
        private ShowMsg AddMsgDelegate;

        HPSocketCS.TcpPullServer<ClientInfo> server = new HPSocketCS.TcpPullServer<ClientInfo>();
        
        // 包头大小
        int pkgHeaderSize = Marshal.SizeOf(new PkgHeader());

        private string title = "Echo-TcpPullServer [ 'C' - clear list box ]";
        public frmServer()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            try
            {

                this.Text = title;
                // 本机测试没必要改地址,有需求请注释或删除
                this.txtIpAddress.ReadOnly = true;

                // 加个委托显示msg,因为on系列都是在工作线程中调用的,ui不允许直接操作
                AddMsgDelegate = new ShowMsg(AddMsg);

                // 设置服务器事件
                server.OnPrepareListen += new TcpServerEvent.OnPrepareListenEventHandler(OnPrepareListen);
                server.OnAccept += new TcpServerEvent.OnAcceptEventHandler(OnAccept);
                server.OnSend += new TcpServerEvent.OnSendEventHandler(OnSend);
                server.OnReceive += new TcpPullServerEvent.OnReceiveEventHandler(OnReceive);
                server.OnClose += new TcpServerEvent.OnCloseEventHandler(OnClose);
                server.OnShutdown += new TcpServerEvent.OnShutdownEventHandler(OnShutdown);

                SetAppState(AppState.Stoped);
            }
            catch (Exception ex)
            {
                SetAppState(AppState.Error);
                AddMsg(ex.Message);
            }
        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            try
            {
                String ip = this.txtIpAddress.Text.Trim();
                ushort port = ushort.Parse(this.txtPort.Text.Trim());

                // 写在这个位置是上面可能会异常
                SetAppState(AppState.Starting);

                server.IpAddress = ip;
                server.Port = port;
                // 启动服务
                if (server.Start())
                {
                    this.Text = string.Format("{2} - ({0}:{1})", ip, port, title);
                    SetAppState(AppState.Started);
                    AddMsg(string.Format("$Server Start OK -> ({0}:{1})", ip, port));
                }
                else
                {
                    SetAppState(AppState.Stoped);
                    throw new Exception(string.Format("$Server Start Error -> {0}({1})", server.ErrorMessage, server.ErrorCode));
                }
            }
            catch (Exception ex)
            {
                AddMsg(ex.Message);
            }
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            SetAppState(AppState.Stoping);

            // 停止服务
            AddMsg("$Server Stop");
            if (server.Stop())
            {
                this.Text = title;
                SetAppState(AppState.Stoped);
            }
            else
            {
                AddMsg(string.Format("$Stop Error -> {0}({1})", server.ErrorMessage, server.ErrorCode));
            }
        }

        private void btnDisconn_Click(object sender, EventArgs e)
        {
            try
            {
                IntPtr connId = (IntPtr)Convert.ToUInt32(this.txtDisConn.Text.Trim());

                // 断开指定客户
                if (server.Disconnect(connId, true))
                {
                    AddMsg(string.Format("$({0}) Disconnect OK", connId));
                }
                else
                {
                    throw new Exception(string.Format("Disconnect({0}) Error", connId));
                }
            }
            catch (Exception ex)
            {
                AddMsg(ex.Message);
            }
        }


        HandleResult OnPrepareListen(IntPtr soListen)
        {
            // 监听事件到达了,一般没什么用吧?

            return HandleResult.Ok;
        }

        HandleResult OnAccept(IntPtr connId, IntPtr pClient)
        {
            // 客户进入了


            // 获取客户端ip和端口
            string ip = string.Empty;
            ushort port = 0;
            if (server.GetRemoteAddress(connId, ref ip, ref port))
            {
                AddMsg(string.Format(" > [{0},OnAccept] -> PASS({1}:{2})", connId, ip.ToString(), port));
            }
            else
            {
                AddMsg(string.Format(" > [{0},OnAccept] -> Server_GetClientAddress() Error", connId));
            }


            // 设置附加数据
            ClientInfo clientInfo = new ClientInfo();
            clientInfo.ConnId = connId;
            clientInfo.IpAddress = ip;
            clientInfo.Port = port;
            clientInfo.PkgInfo = new PkgInfo()
            {
                IsHeader = true,
                Length = pkgHeaderSize,
            };
            if (server.SetExtra(connId, clientInfo) == false)
            {
                AddMsg(string.Format(" > [{0},OnAccept] -> SetConnectionExtra fail", connId));
            }

            return HandleResult.Ok;
        }

        HandleResult OnSend(IntPtr connId, byte[] bytes)
        {
            // 服务器发数据了


            AddMsg(string.Format(" > [{0},OnSend] -> ({1} bytes)", connId, bytes.Length));

            return HandleResult.Ok;
        }

        HandleResult OnReceive(IntPtr connId, int length)
        {
            // 数据到达了
            // clientInfo 就是accept里传入的附加数据了
            var clientInfo = server.GetExtra(connId);
            if (clientInfo == null)
            {
                return HandleResult.Error;
            }

            PkgInfo pkgInfo = clientInfo.PkgInfo;


            // 需要长度
            int required = pkgInfo.Length;

            // 剩余大小
            int remain = length;

            while (remain >= required)
            {
                IntPtr bufferPtr = IntPtr.Zero;
                try
                {
                    remain -= required;
                    bufferPtr = Marshal.AllocHGlobal(required); ;
                    if (server.Fetch(connId, bufferPtr, required) == FetchResult.Ok)
                    {
                        if (pkgInfo.IsHeader == true)
                        {
                            PkgHeader header = (PkgHeader)Marshal.PtrToStructure(bufferPtr, typeof(PkgHeader));

                            // 调试信息
                            Console.WriteLine("[Server] head -> Id: {0}, BodySize: {1}\r\n", header.Id, header.BodySize);

                            required = header.BodySize;
                        }
                        else
                        {
                            // 调试信息
                            string recvString = Marshal.PtrToStringAnsi(bufferPtr, required);
                            Console.WriteLine("[Server] body -> text: {0}\r\n", recvString);

                            // intptr转string
                            //Marshal.PtrToStringAnsi(bufferPtr, required);

                            //intptr转byte[]
                            // byte[] bytes = new byte[required];
                            // Marshal.Copy(bufferPtr, bytes, 0, required);

                            required = pkgHeaderSize;

                        }

                        AddMsg(string.Format(" > [{0},OnReceive] -> ({1} bytes)", connId, pkgInfo.Length));

                        // 回发数据
                        byte[] sendBytes = new byte[pkgInfo.Length];
                        Marshal.Copy(bufferPtr, sendBytes, 0, sendBytes.Length);
                        if (server.Send(connId, sendBytes, sendBytes.Length) == false)
                        {
                            throw new Exception("server.Send() == false");
                        }

                        // 在后面赋值,因为前面需要用到pkgInfo.Length
                        pkgInfo.IsHeader = !pkgInfo.IsHeader;
                        pkgInfo.Length = required;
                        if (server.SetExtra(connId, clientInfo) == false)
                        {
                            return HandleResult.Error;
                        }
                    }

                }
                catch
                {
                    return HandleResult.Error;
                }
                finally
                {
                    if (bufferPtr != IntPtr.Zero)
                    {
                        Marshal.FreeHGlobal(bufferPtr);
                        bufferPtr = IntPtr.Zero;
                    }
                }
            }

            return HandleResult.Ok;
        }

        HandleResult OnClose(IntPtr connId, SocketOperation enOperation, int errorCode)
        {
            if(errorCode == 0)
                // 客户离开了
                AddMsg(string.Format(" > [{0},OnClose]", connId));
            else
                // 客户出错了
                AddMsg(string.Format(" > [{0},OnError] -> OP:{1},CODE:{2}", connId, enOperation, errorCode));
            // return HPSocketSdk.HandleResult.Ok;

            // 释放附加数据
            if (server.RemoveExtra(connId) == false)
            {
                AddMsg(string.Format(" > [{0},OnClose] -> SetConnectionExtra({0}, null) fail", connId));
            }

            return HandleResult.Ok;
        }

        HandleResult OnShutdown()
        {
            // 服务关闭了


            AddMsg(" > [OnShutdown]");
            return HandleResult.Ok;
        }


        /// <summary>
        /// 设置程序状态
        /// </summary>
        /// <param name="state"></param>
        void SetAppState(AppState state)
        {
            appState = state;
            this.btnStart.Enabled = (appState == AppState.Stoped);
            this.btnStop.Enabled = (appState == AppState.Started);
            this.txtIpAddress.Enabled = (appState == AppState.Stoped);
            this.txtPort.Enabled = (appState == AppState.Stoped);
            this.txtDisConn.Enabled = (appState == AppState.Started);
            this.btnDisconn.Enabled = (appState == AppState.Started && this.txtDisConn.Text.Length > 0);
        }

        /// <summary>
        /// 往listbox加一条项目
        /// </summary>
        /// <param name="msg"></param>
        void AddMsg(string msg)
        {
            if (this.lbxMsg.InvokeRequired)
            {
                // 很帅的调自己
                this.lbxMsg.Invoke(AddMsgDelegate, msg);
            }
            else
            {
                if (this.lbxMsg.Items.Count > 100)
                {
                    this.lbxMsg.Items.RemoveAt(0);
                }
                this.lbxMsg.Items.Add(msg);
                this.lbxMsg.TopIndex = this.lbxMsg.Items.Count - (int)(this.lbxMsg.Height / this.lbxMsg.ItemHeight);
            }
        }

        private void txtDisConn_TextChanged(object sender, EventArgs e)
        {
            // CONNID框被改变事件
            this.btnDisconn.Enabled = (appState == AppState.Started && this.txtDisConn.Text.Length > 0);
        }

        private void lbxMsg_KeyPress(object sender, KeyPressEventArgs e)
        {
            // 清理listbox
            if (e.KeyChar == 'c' || e.KeyChar == 'C')
            {
                this.lbxMsg.Items.Clear();
            }
        }

        private void frmServer_FormClosed(object sender, FormClosedEventArgs e)
        {
            server.Destroy();
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public class ClientInfo
    {
        public IntPtr ConnId { get; set; }
        public string IpAddress { get; set; }
        public ushort Port { get; set; }
        public PkgInfo PkgInfo { get; set; }
    }

    [StructLayout(LayoutKind.Sequential)]
    public class PkgHeader
    {
        public int Id;
        public int BodySize;
    }

    [StructLayout(LayoutKind.Sequential)]
    public class PkgInfo
    {
        public bool IsHeader;
        public int Length;
    }
}
