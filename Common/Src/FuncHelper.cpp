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
#include "FuncHelper.h"
#include "WaitFor.h"

#ifndef _WIN32_WCE
	#define CloseToolhelp32Snapshot(h)	CloseHandle(h)

	#include <psapi.h>
	#include <Shellapi.h>

	#pragma comment(lib, "Psapi")
#else
	#pragma comment(lib, "Toolhelp")
#endif

BYTE DoubleCharToByte(LPCTSTR psValue)
{ 
	return (BYTE)DOUBLECHARTOVALUE(psValue);
}

LPTSTR ByteToDoubleChar(BYTE b, LPTSTR des)
{
	VALUETODOUBLECHAR(des, b);
	return des;
}

UINT HexStrToInt(LPCTSTR pHexText, int len)
{
	LPTSTR pTemp	= (LPTSTR)pHexText;
	int Val			= 0;
	int iLen		= lstrlen(pHexText);
	if(len > 0 && len < iLen)
		iLen = len;
	if(iLen % 2)
	{
		pTemp = (TCHAR*)_alloca(sizeof(TCHAR) * ((++iLen) + 1));
		lstrcpyn(pTemp+1, pHexText, iLen);
		pTemp[0] = '0';
	}

	for(int i = 0; i < iLen; i+=2)
		Val += (DOUBLECHARTOVALUE(&pTemp[i]) << ((iLen - i)/2 - 1) * 8);

	return Val;
}

UINT DecStrToInt(LPCTSTR pDecText, int len)
{
	int Val = 0;
	int iLen = lstrlen(pDecText);
	if(len > 0 && len < iLen)
	{
		++len;
		CString text;
		LPTSTR ptext = text.GetBuffer(len);
		lstrcpyn(ptext, pDecText, len);
		Val = _ttol(ptext);
		text.ReleaseBuffer();
	}
	else
		Val = _ttol(pDecText);
	return Val;
}

CString& IntToHexStr(CString& dest, UINT v, int len)
{
	if(len > 0)
	{
		CString format;
		format.Format(_T("%%0%uX"), len);
		dest.Format(format, v);
		if(dest.GetLength() > len)
			dest = dest.Right(len);
	}
	else
		dest.Format(_T("%X"), v);
	return dest;
}

CString& IntToDecStr(CString& dest, UINT v, int len)
{
	if(len > 0)
	{
		CString format;
		format.Format(_T("%%0%uu"), len);
		dest.Format(format, v);
		if(dest.GetLength() > len)
			dest = dest.Right(len);
	}
	else
		dest.Format(_T("%u"), v);
	return dest;
}

CString& HexAddrToDecAddr(CString& dest, LPCTSTR src, int destlen, int srclen)
{
	return IntToDecStr(dest, HexStrToInt(src, srclen), destlen);
}
CString& DecAddrToHexAddr(CString& dest, LPCTSTR src, int destlen, int srclen)
{
	return IntToHexStr(dest, DecStrToInt(src, srclen), destlen);
}


//-----------------------------MultiByte字符和Unicode字符之间的转换-----------------------------//

EnCodePage GetCodePageByName(LPCTSTR lpszCodePageName)
{
	if(!lpszCodePageName || !*lpszCodePageName)
		return XCP_ACP;
	else if(_tcsicmp(lpszCodePageName, _T("GB2312")) == 0)
		return XCP_GB2312;
	else if(_tcsicmp(lpszCodePageName, _T("GBK")) == 0)
		return XCP_GBK;
	else if(_tcsicmp(lpszCodePageName, _T("UTF-8")) == 0)
		return XCP_UTF8;
	else if(_tcsicmp(lpszCodePageName, _T("UTF-7")) == 0)
		return XCP_UTF7;
	else if(_tcsicmp(lpszCodePageName, _T("BIG5")) == 0)
		return XCP_BIG5;

	return XCP_ACP;
}

