/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Author	: Bruce Liang
 * Website	: https://github.com/ldcsaa
 * Project	: https://github.com/ldcsaa/HP-Socket
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 44636872, 75375912
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
 
#include "UdpArqServer.h"

#ifdef _UDP_SUPPORT

BOOL CUdpArqServer::CheckParams()
{
	DWORD dwMaxDatagramSize = GetMaxDatagramSize();

	if(m_dwMtu == 0)
		m_arqAttr.dwMtu = dwMaxDatagramSize;
	else
	{
		if(m_dwMtu > dwMaxDatagramSize)
			return FALSE;

		m_arqAttr.dwMtu = m_dwMtu;
	}

	return __super::CheckParams() && m_arqAttr.IsValid();
}

void CUdpArqServer::PrepareStart()
{
	__super::PrepareStart();

	m_ssPool.SetSessionLockTime(GetFreeSocketObjLockTime());
	m_ssPool.SetSessionPoolSize(GetFreeSocketObjPool());
	m_ssPool.SetSessionPoolHold(GetFreeSocketObjHold());

	m_ssPool.Prepare();
}

void CUdpArqServer::Reset()
{
	::ClearPtrMap(m_rcBuffers);

	m_ssPool.Clear();

	__super::Reset();
}

void CUdpArqServer::OnWorkerThreadStart(THR_ID dwThreadID)
{
	{
		CCriSecLock locallock(m_csRcBuffers);
		m_rcBuffers[dwThreadID] = new CBufferPtr(m_arqAttr.dwMaxMessageSize);
	}

	while((DWORD)m_rcBuffers.size() < GetWorkerThreadCount())
		::WaitFor(3);
}

BOOL CUdpArqServer::Send(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset)
{
	ASSERT(pBuffer && iLength > 0 && iLength <= (int)m_arqAttr.dwMaxMessageSize);

	int result = NO_ERROR;

	if(pBuffer && iLength > 0 && iLength <= (int)m_arqAttr.dwMaxMessageSize)
	{
		if(iOffset != 0) pBuffer += iOffset;
		TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

		if(TUdpSocketObj::IsValid(pSocketObj))
			result = SendArq(pSocketObj, pBuffer, iLength);
		else
			result = ERROR_OBJECT_NOT_FOUND;
	}
	else
		result = ERROR_INVALID_PARAMETER;

	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);
}

BOOL CUdpArqServer::SendPackets(CONNID dwConnID, const WSABUF pBuffers[], int iCount)
{
	ASSERT(pBuffers && iCount > 0);

	if(!pBuffers || iCount <= 0)
		return ERROR_INVALID_PARAMETER;
	if(iCount == 1)
		return Send(dwConnID, (const BYTE*)pBuffers[0].buf, pBuffers[0].len);

	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(!TUdpSocketObj::IsValid(pSocketObj))
	{
		::SetLastError(ERROR_OBJECT_NOT_FOUND);
		return FALSE;
	}

	int iLength	= 0;
	int iMaxLen	= (int)m_arqAttr.dwMaxMessageSize;

	for(int i = 0; i < iCount; i++)
		iLength += pBuffers[i].len;

	if(iLength <= 0 || iLength > iMaxLen)
		return ERROR_INCORRECT_SIZE;

	CBufferPtr sndBuffer(iLength);
	sndBuffer.SetSize(0);

	for(int i = 0; i < iCount; i++)
	{
		int iBufLen = pBuffers[i].len;

		if(iBufLen > 0)
		{
			BYTE* pBuffer = (BYTE*)pBuffers[i].buf;
			ASSERT(pBuffer);

			sndBuffer.Cat(pBuffer, iBufLen);
		}
	}

	int result = SendArq(pSocketObj, sndBuffer.Ptr(), (int)sndBuffer.Size());

	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);
}

int CUdpArqServer::SendArq(TUdpSocketObj* pSocketObj, const BYTE* pBuffer, int iLength)
{
	CArqSessionEx* pSession = nullptr;
	GetConnectionReserved(pSocketObj, (PVOID*)&pSession);

	if(pSession == nullptr)
		return ERROR_OBJECT_NOT_FOUND;

	return pSession->Send(pBuffer, iLength);
}

int CUdpArqServer::ArqOutputProc(const char* pBuffer, int iLength, IKCPCB* kcp, LPVOID pv)
{
	TUdpSocketObj* pSocketObj = (TUdpSocketObj*)pv;

	if(!TUdpSocketObj::IsValid(pSocketObj))
		return ERROR_OBJECT_NOT_FOUND;

	CUdpArqServer* pServer = (CUdpArqServer*)IUdpArqServer::FromS((IUdpServer*)pSocketObj->pHolder);

	TItemPtr itPtr(pServer->m_bfObjPool, pServer->m_bfObjPool.PickFreeItem());
	itPtr->Cat((const BYTE*)pBuffer, iLength);

	return pServer->SendInternal(pSocketObj, itPtr);
}

EnHandleResult CUdpArqServer::FireAccept(TUdpSocketObj* pSocketObj)
{
	EnHandleResult result = DoFireAccept(pSocketObj);

	if(result != HR_ERROR)
	{
		CArqSessionEx* pSession = m_ssPool.PickFreeSession(this, pSocketObj, m_arqAttr);
		ENSURE(SetConnectionReserved(pSocketObj, pSession));
	}

	return result;
}

EnHandleResult CUdpArqServer::FireReceive(TUdpSocketObj* pSocketObj, const BYTE* pData, int iLength)
{
	CArqSessionEx* pSession = nullptr;
	GetConnectionReserved(pSocketObj, (PVOID*)&pSession);

	CBufferPtr& rcBuffer = *m_rcBuffers[SELF_THREAD_ID];
	return pSession->Receive(pData, iLength, rcBuffer.Ptr(), (int)rcBuffer.Size());
}

EnHandleResult CUdpArqServer::FireClose(TUdpSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
{
	EnHandleResult result = DoFireClose(pSocketObj, enOperation, iErrorCode);

	CArqSessionEx* pSession = nullptr;
	GetConnectionReserved(pSocketObj, (PVOID*)&pSession);

	if(pSession != nullptr)
		m_ssPool.PutFreeSession(pSession);

	return result;
}

BOOL CUdpArqServer::GetWaitingSendMessageCount(CONNID dwConnID, int& iCount)
{
	TUdpSocketObj* pSocketObj = FindSocketObj(dwConnID);

	if(!TUdpSocketObj::IsValid(pSocketObj))
	{
		::SetLastError(ERROR_OBJECT_NOT_FOUND);
		return FALSE;
	}

	CArqSessionEx* pSession = nullptr;
	GetConnectionReserved(pSocketObj, (PVOID*)&pSession);

	if(pSession == nullptr)
	{
		::SetLastError(ERROR_OBJECT_NOT_FOUND);
		return FALSE;
	}

	iCount = pSession->GetWaitingSend();

	return (iCount >= 0);
}

#endif
