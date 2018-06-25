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
#include "SysHelper.h"
#include "FuncHelper.h"

void __EXIT_FN_(void (*fn)(int), LPCSTR lpszFnName, int* lpiExitCode, int iErrno, LPCSTR lpszFile, int iLine, LPCSTR lpszFunc, LPCSTR lpszTitle)
{
	if(iErrno >= 0)
		SetLastError(iErrno);
	else
		iErrno = GetLastError();

	if(!lpszTitle)
	{
		lpszTitle = CreateLocalObjects(char, 50);

		if(lpiExitCode)
			sprintf((LPSTR)lpszTitle, "(#%d, %u) > %s(%d) [%d]", SELF_PROCESS_ID, SELF_THREAD_ID, lpszFnName, *lpiExitCode, iErrno);
		else
			sprintf((LPSTR)lpszTitle, "(#%d, %u) > %s() [%d]", SELF_PROCESS_ID, SELF_THREAD_ID, lpszFnName, iErrno);
	}

	if(lpszFile && iLine > 0)
		FPRINTLN(stderr, "%s : %s\n    -> %s (%d) : %s", lpszTitle, strerror(iErrno), lpszFile, iLine, lpszFunc ? lpszFunc : "");
	else
		FPRINTLN(stderr, "%s : %s", lpszTitle, strerror(iErrno));

	if(lpiExitCode)
		fn(*lpiExitCode);
	else
		((void (*)())fn)();
}

void EXIT(int iExitCode, int iErrno, LPCSTR lpszFile, int iLine, LPCSTR lpszFunc, LPCSTR lpszTitle)
{
	__EXIT_FN_(exit, "exit", &iExitCode, iErrno, lpszFile, iLine, lpszFunc, lpszTitle);
}

void _EXIT(int iExitCode, int iErrno, LPCSTR lpszFile, int iLine, LPCSTR lpszFunc, LPCSTR lpszTitle)
{
	__EXIT_FN_(_exit, "_exit", &iExitCode, iErrno, lpszFile, iLine, lpszFunc, lpszTitle);
}

void ABORT(int iErrno, LPCSTR lpszFile, int iLine, LPCSTR lpszFunc, LPCSTR lpszTitle)
{
	__EXIT_FN_((void (*)(int))abort, "abort", nullptr, iErrno, lpszFile, iLine, lpszFunc, lpszTitle);
}
