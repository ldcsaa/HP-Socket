#include "../../global/helper.h"
#include "../../../src/TcpPullClient.h"

class CListenerImpl : public CTcpPullClientListener
{

public:
	virtual EnHandleResult OnPrepareConnect(ITcpClient* pSender, CONNID dwConnID, SOCKET socket) override
	{
		return HR_OK;
	}

	virtual EnHandleResult OnConnect(ITcpClient* pSender, CONNID dwConnID) override
	{
		TCHAR szAddress[50];
		int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
		USHORT usPort;

		pSender->GetRemoteHost(szAddress, iAddressLen, usPort);

		::PostOnConnect2(dwConnID, szAddress, usPort);

		return HR_OK;
	}

	virtual EnHandleResult OnHandShake(ITcpClient* pSender, CONNID dwConnID) override
	{
		return HR_OK;
	}

	virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, int iLength) override
	{
		ITcpPullClient* pClient	= ITcpPullClient::FromS(pSender);
		TPkgInfo* pInfo			= (TPkgInfo*)pSender->GetExtra();

		int required = pInfo->length;
		int remain	 = iLength;

		while(remain >= required)
		{
			remain -= required;
			CBufferPtr buffer(required);

			EnFetchResult result = pClient->Fetch(buffer, (int)buffer.Size());
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

	virtual EnHandleResult OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
		::PostOnSend(dwConnID, pData, iLength);
		return HR_OK;
	}

	virtual EnHandleResult OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) override
	{
		iErrorCode == SE_OK ? ::PostOnClose(dwConnID) :
		::PostOnError(dwConnID, enOperation, iErrorCode);

		return HR_OK;
	}

};

CListenerImpl s_listener;
CTcpPullClient s_client(&s_listener);
TPkgInfo s_pkgInfo;

void OnCmdStart(CCommandParser* pParser)
{
	if(s_client.HasStarted())
	{
		::LogClientStartFail(SE_ILLEGAL_STATE, ::GetSocketErrorDesc(SE_ILLEGAL_STATE));
		return;
	}

	if(s_client.GetExtra() == nullptr)
		s_client.SetExtra(&s_pkgInfo);

	s_client.SetKeepAliveTime(g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);

	if(s_client.Start(g_app_arg.remote_addr, g_app_arg.port, g_app_arg.async, g_app_arg.bind_addr))
		::LogClientStart(g_app_arg.remote_addr, g_app_arg.port);
	else
		::LogClientStartFail(s_client.GetLastError(), s_client.GetLastErrorDesc());
}

void OnCmdStop(CCommandParser* pParser)
{
	if(s_client.Stop())
		::LogClientStop();
	else
		::LogClientStopFail(s_client.GetLastError(), s_client.GetLastErrorDesc());
}

void OnCmdStatus(CCommandParser* pParser)
{
	pParser->PrintStatus(s_client.GetState());
}

void OnCmdSend(CCommandParser* pParser)
{
	static DWORD SEQ = 0;

	unique_ptr<CBufferPtr> buffer(::GeneratePkgBuffer(++SEQ, _T("HP-Client"), 23, pParser->m_strMessage));

	if(s_client.Send(buffer->Ptr(), (int)buffer->Size()))
		::LogSend(s_client.GetConnectionID(), pParser->m_strMessage);
	else
		::LogSendFail(s_client.GetConnectionID(), ::GetLastError(), ::GetLastErrorStr());
}

void OnCmdPause(CCommandParser* pParser)
{
	if(s_client.PauseReceive(pParser->m_bFlag))
		::LogPause(s_client.GetConnectionID(), pParser->m_bFlag);
	else
		::LogPauseFail(s_client.GetConnectionID(), pParser->m_bFlag);
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

	CCommandParser s_cmd_parser(CCommandParser::AT_CLIENT, fnCmds);
	s_cmd_parser.Run();

	return EXIT_CODE_OK;
}
