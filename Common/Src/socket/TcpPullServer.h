/*
 * Copyright Bruce Liang (ldcsaa@gmail.com)
 *
 * Version	: 3.1.1
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * WeiBo	: http://weibo.com/u/1402935851
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

#include "TcpServer.h"
#include "../bufferpool.h"

class CTcpPullServer : public IPullServer, public CTcpServer
{
public:
	virtual EnFetchResult Fetch(CONNID dwConnID, BYTE* pData, int iLength);

protected:
	virtual ISocketListener::EnHandleResult FireAccept(CONNID dwConnID, SOCKET soClient);
	virtual ISocketListener::EnHandleResult FireReceive(CONNID dwConnID, const BYTE* pData, int iLength);
	virtual ISocketListener::EnHandleResult FireClose(CONNID dwConnID);
	virtual ISocketListener::EnHandleResult FireError(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	virtual ISocketListener::EnHandleResult FireServerShutdown();

	virtual BOOL CheckParams()
	{
		m_bfPool.SetItemCapacity	(GetSocketBufferSize());
		m_bfPool.SetItemPoolSize	(GetFreeBufferObjPool());
		m_bfPool.SetItemPoolHold	(GetFreeBufferObjHold());
		m_bfPool.SetBufferLockTime	(GetFreeSocketObjLockTime());
		m_bfPool.SetBufferPoolSize	(GetFreeSocketObjPool());
		m_bfPool.SetBufferPoolHold	(GetFreeSocketObjHold());

		return __super::CheckParams();
	}

public:
	CTcpPullServer(ITcpServerListener* psoListener) : CTcpServer(psoListener)
	{

	}

	virtual ~CTcpPullServer()
	{
		if(HasStarted())
			Stop();
	}

private:
	CBufferPool m_bfPool;
};
