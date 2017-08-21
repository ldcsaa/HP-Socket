using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using HPSocketCS;
using System.Runtime.InteropServices;
using System.IO;
using System.Threading;

namespace SSLClientNS
{
    public enum AppState
    {
        Starting, Started, Stoping, Stoped, Error
    }

    public enum StudentType
    {
        None, Array, List, Single,
    }

    public partial class frmClient : Form
    {
        private AppState appState = AppState.Stoped;

        private delegate void ConnectUpdateUiDelegate();
        private delegate void SetAppStateDelegate(AppState state);
        private delegate void ShowMsg(string msg);
        private ShowMsg AddMsgDelegate;
        HPSocketCS.SSLClient client = null;

        bool isSendFile = false;
        StudentType studentType = StudentType.None;
        public frmClient()
        {
            InitializeComponent();
        }

        private void frmClient_Load(object sender, EventArgs e)
        {
            try
            {
                client = new HPSocketCS.SSLClient();
                client.VerifyMode = SSLVerifyMode.Peer | SSLVerifyMode.FailIfNoPeerCert;
                client.CAPemCertFileOrPath = "ssl-cert\\ca.crt";
                client.PemCertFile = "ssl-cert\\client.cer";
                client.PemKeyFile = "ssl-cert\\client.key";
                client.KeyPasswod = "123456";

                // 加个委托显示msg,因为on系列都是在工作线程中调用的,ui不允许直接操作
                AddMsgDelegate = new ShowMsg(AddMsg);

                // 设置client事件
                client.OnPrepareConnect += new TcpClientEvent.OnPrepareConnectEventHandler(OnPrepareConnect);
                client.OnConnect += new TcpClientEvent.OnConnectEventHandler(OnConnect);
                client.OnSend += new TcpClientEvent.OnSendEventHandler(OnSend);
                client.OnReceive += new TcpClientEvent.OnReceiveEventHandler(OnReceive);
                client.OnClose += new TcpClientEvent.OnCloseEventHandler(OnClose);

                client.OnHandShake += new TcpClientEvent.OnHandShakeEventHandler(OnHandShake);

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

                AddMsg(string.Format("$Client Starting ... -> ({0}:{1})", ip, port));

                if (client.Connect(ip, port, this.cbxAsyncConn.Checked))
                {
                    if (cbxAsyncConn.Checked == false)
                    {
                        SetAppState(AppState.Started);
                    }

                    AddMsg(string.Format("$Client Start OK -> ({0}:{1})", ip, port));
                }
                else
                {
                    SetAppState(AppState.Stoped);
                    throw new Exception(string.Format("$Client Start Error -> {0}({1})", client.ErrorMessage, client.ErrorCode));
                }
            }
            catch (Exception ex)
            {
                AddMsg(ex.Message);
            }
        }

        private void btnStop_Click(object sender, EventArgs e)
        {

            // 停止服务
            AddMsg("$Server Stop");
            if (client.Stop())
            {
                SetAppState(AppState.Stoped);
            }
            else
            {
                AddMsg(string.Format("$Stop Error -> {0}({1})", client.ErrorMessage, client.ErrorCode));
            }
        }

        private void btnSend_Click(object sender, EventArgs e)
        {
            try
            {
                string send = this.txtSend.Text;
                if (send.Length == 0)
                {
                    return;
                }

                byte[] bytes = Encoding.Default.GetBytes(send);
                IntPtr connId = client.ConnectionId;

                // 发送
                if (client.Send(bytes, bytes.Length))
                {
                    AddMsg(string.Format("$ ({0}) Send OK --> {1}", connId, send));
                }
                else
                {
                    AddMsg(string.Format("$ ({0}) Send Fail --> {1} ({2})", connId, send, bytes.Length));
                }

            }
            catch (Exception ex)
            {
                AddMsg(string.Format("$ Send Fail -->  msg ({0})", ex.Message));
            }

        }

