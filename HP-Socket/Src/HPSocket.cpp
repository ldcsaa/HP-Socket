/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 4.0.1
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
#include "TcpAgent.h"
#include "TcpClient.h"
#include "TcpPullServer.h"
#include "TcpPullClient.h"
#include "TcpPullAgent.h"
#include "TcpPackServer.h"
#include "TcpPackClient.h"
#include "TcpPackAgent.h"
#include "UdpServer.h"
#include "UdpClient.h"
#include "UdpCast.h"

#include "HttpServer.h"
#include "HttpAgent.h"
#include "HttpClient.h"

/*****************************************************************************************************************************************************/
/****************************************************************** TCP/UDP Exports ******************************************************************/
/*****************************************************************************************************************************************************/

HPSOCKET_API ITcpServer* HP_Create_TcpServer(ITcpServerListener* pListener)
{
	return new CTcpServer(pListener);
}

HPSOCKET_API ITcpAgent* HP_Create_TcpAgent(ITcpAgentListener* pListener)
{
	return new CTcpAgent(pListener);
}

HPSOCKET_API ITcpClient* HP_Create_TcpClient(ITcpClientListener* pListener)
{
	return new CTcpClient(pListener);
}

HPSOCKET_API ITcpPullServer* HP_Create_TcpPullServer(ITcpServerListener* pListener)
{
	return (ITcpPullServer*)(new CTcpPullServer(pListener));
}

HPSOCKET_API ITcpPullAgent* HP_Create_TcpPullAgent(ITcpAgentListener* pListener)
{
	return (ITcpPullAgent*)(new CTcpPullAgent(pListener));
}

HPSOCKET_API ITcpPullClient* HP_Create_TcpPullClient(ITcpClientListener* pListener)
{
	return (ITcpPullClient*)(new CTcpPullClient(pListener));
}

HPSOCKET_API ITcpPackServer* HP_Create_TcpPackServer(ITcpServerListener* pListener)
{
	return (ITcpPackServer*)(new CTcpPackServer(pListener));
}

HPSOCKET_API ITcpPackAgent* HP_Create_TcpPackAgent(ITcpAgentListener* pListener)
{
	return (ITcpPackAgent*)(new CTcpPackAgent(pListener));
}

HPSOCKET_API ITcpPackClient* HP_Create_TcpPackClient(ITcpClientListener* pListener)
{
	return (ITcpPackClient*)(new CTcpPackClient(pListener));
}

HPSOCKET_API IUdpServer* HP_Create_UdpServer(IUdpServerListener* pListener)
{
	return new CUdpServer(pListener);
}

HPSOCKET_API IUdpClient* HP_Create_UdpClient(IUdpClientListener* pListener)
{
	return new CUdpClient(pListener);
}

HPSOCKET_API IUdpCast* HP_Create_UdpCast(IUdpCastListener* pListener)
{
	return new CUdpCast(pListener);
}

HPSOCKET_API void HP_Destroy_TcpServer(ITcpServer* pServer)
{
	delete pServer;
}

HPSOCKET_API void HP_Destroy_TcpAgent(ITcpAgent* pAgent)
{
	delete pAgent;
}

HPSOCKET_API void HP_Destroy_TcpClient(ITcpClient* pClient)
{
	delete pClient;
}

HPSOCKET_API void HP_Destroy_TcpPullServer(ITcpPullServer* pServer)
{
	delete pServer;
}

HPSOCKET_API void HP_Destroy_TcpPullAgent(ITcpPullAgent* pAgent)
{
	delete pAgent;
}

HPSOCKET_API void HP_Destroy_TcpPullClient(ITcpPullClient* pClient)
{
	delete pClient;
}

HPSOCKET_API void HP_Destroy_TcpPackServer(ITcpPackServer* pServer)
{
	delete pServer;
}

HPSOCKET_API void HP_Destroy_TcpPackAgent(ITcpPackAgent* pAgent)
{
	delete pAgent;
}

HPSOCKET_API void HP_Destroy_TcpPackClient(ITcpPackClient* pClient)
{
	delete pClient;
}

HPSOCKET_API void HP_Destroy_UdpServer(IUdpServer* pServer)
{
	delete pServer;
}

HPSOCKET_API void HP_Destroy_UdpClient(IUdpClient* pClient)
{
	delete pClient;
}

HPSOCKET_API void HP_Destroy_UdpCast(IUdpCast* pCast)
{
	delete pCast;
}

/*****************************************************************************************************************************************************/
/******************************************************************** HTTP Exports *******************************************************************/
/*****************************************************************************************************************************************************/

HPSOCKET_API IHttpServer* HP_Create_HttpServer(IHttpServerListener* pListener)
{
	return (IHttpServer*)(new CHttpServer(pListener));
}

HPSOCKET_API IHttpAgent* HP_Create_HttpAgent(IHttpAgentListener* pListener)
{
	return (IHttpAgent*)(new CHttpAgent(pListener));
}

HPSOCKET_API IHttpClient* HP_Create_HttpClient(IHttpClientListener* pListener)
{
	return (IHttpClient*)(new CHttpClient(pListener));
}

HPSOCKET_API void HP_Destroy_HttpServer(IHttpServer* pServer)
{
	delete pServer;
}

HPSOCKET_API void HP_Destroy_HttpAgent(IHttpAgent* pAgent)
{
	delete pAgent;
}

HPSOCKET_API void HP_Destroy_HttpClient(IHttpClient* pClient)
{
	delete pClient;
}

/*****************************************************************************************************************************************************/
/*************************************************************** Global Function Exports *************************************************************/
/*****************************************************************************************************************************************************/

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

HPSOCKET_API int SYS_SSO_NoDelay(SOCKET sock, BOOL bNoDelay)
{
	return ::SSO_NoDelay(sock, bNoDelay);
}

HPSOCKET_API int SYS_SSO_DontLinger(SOCKET sock, BOOL bDont)
{
	return ::SSO_DontLinger(sock, bDont);
}

HPSOCKET_API int SYS_SSO_Linger(SOCKET sock, USHORT l_onoff, USHORT l_linger)
{
	return ::SSO_Linger(sock, l_onoff, l_linger);
}

HPSOCKET_API int SYS_SSO_RecvBuffSize(SOCKET sock, int size)
{
	return ::SSO_RecvBuffSize(sock, size);
}

HPSOCKET_API int SYS_SSO_SendBuffSize(SOCKET sock, int size)
{
	return ::SSO_SendBuffSize(sock, size);
}

HPSOCKET_API int SYS_SSO_ReuseAddress(SOCKET sock, BOOL bReuse)
{
	return ::SSO_ReuseAddress(sock, bReuse);
}

HPSOCKET_API BOOL SYS_GetSocketLocalAddress(SOCKET socket, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
{
	return ::GetSocketLocalAddress(socket, lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API BOOL SYS_GetSocketRemoteAddress(SOCKET socket, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
{
	return ::GetSocketRemoteAddress(socket, lpszAddress, *piAddressLen, *pusPort);
}
