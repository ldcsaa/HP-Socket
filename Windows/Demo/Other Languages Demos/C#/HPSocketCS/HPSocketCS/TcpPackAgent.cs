using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HPSocketCS
{

    public class TcpPackAgent<T> : TcpPackAgent
    {
        public new T GetExtra(IntPtr connId)
        {
            return base.GetExtra<T>(connId);
        }

        public bool SetExtra(IntPtr connId, T obj)
        {
            return base.SetExtra(connId, obj);
        }
    }

    public class TcpPackAgent : TcpAgent
    {
        /// <summary>
        /// 创建socket监听&服务组件
        /// </summary>
        /// <returns></returns>
        protected override bool CreateListener()
        {
            if (IsCreate == true || pListener != IntPtr.Zero || pAgent != IntPtr.Zero)
            {
                return false;
            }

            pListener = Sdk.Create_HP_TcpPackAgentListener();
            if (pListener == IntPtr.Zero)
            {
                return false;
            }

            pAgent = Sdk.Create_HP_TcpPackAgent(pListener);
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
        public override void Destroy()
        {
            Stop();

            if (pAgent != IntPtr.Zero)
            {
                Sdk.Destroy_HP_TcpPackAgent(pAgent);
                pAgent = IntPtr.Zero;
            }
            if (pListener != IntPtr.Zero)
            {
                Sdk.Destroy_HP_TcpPackAgentListener(pListener);
                pListener = IntPtr.Zero;
            }

            IsCreate = false;
        }

        /// <summary>
        /// 读取或设置数据包最大长度
        /// 有效数据包最大长度不能超过 524287/0x7FFFF 字节，默认：262144/0x40000
        /// </summary>
        public uint MaxPackSize
        {
            get
            {
                return Sdk.HP_TcpPackAgent_GetMaxPackSize(pAgent);
            }
            set
            {
                Sdk.HP_TcpPackAgent_SetMaxPackSize(pAgent, value);
            }
        }

        /// <summary>
        /// 读取或设置包头标识
        /// 有效包头标识取值范围 0 ~ 8191/0x1FFF，当包头标识为 0 时不校验包头，默认：0
        /// </summary>
        public ushort PackHeaderFlag
        {
            get
            {
                return Sdk.HP_TcpPackAgent_GetPackHeaderFlag(pAgent);
            }
            set
            {
                Sdk.HP_TcpPackAgent_SetPackHeaderFlag(pAgent, value);
            }
        }


    }
}
