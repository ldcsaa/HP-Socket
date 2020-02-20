/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Author	: Bruce Liang
 * Website	: https://github.com/ldcsaa
 * Project	: https://github.com/ldcsaa/HP-Socket
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 44636872, 75375912
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
 
#include "HttpHelper.h"

#ifdef _HTTP_SUPPORT

#if !defined(z_const)
	#define z_const
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////

CStringA& GetHttpVersionStr(EnHttpVersion enVersion, CStringA& strResult)
{
	strResult.Format("HTTP/%d.%d", LOBYTE(enVersion), HIBYTE(enVersion));
	return strResult;
}

CStringA& AdjustRequestPath(BOOL bConnect, LPCSTR lpszPath, CStringA& strPath)
{
	strPath = lpszPath;

	if(strPath.IsEmpty() || (!bConnect && strPath.GetAt(0) != HTTP_PATH_SEPARATOR_CHAR))
		strPath.Insert(0, HTTP_PATH_SEPARATOR_CHAR);

	return strPath;
}

LPCSTR GetHttpDefaultStatusCodeDesc(EnHttpStatusCode enCode)
{
	switch(enCode)
	{
	case HSC_CONTINUE						: return "Continue";
	case HSC_SWITCHING_PROTOCOLS			: return "Switching Protocols";
	case HSC_PROCESSING						: return "Processing";

	case HSC_OK								: return "OK";
	case HSC_CREATED						: return "Created";
	case HSC_ACCEPTED						: return "Accepted";
	case HSC_NON_AUTHORITATIVE_INFORMATION	: return "Non-Authoritative Information";
	case HSC_NO_CONTENT						: return "No Content";
	case HSC_RESET_CONTENT					: return "Reset Content";
	case HSC_PARTIAL_CONTENT				: return "Partial Content";
	case HSC_MULTI_STATUS					: return "Multi-Status";
	case HSC_ALREADY_REPORTED				: return "Already Reported";
	case HSC_IM_USED						: return "IM Used";

	case HSC_MULTIPLE_CHOICES				: return "Multiple Choices";
	case HSC_MOVED_PERMANENTLY				: return "Moved Permanently";
	case HSC_MOVED_TEMPORARILY				: return "Move temporarily";
	case HSC_SEE_OTHER						: return "See Other";
	case HSC_NOT_MODIFIED					: return "Not Modified";
	case HSC_USE_PROXY						: return "Use Proxy";
	case HSC_SWITCH_PROXY					: return "Switch Proxy";
	case HSC_TEMPORARY_REDIRECT				: return "Temporary Redirect";
	case HSC_PERMANENT_REDIRECT				: return "Permanent Redirect";

	case HSC_BAD_REQUEST					: return "Bad Request";
	case HSC_UNAUTHORIZED					: return "Unauthorized";
	case HSC_PAYMENT_REQUIRED				: return "Payment Required";
	case HSC_FORBIDDEN						: return "Forbidden";
	case HSC_NOT_FOUND						: return "Not Found";
	case HSC_METHOD_NOT_ALLOWED				: return "Method Not Allowed";
	case HSC_NOT_ACCEPTABLE					: return "Not Acceptable";
	case HSC_PROXY_AUTHENTICATION_REQUIRED	: return "Proxy Authentication Required";
	case HSC_REQUEST_TIMEOUT				: return "Request Timeout";
	case HSC_CONFLICT						: return "Conflict";
	case HSC_GONE							: return "Gone";
	case HSC_LENGTH_REQUIRED				: return "Length Required";
	case HSC_PRECONDITION_FAILED			: return "Precondition Failed";
	case HSC_REQUEST_ENTITY_TOO_LARGE		: return "Request Entity Too Large";
	case HSC_REQUEST_URI_TOO_LONG			: return "Request-URI Too Long";
	case HSC_UNSUPPORTED_MEDIA_TYPE			: return "Unsupported Media Type";
	case HSC_REQUESTED_RANGE_NOT_SATISFIABLE: return "Requested Range Not Satisfiable";
	case HSC_EXPECTATION_FAILED				: return "Expectation Failed";
	case HSC_MISDIRECTED_REQUEST			: return "Misdirected Request";
	case HSC_UNPROCESSABLE_ENTITY			: return "Unprocessable Entity";
	case HSC_LOCKED							: return "Locked";
	case HSC_FAILED_DEPENDENCY				: return "Failed Dependency";
	case HSC_UNORDERED_COLLECTION			: return "Unordered Collection";
	case HSC_UPGRADE_REQUIRED				: return "Upgrade Required";
	case HSC_PRECONDITION_REQUIRED			: return "Precondition Required";
	case HSC_TOO_MANY_REQUESTS				: return "Too Many Requests";
	case HSC_REQUEST_HEADER_FIELDS_TOO_LARGE: return "Request Header Fields Too Large";
	case HSC_UNAVAILABLE_FOR_LEGAL_REASONS	: return "Unavailable For Legal Reasons";
	case HSC_RETRY_WITH						: return "Retry With";

	case HSC_INTERNAL_SERVER_ERROR			: return "Internal Server Error";
	case HSC_NOT_IMPLEMENTED				: return "Not Implemented";
	case HSC_BAD_GATEWAY					: return "Bad Gateway";
	case HSC_SERVICE_UNAVAILABLE			: return "Service Unavailable";
	case HSC_GATEWAY_TIMEOUT				: return "Gateway Timeout";
	case HSC_HTTP_VERSION_NOT_SUPPORTED		: return "HTTP Version Not Supported";
	case HSC_VARIANT_ALSO_NEGOTIATES		: return "Variant Also Negotiates";
	case HSC_INSUFFICIENT_STORAGE			: return "Insufficient Storage";
	case HSC_LOOP_DETECTED					: return "Loop Detected";
	case HSC_BANDWIDTH_LIMIT_EXCEEDED		: return "Bandwidth Limit Exceeded";
	case HSC_NOT_EXTENDED					: return "Not Extended";
	case HSC_NETWORK_AUTHENTICATION_REQUIRED: return "Network Authentication Required";

	case HSC_UNPARSEABLE_RESPONSE_HEADERS	: return "Unparseable Response Headers";
	default									: return "***";

	}
}

