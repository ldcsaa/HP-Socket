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

#include "SysHelper.h"

#include <stdio.h>
#include <sys/utsname.h>

DWORD _GetKernelVersion()
{
	utsname uts;
	
	if(uname(&uts) == RS_FAIL)
		return 0;

	char c;
	int major, minor, revise;

	if(sscanf(uts.release, "%d.%d.%d%c", &major, &minor, &revise, &c) < 3)
		return 0;

	return (DWORD)((major << 16) | (minor << 8) | revise);
}

DWORD GetSysPageSize()
{
	static const DWORD _s_page_size = (DWORD)SysGetPageSize();
	return _s_page_size;
}

DWORD GetKernelVersion()
{
	static const DWORD _s_kernel_version = _GetKernelVersion();
	return _s_kernel_version;
}

BOOL IsKernelVersionAbove(BYTE major, BYTE minor, BYTE revise)
{
	return GetKernelVersion() >= (DWORD)((major << 16) | (minor << 8) | revise);
}

DWORD GetDefaultWorkerThreadCount()
{
	static const DWORD _s_dwtc = MIN((PROCESSOR_COUNT * 2 + 2), MAX_WORKER_THREAD_COUNT);
	return _s_dwtc;
}
