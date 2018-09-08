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
#include "SSLClient.h"
#include "SSLHelper.h"

#ifdef _SSL_SUPPORT

BOOL CSSLClient::CheckParams()
{
	if(!m_sslCtx.IsValid())
	{
		SetLastError(SE_SSL_ENV_NOT_READY, __FUNCTION__, ERROR_NOT_READY);
		return FALSE;
	}

	return __super::CheckParams();
}

void CSSLClient::PrepareStart()
{
	m_dwMainThreadID = ::GetCurrentThreadId();

	__super::PrepareStart();
}

void CSSLClient::Reset()
{
	m_sslSession.Reset();

	if(m_dwMainThreadID != 0)
	{
		m_sslCtx.RemoveThreadLocalState(m_dwMainThreadID);
		m_dwMainThreadID = 0;
	}

	__super::Reset();
}

void CSSLClient::OnWorkerThreadEnd(DWORD dwThreadID)
{
	m_sslCtx.RemoveThreadLocalState();

	__super::OnWorkerThreadEnd(dwThreadID);
}

BOOL CSSLClient::SendPackets(const WSABUF pBuffers[], int iCount)
{
	ASSERT(pBuffers && iCount > 0);

	return ::ProcessSend(this, this, &m_sslSession, pBuffers, iCount);
}

EnHandleResult CSSLClient::FireConnect()
{
	EnHandleResult result = DoFireConnect(this);

	if(result != HR_ERROR)
	{
		m_sslSession.Renew(m_sslCtx, m_strHost);
		ENSURE(::ProcessHandShake(this, this, &m_sslSession) == HR_OK);
	}

	return result;
}

EnHandleResult CSSLClient::FireReceive(const BYTE* pData, int iLength)
{
	return ::ProcessReceive(this, this, &m_sslSession, pData, iLength);
}

#endif