using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace HPSocketCS
{
    public class TcpClientEvent
    {
        public delegate HandleResult OnPrepareConnectEventHandler(TcpClient sender, IntPtr socket);
        public delegate HandleResult OnConnectEventHandler(TcpClient sender);
        public delegate HandleResult OnSendEventHandler(TcpClient sender, byte[] bytes);
        public delegate HandleResult OnReceiveEventHandler(TcpClient sender, byte[] bytes);
        public delegate HandleResult OnPointerDataReceiveEventHandler(TcpClient sender, IntPtr pData, int length);
        public delegate HandleResult OnCloseEventHandler(TcpClient sender, SocketOperation enOperation, int errorCode);
        public delegate HandleResult OnHandShakeEventHandler(TcpClient sender);
    }

    public class TcpClient<T> : TcpClient
    {
        public T GetExtra()
        {
            return base.GetExtra<T>();
        }

        public bool SetExtra(T obj)
        {
            return base.SetExtra(obj);
        }
    }

    public class TcpClient
    {
        protected IntPtr _pClient = IntPtr.Zero;
        protected IntPtr pClient
        {
            get
            {
                return _pClient;
            }

            set
            {
                _pClient = value;
            }
        }

        protected IntPtr pListener = IntPtr.Zero;

        protected bool IsCreate = false;
        ConnectionExtra ExtraData = new ConnectionExtra();
        /****************************************************/
        /// <summary>
        /// 准备连接了事件
        /// </summary>
        public event TcpClientEvent.OnPrepareConnectEventHandler OnPrepareConnect;
        /// <summary>
        /// 连接事件
        /// </summary>
        public event TcpClientEvent.OnConnectEventHandler OnConnect;
        /// <summary>
        /// 数据发送事件
        /// </summary>
        public event TcpClientEvent.OnSendEventHandler OnSend;
        /// <summary>
        /// 数据到达事件
        /// </summary>
        public event TcpClientEvent.OnReceiveEventHandler OnReceive;
        /// <summary>
        /// 数据到达事件(指针数据)
        /// </summary>
        public event TcpClientEvent.OnPointerDataReceiveEventHandler OnPointerDataReceive;
        /// <summary>
        /// 连接关闭事件
        /// </summary>
        public event TcpClientEvent.OnCloseEventHandler OnClose;
        /// <summary>
        /// 握手事件
        /// </summary>
        public event TcpClientEvent.OnHandShakeEventHandler OnHandShake;
        

        public TcpClient()
        {
            CreateListener();
        }

        ~TcpClient()
        {
            Destroy();
        }

        public IntPtr Sender
        {
            get
            {
                return pClient;
            }
        }


        /// <summary>
        /// 创建socket监听&服务组件
        /// </summary>
        /// <returns></returns>
        protected virtual bool CreateListener()
        {
            if (IsCreate == true || pListener != IntPtr.Zero || pClient != IntPtr.Zero)
            {
                return false;
            }


            pListener = Sdk.Create_HP_TcpClientListener();
            if (pListener == IntPtr.Zero)
            {
                return false;
            }

            pClient = Sdk.Create_HP_TcpClient(pListener);
            if (pClient == IntPtr.Zero)
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

            if (pClient != IntPtr.Zero)
            {
                Sdk.Destroy_HP_TcpClient(pClient);
                pClient = IntPtr.Zero;
            }
            if (pListener != IntPtr.Zero)
            {
                Sdk.Destroy_HP_TcpClientListener(pListener);
                pListener = IntPtr.Zero;
            }

            IsCreate = false;
        }

        /// <summary>
        /// 启动通讯组件并连接到服务器
        /// </summary>
        /// <param name="address">远程地址</param>
        /// <param name="port">远程端口</param>
        /// <param name="async">是否异步</param>
        /// <returns></returns>
        public bool Connect(string address, ushort port, bool async = true)
        {
            if (string.IsNullOrEmpty(address) == true)
            {
                throw new Exception("address is null");
            }
            else if (port == 0)
            {
                throw new Exception("port is zero");
            }

            if (IsStarted == true)
            {
                return false;
            }

            this.SetCallback();

            return Sdk.HP_Client_Start(pClient, address, port, async);
        }

        /// <summary>
        /// 启动通讯组件并连接到服务器
        /// </summary>
        /// <param name="address">远程地址</param>
        /// <param name="port">远程端口</param>
        /// <param name="bindAddress">本地绑定到哪个ip?,多ip下可以选择绑定到指定ip</param>
        /// <param name="async">是否异步</param>
        /// <returns></returns>
        public bool Connect(string address, ushort port, string bindAddress, bool async = true)
        {
            if (string.IsNullOrEmpty(address) == true)
            {
                throw new Exception("address is null");
            }
            else if (port == 0)
            {
                throw new Exception("port is zero");
            }

            if (IsStarted == true)
            {
                return false;
            }

            this.SetCallback();

            return Sdk.HP_Client_StartWithBindAddress(pClient, address, port, async, bindAddress);
        }

        /// <summary>
        /// 启动通讯组件并连接到服务器
        /// </summary>
        /// <param name="address">远程地址</param>
        /// <param name="port">远程端口</param>
        /// <param name="bindAddress">本地绑定到哪个ip?,多ip下可以选择绑定到指定ip</param>
        /// <param name="usLocalPort">本地端口</param>
        /// <param name="async">是否异步</param>
        /// <returns></returns>
        public bool Connect(string address, ushort port, string bindAddress, ushort usLocalPort, bool async = true)
        {
            if (string.IsNullOrEmpty(address) == true)
            {
                throw new Exception("address is null");
            }
            else if (port == 0)
            {
                throw new Exception("port is zero");
            }

            if (IsStarted == true)
            {
                return false;
            }

            this.SetCallback();

            return Sdk.HP_Client_StartWithBindAddressAndLocalPort(pClient, address, port, async, bindAddress, usLocalPort);
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
            return Sdk.HP_Client_Stop(pClient);
        }

        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="bytes"></param>
        /// <param name="size"></param>
        /// <returns></returns>
        public bool Send(byte[] bytes, int size)
        {
            return Sdk.HP_Client_Send(pClient, bytes, size);
        }

        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="bufferPtr"></param>
        /// <param name="size"></param>
        /// <returns></returns>
        public bool Send(IntPtr bufferPtr, int size)
        {
            return Sdk.HP_Client_Send(pClient, bufferPtr, size);
        }


        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="bufferPtr"></param>
        /// <param name="size"></param>
        /// <returns></returns>
        public bool Send<T>(T obj)
        {
            byte[] buffer = StructureToByte<T>(obj);
            return Send(buffer, buffer.Length);
        }

        /// <summary>
        /// 序列化对象后发送数据,序列化对象所属类必须标记[Serializable]
        /// </summary>
        /// <param name="bufferPtr"></param>
        /// <param name="size"></param>
        /// <returns></returns>
        public bool SendBySerializable(object obj)
        {
            byte[] buffer = ObjectToBytes(obj);
            return Send(buffer, buffer.Length);
        }

        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="bytes"></param>
        /// <param name="offset">针对bytes的偏移</param>
        /// <param name="size">发多大</param>
        /// <returns></returns>
        public bool Send(byte[] bytes, int offset, int size)
        {
            return Sdk.HP_Client_SendPart(pClient, bytes, size, offset);
        }

        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="bufferPtr"></param>
        /// <param name="offset">针对bufferPtr的偏移</param>
        /// <param name="size">发多大</param>
        /// <returns></returns>
        public bool Send(IntPtr bufferPtr, int offset, int size)
        {
            return Sdk.HP_Client_SendPart(pClient, bufferPtr, size, offset);
        }

        /// <summary>
        /// 发送多组数据
        /// 向指定连接发送多组数据
        /// TCP - 顺序发送所有数据包
        /// </summary>
        /// <param name="pBuffers">发送缓冲区数组</param>
        /// <param name="iCount">发送缓冲区数目</param>
        /// <returns>TRUE.成功,FALSE.失败，可通过 SYSGetLastError() 获取 Windows 错误代码</returns>
        public bool SendPackets(WSABUF[] buffers, int count)
        {
            return Sdk.HP_Client_SendPackets(pClient, buffers, count);
        }


        /// <summary>
        /// 发送多组数据
        /// 向指定连接发送多组数据
        /// TCP - 顺序发送所有数据包
        /// </summary>
        /// <param name="pBuffers">发送缓冲区数组</param>
        /// <param name="iCount">发送缓冲区数目</param>
        /// <returns>TRUE.成功,FALSE.失败，可通过 SYSGetLastError() 获取 Windows 错误代码</returns>
        public bool SendPackets<T>(T[] objects)
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
                ret = SendPackets(buffer, buffer.Length);
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
        /// 名称：发送小文件
        /// 描述：向指定连接发送 4096 KB 以下的小文件
        /// </summary>
        /// <param name="filePath">文件路径</param>
        /// <param name="head">头部附加数据</param>
        /// <param name="tail">尾部附加数据</param>
        /// <returns>TRUE.成功,FALSE.失败，可通过 SYSGetLastError() 获取 Windows 错误代码</returns>
        public bool SendSmallFile(string filePath, ref WSABUF head, ref WSABUF tail)
        {
            return Sdk.HP_TcpClient_SendSmallFile(pClient, filePath, ref head, ref tail);
        }

        /// <summary>
        /// 名称：发送小文件
        /// 描述：向指定连接发送 4096 KB 以下的小文件
        /// </summary>
        /// <param name="filePath">文件路径</param>
        /// <param name="head">头部附加数据,可以为null</param>
        /// <param name="tail">尾部附加数据,可以为null</param>
        /// <returns>TRUE.成功,FALSE.失败，可通过 SYSGetLastError() 获取 Windows 错误代码</returns>
        public bool SendSmallFile(string filePath, byte[] head, byte[] tail)
        {
            IntPtr pHead = IntPtr.Zero;
            IntPtr pTail = IntPtr.Zero;
            WSABUF wsaHead = new WSABUF() { Length = 0, Buffer = pHead };
            WSABUF wsatail = new WSABUF() { Length = 0, Buffer = pTail };
            if (head != null)
            {
                wsaHead.Length = head.Length;
                wsaHead.Buffer = Marshal.UnsafeAddrOfPinnedArrayElement(head, 0);
            }

            if (tail != null)
            {
                wsaHead.Length = tail.Length;
                wsaHead.Buffer = Marshal.UnsafeAddrOfPinnedArrayElement(tail, 0);
            }

            return SendSmallFile(filePath, ref wsaHead, ref wsatail);
        }

        /// <summary>
        /// 名称：发送小文件
        /// 描述：向指定连接发送 4096 KB 以下的小文件
        /// </summary>
        /// <param name="filePath">文件路径</param>
        /// <param name="head">头部附加数据,可以为null</param>
        /// <param name="tail">尾部附加数据,可以为null</param>
        /// <returns>TRUE.成功,FALSE.失败，可通过 SYSGetLastError() 获取 Windows 错误代码</returns>
        public bool SendSmallFile<T1, T2>(string filePath, T1 head, T2 tail)
        {

            byte[] headBuffer = null;
            if (head != null)
            {
                headBuffer = StructureToByte<T1>(head);
            }

            byte[] tailBuffer = null;
            if (tail != null)
            {
                tailBuffer = StructureToByte<T2>(tail);
            }
            return SendSmallFile(filePath, headBuffer, tailBuffer);
        }

        /// <summary>
        /// 获取错误码
        /// </summary>
        public SocketError ErrorCode
        {
            get
            {
                return Sdk.HP_Client_GetLastError(pClient);
            }
        }

        /// <summary>
        /// 版本号
        /// </summary>
        public string Version
        {
            get
            {
                return Sdk.GetHPSocketVersion();
            }
        }

        /// <summary>
        /// 获取错误信息
        /// </summary>
        public string ErrorMessage
        {
            get
            {
                IntPtr ptr = Sdk.HP_Client_GetLastErrorDesc(pClient);
                string desc = Marshal.PtrToStringUni(ptr);
                return desc;
            }
        }


        /// <summary>
        /// 获取或设置接收状态
        /// </summary>
        public ReceiveState ReceiveState
        {
            get
            {
                int state = -1;

                if (Sdk.HP_Client_IsPauseReceive(pClient, ref state))
                {
                    return (ReceiveState)state;
                }
                return ReceiveState.Unknown;
            }
            set
            {
                Sdk.HP_Client_PauseReceive(pClient, (int)value);
            }
        }

        /// <summary>
        /// </summary>
        /// <param name="length"></param>
        /// <returns></returns>
        public bool GetPendingDataLength(ref int length)
        {
            return Sdk.HP_Client_GetPendingDataLength(pClient, ref length);
        }

        /// <summary>
        /// 获取附加数据
        /// </summary>
        /// <returns></returns>
        public T GetExtra<T>()
        {
            return (T)ExtraData.GetExtra(pClient);
        }

        /// <summary>
        /// 设置附加数据
        /// 
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public bool SetExtra(object newValue)
        {
            return ExtraData.SetExtra(pClient, newValue);
        }

        /// <summary>
        /// 删除附加数据
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        public bool RemoveExtra()
        {
            return ExtraData.RemoveExtra(pClient);
        }


        ///// <summary>
        ///// 设置连接的附加数据
        ///// </summary>
        ///// <param name="obj">如果为null,则为释放设置的数据</param>
        ///// <returns></returns>
        //public void SetExtra(object obj)
        //{
        //    // 释放附加数据
        //    IntPtr ptr = Sdk.HP_Client_GetExtra(pClient);
        //    if (ptr != IntPtr.Zero)
        //    {
        //        Marshal.FreeHGlobal(ptr);
        //        ptr = IntPtr.Zero;
        //    }

        //    if (obj != null)
        //    {
        //        // 设置附加数据
        //        ptr = Marshal.AllocHGlobal(Marshal.SizeOf(obj));
        //        Marshal.StructureToPtr(obj, ptr, false);
        //    }
        //    Sdk.HP_Client_SetExtra(pClient, ptr);
        //}

        ///// <summary>
        ///// 获取附加数据
        ///// 如设置的是个结构体/类对象,可以用 Type objA = (Type)Marshal.PtrToStructure(ptr, typeof(Type)) 获取
        ///// 其中Type是结构体/类名,ptr是该方法的传出值,在该方法返回为true的时候可用
        ///// </summary>
        ///// <param name="ptr"></param>
        ///// <returns></returns>
        //public T GetExtra<T>()
        //{
        //    IntPtr ptr = Sdk.HP_Client_GetExtra(pClient);
        //    T obj = default(T);
        //    if (ptr != IntPtr.Zero)
        //    {
        //        obj = (T)Marshal.PtrToStructure(ptr, typeof(T));
        //    }
        //    return obj;
        //}
        ///// <summary>
        ///// 移除连接中的附加数据, 同SetExtra(null)
        ///// </summary>
        ///// <returns></returns>
        //public void RemoveExtra()
        //{
        //    SetExtra(null);
        //}

        ///// <summary>
        ///// 获取或这是附加数据
        ///// 推荐使用泛型方法:T GetExtra<T>()
        ///// </summary>
        //public object Extra
        //{
        //    get
        //    {
        //        return GetExtra<object>();
        //    }
        //    set
        //    {
        //        SetExtra(value);
        //    }
        //}

        /// <summary>
        /// 获取监听socket的地址信息
        /// </summary>
        /// <param name="ip"></param>
        /// <param name="ipLength"></param>
        /// <param name="port"></param>
        /// <returns></returns>
        public bool GetListenAddress(ref string ip, ref ushort port)
        {
            int ipLength = 40;

            StringBuilder sb = new StringBuilder(ipLength);

            bool ret = Sdk.HP_Client_GetLocalAddress(pClient, sb, ref ipLength, ref port);
            if (ret == true)
            {
                ip = sb.ToString();
            }
            return ret;
        }

        /// <summary>
        /// 获取连接的远程主机信息
        /// </summary>
        /// <param name="host"></param>
        /// <param name="port"></param>
        /// <returns></returns>
        public bool GetRemoteHost(ref string host, ref ushort port)
        {
            int ipLength = 40;

            StringBuilder sb = new StringBuilder(ipLength);

            bool ret = Sdk.HP_Client_GetRemoteHost(pClient, sb, ref ipLength, ref port);
            if (ret == true)
            {
                host = sb.ToString();
            }
            return ret;
        }

        /// <summary>
        /// 是否启动
        /// </summary>
        public bool IsStarted
        {
            get
            {
                if (pClient == IntPtr.Zero)
                {
                    return false;
                }
                return Sdk.HP_Client_HasStarted(pClient);
            }
        }

        /// <summary>
        /// 状态
        /// </summary>
        public ServiceState State
        {
            get
            {
                return Sdk.HP_Client_GetState(pClient);
            }

        }

        /// <summary>
        /// 获取该组件对象的连接Id
        /// </summary>
        public IntPtr ConnectionId
        {
            get
            {
                return Sdk.HP_Client_GetConnectionID(pClient);
            }

        }

        /// <summary>
        /// 检测是否为安全连接（SSL/HTTPS）
        /// </summary>
        public bool IsSecure
        {
            get
            {
                return Sdk.HP_Client_IsSecure(pClient);
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////

        /// <summary>
        /// 读取或设置内存块缓存池大小（通常设置为 -> PUSH 模型：5 - 10；PULL 模型：10 - 20 ）
        /// </summary>
        public uint FreeBufferPoolSize
        {
            get
            {
                return Sdk.HP_Client_GetFreeBufferPoolSize(pClient);
            }
            set
            {
                Sdk.HP_Client_SetFreeBufferPoolSize(pClient, value);
            }
        }

        /// <summary>
        ///  读取或设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍）
        /// </summary>
        public uint FreeBufferPoolHold
        {
            get
            {
                return Sdk.HP_Client_GetFreeBufferPoolHold(pClient);
            }
            set
            {
                Sdk.HP_Client_SetFreeBufferPoolHold(pClient, value);
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        ///  读取或设置通信数据缓冲区大小（根据平均通信数据包大小调整设置，通常设置为：(N * 1024) - sizeof(TBufferObj)）
        /// </summary>
        public uint SocketBufferSize
        {
            get
            {
                return Sdk.HP_TcpClient_GetSocketBufferSize(pClient);
            }
            set
            {
                Sdk.HP_TcpClient_SetSocketBufferSize(pClient, value);
            }
        }

        /// <summary>
        ///  读取或设置心跳包间隔（毫秒，0 则不发送心跳包）
        /// </summary>
        public uint KeepAliveTime
        {
            get
            {
                return Sdk.HP_TcpClient_GetKeepAliveTime(pClient);
            }
            set
            {
                Sdk.HP_TcpClient_SetKeepAliveTime(pClient, value);
            }
        }

        /// <summary>
        ///  读取或设置心跳确认包检测间隔（毫秒，0 不发送心跳包，如果超过若干次 [默认：WinXP 5 次, Win7 10 次] 检测不到心跳确认包则认为已断线）
        /// </summary>
        public uint KeepAliveInterval
        {
            get
            {
                return Sdk.HP_TcpClient_GetKeepAliveInterval(pClient);
            }
            set
            {
                Sdk.HP_TcpClient_SetKeepAliveInterval(pClient, value);
            }
        }


        /// <summary>
        /// 根据错误码返回错误信息
        /// </summary>
        /// <param name="code"></param>
        /// <returns></returns>
        public string GetSocketErrorDesc(SocketError code)
        {
            IntPtr ptr = Sdk.HP_GetSocketErrorDesc(code);
            string desc = Marshal.PtrToStringUni(ptr);
            return desc;
        }

        ///////////////////////////////////////////////////////////////////////////////////////
        protected Sdk.OnPrepareConnect _OnPrepareConnect = null;
        protected Sdk.OnConnect _OnConnect = null;
        protected Sdk.OnReceive _OnReceive = null;
        protected Sdk.OnSend _OnSend = null;
        protected Sdk.OnClose _OnClose = null;
        protected Sdk.OnHandShake _OnHandShake = null;
        /// <summary>
        /// 设置回调函数
        /// </summary>
        protected virtual void SetCallback()
        {
            // 设置 Socket 监听器回调函数
            _OnPrepareConnect = new Sdk.OnPrepareConnect(SDK_OnPrepareConnect);
            _OnConnect = new Sdk.OnConnect(SDK_OnConnect);
            _OnSend = new Sdk.OnSend(SDK_OnSend);
            _OnReceive = new Sdk.OnReceive(SDK_OnReceive);
            _OnClose = new Sdk.OnClose(SDK_OnClose);
            _OnHandShake = new Sdk.OnHandShake(SDK_OnHandShake);

            Sdk.HP_Set_FN_Client_OnPrepareConnect(pListener, _OnPrepareConnect);
            Sdk.HP_Set_FN_Client_OnConnect(pListener, _OnConnect);
            Sdk.HP_Set_FN_Client_OnSend(pListener, _OnSend);
            Sdk.HP_Set_FN_Client_OnReceive(pListener, _OnReceive);
            Sdk.HP_Set_FN_Client_OnClose(pListener, _OnClose);
            Sdk.HP_Set_FN_Client_OnHandShake(pListener, _OnHandShake);
        }

        protected HandleResult SDK_OnPrepareConnect(IntPtr pSender, IntPtr connId, IntPtr socket)
        {
            if (OnPrepareConnect != null)
            {
                return OnPrepareConnect(this, socket);
            }
            return HandleResult.Ignore;
        }

        protected HandleResult SDK_OnConnect(IntPtr pSender, IntPtr connId)
        {
            if (OnConnect != null)
            {
                return OnConnect(this);
            }
            return HandleResult.Ignore;
        }

        protected HandleResult SDK_OnSend(IntPtr pSender, IntPtr connId, IntPtr pData, int length)
        {
            if (OnSend != null)
            {
                byte[] bytes = new byte[length];
                Marshal.Copy(pData, bytes, 0, length);
                return OnSend(this, bytes);
            }
            return HandleResult.Ignore;
        }

        protected HandleResult SDK_OnReceive(IntPtr pSender, IntPtr connId, IntPtr pData, int length)
        {
            if (OnPointerDataReceive != null)
            {
                return OnPointerDataReceive(this, pData, length);
            }
            else if (OnReceive != null)
            {
                byte[] bytes = new byte[length];
                Marshal.Copy(pData, bytes, 0, length);
                return OnReceive(this, bytes);
            }
            return HandleResult.Ignore;
        }

        protected HandleResult SDK_OnClose(IntPtr pSender, IntPtr connId, SocketOperation enOperation, int errorCode)
        {
            if (OnClose != null)
            {
                return OnClose(this, enOperation, errorCode);
            }
            return HandleResult.Ignore;
        }

        protected HandleResult SDK_OnHandShake(IntPtr pSender, IntPtr connId)
        {
            if (OnHandShake != null)
            {
                return OnHandShake(this);
            }
            return HandleResult.Ignore;
        }
        ///////////////////////////////////////////////////////////////////////////

        /// <summary>
        /// 获取系统返回的错误码
        /// </summary>
        public int SYSGetLastError()
        {
            return Sdk.SYS_GetLastError();
        }

        /// <summary>
        /// 调用系统的 ::WSAGetLastError() 方法获取通信错误代码
        /// </summary>
        public int SYSWSAGetLastError()
        {
            return Sdk.SYS_WSAGetLastError();
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
            return Sdk.SYS_SetSocketOption(sock, level, name, val, len);
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
            return Sdk.SYS_GetSocketOption(sock, level, name, val, ref len);
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
            return Sdk.SYS_IoctlSocket(sock, cmd, arg);
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
            return Sdk.SYS_WSAIoctl(sock, dwIoControlCode, lpvInBuffer, cbInBuffer,
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

        /// <summary>
        /// byte[]转结构体
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="bytes"></param>
        /// <returns></returns>
        public T BytesToStruct<T>(byte[] bytes)
        {
            Type strcutType = typeof(T);
            int size = Marshal.SizeOf(strcutType);
            IntPtr buffer = Marshal.AllocHGlobal(size);
            try
            {
                Marshal.Copy(bytes, 0, buffer, size);
                return (T)Marshal.PtrToStructure(buffer, strcutType);
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }
        }
    }
}
