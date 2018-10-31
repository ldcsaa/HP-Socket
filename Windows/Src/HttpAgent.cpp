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
 
#include "stdafx.h"
#include "HttpAgent.h"

#ifdef _HTTP_SUPPORT

template<class T, USHORT default_port> BOOL CHttpAgentT<T, default_port>::CheckParams()
{
	if(m_enLocalVersion != HV_1_1 && m_enLocalVersion != HV_1_0)
	{
		SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	return __super::CheckParams();
}

template<class T, USHORT default_port> void CHttpAgentT<T, default_port>::PrepareStart()
{
	__super::PrepareStart();

	m_objPool.SetHttpObjLockTime(GetFreeSocketObjLockTime());
	m_objPool.SetHttpObjPoolSize(GetFreeSocketObjPool());
	m_objPool.SetHttpObjPoolHold(GetFreeSocketObjHold());

	m_objPool.Prepare();
}

template<class T, USHORT default_port> BOOL CHttpAgentT<T, default_port>::SendRequest(CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszPath, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
	{
		::SetLastError(ERROR_OBJECT_NOT_FOUND);
		return FALSE;
	}
	
	WSABUF szBuffer[2];
	CStringA strHeader;

	LPCSTR lpszHost	= nullptr;
	USHORT usPort	= 0;
	BOOL bConnect	= (_stricmp(lpszMethod, HTTP_METHOD_CONNECT) == 0);

	if(!bConnect)
	{
		GetRemoteHost(dwConnID, &lpszHost, &usPort);
		if(usPort == default_port) usPort = 0;
	}

	CStringA strPath;
	::AdjustRequestPath(bConnect, lpszPath, strPath);

	pHttpObj->SetRequestPath(lpszMethod, strPath);
	pHttpObj->ReloadCookies();

	::MakeRequestLine(lpszMethod, strPath, m_enLocalVersion, strHeader);
	::MakeHeaderLines(lpHeaders, iHeaderCount, &pHttpObj->GetCookieMap(), iLength, TRUE, -1, lpszHost, usPort, strHeader);
	::MakeHttpPacket(strHeader, pBody, iLength, szBuffer);

	return SendPackets(dwConnID, szBuffer, 2);
}

template<class T, USHORT default_port> BOOL CHttpAgentT<T, default_port>::SendLocalFile(CONNID dwConnID, LPCSTR lpszFileName, LPCSTR lpszMethod, LPCSTR lpszPath, const THeader lpHeaders[], int iHeaderCount)
{
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

template<class T, USHORT default_port> BOOL CHttpAgentT<T, default_port>::SendWSMessage(CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], BYTE* pData, int iLength, ULONGLONG ullBodyLen)
{
	WSABUF szBuffer[2];
	BYTE szHeader[HTTP_MAX_WS_HEADER_LEN];

	if(!::MakeWSPacket(bFinal, iReserved, iOperationCode, lpszMask, pData, iLength, ullBodyLen, szHeader, szBuffer))
		return FALSE;

	return SendPackets(dwConnID, szBuffer, 2);
}

template<class T, USHORT default_port> EnHandleResult CHttpAgentT<T, default_port>::DoFireConnect(TSocketObj* pSocketObj)
{
	EnHandleResult result = __super::DoFireConnect(pSocketObj);

	if(result != HR_ERROR)
	{
		THttpObj* pHttpObj = m_objPool.PickFreeHttpObj(this, pSocketObj);
		ENSURE(SetConnectionReserved(pSocketObj, pHttpObj));
	}

	return result;
}

template<class T, USHORT default_port> EnHandleResult CHttpAgentT<T, default_port>::DoFireHandShake(TSocketObj* pSocketObj)
{
	EnHandleResult result = __super::DoFireHandShake(pSocketObj);

	if(result == HR_ERROR)
	{
		THttpObj* pHttpObj = FindHttpObj(pSocketObj);
		ENSURE(pHttpObj);

		m_objPool.PutFreeHttpObj(pHttpObj);
		SetConnectionReserved(pSocketObj, nullptr);
	}

	return result;
}

template<class T, USHORT default_port> EnHandleResult CHttpAgentT<T, default_port>::DoFireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
{
	THttpObj* pHttpObj = FindHttpObj(pSocketObj);
	ASSERT(pHttpObj);

	return pHttpObj->Execute(pData, iLength);
}

template<class T, USHORT default_port> EnHandleResult CHttpAgentT<T, default_port>::DoFireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
{
	THttpObj* pHttpObj = FindHttpObj(pSocketObj);

	if(pHttpObj != nullptr)
		pHttpObj->CheckBodyIdentityEof();

	EnHandleResult result = __super::DoFireClose(pSocketObj, enOperation, iErrorCode);

	if(pHttpObj != nullptr)
		m_objPool.PutFreeHttpObj(pHttpObj);

	return result;
}

template<class T, USHORT default_port> EnHandleResult CHttpAgentT<T, default_port>::DoFireShutdown()
{
	EnHandleResult result = __super::DoFireShutdown();

	m_objPool.Clear();

	return result;
}

template<class T, USHORT default_port> BOOL CHttpAgentT<T, default_port>::IsUpgrade(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->IsUpgrade();
}

template<class T, USHORT default_port> BOOL CHttpAgentT<T, default_port>::IsKeepAlive(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->IsKeepAlive();
}

template<class T, USHORT default_port> USHORT CHttpAgentT<T, default_port>::GetVersion(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return 0;

	return pHttpObj->GetVersion();
}

template<class T, USHORT default_port> ULONGLONG CHttpAgentT<T, default_port>::GetContentLength(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return 0;

	return pHttpObj->GetContentLength();
}

template<class T, USHORT default_port> LPCSTR CHttpAgentT<T, default_port>::GetContentType(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetContentType();
}

template<class T, USHORT default_port> LPCSTR CHttpAgentT<T, default_port>::GetContentEncoding(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetContentEncoding();
}

template<class T, USHORT default_port> LPCSTR CHttpAgentT<T, default_port>::GetTransferEncoding(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetTransferEncoding();
}

template<class T, USHORT default_port> EnHttpUpgradeType CHttpAgentT<T, default_port>::GetUpgradeType(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return HUT_NONE;

	return pHttpObj->GetUpgradeType();
}

template<class T, USHORT default_port> USHORT CHttpAgentT<T, default_port>::GetParseErrorCode(CONNID dwConnID, LPCSTR* lpszErrorDesc)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return 0;

	return pHttpObj->GetParseErrorCode(lpszErrorDesc);
}

