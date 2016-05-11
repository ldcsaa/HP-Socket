using System;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using HPSocketCS.SDK;

namespace HPSocketCS
{
    public class UdpClientEvent
    {
        public delegate HandleResult OnPrepareConnectEventHandler(UdpClient sender, uint socket);
        public delegate HandleResult OnConnectEventHandler(UdpClient sender);
        public delegate HandleResult OnSendEventHandler(UdpClient sender, IntPtr pData, int length);
        public delegate HandleResult OnReceiveEventHandler(UdpClient sender, IntPtr pData, int length);
        public delegate HandleResult OnCloseEventHandler(UdpClient sender, SocketOperation enOperation, int errorCode);
    }

    public class UdpClient
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

        /****************************************************/
        /// <summary>
        /// 准备连接了事件
        /// </summary>
        public event UdpClientEvent.OnPrepareConnectEventHandler OnPrepareConnect;
        /// <summary>
        /// 连接事件
        /// </summary>
        public event UdpClientEvent.OnConnectEventHandler OnConnect;
        /// <summary>
        /// 数据发送事件
        /// </summary>
        public event UdpClientEvent.OnSendEventHandler OnSend;
        /// <summary>
        /// 数据到达事件
        /// </summary>
        public event UdpClientEvent.OnReceiveEventHandler OnReceive;
        /// <summary>
        /// 连接关闭事件
        /// </summary>
        public event UdpClientEvent.OnCloseEventHandler OnClose;

        public UdpClient()
        {
            CreateListener();
        }

        ~UdpClient()
        {
            Destroy();
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


            pListener = HPSocketSdk.Create_HP_UdpClientListener();
            if (pListener == IntPtr.Zero)
            {
                return false;
            }

            pClient = HPSocketSdk.Create_HP_UdpClient(pListener);
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
                HPSocketSdk.Destroy_HP_UdpClient(pClient);
                pClient = IntPtr.Zero;
            }
            if (pListener != IntPtr.Zero)
            {
                HPSocketSdk.Destroy_HP_UdpClientListener(pListener);
                pListener = IntPtr.Zero;
            }

