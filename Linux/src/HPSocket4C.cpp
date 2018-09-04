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
 
#include "SocketObject4C.h"
#include "TcpServer.h"
#include "TcpClient.h"
#include "TcpAgent.h"
#include "TcpPullServer.h"
#include "TcpPullClient.h"
#include "TcpPullAgent.h"
#include "TcpPackServer.h"
#include "TcpPackClient.h"
#include "TcpPackAgent.h"
#include "UdpServer.h"
#include "UdpClient.h"
#include "UdpCast.h"
#include "HPThreadPool.h"

#ifdef _HTTP_SUPPORT
#include "HttpServer.h"
#include "HttpAgent.h"
#include "HttpClient.h"
#endif

/*****************************************************************************************************************************************************/
/****************************************************************** TCP/UDP Exports ******************************************************************/
/*****************************************************************************************************************************************************/

typedef C_HP_ObjectT<CTcpServer, ITcpServerListener>							C_HP_TcpServer;
typedef C_HP_ObjectT<CTcpPullServer, ITcpServerListener, sizeof(IPullSocket)>	C_HP_TcpPullServer;
typedef C_HP_ObjectT<CTcpPackServer, ITcpServerListener, sizeof(IPackSocket)>	C_HP_TcpPackServer;

typedef C_HP_ObjectT<CTcpAgent, ITcpAgentListener>								C_HP_TcpAgent;
typedef C_HP_ObjectT<CTcpPullAgent, ITcpAgentListener, sizeof(IPullSocket)>		C_HP_TcpPullAgent;
typedef C_HP_ObjectT<CTcpPackAgent, ITcpAgentListener, sizeof(IPackSocket)>		C_HP_TcpPackAgent;

typedef C_HP_ObjectT<CTcpClient, ITcpClientListener>							C_HP_TcpClient;
typedef C_HP_ObjectT<CTcpPullClient, ITcpClientListener, sizeof(IPullClient)>	C_HP_TcpPullClient;
typedef C_HP_ObjectT<CTcpPackClient, ITcpClientListener, sizeof(IPackClient)>	C_HP_TcpPackClient;

typedef C_HP_ObjectT<CUdpServer, IUdpServerListener>							C_HP_UdpServer;
typedef C_HP_ObjectT<CUdpClient, IUdpClientListener>							C_HP_UdpClient;
typedef C_HP_ObjectT<CUdpCast, IUdpCastListener>								C_HP_UdpCast;

/****************************************************/
/**************** TCP/UDP 对象创建函数 ***************/

HPSOCKET_API HP_TcpServer __HP_CALL Create_HP_TcpServer(HP_TcpServerListener pListener)
{
	return (HP_TcpServer)(new C_HP_TcpServer((ITcpServerListener*)pListener));
}

HPSOCKET_API HP_TcpAgent __HP_CALL Create_HP_TcpAgent(HP_TcpAgentListener pListener)
{
	return (HP_TcpAgent)(new C_HP_TcpAgent((ITcpAgentListener*)pListener));
}

HPSOCKET_API HP_TcpClient __HP_CALL Create_HP_TcpClient(HP_TcpClientListener pListener)
{
	return (HP_TcpClient)(new C_HP_TcpClient((ITcpClientListener*)pListener));
}

HPSOCKET_API HP_TcpPullServer __HP_CALL Create_HP_TcpPullServer(HP_TcpPullServerListener pListener)
{
	return (HP_TcpPullServer)(new C_HP_TcpPullServer((ITcpServerListener*)pListener));
}

HPSOCKET_API HP_TcpPullAgent __HP_CALL Create_HP_TcpPullAgent(HP_TcpPullAgentListener pListener)
{
	return (HP_TcpPullAgent)(new C_HP_TcpPullAgent((ITcpAgentListener*)pListener));
}

HPSOCKET_API HP_TcpPullClient __HP_CALL Create_HP_TcpPullClient(HP_TcpPullClientListener pListener)
{
	return (HP_TcpPullClient)(new C_HP_TcpPullClient((ITcpClientListener*)pListener));
}

HPSOCKET_API HP_TcpPackServer __HP_CALL Create_HP_TcpPackServer(HP_TcpServerListener pListener)
{
	return (HP_TcpPackServer)(new C_HP_TcpPackServer((ITcpServerListener*)pListener));
}

HPSOCKET_API HP_TcpPackAgent __HP_CALL Create_HP_TcpPackAgent(HP_TcpAgentListener pListener)
{
	return (HP_TcpPackAgent)(new C_HP_TcpPackAgent((ITcpAgentListener*)pListener));
}

HPSOCKET_API HP_TcpPackClient __HP_CALL Create_HP_TcpPackClient(HP_TcpClientListener pListener)
{
	return (HP_TcpPackClient)(new C_HP_TcpPackClient((ITcpClientListener*)pListener));
}

HPSOCKET_API HP_UdpServer __HP_CALL Create_HP_UdpServer(HP_UdpServerListener pListener)
{
	return (HP_UdpServer)(new C_HP_UdpServer((IUdpServerListener*)pListener));
}

HPSOCKET_API HP_UdpClient __HP_CALL Create_HP_UdpClient(HP_UdpClientListener pListener)
{
	return (HP_UdpClient)(new C_HP_UdpClient((IUdpClientListener*)pListener));
}

