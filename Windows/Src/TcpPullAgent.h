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
 
#pragma once

#include "TcpAgent.h"
#include "../Common/Src/BufferPool.h"

template<class T> class CTcpPullAgentT : public IPullSocket, public T
{
public:
	virtual EnFetchResult Fetch(CONNID dwConnID, BYTE* pData, int iLength)
	{
		TBuffer* pBuffer = m_bfPool[dwConnID];
		return ::FetchBuffer(pBuffer, pData, iLength);
	}

	virtual EnFetchResult Peek(CONNID dwConnID, BYTE* pData, int iLength)
	{
		TBuffer* pBuffer = m_bfPool[dwConnID];
		return ::PeekBuffer(pBuffer, pData, iLength);
	}

protected:
	virtual EnHandleResult DoFireHandShake(TSocketObj* pSocketObj)
	{
		EnHandleResult result = __super::DoFireHandShake(pSocketObj);

		if(result != HR_ERROR)
		{
			TBuffer* pBuffer = m_bfPool.PutCacheBuffer(pSocketObj->connID);
			ENSURE(SetConnectionReserved(pSocketObj, pBuffer));
		}

		return result;
	}

	virtual EnHandleResult DoFireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
	{
		TBuffer* pBuffer = nullptr;
		GetConnectionReserved(pSocketObj, (PVOID*)&pBuffer);
		ASSERT(pBuffer && pBuffer->IsValid());

		pBuffer->Cat(pData, iLength);

		return __super::DoFireReceive(pSocketObj, pBuffer->Length());
	}

	virtual EnHandleResult DoFireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
	{
		EnHandleResult result = __super::DoFireClose(pSocketObj, enOperation, iErrorCode);

		TBuffer* pBuffer = nullptr;
		GetConnectionReserved(pSocketObj, (PVOID*)&pBuffer);

		if(pBuffer != nullptr)
			m_bfPool.PutFreeBuffer(pBuffer);

		return result;
	}

	virtual EnHandleResult DoFireShutdown()
	{
		EnHandleResult result = __super::DoFireShutdown();

		m_bfPool.Clear();

		return result;
	}

	virtual void PrepareStart()
	{
		__super::PrepareStart();

		m_bfPool.SetMaxCacheSize	(GetMaxConnectionCount());
		m_bfPool.SetItemCapacity	(GetSocketBufferSize());
		m_bfPool.SetItemPoolSize	(GetFreeBufferObjPool());
		m_bfPool.SetItemPoolHold	(GetFreeBufferObjHold());
		m_bfPool.SetBufferLockTime	(GetFreeSocketObjLockTime());
		m_bfPool.SetBufferPoolSize	(GetFreeSocketObjPool());
		m_bfPool.SetBufferPoolHold	(GetFreeSocketObjHold());

		m_bfPool.Prepare();
	}

public:
	CTcpPullAgentT(ITcpAgentListener* pListener)
	: T(pListener)
	{

	}

	virtual ~CTcpPullAgentT()
	{
		Stop();
	}

private:
	CBufferPool m_bfPool;
};

typedef CTcpPullAgentT<CTcpAgent> CTcpPullAgent;

#ifdef _SSL_SUPPORT

#include "SSLAgent.h"
typedef CTcpPullAgentT<CSSLAgent> CSSLPullAgent;

#endif
