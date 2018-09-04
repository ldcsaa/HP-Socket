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
#include "HttpServer.h"

#ifdef _HTTP_SUPPORT

template<class T, USHORT default_port> BOOL CHttpServerT<T, default_port>::Start(LPCTSTR lpszBindAddress, USHORT usPort)
{
	BOOL isOK = __super::Start(lpszBindAddress, usPort);

	if(isOK) ENSURE(m_thCleaner.Start(this, &CHttpServerT::CleanerThreadProc));

	return isOK;
}

template<class T, USHORT default_port> BOOL CHttpServerT<T, default_port>::CheckParams()
{
	if	((m_enLocalVersion != HV_1_1 && m_enLocalVersion != HV_1_0)								||
		(m_dwReleaseDelay < MIN_HTTP_RELEASE_DELAY || m_dwReleaseDelay > MAX_HTTP_RELEASE_DELAY))
	{
		SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	return __super::CheckParams();
}

template<class T, USHORT default_port> void CHttpServerT<T, default_port>::PrepareStart()
{
	__super::PrepareStart();

	m_objPool.SetHttpObjLockTime(GetFreeSocketObjLockTime());
	m_objPool.SetHttpObjPoolSize(GetFreeSocketObjPool());
	m_objPool.SetHttpObjPoolHold(GetFreeSocketObjHold());

	m_objPool.Prepare();
}

template<class T, USHORT default_port> BOOL CHttpServerT<T, default_port>::SendResponse(CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc, const THeader lpHeaders[], int iHeaderCount, const BYTE* pData, int iLength)
{
	WSABUF szBuffer[2];
	CStringA strHeader;

	::MakeStatusLine(m_enLocalVersion, usStatusCode, lpszDesc, strHeader);
	::MakeHeaderLines(lpHeaders, iHeaderCount, nullptr, iLength, FALSE, IsKeepAlive(dwConnID), nullptr, 0, strHeader);
	::MakeHttpPacket(strHeader, pData, iLength, szBuffer);

	return SendPackets(dwConnID, szBuffer, 2);
}

template<class T, USHORT default_port> BOOL CHttpServerT<T, default_port>::SendLocalFile(CONNID dwConnID, LPCSTR lpszFileName, USHORT usStatusCode, LPCSTR lpszDesc, const THeader lpHeaders[], int iHeaderCount)
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

template<class T, USHORT default_port> BOOL CHttpServerT<T, default_port>::Release(CONNID dwConnID)
{
	if(!HasStarted())
		return FALSE;

	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr || pHttpObj->HasReleased())
		return FALSE;

	pHttpObj->Release();

	m_lsDyingQueue.PushBack(TDyingConnection::Construct(dwConnID));

	return TRUE;
}

template<class T, USHORT default_port> BOOL CHttpServerT<T, default_port>::SendWSMessage(CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], BYTE* pData, int iLength, ULONGLONG ullBodyLen)
{
	WSABUF szBuffer[2];
	BYTE szHeader[HTTP_MAX_WS_HEADER_LEN];

	if(!::MakeWSPacket(bFinal, iReserved, iOperationCode, lpszMask, pData, iLength, ullBodyLen, szHeader, szBuffer))
		return FALSE;

	return SendPackets(dwConnID, szBuffer, 2);
}

template<class T, USHORT default_port> UINT CHttpServerT<T, default_port>::CleanerThreadProc(PVOID pv)
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

