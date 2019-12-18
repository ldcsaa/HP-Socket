# [HP-Socket for MacOS](https://gitee.com/xin_chong/HP-Socket-for-macOS)

### Project Introduction
Due to platform restrictions, some mechanisms may not be available properly.

Known mechanisms that cannot be provided normally:

1. Keep Live time in TCP protocol: `TCP_KEEPALIVE`,` TCP_KEEPINTVL` and `TCP_KEEPCNT`（[Solution](http://www.voidcn.com/article/p-afuibcmg-bqk.html)）。

2. Platform version (MacOS version) must be `10.12` or above (included).

The requirements for the platform version are mainly dependent on `shared_timed_mutex` in the component` RWLock.h`, and according to the definition of `_LIBCPP_AVAILABILITY_SHARED_MUTEX`, the platform version is required (if boost is used to replace this, it can also solve this problem).

HP-Socket for MacOS just provides the same callback interface as on other platforms, and internally uses some other mechanisms and components to replace corresponding incompatible components.

#### Mechanism

`epoll` => `kqueue`  
`ONESHOT`=>`DISPATCH`

```tex
`DISPATCH` is equivalent to` edge-triggered (ET) `in epoll.` ONESHOT` will be deleted from the kernel in MacOS. 
For large data exchange, it means a large number of deletions and additions. 
Using ET (DISPATCH) will only Resetting the state (ENABLE) makes it invalid.
```

### Supported

- `TcpServer/Client` 
- `UdpServer/Client`
- `TcpPackServer/Client`
- `TcpAgent`
- `TcpPackAgent`
- `Pull`
- `HttpClient/Server`
- `ARQ-UdpServer/Client`
- `SSL`

------

### Files and Directory

| Directory Mame                  | Description                           |
| ----------------------- | ------------------------------ |
| hpsocket                | All files related to hpsocket |
| hpsocket\src            | hpsocket source file               |
| hpsocket\ssl-cert       | Testing SSL related certificates              |
| hpsocket\test           | Some simple test examples             |
| hpsocket\CMakeLists.txt | cmake configuration file                |
| hpsocket\4C             | C-like interface provided by hpsocket      |

Note the dependency header files and library paths in the example.

> CMakeLists.txt

```
include_directories(/opt/local/include)
link_directories(/opt/local/lib)

link_libraries(ssl iconv z crypto)
add_definitions(-D_NEED_HTTP -D_SSL_SUPPORT -D_NEED_SSL)
```

------

### How to use the C interface?

> Place the files in `4C` directory into `src` directory and delete the following files

```tex
HPSocket.cpp
HPSocket.h
HPSocket-SSL.cpp
HPSocket-SSL.h
```

And provide an official http example `test_server_4c` using 4C interface

Full support for official usage examples

------

### How to install related dependencies?

By using `Brew` or `MacPort`; `MacPort` is used in the example.

The following dependencies are installed:

`openssl`, `zlib`