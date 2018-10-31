#include "../../global/helper.h"
#include "../../../src/HttpClient.h"

class CHttpClientListenerImpl : public CHttpClientListener
{
private:
	virtual EnHandleResult OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult OnConnect(ITcpClient* pSender, CONNID dwConnID);
	virtual EnHandleResult OnHandShake(ITcpClient* pSender, CONNID dwConnID);

	virtual EnHttpParseResult OnMessageBegin(IHttpClient* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnStatusLine(IHttpClient* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc);
	virtual EnHttpParseResult OnHeader(IHttpClient* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue);
	virtual EnHttpParseResult OnHeadersComplete(IHttpClient* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnBody(IHttpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHttpParseResult OnChunkHeader(IHttpClient* pSender, CONNID dwConnID, int iLength);
	virtual EnHttpParseResult OnChunkComplete(IHttpClient* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnMessageComplete(IHttpClient* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnUpgrade(IHttpClient* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType);
	virtual EnHttpParseResult OnParseError(IHttpClient* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc);

	virtual EnHandleResult OnWSMessageHeader(IHttpClient* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen);
	virtual EnHandleResult OnWSMessageBody(IHttpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnWSMessageComplete(IHttpClient* pSender, CONNID dwConnID);

private:
	CStringA GetHeaderSummary(IHttpClient* pHttpClient, LPCSTR lpszSep = "  ", int iSepCount = 0, BOOL bWithContentLength = TRUE);

public:
	CHttpClientListenerImpl(LPCTSTR lpszName)
		: m_strName(lpszName)
	{
	}

public:
	CString m_strName;

};

// ------------------------------------------------------------------------------------------------------------- //

EnHandleResult CHttpClientListenerImpl::OnConnect(ITcpClient* pSender, CONNID dwConnID)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	pSender->GetRemoteHost(szAddress, iAddressLen, usPort);
	::PostOnConnect2(dwConnID, szAddress, usPort, m_strName);

	return HR_OK;
}

EnHandleResult CHttpClientListenerImpl::OnHandShake(ITcpClient* pSender, CONNID dwConnID)
{
	::PostOnHandShake(dwConnID, m_strName);

	return HR_OK;
}

EnHandleResult CHttpClientListenerImpl::OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength, m_strName);
	return HR_OK;
}

EnHandleResult CHttpClientListenerImpl::OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnReceive(dwConnID, pData, iLength, m_strName);
	return HR_OK;
}

EnHandleResult CHttpClientListenerImpl::OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID, m_strName)	:
	::PostOnError(dwConnID, enOperation, iErrorCode, m_strName)	;

	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHttpParseResult CHttpClientListenerImpl::OnMessageBegin(IHttpClient* pSender, CONNID dwConnID)
{
	::PostOnMessageBegin(dwConnID, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpClientListenerImpl::OnStatusLine(IHttpClient* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)
{
	::PostOnStatusLine(dwConnID, usStatusCode, lpszDesc, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpClientListenerImpl::OnHeader(IHttpClient* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
{
	::PostOnHeader(dwConnID, lpszName, lpszValue, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpClientListenerImpl::OnHeadersComplete(IHttpClient* pSender, CONNID dwConnID)
{
	CStringA strSummary = GetHeaderSummary(pSender, "    ", 0, TRUE);
	::PostOnHeadersComplete(dwConnID, strSummary, m_strName);

	return HPR_OK;
}

EnHttpParseResult CHttpClientListenerImpl::OnBody(IHttpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnBody(dwConnID, pData, iLength, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpClientListenerImpl::OnChunkHeader(IHttpClient* pSender, CONNID dwConnID, int iLength)
{
	::PostOnChunkHeader(dwConnID, iLength, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpClientListenerImpl::OnChunkComplete(IHttpClient* pSender, CONNID dwConnID)
{
	::PostOnChunkComplete(dwConnID, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpClientListenerImpl::OnMessageComplete(IHttpClient* pSender, CONNID dwConnID)
{
	::PostOnMessageComplete(dwConnID, m_strName);

	return HPR_OK;
}

EnHttpParseResult CHttpClientListenerImpl::OnUpgrade(IHttpClient* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
{
	::PostOnUpgrade(dwConnID, enUpgradeType, m_strName);

	return HPR_OK;
}

EnHttpParseResult CHttpClientListenerImpl::OnParseError(IHttpClient* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
{
	::PostOnParseError(dwConnID, iErrorCode, lpszErrorDesc, m_strName);
	return HPR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHandleResult CHttpClientListenerImpl::OnWSMessageHeader(IHttpClient* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
{
	::PostOnWSMessageHeader(dwConnID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen, m_strName);

	return HR_OK;
}

EnHandleResult CHttpClientListenerImpl::OnWSMessageBody(IHttpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnWSMessageBody(dwConnID, pData, iLength, m_strName);

	return HR_OK;
}

EnHandleResult CHttpClientListenerImpl::OnWSMessageComplete(IHttpClient* pSender, CONNID dwConnID)
{
	::PostOnWSMessageComplete(dwConnID, m_strName);

	/*
	BYTE iOperationCode;

	VERIFY(pSender->GetWSMessageState(nullptr, nullptr, &iOperationCode, nullptr, nullptr, nullptr));

	if(iOperationCode == 0x8)
		return HR_ERROR;
	*/

	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

CStringA CHttpClientListenerImpl::GetHeaderSummary(IHttpClient* pHttpClient, LPCSTR lpszSep, int iSepCount, BOOL bWithContentLength)
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
	strResult.AppendFormat("%s%13s: %u%s", SEP2, "Status Code", pHttpClient->GetStatusCode(), CRLF);

	DWORD dwHeaderCount = 0;
	pHttpClient->GetAllHeaders(nullptr, dwHeaderCount);

	strResult.AppendFormat("%s[Response Headers]%s", SEP1, CRLF);

	if(dwHeaderCount == 0)
		strResult.AppendFormat("%s(no header)%s", SEP2, CRLF);
	else
	{
		unique_ptr<THeader[]> headers(new THeader[dwHeaderCount]);
		VERIFY(pHttpClient->GetAllHeaders(headers.get(), dwHeaderCount));

		for(DWORD i = 0; i < dwHeaderCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, headers[i].name, headers[i].value, CRLF);
	}

	DWORD dwCookieCount = 0;
	pHttpClient->GetAllCookies(nullptr, dwCookieCount);

	strResult.AppendFormat("%s[Cookies]%s", SEP1, CRLF);

	if(dwCookieCount == 0)
		strResult.AppendFormat("%s(no cookie)%s", SEP2, CRLF);
	else
	{
		unique_ptr<TCookie[]> cookies(new TCookie[dwCookieCount]);
		VERIFY(pHttpClient->GetAllCookies(cookies.get(), dwCookieCount));

		for(DWORD i = 0; i < dwCookieCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, cookies[i].name, cookies[i].value, CRLF);
	}

	CStringA strVersion;
	::HttpVersionToString((EnHttpVersion)pHttpClient->GetVersion(), strVersion);
	EnHttpUpgradeType enUpgType	= pHttpClient->GetUpgradeType();
	LPCSTR lpszUpgrade			= enUpgType != HUT_NONE ? "true" : "false";
	LPCSTR lpszKeepAlive		= pHttpClient->IsKeepAlive() ? "true" : "false";

	strResult.AppendFormat("%s[Basic Info]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "Version", (LPCSTR)strVersion, CRLF);
	strResult.AppendFormat("%s%13s: %u%s", SEP2, "Status Code", pHttpClient->GetStatusCode(), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsUpgrade", lpszUpgrade, CRLF);
	if(enUpgType != HUT_NONE)
		strResult.AppendFormat("%s%13s: %d%s", SEP2, "UpgradeType", enUpgType, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsKeepAlive", lpszKeepAlive, CRLF);
	if(bWithContentLength)
		strResult.AppendFormat("%s%13s: %lld%s", SEP2, "ContentLength", pHttpClient->GetContentLength(), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "ContentType", pHttpClient->GetContentType(), CRLF);
 
	return strResult;
}

// ------------------------------------------------------------------------------------------------------------- //

CHttpClientListenerImpl s_listener_1(HTTP_NAME);
CHttpClientListenerImpl s_listener_2(HTTPS_NAME);
CHttpSyncClient s_http_client(&s_listener_1);
CHttpsSyncClient s_https_client(&s_listener_2);

BOOL s_bHttps				= FALSE;
LPCTSTR s_lpszName			= HTTP_NAME;
USHORT s_usPort				= 0;
IHttpSyncClient* s_pClient	= (IHttpSyncClient*)&s_http_client;

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

	CONNID dwConnID = s_pClient->GetConnectionID();

	CString strContent;
	strContent.Format(_T("[%s] %s://%s:%u %s"), (LPCTSTR)pParser->m_strMethod, s_lpszName, (LPCTSTR)g_app_arg.remote_addr, s_usPort, (LPCTSTR)pParser->m_strPath);
	::LogSending(dwConnID, strContent, s_lpszName);

	BOOL isOK = FALSE;

	if(!pParser->m_strData.IsEmpty() || pParser->m_strFilePath.IsEmpty())
		isOK = s_pClient->SendRequest(pParser->m_strMethod, pParser->m_strPath, szHeaders.get(), iSize, (LPBYTE)(LPCSTR)pParser->m_strData, pParser->m_strData.GetLength());
	else
		isOK = s_pClient->SendLocalFile((LPCSTR)pParser->m_strFilePath, pParser->m_strMethod, pParser->m_strPath, szHeaders.get(), iSize);
	
	if(!isOK)
	{
		::LogSendFail(dwConnID, ::GetLastError(), ::GetLastErrorStr(), s_lpszName);
		return;
	}

	LPCBYTE pData	= nullptr;
	int iLength		= 0;

	s_pClient->GetResponseBody(&pData, &iLength);

	if(iLength > 0)
	{
		LPCSTR lpszEnc = s_pClient->GetContentEncoding();

		if(lpszEnc && ::StrStr(lpszEnc, "gzip"))
		{
			int rs		= 0;
			DWORD dwLen	= ::GZipGuessUncompressBound(pData, iLength);

			if(dwLen == 0 || dwLen > 5 * 1024 * 1024)
				rs = -10;
			else
			{
				CBufferPtr szBuff(dwLen);
				rs = ::GZipUncompress(pData, iLength, szBuff, dwLen);
			}

			if(rs == 0)
				::PostUncompressBody(dwConnID, dwLen, s_lpszName);
			else
			{
				::PostUncompressBodyFail(dwConnID, rs, s_lpszName);

				s_pClient->Stop();
			}
		}
	}
}

void SendWebSocket(CHttpCommandParser* pParser)
{
	BYTE bCode		= 0x8;
	int iLength		= 0;
	BYTE* pData		= nullptr;
	CONNID dwConnID = s_pClient->GetConnectionID();

	if(pParser->m_strData.CompareNoCase(HTTP_WEB_SOCKET_CLOSE_FLAG) != 0)
	{
		bCode	= 0x1;
		iLength	= pParser->m_strData.GetLength();
		pData	= (BYTE*)(LPCSTR)pParser->m_strData;
	}

	CString strContent;
	strContent.Format(_T("[WebSocket] (oc: 0x%X, len: %d)"), bCode, iLength);
	::LogSending(dwConnID, strContent, s_lpszName);

	if(!s_pClient->SendWSMessage(TRUE, 0, bCode, HTTP_WEB_SOCKET_MASK_KEY, pData, iLength))
	{
		::LogSendFail(dwConnID, ::GetLastError(), ::GetLastErrorStr(), s_lpszName);
		return;
	}

	if(bCode == 0x8)
		return;

	BOOL bFinal;
	BYTE iReserved;
	BYTE iOperationCode;
	LPCBYTE lpszMask;
	ULONGLONG ullBodyLen;

	VERIFY(s_pClient->GetWSMessageState(&bFinal, &iReserved, &iOperationCode, &lpszMask, &ullBodyLen, nullptr));

	if(ullBodyLen > 0)
	{
		BYTE* pData;
		int iLength;

		s_pClient->GetResponseBody((LPCBYTE*)&pData, &iLength);
	}

	if(iOperationCode == 0x8)
	{
		s_pClient->SendWSMessage(TRUE, 0, 0x8, HTTP_WEB_SOCKET_MASK_KEY, nullptr, 0);
		s_pClient->Stop();
	}
}

void OnCmdStart(CHttpCommandParser* pParser)
{
	if(s_pClient->HasStarted())
	{
		::LogClientStartFail(SE_ILLEGAL_STATE, ::GetSocketErrorDesc(SE_ILLEGAL_STATE));
		return;
	}

	s_bHttps	= pParser->m_bHttps;
	s_lpszName	= pParser->m_bHttps ? HTTPS_NAME : HTTP_NAME;
	s_usPort	= pParser->m_bHttps ? g_app_arg.https_port : g_app_arg.http_port;
	s_pClient	= pParser->m_bHttps ? (IHttpSyncClient*)&s_https_client : (IHttpSyncClient*)&s_http_client;

	if(!s_pClient->Start(g_app_arg.remote_addr, s_usPort, g_app_arg.async, g_app_arg.bind_addr))
		::LogClientStartFail(s_pClient->GetLastError(), s_pClient->GetLastErrorDesc(), s_lpszName);
}

void OnCmdStop(CHttpCommandParser* pParser)
{
	if(s_pClient->Stop())
		::LogClientStop(s_lpszName);
	else
		::LogClientStopFail(s_pClient->GetLastError(), s_pClient->GetLastErrorDesc(), s_lpszName);
}

void OnCmdStatus(CHttpCommandParser* pParser)
{
	pParser->PrintStatus(s_pClient->GetState(), s_lpszName);
}

void OnCmdSend(CHttpCommandParser* pParser)
{
	if(!s_pClient->HasStarted())
	{
		::LogSendFail(s_pClient->GetConnectionID(), SE_ILLEGAL_STATE, ::GetSocketErrorDesc(SE_ILLEGAL_STATE), s_lpszName);
		return;
	}

	s_pClient->GetUpgradeType() == HUT_WEB_SOCKET ? SendWebSocket(pParser) : SendHttp(pParser);
	s_pClient->CleanupRequestResult();
}

void OnCmdOpenUrl(CHttpCommandParser* pParser)
{
	LPCTSTR lpszName		 = pParser->m_bHttps ? HTTPS_NAME : HTTP_NAME;
	IHttpSyncClient* pClient = pParser->m_bHttps ? (IHttpSyncClient*)&s_https_client : (IHttpSyncClient*)&s_http_client;

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
	::LogSending(pClient->GetConnectionID(), strContent, lpszName);

	LPCBYTE pBody	= (LPCBYTE)(LPCSTR)pParser->m_strData;
	int iLength		= pParser->m_strData.GetLength();

	if(pParser->m_strData.IsEmpty() && !pParser->m_strFilePath.IsEmpty())
	{
		pBody	= (LPCBYTE)(LPCSTR)pParser->m_strFilePath;
		iLength	= -1;
	}

	if(!pClient->OpenUrl(pParser->m_strMethod, pParser->m_strPath, szHeaders.get(), iSize, pBody, iLength, pParser->m_bFlag))
	{
		::LogSendFail(pClient->GetConnectionID(), ::GetLastError(), ::GetLastErrorStr(), lpszName);
		return;
	}

	pBody	= nullptr;
	iLength	= 0;

	pClient->GetResponseBody(&pBody, &iLength);

	if(iLength > 0)
	{
		LPCSTR lpszEnc = pClient->GetContentEncoding();

		if(lpszEnc && ::StrStr(lpszEnc, "gzip"))
		{
			int rs		= 0;
			DWORD dwLen	= ::GZipGuessUncompressBound(pBody, iLength);

			if(dwLen == 0 || dwLen > 5 * 1024 * 1024)
				rs = -10;
			else
			{
				CBufferPtr szBuff(dwLen);
				rs = ::GZipUncompress(pBody, iLength, szBuff, dwLen);
			}

			if(rs == 0)
				::PostUncompressBody(pClient->GetConnectionID(), dwLen, lpszName);
			else
			{
				::PostUncompressBodyFail(pClient->GetConnectionID(), rs, lpszName);

				pClient->Stop();
			}
		}
	}
}

void OnCmdPause(CHttpCommandParser* pParser)
{
	if(s_pClient->PauseReceive(pParser->m_bFlag))
		::LogPause(s_pClient->GetConnectionID(), pParser->m_bFlag, s_lpszName);
	else
		::LogPauseFail(s_pClient->GetConnectionID(), pParser->m_bFlag, s_lpszName);
}

BOOL InitHttpClients()
{
	if(!s_https_client.SetupSSLContext(SSL_VM_NONE, g_c_lpszPemCertFile, g_c_lpszPemKeyFile, g_c_lpszKeyPasswod, g_c_lpszCAPemCertFileOrPath))
	{
		::LogClientStartFail(::GetLastError(), _T("initialize SSL env fail"), s_lpszName);
		return FALSE;
	}

	s_usPort = g_app_arg.http_port;
	s_http_client.SetUseCookie(g_app_arg.http_use_cookie);
	s_https_client.SetUseCookie(g_app_arg.http_use_cookie);
	s_http_client.SetKeepAliveTime(g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);
	s_https_client.SetKeepAliveTime(g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);

	return TRUE;
}

int main(int argc, char* const argv[])
{
	CTermAttrInitializer term_attr;
	CAppSignalHandler s_signal_handler({SIGTTOU, SIGINT});

	g_app_arg.ParseArgs(argc, argv);

	if(g_app_arg.http_use_cookie)
		g_CookieMgr.LoadFromFile(g_lpszDefaultCookieFile, FALSE);

	if(!InitHttpClients())
		return EXIT_CODE_CONFIG;
	
	CCommandParser::CMD_FUNC fnCmds[CCommandParser::CT_MAX] = {0};

	fnCmds[CCommandParser::CT_START]	= (CCommandParser::CMD_FUNC)OnCmdStart;
	fnCmds[CCommandParser::CT_STOP]		= (CCommandParser::CMD_FUNC)OnCmdStop;
	fnCmds[CCommandParser::CT_STATUS]	= (CCommandParser::CMD_FUNC)OnCmdStatus;
	fnCmds[CCommandParser::CT_SEND]		= (CCommandParser::CMD_FUNC)OnCmdSend;
	fnCmds[CCommandParser::CT_CONNECT]	= (CCommandParser::CMD_FUNC)OnCmdOpenUrl;
	fnCmds[CCommandParser::CT_PAUSE]	= (CCommandParser::CMD_FUNC)OnCmdPause;

	CHttpCommandParser s_cmd_parser(CCommandParser::AT_CLIENT, fnCmds);
	s_cmd_parser.Run();

	if(g_app_arg.http_use_cookie)
		g_CookieMgr.SaveToFile(g_lpszDefaultCookieFile, TRUE);

	return EXIT_CODE_OK;
}
