#include "../../global/helper.h"

#include <hpsocket/HPSocket4C.h>

TPkgInfo* AgentFindPkgInfo(HP_Agent pSender, HP_CONNID dwConnID)
{
	PVOID pInfo = nullptr;
	::HP_Agent_GetConnectionExtra(pSender, dwConnID, &pInfo);

	return (TPkgInfo*)pInfo;
}

void AgentRemovePkgInfo(HP_Agent pSender, CONNID dwConnID)
{
	TPkgInfo* pInfo = AgentFindPkgInfo(pSender, dwConnID);
	ASSERT(pInfo != nullptr);

	DestructPkgInfo(pInfo);
}

En_HP_HandleResult __HP_CALL OnConnect(HP_Agent pSender, HP_CONNID dwConnID)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	::HP_Agent_GetRemoteHost(pSender, dwConnID, szAddress, &iAddressLen, &usPort);

	::PostOnConnect2(dwConnID, szAddress, usPort);

	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnReceive(HP_Agent pSender, HP_CONNID dwConnID, int iLength)
{
	TPkgInfo* pInfo = AgentFindPkgInfo(pSender, dwConnID);

	int required = pInfo->length;
	int remain = iLength;

	while(remain >= required)
	{
		remain -= required;
		CBufferPtr buffer(required);

		En_HP_FetchResult result = ::HP_TcpPullAgent_Fetch(pSender, dwConnID, buffer, (int)buffer.Size());

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

			pInfo->is_header = !pInfo->is_header;
			pInfo->length	 = required;
		}
	}

	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnSend(HP_Agent pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength);
	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnClose(HP_Agent pSender, HP_CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID) :
	::PostOnError(dwConnID, enOperation, iErrorCode);

	AgentRemovePkgInfo(pSender, dwConnID);

	return HR_OK;
}

En_HP_HandleResult __HP_CALL OnShutdown(HP_Agent pSender)
{
	::PostOnShutdown();
	return HR_OK;
}

HP_TcpPullAgentListener s_listener;
HP_TcpPullAgent s_agent;

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
	TPkgInfo* pInfo = ::ConstructPkgInfo();

	::LogConnect(pParser->m_strRemoteAddr, pParser->m_usRemotePort);

	if(!::HP_Agent_ConnectWithExtra(s_agent, pParser->m_strRemoteAddr, pParser->m_usRemotePort, nullptr, pInfo))
	{
		::DestructPkgInfo(pInfo);
		::LogConnectFail(::SYS_GetLastError(), ::SYS_GetLastErrorStr());
	}
}

void OnCmdSend(CCommandParser* pParser)
{
	static DWORD SEQ = 0;

	unique_ptr<CBufferPtr> buffer(::GeneratePkgBuffer(++SEQ, _T("HP-Agent"), 23, pParser->m_strMessage));

	if(::HP_Agent_Send(s_agent, pParser->m_dwConnID, buffer->Ptr(), (int)buffer->Size()))
		::LogSend(pParser->m_dwConnID, pParser->m_strMessage);
	else
		::LogSendFail(pParser->m_dwConnID, ::GetLastError(), ::GetLastErrorStr());
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

void CreateHPSocketObjects()
{
	// 创建监听器对象
	s_listener	= ::Create_HP_TcpPullAgentListener();
	// 创建 Socket 对象
	s_agent		= ::Create_HP_TcpPullAgent(s_listener);

	// 设置 Socket 监听器回调函数
	::HP_Set_FN_Agent_OnConnect(s_listener, OnConnect);
	::HP_Set_FN_Agent_OnSend(s_listener, OnSend);
	::HP_Set_FN_Agent_OnPullReceive(s_listener, OnReceive);
	::HP_Set_FN_Agent_OnClose(s_listener, OnClose);
	::HP_Set_FN_Agent_OnShutdown(s_listener, OnShutdown);

	::HP_TcpAgent_SetKeepAliveTime(s_agent, g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);
}

void DestroyHPSocketObjects()
{
	// 销毁 Socket 对象
	::Destroy_HP_TcpPullAgent(s_agent);
	// 销毁监听器对象
	::Destroy_HP_TcpPullAgentListener(s_listener);
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
