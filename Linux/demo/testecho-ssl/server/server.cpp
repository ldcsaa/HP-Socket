#include "../../global/helper.h"
#include "../../../src/SSLServer.h"
#include "../../../src/common/FileHelper.h"

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
		BOOL bPass = TRUE;
		TCHAR szAddress[50];
		int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
		USHORT usPort;

		pSender->GetRemoteAddress(dwConnID, szAddress, iAddressLen, usPort);

		if(!g_app_arg.reject_addr.IsEmpty())
		{
			if(g_app_arg.reject_addr.CompareNoCase(szAddress) == 0)
				bPass = FALSE;
		}

		::PostOnAccept(dwConnID, szAddress, usPort, bPass);

		return bPass ? HR_OK : HR_ERROR;
	}

	virtual EnHandleResult OnHandShake(ITcpServer* pSender, CONNID dwConnID) override
	{
		::PostOnHandShake(dwConnID);
		return HR_OK;
	}

	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
		::PostOnReceive(dwConnID, pData, iLength);

		if(pSender->Send(dwConnID, pData, iLength))
			return HR_OK;
		
		return HR_ERROR;
	}

	virtual EnHandleResult OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
		::PostOnSend(dwConnID, pData, iLength);
		return HR_OK;
	}

	virtual EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) override
	{
		iErrorCode == SE_OK ? ::PostOnClose(dwConnID)	:
		::PostOnError(dwConnID, enOperation, iErrorCode);

		return HR_OK;
	}

	virtual EnHandleResult OnShutdown(ITcpServer* pSender) override
	{
		::PostOnShutdown();
		return HR_OK;
	}

};

CListenerImpl s_listener;
CSSLServer s_server(&s_listener);

void OnCmdStart(CCommandParser* pParser)
{
	if(s_server.Start(g_app_arg.bind_addr, g_app_arg.port))
		::LogServerStart(g_app_arg.bind_addr, g_app_arg.port);
	else
		::LogServerStartFail(s_server.GetLastError(), s_server.GetLastErrorDesc());
}

void OnCmdStop(CCommandParser* pParser)
{
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
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag);
}

void OnCmdKickSilence(CCommandParser* pParser)
{
	if(s_server.DisconnectSilenceConnections(pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag);
}

int main(int argc, char* const argv[])
{
	CTermAttrInitializer term_attr;
	CAppSignalHandler s_signal_handler({SIGTTOU, SIGINT});

	g_app_arg.ParseArgs(argc, argv);

	if(!s_server.SetupSSLContext(g_s_iVerifyMode, g_s_lpszPemCertFile, g_s_lpszPemKeyFile, g_s_lpszKeyPasswod, g_s_lpszCAPemCertFileOrPath))
	{
		::LogServerStartFail(::GetLastError(), _T("initialize SSL env fail"));
		return EX_CONFIG;
	}

	s_server.SetKeepAliveTime(g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);

	CCommandParser::CMD_FUNC fnCmds[CCommandParser::CT_MAX] = {0};

	fnCmds[CCommandParser::CT_START]	= OnCmdStart;
	fnCmds[CCommandParser::CT_STOP]		= OnCmdStop;
	fnCmds[CCommandParser::CT_STATUS]	= OnCmdStatus;
	fnCmds[CCommandParser::CT_SEND]		= OnCmdSend;
	fnCmds[CCommandParser::CT_PAUSE]	= OnCmdPause;
	fnCmds[CCommandParser::CT_KICK]		= OnCmdKick;
	fnCmds[CCommandParser::CT_KICK_L]	= OnCmdKickLong;
	fnCmds[CCommandParser::CT_KICK_S]	= OnCmdKickSilence;

	CCommandParser s_cmd_parser(CCommandParser::AT_SERVER, fnCmds);
	s_cmd_parser.Run();

	s_server.CleanupSSLContext();

	return EXIT_CODE_OK;
}
