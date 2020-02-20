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

#pragma once

#include "FuncHelper.h"
#include "StringT.h"

#include <unistd.h>
#include <sys/uio.h>
#include <sys/mman.h>

#define INVALID_MAP_ADDR	((PBYTE)(MAP_FAILED))

CString GetCurrentDirectory();
CString GetModuleFileName(pid_t pid = 0);
BOOL SetCurrentPathToModulePath(pid_t pid = 0);

class CFile
{
public:
	BOOL Open(LPCTSTR lpszFilePath, int iFlag, mode_t iMode = 0);
	BOOL Close();
	BOOL Stat(struct stat& st);
	BOOL GetSize(SIZE_T& dwSize);

	SSIZE_T Read(PVOID pBuffer, SIZE_T dwCount)
		{return read(m_fd, pBuffer, dwCount);}
	SSIZE_T Write(PVOID pBuffer, SIZE_T dwCount)
		{return write(m_fd, pBuffer, dwCount);}
	SSIZE_T PRead(PVOID pBuffer, SIZE_T dwCount, SIZE_T dwOffset)
		{return pread(m_fd, pBuffer, dwCount, dwOffset);}
	SSIZE_T PWrite(PVOID pBuffer, SIZE_T dwCount, SIZE_T dwOffset)
		{return pwrite(m_fd, pBuffer, dwCount, dwOffset);}
	SSIZE_T ReadV(const iovec* pVec, int iVecCount)
		{return readv(m_fd, pVec, iVecCount);}
	SSIZE_T WriteV(const iovec* pVec, int iVecCount)
		{return writev(m_fd, pVec, iVecCount);}
	SSIZE_T Seek(SSIZE_T lOffset, int iWhence)
		{return lseek(m_fd, lOffset, iWhence);}

	BOOL IsValid()	{return IS_VALID_FD(m_fd);}
	operator FD ()	{return m_fd;}

	BOOL IsExist()	{return IsValid();}

	BOOL IsDirectory();
	BOOL IsFile();

	static BOOL IsExist(LPCTSTR lpszFilePath);
	static BOOL IsDirectory(LPCTSTR lpszFilePath);
	static BOOL IsFile(LPCTSTR lpszFilePath);
	static BOOL IsLink(LPCTSTR lpszFilePath);

public:
	CFile(LPCTSTR lpszFilePath = nullptr, int iFlag = O_RDONLY, mode_t iMode = 0)
	: m_fd(INVALID_FD)
	{
		if(lpszFilePath != nullptr)
			Open(lpszFilePath, iFlag, iMode);
	}

	~CFile()
	{
		if(IsValid())
			Close();
	}

private:
	FD m_fd;
};

class CFileMapping
{
public:
	BOOL Map(LPCTSTR lpszFilePath, SIZE_T dwSize = 0, SIZE_T dwOffset = 0, int iProtected = PROT_READ, int iFlag = MAP_PRIVATE);
	BOOL Map(FD fd, SIZE_T dwSize = 0, SIZE_T dwOffset = 0, int iProtected = PROT_READ, int iFlag = MAP_PRIVATE);
	BOOL Unmap();
	BOOL MSync(int iFlag = MS_SYNC, SIZE_T dwSize = 0);
	
	BOOL IsValid	()		{return m_pv != INVALID_MAP_ADDR;}
	SIZE_T Size		()		{return m_dwSize;}
	LPBYTE Ptr		()		{return m_pv;}
	operator LPBYTE	()		{return Ptr();}

public:
	CFileMapping()
	: m_pv(INVALID_MAP_ADDR)
	, m_dwSize(0)
	{
		
	}

	~CFileMapping()
	{
		if(IsValid())
			Unmap();
	}

private:
	PBYTE	m_pv;
	SIZE_T	m_dwSize;
};
