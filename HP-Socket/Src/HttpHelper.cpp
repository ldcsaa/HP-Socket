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
 
#include "stdafx.h"
#include "HttpHelper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////

const DWORD	MIN_HTTP_RELEASE_CHECK_INTERVAL	= 1000;
const DWORD	MIN_HTTP_RELEASE_DELAY			= 100;
const DWORD	MAX_HTTP_RELEASE_DELAY			= 60 * 1000;
const DWORD	DEFAULT_HTTP_RELEASE_DELAY		= 3 * 1000;
const EnHttpVersion DEFAULT_HTTP_VERSION	= HV_1_1;

///////////////////////////////////////////////////////////////////////////////////////////////////////

CStringA& GetHttpVersionStr(EnHttpVersion enVersion, CStringA& strResult)
{
	strResult.Format("HTTP/%d.%d", LOBYTE(enVersion), HIBYTE(enVersion));
	return strResult;
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

	case HSC_MULTIPLE_CHOICES				: return "Multiple Choices";
	case HSC_MOVED_PERMANENTLY				: return "Moved Permanently";
	case HSC_MOVED_TEMPORARILY				: return "Move temporarily";
	case HSC_SEE_OTHER						: return "See Other";
	case HSC_NOT_MODIFIED					: return "Not Modified";
	case HSC_USE_PROXY						: return "Use Proxy";
	case HSC_SWITCH_PROXY					: return "Switch Proxy";
	case HSC_TEMPORARY_REDIRECT				: return "Temporary Redirect";

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
	case HSC_UNPROCESSABLE_ENTITY			: return "Unprocessable Entity";
	case HSC_LOCKED							: return "Locked";
	case HSC_FAILED_DEPENDENCY				: return "Failed Dependency";
	case HSC_UNORDERED_COLLECTION			: return "Unordered Collection";
	case HSC_UPGRADE_REQUIRED				: return "Upgrade Required";
	case HSC_RETRY_WITH						: return "Retry With";

	case HSC_INTERNAL_SERVER_ERROR			: return "Internal Server Error";
	case HSC_NOT_IMPLEMENTED				: return "Not Implemented";
	case HSC_BAD_GATEWAY					: return "Bad Gateway";
	case HSC_SERVICE_UNAVAILABLE			: return "Service Unavailable";
	case HSC_GATEWAY_TIMEOUT				: return "Gateway Timeout";
	case HSC_HTTP_VERSION_NOT_SUPPORTED		: return "HTTP Version Not Supported";
	case HSC_VARIANT_ALSO_NEGOTIATES		: return "Variant Also Negotiates";
	case HSC_INSUFFICIENT_STORAGE			: return "Insufficient Storage";
	case HSC_BANDWIDTH_LIMIT_EXCEEDED		: return "Bandwidth Limit Exceeded";
	case HSC_NOT_EXTENDED					: return "Not Extended";

	case HSC_UNPARSEABLE_RESPONSE_HEADERS	: return "Unparseable Response Headers";
	default									: return "***";

	}
}

static inline CStringA& AppendHeader(LPCSTR lpszName, LPCSTR lpszValue, CStringA& strValue)
{
	strValue.Append(lpszName);
	strValue.Append(HEADER_SEPARATOR);
	strValue.Append(lpszValue);
	strValue.Append(CRLF);

	return strValue;
}

void MakeRequestLine(LPCSTR lpszMethod, LPCSTR lpszPath, EnHttpVersion enVersion, CStringA& strValue)
{
	ASSERT(lpszMethod && lpszPath);

	strValue.Format("%s %s HTTP/%d.%d%s", lpszMethod, lpszPath, LOBYTE(enVersion), HIBYTE(enVersion), CRLF);
}

void MakeStatusLine(EnHttpVersion enVersion, USHORT usStatusCode, LPCSTR lpszDesc, CStringA& strValue)
{
	if(!lpszDesc) lpszDesc = ::GetHttpDefaultStatusCodeDesc((EnHttpStatusCode)usStatusCode);
	strValue.Format("HTTP/%d.%d %d %s%s", LOBYTE(enVersion), HIBYTE(enVersion), usStatusCode, lpszDesc, CRLF);
}

void MakeHeaderLines(const THeader lpHeaders[], int iHeaderCount, TCookieMap* pCookies, int iBodyLength, BOOL bRequest, LPCSTR lpszDefaultHost, USHORT usPort, CStringA& strValue)
{
	unordered_set<LPCSTR, str_hash_func::hash, str_hash_func::equal_to> szHeaderNames;

	if(iHeaderCount > 0)
	{
		ASSERT(lpHeaders);

		for(int i = 0; i < iHeaderCount; i++)
		{
			const THeader& header = lpHeaders[i];

			if(header.name != nullptr)
			{
				ASSERT(strlen(header.name) > 0);

				szHeaderNames.emplace(header.name);
				AppendHeader(header.name, header.value, strValue);
			}
		}
	}

	if(	(!bRequest || iBodyLength > 0)										&&
		(szHeaderNames.empty()												||	
		(szHeaderNames.find(CONTENT_LENGTH_HEADER) == szHeaderNames.end()	&&
		szHeaderNames.find(TRANSFER_ENCODING_HEADER) == szHeaderNames.end())))
	{
		CStringA strLength;
		strLength.Format("%i", iBodyLength);
		AppendHeader(CONTENT_LENGTH_HEADER, strLength, strValue);
	}

	if(	bRequest && lpszDefaultHost && lpszDefaultHost[0] != 0		&&
		(szHeaderNames.empty()										||	
		(szHeaderNames.find(HOST_HEADER) == szHeaderNames.end())	))
	{
		CStringA strHost;
		strHost.Format("%s:%u", lpszDefaultHost, usPort);
		AppendHeader(HOST_HEADER, strHost, strValue);
	}

	szHeaderNames.clear();

	if(pCookies != nullptr)
	{
		DWORD dwSize = (DWORD)pCookies->size();

		if(dwSize > 0)
		{
			strValue.Append(COOKIE_HEADER);
			strValue.Append(HEADER_SEPARATOR);

			DWORD dwIndex = 0;

			for(TCookieMapCI it = pCookies->begin(), end = pCookies->end(); it != end; ++it, ++dwIndex)
			{
				strValue.Append(it->first);
				strValue.AppendChar(NV_SEPARATOR_CHAR);
				strValue.Append(it->second);

				if(dwIndex < dwSize - 1)
					strValue.Append(COOKIE_TOKENIZE);
			}

			strValue.Append(CRLF);
		}
	}

	strValue.Append(CRLF);
}

void MakeHttpPacket(const CStringA& strHeader, const BYTE* pBody, int iLength, WSABUF szBuffer[2])
{
	ASSERT(pBody != nullptr || iLength == 0);

	szBuffer[0].buf = (LPSTR)(LPCSTR)strHeader;
	szBuffer[0].len = strHeader.GetLength();
	szBuffer[1].buf = (LPSTR)(LPCSTR)pBody;
	szBuffer[1].len = iLength;
}
