using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using HPSocketCS;
namespace WebsocketClient
{
    class Program
    {
        public static HPSocketCS.HttpAgent agentboj = new HttpAgent();
        public static IntPtr m_connid;
        static void Main(string[] args)
        {
            agentboj.OnHandShake += delegate (IntPtr connid)
            {
                Console.WriteLine("{0}:HandShake Is ok!Send Up Grade", connid);

                THeader[] Header = new THeader[4];
                Header[0].Name = "Sec-WebSocket-Version";
                Header[0].Value = "13";

                Header[1].Name = "User-Agent";
                Header[1].Value = "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36";

                Header[2].Name = "Upgrade";
                Header[2].Value = "websocket";

                Header[3].Name = "Connection";
                Header[3].Value = "Upgrade";

                agentboj.SendRequest(connid, HttpMethod.Get,"/", Header, null,0);

                return 0;
            };
            agentboj.OnUpgrade += delegate (IntPtr connid, HttpUpgradeType upgradeType) {
                Console.WriteLine("{0}:Upgrade Is {1},Send WSMessage",connid, upgradeType);

                if (upgradeType == HttpUpgradeType.WebSocket) {
                    string body = "halo~";

                    agentboj.SendWSMessage(connid,true,0,1,new byte[] {1,2,3,4 }, Encoding.UTF8.GetBytes(body), 0);
                }
                
                return 0;
            };

            agentboj.OnWSMessageBody += delegate (IntPtr connid, byte[] data)
            {
                Console.WriteLine("{0}:WSMessageBody! body:{1}", connid, Encoding.UTF8.GetString(data));

                return 0;
            };


            agentboj.Start("0.0.0.0", !true);

            if (agentboj.Connect("118.25.40.163", 8088, ref m_connid))
            {
                Console.WriteLine("{0}:Connect Ok", m_connid);



            }
            else {
                Console.WriteLine("{0}:Connect fail! Info:{1}", m_connid, agentboj.ErrorMessage);
            }

            while(true){
                string sendbody = Console.ReadLine();
                if (sendbody == "") {
                    break;
                }
                agentboj.SendWSMessage(m_connid, true, 0, 1, new byte[] { 1, 2, 3, 4 }, Encoding.UTF8.GetBytes(sendbody), 0);
            }
            agentboj.Disconnect(m_connid);
            agentboj.Stop();
            agentboj.Destroy();

        }
    }
}
