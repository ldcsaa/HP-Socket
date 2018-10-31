using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HPSocketCS
{
    public class TcpPullClientEvent
    {

        public delegate HandleResult OnReceiveEventHandler(TcpPullClient sender, int length);
    }

    public class TcpPullClient<T> : TcpPullClient
    {
        public T GetExtra()
        {
            return base.GetExtra<T>();
        }

        public bool SetExtra(T obj)
        {
            return base.SetExtra(obj);
        }
    }
    public class TcpPullClient : TcpClient
    {
        /// <summary>
        /// 数据到达事件
        /// </summary>
        public new event TcpPullClientEvent.OnReceiveEventHandler OnReceive;
       

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

            pClient = Sdk.Create_HP_TcpPullClient(pListener);
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
        protected  override void SetCallback()
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
                Sdk.Destroy_HP_TcpPullClient(pClient);
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
