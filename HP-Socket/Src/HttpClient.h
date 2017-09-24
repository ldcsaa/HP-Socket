/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 5.0.2
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 75375912, 44636872
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#pragma once

#include "TcpClient.h"
#include "HttpHelper.h"

template<class R, class T, USHORT default_port> class CHttpClientT : public R, public T
{
protected:
	typedef THttpObjT<CHttpClientT, IHttpClient>	THttpObj;
	friend struct									THttpObj;

public:
	virtual BOOL SendRequest(LPCSTR lpszMethod, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0, const BYTE* pBody = nullptr, int iLength = 0);
	virtual BOOL SendLocalFile(LPCSTR lpszFileName, LPCSTR lpszMethod, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0);

	virtual BOOL SendPost(LPCSTR lpszPath, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
		{return SendRequest(HTTP_METHOD_POST, lpszPath, lpHeaders, iHeaderCount, pBody, iLength);}
	virtual BOOL SendPut(LPCSTR lpszPath, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
		{return SendRequest(HTTP_METHOD_PUT, lpszPath, lpHeaders, iHeaderCount, pBody, iLength);}
	virtual BOOL SendPatch(LPCSTR lpszPath, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
		{return SendRequest(HTTP_METHOD_PATCH, lpszPath, lpHeaders, iHeaderCount, pBody, iLength);}
	virtual BOOL SendGet(LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)
		{return SendRequest(HTTP_METHOD_GET, lpszPath, lpHeaders, iHeaderCount);}
	virtual BOOL SendDelete(LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)
		{return SendRequest(HTTP_METHOD_DELETE, lpszPath, lpHeaders, iHeaderCount);}
	virtual BOOL SendHead(LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)
		{return SendRequest(HTTP_METHOD_HEAD, lpszPath, lpHeaders, iHeaderCount);}
	virtual BOOL SendTrace(LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)
		{return SendRequest(HTTP_METHOD_TRACE, lpszPath, lpHeaders, iHeaderCount);}
	virtual BOOL SendOptions(LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)
		{return SendRequest(HTTP_METHOD_OPTIONS, lpszPath, lpHeaders, iHeaderCount);}
	virtual BOOL SendConnect(LPCSTR lpszHost, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)
		{return SendRequest(HTTP_METHOD_CONNECT, lpszHost, lpHeaders, iHeaderCount);}

	virtual BOOL SendWSMessage(BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4] = nullptr, BYTE* pData = nullptr, int iLength = 0, ULONGLONG ullBodyLen = 0);

public:
	virtual void SetUseCookie(BOOL bUseCookie)					{m_pCookieMgr = bUseCookie ? &g_CookieMgr : nullptr;}
	virtual BOOL IsUseCookie()									{return m_pCookieMgr != nullptr;}

	virtual void SetLocalVersion(EnHttpVersion enLocalVersion)	{m_enLocalVersion = enLocalVersion;}
	virtual EnHttpVersion GetLocalVersion()						{return m_enLocalVersion;}

	virtual BOOL IsUpgrade()
		{return m_objHttp.IsUpgrade();}
	virtual BOOL IsKeepAlive()
		{return m_objHttp.IsKeepAlive();}
	virtual USHORT GetVersion()
		{return m_objHttp.GetVersion();}
	virtual ULONGLONG GetContentLength()
		{return m_objHttp.GetContentLength();}
	virtual LPCSTR GetContentType()
		{return m_objHttp.GetContentType();}
	virtual LPCSTR GetContentEncoding()
		{return m_objHttp.GetContentEncoding();}
	virtual LPCSTR GetTransferEncoding()
		{return m_objHttp.GetTransferEncoding();}
	virtual EnHttpUpgradeType GetUpgradeType()
		{return m_objHttp.GetUpgradeType();}
	virtual USHORT GetParseErrorCode(LPCSTR* lpszErrorDesc = nullptr)
		{return m_objHttp.GetParseErrorCode(lpszErrorDesc);}

	virtual BOOL GetHeader(LPCSTR lpszName, LPCSTR* lpszValue)
		{return m_objHttp.GetHeader(lpszName, lpszValue);}
	virtual BOOL GetHeaders(LPCSTR lpszName, LPCSTR lpszValue[], DWORD& dwCount)
		{return m_objHttp.GetHeaders(lpszName, lpszValue, dwCount);}
	virtual BOOL GetAllHeaders(THeader lpHeaders[], DWORD& dwCount)
		{return m_objHttp.GetAllHeaders(lpHeaders, dwCount);}
	virtual BOOL GetAllHeaderNames(LPCSTR lpszName[], DWORD& dwCount)
		{return m_objHttp.GetAllHeaderNames(lpszName, dwCount);}

	virtual BOOL GetCookie(LPCSTR lpszName, LPCSTR* lpszValue)
		{return m_objHttp.GetCookie(lpszName, lpszValue);}
	virtual BOOL GetAllCookies(TCookie lpCookies[], DWORD& dwCount)
		{return m_objHttp.GetAllCookies(lpCookies, dwCount);}

	virtual USHORT GetStatusCode()
		{return m_objHttp.GetStatusCode();}

	virtual BOOL GetWSMessageState(BOOL* lpbFinal, BYTE* lpiReserved, BYTE* lpiOperationCode, LPCBYTE* lpszMask, ULONGLONG* lpullBodyLen, ULONGLONG* lpullBodyRemain)
		{return m_objHttp.GetWSMessageState(lpbFinal, lpiReserved, lpiOperationCode, lpszMask, lpullBodyLen, lpullBodyRemain);}

private:
	virtual BOOL CheckParams();

	virtual EnHandleResult DoFireReceive(ITcpClient* pSender, const BYTE* pData, int iLength)
		{ASSERT(pSender == this); return m_objHttp.Execute(pData, iLength);}

	EnHandleResult DoFireSuperReceive(IHttpClient* pSender, const BYTE* pData, int iLength)
		{ASSERT(pSender == (IHttpClient*)this); return __super::DoFireReceive(pSender, pData, iLength);}

	virtual EnHandleResult DoFireClose(ITcpClient* pSender, EnSocketOperation enOperation, int iErrorCode)
	{
		ASSERT(pSender == (IHttpClient*)this);

		m_objHttp.CheckBodyIdentityEof();

		return __super::DoFireClose(pSender, enOperation, iErrorCode);
	}

	virtual void Reset()
	{
		m_objHttp.Reset();

		__super::Reset();
	}

	EnHttpParseResult FireMessageBegin(IHttpClient* pSender)
		{return m_pListener->OnMessageBegin(pSender, pSender->GetConnectionID());}
	EnHttpParseResult FireRequestLine(IHttpClient* pSender, LPCSTR lpszMethod, LPCSTR lpszUrl)
		{return m_pListener->OnRequestLine(pSender, pSender->GetConnectionID(), lpszMethod, lpszUrl);}
	EnHttpParseResult FireStatusLine(IHttpClient* pSender, USHORT usStatusCode, LPCSTR lpszDesc)
		{return m_pListener->OnStatusLine(pSender, pSender->GetConnectionID(), usStatusCode, lpszDesc);}
	EnHttpParseResult FireHeader(IHttpClient* pSender, LPCSTR lpszName, LPCSTR lpszValue)
		{return m_pListener->OnHeader(pSender, pSender->GetConnectionID(), lpszName, lpszValue);}
	EnHttpParseResult FireHeadersComplete(IHttpClient* pSender)
		{return m_pListener->OnHeadersComplete(pSender, pSender->GetConnectionID());}
	EnHttpParseResult FireBody(IHttpClient* pSender, const BYTE* pData, int iLength)
		{return m_pListener->OnBody(pSender, pSender->GetConnectionID(), pData, iLength);}
	EnHttpParseResult FireChunkHeader(IHttpClient* pSender, int iLength)
		{return m_pListener->OnChunkHeader(pSender, pSender->GetConnectionID(), iLength);}
	EnHttpParseResult FireChunkComplete(IHttpClient* pSender)
		{return m_pListener->OnChunkComplete(pSender, pSender->GetConnectionID());}
	EnHttpParseResult FireMessageComplete(IHttpClient* pSender)
		{return m_pListener->OnMessageComplete(pSender, pSender->GetConnectionID());}
	EnHttpParseResult FireUpgrade(IHttpClient* pSender, EnHttpUpgradeType enUpgradeType)
		{return m_pListener->OnUpgrade(pSender, pSender->GetConnectionID(), enUpgradeType);}
	EnHttpParseResult FireParseError(IHttpClient* pSender, int iErrorCode, LPCSTR lpszErrorDesc)
		{return m_pListener->OnParseError(pSender, pSender->GetConnectionID(), iErrorCode, lpszErrorDesc);}

	EnHandleResult FireWSMessageHeader(IHttpClient* pSender, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
		{return m_pListener->OnWSMessageHeader(pSender, pSender->GetConnectionID(), bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen);}
	EnHandleResult FireWSMessageBody(IHttpClient* pSender, const BYTE* pData, int iLength)
		{return m_pListener->OnWSMessageBody(pSender, pSender->GetConnectionID(), pData, iLength);}
	EnHandleResult FireWSMessageComplete(IHttpClient* pSender)
		{return m_pListener->OnWSMessageComplete(pSender, pSender->GetConnectionID());}

	CCookieMgr* GetCookieMgr()						{return m_pCookieMgr;}
	LPCSTR GetRemoteDomain(IHttpClient* pSender)	{LPCSTR lpszDomain; GetRemoteHost(&lpszDomain); return lpszDomain;}

public:
	CHttpClientT(IHttpClientListener* pListener)
	: T					(pListener)
	, m_pListener		(pListener)
	, m_pCookieMgr		(&g_CookieMgr)
	, m_enLocalVersion	(DEFAULT_HTTP_VERSION)
	, m_objHttp			(FALSE, this, (IHttpClient*)this)
	{

	}

	virtual ~CHttpClientT()
	{
		Stop();
	}

protected:
	THttpObj				m_objHttp;

private:
	IHttpClientListener*	m_pListener;
	CCookieMgr*				m_pCookieMgr;
	EnHttpVersion			m_enLocalVersion;

};

// ------------------------------------------------------------------------------------------------------------- //

template<class T, USHORT default_port> class CHttpSyncClientT : public CHttpClientT<IHttpSyncRequester, T, default_port>, private CHttpClientListener
{
public:
	virtual BOOL Start(LPCTSTR lpszRemoteAddress, USHORT usPort, BOOL bAsyncConnect = TRUE, LPCTSTR lpszBindAddress = nullptr);
public:
	virtual BOOL SendRequest(LPCSTR lpszMethod, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0, const BYTE* pBody = nullptr, int iLength = 0);
	virtual BOOL SendWSMessage(BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4] = nullptr, BYTE* pData = nullptr, int iLength = 0, ULONGLONG ullBodyLen = 0);

public:
	virtual BOOL IsUpgrade()
		{return m_pHttpObj->IsUpgrade();}
	virtual BOOL IsKeepAlive()
		{return m_pHttpObj->IsKeepAlive();}
	virtual USHORT GetVersion()
		{return m_pHttpObj->GetVersion();}
	virtual ULONGLONG GetContentLength()
		{return m_pHttpObj->GetContentLength();}
	virtual LPCSTR GetContentType()
		{return m_pHttpObj->GetContentType();}
	virtual LPCSTR GetContentEncoding()
		{return m_pHttpObj->GetContentEncoding();}
	virtual LPCSTR GetTransferEncoding()
		{return m_pHttpObj->GetTransferEncoding();}
	virtual EnHttpUpgradeType GetUpgradeType()
		{return m_pHttpObj->GetUpgradeType();}
	virtual USHORT GetParseErrorCode(LPCSTR* lpszErrorDesc = nullptr)
		{return m_pHttpObj->GetParseErrorCode(lpszErrorDesc);}

	virtual BOOL GetHeader(LPCSTR lpszName, LPCSTR* lpszValue)
		{return m_pHttpObj->GetHeader(lpszName, lpszValue);}
	virtual BOOL GetHeaders(LPCSTR lpszName, LPCSTR lpszValue[], DWORD& dwCount)
		{return m_pHttpObj->GetHeaders(lpszName, lpszValue, dwCount);}
	virtual BOOL GetAllHeaders(THeader lpHeaders[], DWORD& dwCount)
		{return m_pHttpObj->GetAllHeaders(lpHeaders, dwCount);}
	virtual BOOL GetAllHeaderNames(LPCSTR lpszName[], DWORD& dwCount)
		{return m_pHttpObj->GetAllHeaderNames(lpszName, dwCount);}

	virtual BOOL GetCookie(LPCSTR lpszName, LPCSTR* lpszValue)
		{return m_pHttpObj->GetCookie(lpszName, lpszValue);}
	virtual BOOL GetAllCookies(TCookie lpCookies[], DWORD& dwCount)
		{return m_pHttpObj->GetAllCookies(lpCookies, dwCount);}

	virtual USHORT GetStatusCode()
		{return m_pHttpObj->GetStatusCode();}

	virtual BOOL GetWSMessageState(BOOL* lpbFinal, BYTE* lpiReserved, BYTE* lpiOperationCode, LPCBYTE* lpszMask, ULONGLONG* lpullBodyLen, ULONGLONG* lpullBodyRemain)
		{return m_pHttpObj->GetWSMessageState(lpbFinal, lpiReserved, lpiOperationCode, lpszMask, lpullBodyLen, lpullBodyRemain);}

public:
	virtual BOOL OpenUrl(LPCSTR lpszMethod, LPCSTR lpszUrl, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0, const BYTE* pBody = nullptr, int iLength = 0, BOOL bForceReconnect = FALSE);
	virtual BOOL CleanupRequestResult();

public:
	virtual BOOL GetResponseBody	(LPCBYTE* lpszBody, int* iLength);

	virtual void SetConnectTimeout	(DWORD dwConnectTimeout)	{m_dwConnectTimeout = dwConnectTimeout;}
	virtual void SetRequestTimeout	(DWORD dwRequestTimeout)	{m_dwRequestTimeout = dwRequestTimeout;}

	virtual DWORD GetConnectTimeout	()	{return m_dwConnectTimeout;}
	virtual DWORD GetRequestTimeout	()	{return m_dwRequestTimeout;}

private:
	virtual EnHandleResult OnHandShake(ITcpClient* pSender, CONNID dwConnID);
	virtual EnHandleResult OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);

	virtual EnHttpParseResult OnBody(IHttpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHttpParseResult OnMessageComplete(IHttpClient* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnUpgrade(IHttpClient* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType);
	virtual EnHttpParseResult OnParseError(IHttpClient* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc);

	virtual EnHandleResult OnWSMessageBody(IHttpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnWSMessageComplete(IHttpClient* pSender, CONNID dwConnID);

	virtual EnHandleResult OnPrepareConnect(ITcpClient* pSender, CONNID dwConnID, SOCKET socket);
	virtual EnHandleResult OnConnect(ITcpClient* pSender, CONNID dwConnID);
	virtual EnHandleResult OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength);

	virtual EnHttpParseResult OnMessageBegin(IHttpClient* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnStatusLine(IHttpClient* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc);
	virtual EnHttpParseResult OnHeader(IHttpClient* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue);
	virtual EnHttpParseResult OnHeadersComplete(IHttpClient* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnChunkHeader(IHttpClient* pSender, CONNID dwConnID, int iLength);
	virtual EnHttpParseResult OnChunkComplete(IHttpClient* pSender, CONNID dwConnID);

	virtual EnHandleResult OnWSMessageHeader(IHttpClient* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen);


private:
	void SetRequestEvent(EnHttpSyncRequestProgress enProgress, BOOL bCopyHttpObj = TRUE);

public:
	CHttpSyncClientT(IHttpClientListener* pListener = nullptr)
	: CHttpClientT			(this)
	, m_enProgress			(HSRP_DONE)
	, m_objHttp2			(FALSE, this, (IHttpClient*)this)
	, m_pHttpObj			(nullptr)
	, m_pListener2			(pListener)
	, m_dwConnectTimeout	(DEFAULT_HTTP_SYNC_CONNECT_TIMEOUT)
	, m_dwRequestTimeout	(DEFAULT_HTTP_SYNC_REQUEST_TIMEOUT)
	{

	}

	virtual ~CHttpSyncClientT()
	{
		Stop();
	}

private:
	DWORD		m_dwConnectTimeout;
	DWORD		m_dwRequestTimeout;

	CEvt		m_evWait;
	THttpObj	m_objHttp2;

	THttpObj*				m_pHttpObj;
	IHttpClientListener*	m_pListener2;

	EnHttpSyncRequestProgress		m_enProgress;
	CBufferPtrT<BYTE, 16 * 1024>	m_szBuffer;
};

// ------------------------------------------------------------------------------------------------------------- //

typedef CHttpClientT<IHttpRequester, CTcpClient, HTTP_DEFAULT_PORT>		CHttpClient;
typedef CHttpSyncClientT<CTcpClient, HTTP_DEFAULT_PORT>					CHttpSyncClient;

#ifdef _SSL_SUPPORT

#include "SSLClient.h"

typedef CHttpClientT<IHttpRequester, CSSLClient, HTTPS_DEFAULT_PORT>	CHttpsClient;
typedef CHttpSyncClientT<CSSLClient, HTTPS_DEFAULT_PORT>				CHttpsSyncClient;

#endif
