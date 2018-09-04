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
#include "MiscHelper.h"
#include "../Common/Src/BufferPool.h"

template<class T> class CTcpPackAgentT : public IPackSocket, public T
{
public:
	virtual BOOL SendPackets(CONNID dwConnID, const WSABUF pBuffers[], int iCount)
	{
		int iNewCount = iCount + 1;
		unique_ptr<WSABUF[]> buffers(new WSABUF[iNewCount]);

		DWORD header;
		if(!::AddPackHeader(pBuffers, iCount, buffers, m_dwMaxPackSize, m_usHeaderFlag, header))
			return FALSE;

		return __super::SendPackets(dwConnID, buffers.get(), iNewCount);
	}

protected:
	virtual EnHandleResult DoFireHandShake(TSocketObj* pSocketObj)
	{
		EnHandleResult result = __super::DoFireHandShake(pSocketObj);

		if(result != HR_ERROR)
		{
			TBuffer* pBuffer = m_bfPool.PickFreeBuffer(pSocketObj->connID);
			ENSURE(SetConnectionReserved(pSocketObj, TBufferPackInfo::Construct(pBuffer)));
		}

		return result;
	}

	virtual EnHandleResult DoFireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
	{
		TBufferPackInfo* pInfo = nullptr;
		GetConnectionReserved(pSocketObj, (PVOID*)&pInfo);
		ASSERT(pInfo);

		TBuffer* pBuffer = (TBuffer*)pInfo->pBuffer;
		ASSERT(pBuffer && pBuffer->IsValid());

		return ParsePack(this, pInfo, pBuffer, pSocketObj, m_dwMaxPackSize, m_usHeaderFlag, pData, iLength);
	}

	virtual EnHandleResult DoFireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
	{
		EnHandleResult result = __super::DoFireClose(pSocketObj, enOperation, iErrorCode);

		TBufferPackInfo* pInfo = nullptr;
		GetConnectionReserved(pSocketObj, (PVOID*)&pInfo);

		if(pInfo != nullptr)
		{
			m_bfPool.PutFreeBuffer(pInfo->pBuffer);
			TBufferPackInfo::Destruct(pInfo);
		}

		return result;
	}

	virtual EnHandleResult DoFireShutdown()
	{
		EnHandleResult result = __super::DoFireShutdown();

		m_bfPool.Clear();

		return result;
	}

	virtual BOOL CheckParams()
	{
		if	((m_dwMaxPackSize > 0 && m_dwMaxPackSize <= TCP_PACK_MAX_SIZE_LIMIT)	&&
			(m_usHeaderFlag >= 0 && m_usHeaderFlag <= TCP_PACK_HEADER_FLAG_LIMIT)	)
			return __super::CheckParams();

		SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
		return FALSE;
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
	virtual void SetMaxPackSize		(DWORD dwMaxPackSize)		{m_dwMaxPackSize = dwMaxPackSize;}
	virtual void SetPackHeaderFlag	(USHORT usPackHeaderFlag)	{m_usHeaderFlag  = usPackHeaderFlag;}
	virtual DWORD GetMaxPackSize	()							{return m_dwMaxPackSize;}
	virtual USHORT GetPackHeaderFlag()							{return m_usHeaderFlag;}

private:
	EnHandleResult DoFireSuperReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return __super::DoFireReceive(pSocketObj, pData, iLength);}

	friend EnHandleResult ParsePack<>	(CTcpPackAgentT* pThis, TBufferPackInfo* pInfo, TBuffer* pBuffer, TSocketObj* pSocket,
										DWORD dwMaxPackSize, USHORT usPackHeaderFlag, const BYTE* pData, int iLength);

public:
	CTcpPackAgentT(ITcpAgentListener* pListener)
	: T					(pListener)
	, m_dwMaxPackSize	(TCP_PACK_DEFAULT_MAX_SIZE)
	, m_usHeaderFlag	(TCP_PACK_DEFAULT_HEADER_FLAG)
	{

	}

	virtual ~CTcpPackAgentT()
	{
		Stop();
	}

private:
	DWORD	m_dwMaxPackSize;
	USHORT	m_usHeaderFlag;

	CBufferPool m_bfPool;
};

typedef CTcpPackAgentT<CTcpAgent> CTcpPackAgent;

#ifdef _SSL_SUPPORT

#include "SSLAgent.h"
typedef CTcpPackAgentT<CSSLAgent> CSSLPackAgent;

#endif
