using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace HPSocketCS
{
    public class SSLClientEvent
    {
        public delegate HandleResult OnHandShakeEventHandler(TcpClient sender);
    }

    public class SSLClient : TcpClient
    {
        static int ObjectReferer = 0;
        static string SSLInitLock = "SSL初始化锁";
        Sdk.OnHandShake _OnHandShake = null;
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
        /// SSL握手成功事件
        /// </summary>
        public event SSLClientEvent.OnHandShakeEventHandler OnHandShake;


        public SSLClient()
        {
            Interlocked.Increment(ref ObjectReferer);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="_verifyModel">验证模式</param>
        /// <param name="_pemCertFile">证书文件（客户端可选）</param>
        /// <param name="_pemKeyFile">私钥文件（客户端可选）</param>
        /// <param name="_keyPasswod">私钥密码（没有密码则为空）</param>
        /// <param name="_caPemCertFileOrPath">CA 证书文件或目录（单向验证或客户端可选）</param>
        public SSLClient(SSLVerifyMode _verifyModel, string _pemCertFile, string _pemKeyFile, string _keyPasswod, string _caPemCertFileOrPath)
        {
            Interlocked.Increment(ref ObjectReferer);
            this.VerifyMode = _verifyModel;
            this.PemCertFile = _pemCertFile;
            this.PemKeyFile = _pemKeyFile;
            this.KeyPasswod = _keyPasswod;
            this.CAPemCertFileOrPath = _caPemCertFileOrPath;
            Initialize();
        }

        ~SSLClient()
        {
            Uninitialize();
        }

        protected override bool CreateListener()
        {
            if (IsCreate == true || pListener != IntPtr.Zero || pClient != IntPtr.Zero)
            {
                return false;
            }

            pListener = Sdk.Create_HP_TcpClientListener();
            if (pListener == IntPtr.Zero)
            {
                return false;
            }

            pClient = SSLSdk.Create_HP_SSLClient(pListener);
            if (pClient == IntPtr.Zero)
            {
                return false;
            }

            IsCreate = true;

            return true;
        }

        protected override void SetCallback()
        {
            _OnHandShake = new Sdk.OnHandShake(SDK_OnHandShake);
            Sdk.HP_Set_FN_Server_OnHandShake(pListener, _OnHandShake);
            base.SetCallback();
        }

        /// <summary>
        /// 初始化SSL环境
        /// </summary>
        /// <returns></returns>
        protected virtual bool Initialize()
        {
            lock (SSLInitLock)
            {
                if (SSLSdk.HP_SSL_IsValid() == false)
                {
                    if (string.IsNullOrWhiteSpace(PemCertFile))
                    {
                        throw new NullReferenceException("PemCertFile");
                    }

                    if (string.IsNullOrWhiteSpace(PemKeyFile))
                    {
                        throw new NullReferenceException("PemKeyFile");
                    }

                    if (string.IsNullOrWhiteSpace(KeyPasswod))
                    {
                        throw new NullReferenceException("KeyPasswod");
                    }

                    if (string.IsNullOrWhiteSpace(CAPemCertFileOrPath))
                    {
                        throw new NullReferenceException("CAPemCertFileOrPath");
                    }

                    return SSLSdk.HP_SSL_Initialize(SSLSessionMode.Client, VerifyMode, PemCertFile, PemKeyFile, KeyPasswod, CAPemCertFileOrPath);

                }

                return true;
            }
        }

        /// <summary>
        /// 反初始化SSL环境
        /// </summary>
        protected virtual void Uninitialize()
        {
            if (Interlocked.Decrement(ref ObjectReferer) == 0)
            {
                SSLSdk.HP_SSL_Cleanup();
            }
        }

        public new bool Connetion(string address, ushort port, bool async = false)
        {
            bool ret = false;
            if (Initialize())
            {
                ret = base.Connetion(address, port, async);
            }

            return ret;
        }

        public override void Destroy()
        {
            Stop();
            if (pClient != IntPtr.Zero)
            {
                SSLSdk.Destroy_HP_SSLClient(pClient);
                pClient = IntPtr.Zero;
            }
            if (pListener != IntPtr.Zero)
            {
                Sdk.Destroy_HP_TcpClientListener(pListener);
                pListener = IntPtr.Zero;
            }

            IsCreate = false;
        }

        protected HandleResult SDK_OnHandShake(IntPtr connId)
        {
            if (OnHandShake != null)
            {
                return OnHandShake(this);
            }
            return HandleResult.Ignore;
        }
    }
}