        private void btnSendSerializableObject_Click(object sender, EventArgs e)
        {
            try
            {
                // 注: 对象序列化发送数据较大

                if (studentType != StudentType.None)
                {
                    // 正在执行
                    throw new Exception("being implemented");
                }



                IntPtr connId = client.ConnectionId;

                Thread thread = new Thread(new ThreadStart(delegate()
                {
                    Student[] students = new Student[2];
                    students[0] = new Student();
                    students[0].Id = 1;
                    students[0].Name = "张三";
                    students[0].Sex = Sex.Female;
                    students[1] = new Student();
                    students[1].Id = 2;
                    students[1].Name = "李四";
                    students[1].Sex = Sex.Male;

                    // 发送数组对象
                    studentType = StudentType.Array;
                    if (client.SendBySerializable(students))
                    {
                        AddMsg(string.Format("$ ({0}) Send OK --> {1}", connId, "Student[]"));
                    }
                    else
                    {
                        AddMsg(string.Format("$ ({0}) Send Fail --> {1}", connId, "Student[]"));
                    }

                    ////////////////////////////////////////////////////////////////////////////////

                    // 改变性别
                    students[0].Sex = Sex.Transsexual;

                    List<Student> stuList = new List<Student>();
                    stuList.Add(students[0]);
                    stuList.Add(students[1]);

                    // 防止粘包,延迟2秒发送下一组数据
                    AddMsg(" *** 2 seconds after sending ...");
                    Thread.Sleep(2000);

                    // 发送list对象
                    studentType = StudentType.List;
                    if (client.SendBySerializable(stuList))
                    {
                        AddMsg(string.Format("$ ({0}) Send OK --> {1}", connId, "List<Student>"));
                    }
                    else
                    {
                        AddMsg(string.Format("$ ({0}) Send Fail --> {1}", connId, "List<Student>"));
                    }

                    ////////////////////////////////////////////////////////////////////////////////

                    // 防止粘包,延迟2秒发送下一组数据
                    AddMsg(" *** 2 seconds after sending ...");
                    Thread.Sleep(2000);

                    // 改变性别
                    students[0].Sex = Sex.Unknown;

                    // 发送单个学员信息
                    studentType = StudentType.Single;
                    if (client.SendBySerializable(students[0]))
                    {
                        AddMsg(string.Format("$ ({0}) Send OK --> {1}", connId, "Student"));
                    }
                    else
                    {
                        AddMsg(string.Format("$ ({0}) Send Fail --> {1}", connId, "Student"));
                    }

                }));
                thread.Start();
            }
            catch (Exception ex)
            {
                studentType = StudentType.None;
                AddMsg(string.Format("$ Send Fail -->  msg ({0})", ex.Message));
            }
        }

        private void lbxMsg_KeyPress(object sender, KeyPressEventArgs e)
        {

            // 清理listbox
            if (e.KeyChar == 'c' || e.KeyChar == 'C')
            {
                this.lbxMsg.Items.Clear();
            }
        }

        void ConnectUpdateUi()
        {
            if (this.cbxAsyncConn.Checked == true)
            {
                SetAppState(AppState.Started);
            }
        }


        private bool SendFileToServer(string filePath)
        {
            FileInfo fi = new FileInfo(filePath);
            if (fi.Length > 4096 * 1024)
            {
                throw new Exception("被发送文件大小不能超过4096KB。");
            }
            else if (fi.Length == 0)
            {
                throw new Exception("文件大小不能为0。");
            }

            // 头附加数据
            MyFileInfo myFile = new MyFileInfo()
            {
                FilePath = filePath,
                FileSize = fi.Length,
            };

            // 发送文件标记,用来在onrecv里判断是否文件回包
            isSendFile = true;

            // 不附加尾数据
            bool ret = client.SendSmallFile<MyFileInfo, object>(filePath, myFile, null);
            if (ret == false)
            {
                isSendFile = false;
                int error = client.SYSGetLastError();
                AddMsg(string.Format(" > [SendSmallFile] errorCode:{0}", error));
            }

            return ret;
        }

        HandleResult OnPrepareConnect(TcpClient sender, IntPtr socket)
        {
            return HandleResult.Ok;
        }

        HandleResult OnConnect(TcpClient sender)
        {
            // 已连接 到达一次

            // 如果是异步联接,更新界面状态
            this.Invoke(new ConnectUpdateUiDelegate(ConnectUpdateUi));

            AddMsg(string.Format(" > [{0},OnConnect]", sender.ConnectionId));

            return HandleResult.Ok;
        }

        HandleResult OnSend(TcpClient sender, byte[] bytes)
        {
            // 客户端发数据了
            AddMsg(string.Format(" > [{0},OnSend] -> ({1} bytes)", sender.ConnectionId, bytes.Length));

            return HandleResult.Ok;
        }

