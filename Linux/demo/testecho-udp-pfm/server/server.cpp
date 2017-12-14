#include "../../global/helper.h"
#include "../../../src/UdpServer.h"

server_statistics_info s_stat;

class CListenerImpl : public CUdpServerListener
{

public:
	virtual EnHandleResult OnPrepareListen(IUdpServer* pSender, SOCKET soListen) override
	{
		TCHAR szAddress[50];
		int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
		USHORT usPort;

		pSender->GetListenAddress(szAddress, iAddressLen, usPort);
		::PostOnPrepareListen(szAddress, usPort);

		VERIFY(::SSO_RecvBuffSize(soListen, 4 * 1024 * 1024) == NO_ERROR);

		return HR_OK;
	}

	virtual EnHandleResult OnAccept(IUdpServer* pSender, CONNID dwConnID, UINT_PTR soClient) override
	{
		s_stat.CheckClientCount();
		::PostOnAccept2(dwConnID);

		return HR_OK;
	}

	virtual EnHandleResult OnHandShake(IUdpServer* pSender, CONNID dwConnID) override
	{
		return HR_OK;
	}

	virtual EnHandleResult OnReceive(IUdpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
#ifdef _DEBUG2
		::PostOnReceive(dwConnID, pData, iLength);
#endif

		s_stat.AddTotalRecv(iLength);

		if(pSender->Send(dwConnID, pData, iLength))
			return HR_OK;
		
		return HR_ERROR;
	}

	virtual EnHandleResult OnSend(IUdpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
#ifdef _DEBUG2
		::PostOnSend(dwConnID, pData, iLength);
#endif
		s_stat.AddTotalSend(iLength);

		return HR_OK;
	}

	virtual EnHandleResult OnClose(IUdpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) override
	{
		iErrorCode == SE_OK ? ::PostOnClose(dwConnID)	:
		::PostOnError(dwConnID, enOperation, iErrorCode);

		s_stat.CheckStatistics();

		return HR_OK;
	}

	virtual EnHandleResult OnShutdown(IUdpServer* pSender) override
	{
		::PostOnShutdown();
		return HR_OK;
	}

};

CListenerImpl s_listener;
CUdpServer s_server(&s_listener);

void OnCmdStart(CCommandParser* pParser)
{
	if(s_server.HasStarted())
	{
		::LogServerStartFail(SE_ILLEGAL_STATE, ::GetSocketErrorDesc(SE_ILLEGAL_STATE));
		return;
	}

	s_stat.Reset();

	s_server.SetWorkerThreadCount(g_app_arg.thread_count);
	s_server.SetMaxConnectionCount(g_app_arg.max_conn);
	s_server.SetSendPolicy(g_app_arg.send_policy);
	s_server.SetDetectAttempts(g_app_arg.keep_alive ? UDP_DETECT_ATTEMPTS : 0);

	if(s_server.Start(g_app_arg.bind_addr, g_app_arg.port))
		::LogServerStart(g_app_arg.bind_addr, g_app_arg.port);
	else
		::LogServerStartFail(s_server.GetLastError(), s_server.GetLastErrorDesc());
}

void OnCmdStop(CCommandParser* pParser)
{
	if(!s_server.HasStarted())
	{
		::LogServerStopFail(SE_ILLEGAL_STATE, ::GetSocketErrorDesc(SE_ILLEGAL_STATE));
		return;
	}

	if(s_server.Stop())
		::LogServerStop();
	else
		::LogServerStopFail(s_server.GetLastError(), s_server.GetLastErrorDesc());
}

void OnCmdStatus(CCommandParser* pParser)
{
	pParser->PrintStatus(s_server.GetState());
}

void OnCmdSend(CCommandParser* pParser)
{
	if(s_server.Send(pParser->m_dwConnID, (LPBYTE)(LPCTSTR)pParser->m_strMessage, pParser->m_strMessage.GetLength()))
		::LogSend(pParser->m_dwConnID, pParser->m_strMessage);
	else
		::LogSendFail(pParser->m_dwConnID, ::GetLastError(), ::GetLastErrorStr());
}

void OnCmdPause(CCommandParser* pParser)
{
	if(s_server.PauseReceive(pParser->m_dwConnID, pParser->m_bFlag))
		::LogPause(pParser->m_dwConnID, pParser->m_bFlag);
	else
		::LogPauseFail(pParser->m_dwConnID, pParser->m_bFlag);
}

void OnCmdKick(CCommandParser* pParser)
{
	if(s_server.Disconnect(pParser->m_dwConnID, pParser->m_bFlag))
		::LogDisconnect2(pParser->m_dwConnID, pParser->m_bFlag);
	else
		::LogDisconnectFail2(pParser->m_dwConnID, pParser->m_bFlag);
}

void OnCmdKickLong(CCommandParser* pParser)
{
	if(s_server.DisconnectLongConnections(pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnect2(pParser->m_dwSeconds, pParser->m_bFlag);
	else
		::LogDisconnectFail2(pParser->m_dwSeconds, pParser->m_bFlag);
}

void OnCmdKickSilence(CCommandParser* pParser)
{
	if(s_server.DisconnectSilenceConnections(pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnect2(pParser->m_dwSeconds, pParser->m_bFlag);
	else
		::LogDisconnectFail2(pParser->m_dwSeconds, pParser->m_bFlag);
}

void OnCmdStat(CCommandParser* pParser)
{
	::PostServerTemporaryStatics((LONGLONG)s_stat.m_llTotalSent, (LONGLONG)s_stat.m_llTotalReceived);
}

int main(int argc, char* const argv[])
{
	CTermAttrInitializer term_attr;
	CAppSignalHandler s_signal_handler({SIGTTOU, SIGINT});

	g_app_arg.ParseArgs(argc, argv);

	CCommandParser::CMD_FUNC fnCmds[CCommandParser::CT_MAX] = {0};

	fnCmds[CCommandParser::CT_START]	= OnCmdStart;
	fnCmds[CCommandParser::CT_STOP]		= OnCmdStop;
	fnCmds[CCommandParser::CT_STATUS]	= OnCmdStatus;
	fnCmds[CCommandParser::CT_SEND]		= OnCmdSend;
	fnCmds[CCommandParser::CT_PAUSE]	= OnCmdPause;
	fnCmds[CCommandParser::CT_KICK]		= OnCmdKick;
	fnCmds[CCommandParser::CT_KICK_L]	= OnCmdKickLong;
	fnCmds[CCommandParser::CT_KICK_S]	= OnCmdKickSilence;
	fnCmds[CCommandParser::CT_STAT]		= OnCmdStat;

	CCommandParser s_cmd_parser(CCommandParser::AT_SERVER, fnCmds);
	s_cmd_parser.Run();

	return EXIT_CODE_OK;
}
