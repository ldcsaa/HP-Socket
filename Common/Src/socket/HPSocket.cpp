/*
 * Copyright Bruce Liang (ldcsaa@gmail.com)
 *
 * Version	: 3.1.1
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * WeiBo	: http://weibo.com/u/1402935851
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

#if !defined(_WIN64)
	#pragma comment(linker, "/EXPORT:HP_Create_TcpClient=_HP_Create_TcpClient")
	#pragma comment(linker, "/EXPORT:HP_Create_TcpPullClient=_HP_Create_TcpPullClient")
	#pragma comment(linker, "/EXPORT:HP_Create_TcpPullServer=_HP_Create_TcpPullServer")
	#pragma comment(linker, "/EXPORT:HP_Create_TcpServer=_HP_Create_TcpServer")
	#pragma comment(linker, "/EXPORT:HP_Create_UdpClient=_HP_Create_UdpClient")
	#pragma comment(linker, "/EXPORT:HP_Create_UdpServer=_HP_Create_UdpServer")
	#pragma comment(linker, "/EXPORT:HP_Destroy_TcpClient=_HP_Destroy_TcpClient")
	#pragma comment(linker, "/EXPORT:HP_Destroy_TcpPullClient=_HP_Destroy_TcpPullClient")
	#pragma comment(linker, "/EXPORT:HP_Destroy_TcpPullServer=_HP_Destroy_TcpPullServer")
	#pragma comment(linker, "/EXPORT:HP_Destroy_TcpServer=_HP_Destroy_TcpServer")
	#pragma comment(linker, "/EXPORT:HP_Destroy_UdpClient=_HP_Destroy_UdpClient")
	#pragma comment(linker, "/EXPORT:HP_Destroy_UdpServer=_HP_Destroy_UdpServer")
#endif

HPSOCKET_API ITcpServer* HP_Create_TcpServer(ITcpServerListener* pListener)
{
	return new CTcpServer(pListener);
}

HPSOCKET_API ITcpClient* HP_Create_TcpClient(ITcpClientListener* pListener)
{
	return new CTcpClient(pListener);
}

HPSOCKET_API ITcpPullServer* HP_Create_TcpPullServer(ITcpServerListener* pListener)
{
	return (ITcpPullServer*)(new CTcpPullServer(pListener));
}

HPSOCKET_API ITcpPullClient* HP_Create_TcpPullClient(ITcpClientListener* pListener)
{
	return (ITcpPullClient*)(new CTcpPullClient(pListener));
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

HPSOCKET_API void HP_Destroy_TcpPullServer(ITcpPullServer* pServer)
{
	delete pServer;
}

HPSOCKET_API void HP_Destroy_TcpPullClient(ITcpPullClient* pClient)
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
