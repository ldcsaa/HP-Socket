/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.4.4
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
 
#pragma once

#include "TcpAgent.h"
#include "MiscHelper.h"
#include "../../Common/Src/bufferpool.h"

class CTcpPackAgent : public IPackSocket, public CTcpAgent
{
public:
	virtual BOOL SendPackets(CONNID dwConnID, const WSABUF pBuffers[], int iCount);

protected:
	virtual EnHandleResult FireConnect(TSocketObj* pSocketObj);
	virtual EnHandleResult FireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength);
	virtual EnHandleResult FireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult FireShutdown();

	virtual BOOL CheckParams();
	virtual void PrepareStart();

public:
	virtual void SetMaxPackSize		(DWORD dwMaxPackSize)		{m_dwMaxPackSize = dwMaxPackSize;}
	virtual void SetPackHeaderFlag	(USHORT usPackHeaderFlag)	{m_usHeaderFlag  = usPackHeaderFlag;}
	virtual DWORD GetMaxPackSize	()	{return m_dwMaxPackSize;}
	virtual USHORT GetPackHeaderFlag()	{return m_usHeaderFlag;}

private:
	EnHandleResult FireSuperReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return __super::FireReceive(pSocketObj, pData, iLength);}

	friend EnHandleResult ParsePack<>	(CTcpPackAgent* pThis, TBufferPackInfo* pInfo, TBuffer* pBuffer, TSocketObj* pSocket,
										DWORD dwMaxPackSize, USHORT usPackHeaderFlag, const BYTE* pData, int iLength);

public:
	CTcpPackAgent(ITcpAgentListener* psoListener) : CTcpAgent(psoListener)
	, m_dwMaxPackSize	(TCP_PACK_DEFAULT_MAX_SIZE)
	, m_usHeaderFlag	(TCP_PACK_DEFAULT_HEADER_FLAG)
	{

	}

	virtual ~CTcpPackAgent()
	{
		if(HasStarted())
			Stop();
	}

private:
	DWORD	m_dwMaxPackSize;
	USHORT	m_usHeaderFlag;

	CBufferPool m_bfPool;
};
