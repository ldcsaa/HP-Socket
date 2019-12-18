# [HP-Socket for MacOS](https://gitee.com/xin_chong/HP-Socket-for-macOS)

#### 项目介绍
由于平台限制某些机制某些机制会无法正常提供。

己知无法正常提供机制：

1、TCP协议中设置Keep live 时间:`TCP_KEEPALIVE`、`TCP_KEEPINTVL`和`TCP_KEEPCNT`（[解决方法](http://www.voidcn.com/article/p-afuibcmg-bqk.html)）。

2、平台版本(MacOS系统版本)不能低于`10.12`版本。

对平台版本的要求主要是其中组件`RWLock.h`中依赖`shared_timed_mutex`，而根据`_LIBCPP_AVAILABILITY_SHARED_MUTEX`定义对平台版本有所要求(若引入boost进行替代也可解决此问题)。

HP-Socket for MacOS版本只是提供与其他平台上相同的回调接口，内部使用其他一些机制与组件替换相应不兼容的组件。

#### 机制

`epoll` => `kqueue`  
`ONESHOT`=>`DISPATCH`

```tex
`DISPATCH`等同于epoll中的`edge-triggered (ET)`，在MacOS中`ONESHOT`会从内核中删除，
而对于大量数据交换就代表有大量删除与添加，而使用ET(DISPATCH)只会重置状态(ENABLE)使其无效。
```

#### 支持

- `TcpServer/Client` 
- `UdpServer/Client`
- `TcpPackServer/Client`
- `TcpAgent`
- `TcpPackAgent`
- Pull
- `HttpClient/Server`
- `ARQ-UdpServer/Client`
- `SSL`

------

#### 文件目录介绍

| 目录名                  | 描述                           |
| ----------------------- | ------------------------------ |
| hpsocket                | hpsocket网络框架相关的所有文件 |
| hpsocket\src            | hpsocket的源文件               |
| hpsocket\ssl-cert       | 测试ssl的相关证书              |
| hpsocket\test           | 一些简单的测试例子             |
| hpsocket\CMakeLists.txt | cmake的配置文件                |
| hpsocket\4C             | hpsocket框架提供的类C接口      |

提供的例子中请注意依赖头文件与库路径。

> CMakeLists.txt

```
include_directories(/opt/local/include)
link_directories(/opt/local/lib)

link_libraries(ssl iconv z crypto)
add_definitions(-D_NEED_HTTP -D_SSL_SUPPORT -D_NEED_SSL)
```

------

#### 如何使用C接口？

> 将`4C`文件夹下的文件放置到`src`文件夹，删除以下文件

```tex
HPSocket.cpp
HPSocket.h
HPSocket-SSL.cpp
HPSocket-SSL.h
```

提供了一个官方使用4C接口的http例子`test_server_4c`

完全支持官方使用示例

------

#### 相关依赖如何安装？

通过使用brew或MacPort；例子中使用的是MacPort。

具体安装了以下依赖：

`openssl`, `zlib`