HPSOCKET_API HP_UdpCast __HP_CALL Create_HP_UdpCast(HP_UdpCastListener pListener)
{
	return (HP_UdpCast)(new C_HP_UdpCast((IUdpCastListener*)pListener));
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpServer(HP_TcpServer pServer)
{
	delete (C_HP_TcpServer*)pServer;
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpAgent(HP_TcpAgent pAgent)
{
	delete (C_HP_TcpAgent*)pAgent;
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpClient(HP_TcpClient pClient)
{
	delete (C_HP_TcpClient*)pClient;
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpPullServer(HP_TcpPullServer pServer)
{
	delete (C_HP_TcpPullServer*)pServer;
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpPullAgent(HP_TcpPullAgent pAgent)
{
	delete (C_HP_TcpPullAgent*)pAgent;
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpPullClient(HP_TcpPullClient pClient)
{
	delete (C_HP_TcpPullClient*)pClient;
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpPackServer(HP_TcpPackServer pServer)
{
	delete (C_HP_TcpPackServer*)pServer;
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpPackAgent(HP_TcpPackAgent pAgent)
{
	delete (C_HP_TcpPackAgent*)pAgent;
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpPackClient(HP_TcpPackClient pClient)
{
	delete (C_HP_TcpPackClient*)pClient;
}

HPSOCKET_API void __HP_CALL Destroy_HP_UdpServer(HP_UdpServer pServer)
{
	delete (C_HP_UdpServer*)pServer;
}

HPSOCKET_API void __HP_CALL Destroy_HP_UdpClient(HP_UdpClient pClient)
{
	delete (C_HP_UdpClient*)pClient;
}

HPSOCKET_API void __HP_CALL Destroy_HP_UdpCast(HP_UdpCast pCast)
{
	delete (C_HP_UdpCast*)pCast;
}

HPSOCKET_API HP_TcpServerListener __HP_CALL Create_HP_TcpServerListener()
{
	return (HP_TcpServerListener)(new C_HP_TcpServerListener);
}

HPSOCKET_API HP_TcpAgentListener __HP_CALL Create_HP_TcpAgentListener()
{
	return (HP_TcpAgentListener)(new C_HP_TcpAgentListener);
}

HPSOCKET_API HP_TcpClientListener __HP_CALL Create_HP_TcpClientListener()
{
	return (HP_TcpClientListener)(new C_HP_TcpClientListener);
}

HPSOCKET_API HP_TcpPullServerListener __HP_CALL Create_HP_TcpPullServerListener()
{
	return (HP_TcpPullServerListener)(new C_HP_TcpPullServerListener);
}

HPSOCKET_API HP_TcpPullAgentListener __HP_CALL Create_HP_TcpPullAgentListener()
{
	return (HP_TcpPullAgentListener)(new C_HP_TcpPullAgentListener);
}

HPSOCKET_API HP_TcpPullClientListener __HP_CALL Create_HP_TcpPullClientListener()
{
	return (HP_TcpPullClientListener)(new C_HP_TcpPullClientListener);
}

HPSOCKET_API HP_TcpPackServerListener __HP_CALL Create_HP_TcpPackServerListener()
{
	return (HP_TcpPackServerListener)(new C_HP_TcpPackServerListener);
}

HPSOCKET_API HP_TcpPackAgentListener __HP_CALL Create_HP_TcpPackAgentListener()
{
	return (HP_TcpPackAgentListener)(new C_HP_TcpPackAgentListener);
}

HPSOCKET_API HP_TcpPackClientListener __HP_CALL Create_HP_TcpPackClientListener()
{
	return (HP_TcpPackClientListener)(new C_HP_TcpPackClientListener);
}

HPSOCKET_API HP_UdpServerListener __HP_CALL Create_HP_UdpServerListener()
{
	return (HP_UdpServerListener)(new C_HP_UdpServerListener);
}

HPSOCKET_API HP_UdpClientListener __HP_CALL Create_HP_UdpClientListener()
{
	return (HP_UdpClientListener)(new C_HP_UdpClientListener);
}

HPSOCKET_API HP_UdpCastListener __HP_CALL Create_HP_UdpCastListener()
{
	return (HP_UdpCastListener)(new C_HP_UdpCastListener);
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpServerListener(HP_TcpServerListener pListener)
{
	delete (C_HP_TcpServerListener*)pListener;
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpAgentListener(HP_TcpAgentListener pListener)
{
	delete (C_HP_TcpAgentListener*)pListener;
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpClientListener(HP_TcpClientListener pListener)
{
	delete (C_HP_TcpClientListener*)pListener;
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpPullServerListener(HP_TcpPullServerListener pListener)
{
	delete (C_HP_TcpPullServerListener*)pListener;
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpPullAgentListener(HP_TcpPullAgentListener pListener)
{
	delete (C_HP_TcpPullAgentListener*)pListener;
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpPullClientListener(HP_TcpPullClientListener pListener)
{
	delete (C_HP_TcpPullClientListener*)pListener;
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpPackServerListener(HP_TcpPackServerListener pListener)
{
	delete (C_HP_TcpPackServerListener*)pListener;
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpPackAgentListener(HP_TcpPackAgentListener pListener)
{
	delete (C_HP_TcpPackAgentListener*)pListener;
}

HPSOCKET_API void __HP_CALL Destroy_HP_TcpPackClientListener(HP_TcpPackClientListener pListener)
{
	delete (C_HP_TcpPackClientListener*)pListener;
}

HPSOCKET_API void __HP_CALL Destroy_HP_UdpServerListener(HP_UdpServerListener pListener)
{
	delete (C_HP_UdpServerListener*)pListener;
}

HPSOCKET_API void __HP_CALL Destroy_HP_UdpClientListener(HP_UdpClientListener pListener)
{
	delete (C_HP_UdpClientListener*)pListener;
}

HPSOCKET_API void __HP_CALL Destroy_HP_UdpCastListener(HP_UdpCastListener pListener)
{
	delete (C_HP_UdpClientListener*)pListener;
}

/**********************************************************************************/
/***************************** Server 回调函数设置方法 *****************************/

HPSOCKET_API void __HP_CALL HP_Set_FN_Server_OnPrepareListen(HP_ServerListener pListener, HP_FN_Server_OnPrepareListen fn)
{
	((C_HP_TcpServerListener*)pListener)->m_fnOnPrepareListen = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Server_OnAccept(HP_ServerListener pListener, HP_FN_Server_OnAccept fn)
{
	((C_HP_TcpServerListener*)pListener)->m_fnOnAccept = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Server_OnHandShake(HP_ServerListener pListener, HP_FN_Server_OnHandShake fn)
{
	((C_HP_TcpServerListener*)pListener)->m_fnOnHandShake = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Server_OnSend(HP_ServerListener pListener, HP_FN_Server_OnSend fn)
{
	((C_HP_TcpServerListener*)pListener)->m_fnOnSend = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Server_OnReceive(HP_ServerListener pListener, HP_FN_Server_OnReceive fn)
{
	((C_HP_TcpServerListener*)pListener)->m_fnOnReceive = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Server_OnPullReceive(HP_ServerListener pListener, HP_FN_Server_OnPullReceive fn)
{
	((C_HP_TcpServerListener*)pListener)->m_fnOnPullReceive = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Server_OnClose(HP_ServerListener pListener, HP_FN_Server_OnClose fn)
{
	((C_HP_TcpServerListener*)pListener)->m_fnOnClose = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Server_OnShutdown(HP_ServerListener pListener, HP_FN_Server_OnShutdown fn)
{
	((C_HP_TcpServerListener*)pListener)->m_fnOnShutdown = fn;
}

/**********************************************************************************/
/***************************** Agent 回调函数设置方法 *****************************/

HPSOCKET_API void __HP_CALL HP_Set_FN_Agent_OnPrepareConnect(HP_AgentListener pListener, HP_FN_Agent_OnPrepareConnect fn)
{
	((C_HP_TcpAgentListener*)pListener)->m_fnOnPrepareConnect = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Agent_OnConnect(HP_AgentListener pListener, HP_FN_Agent_OnConnect fn)
{
	((C_HP_TcpAgentListener*)pListener)->m_fnOnConnect = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Agent_OnHandShake(HP_AgentListener pListener, HP_FN_Agent_OnHandShake fn)
{
	((C_HP_TcpAgentListener*)pListener)->m_fnOnHandShake = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Agent_OnSend(HP_AgentListener pListener, HP_FN_Agent_OnSend fn)
{
	((C_HP_TcpAgentListener*)pListener)->m_fnOnSend = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Agent_OnReceive(HP_AgentListener pListener, HP_FN_Agent_OnReceive fn)
{
	((C_HP_TcpAgentListener*)pListener)->m_fnOnReceive = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Agent_OnPullReceive(HP_AgentListener pListener, HP_FN_Agent_OnPullReceive fn)
{
	((C_HP_TcpAgentListener*)pListener)->m_fnOnPullReceive = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Agent_OnClose(HP_AgentListener pListener, HP_FN_Agent_OnClose fn)
{
	((C_HP_TcpAgentListener*)pListener)->m_fnOnClose = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Agent_OnShutdown(HP_AgentListener pListener, HP_FN_Agent_OnShutdown fn)
{
	((C_HP_TcpAgentListener*)pListener)->m_fnOnShutdown = fn;
}

/**********************************************************************************/
/***************************** Client 回调函数设置方法 *****************************/

HPSOCKET_API void __HP_CALL HP_Set_FN_Client_OnPrepareConnect(HP_ClientListener pListener, HP_FN_Client_OnPrepareConnect fn)
{
	((C_HP_TcpClientListener*)pListener)->m_fnOnPrepareConnect = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Client_OnConnect(HP_ClientListener pListener, HP_FN_Client_OnConnect fn)
{
	((C_HP_TcpClientListener*)pListener)->m_fnOnConnect = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Client_OnHandShake(HP_ClientListener pListener, HP_FN_Client_OnHandShake fn)
{
	((C_HP_TcpClientListener*)pListener)->m_fnOnHandShake = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Client_OnSend(HP_ClientListener pListener, HP_FN_Client_OnSend fn)
{
	((C_HP_TcpClientListener*)pListener)->m_fnOnSend = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Client_OnReceive(HP_ClientListener pListener, HP_FN_Client_OnReceive fn)
{
	((C_HP_TcpClientListener*)pListener)->m_fnOnReceive = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Client_OnPullReceive(HP_ClientListener pListener, HP_FN_Client_OnPullReceive fn)
{
	((C_HP_TcpClientListener*)pListener)->m_fnOnPullReceive = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_Client_OnClose(HP_ClientListener pListener, HP_FN_Client_OnClose fn)
{
	((C_HP_TcpClientListener*)pListener)->m_fnOnClose = fn;
}

/**************************************************************************/
/***************************** Server 操作方法 *****************************/

HPSOCKET_API BOOL __HP_CALL HP_Server_Start(HP_Server pServer, LPCTSTR lpszBindAddress, USHORT usPort)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->Start(lpszBindAddress, usPort);
}

HPSOCKET_API BOOL __HP_CALL HP_Server_Stop(HP_Server pServer)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->Stop();
}

HPSOCKET_API BOOL __HP_CALL HP_Server_Send(HP_Server pServer, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->Send(dwConnID, pBuffer, iLength);
}

HPSOCKET_API BOOL __HP_CALL HP_Server_SendPart(HP_Server pServer, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->Send(dwConnID, pBuffer, iLength, iOffset);
}

HPSOCKET_API BOOL __HP_CALL HP_Server_SendPackets(HP_Server pServer, HP_CONNID dwConnID, const WSABUF pBuffers[], int iCount)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->SendPackets(dwConnID, pBuffers, iCount);
}

HPSOCKET_API BOOL __HP_CALL HP_Server_PauseReceive(HP_Server pServer, HP_CONNID dwConnID, BOOL bPause)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->PauseReceive(dwConnID, bPause);
}

HPSOCKET_API BOOL __HP_CALL HP_Server_Disconnect(HP_Server pServer, HP_CONNID dwConnID, BOOL bForce)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->Disconnect(dwConnID, bForce);
}

HPSOCKET_API BOOL __HP_CALL HP_Server_DisconnectLongConnections(HP_Server pServer, DWORD dwPeriod, BOOL bForce)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->DisconnectLongConnections(dwPeriod, bForce);
}

HPSOCKET_API BOOL __HP_CALL HP_Server_DisconnectSilenceConnections(HP_Server pServer, DWORD dwPeriod, BOOL bForce)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->DisconnectSilenceConnections(dwPeriod, bForce);
}

/******************************************************************************/
/***************************** Server 属性访问方法 *****************************/

HPSOCKET_API BOOL __HP_CALL HP_Server_SetConnectionExtra(HP_Server pServer, HP_CONNID dwConnID, PVOID pExtra)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->SetConnectionExtra(dwConnID, pExtra);
}

HPSOCKET_API BOOL __HP_CALL HP_Server_GetConnectionExtra(HP_Server pServer, HP_CONNID dwConnID, PVOID* ppExtra)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetConnectionExtra(dwConnID, ppExtra);
}

HPSOCKET_API BOOL __HP_CALL HP_Server_IsSecure(HP_Server pServer)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->IsSecure();
}

HPSOCKET_API BOOL __HP_CALL HP_Server_HasStarted(HP_Server pServer)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->HasStarted();
}

HPSOCKET_API En_HP_ServiceState __HP_CALL HP_Server_GetState(HP_Server pServer)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetState();
}

HPSOCKET_API En_HP_SocketError __HP_CALL HP_Server_GetLastError(HP_Server pServer)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetLastError();
}

HPSOCKET_API LPCTSTR __HP_CALL HP_Server_GetLastErrorDesc(HP_Server pServer)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetLastErrorDesc();
}

HPSOCKET_API BOOL __HP_CALL HP_Server_GetPendingDataLength(HP_Server pServer, HP_CONNID dwConnID, int* piPending)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetPendingDataLength(dwConnID, *piPending);
}

HPSOCKET_API BOOL __HP_CALL HP_Server_IsPauseReceive(HP_Server pServer, HP_CONNID dwConnID, BOOL* pbPaused)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->IsPauseReceive(dwConnID, *pbPaused);
}

HPSOCKET_API BOOL __HP_CALL HP_Server_IsConnected(HP_Server pServer, HP_CONNID dwConnID)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->IsConnected(dwConnID);
}

HPSOCKET_API DWORD __HP_CALL HP_Server_GetConnectionCount(HP_Server pServer)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetConnectionCount();
}

HPSOCKET_API BOOL __HP_CALL HP_Server_GetAllConnectionIDs(HP_Server pServer, HP_CONNID pIDs[], DWORD* pdwCount)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetAllConnectionIDs(pIDs, *pdwCount);
}

HPSOCKET_API BOOL __HP_CALL HP_Server_GetConnectPeriod(HP_Server pServer, HP_CONNID dwConnID, DWORD* pdwPeriod)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetConnectPeriod(dwConnID, *pdwPeriod);
}

HPSOCKET_API BOOL __HP_CALL HP_Server_GetSilencePeriod(HP_Server pServer, HP_CONNID dwConnID, DWORD* pdwPeriod)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetSilencePeriod(dwConnID, *pdwPeriod);
}

HPSOCKET_API BOOL __HP_CALL HP_Server_GetListenAddress(HP_Server pServer, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetListenAddress(lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API BOOL __HP_CALL HP_Server_GetLocalAddress(HP_Server pServer, HP_CONNID dwConnID, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetLocalAddress(dwConnID, lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API BOOL __HP_CALL HP_Server_GetRemoteAddress(HP_Server pServer, HP_CONNID dwConnID, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetRemoteAddress(dwConnID, lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API void __HP_CALL HP_Server_SetSendPolicy(HP_Server pServer, En_HP_SendPolicy enSendPolicy)
{
	C_HP_Object::ToSecond<IServer>(pServer)->SetSendPolicy(enSendPolicy);
}

HPSOCKET_API void __HP_CALL HP_Server_SetFreeSocketObjLockTime(HP_Server pServer, DWORD dwFreeSocketObjLockTime)
{
	C_HP_Object::ToSecond<IServer>(pServer)->SetFreeSocketObjLockTime(dwFreeSocketObjLockTime);
}

HPSOCKET_API void __HP_CALL HP_Server_SetFreeSocketObjPool(HP_Server pServer, DWORD dwFreeSocketObjPool)
{
	C_HP_Object::ToSecond<IServer>(pServer)->SetFreeSocketObjPool(dwFreeSocketObjPool);
}

HPSOCKET_API void __HP_CALL HP_Server_SetFreeBufferObjPool(HP_Server pServer, DWORD dwFreeBufferObjPool)
{
	C_HP_Object::ToSecond<IServer>(pServer)->SetFreeBufferObjPool(dwFreeBufferObjPool);
}

HPSOCKET_API void __HP_CALL HP_Server_SetFreeSocketObjHold(HP_Server pServer, DWORD dwFreeSocketObjHold)
{
	C_HP_Object::ToSecond<IServer>(pServer)->SetFreeSocketObjHold(dwFreeSocketObjHold);
}

HPSOCKET_API void __HP_CALL HP_Server_SetFreeBufferObjHold(HP_Server pServer, DWORD dwFreeBufferObjHold)
{
	C_HP_Object::ToSecond<IServer>(pServer)->SetFreeBufferObjHold(dwFreeBufferObjHold);
}

HPSOCKET_API void __HP_CALL HP_Server_SetMaxConnectionCount(HP_Server pServer, DWORD dwMaxConnectionCount)
{
	C_HP_Object::ToSecond<IServer>(pServer)->SetMaxConnectionCount(dwMaxConnectionCount);
}

HPSOCKET_API void __HP_CALL HP_Server_SetWorkerThreadCount(HP_Server pServer, DWORD dwWorkerThreadCount)
{
	C_HP_Object::ToSecond<IServer>(pServer)->SetWorkerThreadCount(dwWorkerThreadCount);
}

HPSOCKET_API void __HP_CALL HP_Server_SetMarkSilence(HP_Server pServer, BOOL bMarkSilence)
{
	C_HP_Object::ToSecond<IServer>(pServer)->SetMarkSilence(bMarkSilence);
}

HPSOCKET_API En_HP_SendPolicy __HP_CALL HP_Server_GetSendPolicy(HP_Server pServer)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetSendPolicy();
}

HPSOCKET_API DWORD __HP_CALL HP_Server_GetFreeSocketObjLockTime(HP_Server pServer)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetFreeSocketObjLockTime();
}

HPSOCKET_API DWORD __HP_CALL HP_Server_GetFreeSocketObjPool(HP_Server pServer)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetFreeSocketObjPool();
}

HPSOCKET_API DWORD __HP_CALL HP_Server_GetFreeBufferObjPool(HP_Server pServer)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetFreeBufferObjPool();
}

HPSOCKET_API DWORD __HP_CALL HP_Server_GetFreeSocketObjHold(HP_Server pServer)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetFreeSocketObjHold();
}

HPSOCKET_API DWORD __HP_CALL HP_Server_GetFreeBufferObjHold(HP_Server pServer)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetFreeBufferObjHold();
}

HPSOCKET_API DWORD __HP_CALL HP_Server_GetMaxConnectionCount(HP_Server pServer)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetMaxConnectionCount();
}

HPSOCKET_API DWORD __HP_CALL HP_Server_GetWorkerThreadCount(HP_Server pServer)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->GetWorkerThreadCount();
}

HPSOCKET_API BOOL __HP_CALL HP_Server_IsMarkSilence(HP_Server pServer)
{
	return C_HP_Object::ToSecond<IServer>(pServer)->IsMarkSilence();
}

/**********************************************************************************/
/******************************* TCP Server 操作方法 *******************************/

HPSOCKET_API BOOL __HP_CALL HP_TcpServer_SendSmallFile(HP_Server pServer, HP_CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail)
{
	return C_HP_Object::ToSecond<ITcpServer>(pServer)->SendSmallFile(dwConnID, lpszFileName, pHead, pTail);
}

/**********************************************************************************/
/***************************** TCP Server 属性访问方法 *****************************/

HPSOCKET_API void __HP_CALL HP_TcpServer_SetAcceptSocketCount(HP_TcpServer pServer, DWORD dwAcceptSocketCount)
{
	C_HP_Object::ToSecond<ITcpServer>(pServer)->SetAcceptSocketCount(dwAcceptSocketCount);
}

HPSOCKET_API void __HP_CALL HP_TcpServer_SetSocketBufferSize(HP_TcpServer pServer, DWORD dwSocketBufferSize)
{
	C_HP_Object::ToSecond<ITcpServer>(pServer)->SetSocketBufferSize(dwSocketBufferSize);
}

HPSOCKET_API void __HP_CALL HP_TcpServer_SetSocketListenQueue(HP_TcpServer pServer, DWORD dwSocketListenQueue)
{
	C_HP_Object::ToSecond<ITcpServer>(pServer)->SetSocketListenQueue(dwSocketListenQueue);
}

HPSOCKET_API void __HP_CALL HP_TcpServer_SetKeepAliveTime(HP_TcpServer pServer, DWORD dwKeepAliveTime)
{
	C_HP_Object::ToSecond<ITcpServer>(pServer)->SetKeepAliveTime(dwKeepAliveTime);
}

HPSOCKET_API void __HP_CALL HP_TcpServer_SetKeepAliveInterval(HP_TcpServer pServer, DWORD dwKeepAliveInterval)
{
	C_HP_Object::ToSecond<ITcpServer>(pServer)->SetKeepAliveInterval(dwKeepAliveInterval);
}

HPSOCKET_API DWORD __HP_CALL HP_TcpServer_GetAcceptSocketCount(HP_TcpServer pServer)
{
	return C_HP_Object::ToSecond<ITcpServer>(pServer)->GetAcceptSocketCount();
}

HPSOCKET_API DWORD __HP_CALL HP_TcpServer_GetSocketBufferSize(HP_TcpServer pServer)
{
	return C_HP_Object::ToSecond<ITcpServer>(pServer)->GetSocketBufferSize();
}

HPSOCKET_API DWORD __HP_CALL HP_TcpServer_GetSocketListenQueue(HP_TcpServer pServer)
{
	return C_HP_Object::ToSecond<ITcpServer>(pServer)->GetSocketListenQueue();
}

HPSOCKET_API DWORD __HP_CALL HP_TcpServer_GetKeepAliveTime(HP_TcpServer pServer)
{
	return C_HP_Object::ToSecond<ITcpServer>(pServer)->GetKeepAliveTime();
}

HPSOCKET_API DWORD __HP_CALL HP_TcpServer_GetKeepAliveInterval(HP_TcpServer pServer)
{
	return C_HP_Object::ToSecond<ITcpServer>(pServer)->GetKeepAliveInterval();
}

/**********************************************************************************/
/***************************** UDP Server 属性访问方法 *****************************/

HPSOCKET_API void __HP_CALL HP_UdpServer_SetMaxDatagramSize(HP_UdpServer pServer, DWORD dwMaxDatagramSize)
{
	C_HP_Object::ToSecond<IUdpServer>(pServer)->SetMaxDatagramSize(dwMaxDatagramSize);
}

HPSOCKET_API DWORD __HP_CALL HP_UdpServer_GetMaxDatagramSize(HP_UdpServer pServer)
{
	return C_HP_Object::ToSecond<IUdpServer>(pServer)->GetMaxDatagramSize();
}

HPSOCKET_API void __HP_CALL HP_UdpServer_SetPostReceiveCount(HP_UdpServer pServer, DWORD dwPostReceiveCount)
{
	C_HP_Object::ToSecond<IUdpServer>(pServer)->SetPostReceiveCount(dwPostReceiveCount);
}

HPSOCKET_API DWORD __HP_CALL HP_UdpServer_GetPostReceiveCount(HP_UdpServer pServer)
{
	return C_HP_Object::ToSecond<IUdpServer>(pServer)->GetPostReceiveCount();
}

HPSOCKET_API void __HP_CALL HP_UdpServer_SetDetectAttempts(HP_UdpServer pServer, DWORD dwDetectAttempts)
{
	C_HP_Object::ToSecond<IUdpServer>(pServer)->SetDetectAttempts(dwDetectAttempts);
}

HPSOCKET_API void __HP_CALL HP_UdpServer_SetDetectInterval(HP_UdpServer pServer, DWORD dwDetectInterval)
{
	C_HP_Object::ToSecond<IUdpServer>(pServer)->SetDetectInterval(dwDetectInterval);
}

HPSOCKET_API DWORD __HP_CALL HP_UdpServer_GetDetectAttempts(HP_UdpServer pServer)
{
	return C_HP_Object::ToSecond<IUdpServer>(pServer)->GetDetectAttempts();
}

HPSOCKET_API DWORD __HP_CALL HP_UdpServer_GetDetectInterval(HP_UdpServer pServer)
{
	return C_HP_Object::ToSecond<IUdpServer>(pServer)->GetDetectInterval();
}

/**************************************************************************/
/***************************** Agent 操作方法 *****************************/

HPSOCKET_API BOOL __HP_CALL HP_Agent_Start(HP_Agent pAgent, LPCTSTR lpszBindAddress, BOOL bAsyncConnect)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->Start(lpszBindAddress, bAsyncConnect);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_Stop(HP_Agent pAgent)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->Stop();
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_Connect(HP_Agent pAgent, LPCTSTR lpszRemoteAddress, USHORT usPort, HP_CONNID* pdwConnID)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->Connect(lpszRemoteAddress, usPort, pdwConnID);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_ConnectWithExtra(HP_Agent pAgent, LPCTSTR lpszRemoteAddress, USHORT usPort, HP_CONNID* pdwConnID, PVOID pExtra)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->Connect(lpszRemoteAddress, usPort, pdwConnID, pExtra);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_ConnectWithLocalPort(HP_Agent pAgent, LPCTSTR lpszRemoteAddress, USHORT usPort, HP_CONNID* pdwConnID, USHORT usLocalPort)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->Connect(lpszRemoteAddress, usPort, pdwConnID, nullptr, usLocalPort);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_ConnectWithExtraAndLocalPort(HP_Agent pAgent, LPCTSTR lpszRemoteAddress, USHORT usPort, HP_CONNID* pdwConnID, PVOID pExtra, USHORT usLocalPort)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->Connect(lpszRemoteAddress, usPort, pdwConnID, pExtra, usLocalPort);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_Send(HP_Agent pAgent, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->Send(dwConnID, pBuffer, iLength);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_SendPart(HP_Agent pAgent, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->Send(dwConnID, pBuffer, iLength, iOffset);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_SendPackets(HP_Agent pAgent, HP_CONNID dwConnID, const WSABUF pBuffers[], int iCount)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->SendPackets(dwConnID, pBuffers, iCount);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_PauseReceive(HP_Agent pAgent, HP_CONNID dwConnID, BOOL bPause)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->PauseReceive(dwConnID, bPause);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_Disconnect(HP_Agent pAgent, HP_CONNID dwConnID, BOOL bForce)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->Disconnect(dwConnID, bForce);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_DisconnectLongConnections(HP_Agent pAgent, DWORD dwPeriod, BOOL bForce)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->DisconnectLongConnections(dwPeriod, bForce);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_DisconnectSilenceConnections(HP_Agent pAgent, DWORD dwPeriod, BOOL bForce)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->DisconnectSilenceConnections(dwPeriod, bForce);
}

/******************************************************************************/
/***************************** Agent 属性访问方法 *****************************/

HPSOCKET_API BOOL __HP_CALL HP_Agent_SetConnectionExtra(HP_Agent pAgent, HP_CONNID dwConnID, PVOID pExtra)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->SetConnectionExtra(dwConnID, pExtra);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_GetConnectionExtra(HP_Agent pAgent, HP_CONNID dwConnID, PVOID* ppExtra)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetConnectionExtra(dwConnID, ppExtra);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_IsSecure(HP_Agent pAgent)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->IsSecure();
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_HasStarted(HP_Agent pAgent)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->HasStarted();
}

HPSOCKET_API En_HP_ServiceState __HP_CALL HP_Agent_GetState(HP_Agent pAgent)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetState();
}

HPSOCKET_API En_HP_SocketError __HP_CALL HP_Agent_GetLastError(HP_Agent pAgent)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetLastError();
}

HPSOCKET_API LPCTSTR __HP_CALL HP_Agent_GetLastErrorDesc(HP_Agent pAgent)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetLastErrorDesc();
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_GetPendingDataLength(HP_Agent pAgent, HP_CONNID dwConnID, int* piPending)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetPendingDataLength(dwConnID, *piPending);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_IsPauseReceive(HP_Agent pAgent, HP_CONNID dwConnID, BOOL* pbPaused)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->IsPauseReceive(dwConnID, *pbPaused);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_IsConnected(HP_Agent pAgent, HP_CONNID dwConnID)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->IsConnected(dwConnID);
}

HPSOCKET_API DWORD __HP_CALL HP_Agent_GetConnectionCount(HP_Agent pAgent)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetConnectionCount();
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_GetAllConnectionIDs(HP_Agent pAgent, HP_CONNID pIDs[], DWORD* pdwCount)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetAllConnectionIDs(pIDs, *pdwCount);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_GetConnectPeriod(HP_Agent pAgent, HP_CONNID dwConnID, DWORD* pdwPeriod)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetConnectPeriod(dwConnID, *pdwPeriod);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_GetSilencePeriod(HP_Agent pAgent, HP_CONNID dwConnID, DWORD* pdwPeriod)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetSilencePeriod(dwConnID, *pdwPeriod);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_GetLocalAddress(HP_Agent pAgent, HP_CONNID dwConnID, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetLocalAddress(dwConnID, lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_GetRemoteAddress(HP_Agent pAgent, HP_CONNID dwConnID, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetRemoteAddress(dwConnID, lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_GetRemoteHost(HP_Agent pAgent, HP_CONNID dwConnID, TCHAR lpszHost[], int* piHostLen, USHORT* pusPort)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetRemoteHost(dwConnID, lpszHost, *piHostLen, *pusPort);
}

HPSOCKET_API void __HP_CALL HP_Agent_SetSendPolicy(HP_Agent pAgent, En_HP_SendPolicy enSendPolicy)
{
	C_HP_Object::ToSecond<IAgent>(pAgent)->SetSendPolicy(enSendPolicy);
}

HPSOCKET_API void __HP_CALL HP_Agent_SetFreeSocketObjLockTime(HP_Agent pAgent, DWORD dwFreeSocketObjLockTime)
{
	C_HP_Object::ToSecond<IAgent>(pAgent)->SetFreeSocketObjLockTime(dwFreeSocketObjLockTime);
}

HPSOCKET_API void __HP_CALL HP_Agent_SetFreeSocketObjPool(HP_Agent pAgent, DWORD dwFreeSocketObjPool)
{
	C_HP_Object::ToSecond<IAgent>(pAgent)->SetFreeSocketObjPool(dwFreeSocketObjPool);
}

HPSOCKET_API void __HP_CALL HP_Agent_SetFreeBufferObjPool(HP_Agent pAgent, DWORD dwFreeBufferObjPool)
{
	C_HP_Object::ToSecond<IAgent>(pAgent)->SetFreeBufferObjPool(dwFreeBufferObjPool);
}

HPSOCKET_API void __HP_CALL HP_Agent_SetFreeSocketObjHold(HP_Agent pAgent, DWORD dwFreeSocketObjHold)
{
	C_HP_Object::ToSecond<IAgent>(pAgent)->SetFreeSocketObjHold(dwFreeSocketObjHold);
}

HPSOCKET_API void __HP_CALL HP_Agent_SetFreeBufferObjHold(HP_Agent pAgent, DWORD dwFreeBufferObjHold)
{
	C_HP_Object::ToSecond<IAgent>(pAgent)->SetFreeBufferObjHold(dwFreeBufferObjHold);
}

HPSOCKET_API void __HP_CALL HP_Agent_SetMaxConnectionCount(HP_Agent pAgent, DWORD dwMaxConnectionCount)
{
	C_HP_Object::ToSecond<IAgent>(pAgent)->SetMaxConnectionCount(dwMaxConnectionCount);
}

HPSOCKET_API void __HP_CALL HP_Agent_SetWorkerThreadCount(HP_Agent pAgent, DWORD dwWorkerThreadCount)
{
	C_HP_Object::ToSecond<IAgent>(pAgent)->SetWorkerThreadCount(dwWorkerThreadCount);
}

HPSOCKET_API void __HP_CALL HP_Agent_SetMarkSilence(HP_Agent pAgent, BOOL bMarkSilence)
{
	C_HP_Object::ToSecond<IAgent>(pAgent)->SetMarkSilence(bMarkSilence);
}

HPSOCKET_API En_HP_SendPolicy __HP_CALL HP_Agent_GetSendPolicy(HP_Agent pAgent)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetSendPolicy();
}

HPSOCKET_API DWORD __HP_CALL HP_Agent_GetFreeSocketObjLockTime(HP_Agent pAgent)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetFreeSocketObjLockTime();
}

HPSOCKET_API DWORD __HP_CALL HP_Agent_GetFreeSocketObjPool(HP_Agent pAgent)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetFreeSocketObjPool();
}

HPSOCKET_API DWORD __HP_CALL HP_Agent_GetFreeBufferObjPool(HP_Agent pAgent)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetFreeBufferObjPool();
}

HPSOCKET_API DWORD __HP_CALL HP_Agent_GetFreeSocketObjHold(HP_Agent pAgent)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetFreeSocketObjHold();
}

HPSOCKET_API DWORD __HP_CALL HP_Agent_GetFreeBufferObjHold(HP_Agent pAgent)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetFreeBufferObjHold();
}

HPSOCKET_API DWORD __HP_CALL HP_Agent_GetMaxConnectionCount(HP_Agent pAgent)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetMaxConnectionCount();
}

HPSOCKET_API DWORD __HP_CALL HP_Agent_GetWorkerThreadCount(HP_Agent pAgent)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->GetWorkerThreadCount();
}

HPSOCKET_API BOOL __HP_CALL HP_Agent_IsMarkSilence(HP_Agent pAgent)
{
	return C_HP_Object::ToSecond<IAgent>(pAgent)->IsMarkSilence();
}

/**********************************************************************************/
/******************************* TCP Agent 操作方法 *******************************/

HPSOCKET_API BOOL __HP_CALL HP_TcpAgent_SendSmallFile(HP_Agent pAgent, HP_CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail)
{
	return C_HP_Object::ToSecond<ITcpAgent>(pAgent)->SendSmallFile(dwConnID, lpszFileName, pHead, pTail);
}

/**********************************************************************************/
/***************************** TCP Agent 属性访问方法 *****************************/

HPSOCKET_API void __HP_CALL HP_TcpAgent_SetReuseAddress(HP_TcpAgent pAgent, BOOL bReuseAddress)
{
	C_HP_Object::ToSecond<ITcpAgent>(pAgent)->SetReuseAddress(bReuseAddress);
}

HPSOCKET_API BOOL __HP_CALL HP_TcpAgent_IsReuseAddress(HP_TcpAgent pAgent)
{
	return C_HP_Object::ToSecond<ITcpAgent>(pAgent)->IsReuseAddress();
}

HPSOCKET_API void __HP_CALL HP_TcpAgent_SetSocketBufferSize(HP_TcpAgent pAgent, DWORD dwSocketBufferSize)
{
	C_HP_Object::ToSecond<ITcpAgent>(pAgent)->SetSocketBufferSize(dwSocketBufferSize);
}

HPSOCKET_API void __HP_CALL HP_TcpAgent_SetKeepAliveTime(HP_TcpAgent pAgent, DWORD dwKeepAliveTime)
{
	C_HP_Object::ToSecond<ITcpAgent>(pAgent)->SetKeepAliveTime(dwKeepAliveTime);
}

HPSOCKET_API void __HP_CALL HP_TcpAgent_SetKeepAliveInterval(HP_TcpAgent pAgent, DWORD dwKeepAliveInterval)
{
	C_HP_Object::ToSecond<ITcpAgent>(pAgent)->SetKeepAliveInterval(dwKeepAliveInterval);
}

HPSOCKET_API DWORD __HP_CALL HP_TcpAgent_GetSocketBufferSize(HP_TcpAgent pAgent)
{
	return C_HP_Object::ToSecond<ITcpAgent>(pAgent)->GetSocketBufferSize();
}

HPSOCKET_API DWORD __HP_CALL HP_TcpAgent_GetKeepAliveTime(HP_TcpAgent pAgent)
{
	return C_HP_Object::ToSecond<ITcpAgent>(pAgent)->GetKeepAliveTime();
}

HPSOCKET_API DWORD __HP_CALL HP_TcpAgent_GetKeepAliveInterval(HP_TcpAgent pAgent)
{
	return C_HP_Object::ToSecond<ITcpAgent>(pAgent)->GetKeepAliveInterval();
}

/******************************************************************************/
/***************************** Client 组件操作方法 *****************************/

HPSOCKET_API BOOL __HP_CALL HP_Client_Start(HP_Client pClient, LPCTSTR lpszRemoteAddress, USHORT usPort, BOOL bAsyncConnect)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->Start(lpszRemoteAddress, usPort, bAsyncConnect);
}

HPSOCKET_API BOOL __HP_CALL HP_Client_StartWithBindAddress(HP_Client pClient, LPCTSTR lpszRemoteAddress, USHORT usPort, BOOL bAsyncConnect, LPCTSTR lpszBindAddress)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->Start(lpszRemoteAddress, usPort, bAsyncConnect, lpszBindAddress);
}

HPSOCKET_API BOOL __HP_CALL HP_Client_StartWithBindAddressAndLocalPort(HP_Client pClient, LPCTSTR lpszRemoteAddress, USHORT usPort, BOOL bAsyncConnect, LPCTSTR lpszBindAddress, USHORT usLocalPort)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->Start(lpszRemoteAddress, usPort, bAsyncConnect, lpszBindAddress, usLocalPort);
}

HPSOCKET_API BOOL __HP_CALL HP_Client_Stop(HP_Client pClient)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->Stop();
}

HPSOCKET_API BOOL __HP_CALL HP_Client_Send(HP_Client pClient, const BYTE* pBuffer, int iLength)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->Send(pBuffer, iLength);
}

HPSOCKET_API BOOL __HP_CALL HP_Client_SendPart(HP_Client pClient, const BYTE* pBuffer, int iLength, int iOffset)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->Send(pBuffer, iLength, iOffset);
}

