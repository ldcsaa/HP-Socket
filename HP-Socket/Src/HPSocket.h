/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.2.3
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

/******************************************************************************
Module:  HPSocket DLL

Usage:
		方法一：
		--------------------------------------------------------------------------------------
		0. 应用程序包含 SocketInterface.h 和 HPSocket.h 头文件
		1. 调用 HP_Create_Xxx() 函数创建 HPSocket 对象
		2. 使用完毕后调用 HP_Destroy_Xxx() 函数销毁 HPSocket 对象

		方法二：
		--------------------------------------------------------------------------------------
		0. 应用程序包含 SocketInterface.h 和 HPSocket.h 头文件
		1. 创建 CXxxPtr 智能指针，通过智能指针使用 HPSocket 对象

Release:
		1. x86/HPSocket.dll		- (32位/MBCS/Release)
		2. x86/HPSocket_D.dll	- (32位/MBCS/DeBug)
		3. x86/HPSocket_U.dll	- (32位/UNICODE/Release)
		4. x86/HPSocket_UD.dll	- (32位/UNICODE/DeBug)
		5. x64/HPSocket.dll		- (64位/MBCS/Release)
		6. x64/HPSocket_D.dll	- (64位/MBCS/DeBug)
		7. x64/HPSocket_U.dll	- (64位/UNICODE/Release)
		8. x64/HPSocket_UD.dll	- (64位/UNICODE/DeBug)

******************************************************************************/

#pragma once

/**************************************************/
/********* imports / exports HPSocket.dll *********/

#ifdef HPSOCKET_EXPORTS
	#define HPSOCKET_API EXTERN_C __declspec(dllexport)
#else
	#define HPSOCKET_API EXTERN_C __declspec(dllimport)
#endif

#include "SocketInterface.h"

/**************************************************/
/************** HPSocket.dll 导出函数 **************/

// 创建 ITcpServer 对象
HPSOCKET_API ITcpServer* HP_Create_TcpServer(ITcpServerListener* pListener);
// 创建 ITcpClient 对象
HPSOCKET_API ITcpClient* HP_Create_TcpClient(ITcpClientListener* pListener);
// 创建 ITcpAgent 对象
HPSOCKET_API ITcpAgent* HP_Create_TcpAgent(ITcpAgentListener* pListener);
// 创建 ITcpPullServer 对象
HPSOCKET_API ITcpPullServer* HP_Create_TcpPullServer(ITcpServerListener* pListener);
// 创建 ITcpPullClient 对象
HPSOCKET_API ITcpPullClient* HP_Create_TcpPullClient(ITcpClientListener* pListener);
// 创建 ITcpPullAgent 对象
HPSOCKET_API ITcpPullAgent* HP_Create_TcpPullAgent(ITcpAgentListener* pListener);
// 创建 IUdpServer 对象
HPSOCKET_API IUdpServer* HP_Create_UdpServer(IUdpServerListener* pListener);
// 创建 IUdpClient 对象
HPSOCKET_API IUdpClient* HP_Create_UdpClient(IUdpClientListener* pListener);

// 销毁 ITcpServer 对象
HPSOCKET_API void HP_Destroy_TcpServer(ITcpServer* pServer);
// 销毁 ITcpClient 对象
HPSOCKET_API void HP_Destroy_TcpClient(ITcpClient* pClient);
// 销毁 ITcpAgent 对象
HPSOCKET_API void HP_Destroy_TcpAgent(ITcpAgent* pAgent);
// 销毁 ITcpPullServer 对象
HPSOCKET_API void HP_Destroy_TcpPullServer(ITcpPullServer* pServer);
// 销毁 ITcpPullClient 对象
HPSOCKET_API void HP_Destroy_TcpPullClient(ITcpPullClient* pClient);
// 销毁 ITcpPullAgent 对象
HPSOCKET_API void HP_Destroy_TcpPullAgent(ITcpPullAgent* pAgent);
// 销毁 IUdpServer 对象
HPSOCKET_API void HP_Destroy_UdpServer(IUdpServer* pServer);
// 销毁 IUdpClient 对象
HPSOCKET_API void HP_Destroy_UdpClient(IUdpClient* pClient);

// 获取错误描述文本
HPSOCKET_API LPCTSTR HP_GetSocketErrorDesc(EnSocketError enCode);
// 调用系统的 ::GetLastError() 方法获取系统错误代码
HPSOCKET_API DWORD SYS_GetLastError	();
// 调用系统的 ::WSAGetLastError() 方法获取通信错误代码
HPSOCKET_API int SYS_WSAGetLastError();
// 调用系统的 setsockopt()
HPSOCKET_API int SYS_SetSocketOption(SOCKET sock, int level, int name, LPVOID val, int len);
// 调用系统的 getsockopt()
HPSOCKET_API int SYS_GetSocketOption(SOCKET sock, int level, int name, LPVOID val, int* len);
// 调用系统的 ioctlsocket()
HPSOCKET_API int SYS_IoctlSocket(SOCKET sock, long cmd, u_long* arg);
// 调用系统的 ::WSAIoctl()
HPSOCKET_API int SYS_WSAIoctl(SOCKET sock, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer, LPDWORD lpcbBytesReturned);

// ITcpServer 对象创建器
struct TcpServer_Creator
{
	static ITcpServer* Create(ITcpServerListener* pListener)
	{
		return HP_Create_TcpServer(pListener);
	}

