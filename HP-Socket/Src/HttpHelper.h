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

#include "SocketHelper.h"
#include "../../Common/Src/http/http_parser.h"

/************************************************************************
名称：HTTP 全局常量
描述：声明 HTTP 组件的公共全局常量
************************************************************************/

#define CRLF						"\r\n"
#define NV_SEPARATOR_CHAR			'='
#define HEADER_SEPARATOR			": "
#define COOKIE_TOKENIZE				"; "
#define STR_HTTP_1_0				"HTTP/1.0"
#define STR_HTTP_1_1				"HTTP/1.1"
#define HOST_HEADER					"Host"
#define COOKIE_HEADER				"Cookie"
#define SET_COOKIE_HEADER			"Set-Cookie"
#define CONTENT_TYPE_HEADER			"Content-Type"
#define CONTENT_LENGTH_HEADER		"Content-Length"
#define CONTENT_ENCODING_HEADER		"Content-Encoding"
#define TRANSFER_ENCODING_HEADER	"Transfer-Encoding"
#define UPGRADE_HEADER				"Upgrade"
#define WEB_SOCKET_HEADER_VALUE		"WebSocket"

#define HTTP_METHOD_POST			"POST"
#define HTTP_METHOD_PUT				"PUT"
#define HTTP_METHOD_PATCH			"PATCH"
#define HTTP_METHOD_GET				"GET"
#define HTTP_METHOD_DELETE			"DELETE"
#define HTTP_METHOD_HEAD			"HEAD"
#define HTTP_METHOD_TRACE			"TRACE"
#define HTTP_METHOD_OPTIONS			"OPTIONS"
#define HTTP_METHOD_CONNECT			"CONNECT"


extern const DWORD MIN_HTTP_RELEASE_CHECK_INTERVAL;
extern const DWORD MIN_HTTP_RELEASE_DELAY;
extern const DWORD MAX_HTTP_RELEASE_DELAY;
extern const DWORD DEFAULT_HTTP_RELEASE_DELAY;
extern const EnHttpVersion DEFAULT_HTTP_VERSION;


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

struct str_hash_func
{
	struct hash
	{
		size_t operator() (const char* p) const
		{
			return hash_value(p);
		}
	};

	struct equal_to
	{
		bool operator () (const char* p1, const char* p2) const
		{
			return strcmp(p1, p2) == 0;
		}
	};

};

struct cstringa_hash_func
{
	struct hash
	{
		size_t operator() (const CStringA& str) const
		{
			size_t s = hash_value((LPCSTR)str);
			return s;
		}
	};

	struct equal_to
	{
		bool operator () (const CStringA& strA, const CStringA& strB) const
		{
			return strA == strB;
		}
	};

};

typedef unordered_multimap<CStringA, CStringA,
		cstringa_hash_func::hash, cstringa_hash_func::equal_to>	THeaderMap;
typedef THeaderMap::const_iterator								THeaderMapCI;
typedef THeaderMap::iterator									THeaderMapI;

typedef unordered_map<CStringA, CStringA,
		cstringa_hash_func::hash, cstringa_hash_func::equal_to>	TCookieMap;
typedef TCookieMap::const_iterator								TCookieMapCI;
typedef TCookieMap::iterator									TCookieMapI;