HPSOCKET_API BOOL __HP_CALL HP_Client_SendPackets(HP_Client pClient, const WSABUF pBuffers[], int iCount)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->SendPackets(pBuffers, iCount);
}

HPSOCKET_API BOOL __HP_CALL HP_Client_PauseReceive(HP_Client pClient, BOOL bPause)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->PauseReceive(bPause);
}

/******************************************************************************/
/***************************** Client 属性访问方法 *****************************/

HPSOCKET_API void __HP_CALL HP_Client_SetExtra(HP_Client pClient, PVOID pExtra)
{
	C_HP_Object::ToSecond<IClient>(pClient)->SetExtra(pExtra);
}

HPSOCKET_API PVOID __HP_CALL HP_Client_GetExtra(HP_Client pClient)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->GetExtra();
}

HPSOCKET_API BOOL __HP_CALL HP_Client_IsSecure(HP_Client pClient)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->IsSecure();
}

HPSOCKET_API BOOL __HP_CALL HP_Client_HasStarted(HP_Client pClient)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->HasStarted();
}

HPSOCKET_API En_HP_ServiceState	__HP_CALL HP_Client_GetState(HP_Client pClient)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->GetState();
}

HPSOCKET_API En_HP_SocketError	__HP_CALL HP_Client_GetLastError(HP_Client pClient)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->GetLastError();
}

