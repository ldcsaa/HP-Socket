using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WebSocketDemo
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                ushort port = 8081;
                Console.Title = port.ToString();
                // websocket server
                new MyWebSocketServer().Run("0.0.0.0", port);
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
