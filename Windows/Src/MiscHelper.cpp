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
 
#include "stdafx.h"
#include "MiscHelper.h"

BOOL AddPackHeader(const WSABUF * pBuffers, int iCount, unique_ptr<WSABUF[]>& buffers, DWORD dwMaxPackSize, USHORT usPackHeaderFlag, DWORD& header)
{
	ASSERT(pBuffers && iCount > 0);

	DWORD iLength = 0;

	for(int i = 0; i < iCount; i++)
	{
		const WSABUF& buf	= pBuffers[i];
		buffers[i + 1]		= buf;
		iLength			   += buf.len;
	}

	if(iLength == 0 || iLength > dwMaxPackSize)
	{
		::SetLastError(ERROR_BAD_LENGTH);
		return FALSE;
	}

	header = (usPackHeaderFlag << TCP_PACK_LENGTH_BITS) | iLength;

	buffers[0].len = sizeof(header);
	buffers[0].buf = (char*)&header;

	return TRUE;
}
