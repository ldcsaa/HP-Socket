#include "../../global/helper.h"

#include <hpsocket/HPSocket4C-SSL.h>

En_HP_HandleResult __HP_CALL OnPrepareConnect(HP_Agent pSender, CONNID dwConnID, UINT_PTR socket)
{
	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnConnect(HP_Agent pSender, CONNID dwConnID)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	::HP_Agent_GetRemoteAddress(pSender, dwConnID, szAddress, &iAddressLen, &usPort);

	::PostOnConnect2(dwConnID, szAddress, usPort);

	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnHandShake(HP_Agent pSender, CONNID dwConnID)
{
	::PostOnHandShake(dwConnID);
	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnReceive(HP_Agent pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnReceive(dwConnID, pData, iLength);
	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnSend(HP_Agent pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength);
	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnClose(HP_Agent pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID) :
	::PostOnError(dwConnID, enOperation, iErrorCode);

	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnShutdown(HP_Agent pSender)
{
	::PostOnShutdown();
	return HR_OK;
}

HP_TcpPackAgentListener s_listener;
HP_SSLPackAgent s_agent;

void OnCmdStart(CCommandParser* pParser)
{
	if(::HP_Agent_Start(s_agent, g_app_arg.bind_addr, g_app_arg.async))
		::LogAgentStart(g_app_arg.bind_addr, g_app_arg.async);
	else
		::LogAgentStartFail(::HP_Agent_GetLastError(s_agent), ::HP_Agent_GetLastErrorDesc(s_agent));
}

void OnCmdStop(CCommandParser* pParser)
{
	if(::HP_Agent_Stop(s_agent))
		::LogAgentStop();
	else
		::LogAgentStopFail(::HP_Agent_GetLastError(s_agent), ::HP_Agent_GetLastErrorDesc(s_agent));
}

void OnCmdStatus(CCommandParser* pParser)
{
	pParser->PrintStatus(::HP_Agent_GetState(s_agent));
}

void OnCmdConnect(CCommandParser* pParser)
{
	::LogConnect(pParser->m_strRemoteAddr, pParser->m_usRemotePort);

	if(!::HP_Agent_Connect(s_agent, pParser->m_strRemoteAddr, pParser->m_usRemotePort, nullptr))
		::LogConnectFail(::SYS_GetLastError(), ::SYS_GetLastErrorStr());
}

void OnCmdSend(CCommandParser* pParser)
{
	if(::HP_Agent_Send(s_agent, pParser->m_dwConnID, (LPBYTE)(LPCTSTR)pParser->m_strMessage, pParser->m_strMessage.GetLength()))
		::LogSend(pParser->m_dwConnID, pParser->m_strMessage);
	else
		::LogSendFail(pParser->m_dwConnID, ::SYS_GetLastError(), ::SYS_GetLastErrorStr());
}

void OnCmdPause(CCommandParser* pParser)
{
	if(::HP_Agent_PauseReceive(s_agent, pParser->m_dwConnID, pParser->m_bFlag))
		::LogPause(pParser->m_dwConnID, pParser->m_bFlag);
	else
		::LogPauseFail(pParser->m_dwConnID, pParser->m_bFlag);
}

void OnCmdKick(CCommandParser* pParser)
{
	if(::HP_Agent_Disconnect(s_agent, pParser->m_dwConnID, pParser->m_bFlag))
		::LogDisconnect2(pParser->m_dwConnID, pParser->m_bFlag);
	else
		::LogDisconnectFail2(pParser->m_dwConnID, pParser->m_bFlag);
}

void OnCmdKickLong(CCommandParser* pParser)
{
	if(::HP_Agent_DisconnectLongConnections(s_agent, pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag);
}

void OnCmdKickSilence(CCommandParser* pParser)
{
	if(::HP_Agent_DisconnectSilenceConnections(s_agent, pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag);
}

void DestroyHPSocketObjects()
{
	// 清理 SSL 全局运行环境
	::HP_SSLAgent_CleanupSSLContext(s_agent);

	// 销毁 Socket 对象
	::Destroy_HP_SSLPackAgent(s_agent);
	// 销毁监听器对象
	::Destroy_HP_TcpPackAgentListener(s_listener);
}

void CreateHPSocketObjects()
{
	// 创建监听器对象
	s_listener = ::Create_HP_TcpPackAgentListener();
	// 创建 Socket 对象
	s_agent = ::Create_HP_SSLPackAgent(s_listener);

	// 初始化 SSL 全局环境参数
	if(!::HP_SSLAgent_SetupSSLContext(s_agent, g_c_iVerifyMode, g_c_lpszPemCertFile, g_c_lpszPemKeyFile, g_c_lpszKeyPasswod, g_c_lpszCAPemCertFileOrPath))
	{
		int iErrCode = ::SYS_GetLastError();
		::LogClientStartFail(iErrCode, _T("initialize SSL env fail"));

		DestroyHPSocketObjects();

		EXIT(EX_CONFIG, iErrCode);
	}

	// 设置 Socket 监听器回调函数
	::HP_Set_FN_Agent_OnPrepareConnect(s_listener, OnPrepareConnect);
	::HP_Set_FN_Agent_OnConnect(s_listener, OnConnect);
	::HP_Set_FN_Agent_OnHandShake(s_listener, OnHandShake);
	::HP_Set_FN_Agent_OnSend(s_listener, OnSend);
	::HP_Set_FN_Agent_OnReceive(s_listener, OnReceive);
	::HP_Set_FN_Agent_OnClose(s_listener, OnClose);
	::HP_Set_FN_Agent_OnShutdown(s_listener, OnShutdown);
}

int main(int argc, char* const argv[])
{
	CTermAttrInitializer term_attr;
	CAppSignalHandler s_signal_handler({SIGTTOU, SIGINT});

	g_app_arg.ParseArgs(argc, argv);

	CreateHPSocketObjects();

	::HP_TcpPackAgent_SetMaxPackSize(s_agent, 0x7FF);
	::HP_TcpPackAgent_SetPackHeaderFlag(s_agent, 0x169);
	::HP_TcpAgent_SetKeepAliveTime(s_agent, g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);

	CCommandParser::CMD_FUNC fnCmds[CCommandParser::CT_MAX] = {0};

	fnCmds[CCommandParser::CT_START]	= OnCmdStart;
	fnCmds[CCommandParser::CT_STOP]		= OnCmdStop;
	fnCmds[CCommandParser::CT_STATUS]	= OnCmdStatus;
	fnCmds[CCommandParser::CT_CONNECT]	= OnCmdConnect;
	fnCmds[CCommandParser::CT_SEND]		= OnCmdSend;
	fnCmds[CCommandParser::CT_PAUSE]	= OnCmdPause;
	fnCmds[CCommandParser::CT_KICK]		= OnCmdKick;
	fnCmds[CCommandParser::CT_KICK_L]	= OnCmdKickLong;
	fnCmds[CCommandParser::CT_KICK_S]	= OnCmdKickSilence;

	CCommandParser s_cmd_parser(CCommandParser::AT_AGENT, fnCmds);
	s_cmd_parser.Run();

	DestroyHPSocketObjects();

	return EXIT_CODE_OK;
}
