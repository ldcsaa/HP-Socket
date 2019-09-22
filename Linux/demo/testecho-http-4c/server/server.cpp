#include "../../global/helper.h"

#include <hpsocket/HPSocket4C-SSL.h>

// ------------------------------------------------------------------------------------------------------------- //

HP_HttpServerListener s_http_listener;
HP_HttpServer s_http_server;
HP_HttpsServer s_https_server;

unordered_map<HP_Object, LPCTSTR> s_sender_names;

// ------------------------------------------------------------------------------------------------------------- //

CStringA GetHeaderSummary(HP_HttpServer pSender, CONNID dwConnID, LPCSTR lpszSep, int iSepCount, BOOL bWithContentLength)
{
	CStringA strSEP1;

	for(int i = 0; i < iSepCount; i++)
		strSEP1 += lpszSep;

	CStringA strSEP2(strSEP1);
	strSEP2 += lpszSep;

	LPCSTR SEP1 = (LPCSTR)strSEP1;
	LPCSTR SEP2 = (LPCSTR)strSEP2;

	CStringA strResult;

	//USHORT usUrlFieldSet = ::HP_HttpServer_GetUrlFieldSet(pSender, dwConnID);

	strResult.AppendFormat("%s[URL Fields]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "SCHEMA", ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_SCHEMA), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "HOST", ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_HOST), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "PORT", ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_PORT), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "PATH", ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_PATH), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "QUERY", ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_QUERY), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "FRAGMENT", ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_FRAGMENT), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "USERINFO", ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_USERINFO), CRLF);	

	DWORD dwHeaderCount = 0;
	::HP_HttpServer_GetAllHeaders(pSender, dwConnID, nullptr, &dwHeaderCount);

	strResult.AppendFormat("%s[Request Headers]%s", SEP1, CRLF);

	if(dwHeaderCount == 0)
		strResult.AppendFormat("%s(no header)%s", SEP2, CRLF);
	else
	{
		unique_ptr<THeader[]> headers(new THeader[dwHeaderCount]);
		VERIFY(::HP_HttpServer_GetAllHeaders(pSender, dwConnID, headers.get(), &dwHeaderCount));

		for(DWORD i = 0; i < dwHeaderCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, headers[i].name, headers[i].value, CRLF);
	}

	DWORD dwCookieCount = 0;
	::HP_HttpServer_GetAllCookies(pSender, dwConnID, nullptr, &dwCookieCount);

	strResult.AppendFormat("%s[Cookies]%s", SEP1, CRLF);

	if(dwCookieCount == 0)
		strResult.AppendFormat("%s(no cookie)%s", SEP2, CRLF);
	else
	{
		unique_ptr<TCookie[]> cookies(new TCookie[dwCookieCount]);
		VERIFY(::HP_HttpServer_GetAllCookies(pSender, dwConnID, cookies.get(), &dwCookieCount));

		for(DWORD i = 0; i < dwCookieCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, cookies[i].name, cookies[i].value, CRLF);
	}

	CStringA strVersion;
	::HttpVersionToString((EnHttpVersion)::HP_HttpServer_GetVersion(pSender, dwConnID), strVersion);
	EnHttpUpgradeType enUpgType	= ::HP_HttpServer_GetUpgradeType(pSender, dwConnID);
	LPCSTR lpszUpgrade			= enUpgType != HUT_NONE ? "true" : "false";
	LPCSTR lpszKeepAlive		= ::HP_HttpServer_IsKeepAlive(pSender, dwConnID) ? "true" : "false";

	strResult.AppendFormat("%s[Basic Info]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "Version", (LPCSTR)strVersion, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "Method", ::HP_HttpServer_GetMethod(pSender, dwConnID), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsUpgrade", lpszUpgrade, CRLF);
	if(enUpgType != HUT_NONE)
		strResult.AppendFormat("%s%13s: %d%s", SEP2, "UpgradeType", enUpgType, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsKeepAlive", lpszKeepAlive, CRLF);
	if(bWithContentLength)
		strResult.AppendFormat("%s%13s: %lld%s", SEP2, "ContentLength", ::HP_HttpServer_GetContentLength(pSender, dwConnID), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "ContentType", ::HP_HttpServer_GetContentType(pSender, dwConnID), CRLF);

	return strResult;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHandleResult __HP_CALL OnPrepareListen(HP_HttpServer pSender, UINT_PTR soListen)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;
	
	::HP_Server_GetListenAddress(pSender, szAddress, &iAddressLen, &usPort);
	::PostOnPrepareListen(szAddress, usPort, s_sender_names[pSender]);

	return HR_OK;
}

EnHandleResult __HP_CALL OnAccept(HP_HttpServer pSender, CONNID dwConnID, UINT_PTR soClient)
{
	BOOL bPass = TRUE;
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	::HP_Server_GetRemoteAddress(pSender, dwConnID, szAddress, &iAddressLen, &usPort);

	::PostOnAccept(dwConnID, szAddress, usPort, bPass, s_sender_names[pSender]);

	return bPass ? HR_OK : HR_ERROR;
}

EnHandleResult __HP_CALL OnHandShake(HP_HttpServer pSender, CONNID dwConnID)
{
	::PostOnHandShake(dwConnID, s_sender_names[pSender]);
	return HR_OK;
}

EnHandleResult __HP_CALL OnReceive(HP_HttpServer pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnReceive(dwConnID, pData, iLength, s_sender_names[pSender]);

	if(::HP_Server_Send(pSender, dwConnID, pData, iLength))
		return HR_OK;
	else
		return HR_ERROR;
}

EnHandleResult __HP_CALL OnSend(HP_HttpServer pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength, s_sender_names[pSender]);
	return HR_OK;
}

EnHandleResult __HP_CALL OnClose(HP_HttpServer pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID, s_sender_names[pSender])		:
	::PostOnError(dwConnID, enOperation, iErrorCode, s_sender_names[pSender])	;

	CBufferPtr* pBuffer = nullptr;
	::HP_Server_GetConnectionExtra(pSender, dwConnID, (PVOID*)&pBuffer);

	if(pBuffer) delete pBuffer;

	return HR_OK;
}