BOOL MbcsToUnicode(const char* pszInString, WCHAR** ptrOutWStr, int& nSizeCount)
{
	return CPToUni(pszInString, ptrOutWStr, CP_ACP, nSizeCount);
}

BOOL UnicodeToMbcs(const WCHAR* pwzInString, char** ptrOutStr, int& nSizeCount)
{
	return UniToCP(pwzInString, ptrOutStr, CP_ACP, nSizeCount);
}

BOOL Utf8ToUnicode(const char* pszInString, WCHAR** ptrOutWStr, int& nSizeCount)
{
	return CPToUni(pszInString, ptrOutWStr, CP_UTF8, nSizeCount);
}

BOOL UnicodeToUtf8(const WCHAR* pwzInString, char** ptrOutStr, int& nSizeCount)
{
	return UniToCP(pwzInString, ptrOutStr, CP_UTF8, nSizeCount);
}

BOOL CPToUni(const char* pszInString, WCHAR** ptrOutWStr, unsigned int nCodePage, int& nSizeCount)
{
	nSizeCount = 0 ;
	if( pszInString == nullptr || ptrOutWStr == nullptr )
		return FALSE ;

	nSizeCount = MultiByteToWideChar( nCodePage, 0, pszInString, -1, nullptr, 0 ) ;
	if( 0 == nSizeCount )
		return FALSE ;

	(*ptrOutWStr) = new WCHAR[nSizeCount] ;
	if( nullptr == (*ptrOutWStr) )
		return FALSE ;

	if( 0 == MultiByteToWideChar( nCodePage, 0, pszInString, -1, (*ptrOutWStr), nSizeCount ) )
	{
		delete[] (*ptrOutWStr);
		return FALSE ;
	}

	return TRUE;
}

BOOL UniToCP(const WCHAR* pwzInString, char** ptrOutStr, unsigned int nCodePage, int& nSizeCount)
{
	nSizeCount = 0 ;
	if( pwzInString == nullptr || ptrOutStr == nullptr )
		return FALSE ;

	nSizeCount = WideCharToMultiByte( nCodePage, 0, pwzInString, -1, nullptr, 0, nullptr, nullptr) ;
	if( 0 == nSizeCount )
		return FALSE ;

	(*ptrOutStr) = new char[nSizeCount] ;
	if( nullptr == (*ptrOutStr) )
		return FALSE ;

	if(0 == WideCharToMultiByte( nCodePage, 0, pwzInString, -1, (*ptrOutStr), nSizeCount, nullptr, nullptr))
	{
		delete[] (*ptrOutStr);
		return FALSE ;
	}

	return TRUE;
}

int BytesToHex(const BYTE* pBytes, int nLength, LPTSTR* lpszDest)
{
	int des_length	= nLength * 2;

	if(des_length > 0)
	{
		LPTSTR dest = new TCHAR[des_length + 1];
		dest[des_length] = '\0';

		TCHAR chs[3] = {0};
		for(int i = 0; i < nLength; i++)
		{
			ByteToDoubleChar(pBytes[i], chs);
			dest[2 * i] = chs[0];
			dest[2 * i + 1] = chs[1];
		}

		*lpszDest = dest;
	}
	else
		*lpszDest = nullptr;

	return des_length;
}

int HexToBytes(LPCTSTR lpszHex, BYTE** ppBytes, int* pnLength)
{
	int src_length = lstrlen(lpszHex);
	int des_length = src_length / 2;

	*pnLength = des_length;

	if(des_length > 0)
	{
		BYTE* pBytes = new BYTE[des_length];

		for(int i = 0; i < des_length; i++)
			pBytes[i] = DoubleCharToByte(&lpszHex[2 * i]);

		*ppBytes = pBytes;
	}
	else
		*ppBytes = nullptr;

	return des_length;
}