/* Http 缓冲区结构 */
template<class T, class S> struct THttpObjT
{
public:
	EnHandleResult Execute(const BYTE* pData, int iLength)
	{
		ASSERT(pData != nullptr && iLength > 0);

		if(m_parser.upgrade)
			return m_pContext->DoFireSuperReceive(m_pSocket, pData, iLength);

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

	static int on_message_begin(http_parser* p)
	{
		THttpObjT* pSelf = Self(p);

		pSelf->ResetHeaderState();

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

		if(pSelf->m_bRequest && pSelf->m_strCurHeader == COOKIE_HEADER)
			hpr = pSelf->ParseCookie();
		else
		{
			pSelf->m_headers.emplace(move(THeaderMap::value_type(pSelf->m_strCurHeader, pSelf->GetBuffer())));
			hpr = pSelf->m_pContext->FireHeader(pSelf->m_pSocket, pSelf->m_strCurHeader, pSelf->GetBuffer());
		}

		pSelf->ResetBuffer();

		return hpr;
	}

	static int on_headers_complete(http_parser* p)
	{
		THttpObjT* pSelf = Self(p);

		pSelf->CheckUpgrade();
		pSelf->ResetHeaderBuffer();

		return pSelf->m_pContext->FireHeadersComplete(pSelf->m_pSocket);
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

private:

	EnHandleResult Upgrade(const BYTE* pData, int iLength, int iPased)
	{
		ASSERT(m_parser.upgrade);

		if(m_pContext->FireUpgrade(m_pSocket, m_enUpgrade) != HPR_OK)
			return HR_ERROR;

		ResetHeaderState();

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
			if(GetHeader(UPGRADE_HEADER, &lpszValue) && _stricmp(WEB_SOCKET_HEADER_VALUE, lpszValue) == 0)
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
		LPCSTR lpszBuffer	= (LPCSTR)m_strBuffer;

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
			CStringA tk = m_strBuffer.Tokenize(COOKIE_TOKENIZE, i);

			if(i != -1)
			{
				int j = tk.Find(NV_SEPARATOR_CHAR);

				if(j <= 0)
				{
					m_parser.http_errno = HPE_INVALID_HEADER_TOKEN;
					return HPR_ERROR;
				}

				m_cookies.emplace(move(TCookieMap::value_type(tk.Left(j), tk.Mid(j + 1))));
			}

		} while(i != -1);

		return HPR_OK;
	}

public:
	BOOL IsUpgrade()				{return m_parser.upgrade;}
	BOOL IsKeepAlive()				{return ::http_should_keep_alive(&m_parser);}
	USHORT GetVersion()				{return MAKEWORD(m_parser.http_major, m_parser.http_minor);}
	ULONGLONG GetContentLength()	{return m_parser.content_length;}

	LPCSTR GetMethod()				{return ::http_method_str((http_method)(m_parser.method));}
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
		GetHeader(CONTENT_TYPE_HEADER, &lpszValue);

		return lpszValue;
	}

	LPCSTR GetContentEncoding()
	{
		LPCSTR lpszValue = nullptr;
		GetHeader(CONTENT_ENCODING_HEADER, &lpszValue);

		return lpszValue;
	}

	LPCSTR GetTransferEncoding()
	{
		LPCSTR lpszValue = nullptr;
		GetHeader(TRANSFER_ENCODING_HEADER, &lpszValue);

		return lpszValue;
	}

	LPCSTR GetHost()
	{
		LPCSTR lpszValue = nullptr;
		GetHeader(HOST_HEADER, &lpszValue);

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
		ASSERT(m_pstrUrlFileds && enField < HUF_MAX);

		if(m_pstrUrlFileds && enField < HUF_MAX)
			return (LPCSTR)m_pstrUrlFileds[enField];

		return nullptr;
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

		if(bRelpace)
			DeleteCookie(lpszName);

		return m_cookies.emplace(TCookieMap::value_type(lpszName, lpszValue)).second;
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

public:
	THttpObjT			(BOOL bRequest, T* pContext, S* pSocket)
	: m_pContext		(pContext)
	, m_pSocket			(pSocket)
	, m_bRequest		(bRequest)
	, m_bReleased		(FALSE)
	, m_usUrlFieldSet	(0)
	, m_pstrUrlFileds	(nullptr)
	, m_enUpgrade		(HUT_NONE)
	{
		if(m_bRequest)
			m_pstrUrlFileds = new CStringA[HUF_MAX];

		ResetParser();
	}

	~THttpObjT()
	{
		if(m_pstrUrlFileds)
			delete[] m_pstrUrlFileds;
	}

	static THttpObjT* Construct(BOOL bRequest, T* pContext, S* pSocket)
		{return new THttpObjT(bRequest, pContext, pSocket);}

	static void Destruct(THttpObjT* pHttpObj)
		{if(pHttpObj) delete pHttpObj;}

	void Reset()
	{
		ResetParser();
		ResetHeaderState();
	}

private:

	void ResetParser()
	{
		::http_parser_init(&m_parser, m_bRequest ? HTTP_REQUEST : HTTP_RESPONSE);
		m_parser.data = this;		
	}

	void ResetHeaderState()
	{
		if(m_pstrUrlFileds && m_usUrlFieldSet != 0)
		{
			m_usUrlFieldSet = 0;

			for(int i = 0; i < HUF_MAX; i++)
				m_pstrUrlFileds[i].Empty();
		}

		if(m_bRequest)
			DeleteAllCookies();
			
		m_headers.clear();
		ResetHeaderBuffer();
	}

	void ResetHeaderBuffer()
	{
		ResetBuffer();
		m_strCurHeader.Empty();
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

	USHORT		m_usUrlFieldSet;
	CStringA*	m_pstrUrlFileds;

	EnHttpUpgradeType m_enUpgrade;

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


extern CStringA& GetHttpVersionStr(EnHttpVersion enVersion, CStringA& strResult);
extern LPCSTR GetHttpDefaultStatusCodeDesc(EnHttpStatusCode enCode);
extern void MakeRequestLine(LPCSTR lpszMethod, LPCSTR lpszPath, EnHttpVersion enVersion, CStringA& strValue);
extern void MakeStatusLine(EnHttpVersion enVersion, USHORT usStatusCode, LPCSTR lpszDesc, CStringA& strValue);
extern void MakeHeaderLines(const THeader lpHeaders[], int iHeaderCount, TCookieMap* pCookies, int iBodyLength, BOOL bRequest, LPCSTR lpszDefaultHost, USHORT usPort, CStringA& strValue);
extern void MakeHttpPacket(const CStringA& strHeader, const BYTE* pBody, int iLength, WSABUF szBuffer[2]);