template<class T, USHORT default_port> void CHttpServerT<T, default_port>::KillDyingConnection()
{
	TDyingConnection* pDyingConn		= nullptr;
	TDyingConnection* pFirstDyingConn	= nullptr;
	DWORD now							= ::TimeGetTime();

	while(m_lsDyingQueue.UnsafePeekFront(&pDyingConn))
	{
		if((int)(now - pDyingConn->killTime) < (int)m_dwReleaseDelay)
			break;

		BOOL bDisconnect = TRUE;
		BOOL bDestruct	 = TRUE;

		ENSURE(m_lsDyingQueue.UnsafePopFront(&pDyingConn));

		int iPending;
		if(!GetPendingDataLength(pDyingConn->connID, iPending))
			bDisconnect = FALSE;
		else if(iPending > 0)
		{
			bDisconnect	= FALSE;
			bDestruct	= FALSE;
		}

		if(bDisconnect)
			Disconnect(pDyingConn->connID, TRUE);

		if(bDestruct)
		{
			TDyingConnection::Destruct(pDyingConn);

			if(pFirstDyingConn == pDyingConn)
				pFirstDyingConn = nullptr;
		}
		else
		{
			m_lsDyingQueue.PushBack(pDyingConn);

			if(pFirstDyingConn == nullptr)
				pFirstDyingConn = pDyingConn;
			else if(pFirstDyingConn == pDyingConn)
				break;
		}
	}
}

template<class T, USHORT default_port> void CHttpServerT<T, default_port>::ReleaseDyingConnection()
{
	TDyingConnection* pDyingConn = nullptr;

	while(m_lsDyingQueue.UnsafePopFront(&pDyingConn))
		TDyingConnection::Destruct(pDyingConn);

	ENSURE(m_lsDyingQueue.IsEmpty());
}

template<class T, USHORT default_port> EnHandleResult CHttpServerT<T, default_port>::DoFireAccept(TSocketObj* pSocketObj)
{
	EnHandleResult result = __super::DoFireAccept(pSocketObj);

	if(result != HR_ERROR)
	{
		THttpObj* pHttpObj = m_objPool.PickFreeHttpObj(this, pSocketObj);
		ENSURE(SetConnectionReserved(pSocketObj, pHttpObj));
	}

	return result;
}

template<class T, USHORT default_port> EnHandleResult CHttpServerT<T, default_port>::DoFireHandShake(TSocketObj* pSocketObj)
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

template<class T, USHORT default_port> EnHandleResult CHttpServerT<T, default_port>::DoFireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
{
	THttpObj* pHttpObj = FindHttpObj(pSocketObj);
	ASSERT(pHttpObj);

	if(pHttpObj->HasReleased())
		return HR_ERROR;

	return pHttpObj->Execute(pData, iLength);
}

template<class T, USHORT default_port> EnHandleResult CHttpServerT<T, default_port>::DoFireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
{
	EnHandleResult result = __super::DoFireClose(pSocketObj, enOperation, iErrorCode);

	THttpObj* pHttpObj = FindHttpObj(pSocketObj);

	if(pHttpObj != nullptr)
		m_objPool.PutFreeHttpObj(pHttpObj);

	return result;
}

template<class T, USHORT default_port> EnHandleResult CHttpServerT<T, default_port>::DoFireShutdown()
{
	EnHandleResult result = __super::DoFireShutdown();

	m_objPool.Clear();
	WaitForCleanerThreadEnd();

	return result;
}

template<class T, USHORT default_port> void CHttpServerT<T, default_port>::WaitForCleanerThreadEnd()
{
	if(m_thCleaner.IsRunning())
	{
		m_evCleaner.Set();
		ENSURE(m_thCleaner.Join(TRUE));
		m_evCleaner.Reset();
	}
}

template<class T, USHORT default_port> BOOL CHttpServerT<T, default_port>::IsUpgrade(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->IsUpgrade();
}

template<class T, USHORT default_port> BOOL CHttpServerT<T, default_port>::IsKeepAlive(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->IsKeepAlive();
}

template<class T, USHORT default_port> USHORT CHttpServerT<T, default_port>::GetVersion(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return 0;

	return pHttpObj->GetVersion();
}

template<class T, USHORT default_port> LPCSTR CHttpServerT<T, default_port>::GetHost(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetHost();
}

template<class T, USHORT default_port> ULONGLONG CHttpServerT<T, default_port>::GetContentLength(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return 0;

	return pHttpObj->GetContentLength();
}

template<class T, USHORT default_port> LPCSTR CHttpServerT<T, default_port>::GetContentType(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetContentType();
}

template<class T, USHORT default_port> LPCSTR CHttpServerT<T, default_port>::GetContentEncoding(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetContentEncoding();
}

