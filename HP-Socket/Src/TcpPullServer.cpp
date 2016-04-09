/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.4.2
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
#include "MiscHelper.h"

EnHandleResult CTcpPullServer::FireAccept(TSocketObj* pSocketObj)
{
	EnHandleResult result = __super::FireAccept(pSocketObj);

	if(result != HR_ERROR)
	{
		TBuffer* pBuffer = m_bfPool.PutCacheBuffer(pSocketObj->connID);
		VERIFY(SetConnectionReserved(pSocketObj, pBuffer));
	}

	return result;
}

EnHandleResult CTcpPullServer::FireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
{
	TBuffer* pBuffer = nullptr;
	GetConnectionReserved(pSocketObj, (PVOID*)&pBuffer);
	ASSERT(pBuffer && pBuffer->IsValid());

	pBuffer->Cat(pData, iLength);

	return __super::FireReceive(pSocketObj, pBuffer->Length());
}

EnHandleResult CTcpPullServer::FireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
{
	EnHandleResult result = __super::FireClose(pSocketObj, enOperation, iErrorCode);

	m_bfPool.PutFreeBuffer(pSocketObj->connID);

	return result;
}

EnHandleResult CTcpPullServer::FireShutdown()
{
	EnHandleResult result = __super::FireShutdown();

	m_bfPool.Clear();

	return result;
}

EnFetchResult CTcpPullServer::Fetch(CONNID dwConnID, BYTE* pData, int iLength)
{
	TBuffer* pBuffer = m_bfPool[dwConnID];
	return ::FetchBuffer(pBuffer, pData, iLength);
}

EnFetchResult CTcpPullServer::Peek(CONNID dwConnID, BYTE* pData, int iLength)
{
	TBuffer* pBuffer = m_bfPool[dwConnID];
	return ::PeekBuffer(pBuffer, pData, iLength);
}
