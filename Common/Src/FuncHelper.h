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
 
#pragma once

#include "GeneralHelper.h"

#include <tlhelp32.h>
#include <malloc.h>

#define PATH_SEPARATOR				_T("\\")
#define PATH_SEPARATOR_CHAR			_T('\\')
#define FILE_EXTEND_SEPARATOR		_T(".")
#define FILE_EXTEND_SEPARATOR_CHAR	_T('.')
#define DISK_SYMBLE					_T(":")
#define DISK_SYMBLE_CHAR			_T(':')
#define EXE_FILE_EXTEND_NAME		_T(".exe")

enum EnCodePage
{
	XCP_ACP		= CP_ACP,
	XCP_GB2312	= 936,
#ifdef _WIN32_WCE
	XCP_BIG5	= CP_ACP,
#else
	XCP_BIG5	= 950,
#endif
	XCP_GBK		= XCP_GB2312,
	XCP_UTF7	= CP_UTF7,
	XCP_UTF8	= CP_UTF8,
};

/************************************* 字符操作 *************************************/

// 把字符转换成其表示的值: 如字符 'F' 转换成 数值 15
#define CHARTOVALUE(c)				(c <= '9' ? c - '0' : (c <= 'F' ? c - 'A' + 0x0A : c - 'a' + 0X0A))
// 把双字符转换成其表示的值: 如字符 '1F' 转换成 数值 31
#define DOUBLECHARTOVALUE(pc)		(((CHARTOVALUE(*(pc))) << 4) | (CHARTOVALUE(*(pc + 1))))

// 把数值转换成其十六进制字符: 如数值 15 转换成 字符 'F'
#define VALUETOCHAR(n)				(n <= 9 ? n + '0' : (n <= 'F' ? n + 'A' - 0X0A : n + 'a' - 0X0A))
// 把数值字符转换成其两个十六进制字符: 如数值 11 转换成 字符 '0B'
#define VALUETODOUBLECHAR(pc, n)	{*(pc) = VALUETOCHAR((n >> 4)); *((pc) + 1) = VALUETOCHAR((n & 0X0F));}

// 把双字符转换成其表示的值: 如字符 '1F' 转换成 数值 31
BYTE DoubleCharToByte(LPCTSTR psValue);
// 把数值字符转换成其两个十六进制字符: 如数值 11 转换成 字符 '0B'
LPTSTR ByteToDoubleChar(BYTE b, LPTSTR des);
// 把十六进制转换成其表示的值: 如字符 '601F' 转换成 数值 24607
UINT HexStrToInt(LPCTSTR pHexText, int len = -1);
// 把十进制转换成其表示的值: 如字符 '0000024607' 转换成 数值 24607
UINT DecStrToInt(LPCTSTR pDecText, int len = -1);
// 把数值换成其十六进制字符串表示的值: 如数值 24607 转换成 字符 '601F'
CString& IntToHexStr(CString& dest, UINT v, int len = -1);
// 把数值换成其十进制字符串表示的值: 如数值 24607 转换成 字符 '24607'
CString& IntToDecStr(CString& dest, UINT v, int len = -1);
// 把十六进制表示的地址改为十进制表示的地址: 如 "000C35CE" 转换成 '0000800236'
CString& HexAddrToDecAddr(CString& dest, LPCTSTR src, int destlen = 10, int srclen = -1);
// 把十进制表示的地址改为十六进制表示的地址: 如 "0000800236" 转换成 '000C35CE'
CString& DecAddrToHexAddr(CString& dest, LPCTSTR src, int destlen = 8, int srclen = -1);

// Code Page Name -> Code Page Value
EnCodePage GetCodePageByName(LPCTSTR lpszCodePageName);

// MBCS -> UNICODE
BOOL MbcsToUnicode(const char* pszInString, WCHAR** ptrOutWStr, int& nSizeCount);
// UNICODE -> MBCS
BOOL UnicodeToMbcs(const WCHAR* pwzInString, char** ptrOutStr, int& nSizeCount);
// UTF8 -> UNICODE
BOOL Utf8ToUnicode(const char* pszInString, WCHAR** ptrOutWStr, int& nSizeCount);
// UNICODE -> UTF8
BOOL UnicodeToUtf8(const WCHAR* pwzInString, char** ptrOutStr, int& nSizeCount);
// CP_XXX -> UNICODE
BOOL CPToUni(const char* pszInString, WCHAR** ptrOutWStr, unsigned int nCodePage, int& nSizeCount);
// UNICODE -> CP_XXX
BOOL UniToCP(const WCHAR* pwzInString, char** ptrOutStr, unsigned int nCodePage, int& nSizeCount);

