/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 5.0.2
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
#include "MiscHelper.h"
#include "../../Common/Src/bufferpool.h"

template<class T> class CTcpPullClientT : public IPullClient, public T
{
public:
	virtual EnFetchResult Fetch(BYTE* pData, int iLength)
	{
		return ::FetchBuffer(&m_lsBuffer, pData, iLength);
	}

	virtual EnFetchResult Peek(BYTE* pData, int iLength)
	{
		return ::PeekBuffer(&m_lsBuffer, pData, iLength);
	}

protected:
	virtual EnHandleResult DoFireReceive(ITcpClient* pSender, const BYTE* pData, int iLength)
	{
		m_lsBuffer.Cat(pData, iLength);

		return __super::DoFireReceive(pSender, m_lsBuffer.Length());
	}

	virtual void Reset()
	{
		m_lsBuffer.Clear();

		__super::Reset();
	}

public:
	CTcpPullClientT(ITcpClientListener* pListener)
	: T			(pListener)
	, m_lsBuffer(m_itPool)
	{

	}

	virtual ~CTcpPullClientT()
	{
		Stop();
	}

private:
	TItemListEx	m_lsBuffer;
};

typedef CTcpPullClientT<CTcpClient> CTcpPullClient;

#ifdef _SSL_SUPPORT

#include "SSLClient.h"
typedef CTcpPullClientT<CSSLClient> CSSLPullClient;

#endif
