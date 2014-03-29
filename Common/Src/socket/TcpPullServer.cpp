/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.1.3
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
#include "TcpPullServer.h"

ISocketListener::EnHandleResult CTcpPullServer::FireAccept(CONNID dwConnID, SOCKET soClient)
{
	ISocketListener::EnHandleResult result = __super::FireAccept(dwConnID, soClient);

	if(result != ISocketListener::HR_ERROR)
		m_bfPool.PutCacheBuffer(dwConnID);

	return result;
}

ISocketListener::EnHandleResult CTcpPullServer::FireReceive(CONNID dwConnID, const BYTE* pData, int iLength)
{
	TBuffer* pBuffer = m_bfPool[dwConnID];

	if(pBuffer != nullptr && pBuffer->IsValid())
	{
		int len = 0;

		{
			CCriSecLock locallock(pBuffer->CriSec());

			if(pBuffer->IsValid())
			{
				pBuffer->Cat(pData, iLength);
				len = pBuffer->Length();
			}
		}

		if(len > 0) return __super::FireReceive(dwConnID, len);
	}

	return ISocketListener::HR_IGNORE;
}

ISocketListener::EnHandleResult CTcpPullServer::FireClose(CONNID dwConnID)
{
	ISocketListener::EnHandleResult result = __super::FireClose(dwConnID);

	m_bfPool.PutFreeBuffer(dwConnID);

	return result;
}

ISocketListener::EnHandleResult CTcpPullServer::FireError(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	ISocketListener::EnHandleResult result = __super::FireError(dwConnID, enOperation, iErrorCode);

	m_bfPool.PutFreeBuffer(dwConnID);

	return result;
}

ISocketListener::EnHandleResult CTcpPullServer::FireServerShutdown()
{
	ISocketListener::EnHandleResult result = __super::FireServerShutdown();

	m_bfPool.Clear();

	return ISocketListener::HR_OK;
}

IPullSocket::EnFetchResult CTcpPullServer::Fetch(CONNID dwConnID, BYTE* pData, int iLength)
{
	ASSERT(pData != nullptr && iLength > 0);

	EnFetchResult result	= IPullSocket::FR_DATA_NOT_FOUND;
	TBuffer* pBuffer		= m_bfPool[dwConnID];

	if(pBuffer != nullptr && pBuffer->IsValid())
	{
		CCriSecLock locallock(pBuffer->CriSec());

		if(pBuffer->IsValid())
		{
			if(pBuffer->Length() >= iLength)
			{
				pBuffer->Fetch(pData, iLength);
				result = IPullSocket::FR_OK;
			}
			else
				result = IPullSocket::FR_LENGTH_TOO_LONG;
		}
	}

	return result;
}
