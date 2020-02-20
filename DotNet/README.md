# [HPSocket.Net](https://gitee.com/int2e/HPSocket.Net)

#### .Net Framework Supported
* `.Net Framework 2.0+`
* `.Net Core 2.0+`

#### Platform supported
* `Windows 7+ x86/x64`
* `Linux kernel 2.6.32+ x86/x64`
* `mac OS 10.12+ x64`

## Installation Guide
`HPSocket.Net` deploy via NuGet package manager

Use the following command in the Package Manager console to manually install `HPSocket.Net`
```
Install-Package HPSocket.Net
```
Or right-click on the project name in the solution of Visual Studio-> Manage NuGet Packages-> Browse the page-> search box and enter HPSocket.Net and then install


### About macOS
`HPSocket.Net` now supports development using` .net core2.0 + `in` osx 10.12 + `

`Libhpsocket4c.dylib` in Nuget package is compiled from [HP-Socket-for-macOS](https://gitee.com/xin_chong/HP-Socket-for-macOS)


## Components List
### Basic Components
Basic component is the original component provided by HP-Socket. For related usage, please refer to [HP-Socket Doc](https://github.com/ldcsaa/HP-Socket/tree/master/Doc)

##### TCP
+ `ITcpServer`
+ `ITcpAgent`
+ `ITcpClient`
+ `ITcpPullServer`
+ `ITcpPullAgent`
+ `ITcpPullClient`
+ `ITcpPackServer`
+ `ITcpPackAgent`
+ `ITcpPacClient`

##### UDP
+ `IUdpServer`
+ `IUdpClient`
+ `IUdpCast`
+ `IUdpArqServer`
+ `IUdpArqClient`
+ `IUdpNode`

##### SSL
+ `ISslServer`
+ `ISslAgent`
+ `ISslClient`
+ `ISslPullServer`
+ `ISslPullAgent`
+ `ISslPullClient`
+ `ISslPackServer`
+ `ISslPackAgent`
+ `ISslPackClient`

##### HTTP
+ `IHttpServer`
+ `IHttpsServer`
+ `IHttpAgent`
+ `IHttpsAgent`
+ `IHttpClient`
+ `IHttpsClient`
+ `IHttpSyncClient`
+ `IHttpsSyncClient`

#### ThreadPool
+ `ThreadPool`

### Extended components
+ `ITcpPortForwarding`
+ `IHttpEasyServer`
+ `IHttpsEasyServer`
+ `IHttpEasyAgent`
+ `IHttpsEasyAgent`
+ `IHttpEasyClient`
+ `IHttpsEasyClient`
+ `IWebSocketServer`
+ `IWebSocketAgent`
+ `ITcpServer<TRequestBodyType>`
+ `ITcpClient<TRequestBodyType>`
+ `ITcpAgent<TRequestBodyType>`
+ `ISslServer<TRequestBodyType>`
+ `ISslClient<TRequestBodyType>`
+ `ISslAgent<TRequestBodyType>`

`HPSocket.Net` provides a TCP port forwarding component` ITcpPortForwarding`, 10 lines of code can complete TCP port forwarding.

`HPSocket.Net` currently provides 6 Easy components and 2 WebSocket components for easier processing of http / https / ws data packets. The basic http components provided by `HP-Socket` need to implement the data packets themselves. Complete acquisition, Easy component has done these processing, http / https Easy component is bound to the following events, when the event arrives, you can get the complete data packet.
 
+ `OnEasyChunkData` Complete packet event for http CHUNK message
+ `OnEasyMessageData` Complete packet event for http GET or POST message
+ `OnEasyWebSocketMessageData` Complete packet event for WebSocket message

`WebSocket` can also use the following two components directly

+ `IWebSocketServer` WebSocket Server
+ `IWebSocketAgent` WebSocket Client (Unlike other Agent components, the WebSocket Agent component does not support connecting to different WebSocket Servers, which means that all connections of the `IWebSocketAgent` component can only connect to the same server)


## Instructions
1. For the use of most components, please refer to the project in the `demo` directory.
2. In addition to the `Pack` series model, the `Agent` series components provided by `HPSocket.Net` (including the `ITcpPortForwarding` component) support to setting `HTTP` or `Socks5` proxy, which can be set in the manner of`List<IProxy>`. Multiple proxies can be set at the same time, which will be used randomly, and can be mixed with `HTTP` and `Socks5` proxy at the same time. For the usage method, refer to the `demo` of each` Agent` component.

### Easy component event binding example

#### IHttpEasyServer
```cs
// Create HttpEasyServer instance
using(IHttpEasyServer httpServer = new HttpEasyServer())
{
    // ... other settings

    // Binding OnEasyMessageData event
    httpServer.OnEasyMessageData += (sender, id, data) => 
    {
        // The data parameter is a complete packet each time
        // ... Process data

        return HttpParseResult.Ok;
    };
}
```

#### IHttpEasyAgent
```cs
// Create HttpEasyAgent instance
using(IHttpEasyAgent httpAgent = new HttpEasyAgent())
{
    // ... other settings

    // Binding OnEasyMessageData event
    httpAgent.OnEasyMessageData += (sender, id, data) => 
    {
        // The data parameter is a complete packet each time
        // ... Process data

        return HttpParseResult.Ok;
    };
}
```

#### IHttpEasyClient
```cs
// Create HttpEasyClient instance
using(IHttpEasyClient httpClient = new HttpEasyClient())
{
    // ... other settings

    // Binding OnEasyMessageData event
    httpClient.OnEasyMessageData += (sender, data) => 
    {
        // The data parameter is a complete packet each time
        // ... Process data

        return HttpParseResult.Ok;
    };
}
```


### Data receiving adapter component

Full Example: `demo/TcpServer-TestEcho-Adapter`

This series of components are `Data Receive Adapter` extension components of `HPSocket.Net`. Users can process `sticky packets`,`half packets`, etc. that may occur in TCP communication through `Custom Data Receive Adapter`. The `Data Receive Adapter` component looks similar to the `Pack` component of HP-Socket, but it is more flexible and the adaptation is very simple and convenient.


+ `ITcpServer<TRequestBodyType>`/`ITcpClient<TRequestBodyType>`/`ITcpAgent<TRequestBodyType>`
+ `ISslServer<TRequestBodyType>`/`ISslClient<TRequestBodyType>`/`ISslAgent<TRequestBodyType>` 

`<TRequestBodyType>` generic type object will be called back in the `OnParseRequestBody` event of the components listed above.

```cs
using (ITcpServer<byte[]> server = new TcpServer<byte[]>
    {
        // Specify the data receiving adapter
        DataReceiveAdapter = new BinaryDataReceiveAdapter(),
    })
{
    // No need to bind OnReceive event
    // The data of the request body event parsed here is the data returned by BinaryDataReceiveAdapter.ParseRequestBody ()
    // The type of data is the byte[] specified when ITcpServer <byte[]> is instantiated
    server.OnParseRequestBody += (sender, id, data) =>
    {
        Console.WriteLine($"OnParseRequestBody({id}) -> data length: {data.Length}");

        return HandleResult.Ok;
    };
}
```

`4` adapters currently supported

#### 1. `FixedHeaderDataReceiveAdapter` Fixed header data receiving adapter

Usage scenario: The header length of the data packet is fixed and the header contains the body length.

Example: The first `4 bytes` identifies the body length (little-endian). `0x00000003` indicates that the length of the body is` 3 bytes`, and `{0x61, 0x62 0x63}` is the body.
```
{ 0x03, 0x00, 0x00, 0x00, 0x61, 0x62, 0x63 }
```
`FixedHeaderDataReceiveAdapter` Designed for this structure

Subclasses inherit `FixedHeaderDataReceiveAdapter`
Call the parent class constructor in its own `constructor`, passing in` header length` and `maximum allowed packet length`
Override the `GetBodySize ()` method, the length of its parameter `header` is the length of the header specified in the constructor, the user needs to parse this parameter and return the actual` body` length
Override the `ParseRequestBody ()` method to deserialize the current `bytes` into a generic type (` <TRequestBodyType> `) object

```cs
using System;
using System.Text;
using HPSocket.Adapter;
using Models;
using Newtonsoft.Json;

namespace TcpServerTestEchoAdapter.DataReceiveAdapter
{
    /// <summary>
    /// Fixed header data receiving adapter
    /// </summary>
    public class PacketDataReceiveAdapter : FixedHeaderDataReceiveAdapter<Packet>
    {
        /// <summary>
        /// Call the parent class constructor, specifying the length of the fixed header and the maximum packet length
        /// </summary>
        public PacketDataReceiveAdapter()
            : base(
                headerSize: 4,        // Specify a 4-byte header here
                maxPacketSize: 0x1000 // The maximum packet length specified here cannot exceed 4K
                )
        {

        }

        /// <summary>
        /// Get request body length
        /// <remarks>Subclasses must override this method</remarks>
        /// </summary>
        /// <param name="header">Header. The length of the header is the length specified in the constructor. This method is called when a header of the specified length is received.</param>
        /// <returns>Returns the body length</returns>
        protected override int GetBodySize(byte[] header)
        {
            // Adapt the byte order according to the business scenario, and the byte order at both ends must be consistent.

            // If the current environment is not little-endian
            if (!BitConverter.IsLittleEndian)
            {
                // Convert to little-endian byte order
                Array.Reverse(header);
            }

            // Because the header is 4 bytes, it is directly converted to int and returned
            return BitConverter.ToInt32(header, 0);
        }

        /// <summary>
        /// Parsing the request body
        /// <remarks>Subclasses must override this method</remarks>
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        protected override Packet ParseRequestBody(byte[] data)
        {
            // Deserialize data into object
            // Here is the object of the Packet class
            return JsonConvert.DeserializeObject<Packet>(Encoding.UTF8.GetString(data));
        }
    }
}
```


#### 2. `FixedSizeDataReceiveAdapter` Fixed-length packet data receiving adapter

The packet length is fixed, and each packet is the same size. Use this adaptation mode.

`FixedSizeDataReceiveAdapter` Designed for this structure

```cs
using HPSocket.Adapter;

namespace TcpServerTestEchoAdapter.DataReceiveAdapter
{
    /// <summary>
    /// Fixed-length packet data receiving adapter
    /// </summary>
    public class BinaryDataReceiveAdapter : FixedSizeDataReceiveAdapter<byte[]>
    {
        /// <summary>
        /// Call the parent class constructor and specify the fixed-length package length
        /// </summary>
        public BinaryDataReceiveAdapter()
            : base(
                packetSize: 1024 // Fixed length packet length 1K bytes
            )
        {
        }

        /// <summary>
        /// Parsing the request body
        /// <remarks>Subclasses must override this method</remarks>
        /// </summary>
        /// <param name="data">Fixed-length data handled by the parent class</param>
        /// <returns></returns>
        protected override byte[] ParseRequestBody(byte[] data)
        {
            // Because inherited from FixedSizeDataReceiveAdapter <byte[]>, it is returned directly here, if it is other types, please complete the conversion and then return
            return data;
        }
    }
}
```


#### 3. `TerminatorDataReceiveAdapter` Terminator data receiving adapter

There is no feature in the header, and the end of the packet uses a specific flag as the terminator. Use this adapter.

Example: The following package structure ends with `\r\n`.
```
hello world 1\r\n
hello world 2\r\n
hello world 3\r\n
```

`TerminatorDataReceiveAdapter` Designed for this structure


```cs
using System.Text;
using HPSocket.Adapter;

namespace TcpServerTestEchoAdapter.DataReceiveAdapter
{
    /// <summary>
    /// Terminator data receiving adapter
    /// </summary>
    public class TextDataReceiveAdapter : TerminatorDataReceiveAdapter<string>
    {
        /// <summary>
        /// Call parent constructor, specify terminator
        /// </summary>
        public TextDataReceiveAdapter()
            : base(
                terminator: Encoding.UTF8.GetBytes("\r\n") // Specify the terminator as \r\n, which means that each piece of data ends with \r\n. Pay attention to the encoding problem, and keep the two sides consistent.
                )
        {
        }

        /// <summary>
        /// Parsing the request body
        /// <remarks>Subclasses must override this method</remarks>
        /// </summary>
        /// <param name="data">Data that has been processed by the parent class, without terminator.</param>
        /// <returns></returns>
        protected override string ParseRequestBody(byte[] data)
        {
            // Converted to the request object, pay attention to the character encoding, both sides must be consistent.
            return Encoding.UTF8.GetString(data);
        }
    }
}
```


#### 4. `BetweenAndDataReceiveAdapter` Interval data receiving adapter

Both the header and the end of the package specify the characteristic symbol, using this adapter.

Example: The packet header starts with a certain characteristic symbol, and the packet tail ends with some other characteristic symbol.
```
##hello world!## // begin with ##, end with ##
```

OR

```
##hello world!|| // begin with ##, end with ||
```

OR

```
**hello world!|##| // begin with **, end with |##|
```

`BetweenAndDataReceiveAdapter` Designed for this structure


```cs
using System.Text;
using HPSocket.Adapter;

namespace TcpServerTestEchoAdapter.DataReceiveAdapter
{
    /// <summary>
    /// Interval data receiving adapter
    /// </summary>
    public class HeadTailDataReceiveAdapter : BetweenAndDataReceiveAdapter<string>
    {
        /// <summary>
        /// Call the parent class constructor, specifying the start and end characteristic characters of the section
        /// </summary>
        public HeadTailDataReceiveAdapter() 
            : base( // For example, the data format is "#* 123456 *#", which starts with #* and ends with *#, and the middle 123456 part is the real data
                start : Encoding.UTF8.GetBytes("#*"),  // Section start identifier, starting with #* here, pay attention to encoding issues, and be consistent at both sides
                end : Encoding.UTF8.GetBytes("*#")  // Section end identifier, starting with *# here, pay attention to encoding issues, and be consistent at both sides
                )
        {
        }

        /// <summary>
        /// Parsing the request body
        /// <remarks>Subclasses must override this method</remarks>
        /// </summary>
        /// <param name="data">Data that has not been processed by the parent class identifier character</param>
        /// <returns></returns>
        protected override string ParseRequestBody(byte[] data)
        {
            // Converted to the request object, pay attention to the character encoding, both sides must be consistent.
            return Encoding.UTF8.GetString(data);
        }
    }
}
```


## Contribute

1.  Fork this Repository
2.  Create a new Feat_xxx branch
3.  Submit code
4.  Create a new Pull Request