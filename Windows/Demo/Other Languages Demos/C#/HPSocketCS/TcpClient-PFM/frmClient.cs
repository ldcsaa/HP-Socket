using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Diagnostics;
using HPSocketCS;

namespace TcpClient_PFM
{
    public enum AppState
    {
        Starting, Started, Stoping, Stoped, Error
    }

    public partial class frmClient : Form
    {
        private AppState appState = AppState.Stoped;
        private delegate void ShowMsg(string msg);
        private ShowMsg AddMsgDelegate;

        Stopwatch StopWatch = new Stopwatch();

        int TestTimes = 0;
        int TestInterv = 0;
        int ThreadCount = 0;
        int ThreadInterv = 0;
        int DataLength = 0;

        long Timeconsuming = 0;
        Int64 TotalReceived = 0;
        Int64 TotalSent = 0;
        Int64 ExpectReceived = 0;

        Thread testThread = null;

        List<TcpClient> clientList = new List<TcpClient>();

        public frmClient()
        {
            InitializeComponent();
        }

        private void frmClient_Load(object sender, EventArgs e)
        {

            // 初始化测试值
            this.cbxTestTime.SelectedIndex = 5;
            this.cbxTestInterv.SelectedIndex = 1;
            this.cbxThreadInterv.SelectedIndex = 0;
            this.cbxThreadCount.SelectedIndex = 5;
            this.cbxDataLength.SelectedIndex = 5;

            try
            {
                // 加个委托显示msg,因为on系列都是在工作线程中调用的,ui不允许直接操作
                AddMsgDelegate = new ShowMsg(AddMsg);

                SetAppState(AppState.Stoped);

                AddMsg("提示:32位系统和程序,调整参数时,组合包的【大小】不要超过系统支持的内存。");
            }
            catch (Exception ex)
            {
                SetAppState(AppState.Error);
                AddMsg(ex.Message);
            }
        }

        private bool CheckParams()
        {
            bool ret = false;
            if (TestTimes <= 0)
            {
                this.cbxTestTime.Focus();
            }
            else if (TestInterv < 0)
            {
                this.cbxTestInterv.Focus();
            }
            else if (ThreadCount <= 0)
            {
                this.cbxThreadCount.Focus();
            }
            else if (ThreadInterv < 0)
            {
                this.cbxThreadInterv.Focus();
            }
            else if (DataLength <= 0)
            {
                this.cbxDataLength.Focus();
            }
            else
            {
                ret = true;
            }

            return ret;
        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            try
            {
                string address = this.txtIpAddress.Text.Trim();
                ushort port = ushort.Parse(this.txtPort.Text.Trim());
                TestTimes = int.Parse(this.cbxTestTime.Text.Trim());
                TestInterv = int.Parse(this.cbxTestInterv.Text.Trim());
                ThreadCount = int.Parse(this.cbxThreadCount.Text.Trim());
                ThreadInterv = int.Parse(this.cbxThreadInterv.Text.Trim());
                DataLength = int.Parse(this.cbxDataLength.Text.Trim());
                if (CheckParams() == false)
                {
                    throw new Exception("params error!");
                }

                SetAppState(AppState.Starting);

                Timeconsuming = 0;
                TotalReceived = 0;
                TotalSent = 0;
                ExpectReceived = (Int64)TestTimes * (Int64)ThreadCount * (Int64)DataLength;

                clientList.Clear();

                // 创建指定线程个数的客户端
                for (int i = 0; i < ThreadCount; i++)
                {
                    TcpClient client = new TcpClient();

                    // 设置client事件
                    client.OnPrepareConnect += new TcpClientEvent.OnPrepareConnectEventHandler(OnPrepareConnect);
                    client.OnConnect += new TcpClientEvent.OnConnectEventHandler(OnConnect);
                    client.OnSend += new TcpClientEvent.OnSendEventHandler(OnSend);
                    client.OnReceive += new TcpClientEvent.OnReceiveEventHandler(OnReceive);
                    client.OnClose += new TcpClientEvent.OnCloseEventHandler(OnClose);

                    if (client.Connect(address, port) == true)
                    {
                        clientList.Add(client);
                    }
                    else
                    {
                        foreach (var c in clientList)
                        {
                            c.Stop();
                        }
                        clientList.Clear();
                        throw new Exception(string.Format(" > {2}, Connection to server fail ->({0},{1})", client.ErrorCode, client.ErrorMessage, i));
                    }
                }

                AddMsg(string.Format("$ Client start ok -> ({0}:{1})", address, port));

                // 延迟3秒
                int sendDelay = 3;
                AddMsg(string.Format(" *** willing to send data after {0} seconds ...", sendDelay));
                // Delay2(sendDelay * 1000);

                SetAppState(AppState.Started);

                testThread = new Thread(delegate()
                {
                    Thread.Sleep(sendDelay * 1000);
                    AddMsg(string.Format(" *** begin... ***", sendDelay));
                    // 发送的数据包
                    byte[] sendBytes = new byte[DataLength];


                    StopWatch.Restart();
                    bool isTerminated = false;
                    for (int i = 0; i < TestTimes; i++)
                    {
                        for (int j = 0; j < ThreadCount; j++)
                        {
                            TcpClient client = clientList[j];
                            if (client.Send(sendBytes, sendBytes.Length) == false)
                            {
                                AddMsg(string.Format("$ Client send fail [sock: {0}, seq: {1}] -> {3} ({2})",
                                                     i + 1,
                                                     j + 1,
                                                     client.SYSGetLastError(),
                                                     client.GetSocketErrorDesc(SocketError.DataSend)));
                                isTerminated = true;
                                break;
                            }

                            if (ThreadInterv > 0 && j + 1 < ThreadCount)
                            {
                                Thread.Sleep(ThreadInterv);
                                //Delay2(ThreadInterv);
                            }
                        }

                        if (isTerminated == true)
                        {
                            break;
                        }

                        if (TestInterv > 0 && i + 1 < TestTimes)
                        {
                            Thread.Sleep(TestInterv);
                            //Delay2(TestInterv);
                        }
                    }
                });
                testThread.Start();
            }
            catch (Exception ex)
            {
                SetAppState(AppState.Stoped);
                AddMsg(string.Format("$ Start fail msg:{0}", ex.Message));
            }
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            SetAppState(AppState.Stoping);

            for (int i = 0; i < clientList.Count; i++)
            {
                if (clientList[i].Stop() == true)
                {
                    AddMsg(string.Format("$ Client stopping ... --> ({0})", i + 1));
                }
            }

            if (testThread.ThreadState == System.Threading.ThreadState.Running)
            {
                testThread.Abort();
            }

            Delay2(100);

            AddMsg(string.Format("*** Summary: expect -  {0}, send - {1}, recv - {2}",
                                 ExpectReceived, TotalSent, TotalReceived));

            if (ExpectReceived == TotalSent && TotalSent == TotalReceived)
            {
                AddMsg(string.Format(" *** Success: time consuming -  {0} millisecond !", Timeconsuming));
            }
            else
            {
                AddMsg("*** Fail: manual terminated ? (or data lost)");
            }

            SetAppState(AppState.Stoped);
        }

