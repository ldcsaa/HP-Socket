using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using HPSocketCS;
using System.Net;

namespace HPSocketCS
{

    /// <summary>
    /// 通信组件服务状态,用程序可以通过通信组件的 GetState() 方法获取组件当前服务状态
    /// </summary>
    public enum ServiceState
    {
        /// <summary>
        /// 正在启动
        /// </summary>
        Starting = 0,
        /// <summary>
        /// 已经启动
        /// </summary>
        Started = 1,
        /// <summary>
        /// 正在停止
        /// </summary>
        Stoping = 2,
        /// <summary>
        /// 已经启动
        /// </summary>
        Stoped = 3,
    }

    /// <summary>
    /// Socket 操作类型,应用程序的 OnErrror() 事件中通过该参数标识是哪种操作导致的错误
    /// </summary>
    public enum SocketOperation
    {
        Unknown = 0,    // Unknown
        Acccept = 1,    // Acccept
        Connnect = 2,   // Connnect
        Send = 3,       // Send
        Receive = 4,    // Receive
        Close = 5,    // Receive
    };

    /// <summary>
    /// 事件通知处理结果,事件通知的返回值，不同的返回值会影响通信组件的后续行为
    /// </summary>
    public enum HandleResult
    {
        /// <summary>
        /// 成功
        /// </summary>
        Ok = 0,
        /// <summary>
        /// 忽略
        /// </summary>
        Ignore = 1,
        /// <summary>
        /// 错误
        /// </summary>
        Error = 2,
    };


    /// <summary>
    /// 名称：操作结果代码
    /// 描述：组件 Start() / Stop() 方法执行失败时，可通过 GetLastError() 获取错误代码
    /// </summary>
    public enum SocketError
    {
        /// <summary>
        /// 成功
        /// </summary>
        Ok = 0,
        /// <summary>
        /// 当前状态不允许操作
        /// </summary>
        IllegalState = 1,
        /// <summary>
        /// 非法参数
        /// </summary>
        InvalidParam = 2,
        /// <summary>
        /// 创建 SOCKET 失败
        /// </summary>
        SocketCreate = 3,
        /// <summary>
        /// 绑定 SOCKET 失败
        /// </summary>
        SocketBind = 4,
        /// <summary>
        /// 设置 SOCKET 失败
        /// </summary>
        SocketPrepare = 5,
        /// <summary>
        /// 监听 SOCKET 失败
        /// </summary>
        SocketListen = 6,
        /// <summary>
        /// 创建完成端口失败
        /// </summary>
        CPCreate = 7,
        /// <summary>
        /// 创建工作线程失败
        /// </summary>
        WorkerThreadCreate = 8,
        /// <summary>
        /// 创建监测线程失败
        /// </summary>
        DetectThreadCreate = 9,
        /// <summary>
        /// 绑定完成端口失败
        /// </summary>
        SocketAttachToCP = 10,
        /// <summary>
        /// 连接服务器失败
        /// </summary>
        ConnectServer = 11,
        /// <summary>
        /// 网络错误
        /// </summary>
        Network = 12,
        /// <summary>
        /// 数据处理错误
        /// </summary>
        DataProc = 13,
        /// <summary>
        /// 数据发送失败
        /// </summary>
        DataSend = 14,

        /***** SSL Socket 扩展操作结果代码 *****/
        /// <summary>
        /// SSL 环境未就绪
        /// </summary>
        SSLEnvNotReady = 101,
    };

    /// <summary>
    /// 数据抓取结果,数据抓取操作的返回值
    /// </summary>
    public enum FetchResult
    {
        /// <summary>
        /// 成功
        /// </summary>
        Ok = 0,
        /// <summary>
        /// 抓取长度过大
        /// </summary>
        LengthTooLong = 1,
        /// <summary>
        /// 找不到 ConnID 对应的数据
        /// </summary>
        DataNotFound = 2,
    };

    /// <summary>
    /// 发送策略
    /// </summary>
    public enum SendPolicy
    {
        /// <summary>
        /// 打包模式（默认）
        /// </summary>
        Pack = 0,
        /// <summary>
        /// 安全模式
        /// </summary>
        Safe = 1,
        /// <summary>
        /// 直接模式
        /// </summary>
        Direct = 2,
    };

    /// <summary>
    /// 播送模式  UDP 组件的播送模式（组播或广播）
    /// </summary>
    public enum CastMode
    {
        /// <summary>
        /// 组播
        /// </summary>
        Multicast = 0,
        /// <summary>
        /// 广播
        /// </summary>
        Broadcast = 1,

    }

    /// <summary>
    /// 名称：IP 地址类型
    /// 描述：IP 地址类型枚举值
    /// </summary>
    public enum IPAddrType
    {
        /// <summary>
        /// 所有
        /// </summary>
        All = 0,
        /// <summary>
        /// IPv4
        /// </summary>
        IPV4 = 1,
        /// <summary>
        /// IPv6
        /// </summary>
        IPV6 = 2,
        /// <summary>
        /// 其他
        /// </summary>
        Other = 100,
    }

    /// <summary>
    /// 接收状态
    /// </summary>
    public enum ReceiveState : int
    {
        /// <summary>
        /// 未知
        /// </summary>
        Unknown = -1,
        /// <summary>
        /// 唤醒状态
        /// </summary>
        Resume = 0,
        /// <summary>
        /// 暂停状态
        /// </summary>
        Pause = 1,
    }

    /****************************************************/
    /************** sockaddr结构体,udp服务器时OnAccept最后个参数可转化 **************/
    [StructLayout(LayoutKind.Sequential)]
    public struct in_addr
    {
        public ulong S_addr;
    }

    //[StructLayout(LayoutKind.Sequential)]
    //public struct in_addr
    //{
    //    public byte s_b1, s_b2, s_b3, s_b4;
    //}

    [StructLayout(LayoutKind.Sequential)]
    public struct sockaddr_in
    {
        public short sin_family;
        public ushort sin_port;
        public in_addr sin_addr;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
        public byte[] sLibNamesin_zero;
    }

    /****************************************************/

    [StructLayout(LayoutKind.Sequential)]
    public struct WSABUF
    {
        public int Length;
        public IntPtr Buffer;
    }

    /// <summary>
    /// 名称：IP 地址条目结构体
    /// 描述：IP 地址的地址簇/地址值结构体
    /// </summary>
    public struct IPAddr
    {
        public IPAddrType Type;
        public string Address;
        public string TypeString;
    }


    /// <summary>
    /// DnsHelper
    /// </summary>
    public class DnsHelper
    {
        /// <summary>
        /// 获取本机ipv4+ipv6地址
        /// hp-socket可用的ip地址结构
        /// </summary>
        /// <returns></returns>
        public static List<IPAddr> GetLocalHostAddressesToIPAddrList()
        {
            return Dns.GetHostAddresses(Dns.GetHostName()).Where(p => p.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork ||
                                                                      p.AddressFamily == System.Net.Sockets.AddressFamily.InterNetworkV6).
                Select(p => new IPAddr
                {
                    Type = p.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork ? IPAddrType.IPV4 : IPAddrType.IPV6,
                    Address = p.ToString(),
                }).ToList();
        }

        /// <summary>
        /// 获取本机ipv4+ipv6地址
        /// .net自带的ip地址结构
        /// </summary>
        /// <returns></returns>
        public static IPAddress[] GetLocalHostAddresses()
        {
            return Dns.GetHostAddresses(Dns.GetHostName());
        }
    }
}


/// <summary>
/// Unicode版本
/// </summary>
public class Sdk
{


    /// <summary>
    /// HPSocket的文件路径
    /// </summary>
    public const string HPSOCKET_DLL_PATH = "HPSocket4C_U.dll";

    /*****************************************************************************************************/
    /******************************************** 公共类、接口 ********************************************/
    /*****************************************************************************************************/



    /****************************************************/
    /************** HPSocket4C.dll 回调函数 **************/
    /* Agent & Server & Clent */
    public delegate HandleResult OnSend(IntPtr pSender, IntPtr connId, IntPtr pData, int length);
    public delegate HandleResult OnReceive(IntPtr pSender, IntPtr connId, IntPtr pData, int length);
    public delegate HandleResult OnPullReceive(IntPtr pSender, IntPtr connId, int length);
    public delegate HandleResult OnClose(IntPtr pSender, IntPtr connId, SocketOperation enOperation, int errorCode);
    public delegate HandleResult OnHandShake(IntPtr pSender, IntPtr connId);


    /* Agent & Server */
    public delegate HandleResult OnShutdown(IntPtr pSender);

    /* Agent & Client */
    public delegate HandleResult OnPrepareConnect(IntPtr pSender, IntPtr connId /* IntPtr pClient */, IntPtr socket);
    public delegate HandleResult OnConnect(IntPtr pSender, IntPtr connId /* IntPtr pClient */);

    /* Server */
    public delegate HandleResult OnPrepareListen(IntPtr pSender, IntPtr soListen);
    public delegate HandleResult OnAccept(IntPtr pSender, IntPtr connId, IntPtr pClient);

    /****************************************************/
    /************** HPSocket4C.dll 导出函数 **************/

    /// <summary>
    /// 创建 TcpServer 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpServer(IntPtr pListener);

    /// <summary>
    /// 创建 TcpClient 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpClient(IntPtr pListener);

    /// <summary>
    /// 创建 TcpAgent 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpAgent(IntPtr pListener);

    /// <summary>
    /// 创建 TcpPullServer 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpPullServer(IntPtr pListener);

    /// <summary>
    /// 创建 TcpPullClient 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpPullClient(IntPtr pListener);

    /// <summary>
    /// 创建 TcpPullAgent 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpPullAgent(IntPtr pListener);

