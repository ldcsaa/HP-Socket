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

#include "FileHelper.h"

#include <sys/stat.h>

CString GetCurrentDirectory()
{
	char szPath[MAX_PATH];

	if(getcwd(szPath, sizeof(szPath) - 1) == nullptr)
		szPath[0] = 0;

	return szPath;
}

CString GetModuleFileName(pid_t pid)
{
	if(pid == 0)
		pid = SELF_PROCESS_ID;

	char szLink[MAX_PATH];
	char szPath[MAX_PATH];

	sprintf(szLink, "/proc/%d/exe", pid);

	SSIZE_T rs = readlink(szLink, szPath, sizeof(szPath) - 1);

	if(rs < 0) rs = 0;
	szPath[rs]	  = 0;

	return szPath;
}

BOOL SetCurrentPathToModulePath(pid_t pid)
{
	CString strPath = GetModuleFileName(pid);

	if(strPath.IsEmpty())
		return FALSE;
	
	CString::size_type pos = strPath.rfind('/');

	if(pos == CString::npos)
		return FALSE;

	return IS_NO_ERROR(chdir(strPath.substr(0, pos + 1)));
}

BOOL CFile::Open(LPCTSTR lpszFilePath, int iFlag, mode_t iMode)
{
	CHECK_ERROR(!IsValid(), ERROR_INVALID_STATE);

	m_fd = open(lpszFilePath, iFlag, iMode);

	return IS_VALID_FD(m_fd);
}

BOOL CFile::Close()
{
	CHECK_ERROR(IsValid(), ERROR_INVALID_STATE);

	if(IS_NO_ERROR(close(m_fd)))
	{
		m_fd = INVALID_FD;
		return TRUE;
	}

	return FALSE;
}

BOOL CFile::Stat(struct stat& st)
{
	CHECK_ERROR_INVOKE(fstat(m_fd, &st));
	return TRUE;
}

BOOL CFile::GetSize(SIZE_T& dwSize)
{
	struct stat st;
	CHECK_IS_OK(Stat(st));

	dwSize = st.st_size;

	return TRUE;
}

BOOL CFile::IsDirectory()
{
	struct stat st;
	CHECK_IS_OK(Stat(st));

	return S_ISDIR(st.st_mode);
}

BOOL CFile::IsFile()
{
	struct stat st;
	CHECK_IS_OK(Stat(st));

	return S_ISREG(st.st_mode);
}

BOOL CFile::IsExist(LPCTSTR lpszFilePath)
{
	return IS_NO_ERROR(access(lpszFilePath, F_OK));
}

BOOL CFile::IsDirectory(LPCTSTR lpszFilePath)
{
	struct stat st;
	CHECK_ERROR_INVOKE(stat(lpszFilePath, &st));

	return S_ISDIR(st.st_mode);
}

BOOL CFile::IsFile(LPCTSTR lpszFilePath)
{
	struct stat st;
	CHECK_ERROR_INVOKE(stat(lpszFilePath, &st));

	return S_ISREG(st.st_mode);
}

BOOL CFile::IsLink(LPCTSTR lpszFilePath)
{
	struct stat st;
	CHECK_ERROR_INVOKE(lstat(lpszFilePath, &st));

	return S_ISLNK(st.st_mode);
}

BOOL CFileMapping::Map(LPCTSTR lpszFilePath, SIZE_T dwSize, SIZE_T dwOffset, int iProtected, int iFlag)
{
	CHECK_ERROR(!IsValid(), ERROR_INVALID_STATE);

	FD fd = INVALID_FD;

	if(lpszFilePath != nullptr)
	{
		int iFileFlag = O_RDONLY;

		if(iProtected & PROT_WRITE)
		{
			if(iProtected & PROT_READ)
				iFileFlag = O_RDWR;
			else
				iFileFlag = O_WRONLY;
		}

		fd = open(lpszFilePath, iFileFlag);
		CHECK_ERROR_FD(fd);
	}

	BOOL isOK = Map(fd, dwSize, dwOffset, iProtected, iFlag);

	if(IS_VALID_FD(fd)) EXECUTE_RESTORE_ERROR(close(fd));

	return isOK;
}

BOOL CFileMapping::Map(FD fd, SIZE_T dwSize, SIZE_T dwOffset, int iProtected, int iFlag)
{
	CHECK_ERROR(!IsValid(), ERROR_INVALID_STATE);

	if(IS_INVALID_FD(fd))
	{
		CHECK_EINVAL((iFlag & MAP_ANONYMOUS) && (dwSize > 0));
	}
	else
	{
		CHECK_EINVAL((iFlag & MAP_ANONYMOUS) == 0);

		struct stat st;
		CHECK_ERROR_INVOKE(fstat(fd, &st));

		CHECK_ERROR(S_ISREG(st.st_mode), ERROR_BAD_FILE_TYPE);

		if(dwSize == 0)
			dwSize = st.st_size;
	}

	m_pv = (PBYTE)mmap(nullptr, dwSize, iProtected, iFlag, fd, dwOffset);

	if(IsValid())
	{
		m_dwSize = dwSize;
		return TRUE;
	}

	return FALSE;
}

BOOL CFileMapping::Unmap()
{
	CHECK_ERROR(IsValid(), ERROR_INVALID_STATE);
		
	if(IS_NO_ERROR(munmap(m_pv, m_dwSize)))
	{
		m_pv	 = INVALID_MAP_ADDR;
		m_dwSize = 0;

		return TRUE;
	}

	return FALSE;
}

BOOL CFileMapping::MSync(int iFlag, SIZE_T dwSize)
{
	CHECK_ERROR(IsValid(), ERROR_INVALID_STATE);

	if(dwSize == 0) dwSize = m_dwSize;

	return IS_NO_ERROR(msync(m_pv, dwSize, iFlag));
}
