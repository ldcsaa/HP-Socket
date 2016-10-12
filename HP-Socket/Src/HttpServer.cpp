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
#include "HttpServer.h"

template<class T> BOOL CHttpServerT<T>::Start(LPCTSTR lpszBindAddress, USHORT usPort)
{
	BOOL isOK = __super::Start(lpszBindAddress, usPort);

	if(isOK) VERIFY(m_thCleaner.Start(this, &CHttpServerT::CleanerThreadProc));

	return isOK;
}

template<class T> BOOL CHttpServerT<T>::CheckParams()
{
	if	((m_enLocalVersion != HV_1_1 && m_enLocalVersion != HV_1_0)								||
		(m_dwReleaseDelay < MIN_HTTP_RELEASE_DELAY || m_dwReleaseDelay > MAX_HTTP_RELEASE_DELAY))
	{
		SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	return __super::CheckParams();
}

template<class T> BOOL CHttpServerT<T>::SendResponse(CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc, const THeader lpHeaders[], int iHeaderCount, const BYTE* pData, int iLength)
{
	WSABUF szBuffer[2];
	CStringA strHeader;

	::MakeStatusLine(m_enLocalVersion, usStatusCode, lpszDesc, strHeader);
	::MakeHeaderLines(lpHeaders, iHeaderCount, nullptr, iLength, FALSE, nullptr, 0, strHeader);
	::MakeHttpPacket(strHeader, pData, iLength, szBuffer);

	return SendPackets(dwConnID, szBuffer, 2);
}

template<class T> BOOL CHttpServerT<T>::SendLocalFile(CONNID dwConnID, LPCSTR lpszFileName, USHORT usStatusCode, LPCSTR lpszDesc, const THeader lpHeaders[], int iHeaderCount)
{
	CAtlFile file;
	CAtlFileMapping<> fmap;

	HRESULT hr = ::ReadSmallFile(CA2T(lpszFileName), file, fmap);

	if(FAILED(hr))
	{
		::SetLastError(HRESULT_CODE(hr));
		return FALSE;
	}

	return SendResponse(dwConnID, usStatusCode, lpszDesc, lpHeaders, iHeaderCount, (BYTE*)(char*)fmap, (int)fmap.GetMappingSize());
}

template<class T> BOOL CHttpServerT<T>::Release(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr || pHttpObj->HasReleased())
		return FALSE;

	pHttpObj->Release();
	m_lsDyingQueue.PushBack(TDyingConnection::Construct(dwConnID));

	return TRUE;
}

template<class T> UINT CHttpServerT<T>::CleanerThreadProc()
{
	TRACE("---------------> Connection Cleaner Thread 0x%08X started <---------------\n", ::GetCurrentThreadId());

	DWORD dwRetValue = 0;
	DWORD dwInterval = max(MIN_HTTP_RELEASE_CHECK_INTERVAL, (m_dwReleaseDelay - MIN_HTTP_RELEASE_DELAY / 2));

	while(HasStarted())
	{
		dwRetValue = ::WaitForSingleObject(m_evCleaner, dwInterval);

		if(dwRetValue == WAIT_TIMEOUT)
			KillDyingConnection();
		else if(dwRetValue == WAIT_OBJECT_0)
			break;
		else
			ASSERT(FALSE);
	}

	ReleaseDyingConnection();

	TRACE("---------------> Connection Cleaner Thread 0x%08X stoped <---------------\n", ::GetCurrentThreadId());

	return 0;
}

template<class T> void CHttpServerT<T>::KillDyingConnection()
{
	TDyingConnection* pDyingConn = nullptr;
	DWORD now					 = ::TimeGetTime();

	while(m_lsDyingQueue.UnsafePeekFront(&pDyingConn))
	{
		if((int)(now - pDyingConn->killTime) < (int)m_dwReleaseDelay)
			break;

		Disconnect(pDyingConn->connID, TRUE);

		VERIFY(m_lsDyingQueue.UnsafePopFront(&pDyingConn));
		TDyingConnection::Destruct(pDyingConn);
	}
}

template<class T> void CHttpServerT<T>::ReleaseDyingConnection()
{
	TDyingConnection* pDyingConn = nullptr;

	while(m_lsDyingQueue.UnsafePopFront(&pDyingConn))
		TDyingConnection::Destruct(pDyingConn);

	VERIFY(m_lsDyingQueue.IsEmpty());
}

template<class T> EnHandleResult CHttpServerT<T>::DoFireAccept(TSocketObj* pSocketObj)
{
	EnHandleResult result = __super::DoFireAccept(pSocketObj);

	if(result != HR_ERROR)
		VERIFY(SetConnectionReserved(pSocketObj, THttpObj::Construct(TRUE, this, pSocketObj)));

	return result;
}