    /// <summary>
    /// 创建 HP_TcpPackServer 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpPackServer(IntPtr pListener);

    /// <summary>
    /// 创建 HP_TcpPackAgent 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpPackAgent(IntPtr pListener);

    /// <summary>
    /// 创建 HP_TcpPackAgent 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpPackClient(IntPtr pListener);

    /// <summary>
    /// 创建 UdpServer 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_UdpServer(IntPtr pListener);

    /// <summary>
    /// 创建 UdpClient 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_UdpClient(IntPtr pListener);


    /// <summary>
    /// 创建 HP_UdpCast 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_UdpCast(IntPtr pListener);


    /// <summary>
    /// 销毁 TcpServer 对象
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpServer(IntPtr pServer);

    /// <summary>
    /// 销毁 TcpClient 对象
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpClient(IntPtr pClient);

    /// <summary>
    /// 销毁 TcpAgent 对象
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpAgent(IntPtr pAgent);

    /// <summary>
    /// 销毁 TcpPullServer 对象
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpPullServer(IntPtr pServer);

    /// <summary>
    /// 销毁 TcpPullClient 对象
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpPullClient(IntPtr pClient);

    /// <summary>
    /// 销毁 TcpPullAgent 对象
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpPullAgent(IntPtr pAgent);

    /// <summary>
    /// 销毁 HP_TcpPackServer 对象
    /// </summary>
    /// <param name="pServer"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpPackServer(IntPtr pServer);

    /// <summary>
    /// 销毁 HP_TcpPackAgent 对象
    /// </summary>
    /// <param name="pAgent"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpPackAgent(IntPtr pAgent);

    /// <summary>
    /// 销毁 HP_TcpPackClient 对象
    /// </summary>
    /// <param name="pClient"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpPackClient(IntPtr pClient);


    /// <summary>
    /// 销毁 UdpServer 对象
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_UdpServer(IntPtr pServer);

    /// <summary>
    /// 销毁 UdpClient 对象
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_UdpClient(IntPtr pClient);

    /// <summary>
    /// 销毁 HP_UdpCast 对象
    /// </summary>
    /// <param name="pCast"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_UdpCast(IntPtr pCast);


    /// <summary>
    /// 创建 TcpServerListener 对象
    /// </summary>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpServerListener();

    /// <summary>
    /// 创建 TcpClientListener 对象
    /// </summary>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpClientListener();

    /// <summary>
    /// 创建 TcpAgentListener 对象
    /// </summary>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpAgentListener();

    /// <summary>
    /// 创建 TcpPullServerListener 对象
    /// </summary>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpPullServerListener();

    /// <summary>
    /// 创建 TcpPullClientListener 对象
    /// </summary>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpPullClientListener();

    /// <summary>
    /// 创建 TcpPullAgentListener 对象
    /// </summary>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpPullAgentListener();

    /// <summary>
    /// 创建 TcpPackServerListener 对象
    /// </summary>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpPackServerListener();

    /// <summary>
    /// 创建 TcpPackClientListener 对象
    /// </summary>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpPackClientListener();

    /// <summary>
    /// 创建 TcpPackAgentListener 对象
    /// </summary>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_TcpPackAgentListener();

    /// <summary>
    /// 创建 UdpServerListener 对象
    /// </summary>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_UdpServerListener();

    /// <summary>
    /// 创建 UdpClientListener 对象
    /// </summary>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_UdpClientListener();

    /// <summary>
    /// 创建 HP_UdpCastListener 对象
    /// </summary>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr Create_HP_UdpCastListener();



    /// <summary>
    /// 销毁 TcpServerListener 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpServerListener(IntPtr pListener);

    /// <summary>
    /// 销毁 TcpClientListener 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpClientListener(IntPtr pListener);

    /// <summary>
    /// 销毁 TcpAgentListener 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpAgentListener(IntPtr pListener);


    /// <summary>
    /// 销毁 TcpPullServerListener 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpPullServerListener(IntPtr pListener);

    /// <summary>
    /// 销毁 TcpPullClientListener 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpPullClientListener(IntPtr pListener);

    /// <summary>
    /// 销毁 TcpPullAgentListener 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpPullAgentListener(IntPtr pListener);

    /// <summary>
    /// 销毁 TcpPackServerListener 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpPackServerListener(IntPtr pListener);

    /// <summary>
    /// 销毁 TcpPackClientListener 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpPackClientListener(IntPtr pListener);

    /// <summary>
    /// 销毁 TcpPackAgentListener 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_TcpPackAgentListener(IntPtr pListener);

    /// <summary>
    /// 销毁 UdpServerListener 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_UdpServerListener(IntPtr pListener);

    /// <summary>
    /// 销毁 UdpClientListener 对象
    /// </summary>
    /// <param name="pListener"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_UdpClientListener(IntPtr pListener);

    /// <summary>
    /// 销毁 HP_UdpCastListener 对象
    /// </summary>
    /// <param name="pListener"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void Destroy_HP_UdpCastListener(IntPtr pListener);


