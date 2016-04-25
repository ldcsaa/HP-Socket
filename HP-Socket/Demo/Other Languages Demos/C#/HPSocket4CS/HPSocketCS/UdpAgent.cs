using System;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using HPSocketCS.SDK;

namespace HPSocketCS
{
    public class UdpAgentEvent
    {
        public delegate HandleResult OnConnectEventHandler(IntPtr connId /* IntPtr pClient */);
        public delegate HandleResult OnSendEventHandler(IntPtr connId, IntPtr pData, int length);
        public delegate HandleResult OnReceiveEventHandler(IntPtr connId, IntPtr pData, int length);
        public delegate HandleResult OnCloseEventHandler(IntPtr connId, SocketOperation enOperation, int errorCode);
        public delegate HandleResult OnShutdownEventHandler();
        public delegate HandleResult OnPrepareConnectEventHandler(IntPtr connId /* IntPtr pClient */, uint socket);
    }
    public class UdpAgent
    {
        protected IntPtr _pAgent = IntPtr.Zero;

        protected IntPtr pAgent
        {
            get
            {
                //if (_pClient == IntPtr.Zero)
                //{
                //    throw new Exception("pClient == 0");
                //}

                return _pAgent;
            }

            set
            {
                _pAgent = value;
            }
        }


        protected IntPtr pListener = IntPtr.Zero;

        /// <summary>
        /// 连接事件
        /// </summary>
        public UdpAgentEvent.OnConnectEventHandler OnConnect;
        /// <summary>
        /// 数据发送事件
        /// </summary>
        public UdpAgentEvent.OnSendEventHandler OnSend;
        /// <summary>
        /// 准备连接事件
        /// </summary>
        public UdpAgentEvent.OnPrepareConnectEventHandler OnPrepareConnect;
        /// <summary>
        /// 数据到达事件
        /// </summary>
        public UdpAgentEvent.OnReceiveEventHandler OnReceive;
        /// <summary>
        /// 连接关闭事件
        /// </summary>
        public UdpAgentEvent.OnCloseEventHandler OnClose;
        /// <summary>
        /// 服务关闭事件
        /// </summary>
        public UdpAgentEvent.OnShutdownEventHandler OnShutdown;


        protected bool IsCreate = false;

        public UdpAgent()
        {
            CreateListener();
        }

        ~UdpAgent()
        {
            Destroy();
        }

