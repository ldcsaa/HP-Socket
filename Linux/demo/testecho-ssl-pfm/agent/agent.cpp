#include "../../global/helper.h"

#include <hpsocket/HPSocket-SSL.h>

client_statistics_info s_stat;

class CListenerImpl : public CTcpAgentListener
{

public:
	virtual EnHandleResult OnPrepareConnect(ITcpAgent* pSender, CONNID dwConnID, SOCKET socket) override
	{
		return HR_OK;
	}

	virtual EnHandleResult OnConnect(ITcpAgent* pSender, CONNID dwConnID) override
	{
#ifdef _DEBUG
		::PostOnConnect3(dwConnID);
#endif
		return HR_OK;
	}

	virtual EnHandleResult OnHandShake(ITcpAgent* pSender, CONNID dwConnID) override
	{
#ifdef _DEBUG
		::PostOnHandShake(dwConnID);
#endif
		s_stat.AddConnected();

		return HR_OK;
	}

	virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
#ifdef _DEBUG2
		::PostOnReceive(dwConnID, pData, iLength);
#endif
		s_stat.AddTotalRecv(iLength);

		return HR_OK;
	}

	virtual EnHandleResult OnSend(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
#ifdef _DEBUG2
		::PostOnSend(dwConnID, pData, iLength);
#endif
		s_stat.AddTotalSend(iLength);

		return HR_OK;
	}

	virtual EnHandleResult OnClose(ITcpAgent* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) override
	{
		if(iErrorCode == SE_OK)
		{
#ifdef _DEBUG
			::PostOnClose(dwConnID);
#endif
		}
		else
		{
			::PostOnError(dwConnID, enOperation, iErrorCode);
		}

		s_stat.TermConnected();

		return HR_OK;
	}

	virtual EnHandleResult OnShutdown(ITcpAgent* pSender) override
	{
		::PostOnShutdown();
		return HR_OK;
	}

};

CListenerImpl s_listener;
CSSLAgentPtr s_pagent(&s_listener);
CBufferPtr s_sendBuffer;

void OnCmdStart(CCommandParser* pParser)
{
	if(s_pagent->HasStarted())
	{
		::LogAgentStartFail(SE_ILLEGAL_STATE, ::HP_GetSocketErrorDesc(SE_ILLEGAL_STATE));
		return;
	}

	s_stat.Reset();

	BOOL isOK = FALSE;

	if(s_pagent->Start(g_app_arg.bind_addr, g_app_arg.async))
	{
		for(DWORD i = 0; i < g_app_arg.conn_count; i++)
		{
			if(s_pagent->Connect(g_app_arg.remote_addr, g_app_arg.port))
			{
				if(i == g_app_arg.conn_count - 1)
					isOK = TRUE;
			}
			else
			{
				::LogAgentStartFail(::SYS_GetLastError(), ::HP_GetSocketErrorDesc(SE_CONNECT_SERVER));
				break;
			}
		}
	}
	else
		::LogAgentStartFail(s_pagent->GetLastError(), s_pagent->GetLastErrorDesc());

	if(!isOK)
	{
		s_pagent->Stop();
		return;
	}

	::LogAgentStart(g_app_arg.remote_addr, g_app_arg.async);

	DWORD dwSendDelay = 3;
	CString strMsg;

	strMsg.Format(_T("*** willing to send data after %d seconds ***"), dwSendDelay);
	::LogMsg(strMsg);

	::WaitFor(dwSendDelay * 500);

	while(true)
	{
		int iConnected = s_stat.GetConnected();

		if(iConnected < 0)
		{
			::LogClientStartFail(ERROR_CONNREFUSED, ::HP_GetSocketErrorDesc(SE_CONNECT_SERVER));

			s_pagent->Stop();

			return;
		}
		else if(iConnected == (int)g_app_arg.conn_count)
			break;

		::WaitFor(3);
	}

	::WaitFor(dwSendDelay * 500);
	s_sendBuffer.Malloc(g_app_arg.data_length, true);

	DWORD count = g_app_arg.conn_count;
	CONNID connIDs[g_app_arg.conn_count];

	VERIFY(s_pagent->GetAllConnectionIDs(connIDs, count));

	::LogMsg(_T("*** Go Now ! ***"));

	s_stat.StartTest();

	BOOL bTerminated = FALSE;
	for(DWORD i = 0; i < g_app_arg.test_times; i++)
	{
		for(DWORD j = 0; j < g_app_arg.conn_count; j++)
		{
			if(!s_pagent->Send(connIDs[j], s_sendBuffer, (int)s_sendBuffer.Size()))
			{
				::LogClientSendFail(i + 1, j + 1, ::SYS_GetLastError(), ::HP_GetSocketErrorDesc(SE_DATA_SEND));

				bTerminated = TRUE;
				break;
			}
		}

		if(bTerminated)
			break;

		if(g_app_arg.test_interval > 0 && i + 1 < g_app_arg.test_times)
			::WaitFor(g_app_arg.test_interval);
	}

	s_sendBuffer.Free();
}

void OnCmdStop(CCommandParser* pParser)
{
	if(!s_pagent->HasStarted())
	{
		::LogAgentStopFail(SE_ILLEGAL_STATE, ::HP_GetSocketErrorDesc(SE_ILLEGAL_STATE));
		return;
	}

	::LogAgentStopping(s_pagent->GetConnectionCount());

	s_pagent->Stop();

	s_stat.CheckStatistics(FALSE);
}

void OnCmdStatus(CCommandParser* pParser)
{
	pParser->PrintStatus(s_pagent->GetState());
}

int main(int argc, char* const argv[])
{
	CTermAttrInitializer term_attr;
	CAppSignalHandler s_signal_handler({SIGTTOU, SIGINT});

	g_app_arg.ParseArgs(argc, argv);

	g_app_arg.async = FALSE;
	g_app_arg.ShowPFMTestArgs(TRUE);

	if(!s_pagent->SetupSSLContext(g_c_iVerifyMode, g_c_lpszPemCertFile, g_c_lpszPemKeyFile, g_c_lpszKeyPasswod, g_c_lpszCAPemCertFileOrPath))
	{
		int iErrCode = ::SYS_GetLastError();
		::LogAgentStartFail(iErrCode, _T("initialize SSL env fail"));

		EXIT(EX_CONFIG, iErrCode);
	}

	s_pagent->SetWorkerThreadCount(g_app_arg.thread_count);
	s_pagent->SetMaxConnectionCount(g_app_arg.max_conn);
	s_pagent->SetSendPolicy(g_app_arg.send_policy);
	s_pagent->SetKeepAliveTime(g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);

	CCommandParser::CMD_FUNC fnCmds[CCommandParser::CT_MAX] = {0};

	fnCmds[CCommandParser::CT_START]  = OnCmdStart;
	fnCmds[CCommandParser::CT_STOP]	  = OnCmdStop;
	fnCmds[CCommandParser::CT_STATUS] = OnCmdStatus;

	CCommandParser s_cmd_parser(CCommandParser::AT_AGENT, fnCmds);
	s_cmd_parser.Run();

	if(s_pagent->HasStarted())
		OnCmdStop(&s_cmd_parser);

	return EXIT_CODE_OK;
}
