/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
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

#include "TcpAgent.h"
#include "HttpHelper.h"

template<class T, USHORT default_port> class CHttpAgentT : public IComplexHttpRequester, public T
{
protected:
	typedef CHttpObjPoolT<FALSE, CHttpAgentT, TSocketObj>	CHttpObjPool;
	typedef THttpObjT<CHttpAgentT, TSocketObj>				THttpObj;
	friend struct											THttpObj;

public:
	virtual BOOL SendRequest(CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0, const BYTE* pBody = nullptr, int iLength = 0);
	virtual BOOL SendLocalFile(CONNID dwConnID, LPCSTR lpszFileName, LPCSTR lpszMethod, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0);

	virtual BOOL SendPost(CONNID dwConnID, LPCSTR lpszPath, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
		{return SendRequest(dwConnID, HTTP_METHOD_POST, lpszPath, lpHeaders, iHeaderCount, pBody, iLength);}
	virtual BOOL SendPut(CONNID dwConnID, LPCSTR lpszPath, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
		{return SendRequest(dwConnID, HTTP_METHOD_PUT, lpszPath, lpHeaders, iHeaderCount, pBody, iLength);}
	virtual BOOL SendPatch(CONNID dwConnID, LPCSTR lpszPath, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
		{return SendRequest(dwConnID, HTTP_METHOD_PATCH, lpszPath, lpHeaders, iHeaderCount, pBody, iLength);}
	virtual BOOL SendGet(CONNID dwConnID, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)
		{return SendRequest(dwConnID, HTTP_METHOD_GET, lpszPath, lpHeaders, iHeaderCount);}
	virtual BOOL SendDelete(CONNID dwConnID, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)
		{return SendRequest(dwConnID, HTTP_METHOD_DELETE, lpszPath, lpHeaders, iHeaderCount);}
	virtual BOOL SendHead(CONNID dwConnID, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)
		{return SendRequest(dwConnID, HTTP_METHOD_HEAD, lpszPath, lpHeaders, iHeaderCount);}
	virtual BOOL SendTrace(CONNID dwConnID, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)
		{return SendRequest(dwConnID, HTTP_METHOD_TRACE, lpszPath, lpHeaders, iHeaderCount);}
	virtual BOOL SendOptions(CONNID dwConnID, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)
		{return SendRequest(dwConnID, HTTP_METHOD_OPTIONS, lpszPath, lpHeaders, iHeaderCount);}
	virtual BOOL SendConnect(CONNID dwConnID, LPCSTR lpszHost, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)
		{return SendRequest(dwConnID, HTTP_METHOD_CONNECT, lpszHost, lpHeaders, iHeaderCount);}

	virtual BOOL SendWSMessage(CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4] = nullptr, BYTE* pData = nullptr, int iLength = 0, ULONGLONG ullBodyLen = 0);

public:
	virtual void SetUseCookie(BOOL bUseCookie)					{m_pCookieMgr = bUseCookie ? &g_CookieMgr : nullptr;}
	virtual BOOL IsUseCookie()									{return m_pCookieMgr != nullptr;}

	virtual void SetLocalVersion(EnHttpVersion enLocalVersion)	{m_enLocalVersion = enLocalVersion;}
	virtual EnHttpVersion GetLocalVersion()						{return m_enLocalVersion;}

	virtual BOOL IsUpgrade(CONNID dwConnID);
	virtual BOOL IsKeepAlive(CONNID dwConnID);
	virtual USHORT GetVersion(CONNID dwConnID);
	virtual ULONGLONG GetContentLength(CONNID dwConnID);
	virtual LPCSTR GetContentType(CONNID dwConnID);
	virtual LPCSTR GetContentEncoding(CONNID dwConnID);
	virtual LPCSTR GetTransferEncoding(CONNID dwConnID);
	virtual EnHttpUpgradeType GetUpgradeType(CONNID dwConnID);
	virtual USHORT GetParseErrorCode(CONNID dwConnID, LPCSTR* lpszErrorDesc = nullptr);

	virtual BOOL GetHeader(CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue);
	virtual BOOL GetHeaders(CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue[], DWORD& dwCount);
	virtual BOOL GetAllHeaders(CONNID dwConnID, THeader lpHeaders[], DWORD& dwCount);
	virtual BOOL GetAllHeaderNames(CONNID dwConnID, LPCSTR lpszName[], DWORD& dwCount);

	virtual BOOL GetCookie(CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue);
	virtual BOOL GetAllCookies(CONNID dwConnID, TCookie lpCookies[], DWORD& dwCount);

	virtual USHORT GetStatusCode(CONNID dwConnID);

	virtual BOOL GetWSMessageState(CONNID dwConnID, BOOL* lpbFinal, BYTE* lpiReserved, BYTE* lpiOperationCode, LPCBYTE* lpszMask, ULONGLONG* lpullBodyLen, ULONGLONG* lpullBodyRemain);

private:
	virtual BOOL CheckParams();
	virtual void PrepareStart();
	virtual EnHandleResult DoFireConnect(TSocketObj* pSocketObj);
	virtual EnHandleResult DoFireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength);
	virtual EnHandleResult DoFireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult DoFireShutdown();

	EnHandleResult DoFireSuperReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return __super::DoFireReceive(pSocketObj, pData, iLength);}

	EnHttpParseResult FireMessageBegin(TSocketObj* pSocketObj)
		{return m_pListener->OnMessageBegin((IHttpAgent*)this, pSocketObj->connID);}
	EnHttpParseResult FireRequestLine(TSocketObj* pSocketObj, LPCSTR lpszMethod, LPCSTR lpszUrl)
		{return m_pListener->OnRequestLine((IHttpAgent*)this, pSocketObj->connID, lpszMethod, lpszUrl);}
	EnHttpParseResult FireStatusLine(TSocketObj* pSocketObj, USHORT usStatusCode, LPCSTR lpszDesc)
		{return m_pListener->OnStatusLine((IHttpAgent*)this, pSocketObj->connID, usStatusCode, lpszDesc);}
	EnHttpParseResult FireHeader(TSocketObj* pSocketObj, LPCSTR lpszName, LPCSTR lpszValue)
		{return m_pListener->OnHeader((IHttpAgent*)this, pSocketObj->connID, lpszName, lpszValue);}
	EnHttpParseResult FireHeadersComplete(TSocketObj* pSocketObj)
		{return m_pListener->OnHeadersComplete((IHttpAgent*)this, pSocketObj->connID);}
	EnHttpParseResult FireBody(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return m_pListener->OnBody((IHttpAgent*)this, pSocketObj->connID, pData, iLength);}
	EnHttpParseResult FireChunkHeader(TSocketObj* pSocketObj, int iLength)
		{return m_pListener->OnChunkHeader((IHttpAgent*)this, pSocketObj->connID, iLength);}
	EnHttpParseResult FireChunkComplete(TSocketObj* pSocketObj)
		{return m_pListener->OnChunkComplete((IHttpAgent*)this, pSocketObj->connID);}
	EnHttpParseResult FireMessageComplete(TSocketObj* pSocketObj)
		{return m_pListener->OnMessageComplete((IHttpAgent*)this, pSocketObj->connID);}
	EnHttpParseResult FireUpgrade(TSocketObj* pSocketObj, EnHttpUpgradeType enUpgradeType)
		{return m_pListener->OnUpgrade((IHttpAgent*)this, pSocketObj->connID, enUpgradeType);}
	EnHttpParseResult FireParseError(TSocketObj* pSocketObj, int iErrorCode, LPCSTR lpszErrorDesc)
		{return m_pListener->OnParseError((IHttpAgent*)this, pSocketObj->connID, iErrorCode, lpszErrorDesc);}

	EnHandleResult FireWSMessageHeader(TSocketObj* pSocketObj, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
		{return m_pListener->OnWSMessageHeader((IHttpAgent*)this, pSocketObj->connID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen);}
	EnHandleResult FireWSMessageBody(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return m_pListener->OnWSMessageBody((IHttpAgent*)this, pSocketObj->connID, pData, iLength);}
	EnHandleResult FireWSMessageComplete(TSocketObj* pSocketObj)
		{return m_pListener->OnWSMessageComplete((IHttpAgent*)this, pSocketObj->connID);}

	inline THttpObj* FindHttpObj(CONNID dwConnID);
	inline THttpObj* FindHttpObj(TSocketObj* pSocketObj);

	CCookieMgr* GetCookieMgr()						{return m_pCookieMgr;}
	LPCSTR GetRemoteDomain(TSocketObj* pSocketObj)	{LPCSTR lpszDomain; pSocketObj->GetRemoteHost(&lpszDomain); return lpszDomain;}

public:
	CHttpAgentT(IHttpAgentListener* pListener)
	: T					(pListener)
	, m_pListener		(pListener)
	, m_pCookieMgr		(&g_CookieMgr)
	, m_enLocalVersion	(DEFAULT_HTTP_VERSION)
	{

	}

	virtual ~CHttpAgentT()
	{
		Stop();
	}

private:
	IHttpAgentListener*	m_pListener;
	CCookieMgr*			m_pCookieMgr;
	EnHttpVersion		m_enLocalVersion;

	CHttpObjPool		m_objPool;
};

// ------------------------------------------------------------------------------------------------------------- //

typedef CHttpAgentT<CTcpAgent, HTTP_DEFAULT_PORT> CHttpAgent;

#ifdef _SSL_SUPPORT

#include "SSLAgent.h"

typedef CHttpAgentT<CSSLAgent, HTTPS_DEFAULT_PORT> CHttpsAgent;

#endif
