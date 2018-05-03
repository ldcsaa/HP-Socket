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

#include "GlobalDef.h"

#include <poll.h>
#include <signal.h>

#if __USE_GNU
	#define _POLL_READ_PRI_EVENTS	(POLLPRI | POLLRDHUP)
	#define _POLL_READ_EVENTS		(POLLIN | POLLRDHUP)
	#define _POLL_HUNGUP_EVENTS		(POLLHUP | POLLRDHUP)
#else
	#define _POLL_READ_PRI_EVENTS	(POLLPRI)
	#define _POLL_READ_EVENTS		(POLLIN)
	#define _POLL_HUNGUP_EVENTS		(POLLHUP)
#endif

#define _POLL_ALL_READ_EVENTS		(_POLL_READ_EVENTS | _POLL_READ_PRI_EVENTS)
#define _POLL_WRITE_EVENTS			(POLLOUT)
#define _POLL_NORMAL_RW_EVENTS		(_POLL_READ_EVENTS | _POLL_WRITE_EVENTS)
#define _POLL_ALL_RW_EVENTS			(_POLL_ALL_READ_EVENTS | _POLL_WRITE_EVENTS)
#define _POLL_ERROR_EVENTS			(POLLERR | POLLNVAL)
#define _POLL_ALL_ERROR_EVENTS		(_POLL_ERROR_EVENTS | _POLL_HUNGUP_EVENTS)
#define _POLL_ALL_NORMAL_EVENTS		(_POLL_NORMAL_RW_EVENTS | _POLL_ALL_ERROR_EVENTS)
#define _POLL_ALL_EVENTS			(_POLL_ALL_RW_EVENTS | _POLL_ALL_ERROR_EVENTS)

long PollForSingleObject(pollfd& pfd, long lTimeout = INFINITE, const sigset_t* pSigSet = nullptr);
long PollForMultipleObjects(pollfd pfds[], int iCount, long lTimeout = INFINITE, const sigset_t* pSigSet = nullptr);
