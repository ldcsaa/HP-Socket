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

#include "SysHelper.h"

DWORD GetDefaultBufferSize()
{
	static const DWORD s_dtsbs = (DWORD)SysGetPageSize();
	return s_dtsbs;
}

DWORD GetDefaultWorkerThreadCount()
{
	static const DWORD s_dwtc = MIN((PROCESSOR_COUNT * 2 + 2), MAX_WORKER_THREAD_COUNT);
	return s_dwtc;
}


#if defined(__ANDROID__)

#if defined(__ANDROID_API__)
#if (__ANDROID_API__ < 21)

#include <limits.h>
#include <string.h>

int sigaddset(sigset_t *set, int signum)
{
	signum--;
	unsigned long *local_set = (unsigned long *)set;

	local_set[signum/LONG_BIT] |= 1UL << (signum%LONG_BIT);

	return 0;
}

int sigdelset(sigset_t *set, int signum)
{
	signum--;
	unsigned long *local_set = (unsigned long *)set;

	local_set[signum/LONG_BIT] &= ~(1UL << (signum%LONG_BIT));

	return 0;
}

int sigemptyset(sigset_t *set)
{
	memset(set, 0, sizeof *set);
	return 0;
}

int sigfillset(sigset_t *set)
{
	memset(set, ~0, sizeof *set);
	return 0;
}

int sigismember(sigset_t *set, int signum)
{
	signum--;
	unsigned long *local_set = (unsigned long *)set;

	return (int)((local_set[signum/LONG_BIT] >> (signum%LONG_BIT)) & 1);
}

#endif
#endif

#endif
