using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace HPSocketCS
{
    public class HttpsAgent : SSLHttpAgent
    {
        public HttpsAgent()
           : base()
        {

        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="verifyModel">验证模式</param>
        /// <param name="pemCertFile">证书文件</param>
        /// <param name="pemKeyFile">私钥文件</param>
        /// <param name="keyPasswod">私钥密码（没有密码则为空）</param>
        /// <param name="caPemCertFileOrPath">CA 证书文件或目录（单向验证或客户端可选）</param>
        public HttpsAgent(SSLVerifyMode verifyModel, string pemCertFile, string pemKeyFile, string keyPasswod, string caPemCertFileOrPath)
            : base(verifyModel, pemCertFile, pemKeyFile, keyPasswod, caPemCertFileOrPath)
        {
        }
    }
    public class SSLHttpAgent : HttpAgent
    {
        /// <summary>
        /// 验证模式
        /// </summary>
        public SSLVerifyMode VerifyMode { get; set; }
        /// <summary>
        /// 证书文件（客户端可选）
        /// </summary>
        public string PemCertFile { get; set; }
        /// <summary>
        /// 私钥文件（客户端可选）
        /// </summary>
        public string PemKeyFile { get; set; }
        /// <summary>
        /// 私钥密码（没有密码则为空）
        /// </summary>
        public string KeyPasswod { get; set; }
        /// <summary>
        /// CA 证书文件或目录（单向验证或客户端可选）
        /// </summary>
        public string CAPemCertFileOrPath { get; set; }

        public SSLHttpAgent()
            : base()
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="verifyModel">验证模式</param>
        /// <param name="pemCertFile">证书文件</param>
        /// <param name="pemKeyFile">私钥文件</param>
        /// <param name="keyPasswod">私钥密码（没有密码则为空）</param>
        /// <param name="caPemCertFileOrPath">CA 证书文件或目录（单向验证或客户端可选）</param>
        public SSLHttpAgent(SSLVerifyMode verifyModel, string pemCertFile, string pemKeyFile, string keyPasswod, string caPemCertFileOrPath)
        {
            this.VerifyMode = verifyModel;
            this.PemCertFile = pemCertFile;
            this.PemKeyFile = pemKeyFile;
            this.KeyPasswod = keyPasswod;
            this.CAPemCertFileOrPath = caPemCertFileOrPath;
        }

        protected override bool CreateListener()
        {
            if (IsCreate == true || pListener != IntPtr.Zero || pAgent != IntPtr.Zero)
            {
                return false;
            }

            pListener = HttpSdk.Create_HP_HttpAgentListener();
            if (pListener == IntPtr.Zero)
            {
                return false;
            }

            pAgent = SSLHttpSdk.Create_HP_HttpsAgent(pListener);
            if (pAgent == IntPtr.Zero)
            {
                return false;
            }

            IsCreate = true;

            return true;
        }

        /// <summary>
        /// 初始化SSL环境
        /// </summary>
        /// <returns></returns>
        public virtual bool Initialize()
        {
            if (pAgent != IntPtr.Zero)
            {

                PemCertFile = string.IsNullOrWhiteSpace(PemCertFile) ? null : PemCertFile;
                PemKeyFile = string.IsNullOrWhiteSpace(PemKeyFile) ? null : PemKeyFile;
                KeyPasswod = string.IsNullOrWhiteSpace(KeyPasswod) ? null : KeyPasswod;
                CAPemCertFileOrPath = string.IsNullOrWhiteSpace(CAPemCertFileOrPath) ? null : CAPemCertFileOrPath;

                return SSLSdk.HP_SSLAgent_SetupSSLContext(pAgent, VerifyMode, PemCertFile, PemKeyFile, KeyPasswod, CAPemCertFileOrPath);
            }

            return true;

        }

        /// <summary>
        /// 反初始化SSL环境
        /// </summary>
        public virtual void Uninitialize()
        {
            if (pAgent != IntPtr.Zero)
            {
                SSLSdk.HP_SSLAgent_CleanupSSLContext(pAgent);
            }
        }

        public override void Destroy()
        {

            Stop();

            if (pAgent != IntPtr.Zero)
            {
                SSLHttpSdk.Destroy_HP_HttpsAgent(pAgent);
                pAgent = IntPtr.Zero;
            }
            if (pListener != IntPtr.Zero)
            {
                HttpSdk.Destroy_HP_HttpAgentListener(pListener);
                pListener = IntPtr.Zero;
            }

            IsCreate = false;
        }
    }
}
