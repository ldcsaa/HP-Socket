using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HPSocketCS
{
    public class SSLPackAgent : SSLAgent
    {
        public SSLPackAgent()
            : base()
        {

        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_verifyModel">验证模式</param>
        /// <param name="_pemCertFile">证书文件（客户端可选）</param>
        /// <param name="_pemKeyFile">私钥文件（客户端可选）</param>
        /// <param name="_keyPasswod">私钥密码（没有密码则为空）</param>
        /// <param name="_caPemCertFileOrPath">CA 证书文件或目录（单向验证或客户端可选）</param>
        public SSLPackAgent(SSLVerifyMode _verifyModel, string _pemCertFile, string _pemKeyFile, string _keyPasswod, string _caPemCertFileOrPath)
            : base(_verifyModel, _pemCertFile, _pemKeyFile, _keyPasswod, _caPemCertFileOrPath)
        {

        }

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

            pAgent = SSLSdk.Create_HP_SSLPackAgent(pListener);
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
                SSLSdk.Destroy_HP_SSLPackAgent(pAgent);
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