HPSOCKET_API LPCTSTR __HP_CALL HP_Client_GetLastErrorDesc(HP_Client pClient)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->GetLastErrorDesc();
}

HPSOCKET_API HP_CONNID __HP_CALL HP_Client_GetConnectionID(HP_Client pClient)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->GetConnectionID();
}

HPSOCKET_API BOOL __HP_CALL HP_Client_GetLocalAddress(HP_Client pClient, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->GetLocalAddress(lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API BOOL __HP_CALL HP_Client_GetRemoteHost(HP_Client pClient, TCHAR lpszHost[], int* piHostLen, USHORT* pusPort)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->GetRemoteHost(lpszHost, *piHostLen, *pusPort);
}

HPSOCKET_API BOOL __HP_CALL HP_Client_GetPendingDataLength(HP_Client pClient, int* piPending)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->GetPendingDataLength(*piPending);
}

HPSOCKET_API BOOL __HP_CALL HP_Client_IsPauseReceive(HP_Client pClient, BOOL* pbPaused)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->IsPauseReceive(*pbPaused);
}

HPSOCKET_API BOOL __HP_CALL HP_Client_IsConnected(HP_Client pClient)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->IsConnected();
}

HPSOCKET_API void __HP_CALL HP_Client_SetFreeBufferPoolSize(HP_Client pClient, DWORD dwFreeBufferPoolSize)
{
	C_HP_Object::ToSecond<IClient>(pClient)->SetFreeBufferPoolSize(dwFreeBufferPoolSize);
}

HPSOCKET_API void __HP_CALL HP_Client_SetFreeBufferPoolHold(HP_Client pClient, DWORD dwFreeBufferPoolHold)
{
	C_HP_Object::ToSecond<IClient>(pClient)->SetFreeBufferPoolHold(dwFreeBufferPoolHold);
}

HPSOCKET_API DWORD __HP_CALL HP_Client_GetFreeBufferPoolSize(HP_Client pClient)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->GetFreeBufferPoolSize();
}

HPSOCKET_API DWORD __HP_CALL HP_Client_GetFreeBufferPoolHold(HP_Client pClient)
{
	return C_HP_Object::ToSecond<IClient>(pClient)->GetFreeBufferPoolHold();
}

/**********************************************************************************/
/******************************* TCP Client 操作方法 *******************************/

HPSOCKET_API BOOL __HP_CALL HP_TcpClient_SendSmallFile(HP_Client pClient, LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail)
{
	return C_HP_Object::ToSecond<ITcpClient>(pClient)->SendSmallFile(lpszFileName, pHead, pTail);
}

/**********************************************************************************/
/***************************** TCP Client 属性访问方法 *****************************/

HPSOCKET_API void __HP_CALL HP_TcpClient_SetSocketBufferSize(HP_TcpClient pClient, DWORD dwSocketBufferSize)
{
	C_HP_Object::ToSecond<ITcpClient>(pClient)->SetSocketBufferSize(dwSocketBufferSize);
}

HPSOCKET_API void __HP_CALL HP_TcpClient_SetKeepAliveTime(HP_TcpClient pClient, DWORD dwKeepAliveTime)
{
	C_HP_Object::ToSecond<ITcpClient>(pClient)->SetKeepAliveTime(dwKeepAliveTime);
}

HPSOCKET_API void __HP_CALL HP_TcpClient_SetKeepAliveInterval(HP_TcpClient pClient, DWORD dwKeepAliveInterval)
{
	C_HP_Object::ToSecond<ITcpClient>(pClient)->SetKeepAliveInterval(dwKeepAliveInterval);
}

HPSOCKET_API DWORD __HP_CALL HP_TcpClient_GetSocketBufferSize(HP_TcpClient pClient)
{
	return C_HP_Object::ToSecond<ITcpClient>(pClient)->GetSocketBufferSize();
}

HPSOCKET_API DWORD __HP_CALL HP_TcpClient_GetKeepAliveTime(HP_TcpClient pClient)
{
	return C_HP_Object::ToSecond<ITcpClient>(pClient)->GetKeepAliveTime();
}

HPSOCKET_API DWORD __HP_CALL HP_TcpClient_GetKeepAliveInterval(HP_TcpClient pClient)
{
	return C_HP_Object::ToSecond<ITcpClient>(pClient)->GetKeepAliveInterval();
}

/**********************************************************************************/
/***************************** UDP Client 属性访问方法 *****************************/

HPSOCKET_API void __HP_CALL HP_UdpClient_SetMaxDatagramSize(HP_UdpClient pClient, DWORD dwMaxDatagramSize)
{
	C_HP_Object::ToSecond<IUdpClient>(pClient)->SetMaxDatagramSize(dwMaxDatagramSize);
}

HPSOCKET_API DWORD __HP_CALL HP_UdpClient_GetMaxDatagramSize(HP_UdpClient pClient)
{
	return C_HP_Object::ToSecond<IUdpClient>(pClient)->GetMaxDatagramSize();
}

HPSOCKET_API void __HP_CALL HP_UdpClient_SetDetectAttempts(HP_UdpClient pClient, DWORD dwDetectAttempts)
{
	C_HP_Object::ToSecond<IUdpClient>(pClient)->SetDetectAttempts(dwDetectAttempts);
}

HPSOCKET_API void __HP_CALL HP_UdpClient_SetDetectInterval(HP_UdpClient pClient, DWORD dwDetectInterval)
{
	C_HP_Object::ToSecond<IUdpClient>(pClient)->SetDetectInterval(dwDetectInterval);
}

HPSOCKET_API DWORD __HP_CALL HP_UdpClient_GetDetectAttempts(HP_UdpClient pClient)
{
	return C_HP_Object::ToSecond<IUdpClient>(pClient)->GetDetectAttempts();
}

HPSOCKET_API DWORD __HP_CALL HP_UdpClient_GetDetectInterval(HP_UdpClient pClient)
{
	return C_HP_Object::ToSecond<IUdpClient>(pClient)->GetDetectInterval();
}

/**********************************************************************************/
/****************************** UDP Cast 属性访问方法 ******************************/

HPSOCKET_API void __HP_CALL HP_UdpCast_SetMaxDatagramSize(HP_UdpCast pCast, DWORD dwMaxDatagramSize)
{
	C_HP_Object::ToSecond<IUdpCast>(pCast)->SetMaxDatagramSize(dwMaxDatagramSize);
}

HPSOCKET_API DWORD __HP_CALL HP_UdpCast_GetMaxDatagramSize(HP_UdpCast pCast)
{
	return C_HP_Object::ToSecond<IUdpCast>(pCast)->GetMaxDatagramSize();
}

