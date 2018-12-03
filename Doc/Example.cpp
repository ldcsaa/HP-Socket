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