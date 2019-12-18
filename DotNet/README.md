# [HPSocket.Net](https://gitee.com/int2e/HPSocket.Net)

## Overview
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
### Basic COmponents
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

## Contribute

1.  Fork this Repository
2.  Create a new Feat_xxx branch
3.  Submit code
4.  Create a new Pull Request