            IsCreate = false;
        }

        /// <summary>
        /// 启动通讯组件并连接到服务器
        /// </summary>
        /// <param name="address"></param>
        /// <param name="port"></param>
        /// <param name="async">是否异步</param>
        /// <returns></returns>
        public bool Connetion(string address, ushort port, bool async = false)
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

            return HPSocketSdk.HP_Client_Start(pClient, address, port, async);
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
            return HPSocketSdk.HP_Client_Stop(pClient);
        }

        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="bytes"></param>
        /// <param name="size"></param>
        /// <returns></returns>
        public bool Send(byte[] bytes, int size)
        {
            return HPSocketSdk.HP_Client_Send(pClient, bytes, size);
        }

        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="bufferPtr"></param>
        /// <param name="size"></param>
        /// <returns></returns>
        public bool Send(IntPtr bufferPtr, int size)
        {
            return HPSocketSdk.HP_Client_Send(pClient, bufferPtr, size);
        }


        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="connId"></param>
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
        /// <param name="connId"></param>
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
        /// <param name="connId"></param>
        /// <param name="bytes"></param>
        /// <param name="offset">针对bytes的偏移</param>
        /// <param name="size">发多大</param>
        /// <returns></returns>
        public bool Send(byte[] bytes, int offset, int size)
        {
            return HPSocketSdk.HP_Client_SendPart(pClient, bytes, size, offset);
        }

        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="bufferPtr"></param>
        /// <param name="offset">针对bufferPtr的偏移</param>
        /// <param name="size">发多大</param>
        /// <returns></returns>
        public bool Send(IntPtr bufferPtr, int offset, int size)
        {
            return HPSocketSdk.HP_Client_SendPart(pClient, bufferPtr, size, offset);
        }

        /// <summary>
        /// 发送多组数据
        /// 向指定连接发送多组数据
        /// UDP - 顺序发送所有数据包
        /// </summary>
        /// <param name="connId">连接 ID</param>
        /// <param name="pBuffers">发送缓冲区数组</param>
        /// <param name="iCount">发送缓冲区数目</param>
        /// <returns>TRUE.成功,FALSE.失败，可通过 SYSGetLastError() 获取 Windows 错误代码</returns>
        public bool SendPackets(WSABUF[] buffers, int count)
        {
            return HPSocketSdk.HP_Client_SendPackets(pClient, buffers, count);
        }


        /// <summary>
        /// 发送多组数据
        /// 向指定连接发送多组数据
        /// UDP - 顺序发送所有数据包
        /// </summary>
        /// <param name="connId">连接 ID</param>
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
                StructureToByte<T1>(head);
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
                return HPSocketSdk.HP_Client_GetLastError(pClient);
            }
        }

        /// <summary>
        /// 获取错误信息
        /// </summary>
        public string ErrorMessage
        {
            get
            {
                IntPtr ptr = HPSocketSdk.HP_Client_GetLastErrorDesc(pClient);
                string desc = Marshal.PtrToStringUni(ptr);
                return desc;
            }
        }

        /// <summary>
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="length"></param>
        /// <returns></returns>
        public bool GetPendingDataLength(ref int length)
        {
            return HPSocketSdk.HP_Client_GetPendingDataLength(pClient, ref length);
        }


        /// <summary>
        /// 设置连接的附加数据
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="obj">如果为null,则为释放设置的数据</param>
        /// <returns></returns>
        public void SetExtra(object obj)
        {
            // 释放附加数据
            IntPtr ptr = GetExtra();
            if (ptr != IntPtr.Zero)
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
            HPSocketSdk.HP_Client_SetExtra(pClient, ptr);
        }

        /// <summary>
        /// 获取附加数据
        /// 如设置的是个结构体/类对象,可以用 Type objA = (Type)Marshal.PtrToStructure(ptr, typeof(Type)) 获取
        /// 其中Type是结构体/类名,ptr是该方法的传出值,在该方法返回为true的时候可用
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="ptr"></param>
        /// <returns></returns>
        public IntPtr GetExtra()
        {
            return HPSocketSdk.HP_Client_GetExtra(pClient);
        }

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

            bool ret = HPSocketSdk.HP_Client_GetLocalAddress(pClient, sb, ref ipLength, ref port);
            if (ret == true)
            {
                ip = sb.ToString();
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
                return HPSocketSdk.HP_Client_HasStarted(pClient);
            }
        }

        /// <summary>
        /// 状态
        /// </summary>
        public ServiceState State
        {
            get
            {
                return HPSocketSdk.HP_Client_GetState(pClient);
            }

        }

        /// <summary>
        /// 获取该组件对象的连接Id
        /// </summary>
        public IntPtr ConnectionId
        {
            get
            {
                return HPSocketSdk.HP_Client_GetConnectionID(pClient);
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
                return HPSocketSdk.HP_Client_GetFreeBufferPoolSize(pClient);
            }
            set
            {
                HPSocketSdk.HP_Client_SetFreeBufferPoolSize(pClient, value);
            }
        }

        /// <summary>
        ///  读取或设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍）
        /// </summary>
        public uint FreeBufferPoolHold
        {
            get
            {
                return HPSocketSdk.HP_Client_GetFreeBufferPoolHold(pClient);
            }
            set
            {
                HPSocketSdk.HP_Client_SetFreeBufferPoolHold(pClient, value);
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
        /// <summary>
        /// 设置回调函数
        /// </summary>
        protected virtual void SetCallback()
        {
            // 设置 Socket 监听器回调函数
            _OnPrepareConnect = new HPSocketSdk.OnPrepareConnect(SDK_OnPrepareConnect);
            _OnConnect = new HPSocketSdk.OnConnect(SDK_OnConnect);
            _OnSend = new HPSocketSdk.OnSend(SDK_OnSend);
            _OnReceive = new HPSocketSdk.OnReceive(SDK_OnReceive);
            _OnClose = new HPSocketSdk.OnClose(SDK_OnClose);

            HPSocketSdk.HP_Set_FN_Client_OnPrepareConnect(pListener, _OnPrepareConnect);
            HPSocketSdk.HP_Set_FN_Client_OnConnect(pListener, _OnConnect);
            HPSocketSdk.HP_Set_FN_Client_OnSend(pListener, _OnSend);
            HPSocketSdk.HP_Set_FN_Client_OnReceive(pListener, _OnReceive);
            HPSocketSdk.HP_Set_FN_Client_OnClose(pListener, _OnClose);
        }

        protected HandleResult SDK_OnPrepareConnect(IntPtr pClient, uint socket)
        {
            if (OnPrepareConnect != null)
            {
                return OnPrepareConnect(this, socket);
            }
            return HandleResult.Ignore;
        }

        protected HandleResult SDK_OnConnect(IntPtr pClient)
        {
            if (OnConnect != null)
            {
                return OnConnect(this);
            }
            return HandleResult.Ignore;
        }

        protected HandleResult SDK_OnSend(IntPtr pClient, IntPtr pData, int length)
        {
            if (OnSend != null)
            {
                return OnSend(this, pData, length);
            }
            return HandleResult.Ignore;
        }

        protected HandleResult SDK_OnReceive(IntPtr pClient, IntPtr pData, int length)
        {
            if (OnReceive != null)
            {
                return OnReceive(this, pData, length);
            }
            return HandleResult.Ignore;
        }

         protected HandleResult SDK_OnClose(IntPtr pClient, SocketOperation enOperation, int errorCode)
        {
            if (OnClose != null)
            {
                return OnClose(this, enOperation, errorCode);
            }
            return HandleResult.Ignore;
        }

        ///////////////////////////////////////////////////////////////////////////

        /// <summary>
        /// 获取系统返回的错误码
        /// </summary>
        public int SYSGetLastError()
        {
            return HPSocketSdk.SYS_GetLastError();
        }

        /// <summary>
        /// 调用系统的 ::WSAGetLastError() 方法获取通信错误代码
        /// </summary>
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