    /**********************************************************************************/
    /***************************** Server 回调函数设置方法 *****************************/

    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Server_OnPrepareListen(IntPtr pListener, OnPrepareListen fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Server_OnAccept(IntPtr pListener, OnAccept fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Server_OnHandShake(IntPtr pListener, OnHandShake fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Server_OnSend(IntPtr pListener, OnSend fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Server_OnReceive(IntPtr pListener, OnReceive fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Server_OnPullReceive(IntPtr pListener, OnPullReceive fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Server_OnClose(IntPtr pListener, OnClose fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Server_OnShutdown(IntPtr pListener, OnShutdown fn);

    /**********************************************************************************/
    /***************************** Client 回调函数设置方法 *****************************/

    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Client_OnPrepareConnect(IntPtr pListener, OnPrepareConnect fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Client_OnConnect(IntPtr pListener, OnConnect fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Client_OnHandShake(IntPtr pListener, OnHandShake fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Client_OnSend(IntPtr pListener, OnSend fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Client_OnReceive(IntPtr pListener, OnReceive fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Client_OnPullReceive(IntPtr pListener, OnPullReceive fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Client_OnClose(IntPtr pListener, OnClose fn);

    /**********************************************************************************/
    /****************************** Agent 回调函数设置方法 *****************************/

    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Agent_OnPrepareConnect(IntPtr pListener, OnPrepareConnect fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Agent_OnConnect(IntPtr pListener, OnConnect fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Agent_OnHandShake(IntPtr pListener, OnHandShake fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Agent_OnSend(IntPtr pListener, OnSend fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Agent_OnReceive(IntPtr pListener, OnReceive fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Agent_OnPullReceive(IntPtr pListener, OnPullReceive fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Agent_OnClose(IntPtr pListener, OnClose fn);
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Set_FN_Agent_OnShutdown(IntPtr pListener, OnShutdown fn);

    /**************************************************************************/
    /***************************** Server 操作方法 *****************************/

    /// <summary>
    /// 名称：启动通信组件
    /// 描述：启动服务端通信组件，启动完成后可开始接收客户端连接并收发数据
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="pszBindAddress">监听地址</param>
    /// <param name="usPort">监听端口</param>
    /// <returns>失败，可通过 GetLastError() 获取错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode)]
    public static extern bool HP_Server_Start(IntPtr pServer, string pszBindAddress, ushort usPort);

    /// <summary>
    /// 关闭服务端通信组件，关闭完成后断开所有客户端连接并释放所有资源
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns>失败，可通过 GetLastError() 获取错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_Stop(IntPtr pServer);

    /// <summary>
    /// 用户通过该方法向指定客户端发送数据
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="pBuffer">发送数据长度</param>
    /// <param name="length">发送数据长度</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
    public static extern bool HP_Server_Send(IntPtr pServer, IntPtr connId, byte[] pBuffer, int length);

    /// <summary>
    /// 用户通过该方法向指定客户端发送数据
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="pBuffer">发送数据长度</param>
    /// <param name="length">发送数据长度</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern bool HP_Server_Send(IntPtr pServer, IntPtr connId, IntPtr pBuffer, int length);

    /// <summary>
    /// 用户通过该方法向指定客户端发送数据
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId"></param>
    /// <param name="pBuffer"></param>
    /// <param name="length"></param>
    /// <param name="iOffset">针对pBuffer的偏移</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
    public static extern bool HP_Server_SendPart(IntPtr pServer, IntPtr connId, byte[] pBuffer, int length, int iOffset);

    /// <summary>
    /// 用户通过该方法向指定客户端发送数据
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId"></param>
    /// <param name="pBuffer"></param>
    /// <param name="length"></param>
    /// <param name="iOffset">针对pBuffer的偏移</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern bool HP_Server_SendPart(IntPtr pServer, IntPtr connId, IntPtr pBuffer, int length, int iOffset);


    /// <summary>
    /// 发送多组数据
    /// 向指定连接发送多组数据
    /// TCP - 顺序发送所有数据包 
    /// UDP - 把所有数据包组合成一个数据包发送（数据包的总长度不能大于设置的 UDP 包最大长度） 
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="pBuffers">发送缓冲区数组</param>
    /// <param name="iCount">发送缓冲区数目</param>
    /// <returns>TRUE.成功,FALSE.失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern bool HP_Server_SendPackets(IntPtr pServer, IntPtr connId, WSABUF[] pBuffers, int iCount);
   
    /// <summary>
    /// 名称：暂停/恢复接收
    /// 描述：暂停/恢复某个连接的数据接收工作
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwConnID">连接 ID</param>
    /// <param name="bPause">TRUE - 暂停, FALSE - 恢复</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern bool HP_Server_PauseReceive(IntPtr pServer, IntPtr dwConnID, bool bPause);



    /// <summary>
    /// 断开与某个客户端的连接
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="bForce">是否强制断开连接</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_Disconnect(IntPtr pServer, IntPtr connId, bool bForce);

    /// <summary>
    /// 断开超过指定时长的连接
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwPeriod">时长（毫秒）</param>
    /// <param name="bForce">是否强制断开连接</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_DisconnectLongConnections(IntPtr pServer, uint dwPeriod, bool bForce);

    /// <summary>
    /// 断开超过指定时长的静默连接
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwPeriod">时长（毫秒）</param>
    /// <param name="bForce">是否强制断开连接</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_DisconnectSilenceConnections(IntPtr pServer, uint dwPeriod, bool bForce);

    /******************************************************************************/
    /***************************** Server 属性访问方法 *****************************/

    /// <summary>
    /// 设置数据发送策略
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="enSendPolicy"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Server_SetSendPolicy(IntPtr pServer, SendPolicy enSendPolicy);

    /// <summary>
    /// 获取数据发送策略
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern SendPolicy HP_Server_GetSendPolicy(IntPtr pServer);

    /// <summary>
    /// 设置连接的附加数据
    /// 是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序只身决定
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="pExtra"></param>
    /// <returns>若返回 false 失败则为（无效的连接 ID）</returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_SetConnectionExtra(IntPtr pServer, IntPtr connId, IntPtr pExtra);

    /// <summary>
    /// 获取连接的附加数据
    /// 是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序只身决定
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="pExtra">数据指针</param>
    /// <returns>若返回 false 失败则为（无效的连接 ID）</returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_GetConnectionExtra(IntPtr pServer, IntPtr connId, ref IntPtr pExtra);

    /// <summary>
    /// 检测是否为安全连接（SSL/HTTPS）
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_IsSecure(IntPtr pServer);


    /// <summary>
    /// 检查通信组件是否已启动
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_HasStarted(IntPtr pServer);

    /// <summary>
    /// 查看通信组件当前状态
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern ServiceState HP_Server_GetState(IntPtr pServer);

    /// <summary>
    /// 获取最近一次失败操作的错误代码
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern SocketError HP_Server_GetLastError(IntPtr pServer);

    /// <summary>
    /// 获取最近一次失败操作的错误描述
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr HP_Server_GetLastErrorDesc(IntPtr pServer);


    /// <summary>
    /// 获取连接中未发出数据的长度
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId"></param>
    /// <param name="piPending"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_GetPendingDataLength(IntPtr pServer, IntPtr connId, ref int piPending);

    /// <summary>
    /// 获取连接的数据接收状态
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwConnID"></param>
    /// <param name="pbPaused"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_IsPauseReceive(IntPtr pServer, IntPtr dwConnID, ref int pbPaused);

    /// <summary>
    /// 检测是否有效连接
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwConnID"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_IsConnected(IntPtr pServer, IntPtr dwConnID);


    /// <summary>
    /// 获取客户端连接数
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Server_GetConnectionCount(IntPtr pServer);

    /// <summary>
    /// 获取所有连接的 CONNID
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="pIDs"></param>
    /// <param name="pdwCount"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_GetAllConnectionIDs(IntPtr pServer, IntPtr[] pIDs, ref uint pdwCount);

    /// <summary>
    /// 获取某个客户端连接时长（毫秒）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId"></param>
    /// <param name="pdwPeriod"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_GetConnectPeriod(IntPtr pServer, IntPtr connId, ref uint pdwPeriod);

    /// <summary>
    /// 获取某个连接静默时间（毫秒）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId"></param>
    /// <param name="pdwPeriod"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_GetSilencePeriod(IntPtr pServer, IntPtr connId, ref uint pdwPeriod);

    /// <summary>
    /// 获取监听 Socket 的地址信息
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="lpszAddress"></param>
    /// <param name="piAddressLen"></param>
    /// <param name="pusPort"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_GetListenAddress(IntPtr pServer, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder lpszAddress, ref int piAddressLen, ref ushort pusPort);

    /// <summary>
    /// 获取某个连接的本地地址信息
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId"></param>
    /// <param name="lpszAddress"></param>
    /// <param name="piAddressLen">传入传出值,大小最好在222.222.222.222的长度以上</param>
    /// <param name="pusPort"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_GetLocalAddress(IntPtr pServer, IntPtr connId, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder lpszAddress, ref int piAddressLen, ref ushort pusPort);

    /// <summary>
    /// 获取某个连接的远程地址信息
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId"></param>
    /// <param name="lpszAddress"></param>
    /// <param name="piAddressLen">传入传出值,大小最好在222.222.222.222的长度以上</param>
    /// <param name="pusPort"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_GetRemoteAddress(IntPtr pServer, IntPtr connId, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder lpszAddress, ref int piAddressLen, ref ushort pusPort);


    /// <summary>
    /// 设置 Socket 缓存对象锁定时间（毫秒，在锁定期间该 Socket 缓存对象不能被获取使用）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwFreeSocketObjLockTime"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Server_SetFreeSocketObjLockTime(IntPtr pServer, uint dwFreeSocketObjLockTime);

    /// <summary>
    /// 设置 Socket 缓存池大小（通常设置为平均并发连接数量的 1/3 - 1/2）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwFreeSocketObjPool"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Server_SetFreeSocketObjPool(IntPtr pServer, uint dwFreeSocketObjPool);

    /// <summary>
    /// 设置内存块缓存池大小（通常设置为 Socket 缓存池大小的 2 - 3 倍）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwFreeBufferObjPool"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Server_SetFreeBufferObjPool(IntPtr pServer, uint dwFreeBufferObjPool);

    /// <summary>
    /// 设置 Socket 缓存池回收阀值（通常设置为 Socket 缓存池大小的 3 倍）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwFreeSocketObjHold"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Server_SetFreeSocketObjHold(IntPtr pServer, uint dwFreeSocketObjHold);

    /// <summary>
    /// 设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwFreeBufferObjHold"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Server_SetFreeBufferObjHold(IntPtr pServer, uint dwFreeBufferObjHold);

    /// <summary>
    /// 设置最大连接数（组件会根据设置值预分配内存，因此需要根据实际情况设置，不宜过大）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwMaxConnectionCount"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Server_SetMaxConnectionCount(IntPtr pServer, uint dwWMaxConnectionCount);

    /// <summary>
    /// 设置工作线程数量（通常设置为 2 * CPU + 2）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwWorkerThreadCount"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Server_SetWorkerThreadCount(IntPtr pServer, uint dwWorkerThreadCount);

    /// <summary>
    /// 设置是否标记静默时间（设置为 TRUE 时 DisconnectSilenceConnections() 和 GetSilencePeriod() 才有效，默认：FALSE）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="bMarkSilence"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Server_SetMarkSilence(IntPtr pServer, bool bMarkSilence);

    /// <summary>
    /// 获取 Socket 缓存对象锁定时间
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Server_GetFreeSocketObjLockTime(IntPtr pServer);

    /// <summary>
    /// 获取 Socket 缓存池大小
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Server_GetFreeSocketObjPool(IntPtr pServer);

    /// <summary>
    /// 获取内存块缓存池大小
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Server_GetFreeBufferObjPool(IntPtr pServer);

    /// <summary>
    /// 获取 Socket 缓存池回收阀值
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Server_GetFreeSocketObjHold(IntPtr pServer);

    /// <summary>
    /// 获取内存块缓存池回收阀值
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Server_GetFreeBufferObjHold(IntPtr pServer);

    /// <summary>
    /// 获取最大连接数
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Server_GetMaxConnectionCount(IntPtr pServer);

    /// <summary>
    /// 获取工作线程数量
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Server_GetWorkerThreadCount(IntPtr pServer);

    /// <summary>
    /// 检测是否标记静默时间
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Server_IsMarkSilence(IntPtr pServer);

    /**********************************************************************************/
    /***************************** TCP Server 操作方法 *****************************/

    /// <summary>
    ///  名称：发送小文件
    ///  描述：向指定连接发送 4096 KB 以下的小文件
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="lpszFileName">文件路径</param>
    /// <param name="pHead">头部附加数据</param>
    /// <param name="pTail">尾部附加数据</param>
    /// <returns>TRUE.成功 FALSE	-- 失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern bool HP_TcpServer_SendSmallFile(IntPtr pServer, IntPtr connId, string lpszFileName, ref WSABUF pHead, ref WSABUF pTail);

    /**********************************************************************************/
    /***************************** TCP Server 属性访问方法 *****************************/

    /// <summary>
    /// 设置 Accept 预投递数量（根据负载调整设置，Accept 预投递数量越大则支持的并发连接请求越多）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwAcceptSocketCount"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpServer_SetAcceptSocketCount(IntPtr pServer, uint dwAcceptSocketCount);

    /// <summary>
    /// 设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为 1024 的倍数）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwSocketBufferSize"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpServer_SetSocketBufferSize(IntPtr pServer, uint dwSocketBufferSize);

    /// <summary>
    /// 设置监听 Socket 的等候队列大小（根据并发连接数量调整设置）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwSocketListenQueue"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpServer_SetSocketListenQueue(IntPtr pServer, uint dwSocketListenQueue);

    /// <summary>
    /// 设置心跳包间隔（毫秒，0 则不发送心跳包）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwKeepAliveTime"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpServer_SetKeepAliveTime(IntPtr pServer, uint dwKeepAliveTime);

    /// <summary>
    /// 设置心跳确认包检测间隔（毫秒，0 不发送心跳包，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwKeepAliveInterval"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpServer_SetKeepAliveInterval(IntPtr pServer, uint dwKeepAliveInterval);


    /// <summary>
    /// 获取 Accept 预投递数量
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_TcpServer_GetAcceptSocketCount(IntPtr pServer);

    /// <summary>
    /// 获取通信数据缓冲区大小
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_TcpServer_GetSocketBufferSize(IntPtr pServer);

    /// <summary>
    /// 获取监听 Socket 的等候队列大小
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_TcpServer_GetSocketListenQueue(IntPtr pServer);

    /// <summary>
    /// 获取心跳检查次数
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_TcpServer_GetKeepAliveTime(IntPtr pServer);

    /// <summary>
    /// 获取心跳检查间隔
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_TcpServer_GetKeepAliveInterval(IntPtr pServer);


    /**********************************************************************************/
    /***************************** UDP Server 属性访问方法 *****************************/

    /// <summary>
    /// 设置数据报文最大长度（建议在局域网环境下不超过 1472 字节，在广域网环境下不超过 548 字节）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwMaxDatagramSize"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_UdpServer_SetMaxDatagramSize(IntPtr pServer, uint dwMaxDatagramSize);

    /// <summary>
    /// 获取数据报文最大长度
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_UdpServer_GetMaxDatagramSize(IntPtr pServer);

    /// <summary>
    /// 设置 Receive 预投递数量（根据负载调整设置，Receive 预投递数量越大则丢包概率越小） 
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwPostReceiveCount"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_UdpServer_SetPostReceiveCount(IntPtr pServer, uint dwPostReceiveCount);

    /// <summary>
    /// 获取 Receive 预投递数量
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_UdpServer_GetPostReceiveCount(IntPtr pServer);

    /// <summary>
    /// 设置监测包尝试次数（0 则不发送监测跳包，如果超过最大尝试次数则认为已断线）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwMaxDatagramSize"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_UdpServer_SetDetectAttempts(IntPtr pServer, uint dwMaxDatagramSize);

    /// <summary>
    /// 设置监测包发送间隔（秒，0 不发送监测包）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwMaxDatagramSize"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_UdpServer_SetDetectInterval(IntPtr pServer, uint dwMaxDatagramSize);

    /// <summary>
    /// 获取心跳检查次数
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_UdpServer_GetDetectAttempts(IntPtr pServer);

    /// <summary>
    /// 获取心跳检查间隔
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_UdpServer_GetDetectInterval(IntPtr pServer);
    /******************************************************************************/
    /***************************** Client 组件操作方法 *****************************/

    /// <summary>
    /// 启动客户端通信组件并连接服务端，启动完成后可开始收发数据
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="pszRemoteAddress">服务端地址</param>
    /// <param name="usPort">服务端端口</param>
    /// <param name="bAsyncConnect">是否采用异步 Connnect</param>
    /// <returns>失败，可通过 HP_Client_GetLastError() 获取错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode)]
    public static extern bool HP_Client_Start(IntPtr pClient, string pszRemoteAddress, ushort usPort, bool bAsyncConnect);


    /// <summary>
    /// 启动通信组件（并指定绑定地址）, 启动客户端通信组件并连接服务端，启动完成后可开始收发数据
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="lpszRemoteAddress">服务端地址</param>
    /// <param name="usPort">服务端端口</param>
    /// <param name="bAsyncConnect">是否采用异步 Connect</param>
    /// <param name="lpszBindAddress">绑定地址（默认：nullptr，TcpClient/UdpClient -> 不执行绑定操作，UdpCast 绑定 -> 0.0.0.0）</param>
    /// <returns>失败，可通过 HP_Client_GetLastError() 获取错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode)]
    public static extern bool HP_Client_StartWithBindAddress(IntPtr pClient, string lpszRemoteAddress, ushort usPort, bool bAsyncConnect, string lpszBindAddress);

    /// <summary>
    /// 名称：启动通信组件（并指定绑定地址）
    /// 描述：启动客户端通信组件并连接服务端，启动完成后可开始收发数据
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="lpszRemoteAddress">服务端地址</param>
    /// <param name="usPort">服务端端口</param>
    /// <param name="bAsyncConnect">是否采用异步 Connect</param>
    /// <param name="lpszBindAddress">绑定地址（默认：nullptr，TcpClient/UdpClient -> 不执行绑定操作，UdpCast 绑定 -> 0.0.0.0）</param>
    /// <param name="usLocalPort">本地端口（默认：0）</param>
    /// <returns>失败，可通过 HP_Client_GetLastError() 获取错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode)]
    public static extern bool HP_Client_StartWithBindAddressAndLocalPort(IntPtr pClient, string lpszRemoteAddress, ushort usPort, bool bAsyncConnect, string lpszBindAddress, ushort usLocalPort);


    /// <summary>
    /// 关闭客户端通信组件，关闭完成后断开与服务端的连接并释放所有资源
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns>失败，可通过 HP_Client_GetLastError() 获取错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Client_Stop(IntPtr pClient);

    /// <summary>
    /// 用户通过该方法向服务端发送数据
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="connId">连接 ID（保留参数，目前该参数并未使用）</param>
    /// <param name="pBuffer">发送数据缓冲区</param>
    /// <param name="length">发送数据长度</param>
    /// <returns>失败，可通过 HP_Client_GetLastError() 获取错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
    public static extern bool HP_Client_Send(IntPtr pClient, byte[] pBuffer, int length);

    /// <summary>
    /// 用户通过该方法向服务端发送数据
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="connId">连接 ID（保留参数，目前该参数并未使用）</param>
    /// <param name="pBuffer">发送数据缓冲区</param>
    /// <param name="length">发送数据长度</param>
    /// <returns>失败，可通过 HP_Client_GetLastError() 获取错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern bool HP_Client_Send(IntPtr pClient, IntPtr pBuffer, int length);

    /// <summary>
    /// 用户通过该方法向服务端发送数据
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="pBuffer"></param>
    /// <param name="length"></param>
    /// <param name="iOffset">针对pBuffer的偏移</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
    public static extern bool HP_Client_SendPart(IntPtr pClient, byte[] pBuffer, int length, int iOffset);

    /// <summary>
    /// 用户通过该方法向服务端发送数据
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="pBuffer"></param>
    /// <param name="length"></param>
    /// <param name="iOffset">针对pBuffer的偏移</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern bool HP_Client_SendPart(IntPtr pClient, IntPtr pBuffer, int length, int iOffset);


    /// <summary>
    /// 发送多组数据
    /// 向服务端发送多组数据
    /// TCP - 顺序发送所有数据包 
    /// UDP - 把所有数据包组合成一个数据包发送（数据包的总长度不能大于设置的 UDP 包最大长度） 
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="pBuffers">发送缓冲区数组</param>
    /// <param name="iCount">发送缓冲区数目</param>
    /// <returns>TRUE.成功,FALSE.失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern bool HP_Client_SendPackets(IntPtr pClient, WSABUF[] pBuffers, int iCount);


    /// <summary>
    /// 名称：暂停/恢复接收
    /// 描述：暂停/恢复某个连接的数据接收工作
    /// </summary>
    /// <param name="pClient">TRUE - 暂停, FALSE - 恢复</param>
    /// <param name="bPause">TRUE - 暂停, FALSE - 恢复</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern bool HP_Client_PauseReceive(IntPtr pClient, int bPause);

    /******************************************************************************/
    /***************************** Client 属性访问方法 *****************************/

    /// <summary>
    /// 设置连接的附加数据
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="pExtra"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Client_SetExtra(IntPtr pClient, IntPtr pExtra);


    /// <summary>
    /// 获取连接的附加数据
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr HP_Client_GetExtra(IntPtr pClient);


    /// <summary>
    /// 检测是否为安全连接（SSL/HTTPS）
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Client_IsSecure(IntPtr pClient);


    /// <summary>
    /// 检查通信组件是否已启动
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Client_HasStarted(IntPtr pClient);

    /// <summary>
    /// 查看通信组件当前状态
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern ServiceState HP_Client_GetState(IntPtr pClient);

    /// <summary>
    /// 获取最近一次失败操作的错误代码
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern SocketError HP_Client_GetLastError(IntPtr pClient);

    /// <summary>
    /// 获取最近一次失败操作的错误描述
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr HP_Client_GetLastErrorDesc(IntPtr pClient);

    /// <summary>
    /// 获取该组件对象的连接 ID
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr HP_Client_GetConnectionID(IntPtr pClient);

    /// <summary>
    /// 获取 Client Socket 的地址信息
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="lpszAddress"></param>
    /// <param name="piAddressLen"></param>
    /// <param name="pusPort"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Client_GetLocalAddress(IntPtr pClient, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder lpszAddress, ref int piAddressLen, ref ushort pusPort);

    /// <summary>
    /// 获取连接的远程主机信息
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="lpszHost"></param>
    /// <param name="piHostLen"></param>
    /// <param name="pusPort"></param>
    /// <returns></returns>

    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Client_GetRemoteHost(IntPtr pClient, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder lpszHost, ref int piHostLen, ref ushort pusPort);


    /// <summary>
    /// 获取连接中未发出数据的长度
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="piPending"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Client_GetPendingDataLength(IntPtr pClient, ref int piPending);

    /// <summary>
    /// 获取连接的数据接收状态
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="pbPaused"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Client_IsPauseReceive(IntPtr pClient, ref int pbPaused);

    /// <summary>
    /// 检测是否有效连接
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Client_IsConnected(IntPtr pClient);


    /// <summary>
    /// 设置内存块缓存池大小（通常设置为 -> PUSH 模型：5 - 10；PULL 模型：10 - 20 ）
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="dwFreeBufferPoolSize"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Client_SetFreeBufferPoolSize(IntPtr pClient, uint dwFreeBufferPoolSize);

    /// <summary>
    /// 设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍）
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="dwFreeBufferPoolHold"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Client_SetFreeBufferPoolHold(IntPtr pClient, uint dwFreeBufferPoolHold);

    /// <summary>
    /// 获取内存块缓存池大小
    /// </summary>
    /// <param name="pClient"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Client_GetFreeBufferPoolSize(IntPtr pClient);

    /// <summary>
    /// 获取内存块缓存池回收阀值
    /// </summary>
    /// <param name="pClient"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Client_GetFreeBufferPoolHold(IntPtr pClient);
    /**********************************************************************************/
    /***************************** TCP Client 操作方法 *****************************/

    /// <summary>
    ///  名称：发送小文件
    ///  描述：向指定连接发送 4096 KB 以下的小文件
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="lpszFileName">文件路径</param>
    /// <param name="pHead">头部附加数据</param>
    /// <param name="pTail">尾部附加数据</param>
    /// <returns>TRUE.成功 FALSE	-- 失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern bool HP_TcpClient_SendSmallFile(IntPtr pClient, string lpszFileName, ref WSABUF pHead, ref WSABUF pTail);

    /**********************************************************************************/
    /***************************** TCP Client 属性访问方法 *****************************/

    /// <summary>
    /// 设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为：(N * 1024) - sizeof(TBufferObj)）
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="dwSocketBufferSize"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpClient_SetSocketBufferSize(IntPtr pClient, uint dwSocketBufferSize);

    /// <summary>
    /// 设置心跳包间隔（毫秒，0 则不发送心跳包）
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="dwKeepAliveTime"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpClient_SetKeepAliveTime(IntPtr pClient, uint dwKeepAliveTime);

    /// <summary>
    /// 设置心跳确认包检测间隔（毫秒，0 不发送心跳包，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线）
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="dwKeepAliveInterval"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpClient_SetKeepAliveInterval(IntPtr pClient, uint dwKeepAliveInterval);

    /// <summary>
    /// 获取通信数据缓冲区大小
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_TcpClient_GetSocketBufferSize(IntPtr pClient);

    /// <summary>
    /// 获取心跳检查次数
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_TcpClient_GetKeepAliveTime(IntPtr pClient);

    /// <summary>
    /// 获取心跳检查间隔
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_TcpClient_GetKeepAliveInterval(IntPtr pClient);

    /**********************************************************************************/
    /***************************** UDP Client 属性访问方法 *****************************/

    /// <summary>
    /// 设置数据报文最大长度（建议在局域网环境下不超过 1472 字节，在广域网环境下不超过 548 字节）
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="dwMaxDatagramSize"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_UdpClient_SetMaxDatagramSize(IntPtr pClient, uint dwMaxDatagramSize);

    /// <summary>
    /// 获取数据报文最大长度
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_UdpClient_GetMaxDatagramSize(IntPtr pClient);

    /// <summary>
    /// 设置监测包尝试次数（0 则不发送监测跳包，如果超过最大尝试次数则认为已断线
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="dwDetectAttempts"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_UdpClient_SetDetectAttempts(IntPtr pClient, uint dwDetectAttempts);

    /// <summary>
    /// 设置监测包发送间隔（秒，0 不发送监测包）
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="dwDetectInterval"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_UdpClient_SetDetectInterval(IntPtr pClient, uint dwDetectInterval);

    /// <summary>
    /// 获取心跳检查次数
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_UdpClient_GetDetectAttempts(IntPtr pClient);

    /// <summary>
    /// 获取心跳检查间隔
    /// </summary>
    /// <param name="pClient"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_UdpClient_GetDetectInterval(IntPtr pClient);


    /**********************************************************************************/
    /****************************** UDP Cast 属性访问方法 ******************************/
    
    /// <summary>
    /// 设置数据报文最大长度（建议在局域网环境下不超过 1472 字节，在广域网环境下不超过 548 字节
    /// </summary>
    /// <param name="pCast"></param>
    /// <param name="dwMaxDatagramSize"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_UdpCast_SetMaxDatagramSize(IntPtr pCast, uint dwMaxDatagramSize);

    /// <summary>
    /// 获取数据报文最大长度
    /// </summary>
    /// <param name="pCast"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_UdpCast_GetMaxDatagramSize(IntPtr pCast);
    
    /// <summary>
    /// 获取当前数据报的远程地址信息（通常在 OnReceive 事件中调用）
    /// </summary>
    /// <param name="pCast"></param>
    /// <param name="lpszAddress"></param>
    /// <param name="piAddressLen"></param>
    /// <param name="pusPort"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_UdpCast_GetRemoteAddress(IntPtr pCast, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder lpszAddress, int piAddressLen, ref ushort pusPort);

    /// <summary>
    /// 设置是否启用地址重用机制（默认：不启用）
    /// </summary>
    /// <param name="pCast"></param>
    /// <param name="bReuseAddress"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_UdpCast_SetReuseAddress(IntPtr pCast, bool bReuseAddress);

    /// <summary>
    /// 检测是否启用地址重用机制
    /// </summary>
    /// <param name="pCast"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_UdpCast_IsReuseAddress(IntPtr pCast);

    /// <summary>
    /// 设置传播模式（组播或广播）
    /// </summary>
    /// <param name="pCast"></param>
    /// <param name="enCastMode"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_UdpCast_SetCastMode(IntPtr pCast, CastMode enCastMode);

    /// <summary>
    /// 获取传播模式
    /// </summary>
    /// <param name="pCast"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern CastMode HP_UdpCast_GetCastMode(IntPtr pCast);

    /// <summary>
    /// 设置组播报文的 TTL（0 - 255）
    /// </summary>
    /// <param name="pCast"></param>
    /// <param name="iMCTtl"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_UdpCast_SetMultiCastTtl(IntPtr pCast, int iMCTtl);

    /// <summary>
    /// 获取组播报文的 TTL
    /// </summary>
    /// <param name="pCast"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern int HP_UdpCast_GetMultiCastTtl(IntPtr pCast);

    /// <summary>
    /// 设置是否启用组播环路（TRUE or FALSE）
    /// </summary>
    /// <param name="pCast"></param>
    /// <param name="bMCLoop"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_UdpCast_SetMultiCastLoop(IntPtr pCast, bool bMCLoop);

    /// <summary>
    /// 检测是否启用组播环路
    /// </summary>
    /// <param name="pCast"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_UdpCast_IsMultiCastLoop(IntPtr pCast);

    /**************************************************************************/
    /***************************** Agent 操作方法 *****************************/

    /// <summary>
    /// 启动通信组件
    /// 启动通信代理组件，启动完成后可开始连接远程服务器
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="pszBindAddress">监听地址</param>
    /// <param name="bAsyncConnect">是否采用异步 Connect</param>
    /// <returns>失败，可通过 GetLastError() 获取错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode)]
    public static extern bool HP_Agent_Start(IntPtr pAgent, string pszBindAddress, bool bAsyncConnect);

    /// <summary>
    /// 关闭通信组件
    /// 关闭通信组件，关闭完成后断开所有连接并释放所有资源
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns>-- 失败，可通过 GetLastError() 获取错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_Stop(IntPtr pAgent);

    /// <summary>
    /// 连接服务器
    /// 连接服务器，连接成功后 IAgentListener 会接收到 OnConnect() 事件
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="pszBindAddress">服务端地址</param>
    /// <param name="usPort">服务端端口</param>
    /// <param name="pconnId">传出连接 ID</param>
    /// <returns>失败，可通过 SYS_GetLastError() 获取 Windows 错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern bool HP_Agent_Connect(IntPtr pAgent, string pszBindAddress, ushort usPort, ref IntPtr pconnId);

    /// <summary>
    /// 名称：连接服务器 
    /// 描述：连接服务器，连接成功后 IAgentListener 会接收到 OnConnect() / OnHandShake() 事件
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="lpszRemoteAddress">服务端地址</param>
    /// <param name="usPort">服务端端口</param>
    /// <param name="pdwConnID">传出连接</param>
    /// <param name="pExtra">连接附加数据（默认：nullptr）</param>
    /// <returns>失败，可通过函数 SYS_GetLastError() 获取 Windows 错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern bool HP_Agent_ConnectWithExtra(IntPtr pAgent, string lpszRemoteAddress, ushort usPort, ref IntPtr pdwConnID, IntPtr pExtra);

    /// <summary>
    /// 名称：连接服务器
    /// 描述：连接服务器，连接成功后 IAgentListener 会接收到 OnConnect() / OnHandShake() 事件
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="lpszRemoteAddress">服务端地址</param>
    /// <param name="usPort">服务端端口</param>
    /// <param name="pdwConnID">连接 ID（默认：nullptr，不获取连接 ID）</param>
    /// <param name="usLocalPort">本地端口（默认：0）</param>
    /// <returns>失败，可通过函数 SYS_GetLastError() 获取 Windows 错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern bool HP_Agent_ConnectWithLocalPort(IntPtr pAgent, string lpszRemoteAddress, ushort usPort, ref IntPtr pdwConnID, ushort usLocalPort);


    /// <summary>
    /// 名称：连接服务器
    /// 描述：连接服务器，连接成功后 IAgentListener 会接收到 OnConnect() / OnHandShake() 事件
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="lpszRemoteAddress">服务端地址</param>
    /// <param name="usPort">服务端端口</param>
    /// <param name="pdwConnID">连接 ID（默认：nullptr，不获取连接 ID）</param>
    /// <param name="pExtra">连接附加数据（默认：nullptr）</param>
    /// <param name="usLocalPort">本地端口（默认：0）</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern bool HP_Agent_ConnectWithExtraAndLocalPort(IntPtr pAgent, string lpszRemoteAddress, ushort usPort, ref IntPtr pdwConnID, IntPtr pExtra, ushort usLocalPort);
 

    /// <summary>
    /// 发送数据
    /// 用户通过该方法向指定连接发送数据
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="pBuffer">发送数据缓冲区</param>
    /// <param name="length">发送数据长度</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Ansi, SetLastError = true)]
    public static extern bool HP_Agent_Send(IntPtr pAgent, IntPtr connId, byte[] pBuffer, int length);

    /// <summary>
    /// 发送数据
    /// 用户通过该方法向指定连接发送数据
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="pBuffer">发送数据缓冲区</param>
    /// <param name="length">发送数据长度</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern bool HP_Agent_Send(IntPtr pAgent, IntPtr connId, IntPtr pBuffer, int length);

    /// <summary>
    /// 发送数据
    /// 用户通过该方法向指定连接发送数据
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="connId"></param>
    /// <param name="pBuffer"></param>
    /// <param name="length"></param>
    /// <param name="iOffset">针对pBuffer的偏移</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern bool HP_Agent_SendPart(IntPtr pAgent, IntPtr connId, byte[] pBuffer, int length, int iOffset);

    /// <summary>
    /// 发送数据
    /// 用户通过该方法向指定连接发送数据
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="connId"></param>
    /// <param name="pBuffer"></param>
    /// <param name="length"></param>
    /// <param name="iOffset">针对pBuffer的偏移</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern bool HP_Agent_SendPart(IntPtr pAgent, IntPtr connId, IntPtr pBuffer, int length, int iOffset);


    /// <summary>
    /// 发送多组数据
    /// 向指定连接发送多组数据
    /// TCP - 顺序发送所有数据包 
    /// UDP - 把所有数据包组合成一个数据包发送（数据包的总长度不能大于设置的 UDP 包最大长度） 
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="pBuffers">发送缓冲区数组</param>
    /// <param name="iCount">发送缓冲区数目</param>
    /// <returns>TRUE.成功,FALSE	.失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern bool HP_Agent_SendPackets(IntPtr pAgent, IntPtr connId, WSABUF[] pBuffers, int iCount);

    /// <summary>
    /// 名称：暂停/恢复接收 
    /// 描述：暂停/恢复某个连接的数据接收工作
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="dwConnID"></param>
    /// <param name="bPause">TRUE - 暂停, FALSE - 恢复</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern bool HP_Agent_PauseReceive(IntPtr pAgent, IntPtr dwConnID, bool bPause);

    /// <summary>
    /// 断开某个连接
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="bForce">是否强制断开连接</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_Disconnect(IntPtr pAgent, IntPtr connId, bool bForce);

    /// <summary>
    /// 断开超过指定时长的连接
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="dwPeriod">时长（毫秒）</param>
    /// <param name="bForce">是否强制断开连接</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_DisconnectLongConnections(IntPtr pAgent, uint dwPeriod, bool bForce);

    /// <summary>
    /// 断开超过指定时长的静默连接
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwPeriod">时长（毫秒）</param>
    /// <param name="bForce">是否强制断开连接</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_DisconnectSilenceConnections(IntPtr pAgent, uint dwPeriod, bool bForce);

    /******************************************************************************/
    /***************************** Agent 操作方法 *****************************/

    /// <summary>
    ///  名称：发送小文件
    ///  描述：向指定连接发送 4096 KB 以下的小文件
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="lpszFileName">文件路径</param>
    /// <param name="pHead">头部附加数据</param>
    /// <param name="pTail">尾部附加数据</param>
    /// <returns>TRUE.成功 FALSE	-- 失败，可通过 Windows API 函数 ::GetLastError() 获取 Windows 错误代码</returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern bool HP_TcpAgent_SendSmallFile(IntPtr pAgent, IntPtr connId, string lpszFileName, ref WSABUF pHead, ref WSABUF pTail);

    /******************************************************************************/
    /***************************** Agent 属性访问方法 *****************************/

    /// <summary>
    /// 设置数据发送策略
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="enSendPolicy"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Agent_SetSendPolicy(IntPtr pAgent, SendPolicy enSendPolicy);

    /// <summary>
    /// 获取数据发送策略
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern SendPolicy HP_Agent_GetSendPolicy(IntPtr pAgent);

    /// <summary>
    /// 设置连接的附加数据
    /// 是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序只身决定
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="pExtra">数据</param>
    /// <returns>FALSE    -- 失败（无效的连接 ID）</returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_SetConnectionExtra(IntPtr pAgent, IntPtr connId, IntPtr pExtra);

    /// <summary>
    /// 获取连接的附加数据
    /// 是否为连接绑定附加数据或者绑定什么样的数据，均由应用程序只身决定
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="connId"></param>
    /// <param name="pExtra"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_GetConnectionExtra(IntPtr pAgent, IntPtr connId, ref IntPtr pExtra);

    /// <summary>
    /// 检测是否为安全连接（SSL/HTTPS）
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_IsSecure(IntPtr pAgent);

    /// <summary>
    /// 检查通信组件是否已启动
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_HasStarted(IntPtr pAgent);

    /// <summary>
    /// 查看通信组件当前状态
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern ServiceState HP_Agent_GetState(IntPtr pAgent);

    /// <summary>
    /// 获取连接数
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Agent_GetConnectionCount(IntPtr pAgent);

    /// <summary>
    /// 获取所有连接的 CONNID
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="pIDs"></param>
    /// <param name="pdwCount"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_GetAllConnectionIDs(IntPtr pAgent, IntPtr[] pIDs, ref uint pdwCount);

    /// <summary>
    /// 获取某个连接时长（毫秒）
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="connId"></param>
    /// <param name="pdwPeriod"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_GetConnectPeriod(IntPtr pAgent, IntPtr connId, ref uint pdwPeriod);

    /// <summary>
    /// 获取某个连接静默时间（毫秒）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId"></param>
    /// <param name="pdwPeriod"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_GetSilencePeriod(IntPtr pAgent, IntPtr connId, ref uint pdwPeriod);

    /// <summary>
    /// 获取某个连接的本地地址信息
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="connId"></param>
    /// <param name="lpszAddress"></param>
    /// <param name="piAddressLen"></param>
    /// <param name="pusPort"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_GetLocalAddress(IntPtr pAgent, IntPtr connId, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder lpszAddress, ref int piAddressLen, ref ushort pusPort);

    /// <summary>
    /// 获取某个连接的远程地址信息
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="connId"></param>
    /// <param name="lpszAddress"></param>
    /// <param name="piAddressLen"></param>
    /// <param name="pusPort"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_GetRemoteAddress(IntPtr pAgent, IntPtr connId, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder lpszAddress, ref int piAddressLen, ref ushort pusPort);

    /// <summary>
    /// 获取某个连接的远程主机信息
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="dwConnID"></param>
    /// <param name="lpszAddress"></param>
    /// <param name="piHostLen"></param>
    /// <param name="pusPort"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_GetRemoteHost(IntPtr pAgent, IntPtr dwConnID, [MarshalAs(UnmanagedType.LPWStr)]StringBuilder lpszAddress, ref int piHostLen, ref ushort pusPort);


    /// <summary>
    /// 获取最近一次失败操作的错误代码
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern SocketError HP_Agent_GetLastError(IntPtr pAgent);

    /// <summary>
    /// 获取最近一次失败操作的错误描述
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr HP_Agent_GetLastErrorDesc(IntPtr pAgent);

    /// <summary>
    /// 获取连接中未发出数据的长度
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="connId"></param>
    /// <param name="piPending"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_GetPendingDataLength(IntPtr pAgent, IntPtr connId, ref int piPending);

    /// <summary>
    /// 获取连接的数据接收状态
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="dwConnID"></param>
    /// <param name="pbPaused"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_IsPauseReceive(IntPtr pAgent, IntPtr dwConnID, ref int pbPaused);

    /// <summary>
    /// 检测是否有效连接
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="dwConnID"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_IsConnected(IntPtr pAgent, IntPtr dwConnID);


    /// <summary>
    /// 设置 Socket 缓存对象锁定时间（毫秒，在锁定期间该 Socket 缓存对象不能被获取使用）
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="dwFreeSocketObjLockTime"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Agent_SetFreeSocketObjLockTime(IntPtr pAgent, uint dwFreeSocketObjLockTime);

    /// <summary>
    /// 设置 Socket 缓存池大小（通常设置为平均并发连接数量的 1/3 - 1/2）
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="dwFreeSocketObjPool"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Agent_SetFreeSocketObjPool(IntPtr pAgent, uint dwFreeSocketObjPool);

    /// <summary>
    /// 设置内存块缓存池大小（通常设置为 Socket 缓存池大小的 2 - 3 倍）
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="dwFreeBufferObjPool"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Agent_SetFreeBufferObjPool(IntPtr pAgent, uint dwFreeBufferObjPool);

    /// <summary>
    /// 设置 Socket 缓存池回收阀值（通常设置为 Socket 缓存池大小的 3 倍）
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="dwFreeSocketObjHold"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Agent_SetFreeSocketObjHold(IntPtr pAgent, uint dwFreeSocketObjHold);

    /// <summary>
    /// 设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍）
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="dwFreeBufferObjHold"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Agent_SetFreeBufferObjHold(IntPtr pAgent, uint dwFreeBufferObjHold);

    /// <summary>
    /// 设置最大连接数（组件会根据设置值预分配内存，因此需要根据实际情况设置，不宜过大）
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="dwMaxConnectionCount"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Agent_SetMaxConnectionCount(IntPtr pAgent, uint dwMaxConnectionCount);

    /// <summary>
    /// 设置工作线程数量（通常设置为 2 * CPU + 2）
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="dwWorkerThreadCount"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Agent_SetWorkerThreadCount(IntPtr pAgent, uint dwWorkerThreadCount);

    /// <summary>
    /// 设置是否标记静默时间（设置为 TRUE 时 DisconnectSilenceConnections() 和 GetSilencePeriod() 才有效，默认：FALSE）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="bMarkSilence"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_Agent_SetMarkSilence(IntPtr pAgent, bool bMarkSilence);

    /// <summary>
    /// 获取 Socket 缓存对象锁定时间
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Agent_GetFreeSocketObjLockTime(IntPtr pAgent);

    /// <summary>
    /// 获取 Socket 缓存池大小
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Agent_GetFreeSocketObjPool(IntPtr pAgent);

    /// <summary>
    /// 获取内存块缓存池大小
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Agent_GetFreeBufferObjPool(IntPtr pAgent);

    /// <summary>
    /// 获取 Socket 缓存池回收阀值
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Agent_GetFreeSocketObjHold(IntPtr pAgent);

    /// <summary>
    /// 获取内存块缓存池回收阀值
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Agent_GetFreeBufferObjHold(IntPtr pAgent);

    /// <summary>
    /// 获取最大连接数
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Agent_GetMaxConnectionCount(IntPtr pAgent);

    /// <summary>
    /// 获取工作线程数量
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_Agent_GetWorkerThreadCount(IntPtr pAgent);

    /// <summary>
    /// 检测是否标记静默时间
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_Agent_IsMarkSilence(IntPtr pAgent);

    /**********************************************************************************/
    /***************************** TCP Agent 属性访问方法 *****************************/

    /// <summary>
    /// 置是否启用地址重用机制（默认：不启用）
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="bReuseAddress"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpAgent_SetReuseAddress(IntPtr pAgent, bool bReuseAddress);

    /// <summary>
    /// 检测是否启用地址重用机制
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool HP_TcpAgent_IsReuseAddress(IntPtr pAgent);

    /// <summary>
    /// 设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为 1024 的倍数）
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="dwSocketBufferSize"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpAgent_SetSocketBufferSize(IntPtr pAgent, uint dwSocketBufferSize);

    /// <summary>
    /// 设置心跳包间隔（毫秒，0 则不发送心跳包）
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="dwKeepAliveTime"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpAgent_SetKeepAliveTime(IntPtr pAgent, uint dwKeepAliveTime);

    /// <summary>
    /// 设置心跳确认包检测间隔（毫秒，0 不发送心跳包，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线）
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="dwKeepAliveInterval"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpAgent_SetKeepAliveInterval(IntPtr pAgent, uint dwKeepAliveInterval);

    /// <summary>
    /// 获取通信数据缓冲区大小
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_TcpAgent_GetSocketBufferSize(IntPtr pAgent);

    /// <summary>
    /// 获取心跳检查次数
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_TcpAgent_GetKeepAliveTime(IntPtr pAgent);

    /// <summary>
    /// 获取心跳检查间隔
    /// </summary>
    /// <param name="pAgent"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_TcpAgent_GetKeepAliveInterval(IntPtr pAgent);

    /***************************************************************************************/
    /***************************** TCP Pull Server 组件操作方法 *****************************/

    /// <summary>
    /// 抓取数据
    /// 用户通过该方法从 Socket 组件中抓取数据
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="pBuffer">数据抓取缓冲区</param>
    /// <param name="length">抓取数据长度</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern FetchResult HP_TcpPullServer_Fetch(IntPtr pServer, IntPtr connId, IntPtr pBuffer, int length);

    /// <summary>
    /// 窥探数据（不会移除缓冲区数据） 
    /// 描述：用户通过该方法从 Socket 组件中窥探数据
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="pBuffer">窥探缓冲区</param>
    /// <param name="length">窥探数据长度</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern FetchResult HP_TcpPullServer_Peek(IntPtr pServer, IntPtr connId, IntPtr pBuffer, int length);

    /***************************************************************************************/
    /***************************** TCP Pull Server 属性访问方法 *****************************/

    /***************************************************************************************/
    /***************************** TCP Pull Client 组件操作方法 *****************************/

    /// <summary>
    /// 抓取数据
    /// 用户通过该方法从 Socket 组件中抓取数据
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="pBuffer">数据抓取缓冲区</param>
    /// <param name="length">抓取数据长度</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern FetchResult HP_TcpPullClient_Fetch(IntPtr pClient, IntPtr pBuffer, int length);

    /// <summary>
    /// 名称：窥探数据（不会移除缓冲区数据）
    /// 描述：用户通过该方法从 Socket 组件中窥探数据
    /// </summary>
    /// <param name="pClient"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="pBuffer">数据抓取缓冲区</param>
    /// <param name="length">抓取数据长度</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern FetchResult HP_TcpPullClient_Peek(IntPtr pClient, IntPtr pBuffer, int length);

    /***************************************************************************************/
    /***************************** TCP Pull Client 属性访问方法 *****************************/

    /***************************************************************************************/
    /***************************** TCP Pull Agent 组件操作方法 *****************************/

    /// <summary>
    /// 抓取数据
    /// 用户通过该方法从 Socket 组件中抓取数据
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="pBuffer">数据抓取缓冲区</param>
    /// <param name="length">抓取数据长度</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern FetchResult HP_TcpPullAgent_Fetch(IntPtr pAgent, IntPtr connId, IntPtr pBuffer, int length);

    /// <summary>
    /// 名称：窥探数据（不会移除缓冲区数据） 
    /// 描述：用户通过该方法从 Socket 组件中窥探数据
    /// </summary>
    /// <param name="pAgent"></param>
    /// <param name="connId">连接 ID</param>
    /// <param name="pBuffer">数据抓取缓冲区</param>
    /// <param name="length">抓取数据长度</param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern FetchResult HP_TcpPullAgent_Peek(IntPtr pAgent, IntPtr connId, IntPtr pBuffer, int length);

    /***************************************************************************************/
    /***************************** TCP Pull Agent 属性访问方法 *****************************/

    /***************************************************************************************/

    /***************************************************************************************/
    /***************************** TCP Pack Server 组件操作方法 *****************************/

    /***************************************************************************************/
    /***************************** TCP Pack Server 属性访问方法 *****************************/

    /// <summary>
    /// 设置数据包最大长度（有效数据包最大长度不能超过 4194303/0x3FFFFF  字节，默认：262144/0x40000）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwMaxPackSize">有效数据包最大长度不能超过 4194303/0x3FFFFF 字节，默认：262144/0x40000</param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpPackServer_SetMaxPackSize(IntPtr pServer, uint dwMaxPackSize);

    /// <summary>
    /// 设置包头标识（有效包头标识取值范围 0 ~ 1023/0x3FF，当包头标识为 0 时不校验包头，默认：0）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="usPackHeaderFlag">有效包头标识取值范围 0 ~ 1023/0x3FF，当包头标识为 0 时不校验包头，默认：0</param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpPackServer_SetPackHeaderFlag(IntPtr pServer, ushort usPackHeaderFlag);

    /// <summary>
    /// 获取数据包最大长度
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_TcpPackServer_GetMaxPackSize(IntPtr pServer);

    /// <summary>
    /// 获取包头标识
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern ushort HP_TcpPackServer_GetPackHeaderFlag(IntPtr pServer);
    /***************************************************************************************/
    /***************************** TCP Pack Agent 组件操作方法 *****************************/

    /***************************************************************************************/
    /***************************** TCP Pack Agent 属性访问方法 *****************************/

    /// <summary>
    /// 设置数据包最大长度（有效数据包最大长度不能超过 524287/0x7FFFF 字节，默认：262144/0x40000）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwMaxPackSize">有效数据包最大长度不能超过 524287/0x7FFFF 字节，默认：262144/0x40000</param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpPackAgent_SetMaxPackSize(IntPtr pAgent, uint dwMaxPackSize);

    /// <summary>
    /// 设置包头标识（有效包头标识取值范围 0 ~ 8191/0x1FFF，当包头标识为 0 时不校验包头，默认：0）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="usPackHeaderFlag">有效包头标识取值范围 0 ~ 8191/0x1FFF，当包头标识为 0 时不校验包头，默认：0</param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpPackAgent_SetPackHeaderFlag(IntPtr pAgent, ushort usPackHeaderFlag);

    /// <summary>
    /// 获取数据包最大长度
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_TcpPackAgent_GetMaxPackSize(IntPtr pAgent);

    /// <summary>
    /// 获取包头标识
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern ushort HP_TcpPackAgent_GetPackHeaderFlag(IntPtr pAgent);
    /***************************************************************************************/
    /***************************** TCP Pack Client 组件操作方法 *****************************/

    /***************************************************************************************/
    /***************************** TCP Pack Client 属性访问方法 *****************************/
    /// <summary>
    /// 设置数据包最大长度（有效数据包最大长度不能超过 524287/0x7FFFF 字节，默认：262144/0x40000）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="dwMaxPackSize">有效数据包最大长度不能超过 524287/0x7FFFF 字节，默认：262144/0x40000</param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpPackClient_SetMaxPackSize(IntPtr pClient, uint dwMaxPackSize);

    /// <summary>
    /// 设置包头标识（有效包头标识取值范围 0 ~ 8191/0x1FFF，当包头标识为 0 时不校验包头，默认：0）
    /// </summary>
    /// <param name="pServer"></param>
    /// <param name="usPackHeaderFlag">有效包头标识取值范围 0 ~ 8191/0x1FFF，当包头标识为 0 时不校验包头，默认：0</param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void HP_TcpPackClient_SetPackHeaderFlag(IntPtr pClient, ushort usPackHeaderFlag);

    /// <summary>
    /// 获取数据包最大长度
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_TcpPackClient_GetMaxPackSize(IntPtr pClient);

    /// <summary>
    /// 获取包头标识
    /// </summary>
    /// <param name="pServer"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern ushort HP_TcpPackClient_GetPackHeaderFlag(IntPtr pClient);
    /***************************************************************************************/
    /*************************************** 其它方法 ***************************************/

    /// <summary>
    /// 获取 HPSocket 版本号（4 个字节分别为：主版本号，子版本号，修正版本号，构建编号）
    /// </summary>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern uint HP_GetHPSocketVersion();

    /// <summary>
    /// 获取版本号
    /// </summary>
    /// <returns></returns>
    public static string GetHPSocketVersion()
    {
        /*
        var ver = BitConverter.GetBytes(HP_GetHPSocketVersion());
        var major = ver[3];
        var minor = ver[2];
        var revision = ver[1];
        var build = ver[0];*/

        uint ver = HP_GetHPSocketVersion();
        var major = ver >> 24;
        var minor = (ver << 8) >> 24;
        var revision = (ver << 16) >> 24;
        var build = (ver << 24) >> 24;

        return string.Format("{0}.{1}.{2}_{3}", major, minor, revision, build);
    }

    /// <summary>
    /// 获取错误描述文本
    /// </summary>
    /// <param name="enCode"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr HP_GetSocketErrorDesc(SocketError enCode);

    /// <summary>
    /// 调用系统的 ::GetLastError() 方法获取系统错误代码
    /// </summary>
    /// <returns></returns>
    public static int SYS_GetLastError()
    {
        return Marshal.GetLastWin32Error();
    }

    /// <summary>
    /// 调用系统的 ::WSAGetLastError() 方法获取通信错误代码
    /// </summary>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern int SYS_WSAGetLastError();

    /// <summary>
    /// 调用系统的 setsockopt()
    /// </summary>
    /// <param name="sock"></param>
    /// <param name="level"></param>
    /// <param name="name"></param>
    /// <param name="val"></param>
    /// <param name="len"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern int SYS_SetSocketOption(IntPtr sock, int level, int name, IntPtr val, int len);

    /// <summary>
    /// 调用系统的 getsockopt()
    /// </summary>
    /// <param name="sock"></param>
    /// <param name="level"></param>
    /// <param name="name"></param>
    /// <param name="val"></param>
    /// <param name="len"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern int SYS_GetSocketOption(IntPtr sock, int level, int name, IntPtr val, ref int len);

    /// <summary>
    /// 调用系统的 ioctlsocket()
    /// </summary>
    /// <param name="sock"></param>
    /// <param name="cmd"></param>
    /// <param name="arg"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern int SYS_IoctlSocket(IntPtr sock, long cmd, IntPtr arg);

    /// <summary>
    /// 调用系统的 ::WSAIoctl()
    /// </summary>
    /// <param name="sock"></param>
    /// <param name="dwIoControlCode"></param>
    /// <param name="lpvInBuffer"></param>
    /// <param name="cbInBuffer"></param>
    /// <param name="lpvOutBuffer"></param>
    /// <param name="cbOutBuffer"></param>
    /// <param name="lpcbBytesReturned"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, SetLastError = true)]
    public static extern int SYS_WSAIoctl(IntPtr sock, uint dwIoControlCode, IntPtr lpvInBuffer, uint cbInBuffer,
                                          IntPtr lpvOutBuffer, uint cbOutBuffer, uint lpcbBytesReturned);

    /// <summary>
    /// 设置 socket 选项：IPPROTO_TCP -> TCP_NODELAY
    /// </summary>
    /// <param name="sock"></param>
    /// <param name="bNoDelay"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern int SYS_SSO_NoDelay(IntPtr sock, bool bNoDelay);


    /// <summary>
    /// 设置 socket 选项：SOL_SOCKET -> SO_DONTLINGER
    /// </summary>
    /// <param name="sock"></param>
    /// <param name="bDont"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern int SYS_SSO_DontLinger(IntPtr sock, bool bDont);

    /// <summary>
    /// 设置 socket 选项：SOL_SOCKET -> SO_LINGER
    /// </summary>
    /// <param name="sock"></param>
    /// <param name="l_onoff"></param>
    /// <param name="l_linger"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern int SYS_SSO_Linger(IntPtr sock, ushort l_onoff, ushort l_linger);


    /// <summary>
    /// 设置 socket 选项：SOL_SOCKET -> SO_RCVBUF
    /// </summary>
    /// <param name="sock"></param>
    /// <param name="size"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern int SYS_SSO_RecvBuffSize(IntPtr sock, int size);

    /// <summary>
    /// 设置 socket 选项：SOL_SOCKET -> SO_SNDBUF
    /// </summary>
    /// <param name="sock"></param>
    /// <param name="size"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern int SYS_SSO_SendBuffSize(IntPtr sock, int size);

    /// <summary>
    /// 设置 socket 选项：SOL_SOCKET -> SO_REUSEADDR
    /// </summary>
    /// <param name="sock"></param>
    /// <param name="bReuse"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern int SYS_SSO_ReuseAddress(IntPtr sock, bool bReuse);

    /// <summary>
    /// 获取 SOCKET 本地地址信息
    /// </summary>
    /// <param name="pSocket"></param>
    /// <param name="lpszAddress"></param>
    /// <param name="piAddressLen">传入传出值,大小最好在222.222.222.222的长度以上</param>
    /// <param name="pusPort"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool SYS_GetSocketLocalAddress(IntPtr pSocket, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder lpszAddress, ref int piAddressLen, ref ushort pusPort);

    /// <summary>
    /// 获取 SOCKET 远程地址信息
    /// </summary>
    /// <param name="pSocket"></param>
    /// <param name="lpszAddress"></param>
    /// <param name="piAddressLen">传入传出值,大小最好在222.222.222.222的长度以上</param>
    /// <param name="pusPort"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool SYS_GetSocketRemoteAddress(IntPtr pSocket, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder lpszAddress, ref int piAddressLen, ref ushort pusPort);


    /// <summary>
    /// 枚举主机 IP 地址
    /// 不要用,未测试
    /// 不要用,未测试
    /// 不要用,未测试
    /// </summary>
    /// <param name="lpszHost"></param>
    /// <param name="enType"></param>
    /// <param name="lpppIPAddr"></param>
    /// <param name="piIPAddrCount"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode)]
    public static extern bool SYS_EnumHostIPAddresses(string lpszHost, IPAddrType enType, ref IntPtr lpppIPAddr, ref int piIPAddrCount);


    /// <summary>
    /// 释放 HP_LPTIPAddr
    /// </summary>
    /// <param name="lppIPAddr"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern bool SYS_FreeHostIPAddresses(IntPtr lppIPAddr);


    /*
     * 已删除
    /// <summary>
    /// 获取 IPv4 地址
    /// </summary>
    /// <param name="lpszAddress"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode)]
    public static extern uint SYS_GetIPv4InAddr(string lpszAddress);
    */
    /// <summary>
    /// 检查字符串是否符合 IP 地址格式
    /// </summary>
    /// <param name="lpszAddress"></param>
    /// <param name="penType"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode)]
    public static extern bool SYS_IsIPAddress(string lpszAddress, ref IPAddrType penType);

    /// <summary>
    /// 通过主机名获取 IP 地址
    /// </summary>
    /// <param name="lpszHost"></param>
    /// <param name="lpszIP"></param>
    /// <param name="piIPLenth"></param>
    /// <param name="penType"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode)]
    public static extern bool SYS_GetIPAddress(string lpszHost, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder lpszIP, ref int piIPLenth, ref IPAddrType penType);

    /*
     * 已删除
    /// <summary>
    /// 通过主机名获取最优的 IP 地址
    /// </summary>
    /// <param name="lpszHost"></param>
    /// <param name="pulAddr"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH, CharSet = CharSet.Unicode)]
    public static extern bool SYS_GetOptimalIPByHostName(string lpszHost, ref uint pulAddr);
    */

    /// <summary>
    /// 64 位网络字节序转主机字节序
    /// </summary>
    /// <param name="value"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern ulong SYS_NToH64(ulong value);

    /// <summary>
    /// 64 位主机字节序转网络字节序
    /// </summary>
    /// <param name="value"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern ulong SYS_HToN64(ulong value);


    /// <summary>
    /// 分配内存
    /// </summary>
    /// <param name="value"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr SYS_Malloc(int value);

    /// <summary>
    /// 重新分配内存
    /// </summary>
    /// <param name="p"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern IntPtr SYS_Realloc(IntPtr p, int value);

    /// <summary>
    /// 释放内存
    /// </summary>
    /// <param name="p"></param>
    [DllImport(HPSOCKET_DLL_PATH)]
    public static extern void SYS_Free(IntPtr p);


    /*
     * 编码转换
    // CP_XXX -> UNICODE
    public static extern bool  SYS_CodePageToUnicode(int iCodePage, const char szSrc[], WCHAR szDest[], int& iDestLength);

    // UNICODE -> CP_XXX
    public static extern bool  SYS_UnicodeToCodePage(int iCodePage, const WCHAR szSrc[], char szDest[], int& iDestLength);

    // GBK -> UNICODE
    public static extern bool  SYS_GbkToUnicode(const char szSrc[], WCHAR szDest[], int& iDestLength);

    // UNICODE -> GBK
    public static extern bool  SYS_UnicodeToGbk(const WCHAR szSrc[], char szDest[], int& iDestLength);

    // UTF8 -> UNICODE
    public static extern bool  SYS_Utf8ToUnicode(const char szSrc[], WCHAR szDest[], int& iDestLength);

    // UNICODE -> UTF8
    public static extern bool  SYS_UnicodeToUtf8(const WCHAR szSrc[], char szDest[], int& iDestLength);

    // GBK -> UTF8
    public static extern bool  SYS_GbkToUtf8(const char szSrc[], char szDest[], int& iDestLength);

    // UTF8 -> GBK
    public static extern bool  SYS_Utf8ToGbk(const char szSrc[], char szDest[], int& iDestLength);
    */



}
