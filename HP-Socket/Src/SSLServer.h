/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.5.2
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

#include "TcpServer.h"
#include "SocketInterface-SSL.h"
#include "SSLHelper.h"

class CSSLServer : public CTcpServer
{
public:
	virtual BOOL SendPackets(CONNID dwConnID, const WSABUF pBuffers[], int iCount);

protected:
	virtual EnHandleResult FireAccept(TSocketObj* pSocketObj);
	virtual EnHandleResult FireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength);
	virtual EnHandleResult FireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult FireShutdown();

	virtual BOOL CheckParams();
	virtual void PrepareStart();
	virtual void Reset(BOOL bAll = TRUE);

	virtual void OnWorkerThreadEnd(DWORD dwThreadID);

private:
	friend EnHandleResult ProcessHandShake<>(CSSLServer* pThis, TSocketObj* pSocketObj, CSSLSession* pSession);
	friend EnHandleResult ProcessReceive<>(CSSLServer* pThis, TSocketObj* pSocketObj, CSSLSession* pSession, const BYTE* pData, int iLength);
	friend BOOL ProcessSend<>(CSSLServer* pThis, TSocketObj* pSocketObj, CSSLSession* pSession, const WSABUF * pBuffers, int iCount);

public:
	CSSLServer(ITcpServerListener* psoListener)
	: CTcpServer(psoListener)
	{

	}

	virtual ~CSSLServer()
	{
		if(HasStarted())
			Stop();
	}

private:
	CSSLSessionPool m_sslPool;
};
