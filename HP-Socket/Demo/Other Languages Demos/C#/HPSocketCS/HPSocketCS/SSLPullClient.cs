using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HPSocketCS
{
    public class SSLPullClientEvent
    {

        public delegate HandleResult OnReceiveEventHandler(SSLPullClient sender, int length);
    }

    public class SSLPullClient : SSLClient
    {
        /// <summary>
        /// 数据到达事件
        /// </summary>
        public new event SSLPullClientEvent.OnReceiveEventHandler OnReceive;

        public SSLPullClient()
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
        public SSLPullClient(SSLVerifyMode _verifyModel, string _pemCertFile, string _pemKeyFile, string _keyPasswod, string _caPemCertFileOrPath)
            : base(_verifyModel, _pemCertFile, _pemKeyFile, _keyPasswod, _caPemCertFileOrPath)
        {

        }

        ~SSLPullClient()
        {
            Destroy();
        }

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

            pListener = Sdk.Create_HP_TcpPullClientListener();
            if (pListener == IntPtr.Zero)
            {
                return false;
            }

            pClient = SSLSdk.Create_HP_SSLPullClient(pListener);
            if (pClient == IntPtr.Zero)
            {
                return false;
            }

            IsCreate = true;

            return true;
        }


        /// <summary>
        /// 抓取数据
        /// 用户通过该方法从 Socket 组件中抓取数据
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="pBuffer"></param>
        /// <param name="size"></param>
        /// <returns></returns>
        public FetchResult Fetch(IntPtr pBuffer, int size)
        {
            return Sdk.HP_TcpPullClient_Fetch(pClient, pBuffer, size);
        }

        /// <summary>
        /// 抓取数据
        /// 用户通过该方法从 Socket 组件中抓取数据
        /// </summary>
        /// <param name="connId"></param>
        /// <param name="pBuffer"></param>
        /// <param name="size"></param>
        /// <returns></returns>
        public FetchResult Peek(IntPtr pBuffer, int size)
        {
            return Sdk.HP_TcpPullClient_Peek(pClient, pBuffer, size);
        }

        new Sdk.OnPullReceive _OnReceive = null;

        /// <summary>
        /// 设置回调函数
        /// </summary>
        protected override void SetCallback()
        {
            _OnReceive = new Sdk.OnPullReceive(SDK_OnReceive);
            Sdk.HP_Set_FN_Client_OnPullReceive(pListener, _OnReceive);
            base.SetCallback();
        }


        protected HandleResult SDK_OnReceive(IntPtr pSender, IntPtr pClient, int length)
        {
            if (OnReceive != null)
            {
                return OnReceive(this, length);
            }

            return HandleResult.Ignore;
        }

        /// <summary>
        /// 终止服务并释放资源
        /// </summary>
        public override void Destroy()
        {
            Stop();

            if (pClient != IntPtr.Zero)
            {
                SSLSdk.Destroy_HP_SSLPullClient(pClient);
                pClient = IntPtr.Zero;
            }
            if (pListener != IntPtr.Zero)
            {
                Sdk.Destroy_HP_TcpPullClientListener(pListener);
                pListener = IntPtr.Zero;
            }

            IsCreate = false;
        }
    }
}
