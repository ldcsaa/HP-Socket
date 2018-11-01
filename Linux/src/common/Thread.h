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

#include "GlobalDef.h"
#include "GlobalErrno.h"
#include "RWLock.h"
#include "STLHelper.h"

#include <pthread.h>
#include <signal.h>
#include <utility>

using namespace std;

/* Used to retry syscalls that can return EINTR. */
#define NO_EINTR_EXCEPT_THR_INTR(exp) ({							\
	long int _rc;													\
	do {_rc = (long int)(exp);}										\
	while (IS_HAS_ERROR(_rc) && IS_INTR_ERROR()						\
			&& !::IsThreadInterrupted());							\
    _rc; })

#define NO_EINTR_EXCEPT_THR_INTR_INT(exp)	((int)NO_EINTR_EXCEPT_THR_INTR(exp))

class __CThread_Interrupt_
{
public:
	static const int SIG_NO = (_NSIG - 5);

private:
	friend BOOL IsThreadInterrupted();
	template<typename T, typename P, typename R> friend class CThread;

private:
	static BOOL IsInterrupted();
	static BOOL InitSigAction();
	static void SignalHandler(int sig);

private:
	~__CThread_Interrupt_();

private:
	static BOOL sm_bInitFlag;
};

inline BOOL IsThreadInterrupted() {return __CThread_Interrupt_::IsInterrupted();}

class __CFakeRunnerClass_ {};

