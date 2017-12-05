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

#include "TcpClient.h"
#include "SSLHelper.h"

class CSSLClient : public CTcpClient
{
public:
	virtual BOOL IsSecure() {return TRUE;}
	virtual BOOL SendPackets(const WSABUF pBuffers[], int iCount);

	virtual BOOL SetupSSLContext(int iVerifyMode = SSL_VM_NONE, LPCTSTR lpszPemCertFile = nullptr, LPCTSTR lpszPemKeyFile = nullptr, LPCTSTR lpszKeyPasswod = nullptr, LPCTSTR lpszCAPemCertFileOrPath = nullptr)
		{return m_sslCtx.Initialize(SSL_SM_CLIENT, iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath, nullptr);}
	virtual void CleanupSSLContext()
		{m_sslCtx.Cleanup();}

protected:
	virtual EnHandleResult FireConnect();
	virtual EnHandleResult FireReceive(const BYTE* pData, int iLength);

	virtual BOOL CheckParams();
	virtual void PrepareStart();
	virtual void Reset();

	virtual void OnWorkerThreadEnd(DWORD dwThreadID);

private:

	friend EnHandleResult ProcessHandShake<>(CSSLClient* pThis, CSSLClient* pSocketObj, CSSLSession* pSession);
	friend EnHandleResult ProcessReceive<>(CSSLClient* pThis, CSSLClient* pSocketObj, CSSLSession* pSession, const BYTE* pData, int iLength);
	friend BOOL ProcessSend<>(CSSLClient* pThis, CSSLClient* pSocketObj, CSSLSession* pSession, const WSABUF * pBuffers, int iCount);

public:
	CSSLClient(ITcpClientListener* pListener)
	: CTcpClient(pListener)
	, m_sslSession		(m_itPool)
	, m_dwMainThreadID	(0)
	{

	}

	virtual ~CSSLClient()
	{
		Stop();
	}

private:
	CSSLContext m_sslCtx;
	CSSLSession m_sslSession;
	DWORD		m_dwMainThreadID;
};