CString& StrToHex(const TCHAR* src, CString& strDec)
{
	BYTE* t = (BYTE*)src;
	int src_length = lstrlen(src) * sizeof(TCHAR);
	strDec.Empty();

#ifdef UNICODE
	char* temp = nullptr;
	UnicodeToMbcs(src, &temp, src_length);

	src_length -= 1;
	t = (BYTE*)temp;
#else
	t = (BYTE*)src;
	src_length = lstrlen(src);
#endif

	for(int i = 0; i < src_length; ++i, ++t)
	{
		TCHAR tc[3] = {0, 0, 0};
		VALUETODOUBLECHAR(tc, *t);
		strDec += tc;
	}

#ifdef UNICODE
	delete[] temp;
#endif

	return strDec;
}

CString& HexToStr(const TCHAR* src, CString& strDec)
{
	char* temp		= nullptr;
	int src_length	= 0;

	strDec.Empty();

#ifdef UNICODE
	char* temp1 = new char[(src_length = lstrlen(src)) +1];
	temp = temp1;
	while(*(temp1++) = (char)*(src++));
#else
	temp = (char*)src;
	src_length = lstrlen(src);
	//t = strDec.GetBuffer(src_length/2 + 1);
#endif
	int i=0;
	for(; i < src_length / 2; ++i)
	{
		temp[i] = DOUBLECHARTOVALUE(temp + 2 * i);
	}
	temp[i] = 0;

#ifdef UNICODE
	int iLen	= 0;
	WCHAR* wh	= nullptr;

	MbcsToUnicode(temp, &wh, iLen);

	strDec = wh;
	delete[] wh;
	delete[] temp;
#else
	strDec.ReleaseBuffer();
#endif

	return strDec;
}

CString& StrToUtf8Hex(const TCHAR* src, CString& strDec)
{
	char* t			= nullptr;
	WCHAR* temp		= nullptr;
	int src_length	= 0;

	strDec.Empty();

#ifndef UNICODE
	MbcsToUnicode(src, &temp, src_length);
#else
	temp = (TCHAR*)src;
#endif

	UnicodeToUtf8(temp, &t, src_length);
	src_length -= 1;

	for(int i = 0; i < src_length; ++i)
	{
		TCHAR tc[3] = {0, 0, 0};
		VALUETODOUBLECHAR(tc, t[i]);
		strDec += tc;
	}

#ifndef UNICODE
	delete[] temp;
#endif
	delete[] t;

	return strDec;
}

CString& HexUtf8ToStr(const TCHAR* src, CString& strDec)
{
	char* temp		= nullptr;
	WCHAR* pwsz		= nullptr;
	int iLen		= 0;
	int src_length	= 0;

	strDec.Empty();

#ifdef UNICODE
	char* temp1	= new char[(src_length = lstrlen(src)) +1];
	temp		= temp1;

	while(*(temp1++) = (char)*(src++));
#else
	temp		= (char*)src;
	src_length	= lstrlen(src);
#endif
	int i=0;
	for(; i < src_length / 2; ++i)
	{
		temp[i] = DOUBLECHARTOVALUE(temp + 2*i);
	}

	temp[i] = 0;

	Utf8ToUnicode(temp, &pwsz, iLen);

#ifdef UNICODE
	strDec = pwsz;
	delete[] temp;
#else
	char* psz = nullptr;
	UnicodeToMbcs(pwsz, &psz, iLen);

	strDec = psz;
	delete[] psz;
#endif
	delete[] pwsz;

	return strDec;
}

CString GetSystemErrorDesc(DWORD dwCode)
{
	CString msg;
	LPTSTR lpMsgBuf;

	if(::FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							nullptr,
							dwCode,
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							(LPTSTR)&lpMsgBuf,
							0,
							nullptr
						))
	{
		msg = lpMsgBuf;
		::LocalFree(lpMsgBuf);
	}

	return msg;
}

