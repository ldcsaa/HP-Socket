using System;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using System.Runtime.Serialization;
using HPSocketCS.SDK;

namespace HPSocketCS
{
    public class UdpServerEvent
    {
        public delegate HandleResult OnSendEventHandler(IntPtr connId, IntPtr pData, int length);
        public delegate HandleResult OnReceiveEventHandler(IntPtr connId, IntPtr pData, int length);
        public delegate HandleResult OnCloseEventHandler(IntPtr connId, SocketOperation enOperation, int errorCode);
        public delegate HandleResult OnShutdownEventHandler();
        public delegate HandleResult OnPrepareListenEventHandler(IntPtr soListen);
        public delegate HandleResult OnAcceptEventHandler(IntPtr connId, IntPtr pClient);
    }

    public class UdpServer
    {
        protected IntPtr _pServer = IntPtr.Zero;
        protected IntPtr pServer
        {
            get
            {
                return _pServer;
            }

            set
            {
                _pServer = value;
            }
        }

        protected IntPtr pListener = IntPtr.Zero;


        /// <summary>
        /// 服务器ip
        /// </summary>
        public string IpAddress { get; set; }

        /// <summary>
        /// 服务器端口
        /// </summary>
        public ushort Port { get; set; }

        /// <summary>
        /// 连接到达事件
        /// </summary>
        public event UdpServerEvent.OnAcceptEventHandler OnAccept;
        /// <summary>
        /// 数据包发送事件
        /// </summary>
        public event UdpServerEvent.OnSendEventHandler OnSend;
        /// <summary>
        /// 准备监听了事件
        /// </summary>
        public event UdpServerEvent.OnPrepareListenEventHandler OnPrepareListen;
        /// <summary>
        /// 数据到达事件
        /// </summary>
        public event UdpServerEvent.OnReceiveEventHandler OnReceive;
        /// <summary>
        /// 连接关闭事件
        /// </summary>
        public event UdpServerEvent.OnCloseEventHandler OnClose;
        /// <summary>
        /// 服务器关闭事件
        /// </summary>
        public event UdpServerEvent.OnShutdownEventHandler OnShutdown;

        protected bool IsCreate = false;

        /// <summary>
        /// udpserver构造
        /// </summary>
        public UdpServer()
        {
            CreateListener();
        }

        ~UdpServer()
        {
            Destroy();
        }

        /// <summary>
        /// 创建socket监听&服务组件
        /// </summary>
        /// <param name="isUseDefaultCallback">是否使用udpserver类默认回调函数</param>
        /// <returns></returns>
        protected virtual bool CreateListener()
        {
            if (IsCreate == true || pListener != IntPtr.Zero || pServer != IntPtr.Zero)
            {
                return false;
            }

            pListener = HPSocketSdk.Create_HP_UdpServerListener();
            if (pListener == IntPtr.Zero)
            {
                return false;
            }
            pServer = HPSocketSdk.Create_HP_UdpServer(pListener);
            if (pServer == IntPtr.Zero)
            {
                return false;
            }

            IsCreate = true;

            return true;
        }

        /// <summary>
        /// 释放UdpServer和UdpServerListener
        /// </summary>
        public virtual void Destroy()
        {
            Stop();

            if (pServer != IntPtr.Zero)
            {
                HPSocketSdk.Destroy_HP_UdpServer(pServer);
                pServer = IntPtr.Zero;
            }
            if (pListener != IntPtr.Zero)
            {
                HPSocketSdk.Destroy_HP_UdpServerListener(pListener);
                pListener = IntPtr.Zero;
            }

            IsCreate = false;
        }


        /// <summary>
        /// 启动服务
        /// </summary>
        /// <param name="address"></param>
        /// <param name="port"></param>
        /// <returns></returns>
        public bool Start()
        {
            if (IsStarted == true)
            {
                return false;
            }

            SetCallback();

            return HPSocketSdk.HP_Server_Start(pServer, IpAddress, Port);
        }

