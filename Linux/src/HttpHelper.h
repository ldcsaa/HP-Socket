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

#include "SocketHelper.h"
#include "HttpCookie.h"

#ifdef _HTTP_SUPPORT

#include "common/http/http_parser.h"

/************************************************************************
名称：HTTP 全局常量
描述：声明 HTTP 组件的公共全局常量
************************************************************************/

#define HTTP_DEFAULT_PORT					80
#define HTTPS_DEFAULT_PORT					443

#define HTTP_SCHEMA							"http://"
#define HTTPS_SCHEMA						"https://"

#define HTTP_CRLF							"\r\n"
#define HTTP_PATH_SEPARATOR_CHAR			'/'
#define HTTP_PATH_SEPARATOR					"/"
#define HTTP_HEADER_SEPARATOR				": "
#define HTTP_COOKIE_SEPARATOR				"; "
#define HTTP_1_0_STR						"HTTP/1.0"
#define HTTP_1_1_STR						"HTTP/1.1"

#define HTTP_HEADER_HOST					"Host"
#define HTTP_HEADER_COOKIE					"Cookie"
#define HTTP_HEADER_SET_COOKIE				"Set-Cookie"
#define HTTP_HEADER_CONTENT_TYPE			"Content-Type"
#define HTTP_HEADER_CONTENT_LENGTH			"Content-Length"
#define HTTP_HEADER_CONTENT_ENCODING		"Content-Encoding"
#define HTTP_HEADER_TRANSFER_ENCODING		"Transfer-Encoding"
#define HTTP_HEADER_CONNECTION				"Connection"
#define HTTP_HEADER_UPGRADE					"Upgrade"
#define HTTP_HEADER_VALUE_WEB_SOCKET		"WebSocket"

#define HTTP_CONNECTION_CLOSE_VALUE			"close"
#define HTTP_CONNECTION_KEEPALIVE_VALUE		"keep-alive"

#define HTTP_METHOD_POST					"POST"
#define HTTP_METHOD_PUT						"PUT"
#define HTTP_METHOD_PATCH					"PATCH"
#define HTTP_METHOD_GET						"GET"
#define HTTP_METHOD_DELETE					"DELETE"
#define HTTP_METHOD_HEAD					"HEAD"
#define HTTP_METHOD_TRACE					"TRACE"
#define HTTP_METHOD_OPTIONS					"OPTIONS"
#define HTTP_METHOD_CONNECT					"CONNECT"

#define HTTP_MIN_WS_HEADER_LEN				2
#define HTTP_MAX_WS_HEADER_LEN				14

#define MIN_HTTP_RELEASE_CHECK_INTERVAL		((DWORD)1000)
#define MIN_HTTP_RELEASE_DELAY				100
#define MAX_HTTP_RELEASE_DELAY				(60 * 1000)
#define DEFAULT_HTTP_RELEASE_DELAY			(3 * 1000)
#define DEFAULT_HTTP_VERSION				HV_1_1

#define DEFAULT_HTTP_SYNC_CONNECT_TIMEOUT	5000
#define DEFAULT_HTTP_SYNC_REQUEST_TIMEOUT	10000

// ------------------------------------------------------------------------------------------------------------- //

enum EnHttpSyncRequestProgress
{
	HSRP_DONE,
	HSRP_WAITING,
	HSRP_ERROR,
	HSRP_CLOSE
};

struct TDyingConnection
{
	CONNID			connID;
	DWORD			killTime;

	TDyingConnection(CONNID id, DWORD kt = 0)
	: connID		(id)
	, killTime		(kt == 0 ? ::TimeGetTime() : kt)
	{

	}

	static TDyingConnection* Construct(CONNID id, DWORD kt = 0)	{return new TDyingConnection(id, kt);}
	static void Destruct(TDyingConnection* pObj)				{if(pObj) delete pObj;}

};

typedef unordered_multimap<CStringA, CStringA,
		cstringa_nc_hash_func::hash, cstringa_nc_hash_func::equal_to>	THeaderMap;
typedef THeaderMap::const_iterator										THeaderMapCI;
typedef THeaderMap::iterator											THeaderMapI;

typedef unordered_map<CStringA, CStringA,
		cstringa_hash_func::hash, cstringa_hash_func::equal_to>			TCookieMap;
typedef TCookieMap::const_iterator										TCookieMapCI;
typedef TCookieMap::iterator											TCookieMapI;

// ------------------------------------------------------------------------------------------------------------- //

struct TBaseWSHeader
{
public:
	BOOL fin()
	{
		return (data >> 7) & 0x1;
	}

	void set_fin(BOOL v)
	{
		data |= ((v ? 1 : 0) << 7);
	}

	BYTE rsv()
	{
		return (data >> 4) & 0x7;
	}

	void set_rsv(BYTE v)
	{
		data |= ((v & 0x7) << 4);
	}

	BYTE code()
	{
		return data & 0xF;
	}

	void set_code(BYTE v)
	{
		data |= (v & 0xF);
	}

	BOOL mask()
	{
		return (data >> 15) & 0x1;
	}

	void set_mask(BOOL v)
	{
		data |= ((v ? 1 : 0) << 15);
	}

	BYTE len()
	{
		return (data >> 8) & 0x7F;
	}

	void set_len(BYTE v)
	{
		data |= ((v & 0x7F) << 8);
	}

