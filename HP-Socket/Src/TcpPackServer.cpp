/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.4.1
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
#include "TcpPackServer.h"
#include "MiscHelper.h"

BOOL CTcpPackServer::CheckParams()
{
	m_bfPool.SetItemCapacity	(GetSocketBufferSize());
	m_bfPool.SetItemPoolSize	(GetFreeBufferObjPool());
	m_bfPool.SetItemPoolHold	(GetFreeBufferObjHold());
	m_bfPool.SetBufferLockTime	(GetFreeSocketObjLockTime());
	m_bfPool.SetBufferPoolSize	(GetFreeSocketObjPool());
	m_bfPool.SetBufferPoolHold	(GetFreeSocketObjHold());

	if(m_dwMaxPackSize > 0 && m_dwMaxPackSize <= TCP_PACK_MAX_SIZE_LIMIT)
		if(m_usHeaderFlag >= 0 && m_usHeaderFlag <= TCP_PACK_HEADER_FLAG_LIMIT)
			return __super::CheckParams();;

	SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
	return FALSE;
}

BOOL CTcpPackServer::SendPackets(CONNID dwConnID, const WSABUF pBuffers[], int iCount)
{
	int iNewCount = iCount + 1;
	unique_ptr<WSABUF[]> buffers(new WSABUF[iNewCount]);

	DWORD header;
	if(!::AddPackHeader(pBuffers, iCount, buffers, m_dwMaxPackSize, m_usHeaderFlag, header))
		return FALSE;

	return __super::SendPackets(dwConnID, buffers.get(), iNewCount);
}

EnHandleResult CTcpPackServer::FireAccept(TSocketObj* pSocketObj)
{
	EnHandleResult result = __super::FireAccept(pSocketObj);

	if(result != HR_ERROR)
	{
		TBuffer* pBuffer = m_bfPool.PutCacheBuffer(pSocketObj->connID);
		VERIFY(SetConnectionReserved(pSocketObj, TBufferPackInfo::Construct(pBuffer)));
	}

	return result;
}

EnHandleResult CTcpPackServer::FireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
{
	TBufferPackInfo* pInfo = nullptr;
	GetConnectionReserved(pSocketObj, (PVOID*)&pInfo);
	ASSERT(pInfo);

	TBuffer* pBuffer = (TBuffer*)pInfo->pBuffer;
	ASSERT(pBuffer && pBuffer->IsValid());

	return ParsePack(this, pInfo, pBuffer, pSocketObj, m_dwMaxPackSize, m_usHeaderFlag, pData, iLength);
}

EnHandleResult CTcpPackServer::FireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
{
	EnHandleResult result = __super::FireClose(pSocketObj, enOperation, iErrorCode);

	TBufferPackInfo* pInfo = nullptr;
	GetConnectionReserved(pSocketObj, (PVOID*)&pInfo);
	ASSERT(pInfo);

	TBufferPackInfo::Destruct(pInfo);
	m_bfPool.PutFreeBuffer(pSocketObj->connID);

	return result;
}

EnHandleResult CTcpPackServer::FireShutdown()
{
	EnHandleResult result = __super::FireShutdown();

	m_bfPool.Clear();

	return HR_OK;
}