template<class T, class P = VOID, class R = UINT_PTR> class CThread
{
public:
	using F	 = R (T::*)(P*);
	using SF = R (*)(P*);

	struct TWorker
	{
		CThread* m_pThread;
		BOOL	 m_bDetach;

		T*	m_pRunner;
		F	m_pFunc;
		P*	m_pArg;

	public:
		static TWorker* Construct(CThread* pThread, T* pRunner, F pFunc, P* pArg)
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

		TWorker(CThread* pThread = nullptr, BOOL bDetach = FALSE, T* pRunner = nullptr, F pFunc = nullptr, P* pArg = nullptr)
		: m_pThread(pThread), m_bDetach(bDetach), m_pRunner(pRunner), m_pFunc(pFunc), m_pArg(pArg)
		{

		}

	public:
		template<typename T_, typename R_, typename = enable_if_t<!is_same<T_, __CFakeRunnerClass_>::value && !is_void<R_>::value>>
		PVOID Run(T_*, R_*)
		{
			return (PVOID)(UINT_PTR)((m_pRunner->*m_pFunc)(m_pArg));
		}

		template<typename T_, typename = enable_if_t<!is_same<T_, __CFakeRunnerClass_>::value>>
		PVOID Run(T_*, PVOID)
		{
			(m_pRunner->*m_pFunc)(m_pArg);

			return nullptr;
		}

		template<typename R_, typename = enable_if_t<!is_void<R_>::value>>
		PVOID Run(__CFakeRunnerClass_*, R_*)
		{
			return (PVOID)(UINT_PTR)(*(SF*)&m_pFunc)(m_pArg);
		}

		PVOID Run(__CFakeRunnerClass_*, VOID*)
		{
			(*(SF*)&m_pFunc)(m_pArg);

			return nullptr;
		}
	};

	friend class TWorker;

public:

	BOOL Start(SF pFunc, P* pArg = nullptr, BOOL bDetach = FALSE, const pthread_attr_t* pAttr = nullptr)
	{
		return Start((__CFakeRunnerClass_*)nullptr, *(F*)&pFunc, pArg, bDetach, pAttr);
	}

	BOOL Start(T* pRunner, F pFunc, P* pArg = nullptr, BOOL bDetach = FALSE, const pthread_attr_t* pAttr = nullptr)
	{
		int rs = ERROR_INVALID_STATE;

		if(IsRunning())
			::SetLastError(rs);
		else
		{
			unique_ptr<TWorker> wkPtr = make_unique<TWorker>(this, bDetach, pRunner, pFunc, pArg);
			wkPtr->CopyTo(m_Worker);

			SetRunning(TRUE);

			rs = pthread_create(&m_ulThreadID, pAttr, ThreadProc, (PVOID)wkPtr.get());

			if(rs == NO_ERROR)
				wkPtr.release();
			else
			{
				Reset();
				::SetLastError(rs);
			}
		}

		return (rs == NO_ERROR);
	}

#if !defined(__ANDROID__)

	BOOL Cancel()
	{
		int rs = NO_ERROR;

		if(!IsRunning() || ::IsSelfThread(m_ulThreadID))
			rs = ERROR_INVALID_STATE;
		else
			rs = pthread_cancel(m_ulThreadID);

		if(rs != NO_ERROR)
			::SetLastError(rs);

		return (rs == NO_ERROR);
	}

	BOOL Join(R* pResult = nullptr, BOOL bWait = TRUE, LONG lWaitMillsec = INFINITE)
	{
		int rs = NO_ERROR;

		if(!IsRunning() || ::IsSelfThread(m_ulThreadID))
			rs = ERROR_INVALID_STATE;
		else
		{
			if(!bWait)
				rs = pthread_tryjoin_np(m_ulThreadID, (PVOID*)pResult);
			else if(IS_INFINITE(lWaitMillsec))
				rs = pthread_join(m_ulThreadID, (PVOID*)pResult);
			else
			{
				timespec ts;
				::GetFutureTimespec(lWaitMillsec, ts, CLOCK_REALTIME);

				rs = pthread_timedjoin_np(m_ulThreadID, (PVOID*)pResult, &ts);
			}
		}

		if(rs == NO_ERROR)
			SetRunning(FALSE);
		else
			::SetLastError(rs);

		return (rs == NO_ERROR);
	}

#else

	BOOL Cancel()
	{
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
		return FALSE;
	}

	BOOL Join(R* pResult = nullptr)
	{
		int rs = NO_ERROR;

		if(!IsRunning() || ::IsSelfThread(m_ulThreadID))
			rs = ERROR_INVALID_STATE;
		else
			rs = pthread_join(m_ulThreadID, (PVOID*)pResult);

		if(rs == NO_ERROR)
			SetRunning(FALSE);
		else
			::SetLastError(rs);

		return (rs == NO_ERROR);
	}

#endif

	BOOL Detach()
	{
		int rs = NO_ERROR;

		if(!IsRunning())
			rs = ERROR_INVALID_STATE;
		else
			rs = pthread_detach(m_ulThreadID);

		if(rs == NO_ERROR)
			Reset();
		else
			::SetLastError(rs);

		return (rs == NO_ERROR);
	}

	void Reset()
	{
		SetRunning(FALSE);

		m_ulThreadID	= 0;
		m_lNativeID		= 0;

		::ZeroObject(m_Worker);
	}

	BOOL Interrupt()
	{
		if(!IsRunning())
		{
			SetLastError(ERROR_INVALID_STATE);
			return FALSE;
		}

		return (IS_NO_ERROR(pthread_kill(m_ulThreadID, __CThread_Interrupt_::SIG_NO)));
	}

	void SetRunning(BOOL bRunning) {m_bRunning = bRunning;}

	BOOL IsRunning		() const {return m_bRunning;}
	T* GetRunner		() const {return m_Worker.m_pRunner;}
	F GetFunc			() const {return m_Worker.m_pFunc;}
	SF GetSFunc			() const {return *(SF*)&m_Worker.m_pFunc;}
	P* GetArg			() const {return m_Worker.m_pArg;}
	THR_ID GetThreadID	() const {return m_ulThreadID;}
	NTHR_ID GetNativeID	() const {return m_lNativeID;}

	BOOL IsInMyThread		()					const {return IsMyThreadID(::GetCurrentThreadId());}
	BOOL IsMyThreadID		(THR_ID ulThreadID)	const {return ::IsSameThread(ulThreadID, m_ulThreadID);}
	BOOL IsMyNativeThreadID	(NTHR_ID lNativeID)	const {return ::IsSameNativeThread(lNativeID, m_lNativeID);}

private:
	static PVOID ThreadProc(LPVOID pv)
	{
		UnmaskInterruptSignal();

		__CThread_Interrupt_ tlsInterrupt;

		TWorker* pWorker = (TWorker*)pv;

		if(pWorker->m_bDetach)
			pWorker->m_pThread->Detach();
		else
			pWorker->m_pThread->m_lNativeID = SELF_NATIVE_THREAD_ID;

		PVOID pResult = pWorker->Run((T*)nullptr, (R*)nullptr);

		TWorker::Destruct(pWorker);

		return pResult;
	}

	static void UnmaskInterruptSignal()
	{
		sigset_t ss;

		sigemptyset(&ss);
		sigaddset(&ss, __CThread_Interrupt_::SIG_NO);

		pthread_sigmask(SIG_UNBLOCK, &ss, nullptr);
	}

public:
	CThread()
	{
		Reset();
	}

	virtual ~CThread()
	{
		if(IsRunning())
		{
			Interrupt();
			Join(nullptr);
		}

		ASSERT(!IsRunning());
	}

	DECLARE_NO_COPY_CLASS(CThread)

private:
	THR_ID	m_ulThreadID;
	NTHR_ID	m_lNativeID;
	BOOL	m_bRunning;

	TWorker	m_Worker;
};