HPSOCKET_API BOOL __HP_CALL HP_UdpCast_GetRemoteAddress(HP_UdpCast pCast, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToSecond<IUdpCast>(pCast)->GetRemoteAddress(lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API void __HP_CALL HP_UdpCast_SetReuseAddress(HP_UdpCast pCast, BOOL bReuseAddress)
{
	C_HP_Object::ToSecond<IUdpCast>(pCast)->SetReuseAddress(bReuseAddress);
}

HPSOCKET_API BOOL __HP_CALL HP_UdpCast_IsReuseAddress(HP_UdpCast pCast)
{
	return C_HP_Object::ToSecond<IUdpCast>(pCast)->IsReuseAddress();
}

HPSOCKET_API void __HP_CALL HP_UdpCast_SetCastMode(HP_UdpCast pCast, En_HP_CastMode enCastMode)
{
	C_HP_Object::ToSecond<IUdpCast>(pCast)->SetCastMode(enCastMode);
}

HPSOCKET_API En_HP_CastMode __HP_CALL HP_UdpCast_GetCastMode(HP_UdpCast pCast)
{
	return C_HP_Object::ToSecond<IUdpCast>(pCast)->GetCastMode();
}

HPSOCKET_API void __HP_CALL HP_UdpCast_SetMultiCastTtl(HP_UdpCast pCast, int iMCTtl)
{
	C_HP_Object::ToSecond<IUdpCast>(pCast)->SetMultiCastTtl(iMCTtl);
}

HPSOCKET_API int __HP_CALL HP_UdpCast_GetMultiCastTtl(HP_UdpCast pCast)
{
	return C_HP_Object::ToSecond<IUdpCast>(pCast)->GetMultiCastTtl();
}

HPSOCKET_API void __HP_CALL HP_UdpCast_SetMultiCastLoop(HP_UdpCast pCast, BOOL bMCLoop)
{
	C_HP_Object::ToSecond<IUdpCast>(pCast)->SetMultiCastLoop(bMCLoop);
}

HPSOCKET_API BOOL __HP_CALL HP_UdpCast_IsMultiCastLoop(HP_UdpCast pCast)
{
	return C_HP_Object::ToSecond<IUdpCast>(pCast)->IsMultiCastLoop();
}

/***************************************************************************************/
/***************************** TCP Pull Server 组件操作方法 *****************************/

HPSOCKET_API En_HP_FetchResult __HP_CALL HP_TcpPullServer_Fetch(HP_TcpPullServer pServer, HP_CONNID dwConnID, BYTE* pData, int iLength)
{
	return C_HP_Object::ToFirst<IPullSocket>(pServer)->Fetch(dwConnID, pData, iLength);
}

HPSOCKET_API En_HP_FetchResult __HP_CALL HP_TcpPullServer_Peek(HP_TcpPullServer pServer, HP_CONNID dwConnID, BYTE* pData, int iLength)
{
	return C_HP_Object::ToFirst<IPullSocket>(pServer)->Peek(dwConnID, pData, iLength);
}

/***************************************************************************************/
/***************************** TCP Pull Server 属性访问方法 *****************************/

/***************************************************************************************/
/***************************** TCP Pull Agent 组件操作方法 *****************************/

HPSOCKET_API En_HP_FetchResult __HP_CALL HP_TcpPullAgent_Fetch(HP_TcpPullAgent pAgent, HP_CONNID dwConnID, BYTE* pData, int iLength)
{
	return C_HP_Object::ToFirst<IPullSocket>(pAgent)->Fetch(dwConnID, pData, iLength);
}

HPSOCKET_API En_HP_FetchResult __HP_CALL HP_TcpPullAgent_Peek(HP_TcpPullAgent pAgent, HP_CONNID dwConnID, BYTE* pData, int iLength)
{
	return C_HP_Object::ToFirst<IPullSocket>(pAgent)->Peek(dwConnID, pData, iLength);
}

/***************************************************************************************/
/***************************** TCP Pull Agent 属性访问方法 *****************************/

/***************************************************************************************/
/***************************** TCP Pull Client 组件操作方法 *****************************/

HPSOCKET_API En_HP_FetchResult __HP_CALL HP_TcpPullClient_Fetch(HP_TcpPullClient pClient, BYTE* pData, int iLength)
{
	return C_HP_Object::ToFirst<IPullClient>(pClient)->Fetch(pData, iLength);
}

HPSOCKET_API En_HP_FetchResult __HP_CALL HP_TcpPullClient_Peek(HP_TcpPullClient pClient, BYTE* pData, int iLength)
{
	return C_HP_Object::ToFirst<IPullClient>(pClient)->Peek(pData, iLength);
}

/***************************************************************************************/
/***************************** TCP Pull Client 属性访问方法 *****************************/

/***************************************************************************************/
/***************************** TCP Pack Server 组件操作方法 *****************************/

/***************************************************************************************/
/***************************** TCP Pack Server 属性访问方法 *****************************/

HPSOCKET_API void __HP_CALL HP_TcpPackServer_SetMaxPackSize(HP_TcpPackServer pServer, DWORD dwMaxPackSize)
{
	C_HP_Object::ToFirst<IPackSocket>(pServer)->SetMaxPackSize(dwMaxPackSize);
}

HPSOCKET_API void __HP_CALL HP_TcpPackServer_SetPackHeaderFlag(HP_TcpPackServer pServer, USHORT usPackHeaderFlag)
{
	C_HP_Object::ToFirst<IPackSocket>(pServer)->SetPackHeaderFlag(usPackHeaderFlag);
}

HPSOCKET_API DWORD __HP_CALL HP_TcpPackServer_GetMaxPackSize(HP_TcpPackServer pServer)
{
	return C_HP_Object::ToFirst<IPackSocket>(pServer)->GetMaxPackSize();
}

HPSOCKET_API USHORT __HP_CALL HP_TcpPackServer_GetPackHeaderFlag(HP_TcpPackServer pServer)
{
	return C_HP_Object::ToFirst<IPackSocket>(pServer)->GetPackHeaderFlag();
}

/***************************************************************************************/
/***************************** TCP Pack Agent 组件操作方法 *****************************/

/***************************************************************************************/
/***************************** TCP Pack Agent 属性访问方法 *****************************/

HPSOCKET_API void __HP_CALL HP_TcpPackAgent_SetMaxPackSize(HP_TcpPackAgent pAgent, DWORD dwMaxPackSize)
{
	C_HP_Object::ToFirst<IPackSocket>(pAgent)->SetMaxPackSize(dwMaxPackSize);
}

HPSOCKET_API void __HP_CALL HP_TcpPackAgent_SetPackHeaderFlag(HP_TcpPackAgent pAgent, USHORT usPackHeaderFlag)
{
	C_HP_Object::ToFirst<IPackSocket>(pAgent)->SetPackHeaderFlag(usPackHeaderFlag);
}

HPSOCKET_API DWORD __HP_CALL HP_TcpPackAgent_GetMaxPackSize(HP_TcpPackAgent pAgent)
{
	return C_HP_Object::ToFirst<IPackSocket>(pAgent)->GetMaxPackSize();
}

HPSOCKET_API USHORT __HP_CALL HP_TcpPackAgent_GetPackHeaderFlag(HP_TcpPackAgent pAgent)
{
	return C_HP_Object::ToFirst<IPackSocket>(pAgent)->GetPackHeaderFlag();
}

/***************************************************************************************/
/***************************** TCP Pack Client 组件操作方法 *****************************/

/***************************************************************************************/
/***************************** TCP Pack Client 属性访问方法 *****************************/

HPSOCKET_API void __HP_CALL HP_TcpPackClient_SetMaxPackSize(HP_TcpPackClient pClient, DWORD dwMaxPackSize)
{
	C_HP_Object::ToFirst<IPackClient>(pClient)->SetMaxPackSize(dwMaxPackSize);
}

HPSOCKET_API void __HP_CALL HP_TcpPackClient_SetPackHeaderFlag(HP_TcpPackClient pClient, USHORT usPackHeaderFlag)
{
	C_HP_Object::ToFirst<IPackClient>(pClient)->SetPackHeaderFlag(usPackHeaderFlag);
}

HPSOCKET_API DWORD __HP_CALL HP_TcpPackClient_GetMaxPackSize(HP_TcpPackClient pClient)
{
	return C_HP_Object::ToFirst<IPackClient>(pClient)->GetMaxPackSize();
}

HPSOCKET_API USHORT __HP_CALL HP_TcpPackClient_GetPackHeaderFlag(HP_TcpPackClient pClient)
{
	return C_HP_Object::ToFirst<IPackClient>(pClient)->GetPackHeaderFlag();
}

/*****************************************************************************************************************************************************/
/*************************************************************** Global Function Exports *************************************************************/
/*****************************************************************************************************************************************************/

HPSOCKET_API DWORD __HP_CALL HP_GetHPSocketVersion()
{
	return ::GetHPSocketVersion();
}

HPSOCKET_API LPCTSTR __HP_CALL HP_GetSocketErrorDesc(En_HP_SocketError enCode)
{
	return ::GetSocketErrorDesc(enCode);
}

HPSOCKET_API DWORD __HP_CALL SYS_GetLastError()
{
	return ::GetLastError();
}

HPSOCKET_API LPCSTR __HP_CALL SYS_GetLastErrorStr()
{
	return ::GetLastErrorStr();
}

HPSOCKET_API int __HP_CALL SYS_SetSocketOption(SOCKET sock, int level, int name, LPVOID val, int len)
{
	return ::SSO_SetSocketOption(sock, level, name, val, len);
}

HPSOCKET_API int __HP_CALL SYS_GetSocketOption(SOCKET sock, int level, int name, LPVOID val, int* len)
{
	return ::SSO_GetSocketOption(sock, level, name, val, len);
}

HPSOCKET_API int __HP_CALL SYS_IoctlSocket(SOCKET sock, long cmd, ULONG* arg)
{
	return ::SSO_IoctlSocket(sock, cmd, arg);
}

HPSOCKET_API BOOL __HP_CALL SYS_fcntl_SETFL(FD fd, INT fl, BOOL bSet)
{
	return ::fcntl_SETFL(fd, fl, bSet);
}

HPSOCKET_API int __HP_CALL SYS_SSO_NoBlock(SOCKET sock, BOOL bNoBlock)
{
	return ::SSO_NoBlock(sock, bNoBlock);
}

HPSOCKET_API int __HP_CALL SYS_SSO_NoDelay(SOCKET sock, BOOL bNoDelay)
{
	return ::SSO_NoDelay(sock, bNoDelay);
}

HPSOCKET_API int __HP_CALL SYS_SSO_DontLinger(SOCKET sock, BOOL bDont)
{
	return ::SSO_DontLinger(sock, bDont);
}

HPSOCKET_API int __HP_CALL SYS_SSO_Linger(SOCKET sock, USHORT l_onoff, USHORT l_linger)
{
	return ::SSO_Linger(sock, l_onoff, l_linger);
}

HPSOCKET_API int __HP_CALL SYS_SSO_RecvBuffSize(SOCKET sock, int size)
{
	return ::SSO_RecvBuffSize(sock, size);
}

HPSOCKET_API int __HP_CALL SYS_SSO_SendBuffSize(SOCKET sock, int size)
{
	return ::SSO_SendBuffSize(sock, size);
}

HPSOCKET_API int __HP_CALL SYS_SSO_ReuseAddress(SOCKET sock, BOOL bReuse)
{
	return ::SSO_ReuseAddress(sock, bReuse);
}

HPSOCKET_API BOOL __HP_CALL SYS_GetSocketLocalAddress(SOCKET socket, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
{
	return ::GetSocketLocalAddress(socket, lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API BOOL __HP_CALL SYS_GetSocketRemoteAddress(SOCKET socket, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
{
	return ::GetSocketRemoteAddress(socket, lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API BOOL __HP_CALL SYS_EnumHostIPAddresses(LPCTSTR lpszHost, En_HP_IPAddrType enType, HP_LPTIPAddr** lpppIPAddr, int* piIPAddrCount)
{
	return ::EnumHostIPAddresses(lpszHost, enType, lpppIPAddr, *piIPAddrCount);
}

HPSOCKET_API BOOL __HP_CALL SYS_FreeHostIPAddresses(HP_LPTIPAddr* lppIPAddr)
{
	return ::FreeHostIPAddresses(lppIPAddr);
}

HPSOCKET_API BOOL __HP_CALL SYS_IsIPAddress(LPCTSTR lpszAddress, En_HP_IPAddrType* penType)
{
	return ::IsIPAddress(lpszAddress, penType);
}

HPSOCKET_API BOOL __HP_CALL SYS_GetIPAddress(LPCTSTR lpszHost, TCHAR lpszIP[], int* piIPLenth, En_HP_IPAddrType* penType)
{
	return ::GetIPAddress(lpszHost, lpszIP, *piIPLenth, *penType);
}

HPSOCKET_API ULONGLONG __HP_CALL SYS_NToH64(ULONGLONG value)
{
	return ::NToH64(value);
}

HPSOCKET_API ULONGLONG __HP_CALL SYS_HToN64(ULONGLONG value)
{
	return ::HToN64(value);
}

HPSOCKET_API LPBYTE __HP_CALL SYS_Malloc(int size)
{
	return MALLOC(BYTE, size);
}

HPSOCKET_API LPBYTE __HP_CALL SYS_Realloc(LPBYTE p, int size)
{
	return REALLOC(p, BYTE, size);
}

HPSOCKET_API VOID __HP_CALL SYS_Free(LPBYTE p)
{
	FREE(p);
}

#ifdef _ICONV_SUPPORT

HPSOCKET_API BOOL __HP_CALL SYS_CharsetConvert(LPCSTR lpszFromCharset, LPCSTR lpszToCharset, LPCSTR lpszInBuf, int iInBufLen, LPSTR lpszOutBuf, int* piOutBufLen)
{
	return ::CharsetConvert(lpszFromCharset, lpszToCharset, lpszInBuf, iInBufLen, lpszOutBuf, *piOutBufLen);
}

HPSOCKET_API BOOL __HP_CALL SYS_GbkToUnicode(const char szSrc[], WCHAR szDest[], int* piDestLength)
{
	return ::GbkToUnicode(szSrc, szDest, *piDestLength);
}

HPSOCKET_API BOOL __HP_CALL SYS_UnicodeToGbk(const WCHAR szSrc[], char szDest[], int* piDestLength)
{
	return ::UnicodeToGbk(szSrc, szDest, *piDestLength);
}

HPSOCKET_API BOOL __HP_CALL SYS_Utf8ToUnicode(const char szSrc[], WCHAR szDest[], int* piDestLength)
{
	return ::Utf8ToUnicode(szSrc, szDest, *piDestLength);
}

HPSOCKET_API BOOL __HP_CALL SYS_UnicodeToUtf8(const WCHAR szSrc[], char szDest[], int* piDestLength)
{
	return ::UnicodeToUtf8(szSrc, szDest, *piDestLength);
}

HPSOCKET_API BOOL __HP_CALL SYS_GbkToUtf8(const char szSrc[], char szDest[], int* piDestLength)
{
	return ::GbkToUtf8(szSrc, szDest, *piDestLength);
}

HPSOCKET_API BOOL __HP_CALL SYS_Utf8ToGbk(const char szSrc[], char szDest[], int* piDestLength)
{
	return ::Utf8ToGbk(szSrc, szDest, *piDestLength);
}

#endif

#ifdef _ZLIB_SUPPORT

HPSOCKET_API int __HP_CALL SYS_Compress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen)
{
	return ::Compress(lpszSrc, dwSrcLen, lpszDest, *pdwDestLen);
}

HPSOCKET_API int __HP_CALL SYS_CompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen, int iLevel, int iMethod, int iWindowBits, int iMemLevel, int iStrategy)
{
	return ::CompressEx(lpszSrc, dwSrcLen, lpszDest, *pdwDestLen, iLevel, iMethod, iWindowBits, iMemLevel, iStrategy);
}

HPSOCKET_API int __HP_CALL SYS_Uncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen)
{
	return ::Uncompress(lpszSrc, dwSrcLen, lpszDest, *pdwDestLen);
}

HPSOCKET_API int __HP_CALL SYS_UncompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen, int iWindowBits)
{
	return ::UncompressEx(lpszSrc, dwSrcLen, lpszDest, *pdwDestLen, iWindowBits);
}

HPSOCKET_API DWORD __HP_CALL SYS_GuessCompressBound(DWORD dwSrcLen, BOOL bGZip)
{
	return ::GuessCompressBound(dwSrcLen, bGZip);
}

HPSOCKET_API int __HP_CALL SYS_GZipCompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen)
{
	return ::GZipCompress(lpszSrc, dwSrcLen, lpszDest, *pdwDestLen);
}

HPSOCKET_API int __HP_CALL SYS_GZipUncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen)
{
	return ::GZipUncompress(lpszSrc, dwSrcLen, lpszDest, *pdwDestLen);
}

HPSOCKET_API DWORD __HP_CALL SYS_GZipGuessUncompressBound(const BYTE* lpszSrc, DWORD dwSrcLen)
{
	return ::GZipGuessUncompressBound(lpszSrc, dwSrcLen);
}

#endif

HPSOCKET_API DWORD __HP_CALL SYS_GuessBase64EncodeBound(DWORD dwSrcLen)
{
	return ::GuessBase64EncodeBound(dwSrcLen);
}

HPSOCKET_API DWORD __HP_CALL SYS_GuessBase64DecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen)
{
	return ::GuessBase64DecodeBound(lpszSrc, dwSrcLen);
}

HPSOCKET_API int __HP_CALL SYS_Base64Encode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen)
{
	return ::Base64Encode(lpszSrc, dwSrcLen, lpszDest, *pdwDestLen);
}

