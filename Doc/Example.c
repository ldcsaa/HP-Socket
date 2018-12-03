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
	::HP_Agent_Stop(s_agent)

	// 7. Destroy component object
	::Destroy_HP_TcpPullAgent(s_agent);
	// 8. Destroy listener object
	::Destroy_HP_TcpPullAgentListener(s_listener);
	
	return 0;
}