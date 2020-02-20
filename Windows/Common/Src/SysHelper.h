/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Author	: Bruce Liang
 * Website	: https://github.com/ldcsaa
 * Project	: https://github.com/ldcsaa/HP-Socket/HP-Socket
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

typedef DWORD							TID, THR_ID, NTHR_ID, PID, PRO_ID;

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

#define SYS_PAGE_SIZE					(GetSysPageSize())
#define DEFAULT_WORKER_THREAD_COUNT		(GetDefaultWorkerThreadCount())
#define SELF_PROCESS_ID					(::GetCurrentProcessId())
#define SELF_THREAD_ID					(::GetCurrentThreadId())
#define IsSameThread(tid1, tid2)		((tid1) == (tid2))
#define IsSelfThread(tid)				IsSameThread((tid), SELF_THREAD_ID)
#define IsSameProcess(pid1, pid2)		((pid1) == (pid2))
#define IsSelfProcess(pid)				IsSameProcess((pid), SELF_PROCESS_ID)

DWORD GetSysPageSize();
DWORD GetDefaultWorkerThreadCount();

// 获取系统信息
VOID SysGetSystemInfo(LPSYSTEM_INFO pInfo);
// 获取 CPU 核数
DWORD SysGetNumberOfProcessors();
// 获取页面大小
DWORD SysGetPageSize();
