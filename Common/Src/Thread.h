/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.11
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

#include <process.h>
#include "RWLock.h"
#include "STLHelper.h"

template<class T = void> class CThread
{
public:
	virtual UINT Run(T* lpParam)  = 0;

	BOOL Start(T* lpParam = nullptr, int iPriority = THREAD_PRIORITY_NORMAL, UINT uiStackSize = 0, LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr)
	{
		BOOL isOK = TRUE;

		if(!IsRunning())
		{
			Release();

			m_lpParam	= lpParam;
			m_iPriority	= iPriority;
			m_hThread	= (HANDLE)_beginthreadex(lpThreadAttributes, uiStackSize, ThreadProc, (LPVOID)this, 0, nullptr);
			isOK		= (m_hThread != nullptr);
		
			if(!isOK)	::SetLastError(_doserrno);
		}
		else
		{
			::SetLastError(ERROR_INVALID_STATE);
			isOK = FALSE;
		}

		return isOK;
	}

	DWORD Join(DWORD dwMilliseconds = INFINITE)
	{
		return ::WaitForSingleObject(m_hThread, dwMilliseconds);
	}

	BOOL IsRunning()
	{
		DWORD dwExitCode;

		if(GetExitCode(&dwExitCode))
			return dwExitCode == STILL_ACTIVE;

		return FALSE;
	}

	VOID Release()
	{
		if(m_hThread != nullptr)
		{
			::CloseHandle(m_hThread);
			m_hThread = nullptr;
		}
	}

	HANDLE Detatch()
	{
		HANDLE h	= m_hThread;
		m_hThread	= nullptr;

		return h;
	}

	CThread& Attatch(HANDLE hThread)
	{
		if(hThread != m_hThread)
		{
			Release();

			m_hThread = hThread;
		}

		return *this;
	}

#if _WIN32_WINNT >= _WIN32_WINNT_WS03
	DWORD GetID			()						{return ::GetThreadId(m_hThread);}
#endif

	BOOL Terminate		(DWORD dwExitCode)		{return ::TerminateThread(m_hThread, dwExitCode);}
	BOOL GetExitCode	(LPDWORD lpExitCode)	{return ::GetExitCodeThread(m_hThread, lpExitCode);}
	DWORD Suspend		()						{return ::SuspendThread(m_hThread);}
	DWORD Resume		()						{return ::ResumeThread(m_hThread);}

	BOOL IsValid		()	{return m_hThread != nullptr;}
	HANDLE& GetHandle	() 	{return m_hThread;}
	operator HANDLE		()	{return m_hThread;}

public:
	CThread()
	: m_hThread(nullptr)
	, m_lpParam(nullptr)
	, m_iPriority(THREAD_PRIORITY_NORMAL)
	{
	}
	
	virtual ~CThread()
	{
		Release();
	}

private:
	static UINT WINAPI ThreadProc(LPVOID pv)
	{
		CThread* pThis = (CThread*)pv;

		if(pThis->m_iPriority != THREAD_PRIORITY_NORMAL)
			::SetThreadPriority(GetCurrentThread(), pThis->m_iPriority);

		return pThis->Run(pThis->m_lpParam);
	}

private:
	HANDLE	m_hThread;
	T*		m_lpParam;
	int		m_iPriority;

	DECLARE_NO_COPY_CLASS(CThread)
};

template<class T, typename construct_param_type = void*> class CTlsObj
{
	typedef unordered_map<DWORD, T*>			TLocalMap;
	typedef typename TLocalMap::iterator		TLocalMapI;
	typedef typename TLocalMap::const_iterator	TLocalMapCI;

public:
	T* TryGet(DWORD dwTID = 0)
	{
		T* pValue = nullptr;
		if(dwTID == 0) dwTID = ::GetCurrentThreadId();

		{
			CReadLock locallock(m_lock);
			TLocalMapCI it = m_map.find(dwTID);

			if(it != m_map.end())
				pValue = it->second;
		}

		return pValue;
	}

	T* Get()
	{
		DWORD dwTID	= ::GetCurrentThreadId();
		T* pValue	= TryGet(dwTID);

		if(pValue == nullptr)
		{
			CWriteLock locallock(m_lock);
			TLocalMapCI it = m_map.find(dwTID);

			if(it != m_map.end())
				pValue = it->second;

			if(pValue == nullptr)
			{
				pValue		 = Construct();
				m_map[dwTID] = pValue;
			}
		}

		return pValue;
	}

	T& GetRef()
	{
		return *Get();
	}

	T* Get(construct_param_type construct_param)
	{
		DWORD dwTID	= ::GetCurrentThreadId();
		T* pValue	= TryGet(dwTID);

		if(pValue == nullptr)
		{

			CWriteLock locallock(m_lock);
			TLocalMapCI it = m_map.find(dwTID);

			if(it != m_map.end())
				pValue = it->second;

			if(pValue == nullptr)
			{
				pValue		 = ConstructWithParam(construct_param);
				m_map[dwTID] = pValue;
			}
		}

		return pValue;
	}

	T& GetRef(construct_param_type construct_param)
	{
		return *Get(construct_param);
	}

	T* SetAndRetriveOldValue(T* pValue)
	{
		DWORD dwTID  = ::GetCurrentThreadId();
		T* pOldValue = TryGet(dwTID);

		if(pValue != pOldValue)
		{
			CWriteLock locallock(m_lock);
			m_map[dwTID] = pValue;
		}

		return pOldValue;
	}

	void Set(T* pValue)
	{
		T* pOldValue = SetAndRetriveOldValue(pValue);

		if(pValue != pOldValue)
			Delete(pOldValue);
	}

	void Remove()
	{
		DWORD dwTID  = ::GetCurrentThreadId();
		T* pOldValue = TryGet(dwTID);

		Delete(pOldValue);

		{
			CWriteLock locallock(m_lock);
			m_map.erase(dwTID);
		}
	}

	void Clear()
	{
		CWriteLock locallock(m_lock);

		if(!IsEmpty())
		{
			for(TLocalMapCI it = m_map.begin(); it != m_map.end(); ++it)
				Delete(it->second);

			m_map.clear();
		}
	}

	TLocalMap&			GetLocalMap()			{return m_map;}
	const TLocalMap&	GetLocalMap()	const	{return m_map;}

	T*					operator ->	()			{return Get();}
	const T*			operator ->	()	const	{return Get();}
	T&					operator *	()			{return GetRef();}
	const T&			operator *	()	const	{return GetRef();}
	size_t Size			()				const	{return m_map.size();}
	bool IsEmpty		()				const	{return m_map.empty();}

public:
	CTlsObj()
	{

	}

	~CTlsObj()
	{
		Clear();
	}

private:
	static inline void Delete(T* pValue)
	{
		if(pValue != nullptr)
			delete pValue;
	}

	static inline T* Construct()
	{
		return new T;
	}

	static inline T* ConstructWithParam(construct_param_type construct_param)
	{
		return new T(construct_param);
	}

private:
	CSimpleRWLock	m_lock;
	TLocalMap		m_map;

	DECLARE_NO_COPY_CLASS(CTlsObj)
};