static inline CStringA& AppendHeader(LPCSTR lpszName, LPCSTR lpszValue, CStringA& strValue)
{
	strValue.Append(lpszName);
	strValue.Append(HTTP_HEADER_SEPARATOR);
	strValue.Append(lpszValue);
	strValue.Append(HTTP_CRLF);

	return strValue;
}

void MakeRequestLine(LPCSTR lpszMethod, LPCSTR lpszPath, EnHttpVersion enVersion, CStringA& strValue)
{
	ASSERT(lpszMethod);

	strValue.Format("%s %s HTTP/%d.%d%s", (LPCSTR)(CStringA(lpszMethod).MakeUpper()), lpszPath, LOBYTE(enVersion), HIBYTE(enVersion), HTTP_CRLF);
}

void MakeStatusLine(EnHttpVersion enVersion, USHORT usStatusCode, LPCSTR lpszDesc, CStringA& strValue)
{
	if(!lpszDesc) lpszDesc = ::GetHttpDefaultStatusCodeDesc((EnHttpStatusCode)usStatusCode);
	strValue.Format("HTTP/%d.%d %d %s%s", LOBYTE(enVersion), HIBYTE(enVersion), usStatusCode, lpszDesc, HTTP_CRLF);
}

void MakeHeaderLines(const THeader lpHeaders[], int iHeaderCount, const TCookieMap* pCookies, int iBodyLength, BOOL bRequest, int iConnFlag, LPCSTR lpszDefaultHost, USHORT usPort, CStringA& strValue)
{
	unordered_set<LPCSTR, str_hash_func::hash, str_hash_func::equal_to> szHeaderNames;

	if(iHeaderCount > 0)
	{
		ASSERT(lpHeaders);

		for(int i = 0; i < iHeaderCount; i++)
		{
			const THeader& header = lpHeaders[i];

			ASSERT(!::IsStrEmptyA(header.name));

			if(!::IsStrEmptyA(header.name))
			{
				szHeaderNames.emplace(header.name);
				AppendHeader(header.name, header.value, strValue);
			}
		}
	}

	if(	(!bRequest || iBodyLength > 0)											&&
		(szHeaderNames.empty()													||	
		(szHeaderNames.find(HTTP_HEADER_CONTENT_LENGTH) == szHeaderNames.end()	&&
		szHeaderNames.find(HTTP_HEADER_TRANSFER_ENCODING) == szHeaderNames.end())))
	{
		char szBodyLength[16];
		itoa(iBodyLength, szBodyLength, 10);

		AppendHeader(HTTP_HEADER_CONTENT_LENGTH, szBodyLength, strValue);
	}

	if(	(iConnFlag == 0 || iConnFlag == 1)										&&
		(szHeaderNames.empty()													||	
		szHeaderNames.find(HTTP_HEADER_CONNECTION) == szHeaderNames.end()		))
	{
		LPCSTR lpszValue = iConnFlag == 0 ? HTTP_CONNECTION_CLOSE_VALUE : HTTP_CONNECTION_KEEPALIVE_VALUE;
		AppendHeader(HTTP_HEADER_CONNECTION, lpszValue, strValue);
	}

	if(	bRequest && !::IsStrEmptyA(lpszDefaultHost)						&&
		(szHeaderNames.empty()											||	
		(szHeaderNames.find(HTTP_HEADER_HOST) == szHeaderNames.end())	))
	{
		CStringA strHost(lpszDefaultHost);
		if(usPort != 0) strHost.AppendFormat(":%u", usPort);

		AppendHeader(HTTP_HEADER_HOST, strHost, strValue);
	}

	szHeaderNames.clear();

	if(pCookies != nullptr)
	{
		DWORD dwSize = (DWORD)pCookies->size();

		if(dwSize > 0)
		{
			strValue.Append(HTTP_HEADER_COOKIE);
			strValue.Append(HTTP_HEADER_SEPARATOR);

			DWORD dwIndex = 0;

			for(TCookieMapCI it = pCookies->begin(), end = pCookies->end(); it != end; ++it, ++dwIndex)
			{
				strValue.Append(it->first);
				strValue.AppendChar(COOKIE_KV_SEP_CHAR);
				strValue.Append(it->second);

				if(dwIndex < dwSize - 1)
					strValue.Append(HTTP_COOKIE_SEPARATOR);
			}

			strValue.Append(HTTP_CRLF);
		}
	}

	strValue.Append(HTTP_CRLF);
}

