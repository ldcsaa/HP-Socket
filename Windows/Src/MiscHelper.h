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

#include "SocketHelper.h"

/* Pack Data Info */
template<typename B = void> struct TPackInfo
{
	bool	header;
	DWORD	length;
	B*		pBuffer;

	static TPackInfo* Construct(B* pbuf = nullptr, bool head = true, DWORD len = sizeof(DWORD))
	{
		return new TPackInfo(pbuf, head, len);
	}

	static void Destruct(TPackInfo* pPackInfo)
	{
		if(pPackInfo)
			delete pPackInfo;
	}

	TPackInfo(B* pbuf = nullptr, bool head = true, DWORD len = sizeof(DWORD))
	: header(head), length(len), pBuffer(pbuf)
	{
	}

	void Reset()
	{
		header	= true;
		length	= sizeof(DWORD);
		pBuffer	= nullptr;
	}
};

typedef TPackInfo<TBuffer>	TBufferPackInfo;

BOOL AddPackHeader(const WSABUF * pBuffers, int iCount, unique_ptr<WSABUF[]>& buffers, DWORD dwMaxPackSize, USHORT usPackHeaderFlag, DWORD& header);

template<class B> EnFetchResult FetchBuffer(B* pBuffer, BYTE* pData, int iLength)
{
	ASSERT(pBuffer	!= nullptr);
	ASSERT(pData	!= nullptr && iLength > 0);

	EnFetchResult result = FR_OK;

	if(pBuffer->Length() >= iLength)
		pBuffer->Fetch(pData, iLength);
	else
		result = FR_LENGTH_TOO_LONG;

	return result;
}

template<class B> EnFetchResult PeekBuffer(B* pBuffer, BYTE* pData, int iLength)
{
	ASSERT(pBuffer	!= nullptr);
	ASSERT(pData	!= nullptr && iLength > 0);

	EnFetchResult result = FR_OK;

	if(pBuffer->Length() >= iLength)
		pBuffer->Peek(pData, iLength);
	else
		result = FR_LENGTH_TOO_LONG;

	return result;
}

template<class T, class B, class S> EnHandleResult ParsePack(T* pThis, TPackInfo<B>* pInfo, B* pBuffer, S* pSocket, DWORD dwMaxPackSize, USHORT usPackHeaderFlag, const BYTE* pData, int iLength)
{
	EnHandleResult rs = HR_OK;

	pBuffer->Cat(pData, iLength);
	iLength = pBuffer->Length();

	int required = pInfo->length;
	int remain	 = iLength;

	while(remain >= required)
	{
		remain -= required;
		CBufferPtr buffer(required);

		pBuffer->Fetch(buffer, (int)buffer.Size());

		if(pInfo->header)
		{
			DWORD header = *((DWORD*)(byte*)buffer);

			if(usPackHeaderFlag != 0)
			{
				USHORT flag = (USHORT)(header >> TCP_PACK_LENGTH_BITS);

				if(flag != usPackHeaderFlag)
				{
					::SetLastError(ERROR_INVALID_DATA);
					return HR_ERROR;
				}
			}

			DWORD len = header & TCP_PACK_LENGTH_MASK;

			if(len == 0 || len > dwMaxPackSize)
			{
				::SetLastError(ERROR_BAD_LENGTH);
				return HR_ERROR;
			}

			required = len;
		}
		else
		{
			rs = pThis->DoFireSuperReceive(pSocket, (const BYTE*)buffer, (int)buffer.Size());

			if(rs == HR_ERROR)
				return rs;

			required = sizeof(DWORD);
		}

		pInfo->header = !pInfo->header;
		pInfo->length = required;
	}

	return rs;
}