	static void Destroy(ITcpServer* pServer)
	{
		HP_Destroy_TcpServer(pServer);
	}
};

// ITcpClient 对象创建器
struct TcpClient_Creator
{
	static ITcpClient* Create(ITcpClientListener* pListener)
	{
		return HP_Create_TcpClient(pListener);
	}

	static void Destroy(ITcpClient* pClient)
	{
		HP_Destroy_TcpClient(pClient);
	}
};

// ITcpAgent 对象创建器
struct TcpAgent_Creator
{
	static ITcpAgent* Create(ITcpAgentListener* pListener)
	{
		return HP_Create_TcpAgent(pListener);
	}

	static void Destroy(ITcpAgent* pAgent)
	{
		HP_Destroy_TcpAgent(pAgent);
	}
};

// ITcpPullServer 对象创建器
struct TcpPullServer_Creator
{
	static ITcpPullServer* Create(ITcpServerListener* pListener)
	{
		return HP_Create_TcpPullServer(pListener);
	}

	static void Destroy(ITcpPullServer* pServer)
	{
		HP_Destroy_TcpPullServer(pServer);
	}
};

// ITcpPullClient 对象创建器
struct TcpPullClient_Creator
{
	static ITcpPullClient* Create(ITcpClientListener* pListener)
	{
		return HP_Create_TcpPullClient(pListener);
	}

	static void Destroy(ITcpPullClient* pClient)
	{
		HP_Destroy_TcpPullClient(pClient);
	}
};

// ITcpPullAgent 对象创建器
struct TcpPullAgent_Creator
{
	static ITcpPullAgent* Create(ITcpAgentListener* pListener)
	{
		return HP_Create_TcpPullAgent(pListener);
	}

	static void Destroy(ITcpPullAgent* pAgent)
	{
		HP_Destroy_TcpPullAgent(pAgent);
	}
};

// IUdpServer 对象创建器
struct UdpServer_Creator
{
	static IUdpServer* Create(IUdpServerListener* pListener)
	{
		return HP_Create_UdpServer(pListener);
	}

	static void Destroy(IUdpServer* pServer)
	{
		HP_Destroy_UdpServer(pServer);
	}
};

// IUdpClient 对象创建器
struct UdpClient_Creator
{
	static IUdpClient* Create(IUdpClientListener* pListener)
	{
		return HP_Create_UdpClient(pListener);
	}

	static void Destroy(IUdpClient* pClient)
	{
		HP_Destroy_UdpClient(pClient);
	}
};

/**************************************************/
/************** HPSocket 对象智能指针 **************/

template<class T, class _Listener, class _Creator> class CHPSocketPtr
{
public:
	CHPSocketPtr(_Listener* pListener = nullptr)
	{
		if(pListener)
			m_pObj = _Creator::Create(pListener);
		else
			m_pObj = nullptr;
	}

	~CHPSocketPtr()
	{
		Reset();
	}

public:
	CHPSocketPtr&  Reset(T* pObj = nullptr)
	{
		if(pObj != m_pObj)
		{
			if(m_pObj)
				_Creator::Destroy(m_pObj);

			m_pObj = pObj;
		}

		return *this;
	}

	CHPSocketPtr& Attach(T* pObj)
	{
		return Reset(pObj);
	}

	T* Detach()
	{
		T* pObj	= m_pObj;
		m_pObj	= nullptr;

		return pObj;
	}

	BOOL IsValid	()	const	{return m_pObj != nullptr	;}
	T* Get			()	const	{return m_pObj				;}
	T* operator ->	()	const	{return m_pObj				;}
	operator T*		()	const	{return m_pObj				;}

	CHPSocketPtr& operator = (T* pObj)	{return Reset(pObj)	;}

private:
	CHPSocketPtr(const CHPSocketPtr&);
	CHPSocketPtr& operator = (const CHPSocketPtr&);

private:
	T* m_pObj;
};

// ITcpServer 对象智能指针
typedef CHPSocketPtr<ITcpServer, ITcpServerListener, TcpServer_Creator>			CTcpServerPtr;
// ITcpClient 对象智能指针
typedef CHPSocketPtr<ITcpClient, ITcpClientListener, TcpClient_Creator>			CTcpClientPtr;
// ITcpAgent 对象智能指针
typedef CHPSocketPtr<ITcpAgent, ITcpAgentListener, TcpAgent_Creator>			CTcpAgentPtr;
// ITcpPullServer 对象智能指针
typedef CHPSocketPtr<ITcpPullServer, ITcpServerListener, TcpPullServer_Creator>	CTcpPullServerPtr;
// ITcpPullClient 对象智能指针
typedef CHPSocketPtr<ITcpPullClient, ITcpClientListener, TcpPullClient_Creator>	CTcpPullClientPtr;
// ITcpPullAgent 对象智能指针
typedef CHPSocketPtr<ITcpPullAgent, ITcpAgentListener, TcpPullAgent_Creator>	CTcpPullAgentPtr;
// IUdpServer 对象智能指针
typedef CHPSocketPtr<IUdpServer, IUdpServerListener, UdpServer_Creator>			CUdpServerPtr;
// IUdpClient 对象智能指针
typedef CHPSocketPtr<IUdpClient, IUdpClientListener, UdpClient_Creator>			CUdpClientPtr;