void MakeHttpPacket(const CStringA& strHeader, const BYTE* pBody, int iLength, WSABUF szBuffer[2])
{
	ASSERT(pBody != nullptr || iLength == 0);

	szBuffer[0].buf = (LPBYTE)(LPCSTR)strHeader;
	szBuffer[0].len = strHeader.GetLength();
	szBuffer[1].buf = (LPBYTE)pBody;
	szBuffer[1].len = iLength;
}

int MakeChunkPackage(const BYTE* pData, int iLength, LPCSTR lpszExtensions, char szLen[12], WSABUF bufs[5])
{
	ASSERT(iLength == 0 || pData != nullptr);

	int i = 0;

	if(::IsStrEmptyA(lpszExtensions))
	{
		sprintf(szLen, "%x" HTTP_CRLF, iLength);

		bufs[i].buf = (LPBYTE)szLen;
		bufs[i].len = (int)strlen(szLen);
		++i;
	}
	else
	{
		LPCSTR lpszSep = lpszExtensions[0] == ';' ? " " : " ;";

		sprintf(szLen, "%x%s", iLength, lpszSep);

		bufs[i].buf = (LPBYTE)szLen;
		bufs[i].len = (int)strlen(szLen);
		++i;

		bufs[i].buf = (LPBYTE)lpszExtensions;
		bufs[i].len = (int)strlen(lpszExtensions);
		++i;

		bufs[i].buf = (LPBYTE)HTTP_CRLF;
		bufs[i].len = 2;
		++i;
	}

	if(iLength > 0)
	{
		bufs[i].buf = (LPBYTE)pData;
		bufs[i].len = iLength;
		++i;
	}

	bufs[i].buf = (LPBYTE)HTTP_CRLF;
	bufs[i].len = 2;
	++i;

	return i;
}

