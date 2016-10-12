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
#include "HttpClient.h"

template<class T> BOOL CHttpClientT<T>::CheckParams()
{
	if	(m_enLocalVersion != HV_1_1 && m_enLocalVersion != HV_1_0)
	{
		SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	return __super::CheckParams();
}

template<class T> BOOL CHttpClientT<T>::SendRequest(LPCSTR lpszMethod, LPCSTR lpszPath, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
{
	USES_CONVERSION;

	WSABUF szBuffer[2];
	CStringA strHeader;

	LPCSTR lpszHost	= nullptr;
	USHORT usPort	= 0;

	GetRemoteHost(&lpszHost, &usPort);

	::MakeRequestLine(lpszMethod, lpszPath, m_enLocalVersion, strHeader);
	::MakeHeaderLines(lpHeaders, iHeaderCount, &m_objHttp.GetCookieMap(), iLength, TRUE, lpszHost, usPort, strHeader);
	::MakeHttpPacket(strHeader, pBody, iLength, szBuffer);

	return SendPackets(szBuffer, 2);
}

template<class T> BOOL CHttpClientT<T>::SendLocalFile(LPCSTR lpszFileName, LPCSTR lpszMethod, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)
{
	if(
		strcmp(lpszMethod, HTTP_METHOD_POST)  != 0	&&
		strcmp(lpszMethod, HTTP_METHOD_PUT)	  != 0	&&
		strcmp(lpszMethod, HTTP_METHOD_PATCH) != 0 	)
	{
		::SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	CAtlFile file;
	CAtlFileMapping<> fmap;

	HRESULT hr = ::ReadSmallFile(CA2T(lpszFileName), file, fmap);

	if(FAILED(hr))
	{
		::SetLastError(HRESULT_CODE(hr));
		return FALSE;
	}

	return SendRequest(lpszMethod, lpszPath, lpHeaders, iHeaderCount, (BYTE*)(char*)fmap, (int)fmap.GetMappingSize());
}

template class CHttpClientT<CTcpClient>;

#ifdef _SSL_SUPPORT

#include "SSLClient.h"

template class CHttpClientT<CSSLClient>;

#endif
