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
 
#include "stdafx.h"
#include "HttpHelper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////

CStringA& GetHttpVersionStr(EnHttpVersion enVersion, CStringA& strResult)
{
	strResult.Format("HTTP/%d.%d", LOBYTE(enVersion), HIBYTE(enVersion));
	return strResult;
}

CStringA& AdjustRequestPath(LPCSTR lpszPath, CStringA& strPath)
{
	strPath = lpszPath;

	if(strPath.IsEmpty() || strPath.GetAt(0) != HTTP_PATH_SEPARATOR_CHAR)
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

	strValue.Format("%s %s HTTP/%d.%d%s", CStringA(lpszMethod).MakeUpper(), lpszPath, LOBYTE(enVersion), HIBYTE(enVersion), HTTP_CRLF);
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

			if(header.name != nullptr)
			{
				ASSERT(strlen(header.name) > 0);

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
		_itoa(iBodyLength, szBodyLength, 10);

		AppendHeader(HTTP_HEADER_CONTENT_LENGTH, szBodyLength, strValue);
	}

	if(	(iConnFlag == 0 || iConnFlag == 1)										&&
		(szHeaderNames.empty()													||	
		szHeaderNames.find(HTTP_HEADER_CONNECTION) == szHeaderNames.end()		))
	{
		LPCSTR lpszValue = iConnFlag == 0 ? HTTP_CONNECTION_CLOSE_VALUE : HTTP_CONNECTION_KEEPALIVE_VALUE;
		AppendHeader(HTTP_HEADER_CONNECTION, lpszValue, strValue);
	}

	if(	bRequest && lpszDefaultHost && lpszDefaultHost[0] != 0			&&
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

	szBuffer[0].buf = (LPSTR)(LPCSTR)strHeader;
	szBuffer[0].len = strHeader.GetLength();
	szBuffer[1].buf = (LPSTR)(LPCSTR)pBody;
	szBuffer[1].len = iLength;
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

	szBuffer[0].buf = (LPSTR)(LPCSTR)szHeader;
	szBuffer[0].len = iHeaderLen;
	szBuffer[1].buf = (LPSTR)(LPCSTR)pData;
	szBuffer[1].len = iLength;

	return TRUE;
}

BOOL ParseUrl(const CStringA& strUrl, BOOL& bHttps, CStringA& strHost, USHORT& usPort, CStringA& strPath)
{
	int iSchemaLength	= (int)strlen(HTTP_SCHEMA);
	CStringA strSchema	= strUrl.Left(iSchemaLength);

	strSchema.MakeLower();

	if(strSchema.Compare(HTTP_SCHEMA) == 0)
		bHttps = FALSE;
	else
	{
		iSchemaLength	= (int)strlen(HTTPS_SCHEMA);
		strSchema		= strUrl.Left(iSchemaLength);

		strSchema.MakeLower();

		if(strSchema.Compare(HTTPS_SCHEMA) == 0)
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

BOOL CodePageToUnicode(int iCodePage, const char szSrc[], WCHAR szDest[], int& iDestLength)
{
	ASSERT(szSrc);

	int iSize = ::MultiByteToWideChar(iCodePage, 0, szSrc, -1, nullptr, 0);

	if(iSize == 0 || iSize > iDestLength || !szDest || iDestLength == 0)
	{
		iDestLength = iSize;
		return FALSE;
	}

	if(::MultiByteToWideChar(iCodePage, 0, szSrc, -1, szDest, iSize) != 0)
		iDestLength = iSize;
	else
		iDestLength = 0;

	return iDestLength != 0;
}

BOOL UnicodeToCodePage(int iCodePage, const WCHAR szSrc[], char szDest[], int& iDestLength)
{
	ASSERT(szSrc);

	int iSize = ::WideCharToMultiByte(iCodePage, 0, szSrc, -1, nullptr, 0, nullptr, nullptr);

	if(iSize == 0 || iSize > iDestLength || !szDest || iDestLength == 0)
	{
		iDestLength = iSize;
		return FALSE;
	}

	if(::WideCharToMultiByte(iCodePage, 0, szSrc, -1, szDest, iSize, nullptr, nullptr) != 0)
		iDestLength = iSize;
	else
		iDestLength = 0;

	return iDestLength != 0;
}

BOOL GbkToUnicode(const char szSrc[], WCHAR szDest[], int& iDestLength)
{
	return CodePageToUnicode(CP_ACP, szSrc, szDest, iDestLength);
}

BOOL UnicodeToGbk(const WCHAR szSrc[], char szDest[], int& iDestLength)
{
	return UnicodeToCodePage(CP_ACP, szSrc, szDest, iDestLength);
}

BOOL Utf8ToUnicode(const char szSrc[], WCHAR szDest[], int& iDestLength)
{
	return CodePageToUnicode(CP_UTF8, szSrc, szDest, iDestLength);
}

BOOL UnicodeToUtf8(const WCHAR szSrc[], char szDest[], int& iDestLength)
{
	return UnicodeToCodePage(CP_UTF8, szSrc, szDest, iDestLength);
}

BOOL GbkToUtf8(const char szSrc[], char szDest[], int& iDestLength)
{
	int iMiddleLength = 0;
	GbkToUnicode(szSrc, nullptr, iMiddleLength);

	if(iMiddleLength == 0)
	{
		iDestLength = 0;
		return FALSE;
	}

	unique_ptr<WCHAR[]> p(new WCHAR[iMiddleLength]);
	VERIFY(GbkToUnicode(szSrc, p.get(), iMiddleLength));

	return UnicodeToUtf8(p.get(), szDest, iDestLength);
}

BOOL Utf8ToGbk(const char szSrc[], char szDest[], int& iDestLength)
{
	int iMiddleLength = 0;
	Utf8ToUnicode(szSrc, nullptr, iMiddleLength);

	if(iMiddleLength == 0)
	{
		iDestLength = 0;
		return FALSE;
	}

	unique_ptr<WCHAR[]> p(new WCHAR[iMiddleLength]);
	VERIFY(Utf8ToUnicode(szSrc, p.get(), iMiddleLength));

	return UnicodeToGbk(p.get(), szDest, iDestLength);
}

int Compress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	return CompressEx(lpszSrc, dwSrcLen, lpszDest, dwDestLen);
}

int CompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iLevel, int iMethod, int iWindowBits, int iMemLevel, int iStrategy)
{
	z_stream stream;

	stream.next_in	 = (z_const Bytef*)lpszSrc;
	stream.avail_in	 = dwSrcLen;
	stream.next_out	 = lpszDest;
	stream.avail_out = dwDestLen;
	stream.zalloc	 = nullptr;
	stream.zfree	 = nullptr;
	stream.opaque	 = nullptr;

	int err = ::deflateInit2(&stream, iLevel, iMethod, iWindowBits, iMemLevel, iStrategy);

	if(err != Z_OK) return err;

	err = ::deflate(&stream, Z_FINISH);

	if(err != Z_STREAM_END)
	{
		::deflateEnd(&stream);
		return err == Z_OK ? Z_BUF_ERROR : err;
	}

	if(dwDestLen > stream.total_out)
	{
		lpszDest[stream.total_out]	= 0;
		dwDestLen					= stream.total_out;
	}

	return ::deflateEnd(&stream);
}

int Uncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	return UncompressEx(lpszSrc, dwSrcLen, lpszDest, dwDestLen);
}

int UncompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iWindowBits)
{
	z_stream stream;

	stream.next_in	 = (z_const Bytef*)lpszSrc;
	stream.avail_in	 = (uInt)dwSrcLen;
	stream.next_out	 = lpszDest;
	stream.avail_out = dwDestLen;
	stream.zalloc	 = nullptr;
	stream.zfree	 = nullptr;

	int err = ::inflateInit2(&stream, iWindowBits);

	if(err != Z_OK) return err;

	err = ::inflate(&stream, Z_FINISH);

	if(err != Z_STREAM_END)
	{
		::inflateEnd(&stream);
		return (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0)) ? Z_DATA_ERROR : err;
	}

	if(dwDestLen > stream.total_out)
	{
		lpszDest[stream.total_out]	= 0;
		dwDestLen					= stream.total_out;
	}

	return inflateEnd(&stream);
}

DWORD GuessCompressBound(DWORD dwSrcLen, BOOL bGZip)
{
	DWORD dwBound = ::compressBound(dwSrcLen);
	
	if(bGZip) dwBound += 11;

	return dwBound;
}

int GZipCompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	return CompressEx(lpszSrc, dwSrcLen, lpszDest, dwDestLen, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS + 16);
}

int GZipUncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	return UncompressEx(lpszSrc, dwSrcLen, lpszDest, dwDestLen, MAX_WBITS + 32);
}

