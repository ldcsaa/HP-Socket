using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace HPSocketCS
{

    /// <summary>
    /// 名称：拒绝策略
    /// 描述：调用被拒绝后的处理策略
    /// </summary>
    public enum RejectedPolicy
    {
        /// <summary>
        /// 立刻返回失败
        /// </summary>
        TRP_CALL_FAIL = 0,
        /// <summary>
        /// 等待（直到成功、超时或线程池关闭等原因导致失败）
        /// </summary>
        TRP_WAIT_FOR = 1,
        /// <summary>
        /// 调用者线程直接执行
        /// </summary>
        TRP_CALLER_RUN = 2,
    }


    /// <summary>
    /// 名称：任务缓冲区类型
    /// 描述：TSockeTask 对象创建和销毁时，根据不同类型的缓冲区类型作不同的处理
    /// </summary>
    public enum TaskBufferType
    {
        /// <summary>
        /// 深拷贝
        /// </summary>
        TBT_COPY = 0,
        /// <summary>
        ///  浅拷贝
        /// </summary>
        TBT_REFER = 1,
        /// <summary>
        /// 连接（不负责创建，但负责销毁）
        /// </summary>
        TBT_ATTACH = 2,
    }


    /************************************************************************
    
    ************************************************************************/
    /// <summary>
    /// 名称：任务处理函数
    /// 描述：任务处理入口函数
    /// 返回值：（无）
    /// </summary>
    /// <param name="pvArg">自定义参数</param>
    public delegate void TaskProc(IntPtr pvArg);


    /// <summary>
    /// 名称：Socket 任务处理函数
    /// 描述：Socket 任务处理入口函数
    /// 返回值：（无）
    /// </summary>
    /// <param name="pTask">pTask -- Socket 任务结构体指针</param>
    public delegate void SocketTaskProc(SocketTask task);

    /// <summary>
    /// 名称：Socket 任务结构体
    /// 描述：封装 Socket 任务相关数据结构
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct SocketTask
    {
        /// <summary>
        /// 任务处理函数
        /// </summary>
        public SocketTaskProc Fn;
        /// <summary>
        /// 发起对象
        /// </summary>
        public IntPtr Sender;
        /// <summary>
        /// 连接 ID
        /// </summary>
        public IntPtr ConnId;
        /// <summary>
        /// 数据缓冲区
        /// </summary>
        public IntPtr Buffer;
        /// <summary>
        /// 数据缓冲区长度
        /// </summary>
        public int BufferSize;
        /// <summary>
        /// 缓冲区类型
        /// </summary>
        public TaskBufferType BufferType;
        /// <summary>
        /// 自定义参数
        /// </summary>
        public IntPtr WParam;
        /// <summary>
        /// 自定义参数
        /// </summary>
        public IntPtr LParam;
    }

    public class ThreadPoolSdk
    {
        /// <summary>
        /// 创建 IHPThreadPool 对象
        /// </summary>
        /// <param name="pThreadPool"></param>
        /// <returns></returns>
        [DllImport(Sdk.HPSOCKET_DLL_PATH)]
        public static extern IntPtr Create_HP_ThreadPool();

        /// <summary>
        /// 销毁 IHPThreadPool 对象
        /// </summary>
        /// <param name="pThreadPool"></param>
        [DllImport(Sdk.HPSOCKET_DLL_PATH)]
        public static extern void Destroy_HP_ThreadPool(IntPtr pThreadPool);


        /// <summary>
        /// 名称：创建 TSocketTask 对象
        /// 描述：创建任务对象，该对象最终需由 HP_Destroy_SocketTaskObj() 销毁
        /// </summary>
        /// <param name="fnTaskProc">任务处理函数</param>
        /// <param name="pSender">发起对象</param>
        /// <param name="dwConnID">连接 ID</param>
        /// <param name="pBuffer">数据缓冲区</param>
        /// <param name="iBuffLen">数据缓冲区长度</param>
        /// <param name="enBuffType">数据缓冲区类型（默认：TBT_COPY） </param>
        /// <param name="wParam">自定义参数</param>
        /// <param name="lParam">自定义参数</param>
        /// <returns></returns>
        [DllImport(Sdk.HPSOCKET_DLL_PATH)]
        public static extern IntPtr Create_HP_SocketTaskObj(SocketTaskProc fnTaskProc, IntPtr pSender, IntPtr dwConnID, byte[] pBuffer, int iBuffLen, TaskBufferType enBuffType /*= TBT_COPY*/, IntPtr wParam /*= 0*/, IntPtr lParam /*= 0*/);

        /// <summary>
        ///  销毁 TSocketTask 对象
        /// </summary>
        /// <param name="pTask"></param>
        /// <returns></returns>
        [DllImport(Sdk.HPSOCKET_DLL_PATH)]
        public static extern void Destroy_HP_SocketTaskObj(IntPtr pTask);

        /// <summary>
        /// 名称：启动线程池组件
        /// </summary>
        /// <param name="pThreadPool"></param>
        /// <param name="dwThreadCount">线程数量，（默认：0）</param>
        /// <param name="dwMaxQueueSize">任务队列最大容量（默认：0，不限制）</param>
        /// <param name="enRejectedPolicy">任务拒绝处理策略</param>
        /// <param name="dwStackSize">线程堆栈空间大小（默认：0 -> 操作系统默认）</param>
        /// <returns>  TRUE	-- 成功 ,  FALSE	-- 失败，可通过 SYS_GetLastError() 获取系统错误代码
        /// </returns>
        [DllImport(Sdk.HPSOCKET_DLL_PATH, SetLastError = true)]
        public static extern bool HP_ThreadPool_Start(IntPtr pThreadPool, uint dwThreadCount /*= 0*/, uint dwMaxQueueSize /*= 0*/, RejectedPolicy enRejectedPolicy /*= TRP_CALL_FAIL*/, uint dwStackSize /*= 0*/);

        /// <summary>
        /// 名称：关闭线程池组件
        /// 描述：在规定时间内关闭线程池组件，如果工作线程在最大等待时间内未能正常关闭，会尝试强制关闭，这种情况下很可能会造成系统资源泄漏
        /// </summary>
        /// <param name="pThreadPool"></param>
        /// <param name="dwMaxWait">最大等待时间（毫秒，默认：INFINITE，一直等待）</param>
        /// <returns> TRUE	-- 成功,   FALSE	-- 失败，可通过 SYS_GetLastError() 获取系统错误代码
        /// </returns>
        [DllImport(Sdk.HPSOCKET_DLL_PATH, SetLastError = true)]
        public static extern bool HP_ThreadPool_Stop(IntPtr pThreadPool, int dwMaxWait /*= INFINITE*/);

        /// <summary>
        /// 名称：提交任务
        /// 描述：向线程池提交异步任务
        /// </summary>
        /// <param name="pThreadPool"></param>
        /// <param name="fnTaskProc">任务处理函数</param>
        /// <param name="pvArg">任务参数</param>
        /// <param name="dwMaxWait">任务提交最大等待时间（仅对 TRP_WAIT_FOR 类型线程池生效，默认：INFINITE，一直等待）</param>
        /// <returns>TRUE	-- 成功,   FALSE	-- 失败，可通过 SYS_GetLastError() 获取系统错误代码， 其中，错误码 ERROR_DESTINATION_ELEMENT_FULL 表示任务队列已满 </returns>
        [DllImport(Sdk.HPSOCKET_DLL_PATH, SetLastError = true)]
        public static extern bool HP_ThreadPool_Submit(IntPtr pThreadPool, TaskProc fnTaskProc, IntPtr pvArg, int dwMaxWait /*= INFINITE*/);

        /// <summary>
        /// 名称：提交 Socket 任务
        /// 描述：向线程池提交异步 Socket 任务
        /// </summary>
        /// <param name="pThreadPool"></param>
        /// <param name="pTask">任务参数 HP_LPTSocketTask</param>
        /// <param name="dwMaxWait">任务提交最大等待时间（仅对 TRP_WAIT_FOR 类型线程池生效，默认：INFINITE，一直等待）</param>
        /// <returns>TRUE	-- 成功,   FALSE	-- 失败，可通过 SYS_GetLastError() 获取系统错误代码其中，错误码 ERROR_DESTINATION_ELEMENT_FULL 表示任务队列已满</returns>
        [DllImport(Sdk.HPSOCKET_DLL_PATH, SetLastError = true)]
        public static extern bool HP_ThreadPool_Submit_Task(IntPtr pThreadPool, IntPtr pTask, int dwMaxWait /*= INFINITE*/);

        /// <summary>
        /// 名称：调整线程池大小
        /// 描述：增加或减少线程池的工作线程数量
        /// </summary>
        /// <param name="pThreadPool"></param>
        /// <param name="dwNewThreadCount">线程数量</param>
        /// <returns>TRUE	-- 成功,  FALSE	-- 失败，可通过 SYS_GetLastError() 获取系统错误代码</returns>
        [DllImport(Sdk.HPSOCKET_DLL_PATH, SetLastError = true)]
        public static extern bool HP_ThreadPool_AdjustThreadCount(IntPtr pThreadPool, int dwNewThreadCount);

        /// <summary>
        /// 检查线程池组件是否已启动
        /// </summary>
        /// <param name="pThreadPool"></param>
        /// <returns></returns>
        [DllImport(Sdk.HPSOCKET_DLL_PATH)]
        public static extern bool HP_ThreadPool_HasStarted(IntPtr pThreadPool);


        /// <summary>
        /// 查看线程池组件当前状态
        /// </summary>
        /// <param name="pThreadPool"></param>
        /// <returns></returns>
        [DllImport(Sdk.HPSOCKET_DLL_PATH)]
        public static extern ServiceState HP_ThreadPool_GetState(IntPtr pThreadPool);


        /// <summary>
        /// 获取当前任务队列大小
        /// </summary>
        /// <param name="pThreadPool"></param>
        /// <returns></returns>
        [DllImport(Sdk.HPSOCKET_DLL_PATH)]
        public static extern uint HP_ThreadPool_GetQueueSize(IntPtr pThreadPool);

        /// <summary>
        /// 获取工作线程数量
        /// </summary>
        /// <param name="pThreadPool"></param>
        /// <returns></returns>
        [DllImport(Sdk.HPSOCKET_DLL_PATH)]
        public static extern int HP_ThreadPool_GetThreadCount(IntPtr pThreadPool);

        /// <summary>
        /// 获取任务队列最大容量
        /// </summary>
        /// <param name="pThreadPool"></param>
        /// <returns></returns>
        [DllImport(Sdk.HPSOCKET_DLL_PATH)]
        public static extern uint HP_ThreadPool_GetMaxQueueSize(IntPtr pThreadPool);

        /// <summary>
        /// 获取任务拒绝处理策略
        /// </summary>
        /// <param name="pThreadPool"></param>
        /// <returns></returns>
        [DllImport(Sdk.HPSOCKET_DLL_PATH)]
        public static extern RejectedPolicy HP_ThreadPool_GetRejectedPolicy(IntPtr pThreadPool);
    }
}
