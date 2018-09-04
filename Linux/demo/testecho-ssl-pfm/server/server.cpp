#include "../../global/helper.h"

#include <hpsocket/HPSocket-SSL.h>

server_statistics_info s_stat;

class CListenerImpl : public CTcpServerListener
{

public:
	virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen) override
	{
		TCHAR szAddress[50];
		int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
		USHORT usPort;

		pSender->GetListenAddress(szAddress, iAddressLen, usPort);
		::PostOnPrepareListen(szAddress, usPort);

		return HR_OK;
	}

	virtual EnHandleResult OnAccept(ITcpServer* pSender, CONNID dwConnID, UINT_PTR soClient) override
	{
		::PostOnAccept2(dwConnID);
		return HR_OK;
	}

	virtual EnHandleResult OnHandShake(ITcpServer* pSender, CONNID dwConnID) override
	{
		s_stat.CheckClientCount();
		::PostOnHandShake(dwConnID);
		return HR_OK;
	}

	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
#ifdef _DEBUG2
		::PostOnReceive(dwConnID, pData, iLength);
#endif

		s_stat.AddTotalRecv(iLength);

		if(pSender->Send(dwConnID, pData, iLength))
			return HR_OK;
		
		return HR_ERROR;
	}

	virtual EnHandleResult OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
#ifdef _DEBUG2
		::PostOnSend(dwConnID, pData, iLength);
#endif
		s_stat.AddTotalSend(iLength);

		return HR_OK;
	}

	virtual EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) override
	{
		iErrorCode == SE_OK ? ::PostOnClose(dwConnID)	:
		::PostOnError(dwConnID, enOperation, iErrorCode);

		s_stat.CheckStatistics();

		return HR_OK;
	}

	virtual EnHandleResult OnShutdown(ITcpServer* pSender) override
	{
		::PostOnShutdown();
		return HR_OK;
	}

};

CListenerImpl s_listener;
CSSLServerPtr s_pserver(&s_listener);

void OnCmdStart(CCommandParser* pParser)
{
	if(s_pserver->HasStarted())
	{
		::LogServerStartFail(SE_ILLEGAL_STATE, ::HP_GetSocketErrorDesc(SE_ILLEGAL_STATE));
		return;
	}

	s_stat.Reset();

	if(s_pserver->Start(g_app_arg.bind_addr, g_app_arg.port))
		::LogServerStart(g_app_arg.bind_addr, g_app_arg.port);
	else
		::LogServerStartFail(s_pserver->GetLastError(), s_pserver->GetLastErrorDesc());
}

void OnCmdStop(CCommandParser* pParser)
{
	if(!s_pserver->HasStarted())
	{
		::LogServerStopFail(SE_ILLEGAL_STATE, ::HP_GetSocketErrorDesc(SE_ILLEGAL_STATE));
		return;
	}

	if(s_pserver->Stop())
		::LogServerStop();
	else
		::LogServerStopFail(s_pserver->GetLastError(), s_pserver->GetLastErrorDesc());
}

void OnCmdStatus(CCommandParser* pParser)
{
	pParser->PrintStatus(s_pserver->GetState());
}

void OnCmdSend(CCommandParser* pParser)
{
	if(s_pserver->Send(pParser->m_dwConnID, (LPBYTE)(LPCTSTR)pParser->m_strMessage, pParser->m_strMessage.GetLength()))
		::LogSend(pParser->m_dwConnID, pParser->m_strMessage);
	else
		::LogSendFail(pParser->m_dwConnID, ::SYS_GetLastError(), ::SYS_GetLastErrorStr());
}

void OnCmdPause(CCommandParser* pParser)
{
	if(s_pserver->PauseReceive(pParser->m_dwConnID, pParser->m_bFlag))
		::LogPause(pParser->m_dwConnID, pParser->m_bFlag);
	else
		::LogPauseFail(pParser->m_dwConnID, pParser->m_bFlag);
}

void OnCmdKick(CCommandParser* pParser)
{
	if(s_pserver->Disconnect(pParser->m_dwConnID, pParser->m_bFlag))
		::LogDisconnect2(pParser->m_dwConnID, pParser->m_bFlag);
	else
		::LogDisconnectFail2(pParser->m_dwConnID, pParser->m_bFlag);
}

void OnCmdKickLong(CCommandParser* pParser)
{
	if(s_pserver->DisconnectLongConnections(pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag);
}

void OnCmdKickSilence(CCommandParser* pParser)
{
	if(s_pserver->DisconnectSilenceConnections(pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag);
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

	if(!s_pserver->SetupSSLContext(g_s_iVerifyMode, g_s_lpszPemCertFile, g_s_lpszPemKeyFile, g_s_lpszKeyPasswod, g_s_lpszCAPemCertFileOrPath))
	{
		int iErrCode = ::SYS_GetLastError();
		::LogServerStartFail(iErrCode, _T("initialize SSL env fail"));

		EXIT(EX_CONFIG, iErrCode);
	}

	s_pserver->SetWorkerThreadCount(g_app_arg.thread_count);
	s_pserver->SetMaxConnectionCount(g_app_arg.max_conn);
	s_pserver->SetSendPolicy(g_app_arg.send_policy);
	s_pserver->SetKeepAliveTime(g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);

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