        HandleResult OnPrepareConnect(TcpClient sender, IntPtr socket)
        {
            return HandleResult.Ok;
        }

        HandleResult OnConnect(TcpClient sender)
        {
            // 已连接 到达一次
            AddMsg(string.Format(" > [{0}, OnConnect]", sender.ConnectionId));
            return HandleResult.Ok;
        }

        HandleResult OnSend(TcpClient sender, byte[] bytes)
        {
            // 客户端发数据了
            Interlocked.Add(ref TotalSent, bytes.Length);
            return HandleResult.Ok;
        }

        HandleResult OnReceive(TcpClient sender, byte[] bytes)
        {
            // 数据到达了

            Interlocked.Add(ref TotalReceived, bytes.Length);
            if (TotalReceived == ExpectReceived)
            {
                StopWatch.Stop();
                Timeconsuming = StopWatch.ElapsedMilliseconds;
                AddMsg(string.Format("Total time consuming: {0}", Timeconsuming));
            }

            return HandleResult.Ok;
        }

        HandleResult OnClose(TcpClient sender, SocketOperation enOperation, int errorCode)
        {
            // 连接关闭了
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
            this.cbxTestTime.Enabled = (appState == AppState.Stoped);
            this.cbxTestInterv.Enabled = (appState == AppState.Stoped);
            this.cbxThreadInterv.Enabled = (appState == AppState.Stoped);
            this.cbxThreadCount.Enabled = (appState == AppState.Stoped);
            this.cbxDataLength.Enabled = (appState == AppState.Stoped);
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

        public delegate void SleepDelegate(int num);

        /// <summary>
        /// 精确延迟,摘自网络
        /// </summary>
        /// <param name="time"></param>
        private void Delay1(long time)
        {
            long b = DateTime.Now.Ticks / 10000;
            long e = 0; long c = 0; ;
            do
            {
                e = DateTime.Now.Ticks / 10000;
                c = e - b;
                Application.DoEvents();
            }
            while (c < time);
        }

        private void AsyncDelay(int time)
        {
            Thread.Sleep(time);
        }

        /// <summary>
        /// 精确延迟,摘自网络
        /// </summary>
        /// <param name="time"></param>
        private void Delay2(int time)
        {
            SleepDelegate delay = new SleepDelegate(AsyncDelay);
            IAsyncResult ar = delay.BeginInvoke(time, null, null);
            while (ar.IsCompleted == false)
            {
                Application.DoEvents();
            }
            delay.EndInvoke(ar);
        }

        private void lbxMsg_KeyPress(object sender, KeyPressEventArgs e)
        {
            // 清理listbox
            if (e.KeyChar == 'c' || e.KeyChar == 'C')
            {
                this.lbxMsg.Items.Clear();
            }
        }

        private void frmClient_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (appState == AppState.Started)
            {
                btnStop_Click(sender, e);
            }
        }

    }
}