BOOL SplitStr(LPCTSTR pszSrc, vector<CString>& vtItem, LPCTSTR pszSepectors, LPCTSTR pszQMarks)
{
	vtItem.clear();

	CString strQMarks		= pszQMarks;
	CString strSepectors	= pszSepectors;

	if(strSepectors.IsEmpty())
		strSepectors = _T(" ");

	if(!strQMarks.IsEmpty())
		if(strQMarks.FindOneOf(strSepectors) != -1)
			return FALSE;

	BOOL bRetVal	= TRUE;
	CString strSrc	= pszSrc;

	while(!strSrc.Trim(strSepectors).IsEmpty())
	{
		CString strItem;

		int iSrcLen	= strSrc.GetLength();
		int iPos1	= strSrc.FindOneOf(strSepectors);
		int iPos2	= !strQMarks.IsEmpty() ? strSrc.FindOneOf(strQMarks) : -1;
		int iPos3	= -1;

		if(iPos1 == -1 && iPos2 == -1)
			strItem = strSrc;
		else if(iPos1 != -1 && (iPos1 < iPos2 || iPos2 == -1))
			strItem = strSrc.Left(iPos1);
		else	// (iPos1 > iPos2 || iPos1 == -1)
		{
			TCHAR tc	= strSrc[iPos2];
			iPos3		= strSrc.Find(tc, iPos2 + 1);
			if(iPos3 != -1)
				strItem = strSrc.Mid(iPos2 + 1, iPos3 - iPos2 - 1);
			else
			{
				vtItem.clear();
				bRetVal = FALSE;
				break;
			}
		}

		vtItem.push_back(strItem);

		strSrc = strSrc.Right(iPos3 == -1 ? (iSrcLen - (iPos1 == -1 ? strItem.GetLength() : iPos1 + 1)) : (iSrcLen - iPos3 - 1));
	}

	return bRetVal;
}

CString ExtractFileName(LPCTSTR lpszFullFileName)
{
	CString strPath = lpszFullFileName;
	strPath.Trim();

	if(!strPath.IsEmpty())
	{
		int iLen		= strPath.GetLength();
		int iLastSep	= strPath.ReverseFind(PATH_SEPARATOR_CHAR);

		if(iLastSep != -1)
			strPath		= strPath.Right(iLen - 1 - iLastSep);
	}
	
	return strPath;
}

CString ExtractPath(LPCTSTR lpszFullFileName)
{
	CString strPath = lpszFullFileName;
	strPath.Trim();

	if(strPath.IsEmpty())
		return PATH_SEPARATOR;

	int iLen		= strPath.GetLength();
	int iLastSep	= strPath.ReverseFind(PATH_SEPARATOR_CHAR);
	int iLastDot	= strPath.ReverseFind(FILE_EXTEND_SEPARATOR_CHAR);

	if(iLastSep == -1 && iLastDot == -1)
		strPath.Append(PATH_SEPARATOR);
	else if(iLastSep == -1 && iLastDot != -1)
		strPath = PATH_SEPARATOR;
	else if(iLastSep > iLastDot)
	{
		if(iLastSep < iLen)
			strPath.Append(PATH_SEPARATOR);
	}
	else
	{
		strPath = strPath.Left(iLastSep + 1);
	}

	return strPath;
}

CString ExtractModulePath(HMODULE hModule)
{
	CString strCurPath;
	LPTSTR lpszCurPath = strCurPath.GetBuffer(MAX_PATH);

	BOOL isOK = ::GetModuleFileName(hModule, lpszCurPath, MAX_PATH);
	strCurPath.ReleaseBuffer();

	if(isOK)
	{
		strCurPath = ::ExtractPath(strCurPath);
		ASSERT(!strCurPath.IsEmpty());
	}

	return strCurPath;
}