// 把字节数组转换成16进制字符串
int BytesToHex(const BYTE* pBytes, int nLength, LPTSTR* lpszDest);
// 把16进制字符串转换成字节数组
int HexToBytes(LPCTSTR lpszHex, BYTE** ppBytes, int* pnLength);

// 把普通字符串转换成16进制字符串
CString& StrToHex(const TCHAR* src, CString& des);
// 把16进制字符串转换成普通字符串
CString& HexToStr(const TCHAR* src, CString& des);
// 把普通字符串转换成UTF8字符串, 然后再把该UTF8字符串转换成16进制字符串
CString& StrToUtf8Hex(const TCHAR* src, CString& strDec);
// 把16进制字符串转换成UTF8字符串, 然后再把该UTF8字符串转换成普通字符串
CString& HexUtf8ToStr(const TCHAR* src, CString& strDec);

// 取得系统错误代码的描述信息
CString GetSystemErrorDesc(DWORD dwCode);

// 分隔字符串
BOOL SplitStr(LPCTSTR pszSrc, vector<CString>& vtItem, LPCTSTR pszSepectors = nullptr, LPCTSTR pszQMarks = nullptr);

// 提取文件名称
CString ExtractFileName(LPCTSTR lpszFullFileName);
// 提取文件路径
CString ExtractPath(LPCTSTR lpszFullFileName);
// 提取文件路径
CString ExtractPath(LPCTSTR lpszFullFileName);
// 提取当前模块路径
CString ExtractModulePath(HMODULE hModule = nullptr);

// 启动进程
BOOL RunProcess(LPCTSTR sFileName, LPCTSTR cmdline = nullptr, BOOL bHide = TRUE, LPCTSTR dir = nullptr, BOOL bWait = TRUE, DWORD dwWaitTime = INFINITE);
BOOL ShellRunExe(LPCTSTR lpszPath, LPCTSTR lpszParams = nullptr, int iShow = SW_SHOWNORMAL, HANDLE* phProcess = nullptr, BOOL bWait = FALSE, DWORD dwWaitTime = INFINITE);

// 查找进程
BOOL FindProcess(LPCTSTR pszProcessName);

// 查找进程句柄
HANDLE FindProcessHandle(LPCTSTR pszProcessName, DWORD dwDesiredAccess = PROCESS_QUERY_INFORMATION, BOOL bInheritHandle = FALSE);

typedef map<DWORD, PROCESSENTRY32*>									ProcessInfos;
typedef MapWrapper<ProcessInfos, PtrMap_Cleaner<ProcessInfos>>		ProcessInfoMap;

// 查找当前运行的信息
BOOL FindRunningProcessesInfo(ProcessInfoMap& infoMap);

// 查找进程的主窗口
HWND FindProcessMainWindow(HANDLE hProcess, LPCTSTR lpszClassName = nullptr);

BOOL TerminateProcessFairily(HANDLE hProcess, DWORD dwWait = INFINITE);

// 简单记录日志
void WriteLog(LPCTSTR pszLogFileName, LPCTSTR pszLog);

#ifndef _WIN32_WCE
	// 设置当前路径
	BOOL SetCurrentPathToModulePath(HMODULE hModule = nullptr);
	// 查找进程
	BOOL FindProcessEx(LPCTSTR pszProcessName);
	// 读取 INI 配置
	CString GetIniString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpFileName, DWORD dwSize);
#endif

#ifdef _AFX
	// 秒转换成时间字符串
	CString SecondToTimeStr(DWORD dwSeconds, BOOL bDayOnly = FALSE);
#endif

// 设置注册表项
BOOL GetRegistryValue(HKEY hRoot, LPCTSTR wcSubKey, LPCTSTR wcName, LPBYTE pValue, DWORD* pdwSize, DWORD* pdwType);
// 获取注册表项
BOOL SetRegistryValue(HKEY hRoot, LPCTSTR wcSubKey, LPCTSTR wcName, LPBYTE pValue, DWORD dwSize, DWORD dwType);