/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.15
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

template<class T> class CThread
{
private:
	typedef UINT (T::*F)();

public:
	BOOL Start(T* pRunner, F pFunc, int iPriority = THREAD_PRIORITY_NORMAL, UINT uiStackSize = 0, LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr)
	{
		BOOL isOK = TRUE;

		if(!IsRunning())
		{
			Release();

			m_pRunner	= pRunner;
			m_pFunc		= pFunc;

			if(iPriority == THREAD_PRIORITY_NORMAL)
				m_hThread = (HANDLE)_beginthreadex(lpThreadAttributes, uiStackSize, ThreadProc, (LPVOID)this, 0, &m_uiThreadID);
			else
			{
				m_hThread = (HANDLE)_beginthreadex(lpThreadAttributes, uiStackSize, ThreadProc, (LPVOID)this, CREATE_SUSPENDED, &m_uiThreadID);

				if(IsValid())
				{
					::SetThreadPriority(m_hThread, iPriority);
					::ResumeThread(m_hThread);
				}
			}

			if(!IsValid())
			{
				::SetLastError(_doserrno);
				isOK = FALSE;
			}
		}
		else
		{
			::SetLastError(ERROR_INVALID_STATE);
			isOK = FALSE;
		}

		return isOK;
	}

	BOOL Join(BOOL bWithMessageLoop = FALSE, DWORD dwMilliseconds = INFINITE)
	{
		BOOL isOK = bWithMessageLoop											?
			::MsgWaitForSingleObject(m_hThread, dwMilliseconds)					:
			::WaitForSingleObject(m_hThread, dwMilliseconds) == WAIT_OBJECT_0	;

		Release();

		return isOK;
	}

	BOOL IsRunning()
	{
		if(!IsValid())
			return FALSE;

		DWORD dwExitCode;

		if(GetExitCode(&dwExitCode))
			return dwExitCode == STILL_ACTIVE;

		return FALSE;
	}

	VOID Release()
	{
		if(IsValid())
		{
			::CloseHandle(m_hThread);
			Reset();
		}
	}

	HANDLE Detatch()
	{
		HANDLE h = m_hThread;

		Reset();

		return h;
	}

	BOOL Terminate		(DWORD dwExitCode)		{return ::TerminateThread(m_hThread, dwExitCode);}
	BOOL GetExitCode	(LPDWORD lpExitCode)	{return ::GetExitCodeThread(m_hThread, lpExitCode);}
	DWORD Suspend		()						{return ::SuspendThread(m_hThread);}
	DWORD Resume		()						{return ::ResumeThread(m_hThread);}

	BOOL IsValid		()	{return m_hThread != nullptr;}
	T* GetRunner		()	{return m_pRunner;}
	F GetFunc			()	{return m_pFunc;}
	DWORD GetThreadID	()	{return m_uiThreadID;}

	HANDLE& GetThreadHandle			() 			{return m_hThread;}
	const HANDLE& GetThreadHandle	() const	{return m_hThread;}

public:
	CThread()
	{
		Reset();
	}
	
	virtual ~CThread()
	{
		Release();
	}

private:
	static UINT WINAPI ThreadProc(LPVOID pv)
	{
		CThread* pThis = (CThread*)pv;

		return ((pThis->m_pRunner)->*(pThis->m_pFunc))();
	}

	void Reset()
	{
		m_uiThreadID = 0;
		m_hThread	 = nullptr;
		m_pRunner	 = nullptr;
		m_pFunc		 = nullptr;
	}

private:
	UINT	m_uiThreadID;
	HANDLE	m_hThread;
	T*		m_pRunner;
	F		m_pFunc;

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
