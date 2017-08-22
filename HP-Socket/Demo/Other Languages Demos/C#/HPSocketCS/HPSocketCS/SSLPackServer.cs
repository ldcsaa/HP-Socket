using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HPSocketCS
{
    public class SSLPackServer : SSLServer
    {
        public SSLPackServer()
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
        public SSLPackServer(SSLVerifyMode _verifyModel, string _pemCertFile, string _pemKeyFile, string _keyPasswod, string _caPemCertFileOrPath, SSLSdk.SNIServerNameCallback _sniServerNameCallba)
            : base(_verifyModel, _pemCertFile, _pemKeyFile, _keyPasswod, _caPemCertFileOrPath, _sniServerNameCallba)
        {

        }

        /// <summary>
        /// 创建socket监听&服务组件
        /// </summary>
        /// <returns></returns>
        protected override bool CreateListener()
        {
            if (IsCreate == true || pListener != IntPtr.Zero || pServer != IntPtr.Zero)
            {
                return false;
            }

            pListener = Sdk.Create_HP_TcpPackServerListener();
            if (pListener == IntPtr.Zero)
            {
                return false;
            }

            pServer = SSLSdk.Create_HP_SSLPackServer(pListener);
            if (pServer == IntPtr.Zero)
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

            if (pServer != IntPtr.Zero)
            {
                SSLSdk.Destroy_HP_SSLPackServer(pServer);
                pServer = IntPtr.Zero;
            }
            if (pListener != IntPtr.Zero)
            {
                Sdk.Destroy_HP_TcpPackServerListener(pListener);
                pListener = IntPtr.Zero;
            }

            IsCreate = false;
        }

        /// <summary>
        /// 读取或设置数据包最大长度
        /// 有效数据包最大长度不能超过 4194303/0x3FFFFF 字节，默认：262144/0x40000
        /// </summary>
        public uint MaxPackSize
        {
            get
            {
                return Sdk.HP_TcpPackServer_GetMaxPackSize(pServer);
            }
            set
            {
                Sdk.HP_TcpPackServer_SetMaxPackSize(pServer, value );
            }
        }

        /// <summary>
        /// 读取或设置包头标识
        /// 有效包头标识取值范围  0 ~ 1023/0x3FF，当包头标识为 0 时不校验包头，默认：0
        /// </summary>
        public ushort PackHeaderFlag
        {
            get
            {
                return Sdk.HP_TcpPackServer_GetPackHeaderFlag(pServer);
            }
            set
            {
                Sdk.HP_TcpPackServer_SetPackHeaderFlag(pServer, value);
            }
        }

        
    }
}
