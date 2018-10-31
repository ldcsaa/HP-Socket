using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HPSocketCS
{
    public class HPThreadPool
    {
        private IntPtr pThreadPool = IntPtr.Zero;

        public HPThreadPool()
        {
            pThreadPool = ThreadPoolSdk.Create_HP_ThreadPool();
        }


        ~HPThreadPool()
        {
            if (pThreadPool != IntPtr.Zero)
            {
                ThreadPoolSdk.Destroy_HP_ThreadPool(pThreadPool);
            }
        }

        /// <summary>
        /// 启动线程池组件
        /// </summary>
        /// <param name="threadCount">线程数量, 大于0 -> dwThreadCount, 等于0 -> (CPU核数* 2 + 2), 小于0 -> (CPU核数* (-threadCount))</param>
        /// <param name="policy">任务拒绝处理策略</param>
        /// <param name="maxQueueSize">任务队列最大容量（0：不限制，默认：0）</param>
        /// <param name="stackSize">线程堆栈空间大小（默认：0 -> 操作系统默认）</param>
        /// <returns>TRUE -- 成功,FALSE -- 失败，可通过 SYS_GetLastError() 获取系统错误代码</returns>
        public bool Start(uint threadCount, RejectedPolicy policy, uint maxQueueSize = 0, uint stackSize = 0)
        {
            return ThreadPoolSdk.HP_ThreadPool_Start(pThreadPool, threadCount, maxQueueSize, policy, stackSize);
        }

        /// <summary>
        /// 在规定时间内关闭线程池组件，如果工作线程在最大等待时间内未能正常关闭，会尝试强制关闭，这种情况下很可能会造成系统资源泄漏
        /// </summary>
        /// <param name="maxWait">最大等待时间（毫秒，默认：INFINITE即-1，一直等待）</param>
        /// <returns>TRUE -- 成功,FALSE -- 失败，可通过 SYS_GetLastError() 获取系统错误代码</returns>
        public bool Stop(int maxWait = -1)
        {
            return ThreadPoolSdk.HP_ThreadPool_Stop(pThreadPool, maxWait);
        }

        /// <summary>
        /// 向线程池提交异步任务
        /// </summary>
        /// <param name="fnTaskProc">任务处理函数</param>
        /// <param name="args">任务参数</param>
        /// <param name="maxWait">最大等待时间（毫秒，默认：INFINITE即-1，一直等待）</param>
        /// <returns>TRUE -- 成功,FALSE -- 失败，可通过 SYS_GetLastError() 获取系统错误代码,其中，错误码 #ERROR_DESTINATION_ELEMENT_FULL 表示任务队列已满</returns>
        public bool Submit(TaskProc fnTaskProc, IntPtr args, int maxWait = -1)
        {
            return ThreadPoolSdk.HP_ThreadPool_Submit(pThreadPool, fnTaskProc, args, maxWait);
        }

        /// <summary>
        /// 创建 TSocketTask 对象
        /// 创建任务对象，该对象最终需由 DestroySocketTask() 销毁
        /// </summary>
        /// <param name="socketTaskProc">任务入口函数</param>
        /// <param name="sender">发起对象, 如server, client,agent对象</param>
        /// <param name="connId">连接id</param>
        /// <param name="buffer">数据</param>
        /// <param name="bufferSize">数据长度</param>
        /// <param name="taskBufferType">数据类型</param>
        /// <param name="wParam">自定义参数</param>
        /// <param name="lParam">自定义参数</param>
        /// <returns>TRUE -- 成功,FALSE -- 失败，可通过 SYS_GetLastError() 获取系统错误代码,其中，错误码 ERROR_DESTINATION_ELEMENT_FULL 表示任务队列已满</returns>
        public static IntPtr CreateSocketTask(SocketTaskProc socketTaskProc, IntPtr sender, IntPtr connId, byte[] buffer, int bufferSize, TaskBufferType taskBufferType, IntPtr wParam, IntPtr lParam)
        {
            return ThreadPoolSdk.Create_HP_SocketTaskObj(socketTaskProc, sender, connId, buffer, bufferSize, taskBufferType, wParam, lParam);
        }

        /// <summary>
        /// 销毁 TSocketTask 对象
        /// </summary>
        /// <param name="task"></param>
        public static void DestroySocketTask(IntPtr task)
        {
            ThreadPoolSdk.Destroy_HP_SocketTaskObj(task);
        }

        /// <summary>
        ///  提交 Socket 任务
        ///  向线程池提交异步 Socket 任务
        /// </summary>
        /// <param name="task">任务参数 HP_LPTSocketTask</param>
        /// <param name="maxWait">最大等待时间（毫秒，默认：INFINITE即-1，一直等待）</param>
        /// <returns>TRUE -- 成功,FALSE -- 失败，可通过 SYS_GetLastError() 获取系统错误代码,其中，错误码 ERROR_DESTINATION_ELEMENT_FULL 表示任务队列已满</returns>
        public bool SubmitSocketTask(IntPtr task, int maxWait = -1)
        {
            return ThreadPoolSdk.HP_ThreadPool_Submit_Task(pThreadPool, task, maxWait);
        }


        /// <summary>
        /// 创建 TSocketTask 对象并向线程池提交异步 Socket 任务
        /// </summary>
        /// <param name="socketTaskProc">任务入口函数</param>
        /// <param name="sender">发起对象, 如server, client,agent对象</param>
        /// <param name="connId">连接id</param>
        /// <param name="buffer">数据</param>
        /// <param name="bufferSize">数据长度</param>
        /// <param name="taskBufferType">数据类型</param>
        /// <param name="wParam">自定义参数</param>
        /// <param name="lParam">自定义参数</param>
        /// <param name="maxWait">最大等待时间（毫秒，默认：INFINITE即-1，一直等待）</param>
        /// <returns>TRUE -- 成功,FALSE -- 失败，可通过 SYS_GetLastError() 获取系统错误代码,其中，错误码 ERROR_DESTINATION_ELEMENT_FULL 表示任务队列已满</returns>
        public bool SubmitSocketTask(SocketTaskProc socketTaskProc, IntPtr sender, IntPtr connId, byte[] buffer, int bufferSize, TaskBufferType taskBufferType, IntPtr wParam, IntPtr lParam, int maxWait = -1)
        {
            var task = CreateSocketTask(socketTaskProc, sender, connId, buffer, bufferSize, taskBufferType, wParam, lParam);
            if (task == IntPtr.Zero)
            {
                return false;
            }

            var ret = SubmitSocketTask(task, maxWait);
            if (!ret)
            {
                DestroySocketTask(task);
            }

            return ret;
        }


        /// <summary>
        /// 增加或减少线程池的工作线程数量
        /// </summary>
        /// <param name="newThreadCount">线程数量, 大于0 -> dwNewThreadCount, 等于0 -> (CPU核数 * 2 + 2),小于0 -> (CPU核数 * (-dwNewThreadCount))</param>
        /// <returns>TRUE -- 成功,FALSE -- 失败，可通过 SYS_GetLastError() 获取系统错误代码</returns>
        public bool AdjustThreadCount(int newThreadCount)
        {
            return ThreadPoolSdk.HP_ThreadPool_AdjustThreadCount(pThreadPool, newThreadCount);
        }

        /// <summary>
        /// 获取或设置线程池数量
        /// </summary>
        /// <returns></returns>
        public int ThreadCount
        {
            get
            {
                return (int)ThreadPoolSdk.HP_ThreadPool_GetThreadCount(pThreadPool);
            }
            set
            {
                if (pThreadPool == IntPtr.Zero)
                {
                    throw new InvalidOperationException("必须先调用Create方法!");
                }
                AdjustThreadCount(value);
            }
        }

        /// <summary>
        /// 检查线程池组件是否已启动
        /// </summary>
        /// <returns></returns>
        public bool HasStarted()
        {
            return ThreadPoolSdk.HP_ThreadPool_HasStarted(pThreadPool);
        }

        /// <summary>
        /// 查看线程池组件当前状态
        /// </summary>
        public ServiceState State
        {
            get
            {
                return ThreadPoolSdk.HP_ThreadPool_GetState(pThreadPool);
            }
        }

        /// <summary>
        /// 获取当前任务队列大小
        /// </summary>
        public uint QueueSize
        {
            get
            {
                return ThreadPoolSdk.HP_ThreadPool_GetQueueSize(pThreadPool);
            }
        }

        /// <summary>
        /// 获取任务队列最大容量
        /// </summary>
        public uint MaxQueueSize
        {
            get
            {
                return ThreadPoolSdk.HP_ThreadPool_GetMaxQueueSize(pThreadPool);
            }
        }

        public RejectedPolicy RejectedPolicy
        {
            get
            {
                return ThreadPoolSdk.HP_ThreadPool_GetRejectedPolicy(pThreadPool);
            }
        }

        /// <summary>
        /// 获取系统返回的错误码
        /// </summary>
        /// <returns></returns>
        public int SYSGetLastError()
        {
            return Sdk.SYS_GetLastError();
        }
    }
}
