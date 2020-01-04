# [HPSocket.Net](https://gitee.com/int2e/HPSocket.Net)

#### .Net 框架支持
* `.Net Framework 2.0+`
* `.Net Core 2.0+`

#### 平台支持
* `Windows 7+ x86/x64`
* `Linux kernel 2.6.32+ x86/x64`
* `mac OS 10.12+ x64`

## 安装教程
`HPSocket.Net`通过NuGet软件包管理器部署

在 Package Manager 控制台中使用以下命令来手动安装 `HPSocket.Net`
```
Install-Package HPSocket.Net
```
或在`Visual Studio`的解决方案中的`项目名`上`鼠标右键`->`管理 NuGet 程序包`->`浏览`页面->搜索框输入`HPSocket.Net`->然后安装


### 关于macOS
`HPSocket.Net`现在支持在`osx 10.12+`中使用`.net core2.0+`进行开发

Nuget软件包中的`libhpsocket4c.dylib`编译自`HP-Socket`的`macOS分支`[HP-Socket-for-macOS](https://gitee.com/xin_chong/HP-Socket-for-macOS)


## 组件列表
### 基础组件
基础组件是`HP-Socket`提供的原始组件，相关使用方法请参考[HP-Socket Doc](https://github.com/ldcsaa/HP-Socket/tree/master/Doc)

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

### 扩展组件
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

`HPSocket.Net` 提供一个Tcp端口转发组件`ITcpPortForwarding`，10行代码即可完成TCP端口转发

`HPSocket.Net`目前提供6个Easy组件和2个WebSocket组件，用来更简单的处理http/https/ws的数据包，`HP-Socket`提供的基础http组件，需要自己来实现数据包的完整获取，Easy组件已经做了这些处理，http/https的Easy组件绑定以下事件，当事件到达，即可获得完整数据包
 
+ `OnEasyChunkData` http CHUNK消息的完整数据包事件
+ `OnEasyMessageData` http GET或POST的完整数据包事件
+ `OnEasyWebSocketMessageData` WebSocket消息的完整数据包事件

`WebSocket` 也可以直接使用以下两个组件

+ `IWebSocketServer` WebSocket 服务端
+ `IWebSocketAgent` WebSocket 客户端  （与其他Agent组件不同，WebSocket的Agent组件不支持连接到不同的WebSocket Server，也就是说`IWebSocketAgent`组件所有的连接都只能连接到同一个服务器）



## 使用说明
1. 大部分组件使用方法请参考`demo`目录下的工程
2.  `HPSocket.Net`提供的`Agent`系列组件除`Pack`系列模型外，包括`ITcpPortForwarding`组件，都支持设置`http`或`socks5`代理，以`List<IProxy>`方式设置，可同时设置多个代理，组件内部会随机使用，可以同时混用`http`和`socks5`代理，使用方法参考各`Agent`组件的`demo`


### Easy扩展组件事件绑定示例

#### IHttpEasyServer
```cs
// 创建 HttpEasyServer 的实例
using(IHttpEasyServer httpServer = new HttpEasyServer())
{
    // ...其他设置

    // 绑定 OnEasyMessageData 事件
    httpServer.OnEasyMessageData += (sender, id, data) => 
    {
        // data 参数每次都是一个完整的数据包
        // ... 处理 data

        return HttpParseResult.Ok;
    };
}
```

#### IHttpEasyAgent
```cs
// 创建 HttpEasyAgent 的实例
using(IHttpEasyAgent httpAgent = new HttpEasyAgent())
{
    // ...其他设置

    // 绑定 OnEasyMessageData 事件
    httpAgent.OnEasyMessageData += (sender, id, data) => 
    {
        // data 参数每次都是一个完整的数据包
        // ... 处理 data

        return HttpParseResult.Ok;
    };
}
```

#### IHttpEasyClient
```cs
// 创建 HttpEasyClient 的实例
using(IHttpEasyClient httpClient = new HttpEasyClient())
{
    // ...其他设置

    // 绑定 OnEasyMessageData 事件
    httpClient.OnEasyMessageData += (sender, data) => 
    {
        // data 参数每次都是一个完整的数据包
        // ... 处理 data

        return HttpParseResult.Ok;
    };
}
```


### 数据接收适配器组件

完整示例在`demo/TcpServer-TestEcho-Adapter`

该系列组件是`HPSocket.Net`的`数据接收适配器`扩展组件，用户通过`自定义数据接收适配器`处理TCP通信中可能出现的`粘包`、`半包`等情况。`数据接收适配器`组件看起来这与HP-Socket的`Pack`组件有些相似，但它更加灵活，适配也非常简单方便。


+ `ITcpServer<TRequestBodyType>`/`ITcpClient<TRequestBodyType>`/`ITcpAgent<TRequestBodyType>`
+ `ISslServer<TRequestBodyType>`/`ISslClient<TRequestBodyType>`/`ISslAgent<TRequestBodyType>` 

`<TRequestBodyType>`泛型类型对象将在上列组件的`OnParseRequestBody`事件中回调。

```cs
using (ITcpServer<byte[]> server = new TcpServer<byte[]>
    {
        // 指定数据接收适配器
        DataReceiveAdapter = new BinaryDataReceiveAdapter(),
    })
{
    // 不需要绑定OnReceive事件
    // 这里解析的请求体事件的data就是BinaryDataReceiveAdapter.ParseRequestBody()返回的数据
    // data的类型就是ITcpServer<byte[]>实例化时指定的byte[]
    server.OnParseRequestBody += (sender, id, data) =>
    {
        Console.WriteLine($"OnParseRequestBody({id}) -> data length: {data.Length}");

        return HandleResult.Ok;
    };
}
```

目前支持`4`种适配器

#### 1. `FixedHeaderDataReceiveAdapter` 固定包头数据接收适配器

使用场景：数据包的包头长度固定且包头含包体长度。

示例：前`4字节`标识包体长度(小端字节序)。`0x00000003`表示包体长度为`3字节`，`{ 0x61, 0x62 0x63 }`为包体。
```
{ 0x03, 0x00, 0x00, 0x00, 0x61, 0x62, 0x63 }
```
`FixedHeaderDataReceiveAdapter`专为这种结构设计

子类继承`FixedHeaderDataReceiveAdapter`
在自身的`构造函数`中调用父类构造函数, 传入`包头长度`和`最大允许的封包长度`
覆盖`GetBodySize()`方法, 其参数`header`的长度为构造函数中指定的包头长度, 需用户解析这个参数, 返回实际的`body`长度
覆盖`ParseRequestBody()`方法, 将当前的`bytes`反序列化为泛型类型(`<TRequestBodyType>`)对象

```cs
using System;
using System.Text;
using HPSocket.Adapter;
using Models;
using Newtonsoft.Json;

namespace TcpServerTestEchoAdapter.DataReceiveAdapter
{
    /// <summary>
    /// 固定包头数据接收适配器
    /// </summary>
    public class PacketDataReceiveAdapter : FixedHeaderDataReceiveAdapter<Packet>
    {
        /// <summary>
        /// 调用父类构造函数，指定固定包头的长度及最大封包长度
        /// </summary>
        public PacketDataReceiveAdapter()
            : base(
                headerSize: 4,        // 这里指定4字节包头
                maxPacketSize: 0x1000 // 这里指定最大封包长度不能超过4K
                )
        {

        }

        /// <summary>
        /// 获取请求体长度
        /// <remarks>子类必须覆盖此方法</remarks>
        /// </summary>
        /// <param name="header">包头，header的长度是构造函数里指定的长度，当接收到了指定长度的包头再调用此方法</param>
        /// <returns>返回包体长度</returns>
        protected override int GetBodySize(byte[] header)
        {
            // 根据业务场景来适配字节序, 两端字节序要保持一致

            // 如果当前环境不是小端字节序
            if (!BitConverter.IsLittleEndian)
            {
                // 转换为小端字节序
                Array.Reverse(header);
            }

            // 因为包头是4字节，所以直接转int并返回
            return BitConverter.ToInt32(header, 0);
        }

        /// <summary>
        /// 解析请求体
        /// <remarks>子类必须覆盖此方法</remarks>
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        protected override Packet ParseRequestBody(byte[] data)
        {
            // 将data反序列化为对象
            // 这里是Packet类的对象
            return JsonConvert.DeserializeObject<Packet>(Encoding.UTF8.GetString(data));
        }
    }
}
```


#### 2. `FixedSizeDataReceiveAdapter` 定长包数据接收适配器

包长度固定，每个包都是同样大小，使用这种适配器

`FixedSizeDataReceiveAdapter`专为这种结构设计

```cs
using HPSocket.Adapter;

namespace TcpServerTestEchoAdapter.DataReceiveAdapter
{
    /// <summary>
    /// 定长包数据接收适配器
    /// </summary>
    public class BinaryDataReceiveAdapter : FixedSizeDataReceiveAdapter<byte[]>
    {
        /// <summary>
        /// 调用父类构造函数，指定定长包长度
        /// </summary>
        public BinaryDataReceiveAdapter()
            : base(
                packetSize: 1024 // 定长包包长1K字节
            )
        {
        }

        /// <summary>
        /// 解析请求体
        /// <remarks>子类必须覆盖此方法</remarks>
        /// </summary>
        /// <param name="data">父类处理好的定长数据</param>
        /// <returns></returns>
        protected override byte[] ParseRequestBody(byte[] data)
        {
            // 因为继承自FixedSizeDataReceiveAdapter<byte[]>，所以这里直接返回了，如果是其他类型请做完转换工作再返回
            return data;
        }
    }
}
```


#### 3. `TerminatorDataReceiveAdapter` 结束符数据接收适配器

包头无特征, 包尾使用特定标志作为结束符，使用这种适配器

示例：下面这种包结构以`\r\n`结尾
```
hello world 1\r\n
hello world 2\r\n
hello world 3\r\n
```

`TerminatorDataReceiveAdapter`专为这种结构设计


```cs
using System.Text;
using HPSocket.Adapter;

namespace TcpServerTestEchoAdapter.DataReceiveAdapter
{
    /// <summary>
    /// 结束符数据接收适配器
    /// </summary>
    public class TextDataReceiveAdapter : TerminatorDataReceiveAdapter<string>
    {
        /// <summary>
        /// 调用父类构造函数，指定结束符
        /// </summary>
        public TextDataReceiveAdapter()
            : base(
                terminator: Encoding.UTF8.GetBytes("\r\n") // 指定结束符为\r\n，也就是说每条数据以\r\n结尾，注意编码问题，要两端保持一致
                )
        {
        }

        /// <summary>
        /// 解析请求体
        /// <remarks>子类必须覆盖此方法</remarks>
        /// </summary>
        /// <param name="data">父类已经处理好的不含结束符的数据</param>
        /// <returns></returns>
        protected override string ParseRequestBody(byte[] data)
        {
            // 转换成请求对象, 注意字符编码，要两端保持一致
            return Encoding.UTF8.GetString(data);
        }
    }
}
```


#### 4. `BetweenAndDataReceiveAdapter` 区间数据接收适配器

包头包尾都指定特征符号, 使用这种适配器

示例：数据包头以某特征符号开始，包尾以其它某特征符号结束
```
##hello world!## // ##开头，##结尾
```

或

```
##hello world!|| // ##开头，||结尾
```

或

```
**hello world!|##| // **开头，|##|结尾
```

`BetweenAndDataReceiveAdapter`专为这种结构设计


```cs
using System.Text;
using HPSocket.Adapter;

namespace TcpServerTestEchoAdapter.DataReceiveAdapter
{
    /// <summary>
    /// 区间数据接收适配器
    /// </summary>
    public class HeadTailDataReceiveAdapter : BetweenAndDataReceiveAdapter<string>
    {
        /// <summary>
        /// 调用父类构造函数，指定区间开始和结束特征字符
        /// </summary>
        public HeadTailDataReceiveAdapter() 
            : base( // 例如数据格式是"#*123456*#"，其中以#*开头，以*#结尾，中间123456部分是真实数据
                start : Encoding.UTF8.GetBytes("#*")， // 区间起始标志，这里以#*开始，注意编码问题，要两端保持一致
                end : Encoding.UTF8.GetBytes("*#")  // 区间结束标志，这里以*#开始，注意编码问题，要两端保持一致
                )
        {
        }

        /// <summary>
        /// 解析请求体
        /// <remarks>子类必须覆盖此方法</remarks>
        /// </summary>
        /// <param name="data">父类已处理好的不含区间起始标识符的数据</param>
        /// <returns></returns>
        protected override string ParseRequestBody(byte[] data)
        {
            // 转换成请求对象，注意编码问题，要两端保持一致
            return Encoding.UTF8.GetString(data);
        }
    }
}
```


## 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request