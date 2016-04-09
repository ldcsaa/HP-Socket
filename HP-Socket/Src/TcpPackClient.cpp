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
#include "TcpPackClient.h"

BOOL CTcpPackClient::CheckParams()
{
	if(m_dwMaxPackSize > 0 && m_dwMaxPackSize <= TCP_PACK_MAX_SIZE_LIMIT)
		if(m_usHeaderFlag >= 0 && m_usHeaderFlag <= TCP_PACK_HEADER_FLAG_LIMIT)
			return __super::CheckParams();

	SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
	return FALSE;
}

BOOL CTcpPackClient::SendPackets(const WSABUF pBuffers[], int iCount)
{
	int iNewCount = iCount + 1;
	unique_ptr<WSABUF[]> buffers(new WSABUF[iNewCount]);

	DWORD header;
	if(!::AddPackHeader(pBuffers, iCount, buffers, m_dwMaxPackSize, m_usHeaderFlag, header))
		return FALSE;

	return __super::SendPackets(buffers.get(), iNewCount);
}

EnHandleResult CTcpPackClient::FireReceive(IClient* pClient, const BYTE* pData, int iLength)
{
	return ParsePack(this, &m_pkInfo, &m_lsBuffer, pClient, m_dwMaxPackSize, m_usHeaderFlag, pData, iLength);
}

void CTcpPackClient::Reset(BOOL bAll)
{
	m_lsBuffer.Clear();
	m_pkInfo.Reset();

	return __super::Reset(bAll);
}