	USHORT extlen()
	{
		return ntohs((USHORT)(data >> 16));
	}

	void set_extlen(USHORT v)
	{
		data |= (htons(v) << 16);
	}

	TBaseWSHeader(const BYTE* p, BOOL bZero = FALSE)
	: data(*(UINT*)p)
	{
		if(bZero) data = 0;
	}

private:
	UINT& data;
};

template<class T> struct TWSContext
{
public:
	EnHandleResult Parse(const BYTE* pData, int iLength)
	{
		ASSERT(pData != nullptr && iLength > 0);

		EnHandleResult hr	= HR_OK;
		BYTE* pTemp			= (BYTE*)pData;
		int iRemain			= iLength;
		int iMin			= 0;

		while(iRemain > 0)
		{
			if(m_bHeader)
			{
				iMin = min(m_iHeaderRemain, iRemain);
				memcpy(m_szHeader + m_iHeaderLen - m_iHeaderRemain, pTemp, iMin);

				m_iHeaderRemain	-= iMin;

				if(m_iHeaderRemain == 0)
				{
					TBaseWSHeader bh(m_szHeader);

					int iLen			= bh.len();
					int iExtLen			= iLen < 126 ? 0 : (iLen == 126 ? 2 : 8);
					int iMaskLen		= bh.mask() ? 4 : 0;
					int iRealHeaderLen	= HTTP_MIN_WS_HEADER_LEN + iExtLen + iMaskLen;

					if(m_iHeaderLen < iRealHeaderLen)
					{
						m_iHeaderRemain	= iRealHeaderLen - m_iHeaderLen;
						m_iHeaderLen	= iRealHeaderLen;
					}
					else
					{
						m_ullBodyLen	= iExtLen == 0 ? iLen : (iExtLen == 2 ? bh.extlen() : NToH64(*(ULONGLONG*)(m_szHeader + HTTP_MIN_WS_HEADER_LEN)));
						m_ullBodyRemain	= m_ullBodyLen;
						m_lpszMask		= iMaskLen > 0 ? m_szHeader + HTTP_MIN_WS_HEADER_LEN + iExtLen : nullptr;

						hr = m_pHttpObj->on_ws_message_header(bh.fin(), bh.rsv(), bh.code(), m_lpszMask, m_ullBodyLen);

						if(hr == HR_ERROR)
							break;

						if(m_ullBodyLen > 0)
							m_bHeader = FALSE;
						else
						{
							hr = CompleteMessage();

							if(hr == HR_ERROR)
								break;
						}
					}
				}
			}
			else
			{
				iMin = (int)min(m_ullBodyRemain, (ULONGLONG)iRemain);

				if(m_lpszMask)
				{
					int iFactor = (m_ullBodyLen - m_ullBodyRemain) & 0x03;

					for(int i = 0; i < iMin; i++)
						pTemp[i] = pTemp[i] ^ m_lpszMask[(i + iFactor) & 0x03];
				}

				m_ullBodyRemain	-= iMin;

				EnHandleResult hr = m_pHttpObj->on_ws_message_body(pTemp, iMin);

				if(hr == HR_ERROR)
					break;

				if(m_ullBodyRemain == 0)
				{
					hr = CompleteMessage();

					if(hr == HR_ERROR)
						break;
				}
			}

			pTemp	+= iMin;
			iRemain	-= iMin;
		}

		return HR_OK;
	}

	BOOL GetMessageState(BOOL* lpbFinal, BYTE* lpiReserved, BYTE* lpiOperationCode, LPCBYTE* lpszMask, ULONGLONG* lpullBodyLen, ULONGLONG* lpullBodyRemain)
	{
		TBaseWSHeader bh(m_szHeader);

		if(lpbFinal)			*lpbFinal			= bh.fin();
		if(lpiReserved)			*lpiReserved		= bh.rsv();
		if(lpiOperationCode)	*lpiOperationCode	= bh.code();
		if(lpszMask)			*lpszMask			= m_lpszMask;
		if(lpullBodyLen)		*lpullBodyLen		= m_ullBodyLen;
		if(lpullBodyRemain)		*lpullBodyRemain	= m_ullBodyRemain;

		return TRUE;
	}

	BOOL CopyData(const TWSContext& src)
	{
		if(&src == this)
			return FALSE;

		memcpy(m_szHeader, src.m_szHeader, HTTP_MAX_WS_HEADER_LEN);

		if(src.m_lpszMask)
			m_lpszMask = m_szHeader + (src.m_lpszMask - src.m_szHeader);
		else
			m_lpszMask = nullptr;

		m_ullBodyLen	= src.m_ullBodyLen;
		m_ullBodyRemain	= src.m_ullBodyRemain;

		return TRUE;
	}

public:
	TWSContext(T* pHttpObj) : m_pHttpObj(pHttpObj)
	{
		Reset();
	}

private:
	EnHandleResult CompleteMessage()
	{
		EnHandleResult hr = m_pHttpObj->on_ws_message_complete();

		Reset();

		return hr;
	}

	void Reset()
	{
		m_bHeader		= TRUE;
		m_lpszMask		= nullptr;
		m_iHeaderLen	= HTTP_MIN_WS_HEADER_LEN;
		m_iHeaderRemain	= HTTP_MIN_WS_HEADER_LEN;
		m_ullBodyLen	= 0;
		m_ullBodyRemain	= 0;
	}

private:
	T* m_pHttpObj;

	BYTE m_szHeader[HTTP_MAX_WS_HEADER_LEN];
	const BYTE* m_lpszMask;

	BOOL m_bHeader;
	int m_iHeaderLen;
	int m_iHeaderRemain;
	ULONGLONG m_ullBodyLen;
	ULONGLONG m_ullBodyRemain;
};