        /// <summary>
        /// 创建socket监听&服务组件
        /// </summary>
        /// <returns></returns>
        protected virtual bool CreateListener()
        {
            if (IsCreate == true || pListener != IntPtr.Zero || pAgent != IntPtr.Zero)
            {
                return false;
            }

            pListener = HPSocketSdk.Create_HP_UdpAgentListener();
            if (pListener == IntPtr.Zero)
            {
                return false;
            }

            pAgent = HPSocketSdk.Create_HP_UdpAgent(pListener);
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
        public virtual void Destroy()
        {
            Stop();

            if (pAgent != IntPtr.Zero)
            {
                HPSocketSdk.Destroy_HP_UdpAgent(pAgent);
                pAgent = IntPtr.Zero;
            }
            if (pListener != IntPtr.Zero)
            {
                HPSocketSdk.Destroy_HP_UdpAgentListener(pListener);
                pListener = IntPtr.Zero;
            }

            IsCreate = false;
        }

        /// <summary>
        /// 启动通讯组件
        /// 启动完成后可开始连接远程服务器
        /// </summary>
        /// <param name="address">绑定地址</param>
        /// <param name="async">是否异步</param>
        /// <returns></returns>
        public bool Start(string address, bool async = false)
        {
            if (string.IsNullOrEmpty(address) == true)
            {
                throw new Exception("address is null");
            }

            if (IsStarted == true)
            {
                return false;
            }

            SetCallback();

            return HPSocketSdk.HP_Agent_Start(pAgent, address, async);
        }

        /// <summary>
        /// 停止通讯组件
        /// </summary>
        /// <returns></returns>
        public bool Stop()
        {
            if (IsStarted == false)
            {
                return false;
            }
            return HPSocketSdk.HP_Agent_Stop(pAgent);
        }


        /// <summary>
        /// 连接服务器，连接成功后 IAgentListener 会接收到 OnConnect() 事件
        /// </summary>
        /// <param name="address"></param>
        /// <param name="port"></param>
        /// <param name="connId"></param>
        /// <returns></returns>
        public bool Connect(string address, ushort port, ref IntPtr connId)
        {
            return HPSocketSdk.HP_Agent_Connect(pAgent, address, port, ref connId);
        }

        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="bytes"></param>
        /// <param name="size"></param>
        /// <returns></returns>
        public bool Send(IntPtr connId, byte[] bytes, int size)
        {
            return HPSocketSdk.HP_Agent_Send(pAgent, connId, bytes, size);
        }


        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="bufferPtr"></param>
        /// <param name="size"></param>
        /// <returns></returns>
        public bool Send<T>(IntPtr connId, T obj)
        {
            byte[] buffer = StructureToByte<T>(obj);
            return Send(connId, buffer, buffer.Length);
        }

        /// <summary>
        /// 序列化对象后发送数据,序列化对象所属类必须标记[Serializable]
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="bufferPtr"></param>
        /// <param name="size"></param>
        /// <returns></returns>
        public bool SendBySerializable(IntPtr connId, object obj)
        {
            byte[] buffer = ObjectToBytes(obj);
            return Send(connId, buffer, buffer.Length);
        }

        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="bufferPtr"></param>
        /// <param name="size"></param>
        /// <returns></returns>
        public bool Send(IntPtr connId, IntPtr bufferPtr, int size)
        {
            return HPSocketSdk.HP_Agent_Send(pAgent, connId, bufferPtr, size);
        }

        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="bytes"></param>
        /// <param name="offset">针对bytes的偏移</param>
        /// <param name="size">发多大</param>
        /// <returns></returns>
        public bool Send(IntPtr connId, byte[] bytes, int offset, int size)
        {
            return HPSocketSdk.HP_Agent_SendPart(pAgent, connId, bytes, size, offset);
        }

        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="bufferPtr"></param>
        /// <param name="offset">针对bufferPtr的偏移</param>
        /// <param name="size">发多大</param>
        /// <returns></returns>
        public bool Send(IntPtr connId, IntPtr bufferPtr, int offset, int size)
        {
            return HPSocketSdk.HP_Agent_SendPart(pAgent, connId, bufferPtr, size, offset);
        }

        /// <summary>
        /// 发送多组数据
        /// 向指定连接发送多组数据
        /// TCP - 顺序发送所有数据包
        /// </summary>
        /// <param name="connId">连接 ID</param>
        /// <param name="pBuffers">发送缓冲区数组</param>
        /// <param name="iCount">发送缓冲区数目</param>
        /// <returns>TRUE.成功,FALSE.失败，可通过 SYSGetLastError() 获取 Windows 错误代码</returns>
        public bool SendPackets(IntPtr connId, WSABUF[] pBuffers, int count)
        {
            return HPSocketSdk.HP_Agent_SendPackets(pAgent, connId, pBuffers, count);
        }

        /// <summary>
        /// 发送多组数据
        /// 向指定连接发送多组数据
        /// TCP - 顺序发送所有数据包
        /// </summary>
        /// <param name="connId">连接 ID</param>
        /// <param name="pBuffers">发送缓冲区数组</param>
        /// <param name="iCount">发送缓冲区数目</param>
        /// <returns>TRUE.成功,FALSE.失败，可通过 SYSGetLastError() 获取 Windows 错误代码</returns>
        public bool SendPackets<T>(IntPtr connId, T[] objects)
        {
            bool ret = false;

            WSABUF[] buffer = new WSABUF[objects.Length];
            IntPtr[] ptrs = new IntPtr[buffer.Length];
            try
            {

                for (int i = 0; i < objects.Length; i++)
                {
                    buffer[i].Length = Marshal.SizeOf(typeof(T));

                    ptrs[i] = Marshal.AllocHGlobal(buffer[i].Length);
                    Marshal.StructureToPtr(objects[i], ptrs[i], true);

                    buffer[i].Buffer = ptrs[i];
                }
                ret = SendPackets(connId, buffer, buffer.Length);
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                for (int i = 0; i < ptrs.Length; i++)
                {
                    if (ptrs[i] != IntPtr.Zero)
                    {
                        Marshal.FreeHGlobal(ptrs[i]);
                    }
                }
            }

            return ret;
        }

        /// <summary>
        /// 断开某个连接
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="force">强制</param>
        /// <returns></returns>
        public bool Disconnect(IntPtr connId, bool force = true)
        {
            return HPSocketSdk.HP_Agent_Disconnect(pAgent, connId, force);
        }

        /// <summary>
        /// 断开超过指定时间的连接
        /// </summary>
        /// <param name="period">毫秒</param>
        /// <param name="force">强制</param>
        /// <returns></returns>
        public bool DisconnectLongConnections(uint period, bool force = true)
        {
            return HPSocketSdk.HP_Agent_DisconnectLongConnections(pAgent, period, force);
        }

        /// <summary>
        /// 断开超过指定时长的静默连接
        /// </summary>
        /// <param name="period">毫秒</param>
        /// <param name="force">强制</param>
        /// <returns></returns>
        public bool DisconnectSilenceConnections(uint period, bool force = true)
        {
            return HPSocketSdk.HP_Agent_DisconnectSilenceConnections(pAgent, period, force);
        }

        /// <summary>
        /// 设置连接的附加数据
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="obj"></param>
        /// <returns></returns>
        public bool SetConnectionExtra(IntPtr connId, object obj)
        {

            IntPtr ptr = IntPtr.Zero;
            // 释放附加数据
            if (HPSocketSdk.HP_Agent_GetConnectionExtra(pAgent, connId, ref ptr) && ptr != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(ptr);
                ptr = IntPtr.Zero;
            }

            if (obj != null)
            {
                // 设置附加数据
                ptr = Marshal.AllocHGlobal(Marshal.SizeOf(obj));
                Marshal.StructureToPtr(obj, ptr, false);
            }
            return HPSocketSdk.HP_Agent_SetConnectionExtra(pAgent, connId, ptr);
        }

        /// <summary>
        /// 获取附加数据
        /// 如设置的是个结构体/类对象,可以用 Type objA = (Type)Marshal.PtrToStructure(ptr, typeof(Type)) 获取
        /// 其中Type是结构体/类名,ptr是该方法的传出值,在该方法返回为true的时候可用
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="ptr"></param>
        /// <returns></returns>
        public bool GetConnectionExtra(IntPtr connId, ref IntPtr ptr)
        {
            return HPSocketSdk.HP_Agent_GetConnectionExtra(pAgent, connId, ref ptr) && ptr != IntPtr.Zero;
        }

        /// <summary>
        /// 获取错误码
        /// </summary>
        public SocketError ErrorCode
        {
            get
            {
                return HPSocketSdk.HP_Agent_GetLastError(pAgent);
            }
        }

        /// <summary>
        /// 获取错误信息
        /// </summary>
        public string ErrorMessage
        {
            get
            {
                IntPtr ptr = HPSocketSdk.HP_Agent_GetLastErrorDesc(pAgent);
                string desc = Marshal.PtrToStringUni(ptr);
                return desc;
            }
        }


        /// <summary>
        /// 获取连接中未发出数据的长度
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="length"></param>
        /// <returns></returns>
        public bool GetPendingDataLength(IntPtr connId, ref int length)
        {
            return HPSocketSdk.HP_Agent_GetPendingDataLength(pAgent, connId, ref length);
        }

        // 是否启动
        public bool IsStarted
        {
            get
            {
                if (pAgent == IntPtr.Zero)
                {
                    return false;
                }
                return HPSocketSdk.HP_Agent_HasStarted(pAgent);
            }
        }

        /// <summary>
        /// 状态
        /// </summary>
        public ServiceState State
        {
            get
            {
                return HPSocketSdk.HP_Agent_GetState(pAgent);
            }
        }

        public uint ConnectionCount
        {
            get
            {
                return HPSocketSdk.HP_Agent_GetConnectionCount(pAgent);
            }
        }

        /// <summary>
        /// 获取所有连接,未获取到连接返回null
        /// </summary>
        /// <returns></returns>
        public IntPtr[] GetAllConnectionIDs()
        {
            IntPtr[] arr = null;
            do
            {
                uint count = ConnectionCount;
                if (count == 0)
                {
                    break;
                }
                arr = new IntPtr[count];
                if (HPSocketSdk.HP_Agent_GetAllConnectionIDs(pAgent, arr, ref count))
                {
                    if (arr.Length > count)
                    {
                        IntPtr[] newArr = new IntPtr[count];
                        Array.Copy(arr, newArr, count);
                        arr = newArr;
                    }
                    break;
                }
            } while (true);

            return arr;
        }

        /// <summary>
        /// 获取监听socket的地址信息
        /// </summary>
        /// <param name="ip"></param>
        /// <param name="port"></param>
        /// <returns></returns>
        public bool GetLocalAddress(IntPtr connId, ref string ip, ref ushort port)
        {
            int ipLength = 40;

            StringBuilder sb = new StringBuilder(ipLength);

            bool ret = HPSocketSdk.HP_Agent_GetLocalAddress(pAgent, connId, sb, ref ipLength, ref port);
            if (ret == true)
            {
                ip = sb.ToString();
            }
            return ret;
        }

        /// <summary>
        /// 获取该组件对象的连接Id
        /// </summary>
        /// <returns></returns>
        public bool GetRemoteAddress(IntPtr connId, ref string ip, ref ushort port)
        {
            int ipLength = 40;

            StringBuilder sb = new StringBuilder(ipLength);

            bool ret = HPSocketSdk.HP_Agent_GetRemoteAddress(pAgent, connId, sb, ref ipLength, ref port);
            if (ret == true)
            {
                ip = sb.ToString();
            }
            return ret;
        }

        /// <summary>
        /// 获取指定连接的连接时长（毫秒）
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="period"></param>
        /// <returns></returns>
        public bool GetConnectPeriod(IntPtr connId, ref uint period)
        {
            return HPSocketSdk.HP_Agent_GetConnectPeriod(pAgent, connId, ref period);
        }

        /// <summary>
        /// 获取某个连接静默时间（毫秒）
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="period"></param>
        /// <returns></returns>
        public bool GetSilencePeriod(IntPtr connId, ref uint period)
        {
            return HPSocketSdk.HP_Agent_GetSilencePeriod(pAgent, connId, ref period);
        }

        ///////////////////////////////////////////////////////////////////////////////////////

        /// <summary>
        /// 获取或设置是否启用地址重用机制（默认：不启用）
        /// </summary>
//         public bool IsReuseAddress
//         {
//             get
//             {
//                 return HPSocketSdk.HP_UdpAgent_IsReuseAddress(pAgent);
//             }
//             set
//             {
//                 HPSocketSdk.HP_UdpAgent_SetReuseAddress(pAgent, value);
//             }
//         }


        /// <summary>
        /// 读取或设置工作线程数量（通常设置为 2 * CPU + 2）
        /// </summary>
        public uint WorkerThreadCount
        {
            get
            {
                return HPSocketSdk.HP_Agent_GetWorkerThreadCount(pAgent);
            }
            set
            {
                HPSocketSdk.HP_Agent_SetWorkerThreadCount(pAgent, value);
            }
        }

        /// <summary>
        /// 读取或设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为 1024 的倍数）
        /// </summary>
        public uint SocketBufferSize
        {
            get
            {
                return HPSocketSdk.HP_TcpAgent_GetSocketBufferSize(pAgent);
            }
            set
            {
                HPSocketSdk.HP_TcpAgent_SetSocketBufferSize(pAgent, value);
            }
        }

        /// <summary>
        /// 读取或设置 Socket 缓存对象锁定时间（毫秒，在锁定期间该 Socket 缓存对象不能被获取使用）
        /// </summary>
        public uint FreeSocketObjLockTime
        {
            get
            {
                return HPSocketSdk.HP_Agent_GetFreeSocketObjLockTime(pAgent);
            }
            set
            {
                HPSocketSdk.HP_Agent_SetFreeSocketObjLockTime(pAgent, value);
            }
        }

        /// <summary>
        /// 读取或设置 Socket 缓存池大小（通常设置为平均并发连接数量的 1/3 - 1/2）
        /// </summary>
        public uint FreeSocketObjPool
        {
            get
            {
                return HPSocketSdk.HP_Agent_GetFreeSocketObjPool(pAgent);
            }
            set
            {
                HPSocketSdk.HP_Agent_SetFreeSocketObjPool(pAgent, value);
            }
        }

        /// <summary>
        /// 读取或设置内存块缓存池大小（通常设置为 Socket 缓存池大小的 2 - 3 倍）
        /// </summary>
        public uint FreeBufferObjPool
        {
            get
            {
                return HPSocketSdk.HP_Agent_GetFreeBufferObjPool(pAgent);
            }
            set
            {
                HPSocketSdk.HP_Agent_SetFreeBufferObjPool(pAgent, value);
            }
        }

        /// <summary>
        /// 读取或设置 Socket 缓存池回收阀值（通常设置为 Socket 缓存池大小的 3 倍）
        /// </summary>
        public uint FreeSocketObjHold
        {
            get
            {
                return HPSocketSdk.HP_Agent_GetFreeSocketObjHold(pAgent);
            }
            set
            {
                HPSocketSdk.HP_Agent_SetFreeSocketObjHold(pAgent, value);
            }
        }

        /// <summary>
        /// 读取或设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍）
        /// </summary>
        public uint FreeBufferObjHold
        {
            get
            {
                return HPSocketSdk.HP_Agent_GetFreeBufferObjHold(pAgent);
            }
            set
            {
                HPSocketSdk.HP_Agent_SetFreeBufferObjHold(pAgent, value);
            }
        }

        /// <summary>
        /// 读取或设置心跳包间隔（毫秒，0 则不发送心跳包））
        /// </summary>
//         public uint KeepAliveTime
//         {
//             get
//             {
//                 return HPSocketSdk.HP_UdpAgent_GetKeepAliveTime(pAgent);
//             }
//             set
//             {
//                 HPSocketSdk.HP_UdpAgent_SetKeepAliveTime(pAgent, value);
//             }
//         }

        /// <summary>
        /// 读取或设置心跳确认包检测间隔（毫秒，0 不发送心跳包，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线）
        /// </summary>
//         public uint KeepAliveInterval
//         {
//             get
//             {
//                 return HPSocketSdk.HP_UdpAgent_GetKeepAliveInterval(pAgent);
//             }
//             set
//             {
//                 HPSocketSdk.HP_UdpAgent_SetKeepAliveInterval(pAgent, value);
//             }
//         }

        /// <summary>
        /// 读取或设置是否标记静默时间（设置为 TRUE 时 DisconnectSilenceConnections() 和 GetSilencePeriod() 才有效，默认：FALSE）
        /// </summary>
        public bool IsMarkSilence
        {
            get
            {
                return HPSocketSdk.HP_Agent_IsMarkSilence(pAgent);
            }
            set
            {
                HPSocketSdk.HP_Agent_SetMarkSilence(pAgent, value);
            }
        }

        /// <summary>
        /// 读取或设置数据发送策略
        /// </summary>
        public SendPolicy SendPolicy
        {
            get
            {
                return HPSocketSdk.HP_Agent_GetSendPolicy(pAgent);
            }
            set
            {
                HPSocketSdk.HP_Agent_SetSendPolicy(pAgent, value);
            }
        }

        /// <summary>
        /// 根据错误码返回错误信息
        /// </summary>
        /// <param name="code"></param>
        /// <returns></returns>
        public string GetSocketErrorDesc(SocketError code)
        {
            IntPtr ptr = HPSocketSdk.HP_GetSocketErrorDesc(code);
            string desc = Marshal.PtrToStringUni(ptr);
            return desc;
        }

        ///////////////////////////////////////////////////////////////////////////////////////
        HPSocketCS.SDK.HPSocketSdk.OnPrepareConnect _OnPrepareConnect = null;
        HPSocketCS.SDK.HPSocketSdk.OnConnect _OnConnect = null;
        HPSocketCS.SDK.HPSocketSdk.OnReceive _OnReceive = null;
        HPSocketCS.SDK.HPSocketSdk.OnSend _OnSend = null;
        HPSocketCS.SDK.HPSocketSdk.OnClose _OnClose = null;
        HPSocketCS.SDK.HPSocketSdk.OnShutdown _OnShutdown = null;

        /// <summary>
        /// 设置回调函数
        /// </summary>
        protected virtual void SetCallback()
        {
            _OnPrepareConnect = new HPSocketSdk.OnPrepareConnect(SDK_OnPrepareConnect);
            _OnConnect = new HPSocketSdk.OnConnect(SDK_OnConnect);
            _OnSend = new HPSocketSdk.OnSend(SDK_OnSend);
            _OnReceive = new HPSocketSdk.OnReceive(SDK_OnReceive);
            _OnClose = new HPSocketSdk.OnClose(SDK_OnClose);
             _OnShutdown = new HPSocketSdk.OnShutdown(SDK_OnShutdown);

            // 设置 Socket 监听器回调函数
            HPSocketSdk.HP_Set_FN_Agent_OnPrepareConnect(pListener, _OnPrepareConnect);
            HPSocketSdk.HP_Set_FN_Agent_OnConnect(pListener, _OnConnect);
            HPSocketSdk.HP_Set_FN_Server_OnSend(pListener, _OnSend);
            HPSocketSdk.HP_Set_FN_Server_OnReceive(pListener, _OnReceive);
            HPSocketSdk.HP_Set_FN_Server_OnClose(pListener, _OnClose);
            HPSocketSdk.HP_Set_FN_Agent_OnShutdown(pListener, _OnShutdown);

        }

        protected virtual HandleResult SDK_OnPrepareConnect(IntPtr connId, uint socket)
        {
            if (OnPrepareConnect != null)
            {
                return OnPrepareConnect(connId, socket);
            }
            return HandleResult.Ignore;
        }

        protected virtual HandleResult SDK_OnConnect(IntPtr connId)
        {
            if (OnConnect != null)
            {
                return OnConnect(connId);
            }
            return HandleResult.Ignore;
        }

        protected virtual HandleResult SDK_OnSend(IntPtr connId, IntPtr pData, int length)
        {
            if (OnSend != null)
            {
                return OnSend(connId, pData, length);
            }
            return HandleResult.Ignore;
        }

        protected virtual HandleResult SDK_OnReceive(IntPtr connId, IntPtr pData, int length)
        {
            if (OnReceive != null)
            {
                return OnReceive(connId, pData, length);
            }
            return HandleResult.Ignore;
        }

         protected virtual HandleResult SDK_OnClose(IntPtr connId, SocketOperation enOperation, int errorCode)
        {
            if (OnClose != null)
            {
                return OnClose(connId, enOperation, errorCode);
            }
            return HandleResult.Ignore;
        }

        protected virtual HandleResult SDK_OnShutdown()
        {
            if (OnShutdown != null)
            {
                return OnShutdown();
            }
            return HandleResult.Ignore;
        }


        /////////////////////////////////////////////////////////////////////////

        /// <summary>
        /// 获取系统返回的错误码
        /// </summary>
        /// <returns></returns>
        public int SYSGetLastError()
        {
            return HPSocketSdk.SYS_GetLastError();
        }

        /// <summary>
        /// 调用系统的 ::WSAGetLastError() 方法获取通信错误代码
        /// </summary>
        /// <returns></returns>
        public int SYSWSAGetLastError()
        {
            return HPSocketSdk.SYS_WSAGetLastError();
        }

        /// <summary>
        /// 调用系统的 setsockopt()
        /// </summary>
        /// <param name="sock"></param>
        /// <param name="level"></param>
        /// <param name="name"></param>
        /// <param name="val"></param>
        /// <param name="len"></param>
        /// <returns></returns>
        /// 
        public int SYS_SetSocketOption(IntPtr sock, int level, int name, IntPtr val, int len)
        {
            return HPSocketSdk.SYS_SetSocketOption(sock, level, name, val, len);
        }

        /// <summary>
        /// 调用系统的 getsockopt()
        /// </summary>
        /// <param name="sock"></param>
        /// <param name="level"></param>
        /// <param name="name"></param>
        /// <param name="val"></param>
        /// <param name="len"></param>
        /// <returns></returns>
        /// 
        public int SYSGetSocketOption(IntPtr sock, int level, int name, IntPtr val, ref int len)
        {
            return HPSocketSdk.SYS_GetSocketOption(sock, level, name, val, ref len);
        }
        /// <summary>
        /// 调用系统的 ioctlsocket()
        /// </summary>
        /// <param name="sock"></param>
        /// <param name="cmd"></param>
        /// <param name="arg"></param>
        /// <returns></returns>
        /// 
        public int SYSIoctlSocket(IntPtr sock, long cmd, IntPtr arg)
        {
            return HPSocketSdk.SYS_IoctlSocket(sock, cmd, arg);
        }

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
        public int SYS_WSAIoctl(IntPtr sock, uint dwIoControlCode, IntPtr lpvInBuffer, uint cbInBuffer,
                                              IntPtr lpvOutBuffer, uint cbOutBuffer, uint lpcbBytesReturned)
        {
            return HPSocketSdk.SYS_WSAIoctl(sock, dwIoControlCode, lpvInBuffer, cbInBuffer,
                                            lpvOutBuffer, cbOutBuffer, lpcbBytesReturned);
        }

        /// <summary>
        /// 由结构体转换为byte数组
        /// </summary>
        public byte[] StructureToByte<T>(T structure)
        {
            int size = Marshal.SizeOf(typeof(T));
            byte[] buffer = new byte[size];
            IntPtr bufferIntPtr = Marshal.AllocHGlobal(size);
            try
            {
                Marshal.StructureToPtr(structure, bufferIntPtr, true);
                Marshal.Copy(bufferIntPtr, buffer, 0, size);
            }
            finally
            {
                Marshal.FreeHGlobal(bufferIntPtr);
            }
            return buffer;
        }

        /// <summary>
        /// 由byte数组转换为结构体
        /// </summary>
        public T ByteToStructure<T>(byte[] dataBuffer)
        {
            object structure = null;
            int size = Marshal.SizeOf(typeof(T));
            IntPtr allocIntPtr = Marshal.AllocHGlobal(size);
            try
            {
                Marshal.Copy(dataBuffer, 0, allocIntPtr, size);
                structure = Marshal.PtrToStructure(allocIntPtr, typeof(T));
            }
            finally
            {
                Marshal.FreeHGlobal(allocIntPtr);
            }
            return (T)structure;
        }

        /// <summary>
        /// 对象序列化成byte[]
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public byte[] ObjectToBytes(object obj)
        {
            using (MemoryStream ms = new MemoryStream())
            {
                IFormatter formatter = new BinaryFormatter();
                formatter.Serialize(ms, obj);
                return ms.GetBuffer();
            }
        }

        /// <summary>
        /// byte[]序列化成对象
        /// </summary>
        /// <param name="Bytes"></param>
        /// <returns></returns>
        public object BytesToObject(byte[] bytes)
        {
            using (MemoryStream ms = new MemoryStream(bytes))
            {
                IFormatter formatter = new BinaryFormatter();
                return formatter.Deserialize(ms);
            }
        }
    }
}