        /// <summary>
        /// 停止服务
        /// </summary>
        /// <returns></returns>
        public bool Stop()
        {
            if (IsStarted == false)
            {
                return false;
            }

            return HPSocketSdk.HP_Server_Stop(pServer);
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
            return HPSocketSdk.HP_Server_Send(pServer, connId, bytes, size);
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
            return HPSocketSdk.HP_Server_Send(pServer, connId, bufferPtr, size);
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
            return HPSocketSdk.HP_Server_SendPart(pServer, connId, bytes, size, offset);
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
            return HPSocketSdk.HP_Server_SendPart(pServer, connId, bufferPtr, size, offset);
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
        /// 发送多组数据
        /// 向指定连接发送多组数据
        /// UDP - 顺序发送所有数据包
        /// </summary>
        /// <param name="connId">连接 ID</param>
        /// <param name="pBuffers">发送缓冲区数组</param>
        /// <param name="iCount">发送缓冲区数目</param>
        /// <returns>TRUE.成功,FALSE.失败，可通过 SYSGetLastError() 获取 Windows 错误代码</returns>
        public bool SendPackets(IntPtr connId, WSABUF[] pBuffers, int count)
        {
            return HPSocketSdk.HP_Server_SendPackets(pServer, connId, pBuffers, count);
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
        /// 断开与某个客户的连接
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="bForce">是否强制断开</param>
        /// <returns></returns>
        public bool Disconnect(IntPtr connId, bool force = true)
        {
            return HPSocketSdk.HP_Server_Disconnect(pServer, connId, force);
        }

        /// <summary>
        /// 断开超过指定时间的连接
        /// </summary>
        /// <param name="period">毫秒</param>
        /// <param name="force">强制</param>
        /// <returns></returns>
        public bool DisconnectLongConnections(uint period, bool force = true)
        {
            return HPSocketSdk.HP_Server_DisconnectLongConnections(pServer, period, force);
        }

        /// <summary>
        /// 断开超过指定时长的静默连接
        /// </summary>
        /// <param name="period">毫秒</param>
        /// <param name="force">强制</param>
        /// <returns></returns>
        public bool DisconnectSilenceConnections(uint period, bool force = true)
        {
            return HPSocketSdk.HP_Server_DisconnectSilenceConnections(pServer, period, force);
        }

        /// <summary>
        /// 获取某个连接的远程ip和端口
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="ip"></param>
        /// <param name="port"></param>
        /// <returns></returns>
        public bool GetRemoteAddress(IntPtr connId, ref string ip, ref ushort port)
        {
            int ipLength = 40;

            StringBuilder sb = new StringBuilder(ipLength);

            bool ret = HPSocketSdk.HP_Server_GetRemoteAddress(pServer, connId, sb, ref ipLength, ref port) && ipLength > 0;
            if (ret == true)
            {
                ip = sb.ToString();
            }

            return ret;
        }


        /// <summary>
        /// 获取连接中未发出数据的长度
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="length"></param>
        /// <returns></returns>
        public bool GetPendingDataLength(IntPtr connId, ref int length)
        {
            return HPSocketSdk.HP_Server_GetPendingDataLength(pServer, connId, ref length);
        }

        /// <summary>
        /// 设置连接的附加数据
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="obj">如果为null,则为释放设置的数据</param>
        /// <returns></returns>
        public bool SetConnectionExtra(IntPtr connId, object obj)
        {

            IntPtr ptr = IntPtr.Zero;
            // 释放附加数据
            if (HPSocketSdk.HP_Server_GetConnectionExtra(pServer, connId, ref ptr) && ptr != IntPtr.Zero)
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

            return HPSocketSdk.HP_Server_SetConnectionExtra(pServer, connId, ptr);
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
            return HPSocketSdk.HP_Server_GetConnectionExtra(pServer, connId, ref ptr) && ptr != IntPtr.Zero;
        }

        // 是否启动
        public bool IsStarted
        {
            get
            {
                if (pServer == IntPtr.Zero)
                {
                    return false;
                }
                return HPSocketSdk.HP_Server_HasStarted(pServer);
            }
        }

        /// <summary>
        /// 状态
        /// </summary>
        public ServiceState State
        {
            get
            {
                return HPSocketSdk.HP_Server_GetState(pServer);
            }

        }

        /// <summary>
        /// 连接数
        /// </summary>
        public uint ConnectionCount
        {
            get
            {
                return HPSocketSdk.HP_Server_GetConnectionCount(pServer);
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
                if (HPSocketSdk.HP_Server_GetAllConnectionIDs(pServer, arr, ref count))
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
        public bool GetListenAddress(ref string ip, ref ushort port)
        {
            int ipLength = 40;

            StringBuilder sb = new StringBuilder(ipLength);

            bool ret = HPSocketSdk.HP_Server_GetListenAddress(pServer, sb, ref ipLength, ref port);
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
            return HPSocketSdk.HP_Server_GetConnectPeriod(pServer, connId, ref period);
        }

        /// <summary>
        /// 获取某个连接静默时间（毫秒）
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="period"></param>
        /// <returns></returns>
        public bool GetSilencePeriod(IntPtr connId, ref uint period)
        {
            return HPSocketSdk.HP_Server_GetSilencePeriod(pServer, connId, ref period);
        }

        ///////////////////////////////////////////////////////////////////////////////////////

        /// <summary>
        /// 读取或设置工作线程数量（通常设置为 2 * CPU + 2）
        /// </summary>
        public uint WorkerThreadCount
        {
            get
            {
                return HPSocketSdk.HP_Server_GetWorkerThreadCount(pServer);
            }
            set
            {
                HPSocketSdk.HP_Server_SetWorkerThreadCount(pServer, value);
            }
        }

        /// <summary>
        /// 读取或设置 Socket 缓存对象锁定时间（毫秒，在锁定期间该 Socket 缓存对象不能被获取使用）
        /// </summary>
        public uint FreeSocketObjLockTime
        {
            get
            {
                return HPSocketSdk.HP_Server_GetFreeSocketObjLockTime(pServer);
            }
            set
            {
                HPSocketSdk.HP_Server_SetFreeSocketObjLockTime(pServer, value);
            }
        }

        /// <summary>
        /// 读取或设置 Socket 缓存池大小（通常设置为平均并发连接数量的 1/3 - 1/2）
        /// </summary>
        public uint FreeSocketObjPool
        {
            get
            {
                return HPSocketSdk.HP_Server_GetFreeSocketObjPool(pServer);
            }
            set
            {
                HPSocketSdk.HP_Server_SetFreeSocketObjPool(pServer, value);
            }
        }

        /// <summary>
        /// 读取或设置内存块缓存池大小（通常设置为 Socket 缓存池大小的 2 - 3 倍）
        /// </summary>
        public uint FreeBufferObjPool
        {
            get
            {
                return HPSocketSdk.HP_Server_GetFreeBufferObjPool(pServer);
            }
            set
            {
                HPSocketSdk.HP_Server_SetFreeBufferObjPool(pServer, value);
            }
        }

        /// <summary>
        /// 读取或设置内存块缓存池大小（通常设置为 Socket 缓存池大小的 2 - 3 倍）
        /// </summary>
        public uint FreeSocketObjHold
        {
            get
            {
                return HPSocketSdk.HP_Server_GetFreeSocketObjHold(pServer);
            }
            set
            {
                HPSocketSdk.HP_Server_SetFreeSocketObjHold(pServer, value);
            }
        }

        /// <summary>
        /// 读取或设置内存块缓存池回收阀值（通常设置为内存块缓存池大小的 3 倍）
        /// </summary>
        public uint FreeBufferObjHold
        {
            get
            {
                return HPSocketSdk.HP_Server_GetFreeBufferObjHold(pServer);
            }
            set
            {
                HPSocketSdk.HP_Server_SetFreeBufferObjHold(pServer, value);
            }
        }

        /// <summary>
        /// 读取或设置是否标记静默时间（设置为 TRUE 时 DisconnectSilenceConnections() 和 GetSilencePeriod() 才有效，默认：FALSE）
        /// </summary>
        public bool MarkSilence
        {
            get
            {
                return HPSocketSdk.HP_Server_IsMarkSilence(pServer);
            }
            set
            {
                HPSocketSdk.HP_Server_SetMarkSilence(pServer, value);
            }
        }

        /// <summary>
        /// 获取或设置数据发送策略
        /// </summary>
        public SendPolicy SendPolicy
        {
            get
            {
                return HPSocketSdk.HP_Server_GetSendPolicy(pServer);
            }
            set
            {
                HPSocketSdk.HP_Server_SetSendPolicy(pServer, value);
            }
        }



        ///////////////////////////////////////////////////////////////////////////////////////

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
        /// 获取错误码
        /// </summary>
        public SocketError ErrorCode
        {
            get
            {
                return HPSocketSdk.HP_Server_GetLastError(pServer);
            }
        }

        /// <summary>
        /// 获取错误信息
        /// </summary>
        public string ErrorMessage
        {
            get
            {
                IntPtr ptr = HPSocketSdk.HP_Server_GetLastErrorDesc(pServer);
                string desc = Marshal.PtrToStringUni(ptr);
                return desc;
            }
        }


        ///////////////////////////////////////////////////////////////////////////////////////
        HPSocketCS.SDK.HPSocketSdk.OnPrepareListen _OnPrepareListen = null;
        HPSocketCS.SDK.HPSocketSdk.OnAccept _OnAccept = null;
        HPSocketCS.SDK.HPSocketSdk.OnReceive _OnReceive = null;
        HPSocketCS.SDK.HPSocketSdk.OnSend _OnSend = null;
        HPSocketCS.SDK.HPSocketSdk.OnClose _OnClose = null;
        HPSocketCS.SDK.HPSocketSdk.OnShutdown _OnShutdown = null;

        protected virtual void SetCallback()
        {
            _OnPrepareListen = new HPSocketSdk.OnPrepareListen(SDK_OnPrepareListen);
            _OnAccept = new HPSocketSdk.OnAccept(SDK_OnAccept);
            _OnSend = new HPSocketSdk.OnSend(SDK_OnSend);
            _OnReceive = new HPSocketSdk.OnReceive(SDK_OnReceive);
            _OnClose = new HPSocketSdk.OnClose(SDK_OnClose);
            _OnShutdown = new HPSocketSdk.OnShutdown(SDK_OnShutdown);

            HPSocketSdk.HP_Set_FN_Server_OnPrepareListen(pListener, _OnPrepareListen);
            HPSocketSdk.HP_Set_FN_Server_OnAccept(pListener, _OnAccept);
            HPSocketSdk.HP_Set_FN_Server_OnSend(pListener, _OnSend);
            HPSocketSdk.HP_Set_FN_Server_OnReceive(pListener, _OnReceive);
            HPSocketSdk.HP_Set_FN_Server_OnClose(pListener, _OnClose);
            HPSocketSdk.HP_Set_FN_Server_OnShutdown(pListener, _OnShutdown);
        }


        protected HandleResult SDK_OnPrepareListen(IntPtr soListen)
        {
            if (OnPrepareListen != null)
            {
                OnPrepareListen(soListen);
            }
            return HandleResult.Ignore;
        }

        protected HandleResult SDK_OnAccept(IntPtr connId, IntPtr pClient)
        {
            if (OnAccept != null)
            {
                OnAccept(connId, pClient);
            }

            return HandleResult.Ignore;
        }

        protected HandleResult SDK_OnSend(IntPtr connId, IntPtr pData, int length)
        {
            if (OnSend != null)
            {
                OnSend(connId, pData, length);
            }
            return HandleResult.Ignore;
        }

        protected HandleResult SDK_OnReceive(IntPtr connId, IntPtr pData, int length)
        {
            if (OnReceive != null)
            {
                OnReceive(connId, pData, length);
            }
            return HandleResult.Ignore;
        }

        protected HandleResult SDK_OnClose(IntPtr connId, SocketOperation enOperation, int errorCode)
        {
            if (OnClose != null)
            {
                OnClose(connId, enOperation, errorCode);
            }
            return HandleResult.Ignore;
        }

        protected HandleResult SDK_OnShutdown()
        {
            if (OnShutdown != null)
            {
                OnShutdown();
            }
            return HandleResult.Ignore;
        }


        /////////////////////////////////////////////////////////////////////////

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
