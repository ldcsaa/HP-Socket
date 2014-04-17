/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.3
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
 
/****************************************************************************
*																			*
* CriticalSection.h 														*
*																			*
* Create by :																*
* Kingfisher	2003-10-15													*
* 																			*
* Description: 																*
* 封装Win32临界量对象和互斥量内核对象											*
****************************************************************************/

#pragma once

class CCriSec
{
public:
	CCriSec()		{::InitializeCriticalSection(&m_crisec);}
	~CCriSec()		{::DeleteCriticalSection(&m_crisec);}

	void Lock()		{::EnterCriticalSection(&m_crisec);}
	void Unlock()	{::LeaveCriticalSection(&m_crisec);}

private:
	CCriSec(const CCriSec& cs);
	CCriSec operator = (const CCriSec& cs);

private:
	CRITICAL_SECTION    m_crisec;
};

class CCriSec2
{
public:
	CCriSec2(BOOL bInitialize = TRUE)
	{
		if(bInitialize)
		{
			m_pcrisec = new CRITICAL_SECTION;
			::InitializeCriticalSection(m_pcrisec);
		}
		else
			m_pcrisec = nullptr;
	}

	~CCriSec2() {Reset();}

	void Attach(CRITICAL_SECTION* pcrisec)
	{
		Reset();
		m_pcrisec = pcrisec;
	}

	CRITICAL_SECTION* Detach()
	{
		CRITICAL_SECTION* pcrisec = m_pcrisec;
		m_pcrisec = nullptr;
		return pcrisec;
	}

	void Lock()		{::EnterCriticalSection(m_pcrisec);}
	void Unlock()	{::LeaveCriticalSection(m_pcrisec);}

private:
	CCriSec2(const CCriSec2& cs);
	CCriSec2 operator = (const CCriSec2& cs);

	void Reset()
	{
		if(m_pcrisec)
		{
			::DeleteCriticalSection(m_pcrisec);
			delete m_pcrisec;
			m_pcrisec = nullptr;
		}
	}

private:
	CRITICAL_SECTION*    m_pcrisec;
};

class CMTX
{
public:
	CMTX(BOOL bInitialOwner = FALSE, LPCTSTR pszName = nullptr, LPSECURITY_ATTRIBUTES pSecurity = nullptr)	
	{
		m_hMutex = ::CreateMutex(pSecurity, bInitialOwner, pszName);
		ASSERT(IsValid());
	}

	~CMTX()
	{
		if(IsValid())
			::CloseHandle(m_hMutex);
	}

	BOOL Open(DWORD dwAccess, BOOL bInheritHandle, LPCTSTR pszName)
	{
		if(IsValid())
			VERIFY(::CloseHandle(m_hMutex));

		m_hMutex = ::OpenMutex(dwAccess, bInheritHandle, pszName);
		return(IsValid());
	}

	void Lock(DWORD dwMilliseconds = INFINITE)	{::WaitForSingleObject(m_hMutex, dwMilliseconds);}
	void Unlock()								{::ReleaseMutex(m_hMutex);}

	HANDLE& GetHandle	() 	{return m_hMutex;}
	operator HANDLE		()	{return m_hMutex;}
	BOOL IsValid		()	{return m_hMutex != nullptr;}

private:
	CMTX(const CMTX& mtx);
	CMTX operator = (const CMTX& mtx);

private:
	HANDLE m_hMutex;
};

template<class CLockObj> class CLocalLock
{
public:
	CLocalLock(CLockObj& obj) : m_lock(obj) {m_lock.Lock();}
	~CLocalLock() {m_lock.Unlock();}
private:
	CLockObj& m_lock;
};

typedef CLocalLock<CCriSec>		CCriSecLock;
typedef CLocalLock<CCriSec2>	CCriSecLock2;
typedef CLocalLock<CMTX>		CMutexLock;