template<class P = VOID, class R = UINT_PTR> using CStaticThread = CThread<__CFakeRunnerClass_, P, R>;

template<class T> class CTlsObj
{
	using TLocalMap = unordered_map<THR_ID, T*>;

public:
	T* TryGet()
	{
		T* pValue = nullptr;

		{
			CReadLock locallock(m_lock);
			auto it = m_map.find(SELF_THREAD_ID);

			if(it != m_map.end())
				pValue = it->second;
		}

		return pValue;
	}

	template<typename  ... _Con_Param> T* Get(_Con_Param&& ... construct_args)
	{
		T* pValue = TryGet();

		if(pValue == nullptr)
		{
			pValue = Construct(forward<_Con_Param>(construct_args) ...);

			CWriteLock locallock(m_lock);
			m_map[SELF_THREAD_ID] = pValue;
		}

		return pValue;
	}

	template<typename  ... _Con_Param> T& GetRef(_Con_Param&& ... construct_args)
	{
		return *Get(forward<_Con_Param>(construct_args) ...);
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
				m_map[SELF_THREAD_ID] = pValue;
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
			for(auto it = m_map.begin(), end = m_map.end(); it != end; ++it)
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

private:
	inline void DoRemove()
	{
		CWriteLock locallock(m_lock);
		m_map.erase(SELF_THREAD_ID);
	}

	static inline void DoDelete(T* pValue)
	{
		if(pValue != nullptr)
			delete pValue;
	}

	template<typename  ... _Con_Param> static inline T* Construct(_Con_Param&& ... construct_args)
	{
		return new T(forward<_Con_Param>(construct_args) ...);
	}

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
	CSimpleRWLock	m_lock;
	TLocalMap		m_map;

	DECLARE_NO_COPY_CLASS(CTlsObj)
};

template<class T> class CTlsSimple
{
	using TLocalMap = unordered_map<THR_ID, T>;

	static const T DEFAULT = (T)(0);

public:
	BOOL TryGet(T& tValue)
	{
		BOOL isOK = FALSE;

		{
			CReadLock locallock(m_lock);
			auto it = m_map.find(SELF_THREAD_ID);

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
		m_map[SELF_THREAD_ID] = tValue;
	}

	void Remove()
	{
		T tValue;

		if(TryGet(tValue))
		{
			CWriteLock locallock(m_lock);
			m_map.erase(SELF_THREAD_ID);
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