EnHandleResult __HP_CALL OnShutdown(HP_HttpServer pSender)
{
	::PostOnShutdown(s_sender_names[pSender]);
	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHttpParseResult __HP_CALL OnMessageBegin(HP_HttpServer pSender, CONNID dwConnID)
{
	::PostOnMessageBegin(dwConnID, s_sender_names[pSender]);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnRequestLine(HP_HttpServer pSender, CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)
{
	::PostOnRequestLine(dwConnID, lpszMethod, ::HP_HttpServer_GetUrlFieldSet(pSender, dwConnID), lpszUrl, s_sender_names[pSender]);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnHeader(HP_HttpServer pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
{
	::PostOnHeader(dwConnID, lpszName, lpszValue, s_sender_names[pSender]);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnHeadersComplete(HP_HttpServer pSender, CONNID dwConnID)
{
	CStringA strSummary = GetHeaderSummary(pSender, dwConnID, "    ", 0, TRUE);
	::PostOnHeadersComplete(dwConnID, strSummary, s_sender_names[pSender]);

	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnBody(HP_HttpServer pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnBody(dwConnID, pData, iLength, s_sender_names[pSender]);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnChunkHeader(HP_HttpServer pSender, CONNID dwConnID, int iLength)
{
	::PostOnChunkHeader(dwConnID, iLength, s_sender_names[pSender]);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnChunkComplete(HP_HttpServer pSender, CONNID dwConnID)
{
	::PostOnChunkComplete(dwConnID, s_sender_names[pSender]);
	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnMessageComplete(HP_HttpServer pSender, CONNID dwConnID)
{
	::PostOnMessageComplete(dwConnID, s_sender_names[pSender]);

	if(::HP_HttpServer_IsUpgrade(pSender, dwConnID))
		return HPR_OK;

	CStringA strBody = GetHeaderSummary(pSender, dwConnID, "    ", 0, FALSE);
	int iBodyLength  = strBody.GetLength();
	BOOL bSkipBody	 = FALSE;

	if(strcmp(::HP_HttpServer_GetMethod(pSender, dwConnID), HTTP_METHOD_HEAD) == 0)
		bSkipBody = TRUE;

	CStringA strContentLength;
	strContentLength.Format("%u", iBodyLength);

	DWORD dwSeq				= 1;
	int iSize				= MAX_PATH;
	LPCSTR lpszReqSequence	= nullptr;
	CStringA strSeq;

	if(::HP_HttpServer_GetCookie(pSender, dwConnID, "__reqSequence_1", &lpszReqSequence))
		dwSeq += atoi(lpszReqSequence);

	strSeq.Format("%u", dwSeq);

	CStringA strSeqCookie1;
	LPSTR lpszSeqCookie = strSeqCookie1.GetBuffer(iSize);
	VERIFY(::HP_HttpCookie_HLP_ToString(lpszSeqCookie, &iSize, "__reqSequence_1", strSeq, nullptr, nullptr, -1, TRUE, TRUE, 2));
	strSeqCookie1.ReleaseBuffer();

	dwSeq			= 1;
	iSize			= MAX_PATH;
	lpszReqSequence	= nullptr;

	if(::HP_HttpServer_GetCookie(pSender, dwConnID, "__reqSequence_2", &lpszReqSequence))
		dwSeq += atoi(lpszReqSequence);

	strSeq.Format("%u", dwSeq);

	CStringA strSeqCookie2;
	lpszSeqCookie = strSeqCookie2.GetBuffer(iSize);
	VERIFY(::HP_HttpCookie_HLP_ToString(lpszSeqCookie, &iSize, "__reqSequence_2", strSeq, nullptr, "/", 300, FALSE, FALSE, 0));
	strSeqCookie2.ReleaseBuffer();

	THeader header[] = {{"Content-Type", "text/plain"}, {"Content-Length", strContentLength}, {"Set-Cookie", strSeqCookie1}, {"Set-Cookie", strSeqCookie2}};
	int iHeaderCount = sizeof(header) / sizeof(THeader);

	if(bSkipBody)
	{
		strBody.Empty();
		iBodyLength = 0;
	}

	::HP_HttpServer_SendResponse(pSender,
		dwConnID, HSC_OK,
		"HP Http Server OK",
		header, iHeaderCount,
		(const BYTE*)(LPCSTR)strBody,
		iBodyLength);

	if(!::HP_HttpServer_IsKeepAlive(pSender, dwConnID))
		::HP_HttpServer_Release(pSender, dwConnID);

	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnUpgrade(HP_HttpServer pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
{
	::PostOnUpgrade(dwConnID, enUpgradeType, s_sender_names[pSender]);

	if(enUpgradeType == HUT_HTTP_TUNNEL)
	{
		::HP_HttpServer_SendResponse(pSender, dwConnID, HSC_OK, "Connection Established", nullptr, 0, nullptr, 0);
	}
	else if(enUpgradeType == HUT_WEB_SOCKET)
	{
		int iHeaderCount = 2;
		THeader header[] = {{"Connection", UPGRADE_HEADER},
							{UPGRADE_HEADER, WEB_SOCKET_HEADER_VALUE},
							{nullptr, nullptr},
							{nullptr, nullptr}};


		LPCSTR lpszAccept = nullptr;
		
		if(!::HP_HttpServer_GetHeader(pSender, dwConnID, "Sec-WebSocket-Key", &lpszAccept))
			return HPR_ERROR;

		CStringA strAccept;
		::MakeSecWebSocketAccept(lpszAccept, strAccept);

		header[2].name	= "Sec-WebSocket-Accept";
		header[2].value	= strAccept;
		++iHeaderCount;

		CStringA strFirst;
		LPCSTR lpszProtocol = nullptr;

		if(::HP_HttpServer_GetHeader(pSender, dwConnID, "Sec-WebSocket-Protocol", &lpszProtocol))
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

		::HP_HttpServer_SendResponse(pSender, dwConnID, HSC_SWITCHING_PROTOCOLS, nullptr, header, iHeaderCount, nullptr, 0);
		::HP_Server_SetConnectionExtra(pSender, dwConnID, new CBufferPtr);
	}
	else
		ASSERT(FALSE);

	return HPR_OK;
}

EnHttpParseResult __HP_CALL OnParseError(HP_HttpServer pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
{
	::PostOnParseError(dwConnID, iErrorCode, lpszErrorDesc, s_sender_names[pSender]);
	return HPR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHandleResult __HP_CALL OnWSMessageHeader(HP_HttpServer pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
{
	::PostOnWSMessageHeader(dwConnID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen, s_sender_names[pSender]);

	return HR_OK;
}

EnHandleResult __HP_CALL OnWSMessageBody(HP_HttpServer pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnWSMessageBody(dwConnID, pData, iLength, s_sender_names[pSender]);

	CBufferPtr* pBuffer = nullptr;
	::HP_Server_GetConnectionExtra(pSender, dwConnID, (PVOID*)&pBuffer);
	VERIFY(pBuffer);

	pBuffer->Cat(pData, iLength);

	return HR_OK;
}

EnHandleResult __HP_CALL OnWSMessageComplete(HP_HttpServer pSender, CONNID dwConnID)
{
	::PostOnWSMessageComplete(dwConnID, s_sender_names[pSender]);

	CBufferPtr* pBuffer = nullptr;
	::HP_Server_GetConnectionExtra(pSender, dwConnID, (PVOID*)&pBuffer);
	VERIFY(pBuffer);

	BOOL bFinal;
	BYTE iReserved, iOperationCode;

	VERIFY(::HP_HttpServer_GetWSMessageState(pSender, dwConnID, &bFinal, &iReserved, &iOperationCode, nullptr, nullptr, nullptr));

	::HP_HttpServer_SendWSMessage(pSender, dwConnID, bFinal, iReserved, iOperationCode, pBuffer->Ptr(), (int)pBuffer->Size(), 0);
	pBuffer->Free();

	if(iOperationCode == 0x8)
		::HP_Server_Disconnect(pSender, dwConnID, TRUE);

	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

const CString SPECIAL_SERVER_NAME	= _T("hpsocket.org");
int SPECIAL_SERVER_INDEX			= -1;

int __HP_CALL SIN_ServerNameCallback(LPCTSTR lpszServerName, PVOID pContext)
{
	if(::SYS_IsIPAddress(lpszServerName, nullptr))
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
	if(::HP_Server_Start(s_http_server, g_app_arg.bind_addr, g_app_arg.http_port))
	{
		::LogServerStart(g_app_arg.bind_addr, g_app_arg.http_port, HTTP_NAME);

		if(::HP_Server_Start(s_https_server, g_app_arg.bind_addr, g_app_arg.https_port))
		{
			::LogServerStart(g_app_arg.bind_addr, g_app_arg.https_port, HTTPS_NAME);
		}
		else
		{
			::HP_Server_Stop(s_http_server);

			::LogServerStartFail(::HP_Server_GetLastError(s_https_server), ::HP_Server_GetLastErrorDesc(s_https_server), HTTPS_NAME);
		}
	}
	else
	{
		::LogServerStartFail(::HP_Server_GetLastError(s_http_server), ::HP_Server_GetLastErrorDesc(s_http_server), HTTP_NAME);
	}
}

void OnCmdStop(CHttpCommandParser* pParser)
{
	if(!::HP_Server_Stop(s_https_server))
		ASSERT(FALSE);

	::LogServerStop(HTTPS_NAME);

	if(!::HP_Server_Stop(s_http_server))
		ASSERT(FALSE);

	::LogServerStop(HTTP_NAME);
}

void OnCmdStatus(CHttpCommandParser* pParser)
{
	TCHAR szPort[10];

	pParser->PrintStatus(::HP_Server_GetState(s_http_server), itoa(g_app_arg.http_port, szPort, 10));
	pParser->PrintStatus(::HP_Server_GetState(s_https_server), itoa(g_app_arg.https_port, szPort, 10));
}

void OnCmdSend(CHttpCommandParser* pParser)
{
	LPCTSTR lpszName		= pParser->m_bHttps ? HTTPS_NAME : HTTP_NAME;
	HP_HttpServer pServer	= pParser->m_bHttps ? s_https_server : s_http_server;

	LPVOID pExtra = nullptr;

	if(!::HP_Server_GetConnectionExtra(pServer, pParser->m_dwConnID, &pExtra))
	{
		::LogSendFail(pParser->m_dwConnID, ERROR_OBJECT_NOT_FOUND, ::GetErrorStr(ERROR_OBJECT_NOT_FOUND), lpszName);
		return;
	}

	if(::HP_HttpServer_GetUpgradeType(pServer, pParser->m_dwConnID) != HUT_WEB_SOCKET)
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

	if(!::HP_HttpServer_SendWSMessage(pServer, pParser->m_dwConnID, TRUE, 0, bCode, pData, iLength, 0))
		::LogSendFail(pParser->m_dwConnID, ::SYS_GetLastError(), ::SYS_GetLastErrorStr(), lpszName);
}

void OnCmdPause(CHttpCommandParser* pParser)
{
	LPCTSTR lpszName		= pParser->m_bHttps ? HTTPS_NAME : HTTP_NAME;
	HP_HttpServer pServer	= pParser->m_bHttps ? s_https_server : s_http_server;

	if(::HP_Server_PauseReceive(pServer, pParser->m_dwConnID, pParser->m_bFlag))
		::LogPause(pParser->m_dwConnID, pParser->m_bFlag, lpszName);
	else
		::LogPauseFail(pParser->m_dwConnID, pParser->m_bFlag, lpszName);
}

void OnCmdKick(CHttpCommandParser* pParser)
{
	LPCTSTR lpszName		= pParser->m_bHttps ? HTTPS_NAME : HTTP_NAME;
	HP_HttpServer pServer	= pParser->m_bHttps ? s_https_server : s_http_server;

	if(::HP_HttpServer_Release(pServer, pParser->m_dwConnID))
		::LogRelease(pParser->m_dwConnID, lpszName);
	else
		::LogReleaseFail(pParser->m_dwConnID, lpszName);
}

void OnCmdKickLong(CHttpCommandParser* pParser)
{
	LPCTSTR lpszName		= pParser->m_bHttps ? HTTPS_NAME : HTTP_NAME;
	HP_HttpServer pServer	= pParser->m_bHttps ? s_https_server : s_http_server;

	if(::HP_Server_DisconnectLongConnections(pServer, pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag, lpszName);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag, lpszName);
}

void OnCmdKickSilence(CHttpCommandParser* pParser)
{
	LPCTSTR lpszName		= pParser->m_bHttps ? HTTPS_NAME : HTTP_NAME;
	HP_HttpServer pServer	= pParser->m_bHttps ? s_https_server : s_http_server;

	if(::HP_Server_DisconnectSilenceConnections(pServer, pParser->m_dwSeconds * 1000, pParser->m_bFlag))
		::LogDisconnectLong(pParser->m_dwSeconds, pParser->m_bFlag, lpszName);
	else
		::LogDisconnectFailLong(pParser->m_dwSeconds, pParser->m_bFlag, lpszName);
}

// ------------------------------------------------------------------------------------------------------------- //

void DestroyHPSocketObjects()
{
	::Destroy_HP_HttpServer(s_http_server);
	::Destroy_HP_HttpsServer(s_https_server);
	::Destroy_HP_HttpServerListener(s_http_listener);
}

BOOL CreateHPSocketObjects()
{
	s_http_listener	= ::Create_HP_HttpServerListener();
	s_http_server	= ::Create_HP_HttpServer(s_http_listener);
	s_https_server	= ::Create_HP_HttpsServer(s_http_listener);

	/*
	if(::HP_SSLServer_SetupSSLContext(s_https_server, SSL_VM_NONE, g_s_lpszPemCertFile, g_s_lpszPemKeyFile, g_s_lpszKeyPasswod, g_s_lpszCAPemCertFileOrPath, SIN_ServerNameCallback))
		SPECIAL_SERVER_INDEX = ::HP_SSLServer_AddSSLContext(s_https_server, SSL_VM_NONE, g_s_lpszPemCertFile2, g_s_lpszPemKeyFile2, g_s_lpszKeyPasswod2, g_s_lpszCAPemCertFileOrPath2);
	*/
	if(::HP_SSLServer_SetupSSLContext(s_https_server, SSL_VM_NONE, g_s_lpszPemCertFile, g_s_lpszPemKeyFile, g_s_lpszKeyPasswod, g_s_lpszCAPemCertFileOrPath, nullptr))
	{
		int iIndex = ::HP_SSLServer_AddSSLContext(s_https_server, SSL_VM_NONE, g_s_lpszPemCertFile2, g_s_lpszPemKeyFile2, g_s_lpszKeyPasswod2, g_s_lpszCAPemCertFileOrPath2);
		VERIFY(::HP_SSLServer_BindSSLServerName(s_https_server, SPECIAL_SERVER_NAME, iIndex));
	}
	else
	{
		::LogServerStartFail(::SYS_GetLastError(), _T("initialize SSL env fail"));

		DestroyHPSocketObjects();
		return FALSE;
	}

	::HP_Set_FN_HttpServer_OnPrepareListen(s_http_listener, OnPrepareListen);
	::HP_Set_FN_HttpServer_OnAccept(s_http_listener, OnAccept);
	::HP_Set_FN_HttpServer_OnHandShake(s_http_listener, OnHandShake);
	::HP_Set_FN_HttpServer_OnReceive(s_http_listener, OnReceive);
	::HP_Set_FN_HttpServer_OnSend(s_http_listener, OnSend);
	::HP_Set_FN_HttpServer_OnClose(s_http_listener, OnClose);
	::HP_Set_FN_HttpServer_OnShutdown(s_http_listener, OnShutdown);

	::HP_Set_FN_HttpServer_OnMessageBegin(s_http_listener, OnMessageBegin);
	::HP_Set_FN_HttpServer_OnRequestLine(s_http_listener, OnRequestLine);
	::HP_Set_FN_HttpServer_OnHeader(s_http_listener, OnHeader);
	::HP_Set_FN_HttpServer_OnHeadersComplete(s_http_listener, OnHeadersComplete);
	::HP_Set_FN_HttpServer_OnBody(s_http_listener, OnBody);
	::HP_Set_FN_HttpServer_OnChunkHeader(s_http_listener, OnChunkHeader);
	::HP_Set_FN_HttpServer_OnChunkComplete(s_http_listener, OnChunkComplete);
	::HP_Set_FN_HttpServer_OnMessageComplete(s_http_listener, OnMessageComplete);
	::HP_Set_FN_HttpServer_OnUpgrade(s_http_listener, OnUpgrade);
	::HP_Set_FN_HttpServer_OnParseError(s_http_listener, OnParseError);

	::HP_Set_FN_HttpServer_OnWSMessageHeader(s_http_listener, OnWSMessageHeader);
	::HP_Set_FN_HttpServer_OnWSMessageBody(s_http_listener, OnWSMessageBody);
	::HP_Set_FN_HttpServer_OnWSMessageComplete(s_http_listener, OnWSMessageComplete);

	::HP_TcpServer_SetKeepAliveTime(s_http_server, g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);
	::HP_TcpServer_SetKeepAliveTime(s_https_server, g_app_arg.keep_alive ? TCP_KEEPALIVE_TIME : 0);

	s_sender_names[s_http_server]	= HTTP_NAME;
	s_sender_names[s_https_server]	= HTTPS_NAME;

	return TRUE;
}

int main(int argc, char* const argv[])
{
	CTermAttrInitializer term_attr;
	CAppSignalHandler s_signal_handler({SIGTTOU, SIGINT});

	g_app_arg.ParseArgs(argc, argv);

	if(!CreateHPSocketObjects())
		return EXIT_CODE_CONFIG;

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

	DestroyHPSocketObjects();

	return EXIT_CODE_OK;
}