DWORD GZipGuessUncompressBound(const BYTE* lpszSrc, DWORD dwSrcLen)
{
	if(dwSrcLen < 20 || *(USHORT*)lpszSrc != 0x8B1F)
		return 0;

	return *(DWORD*)(lpszSrc + dwSrcLen - 4);
}

DWORD GuessBase64EncodeBound(DWORD dwSrcLen)
{
	return 4 * ((dwSrcLen + 2) / 3);
}

DWORD GuessBase64DecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen)
{
	if(dwSrcLen < 2)
		return 0;

	if(lpszSrc[dwSrcLen - 2] == '=')
		dwSrcLen -= 2;
	else if(lpszSrc[dwSrcLen - 1] == '=')
			--dwSrcLen;

	DWORD dwMod = dwSrcLen % 4;
	DWORD dwAdd = dwMod == 2 ? 1 : (dwMod == 3 ? 2 : 0);

	return 3 * (dwSrcLen / 4) + dwAdd;
}

int Base64Encode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	static const BYTE CODES[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	DWORD dwRealLen = GuessBase64EncodeBound(dwSrcLen);

	if(lpszDest == nullptr || dwDestLen < dwRealLen)
	{
		dwDestLen = dwRealLen;
		return -5;
	}

	BYTE* p		= lpszDest;
	DWORD leven	= 3 * (dwSrcLen / 3);
	DWORD i		= 0;

	for (; i < leven; i += 3)
	{
		*p++ = CODES[lpszSrc[0] >> 2];
		*p++ = CODES[((lpszSrc[0] & 3) << 4) + (lpszSrc[1] >> 4)];
		*p++ = CODES[((lpszSrc[1] & 0xf) << 2) + (lpszSrc[2] >> 6)];
		*p++ = CODES[lpszSrc[2] & 0x3f];

		lpszSrc += 3;
	}

	if(i < dwSrcLen)
	{
		BYTE a = lpszSrc[0];
		BYTE b = (i + 1 < dwSrcLen) ? lpszSrc[1] : 0;

		*p++ = CODES[a >> 2];
		*p++ = CODES[((a & 3) << 4) + (b >> 4)];
		*p++ = (i + 1 < dwSrcLen) ? CODES[((b & 0xf) << 2)] : '=';
		*p++ = '=';
	}  

	ASSERT(dwRealLen == (DWORD)(p - lpszDest));

	if(dwDestLen > dwRealLen)
	{
		*p			= 0;
		dwDestLen	= dwRealLen;
	}

	return 0;  
}

int Base64Decode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	static const BYTE MAP[256]	=
	{ 
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 253, 255,
		255, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 253, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
		 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
		255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,
		  7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,
		 19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
		255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
		 37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
		 49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255
	};

	DWORD dwRealLen = GuessBase64DecodeBound(lpszSrc, dwSrcLen);

	if(lpszDest == nullptr || dwDestLen < dwRealLen)
	{
		dwDestLen = dwRealLen;
		return -5;
	}

	BYTE c;
	int g = 3;
	DWORD i, x, y, z;

	for(i = x = y = z = 0; i < dwSrcLen || x != 0;)
	{
		c = i < dwSrcLen ? MAP[lpszSrc[i++]] : 254;

		if(c == 255) {dwDestLen = 0; return -3;}
		else if(c == 254) {c = 0; g--;}
		else if(c == 253) continue;

		z = (z << 6) | c;

		if(++x == 4)
		{
			lpszDest[y++] = (BYTE)((z >> 16) & 255);
			if (g > 1) lpszDest[y++] = (BYTE)((z >> 8) & 255);
			if (g > 2) lpszDest[y++] = (BYTE)(z & 255);

			x = z = 0;
		}
	}

	BOOL isOK = (y == dwRealLen);

	if(!isOK)
		dwDestLen = 0;
	else
	{
		if(dwDestLen > dwRealLen)
		{
			lpszDest[dwRealLen]	= 0;
			dwDestLen			= dwRealLen;
		}
	}

	return isOK ? 0 : -3;
}

