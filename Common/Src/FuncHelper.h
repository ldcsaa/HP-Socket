/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.20
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

#include "GeneralHelper.h"

#include <tlhelp32.h>
#include <malloc.h>

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

/************************************* �ַ����� *************************************/

// ���ַ�ת�������ʾ��ֵ: ���ַ� 'F' ת���� ��ֵ 15
#define CHARTOVALUE(c)				(c <= '9' ? c - '0' : (c <= 'F' ? c - 'A' + 0x0A : c - 'a' + 0X0A))
// ��˫�ַ�ת�������ʾ��ֵ: ���ַ� '1F' ת���� ��ֵ 31
#define DOUBLECHARTOVALUE(pc)		(((CHARTOVALUE(*(pc))) << 4) | (CHARTOVALUE(*(pc + 1))))

// ����ֵת������ʮ�������ַ�: ����ֵ 15 ת���� �ַ� 'F'
#define VALUETOCHAR(n)				(n <= 9 ? n + '0' : (n <= 'F' ? n + 'A' - 0X0A : n + 'a' - 0X0A))
// ����ֵ�ַ�ת����������ʮ�������ַ�: ����ֵ 11 ת���� �ַ� '0B'
#define VALUETODOUBLECHAR(pc, n)	{*(pc) = VALUETOCHAR((n >> 4)); *((pc) + 1) = VALUETOCHAR((n & 0X0F));}

// ��˫�ַ�ת�������ʾ��ֵ: ���ַ� '1F' ת���� ��ֵ 31
BYTE DoubleCharToByte(LPCTSTR psValue);
// ����ֵ�ַ�ת����������ʮ�������ַ�: ����ֵ 11 ת���� �ַ� '0B'
LPTSTR ByteToDoubleChar(BYTE b, LPTSTR des);
// ��ʮ������ת�������ʾ��ֵ: ���ַ� '601F' ת���� ��ֵ 24607
UINT HexStrToInt(LPCTSTR pHexText, int len = -1);
// ��ʮ����ת�������ʾ��ֵ: ���ַ� '0000024607' ת���� ��ֵ 24607
UINT DecStrToInt(LPCTSTR pDecText, int len = -1);
// ����ֵ������ʮ�������ַ�����ʾ��ֵ: ����ֵ 24607 ת���� �ַ� '601F'
CString& IntToHexStr(CString& dest, UINT v, int len = -1);
// ����ֵ������ʮ�����ַ�����ʾ��ֵ: ����ֵ 24607 ת���� �ַ� '24607'
CString& IntToDecStr(CString& dest, UINT v, int len = -1);
// ��ʮ�����Ʊ�ʾ�ĵ�ַ��Ϊʮ���Ʊ�ʾ�ĵ�ַ: �� "000C35CE" ת���� '0000800236'
CString& HexAddrToDecAddr(CString& dest, LPCTSTR src, int destlen = 10, int srclen = -1);
// ��ʮ���Ʊ�ʾ�ĵ�ַ��Ϊʮ�����Ʊ�ʾ�ĵ�ַ: �� "0000800236" ת���� '000C35CE'
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

// ���ֽ�����ת����16�����ַ���
int BytesToHex(const BYTE* pBytes, int nLength, LPTSTR* lpszDest);
// ��16�����ַ���ת�����ֽ�����
int HexToBytes(LPCTSTR lpszHex, BYTE** ppBytes, int* pnLength);

// ����ͨ�ַ���ת����16�����ַ���
CString& StrToHex(const TCHAR* src, CString& des);
// ��16�����ַ���ת������ͨ�ַ���
CString& HexToStr(const TCHAR* src, CString& des);
// ����ͨ�ַ���ת����UTF8�ַ���, Ȼ���ٰѸ�UTF8�ַ���ת����16�����ַ���
CString& StrToUtf8Hex(const TCHAR* src, CString& strDec);
// ��16�����ַ���ת����UTF8�ַ���, Ȼ���ٰѸ�UTF8�ַ���ת������ͨ�ַ���
CString& HexUtf8ToStr(const TCHAR* src, CString& strDec);

// ȡ��ϵͳ��������������Ϣ
CString GetSystemErrorDesc(DWORD dwCode);

// �ָ��ַ���
BOOL SplitStr(LPCTSTR pszSrc, vector<CString>& vtItem, LPCTSTR pszSepectors = nullptr, LPCTSTR pszQMarks = nullptr);

// ��ȡ�ļ�����
CString ExtractFileName(LPCTSTR lpszFullFileName);
// ��ȡ�ļ�·��
CString ExtractPath(LPCTSTR lpszFullFileName);
// ��ȡ�ļ�·��
CString ExtractPath(LPCTSTR lpszFullFileName);
// ��ȡ��ǰģ��·��
CString ExtractModulePath(HMODULE hModule = nullptr);

// ��������
BOOL RunProcess(LPCTSTR sFileName, LPCTSTR cmdline = nullptr, BOOL bHide = TRUE, LPCTSTR dir = nullptr, BOOL bWait = TRUE, DWORD dwWaitTime = INFINITE);
BOOL ShellRunExe(LPCTSTR lpszPath, LPCTSTR lpszParams = nullptr, int iShow = SW_SHOWNORMAL, HANDLE* phProcess = nullptr, BOOL bWait = FALSE, DWORD dwWaitTime = INFINITE);

// ���ҽ���
BOOL FindProcess(LPCTSTR pszProcessName);

// ���ҽ��̾��
HANDLE FindProcessHandle(LPCTSTR pszProcessName, DWORD dwDesiredAccess = PROCESS_QUERY_INFORMATION, BOOL bInheritHandle = FALSE);

typedef map<DWORD, PROCESSENTRY32*>									ProcessInfos;
typedef MapWrapper<ProcessInfos, PtrMap_Cleaner<ProcessInfos>>		ProcessInfoMap;

// ���ҵ�ǰ���е���Ϣ
BOOL FindRunningProcessesInfo(ProcessInfoMap& infoMap);

// ���ҽ��̵�������
HWND FindProcessMainWindow(DWORD dwProcID, LPCTSTR lpszClassName = nullptr);
// ���ҽ��̵�������
HWND FindProcessMainWindow(HANDLE hProcess, LPCTSTR lpszClassName = nullptr);

BOOL TerminateProcessFairily(HANDLE hProcess, DWORD dwWait = INFINITE);

// �򵥼�¼��־
void WriteLog(LPCTSTR pszLogFileName, LPCTSTR pszLog);

#ifndef _WIN32_WCE
	// ���õ�ǰ·��
	BOOL SetCurrentPathToModulePath(HMODULE hModule = nullptr);
	// ���ҽ���
	BOOL FindProcessEx(LPCTSTR pszProcessName);
	// ��ȡ INI ����
	CString GetIniString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpFileName, DWORD dwSize);
#endif

#ifdef _AFX
	// ��ת����ʱ���ַ���
	CString SecondToTimeStr(DWORD dwSeconds, BOOL bDayOnly = FALSE);
#endif

// ����ע�����
BOOL GetRegistryValue(HKEY hRoot, LPCTSTR wcSubKey, LPCTSTR wcName, LPBYTE pValue, DWORD* pdwSize, DWORD* pdwType);
// ��ȡע�����
BOOL SetRegistryValue(HKEY hRoot, LPCTSTR wcSubKey, LPCTSTR wcName, LPBYTE pValue, DWORD dwSize, DWORD dwType);