BOOL RunProcess(LPCTSTR szFileName, LPCTSTR cmdline/* = nullptr*/, BOOL bHide /* = TRUE */, LPCTSTR dir /* =  nullptr*/, BOOL bWait /* = TRUE */, DWORD dwWaitTime /* = INFINITE */)
{
	LPCTSTR process_dir;
	if (dir == nullptr || _tcslen(dir) == 0)
		process_dir = nullptr;
	else
		process_dir = dir;

	LPCTSTR process_name;
	if (szFileName == nullptr || _tcslen(szFileName) == 0)
		process_name = nullptr;
	else
		process_name = szFileName;

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	DWORD dwCreationFlags = 0;
	if (bHide)
	{
		si.wShowWindow = SW_HIDE;
#ifndef _WIN32_WCE
		dwCreationFlags = CREATE_NO_WINDOW;
#endif
	}

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process
	CString strCmd(cmdline);
	LPTSTR pszcmd = (LPTSTR)(LPCTSTR)strCmd;

	BOOL bRet = CreateProcess(
								process_name,
								pszcmd,					// Command line. 
								nullptr,				// Process handle not inheritable. 
								nullptr,				// Thread handle not inheritable. 
								FALSE,					// Set handle inheritance to FALSE. 
								dwCreationFlags,		// No creation flags. 
								nullptr,				// Use parent's environment block. 
								(LPTSTR)process_dir,	// Use parent's starting directory. 
								&si,					// Pointer to STARTUPINFO structure.
								&pi
							);

	if(bRet)
	{
		if(bWait)
			bRet = (::WaitForSingleObject(pi.hProcess, dwWaitTime) != WAIT_FAILED) ? TRUE : FALSE;

		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
	}

	return bRet;
}

BOOL ShellRunExe(LPCTSTR lpszPath, LPCTSTR lpszParams, int iShow, HANDLE* phProcess, BOOL bWait, DWORD dwWaitTime)
{
	CString strPath = lpszPath;
	strPath.Trim();

	ASSERT(strPath.GetLength() > 4 && strPath.Right(4).CompareNoCase(EXE_FILE_EXTEND_NAME) == 0);

#ifdef _WIN32_WCE
	if(strPath.GetAt(0) != PATH_SEPARATOR_CHAR)
#else
	if(strPath.GetAt(1) != DISK_SYMBLE_CHAR)
#endif
	{
		CString strCurPath = ExtractModulePath();
		strPath = strCurPath + strPath;
	}

	SHELLEXECUTEINFO info = {0};

	info.cbSize			= sizeof(SHELLEXECUTEINFO);
	info.lpFile			= strPath;
	info.fMask			= SEE_MASK_FLAG_NO_UI;
	info.nShow			= iShow;
	info.lpParameters	= lpszParams;
	
	if(phProcess || bWait)
		info.fMask		|= SEE_MASK_NOCLOSEPROCESS;

	BOOL isOK = FALSE;

	if(::ShellExecuteEx(&info))
	{
		if(phProcess)
			*phProcess = info.hProcess;

		if(bWait)
		{
			isOK = (::WaitForSingleObject(info.hProcess, dwWaitTime) != WAIT_FAILED) ? TRUE : FALSE;
			::CloseHandle(info.hProcess);
		}
		else
			isOK = TRUE;
	}

	return isOK;
}

