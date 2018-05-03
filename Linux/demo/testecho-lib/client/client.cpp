#include "../../global/helper.h"

#include <hpsocket/HPSocket4C.h>

En_HP_HandleResult __HP_CALL OnConnect(HP_Client pSender, HP_CONNID dwConnID)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	::HP_Client_GetRemoteHost(pSender, szAddress, &iAddressLen, &usPort);

	::PostOnConnect2(dwConnID, szAddress, usPort);

	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnReceive(HP_Client pSender, HP_CONNID dwConnID, int iLength)
{
	TPkgInfo* pInfo = (TPkgInfo*)::HP_Client_GetExtra(pSender);

	int required = pInfo->length;
	int remain	 = iLength;

	while(remain >= required)
	{
		remain -= required;
		CBufferPtr buffer(required);

		En_HP_FetchResult result = HP_TcpPullClient_Fetch(pSender, buffer, (int)buffer.Size());
		if(result == FR_OK)
		{
			::PostOnReceive(dwConnID, buffer, (int)buffer.Size());

			if(pInfo->is_header)
			{
				TPkgHeader* pHeader = (TPkgHeader*)buffer.Ptr();
#ifdef DEBUG
				PRINTLN("(head) -> seq: %d, body_len: %d", pHeader->seq, pHeader->body_len);
#endif
				required = pHeader->body_len;
			}
			else
			{
#ifdef DEBUG
				TPkgBody* pBody = (TPkgBody*)buffer.Ptr();
				PRINTLN("(body) -> name: %s, age: %d, desc: %s", pBody->name, pBody->age, pBody->desc);
#endif
				required = sizeof(TPkgHeader);
			}

			pInfo->is_header	= !pInfo->is_header;
			pInfo->length	= required;
		}
	}

	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnSend(HP_Client pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength);
	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnClose(HP_Client pSender, HP_CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID) :
	::PostOnError(dwConnID, enOperation, iErrorCode);

	return HR_OK;
}

HP_TcpPullClientListener s_listener;
HP_TcpPullClient s_client;
TPkgInfo s_pkgInfo;

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
	static DWORD SEQ = 0;

	unique_ptr<CBufferPtr> buffer(::GeneratePkgBuffer(++SEQ, _T("HP-Client"), 23, pParser->m_strMessage));

	if(::HP_Client_Send(s_client, buffer->Ptr(), (int)buffer->Size()))
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

void CreateHPSocketObjects()
{
	// 创建监听器对象
	s_listener	= ::Create_HP_TcpPullClientListener();
	// 创建 Socket 对象
	s_client	= ::Create_HP_TcpPullClient(s_listener);

	// 设置 Socket 监听器回调函数
	::HP_Set_FN_Client_OnConnect(s_listener, OnConnect);
	::HP_Set_FN_Client_OnSend(s_listener, OnSend);
	::HP_Set_FN_Client_OnPullReceive(s_listener, OnReceive);
	::HP_Set_FN_Client_OnClose(s_listener, OnClose);

	::HP_Client_SetExtra(s_client, &s_pkgInfo);
	::HP_TcpClient_SetKeepAliveTime(s_client, g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);
}

void DestroyHPSocketObjects()
{
	// 销毁 Socket 对象
	::Destroy_HP_TcpPullClient(s_client);
	// 销毁监听器对象
	::Destroy_HP_TcpPullClientListener(s_listener);
}

int main(int argc, char* const argv[])
{
	CTermAttrInitializer term_attr;
	CAppSignalHandler s_signal_handler({SIGTTOU, SIGINT});

	g_app_arg.ParseArgs(argc, argv);

	CreateHPSocketObjects();

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
