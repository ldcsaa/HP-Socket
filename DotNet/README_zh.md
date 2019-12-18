# [HPSocket.Net](https://gitee.com/int2e/HPSocket.Net)

## 概览
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
基础组件是`HP-Socket`提供的原始组件, 相关使用方法请参考[HP-Socket Doc](https://github.com/ldcsaa/HP-Socket/tree/master/Doc)

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

`HPSocket.Net` 提供一个Tcp端口转发组件`ITcpPortForwarding`, 10行代码即可完成TCP端口转发

`HPSocket.Net`目前提供6个Easy组件和2个WebSocket组件, 用来更简单的处理http/https/ws的数据包, `HP-Socket`提供的基础http组件, 需要自己来实现数据包的完整获取, Easy组件已经做了这些处理, http/https的Easy组件绑定以下事件, 当事件到达, 即可获得完整数据包
 
+ `OnEasyChunkData` http CHUNK消息的完整数据包事件
+ `OnEasyMessageData` http GET或POST的完整数据包事件
+ `OnEasyWebSocketMessageData` WebSocket消息的完整数据包事件

`WebSocket` 也可以直接使用以下两个组件

+ `IWebSocketServer` WebSocket 服务端
+ `IWebSocketAgent` WebSocket 客户端  （与其他Agent组件不同, WebSocket的Agent组件不支持连接到不同的WebSocket Server, 也就是说`IWebSocketAgent`组件所有的连接都只能连接到同一个服务器）


## 使用说明
1. 大部分组件使用方法请参考`demo`目录下的工程
2. `HPSocket.Net`提供的`Agent`系列组件除`Pack`系列模型外, 包括`ITcpPortForwarding`组件, 都支持设置`http`或`socks5`代理, 以`List<IProxy>`方式设置, 可同时设置多个代理, 组件内部会随机使用, 可以同时混用`http`和`socks5`代理, 使用方法参考各`Agent`组件的`demo`

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

## 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request