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
#include "SSLAgent.h"
#include "SSLHelper.h"

#ifdef _SSL_SUPPORT

BOOL CSSLAgent::CheckParams()
{
	if(!m_sslCtx.IsValid())
	{
		SetLastError(SE_SSL_ENV_NOT_READY, __FUNCTION__, ERROR_NOT_READY);
		return FALSE;
	}

	return __super::CheckParams();
}

void CSSLAgent::PrepareStart()
{
	__super::PrepareStart();

	m_sslPool.SetItemCapacity	(GetSocketBufferSize());
	m_sslPool.SetItemPoolSize	(GetFreeBufferObjPool());
	m_sslPool.SetItemPoolHold	(GetFreeBufferObjHold());
	m_sslPool.SetSessionLockTime(GetFreeSocketObjLockTime());
	m_sslPool.SetSessionPoolSize(GetFreeSocketObjPool());
	m_sslPool.SetSessionPoolHold(GetFreeSocketObjHold());

	m_sslPool.Prepare();
}

void CSSLAgent::Reset()
{
	m_sslCtx.RemoveThreadLocalState();

	__super::Reset();
}

void CSSLAgent::OnWorkerThreadEnd(DWORD dwThreadID)
{
	m_sslCtx.RemoveThreadLocalState();

	__super::OnWorkerThreadEnd(dwThreadID);
}

BOOL CSSLAgent::SendPackets(CONNID dwConnID, const WSABUF pBuffers[], int iCount)
{
	ASSERT(pBuffers && iCount > 0);

	TSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(!TSocketObj::IsValid(pSocketObj))
	{
		::SetLastError(ERROR_OBJECT_NOT_FOUND);
		return FALSE;
	}

	CSSLSession* pSession = nullptr;
	GetConnectionReserved2(pSocketObj, (PVOID*)&pSession);

	return ::ProcessSend(this, pSocketObj, pSession, pBuffers, iCount);
}

EnHandleResult CSSLAgent::FireConnect(TSocketObj* pSocketObj)
{
	EnHandleResult result = DoFireConnect(pSocketObj);

	if(result != HR_ERROR)
	{
		CSSLSession* pSession = m_sslPool.PickFreeSession(pSocketObj->host);

		ENSURE(SetConnectionReserved2(pSocketObj, pSession));
		ENSURE(::ProcessHandShake(this, pSocketObj, pSession) == HR_OK);
	}

	return result;
}

EnHandleResult CSSLAgent::FireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
{
	CSSLSession* pSession = nullptr;
	GetConnectionReserved2(pSocketObj, (PVOID*)&pSession);
	ASSERT(pSession);

	return ::ProcessReceive(this, pSocketObj, pSession, pData, iLength);
}

EnHandleResult CSSLAgent::FireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
{
	EnHandleResult result = DoFireClose(pSocketObj, enOperation, iErrorCode);

	CSSLSession* pSession = nullptr;
	GetConnectionReserved2(pSocketObj, (PVOID*)&pSession);

	if(pSession != nullptr)
		m_sslPool.PutFreeSession(pSession);

	return result;
}

EnHandleResult CSSLAgent::FireShutdown()
{
	EnHandleResult result = DoFireShutdown();

	m_sslPool.Clear();

	return result;
}

#endif