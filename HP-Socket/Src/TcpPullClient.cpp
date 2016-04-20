/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.4.3
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
#include "TcpPullClient.h"
#include "MiscHelper.h"

EnHandleResult CTcpPullClient::FireReceive(IClient* pClient, const BYTE* pData, int iLength)
{
	m_lsBuffer.Cat(pData, iLength);

	return __super::FireReceive(pClient, m_lsBuffer.Length());
}

EnFetchResult CTcpPullClient::Fetch(BYTE* pData, int iLength)
{
	return ::FetchBuffer(&m_lsBuffer, pData, iLength);
}

EnFetchResult CTcpPullClient::Peek(BYTE* pData, int iLength)
{
	return ::PeekBuffer(&m_lsBuffer, pData, iLength);
}

void CTcpPullClient::Reset(BOOL bAll)
{
	m_lsBuffer.Clear();

	return __super::Reset(bAll);
}
