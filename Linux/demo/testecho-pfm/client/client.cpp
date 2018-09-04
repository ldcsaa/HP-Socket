#include "../../global/helper.h"
#include "../../../src/TcpClient.h"

client_statistics_info s_stat;

class CListenerImpl : public CTcpClientListener
{

public:
	virtual EnHandleResult OnPrepareConnect(ITcpClient* pSender, CONNID dwConnID, SOCKET socket) override
	{
		return HR_OK;
	}

	virtual EnHandleResult OnConnect(ITcpClient* pSender, CONNID dwConnID) override
	{
#ifdef _DEBUG
		::PostOnConnect3(dwConnID);
#endif
		
		return HR_OK;
	}

	virtual EnHandleResult OnHandShake(ITcpClient* pSender, CONNID dwConnID) override
	{
		return HR_OK;
	}

	virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
#ifdef _DEBUG2
		::PostOnReceive(dwConnID, pData, iLength);
#endif
		s_stat.AddTotalRecv(iLength);

		return HR_OK;
	}

	virtual EnHandleResult OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
#ifdef _DEBUG2
		::PostOnSend(dwConnID, pData, iLength);
#endif
		s_stat.AddTotalSend(iLength);

		return HR_OK;
	}

	virtual EnHandleResult OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) override
	{
		if(iErrorCode == SE_OK)
		{
#ifdef _DEBUG
			::PostOnClose(dwConnID);
#endif
		}
		else
		{
			::PostOnError(dwConnID, enOperation, iErrorCode);
		}

		return HR_OK;
	}

};

CListenerImpl s_listener;
vector<ITcpClient*> s_vtClients;
CBufferPtr s_sendBuffer;

void OnCmdStart(CCommandParser* pParser)
{
	if(!s_vtClients.empty())
	{
		::LogClientStartFail(SE_ILLEGAL_STATE, ::GetSocketErrorDesc(SE_ILLEGAL_STATE));
		return;
	}

	s_stat.Reset();

	for(DWORD i = 0; i < g_app_arg.conn_count; i++)
	{
		unique_ptr<ITcpClient> pSocket = make_unique<CTcpClient>(&s_listener);

		pSocket->SetKeepAliveTime(g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);

		if(pSocket->Start(g_app_arg.remote_addr, g_app_arg.port, FALSE))
			s_vtClients.push_back(pSocket.release());
		else
		{
			::LogClientStartFail(pSocket->GetLastError(), pSocket->GetLastErrorDesc());
			ClearPtrSet(s_vtClients);
			return;
		}
	}

	::LogClientStart(g_app_arg.remote_addr, g_app_arg.port);

	DWORD dwSendDelay = 3;
	CString strMsg;

	strMsg.Format(_T("*** willing to send data after %d seconds ***"), dwSendDelay);
	::LogMsg(strMsg);

	::WaitFor(dwSendDelay * 1000);
	s_sendBuffer.Malloc(g_app_arg.data_length, true);

	::LogMsg(_T("*** Go Now ! ***"));

	s_stat.StartTest();

	BOOL bTerminated = FALSE;
	for(DWORD i = 0; i < g_app_arg.test_times; i++)
	{
		for(DWORD j = 0; j < g_app_arg.conn_count; j++)
		{
			ITcpClient* pSocket = s_vtClients[j];
			if(!pSocket->Send(s_sendBuffer, (int)s_sendBuffer.Size()))
			{
				::LogClientSendFail(i + 1, j + 1, ::GetLastError(), ::GetSocketErrorDesc(SE_DATA_SEND));
				bTerminated = TRUE;
				break;
			}
		}

		if(bTerminated)
			break;

		if(g_app_arg.test_interval > 0 && i + 1 < g_app_arg.test_times)
			::WaitFor(g_app_arg.test_interval);
	}

	if(bTerminated)
		::ClearPtrSet(s_vtClients);

	s_sendBuffer.Free();
}

void OnCmdStop(CCommandParser* pParser)
{
	if(s_vtClients.empty())
	{
		::LogClientStopFail(SE_ILLEGAL_STATE, ::GetSocketErrorDesc(SE_ILLEGAL_STATE));
		return;
	}

	::LogClientStopping(s_vtClients.size());

	::ClearPtrSet(s_vtClients);

	s_stat.CheckStatistics();
}

void OnCmdStatus(CCommandParser* pParser)
{
	pParser->PrintStatus(s_vtClients.empty() ? SS_STOPPED : SS_STARTED);
}

int main(int argc, char* const argv[])
{
	CTermAttrInitializer term_attr;
	CAppSignalHandler s_signal_handler({SIGTTOU, SIGINT});

	g_app_arg.ParseArgs(argc, argv);

	g_app_arg.async = FALSE;
	g_app_arg.ShowPFMTestArgs(FALSE);

	CCommandParser::CMD_FUNC fnCmds[CCommandParser::CT_MAX] = {0};

	fnCmds[CCommandParser::CT_START]  = OnCmdStart;
	fnCmds[CCommandParser::CT_STOP]	  = OnCmdStop;
	fnCmds[CCommandParser::CT_STATUS] = OnCmdStatus;

	CCommandParser s_cmd_parser(CCommandParser::AT_CLIENT, fnCmds);
	s_cmd_parser.Run();

	if(!s_vtClients.empty())
		OnCmdStop(&s_cmd_parser);

	return EXIT_CODE_OK;
}
