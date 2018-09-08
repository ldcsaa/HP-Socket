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

#pragma once

#include <process.h>
#include "RWLock.h"
#include "STLHelper.h"
#include "FuncHelper.h"

template<class T, class P = VOID> class CThread
{
private:
	typedef UINT (T::*F)(P*);

	struct TWorker
	{
		T*	m_pRunner;
		F	m_pFunc;
		P*	m_pArg;

	public:
		static TWorker* Construct(T* pRunner, F pFunc, P* pArg)
		{
			return new TWorker(pRunner, pFunc, pArg);
		}

		static void Destruct(TWorker* pWorker)
		{
			if(pWorker != nullptr)
				delete pWorker;
		}

		void CopyTo(TWorker& worker)
		{
			memcpy(&worker, this, sizeof(TWorker));
		}

		TWorker(T* pRunner = nullptr, F pFunc = nullptr, P* pArg = nullptr)
		: m_pRunner(pRunner), m_pFunc(pFunc), m_pArg(pArg)
		{

		}
	};

public:
	BOOL Start(T* pRunner, F pFunc, P* pArg = nullptr, int iPriority = THREAD_PRIORITY_NORMAL, UINT uiStackSize = 0, LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr)
	{
		BOOL isOK = TRUE;

		if(!IsRunning())
		{
			Release();

			unique_ptr<TWorker> wkPtr(TWorker::Construct(pRunner, pFunc, pArg));
			wkPtr->CopyTo(m_Worker);

			if(iPriority == THREAD_PRIORITY_NORMAL)
				m_hThread = (HANDLE)_beginthreadex(lpThreadAttributes, uiStackSize, ThreadProc, (LPVOID)wkPtr.get(), 0, &m_uiThreadID);
			else
			{
				m_hThread = (HANDLE)_beginthreadex(lpThreadAttributes, uiStackSize, ThreadProc, (LPVOID)wkPtr.get(), CREATE_SUSPENDED, &m_uiThreadID);

				if(IsValid())
				{
					::SetThreadPriority(m_hThread, iPriority);
					::ResumeThread(m_hThread);
				}
			}

			if(IsValid())
				wkPtr.release();
			else
			{
				::SetLastError(ERROR_CREATE_FAILED);
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

	HANDLE Detach()
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
	T* GetRunner		()	{return m_Worker.m_pRunner;}
	F GetFunc			()	{return m_Worker.m_pFunc;}
	P* GetArg			()	{return m_Worker.m_pArg;}
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
		TWorker* pWorker = (TWorker*)pv;

		UINT rs = ((pWorker->m_pRunner)->*(pWorker->m_pFunc))(pWorker->m_pArg);

		TWorker::Destruct(pWorker);

		return rs;
	}

	void Reset()
	{
		m_uiThreadID = 0;
		m_hThread	 = nullptr;

		::ZeroObject(m_Worker);
	}

private:
	UINT	m_uiThreadID;
	HANDLE	m_hThread;

	TWorker m_Worker;

	DECLARE_NO_COPY_CLASS(CThread)
};

template<class T, typename construct_param_type = void*> class CTlsObj
{
	typedef unordered_map<DWORD, T*>			TLocalMap;
	typedef typename TLocalMap::iterator		TLocalMapI;
	typedef typename TLocalMap::const_iterator	TLocalMapCI;

public:
	T* TryGet()
	{
		T* pValue = nullptr;

		{
			CReadLock locallock(m_lock);
			TLocalMapCI it = m_map.find(::GetCurrentThreadId());

			if(it != m_map.end())
				pValue = it->second;
		}

		return pValue;
	}

	T* Get()
	{
		T* pValue = TryGet();

		if(pValue == nullptr)
		{
			pValue = Construct();

			CWriteLock locallock(m_lock);
			m_map[::GetCurrentThreadId()] = pValue;
		}

		return pValue;
	}

	T& GetRef()
	{
		return *Get();
	}

	T* Get(construct_param_type& construct_param)
	{
		T* pValue = TryGet();

		if(pValue == nullptr)
		{
			pValue = ConstructWithParam(construct_param);

			CWriteLock locallock(m_lock);
			m_map[::GetCurrentThreadId()] = pValue;
		}

		return pValue;
	}

	T& GetRef(construct_param_type& construct_param)
	{
		return *Get(construct_param);
	}

	T* SetNewAndGetOld(T* pValue)
	{
		T* pOldValue = TryGet();

		if(pValue != pOldValue)
		{
			if(pValue == nullptr)
				DoRemove();
			else
			{
				CWriteLock locallock(m_lock);
				m_map[::GetCurrentThreadId()] = pValue;
			}
		}

		return pOldValue;
	}

	void Set(T* pValue)
	{
		T* pOldValue = SetNewAndGetOld(pValue);

		if(pValue != pOldValue)
			DoDelete(pOldValue);
	}

	void Remove()
	{
		T* pValue = TryGet();

		if(pValue != nullptr)
		{
			DoDelete(pValue);
			DoRemove();
		}
	}

	void Clear()
	{
		CWriteLock locallock(m_lock);

		if(!IsEmpty())
		{
			for(TLocalMapCI it = m_map.begin(), end = m_map.end(); it != end; ++it)
				DoDelete(it->second);

			m_map.clear();
		}
	}

	TLocalMap&			GetLocalMap()			{return m_map;}
	const TLocalMap&	GetLocalMap()	const	{return m_map;}

	CTlsObj&	operator = (T* p)		{Set(p); return *this;}
	T*			operator ->	()			{return Get();}
	const T*	operator ->	()	const	{return Get();}
	T&			operator *	()			{return GetRef();}
	const T&	operator *	()	const	{return GetRef();}
	size_t Size	()				const	{return m_map.size();}
	bool IsEmpty()				const	{return m_map.empty();}

public:
	CTlsObj()
	{

	}

	CTlsObj(T* pValue)
	{
		Set(pValue);
	}

	~CTlsObj()
	{
		Clear();
	}

private:
	inline void DoRemove()
	{
		CWriteLock locallock(m_lock);
		m_map.erase(::GetCurrentThreadId());
	}

	static inline void DoDelete(T* pValue)
	{
		if(pValue != nullptr)
			delete pValue;
	}

	static inline T* Construct()
	{
		return new T;
	}

	static inline T* ConstructWithParam(construct_param_type& construct_param)
	{
		return new T(construct_param);
	}

private:
	CSimpleRWLock	m_lock;
	TLocalMap		m_map;

	DECLARE_NO_COPY_CLASS(CTlsObj)
};

template<class T> class CTlsSimple
{
	typedef unordered_map<DWORD, T>				TLocalMap;
	typedef typename TLocalMap::iterator		TLocalMapI;
	typedef typename TLocalMap::const_iterator	TLocalMapCI;

	static const T DEFAULT = (T)(0);

public:
	BOOL TryGet(T& tValue)
	{
		BOOL isOK = FALSE;

		{
			CReadLock locallock(m_lock);
			TLocalMapCI it = m_map.find(::GetCurrentThreadId());

			if(it != m_map.end())
			{
				tValue	= it->second;
				isOK	= TRUE;
			}
		}

		return isOK;
	}

	T Get(T tDefault = DEFAULT)
	{
		T tValue;

		if(TryGet(tValue))
			return tValue;

		Set(tDefault);

		return tDefault;
	}

	T SetNewAndGetOld(T tValue)
	{
		T tOldValue;

		if(!TryGet(tOldValue))
			tOldValue = DEFAULT;
		else if(tValue != tOldValue)
			Set(tValue);

		return tOldValue;
	}

	void Set(T tValue)
	{
		CWriteLock locallock(m_lock);
		m_map[::GetCurrentThreadId()] = tValue;
	}

	void Remove()
	{
		T tValue;

		if(TryGet(tValue))
		{
			CWriteLock locallock(m_lock);
			m_map.erase(::GetCurrentThreadId());
		}
	}

	void Clear()
	{
		CWriteLock locallock(m_lock);

		if(!IsEmpty())
			m_map.clear();
	}

	TLocalMap&			GetLocalMap()			{return m_map;}
	const TLocalMap&	GetLocalMap()	const	{return m_map;}

	CTlsSimple& operator = (T t) {Set(t); return *this;}
	BOOL operator		== (T t) {return Get() == t;}
	BOOL operator		!= (T t) {return Get() != t;}
	BOOL operator		>= (T t) {return Get() >= t;}
	BOOL operator		<= (T t) {return Get() <= t;}
	BOOL operator		 > (T t) {return Get() > t;}
	BOOL operator		 < (T t) {return Get() < t;}

	size_t Size	()	const	{return m_map.size();}
	bool IsEmpty()	const	{return m_map.empty();}

public:
	CTlsSimple()
	{

	}

	CTlsSimple(T tValue)
	{
		Set(tValue);
	}

	~CTlsSimple()
	{
		Clear();
	}

	DECLARE_NO_COPY_CLASS(CTlsSimple)

private:
	CSimpleRWLock	m_lock;
	TLocalMap		m_map;
};
