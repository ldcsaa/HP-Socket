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
 
#include "UdpArqClient.h"

#ifdef _UDP_SUPPORT

BOOL CUdpArqClient::CheckParams()
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

void CUdpArqClient::PrepareStart()
{
	__super::PrepareStart();
}

void CUdpArqClient::Reset()
{
	m_arqSession.Reset();

	__super::Reset();
}

void CUdpArqClient::OnWorkerThreadStart(THR_ID dwThreadID)
{
	m_arqBuffer.Malloc(m_arqAttr.dwMaxMessageSize);
	m_arqTimer = ::CreateTimer(m_arqAttr.dwFlushInterval);
}

void CUdpArqClient::OnWorkerThreadEnd(THR_ID dwThreadID)
{
	if(IS_VALID_FD(m_arqTimer))
	{
		close(m_arqTimer);
		m_arqTimer = INVALID_FD;
	}

	m_arqBuffer.Free();
}

HANDLE CUdpArqClient::GetUserEvent()
{
	return m_arqTimer;
}

BOOL CUdpArqClient::OnUserEvent()
{
	::ReadTimer(m_arqTimer);

	return m_arqSession.Check();
}

BOOL CUdpArqClient::Send(const BYTE* pBuffer, int iLength, int iOffset)
{
	ASSERT(pBuffer && iLength > 0 && iLength <= (int)m_arqAttr.dwMaxMessageSize);

	int result = NO_ERROR;

	if(pBuffer && iLength > 0 && iLength <= (int)m_arqAttr.dwMaxMessageSize)
	{
		if(IsConnected())
		{
			if(iOffset != 0) pBuffer += iOffset;
			result = m_arqSession.Send(pBuffer, iLength);
		}
		else
			result = ERROR_INVALID_STATE;
	}
	else
		result = ERROR_INVALID_PARAMETER;

	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);
}

BOOL CUdpArqClient::SendPackets(const WSABUF pBuffers[], int iCount)
{
	ASSERT(pBuffers && iCount > 0);

	if(!pBuffers || iCount <= 0)
		return ERROR_INVALID_PARAMETER;
	if(iCount == 1)
		return Send((const BYTE*)pBuffers[0].buf, pBuffers[0].len);
	if(!IsConnected())
		return ERROR_INVALID_STATE;

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

	int result = m_arqSession.Send(sndBuffer.Ptr(), (int)sndBuffer.Size());

	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);
}

int CUdpArqClient::ArqOutputProc(const char* pBuffer, int iLength, IKCPCB* kcp, LPVOID pv)
{
	CUdpArqClient* pClient = (CUdpArqClient*)pv;

	BOOL isOK = pClient->__super::Send((const BYTE*)pBuffer, iLength);

	return isOK ? NO_ERROR : ::WSAGetLastError();
}

EnHandleResult CUdpArqClient::FireConnect()
{
	EnHandleResult result = DoFireConnect(this);

	if(result != HR_ERROR)
		m_arqSession.Renew(this, this, m_arqAttr);

	return result;
}

EnHandleResult CUdpArqClient::FireReceive(const BYTE* pData, int iLength)
{
	return m_arqSession.Receive(pData, iLength, m_arqBuffer.Ptr(), (int)m_arqBuffer.Size());
}

BOOL CUdpArqClient::GetWaitingSendMessageCount(int& iCount)
{
	iCount = m_arqSession.GetWaitingSend();

	return (iCount >= 0);
}

#endif
