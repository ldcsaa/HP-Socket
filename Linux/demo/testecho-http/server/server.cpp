#include "../../global/helper.h"
#include "../../../src/HttpServer.h"

class CHttpServerListenerImpl : public CHttpServerListener
{
private:
	virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen);
	virtual EnHandleResult OnAccept(ITcpServer* pSender, CONNID dwConnID, UINT_PTR soClient);
	virtual EnHandleResult OnHandShake(ITcpServer* pSender, CONNID dwConnID);
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult OnShutdown(ITcpServer* pSender);

	virtual EnHttpParseResult OnMessageBegin(IHttpServer* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnRequestLine(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl);
	virtual EnHttpParseResult OnHeader(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue);
	virtual EnHttpParseResult OnHeadersComplete(IHttpServer* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnBody(IHttpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHttpParseResult OnChunkHeader(IHttpServer* pSender, CONNID dwConnID, int iLength);
	virtual EnHttpParseResult OnChunkComplete(IHttpServer* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnMessageComplete(IHttpServer* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnUpgrade(IHttpServer* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType);
	virtual EnHttpParseResult OnParseError(IHttpServer* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc);

	virtual EnHandleResult OnWSMessageHeader(IHttpServer* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen);
	virtual EnHandleResult OnWSMessageBody(IHttpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnWSMessageComplete(IHttpServer* pSender, CONNID dwConnID);

private:
	CStringA GetHeaderSummary(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszSep = "  ", int iSepCount = 0, BOOL bWithContentLength = TRUE);

public:
	CHttpServerListenerImpl(LPCTSTR lpszName)
	: m_strName	(lpszName)
	{
	}

public:
	CString m_strName;
};

// ------------------------------------------------------------------------------------------------------------- //

EnHandleResult CHttpServerListenerImpl::OnPrepareListen(ITcpServer* pSender, SOCKET soListen)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;
	
	pSender->GetListenAddress(szAddress, iAddressLen, usPort);
	::PostOnPrepareListen(szAddress, usPort, m_strName);

	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnAccept(ITcpServer* pSender, CONNID dwConnID, UINT_PTR soClient)
{
	BOOL bPass = TRUE;
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	pSender->GetRemoteAddress(dwConnID, szAddress, iAddressLen, usPort);

	::PostOnAccept(dwConnID, szAddress, usPort, bPass, m_strName);

	return bPass ? HR_OK : HR_ERROR;
}

EnHandleResult CHttpServerListenerImpl::OnHandShake(ITcpServer* pSender, CONNID dwConnID)
{
	::PostOnHandShake(dwConnID, m_strName);
	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnReceive(dwConnID, pData, iLength, m_strName);

	if(pSender->Send(dwConnID, pData, iLength))
		return HR_OK;
	else
		return HR_ERROR;
}

EnHandleResult CHttpServerListenerImpl::OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength, m_strName);
	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID, m_strName)	:
	::PostOnError(dwConnID, enOperation, iErrorCode, m_strName)	;

	CBufferPtr* pBuffer = nullptr;
	pSender->GetConnectionExtra(dwConnID, (PVOID*)&pBuffer);

	if(pBuffer) delete pBuffer;

	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnShutdown(ITcpServer* pSender)
{
	::PostOnShutdown(m_strName);
	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHttpParseResult CHttpServerListenerImpl::OnMessageBegin(IHttpServer* pSender, CONNID dwConnID)
{
	::PostOnMessageBegin(dwConnID, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnRequestLine(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)
{
	::PostOnRequestLine(dwConnID, lpszMethod, pSender->GetUrlFieldSet(dwConnID), lpszUrl, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnHeader(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
{
	::PostOnHeader(dwConnID, lpszName, lpszValue, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnHeadersComplete(IHttpServer* pSender, CONNID dwConnID)
{
	CStringA strSummary = GetHeaderSummary(pSender, dwConnID, "    ", 0, TRUE);
	::PostOnHeadersComplete(dwConnID, strSummary, m_strName);

	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnBody(IHttpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnBody(dwConnID, pData, iLength, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnChunkHeader(IHttpServer* pSender, CONNID dwConnID, int iLength)
{
	::PostOnChunkHeader(dwConnID, iLength, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnChunkComplete(IHttpServer* pSender, CONNID dwConnID)
{
	::PostOnChunkComplete(dwConnID, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnMessageComplete(IHttpServer* pSender, CONNID dwConnID)
{
	::PostOnMessageComplete(dwConnID, m_strName);

	if(pSender->IsUpgrade(dwConnID))
		return HPR_OK;

	CStringA strBody = GetHeaderSummary(pSender, dwConnID, "    ", 0, FALSE);
	int iBodyLength  = strBody.GetLength();
	BOOL bSkipBody	 = FALSE;

	if(strcmp(pSender->GetMethod(dwConnID), HTTP_METHOD_HEAD) == 0)
		bSkipBody = TRUE;

	CStringA strContentLength;
	strContentLength.Format("%u", iBodyLength);

	DWORD dwSeq				= 1;
	LPCSTR lpszReqSequence	= nullptr;
	CStringA strSeq;

	if(pSender->GetCookie(dwConnID, "__reqSequence_1", &lpszReqSequence))
		dwSeq += atoi(lpszReqSequence);

	strSeq.Format("%u", dwSeq);

	CStringA strSeqCookie1 = CCookie::ToString("__reqSequence_1", strSeq, nullptr, nullptr, -1, TRUE, TRUE, CCookie::SS_LAX);

	dwSeq			= 1;
	lpszReqSequence	= nullptr;

	if(pSender->GetCookie(dwConnID, "__reqSequence_2", &lpszReqSequence))
		dwSeq += atoi(lpszReqSequence);

	strSeq.Format("%u", dwSeq);

	CStringA strSeqCookie2 = CCookie::ToString("__reqSequence_2", strSeq, nullptr, "/", 300, FALSE, FALSE, CCookie::SS_NONE);

	THeader header[] = {{"Content-Type", "text/plain"}, {"Content-Length", strContentLength}, {"Set-Cookie", strSeqCookie1}, {"Set-Cookie", strSeqCookie2}};
	int iHeaderCount = sizeof(header) / sizeof(THeader);

	if(bSkipBody)
	{
		strBody.Empty();
		iBodyLength = 0;
	}

	pSender->SendResponse(	dwConnID,
							HSC_OK,
							"HP Http Server OK",
							header, iHeaderCount,
							(const BYTE*)(LPCSTR)strBody,
							iBodyLength);

	if(!pSender->IsKeepAlive(dwConnID))
		pSender->Release(dwConnID);

	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnUpgrade(IHttpServer* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
{
	::PostOnUpgrade(dwConnID, enUpgradeType, m_strName);

	if(enUpgradeType == HUT_HTTP_TUNNEL)
	{
		pSender->SendResponse(dwConnID, HSC_OK, "Connection Established");
	}
	else if(enUpgradeType == HUT_WEB_SOCKET)
	{
		int iHeaderCount = 2;
		THeader header[] = {{"Connection", UPGRADE_HEADER},
							{UPGRADE_HEADER, WEB_SOCKET_HEADER_VALUE},
							{nullptr, nullptr},
							{nullptr, nullptr}};


		LPCSTR lpszAccept = nullptr;
		
		if(!pSender->GetHeader(dwConnID, "Sec-WebSocket-Key", &lpszAccept))
			return HPR_ERROR;

		CStringA strAccept;
		::MakeSecWebSocketAccept(lpszAccept, strAccept);

		header[2].name	= "Sec-WebSocket-Accept";
		header[2].value	= strAccept;
		++iHeaderCount;

		CStringA strFirst;
		LPCSTR lpszProtocol = nullptr;

		if(pSender->GetHeader(dwConnID, "Sec-WebSocket-Protocol", &lpszProtocol))
		{
			int i = 0;
			CStringA strProtocol(lpszProtocol);
			strFirst = strProtocol.Tokenize(", ", i);

			if(!strFirst.IsEmpty())
			{
				header[3].name	= "Sec-WebSocket-Protocol";
				header[3].value	= strFirst;
				++iHeaderCount;
			}
		}

		pSender->SendResponse(dwConnID, HSC_SWITCHING_PROTOCOLS, nullptr, header, iHeaderCount);
		pSender->SetConnectionExtra(dwConnID, new CBufferPtr);
	}
	else
		ASSERT(FALSE);

	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnParseError(IHttpServer* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
{
	::PostOnParseError(dwConnID, iErrorCode, lpszErrorDesc, m_strName);
	return HPR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHandleResult CHttpServerListenerImpl::OnWSMessageHeader(IHttpServer* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
{
	::PostOnWSMessageHeader(dwConnID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen, m_strName);

	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnWSMessageBody(IHttpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnWSMessageBody(dwConnID, pData, iLength, m_strName);

	CBufferPtr* pBuffer = nullptr;
	pSender->GetConnectionExtra(dwConnID, (PVOID*)&pBuffer);
	VERIFY(pBuffer);

	pBuffer->Cat(pData, iLength);

	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnWSMessageComplete(IHttpServer* pSender, CONNID dwConnID)
{
	::PostOnWSMessageComplete(dwConnID, m_strName);

	CBufferPtr* pBuffer = nullptr;
	pSender->GetConnectionExtra(dwConnID, (PVOID*)&pBuffer);
	VERIFY(pBuffer);

	BOOL bFinal;
	BYTE iReserved, iOperationCode;

	VERIFY(pSender->GetWSMessageState(dwConnID, &bFinal, &iReserved, &iOperationCode, nullptr, nullptr, nullptr));

	pSender->SendWSMessage(dwConnID, bFinal, iReserved, iOperationCode, pBuffer->Ptr(), (int)pBuffer->Size());
	pBuffer->Free();

	if(iOperationCode == 0x8)
		pSender->Disconnect(dwConnID);

	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

CStringA CHttpServerListenerImpl::GetHeaderSummary(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszSep, int iSepCount, BOOL bWithContentLength)
{
	CStringA strSEP1;

	for(int i = 0; i < iSepCount; i++)
		strSEP1 += lpszSep;

	CStringA strSEP2(strSEP1);
	strSEP2 += lpszSep;

	LPCSTR SEP1 = (LPCSTR)strSEP1;
	LPCSTR SEP2 = (LPCSTR)strSEP2;

	CStringA strResult;

	//USHORT usUrlFieldSet = pSender->GetUrlFieldSet(dwConnID);

	strResult.AppendFormat("%s[URL Fields]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "SCHEMA", pSender->GetUrlField(dwConnID, HUF_SCHEMA), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "HOST", pSender->GetUrlField(dwConnID, HUF_HOST), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "PORT", pSender->GetUrlField(dwConnID, HUF_PORT), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "PATH", pSender->GetUrlField(dwConnID, HUF_PATH), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "QUERY", pSender->GetUrlField(dwConnID, HUF_QUERY), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "FRAGMENT", pSender->GetUrlField(dwConnID, HUF_FRAGMENT), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "USERINFO", pSender->GetUrlField(dwConnID, HUF_USERINFO), CRLF);	

	DWORD dwHeaderCount = 0;
	pSender->GetAllHeaders(dwConnID, nullptr, dwHeaderCount);

	strResult.AppendFormat("%s[Request Headers]%s", SEP1, CRLF);

	if(dwHeaderCount == 0)
		strResult.AppendFormat("%s(no header)%s", SEP2, CRLF);
	else
	{
		unique_ptr<THeader[]> headers(new THeader[dwHeaderCount]);
		VERIFY(pSender->GetAllHeaders(dwConnID, headers.get(), dwHeaderCount));

		for(DWORD i = 0; i < dwHeaderCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, headers[i].name, headers[i].value, CRLF);
	}

	DWORD dwCookieCount = 0;
	pSender->GetAllCookies(dwConnID, nullptr, dwCookieCount);

	strResult.AppendFormat("%s[Cookies]%s", SEP1, CRLF);

	if(dwCookieCount == 0)
		strResult.AppendFormat("%s(no cookie)%s", SEP2, CRLF);
	else
	{
		unique_ptr<TCookie[]> cookies(new TCookie[dwCookieCount]);
		VERIFY(pSender->GetAllCookies(dwConnID, cookies.get(), dwCookieCount));

		for(DWORD i = 0; i < dwCookieCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, cookies[i].name, cookies[i].value, CRLF);
	}

	CStringA strVersion;
	::HttpVersionToString((EnHttpVersion)pSender->GetVersion(dwConnID), strVersion);
	EnHttpUpgradeType enUpgType	= pSender->GetUpgradeType(dwConnID);
	LPCSTR lpszUpgrade			= enUpgType != HUT_NONE ? "true" : "false";
	LPCSTR lpszKeepAlive		= pSender->IsKeepAlive(dwConnID) ? "true" : "false";

	strResult.AppendFormat("%s[Basic Info]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "Version", (LPCSTR)strVersion, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "Method", pSender->GetMethod(dwConnID), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsUpgrade", lpszUpgrade, CRLF);
	if(enUpgType != HUT_NONE)
		strResult.AppendFormat("%s%13s: %d%s", SEP2, "UpgradeType", enUpgType, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsKeepAlive", lpszKeepAlive, CRLF);
	if(bWithContentLength)
		strResult.AppendFormat("%s%13s: %lld%s", SEP2, "ContentLength", pSender->GetContentLength(dwConnID), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "ContentType", pSender->GetContentType(dwConnID), CRLF);
 
	return strResult;
}


// ------------------------------------------------------------------------------------------------------------- //

CHttpServerListenerImpl s_listener_1(HTTP_NAME);
CHttpServerListenerImpl s_listener_2(HTTPS_NAME);
CHttpServer s_http_server(&s_listener_1);
CHttpsServer s_https_server(&s_listener_2);

// ------------------------------------------------------------------------------------------------------------- //

const CString SPECIAL_SERVER_NAME	= _T("hpsocket.org");
int SPECIAL_SERVER_INDEX			= -1;

int __HP_CALL SIN_ServerNameCallback(LPCTSTR lpszServerName, PVOID pContext)
{
	if(::IsIPAddress(lpszServerName))
		return 0;

	int len  = lstrlen(lpszServerName);
	int diff = len - SPECIAL_SERVER_NAME.GetLength();

	if(diff < 0)
		return 0;

	if(SPECIAL_SERVER_NAME.CompareNoCase(lpszServerName + diff) != 0)
		return 0;

	return SPECIAL_SERVER_INDEX;
}

void OnCmdStart(CHttpCommandParser* pParser)
{
	if(s_http_server.Start(g_app_arg.bind_addr, g_app_arg.http_port))
	{
		::LogServerStart(g_app_arg.bind_addr, g_app_arg.http_port, HTTP_NAME);

		if(s_https_server.Start(g_app_arg.bind_addr, g_app_arg.https_port))
		{
			::LogServerStart(g_app_arg.bind_addr, g_app_arg.https_port, HTTPS_NAME);
		}
		else
		{
			s_http_server.Stop();

			::LogServerStartFail(s_https_server.GetLastError(), s_https_server.GetLastErrorDesc(), HTTPS_NAME);
		}
	}
	else
	{
		::LogServerStartFail(s_http_server.GetLastError(), s_http_server.GetLastErrorDesc(), HTTP_NAME);
	}
}

void OnCmdStop(CHttpCommandParser* pParser)
{
	if(!s_https_server.Stop())
		ASSERT(FALSE);

	::LogServerStop(HTTPS_NAME);

	if(!s_http_server.Stop())
		ASSERT(FALSE);

	::LogServerStop(HTTP_NAME);
}

void OnCmdStatus(CHttpCommandParser* pParser)
{
	TCHAR szPort[10];

	pParser->PrintStatus(s_http_server.GetState(), itoa(g_app_arg.http_port, szPort, 10));
	pParser->PrintStatus(s_https_server.GetState(), itoa(g_app_arg.https_port, szPort, 10));
}

void OnCmdSend(CHttpCommandParser* pParser)
{
	LPCTSTR lpszName	 = pParser->m_bHttps ? HTTPS_NAME : HTTP_NAME;
	IHttpServer* pServer = pParser->m_bHttps ? (IHttpServer*)&s_https_server : (IHttpServer*)&s_http_server;

	LPVOID pExtra = nullptr;

	if(!pServer->GetConnectionExtra(pParser->m_dwConnID, &pExtra))
	{
		::LogSendFail(pParser->m_dwConnID, ERROR_OBJECT_NOT_FOUND, ::GetErrorStr(ERROR_OBJECT_NOT_FOUND), lpszName);
		return;
	}

	if(pServer->GetUpgradeType(pParser->m_dwConnID) != HUT_WEB_SOCKET)
	{
		::LogSendFail(pParser->m_dwConnID, ERROR_INVALID_STATE, "server can only send WS message, but connection is not in WS mode currently", lpszName);
		return;
	}

	BYTE bCode	= 0x8;
	int iLength = 0;;
	BYTE* pData	= nullptr;

	if(pParser->m_strData.CompareNoCase(HTTP_WEB_SOCKET_CLOSE_FLAG) != 0)
	{
		bCode	= 0x1;
		iLength	= pParser->m_strData.GetLength();
		pData	= (BYTE*)(LPCSTR)pParser->m_strData;
	}

	CString strContent;
	strContent.Format(_T("[WebSocket] (oc: 0x%X, len: %d)"), bCode, iLength);
	::LogSending(pParser->m_dwConnID, strContent, lpszName);

	if(!pServer->SendWSMessage(pParser->m_dwConnID, TRUE, 0, bCode, pData, iLength))
		::LogSendFail(pParser->m_dwConnID, ::GetLastError(), ::GetLastErrorStr(), lpszName);
}

void OnCmdPause(CHttpCommandParser* pParser)
{
	LPCTSTR lpszName	 = pParser->m_bHttps ? HTTPS_NAME : HTTP_NAME;
	IHttpServer* pServer = pParser->m_bHttps ? (IHttpServer*)&s_https_server : (IHttpServer*)&s_http_server;

	if(pServer->PauseReceive(pParser->m_dwConnID, pParser->m_bFlag))
		::LogPause(pParser->m_dwConnID, pParser->m_bFlag, lpszName);
	else
		::LogPauseFail(pParser->m_dwConnID, pParser->m_bFlag, lpszName);
}

void OnCmdKick(CHttpCommandParser* pParser)
{
	LPCTSTR lpszName	 = pParser->m_bHttps ? HTTPS_NAME : HTTP_NAME;
	IHttpServer* pServer = pParser->m_bHttps ? (IHttpServer*)&s_https_server : (IHttpServer*)&s_http_server;

	if(pServer->Release(pParser->m_dwConnID))
		::LogRelease(pParser->m_dwConnID, lpszName);
	else
		::LogReleaseFail(pParser->m_dwConnID, lpszName);
}

void OnCmdKickLong(CHttpCommandParser* pParser)
{
	LPCTSTR lpszName	 = pParser->m_bHttps ? HTTPS_NAME : HTTP_NAME;
	IHttpServer* pServer = pParser->m_bHttps ? (IHttpServer*)&s_https_server : (IHttpServer*)&s_http_server;

	if(pServer->DisconnectLongConnections(pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag, lpszName);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag, lpszName);
}

void OnCmdKickSilence(CHttpCommandParser* pParser)
{
	LPCTSTR lpszName	 = pParser->m_bHttps ? HTTPS_NAME : HTTP_NAME;
	IHttpServer* pServer = pParser->m_bHttps ? (IHttpServer*)&s_https_server : (IHttpServer*)&s_http_server;

	if(pServer->DisconnectSilenceConnections(pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag, lpszName);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag, lpszName);
}

// ------------------------------------------------------------------------------------------------------------- //

int main(int argc, char* const argv[])
{
	CTermAttrInitializer term_attr;
	CAppSignalHandler s_signal_handler({SIGTTOU, SIGINT});

	g_app_arg.ParseArgs(argc, argv);

	/*
	if(s_https_server.SetupSSLContext(SSL_VM_NONE, g_s_lpszPemCertFile, g_s_lpszPemKeyFile, g_s_lpszKeyPasswod, g_s_lpszCAPemCertFileOrPath, SIN_ServerNameCallback))
		SPECIAL_SERVER_INDEX = s_https_server.AddSSLContext(SSL_VM_NONE, g_s_lpszPemCertFile2, g_s_lpszPemKeyFile2, g_s_lpszKeyPasswod2, g_s_lpszCAPemCertFileOrPath2);
	*/
	if(s_https_server.SetupSSLContext(SSL_VM_NONE, g_s_lpszPemCertFile, g_s_lpszPemKeyFile, g_s_lpszKeyPasswod, g_s_lpszCAPemCertFileOrPath))
	{
		int iIndex = s_https_server.AddSSLContext(SSL_VM_NONE, g_s_lpszPemCertFile2, g_s_lpszPemKeyFile2, g_s_lpszKeyPasswod2, g_s_lpszCAPemCertFileOrPath2);
		ASSERT(iIndex > 0);

		s_https_server.BindSSLServerName(SPECIAL_SERVER_NAME, iIndex);
	}
	else
	{
		::LogServerStartFail(::GetLastError(), _T("initialize SSL env fail"));
		return EXIT_CODE_CONFIG;
	}

	s_http_server.SetKeepAliveTime(g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);
	s_https_server.SetKeepAliveTime(g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);

	CCommandParser::CMD_FUNC fnCmds[CCommandParser::CT_MAX] = {0};

	fnCmds[CCommandParser::CT_START]	= (CCommandParser::CMD_FUNC)OnCmdStart;
	fnCmds[CCommandParser::CT_STOP]		= (CCommandParser::CMD_FUNC)OnCmdStop;
	fnCmds[CCommandParser::CT_STATUS]	= (CCommandParser::CMD_FUNC)OnCmdStatus;
	fnCmds[CCommandParser::CT_SEND]		= (CCommandParser::CMD_FUNC)OnCmdSend;
	fnCmds[CCommandParser::CT_PAUSE]	= (CCommandParser::CMD_FUNC)OnCmdPause;
	fnCmds[CCommandParser::CT_KICK]		= (CCommandParser::CMD_FUNC)OnCmdKick;
	fnCmds[CCommandParser::CT_KICK_L]	= (CCommandParser::CMD_FUNC)OnCmdKickLong;
	fnCmds[CCommandParser::CT_KICK_S]	= (CCommandParser::CMD_FUNC)OnCmdKickSilence;

	CHttpCommandParser s_cmd_parser(CCommandParser::AT_SERVER, fnCmds);
	s_cmd_parser.Run();

	return EXIT_CODE_OK;
}