void WriteLog(LPCTSTR pszLogFileName, LPCTSTR pszLog)
{
	HANDLE hLogFile = ::CreateFile(pszLogFileName, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(hLogFile != INVALID_HANDLE_VALUE)
		::SetFilePointer(hLogFile, 0, 0, FILE_END);
	else
		return;

	DWORD dwSize;
	SYSTEMTIME st;
	GetLocalTime(&st);
	CString strLog;
	strLog.Format(_T("%04d-%02d-%02d %02d:%02d:%02d.%03d\t%s\r\n"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, pszLog); 
	::WriteFile(hLogFile, strLog, strLog.GetLength() * sizeof(TCHAR), &dwSize, nullptr);

	::CloseHandle(hLogFile);
}

BOOL FindProcess(LPCTSTR pszProcessName)
{
	BOOL isOK		= FALSE;
	HANDLE hTool	= ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if(hTool != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if(::Process32First(hTool, &pe32))
		{
			do
			{
				if(_tcsicmp(pszProcessName, pe32.szExeFile) == 0)
				{
					isOK = TRUE;
					break;
				}

			} while(::Process32Next(hTool, &pe32));
		}

		VERIFY(::CloseToolhelp32Snapshot(hTool));
	}

	return isOK;
}

HANDLE FindProcessHandle(LPCTSTR pszProcessName, DWORD dwDesiredAccess, BOOL bInheritHandle)
{
	HANDLE hProc	= nullptr;
	HANDLE hTool	= ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if(hTool != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if(::Process32First(hTool, &pe32))
		{
			do
			{
				if(_tcsicmp(pszProcessName, pe32.szExeFile) == 0)
				{
					hProc = ::OpenProcess(dwDesiredAccess, bInheritHandle, pe32.th32ProcessID);
					break;
				}

			} while(::Process32Next(hTool, &pe32));
		}

		VERIFY(::CloseToolhelp32Snapshot(hTool));
	}

	return hProc;
}

BOOL FindRunningProcessesInfo(ProcessInfoMap& infoMap)
{
	infoMap.Clear();

	HANDLE hProc	= nullptr;
	HANDLE hTool	= ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if(hTool != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if(::Process32First(hTool, &pe32))
		{
			do
			{
				PROCESSENTRY32* ppe32 = new PROCESSENTRY32;
				memcpy(ppe32, &pe32, sizeof(PROCESSENTRY32));
				infoMap[pe32.th32ProcessID] = ppe32;
			} while(::Process32Next(hTool, &pe32));
		}

		VERIFY(::CloseToolhelp32Snapshot(hTool));
	}
	else
		return FALSE;

	return TRUE;
}

#ifndef _WIN32_WCE
BOOL FindProcessEx(LPCTSTR pszProcessName)
{
	BOOL bRet = FALSE;

	DWORD aProcesses[1024], cbNeeded, cProcesses;

	if (::EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		cProcesses = cbNeeded / sizeof(DWORD);

		for (DWORD i = 0; i < cProcesses; i++)
		{
			HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

			if (hProcess)
			{
				HMODULE	hMod;
				DWORD	cbNeeded;
				if (::EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
				{
					TCHAR szProcessName[MAX_PATH] = {0};
					if(::GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName)))
					{
						if(_tcsicmp(szProcessName, pszProcessName) == 0)
						{
							DWORD dwExitCode = 0;
							if(::GetExitCodeProcess(hProcess, &dwExitCode) && dwExitCode == STILL_ACTIVE)
							{
								bRet = TRUE;
								break;
							}
						}
					}
				}
				::CloseHandle( hProcess );
			}
		}
	}

	return bRet;
}

CString GetIniString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpFileName, DWORD dwSize)
{
	CString strValue;
	LPTSTR pszBuffer = strValue.GetBuffer(dwSize);
	::GetPrivateProfileString(lpAppName, lpKeyName, _T(""), pszBuffer, dwSize, lpFileName);
	strValue.ReleaseBuffer();
	strValue.Trim();
	return strValue;
}

BOOL SetCurrentPathToModulePath(HMODULE hModule)
{
	TCHAR szPath[MAX_PATH];
	if(::GetModuleFileName(hModule, szPath, MAX_PATH))
	{
		TCHAR drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
		_tsplitpath(szPath, drive, dir, fname, ext);
		lstrcpy(szPath, drive);
		lstrcat(szPath, dir);

		return ::SetCurrentDirectory(szPath);
	}

	return FALSE;
}
#endif

struct TProcWnd
{
	DWORD	dwProcID;
	LPCTSTR lpszClassName;
	HWND	hWnd;
};

