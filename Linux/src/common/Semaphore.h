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

#include "GlobalDef.h"
#include "CriSec.h"

#include <condition_variable>

using namespace std;

class CSEM
{
public:
	void Wait()
	{
		CMutexLock2 lock(m_mtx);

		m_cv.wait(lock);
	}

	template<typename _Predicate>
	void Wait(_Predicate p)
	{
		CMutexLock2 lock(m_mtx);

		m_cv.wait(lock, p);
	}
	
	template<typename _Rep, typename _Period>
	cv_status WaitFor(const chrono::duration<_Rep, _Period>& t)
	{
		CMutexLock2 lock(m_mtx);

		return m_cv.wait_for(lock, t);
	}

	cv_status WaitFor(DWORD dwMilliseconds)
	{
		return WaitFor(chrono::milliseconds(dwMilliseconds));
	}

	template<typename _Rep, typename _Period, typename _Predicate>
	bool WaitFor(const chrono::duration<_Rep, _Period>& t, _Predicate p)
	{
		CMutexLock2 lock(m_mtx);

		return m_cv.wait_for(lock, t, p);
	}

	template<typename _Predicate>
	bool WaitFor(DWORD dwMilliseconds, _Predicate p)
	{
		if(IS_INFINITE(dwMilliseconds))
		{
			Wait(p);
			return TRUE;
		}

		return WaitFor(chrono::milliseconds(dwMilliseconds), p);

	}

	void NotifyOne()
	{
		m_cv.notify_one();
	}

	void NotifyAll()
	{
		m_cv.notify_all();
	}

	void SyncNotifyOne()
	{
		CMutexLock2 lock(m_mtx);

		NotifyOne();
	}

	void SyncNotifyAll()
	{
		CMutexLock2 lock(m_mtx);

		NotifyAll();
	}

private:
	CMTX				m_mtx;
	condition_variable	m_cv;

	DECLARE_NO_COPY_CLASS(CSEM)
	DECLARE_PUBLIC_DEFAULT_CONSTRUCTOR(CSEM)
};

using CCVLock = CSEM;
