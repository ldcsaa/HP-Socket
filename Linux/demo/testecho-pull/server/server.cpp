#include "../../global/helper.h"
#include "../../../src/TcpPullServer.h"

class CListenerImpl : public CTcpPullServerListener
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

		if(bPass) ::CreatePkgInfo(pSender, dwConnID);

		return bPass ? HR_OK : HR_ERROR;
	}

	virtual EnHandleResult OnHandShake(ITcpServer* pSender, CONNID dwConnID) override
	{
		return HR_OK;
	}

	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength) override
	{
		TPkgInfo* pInfo			= ::FindPkgInfo(pSender, dwConnID);
		ITcpPullServer* pServer	= ITcpPullServer::FromS(pSender);

		if(pInfo != nullptr)
		{
			int required = pInfo->length;
			int remain = iLength;

			while(remain >= required)
			{
				remain -= required;
				CBufferPtr buffer(required);

				EnFetchResult result = pServer->Fetch(dwConnID, buffer, (int)buffer.Size());

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
						TPkgBody* pBody = (TPkgBody*)(BYTE*)buffer;
						PRINTLN("(body) -> name: %s, age: %d, desc: %s", pBody->name, pBody->age, pBody->desc);
#endif
						required = sizeof(TPkgHeader);
					}

					pInfo->is_header = !pInfo->is_header;
					pInfo->length	 = required;

					if(!pSender->Send(dwConnID, buffer, (int)buffer.Size()))
						return HR_ERROR;
				}
			}
		}

		return HR_OK;
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

		::RemovePkgInfo(pSender, dwConnID);

		return HR_OK;
	}

	virtual EnHandleResult OnShutdown(ITcpServer* pSender) override
	{
		::PostOnShutdown();
		return HR_OK;
	}

};

CListenerImpl s_listener;
CTcpPullServer s_server(&s_listener);

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
	static DWORD SEQ = 0;

	unique_ptr<CBufferPtr> buffer(::GeneratePkgBuffer(++SEQ, _T("HP-Server"), 32, pParser->m_strMessage));

	if(s_server.Send(pParser->m_dwConnID, buffer->Ptr(), (int)buffer->Size()))
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

	return EXIT_CODE_OK;
}
