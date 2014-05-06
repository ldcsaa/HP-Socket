/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.2.1
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
#include "HPSocket4C.h"
#include "TcpServer.h"
#include "TcpClient.h"
#include "TcpAgent.h"
#include "TcpPullServer.h"
#include "TcpPullClient.h"
#include "TcpPullAgent.h"
#include "UdpServer.h"
#include "UdpClient.h"

#if !defined(_WIN64)
	#pragma comment(linker, "/EXPORT:Create_HP_TcpAgent=_Create_HP_TcpAgent@4")
	#pragma comment(linker, "/EXPORT:Create_HP_TcpAgentListener=_Create_HP_TcpAgentListener@0")
	#pragma comment(linker, "/EXPORT:Create_HP_TcpClient=_Create_HP_TcpClient@4")
	#pragma comment(linker, "/EXPORT:Create_HP_TcpClientListener=_Create_HP_TcpClientListener@0")
	#pragma comment(linker, "/EXPORT:Create_HP_TcpPullAgent=_Create_HP_TcpPullAgent@4")
	#pragma comment(linker, "/EXPORT:Create_HP_TcpPullAgentListener=_Create_HP_TcpPullAgentListener@0")
	#pragma comment(linker, "/EXPORT:Create_HP_TcpPullClient=_Create_HP_TcpPullClient@4")
	#pragma comment(linker, "/EXPORT:Create_HP_TcpPullClientListener=_Create_HP_TcpPullClientListener@0")
	#pragma comment(linker, "/EXPORT:Create_HP_TcpPullServer=_Create_HP_TcpPullServer@4")
	#pragma comment(linker, "/EXPORT:Create_HP_TcpPullServerListener=_Create_HP_TcpPullServerListener@0")
	#pragma comment(linker, "/EXPORT:Create_HP_TcpServer=_Create_HP_TcpServer@4")
	#pragma comment(linker, "/EXPORT:Create_HP_TcpServerListener=_Create_HP_TcpServerListener@0")
	#pragma comment(linker, "/EXPORT:Create_HP_UdpClient=_Create_HP_UdpClient@4")
	#pragma comment(linker, "/EXPORT:Create_HP_UdpClientListener=_Create_HP_UdpClientListener@0")
	#pragma comment(linker, "/EXPORT:Create_HP_UdpServer=_Create_HP_UdpServer@4")
	#pragma comment(linker, "/EXPORT:Create_HP_UdpServerListener=_Create_HP_UdpServerListener@0")
	#pragma comment(linker, "/EXPORT:Destroy_HP_TcpAgent=_Destroy_HP_TcpAgent@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_TcpAgentListener=_Destroy_HP_TcpAgentListener@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_TcpClient=_Destroy_HP_TcpClient@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_TcpClientListener=_Destroy_HP_TcpClientListener@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_TcpPullAgent=_Destroy_HP_TcpPullAgent@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_TcpPullAgentListener=_Destroy_HP_TcpPullAgentListener@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_TcpPullClient=_Destroy_HP_TcpPullClient@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_TcpPullClientListener=_Destroy_HP_TcpPullClientListener@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_TcpPullServer=_Destroy_HP_TcpPullServer@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_TcpPullServerListener=_Destroy_HP_TcpPullServerListener@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_TcpServer=_Destroy_HP_TcpServer@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_TcpServerListener=_Destroy_HP_TcpServerListener@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_UdpClient=_Destroy_HP_UdpClient@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_UdpClientListener=_Destroy_HP_UdpClientListener@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_UdpServer=_Destroy_HP_UdpServer@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_UdpServerListener=_Destroy_HP_UdpServerListener@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_Connect=_HP_Agent_Connect@16")
	#pragma comment(linker, "/EXPORT:HP_Agent_Disconnect=_HP_Agent_Disconnect@12")
	#pragma comment(linker, "/EXPORT:HP_Agent_DisconnectLongConnections=_HP_Agent_DisconnectLongConnections@12")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetConnectPeriod=_HP_Agent_GetConnectPeriod@12")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetConnectionCount=_HP_Agent_GetConnectionCount@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetConnectionExtra=_HP_Agent_GetConnectionExtra@12")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetFreeBufferObjHold=_HP_Agent_GetFreeBufferObjHold@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetFreeBufferObjPool=_HP_Agent_GetFreeBufferObjPool@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetFreeSocketObjHold=_HP_Agent_GetFreeSocketObjHold@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetFreeSocketObjLockTime=_HP_Agent_GetFreeSocketObjLockTime@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetFreeSocketObjPool=_HP_Agent_GetFreeSocketObjPool@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetLastError=_HP_Agent_GetLastError@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetLastErrorDesc=_HP_Agent_GetLastErrorDesc@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetLocalAddress=_HP_Agent_GetLocalAddress@20")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetMaxShutdownWaitTime=_HP_Agent_GetMaxShutdownWaitTime@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetPendingDataLength=_HP_Agent_GetPendingDataLength@12")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetRemoteAddress=_HP_Agent_GetRemoteAddress@20")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetState=_HP_Agent_GetState@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetSendPolicy=_HP_Agent_GetSendPolicy@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetWorkerThreadCount=_HP_Agent_GetWorkerThreadCount@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_HasStarted=_HP_Agent_HasStarted@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_Send=_HP_Agent_Send@16")
	#pragma comment(linker, "/EXPORT:HP_Agent_SendPart=_HP_Agent_SendPart@20")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetConnectionExtra=_HP_Agent_SetConnectionExtra@12")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetFreeBufferObjHold=_HP_Agent_SetFreeBufferObjHold@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetFreeBufferObjPool=_HP_Agent_SetFreeBufferObjPool@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetFreeSocketObjHold=_HP_Agent_SetFreeSocketObjHold@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetFreeSocketObjLockTime=_HP_Agent_SetFreeSocketObjLockTime@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetFreeSocketObjPool=_HP_Agent_SetFreeSocketObjPool@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetMaxShutdownWaitTime=_HP_Agent_SetMaxShutdownWaitTime@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetSendPolicy=_HP_Agent_SetSendPolicy@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetWorkerThreadCount=_HP_Agent_SetWorkerThreadCount@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_Start=_HP_Agent_Start@12")
	#pragma comment(linker, "/EXPORT:HP_Agent_Stop=_HP_Agent_Stop@4")
	#pragma comment(linker, "/EXPORT:HP_Client_GetConnectionID=_HP_Client_GetConnectionID@4")
	#pragma comment(linker, "/EXPORT:HP_Client_GetFreeBufferPoolHold=_HP_Client_GetFreeBufferPoolHold@4")
	#pragma comment(linker, "/EXPORT:HP_Client_GetFreeBufferPoolSize=_HP_Client_GetFreeBufferPoolSize@4")
	#pragma comment(linker, "/EXPORT:HP_Client_GetLastError=_HP_Client_GetLastError@4")
	#pragma comment(linker, "/EXPORT:HP_Client_GetLastErrorDesc=_HP_Client_GetLastErrorDesc@4")
	#pragma comment(linker, "/EXPORT:HP_Client_GetLocalAddress=_HP_Client_GetLocalAddress@16")
	#pragma comment(linker, "/EXPORT:HP_Client_GetPendingDataLength=_HP_Client_GetPendingDataLength@8")
	#pragma comment(linker, "/EXPORT:HP_Client_GetState=_HP_Client_GetState@4")
	#pragma comment(linker, "/EXPORT:HP_Client_HasStarted=_HP_Client_HasStarted@4")
	#pragma comment(linker, "/EXPORT:HP_Client_Send=_HP_Client_Send@12")
	#pragma comment(linker, "/EXPORT:HP_Client_SendPart=_HP_Client_SendPart@16")
	#pragma comment(linker, "/EXPORT:HP_Client_SetFreeBufferPoolHold=_HP_Client_SetFreeBufferPoolHold@8")
	#pragma comment(linker, "/EXPORT:HP_Client_SetFreeBufferPoolSize=_HP_Client_SetFreeBufferPoolSize@8")
	#pragma comment(linker, "/EXPORT:HP_Client_Start=_HP_Client_Start@16")
	#pragma comment(linker, "/EXPORT:HP_Client_Stop=_HP_Client_Stop@4")
	#pragma comment(linker, "/EXPORT:HP_GetSocketErrorDesc=_HP_GetSocketErrorDesc@4")
	#pragma comment(linker, "/EXPORT:HP_Server_Disconnect=_HP_Server_Disconnect@12")
	#pragma comment(linker, "/EXPORT:HP_Server_DisconnectLongConnections=_HP_Server_DisconnectLongConnections@12")
	#pragma comment(linker, "/EXPORT:HP_Server_GetConnectPeriod=_HP_Server_GetConnectPeriod@12")
	#pragma comment(linker, "/EXPORT:HP_Server_GetConnectionCount=_HP_Server_GetConnectionCount@4")
	#pragma comment(linker, "/EXPORT:HP_Server_GetConnectionExtra=_HP_Server_GetConnectionExtra@12")
	#pragma comment(linker, "/EXPORT:HP_Server_GetFreeBufferObjHold=_HP_Server_GetFreeBufferObjHold@4")
	#pragma comment(linker, "/EXPORT:HP_Server_GetFreeBufferObjPool=_HP_Server_GetFreeBufferObjPool@4")
	#pragma comment(linker, "/EXPORT:HP_Server_GetFreeSocketObjHold=_HP_Server_GetFreeSocketObjHold@4")
	#pragma comment(linker, "/EXPORT:HP_Server_GetFreeSocketObjLockTime=_HP_Server_GetFreeSocketObjLockTime@4")
	#pragma comment(linker, "/EXPORT:HP_Server_GetFreeSocketObjPool=_HP_Server_GetFreeSocketObjPool@4")
	#pragma comment(linker, "/EXPORT:HP_Server_GetLastError=_HP_Server_GetLastError@4")
	#pragma comment(linker, "/EXPORT:HP_Server_GetLastErrorDesc=_HP_Server_GetLastErrorDesc@4")
	#pragma comment(linker, "/EXPORT:HP_Server_GetListenAddress=_HP_Server_GetListenAddress@16")
	#pragma comment(linker, "/EXPORT:HP_Server_GetMaxShutdownWaitTime=_HP_Server_GetMaxShutdownWaitTime@4")
	#pragma comment(linker, "/EXPORT:HP_Server_GetPendingDataLength=_HP_Server_GetPendingDataLength@12")
	#pragma comment(linker, "/EXPORT:HP_Server_GetRemoteAddress=_HP_Server_GetRemoteAddress@20")
	#pragma comment(linker, "/EXPORT:HP_Server_GetSendPolicy=_HP_Server_GetSendPolicy@4")
	#pragma comment(linker, "/EXPORT:HP_Server_GetState=_HP_Server_GetState@4")
	#pragma comment(linker, "/EXPORT:HP_Server_GetWorkerThreadCount=_HP_Server_GetWorkerThreadCount@4")
	#pragma comment(linker, "/EXPORT:HP_Server_HasStarted=_HP_Server_HasStarted@4")
	#pragma comment(linker, "/EXPORT:HP_Server_Send=_HP_Server_Send@16")
	#pragma comment(linker, "/EXPORT:HP_Server_SendPart=_HP_Server_SendPart@20")
	#pragma comment(linker, "/EXPORT:HP_Server_SetConnectionExtra=_HP_Server_SetConnectionExtra@12")
	#pragma comment(linker, "/EXPORT:HP_Server_SetFreeBufferObjHold=_HP_Server_SetFreeBufferObjHold@8")
	#pragma comment(linker, "/EXPORT:HP_Server_SetFreeBufferObjPool=_HP_Server_SetFreeBufferObjPool@8")
	#pragma comment(linker, "/EXPORT:HP_Server_SetFreeSocketObjHold=_HP_Server_SetFreeSocketObjHold@8")
	#pragma comment(linker, "/EXPORT:HP_Server_SetFreeSocketObjLockTime=_HP_Server_SetFreeSocketObjLockTime@8")
	#pragma comment(linker, "/EXPORT:HP_Server_SetFreeSocketObjPool=_HP_Server_SetFreeSocketObjPool@8")
	#pragma comment(linker, "/EXPORT:HP_Server_SetMaxShutdownWaitTime=_HP_Server_SetMaxShutdownWaitTime@8")
	#pragma comment(linker, "/EXPORT:HP_Server_SetSendPolicy=_HP_Server_SetSendPolicy@8")
	#pragma comment(linker, "/EXPORT:HP_Server_SetWorkerThreadCount=_HP_Server_SetWorkerThreadCount@8")
	#pragma comment(linker, "/EXPORT:HP_Server_Start=_HP_Server_Start@12")
	#pragma comment(linker, "/EXPORT:HP_Server_Stop=_HP_Server_Stop@4")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Agent_OnAgentShutdown=_HP_Set_FN_Agent_OnAgentShutdown@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Agent_OnClose=_HP_Set_FN_Agent_OnClose@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Agent_OnConnect=_HP_Set_FN_Agent_OnConnect@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Agent_OnError=_HP_Set_FN_Agent_OnError@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Agent_OnPrepareConnect=_HP_Set_FN_Agent_OnPrepareConnect@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Agent_OnPullReceive=_HP_Set_FN_Agent_OnPullReceive@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Agent_OnReceive=_HP_Set_FN_Agent_OnReceive@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Agent_OnSend=_HP_Set_FN_Agent_OnSend@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Client_OnClose=_HP_Set_FN_Client_OnClose@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Client_OnConnect=_HP_Set_FN_Client_OnConnect@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Client_OnError=_HP_Set_FN_Client_OnError@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Client_OnPrepareConnect=_HP_Set_FN_Client_OnPrepareConnect@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Client_OnPullReceive=_HP_Set_FN_Client_OnPullReceive@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Client_OnReceive=_HP_Set_FN_Client_OnReceive@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Client_OnSend=_HP_Set_FN_Client_OnSend@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Server_OnAccept=_HP_Set_FN_Server_OnAccept@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Server_OnClose=_HP_Set_FN_Server_OnClose@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Server_OnError=_HP_Set_FN_Server_OnError@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Server_OnPrepareListen=_HP_Set_FN_Server_OnPrepareListen@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Server_OnPullReceive=_HP_Set_FN_Server_OnPullReceive@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Server_OnReceive=_HP_Set_FN_Server_OnReceive@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Server_OnSend=_HP_Set_FN_Server_OnSend@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Server_OnServerShutdown=_HP_Set_FN_Server_OnServerShutdown@8")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_GetKeepAliveInterval=_HP_TcpAgent_GetKeepAliveInterval@4")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_GetKeepAliveTime=_HP_TcpAgent_GetKeepAliveTime@4")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_GetSocketBufferSize=_HP_TcpAgent_GetSocketBufferSize@4")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_IsReuseAddress=_HP_TcpAgent_IsReuseAddress@4")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_SetKeepAliveInterval=_HP_TcpAgent_SetKeepAliveInterval@8")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_SetKeepAliveTime=_HP_TcpAgent_SetKeepAliveTime@8")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_SetReuseAddress=_HP_TcpAgent_SetReuseAddress@8")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_SetSocketBufferSize=_HP_TcpAgent_SetSocketBufferSize@8")
	#pragma comment(linker, "/EXPORT:HP_TcpClient_GetKeepAliveInterval=_HP_TcpClient_GetKeepAliveInterval@4")
	#pragma comment(linker, "/EXPORT:HP_TcpClient_GetKeepAliveTime=_HP_TcpClient_GetKeepAliveTime@4")
	#pragma comment(linker, "/EXPORT:HP_TcpClient_GetSocketBufferSize=_HP_TcpClient_GetSocketBufferSize@4")
	#pragma comment(linker, "/EXPORT:HP_TcpClient_SetKeepAliveInterval=_HP_TcpClient_SetKeepAliveInterval@8")
	#pragma comment(linker, "/EXPORT:HP_TcpClient_SetKeepAliveTime=_HP_TcpClient_SetKeepAliveTime@8")
	#pragma comment(linker, "/EXPORT:HP_TcpClient_SetSocketBufferSize=_HP_TcpClient_SetSocketBufferSize@8")
	#pragma comment(linker, "/EXPORT:HP_TcpPullAgent_Fetch=_HP_TcpPullAgent_Fetch@16")
	#pragma comment(linker, "/EXPORT:HP_TcpPullClient_Fetch=_HP_TcpPullClient_Fetch@16")
	#pragma comment(linker, "/EXPORT:HP_TcpPullServer_Fetch=_HP_TcpPullServer_Fetch@16")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_GetAcceptSocketCount=_HP_TcpServer_GetAcceptSocketCount@4")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_GetKeepAliveInterval=_HP_TcpServer_GetKeepAliveInterval@4")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_GetKeepAliveTime=_HP_TcpServer_GetKeepAliveTime@4")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_GetSocketBufferSize=_HP_TcpServer_GetSocketBufferSize@4")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_GetSocketListenQueue=_HP_TcpServer_GetSocketListenQueue@4")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_SetAcceptSocketCount=_HP_TcpServer_SetAcceptSocketCount@8")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_SetKeepAliveInterval=_HP_TcpServer_SetKeepAliveInterval@8")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_SetKeepAliveTime=_HP_TcpServer_SetKeepAliveTime@8")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_SetSocketBufferSize=_HP_TcpServer_SetSocketBufferSize@8")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_SetSocketListenQueue=_HP_TcpServer_SetSocketListenQueue@8")
	#pragma comment(linker, "/EXPORT:HP_UdpClient_GetDetectAttempts=_HP_UdpClient_GetDetectAttempts@4")
	#pragma comment(linker, "/EXPORT:HP_UdpClient_GetDetectInterval=_HP_UdpClient_GetDetectInterval@4")
	#pragma comment(linker, "/EXPORT:HP_UdpClient_GetMaxDatagramSize=_HP_UdpClient_GetMaxDatagramSize@4")
	#pragma comment(linker, "/EXPORT:HP_UdpClient_SetDetectAttempts=_HP_UdpClient_SetDetectAttempts@8")
	#pragma comment(linker, "/EXPORT:HP_UdpClient_SetDetectInterval=_HP_UdpClient_SetDetectInterval@8")
	#pragma comment(linker, "/EXPORT:HP_UdpClient_SetMaxDatagramSize=_HP_UdpClient_SetMaxDatagramSize@8")
	#pragma comment(linker, "/EXPORT:HP_UdpServer_GetDetectAttempts=_HP_UdpServer_GetDetectAttempts@4")
	#pragma comment(linker, "/EXPORT:HP_UdpServer_GetDetectInterval=_HP_UdpServer_GetDetectInterval@4")
	#pragma comment(linker, "/EXPORT:HP_UdpServer_GetMaxDatagramSize=_HP_UdpServer_GetMaxDatagramSize@4")
	#pragma comment(linker, "/EXPORT:HP_UdpServer_SetDetectAttempts=_HP_UdpServer_SetDetectAttempts@8")
	#pragma comment(linker, "/EXPORT:HP_UdpServer_SetDetectInterval=_HP_UdpServer_SetDetectInterval@8")
	#pragma comment(linker, "/EXPORT:HP_UdpServer_SetMaxDatagramSize=_HP_UdpServer_SetMaxDatagramSize@8")
	#pragma comment(linker, "/EXPORT:SYS_GetLastError=_SYS_GetLastError@0")
