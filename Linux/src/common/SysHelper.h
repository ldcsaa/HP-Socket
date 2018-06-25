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

#include <unistd.h>
#include <sched.h>
#include <pthread.h>

#include <sys/syscall.h>
#include <sys/sysinfo.h>

using namespace std;

/* 最大工作线程数 */
#define MAX_WORKER_THREAD_COUNT			500

#define SysGetSystemConfig				sysconf
#define SysGetSystemInfo				sysinfo
#if !defined(__ANDROID__)
	#define SysGetNumberOfProcessors	get_nprocs
#else
	#define SysGetNumberOfProcessors()	sysconf(_SC_NPROCESSORS_ONLN)
#endif
#define SysGetPageSize					getpagesize
#define DEFAULT_BUFFER_SIZE				GetDefaultBufferSize()

#define GetCurrentProcessId				getpid
#define SELF_PROCESS_ID					(::GetCurrentProcessId())
#define gettid()						syscall(__NR_gettid)
#define GetCurrentNativeThreadId()		gettid()
#define SELF_NATIVE_THREAD_ID			(::GetCurrentNativeThreadId())
#define GetCurrentThreadId				pthread_self
#define SELF_THREAD_ID					(::GetCurrentThreadId())
#define IsSameThread(tid1, tid2)		pthread_equal((tid1), (tid2))
#define IsSelfThread(tid)				IsSameThread((tid), SELF_THREAD_ID)
inline BOOL IsSameNativeThread(pid_t pid1, pid_t pid2)
										{return (pid1 == pid2);}
#define IsSelfNativeThread(pid)			IsSameNativeThread((pid), SELF_PROCESS_ID)
#define YieldProcessor					sched_yield
#define DEFAULT_WORKER_THREAD_COUNT		GetDefaultWorkerThreadCount()

DWORD GetDefaultBufferSize();
DWORD GetDefaultWorkerThreadCount();
