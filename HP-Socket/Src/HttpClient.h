/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 4.0.1
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 75375912
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

template<class T> class CHttpClientT : public IHttpRequester, public T
{
private:
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

public:

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
	virtual BOOL AddCookie(LPCSTR lpszName, LPCSTR lpszValue, BOOL bRelpace = TRUE)
		{return m_objHttp.AddCookie(lpszName, lpszValue, bRelpace);}
	virtual BOOL DeleteCookie(LPCSTR lpszName)
		{return m_objHttp.DeleteCookie(lpszName);}
	virtual BOOL DeleteAllCookies()
		{m_objHttp.DeleteAllCookies(); return TRUE;}

	virtual USHORT GetStatusCode()
		{return m_objHttp.GetStatusCode();}

protected:
	virtual BOOL CheckParams();

	virtual EnHandleResult DoFireReceive(ITcpClient* pSender, const BYTE* pData, int iLength)
		{ASSERT(pSender == this); return m_objHttp.Execute(pData, iLength);}

	EnHandleResult DoFireSuperReceive(IHttpClient* pSender, const BYTE* pData, int iLength)
		{ASSERT(pSender == (IHttpClient*)this); return __super::DoFireReceive(pSender, pData, iLength);}

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

public:
	CHttpClientT(IHttpClientListener* pListener)
	: T					(pListener)
	, m_pListener		(pListener)
	, m_enLocalVersion	(DEFAULT_HTTP_VERSION)
	, m_objHttp			(FALSE, this, (IHttpClient*)this)
	{

	}

	virtual ~CHttpClientT()
	{
		Stop();
	}

private:
	IHttpClientListener*	m_pListener;

	EnHttpVersion			m_enLocalVersion;

	THttpObj				m_objHttp;

};

typedef CHttpClientT<CTcpClient> CHttpClient;

#ifdef _SSL_SUPPORT

#include "SSLClient.h"

typedef CHttpClientT<CSSLClient> CHttpsClient;

#endif
