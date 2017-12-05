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

#include "TcpServer.h"
#include "HttpHelper.h"
#include "../Common/Src/Thread.h"

template<class T, USHORT default_port> class CHttpServerT : public IComplexHttpResponder, public T
{
protected:
	typedef CThread<CHttpServerT>							CCleanThread;
	friend class											CCleanThread;

	typedef CHttpObjPoolT<TRUE, CHttpServerT, TSocketObj>	CHttpObjPool;
	typedef THttpObjT<CHttpServerT, TSocketObj>				THttpObj;
	friend struct											THttpObj;

public:

	virtual BOOL Start(LPCTSTR lpszBindAddress, USHORT usPort);

	virtual BOOL SendResponse(CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc = nullptr, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0, const BYTE* pData = nullptr, int iLength = 0);
	virtual BOOL SendLocalFile(CONNID dwConnID, LPCSTR lpszFileName, USHORT usStatusCode = HSC_OK, LPCSTR lpszDesc = nullptr, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0);

	virtual BOOL Release(CONNID dwConnID);

	virtual BOOL SendWSMessage(CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4] = nullptr, BYTE* pData = nullptr, int iLength = 0, ULONGLONG ullBodyLen = 0);

public:

	virtual void SetLocalVersion(EnHttpVersion enLocalVersion)	{m_enLocalVersion = enLocalVersion;}
	virtual void SetReleaseDelay(DWORD dwReleaseDelay)			{m_dwReleaseDelay = dwReleaseDelay;}

	virtual EnHttpVersion GetLocalVersion	()	{return m_enLocalVersion;}
	virtual DWORD GetReleaseDelay			()	{return m_dwReleaseDelay;}

	virtual BOOL IsUpgrade(CONNID dwConnID);
	virtual BOOL IsKeepAlive(CONNID dwConnID);
	virtual USHORT GetVersion(CONNID dwConnID);
	virtual LPCSTR GetHost(CONNID dwConnID);
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

	virtual USHORT GetUrlFieldSet(CONNID dwConnID);
	virtual LPCSTR GetUrlField(CONNID dwConnID, EnHttpUrlField enField);
	virtual LPCSTR GetMethod(CONNID dwConnID);

	virtual BOOL GetWSMessageState(CONNID dwConnID, BOOL* lpbFinal, BYTE* lpiReserved, BYTE* lpiOperationCode, LPCBYTE* lpszMask, ULONGLONG* lpullBodyLen, ULONGLONG* lpullBodyRemain);

private:
	virtual BOOL CheckParams();
	virtual void PrepareStart();
	virtual EnHandleResult DoFireAccept(TSocketObj* pSocketObj);
	virtual EnHandleResult DoFireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength);
	virtual EnHandleResult DoFireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult DoFireShutdown();

	EnHandleResult DoFireSuperReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return __super::DoFireReceive(pSocketObj, pData, iLength);}

	EnHttpParseResult FireMessageBegin(TSocketObj* pSocketObj)
		{return m_pListener->OnMessageBegin((IHttpServer*)this, pSocketObj->connID);}
	EnHttpParseResult FireRequestLine(TSocketObj* pSocketObj, LPCSTR lpszMethod, LPCSTR lpszUrl)
		{return m_pListener->OnRequestLine((IHttpServer*)this, pSocketObj->connID, lpszMethod, lpszUrl);}
	EnHttpParseResult FireStatusLine(TSocketObj* pSocketObj, USHORT usStatusCode, LPCSTR lpszDesc)
		{return m_pListener->OnStatusLine((IHttpServer*)this, pSocketObj->connID, usStatusCode, lpszDesc);}
	EnHttpParseResult FireHeader(TSocketObj* pSocketObj, LPCSTR lpszName, LPCSTR lpszValue)
		{return m_pListener->OnHeader((IHttpServer*)this, pSocketObj->connID, lpszName, lpszValue);}
	EnHttpParseResult FireHeadersComplete(TSocketObj* pSocketObj)
		{return m_pListener->OnHeadersComplete((IHttpServer*)this, pSocketObj->connID);}
	EnHttpParseResult FireBody(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return m_pListener->OnBody((IHttpServer*)this, pSocketObj->connID, pData, iLength);}
	EnHttpParseResult FireChunkHeader(TSocketObj* pSocketObj, int iLength)
		{return m_pListener->OnChunkHeader((IHttpServer*)this, pSocketObj->connID, iLength);}
	EnHttpParseResult FireChunkComplete(TSocketObj* pSocketObj)
		{return m_pListener->OnChunkComplete((IHttpServer*)this, pSocketObj->connID);}
	EnHttpParseResult FireMessageComplete(TSocketObj* pSocketObj)
		{return m_pListener->OnMessageComplete((IHttpServer*)this, pSocketObj->connID);}
	EnHttpParseResult FireUpgrade(TSocketObj* pSocketObj, EnHttpUpgradeType enUpgradeType)
		{return m_pListener->OnUpgrade((IHttpServer*)this, pSocketObj->connID, enUpgradeType);}
	EnHttpParseResult FireParseError(TSocketObj* pSocketObj, int iErrorCode, LPCSTR lpszErrorDesc)
		{return m_pListener->OnParseError((IHttpServer*)this, pSocketObj->connID, iErrorCode, lpszErrorDesc);}

	EnHandleResult FireWSMessageHeader(TSocketObj* pSocketObj, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
		{return m_pListener->OnWSMessageHeader((IHttpServer*)this, pSocketObj->connID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen);}
	EnHandleResult FireWSMessageBody(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return m_pListener->OnWSMessageBody((IHttpServer*)this, pSocketObj->connID, pData, iLength);}
	EnHandleResult FireWSMessageComplete(TSocketObj* pSocketObj)
		{return m_pListener->OnWSMessageComplete((IHttpServer*)this, pSocketObj->connID);}

	inline THttpObj* FindHttpObj(CONNID dwConnID);
	inline THttpObj* FindHttpObj(TSocketObj* pSocketObj);

	CCookieMgr* GetCookieMgr()						{return nullptr;}
	LPCSTR GetRemoteDomain(TSocketObj* pSocketObj)	{return nullptr;}

private:
	void KillDyingConnection();
	void ReleaseDyingConnection();

	UINT CleanerThreadProc(PVOID pv = nullptr);
	void WaitForCleanerThreadEnd();

public:
	CHttpServerT(IHttpServerListener* pListener)
	: T					(pListener)
	, m_pListener		(pListener)
	, m_enLocalVersion	(DEFAULT_HTTP_VERSION)
	, m_dwReleaseDelay	(DEFAULT_HTTP_RELEASE_DELAY)
	{

	}

	virtual ~CHttpServerT()
	{
		Stop();
	}

private:
	IHttpServerListener*		m_pListener;

	CEvt						m_evCleaner;
	CCleanThread				m_thCleaner;

	EnHttpVersion				m_enLocalVersion;
	DWORD						m_dwReleaseDelay;

	CCASQueue<TDyingConnection>	m_lsDyingQueue;

	CHttpObjPool				m_objPool;
};

// ------------------------------------------------------------------------------------------------------------- //

typedef CHttpServerT<CTcpServer, HTTP_DEFAULT_PORT> CHttpServer;

#ifdef _SSL_SUPPORT

#include "SSLServer.h"

typedef CHttpServerT<CSSLServer, HTTPS_DEFAULT_PORT> CHttpsServer;

#endif
