using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using HPSocketCS;

namespace ThreadPoolDemo
{
    class Program
    {
        static long count = 0;
        static int t1 = 0, t2 = 0;

        static void Main(string[] args)
        {
            var pool = new HPThreadPool();
            RunTest(pool);

            Console.WriteLine("按任意键继续下一轮测试...");
            Console.ReadKey();

            RunTestSocketTask(pool);

            Console.ReadKey();
        }

        static void RunTestSocketTask(HPThreadPool pool)
        {
            try
            {

                // 启动线程池
                if (!pool.Start(0, RejectedPolicy.TRP_WAIT_FOR, 10, 0))
                {
                    throw new Exception("错误码:" + pool.SYSGetLastError());
                }

                count = 300;
                var loopCount = count;

                t1 = Environment.TickCount;


                for (long i = 1; i <= loopCount; i++)
                {

                    // GetLastError() 错误码 ERROR_DESTINATION_ELEMENT_FULL 表示任务队列已满
                    // dwMaxWait 参数仅对 TRP_WAIT_FOR 类型线程池生效
                    // 如果 dwMaxWait == -1，一直等待，直到成功或线程池关闭等原因导致失败
                    if (!pool.SubmitSocketTask(new SocketTaskProc(SocketTaskProc), 
                        IntPtr.Zero, (IntPtr)i, null, 0, TaskBufferType.TBT_COPY, 
                        IntPtr.Zero, IntPtr.Zero, 100))
                    {
                        // ERROR_DESTINATION_ELEMENT_FULL = 1161 看上面注释
                        Console.WriteLine("{0} - 失败, 错误码: {1}", i, pool.SYSGetLastError());
                        if (Interlocked.Decrement(ref count) == 0)
                        {
                            t2 = Environment.TickCount;
                        }
                    }
                }

                Console.ReadKey();

                Console.WriteLine("耗时:{0}ms", t2 - t1);

                pool.Stop();
            }
            catch (Exception ex)
            {
                Console.WriteLine("发生了异常:" + ex.Message + "\r\n" + ex.StackTrace);
            }
        }

        static void RunTest(HPThreadPool pool)
        {
            try
            {
                // 启动线程池
                if (!pool.Start(0, RejectedPolicy.TRP_WAIT_FOR, 10, 0))
                {
                    throw new Exception("错误码:" + pool.SYSGetLastError());
                }

                count = 300;

                var loopCount = count;

                t1 = Environment.TickCount;

                for (long i = 1; i <= loopCount; i++)
                {
                    // GetLastError() 错误码 ERROR_DESTINATION_ELEMENT_FULL 表示任务队列已满
                    // maxWait 参数仅对 TRP_WAIT_FOR 类型线程池生效
                    // 如果 maxWait == -1，一直等待，直到成功或线程池关闭等原因导致失败
                    if (!pool.Submit(new TaskProc(TaskProc), (IntPtr)i, 100))
                    {
                        // ERROR_DESTINATION_ELEMENT_FULL = 1161 看上面注释
                        Console.WriteLine("{0} - 失败, 错误码: {1}", i, pool.SYSGetLastError());

                        if (Interlocked.Decrement(ref count) == 0)
                        {
                            t2 = Environment.TickCount;
                        }
                        
                    }
                }
                
                Console.ReadKey();

                Console.WriteLine("耗时:{0}ms", t2 - t1);

                pool.Stop();
            }
            catch (Exception ex)
            {
                Console.WriteLine("发生了异常:" + ex.Message + "\r\n" + ex.StackTrace);
            }
        }

        static void SocketTaskProc(SocketTask task)
        {
            var i = (ulong)task.ConnId;

            // 模拟业务处理
            var sleep = new Random().Next(1, 1000);
            Thread.Sleep(sleep);


            Console.WriteLine("{0}\t{1},\tsleep: {2}", Thread.CurrentThread.ManagedThreadId, i, sleep);

            if (Interlocked.Decrement(ref count) == 1)
            {
                t2 = Environment.TickCount;
            }
        }

        static void TaskProc(IntPtr args)
        {
            var i = (long)args;

            // 模拟业务处理
            var sleep = new Random().Next(1, 1000);
            Thread.Sleep(sleep);

            Console.WriteLine("{0}\t{1},\tsleep: {2}", Thread.CurrentThread.ManagedThreadId, i, sleep);

            if (Interlocked.Decrement(ref count) == 0)
            {
                t2 = Environment.TickCount;
            }
        }
    }
}
