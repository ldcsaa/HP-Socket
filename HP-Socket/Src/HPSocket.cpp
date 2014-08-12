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
 
#include "stdafx.h"
#include "HPSocket.h"
#include "TcpServer.h"
#include "TcpClient.h"
#include "TcpPullServer.h"
#include "TcpPullClient.h"
#include "UdpServer.h"
#include "UdpClient.h"
#include "TcpAgent.h"
#include "TcpPullAgent.h"

#if !defined(_WIN64)
	#pragma comment(linker, "/EXPORT:HP_Create_TcpAgent=_HP_Create_TcpAgent")
	#pragma comment(linker, "/EXPORT:HP_Create_TcpPullAgent=_HP_Create_TcpPullAgent")
	#pragma comment(linker, "/EXPORT:HP_Create_TcpClient=_HP_Create_TcpClient")
	#pragma comment(linker, "/EXPORT:HP_Create_TcpPullClient=_HP_Create_TcpPullClient")
	#pragma comment(linker, "/EXPORT:HP_Create_TcpPullServer=_HP_Create_TcpPullServer")
	#pragma comment(linker, "/EXPORT:HP_Create_TcpServer=_HP_Create_TcpServer")
	#pragma comment(linker, "/EXPORT:HP_Create_UdpClient=_HP_Create_UdpClient")
	#pragma comment(linker, "/EXPORT:HP_Create_UdpServer=_HP_Create_UdpServer")
	#pragma comment(linker, "/EXPORT:HP_Destroy_TcpAgent=_HP_Destroy_TcpAgent")
	#pragma comment(linker, "/EXPORT:HP_Destroy_TcpPullAgent=_HP_Destroy_TcpPullAgent")
	#pragma comment(linker, "/EXPORT:HP_Destroy_TcpClient=_HP_Destroy_TcpClient")
	#pragma comment(linker, "/EXPORT:HP_Destroy_TcpPullClient=_HP_Destroy_TcpPullClient")
	#pragma comment(linker, "/EXPORT:HP_Destroy_TcpPullServer=_HP_Destroy_TcpPullServer")
	#pragma comment(linker, "/EXPORT:HP_Destroy_TcpServer=_HP_Destroy_TcpServer")
	#pragma comment(linker, "/EXPORT:HP_Destroy_UdpClient=_HP_Destroy_UdpClient")
	#pragma comment(linker, "/EXPORT:HP_Destroy_UdpServer=_HP_Destroy_UdpServer")
	#pragma comment(linker, "/EXPORT:HP_GetSocketErrorDesc=_HP_GetSocketErrorDesc")
	#pragma comment(linker, "/EXPORT:SYS_GetLastError=_SYS_GetLastError")
	#pragma comment(linker, "/EXPORT:SYS_WSAGetLastError=_SYS_WSAGetLastError")
	#pragma comment(linker, "/EXPORT:SYS_SetSocketOption=_SYS_SetSocketOption")
	#pragma comment(linker, "/EXPORT:SYS_GetSocketOption=_SYS_GetSocketOption")
	#pragma comment(linker, "/EXPORT:SYS_IoctlSocket=_SYS_IoctlSocket")
	#pragma comment(linker, "/EXPORT:SYS_WSAIoctl=_SYS_WSAIoctl")
#endif

HPSOCKET_API ITcpServer* HP_Create_TcpServer(ITcpServerListener* pListener)
{
	return new CTcpServer(pListener);
}

HPSOCKET_API ITcpClient* HP_Create_TcpClient(ITcpClientListener* pListener)
{
	return new CTcpClient(pListener);
}

HPSOCKET_API ITcpAgent* HP_Create_TcpAgent(ITcpAgentListener* pListener)
{
	return new CTcpAgent(pListener);
}

HPSOCKET_API ITcpPullServer* HP_Create_TcpPullServer(ITcpServerListener* pListener)
{
	return (ITcpPullServer*)(new CTcpPullServer(pListener));
}

HPSOCKET_API ITcpPullClient* HP_Create_TcpPullClient(ITcpClientListener* pListener)
{
	return (ITcpPullClient*)(new CTcpPullClient(pListener));
}

HPSOCKET_API ITcpPullAgent* HP_Create_TcpPullAgent(ITcpAgentListener* pListener)
{
	return (ITcpPullAgent*)(new CTcpPullAgent(pListener));
}

HPSOCKET_API IUdpServer* HP_Create_UdpServer(IUdpServerListener* pListener)
{
	return new CUdpServer(pListener);
}

HPSOCKET_API IUdpClient* HP_Create_UdpClient(IUdpClientListener* pListener)
{
	return new CUdpClient(pListener);
}

HPSOCKET_API void HP_Destroy_TcpServer(ITcpServer* pServer)
{
	delete pServer;
}

HPSOCKET_API void HP_Destroy_TcpClient(ITcpClient* pClient)
{
	delete pClient;
}

HPSOCKET_API void HP_Destroy_TcpAgent(ITcpAgent* pAgent)
{
	delete pAgent;
}

HPSOCKET_API void HP_Destroy_TcpPullServer(ITcpPullServer* pServer)
{
	delete pServer;
}

HPSOCKET_API void HP_Destroy_TcpPullClient(ITcpPullClient* pClient)
{
	delete pClient;
}

HPSOCKET_API void HP_Destroy_TcpPullAgent(ITcpPullAgent* pAgent)
{
	delete pAgent;
}

HPSOCKET_API void HP_Destroy_UdpServer(IUdpServer* pServer)
{
	delete pServer;
}

HPSOCKET_API void HP_Destroy_UdpClient(IUdpClient* pClient)
{
	delete pClient;
}

HPSOCKET_API LPCTSTR HP_GetSocketErrorDesc(EnSocketError enCode)
{
	return ::GetSocketErrorDesc(enCode);
}

HPSOCKET_API DWORD SYS_GetLastError()
{
	return ::GetLastError();
}

HPSOCKET_API int SYS_WSAGetLastError()
{
	return ::WSAGetLastError();
}

HPSOCKET_API int SYS_SetSocketOption(SOCKET sock, int level, int name, LPVOID val, int len)
{
	return ::SSO_SetSocketOption(sock, level, name, val, len);
}

HPSOCKET_API int SYS_GetSocketOption(SOCKET sock, int level, int name, LPVOID val, int* len)
{
	return ::SSO_GetSocketOption(sock, level, name, val, len);
}

HPSOCKET_API int SYS_IoctlSocket(SOCKET sock, long cmd, u_long* arg)
{
	return ::SSO_IoctlSocket(sock, cmd, arg);
}

HPSOCKET_API int SYS_WSAIoctl(SOCKET sock, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer, LPDWORD lpcbBytesReturned)
{
	return ::SSO_WSAIoctl(sock, dwIoControlCode, lpvInBuffer, cbInBuffer, lpvOutBuffer, cbOutBuffer, lpcbBytesReturned);
}
