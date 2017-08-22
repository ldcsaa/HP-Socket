/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.20
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
 
/*

Optional Macros:

Windows:
++++++++++++++++++++++
_WIN32_WINNT		: Windows NT �汾	��Ĭ�ϣ�_WIN32_WINNT_WINXP / _WIN32_WINNT_WIN7��
WINVER				: Windows �汾		��Ĭ�ϣ�_WIN32_WINNT��
_USE_MFC			: ʹ�� MFC
_WINSOCK_SUPPORT	: ֧�� Windows Socket
_NO_RIBBONS_SUPPORT	: ��֧�� Ribbons ������
_DETECT_MEMORY_LEAK	: DEBUG ״̬��֧���ڴ�й¶���

Windows CE:
++++++++++++++++++++++
WINVER				: Windows �汾
_USE_MFC			: ʹ�� MFC
_WINSOCK_SUPPORT	: ֧�� Windows Socket
_DETECT_MEMORY_LEAK	: DEBUG ״̬��֧���ڴ�й¶���
_ONLY_DETECT_CONFIRMED_MEMORY_LEAK_	: ֻ�����ܹ�ȷ�ϵ��ڴ�й¶������ȷ���Ĳ����棩
---------------------------
VC 2015
	_MSC_VER == 1900
VC 2013
	_MSC_VER == 1800
VC 2012
	_MSC_VER == 1700
VC 2010
	_MSC_VER == 1600
VC 2008
	_MSC_VER == 1500
VC 2005
	_MSC_VER == 1400
VC 7.1
	_MSC_VER == 1310
VC 7.0
	_MSC_VER == 1300
VC 6.0
	_MSC_VER == 1200
---------------------------
Windows Versions:
_WIN32_WINNT_NT4		x0400
_WIN32_WINNT_WIN2K		0x0500
_WIN32_WINNT_WINXP		0x0501
_WIN32_WINNT_WS03		0x0502
_WIN32_WINNT_WIN6		0x0600
_WIN32_WINNT_VISTA		0x0600
_WIN32_WINNT_WS08		0x0600
_WIN32_WINNT_LONGHORN	0x0600
_WIN32_WINNT_WIN7		0x0601
_WIN32_WINNT_WIN8		0x0602
_WIN32_WINNT_WINBLUE	0x0603
_WIN32_WINNT_WIN10		0x0A00
---------------------------
*/

#pragma once

#ifndef VC_EXTRALEAN
	#define VC_EXTRALEAN
#endif

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
	#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#endif

#if _MSC_VER >= 1400

	#if defined _DEBUG && _MSC_VER < 1600
		#ifndef _SECURE_SCL
			#define _SECURE_SCL					0
		#endif
		#ifndef _HAS_ITERATOR_DEBUGGING
			#define _HAS_ITERATOR_DEBUGGING		0
		#endif
	#endif

	#ifndef _CRT_SECURE_NO_DEPRECATE
		#define _CRT_SECURE_NO_DEPRECATE		1
	#endif

	#ifndef _SCL_SECURE_NO_DEPRECATE
		#define _SCL_SECURE_NO_DEPRECATE		1
	#endif

	#ifndef _ATL_SECURE_NO_WARNINGS
		#define _ATL_SECURE_NO_WARNINGS			1
	#endif

	#ifndef _CRT_NON_CONFORMING_SWPRINTFS
		#define _CRT_NON_CONFORMING_SWPRINTFS	1
	#endif

	#ifndef _SECURE_ATL
		#define _SECURE_ATL						1
	#endif

	#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
		#define _WINSOCK_DEPRECATED_NO_WARNINGS	1
	#endif

#endif

#ifndef _WIN32_WINNT
	#if defined (_WIN64)
		#define _WIN32_WINNT	_WIN32_WINNT_WIN7
	#else
		#define _WIN32_WINNT	_WIN32_WINNT_WINXP
	#endif
#endif

#ifndef WINVER
	#define WINVER	_WIN32_WINNT
#endif

#if _MSC_VER >= 1600
	#include <SDKDDKVer.h>
#else
	#if !defined(nullptr)
		#define nullptr	NULL
	#endif
#endif

#ifdef _DETECT_MEMORY_LEAK
	#ifndef _CRTDBG_MAP_ALLOC
		#define _CRTDBG_MAP_ALLOC
	#endif
#endif

#ifdef _USE_MFC

	#ifndef _AFX_ALL_WARNINGS
		#define _AFX_ALL_WARNINGS
	#endif

	#include <afxwin.h>
	#include <afxext.h>
	#include <afxdisp.h>

	#ifndef _AFX_NO_OLE_SUPPORT
		#include <afxdtctl.h>
	#endif

	#ifndef _AFX_NO_AFXCMN_SUPPORT
		#include <afxcmn.h>
	#endif

	#ifndef _NO_RIBBONS_SUPPORT
		#include <afxcontrolbars.h>
	#endif

#else

	#include <Windows.h>
	#include <WindowsX.h>
	#include <commctrl.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <malloc.h>
	#include <memory.h>
	#include <tchar.h>
	#include <atlstr.h>
	#include <atltime.h>

	#ifndef ASSERT
		#define ASSERT(f)	ATLASSERT(f)
	#endif
	#ifndef VERIFY
		#define VERIFY(f)	ATLVERIFY(f)
	#endif

	#ifndef TRACE
		#include <atltrace.h>

		#define TRACE							AtlTrace
		#define TRACE0(f)						TRACE(f)
		#define TRACE1(f, p1)					TRACE(f, p1)
		#define TRACE2(f, p1, p2)				TRACE(f, p1, p2)
		#define TRACE3(f, p1, p2, p3)			TRACE(f, p1, p2, p3)
		#define TRACE4(f, p1, p2, p3, p4)		TRACE(f, p1, p2, p3, p4)
		#define TRACE5(f, p1, p2, p3, p4, p5)	TRACE(f, p1, p2, p3, p4, p5)
	#endif

#endif

#ifdef _WINSOCK_SUPPORT
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <mswsock.h>
#endif

#include <atlbase.h>
#include <atlconv.h>

#include "Singleton.h"
#include "Event.h"
#include "Semaphore.h"
#include "CriticalSection.h"
#include "STLHelper.h"
#include "Win32Helper.h"
#include "PrivateHeap.h"
#include "bufferptr.h"

#if defined (_DEBUG) && defined (_DETECT_MEMORY_LEAK)
	#include "debug/win32_crtdbg.h"
#endif
