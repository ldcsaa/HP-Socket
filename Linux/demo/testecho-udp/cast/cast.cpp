#include "../../global/helper.h"
#include "../../../src/UdpCast.h"

class CListenerImpl : public CUdpCastListener
{

public:
	virtual EnHandleResult OnPrepareConnect(IUdpCast* pSender, CONNID dwConnID, SOCKET socket) override
	{
		return HR_OK;
	}

	virtual EnHandleResult OnConnect(IUdpCast* pSender, CONNID dwConnID) override
	{
		TCHAR szAddress[50];
		int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
		USHORT usPort;

		pSender->GetRemoteHost(szAddress, iAddressLen, usPort);

		::PostOnConnect2(dwConnID, szAddress, usPort);

		return HR_OK;
	}

	virtual EnHandleResult OnHandShake(IUdpCast* pSender, CONNID dwConnID) override
	{
		return HR_OK;
	}

	virtual EnHandleResult OnReceive(IUdpCast* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
		TCHAR szAddress[50];
		int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
		USHORT usPort;

		(pSender)->GetRemoteAddress(szAddress, iAddressLen, usPort);

		::PostOnReceiveCast(dwConnID, szAddress, usPort, pData, iLength);

		return HR_OK;
	}

	virtual EnHandleResult OnSend(IUdpCast* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
	{
		::PostOnSend(dwConnID, pData, iLength);
		return HR_OK;
	}

	virtual EnHandleResult OnClose(IUdpCast* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) override
	{
		iErrorCode == SE_OK ? ::PostOnClose(dwConnID) :
		::PostOnError(dwConnID, enOperation, iErrorCode);

		return HR_OK;
	}

};

CListenerImpl s_listener;
CUdpCast s_cast(&s_listener);

void OnCmdStart(CCommandParser* pParser)
{
	if(s_cast.Start(g_app_arg.remote_addr, g_app_arg.port, g_app_arg.async, g_app_arg.bind_addr))
		::LogClientStart(g_app_arg.remote_addr, g_app_arg.port);
	else
		::LogClientStartFail(s_cast.GetLastError(), s_cast.GetLastErrorDesc());
}

void OnCmdStop(CCommandParser* pParser)
{
	if(s_cast.Stop())
		::LogClientStop();
	else
		::LogClientStopFail(s_cast.GetLastError(), s_cast.GetLastErrorDesc());
}

void OnCmdStatus(CCommandParser* pParser)
{
	pParser->PrintStatus(s_cast.GetState());
}

void OnCmdSend(CCommandParser* pParser)
{
	if(s_cast.Send((LPBYTE)(LPCTSTR)pParser->m_strMessage, pParser->m_strMessage.GetLength()))
		::LogSend(s_cast.GetConnectionID(), pParser->m_strMessage);
	else
		::LogSendFail(s_cast.GetConnectionID(), ::GetLastError(), ::GetLastErrorStr());
}

void OnCmdPause(CCommandParser* pParser)
{
	if(s_cast.PauseReceive(pParser->m_bFlag))
		::LogPause(s_cast.GetConnectionID(), pParser->m_bFlag);
	else
		::LogPauseFail(s_cast.GetConnectionID(), pParser->m_bFlag);
}

int main(int argc, char* const argv[])
{
	CTermAttrInitializer term_attr;
	CAppSignalHandler s_signal_handler({SIGTTOU, SIGINT});

	g_app_arg.ParseArgs(argc, argv);

	s_cast.SetCastMode(g_app_arg.cast_mode);
	s_cast.SetReuseAddress(g_app_arg.reuse_addr);
	s_cast.SetMultiCastLoop(g_app_arg.ip_loop);
	s_cast.SetMultiCastTtl(g_app_arg.ttl);

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
