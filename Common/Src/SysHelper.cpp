/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.7
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
#include "SysHelper.h"
#include "GeneralHelper.h"

VOID SysGetSystemInfo(LPSYSTEM_INFO pInfo)
{
	ASSERT(pInfo != nullptr);
	::GetNativeSystemInfo(pInfo);
}

DWORD SysGetNumberOfProcessors()
{
	SYSTEM_INFO si;
	SysGetSystemInfo(&si);
	
	return si.dwNumberOfProcessors;
}

DWORD SysGetPageSize()
{
	SYSTEM_INFO si;
	SysGetSystemInfo(&si);

	return si.dwPageSize;
}

#if _MSC_VER < 1800

BOOL SysGetOSVersionInfo(LPOSVERSIONINFO pInfo, BOOL bInfoEx)
{
	ASSERT(pInfo != nullptr);

	pInfo->dwOSVersionInfoSize = bInfoEx ? sizeof(LPOSVERSIONINFOEX) : sizeof(LPOSVERSIONINFO);
	return ::GetVersionEx(pInfo);
}

DWORD SysGetOSVersion()
{
	OSVERSIONINFO vi;
	DWORD dwOSVersion = 0;

	if(SysGetOSVersionInfo(&vi))
		dwOSVersion = (vi.dwMajorVersion << 16) + vi.dwMinorVersion;

	return dwOSVersion;
}

#endif
