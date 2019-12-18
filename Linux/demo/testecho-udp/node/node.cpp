#include "../../global/helper.h"
#include "../../../src/UdpNode.h"

class CListenerImpl : public CUdpNodeListener
{

public:
	virtual EnHandleResult OnPrepareListen(IUdpNode* pSender, SOCKET soListen) override
	{
		TCHAR szAddress[50];
		int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
		USHORT usPort;

		pSender->GetLocalAddress(szAddress, iAddressLen, usPort);

		::PostOnPrepareListen(szAddress, usPort);

		return HR_OK;
	}

	virtual EnHandleResult OnReceive(IUdpNode* pSender, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pData, int iLength) override
	{
		::PostOnReceiveFrom(0, lpszRemoteAddress, usRemotePort, pData, iLength);

		//pSender->Send(lpszRemoteAddress, usRemotePort, pData, iLength);

		return HR_IGNORE;
	}

	virtual EnHandleResult OnSend(IUdpNode* pSender, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pData, int iLength) override
	{
		::PostOnSendTo(0, lpszRemoteAddress, usRemotePort, pData, iLength);
		return HR_IGNORE;
	}

	virtual EnHandleResult OnError(IUdpNode* pSender, EnSocketOperation enOperation, int iErrorCode, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pBuffer, int iLength) override
	{
		::PostOnError2(0, enOperation, iErrorCode, lpszRemoteAddress, usRemotePort, pBuffer, iLength);
		return HR_IGNORE;
	}

	virtual EnHandleResult OnShutdown(IUdpNode* pSender) override
	{
		::PostOnShutdown();
		return HR_IGNORE;
	}

};

CListenerImpl s_listener;
CUdpNode s_node(&s_listener);

void OnCmdStart(CCommandParser* pParser)
{
	if(s_node.Start(g_app_arg.bind_addr, g_app_arg.port, g_app_arg.cast_mode, g_app_arg.remote_addr))
		::LogStart(g_app_arg.bind_addr, g_app_arg.port);
	else
		::LogClientStartFail(s_node.GetLastError(), s_node.GetLastErrorDesc());
}

void OnCmdStop(CCommandParser* pParser)
{
	if(s_node.Stop())
		::LogStop();
	else
		::LogClientStopFail(s_node.GetLastError(), s_node.GetLastErrorDesc());
}

void OnCmdStatus(CCommandParser* pParser)
{
	pParser->PrintStatus(s_node.GetState());
}

void OnCmdSend(CCommandParser* pParser)
{
	if(s_node.Send(pParser->m_strRemoteAddr, pParser->m_usRemotePort, (LPBYTE)(LPCTSTR)pParser->m_strMessage, pParser->m_strMessage.GetLength()))
		::LogSend(pParser->m_strMessage);
	else
		::LogSendFail(::GetLastError(), ::GetLastErrorStr());
}

void OnCmdSendCast(CCommandParser* pParser)
{
	if(s_node.SendCast((LPBYTE)(LPCTSTR)pParser->m_strMessage, pParser->m_strMessage.GetLength()))
		::LogSend(pParser->m_strMessage);
	else
		::LogSendFail(::GetLastError(), ::GetLastErrorStr());
}

int main(int argc, char* const argv[])
{
	CTermAttrInitializer term_attr;
	CAppSignalHandler s_signal_handler({SIGTTOU, SIGINT});

	g_app_arg.cast_mode		= CM_UNICAST;
	g_app_arg.remote_addr	= DEF_MULTI_CAST_ADDRESS;

	g_app_arg.ParseArgs(argc, argv);

	s_node.SetReuseAddressPolicy(g_app_arg.reuse_addr);
	s_node.SetMultiCastLoop(g_app_arg.ip_loop);
	s_node.SetMultiCastTtl(g_app_arg.ttl);

	CCommandParser::CMD_FUNC fnCmds[CCommandParser::CT_MAX] = {0};

	fnCmds[CCommandParser::CT_START]	= OnCmdStart;
	fnCmds[CCommandParser::CT_STOP]		= OnCmdStop;
	fnCmds[CCommandParser::CT_STATUS]	= OnCmdStatus;
	fnCmds[CCommandParser::CT_SEND]		= OnCmdSend;
	fnCmds[CCommandParser::CT_SENDC]	= OnCmdSendCast;

	CCommandParser s_cmd_parser(CCommandParser::AT_NODE, fnCmds);
	s_cmd_parser.Run();

	return EXIT_CODE_OK;
}