BOOL MakeWSPacket(BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], BYTE* pData, int iLength, ULONGLONG ullBodyLen, BYTE szHeader[HTTP_MAX_WS_HEADER_LEN], WSABUF szBuffer[2])
{
	ULONGLONG ullLength = (ULONGLONG)iLength;

	ASSERT(pData != nullptr	|| iLength == 0);
	ASSERT(ullBodyLen == 0	|| ullBodyLen >= ullLength);

	if(ullBodyLen == 0)
		ullBodyLen = ullLength;
	else if(ullBodyLen < ullLength)
	{
		::SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	TBaseWSHeader bh(szHeader, TRUE);

	int iHeaderLen = HTTP_MIN_WS_HEADER_LEN;

	bh.set_fin(bFinal);
	bh.set_rsv(iReserved);
	bh.set_code(iOperationCode);
	bh.set_mask(lpszMask ? TRUE : FALSE);

	if(ullBodyLen < 126)
		bh.set_len((BYTE)ullBodyLen);
	else if(ullBodyLen <= 0xFFFF)
	{
		bh.set_len(126);
		bh.set_extlen((USHORT)ullBodyLen);

		iHeaderLen += 2;
	}
	else
	{
		bh.set_len(127);
		*(ULONGLONG*)(szHeader + HTTP_MIN_WS_HEADER_LEN) = ::HToN64(ullBodyLen);

		iHeaderLen += 8;
	}

	if(lpszMask)
	{
		memcpy(szHeader + iHeaderLen, lpszMask, 4);

		for(int i = 0; i < iLength; i++)
			pData[i] = pData[i] ^ lpszMask[i & 0x03];

		iHeaderLen += 4;
	}

	szBuffer[0].buf = szHeader;
	szBuffer[0].len = iHeaderLen;
	szBuffer[1].buf = (LPBYTE)pData;
	szBuffer[1].len = iLength;

	return TRUE;
}

BOOL ParseUrl(const CStringA& strUrl, BOOL& bHttps, CStringA& strHost, USHORT& usPort, CStringA& strPath)
{
	int iSchemaLength = (int)strlen(HTTP_SCHEMA);

	if(strnicmp(strUrl, HTTP_SCHEMA, iSchemaLength) == 0)
		bHttps = FALSE;
	else
	{
		iSchemaLength = (int)strlen(HTTPS_SCHEMA);

		if(strnicmp(strUrl, HTTPS_SCHEMA, iSchemaLength) == 0)
			bHttps = TRUE;
		else
			return FALSE;
	}

	CStringA strFullHost;
	int i = strUrl.Find(HTTP_PATH_SEPARATOR_CHAR, iSchemaLength);

	if(i > 0)
	{
		strFullHost	= strUrl.Mid(iSchemaLength, i - iSchemaLength);
		strPath		= strUrl.Mid(i);
	}
	else
	{
		strFullHost	= strUrl.Mid(iSchemaLength);
		strPath		= HTTP_PATH_SEPARATOR;
	}

	if(strFullHost.IsEmpty())
		return FALSE;

	CStringA strPort;

	char c = strFullHost.GetAt(0);

	if(!::isalnum(c))
	{
		if(c != IPV6_ADDR_BEGIN_CHAR)
			return FALSE;
		else
		{
			i = strFullHost.ReverseFind(IPV6_ADDR_END_CHAR);

			if(i < 0)
				return FALSE;
			else
			{
				if(strFullHost.GetLength() > i + 1)
				{
					if(strFullHost.GetAt(i + 1) != PORT_SEPARATOR_CHAR)
						return FALSE;

					strPort = strFullHost.Mid(i + 2);

					if(strPort.IsEmpty())
						return FALSE;
				}
				
				strHost = strFullHost.Mid(1, i - 1);
			}
		}
	}
	else
	{
		i = strFullHost.Find(PORT_SEPARATOR_CHAR);

		if(i < 0)
			strHost	= strFullHost;
		else
		{
			strPort = strFullHost.Mid(i + 1);

			if(strPort.IsEmpty())
				return FALSE;

			strHost = strFullHost.Mid(0, i);
		}
	}

	if(strPort.IsEmpty())
		usPort = bHttps ? HTTPS_DEFAULT_PORT : HTTP_DEFAULT_PORT;
	else
		usPort = (USHORT)::atoi(strPort);

	return TRUE;
}

#endif