        HandleResult OnReceive(TcpClient sender, byte[] bytes)
        {
            // 数据到达了
            if (isSendFile == true)
            {
                // 如果发送了文件,并接收到了返回数据
                isSendFile = false;
                MyFileInfo myFile = client.BytesToStruct<MyFileInfo>(bytes);
                int objSize = Marshal.SizeOf(myFile);
                // 因为没有附加尾数据,所以大小可以用length - objSize
                byte[] contentBytes = new byte[bytes.Length - objSize];
                Array.ConstrainedCopy(bytes, objSize, contentBytes, 0, contentBytes.Length);

                string txt = Encoding.Default.GetString(contentBytes);
                string msg = string.Empty;
                if (txt.Length > 100)
                {
                    msg = txt.Substring(0, 100) + "......";
                }
                else
                {
                    msg = txt;
                }

                AddMsg(string.Format(" > [{0},OnReceive] -> FileInfo(Path:\"{1}\",Size:{2})", sender.ConnectionId, myFile.FilePath, myFile.FileSize));
                AddMsg(string.Format(" > [{0},OnReceive] -> FileContent(\"{1}\")", sender.ConnectionId, msg));
            }
            else if (studentType != StudentType.None)
            {
                switch (studentType)
                {
                    case StudentType.Array:
                        Student[] students = sender.BytesToObject(bytes) as Student[];
                        foreach (var stu in students)
                        {
                            AddMsg(string.Format(" > [{0},OnReceive] -> Student({1},{2},{3})", sender.ConnectionId, stu.Id, stu.Name, stu.GetSexString()));
                        }
                        break;
                    case StudentType.List:
                        List<Student> stuList = sender.BytesToObject(bytes) as List<Student>;
                        foreach (var stu in stuList)
                        {
                            AddMsg(string.Format(" > [{0},OnReceive] -> Student({1},{2},{3})", sender.ConnectionId, stu.Id, stu.Name, stu.GetSexString()));
                        }
                        break;
                    case StudentType.Single:
                        Student student = sender.BytesToObject(bytes) as Student;
                        AddMsg(string.Format(" > [{0},OnReceive] -> Student({1},{2},{3})", sender.ConnectionId, student.Id, student.Name, student.GetSexString()));
                        studentType = StudentType.None;
                        break;
                }
            }
            else
            {
                AddMsg(string.Format(" > [{0},OnReceive] -> ({1} bytes)", sender.ConnectionId, bytes.Length));
            }

            return HandleResult.Ok;
        }

        HandleResult OnClose(TcpClient sender, SocketOperation enOperation, int errorCode)
        {
            if(errorCode == 0)
                // 连接关闭了
                AddMsg(string.Format(" > [{0},OnClose]", sender.ConnectionId));
            else
                // 出错了
                AddMsg(string.Format(" > [{0},OnError] -> OP:{1},CODE:{2}", sender.ConnectionId, enOperation, errorCode));

            // 通知界面,只处理了连接错误,也没进行是不是连接错误的判断,所以有错误就会设置界面
            // 生产环境请自己控制
            this.Invoke(new SetAppStateDelegate(SetAppState), AppState.Stoped);

            return HandleResult.Ok;
        }

        HandleResult OnHandShake(TcpClient sender)
        {
            // 握手了
            AddMsg(string.Format(" > [{0},OnHandShake])", sender.ConnectionId));

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
            this.cbxAsyncConn.Enabled = (appState == AppState.Stoped);
            this.btnSend.Enabled = (appState == AppState.Started);
            this.btnSendFile.Enabled = (appState == AppState.Started);
            this.btnSendSerializableObject.Enabled = (appState == AppState.Started);
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

        private void frmClient_FormClosed(object sender, FormClosedEventArgs e)
        {
            client.Destroy();
        }



        private void btnSendFile_Click(object sender, EventArgs e)
        {
            try
            {

                OpenFileDialog ofd = new OpenFileDialog();
                // ofd.InitialDirectory = "c:\\" ;
                //ofd.Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
                ofd.Filter = "txt files (*.txt)|*.txt";
                ofd.RestoreDirectory = true;
                ofd.CheckFileExists = true;
                ofd.CheckPathExists = true;
                if (ofd.ShowDialog() == DialogResult.OK)
                {
                    SendFileToServer(ofd.FileName);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);

            }
        }

    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct MyFileInfo
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 255)]
        public string FilePath;
        public long FileSize;
    }

    /// <summary>
    /// 学员性别
    /// </summary>
    public enum Sex
    {
        /// <summary>
        /// 男
        /// </summary>
        Male = 1,

        /// <summary>
        /// 女
        /// </summary>
        Female = 2,

        /// <summary>
        /// 变性人
        /// </summary>
        Transsexual = 3,

        /// <summary>
        /// 未知
        /// </summary>
        Unknown = 4,

    }

    /// <summary>
    /// 学员类
    /// </summary>
    [Serializable]
    public class Student
    {
        /// <summary>
        /// 编号
        /// </summary>
        public int Id { get; set; }

        /// <summary>
        /// 姓名
        /// </summary>
        public string Name { get; set; }

        /// <summary>
        /// 性别
        /// </summary>
        public Sex Sex { get; set; }

        public string GetSexString()
        {
            string sex = string.Empty;
            switch (Sex)
            {
                case Sex.Male:
                    sex = "男";
                    break;
                case Sex.Female:
                    sex = "女";
                    break;
                case Sex.Transsexual:
                    sex = "变性人";
                    break;
                default:
                    sex = "未知";
                    break;
            }
            return sex;
        }
    }
}
