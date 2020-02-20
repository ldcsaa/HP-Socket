/*
* Copyright: JessMA Open Source (ldcsaa@gmail.com)
*
* Author	: Bruce Liang
* Website	: https://github.com/ldcsaa
* Project	: https://github.com/ldcsaa/HP-Socket
* Blog		: http://www.cnblogs.com/ldcsaa
* Wiki		: http://www.oschina.net/p/hp-socket
* QQ Group	: 44636872, 75375912
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

#include "PollHelper.h"
#include "FuncHelper.h"

long PollForSingleObject(pollfd& pfd, long lTimeout, const sigset_t* pSigSet)
{
	return PollForMultipleObjects(&pfd, 1, lTimeout, pSigSet);
}

long PollForMultipleObjects(pollfd pfds[], int iCount, long lTimeout, const sigset_t* pSigSet)
{
	ASSERT(iCount > 0 && iCount < (int)(sizeof(LONG) * 8));

	timespec* pts = nullptr;

	if(!IS_INFINITE(lTimeout))
	{
		pts = CreateLocalObject(timespec);
		::MillisecondToTimespec(lTimeout, *pts);
	}

	while(TRUE)
	{
		int rs = NO_EINTR_INT(ppoll(pfds, iCount, pts, pSigSet));

		if(rs <= TIMEOUT) return rs;

		LONG lValue = 0L;

		for(int i = 0; i < iCount; i++)
		{
			pollfd& pfd	= pfds[i];
			
			if(pfd.revents & _POLL_ALL_EVENTS)
				lValue |= (1 << i);
		}

		return lValue;
	}
}
