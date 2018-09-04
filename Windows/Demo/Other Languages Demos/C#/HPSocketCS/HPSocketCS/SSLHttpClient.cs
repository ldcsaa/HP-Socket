using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace HPSocketCS
{
    public class HttpsClient : SSLHttpClient
    {
        public HttpsClient()
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
        public HttpsClient(SSLVerifyMode verifyModel, string pemCertFile, string pemKeyFile, string keyPasswod, string caPemCertFileOrPath)
            : base(verifyModel, pemCertFile, pemKeyFile, keyPasswod, caPemCertFileOrPath)
        {
        }
    }
    public class SSLHttpClient : HttpClient
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

        public SSLHttpClient()
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="verifyModel">验证模式</param>
        /// <param name="pemCertFile">证书文件（客户端可选）</param>
        /// <param name="pemKeyFile">私钥文件（客户端可选）</param>
        /// <param name="keyPasswod">私钥密码（没有密码则为空）</param>
        /// <param name="caPemCertFileOrPath">CA 证书文件或目录（单向验证或客户端可选）</param>
        public SSLHttpClient(SSLVerifyMode verifyModel, string pemCertFile, string pemKeyFile, string keyPasswod, string caPemCertFileOrPath)
        {
            this.VerifyMode = verifyModel;
            this.PemCertFile = pemCertFile;
            this.PemKeyFile = pemKeyFile;
            this.KeyPasswod = keyPasswod;
            this.CAPemCertFileOrPath = caPemCertFileOrPath;
        }


        /// <summary>
        /// 初始化SSL环境
        /// </summary>
        /// <returns></returns>
        public virtual bool Initialize()
        {
            if (pClient != IntPtr.Zero)
            {
                PemCertFile = string.IsNullOrWhiteSpace(PemCertFile) ? null : PemCertFile;
                PemKeyFile = string.IsNullOrWhiteSpace(PemKeyFile) ? null : PemKeyFile;
                KeyPasswod = string.IsNullOrWhiteSpace(KeyPasswod) ? null : KeyPasswod;
                CAPemCertFileOrPath = string.IsNullOrWhiteSpace(CAPemCertFileOrPath) ? null : CAPemCertFileOrPath;

                return SSLSdk.HP_SSLClient_SetupSSLContext(pClient, VerifyMode, PemCertFile, PemKeyFile, KeyPasswod, CAPemCertFileOrPath);
            }

            return false;
        }


        /// <summary>
        /// 反初始化SSL环境
        /// </summary>
        public virtual void Uninitialize()
        {
            if (pClient != IntPtr.Zero)
            {
                SSLSdk.HP_SSLClient_CleanupSSLContext(pClient);
            }
        }

        protected override bool CreateListener()
        {
            if (IsCreate == true || pListener != IntPtr.Zero || pClient != IntPtr.Zero)
            {
                return false;
            }

            pListener = HttpSdk.Create_HP_HttpClientListener();
            if (pListener == IntPtr.Zero)
            {
                return false;
            }

            pClient = SSLHttpSdk.Create_HP_HttpsClient(pListener);
            if (pClient == IntPtr.Zero)
            {
                return false;
            }

            IsCreate = true;

            return true;
        }


        public override void Destroy()
        {
            Stop();
            if (pClient != IntPtr.Zero)
            {
                SSLHttpSdk.Destroy_HP_HttpsClient(pClient);
                pClient = IntPtr.Zero;
            }
            if (pListener != IntPtr.Zero)
            {
                HttpSdk.Destroy_HP_HttpClientListener(pListener);
                pListener = IntPtr.Zero;
            }

            IsCreate = false;
        }
    }
}
