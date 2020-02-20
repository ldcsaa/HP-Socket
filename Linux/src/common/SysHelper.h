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

#include <unistd.h>
#include <sched.h>
#include <pthread.h>

#include <sys/syscall.h>
#include <sys/sysinfo.h>

using namespace std;

/* 最大工作线程数 */
#define MAX_WORKER_THREAD_COUNT			512
/* 默认对象缓存对象锁定时间 */
#define DEFAULT_OBJECT_CACHE_LOCK_TIME	(20 * 1000)
/* 默认对象缓存池大小 */
#define DEFAULT_OBJECT_CACHE_POOL_SIZE	600
/* 默认对象缓存池回收阀值 */
#define DEFAULT_OBJECT_CACHE_POOL_HOLD	600
/* 默认内存块缓存容量 */
#define DEFAULT_BUFFER_CACHE_CAPACITY	4096
/* 默认内存块缓存池大小 */
#define DEFAULT_BUFFER_CACHE_POOL_SIZE	1024
/* 默认内存块缓存池回收阀值 */
#define DEFAULT_BUFFER_CACHE_POOL_HOLD	1024


#define SysGetSystemConfig				sysconf
#define SysGetSystemInfo				sysinfo

#if !defined(__ANDROID__)
	#define SysGetPageSize				getpagesize
	#define SysGetNumberOfProcessors	get_nprocs
#else
	#define SysGetPageSize()			sysconf(_SC_PAGESIZE)
	#define SysGetNumberOfProcessors()	sysconf(_SC_NPROCESSORS_ONLN)
#endif

#define SYS_PAGE_SIZE					GetSysPageSize()

#define PROCESSOR_COUNT					(::SysGetNumberOfProcessors())
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
#define DEFAULT_WORKER_THREAD_COUNT		GetDefaultWorkerThreadCount()

inline void __asm_nop()					{__asm__ __volatile__("nop" : : : "memory");}
inline void __asm_rep_nop()				{__asm__ __volatile__("rep; nop" : : : "memory");}

#if defined(__i386__) || defined(__x86_64__)
inline void __asm_pause()				{__asm__ __volatile__("pause;");}
#elif defined(__arm64__)
inline void __asm_pause()				{__asm__ __volatile__("yield" : : : "memory");}
#else
inline void __asm_pause()				{__asm_nop();}
#endif

#define YieldProcessor					__asm_pause
#define SwitchToThread					sched_yield

DWORD GetSysPageSize();
DWORD GetKernelVersion();
BOOL IsKernelVersionAbove(BYTE major, BYTE minor, BYTE revise);
DWORD GetDefaultWorkerThreadCount();


#if defined(__ANDROID__)

#include<android/api-level.h>

#define pthread_cancel(t)

#if defined(__ANDROID_API__)
#if (__ANDROID_API__ < 21)

	#define ppoll(fd, nfds, ptmspec, sig)							poll((fd), (nfds), ((ptmspec) == nullptr) ? -1 : ((ptmspec)->tv_sec * 1000 + (ptmspec)->tv_nsec / 1000000))
	#define epoll_create1(flag)										epoll_create(32)
	#define epoll_pwait(epfd, events, maxevents, timeout, sigmask)	epoll_wait((epfd), (events), (maxevents), (timeout))

#endif
#endif

#endif