#endif

class C_HP_ServerListener : public IServerListener
{
public:
	virtual EnHandleResult OnPrepareListen(UINT_PTR soListen)
	{
		return	(m_fnOnPrepareListen)
				? (EnHandleResult)m_fnOnPrepareListen(soListen)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnAccept(CONNID dwConnID, UINT_PTR soClient)
	{
		ASSERT(m_fnOnAccept);

		return	(m_fnOnAccept)
				? (EnHandleResult)m_fnOnAccept(dwConnID, soClient)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnSend(CONNID dwConnID, const BYTE* pData, int iLength)
	{
		return	(m_fnOnSend)
				? (EnHandleResult)m_fnOnSend(dwConnID, pData, iLength)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnReceive(CONNID dwConnID, const BYTE* pData, int iLength)
	{
		ASSERT(m_fnOnReceive);

		return	(m_fnOnReceive)
				? (EnHandleResult)m_fnOnReceive(dwConnID, pData, iLength)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnReceive(CONNID dwConnID, int iLength)
	{
		ASSERT(m_fnOnPullReceive);

		return	(m_fnOnPullReceive)
				? (EnHandleResult)m_fnOnPullReceive(dwConnID, iLength)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnClose(CONNID dwConnID)
	{
		ASSERT(m_fnOnClose);

		return	(m_fnOnClose)
				? (EnHandleResult)m_fnOnClose(dwConnID)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnError(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
	{
		ASSERT(m_fnOnError);

		return	(m_fnOnError)
				? (EnHandleResult)m_fnOnError(dwConnID, (En_HP_SocketOperation)enOperation, iErrorCode)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnServerShutdown()
	{
		return	(m_fnOnServerShutdown)
				? (EnHandleResult)m_fnOnServerShutdown()
				: HR_IGNORE;
	}

public:
	C_HP_ServerListener()
	: m_fnOnPrepareListen	(nullptr)
	, m_fnOnAccept			(nullptr)
	, m_fnOnSend			(nullptr)
	, m_fnOnReceive			(nullptr)
	, m_fnOnPullReceive		(nullptr)
	, m_fnOnClose			(nullptr)
	, m_fnOnError			(nullptr)
	, m_fnOnServerShutdown	(nullptr)
	{
	}

public:
	HP_FN_OnPrepareListen	m_fnOnPrepareListen	;
	HP_FN_OnAccept			m_fnOnAccept		;
	HP_FN_OnSend			m_fnOnSend			;
	HP_FN_OnReceive			m_fnOnReceive		;
	HP_FN_OnPullReceive		m_fnOnPullReceive	;
	HP_FN_OnClose			m_fnOnClose			;
	HP_FN_OnError			m_fnOnError			;
	HP_FN_OnServerShutdown	m_fnOnServerShutdown;
};

class C_HP_ClientListener : public IClientListener
{
public:
	virtual EnHandleResult OnPrepareConnect(CONNID dwConnID, UINT_PTR socket)
	{
		return	(m_fnOnPrepareConnect)
				? (EnHandleResult)m_fnOnPrepareConnect(dwConnID, socket)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnConnect(CONNID dwConnID)
	{
		ASSERT(m_fnOnConnect);

		return	(m_fnOnConnect)
				? (EnHandleResult)m_fnOnConnect(dwConnID)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnSend(CONNID dwConnID, const BYTE* pData, int iLength)
	{
		return	(m_fnOnSend)
				? (EnHandleResult)m_fnOnSend(dwConnID, pData, iLength)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnReceive(CONNID dwConnID, const BYTE* pData, int iLength)
	{
		ASSERT(m_fnOnReceive);

		return	(m_fnOnReceive)
				? (EnHandleResult)m_fnOnReceive(dwConnID, pData, iLength)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnReceive(CONNID dwConnID, int iLength)
	{
		ASSERT(m_fnOnPullReceive);

		return	(m_fnOnPullReceive)
				? (EnHandleResult)m_fnOnPullReceive(dwConnID, iLength)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnClose(CONNID dwConnID)
	{
		ASSERT(m_fnOnClose);

		return	(m_fnOnClose)
				? (EnHandleResult)m_fnOnClose(dwConnID)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnError(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
	{
		ASSERT(m_fnOnError);

		return	(m_fnOnError)
				? (EnHandleResult)m_fnOnError(dwConnID, (En_HP_SocketOperation)enOperation, iErrorCode)
				: HR_IGNORE;
	}

public:
	C_HP_ClientListener()
	: m_fnOnPrepareConnect	(nullptr)
	, m_fnOnConnect			(nullptr)
	, m_fnOnSend			(nullptr)
	, m_fnOnReceive			(nullptr)
	, m_fnOnPullReceive		(nullptr)
	, m_fnOnClose			(nullptr)
	, m_fnOnError			(nullptr)
	{
	}

public:
	HP_FN_OnPrepareConnect	m_fnOnPrepareConnect;
	HP_FN_OnConnect			m_fnOnConnect		;
	HP_FN_OnSend			m_fnOnSend			;
	HP_FN_OnReceive			m_fnOnReceive		;
	HP_FN_OnPullReceive		m_fnOnPullReceive	;
	HP_FN_OnClose			m_fnOnClose			;
	HP_FN_OnError			m_fnOnError			;
};

class C_HP_AgentListener : public C_HP_ClientListener
{
public:
	virtual EnHandleResult OnAgentShutdown()
	{
		return	(m_fnOnAgentShutdown)
				? (EnHandleResult)m_fnOnAgentShutdown()
				: HR_IGNORE;
	}

public:
	C_HP_AgentListener()
	: m_fnOnAgentShutdown	(nullptr)
	{
	}

public:
	HP_FN_OnAgentShutdown	m_fnOnAgentShutdown	;
};

class C_HP_Object
{
public:

	static IServer* ToServer(PVOID pServer)
	{
		return dynamic_cast<IServer*>((C_HP_Object*)pServer);
	}

	static IClient* ToClient(PVOID pClient)
	{
		return dynamic_cast<IClient*>((C_HP_Object*)pClient);
	}

	static IAgent* ToAgent(PVOID pAgent)
	{
		return dynamic_cast<IAgent*>((C_HP_Object*)pAgent);
	}

	static IPullSocket* ToPullSocket(PVOID pPullSocket)
	{
		return dynamic_cast<IPullSocket*>((C_HP_Object*)pPullSocket);
	}

public:

	static ITcpServer* ToTcpServer(PVOID pServer)
	{
		return (ITcpServer*)ToServer(pServer);
	}

	static IUdpServer* ToUdpServer(PVOID pServer)
	{
		return (IUdpServer*)ToServer(pServer);
	}

	static ITcpClient* ToTcpClient(PVOID pClient)
	{
		return (ITcpClient*)ToClient(pClient);
	}

	static IUdpClient* ToUdpClient(PVOID pClient)
	{
		return (IUdpClient*)ToClient(pClient);
	}

	static ITcpAgent* ToTcpAgent(PVOID pAgent)
	{
		return (ITcpAgent*)ToAgent(pAgent);
	}

public:
	virtual ~C_HP_Object()
	{
	}
};

class C_HP_TcpServer : public C_HP_Object, public CTcpServer
{
public:
	C_HP_TcpServer(ITcpServerListener* pListener) : CTcpServer(pListener) {}
};

class C_HP_TcpPullServer : public C_HP_Object, public CTcpPullServer
{
public:
	C_HP_TcpPullServer(ITcpServerListener* pListener) : CTcpPullServer(pListener) {}
};

class C_HP_UdpServer : public C_HP_Object, public CUdpServer
{
public:
	C_HP_UdpServer(IUdpServerListener* pListener) : CUdpServer(pListener) {}
};

class C_HP_TcpClient : public C_HP_Object, public CTcpClient
{
public:
	C_HP_TcpClient(ITcpClientListener* pListener) : CTcpClient(pListener) {}
};

class C_HP_TcpPullClient : public C_HP_Object, public CTcpPullClient
{
public:
	C_HP_TcpPullClient(ITcpClientListener* pListener) : CTcpPullClient(pListener) {}
};

class C_HP_UdpClient : public C_HP_Object, public CUdpClient
{
public:
	C_HP_UdpClient(IUdpClientListener* pListener) : CUdpClient(pListener) {}
};

class C_HP_TcpAgent : public C_HP_Object, public CTcpAgent
{
public:
	C_HP_TcpAgent(ITcpAgentListener* pListener) : CTcpAgent(pListener) {}
};

class C_HP_TcpPullAgent : public C_HP_Object, public CTcpPullAgent
{
public:
	C_HP_TcpPullAgent(ITcpAgentListener* pListener) : CTcpPullAgent(pListener) {}
};

/****************************************************/
/************** HPSocket4C.dll 导出函数 **************/

HPSOCKET_API HP_TcpServer __stdcall Create_HP_TcpServer(HP_TcpServerListener pListener)
{
	return (HP_TcpServer)(new C_HP_TcpServer((ITcpServerListener*)pListener));
}

HPSOCKET_API HP_TcpClient __stdcall Create_HP_TcpClient(HP_TcpClientListener pListener)
{
	return (HP_TcpClient)(new C_HP_TcpClient((ITcpClientListener*)pListener));
}

HPSOCKET_API HP_TcpAgent __stdcall Create_HP_TcpAgent(HP_TcpAgentListener pListener)
{
	return (HP_TcpAgent)(new C_HP_TcpAgent((ITcpAgentListener*)pListener));
}

HPSOCKET_API HP_TcpPullServer __stdcall Create_HP_TcpPullServer(HP_TcpPullServerListener pListener)
{
	return (HP_TcpPullServer)(new C_HP_TcpPullServer((ITcpServerListener*)pListener));
}

HPSOCKET_API HP_TcpPullClient __stdcall Create_HP_TcpPullClient(HP_TcpPullClientListener pListener)
{
	return (HP_TcpPullClient)(new C_HP_TcpPullClient((ITcpClientListener*)pListener));
}

HPSOCKET_API HP_TcpPullAgent __stdcall Create_HP_TcpPullAgent(HP_TcpPullAgentListener pListener)
{
	return (HP_TcpPullAgent)(new C_HP_TcpPullAgent((ITcpAgentListener*)pListener));
}

HPSOCKET_API HP_UdpServer __stdcall Create_HP_UdpServer(HP_UdpServerListener pListener)
{
	return (HP_UdpServer)(new C_HP_UdpServer((IUdpServerListener*)pListener));
}

HPSOCKET_API HP_UdpClient __stdcall Create_HP_UdpClient(HP_UdpClientListener pListener)
{
	return (HP_UdpClient)(new C_HP_UdpClient((IUdpClientListener*)pListener));
}

HPSOCKET_API void __stdcall Destroy_HP_TcpServer(HP_TcpServer pServer)
{
	delete (C_HP_TcpServer*)pServer;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpClient(HP_TcpClient pClient)
{
	delete (C_HP_TcpClient*)pClient;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpAgent(HP_TcpAgent pAgent)
{
	delete (C_HP_TcpAgent*)pAgent;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpPullServer(HP_TcpPullServer pServer)
{
	delete (C_HP_TcpPullServer*)pServer;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpPullClient(HP_TcpPullClient pClient)
{
	delete (C_HP_TcpPullClient*)pClient;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpPullAgent(HP_TcpPullAgent pAgent)
{
	delete (C_HP_TcpPullAgent*)pAgent;
}

HPSOCKET_API void __stdcall Destroy_HP_UdpServer(HP_UdpServer pServer)
{
	delete (C_HP_UdpServer*)pServer;
}

HPSOCKET_API void __stdcall Destroy_HP_UdpClient(HP_UdpClient pClient)
{
	delete (C_HP_UdpClient*)pClient;
}

HPSOCKET_API HP_TcpServerListener __stdcall Create_HP_TcpServerListener()
{
	return (HP_TcpServerListener)(new C_HP_ServerListener);
}

HPSOCKET_API HP_TcpClientListener __stdcall Create_HP_TcpClientListener()
{
	return (HP_TcpClientListener)(new C_HP_ClientListener);
}

HPSOCKET_API HP_TcpAgentListener __stdcall Create_HP_TcpAgentListener()
{
	return (HP_TcpAgentListener)(new C_HP_AgentListener);
}

HPSOCKET_API HP_TcpPullServerListener __stdcall Create_HP_TcpPullServerListener()
{
	return (HP_TcpPullServerListener)(new C_HP_ServerListener);
}

HPSOCKET_API HP_TcpPullClientListener __stdcall Create_HP_TcpPullClientListener()
{
	return (HP_TcpPullClientListener)(new C_HP_ClientListener);
}

HPSOCKET_API HP_TcpPullAgentListener __stdcall Create_HP_TcpPullAgentListener()
{
	return (HP_TcpPullAgentListener)(new C_HP_AgentListener);
}

HPSOCKET_API HP_UdpServerListener __stdcall Create_HP_UdpServerListener()
{
	return (HP_UdpServerListener)(new C_HP_ServerListener);
}

HPSOCKET_API HP_UdpClientListener __stdcall Create_HP_UdpClientListener()
{
	return (HP_UdpClientListener)(new C_HP_ClientListener);
}

HPSOCKET_API void __stdcall Destroy_HP_TcpServerListener(HP_TcpServerListener pListener)
{
	delete (C_HP_ServerListener*)pListener;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpClientListener(HP_TcpClientListener pListener)
{
	delete (C_HP_ClientListener*)pListener;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpAgentListener(HP_TcpAgentListener pListener)
{
	delete (C_HP_AgentListener*)pListener;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpPullServerListener(HP_TcpPullServerListener pListener)
{
	delete (C_HP_ServerListener*)pListener;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpPullClientListener(HP_TcpPullClientListener pListener)
{
	delete (C_HP_ClientListener*)pListener;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpPullAgentListener(HP_TcpPullAgentListener pListener)
{
	delete (C_HP_AgentListener*)pListener;
}

HPSOCKET_API void __stdcall Destroy_HP_UdpServerListener(HP_UdpServerListener pListener)
{
	delete (C_HP_ServerListener*)pListener;
}

HPSOCKET_API void __stdcall Destroy_HP_UdpClientListener(HP_UdpClientListener pListener)
{
	delete (C_HP_ClientListener*)pListener;
}

/**********************************************************************************/
/***************************** Server 回调函数设置方法 *****************************/

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnPrepareListen(HP_ServerListener pListener, HP_FN_OnPrepareListen fn)
{
	((C_HP_ServerListener*)pListener)->m_fnOnPrepareListen = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnAccept(HP_ServerListener pListener, HP_FN_OnAccept fn)
{
	((C_HP_ServerListener*)pListener)->m_fnOnAccept = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnSend(HP_ServerListener pListener, HP_FN_OnSend fn)
{
	((C_HP_ServerListener*)pListener)->m_fnOnSend = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnReceive(HP_ServerListener pListener, HP_FN_OnReceive fn)
{
	((C_HP_ServerListener*)pListener)->m_fnOnReceive = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnPullReceive(HP_ServerListener pListener, HP_FN_OnPullReceive fn)
{
	((C_HP_ServerListener*)pListener)->m_fnOnPullReceive = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnClose(HP_ServerListener pListener, HP_FN_OnClose fn)
{
	((C_HP_ServerListener*)pListener)->m_fnOnClose = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnError(HP_ServerListener pListener, HP_FN_OnError fn)
{
	((C_HP_ServerListener*)pListener)->m_fnOnError = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnServerShutdown(HP_ServerListener pListener, HP_FN_OnServerShutdown fn)
{
	((C_HP_ServerListener*)pListener)->m_fnOnServerShutdown = fn;
}

/**********************************************************************************/
/***************************** Client 回调函数设置方法 *****************************/

HPSOCKET_API void __stdcall HP_Set_FN_Client_OnPrepareConnect(HP_ClientListener pListener, HP_FN_OnPrepareConnect fn)
{
	((C_HP_ClientListener*)pListener)->m_fnOnPrepareConnect = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Client_OnConnect(HP_ClientListener pListener, HP_FN_OnConnect fn)
{
	((C_HP_ClientListener*)pListener)->m_fnOnConnect = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Client_OnSend(HP_ClientListener pListener, HP_FN_OnSend fn)
{
	((C_HP_ClientListener*)pListener)->m_fnOnSend = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Client_OnReceive(HP_ClientListener pListener, HP_FN_OnReceive fn)
{
	((C_HP_ClientListener*)pListener)->m_fnOnReceive = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Client_OnPullReceive(HP_ClientListener pListener, HP_FN_OnPullReceive fn)
{
	((C_HP_ClientListener*)pListener)->m_fnOnPullReceive = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Client_OnClose(HP_ClientListener pListener, HP_FN_OnClose fn)
{
	((C_HP_ClientListener*)pListener)->m_fnOnClose = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Client_OnError(HP_ClientListener pListener, HP_FN_OnError fn)
{
	((C_HP_ClientListener*)pListener)->m_fnOnError = fn;
}

/**********************************************************************************/
/***************************** Agent 回调函数设置方法 *****************************/

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnPrepareConnect(HP_AgentListener pListener, HP_FN_OnPrepareConnect fn)
{
	((C_HP_AgentListener*)pListener)->m_fnOnPrepareConnect = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnConnect(HP_AgentListener pListener, HP_FN_OnConnect fn)
{
	((C_HP_AgentListener*)pListener)->m_fnOnConnect = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnSend(HP_AgentListener pListener, HP_FN_OnSend fn)
{
	((C_HP_AgentListener*)pListener)->m_fnOnSend = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnReceive(HP_AgentListener pListener, HP_FN_OnReceive fn)
{
	((C_HP_AgentListener*)pListener)->m_fnOnReceive = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnPullReceive(HP_AgentListener pListener, HP_FN_OnPullReceive fn)
{
	((C_HP_AgentListener*)pListener)->m_fnOnPullReceive = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnClose(HP_AgentListener pListener, HP_FN_OnClose fn)
{
	((C_HP_AgentListener*)pListener)->m_fnOnClose = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnError(HP_AgentListener pListener, HP_FN_OnError fn)
{
	((C_HP_AgentListener*)pListener)->m_fnOnError = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnAgentShutdown(HP_AgentListener pListener, HP_FN_OnAgentShutdown fn)
{
	((C_HP_AgentListener*)pListener)->m_fnOnAgentShutdown = fn;
}

/**************************************************************************/
/***************************** Server 操作方法 *****************************/

HPSOCKET_API BOOL __stdcall HP_Server_Start(HP_Server pServer, LPCTSTR pszBindAddress, USHORT usPort)
{
	return C_HP_Object::ToServer(pServer)->Start(pszBindAddress, usPort);
}

HPSOCKET_API BOOL __stdcall HP_Server_Stop(HP_Server pServer)
{
	return C_HP_Object::ToServer(pServer)->Stop();
}

HPSOCKET_API BOOL __stdcall HP_Server_Send(HP_Server pServer, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength)
{
	return C_HP_Object::ToServer(pServer)->Send(dwConnID, pBuffer, iLength);
}

HPSOCKET_API BOOL __stdcall HP_Server_SendPart(HP_Server pServer, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset)
{
	return C_HP_Object::ToServer(pServer)->Send(dwConnID, pBuffer, iLength, iOffset);
}

HPSOCKET_API BOOL __stdcall HP_Server_Disconnect(HP_Server pServer, HP_CONNID dwConnID, BOOL bForce)
{
	return C_HP_Object::ToServer(pServer)->Disconnect(dwConnID, bForce);
}

HPSOCKET_API BOOL __stdcall HP_Server_DisconnectLongConnections(HP_Server pServer, DWORD dwPeriod, BOOL bForce)
{
	return C_HP_Object::ToServer(pServer)->DisconnectLongConnections(dwPeriod, bForce);
}

/******************************************************************************/
/***************************** Server 属性访问方法 *****************************/

HPSOCKET_API BOOL __stdcall HP_Server_SetConnectionExtra(HP_Server pServer, HP_CONNID dwConnID, PVOID pExtra)
{
	return C_HP_Object::ToServer(pServer)->SetConnectionExtra(dwConnID, pExtra);
}

HPSOCKET_API BOOL __stdcall HP_Server_GetConnectionExtra(HP_Server pServer, HP_CONNID dwConnID, PVOID* ppExtra)
{
	return C_HP_Object::ToServer(pServer)->GetConnectionExtra(dwConnID, ppExtra);
}

HPSOCKET_API BOOL __stdcall HP_Server_HasStarted(HP_Server pServer)
{
	return C_HP_Object::ToServer(pServer)->HasStarted();
}

HPSOCKET_API En_HP_ServiceState __stdcall HP_Server_GetState(HP_Server pServer)
{
	return (En_HP_ServiceState)C_HP_Object::ToServer(pServer)->GetState();
}

HPSOCKET_API En_HP_SocketError __stdcall HP_Server_GetLastError(HP_Server pServer)
{
	return (En_HP_SocketError)C_HP_Object::ToServer(pServer)->GetLastError();
}

HPSOCKET_API LPCTSTR __stdcall HP_Server_GetLastErrorDesc(HP_Server pServer)
{
	return C_HP_Object::ToServer(pServer)->GetLastErrorDesc();
}

HPSOCKET_API BOOL __stdcall HP_Server_GetPendingDataLength(HP_Server pServer, HP_CONNID dwConnID, int* piPending)
{
	return C_HP_Object::ToServer(pServer)->GetPendingDataLength(dwConnID, *piPending);
}

HPSOCKET_API DWORD __stdcall HP_Server_GetConnectionCount(HP_Server pServer)
{
	return C_HP_Object::ToServer(pServer)->GetConnectionCount();
}

HPSOCKET_API BOOL __stdcall HP_Server_GetConnectPeriod(HP_Server pServer, HP_CONNID dwConnID, DWORD* pdwPeriod)
{
	return C_HP_Object::ToServer(pServer)->GetConnectPeriod(dwConnID, *pdwPeriod);
}

HPSOCKET_API BOOL __stdcall HP_Server_GetListenAddress(HP_Server pServer, LPTSTR lpszAddress, int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToServer(pServer)->GetListenAddress(lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API BOOL __stdcall HP_Server_GetRemoteAddress(HP_Server pServer, HP_CONNID dwConnID, LPTSTR lpszAddress, int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToServer(pServer)->GetRemoteAddress(dwConnID, lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API void __stdcall HP_Server_SetSendPolicy(HP_Server pServer, En_HP_SendPolicy enSendPolicy)
{
	C_HP_Object::ToServer(pServer)->SetSendPolicy((EnSendPolicy)enSendPolicy);
}

HPSOCKET_API void __stdcall HP_Server_SetFreeSocketObjLockTime(HP_Server pServer, DWORD dwFreeSocketObjLockTime)
{
	C_HP_Object::ToServer(pServer)->SetFreeSocketObjLockTime(dwFreeSocketObjLockTime);
}

HPSOCKET_API void __stdcall HP_Server_SetFreeSocketObjPool(HP_Server pServer, DWORD dwFreeSocketObjPool)
{
	C_HP_Object::ToServer(pServer)->SetFreeSocketObjPool(dwFreeSocketObjPool);
}

HPSOCKET_API void __stdcall HP_Server_SetFreeBufferObjPool(HP_Server pServer, DWORD dwFreeBufferObjPool)
{
	C_HP_Object::ToServer(pServer)->SetFreeBufferObjPool(dwFreeBufferObjPool);
}

HPSOCKET_API void __stdcall HP_Server_SetFreeSocketObjHold(HP_Server pServer, DWORD dwFreeSocketObjHold)
{
	C_HP_Object::ToServer(pServer)->SetFreeSocketObjHold(dwFreeSocketObjHold);
}

HPSOCKET_API void __stdcall HP_Server_SetFreeBufferObjHold(HP_Server pServer, DWORD dwFreeBufferObjHold)
{
	C_HP_Object::ToServer(pServer)->SetFreeBufferObjHold(dwFreeBufferObjHold);
}

HPSOCKET_API void __stdcall HP_Server_SetWorkerThreadCount(HP_Server pServer, DWORD dwWorkerThreadCount)
{
	C_HP_Object::ToServer(pServer)->SetWorkerThreadCount(dwWorkerThreadCount);
}

HPSOCKET_API void __stdcall HP_Server_SetMaxShutdownWaitTime(HP_Server pServer, DWORD dwMaxShutdownWaitTime)
{
	C_HP_Object::ToServer(pServer)->SetMaxShutdownWaitTime(dwMaxShutdownWaitTime);
}

HPSOCKET_API En_HP_SendPolicy __stdcall HP_Server_GetSendPolicy(HP_Server pServer)
{
	return (En_HP_SendPolicy)C_HP_Object::ToServer(pServer)->GetSendPolicy();
}

HPSOCKET_API DWORD __stdcall HP_Server_GetFreeSocketObjLockTime(HP_Server pServer)
{
	return C_HP_Object::ToServer(pServer)->GetFreeSocketObjLockTime();
}

HPSOCKET_API DWORD __stdcall HP_Server_GetFreeSocketObjPool(HP_Server pServer)
{
	return C_HP_Object::ToServer(pServer)->GetFreeSocketObjPool();
}

HPSOCKET_API DWORD __stdcall HP_Server_GetFreeBufferObjPool(HP_Server pServer)
{
	return C_HP_Object::ToServer(pServer)->GetFreeBufferObjPool();
}

HPSOCKET_API DWORD __stdcall HP_Server_GetFreeSocketObjHold(HP_Server pServer)
{
	return C_HP_Object::ToServer(pServer)->GetFreeSocketObjHold();
}

HPSOCKET_API DWORD __stdcall HP_Server_GetFreeBufferObjHold(HP_Server pServer)
{
	return C_HP_Object::ToServer(pServer)->GetFreeBufferObjHold();
}

HPSOCKET_API DWORD __stdcall HP_Server_GetWorkerThreadCount(HP_Server pServer)
{
	return C_HP_Object::ToServer(pServer)->GetWorkerThreadCount();
}

HPSOCKET_API DWORD __stdcall HP_Server_GetMaxShutdownWaitTime(HP_Server pServer)
{
	return C_HP_Object::ToServer(pServer)->GetMaxShutdownWaitTime();
}

/**********************************************************************************/
/***************************** TCP Server 属性访问方法 *****************************/

HPSOCKET_API void __stdcall HP_TcpServer_SetAcceptSocketCount(HP_TcpServer pServer, DWORD dwAcceptSocketCount)
{
	C_HP_Object::ToTcpServer(pServer)->SetAcceptSocketCount(dwAcceptSocketCount);
}

HPSOCKET_API void __stdcall HP_TcpServer_SetSocketBufferSize(HP_TcpServer pServer, DWORD dwSocketBufferSize)
{
	C_HP_Object::ToTcpServer(pServer)->SetSocketBufferSize(dwSocketBufferSize);
}

HPSOCKET_API void __stdcall HP_TcpServer_SetSocketListenQueue(HP_TcpServer pServer, DWORD dwSocketListenQueue)
{
	C_HP_Object::ToTcpServer(pServer)->SetSocketListenQueue(dwSocketListenQueue);
}

HPSOCKET_API void __stdcall HP_TcpServer_SetKeepAliveTime(HP_TcpServer pServer, DWORD dwKeepAliveTime)
{
	C_HP_Object::ToTcpServer(pServer)->SetKeepAliveTime(dwKeepAliveTime);
}

HPSOCKET_API void __stdcall HP_TcpServer_SetKeepAliveInterval(HP_TcpServer pServer, DWORD dwKeepAliveInterval)
{
	C_HP_Object::ToTcpServer(pServer)->SetKeepAliveInterval(dwKeepAliveInterval);
}

HPSOCKET_API DWORD __stdcall HP_TcpServer_GetAcceptSocketCount(HP_TcpServer pServer)
{
	return C_HP_Object::ToTcpServer(pServer)->GetAcceptSocketCount();
}

HPSOCKET_API DWORD __stdcall HP_TcpServer_GetSocketBufferSize(HP_TcpServer pServer)
{
	return C_HP_Object::ToTcpServer(pServer)->GetSocketBufferSize();
}

HPSOCKET_API DWORD __stdcall HP_TcpServer_GetSocketListenQueue(HP_TcpServer pServer)
{
	return C_HP_Object::ToTcpServer(pServer)->GetSocketListenQueue();
}

HPSOCKET_API DWORD __stdcall HP_TcpServer_GetKeepAliveTime(HP_TcpServer pServer)
{
	return C_HP_Object::ToTcpServer(pServer)->GetKeepAliveTime();
}

HPSOCKET_API DWORD __stdcall HP_TcpServer_GetKeepAliveInterval(HP_TcpServer pServer)
{
	return C_HP_Object::ToTcpServer(pServer)->GetKeepAliveInterval();
}

/**********************************************************************************/
/***************************** UDP Server 属性访问方法 *****************************/

HPSOCKET_API void __stdcall HP_UdpServer_SetMaxDatagramSize(HP_UdpServer pServer, DWORD dwMaxDatagramSize)
{
	C_HP_Object::ToUdpServer(pServer)->SetMaxDatagramSize(dwMaxDatagramSize);
}

HPSOCKET_API DWORD __stdcall HP_UdpServer_GetMaxDatagramSize(HP_UdpServer pServer)
{
	return C_HP_Object::ToUdpServer(pServer)->GetMaxDatagramSize();
}

HPSOCKET_API void __stdcall HP_UdpServer_SetDetectAttempts(HP_UdpServer pServer, DWORD dwDetectAttempts)
{
	C_HP_Object::ToUdpServer(pServer)->SetDetectAttempts(dwDetectAttempts);
}

HPSOCKET_API void __stdcall HP_UdpServer_SetDetectInterval(HP_UdpServer pServer, DWORD dwDetectInterval)
{
	C_HP_Object::ToUdpServer(pServer)->SetDetectInterval(dwDetectInterval);
}

HPSOCKET_API DWORD __stdcall HP_UdpServer_GetDetectAttempts(HP_UdpServer pServer)
{
	return C_HP_Object::ToUdpServer(pServer)->GetDetectAttempts();
}

HPSOCKET_API DWORD __stdcall HP_UdpServer_GetDetectInterval(HP_UdpServer pServer)
{
	return C_HP_Object::ToUdpServer(pServer)->GetDetectInterval();
}

/******************************************************************************/
/***************************** Client 组件操作方法 *****************************/

HPSOCKET_API BOOL __stdcall HP_Client_Start(HP_Client pClient, LPCTSTR pszRemoteAddress, USHORT usPort, BOOL bAsyncConnect)
{
	return C_HP_Object::ToClient(pClient)->Start(pszRemoteAddress, usPort, bAsyncConnect);
}

HPSOCKET_API BOOL __stdcall HP_Client_Stop(HP_Client pClient)
{
	return C_HP_Object::ToClient(pClient)->Stop();
}

HPSOCKET_API BOOL __stdcall HP_Client_Send(HP_Client pClient, const BYTE* pBuffer, int iLength)
{
	return C_HP_Object::ToClient(pClient)->Send(pBuffer, iLength);
}

HPSOCKET_API BOOL __stdcall HP_Client_SendPart(HP_Client pClient, const BYTE* pBuffer, int iLength, int iOffset)
{
	return C_HP_Object::ToClient(pClient)->Send(pBuffer, iLength, iOffset);
}

/******************************************************************************/
/***************************** Client 属性访问方法 *****************************/

HPSOCKET_API BOOL __stdcall HP_Client_HasStarted(HP_Client pClient)
{
	return C_HP_Object::ToClient(pClient)->HasStarted();
}

HPSOCKET_API En_HP_ServiceState	__stdcall HP_Client_GetState(HP_Client pClient)
{
	return (En_HP_ServiceState)C_HP_Object::ToClient(pClient)->GetState();
}

HPSOCKET_API En_HP_SocketError	__stdcall HP_Client_GetLastError(HP_Client pClient)
{
	return (En_HP_SocketError)C_HP_Object::ToClient(pClient)->GetLastError();
}

HPSOCKET_API LPCTSTR __stdcall HP_Client_GetLastErrorDesc(HP_Client pClient)
{
	return C_HP_Object::ToClient(pClient)->GetLastErrorDesc();
}

HPSOCKET_API HP_CONNID __stdcall HP_Client_GetConnectionID(HP_Client pClient)
{
	return C_HP_Object::ToClient(pClient)->GetConnectionID();
}

HPSOCKET_API BOOL __stdcall HP_Client_GetLocalAddress(HP_Client pClient, LPTSTR lpszAddress, int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToClient(pClient)->GetLocalAddress(lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API BOOL __stdcall HP_Client_GetPendingDataLength(HP_Client pClient, int* piPending)
{
	return C_HP_Object::ToClient(pClient)->GetPendingDataLength(*piPending);
}

HPSOCKET_API void __stdcall HP_Client_SetFreeBufferPoolSize(HP_Client pClient, DWORD dwFreeBufferPoolSize)
{
	C_HP_Object::ToClient(pClient)->SetFreeBufferPoolSize(dwFreeBufferPoolSize);
}

HPSOCKET_API void __stdcall HP_Client_SetFreeBufferPoolHold(HP_Client pClient, DWORD dwFreeBufferPoolHold)
{
	C_HP_Object::ToClient(pClient)->SetFreeBufferPoolHold(dwFreeBufferPoolHold);
}

HPSOCKET_API DWORD __stdcall HP_Client_GetFreeBufferPoolSize(HP_Client pClient)
{
	return C_HP_Object::ToClient(pClient)->GetFreeBufferPoolSize();
}

HPSOCKET_API DWORD __stdcall HP_Client_GetFreeBufferPoolHold(HP_Client pClient)
{
	return C_HP_Object::ToClient(pClient)->GetFreeBufferPoolHold();
}

/**********************************************************************************/
/***************************** TCP Client 属性访问方法 *****************************/
	
HPSOCKET_API void __stdcall HP_TcpClient_SetSocketBufferSize(HP_TcpClient pClient, DWORD dwSocketBufferSize)
{
	C_HP_Object::ToTcpClient(pClient)->SetSocketBufferSize(dwSocketBufferSize);
}

HPSOCKET_API void __stdcall HP_TcpClient_SetKeepAliveTime(HP_TcpClient pClient, DWORD dwKeepAliveTime)
{
	C_HP_Object::ToTcpClient(pClient)->SetKeepAliveTime(dwKeepAliveTime);
}

HPSOCKET_API void __stdcall HP_TcpClient_SetKeepAliveInterval(HP_TcpClient pClient, DWORD dwKeepAliveInterval)
{
	C_HP_Object::ToTcpClient(pClient)->SetKeepAliveInterval(dwKeepAliveInterval);
}

HPSOCKET_API DWORD __stdcall HP_TcpClient_GetSocketBufferSize(HP_TcpClient pClient)
{
	return C_HP_Object::ToTcpClient(pClient)->GetSocketBufferSize();
}

HPSOCKET_API DWORD __stdcall HP_TcpClient_GetKeepAliveTime(HP_TcpClient pClient)
{
	return C_HP_Object::ToTcpClient(pClient)->GetKeepAliveTime();
}

HPSOCKET_API DWORD __stdcall HP_TcpClient_GetKeepAliveInterval(HP_TcpClient pClient)
{
	return C_HP_Object::ToTcpClient(pClient)->GetKeepAliveInterval();
}

/**********************************************************************************/
/***************************** UDP Client 属性访问方法 *****************************/

HPSOCKET_API void __stdcall HP_UdpClient_SetMaxDatagramSize(HP_UdpClient pClient, DWORD dwMaxDatagramSize)
{
	C_HP_Object::ToUdpClient(pClient)->SetMaxDatagramSize(dwMaxDatagramSize);
}

HPSOCKET_API DWORD __stdcall HP_UdpClient_GetMaxDatagramSize(HP_UdpClient pClient)
{
	return C_HP_Object::ToUdpClient(pClient)->GetMaxDatagramSize();
}

HPSOCKET_API void __stdcall HP_UdpClient_SetDetectAttempts(HP_UdpClient pClient, DWORD dwDetectAttempts)
{
	C_HP_Object::ToUdpClient(pClient)->SetDetectAttempts(dwDetectAttempts);
}

HPSOCKET_API void __stdcall HP_UdpClient_SetDetectInterval(HP_UdpClient pClient, DWORD dwDetectInterval)
{
	C_HP_Object::ToUdpClient(pClient)->SetDetectInterval(dwDetectInterval);
}

HPSOCKET_API DWORD __stdcall HP_UdpClient_GetDetectAttempts(HP_UdpClient pClient)
{
	return C_HP_Object::ToUdpClient(pClient)->GetDetectAttempts();
}

HPSOCKET_API DWORD __stdcall HP_UdpClient_GetDetectInterval(HP_UdpClient pClient)
{
	return C_HP_Object::ToUdpClient(pClient)->GetDetectInterval();
}

/**************************************************************************/
/***************************** Agent 操作方法 *****************************/

HPSOCKET_API BOOL __stdcall HP_Agent_Start(HP_Agent pAgent, LPCTSTR pszBindAddress, BOOL bAsyncConnect)
{
	return C_HP_Object::ToAgent(pAgent)->Start(pszBindAddress, bAsyncConnect);
}

HPSOCKET_API BOOL __stdcall HP_Agent_Stop(HP_Agent pAgent)
{
	return C_HP_Object::ToAgent(pAgent)->Stop();
}

HPSOCKET_API BOOL __stdcall HP_Agent_Connect(HP_Agent pAgent, LPCTSTR pszRemoteAddress, USHORT usPort, HP_CONNID* pdwConnID)
{
	return C_HP_Object::ToAgent(pAgent)->Connect(pszRemoteAddress, usPort, pdwConnID);
}

HPSOCKET_API BOOL __stdcall HP_Agent_Send(HP_Agent pAgent, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength)
{
	return C_HP_Object::ToAgent(pAgent)->Send(dwConnID, pBuffer, iLength);
}

HPSOCKET_API BOOL __stdcall HP_Agent_SendPart(HP_Agent pAgent, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset)
{
	return C_HP_Object::ToAgent(pAgent)->Send(dwConnID, pBuffer, iLength, iOffset);
}

HPSOCKET_API BOOL __stdcall HP_Agent_Disconnect(HP_Agent pAgent, HP_CONNID dwConnID, BOOL bForce)
{
	return C_HP_Object::ToAgent(pAgent)->Disconnect(dwConnID, bForce);
}

HPSOCKET_API BOOL __stdcall HP_Agent_DisconnectLongConnections(HP_Agent pAgent, DWORD dwPeriod, BOOL bForce)
{
	return C_HP_Object::ToAgent(pAgent)->DisconnectLongConnections(dwPeriod, bForce);
}

/******************************************************************************/
/***************************** Agent 属性访问方法 *****************************/

HPSOCKET_API BOOL __stdcall HP_Agent_SetConnectionExtra(HP_Agent pAgent, HP_CONNID dwConnID, PVOID pExtra)
{
	return C_HP_Object::ToAgent(pAgent)->SetConnectionExtra(dwConnID, pExtra);
}

HPSOCKET_API BOOL __stdcall HP_Agent_GetConnectionExtra(HP_Agent pAgent, HP_CONNID dwConnID, PVOID* ppExtra)
{
	return C_HP_Object::ToAgent(pAgent)->GetConnectionExtra(dwConnID, ppExtra);
}

HPSOCKET_API BOOL __stdcall HP_Agent_HasStarted(HP_Agent pAgent)
{
	return C_HP_Object::ToAgent(pAgent)->HasStarted();
}

HPSOCKET_API En_HP_ServiceState __stdcall HP_Agent_GetState(HP_Agent pAgent)
{
	return (En_HP_ServiceState)C_HP_Object::ToAgent(pAgent)->GetState();
}

HPSOCKET_API En_HP_SocketError __stdcall HP_Agent_GetLastError(HP_Agent pAgent)
{
	return (En_HP_SocketError)C_HP_Object::ToAgent(pAgent)->GetLastError();
}

HPSOCKET_API LPCTSTR __stdcall HP_Agent_GetLastErrorDesc(HP_Agent pAgent)
{
	return C_HP_Object::ToAgent(pAgent)->GetLastErrorDesc();
}

HPSOCKET_API BOOL __stdcall HP_Agent_GetPendingDataLength(HP_Agent pAgent, HP_CONNID dwConnID, int* piPending)
{
	return C_HP_Object::ToAgent(pAgent)->GetPendingDataLength(dwConnID, *piPending);
}

HPSOCKET_API DWORD __stdcall HP_Agent_GetConnectionCount(HP_Agent pAgent)
{
	return C_HP_Object::ToAgent(pAgent)->GetConnectionCount();
}

HPSOCKET_API BOOL __stdcall HP_Agent_GetConnectPeriod(HP_Agent pAgent, HP_CONNID dwConnID, DWORD* pdwPeriod)
{
	return C_HP_Object::ToAgent(pAgent)->GetConnectPeriod(dwConnID, *pdwPeriod);
}

HPSOCKET_API BOOL __stdcall HP_Agent_GetLocalAddress(HP_Agent pAgent, HP_CONNID dwConnID, LPTSTR lpszAddress, int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToAgent(pAgent)->GetLocalAddress(dwConnID, lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API BOOL __stdcall HP_Agent_GetRemoteAddress(HP_Agent pAgent, HP_CONNID dwConnID, LPTSTR lpszAddress, int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToAgent(pAgent)->GetRemoteAddress(dwConnID, lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API void __stdcall HP_Agent_SetSendPolicy(HP_Agent pAgent, En_HP_SendPolicy enSendPolicy)
{
	C_HP_Object::ToAgent(pAgent)->SetSendPolicy((EnSendPolicy)enSendPolicy);
}

HPSOCKET_API void __stdcall HP_Agent_SetFreeSocketObjLockTime(HP_Agent pAgent, DWORD dwFreeSocketObjLockTime)
{
	C_HP_Object::ToAgent(pAgent)->SetFreeSocketObjLockTime(dwFreeSocketObjLockTime);
}

HPSOCKET_API void __stdcall HP_Agent_SetFreeSocketObjPool(HP_Agent pAgent, DWORD dwFreeSocketObjPool)
{
	C_HP_Object::ToAgent(pAgent)->SetFreeSocketObjPool(dwFreeSocketObjPool);
}

HPSOCKET_API void __stdcall HP_Agent_SetFreeBufferObjPool(HP_Agent pAgent, DWORD dwFreeBufferObjPool)
{
	C_HP_Object::ToAgent(pAgent)->SetFreeBufferObjPool(dwFreeBufferObjPool);
}

HPSOCKET_API void __stdcall HP_Agent_SetFreeSocketObjHold(HP_Agent pAgent, DWORD dwFreeSocketObjHold)
{
	C_HP_Object::ToAgent(pAgent)->SetFreeSocketObjHold(dwFreeSocketObjHold);
}

HPSOCKET_API void __stdcall HP_Agent_SetFreeBufferObjHold(HP_Agent pAgent, DWORD dwFreeBufferObjHold)
{
	C_HP_Object::ToAgent(pAgent)->SetFreeBufferObjHold(dwFreeBufferObjHold);
}

HPSOCKET_API void __stdcall HP_Agent_SetWorkerThreadCount(HP_Agent pAgent, DWORD dwWorkerThreadCount)
{
	C_HP_Object::ToAgent(pAgent)->SetWorkerThreadCount(dwWorkerThreadCount);
}

HPSOCKET_API void __stdcall HP_Agent_SetMaxShutdownWaitTime(HP_Agent pAgent, DWORD dwMaxShutdownWaitTime)
{
	C_HP_Object::ToAgent(pAgent)->SetMaxShutdownWaitTime(dwMaxShutdownWaitTime);
}

HPSOCKET_API En_HP_SendPolicy __stdcall HP_Agent_GetSendPolicy(HP_Agent pAgent)
{
	return (En_HP_SendPolicy)C_HP_Object::ToAgent(pAgent)->GetSendPolicy();
}

HPSOCKET_API DWORD __stdcall HP_Agent_GetFreeSocketObjLockTime(HP_Agent pAgent)
{
	return C_HP_Object::ToAgent(pAgent)->GetFreeSocketObjLockTime();
}

HPSOCKET_API DWORD __stdcall HP_Agent_GetFreeSocketObjPool(HP_Agent pAgent)
{
	return C_HP_Object::ToAgent(pAgent)->GetFreeSocketObjPool();
}

HPSOCKET_API DWORD __stdcall HP_Agent_GetFreeBufferObjPool(HP_Agent pAgent)
{
	return C_HP_Object::ToAgent(pAgent)->GetFreeBufferObjPool();
}

HPSOCKET_API DWORD __stdcall HP_Agent_GetFreeSocketObjHold(HP_Agent pAgent)
{
	return C_HP_Object::ToAgent(pAgent)->GetFreeSocketObjHold();
}

HPSOCKET_API DWORD __stdcall HP_Agent_GetFreeBufferObjHold(HP_Agent pAgent)
{
	return C_HP_Object::ToAgent(pAgent)->GetFreeBufferObjHold();
}

HPSOCKET_API DWORD __stdcall HP_Agent_GetWorkerThreadCount(HP_Agent pAgent)
{
	return C_HP_Object::ToAgent(pAgent)->GetWorkerThreadCount();
}

HPSOCKET_API DWORD __stdcall HP_Agent_GetMaxShutdownWaitTime(HP_Agent pAgent)
{
	return C_HP_Object::ToAgent(pAgent)->GetMaxShutdownWaitTime();
}

/**********************************************************************************/
/***************************** TCP Agent 属性访问方法 *****************************/

HPSOCKET_API void __stdcall HP_TcpAgent_SetReuseAddress(HP_TcpAgent pAgent, BOOL bReuseAddress)
{
	C_HP_Object::ToTcpAgent(pAgent)->SetReuseAddress(bReuseAddress);
}

HPSOCKET_API BOOL __stdcall HP_TcpAgent_IsReuseAddress(HP_TcpAgent pAgent)
{
	return C_HP_Object::ToTcpAgent(pAgent)->IsReuseAddress();
}

HPSOCKET_API void __stdcall HP_TcpAgent_SetSocketBufferSize(HP_TcpAgent pAgent, DWORD dwSocketBufferSize)
{
	C_HP_Object::ToTcpAgent(pAgent)->SetSocketBufferSize(dwSocketBufferSize);
}

HPSOCKET_API void __stdcall HP_TcpAgent_SetKeepAliveTime(HP_TcpAgent pAgent, DWORD dwKeepAliveTime)
{
	C_HP_Object::ToTcpAgent(pAgent)->SetKeepAliveTime(dwKeepAliveTime);
}

HPSOCKET_API void __stdcall HP_TcpAgent_SetKeepAliveInterval(HP_TcpAgent pAgent, DWORD dwKeepAliveInterval)
{
	C_HP_Object::ToTcpAgent(pAgent)->SetKeepAliveInterval(dwKeepAliveInterval);
}

HPSOCKET_API DWORD __stdcall HP_TcpAgent_GetSocketBufferSize(HP_TcpAgent pAgent)
{
	return C_HP_Object::ToTcpAgent(pAgent)->GetSocketBufferSize();
}

HPSOCKET_API DWORD __stdcall HP_TcpAgent_GetKeepAliveTime(HP_TcpAgent pAgent)
{
	return C_HP_Object::ToTcpAgent(pAgent)->GetKeepAliveTime();
}

HPSOCKET_API DWORD __stdcall HP_TcpAgent_GetKeepAliveInterval(HP_TcpAgent pAgent)
{
	return C_HP_Object::ToTcpAgent(pAgent)->GetKeepAliveInterval();
}

/***************************************************************************************/
/***************************** TCP Pull Server 组件操作方法 *****************************/

HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullServer_Fetch(HP_TcpPullServer pServer, HP_CONNID dwConnID, BYTE* pBuffer, int iLength)
{
	return (En_HP_FetchResult)C_HP_Object::ToPullSocket(pServer)->Fetch(dwConnID, pBuffer, iLength);
}

/***************************************************************************************/
/***************************** TCP Pull Server 属性访问方法 *****************************/

/***************************************************************************************/
/***************************** TCP Pull Client 组件操作方法 *****************************/

HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullClient_Fetch(HP_TcpPullClient pClient, HP_CONNID dwConnID, BYTE* pBuffer, int iLength)
{
	return (En_HP_FetchResult)C_HP_Object::ToPullSocket(pClient)->Fetch(dwConnID, pBuffer, iLength);
}

/***************************************************************************************/
/***************************** TCP Pull Client 属性访问方法 *****************************/

/***************************************************************************************/
/***************************** TCP Pull Agent 组件操作方法 *****************************/

HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullAgent_Fetch(HP_TcpPullAgent pAgent, HP_CONNID dwConnID, BYTE* pBuffer, int iLength)
{
	return (En_HP_FetchResult)C_HP_Object::ToPullSocket(pAgent)->Fetch(dwConnID, pBuffer, iLength);
}

/***************************************************************************************/
/***************************** TCP Pull Agent 属性访问方法 *****************************/

/***************************************************************************************/
/*************************************** 其它方法 ***************************************/

HPSOCKET_API LPCTSTR __stdcall HP_GetSocketErrorDesc(En_HP_SocketError enCode)
{
	return ::GetSocketErrorDesc((EnSocketError)enCode);
}

HPSOCKET_API DWORD __stdcall SYS_GetLastError()
{
	return ::GetLastError();
}
