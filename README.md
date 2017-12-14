# HP-Socket
High Performance Network Framework
## Description
- ***Server*** Based on IOCP/EPOLL communication model, combined with technology of memory pool, private heap etc., efficient memory management is implemented to support large scale and high concurrent communication scenarios.
- ***Agent*** The Agent component is essentially a Multi-Client component that uses the same technical architecture as the Server component. An Agent component object can create and efficiently handle large-scale Socket connections at the same time.
- ***Client*** Based on Event-Select/POLL communication model, each component object creates a communication thread and manages a Socket connection. Client components are suitable for small-scale client scenarios.
## Workflow
1. Create listener object
2. Create component object (and binding with listener object)
3. Start component object
4. Connect to dest host (for *Agent* Component only)
5. process network events (*OnConnect/OnReceive/OnClose* etc.)
6. Stop component object (optional: component object will be stopped before destroy in step 7)
7. Destroy component object
8. Destroy listener object

![Agent Workflow](https://gitee.com/uploads/images/2017/1213/120601_c0d950fb_81720.jpeg "HP-Socket-Agent-Demo.JPG")
## Example
- ***C++ Example***

```
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
	s_pserver->Stop()

	return 0;
	
	// 7. Destroy component object automatically
	// 8. Destroy listener object automatically
}
```

- ***C Example***

```
#include <hpsocket/HPSocket4C.h>

// 5. process network events
EnHandleResult __stdcall OnConnect(HP_Agent pSender, HP_CONNID dwConnID);
EnHandleResult __stdcall OnReceive(HP_Agent pSender, HP_CONNID dwConnID, int iLength);
EnHandleResult __stdcall OnSend(HP_Agent pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
EnHandleResult __stdcall OnClose(HP_Agent pSender, HP_CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode);
EnHandleResult __stdcall OnShutdown(HP_Agent pSender);

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
	::HP_Agent_Stop(s_agent)

	// 7. Destroy component object
	::Destroy_HP_TcpPullAgent(s_agent);
	// 8. Destroy listener object
	::Destroy_HP_TcpPullAgentListener(s_listener);
	
	return 0;
}
```

## Component List
- ***Basic Components***

![Basic Component](https://gitee.com/uploads/images/2017/1213/122948_52f9562e_81720.jpeg "Basic Component.jpg")

- ***SSL Components***

![SSL Component](https://gitee.com/uploads/images/2017/1213/123009_3c4eec14_81720.jpeg "SSL Component.jpg")

- ***HTTP Components***

![HTTP Component](https://gitee.com/uploads/images/2017/1213/123023_872fc11f_81720.jpeg "HTTP Component.jpg")