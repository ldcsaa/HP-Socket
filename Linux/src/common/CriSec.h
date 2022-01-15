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

#include "../../include/hpsocket/GlobalDef.h"
#include "Singleton.h"
#include "FuncHelper.h"

#include <mutex>
#include <atomic>

using namespace std;

class CSpinGuard
{
public:
	CSpinGuard() : m_atFlag(FALSE)
	{

	}

	~CSpinGuard()
	{
		ASSERT(!m_atFlag);
	}

	void Lock(BOOL bWeek = TRUE, memory_order m = memory_order_acquire)
	{
		for(UINT i = 0; !TryLock(bWeek, m); ++i)
			YieldThread(i);
	}

	BOOL TryLock(BOOL bWeek = FALSE, memory_order m = memory_order_acquire)
	{
		BOOL bExpect = FALSE;

		return bWeek
			? m_atFlag.compare_exchange_weak(bExpect, TRUE, m)
			: m_atFlag.compare_exchange_strong(bExpect, TRUE, m);
	}

	void Unlock(memory_order m = memory_order_release)
	{
		ASSERT(m_atFlag);
		m_atFlag.store(FALSE, m);
	}

	DECLARE_NO_COPY_CLASS(CSpinGuard)

private:
	atomic<BOOL>	m_atFlag;
};

class CReentrantSpinGuard
{
public:
	CReentrantSpinGuard()
	: m_atThreadID	(0)
	, m_iCount		(0)
	{

	}

	~CReentrantSpinGuard()
	{
		ASSERT(m_atThreadID	== 0);
		ASSERT(m_iCount		== 0);
	}

	void Lock(BOOL bWeek = TRUE, memory_order m = memory_order_acquire)
	{
		for(UINT i = 0; !_TryLock(i == 0, bWeek, m); ++i)
			YieldThread(i);
	}

	BOOL TryLock(BOOL bWeek = FALSE, memory_order m = memory_order_acquire)
	{
		return _TryLock(TRUE, bWeek, m);
	}

	void Unlock(memory_order m = memory_order_release)
	{
		ASSERT(::IsSelfThread(m_atThreadID));

		if((--m_iCount) == 0)
			m_atThreadID.store(0, m);
	}

private:
	BOOL _TryLock(BOOL bFirst, BOOL bWeek = FALSE, memory_order m = memory_order_acquire)
	{
		THR_ID dwCurrentThreadID = SELF_THREAD_ID;

		if(bFirst && ::IsSameThread(m_atThreadID, dwCurrentThreadID))
		{
			++m_iCount;
			return TRUE;
		}

		THR_ID ulExpect = 0;

		BOOL isOK = bWeek
			? m_atThreadID.compare_exchange_weak(ulExpect, dwCurrentThreadID, m)
			: m_atThreadID.compare_exchange_strong(ulExpect, dwCurrentThreadID, m);


		if(isOK)
		{
			ASSERT(m_iCount == 0);

			m_iCount = 1;

			return TRUE;
		}

		return FALSE;
	}

	DECLARE_NO_COPY_CLASS(CReentrantSpinGuard)

private:
	atomic_tid	m_atThreadID;
	int			m_iCount;
};

class CFakeGuard
{
public:
	void Lock()		{}
	void Unlock()	{}
	BOOL TryLock()	{return TRUE;}
};

template<class CLockObj> class CLocalLock
{
public:
	CLocalLock(CLockObj& obj) : m_lock(obj) {m_lock.Lock();}
	~CLocalLock() {m_lock.Unlock();}
private:
	CLockObj& m_lock;
};

template<class CLockObj> class CLocalTryLock
{
public:
	CLocalTryLock(CLockObj& obj) : m_lock(obj) {m_bValid = m_lock.TryLock();}
	~CLocalTryLock() {if(m_bValid) m_lock.Unlock();}

	BOOL IsValid() {return m_bValid;}

private:
	CLockObj&	m_lock;
	BOOL		m_bValid;
};

template<class CMTXObj> class CMTXTryLock
{
public:
	CMTXTryLock(CMTXObj& obj) : m_lock(obj) {m_bValid = m_lock.try_lock();}
	~CMTXTryLock() {if(m_bValid) m_lock.unlock();}

	BOOL IsValid() {return m_bValid;}

private:
	CMTXObj&	m_lock;
	BOOL		m_bValid;
};

using CSpinLock					= CLocalLock<CSpinGuard>;
using CReentrantSpinLock		= CLocalLock<CReentrantSpinGuard>;
using CFakeLock					= CLocalLock<CFakeGuard>;

