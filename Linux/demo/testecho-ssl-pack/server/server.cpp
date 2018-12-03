#include "../../global/helper.h"

#include <hpsocket/HPSocket4C-SSL.h>

En_HP_HandleResult __HP_CALL OnPrepareListen(HP_Server pSender, UINT_PTR soListen)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	::HP_Server_GetListenAddress(pSender, szAddress, &iAddressLen, &usPort);
	::PostOnPrepareListen(szAddress, usPort);

	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnAccept(HP_Server pSender, CONNID dwConnID, UINT_PTR soClient)
{
	BOOL bPass = TRUE;
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	::HP_Server_GetRemoteAddress(pSender, dwConnID, szAddress, &iAddressLen, &usPort);

	if(!g_app_arg.reject_addr.IsEmpty())
	{
		if(g_app_arg.reject_addr.CompareNoCase(szAddress) == 0)
			bPass = FALSE;
	}

	::PostOnAccept(dwConnID, szAddress, usPort, bPass);

	return bPass ? HR_OK : HR_ERROR;
}

En_HP_HandleResult __HP_CALL OnHandShake(HP_Server pSender, CONNID dwConnID)
{
	::PostOnHandShake(dwConnID);
	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnReceive(HP_Server pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnReceive(dwConnID, pData, iLength);

	if(::HP_Server_Send(pSender, dwConnID, pData, iLength))
		return HR_OK;

	return HR_ERROR;
}

En_HP_HandleResult __HP_CALL OnSend(HP_Server pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength);
	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnClose(HP_Server pSender, CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID) :
	::PostOnError(dwConnID, enOperation, iErrorCode);

	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnShutdown(HP_Server pSender)
{
	::PostOnShutdown();
	return HR_OK;
}

HP_TcpPackServerListener s_listener;
HP_SSLPackServer s_server;

void OnCmdStart(CCommandParser* pParser)
{
	if(::HP_Server_Start(s_server, g_app_arg.bind_addr, g_app_arg.port))
		::LogServerStart(g_app_arg.bind_addr, g_app_arg.port);
	else
		::LogServerStartFail(::HP_Server_GetLastError(s_server), ::HP_Server_GetLastErrorDesc(s_server));
}

void OnCmdStop(CCommandParser* pParser)
{
	if(::HP_Server_Stop(s_server))
		::LogServerStop();
	else
		::LogServerStopFail(::HP_Server_GetLastError(s_server), ::HP_Server_GetLastErrorDesc(s_server));
}

void OnCmdStatus(CCommandParser* pParser)
{
	pParser->PrintStatus(::HP_Server_GetState(s_server));
}

void OnCmdSend(CCommandParser* pParser)
{
	if(::HP_Server_Send(s_server, pParser->m_dwConnID, (LPBYTE)(LPCTSTR)pParser->m_strMessage, pParser->m_strMessage.GetLength()))
		::LogSend(pParser->m_dwConnID, pParser->m_strMessage);
	else
		::LogSendFail(pParser->m_dwConnID, ::SYS_GetLastError(), ::SYS_GetLastErrorStr());
}

void OnCmdPause(CCommandParser* pParser)
{
	if(::HP_Server_PauseReceive(s_server, pParser->m_dwConnID, pParser->m_bFlag))
		::LogPause(pParser->m_dwConnID, pParser->m_bFlag);
	else
		::LogPauseFail(pParser->m_dwConnID, pParser->m_bFlag);
}

void OnCmdKick(CCommandParser* pParser)
{
	if(::HP_Server_Disconnect(s_server, pParser->m_dwConnID, pParser->m_bFlag))
		::LogDisconnect2(pParser->m_dwConnID, pParser->m_bFlag);
	else
		::LogDisconnectFail2(pParser->m_dwConnID, pParser->m_bFlag);
}

void OnCmdKickLong(CCommandParser* pParser)
{
	if(::HP_Server_DisconnectLongConnections(s_server, pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag);
}

void OnCmdKickSilence(CCommandParser* pParser)
{
	if(::HP_Server_DisconnectSilenceConnections(s_server, pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag);
}

void DestroyHPSocketObjects()
{
	// 清理 SSL 全局运行环境
	::HP_SSLServer_CleanupSSLContext(s_server);

	// 销毁 Socket 对象
	::Destroy_HP_SSLPackServer(s_server);
	// 销毁监听器对象
	::Destroy_HP_TcpServerListener(s_listener);
}

void CreateHPSocketObjects()
{
	// 创建监听器对象
	s_listener = ::Create_HP_TcpPackServerListener();
	// 创建 Socket 对象
	s_server = ::Create_HP_SSLPackServer(s_listener);

	// 初始化 SSL 全局环境参数
	if(!::HP_SSLServer_SetupSSLContext(s_server, g_s_iVerifyMode, g_s_lpszPemCertFile, g_s_lpszPemKeyFile, g_s_lpszKeyPasswod, g_s_lpszCAPemCertFileOrPath, nullptr))
	{
		int iErrCode = ::SYS_GetLastError();
		::LogServerStartFail(::SYS_GetLastError(), _T("initialize SSL env fail"));

		DestroyHPSocketObjects();

		EXIT(EX_CONFIG, iErrCode);
	}

	// 设置 Socket 监听器回调函数
	::HP_Set_FN_Server_OnPrepareListen(s_listener, OnPrepareListen);
	::HP_Set_FN_Server_OnAccept(s_listener, OnAccept);
	::HP_Set_FN_Server_OnHandShake(s_listener, OnHandShake);
	::HP_Set_FN_Server_OnSend(s_listener, OnSend);
	::HP_Set_FN_Server_OnReceive(s_listener, OnReceive);
	::HP_Set_FN_Server_OnClose(s_listener, OnClose);
	::HP_Set_FN_Server_OnShutdown(s_listener, OnShutdown);
}

int main(int argc, char* const argv[])
{
	CTermAttrInitializer term_attr;
	CAppSignalHandler s_signal_handler({SIGTTOU, SIGINT});

	g_app_arg.ParseArgs(argc, argv);

	CreateHPSocketObjects();

	::HP_TcpPackServer_SetMaxPackSize(s_server, 0x7FF);
	::HP_TcpPackServer_SetPackHeaderFlag(s_server, 0x169);
	::HP_TcpServer_SetKeepAliveTime(s_server, g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);

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

	DestroyHPSocketObjects();

	return EXIT_CODE_OK;
}
