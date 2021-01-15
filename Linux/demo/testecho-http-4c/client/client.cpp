#include "../../global/helper.h"

#include <hpsocket/HPSocket4C-SSL.h>

// ------------------------------------------------------------------------------------------------------------- //

HP_HttpClientListener s_http_listener;
HP_HttpClient s_http_client;
HP_HttpsClient s_https_client;
HP_HttpClient s_pClient;

BOOL s_bHttps		= FALSE;
LPCTSTR s_lpszName	= HTTP_NAME;
USHORT s_usPort		= 0;

// ------------------------------------------------------------------------------------------------------------- //

CStringA GetHeaderSummary(HP_HttpClient pSender, LPCSTR lpszSep, int iSepCount, BOOL bWithContentLength)
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
	strResult.AppendFormat("%s%13s: %u%s", SEP2, "Status Code", ::HP_HttpClient_GetStatusCode(pSender), CRLF);

	DWORD dwHeaderCount = 0;
	::HP_HttpClient_GetAllHeaders(pSender, nullptr, &dwHeaderCount);

	strResult.AppendFormat("%s[Response Headers]%s", SEP1, CRLF);

	if(dwHeaderCount == 0)
		strResult.AppendFormat("%s(no header)%s", SEP2, CRLF);
	else
	{
		unique_ptr<THeader[]> headers(new THeader[dwHeaderCount]);
		VERIFY(::HP_HttpClient_GetAllHeaders(pSender, headers.get(), &dwHeaderCount));

		for(DWORD i = 0; i < dwHeaderCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, headers[i].name, headers[i].value, CRLF);
	}

	DWORD dwCookieCount = 0;
	::HP_HttpClient_GetAllCookies(pSender, nullptr, &dwCookieCount);

	strResult.AppendFormat("%s[Cookies]%s", SEP1, CRLF);

	if(dwCookieCount == 0)
		strResult.AppendFormat("%s(no cookie)%s", SEP2, CRLF);
	else
	{
		unique_ptr<TCookie[]> cookies(new TCookie[dwCookieCount]);
		VERIFY(::HP_HttpClient_GetAllCookies(pSender, cookies.get(), &dwCookieCount));

		for(DWORD i = 0; i < dwCookieCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, cookies[i].name, cookies[i].value, CRLF);
	}

	CStringA strVersion;
	::HttpVersionToString((EnHttpVersion)::HP_HttpClient_GetVersion(pSender), strVersion);
	EnHttpUpgradeType enUpgType	= ::HP_HttpClient_GetUpgradeType(pSender);
	LPCSTR lpszUpgrade			= enUpgType != HUT_NONE ? "true" : "false";
	LPCSTR lpszKeepAlive		= ::HP_HttpClient_IsKeepAlive(pSender) ? "true" : "false";

	strResult.AppendFormat("%s[Basic Info]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "Version", (LPCSTR)strVersion, CRLF);
	strResult.AppendFormat("%s%13s: %u%s", SEP2, "Status Code", ::HP_HttpClient_GetStatusCode(pSender), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsUpgrade", lpszUpgrade, CRLF);
	if(enUpgType != HUT_NONE)
		strResult.AppendFormat("%s%13s: %d%s", SEP2, "UpgradeType", enUpgType, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsKeepAlive", lpszKeepAlive, CRLF);
	if(bWithContentLength)
		strResult.AppendFormat("%s%13s: %lld%s", SEP2, "ContentLength", ::HP_HttpClient_GetContentLength(pSender), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "ContentType", ::HP_HttpClient_GetContentType(pSender), CRLF);
 
	return strResult;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHandleResult __HP_CALL OnConnect(HP_HttpClient pSender, CONNID dwConnID)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	::HP_Client_GetRemoteHost(pSender, szAddress, &iAddressLen, &usPort);
	::PostOnConnect2(dwConnID, szAddress, usPort, s_lpszName);

	return HR_OK;
}

EnHandleResult __HP_CALL OnHandShake(HP_HttpClient pSender, CONNID dwConnID)
{
	::PostOnHandShake(dwConnID, s_lpszName);

	return HR_OK;
}

EnHandleResult __HP_CALL OnSend(HP_HttpClient pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength, s_lpszName);
	return HR_OK;
}

EnHandleResult __HP_CALL OnReceive(HP_HttpClient pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnReceive(dwConnID, pData, iLength, s_lpszName);
	return HR_OK;
}

EnHandleResult __HP_CALL OnClose(HP_HttpClient pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID, s_lpszName)	:
	::PostOnError(dwConnID, enOperation, iErrorCode, s_lpszName)	;

	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHttpParseResult __HP_CALL OnMessageBegin(HP_HttpClient pSender, CONNID dwConnID)
{
	::PostOnMessageBegin(dwConnID, s_lpszName);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnStatusLine(HP_HttpClient pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)
{
	::PostOnStatusLine(dwConnID, usStatusCode, lpszDesc, s_lpszName);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnHeader(HP_HttpClient pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
{
	::PostOnHeader(dwConnID, lpszName, lpszValue, s_lpszName);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnHeadersComplete(HP_HttpClient pSender, CONNID dwConnID)
{
	CStringA strSummary = GetHeaderSummary(pSender, "    ", 0, TRUE);
	::PostOnHeadersComplete(dwConnID, strSummary, s_lpszName);

	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnBody(HP_HttpClient pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnBody(dwConnID, pData, iLength, s_lpszName);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnChunkHeader(HP_HttpClient pSender, CONNID dwConnID, int iLength)
{
	::PostOnChunkHeader(dwConnID, iLength, s_lpszName);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnChunkComplete(HP_HttpClient pSender, CONNID dwConnID)
{
	::PostOnChunkComplete(dwConnID, s_lpszName);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnMessageComplete(HP_HttpClient pSender, CONNID dwConnID)
{
	::PostOnMessageComplete(dwConnID, s_lpszName);

	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnUpgrade(HP_HttpClient pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
{
	::PostOnUpgrade(dwConnID, enUpgradeType, s_lpszName);

	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnParseError(HP_HttpClient pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
{
	::PostOnParseError(dwConnID, iErrorCode, lpszErrorDesc, s_lpszName);
	return HPR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHandleResult __HP_CALL OnWSMessageHeader(HP_HttpClient pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
{
	::PostOnWSMessageHeader(dwConnID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen, s_lpszName);

	return HR_OK;
}

EnHandleResult __HP_CALL OnWSMessageBody(HP_HttpClient pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnWSMessageBody(dwConnID, pData, iLength, s_lpszName);

	return HR_OK;
}

EnHandleResult __HP_CALL OnWSMessageComplete(HP_HttpClient pSender, CONNID dwConnID)
{
	::PostOnWSMessageComplete(dwConnID, s_lpszName);

	BYTE iOperationCode;

	VERIFY(::HP_HttpClient_GetWSMessageState(pSender, nullptr, nullptr, &iOperationCode, nullptr, nullptr, nullptr));

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

	CONNID dwConnID = ::HP_Client_GetConnectionID(s_pClient);

	CString strContent;
	strContent.Format(_T("[%s] %s://%s:%u %s"), (LPCTSTR)pParser->m_strMethod, s_lpszName, (LPCTSTR)g_app_arg.remote_addr, s_usPort, (LPCTSTR)pParser->m_strPath);
	::LogSending(dwConnID, strContent, s_lpszName);

	BOOL isOK = FALSE;

	if(!pParser->m_strData.IsEmpty() || pParser->m_strFilePath.IsEmpty())
		isOK = ::HP_HttpClient_SendRequest(s_pClient, pParser->m_strMethod, pParser->m_strPath, szHeaders.get(), iSize, (LPBYTE)(LPCSTR)pParser->m_strData, pParser->m_strData.GetLength());
	else
		isOK = ::HP_HttpClient_SendLocalFile(s_pClient, (LPCSTR)pParser->m_strFilePath, pParser->m_strMethod, pParser->m_strPath, szHeaders.get(), iSize);

	if(!isOK)
		::LogSendFail(dwConnID, ::SYS_GetLastError(), ::SYS_GetLastErrorStr(), s_lpszName);
}

void SendWebSocket(CHttpCommandParser* pParser)
{
	BYTE bCode		= 0x8;
	int iLength		= 0;
	BYTE* pData		= nullptr;
	CONNID dwConnID = ::HP_Client_GetConnectionID(s_pClient);

	if(pParser->m_strData.CompareNoCase(HTTP_WEB_SOCKET_CLOSE_FLAG) != 0)
	{
		bCode	= 0x1;
		iLength	= pParser->m_strData.GetLength();
		pData	= (BYTE*)(LPCSTR)pParser->m_strData;
	}

	CString strContent;
	strContent.Format(_T("[WebSocket] (oc: 0x%X, len: %d)"), bCode, iLength);
	::LogSending(dwConnID, strContent, s_lpszName);

	if(!::HP_HttpClient_SendWSMessage(s_pClient, TRUE, 0, bCode, HTTP_WEB_SOCKET_MASK_KEY, pData, iLength, 0))
		::LogSendFail(dwConnID, ::SYS_GetLastError(), ::SYS_GetLastErrorStr(), s_lpszName);
}

void OnCmdStart(CHttpCommandParser* pParser)
{
	if(::HP_Client_HasStarted(s_pClient))
	{
		::LogClientStartFail(SE_ILLEGAL_STATE, ::HP_GetSocketErrorDesc(SE_ILLEGAL_STATE));
		return;
	}

	s_bHttps	= pParser->m_bHttps;
	s_lpszName	= pParser->m_bHttps ? HTTPS_NAME : HTTP_NAME;
	s_usPort	= pParser->m_bHttps ? g_app_arg.https_port : g_app_arg.http_port;
	s_pClient	= pParser->m_bHttps ? s_https_client : s_http_client;

	if(!::HP_Client_StartWithBindAddress(s_pClient, g_app_arg.remote_addr, s_usPort, g_app_arg.async, g_app_arg.bind_addr))
		::LogClientStartFail(::HP_Client_GetLastError(s_pClient), ::HP_Client_GetLastErrorDesc(s_pClient), s_lpszName);
}

void OnCmdStop(CHttpCommandParser* pParser)
{
	if(::HP_Client_Stop(s_pClient))
		::LogClientStop(s_lpszName);
	else
		::LogClientStopFail(::HP_Client_GetLastError(s_pClient), ::HP_Client_GetLastErrorDesc(s_pClient), s_lpszName);
}

void OnCmdStatus(CHttpCommandParser* pParser)
{
	pParser->PrintStatus(::HP_Client_GetState(s_pClient), s_lpszName);
}

void OnCmdSend(CHttpCommandParser* pParser)
{
	if(!::HP_Client_HasStarted(s_pClient))
	{
		::LogSendFail(::HP_Client_GetConnectionID(s_pClient), SE_ILLEGAL_STATE, ::HP_GetSocketErrorDesc(SE_ILLEGAL_STATE), s_lpszName);
		return;
	}

	::HP_HttpClient_GetUpgradeType(s_pClient) == HUT_WEB_SOCKET ? SendWebSocket(pParser) : SendHttp(pParser);
}

void OnCmdPause(CHttpCommandParser* pParser)
{
	if(::HP_Client_PauseReceive(s_pClient, pParser->m_bFlag))
		::LogPause(::HP_Client_GetConnectionID(s_pClient), pParser->m_bFlag, s_lpszName);
	else
		::LogPauseFail(::HP_Client_GetConnectionID(s_pClient), pParser->m_bFlag, s_lpszName);
}

void DestroyHPSocketObjects()
{
	::Destroy_HP_HttpClient(s_http_client);
	::Destroy_HP_HttpsClient(s_https_client);
	::Destroy_HP_HttpClientListener(s_http_listener);
}

BOOL CreateHPSocketObjects()
{
	s_http_listener	= ::Create_HP_HttpClientListener();
	s_http_client	= ::Create_HP_HttpClient(s_http_listener);
	s_https_client	= ::Create_HP_HttpsClient(s_http_listener);

	if(!::HP_SSLClient_SetupSSLContext(s_https_client, SSL_VM_NONE, g_s_lpszPemCertFile, g_s_lpszPemKeyFile, g_s_lpszKeyPasswod, g_s_lpszCAPemCertFileOrPath))
	{
		::LogClientStartFail(::SYS_GetLastError(), _T("initialize SSL env fail"));

		DestroyHPSocketObjects();
		return FALSE;
	}

	::HP_Set_FN_HttpClient_OnConnect(s_http_listener, OnConnect);
	::HP_Set_FN_HttpClient_OnHandShake(s_http_listener, OnHandShake);
	::HP_Set_FN_HttpClient_OnReceive(s_http_listener, OnReceive);
	::HP_Set_FN_HttpClient_OnSend(s_http_listener, OnSend);
	::HP_Set_FN_HttpClient_OnClose(s_http_listener, OnClose);

	::HP_Set_FN_HttpClient_OnMessageBegin(s_http_listener, OnMessageBegin);
	::HP_Set_FN_HttpClient_OnStatusLine(s_http_listener, OnStatusLine);
	::HP_Set_FN_HttpClient_OnHeader(s_http_listener, OnHeader);
	::HP_Set_FN_HttpClient_OnHeadersComplete(s_http_listener, OnHeadersComplete);
	::HP_Set_FN_HttpClient_OnBody(s_http_listener, OnBody);
	::HP_Set_FN_HttpClient_OnChunkHeader(s_http_listener, OnChunkHeader);
	::HP_Set_FN_HttpClient_OnChunkComplete(s_http_listener, OnChunkComplete);
	::HP_Set_FN_HttpClient_OnMessageComplete(s_http_listener, OnMessageComplete);
	::HP_Set_FN_HttpClient_OnUpgrade(s_http_listener, OnUpgrade);
	::HP_Set_FN_HttpClient_OnParseError(s_http_listener, OnParseError);

	::HP_Set_FN_HttpClient_OnWSMessageHeader(s_http_listener, OnWSMessageHeader);
	::HP_Set_FN_HttpClient_OnWSMessageBody(s_http_listener, OnWSMessageBody);
	::HP_Set_FN_HttpClient_OnWSMessageComplete(s_http_listener, OnWSMessageComplete);

	::HP_HttpClient_SetUseCookie(s_http_client, g_app_arg.http_use_cookie);
	::HP_HttpClient_SetUseCookie(s_https_client, g_app_arg.http_use_cookie);
	::HP_TcpClient_SetKeepAliveTime(s_http_client, g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);
	::HP_TcpClient_SetKeepAliveTime(s_https_client, g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);

	s_usPort  = g_app_arg.http_port;
	s_pClient = s_http_client;

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
	fnCmds[CCommandParser::CT_SEND]		= (CCommandParser::CMD_FUNC)OnCmdSend;
	fnCmds[CCommandParser::CT_PAUSE]	= (CCommandParser::CMD_FUNC)OnCmdPause;

	CHttpCommandParser s_cmd_parser(CCommandParser::AT_CLIENT, fnCmds);
	s_cmd_parser.Run();

	DestroyHPSocketObjects();

	if(g_app_arg.http_use_cookie)
		::HP_HttpCookie_MGR_SaveToFile(g_lpszDefaultCookieFile, TRUE);

	return EXIT_CODE_OK;
}
