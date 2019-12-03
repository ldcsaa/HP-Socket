![HP-Socket](https://images.gitee.com/uploads/images/2019/0820/112616_5b8b37bf_81720.png "HP-Socket")
---
*High Performance Network Framework*
## Description
- ***Server*** Based on IOCP/EPOLL communication model, combined with technology of memory pool, private heap etc., efficient memory management is implemented to support large scale and high concurrent communication scenarios.
- ***Agent*** The Agent component is essentially a Multi-Client component that uses the same technical architecture as the Server component. An Agent component object can create and efficiently handle large-scale Socket connections at the same time.
- ***Client*** Based on Event-Select/POLL communication model, each component object creates a communication thread and manages a Socket connection. Client components are suitable for small-scale client scenarios.
## Document
- HP-Socket Development Guide 
[[pdf]](https://github.com/ldcsaa/HP-Socket/tree/master/Doc)
- HP-Socket Class Diagram 
[[uml]](https://github.com/ldcsaa/HP-Socket/tree/master/Doc)
- HP-Socket Class Diagram 
[[jpg]](https://github.com/ldcsaa/HP-Socket/tree/master/Doc)
- HP-Socket SSL Class Diagram 
[[jpg]](https://github.com/ldcsaa/HP-Socket/tree/master/Doc)
- HP-Socket HTTP Class Diagram 
[[jpg]](https://github.com/ldcsaa/HP-Socket/tree/master/Doc)
## Workflow
1. Create listener object
2. Create component object (and binding with listener object)
3. Start component object
4. Connect to dest host (for *Agent* Component only)
5. process network events (*OnConnect/OnReceive/OnClose* etc.)
6. Stop component object (optional: component object will be stopped before destroy in step 7)
7. Destroy component object
8. Destroy listener object

![Agent Workflow](https://gitee.com/uploads/images/2017/1213/120601_c0d950fb_81720.jpeg "HP-Socket Agent Demo")
## Example
- ***C++ Example***

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

- ***C Example***

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

## Component List
- ***Basic Components***

![Basic Component](https://images.gitee.com/uploads/images/2019/0331/102422_3eecfdb7_81720.jpeg "Basic Component")

- ***SSL Components***

![SSL Component](https://gitee.com/uploads/images/2017/1214/143622_d6c1f436_81720.jpeg "SSL Component")

- ***HTTP Components***

![HTTP COmponent](https://gitee.com/uploads/images/2017/1214/143640_0eb6f9e4_81720.jpeg "HTTP Component")

## Reference Projects

- *[jemalloc](https://github.com/jemalloc/jemalloc)*
- *[openssl](https://github.com/openssl/openssl)*
- *[http-parser](https://github.com/nodejs/http-parser)*
- *[zlib](https://github.com/madler/zlib)*
- *[kcp](https://github.com/skywind3000/kcp)*

## Third-party Extension Projects

- *[HP-Socket for macOS](https://gitee.com/xin_chong/HP-Socket-for-macOS)*
- *[HP-Socket for .Net](https://gitee.com/int2e/HPSocket.Net)*

## Donation

![Alibaba Pay](https://images.gitee.com/uploads/images/2019/0918/190004_11ecea7c_81720.jpeg "Alibaba Pay")
&nbsp;&nbsp;&nbsp;&nbsp;
![Wechat Pay](https://images.gitee.com/uploads/images/2019/0918/190115_a1fc5ebf_81720.jpeg "Wechat Pay")