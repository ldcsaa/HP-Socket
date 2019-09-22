![HP-Socket](https://images.gitee.com/uploads/images/2019/0820/112616_5b8b37bf_81720.png "HP-Socket")
---
*高性能跨平台网络通信框架*
## 描述
- ***Server*** 基于IOCP / EPOLL通信模型，并结合缓存池、私有堆等技术实现高效内存管理，支持超大规模、高并发通信场景。
- ***Agent*** Agent组件实质上是Multi-Client组件，与Server组件采用相同的技术架构。一个Agent组件对象可同时建立和高效处理大规模Socket连接。
- ***Client*** 基于Event Select / POLL通信模型，每个组件对象创建一个通信线程并管理一个Socket连接，适用于小规模客户端场景。
## 文档
- HP-Socket开发指南 
[[pdf]](https://github.com/ldcsaa/HP-Socket/tree/master/Doc)
- HP-Socket基础组件类图 
[[uml]](https://github.com/ldcsaa/HP-Socket/tree/master/Doc)
- HP-Socket基础组件类图 
[[jpg]](https://github.com/ldcsaa/HP-Socket/tree/master/Doc)
- HP-Socket SSL组件类图组件 
[[jpg]](https://github.com/ldcsaa/HP-Socket/tree/master/Doc)
- HP-Socket HTTP组件类图
[[jpg]](https://github.com/ldcsaa/HP-Socket/tree/master/Doc)
## 工作流程
1. 创建监听器
2. 创建通信组件（同时绑定监听器）
3. 启动通信组件
4. 连接到目标主机（*Agent*组件）
5. 处理通信事件（*OnConnect/OnReceive/OnClose*等）
6. 停止通信组件（可选：在第7步销毁通信组件时会自动停止组件）
7. 销毁通信组件
8. 销毁监听器

![Agent工作流程](https://gitee.com/uploads/images/2017/1213/120601_c0d950fb_81720.jpeg "HP-Socket Agent 示例")
## 示例
- ***C++示例***

``` C++
#include <hpsocket/HPSocket.h>

/* Listener Class */
class CListenerImpl : public CTcpPullServerListener
{

public:
	// 5. process network events
	virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen);
	virtual EnHandleResult OnAccept(ITcpServer* pSender, CONNID dwConnID, UINT_PTR soClient);
	virtual EnHandleResult OnHandShake(ITcpServer* pSender, CONNID dwConnID);
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength);
	virtual EnHandleResult OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult OnShutdown(ITcpServer* pSender);
};

int main(int argc, char* const argv[])
{
	// 1. Create listener object
	CListenerImpl s_listener;
	// 2. Create component object (and binding with listener object)
	CTcpPullServerPtr s_pserver(&s_listener);
	
	// 3. Start component object
	if(!s_pserver->Start("0.0.0.0", 5555))
		exit(1);
	
	/* wait for exit */
	// ... ... 
	
	// 6. (optional) Stop component object
	s_pserver->Stop();

	return 0;
	
	// 7. Destroy component object automatically
	// 8. Destroy listener object automatically
}
```

- ***C示例***

``` C
#include <hpsocket/HPSocket4C.h>

// 5. process network events
EnHandleResult __HP_CALL OnConnect(HP_Agent pSender, HP_CONNID dwConnID);
EnHandleResult __HP_CALL OnReceive(HP_Agent pSender, HP_CONNID dwConnID, int iLength);
EnHandleResult __HP_CALL OnSend(HP_Agent pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
EnHandleResult __HP_CALL OnClose(HP_Agent pSender, HP_CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode);
EnHandleResult __HP_CALL OnShutdown(HP_Agent pSender);

int main(int argc, char* const argv[])
{
	HP_TcpPullAgentListener s_listener;
	HP_TcpPullAgent s_agent;

	// 1. Create listener object
	s_listener	= ::Create_HP_TcpPullAgentListener();
	// 2. Create component object (and binding with listener object)
	s_agent		= ::Create_HP_TcpPullAgent(s_listener);
	
	/* Set listener callbacks */
	::HP_Set_FN_Agent_OnConnect(s_listener, OnConnect);
	::HP_Set_FN_Agent_OnSend(s_listener, OnSend);
	::HP_Set_FN_Agent_OnPullReceive(s_listener, OnReceive);
	::HP_Set_FN_Agent_OnClose(s_listener, OnClose);
	::HP_Set_FN_Agent_OnShutdown(s_listener, OnShutdown);
	
	// 3. Start component object
	if(::HP_Agent_HasStarted(s_agent))
		exit(1);
	
	// 4. Connect to dest host
	::HP_Agent_Connect(s_agent, "remote.host.1", REMOTE_PORT_1, nullptr);
	::HP_Agent_Connect(s_agent, "remote.host.2", REMOTE_PORT_2, nullptr);
	::HP_Agent_Connect(s_agent, "remote.host.3", REMOTE_PORT_3, nullptr);
	
	/* wait for exit */
	// ... ... 
	
	// 6. (optional) Stop component object
	::HP_Agent_Stop(s_agent);

	// 7. Destroy component object
	::Destroy_HP_TcpPullAgent(s_agent);
	// 8. Destroy listener object
	::Destroy_HP_TcpPullAgentListener(s_listener);
	
	return 0;
}
```

## 组件列表
- ***基础组件***

![Basic Component](https://images.gitee.com/uploads/images/2019/0331/102422_3eecfdb7_81720.jpeg "基础组件")

- ***SSL组件***

![SSL Component](https://gitee.com/uploads/images/2017/1214/143622_d6c1f436_81720.jpeg "SSL 组件")

- ***HTTP组件***

![HTTP COmponent](https://gitee.com/uploads/images/2017/1214/143640_0eb6f9e4_81720.jpeg "HTTP 组件")

## 引用项目

- *[jemalloc](https://github.com/jemalloc/jemalloc)*
- *[openssl](https://github.com/openssl/openssl)*
- *[http-parser](https://github.com/nodejs/http-parser)*
- *[zlib](https://github.com/madler/zlib)*
- *[kcp](https://github.com/skywind3000/kcp)*

## 第三方扩展项目

- *[HP-Socket for macOS](https://gitee.com/xin_chong/HP-Socket-for-macOS)*

## 捐赠支持

![支付宝](https://images.gitee.com/uploads/images/2019/0918/190004_11ecea7c_81720.jpeg "支付宝")
&nbsp;&nbsp;&nbsp;&nbsp;
![微信](https://images.gitee.com/uploads/images/2019/0918/190115_a1fc5ebf_81720.jpeg "微信")