template<class T, USHORT default_port> LPCSTR CHttpServerT<T, default_port>::GetTransferEncoding(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetTransferEncoding();
}

template<class T, USHORT default_port> EnHttpUpgradeType CHttpServerT<T, default_port>::GetUpgradeType(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return HUT_NONE;

	return pHttpObj->GetUpgradeType();
}

template<class T, USHORT default_port> USHORT CHttpServerT<T, default_port>::GetParseErrorCode(CONNID dwConnID, LPCSTR* lpszErrorDesc)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return 0;

	return pHttpObj->GetParseErrorCode(lpszErrorDesc);
}

template<class T, USHORT default_port> BOOL CHttpServerT<T, default_port>::GetHeader(CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetHeader(lpszName, lpszValue);
}

template<class T, USHORT default_port> BOOL CHttpServerT<T, default_port>::GetHeaders(CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue[], DWORD& dwCount)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetHeaders(lpszName, lpszValue, dwCount);
}

template<class T, USHORT default_port> BOOL CHttpServerT<T, default_port>::GetAllHeaders(CONNID dwConnID, THeader lpHeaders[], DWORD& dwCount)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetAllHeaders(lpHeaders, dwCount);
}

template<class T, USHORT default_port> BOOL CHttpServerT<T, default_port>::GetAllHeaderNames(CONNID dwConnID, LPCSTR lpszName[], DWORD& dwCount)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetAllHeaderNames(lpszName, dwCount);
}

template<class T, USHORT default_port> BOOL CHttpServerT<T, default_port>::GetCookie(CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetCookie(lpszName, lpszValue);
}

template<class T, USHORT default_port> BOOL CHttpServerT<T, default_port>::GetAllCookies(CONNID dwConnID, TCookie lpCookies[], DWORD& dwCount)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetAllCookies(lpCookies, dwCount);
}

template<class T, USHORT default_port> USHORT CHttpServerT<T, default_port>::GetUrlFieldSet(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return 0;

	return pHttpObj->GetUrlFieldSet();
}

template<class T, USHORT default_port> LPCSTR CHttpServerT<T, default_port>::GetUrlField(CONNID dwConnID, EnHttpUrlField enField)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetUrlField(enField);
}

template<class T, USHORT default_port> LPCSTR CHttpServerT<T, default_port>::GetMethod(CONNID dwConnID)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return nullptr;

	return pHttpObj->GetMethod();
}

template<class T, USHORT default_port> BOOL CHttpServerT<T, default_port>::GetWSMessageState(CONNID dwConnID, BOOL* lpbFinal, BYTE* lpiReserved, BYTE* lpiOperationCode, LPCBYTE* lpszMask, ULONGLONG* lpullBodyLen, ULONGLONG* lpullBodyRemain)
{
	THttpObj* pHttpObj = FindHttpObj(dwConnID);

	if(pHttpObj == nullptr)
		return FALSE;

	return pHttpObj->GetWSMessageState(lpbFinal, lpiReserved, lpiOperationCode, lpszMask, lpullBodyLen, lpullBodyRemain);
}

template<class T, USHORT default_port> inline typename CHttpServerT<T, default_port>::THttpObj* CHttpServerT<T, default_port>::FindHttpObj(CONNID dwConnID)
{
	THttpObj* pHttpObj = nullptr;
	GetConnectionReserved(dwConnID, (PVOID*)&pHttpObj);

	return pHttpObj;
}

template<class T, USHORT default_port> inline typename CHttpServerT<T, default_port>::THttpObj* CHttpServerT<T, default_port>::FindHttpObj(TSocketObj* pSocketObj)
{
	THttpObj* pHttpObj = nullptr;
	GetConnectionReserved(pSocketObj, (PVOID*)&pHttpObj);

	return pHttpObj;
}

// ------------------------------------------------------------------------------------------------------------- //

template class CHttpServerT<CTcpServer, HTTP_DEFAULT_PORT>;

#ifdef _SSL_SUPPORT

#include "SSLServer.h"

template class CHttpServerT<CSSLServer, HTTPS_DEFAULT_PORT>;

#endif

#endif