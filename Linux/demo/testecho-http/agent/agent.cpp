#include "../../global/helper.h"
#include "../../../src/HttpAgent.h"

class CHttpAgentListenerImpl : public CHttpAgentListener
{
private:
	virtual EnHandleResult OnSend(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnClose(ITcpAgent* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult OnConnect(ITcpAgent* pSender, CONNID dwConnID);
	virtual EnHandleResult OnHandShake(ITcpAgent* pSender, CONNID dwConnID);
	virtual EnHandleResult OnShutdown(ITcpAgent* pSender);

	virtual EnHttpParseResult OnMessageBegin(IHttpAgent* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnStatusLine(IHttpAgent* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc);
	virtual EnHttpParseResult OnHeader(IHttpAgent* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue);
	virtual EnHttpParseResult OnHeadersComplete(IHttpAgent* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnBody(IHttpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHttpParseResult OnChunkHeader(IHttpAgent* pSender, CONNID dwConnID, int iLength);
	virtual EnHttpParseResult OnChunkComplete(IHttpAgent* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnMessageComplete(IHttpAgent* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnUpgrade(IHttpAgent* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType);
	virtual EnHttpParseResult OnParseError(IHttpAgent* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc);

	virtual EnHandleResult OnWSMessageHeader(IHttpAgent* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen);
	virtual EnHandleResult OnWSMessageBody(IHttpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnWSMessageComplete(IHttpAgent* pSender, CONNID dwConnID);

private:
	CStringA GetHeaderSummary(IHttpAgent* pHttpAgent, CONNID dwConnID, LPCSTR lpszSep = "  ", int iSepCount = 0, BOOL bWithContentLength = TRUE);

public:
	CHttpAgentListenerImpl(LPCTSTR lpszName)
		: m_strName(lpszName)
	{
	}

public:
	CString m_strName;

};

// ------------------------------------------------------------------------------------------------------------- //

EnHandleResult CHttpAgentListenerImpl::OnConnect(ITcpAgent* pSender, CONNID dwConnID)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	pSender->GetRemoteHost(dwConnID, szAddress, iAddressLen, usPort);
	::PostOnConnect2(dwConnID, szAddress, usPort, m_strName);

	return HR_OK;
}

EnHandleResult CHttpAgentListenerImpl::OnHandShake(ITcpAgent* pSender, CONNID dwConnID)
{
	::PostOnHandShake(dwConnID, m_strName);

	return HR_OK;
}

EnHandleResult CHttpAgentListenerImpl::OnShutdown(ITcpAgent* pSender)
{
	::PostOnShutdown();
	return HR_OK;
}

EnHandleResult CHttpAgentListenerImpl::OnSend(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength, m_strName);
	return HR_OK;
}

EnHandleResult CHttpAgentListenerImpl::OnReceive(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnReceive(dwConnID, pData, iLength, m_strName);
	return HR_OK;
}

EnHandleResult CHttpAgentListenerImpl::OnClose(ITcpAgent* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID, m_strName)	:
	::PostOnError(dwConnID, enOperation, iErrorCode, m_strName)	;

	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHttpParseResult CHttpAgentListenerImpl::OnMessageBegin(IHttpAgent* pSender, CONNID dwConnID)
{
	::PostOnMessageBegin(dwConnID, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpAgentListenerImpl::OnStatusLine(IHttpAgent* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)
{
	::PostOnStatusLine(dwConnID, usStatusCode, lpszDesc, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpAgentListenerImpl::OnHeader(IHttpAgent* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
{
	::PostOnHeader(dwConnID, lpszName, lpszValue, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpAgentListenerImpl::OnHeadersComplete(IHttpAgent* pSender, CONNID dwConnID)
{
	CStringA strSummary = GetHeaderSummary(pSender, dwConnID, "    ", 0, TRUE);
	::PostOnHeadersComplete(dwConnID, strSummary, m_strName);

	return HPR_OK;
}

EnHttpParseResult CHttpAgentListenerImpl::OnBody(IHttpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnBody(dwConnID, pData, iLength, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpAgentListenerImpl::OnChunkHeader(IHttpAgent* pSender, CONNID dwConnID, int iLength)
{
	::PostOnChunkHeader(dwConnID, iLength, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpAgentListenerImpl::OnChunkComplete(IHttpAgent* pSender, CONNID dwConnID)
{
	::PostOnChunkComplete(dwConnID, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpAgentListenerImpl::OnMessageComplete(IHttpAgent* pSender, CONNID dwConnID)
{
	::PostOnMessageComplete(dwConnID, m_strName);

	return HPR_OK;
}

EnHttpParseResult CHttpAgentListenerImpl::OnUpgrade(IHttpAgent* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
{
	::PostOnUpgrade(dwConnID, enUpgradeType, m_strName);

	return HPR_OK;
}

EnHttpParseResult CHttpAgentListenerImpl::OnParseError(IHttpAgent* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
{
	::PostOnParseError(dwConnID, iErrorCode, lpszErrorDesc, m_strName);
	return HPR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHandleResult CHttpAgentListenerImpl::OnWSMessageHeader(IHttpAgent* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
{
	::PostOnWSMessageHeader(dwConnID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen, m_strName);

	return HR_OK;
}

EnHandleResult CHttpAgentListenerImpl::OnWSMessageBody(IHttpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnWSMessageBody(dwConnID, pData, iLength, m_strName);

	return HR_OK;
}

EnHandleResult CHttpAgentListenerImpl::OnWSMessageComplete(IHttpAgent* pSender, CONNID dwConnID)
{
	::PostOnWSMessageComplete(dwConnID, m_strName);

	BYTE iOperationCode;

	VERIFY(pSender->GetWSMessageState(dwConnID, nullptr, nullptr, &iOperationCode, nullptr, nullptr, nullptr));

	if(iOperationCode == 0x8)
		return HR_ERROR;

	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

CStringA CHttpAgentListenerImpl::GetHeaderSummary(IHttpAgent* pHttpAgent, CONNID dwConnID, LPCSTR lpszSep, int iSepCount, BOOL bWithContentLength)
{
	CStringA strSEP1;

	for(int i = 0; i < iSepCount; i++)
		strSEP1 += lpszSep;

	CStringA strSEP2(strSEP1);
	strSEP2 += lpszSep;

	LPCSTR SEP1 = (LPCSTR)strSEP1;
	LPCSTR SEP2 = (LPCSTR)strSEP2;

	CStringA strResult;

	strResult.AppendFormat("%s[Status Fields]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%13s: %u%s", SEP2, "Status Code", pHttpAgent->GetStatusCode(dwConnID), CRLF);

	DWORD dwHeaderCount = 0;
	pHttpAgent->GetAllHeaders(dwConnID, nullptr, dwHeaderCount);

	strResult.AppendFormat("%s[Response Headers]%s", SEP1, CRLF);

	if(dwHeaderCount == 0)
		strResult.AppendFormat("%s(no header)%s", SEP2, CRLF);
	else
	{
		unique_ptr<THeader[]> headers(new THeader[dwHeaderCount]);
		VERIFY(pHttpAgent->GetAllHeaders(dwConnID, headers.get(), dwHeaderCount));

		for(DWORD i = 0; i < dwHeaderCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, headers[i].name, headers[i].value, CRLF);
	}

	DWORD dwCookieCount = 0;
	pHttpAgent->GetAllCookies(dwConnID, nullptr, dwCookieCount);

	strResult.AppendFormat("%s[Cookies]%s", SEP1, CRLF);

	if(dwCookieCount == 0)
		strResult.AppendFormat("%s(no cookie)%s", SEP2, CRLF);
	else
	{
		unique_ptr<TCookie[]> cookies(new TCookie[dwCookieCount]);
		VERIFY(pHttpAgent->GetAllCookies(dwConnID, cookies.get(), dwCookieCount));

		for(DWORD i = 0; i < dwCookieCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, cookies[i].name, cookies[i].value, CRLF);
	}

	CStringA strVersion;
	::HttpVersionToString((EnHttpVersion)pHttpAgent->GetVersion(dwConnID), strVersion);
	EnHttpUpgradeType enUpgType	= pHttpAgent->GetUpgradeType(dwConnID);
	LPCSTR lpszUpgrade			= enUpgType != HUT_NONE ? "true" : "false";
	LPCSTR lpszKeepAlive		= pHttpAgent->IsKeepAlive(dwConnID) ? "true" : "false";

	strResult.AppendFormat("%s[Basic Info]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "Version", (LPCSTR)strVersion, CRLF);
	strResult.AppendFormat("%s%13s: %u%s", SEP2, "Status Code", pHttpAgent->GetStatusCode(dwConnID), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsUpgrade", lpszUpgrade, CRLF);
	if(enUpgType != HUT_NONE)
		strResult.AppendFormat("%s%13s: %d%s", SEP2, "UpgradeType", enUpgType, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsKeepAlive", lpszKeepAlive, CRLF);
	if(bWithContentLength)
		strResult.AppendFormat("%s%13s: %lld%s", SEP2, "ContentLength", pHttpAgent->GetContentLength(dwConnID), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "ContentType", pHttpAgent->GetContentType(dwConnID), CRLF);
 
	return strResult;
}

// ------------------------------------------------------------------------------------------------------------- //

CHttpAgentListenerImpl s_listener_1(HTTP_NAME);
CHttpAgentListenerImpl s_listener_2(HTTPS_NAME);
CHttpAgent s_http_agent(&s_listener_1);
CHttpsAgent s_https_agent(&s_listener_2);

BOOL s_bHttps			= FALSE;
LPCTSTR s_lpszName		= HTTP_NAME;
USHORT s_usPort			= 0;
IHttpAgent* s_pAgent	= (IHttpAgent*)&s_http_agent;

// ------------------------------------------------------------------------------------------------------------- //

void SendHttp(CHttpCommandParser* pParser)
{
	unique_ptr<THeader[]> szHeaders;
	int iSize = (int)pParser->m_vtHeaders.size() / 2;

	if(iSize > 0)
	{
		szHeaders.reset(new THeader[iSize]);

		for(int i = 0; i < iSize; i ++)
		{
			szHeaders[i].name	= pParser->m_vtHeaders[2 * i];
			szHeaders[i].value	= pParser->m_vtHeaders[2 * i + 1];
		}
	}

	CString strContent;
	strContent.Format(_T("[%s] %s"), (LPCTSTR)pParser->m_strMethod, (LPCTSTR)pParser->m_strPath);
	::LogSending(pParser->m_dwConnID, strContent, s_lpszName);

	BOOL isOK = FALSE;

	if(!pParser->m_strData.IsEmpty() || pParser->m_strFilePath.IsEmpty())
		isOK = s_pAgent->SendRequest(pParser->m_dwConnID, pParser->m_strMethod, pParser->m_strPath, szHeaders.get(), iSize, (LPBYTE)(LPCSTR)pParser->m_strData, pParser->m_strData.GetLength());
	else
		isOK = s_pAgent->SendLocalFile(pParser->m_dwConnID, (LPCSTR)pParser->m_strFilePath, pParser->m_strMethod, pParser->m_strPath, szHeaders.get(), iSize);

	if(!isOK)
		::LogSendFail(pParser->m_dwConnID, ::GetLastError(), ::GetLastErrorStr(), s_lpszName);
}

void SendWebSocket(CHttpCommandParser* pParser)
{
	BYTE bCode		= 0x8;
	int iLength		= 0;
	BYTE* pData		= nullptr;

	if(pParser->m_strData.CompareNoCase(HTTP_WEB_SOCKET_CLOSE_FLAG) != 0)
	{
		bCode	= 0x1;
		iLength	= pParser->m_strData.GetLength();
		pData	= (BYTE*)(LPCSTR)pParser->m_strData;
	}

	CString strContent;
	strContent.Format(_T("[WebSocket] (oc: 0x%X, len: %d)"), bCode, iLength);
	::LogSending(pParser->m_dwConnID, strContent, s_lpszName);

	if(!s_pAgent->SendWSMessage(pParser->m_dwConnID, TRUE, 0, bCode, HTTP_WEB_SOCKET_MASK_KEY, pData, iLength))
		::LogSendFail(pParser->m_dwConnID, ::GetLastError(), ::GetLastErrorStr(), s_lpszName);
}

void OnCmdStart(CHttpCommandParser* pParser)
{
	if(s_pAgent->HasStarted())
	{
		::LogAgentStartFail(SE_ILLEGAL_STATE, ::GetSocketErrorDesc(SE_ILLEGAL_STATE));
		return;
	}

	s_bHttps	= pParser->m_bHttps;
	s_lpszName	= pParser->m_bHttps ? HTTPS_NAME : HTTP_NAME;
	s_usPort	= pParser->m_bHttps ? g_app_arg.https_port : g_app_arg.http_port;
	s_pAgent	= pParser->m_bHttps ? (IHttpAgent*)&s_https_agent : (IHttpAgent*)&s_http_agent;

	if(s_pAgent->Start(g_app_arg.bind_addr, g_app_arg.async))
		::LogAgentStart(g_app_arg.bind_addr, g_app_arg.async, s_lpszName);
	else
		::LogAgentStartFail(s_pAgent->GetLastError(), s_pAgent->GetLastErrorDesc(), s_lpszName);
}

void OnCmdStop(CHttpCommandParser* pParser)
{
	if(s_pAgent->Stop())
		::LogAgentStop(s_lpszName);
	else
		::LogAgentStopFail(s_pAgent->GetLastError(), s_pAgent->GetLastErrorDesc(), s_lpszName);
}

void OnCmdStatus(CHttpCommandParser* pParser)
{
	pParser->PrintStatus(s_pAgent->GetState(), s_lpszName);
}

void OnCmdConnect(CHttpCommandParser* pParser)
{
	USHORT usPort = pParser->m_usRemotePort;

	if(usPort == 0) usPort = s_usPort;

	::LogConnect(pParser->m_strRemoteAddr, usPort, s_lpszName);

	if(!s_pAgent->Connect(pParser->m_strRemoteAddr, usPort))
		::LogConnectFail(::GetLastError(), ::GetLastErrorStr(), s_lpszName);
}

void OnCmdSend(CHttpCommandParser* pParser)
{
	if(!s_pAgent->HasStarted())
	{
		::LogSendFail(pParser->m_dwConnID, SE_ILLEGAL_STATE, ::GetSocketErrorDesc(SE_ILLEGAL_STATE), s_lpszName);
		return;
	}

	s_pAgent->GetUpgradeType(pParser->m_dwConnID) == HUT_WEB_SOCKET ? SendWebSocket(pParser) : SendHttp(pParser);
}

void OnCmdPause(CHttpCommandParser* pParser)
{
	if(s_pAgent->PauseReceive(pParser->m_dwConnID, pParser->m_bFlag))
		::LogPause(pParser->m_dwConnID, pParser->m_bFlag, s_lpszName);
	else
		::LogPauseFail(pParser->m_dwConnID, pParser->m_bFlag, s_lpszName);
}

void OnCmdKick(CHttpCommandParser* pParser)
{
	if(s_pAgent->Disconnect(pParser->m_dwConnID, pParser->m_bFlag))
		::LogDisconnect2(pParser->m_dwConnID, pParser->m_bFlag, s_lpszName);
	else
		::LogDisconnectFail2(pParser->m_dwConnID, pParser->m_bFlag, s_lpszName);
}

void OnCmdKickLong(CHttpCommandParser* pParser)
{
	if(s_pAgent->DisconnectLongConnections(pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag, s_lpszName);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag, s_lpszName);
}

void OnCmdKickSilence(CHttpCommandParser* pParser)
{
	if(s_pAgent->DisconnectSilenceConnections(pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag, s_lpszName);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag, s_lpszName);
}

BOOL InitHttpAgents()
{
	if(!s_https_agent.SetupSSLContext(SSL_VM_NONE, g_c_lpszPemCertFile, g_c_lpszPemKeyFile, g_c_lpszKeyPasswod, g_c_lpszCAPemCertFileOrPath))
	{
		::LogAgentStartFail(::GetLastError(), _T("initialize SSL env fail"), s_lpszName);
		return FALSE;
	}

	s_usPort = g_app_arg.http_port;
	s_http_agent.SetUseCookie(g_app_arg.http_use_cookie);
	s_https_agent.SetUseCookie(g_app_arg.http_use_cookie);
	s_http_agent.SetKeepAliveTime(g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);
	s_https_agent.SetKeepAliveTime(g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);

	return TRUE;
}

int main(int argc, char* const argv[])
{
	CTermAttrInitializer term_attr;
	CAppSignalHandler s_signal_handler({SIGTTOU, SIGINT});

	g_app_arg.ParseArgs(argc, argv);

	if(g_app_arg.http_use_cookie)
		g_CookieMgr.LoadFromFile(g_lpszDefaultCookieFile, FALSE);

	if(!InitHttpAgents())
		return EXIT_CODE_CONFIG;
	
	CCommandParser::CMD_FUNC fnCmds[CCommandParser::CT_MAX] = {0};

	fnCmds[CCommandParser::CT_START]	= (CCommandParser::CMD_FUNC)OnCmdStart;
	fnCmds[CCommandParser::CT_STOP]		= (CCommandParser::CMD_FUNC)OnCmdStop;
	fnCmds[CCommandParser::CT_STATUS]	= (CCommandParser::CMD_FUNC)OnCmdStatus;
	fnCmds[CCommandParser::CT_CONNECT]	= (CCommandParser::CMD_FUNC)OnCmdConnect;
	fnCmds[CCommandParser::CT_SEND]		= (CCommandParser::CMD_FUNC)OnCmdSend;
	fnCmds[CCommandParser::CT_PAUSE]	= (CCommandParser::CMD_FUNC)OnCmdPause;
	fnCmds[CCommandParser::CT_KICK]		= (CCommandParser::CMD_FUNC)OnCmdKick;
	fnCmds[CCommandParser::CT_KICK_L]	= (CCommandParser::CMD_FUNC)OnCmdKickLong;
	fnCmds[CCommandParser::CT_KICK_S]	= (CCommandParser::CMD_FUNC)OnCmdKickSilence;

	CHttpCommandParser s_cmd_parser(CCommandParser::AT_AGENT, fnCmds);
	s_cmd_parser.Run();

	if(g_app_arg.http_use_cookie)
		g_CookieMgr.SaveToFile(g_lpszDefaultCookieFile, TRUE);

	return EXIT_CODE_OK;
}