using CCriSec					= mutex;
using CCriSecLock				= lock_guard<mutex>;
using CCriSecLock2				= unique_lock<mutex>;
using CCriSecTryLock			= CMTXTryLock<mutex>;

using CMTX						= CCriSec;
using CMutexLock				= CCriSecLock;
using CMutexLock2				= CCriSecLock2;
using CMutexTryLock				= CCriSecTryLock;

using CReentrantCriSec			= recursive_mutex;
using CReentrantCriSecLock		= lock_guard<recursive_mutex>;
using CReentrantCriSecLock2		= unique_lock<recursive_mutex>;
using CReentrantCriSecTryLock	= CMTXTryLock<recursive_mutex>;

using CReentrantMTX				= CReentrantCriSec;
using CReentrantMutexLock		= CReentrantCriSecLock;
using CReentrantMutexLock2		= CReentrantCriSecLock2;
using CReentrantMutexTryLock	= CReentrantCriSecTryLock;

template<typename T, typename = enable_if_t<is_arithmetic<T>::value>> class CSafeCounterT
{
public:
	T Increment()				{return ::InterlockedIncrement(&m_iCount);}
	T Decrement()				{return ::InterlockedDecrement(&m_iCount);}
	T AddFetch(T iCount)		{return ::InterlockedAdd(&m_iCount, iCount);}
	T SubFetch(T iCount)		{return ::InterlockedSub(&m_iCount, iCount);}
	T FetchAdd(T iCount)		{return ::InterlockedExchangeAdd(&m_iCount, iCount);}
	T FetchSub(T iCount)		{return ::InterlockedExchangeSub(&m_iCount, iCount);}

	T SetCount(T iCount)		{return (m_iCount = iCount);}
	T ResetCount()				{return SetCount(0);}
	T GetCount()				{return m_iCount;}

	T operator ++ ()			{return Increment();}
	T operator -- ()			{return Decrement();}
	T operator ++ (int)			{return FetchAdd(1);}
	T operator -- (int)			{return FetchSub(1);}
	T operator += (T iCount)	{return AddFetch(iCount);}
	T operator -= (T iCount)	{return SubFetch(iCount);}
	T operator  = (T iCount)	{return SetCount(iCount);}
	operator T	  ()			{return GetCount();}

public:
	CSafeCounterT(T iCount = 0) : m_iCount(iCount) {}

protected:
	volatile T m_iCount;
};

template<typename T, typename = enable_if_t<is_arithmetic<T>::value>> class CUnsafeCounterT
{
public:
	T Increment()				{return ++m_iCount;}
	T Decrement()				{return --m_iCount;}
	T AddFetch(T iCount)		{return m_iCount += iCount;}
	T SubFetch(T iCount)		{return m_iCount -= iCount;}
	T FetchAdd(T iCount)		{T rs = m_iCount; m_iCount += iCount; return rs;}
	T FetchSub(T iCount)		{T rs = m_iCount; m_iCount -= iCount; return rs;}

	T SetCount(T iCount)		{return (m_iCount = iCount);}
	T ResetCount()				{return SetCount(0);}
	T GetCount()				{return m_iCount;}

	T operator ++ ()			{return Increment();}
	T operator -- ()			{return Decrement();}
	T operator ++ (int)			{return FetchAdd(1);}
	T operator -- (int)			{return FetchSub(1);}
	T operator += (T iCount)	{return AddFetch(iCount);}
	T operator -= (T iCount)	{return SubFetch(iCount);}
	T operator  = (T iCount)	{return SetCount(iCount);}
	operator T	  ()			{return GetCount();}

public:
	CUnsafeCounterT(T iCount = 0) : m_iCount(iCount) {}

protected:
	T m_iCount;
};

template<class CCounter> class CLocalCounter
{
public:
	CLocalCounter(CCounter& obj) : m_counter(obj) {m_counter.Increment();}
	~CLocalCounter() {m_counter.Decrement();}
private:
	CCounter& m_counter;
};

using CSafeCounter				= CSafeCounterT<INT>;
using CSafeBigCounter			= CSafeCounterT<LONGLONG>;
using CUnsafeCounter			= CUnsafeCounterT<INT>;
using CUnsafeBigCounter			= CUnsafeCounterT<LONGLONG>;

using CLocalSafeCounter			= CLocalCounter<CSafeCounter>;
using CLocalSafeBigCounter		= CLocalCounter<CSafeBigCounter>;
using CLocalUnsafeCounter		= CLocalCounter<CUnsafeCounter>;
using CLocalUnsafeBigCounter	= CLocalCounter<CUnsafeBigCounter>;
