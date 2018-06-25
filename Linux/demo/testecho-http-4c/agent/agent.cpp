#include "../../global/helper.h"

#include <hpsocket/HPSocket4C-SSL.h>

// ------------------------------------------------------------------------------------------------------------- //

HP_HttpAgentListener s_http_listener;
HP_HttpAgent s_http_agent;
HP_HttpsAgent s_https_agent;
HP_HttpAgent s_pAgent;

BOOL s_bHttps		= FALSE;
LPCTSTR s_lpszName	= HTTP_NAME;
USHORT s_usPort		= 0;

// ------------------------------------------------------------------------------------------------------------- //

CStringA GetHeaderSummary(HP_HttpAgent pSender, CONNID dwConnID, LPCSTR lpszSep, int iSepCount, BOOL bWithContentLength)
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
	strResult.AppendFormat("%s%13s: %u%s", SEP2, "Status Code", ::HP_HttpAgent_GetStatusCode(pSender, dwConnID), CRLF);

	DWORD dwHeaderCount = 0;
	::HP_HttpAgent_GetAllHeaders(pSender, dwConnID, nullptr, &dwHeaderCount);

	strResult.AppendFormat("%s[Response Headers]%s", SEP1, CRLF);

	if(dwHeaderCount == 0)
		strResult.AppendFormat("%s(no header)%s", SEP2, CRLF);
	else
	{
		unique_ptr<THeader[]> headers(new THeader[dwHeaderCount]);
		VERIFY(::HP_HttpAgent_GetAllHeaders(pSender, dwConnID, headers.get(), &dwHeaderCount));

		for(DWORD i = 0; i < dwHeaderCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, headers[i].name, headers[i].value, CRLF);
	}

	DWORD dwCookieCount = 0;
	::HP_HttpAgent_GetAllCookies(pSender, dwConnID, nullptr, &dwCookieCount);

	strResult.AppendFormat("%s[Cookies]%s", SEP1, CRLF);

	if(dwCookieCount == 0)
		strResult.AppendFormat("%s(no cookie)%s", SEP2, CRLF);
	else
	{
		unique_ptr<TCookie[]> cookies(new TCookie[dwCookieCount]);
		VERIFY(::HP_HttpAgent_GetAllCookies(pSender, dwConnID, cookies.get(), &dwCookieCount));

		for(DWORD i = 0; i < dwCookieCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, cookies[i].name, cookies[i].value, CRLF);
	}

	CStringA strVersion;
	::HttpVersionToString((EnHttpVersion)::HP_HttpAgent_GetVersion(pSender, dwConnID), strVersion);
	EnHttpUpgradeType enUpgType	= ::HP_HttpAgent_GetUpgradeType(pSender, dwConnID);
	LPCSTR lpszUpgrade			= enUpgType != HUT_NONE ? "true" : "false";
	LPCSTR lpszKeepAlive		= ::HP_HttpAgent_IsKeepAlive(pSender, dwConnID) ? "true" : "false";

	strResult.AppendFormat("%s[Basic Info]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "Version", (LPCSTR)strVersion, CRLF);
	strResult.AppendFormat("%s%13s: %u%s", SEP2, "Status Code", ::HP_HttpAgent_GetStatusCode(pSender, dwConnID), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsUpgrade", lpszUpgrade, CRLF);
	if(enUpgType != HUT_NONE)
		strResult.AppendFormat("%s%13s: %d%s", SEP2, "UpgradeType", enUpgType, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsKeepAlive", lpszKeepAlive, CRLF);
	if(bWithContentLength)
		strResult.AppendFormat("%s%13s: %lld%s", SEP2, "ContentLength", ::HP_HttpAgent_GetContentLength(pSender, dwConnID), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "ContentType", ::HP_HttpAgent_GetContentType(pSender, dwConnID), CRLF);
 
	return strResult;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHandleResult __HP_CALL OnConnect(HP_HttpAgent pSender, CONNID dwConnID)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	::HP_Agent_GetRemoteHost(pSender, dwConnID, szAddress, &iAddressLen, &usPort);
	::PostOnConnect2(dwConnID, szAddress, usPort, s_lpszName);

	return HR_OK;
}

EnHandleResult __HP_CALL OnHandShake(HP_HttpAgent pSender, CONNID dwConnID)
{
	::PostOnHandShake(dwConnID, s_lpszName);

	return HR_OK;
}

EnHandleResult __HP_CALL OnShutdown(HP_HttpAgent pSender)
{
	::PostOnShutdown();
	return HR_OK;
}

EnHandleResult __HP_CALL OnSend(HP_HttpAgent pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength, s_lpszName);
	return HR_OK;
}

EnHandleResult __HP_CALL OnReceive(HP_HttpAgent pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnReceive(dwConnID, pData, iLength, s_lpszName);
	return HR_OK;
}

EnHandleResult __HP_CALL OnClose(HP_HttpAgent pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID, s_lpszName)	:
	::PostOnError(dwConnID, enOperation, iErrorCode, s_lpszName)	;

	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHttpParseResult __HP_CALL OnMessageBegin(HP_HttpAgent pSender, CONNID dwConnID)
{
	::PostOnMessageBegin(dwConnID, s_lpszName);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnStatusLine(HP_HttpAgent pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)
{
	::PostOnStatusLine(dwConnID, usStatusCode, lpszDesc, s_lpszName);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnHeader(HP_HttpAgent pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
{
	::PostOnHeader(dwConnID, lpszName, lpszValue, s_lpszName);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnHeadersComplete(HP_HttpAgent pSender, CONNID dwConnID)
{
	CStringA strSummary = GetHeaderSummary(pSender, dwConnID, "    ", 0, TRUE);
	::PostOnHeadersComplete(dwConnID, strSummary, s_lpszName);

	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnBody(HP_HttpAgent pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnBody(dwConnID, pData, iLength, s_lpszName);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnChunkHeader(HP_HttpAgent pSender, CONNID dwConnID, int iLength)
{
	::PostOnChunkHeader(dwConnID, iLength, s_lpszName);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnChunkComplete(HP_HttpAgent pSender, CONNID dwConnID)
{
	::PostOnChunkComplete(dwConnID, s_lpszName);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnMessageComplete(HP_HttpAgent pSender, CONNID dwConnID)
{
	::PostOnMessageComplete(dwConnID, s_lpszName);

	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnUpgrade(HP_HttpAgent pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
{
	::PostOnUpgrade(dwConnID, enUpgradeType, s_lpszName);

	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnParseError(HP_HttpAgent pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
{
	::PostOnParseError(dwConnID, iErrorCode, lpszErrorDesc, s_lpszName);
	return HPR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHandleResult __HP_CALL OnWSMessageHeader(HP_HttpAgent pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
{
	::PostOnWSMessageHeader(dwConnID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen, s_lpszName);

	return HR_OK;
}

EnHandleResult __HP_CALL OnWSMessageBody(HP_HttpAgent pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnWSMessageBody(dwConnID, pData, iLength, s_lpszName);

	return HR_OK;
}

EnHandleResult __HP_CALL OnWSMessageComplete(HP_HttpAgent pSender, CONNID dwConnID)
{
	::PostOnWSMessageComplete(dwConnID, s_lpszName);

	BYTE iOperationCode;

	VERIFY(::HP_HttpAgent_GetWSMessageState(pSender, dwConnID, nullptr, nullptr, &iOperationCode, nullptr, nullptr, nullptr));

	if(iOperationCode == 0x8)
		return HR_ERROR;

	return HR_OK;
}

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
		isOK = ::HP_HttpAgent_SendRequest(s_pAgent, pParser->m_dwConnID, pParser->m_strMethod, pParser->m_strPath, szHeaders.get(), iSize, (LPBYTE)(LPCSTR)pParser->m_strData, pParser->m_strData.GetLength());
	else
		isOK = ::HP_HttpAgent_SendLocalFile(s_pAgent, pParser->m_dwConnID, (LPCSTR)pParser->m_strFilePath, pParser->m_strMethod, pParser->m_strPath, szHeaders.get(), iSize);

	if(!isOK)
		::LogSendFail(pParser->m_dwConnID, ::SYS_GetLastError(), ::GetLastErrorStr(), s_lpszName);
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

	if(!::HP_HttpAgent_SendWSMessage(s_pAgent, pParser->m_dwConnID, TRUE, 0, bCode, HTTP_WEB_SOCKET_MASK_KEY, pData, iLength, 0))
		::LogSendFail(pParser->m_dwConnID, ::SYS_GetLastError(), ::SYS_GetLastErrorStr(), s_lpszName);
}

void OnCmdStart(CHttpCommandParser* pParser)
{
	if(::HP_Agent_HasStarted(s_pAgent))
	{
		::LogAgentStartFail(SE_ILLEGAL_STATE, ::HP_GetSocketErrorDesc(SE_ILLEGAL_STATE));
		return;
	}

	s_bHttps	= pParser->m_bHttps;
	s_lpszName	= pParser->m_bHttps ? HTTPS_NAME : HTTP_NAME;
	s_usPort	= pParser->m_bHttps ? g_app_arg.https_port : g_app_arg.http_port;
	s_pAgent	= pParser->m_bHttps ? s_https_agent : s_http_agent;

	if(::HP_Agent_Start(s_pAgent, g_app_arg.bind_addr, g_app_arg.async))
		::LogAgentStart(g_app_arg.bind_addr, g_app_arg.async, s_lpszName);
	else
		::LogAgentStartFail(::HP_Agent_GetLastError(s_pAgent), ::HP_Agent_GetLastErrorDesc(s_pAgent), s_lpszName);
}

void OnCmdStop(CHttpCommandParser* pParser)
{
	if(::HP_Agent_Stop(s_pAgent))
		::LogAgentStop(s_lpszName);
	else
		::LogAgentStopFail(::HP_Agent_GetLastError(s_pAgent), ::HP_Agent_GetLastErrorDesc(s_pAgent), s_lpszName);
}

void OnCmdStatus(CHttpCommandParser* pParser)
{
	pParser->PrintStatus(::HP_Agent_GetState(s_pAgent), s_lpszName);
}

void OnCmdConnect(CHttpCommandParser* pParser)
{
	USHORT usPort = pParser->m_usRemotePort;

	if(usPort == 0) usPort = s_usPort;

	::LogConnect(pParser->m_strRemoteAddr, usPort, s_lpszName);

	if(!::HP_Agent_Connect(s_pAgent, pParser->m_strRemoteAddr, usPort, nullptr))
		::LogConnectFail(::SYS_GetLastError(), ::SYS_GetLastErrorStr(), s_lpszName);
}

void OnCmdSend(CHttpCommandParser* pParser)
{
	if(!::HP_Agent_HasStarted(s_pAgent))
	{
		::LogSendFail(pParser->m_dwConnID, SE_ILLEGAL_STATE, ::HP_GetSocketErrorDesc(SE_ILLEGAL_STATE), s_lpszName);
		return;
	}

	::HP_HttpAgent_GetUpgradeType(s_pAgent, pParser->m_dwConnID) == HUT_WEB_SOCKET ? SendWebSocket(pParser) : SendHttp(pParser);
}

void OnCmdPause(CHttpCommandParser* pParser)
{
	if(::HP_Agent_PauseReceive(s_pAgent, pParser->m_dwConnID, pParser->m_bFlag))
		::LogPause(pParser->m_dwConnID, pParser->m_bFlag, s_lpszName);
	else
		::LogPauseFail(pParser->m_dwConnID, pParser->m_bFlag, s_lpszName);
}

void OnCmdKick(CHttpCommandParser* pParser)
{
	if(::HP_Agent_Disconnect(s_pAgent, pParser->m_dwConnID, pParser->m_bFlag))
		::LogDisconnect2(pParser->m_dwConnID, pParser->m_bFlag, s_lpszName);
	else
		::LogDisconnectFail2(pParser->m_dwConnID, pParser->m_bFlag, s_lpszName);
}

void OnCmdKickLong(CHttpCommandParser* pParser)
{
	if(::HP_Agent_DisconnectLongConnections(s_pAgent, pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag, s_lpszName);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag, s_lpszName);
}

void OnCmdKickSilence(CHttpCommandParser* pParser)
{
	if(::HP_Agent_DisconnectSilenceConnections(s_pAgent, pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag, s_lpszName);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag, s_lpszName);
}

void DestroyHPSocketObjects()
{
	::Destroy_HP_HttpAgent(s_http_agent);
	::Destroy_HP_HttpsAgent(s_https_agent);
	::Destroy_HP_HttpAgentListener(s_http_listener);
}

BOOL CreateHPSocketObjects()
{
	s_http_listener	= ::Create_HP_HttpAgentListener();
	s_http_agent	= ::Create_HP_HttpAgent(s_http_listener);
	s_https_agent	= ::Create_HP_HttpsAgent(s_http_listener);

	if(!::HP_SSLAgent_SetupSSLContext(s_https_agent, SSL_VM_NONE, g_s_lpszPemCertFile, g_s_lpszPemKeyFile, g_s_lpszKeyPasswod, g_s_lpszCAPemCertFileOrPath))
	{
		::LogAgentStartFail(::SYS_GetLastError(), _T("initialize SSL env fail"));

		DestroyHPSocketObjects();
		return FALSE;
	}

	::HP_Set_FN_HttpAgent_OnConnect(s_http_listener, OnConnect);
	::HP_Set_FN_HttpAgent_OnHandShake(s_http_listener, OnHandShake);
	::HP_Set_FN_HttpAgent_OnReceive(s_http_listener, OnReceive);
	::HP_Set_FN_HttpAgent_OnSend(s_http_listener, OnSend);
	::HP_Set_FN_HttpAgent_OnClose(s_http_listener, OnClose);
	::HP_Set_FN_HttpAgent_OnShutdown(s_http_listener, OnShutdown);

	::HP_Set_FN_HttpAgent_OnMessageBegin(s_http_listener, OnMessageBegin);
	::HP_Set_FN_HttpAgent_OnStatusLine(s_http_listener, OnStatusLine);
	::HP_Set_FN_HttpAgent_OnHeader(s_http_listener, OnHeader);
	::HP_Set_FN_HttpAgent_OnHeadersComplete(s_http_listener, OnHeadersComplete);
	::HP_Set_FN_HttpAgent_OnBody(s_http_listener, OnBody);
	::HP_Set_FN_HttpAgent_OnChunkHeader(s_http_listener, OnChunkHeader);
	::HP_Set_FN_HttpAgent_OnChunkComplete(s_http_listener, OnChunkComplete);
	::HP_Set_FN_HttpAgent_OnMessageComplete(s_http_listener, OnMessageComplete);
	::HP_Set_FN_HttpAgent_OnUpgrade(s_http_listener, OnUpgrade);
	::HP_Set_FN_HttpAgent_OnParseError(s_http_listener, OnParseError);

	::HP_Set_FN_HttpAgent_OnWSMessageHeader(s_http_listener, OnWSMessageHeader);
	::HP_Set_FN_HttpAgent_OnWSMessageBody(s_http_listener, OnWSMessageBody);
	::HP_Set_FN_HttpAgent_OnWSMessageComplete(s_http_listener, OnWSMessageComplete);

	::HP_HttpAgent_SetUseCookie(s_http_agent, g_app_arg.http_use_cookie);
	::HP_HttpAgent_SetUseCookie(s_https_agent, g_app_arg.http_use_cookie);
	::HP_TcpAgent_SetKeepAliveTime(s_http_agent, g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);
	::HP_TcpAgent_SetKeepAliveTime(s_https_agent, g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);

	s_usPort = g_app_arg.http_port;
	s_pAgent = s_http_agent;

	return TRUE;
}

int main(int argc, char* const argv[])
{
	CTermAttrInitializer term_attr;
	CAppSignalHandler s_signal_handler({SIGTTOU, SIGINT});

	g_app_arg.ParseArgs(argc, argv);

	if(g_app_arg.http_use_cookie)
		::HP_HttpCookie_MGR_LoadFromFile(g_lpszDefaultCookieFile, FALSE);

	if(!CreateHPSocketObjects())
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

	DestroyHPSocketObjects();

	if(g_app_arg.http_use_cookie)
		::HP_HttpCookie_MGR_SaveToFile(g_lpszDefaultCookieFile, TRUE);

	return EXIT_CODE_OK;
}