// ------------------------------------------------------------------------------------------------------------- //

/* Http 上下文结构 */
template<class T, class S> struct THttpObjT
{
public:
	EnHandleResult Execute(const BYTE* pData, int iLength)
	{
		ASSERT(pData != nullptr && iLength > 0);

		if(m_parser.upgrade)
		{
			if(m_enUpgrade == HUT_WEB_SOCKET)
				return m_pwsContext->Parse(pData, iLength);
			else
				return m_pContext->DoFireSuperReceive(m_pSocket, pData, iLength);
		}

		EnHandleResult hr = HR_OK;
		int iPased		  = (int)::http_parser_execute(&m_parser, &sm_settings, (LPCSTR)pData, iLength);

		if(m_parser.upgrade)
			hr = Upgrade(pData, iLength, iPased);
		else if(m_parser.http_errno != HPE_OK)
		{
			m_pContext->FireParseError(m_pSocket, m_parser.http_errno, ::http_errno_description(HTTP_PARSER_ERRNO(&m_parser)));
			hr = HR_ERROR;
		}
		else
			ASSERT(iPased == iLength);

		return hr;
	}

	void CheckBodyIdentityEof()
	{
		if(m_parser.state == s_body_identity_eof && !m_parser.upgrade)
			::http_parser_execute(&m_parser, &sm_settings, nullptr, 0);
	}

	static int on_message_begin(http_parser* p)
	{
		THttpObjT* pSelf = Self(p);

		pSelf->ResetHeaderState(FALSE, FALSE);

		return pSelf->m_pContext->FireMessageBegin(pSelf->m_pSocket);
	}

	static int on_url(http_parser* p, const char* at, size_t length)
	{
		EnHttpParseResult hpr	= HPR_OK;
		THttpObjT* pSelf		= Self(p);

		pSelf->AppendBuffer(at, length);

		if(p->state != s_req_http_start)
			return hpr;

		hpr = pSelf->ParseUrl();

		if(hpr == HPR_OK)
			hpr = pSelf->m_pContext->FireRequestLine(pSelf->m_pSocket, ::http_method_str((http_method)p->method), pSelf->GetBuffer());

		pSelf->ResetBuffer();

		return hpr;
	}

	static int on_status(http_parser* p, const char* at, size_t length)
	{
		EnHttpParseResult hpr	= HPR_OK;
		THttpObjT* pSelf		= Self(p);

		pSelf->AppendBuffer(at, length);

		if(p->state != s_res_line_almost_done)
			return hpr;

		hpr = pSelf->m_pContext->FireStatusLine(pSelf->m_pSocket, p->status_code, pSelf->GetBuffer());
		pSelf->ResetBuffer();

		return hpr;
	}

	static int on_header_field(http_parser* p, const char* at, size_t length)
	{
		EnHttpParseResult hpr	= HPR_OK;
		THttpObjT* pSelf		= Self(p);

		pSelf->AppendBuffer(at, length);

		if(p->state != s_header_value_discard_ws)
			return hpr;

		pSelf->m_strCurHeader = pSelf->GetBuffer();
		pSelf->ResetBuffer();

		return hpr;
	}

	static int on_header_value(http_parser* p, const char* at, size_t length)
	{
		EnHttpParseResult hpr	= HPR_OK;
		THttpObjT* pSelf		= Self(p);

		pSelf->AppendBuffer(at, length);

		if(p->state != s_header_almost_done && p->state != s_header_field_start)
			return hpr;

		pSelf->m_headers.emplace(move(THeaderMap::value_type(pSelf->m_strCurHeader, pSelf->GetBuffer())));
		hpr = pSelf->m_pContext->FireHeader(pSelf->m_pSocket, pSelf->m_strCurHeader, pSelf->GetBuffer());

		if(hpr != HPR_ERROR)
		{
			if(pSelf->m_bRequest && pSelf->m_strCurHeader == HTTP_HEADER_COOKIE)
				hpr = pSelf->ParseCookie();
			else if(!pSelf->m_bRequest && pSelf->m_strCurHeader == HTTP_HEADER_SET_COOKIE)
				hpr = pSelf->ParseSetCookie();
		}

		pSelf->ResetBuffer();

		return hpr;
	}

	static int on_headers_complete(http_parser* p)
	{
		THttpObjT* pSelf = Self(p);

		pSelf->CheckUpgrade();
		pSelf->ResetHeaderBuffer();

		EnHttpParseResult rs = pSelf->m_pContext->FireHeadersComplete(pSelf->m_pSocket);

		if(!pSelf->m_bRequest && pSelf->GetMethodInt() == HTTP_HEAD && rs == HPR_OK)
			rs = HPR_SKIP_BODY;

		return rs;
	}

	static int on_body(http_parser* p, const char* at, size_t length)
	{
		THttpObjT* pSelf = Self(p);

		return pSelf->m_pContext->FireBody(pSelf->m_pSocket, (const BYTE*)at, (int)length);
	}

	static int on_chunk_header(http_parser* p)
	{
		THttpObjT* pSelf = Self(p);

		if(p->state == s_chunk_data || p->state == s_header_field_start)
			return pSelf->m_pContext->FireChunkHeader(pSelf->m_pSocket, (int)p->content_length);

		return HPR_OK;
	}

	static int on_chunk_complete(http_parser* p)
	{
		THttpObjT* pSelf = Self(p);

		if(p->state == s_headers_done || p->state == s_message_done)
			return pSelf->m_pContext->FireChunkComplete(pSelf->m_pSocket);

		return HPR_OK;
	}

	static int on_message_complete(http_parser* p)
	{
		THttpObjT* pSelf		= Self(p);
		EnHttpParseResult hpr	= pSelf->m_pContext->FireMessageComplete(pSelf->m_pSocket);

		return hpr;
	}

	EnHandleResult on_ws_message_header(BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
	{
		return m_pContext->FireWSMessageHeader(m_pSocket, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen);
	}

	EnHandleResult on_ws_message_body(const BYTE* pData, int iLength)
	{
		return m_pContext->FireWSMessageBody(m_pSocket, pData, iLength);
	}

	EnHandleResult on_ws_message_complete()
	{
		return m_pContext->FireWSMessageComplete(m_pSocket);
	}

private:

	EnHandleResult Upgrade(const BYTE* pData, int iLength, int iPased)
	{
		ASSERT(m_parser.upgrade);

		if(m_pContext->FireUpgrade(m_pSocket, m_enUpgrade) != HPR_OK)
			return HR_ERROR;

		ResetHeaderState();

		if(m_enUpgrade == HUT_WEB_SOCKET)
			m_pwsContext = new TWSContext<THttpObjT<T, S>>(this);

		if(iPased < iLength)
			return Execute(pData + iPased, iLength - iPased);

		return HR_OK;
	}

	void CheckUpgrade()
	{
		if(!m_parser.upgrade)
			return;

		if(m_bRequest && m_parser.method == HTTP_CONNECT)
			m_enUpgrade = HUT_HTTP_TUNNEL;
		else
		{
			LPCSTR lpszValue;
			if(GetHeader(HTTP_HEADER_UPGRADE, &lpszValue) && stricmp(HTTP_HEADER_VALUE_WEB_SOCKET, lpszValue) == 0)
				m_enUpgrade = HUT_WEB_SOCKET;
			else
				m_enUpgrade = HUT_UNKNOWN;
		}
	}

	EnHttpParseResult ParseUrl()
	{
		http_parser_url url = {0};

		BOOL isConnect	= m_parser.method == HTTP_CONNECT;
		int rs			= ::http_parser_parse_url(m_strBuffer, m_strBuffer.GetLength(), isConnect, &url);

		if(rs != HPE_OK)
		{
			m_parser.http_errno = HPE_INVALID_URL;
			return HPR_ERROR;
		}

		m_usUrlFieldSet		= url.field_set;
		LPCSTR lpszBuffer	= m_strBuffer;

		for(int i = 0; i < UF_MAX; i++)
		{
			if((url.field_set & (1 << i)) != 0)
				m_pstrUrlFileds[i].SetString((lpszBuffer + url.field_data[i].off), url.field_data[i].len);
		}

		return HPR_OK;
	}

	EnHttpParseResult ParseCookie()
	{
		int i = 0;

		do 
		{
			CStringA tk = m_strBuffer.Tokenize(COOKIE_FIELD_SEP, i);

			if(i == -1)
				break;

			int j = tk.Trim().Find(COOKIE_KV_SEP_CHAR);

			if(j <= 0)
				continue;
			/*
			{
				m_parser.http_errno = HPE_INVALID_HEADER_TOKEN;
				return HPR_ERROR;
			}
			*/

			AddCookie(tk.Left(j), tk.Mid(j + 1));

		} while(TRUE);

		return HPR_OK;
	}

	EnHttpParseResult ParseSetCookie()
	{
		CCookieMgr* pCookieMgr = m_pContext->GetCookieMgr();

		if(pCookieMgr == nullptr)
			return HPR_OK;

		LPCSTR lpszDomain	= GetDomain();
		LPCSTR lpszPath		= GetPath();

		unique_ptr<CCookie> pCookie(CCookie::FromString(m_strBuffer, lpszDomain, lpszPath));

		if(pCookie == nullptr)
			return HPR_ERROR;

		if(pCookie->Match(lpszDomain, lpszPath, TRUE, m_pContext->IsSecure()))
		{
			if(pCookie->IsExpired())
				DeleteCookie(pCookie->name);
			else
				AddCookie(pCookie->name, pCookie->value);
		}

		if(pCookieMgr->IsEnableThirdPartyCookie() || pCookie->IsSameDomain(lpszDomain))
			pCookieMgr->SetCookie(*pCookie);

		return HPR_OK;
	}

public:
	DWORD GetFreeTime() const		{return m_dwFreeTime;}
	void SetFree()					{m_dwFreeTime = ::TimeGetTime();}

	BOOL IsUpgrade()				{return m_parser.upgrade;}
	BOOL IsKeepAlive()				{return ::http_should_keep_alive(&m_parser);}
	USHORT GetVersion()				{return MAKEWORD(m_parser.http_major, m_parser.http_minor);}
	ULONGLONG GetContentLength()	{return m_parser.content_length;}

	int GetMethodInt()				{return m_bRequest ? m_parser.method : m_sRequestMethod;}
	LPCSTR GetMethod()				{return ::http_method_str((http_method)GetMethodInt());}
	USHORT GetUrlFieldSet()			{return m_usUrlFieldSet;}
	USHORT GetStatusCode()			{return m_parser.status_code;}

	EnHttpUpgradeType GetUpgradeType()	{return m_enUpgrade;}

	THeaderMap& GetHeaderMap()		{return m_headers;}
	TCookieMap& GetCookieMap()		{return m_cookies;}

	BOOL HasReleased()				{return m_bReleased;}
	void Release()					{m_bReleased = TRUE;}

	LPCSTR GetContentType()
	{
		LPCSTR lpszValue = nullptr;
		GetHeader(HTTP_HEADER_CONTENT_TYPE, &lpszValue);

		return lpszValue;
	}

	LPCSTR GetContentEncoding()
	{
		LPCSTR lpszValue = nullptr;
		GetHeader(HTTP_HEADER_CONTENT_ENCODING, &lpszValue);

		return lpszValue;
	}

	LPCSTR GetTransferEncoding()
	{
		LPCSTR lpszValue = nullptr;
		GetHeader(HTTP_HEADER_TRANSFER_ENCODING, &lpszValue);

		return lpszValue;
	}

	LPCSTR GetHost()
	{
		LPCSTR lpszValue = nullptr;
		GetHeader(HTTP_HEADER_HOST, &lpszValue);

		return lpszValue;
	}

	USHORT GetParseErrorCode(LPCSTR* lpszErrorDesc = nullptr)
	{
		if(lpszErrorDesc)
			*lpszErrorDesc = ::http_errno_description(HTTP_PARSER_ERRNO(&m_parser));

		return m_parser.http_errno;
	}

	LPCSTR GetUrlField(EnHttpUrlField enField)
	{
		ASSERT(m_bRequest && enField < HUF_MAX);

		if(!m_bRequest || enField >= HUF_MAX)
			return nullptr;

		return m_pstrUrlFileds[enField];
	}

	LPCSTR GetPath()
	{
		if(m_bRequest)
			return GetUrlField(HUF_PATH);
		else
			return *m_pstrRequestPath;
	}

	LPCSTR GetDomain()
	{
		ASSERT(!m_bRequest);

		return m_pContext->GetRemoteDomain(m_pSocket);
	}

	LPCSTR GetRequestPath()
	{
		if(m_bRequest)
			return nullptr;

		return *m_pstrRequestPath;
	}

	void SetRequestPath(LPCSTR lpszMethod, LPCSTR lpszPath)
	{
		ASSERT(!m_bRequest);

		if(m_bRequest)
			return;

		*m_pstrRequestPath = lpszPath;

		if(stricmp(lpszMethod, HTTP_METHOD_GET) == 0)
			m_sRequestMethod = HTTP_GET;
		else if(stricmp(lpszMethod, HTTP_METHOD_POST) == 0)
			m_sRequestMethod = HTTP_POST;
		else if(stricmp(lpszMethod, HTTP_METHOD_PUT) == 0)
			m_sRequestMethod = HTTP_PUT;
		else if(stricmp(lpszMethod, HTTP_METHOD_DELETE) == 0)
			m_sRequestMethod = HTTP_DELETE;
		else if(stricmp(lpszMethod, HTTP_METHOD_HEAD) == 0)
			m_sRequestMethod = HTTP_HEAD;
		else if(stricmp(lpszMethod, HTTP_METHOD_PATCH) == 0)
			m_sRequestMethod = HTTP_PATCH;
		else if(stricmp(lpszMethod, HTTP_METHOD_TRACE) == 0)
			m_sRequestMethod = HTTP_TRACE;
		else if(stricmp(lpszMethod, HTTP_METHOD_OPTIONS) == 0)
			m_sRequestMethod = HTTP_OPTIONS;
		else if(stricmp(lpszMethod, HTTP_METHOD_CONNECT) == 0)
			m_sRequestMethod = HTTP_CONNECT;
		else
			m_sRequestMethod = -1;
	}

	BOOL GetHeader(LPCSTR lpszName, LPCSTR* lpszValue)
	{
		ASSERT(lpszName);

		BOOL isOK		= FALSE;
		THeaderMapCI it = m_headers.find(lpszName);

		if(it != m_headers.end())
		{
			*lpszValue	= it->second;
			isOK		= TRUE;
		}

		return isOK;
	}

	BOOL GetHeaders(LPCSTR lpszName, LPCSTR lpszValue[], DWORD& dwCount)
	{
		ASSERT(lpszName);

		if(lpszValue == nullptr || dwCount == 0)
		{
			dwCount = (DWORD)m_headers.count(lpszName);
			return FALSE;
		}

		pair<THeaderMapCI, THeaderMapCI> range = m_headers.equal_range(lpszName);

		THeaderMapCI it	= range.first;
		DWORD dwIndex	= 0;

		while(it != range.second)
		{
			if(dwIndex < dwCount)
				lpszValue[dwIndex] = it->second;

			++dwIndex;
			++it;
		}

		BOOL isOK	= (dwIndex > 0 && dwIndex <= dwCount);
		dwCount		= dwIndex;

		return isOK;
	}

	BOOL GetAllHeaders(THeader lpHeaders[], DWORD& dwCount)
	{
		DWORD dwSize = (DWORD)m_headers.size();

		if(lpHeaders == nullptr || dwCount == 0 || dwSize == 0 || dwSize > dwCount)
		{
			dwCount = dwSize;
			return FALSE;
		}

		DWORD dwIndex = 0;

		for(THeaderMapCI it = m_headers.begin(), end = m_headers.end(); it != end; ++it, ++dwIndex)
		{
			lpHeaders[dwIndex].name  = it->first;
			lpHeaders[dwIndex].value = it->second;
		}

		dwCount = dwSize;
		return TRUE;
	}

	BOOL GetAllHeaderNames(LPCSTR lpszName[], DWORD& dwCount)
	{
		DWORD dwSize = (DWORD)m_headers.size();

		if(lpszName == nullptr || dwCount == 0 || dwSize == 0 || dwSize > dwCount)
		{
			dwCount = dwSize;
			return FALSE;
		}

		DWORD dwIndex = 0;

		for(THeaderMapCI it = m_headers.begin(), end = m_headers.end(); it != end; ++it, ++dwIndex)
			lpszName[dwIndex] = it->first;

		dwCount = dwSize;
		return TRUE;
	}

	BOOL AddCookie(LPCSTR lpszName, LPCSTR lpszValue, BOOL bRelpace = TRUE)
	{
		ASSERT(lpszName);

		TCookieMapI it = m_cookies.find(lpszName);

		if(it == m_cookies.end())
			return m_cookies.emplace(move(TCookieMap::value_type(lpszName, lpszValue))).second;

		BOOL isOK = FALSE;

		if(bRelpace)
		{
			it->second	= lpszValue;
			isOK		= TRUE;
		}

		return isOK;
	}

	BOOL DeleteCookie(LPCSTR lpszName)
	{
		ASSERT(lpszName);

		return m_cookies.erase(lpszName) > 0;
	}

	void DeleteAllCookies()
	{
		m_cookies.clear();
	}

	BOOL GetCookie(LPCSTR lpszName, LPCSTR* lpszValue)
	{
		ASSERT(lpszName);

		BOOL isOK		= FALSE;
		TCookieMapCI it = m_cookies.find(lpszName);

		if(it != m_cookies.end())
		{
			*lpszValue	= it->second;
			isOK		= TRUE;
		}

		return isOK;
	}

	BOOL GetAllCookies(TCookie lpCookies[], DWORD& dwCount)
	{
		DWORD dwSize = (DWORD)m_cookies.size();

		if(lpCookies == nullptr || dwCount == 0 || dwSize == 0 || dwSize > dwCount)
		{
			dwCount = dwSize;
			return FALSE;
		}

		DWORD dwIndex = 0;

		for(TCookieMapCI it = m_cookies.begin(), end = m_cookies.end(); it != end; ++it, ++dwIndex)
		{
			lpCookies[dwIndex].name  = it->first;
			lpCookies[dwIndex].value = it->second;
		}

		dwCount = dwSize;
		return TRUE;
	}

	BOOL ReloadCookies()
	{
		CCookieMgr* pCookieMgr = m_pContext->GetCookieMgr();

		if(pCookieMgr == nullptr)
			return TRUE;

		DeleteAllCookies();

		CCookieSet cookies;

		if(!pCookieMgr->GetCookies(cookies, GetDomain(), GetPath(), TRUE, m_pContext->IsSecure()))
			return FALSE;

		for(CCookieSetCI it = cookies.begin(), end = cookies.end(); it != end; ++it)
			AddCookie(it->name, it->value);

		return TRUE;
	}

	BOOL GetWSMessageState(BOOL* lpbFinal, BYTE* lpiReserved, BYTE* lpiOperationCode, LPCBYTE* lpszMask, ULONGLONG* lpullBodyLen, ULONGLONG* lpullBodyRemain)
	{
		if(!m_pwsContext)
			return FALSE;

		return m_pwsContext->GetMessageState(lpbFinal, lpiReserved, lpiOperationCode, lpszMask, lpullBodyLen, lpullBodyRemain);
	}

public:
	THttpObjT			(BOOL bRequest, T* pContext, S* pSocket)
	: m_pContext		(pContext)
	, m_pSocket			(pSocket)
	, m_bRequest		(bRequest)
	, m_bReleased		(FALSE)
	, m_dwFreeTime		(0)
	, m_usUrlFieldSet	(m_bRequest ? 0 : -1)
	, m_pstrUrlFileds	(nullptr)
	, m_enUpgrade		(HUT_NONE)
	, m_pwsContext		(nullptr)
	{
		if(m_bRequest)
			m_pstrUrlFileds	  = new CStringA[HUF_MAX];
		else
			m_pstrRequestPath = new CStringA;

		ResetParser();
	}

	~THttpObjT()
	{
		if(m_bRequest)
			delete[] m_pstrUrlFileds;
		else
			delete m_pstrRequestPath;

		ReleaseWSContext();
	}

	static THttpObjT* Construct(BOOL bRequest, T* pContext, S* pSocket)
		{return new THttpObjT(bRequest, pContext, pSocket);}

	static void Destruct(THttpObjT* pHttpObj)
		{if(pHttpObj) delete pHttpObj;}

	void Reset()
	{
		ResetParser();
		ResetHeaderState();
		ReleaseWSContext();

		m_bReleased  = FALSE;
		m_enUpgrade  = HUT_NONE;
		m_dwFreeTime = 0;
	}

	void Renew(T* pContext, S* pSocket)
	{
		m_pContext	= pContext;
		m_pSocket	= pSocket;

		Reset();
	}

	BOOL CopyData(const THttpObjT& src)
	{
		if(&src == this)
			return FALSE;
		if(m_bRequest != src.m_bRequest)
			return FALSE;

		void* p			= m_parser.data;
		m_parser		= src.m_parser;
		m_parser.data	= p;

		m_headers = src.m_headers;
		m_cookies = src.m_cookies;

		if(m_bRequest)
		{
			m_usUrlFieldSet = src.m_usUrlFieldSet;

			for(int i = 0;i < HUF_MAX; i++)
				m_pstrUrlFileds[i] = src.m_pstrUrlFileds[i];
		}
		else
		{
			m_sRequestMethod	= src.m_sRequestMethod;
			*m_pstrRequestPath	= *src.m_pstrRequestPath;
		}

		m_enUpgrade = src.m_enUpgrade;

		return TRUE;
	}

	BOOL CopyWSContext(const THttpObjT& src)
	{
		if(&src == this)
			return FALSE;
		if(m_bRequest != src.m_bRequest)
			return FALSE;

		if(!src.m_pwsContext && !m_pwsContext)
			;
		else if(!src.m_pwsContext && m_pwsContext)
		{
			delete m_pwsContext;
			m_pwsContext = nullptr;
		}
		else
		{
			if(!m_pwsContext)
				m_pwsContext = new TWSContext<THttpObjT<T, S>>(this);

			m_pwsContext->CopyData(*src.m_pwsContext);
		}

		return TRUE;
	}

private:

	void ResetParser()
	{
		::http_parser_init(&m_parser, m_bRequest ? HTTP_REQUEST : HTTP_RESPONSE);
		m_parser.data = this;		
	}

	void ResetHeaderState(BOOL bClearCookies = TRUE, BOOL bResetRequestData = TRUE)
	{
		if(m_bRequest)
		{
			if(m_usUrlFieldSet != 0)
			{
				m_usUrlFieldSet = 0;

				for(int i = 0; i < HUF_MAX; i++)
					m_pstrUrlFileds[i].Empty();
			}
		}
		else
		{
			if(bResetRequestData)
			{
				m_sRequestMethod = -1;
				m_pstrRequestPath->Empty();
			}
		}

		if(m_bRequest || bClearCookies)
			DeleteAllCookies();
			
		m_headers.clear();
		ResetHeaderBuffer();
	}

	void ResetHeaderBuffer()
	{
		ResetBuffer();
		m_strCurHeader.Empty();
	}

	void ReleaseWSContext()
	{
		if(m_pwsContext)
		{
			delete m_pwsContext;
			m_pwsContext = nullptr;
		}
	}

	void AppendBuffer(const char* at, size_t length)	{m_strBuffer.Append(at, (int)length);}
	void ResetBuffer()									{m_strBuffer.Empty();}
	LPCSTR GetBuffer()									{return m_strBuffer;}

	static THttpObjT* Self(http_parser* p)				{return (THttpObjT*)(p->data);}
	static T* SelfContext(http_parser* p)				{return Self(p)->m_pContext;}
	static S* SelfSocketObj(http_parser* p)				{return Self(p)->m_pSocket;}

private:
	BOOL		m_bRequest;
	BOOL		m_bReleased;
	T*			m_pContext;
	S*			m_pSocket;
	http_parser	m_parser;
	THeaderMap	m_headers;
	TCookieMap	m_cookies;
	CStringA	m_strBuffer;
	CStringA	m_strCurHeader;

	union
	{
		USHORT		m_usUrlFieldSet;
		short		m_sRequestMethod;
	};

	union
	{
		CStringA* m_pstrUrlFileds;
		CStringA* m_pstrRequestPath;
	};

	EnHttpUpgradeType	m_enUpgrade;
	DWORD				m_dwFreeTime;

	TWSContext<THttpObjT<T, S>>* m_pwsContext;

	static http_parser_settings sm_settings;
};

template<class T, class S> http_parser_settings THttpObjT<T, S>::sm_settings = 
{
	on_message_begin,
	on_url,
	on_status,
	on_header_field,
	on_header_value,
	on_headers_complete,
	on_body,
	on_message_complete,
	on_chunk_header,
	on_chunk_complete
};

// ------------------------------------------------------------------------------------------------------------- //

template<BOOL is_request, class T, class S> class CHttpObjPoolT
{
	typedef THttpObjT<T, S>		THttpObj;
	typedef CRingPool<THttpObj>	TSSLHttpObjList;
	typedef CCASQueue<THttpObj>	TSSLHttpObjQueue;

public:
	THttpObj* PickFreeHttpObj(T* pContext, S* pSocket)
	{
		DWORD dwIndex;
		THttpObj* pHttpObj = nullptr;

		if(m_lsFreeHttpObj.TryLock(&pHttpObj, dwIndex))
		{
			if(::GetTimeGap32(pHttpObj->GetFreeTime()) >= m_dwHttpObjLockTime)
				VERIFY(m_lsFreeHttpObj.ReleaseLock(nullptr, dwIndex));
			else
			{
				VERIFY(m_lsFreeHttpObj.ReleaseLock(pHttpObj, dwIndex));
				pHttpObj = nullptr;
			}
		}

		if(pHttpObj)
			pHttpObj->Renew(pContext, pSocket);
		else
		{
			pHttpObj = THttpObj::Construct(is_request, pContext, pSocket);
			ASSERT(pHttpObj);
		}
		
		return pHttpObj;
	}

	void PutFreeHttpObj(THttpObj* pHttpObj)
	{
		pHttpObj->SetFree();

		ReleaseGCHttpObj();

		if(!m_lsFreeHttpObj.TryPut(pHttpObj))
			m_lsGCHttpObj.PushBack(pHttpObj);
	}

	void Prepare()
	{
		m_lsFreeHttpObj.Reset(m_dwHttpObjPoolSize);
	}

	void Clear()
	{
		THttpObj* pHttpObj = nullptr;

		while(m_lsFreeHttpObj.TryGet(&pHttpObj))
			THttpObj::Destruct(pHttpObj);

		VERIFY(m_lsFreeHttpObj.IsEmpty());
		m_lsFreeHttpObj.Reset();

		ReleaseGCHttpObj(TRUE);
		VERIFY(m_lsGCHttpObj.IsEmpty());
	}

private:
	void ReleaseGCHttpObj(BOOL bForce = FALSE)
	{
		::ReleaseGCObj(m_lsGCHttpObj, m_dwHttpObjLockTime, bForce);
	}

public:
	void SetHttpObjLockTime	(DWORD dwHttpObjLockTime)	{m_dwHttpObjLockTime = dwHttpObjLockTime;}
	void SetHttpObjPoolSize	(DWORD dwHttpObjPoolSize)	{m_dwHttpObjPoolSize = dwHttpObjPoolSize;}
	void SetHttpObjPoolHold	(DWORD dwHttpObjPoolHold)	{m_dwHttpObjPoolHold = dwHttpObjPoolHold;}

	DWORD GetHttpObjLockTime()	{return m_dwHttpObjLockTime;}
	DWORD GetHttpObjPoolSize()	{return m_dwHttpObjPoolSize;}
	DWORD GetHttpObjPoolHold()	{return m_dwHttpObjPoolHold;}

public:
	CHttpObjPoolT(	DWORD dwPoolSize = DEFAULT_HTTPOBJ_POOL_SIZE,
					DWORD dwPoolHold = DEFAULT_HTTPOBJ_POOL_HOLD,
					DWORD dwLockTime = DEFAULT_HTTPOBJ_LOCK_TIME)
	: m_dwHttpObjPoolSize(dwPoolSize)
	, m_dwHttpObjPoolHold(dwPoolHold)
	, m_dwHttpObjLockTime(dwLockTime)
	{

	}

	~CHttpObjPoolT()	{Clear();}

	DECLARE_NO_COPY_CLASS(CHttpObjPoolT)

public:
	static const DWORD DEFAULT_HTTPOBJ_LOCK_TIME;
	static const DWORD DEFAULT_HTTPOBJ_POOL_SIZE;
	static const DWORD DEFAULT_HTTPOBJ_POOL_HOLD;

private:
	DWORD				m_dwHttpObjLockTime;
	DWORD				m_dwHttpObjPoolSize;
	DWORD				m_dwHttpObjPoolHold;

	TSSLHttpObjList		m_lsFreeHttpObj;
	TSSLHttpObjQueue	m_lsGCHttpObj;
};

template<BOOL is_request, class T, class S> const DWORD CHttpObjPoolT<is_request, T, S>::DEFAULT_HTTPOBJ_LOCK_TIME	= 15 * 1000;
template<BOOL is_request, class T, class S> const DWORD CHttpObjPoolT<is_request, T, S>::DEFAULT_HTTPOBJ_POOL_SIZE	= 600;
template<BOOL is_request, class T, class S> const DWORD CHttpObjPoolT<is_request, T, S>::DEFAULT_HTTPOBJ_POOL_HOLD	= 600;

// ------------------------------------------------------------------------------------------------------------- //

extern CStringA& GetHttpVersionStr(EnHttpVersion enVersion, CStringA& strResult);
extern CStringA& AdjustRequestPath(BOOL bConnect, LPCSTR lpszPath, CStringA& strPath);
extern LPCSTR GetHttpDefaultStatusCodeDesc(EnHttpStatusCode enCode);
extern void MakeRequestLine(LPCSTR lpszMethod, LPCSTR lpszPath, EnHttpVersion enVersion, CStringA& strValue);
extern void MakeStatusLine(EnHttpVersion enVersion, USHORT usStatusCode, LPCSTR lpszDesc, CStringA& strValue);
extern void MakeHeaderLines(const THeader lpHeaders[], int iHeaderCount, const TCookieMap* pCookies, int iBodyLength, BOOL bRequest, int iConnFlag, LPCSTR lpszDefaultHost, USHORT usPort, CStringA& strValue);
extern void MakeHttpPacket(const CStringA& strHeader, const BYTE* pBody, int iLength, WSABUF szBuffer[2]);
extern BOOL MakeWSPacket(BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], BYTE* pData, int iLength, ULONGLONG ullBodyLen, BYTE szHeader[HTTP_MAX_WS_HEADER_LEN], WSABUF szBuffer[2]);
extern BOOL ParseUrl(const CStringA& strUrl, BOOL& bHttps, CStringA& strHost, USHORT& usPort, CStringA& strPath);

#endif