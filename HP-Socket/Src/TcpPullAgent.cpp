/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.2.3
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
#include "TcpPullAgent.h"

EnHandleResult CTcpPullAgent::FireConnect(CONNID dwConnID)
{
	EnHandleResult result = __super::FireConnect(dwConnID);

	if(result != HR_ERROR)
		m_bfPool.PutCacheBuffer(dwConnID);

	return result;
}

EnHandleResult CTcpPullAgent::FireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
{
	TBuffer* pBuffer = m_bfPool[pSocketObj->connID];

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

		if(len > 0) return __super::FireReceive(pSocketObj, len);
	}

	return HR_IGNORE;
}

EnHandleResult CTcpPullAgent::FireClose(TSocketObj* pSocketObj)
{
	EnHandleResult result = __super::FireClose(pSocketObj);

	m_bfPool.PutFreeBuffer(pSocketObj->connID);

	return result;
}

EnHandleResult CTcpPullAgent::FireError(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
{
	EnHandleResult result = __super::FireError(pSocketObj, enOperation, iErrorCode);

	m_bfPool.PutFreeBuffer(pSocketObj->connID);

	return result;
}

EnHandleResult CTcpPullAgent::FireAgentShutdown()
{
	EnHandleResult result = __super::FireAgentShutdown();

	m_bfPool.Clear();

	return HR_OK;
}

EnFetchResult CTcpPullAgent::Fetch(CONNID dwConnID, BYTE* pData, int iLength)
{
	ASSERT(pData != nullptr && iLength > 0);

	EnFetchResult result	= FR_DATA_NOT_FOUND;
	TBuffer* pBuffer		= m_bfPool[dwConnID];

	if(pBuffer != nullptr && pBuffer->IsValid())
	{
		CCriSecLock locallock(pBuffer->CriSec());

		if(pBuffer->IsValid())
		{
			if(pBuffer->Length() >= iLength)
			{
				pBuffer->Fetch(pData, iLength);
				result = FR_OK;
			}
			else
				result = FR_LENGTH_TOO_LONG;
		}
	}

	return result;
}