template<class T> EnHandleResult CHttpServerT<T>::DoFireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
{
	THttpObj* pHttpObj = FindHttpObj(pSocketObj);
	ASSERT(pHttpObj);

	if(pHttpObj->HasReleased())
		return HR_IGNORE;

	return pHttpObj->Execute(pData, iLength);
}

template<class T> EnHandleResult CHttpServerT<T>::DoFireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
{
	EnHandleResult result = __super::DoFireClose(pSocketObj, enOperation, iErrorCode);

	THttpObj* pHttpObj = FindHttpObj(pSocketObj);

	if(pHttpObj != nullptr)
		THttpObj::Destruct(pHttpObj);

	return result;
}

template<class T> EnHandleResult CHttpServerT<T>::DoFireShutdown()
{
	EnHandleResult result = __super::DoFireShutdown();

	WaitForCleanerThreadEnd();

	return result;
}

template<class T> void CHttpServerT<T>::WaitForCleanerThreadEnd()
{
	if(m_thCleaner.IsRunning())
	{
		m_evCleaner.Set();
		VERIFY(m_thCleaner.Join(TRUE));
		m_evCleaner.Reset();
	}
}

template<class T> BOOL CHttpServerT<T>::IsUpgrade(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->IsUpgrade();
}

template<class T> BOOL CHttpServerT<T>::IsKeepAlive(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->IsKeepAlive();
}

template<class T> USHORT CHttpServerT<T>::GetVersion(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return 0;

	return pHttpObj->GetVersion();
}

template<class T> LPCSTR CHttpServerT<T>::GetHost(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetHost();
}

template<class T> ULONGLONG CHttpServerT<T>::GetContentLength(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return 0;

	return pHttpObj->GetContentLength();
}

template<class T> LPCSTR CHttpServerT<T>::GetContentType(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetContentType();
}

template<class T> LPCSTR CHttpServerT<T>::GetContentEncoding(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetContentEncoding();
}

template<class T> LPCSTR CHttpServerT<T>::GetTransferEncoding(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetTransferEncoding();
}

template<class T> EnHttpUpgradeType CHttpServerT<T>::GetUpgradeType(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return HUT_NONE;

	return pHttpObj->GetUpgradeType();
}

template<class T> USHORT CHttpServerT<T>::GetParseErrorCode(CONNID dwConnID, LPCSTR* lpszErrorDesc)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return 0;

	return pHttpObj->GetParseErrorCode(lpszErrorDesc);
}

template<class T> BOOL CHttpServerT<T>::GetHeader(CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetHeader(lpszName, lpszValue);
}

template<class T> BOOL CHttpServerT<T>::GetHeaders(CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue[], DWORD& dwCount)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetHeaders(lpszName, lpszValue, dwCount);
}

template<class T> BOOL CHttpServerT<T>::GetAllHeaders(CONNID dwConnID, THeader lpHeaders[], DWORD& dwCount)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetAllHeaders(lpHeaders, dwCount);
}

template<class T> BOOL CHttpServerT<T>::GetAllHeaderNames(CONNID dwConnID, LPCSTR lpszName[], DWORD& dwCount)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetAllHeaderNames(lpszName, dwCount);
}

template<class T> BOOL CHttpServerT<T>::GetCookie(CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetCookie(lpszName, lpszValue);
}

template<class T> BOOL CHttpServerT<T>::GetAllCookies(CONNID dwConnID, TCookie lpCookies[], DWORD& dwCount)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetAllCookies(lpCookies, dwCount);
}

template<class T> USHORT CHttpServerT<T>::GetUrlFieldSet(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return 0;

	return pHttpObj->GetUrlFieldSet();
}

template<class T> LPCSTR CHttpServerT<T>::GetUrlField(CONNID dwConnID, EnHttpUrlField enField)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetUrlField(enField);
}

template<class T> LPCSTR CHttpServerT<T>::GetMethod(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetMethod();
}

template<class T> inline typename CHttpServerT<T>::THttpObj* CHttpServerT<T>::FindHttpObj(CONNID dwConnID)
{
	THttpObj* pHttpObj = nullptr;
	GetConnectionReserved(dwConnID, (PVOID*)&pHttpObj);

	return pHttpObj;
}

template<class T> inline typename CHttpServerT<T>::THttpObj* CHttpServerT<T>::FindHttpObj(TSocketObj* pSocketObj)
{
	THttpObj* pHttpObj = nullptr;
	GetConnectionReserved(pSocketObj, (PVOID*)&pHttpObj);

	return pHttpObj;
}

template class CHttpServerT<CTcpServer>;

#ifdef _SSL_SUPPORT

#include "SSLServer.h"

template class CHttpServerT<CSSLServer>;

#endif