DWORD GuessUrlEncodeBound(const BYTE* lpszSrc, DWORD dwSrcLen)
{
	DWORD dwAdd = 0;

	for(DWORD i = 0; i < dwSrcLen; i++)
	{
		BYTE c	= lpszSrc[i];

		if(!(isalnum(c) || c == ' ' || c == '.' || c == '-' || c == '_' || c == '*'))
			dwAdd += 2;
	}

	return dwSrcLen + dwAdd;
}

DWORD GuessUrlDecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen)
{
	DWORD dwPercent = 0;

	for(DWORD i = 0; i < dwSrcLen; i++)
	{
		if(lpszSrc[i] == '%')
		{
			++dwPercent;
			i += 2;
		}
	}

	DWORD dwSub = dwPercent * 2;

	if(dwSrcLen < dwSub)
		return 0;

	return dwSrcLen - dwSub;
}

#define HEX_CHAR_TO_VALUE(c)			(c <= '9' ? c - '0' : (c <= 'F' ? c - 'A' + 0x0A : c - 'a' + 0X0A))
#define HEX_DOUBLE_CHAR_TO_VALUE(pc)	(((HEX_CHAR_TO_VALUE(*(pc))) << 4) | (HEX_CHAR_TO_VALUE(*(pc + 1))))
#define HEX_VALUE_TO_CHAR(n)			(n <= 9 ? n + '0' : (n <= 'F' ? n + 'A' - 0X0A : n + 'a' - 0X0A))
#define HEX_VALUE_TO_DOUBLE_CHAR(pc, n)	{*(pc) = HEX_VALUE_TO_CHAR((n >> 4)); *((pc) + 1) = HEX_VALUE_TO_CHAR((n & 0X0F));}

int UrlEncode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	if(lpszDest == nullptr || dwDestLen == 0)
		goto ERROR_DEST_LEN;

	BYTE c;
	DWORD j = 0;

	for(DWORD i = 0; i < dwSrcLen; i++)
	{
		if(j >= dwDestLen)
			goto ERROR_DEST_LEN;

		c = lpszSrc[i];

		if (isalnum(c) || c == '.' || c == '-' || c == '_' || c == '*')
			lpszDest[j++] = c;
		else if(c == ' ')
			lpszDest[j++] = '+';
		else
		{
			if(j + 3 >= dwDestLen)
				goto ERROR_DEST_LEN;

			lpszDest[j++] = '%';
			HEX_VALUE_TO_DOUBLE_CHAR(lpszDest + j, c);
			j += 2;
			
		}
	}

	if(dwDestLen > j)
	{
		lpszDest[j]	= 0;
		dwDestLen	= j;
	}

	return 0;

ERROR_DEST_LEN:
	dwDestLen = GuessUrlEncodeBound(lpszSrc, dwSrcLen);
	return -5;
}

int UrlDecode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
{
	if(lpszDest == nullptr || dwDestLen == 0)
		goto ERROR_DEST_LEN;

	char c;
	DWORD j = 0;

	for(DWORD i = 0; i < dwSrcLen; i++)
	{
		if(j >= dwDestLen)
			goto ERROR_DEST_LEN;

		c = lpszSrc[i];

		if(c == '+')
			lpszDest[j++] = ' ';
		else if(c != '%')
			lpszDest[j++] = c;
		else
		{
			if(i + 2 >= dwSrcLen)
				goto ERROR_SRC_DATA;

			lpszDest[j++] = HEX_DOUBLE_CHAR_TO_VALUE(lpszSrc + i + 1);
			i += 2;
		}
	}

	if(dwDestLen > j)
	{
		lpszDest[j]	= 0;
		dwDestLen	= j;
	}

	return 0;

ERROR_SRC_DATA:
	dwDestLen = 0;
	return -3;

ERROR_DEST_LEN:
	dwDestLen = GuessUrlDecodeBound(lpszSrc, dwSrcLen);
	return -5;
}