template<class T, USHORT default_port> BOOL CHttpAgentT<T, default_port>::GetHeader(CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetHeader(lpszName, lpszValue);
}

template<class T, USHORT default_port> BOOL CHttpAgentT<T, default_port>::GetHeaders(CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue[], DWORD& dwCount)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetHeaders(lpszName, lpszValue, dwCount);
}

template<class T, USHORT default_port> BOOL CHttpAgentT<T, default_port>::GetAllHeaders(CONNID dwConnID, THeader lpHeaders[], DWORD& dwCount)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetAllHeaders(lpHeaders, dwCount);
}

template<class T, USHORT default_port> BOOL CHttpAgentT<T, default_port>::GetAllHeaderNames(CONNID dwConnID, LPCSTR lpszName[], DWORD& dwCount)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetAllHeaderNames(lpszName, dwCount);
}

template<class T, USHORT default_port> BOOL CHttpAgentT<T, default_port>::GetCookie(CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetCookie(lpszName, lpszValue);
}

template<class T, USHORT default_port> BOOL CHttpAgentT<T, default_port>::GetAllCookies(CONNID dwConnID, TCookie lpCookies[], DWORD& dwCount)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetAllCookies(lpCookies, dwCount);
}

template<class T, USHORT default_port> USHORT CHttpAgentT<T, default_port>::GetStatusCode(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return 0;

	return pHttpObj->GetStatusCode();
}

template<class T, USHORT default_port> BOOL CHttpAgentT<T, default_port>::GetWSMessageState(CONNID dwConnID, BOOL* lpbFinal, BYTE* lpiReserved, BYTE* lpiOperationCode, LPCBYTE* lpszMask, ULONGLONG* lpullBodyLen, ULONGLONG* lpullBodyRemain)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetWSMessageState(lpbFinal, lpiReserved, lpiOperationCode, lpszMask, lpullBodyLen, lpullBodyRemain);
}

template<class T, USHORT default_port> inline typename CHttpAgentT<T, default_port>::THttpObj* CHttpAgentT<T, default_port>::FindHttpObj(CONNID dwConnID)
{
	THttpObj* pHttpObj = nullptr;
	GetConnectionReserved(dwConnID, (PVOID*)&pHttpObj);

	return pHttpObj;
}

template<class T, USHORT default_port> inline typename CHttpAgentT<T, default_port>::THttpObj* CHttpAgentT<T, default_port>::FindHttpObj(TSocketObj* pSocketObj)
{
	THttpObj* pHttpObj = nullptr;
	GetConnectionReserved(pSocketObj, (PVOID*)&pHttpObj);

	return pHttpObj;
}

// ------------------------------------------------------------------------------------------------------------- //

template class CHttpAgentT<CTcpAgent, HTTP_DEFAULT_PORT>;

#ifdef _SSL_SUPPORT

#include "SSLAgent.h"

template class CHttpAgentT<CSSLAgent, HTTPS_DEFAULT_PORT>;

#endif

#endif