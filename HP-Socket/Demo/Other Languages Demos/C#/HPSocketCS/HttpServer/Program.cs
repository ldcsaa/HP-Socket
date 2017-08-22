using HPSocketCS;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HttpServerDemo
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                /*
                // 获取本机所有ip地址
                //var list = DnsHelper.GetLocalHostAddresses(); // 第一种方法
                var list = DnsHelper.GetLocalHostAddressesToIPAddrList();// 第二种方法
                foreach (var item in list)
                {
                    //Console.WriteLine($"类型:{item.AddressFamily}, 地址:{item.ToString()}");// 第一种方法
                    Console.WriteLine($"类型:{item.Type}, 地址:{item.Address}");// 第二种方法
                }
                */
                

                ushort port = 8081;
                Console.Title = port.ToString();
                // http server
                new MyHttpServer().Run("0.0.0.0", port);
                do
                {
                    Console.WriteLine("输入quit退出!");
                } while (Console.ReadLine() != "quit");

            }
            catch (Exception ex)
            {
                Console.WriteLine("ErrorMsg:{0}\r\n\t{1}", ex.Message, ex.StackTrace);
            }
        }
    }
}