HPSOCKET_API int __HP_CALL SYS_Base64Decode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen)
{
	return ::Base64Decode(lpszSrc, dwSrcLen, lpszDest, *pdwDestLen);
}

HPSOCKET_API DWORD __HP_CALL SYS_GuessUrlEncodeBound(const BYTE* lpszSrc, DWORD dwSrcLen)
{
	return ::GuessUrlEncodeBound(lpszSrc, dwSrcLen);
}

HPSOCKET_API DWORD __HP_CALL SYS_GuessUrlDecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen)
{
	return ::GuessUrlDecodeBound(lpszSrc, dwSrcLen);
}

HPSOCKET_API int __HP_CALL SYS_UrlEncode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen)
{
	return ::UrlEncode(lpszSrc, dwSrcLen, lpszDest, *pdwDestLen);
}

HPSOCKET_API int __HP_CALL SYS_UrlDecode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD* pdwDestLen)
{
	return ::UrlDecode(lpszSrc, dwSrcLen, lpszDest, *pdwDestLen);
}

/*****************************************************************************************************************************************************/
/******************************************************************** HTTP Exports *******************************************************************/
/*****************************************************************************************************************************************************/

#ifdef _HTTP_SUPPORT

typedef C_HP_ObjectT<CHttpServer, IHttpServerListener, sizeof(IComplexHttpResponder)>	C_HP_HttpServer;
typedef C_HP_ObjectT<CHttpAgent, IHttpAgentListener, sizeof(IComplexHttpRequester)>		C_HP_HttpAgent;
typedef C_HP_ObjectT<CHttpClient, IHttpClientListener, sizeof(IHttpRequester)>			C_HP_HttpClient;
typedef C_HP_ObjectT<CHttpSyncClient, IHttpClientListener, sizeof(IHttpSyncRequester)>	C_HP_HttpSyncClient;

/****************************************************/
/***************** HTTP 对象创建函数 *****************/

HPSOCKET_API HP_HttpServer __HP_CALL Create_HP_HttpServer(HP_HttpServerListener pListener)
{
	return (HP_HttpServer)(new C_HP_HttpServer((IHttpServerListener*)pListener));
}

HPSOCKET_API HP_HttpAgent __HP_CALL Create_HP_HttpAgent(HP_HttpAgentListener pListener)
{
	return (HP_HttpAgent)(new C_HP_HttpAgent((IHttpAgentListener*)pListener));
}

HPSOCKET_API HP_HttpClient __HP_CALL Create_HP_HttpClient(HP_HttpClientListener pListener)
{
	return (HP_HttpClient)(new C_HP_HttpClient((IHttpClientListener*)pListener));
}

HPSOCKET_API HP_HttpSyncClient __HP_CALL Create_HP_HttpSyncClient(HP_HttpClientListener pListener)
{
	return (HP_HttpSyncClient)(new C_HP_HttpSyncClient((IHttpClientListener*)pListener));
}

HPSOCKET_API void __HP_CALL Destroy_HP_HttpServer(HP_HttpServer pServer)
{
	delete (C_HP_HttpServer*)pServer;
}

HPSOCKET_API void __HP_CALL Destroy_HP_HttpAgent(HP_HttpAgent pAgent)
{
	delete (C_HP_HttpAgent*)pAgent;
}

HPSOCKET_API void __HP_CALL Destroy_HP_HttpClient(HP_HttpClient pClient)
{
	delete (C_HP_HttpClient*)pClient;
}

HPSOCKET_API void __HP_CALL Destroy_HP_HttpSyncClient(HP_HttpSyncClient pClient)
{
	delete (C_HP_HttpSyncClient*)pClient;
}

HPSOCKET_API HP_HttpServerListener __HP_CALL Create_HP_HttpServerListener()
{
	return (HP_HttpServerListener)(new C_HP_HttpServerListener);
}

HPSOCKET_API HP_HttpAgentListener __HP_CALL Create_HP_HttpAgentListener()
{
	return (HP_HttpAgentListener)(new C_HP_HttpAgentListener);
}

HPSOCKET_API HP_HttpClientListener __HP_CALL Create_HP_HttpClientListener()
{
	return (HP_HttpClientListener)(new C_HP_HttpClientListener);
}

HPSOCKET_API void __HP_CALL Destroy_HP_HttpServerListener(HP_HttpServerListener pListener)
{
	delete (C_HP_HttpServerListener*)pListener;
}

HPSOCKET_API void __HP_CALL Destroy_HP_HttpAgentListener(HP_HttpAgentListener pListener)
{
	delete (C_HP_HttpAgentListener*)pListener;
}

HPSOCKET_API void __HP_CALL Destroy_HP_HttpClientListener(HP_HttpClientListener pListener)
{
	delete (C_HP_HttpClientListener*)pListener;
}

