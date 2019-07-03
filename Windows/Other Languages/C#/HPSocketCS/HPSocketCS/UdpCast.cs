using System;
using System.Collections.Generic;
using System.Text;

namespace HPSocketCS
{
    public class UdpCast : UdpClient
    {

        /// <summary>
        /// 创建socket监听&服务组件
        /// </summary>
        /// <returns></returns>
        protected override bool CreateListener()
        {
            if (IsCreate == true || pListener != IntPtr.Zero || pClient != IntPtr.Zero)
            {
                return false;
            }

            pListener = Sdk.Create_HP_UdpCastListener();
            if (pListener == IntPtr.Zero)
            {
                return false;
            }

            pClient = Sdk.Create_HP_UdpCast(pListener);
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
        public override void Destroy()
        {
            Stop();

            if (pClient != IntPtr.Zero)
            {
                Sdk.Destroy_HP_UdpCast(pClient);
                pClient = IntPtr.Zero;
            }
            if (pListener != IntPtr.Zero)
            {
                Sdk.Destroy_HP_UdpCastListener(pListener);
                pListener = IntPtr.Zero;
            }

            IsCreate = false;
        }

        /// <summary>
        /// 获取或设置数据报文最大长度（建议在局域网环境下不超过 1472 字节，在广域网环境下不超过 548 字节）
        /// </summary>
        public uint MaxDatagramSiz
        {
            get
            {
                return Sdk.HP_UdpCast_GetMaxDatagramSize(pClient );
            }
            set
            {
                Sdk.HP_UdpCast_SetMaxDatagramSize(pClient, value);
            }
        }

        /// <summary>
        /// 获取或设置是否启用地址重用机制（默认：不启用）
        /// </summary>
        public bool ReuseAddress
        {
            get
            {
                return Sdk.HP_UdpCast_IsReuseAddress(pClient);
            }
            set
            {
                Sdk.HP_UdpCast_SetReuseAddress(pClient, value);
            }
        }

        /// <summary>
        /// 获取或设置传播模式（组播或广播） 
        /// </summary>
        public CastMode CastMode
        {
            get
            {
                return Sdk.HP_UdpCast_GetCastMode(pClient);
            }
            set
            {
                Sdk.HP_UdpCast_SetCastMode(pClient, value);
            }
        }

        /// <summary>
        /// 获取或设置组播报文的 TTL（0 - 255）
        /// </summary>
        public int MultiCastTtl
        {
            get
            {
                return Sdk.HP_UdpCast_GetMultiCastTtl(pClient);
            }
            set
            {
                if (value < 0 || value > 255)
                {
                    throw new InvalidOperationException("MultiCastTtl取值范围只能是0-255");
                }
                Sdk.HP_UdpCast_SetMultiCastTtl(pClient, value);
            }
        }

        /// <summary>
        /// 获取或设置是否启用组播环路（TRUE or FALSE）
        /// </summary>
        public bool MultiCastLoop
        {
            get
            {
                return Sdk.HP_UdpCast_IsMultiCastLoop(pClient);
            }
            set
            {
                Sdk.HP_UdpCast_SetMultiCastLoop(pClient, value);
            }
        }

        /// <summary>
        /// 获取当前数据报的远程地址信息（通常在 OnReceive 事件中调用） 
        /// </summary>
        /// <returns></returns>
        public bool GetRemoteAddress(IntPtr connId, ref string ip, ref ushort port)
        {
            int ipLength = 40;

            StringBuilder sb = new StringBuilder(ipLength);

            bool ret = Sdk.HP_UdpCast_GetRemoteAddress(pClient,  sb, ref ipLength, ref port);
            if (ret == true)
            {
                ip = sb.ToString();
            }
            return ret;
        }
         
    }
}
