#include "../../global/helper.h"

#include <hpsocket/HPSocket-SSL.h>

class CListenerImpl : public CTcpPullAgentListener
{

public:
	virtual EnHandleResult OnPrepareConnect(ITcpAgent* pSender, CONNID dwConnID, SOCKET socket) override
	{
		return HR_OK;
	}

	virtual EnHandleResult OnConnect(ITcpAgent* pSender, CONNID dwConnID) override
	{
		TCHAR szAddress[50];
		int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
		USHORT usPort;

		pSender->GetRemoteAddress(dwConnID, szAddress, iAddressLen, usPort);

		::PostOnConnect2(dwConnID, szAddress, usPort);

		return HR_OK;
	}

	virtual EnHandleResult OnHandShake(ITcpAgent* pSender, CONNID dwConnID) override
	{
		::PostOnHandShake(dwConnID);
		return HR_OK;
	}

	virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, int iLength) override
	{
		ITcpPullAgent* pAgent = ITcpPullAgent::FromS(pSender);
		TPkgInfo* pInfo		  = ::FindPkgInfo(pSender, dwConnID);

		int required = pInfo->length;
		int remain = iLength;

		while(remain >= required)
		{
			remain -= required;
			CBufferPtr buffer(required);

			EnFetchResult result = pAgent->Fetch(dwConnID, buffer, (int)buffer.Size());

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

	virtual EnHandleResult OnSend(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
		::PostOnSend(dwConnID, pData, iLength);
		return HR_OK;
	}

	virtual EnHandleResult OnClose(ITcpAgent* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) override
	{
		iErrorCode == SE_OK ? ::PostOnClose(dwConnID) :
		::PostOnError(dwConnID, enOperation, iErrorCode);

		::RemovePkgInfo(pSender, dwConnID);

		return HR_OK;
	}

	virtual EnHandleResult OnShutdown(ITcpAgent* pSender) override
	{
		::PostOnShutdown();
		return HR_OK;
	}

};

CListenerImpl s_listener;
CSSLPullAgentPtr s_pagent(&s_listener);

void OnCmdStart(CCommandParser* pParser)
{
	if(s_pagent->Start(g_app_arg.bind_addr, g_app_arg.async))
		::LogAgentStart(g_app_arg.bind_addr, g_app_arg.async);
	else
		::LogAgentStartFail(s_pagent->GetLastError(), s_pagent->GetLastErrorDesc());
}

void OnCmdStop(CCommandParser* pParser)
{
	if(s_pagent->Stop())
		::LogAgentStop();
	else
		::LogAgentStopFail(s_pagent->GetLastError(), s_pagent->GetLastErrorDesc());
}

void OnCmdStatus(CCommandParser* pParser)
{
	pParser->PrintStatus(s_pagent->GetState());
}

void OnCmdConnect(CCommandParser* pParser)
{
	TPkgInfo* pInfo = ::ConstructPkgInfo();

	::LogConnect(pParser->m_strRemoteAddr, pParser->m_usRemotePort);

	if(!s_pagent->Connect(pParser->m_strRemoteAddr, pParser->m_usRemotePort, nullptr, pInfo))
	{
		::DestructPkgInfo(pInfo);
		::LogConnectFail(::GetLastError(), ::GetLastErrorStr());
	}
}

void OnCmdSend(CCommandParser* pParser)
{
	static DWORD SEQ = 0;

	unique_ptr<CBufferPtr> buffer(::GeneratePkgBuffer(++SEQ, _T("HP-Agent"), 23, pParser->m_strMessage));

	if(s_pagent->Send(pParser->m_dwConnID, buffer->Ptr(), (int)buffer->Size()))
		::LogSend(pParser->m_dwConnID, pParser->m_strMessage);
	else
		::LogSendFail(pParser->m_dwConnID, ::GetLastError(), ::GetLastErrorStr());
}

void OnCmdPause(CCommandParser* pParser)
{
	if(s_pagent->PauseReceive(pParser->m_dwConnID, pParser->m_bFlag))
		::LogPause(pParser->m_dwConnID, pParser->m_bFlag);
	else
		::LogPauseFail(pParser->m_dwConnID, pParser->m_bFlag);
}

void OnCmdKick(CCommandParser* pParser)
{
	if(s_pagent->Disconnect(pParser->m_dwConnID, pParser->m_bFlag))
		::LogDisconnect2(pParser->m_dwConnID, pParser->m_bFlag);
	else
		::LogDisconnectFail2(pParser->m_dwConnID, pParser->m_bFlag);
}

void OnCmdKickLong(CCommandParser* pParser)
{
	if(s_pagent->DisconnectLongConnections(pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag);
}

void OnCmdKickSilence(CCommandParser* pParser)
{
	if(s_pagent->DisconnectSilenceConnections(pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag);
}

int main(int argc, char* const argv[])
{
	CTermAttrInitializer term_attr;
	CAppSignalHandler s_signal_handler({SIGTTOU, SIGINT});

	g_app_arg.ParseArgs(argc, argv);

	if(!s_pagent->SetupSSLContext(g_c_iVerifyMode, g_c_lpszPemCertFile, g_c_lpszPemKeyFile, g_c_lpszKeyPasswod, g_c_lpszCAPemCertFileOrPath))
	{
		int iErrCode = ::SYS_GetLastError();
		::LogClientStartFail(iErrCode, _T("initialize SSL env fail"));

		EXIT(EX_CONFIG, iErrCode);
	}

	s_pagent->SetKeepAliveTime(g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);

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

	s_pagent->CleanupSSLContext();

	return EXIT_CODE_OK;
}