/**********************************************************************************/
/*************************** HTTP Server 回调函数设置方法 **************************/

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnMessageBegin(HP_HttpServerListener pListener, HP_FN_HttpServer_OnMessageBegin fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnHttp.m_fnOnMessageBegin = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnRequestLine(HP_HttpServerListener pListener, HP_FN_HttpServer_OnRequestLine fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnHttp.m_fnOnRequestLine = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnHeader(HP_HttpServerListener pListener, HP_FN_HttpServer_OnHeader fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnHttp.m_fnOnHeader = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnHeadersComplete(HP_HttpServerListener pListener, HP_FN_HttpServer_OnHeadersComplete fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnHttp.m_fnOnHeadersComplete = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnBody(HP_HttpServerListener pListener, HP_FN_HttpServer_OnBody fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnHttp.m_fnOnBody = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnChunkHeader(HP_HttpServerListener pListener, HP_FN_HttpServer_OnChunkHeader fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnHttp.m_fnOnChunkHeader = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnChunkComplete(HP_HttpServerListener pListener, HP_FN_HttpServer_OnChunkComplete fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnHttp.m_fnOnChunkComplete = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnMessageComplete(HP_HttpServerListener pListener, HP_FN_HttpServer_OnMessageComplete fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnHttp.m_fnOnMessageComplete = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnUpgrade(HP_HttpServerListener pListener, HP_FN_HttpServer_OnUpgrade fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnHttp.m_fnOnUpgrade = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnParseError(HP_HttpServerListener pListener, HP_FN_HttpServer_OnParseError fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnHttp.m_fnOnParseError = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnWSMessageHeader(HP_HttpServerListener pListener, HP_FN_HttpServer_OnWSMessageHeader fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnHttp.m_fnOnWSMessageHeader = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnWSMessageBody(HP_HttpServerListener pListener, HP_FN_HttpServer_OnWSMessageBody fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnHttp.m_fnOnWSMessageBody = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnWSMessageComplete(HP_HttpServerListener pListener, HP_FN_HttpServer_OnWSMessageComplete fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnHttp.m_fnOnWSMessageComplete = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnPrepareListen(HP_HttpServerListener pListener, HP_FN_HttpServer_OnPrepareListen fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnServer.m_fnOnPrepareListen = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnAccept(HP_HttpServerListener pListener, HP_FN_HttpServer_OnAccept fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnServer.m_fnOnAccept = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnHandShake(HP_HttpServerListener pListener, HP_FN_HttpServer_OnHandShake fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnServer.m_fnOnHandShake = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnSend(HP_HttpServerListener pListener, HP_FN_HttpServer_OnSend fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnServer.m_fnOnSend = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnReceive(HP_HttpServerListener pListener, HP_FN_HttpServer_OnReceive fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnServer.m_fnOnReceive = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnClose(HP_HttpServerListener pListener, HP_FN_HttpServer_OnClose fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnServer.m_fnOnClose = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpServer_OnShutdown(HP_HttpServerListener pListener, HP_FN_HttpServer_OnShutdown fn)
{
	((C_HP_HttpServerListener*)pListener)->m_lsnServer.m_fnOnShutdown = fn;
}

/**********************************************************************************/
/**************************** HTTP Agent 回调函数设置方法 **************************/

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnMessageBegin(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnMessageBegin fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnHttp.m_fnOnMessageBegin = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnStatusLine(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnStatusLine fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnHttp.m_fnOnStatusLine = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnHeader(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnHeader fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnHttp.m_fnOnHeader = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnHeadersComplete(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnHeadersComplete fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnHttp.m_fnOnHeadersComplete = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnBody(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnBody fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnHttp.m_fnOnBody = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnChunkHeader(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnChunkHeader fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnHttp.m_fnOnChunkHeader = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnChunkComplete(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnChunkComplete fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnHttp.m_fnOnChunkComplete = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnMessageComplete(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnMessageComplete fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnHttp.m_fnOnMessageComplete = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnUpgrade(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnUpgrade fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnHttp.m_fnOnUpgrade = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnParseError(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnParseError fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnHttp.m_fnOnParseError = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnWSMessageHeader(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnWSMessageHeader fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnHttp.m_fnOnWSMessageHeader = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnWSMessageBody(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnWSMessageBody fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnHttp.m_fnOnWSMessageBody = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnWSMessageComplete(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnWSMessageComplete fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnHttp.m_fnOnWSMessageComplete = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnPrepareConnect(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnPrepareConnect fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnAgent.m_fnOnPrepareConnect = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnConnect(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnConnect fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnAgent.m_fnOnConnect = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnHandShake(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnHandShake fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnAgent.m_fnOnHandShake = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnReceive(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnReceive fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnAgent.m_fnOnReceive = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnSend(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnSend fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnAgent.m_fnOnSend = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnClose(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnClose fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnAgent.m_fnOnClose = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpAgent_OnShutdown(HP_HttpAgentListener pListener, HP_FN_HttpAgent_OnShutdown fn)
{
	((C_HP_HttpAgentListener*)pListener)->m_lsnAgent.m_fnOnShutdown = fn;
}

/**********************************************************************************/
/*************************** HTTP Client 回调函数设置方法 **************************/

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnMessageBegin(HP_HttpClientListener pListener, HP_FN_HttpClient_OnMessageBegin fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnHttp.m_fnOnMessageBegin = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnStatusLine(HP_HttpClientListener pListener, HP_FN_HttpClient_OnStatusLine fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnHttp.m_fnOnStatusLine = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnHeader(HP_HttpClientListener pListener, HP_FN_HttpClient_OnHeader fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnHttp.m_fnOnHeader = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnHeadersComplete(HP_HttpClientListener pListener, HP_FN_HttpClient_OnHeadersComplete fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnHttp.m_fnOnHeadersComplete = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnBody(HP_HttpClientListener pListener, HP_FN_HttpClient_OnBody fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnHttp.m_fnOnBody = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnChunkHeader(HP_HttpClientListener pListener, HP_FN_HttpClient_OnChunkHeader fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnHttp.m_fnOnChunkHeader = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnChunkComplete(HP_HttpClientListener pListener, HP_FN_HttpClient_OnChunkComplete fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnHttp.m_fnOnChunkComplete = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnMessageComplete(HP_HttpClientListener pListener, HP_FN_HttpClient_OnMessageComplete fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnHttp.m_fnOnMessageComplete = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnUpgrade(HP_HttpClientListener pListener, HP_FN_HttpClient_OnUpgrade fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnHttp.m_fnOnUpgrade = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnParseError(HP_HttpClientListener pListener, HP_FN_HttpClient_OnParseError fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnHttp.m_fnOnParseError = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnWSMessageHeader(HP_HttpClientListener pListener, HP_FN_HttpClient_OnWSMessageHeader fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnHttp.m_fnOnWSMessageHeader = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnWSMessageBody(HP_HttpClientListener pListener, HP_FN_HttpClient_OnWSMessageBody fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnHttp.m_fnOnWSMessageBody = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnWSMessageComplete(HP_HttpClientListener pListener, HP_FN_HttpClient_OnWSMessageComplete fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnHttp.m_fnOnWSMessageComplete = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnPrepareConnect(HP_HttpClientListener pListener, HP_FN_HttpClient_OnPrepareConnect fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnClient.m_fnOnPrepareConnect = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnConnect(HP_HttpClientListener pListener, HP_FN_HttpClient_OnConnect fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnClient.m_fnOnConnect = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnHandShake(HP_HttpClientListener pListener, HP_FN_HttpClient_OnHandShake fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnClient.m_fnOnHandShake = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnReceive(HP_HttpClientListener pListener, HP_FN_HttpClient_OnReceive fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnClient.m_fnOnReceive = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnSend(HP_HttpClientListener pListener, HP_FN_HttpClient_OnSend fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnClient.m_fnOnSend = fn;
}

HPSOCKET_API void __HP_CALL HP_Set_FN_HttpClient_OnClose(HP_HttpClientListener pListener, HP_FN_HttpClient_OnClose fn)
{
	((C_HP_HttpClientListener*)pListener)->m_lsnClient.m_fnOnClose = fn;
}

/**************************************************************************/
/*************************** HTTP Server 操作方法 **************************/

HPSOCKET_API BOOL __HP_CALL HP_HttpServer_SendResponse(HP_HttpServer pServer, HP_CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pData, int iLength)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->SendResponse(dwConnID, usStatusCode, lpszDesc, lpHeaders, iHeaderCount, pData, iLength);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpServer_SendLocalFile(HP_HttpServer pServer, HP_CONNID dwConnID, LPCSTR lpszFileName, USHORT usStatusCode, LPCSTR lpszDesc, const HP_THeader lpHeaders[], int iHeaderCount)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->SendLocalFile(dwConnID, lpszFileName, usStatusCode, lpszDesc, lpHeaders, iHeaderCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpServer_SendWSMessage(HP_HttpServer pServer, HP_CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], BYTE* pData, int iLength, ULONGLONG ullBodyLen)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->SendWSMessage(dwConnID, bFinal, iReserved, iOperationCode, lpszMask, pData, iLength, ullBodyLen);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpServer_Release(HP_HttpServer pServer, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->Release(dwConnID);
}

/******************************************************************************/
/*************************** HTTP Server 属性访问方法 **************************/

HPSOCKET_API void __HP_CALL HP_HttpServer_SetReleaseDelay(HP_HttpServer pServer, DWORD dwReleaseDelay)
{
	C_HP_Object::ToFirst<IHttpServer>(pServer)->SetReleaseDelay(dwReleaseDelay);
}

HPSOCKET_API DWORD __HP_CALL HP_HttpServer_GetReleaseDelay(HP_HttpServer pServer)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetReleaseDelay();
}

HPSOCKET_API USHORT __HP_CALL HP_HttpServer_GetUrlFieldSet(HP_HttpServer pServer, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetUrlFieldSet(dwConnID);
}


HPSOCKET_API LPCSTR __HP_CALL HP_HttpServer_GetUrlField(HP_HttpServer pServer, HP_CONNID dwConnID, En_HP_HttpUrlField enField)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetUrlField(dwConnID, enField);
}

HPSOCKET_API LPCSTR __HP_CALL HP_HttpServer_GetMethod(HP_HttpServer pServer, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetMethod(dwConnID);
}

HPSOCKET_API void __HP_CALL HP_HttpServer_SetLocalVersion(HP_HttpServer pServer, En_HP_HttpVersion usVersion)
{
	C_HP_Object::ToFirst<IHttpServer>(pServer)->SetLocalVersion(usVersion);
}

HPSOCKET_API En_HP_HttpVersion __HP_CALL HP_HttpServer_GetLocalVersion(HP_HttpServer pServer)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetLocalVersion();
}

HPSOCKET_API BOOL __HP_CALL HP_HttpServer_IsUpgrade(HP_HttpServer pServer, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->IsUpgrade(dwConnID);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpServer_IsKeepAlive(HP_HttpServer pServer, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->IsKeepAlive(dwConnID);
}

HPSOCKET_API USHORT __HP_CALL HP_HttpServer_GetVersion(HP_HttpServer pServer, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetVersion(dwConnID);
}

HPSOCKET_API LPCSTR __HP_CALL HP_HttpServer_GetHost(HP_HttpServer pServer, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetHost(dwConnID);
}

HPSOCKET_API ULONGLONG __HP_CALL HP_HttpServer_GetContentLength(HP_HttpServer pServer, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetContentLength(dwConnID);
}

HPSOCKET_API LPCSTR __HP_CALL HP_HttpServer_GetContentType(HP_HttpServer pServer, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetContentType(dwConnID);
}

HPSOCKET_API LPCSTR __HP_CALL HP_HttpServer_GetContentEncoding(HP_HttpServer pServer, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetContentEncoding(dwConnID);
}

HPSOCKET_API LPCSTR __HP_CALL HP_HttpServer_GetTransferEncoding(HP_HttpServer pServer, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetTransferEncoding(dwConnID);
}

HPSOCKET_API En_HP_HttpUpgradeType __HP_CALL HP_HttpServer_GetUpgradeType(HP_HttpServer pServer, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetUpgradeType(dwConnID);
}

HPSOCKET_API USHORT __HP_CALL HP_HttpServer_GetParseErrorCode(HP_HttpServer pServer, HP_CONNID dwConnID, LPCSTR* lpszErrorDesc)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetParseErrorCode(dwConnID, lpszErrorDesc);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpServer_GetHeader(HP_HttpServer pServer, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetHeader(dwConnID, lpszName, lpszValue);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpServer_GetHeaders(HP_HttpServer pServer, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue[], DWORD* pdwCount)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetHeaders(dwConnID, lpszName, lpszValue, *pdwCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpServer_GetAllHeaders(HP_HttpServer pServer, HP_CONNID dwConnID, HP_THeader lpHeaders[], DWORD* pdwCount)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetAllHeaders(dwConnID, lpHeaders, *pdwCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpServer_GetAllHeaderNames(HP_HttpServer pServer, HP_CONNID dwConnID, LPCSTR lpszName[], DWORD* pdwCount)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetAllHeaderNames(dwConnID, lpszName, *pdwCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpServer_GetCookie(HP_HttpServer pServer, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetCookie(dwConnID, lpszName, lpszValue);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpServer_GetAllCookies(HP_HttpServer pServer, HP_CONNID dwConnID, HP_TCookie lpCookies[], DWORD* pdwCount)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetAllCookies(dwConnID, lpCookies, *pdwCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpServer_GetWSMessageState(HP_HttpServer pServer, HP_CONNID dwConnID, BOOL* lpbFinal, BYTE* lpiReserved, BYTE* lpiOperationCode, LPCBYTE* lpszMask, ULONGLONG* lpullBodyLen, ULONGLONG* lpullBodyRemain)
{
	return C_HP_Object::ToFirst<IHttpServer>(pServer)->GetWSMessageState(dwConnID, lpbFinal, lpiReserved, lpiOperationCode, lpszMask, lpullBodyLen, lpullBodyRemain);
}

/**************************************************************************/
/*************************** HTTP Agent 操作方法 ***************************/

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendRequest(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->SendRequest(dwConnID, lpszMethod, lpszPath, lpHeaders, iHeaderCount, pBody, iLength);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendLocalFile(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszFileName, LPCSTR lpszMethod, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->SendLocalFile(dwConnID, lpszFileName, lpszMethod, lpszPath, lpHeaders, iHeaderCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendPost(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->SendPost(dwConnID, lpszPath, lpHeaders, iHeaderCount, pBody, iLength);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendPut(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->SendPut(dwConnID, lpszPath, lpHeaders, iHeaderCount, pBody, iLength);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendPatch(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->SendPatch(dwConnID, lpszPath, lpHeaders, iHeaderCount, pBody, iLength);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendGet(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->SendGet(dwConnID, lpszPath, lpHeaders, iHeaderCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendDelete(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->SendDelete(dwConnID, lpszPath, lpHeaders, iHeaderCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendHead(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->SendHead(dwConnID, lpszPath, lpHeaders, iHeaderCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendTrace(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->SendTrace(dwConnID, lpszPath, lpHeaders, iHeaderCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendOptions(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->SendOptions(dwConnID, lpszPath, lpHeaders, iHeaderCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendConnect(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszHost, const HP_THeader lpHeaders[], int iHeaderCount)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->SendConnect(dwConnID, lpszHost, lpHeaders, iHeaderCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_SendWSMessage(HP_HttpAgent pAgent, HP_CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], BYTE* pData, int iLength, ULONGLONG ullBodyLen)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->SendWSMessage(dwConnID, bFinal, iReserved, iOperationCode, lpszMask, pData, iLength, ullBodyLen);
}

/******************************************************************************/
/*************************** HTTP Agent 属性访问方法 ***************************/

HPSOCKET_API USHORT __HP_CALL HP_HttpAgent_GetStatusCode(HP_HttpAgent pAgent, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->GetStatusCode(dwConnID);
}

HPSOCKET_API void __HP_CALL HP_HttpAgent_SetLocalVersion(HP_HttpAgent pAgent, En_HP_HttpVersion usVersion)
{
	C_HP_Object::ToFirst<IHttpAgent>(pAgent)->SetLocalVersion(usVersion);
}

HPSOCKET_API En_HP_HttpVersion __HP_CALL HP_HttpAgent_GetLocalVersion(HP_HttpAgent pAgent)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->GetLocalVersion();
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_IsUpgrade(HP_HttpAgent pAgent, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->IsUpgrade(dwConnID);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_IsKeepAlive(HP_HttpAgent pAgent, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->IsKeepAlive(dwConnID);
}

HPSOCKET_API USHORT __HP_CALL HP_HttpAgent_GetVersion(HP_HttpAgent pAgent, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->GetVersion(dwConnID);
}

HPSOCKET_API ULONGLONG __HP_CALL HP_HttpAgent_GetContentLength(HP_HttpAgent pAgent, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->GetContentLength(dwConnID);
}

HPSOCKET_API LPCSTR __HP_CALL HP_HttpAgent_GetContentType(HP_HttpAgent pAgent, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->GetContentType(dwConnID);
}

HPSOCKET_API LPCSTR __HP_CALL HP_HttpAgent_GetContentEncoding(HP_HttpAgent pAgent, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->GetContentEncoding(dwConnID);
}

HPSOCKET_API LPCSTR __HP_CALL HP_HttpAgent_GetTransferEncoding(HP_HttpAgent pAgent, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->GetTransferEncoding(dwConnID);
}

HPSOCKET_API En_HP_HttpUpgradeType __HP_CALL HP_HttpAgent_GetUpgradeType(HP_HttpAgent pAgent, HP_CONNID dwConnID)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->GetUpgradeType(dwConnID);
}

HPSOCKET_API USHORT __HP_CALL HP_HttpAgent_GetParseErrorCode(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR* lpszErrorDesc)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->GetParseErrorCode(dwConnID, lpszErrorDesc);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_GetHeader(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->GetHeader(dwConnID, lpszName, lpszValue);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_GetHeaders(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue[], DWORD* pdwCount)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->GetHeaders(dwConnID, lpszName, lpszValue, *pdwCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_GetAllHeaders(HP_HttpAgent pAgent, HP_CONNID dwConnID, HP_THeader lpHeaders[], DWORD* pdwCount)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->GetAllHeaders(dwConnID, lpHeaders, *pdwCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_GetAllHeaderNames(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszName[], DWORD* pdwCount)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->GetAllHeaderNames(dwConnID, lpszName, *pdwCount);
}

HPSOCKET_API void __HP_CALL HP_HttpAgent_SetUseCookie(HP_HttpAgent pAgent, BOOL bUseCookie)
{
	C_HP_Object::ToFirst<IHttpAgent>(pAgent)->SetUseCookie(bUseCookie);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_IsUseCookie(HP_HttpAgent pAgent)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->IsUseCookie();
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_GetCookie(HP_HttpAgent pAgent, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR* lpszValue)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->GetCookie(dwConnID, lpszName, lpszValue);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_GetAllCookies(HP_HttpAgent pAgent, HP_CONNID dwConnID, HP_TCookie lpCookies[], DWORD* pdwCount)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->GetAllCookies(dwConnID, lpCookies, *pdwCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpAgent_GetWSMessageState(HP_HttpAgent pAgent, HP_CONNID dwConnID, BOOL* lpbFinal, BYTE* lpiReserved, BYTE* lpiOperationCode, LPCBYTE* lpszMask, ULONGLONG* lpullBodyLen, ULONGLONG* lpullBodyRemain)
{
	return C_HP_Object::ToFirst<IHttpAgent>(pAgent)->GetWSMessageState(dwConnID, lpbFinal, lpiReserved, lpiOperationCode, lpszMask, lpullBodyLen, lpullBodyRemain);
}

/**************************************************************************/
/*************************** HTTP Client 操作方法 **************************/

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendRequest(HP_HttpClient pClient, LPCSTR lpszMethod, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->SendRequest(lpszMethod, lpszPath, lpHeaders, iHeaderCount, pBody, iLength);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendLocalFile(HP_HttpClient pClient, LPCSTR lpszFileName, LPCSTR lpszMethod, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->SendLocalFile(lpszFileName, lpszMethod, lpszPath, lpHeaders, iHeaderCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendPost(HP_HttpClient pClient, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->SendPost(lpszPath, lpHeaders, iHeaderCount, pBody, iLength);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendPut(HP_HttpClient pClient, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->SendPut(lpszPath, lpHeaders, iHeaderCount, pBody, iLength);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendPatch(HP_HttpClient pClient, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->SendPatch(lpszPath, lpHeaders, iHeaderCount, pBody, iLength);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendGet(HP_HttpClient pClient, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->SendGet(lpszPath, lpHeaders, iHeaderCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendDelete(HP_HttpClient pClient, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->SendDelete(lpszPath, lpHeaders, iHeaderCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendHead(HP_HttpClient pClient, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->SendHead(lpszPath, lpHeaders, iHeaderCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendTrace(HP_HttpClient pClient, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->SendTrace(lpszPath, lpHeaders, iHeaderCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendOptions(HP_HttpClient pClient, LPCSTR lpszPath, const HP_THeader lpHeaders[], int iHeaderCount)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->SendOptions(lpszPath, lpHeaders, iHeaderCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendConnect(HP_HttpClient pClient, LPCSTR lpszHost, const HP_THeader lpHeaders[], int iHeaderCount)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->SendConnect(lpszHost, lpHeaders, iHeaderCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_SendWSMessage(HP_HttpClient pClient, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], BYTE* pData, int iLength, ULONGLONG ullBodyLen)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->SendWSMessage(bFinal, iReserved, iOperationCode, lpszMask, pData, iLength, ullBodyLen);
}

/******************************************************************************/
/*************************** HTTP Client 属性访问方法 **************************/

HPSOCKET_API USHORT __HP_CALL HP_HttpClient_GetStatusCode(HP_HttpClient pClient)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->GetStatusCode();
}

HPSOCKET_API void __HP_CALL HP_HttpClient_SetLocalVersion(HP_HttpClient pClient, En_HP_HttpVersion usVersion)
{
	C_HP_Object::ToFirst<IHttpClient>(pClient)->SetLocalVersion(usVersion);
}

HPSOCKET_API En_HP_HttpVersion __HP_CALL HP_HttpClient_GetLocalVersion(HP_HttpClient pClient)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->GetLocalVersion();
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_IsUpgrade(HP_HttpClient pClient)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->IsUpgrade();
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_IsKeepAlive(HP_HttpClient pClient)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->IsKeepAlive();
}

HPSOCKET_API USHORT __HP_CALL HP_HttpClient_GetVersion(HP_HttpClient pClient)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->GetVersion();
}

HPSOCKET_API ULONGLONG __HP_CALL HP_HttpClient_GetContentLength(HP_HttpClient pClient)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->GetContentLength();
}

HPSOCKET_API LPCSTR __HP_CALL HP_HttpClient_GetContentType(HP_HttpClient pClient)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->GetContentType();
}

HPSOCKET_API LPCSTR __HP_CALL HP_HttpClient_GetContentEncoding(HP_HttpClient pClient)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->GetContentEncoding();
}

HPSOCKET_API LPCSTR __HP_CALL HP_HttpClient_GetTransferEncoding(HP_HttpClient pClient)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->GetTransferEncoding();
}

HPSOCKET_API En_HP_HttpUpgradeType __HP_CALL HP_HttpClient_GetUpgradeType(HP_HttpClient pClient)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->GetUpgradeType();
}

HPSOCKET_API USHORT __HP_CALL HP_HttpClient_GetParseErrorCode(HP_HttpClient pClient, LPCSTR* lpszErrorDesc)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->GetParseErrorCode(lpszErrorDesc);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_GetHeader(HP_HttpClient pClient, LPCSTR lpszName, LPCSTR* lpszValue)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->GetHeader(lpszName, lpszValue);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_GetHeaders(HP_HttpClient pClient, LPCSTR lpszName, LPCSTR lpszValue[], DWORD* pdwCount)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->GetHeaders(lpszName, lpszValue, *pdwCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_GetAllHeaders(HP_HttpClient pClient, HP_THeader lpHeaders[], DWORD* pdwCount)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->GetAllHeaders(lpHeaders, *pdwCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_GetAllHeaderNames(HP_HttpClient pClient, LPCSTR lpszName[], DWORD* pdwCount)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->GetAllHeaderNames(lpszName, *pdwCount);
}

HPSOCKET_API void __HP_CALL HP_HttpClient_SetUseCookie(HP_HttpClient pClient, BOOL bUseCookie)
{
	C_HP_Object::ToFirst<IHttpClient>(pClient)->SetUseCookie(bUseCookie);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_IsUseCookie(HP_HttpClient pClient)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->IsUseCookie();
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_GetCookie(HP_HttpClient pClient, LPCSTR lpszName, LPCSTR* lpszValue)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->GetCookie(lpszName, lpszValue);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_GetAllCookies(HP_HttpClient pClient, HP_TCookie lpCookies[], DWORD* pdwCount)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->GetAllCookies(lpCookies, *pdwCount);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpClient_GetWSMessageState(HP_HttpClient pClient, BOOL* lpbFinal, BYTE* lpiReserved, BYTE* lpiOperationCode, LPCBYTE* lpszMask, ULONGLONG* lpullBodyLen, ULONGLONG* lpullBodyRemain)
{
	return C_HP_Object::ToFirst<IHttpClient>(pClient)->GetWSMessageState(lpbFinal, lpiReserved, lpiOperationCode, lpszMask, lpullBodyLen, lpullBodyRemain);
}

/**************************************************************************/
/************************ HTTP Sync Client 操作方法 ************************/

HPSOCKET_API BOOL __HP_CALL HP_HttpSyncClient_OpenUrl(HP_HttpSyncClient pClient, LPCSTR lpszMethod, LPCSTR lpszUrl, const THeader lpHeaders[], int iHeaderCount, const BYTE* pBody, int iLength, BOOL bForceReconnect)
{
	return C_HP_Object::ToFirst<IHttpSyncClient>(pClient)->OpenUrl(lpszMethod, lpszUrl, lpHeaders, iHeaderCount, pBody, iLength, bForceReconnect);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpSyncClient_CleanupRequestResult(HP_HttpSyncClient pClient)
{
	return C_HP_Object::ToFirst<IHttpSyncClient>(pClient)->CleanupRequestResult();
}

/**************************************************************************/
/********************** HTTP Sync Client 属性访问方法 **********************/

HPSOCKET_API void __HP_CALL HP_HttpSyncClient_SetConnectTimeout(HP_HttpSyncClient pClient, DWORD dwConnectTimeout)
{
	C_HP_Object::ToFirst<IHttpSyncClient>(pClient)->SetConnectTimeout(dwConnectTimeout);
}

HPSOCKET_API void __HP_CALL HP_HttpSyncClient_SetRequestTimeout(HP_HttpSyncClient pClient, DWORD dwRequestTimeout)
{
	C_HP_Object::ToFirst<IHttpSyncClient>(pClient)->SetRequestTimeout(dwRequestTimeout);
}

HPSOCKET_API DWORD __HP_CALL HP_HttpSyncClient_GetConnectTimeout(HP_HttpSyncClient pClient)
{
	return C_HP_Object::ToFirst<IHttpSyncClient>(pClient)->GetConnectTimeout();
}

HPSOCKET_API DWORD __HP_CALL HP_HttpSyncClient_GetRequestTimeout(HP_HttpSyncClient pClient)
{
	return C_HP_Object::ToFirst<IHttpSyncClient>(pClient)->GetRequestTimeout();
}

HPSOCKET_API BOOL __HP_CALL HP_HttpSyncClient_GetResponseBody(HP_HttpSyncClient pClient, LPCBYTE* lpszBody, int* piLength)
{
	return C_HP_Object::ToFirst<IHttpSyncClient>(pClient)->GetResponseBody(lpszBody, piLength);
}

/**************************************************************************/
/*************************** HTTP Cookie 管理方法 **************************/

HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_MGR_LoadFromFile(LPCSTR lpszFile, BOOL bKeepExists)
{
	return g_CookieMgr.LoadFromFile(lpszFile, bKeepExists);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_MGR_SaveToFile(LPCSTR lpszFile, BOOL bKeepExists)
{
	return g_CookieMgr.SaveToFile(lpszFile, bKeepExists);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_MGR_ClearCookies(LPCSTR lpszDomain, LPCSTR lpszPath)
{
	return g_CookieMgr.ClearCookies(lpszDomain, lpszPath);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_MGR_RemoveExpiredCookies(LPCSTR lpszDomain, LPCSTR lpszPath)
{
	return g_CookieMgr.RemoveExpiredCookies(lpszDomain, lpszPath);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_MGR_SetCookie(LPCSTR lpszName, LPCSTR lpszValue, LPCSTR lpszDomain, LPCSTR lpszPath, int iMaxAge, BOOL bHttpOnly, BOOL bSecure, int enSameSite, BOOL bOnlyUpdateValueIfExists)
{
	return g_CookieMgr.SetCookie(lpszName, lpszValue, lpszDomain, lpszPath, iMaxAge, bHttpOnly, bSecure, (CCookie::EnSameSite)enSameSite, bOnlyUpdateValueIfExists);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_MGR_DeleteCookie(LPCSTR lpszDomain, LPCSTR lpszPath, LPCSTR lpszName)
{
	return g_CookieMgr.DeleteCookie(lpszDomain, lpszPath, lpszName);
}

HPSOCKET_API void __HP_CALL HP_HttpCookie_MGR_SetEnableThirdPartyCookie(BOOL bEnableThirdPartyCookie)
{
	g_CookieMgr.SetEnableThirdPartyCookie(bEnableThirdPartyCookie);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_MGR_IsEnableThirdPartyCookie()
{
	return g_CookieMgr.IsEnableThirdPartyCookie();
}

HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_HLP_ParseExpires(LPCSTR lpszExpires, __time64_t* ptmExpires)
{
	return CCookie::ParseExpires(lpszExpires, *ptmExpires);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_HLP_MakeExpiresStr(char lpszBuff[], int* piBuffLen, __time64_t tmExpires)
{
	return CCookie::MakeExpiresStr(lpszBuff, *piBuffLen, tmExpires);
}

HPSOCKET_API BOOL __HP_CALL HP_HttpCookie_HLP_ToString(char lpszBuff[], int* piBuffLen, LPCSTR lpszName, LPCSTR lpszValue, LPCSTR lpszDomain, LPCSTR lpszPath, int iMaxAge, BOOL bHttpOnly, BOOL bSecure, int enSameSite)
{
	return CCookie::ToString(lpszBuff, *piBuffLen, lpszName, lpszValue, lpszDomain, lpszPath, iMaxAge, bHttpOnly, bSecure, (CCookie::EnSameSite)enSameSite);
}

HPSOCKET_API __time64_t __HP_CALL HP_HttpCookie_HLP_CurrentUTCTime()
{
	return CCookie::CurrentUTCTime();
}

HPSOCKET_API __time64_t __HP_CALL HP_HttpCookie_HLP_MaxAgeToExpires(int iMaxAge)
{
	return CCookie::MaxAgeToExpires(iMaxAge);
}

HPSOCKET_API int __HP_CALL HP_HttpCookie_HLP_ExpiresToMaxAge(__time64_t tmExpires)
{
	return CCookie::ExpiresToMaxAge(tmExpires);
}

/*****************************************************************************************************************************************************/
/************************************************************* HTTP Global Function Exports **********************************************************/
/*****************************************************************************************************************************************************/

#endif

/*****************************************************************************************************************************************************/
/**************************************************************** Thread Pool Exports ****************************************************************/
/*****************************************************************************************************************************************************/

/****************************************************/
/******************* 对象创建函数 ********************/

HPSOCKET_API HP_ThreadPool __HP_CALL Create_HP_ThreadPool()
{
	return (HP_ThreadPool)(new CHPThreadPool);
}

HPSOCKET_API void __HP_CALL Destroy_HP_ThreadPool(HP_ThreadPool pThreadPool)
{
	delete (IHPThreadPool*)pThreadPool;
}

HPSOCKET_API LPTSocketTask __HP_CALL Create_HP_SocketTaskObj(Fn_SocketTaskProc fnTaskProc, PVOID pSender, HP_CONNID dwConnID, LPCBYTE pBuffer, INT iBuffLen, En_HP_TaskBufferType enBuffType, WPARAM wParam, LPARAM lParam)
{
	return ::CreateSocketTaskObj(fnTaskProc, pSender, dwConnID, pBuffer, iBuffLen, enBuffType, wParam, lParam);
}

HPSOCKET_API void __HP_CALL Destroy_HP_SocketTaskObj(LPTSocketTask pTask)
{
	::DestroySocketTaskObj(pTask);
}

/***********************************************************************/
/***************************** 组件操作方法 *****************************/

HPSOCKET_API BOOL __HP_CALL HP_ThreadPool_Start(HP_ThreadPool pThreadPool, DWORD dwThreadCount, DWORD dwMaxQueueSize, En_HP_RejectedPolicy enRejectedPolicy, DWORD dwStackSize)
{
	return ((IHPThreadPool*)pThreadPool)->Start(dwThreadCount, dwMaxQueueSize, enRejectedPolicy, dwStackSize);
}

HPSOCKET_API BOOL __HP_CALL HP_ThreadPool_Stop(HP_ThreadPool pThreadPool, DWORD dwMaxWait)
{
	return ((IHPThreadPool*)pThreadPool)->Stop(dwMaxWait);
}

HPSOCKET_API BOOL __HP_CALL HP_ThreadPool_Submit(HP_ThreadPool pThreadPool, HP_Fn_TaskProc fnTaskProc, PVOID pvArg, DWORD dwMaxWait)
{
	return ((IHPThreadPool*)pThreadPool)->Submit(fnTaskProc, pvArg, dwMaxWait);
}

HPSOCKET_API BOOL __HP_CALL HP_ThreadPool_Submit_Task(HP_ThreadPool pThreadPool, HP_LPTSocketTask pTask, DWORD dwMaxWait)
{
	return ((IHPThreadPool*)pThreadPool)->Submit(pTask, dwMaxWait);
}

HPSOCKET_API BOOL __HP_CALL HP_ThreadPool_AdjustThreadCount(HP_ThreadPool pThreadPool, DWORD dwNewThreadCount)
{
	return ((IHPThreadPool*)pThreadPool)->AdjustThreadCount(dwNewThreadCount);
}

/***********************************************************************/
/***************************** 属性访问方法 *****************************/

HPSOCKET_API BOOL __HP_CALL HP_ThreadPool_HasStarted(HP_ThreadPool pThreadPool)
{
	return ((IHPThreadPool*)pThreadPool)->HasStarted();
}

HPSOCKET_API EnServiceState	__HP_CALL HP_ThreadPool_GetState(HP_ThreadPool pThreadPool)
{
	return ((IHPThreadPool*)pThreadPool)->GetState();
}

HPSOCKET_API DWORD __HP_CALL HP_ThreadPool_GetQueueSize(HP_ThreadPool pThreadPool)
{
	return ((IHPThreadPool*)pThreadPool)->GetQueueSize();
}

HPSOCKET_API DWORD __HP_CALL HP_ThreadPool_GetThreadCount(HP_ThreadPool pThreadPool)
{
	return ((IHPThreadPool*)pThreadPool)->GetThreadCount();
}

HPSOCKET_API DWORD __HP_CALL HP_ThreadPool_GetMaxQueueSize(HP_ThreadPool pThreadPool)
{
	return ((IHPThreadPool*)pThreadPool)->GetMaxQueueSize();
}

HPSOCKET_API EnRejectedPolicy __HP_CALL HP_ThreadPool_GetRejectedPolicy(HP_ThreadPool pThreadPool)
{
	return ((IHPThreadPool*)pThreadPool)->GetRejectedPolicy();
}
