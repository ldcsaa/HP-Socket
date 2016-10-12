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
#include "HttpAgent.h"

template<class T> BOOL CHttpAgentT<T>::CheckParams()
{
	if	(m_enLocalVersion != HV_1_1 && m_enLocalVersion != HV_1_0)
	{
		SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	return __super::CheckParams();
}

template<class T> BOOL CHttpAgentT<T>::SendRequest(CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszPath, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);
	ASSERT(pHttpObj != nullptr);

	if(pHttpObj == nullptr)
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}
	
	WSABUF szBuffer[2];
	CStringA strHeader;

	LPCSTR lpszHost	= nullptr;
	USHORT usPort	= 0;

	GetRemoteHost(dwConnID, &lpszHost, &usPort);

	::MakeRequestLine(lpszMethod, lpszPath, m_enLocalVersion, strHeader);
	::MakeHeaderLines(lpHeaders, iHeaderCount, &pHttpObj->GetCookieMap(), iLength, TRUE, lpszHost, usPort, strHeader);
	::MakeHttpPacket(strHeader, pBody, iLength, szBuffer);

	return SendPackets(dwConnID, szBuffer, 2);
}

template<class T> BOOL CHttpAgentT<T>::SendLocalFile(CONNID dwConnID, LPCSTR lpszFileName, LPCSTR lpszMethod, LPCSTR lpszPath, const THeader lpHeaders[] = nullptr, int iHeaderCount = 0)
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

	return SendRequest(dwConnID, lpszMethod, lpszPath, lpHeaders, iHeaderCount, (BYTE*)(char*)fmap, (int)fmap.GetMappingSize());
}

template<class T> EnHandleResult CHttpAgentT<T>::DoFireConnect(TSocketObj* pSocketObj)
{
	EnHandleResult result = __super::DoFireConnect(pSocketObj);

	if(result != HR_ERROR)
		VERIFY(SetConnectionReserved(pSocketObj, THttpObj::Construct(FALSE, this, pSocketObj)));

	return result;
}

template<class T> EnHandleResult CHttpAgentT<T>::DoFireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
{
	THttpObj* pHttpObj = FindHttpObj(pSocketObj);
	ASSERT(pHttpObj);

	return pHttpObj->Execute(pData, iLength);
}

template<class T> EnHandleResult CHttpAgentT<T>::DoFireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
{
	EnHandleResult result = __super::DoFireClose(pSocketObj, enOperation, iErrorCode);

	THttpObj* pHttpObj = FindHttpObj(pSocketObj);

	if(pHttpObj != nullptr)
		THttpObj::Destruct(pHttpObj);

	return result;
}

template<class T> EnHandleResult CHttpAgentT<T>::DoFireShutdown()
{
	EnHandleResult result = __super::DoFireShutdown();

	return result;
}

template<class T> BOOL CHttpAgentT<T>::IsUpgrade(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->IsUpgrade();
}

template<class T> BOOL CHttpAgentT<T>::IsKeepAlive(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->IsKeepAlive();
}

template<class T> USHORT CHttpAgentT<T>::GetVersion(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return 0;

	return pHttpObj->GetVersion();
}

template<class T> ULONGLONG CHttpAgentT<T>::GetContentLength(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return 0;

	return pHttpObj->GetContentLength();
}

template<class T> LPCSTR CHttpAgentT<T>::GetContentType(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetContentType();
}

template<class T> LPCSTR CHttpAgentT<T>::GetContentEncoding(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetContentEncoding();
}

template<class T> LPCSTR CHttpAgentT<T>::GetTransferEncoding(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetTransferEncoding();
}

template<class T> EnHttpUpgradeType CHttpAgentT<T>::GetUpgradeType(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return HUT_NONE;

	return pHttpObj->GetUpgradeType();
}

template<class T> USHORT CHttpAgentT<T>::GetParseErrorCode(CONNID dwConnID, LPCSTR* lpszErrorDesc)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return 0;

	return pHttpObj->GetParseErrorCode(lpszErrorDesc);
}

template<class T> BOOL CHttpAgentT<T>::GetHeader(CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetHeader(lpszName, lpszValue);
}

template<class T> BOOL CHttpAgentT<T>::GetHeaders(CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue[], DWORD& dwCount)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetHeaders(lpszName, lpszValue, dwCount);
}

template<class T> BOOL CHttpAgentT<T>::GetAllHeaders(CONNID dwConnID, THeader lpHeaders[], DWORD& dwCount)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetAllHeaders(lpHeaders, dwCount);
}

template<class T> BOOL CHttpAgentT<T>::GetAllHeaderNames(CONNID dwConnID, LPCSTR lpszName[], DWORD& dwCount)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetAllHeaderNames(lpszName, dwCount);
}

template<class T> BOOL CHttpAgentT<T>::GetCookie(CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetCookie(lpszName, lpszValue);
}

template<class T> BOOL CHttpAgentT<T>::GetAllCookies(CONNID dwConnID, TCookie lpCookies[], DWORD& dwCount)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetAllCookies(lpCookies, dwCount);
}

template<class T> BOOL CHttpAgentT<T>::AddCookie(CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue, BOOL bRelpace)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->AddCookie(lpszName, lpszValue, bRelpace);
}

template<class T> BOOL CHttpAgentT<T>::DeleteCookie(CONNID dwConnID, LPCSTR lpszName)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->DeleteCookie(lpszName);
}

template<class T> BOOL CHttpAgentT<T>::DeleteAllCookies(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	pHttpObj->DeleteAllCookies();

	return TRUE;
}

template<class T> USHORT CHttpAgentT<T>::GetStatusCode(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return 0;

	return pHttpObj->GetStatusCode();
}

template<class T> inline typename CHttpAgentT<T>::THttpObj* CHttpAgentT<T>::FindHttpObj(CONNID dwConnID)
{
	THttpObj* pHttpObj = nullptr;
	GetConnectionReserved(dwConnID, (PVOID*)&pHttpObj);

	return pHttpObj;
}

template<class T> inline typename CHttpAgentT<T>::THttpObj* CHttpAgentT<T>::FindHttpObj(TSocketObj* pSocketObj)
{
	THttpObj* pHttpObj = nullptr;
	GetConnectionReserved(pSocketObj, (PVOID*)&pHttpObj);

	return pHttpObj;
}

template class CHttpAgentT<CTcpAgent>;

#ifdef _SSL_SUPPORT

#include "SSLAgent.h"

template class CHttpAgentT<CSSLAgent>;

#endif