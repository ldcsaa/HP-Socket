#include "../../global/helper.h"

#include <hpsocket/HPSocket4C-SSL.h>

En_HP_HandleResult __HP_CALL OnPrepareConnect(HP_Client pSender, CONNID dwConnID, UINT_PTR socket)
{
	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnConnect(HP_Client pSender, CONNID dwConnID)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	::HP_Client_GetRemoteHost(pSender, szAddress, &iAddressLen, &usPort);

	::PostOnConnect2(dwConnID, szAddress, usPort);

	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnHandShake(HP_Client pSender, CONNID dwConnID)
{
	::PostOnHandShake(dwConnID);
	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnReceive(HP_Client pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnReceive(dwConnID, pData, iLength);
	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnSend(HP_Client pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength);
	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnClose(HP_Client pSender, CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID) :
	::PostOnError(dwConnID, enOperation, iErrorCode);

	return HR_OK;
}

HP_TcpPackClientListener s_listener;
HP_SSLPackClient s_client;

void OnCmdStart(CCommandParser* pParser)
{
	if(::HP_Client_StartWithBindAddress(s_client, g_app_arg.remote_addr, g_app_arg.port, g_app_arg.async, g_app_arg.bind_addr))
		::LogClientStart(g_app_arg.remote_addr, g_app_arg.port);
	else
		::LogClientStartFail(::HP_Client_GetLastError(s_client), ::HP_Client_GetLastErrorDesc(s_client));
}

void OnCmdStop(CCommandParser* pParser)
{
	if(::HP_Client_Stop(s_client))
		::LogClientStop();
	else
		::LogClientStopFail(::HP_Client_GetLastError(s_client), ::HP_Client_GetLastErrorDesc(s_client));
}

void OnCmdStatus(CCommandParser* pParser)
{
	pParser->PrintStatus(::HP_Client_GetState(s_client));
}

void OnCmdSend(CCommandParser* pParser)
{
	if(::HP_Client_Send(s_client, (LPBYTE)(LPCTSTR)pParser->m_strMessage, pParser->m_strMessage.GetLength()))
		::LogSend(::HP_Client_GetConnectionID(s_client), pParser->m_strMessage);
	else
		::LogSendFail(::HP_Client_GetConnectionID(s_client), ::SYS_GetLastError(), ::SYS_GetLastErrorStr());
}

void OnCmdPause(CCommandParser* pParser)
{
	if(::HP_Client_PauseReceive(s_client, pParser->m_bFlag))
		::LogPause(::HP_Client_GetConnectionID(s_client), pParser->m_bFlag);
	else
		::LogPauseFail(::HP_Client_GetConnectionID(s_client), pParser->m_bFlag);
}

void DestroyHPSocketObjects()
{
	// 清理 SSL 全局运行环境
	::HP_SSLClient_CleanupSSLContext(s_client);

	// 销毁 Socket 对象
	::Destroy_HP_SSLPackClient(s_client);
	// 销毁监听器对象
	::Destroy_HP_TcpPackClientListener(s_listener);
}

void CreateHPSocketObjects()
{
	// 创建监听器对象
	s_listener = ::Create_HP_TcpPackClientListener();
	// 创建 Socket 对象
	s_client = ::Create_HP_SSLPackClient(s_listener);

	// 初始化 SSL 全局环境参数
	if(!::HP_SSLClient_SetupSSLContext(s_client, g_c_iVerifyMode, g_c_lpszPemCertFile, g_c_lpszPemKeyFile, g_c_lpszKeyPasswod, g_c_lpszCAPemCertFileOrPath))
	{
		int iErrCode = ::SYS_GetLastError();
		::LogClientStartFail(iErrCode, _T("initialize SSL env fail"));

		DestroyHPSocketObjects();

		EXIT(EX_CONFIG, iErrCode);
	}

	// 设置 Socket 监听器回调函数
	::HP_Set_FN_Client_OnPrepareConnect(s_listener, OnPrepareConnect);
	::HP_Set_FN_Client_OnConnect(s_listener, OnConnect);
	::HP_Set_FN_Client_OnHandShake(s_listener, OnHandShake);
	::HP_Set_FN_Client_OnSend(s_listener, OnSend);
	::HP_Set_FN_Client_OnReceive(s_listener, OnReceive);
	::HP_Set_FN_Client_OnClose(s_listener, OnClose);
}

int main(int argc, char* const argv[])
{
	CTermAttrInitializer term_attr;
	CAppSignalHandler s_signal_handler({SIGTTOU, SIGINT});

	g_app_arg.ParseArgs(argc, argv);

	CreateHPSocketObjects();

	::HP_TcpPackClient_SetMaxPackSize(s_client, 0x7FF);
	::HP_TcpPackClient_SetPackHeaderFlag(s_client, 0x169);
	::HP_TcpClient_SetKeepAliveTime(s_client, g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);

	CCommandParser::CMD_FUNC fnCmds[CCommandParser::CT_MAX] = {0};

	fnCmds[CCommandParser::CT_START]	= OnCmdStart;
	fnCmds[CCommandParser::CT_STOP]		= OnCmdStop;
	fnCmds[CCommandParser::CT_STATUS]	= OnCmdStatus;
	fnCmds[CCommandParser::CT_SEND]		= OnCmdSend;
	fnCmds[CCommandParser::CT_PAUSE]	= OnCmdPause;

	CCommandParser s_cmd_parser(CCommandParser::AT_CLIENT, fnCmds);
	s_cmd_parser.Run();

	DestroyHPSocketObjects();

	return EXIT_CODE_OK;
}
