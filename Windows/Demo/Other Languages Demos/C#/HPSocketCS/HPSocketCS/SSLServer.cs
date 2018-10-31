using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace HPSocketCS
{

    public class SSLServer : TcpServer
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

        /// <summary>
        /// 名称：SNI 服务名称回调函数
        /// 描述：根据服务器名称选择 SSL 证书
        /// 返回值：
		/// 0	 -- 成功，使用默认 SSL 证书
        /// 正数	 -- 成功，使用返回值对应的 SNI 主机证书
        /// 负数	 -- 失败，中断 SSL 握手
        /// </summary>
        /// <param name="serverName"></param>
        /// <returns></returns>
        public SSLSdk.SNIServerNameCallback SNIServerNameCallback { get; set; }

        public SSLServer()
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
        /// <param name="sniServerNameCallback">SNI 回调函数指针（可选）</param>
        public SSLServer(SSLVerifyMode verifyModel, string pemCertFile, string pemKeyFile, string keyPasswod, string caPemCertFileOrPath, SSLSdk.SNIServerNameCallback sniServerNameCallback)
        {
            this.VerifyMode = verifyModel;
            this.PemCertFile = pemCertFile;
            this.PemKeyFile = pemKeyFile;
            this.KeyPasswod = keyPasswod;
            this.CAPemCertFileOrPath = caPemCertFileOrPath;
            this.SNIServerNameCallback = sniServerNameCallback;
        }

        ~SSLServer()
        {
        }

        protected override bool CreateListener()
        {
            if (IsCreate == true || pListener != IntPtr.Zero || pServer != IntPtr.Zero)
            {
                return false;
            }

            pListener = Sdk.Create_HP_TcpServerListener();
            if (pListener == IntPtr.Zero)
            {
                return false;
            }
            pServer = SSLSdk.Create_HP_SSLServer(pListener);
            if (pServer == IntPtr.Zero)
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
            if (pServer != IntPtr.Zero)
            {
                PemCertFile = string.IsNullOrWhiteSpace(PemCertFile) ? null : PemCertFile;
                PemKeyFile = string.IsNullOrWhiteSpace(PemKeyFile) ? null : PemKeyFile;
                KeyPasswod = string.IsNullOrWhiteSpace(KeyPasswod) ? null : KeyPasswod;
                CAPemCertFileOrPath = string.IsNullOrWhiteSpace(CAPemCertFileOrPath) ? null : CAPemCertFileOrPath;

                return SSLSdk.HP_SSLServer_SetupSSLContext(pServer, VerifyMode, PemCertFile, PemKeyFile, KeyPasswod, CAPemCertFileOrPath, SNIServerNameCallback);
            }

            return false;
        }

        /// <summary>
        /// 反初始化SSL环境
        /// </summary>
        public virtual void Uninitialize()
        {
            if (pServer != IntPtr.Zero)
            {
                SSLSdk.HP_SSLServer_CleanupSSLContext(pServer);
            }
        }
        
        public override void Destroy()
        {
            Stop();

            if (pServer != IntPtr.Zero)
            {
                SSLSdk.Destroy_HP_SSLServer(pServer);
                pServer = IntPtr.Zero;
            }
            if (pListener != IntPtr.Zero)
            {
                Sdk.Destroy_HP_TcpServerListener(pListener);
                pListener = IntPtr.Zero;
            }

            IsCreate = false;
        }

        /// <summary>
        /// 名称：增加 SNI 主机证书（只用于服务端）
        /// 描述：SSL 服务端在 SetupSSLContext() 成功后可以调用本方法增加多个 SNI 主机证书
        /// 返回值：正数		-- 成功，并返回 SNI 主机证书对应的索引，该索引用于在 SNI 回调函数中定位 SNI 主机
        /// 返回值：负数		-- 失败，可通过 SYS_GetLastError() 获取失败原因
        /// </summary>
        /// <param name="verifyMode">SSL 验证模式（参考 EnSSLVerifyMode）</param>
        /// <param name="pemCertFile">证书文件</param>
        /// <param name="pemKeyFile">私钥文件</param>
        /// <param name="keyPasswod">私钥密码（没有密码则为空）</param>
        /// <param name="caPemCertFileOrPath">CA 证书文件或目录（单向验证可选）</param>
        /// <returns></returns>
        public int AddServerContext(SSLVerifyMode verifyMode, string pemCertFile, string pemKeyFile, string keyPasswod, string caPemCertFileOrPath)
        {
            if (string.IsNullOrWhiteSpace(pemCertFile))
            {
                throw new ArgumentException("参数无效", pemCertFile);
            }
            if (string.IsNullOrWhiteSpace(pemKeyFile))
            {
                throw new ArgumentException("参数无效", pemKeyFile);
            }
            keyPasswod = string.IsNullOrWhiteSpace(keyPasswod) ? null : keyPasswod;
            caPemCertFileOrPath = string.IsNullOrWhiteSpace(caPemCertFileOrPath) ? null : caPemCertFileOrPath;

            return SSLSdk.HP_SSLServer_AddSSLContext(pServer, verifyMode, pemCertFile, pemKeyFile, KeyPasswod, caPemCertFileOrPath);
        }
    }
}
