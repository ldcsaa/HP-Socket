using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;

namespace HPSocketCS
{
    public class SSLAgent : TcpAgent
    {
        static int ObjectReferer = 0;
        static string SSLInitLock = "SSL初始化锁";

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
        
        public SSLAgent()
        {
            Interlocked.Increment(ref ObjectReferer);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="_verifyModel">验证模式</param>
        /// <param name="_pemCertFile">证书文件</param>
        /// <param name="_pemKeyFile">私钥文件</param>
        /// <param name="_keyPasswod">私钥密码（没有密码则为空）</param>
        /// <param name="_caPemCertFileOrPath">CA 证书文件或目录（单向验证或客户端可选）</param>
        public SSLAgent(SSLVerifyMode _verifyModel, string _pemCertFile, string _pemKeyFile, string _keyPasswod, string _caPemCertFileOrPath)
        {
            Interlocked.Increment(ref ObjectReferer);
            this.VerifyMode = _verifyModel;
            this.PemCertFile = _pemCertFile;
            this.PemKeyFile = _pemKeyFile;
            this.KeyPasswod = _keyPasswod;
            this.CAPemCertFileOrPath = _caPemCertFileOrPath;
            //Initialize();
        }

        ~SSLAgent()
        {
            //Uninitialize();
        }

        protected override bool CreateListener()
        {
            if (IsCreate == true || pListener != IntPtr.Zero || pAgent != IntPtr.Zero)
            {
                return false;
            }

            pListener = Sdk.Create_HP_TcpAgentListener();
            if (pListener == IntPtr.Zero)
            {
                return false;
            }

            pAgent = SSLSdk.Create_HP_SSLAgent(pListener);
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
        protected virtual bool Initialize()
        {
            lock (SSLInitLock)
            {
                //if (SSLSdk.HP_SSL_IsValid() == false)
                {

                    PemCertFile = string.IsNullOrWhiteSpace(PemCertFile) ? null : PemCertFile;
                    PemKeyFile = string.IsNullOrWhiteSpace(PemKeyFile) ? null : PemKeyFile;
                    KeyPasswod = string.IsNullOrWhiteSpace(KeyPasswod) ? null : KeyPasswod;
                    CAPemCertFileOrPath = string.IsNullOrWhiteSpace(CAPemCertFileOrPath) ? null : CAPemCertFileOrPath;


                    var ret = SSLSdk.HP_SSLAgent_SetupSSLContext(pAgent, VerifyMode, PemCertFile, PemKeyFile, KeyPasswod, CAPemCertFileOrPath);
                    System.Diagnostics.Trace.WriteLine($"ssl Initialize : {ret}");
                }

                return true;
            }
        }

        /// <summary>
        /// 反初始化SSL环境
        /// </summary>
        protected virtual void Uninitialize()
        {
            if (Interlocked.Decrement(ref ObjectReferer) == 0 && pAgent != IntPtr.Zero)
            {
                SSLSdk.HP_SSLAgent_CleanupSSLContext(pAgent);
            }
        }

        /// <summary>
        /// 启动通讯组件
        /// 启动完成后可开始连接远程服务器
        /// </summary>
        /// <param name="address">绑定地址</param>
        /// <param name="async">是否异步</param>
        /// <returns></returns>
        public new bool Start(string address, bool async = false)
        {
            Uninitialize();
            bool ret = false;
            if (Initialize())
            {
                ret = base.Start(address, async);
            }

            return ret;
        }


        public override void Destroy()
        {

            Stop();

            if (pAgent != IntPtr.Zero)
            {
                SSLSdk.Destroy_HP_SSLAgent(pAgent);
                pAgent = IntPtr.Zero;
            }
            if (pListener != IntPtr.Zero)
            {
                Sdk.Destroy_HP_TcpAgentListener(pListener);
                pListener = IntPtr.Zero;
            }

            IsCreate = false;
        }

    }
}