BOOL CALLBACK ProcessMainWindowEnumFunc(HWND hwnd, LPARAM lParam)
{
	TProcWnd* ppw = (TProcWnd*)lParam;

	DWORD dwProcID;
	::GetWindowThreadProcessId(hwnd, &dwProcID);

	if(dwProcID == ppw->dwProcID)
	{
		while(TRUE)
		{
			HWND hwParent = ::GetParent(hwnd);

			if(hwParent == nullptr)
			{
				if(ppw->lpszClassName == nullptr)
				{
					ppw->hWnd = hwnd;
					return FALSE;
				}
				else
				{
					int nMaxCount = MAX_PATH - 1;
					TCHAR szName[MAX_PATH] = {0};

					if(::GetClassName(hwnd, szName, nMaxCount))
					{
						if(_tcscmp(szName, ppw->lpszClassName) == 0)
						{
							ppw->hWnd = hwnd;
							return FALSE;
						}
					}
				}

				break;
			}
			else
				hwnd = hwParent;
		}
	}

	return TRUE;
}

HWND FindProcessMainWindow(HANDLE hProcess, LPCTSTR lpszWndClassName)
{
	DWORD dwProcID = ::GetProcessId(hProcess);

	if(dwProcID != 0)
	{
		TProcWnd pw;
		pw.dwProcID			= dwProcID;
		pw.lpszClassName	= lpszWndClassName;
		pw.hWnd				= 0;

		if(::EnumWindows(ProcessMainWindowEnumFunc, (LPARAM)&pw))
			return pw.hWnd;
	}

	return nullptr;
}

BOOL CALLBACK CloseWindowEnumFunc(HWND hwnd, LPARAM lParam)
{
	DWORD dwProcID;
	::GetWindowThreadProcessId(hwnd, &dwProcID);

	if(dwProcID == (DWORD)lParam)
		::PostMessage(hwnd, WM_CLOSE, 0, 0);

	return TRUE;
}

BOOL TerminateProcessFairily(HANDLE hProcess, DWORD dwWait)
{
	DWORD dwProcID = ::GetProcessId(hProcess);

	if(dwProcID != 0)
	{
		VERIFY(::EnumWindows(CloseWindowEnumFunc, (LPARAM)dwProcID));

		if(!::MsgWaitForSingleObject(hProcess, dwWait))
			return ::TerminateProcess(hProcess, -1);

		return TRUE;
	}

	return FALSE;
}

#ifdef _AFX
CString SecondToTimeStr(DWORD dwSeconds, BOOL bDayOnly)
{
	CTime tm(dwSeconds);
	LPCTSTR pszFormat = bDayOnly ? _T("%Y-%m-%d") : _T("%Y-%m-%d %H:%M:%S");
	return tm.Format(pszFormat);
}
#endif

BOOL GetRegistryValue(HKEY hRoot, LPCTSTR wcSubKey, LPCTSTR wcName, LPBYTE pValue, DWORD* pdwSize, DWORD* pdwType)
{
	DWORD result = ERROR_INVALID_DATA;

	CRegKey reg;

	result = reg.Create(hRoot, wcSubKey, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ);
	if(result  == ERROR_SUCCESS)
		result = reg.QueryValue(wcName, pdwType, pValue, pdwSize);
	

	BOOL isOK = (result == ERROR_SUCCESS);

	if(!isOK)
	{
		*pValue		= 0;
		*pdwSize	= 0;
		*pdwType	= REG_NONE;
	}

	return isOK;
}

BOOL SetRegistryValue(HKEY hRoot, LPCTSTR wcSubKey, LPCTSTR wcName, LPBYTE pValue, DWORD dwSize, DWORD dwType)
{
	DWORD result = ERROR_INVALID_DATA;

	CRegKey reg;

	result = reg.Create(hRoot, wcSubKey);
	if(result  == ERROR_SUCCESS)
		result = reg.SetValue(wcName, dwType, pValue, dwSize);

	return (result == ERROR_SUCCESS);
}
