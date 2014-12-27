/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.3.1
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
#include "UdpCast.h"

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
	#pragma comment(linker, "/EXPORT:Create_HP_UdpCast=_Create_HP_UdpCast@4")
	#pragma comment(linker, "/EXPORT:Create_HP_UdpCastListener=_Create_HP_UdpCastListener@0")
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
	#pragma comment(linker, "/EXPORT:Destroy_HP_UdpCast=_Destroy_HP_UdpCast@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_UdpCastListener=_Destroy_HP_UdpCastListener@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_UdpClient=_Destroy_HP_UdpClient@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_UdpClientListener=_Destroy_HP_UdpClientListener@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_UdpServer=_Destroy_HP_UdpServer@4")
	#pragma comment(linker, "/EXPORT:Destroy_HP_UdpServerListener=_Destroy_HP_UdpServerListener@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_Connect=_HP_Agent_Connect@16")
	#pragma comment(linker, "/EXPORT:HP_Agent_Disconnect=_HP_Agent_Disconnect@12")
	#pragma comment(linker, "/EXPORT:HP_Agent_DisconnectLongConnections=_HP_Agent_DisconnectLongConnections@12")
	#pragma comment(linker, "/EXPORT:HP_Agent_DisconnectSilenceConnections=_HP_Agent_DisconnectSilenceConnections@12")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetAllConnectionIDs=_HP_Agent_GetAllConnectionIDs@12")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetConnectPeriod=_HP_Agent_GetConnectPeriod@12")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetSilencePeriod=_HP_Agent_GetSilencePeriod@12")
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
	#pragma comment(linker, "/EXPORT:HP_Agent_IsMarkSilence=_HP_Agent_IsMarkSilence@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetPendingDataLength=_HP_Agent_GetPendingDataLength@12")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetRecvPolicy=_HP_Agent_GetRecvPolicy@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetRemoteAddress=_HP_Agent_GetRemoteAddress@20")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetSendPolicy=_HP_Agent_GetSendPolicy@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetState=_HP_Agent_GetState@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_GetWorkerThreadCount=_HP_Agent_GetWorkerThreadCount@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_HasStarted=_HP_Agent_HasStarted@4")
	#pragma comment(linker, "/EXPORT:HP_Agent_Send=_HP_Agent_Send@16")
	#pragma comment(linker, "/EXPORT:HP_Agent_SendPackets=_HP_Agent_SendPackets@16")
	#pragma comment(linker, "/EXPORT:HP_Agent_SendPart=_HP_Agent_SendPart@20")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetConnectionExtra=_HP_Agent_SetConnectionExtra@12")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetFreeBufferObjHold=_HP_Agent_SetFreeBufferObjHold@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetFreeBufferObjPool=_HP_Agent_SetFreeBufferObjPool@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetFreeSocketObjHold=_HP_Agent_SetFreeSocketObjHold@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetFreeSocketObjLockTime=_HP_Agent_SetFreeSocketObjLockTime@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetFreeSocketObjPool=_HP_Agent_SetFreeSocketObjPool@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetMaxShutdownWaitTime=_HP_Agent_SetMaxShutdownWaitTime@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetMarkSilence=_HP_Agent_SetMarkSilence@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetRecvPolicy=_HP_Agent_SetRecvPolicy@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetSendPolicy=_HP_Agent_SetSendPolicy@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_SetWorkerThreadCount=_HP_Agent_SetWorkerThreadCount@8")
	#pragma comment(linker, "/EXPORT:HP_Agent_Start=_HP_Agent_Start@12")
	#pragma comment(linker, "/EXPORT:HP_Agent_Stop=_HP_Agent_Stop@4")
	#pragma comment(linker, "/EXPORT:HP_Client_GetConnectionID=_HP_Client_GetConnectionID@4")
	#pragma comment(linker, "/EXPORT:HP_Client_GetExtra=_HP_Client_GetExtra@4")
	#pragma comment(linker, "/EXPORT:HP_Client_GetFreeBufferPoolHold=_HP_Client_GetFreeBufferPoolHold@4")
	#pragma comment(linker, "/EXPORT:HP_Client_GetFreeBufferPoolSize=_HP_Client_GetFreeBufferPoolSize@4")
	#pragma comment(linker, "/EXPORT:HP_Client_GetLastError=_HP_Client_GetLastError@4")
	#pragma comment(linker, "/EXPORT:HP_Client_GetLastErrorDesc=_HP_Client_GetLastErrorDesc@4")
	#pragma comment(linker, "/EXPORT:HP_Client_GetLocalAddress=_HP_Client_GetLocalAddress@16")
	#pragma comment(linker, "/EXPORT:HP_Client_GetPendingDataLength=_HP_Client_GetPendingDataLength@8")
	#pragma comment(linker, "/EXPORT:HP_Client_GetState=_HP_Client_GetState@4")
	#pragma comment(linker, "/EXPORT:HP_Client_HasStarted=_HP_Client_HasStarted@4")
	#pragma comment(linker, "/EXPORT:HP_Client_Send=_HP_Client_Send@12")
	#pragma comment(linker, "/EXPORT:HP_Client_SendPackets=_HP_Client_SendPackets@12")
	#pragma comment(linker, "/EXPORT:HP_Client_SendPart=_HP_Client_SendPart@16")
	#pragma comment(linker, "/EXPORT:HP_Client_SetExtra=_HP_Client_SetExtra@8")
	#pragma comment(linker, "/EXPORT:HP_Client_SetFreeBufferPoolHold=_HP_Client_SetFreeBufferPoolHold@8")
	#pragma comment(linker, "/EXPORT:HP_Client_SetFreeBufferPoolSize=_HP_Client_SetFreeBufferPoolSize@8")
	#pragma comment(linker, "/EXPORT:HP_Client_Start=_HP_Client_Start@16")
	#pragma comment(linker, "/EXPORT:HP_Client_Stop=_HP_Client_Stop@4")
	#pragma comment(linker, "/EXPORT:HP_GetSocketErrorDesc=_HP_GetSocketErrorDesc@4")
	#pragma comment(linker, "/EXPORT:HP_Server_Disconnect=_HP_Server_Disconnect@12")
	#pragma comment(linker, "/EXPORT:HP_Server_DisconnectLongConnections=_HP_Server_DisconnectLongConnections@12")
	#pragma comment(linker, "/EXPORT:HP_Server_DisconnectSilenceConnections=_HP_Server_DisconnectSilenceConnections@12")
	#pragma comment(linker, "/EXPORT:HP_Server_GetAllConnectionIDs=_HP_Server_GetAllConnectionIDs@12")
	#pragma comment(linker, "/EXPORT:HP_Server_GetConnectPeriod=_HP_Server_GetConnectPeriod@12")
	#pragma comment(linker, "/EXPORT:HP_Server_GetSilencePeriod=_HP_Server_GetSilencePeriod@12")
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
	#pragma comment(linker, "/EXPORT:HP_Server_IsMarkSilence=_HP_Server_IsMarkSilence@4")
	#pragma comment(linker, "/EXPORT:HP_Server_GetPendingDataLength=_HP_Server_GetPendingDataLength@12")
	#pragma comment(linker, "/EXPORT:HP_Server_GetRecvPolicy=_HP_Server_GetRecvPolicy@4")
	#pragma comment(linker, "/EXPORT:HP_Server_GetRemoteAddress=_HP_Server_GetRemoteAddress@20")
	#pragma comment(linker, "/EXPORT:HP_Server_GetSendPolicy=_HP_Server_GetSendPolicy@4")
	#pragma comment(linker, "/EXPORT:HP_Server_GetState=_HP_Server_GetState@4")
	#pragma comment(linker, "/EXPORT:HP_Server_GetWorkerThreadCount=_HP_Server_GetWorkerThreadCount@4")
	#pragma comment(linker, "/EXPORT:HP_Server_HasStarted=_HP_Server_HasStarted@4")
	#pragma comment(linker, "/EXPORT:HP_Server_Send=_HP_Server_Send@16")
	#pragma comment(linker, "/EXPORT:HP_Server_SendPackets=_HP_Server_SendPackets@16")
	#pragma comment(linker, "/EXPORT:HP_Server_SendPart=_HP_Server_SendPart@20")
	#pragma comment(linker, "/EXPORT:HP_Server_SetConnectionExtra=_HP_Server_SetConnectionExtra@12")
	#pragma comment(linker, "/EXPORT:HP_Server_SetFreeBufferObjHold=_HP_Server_SetFreeBufferObjHold@8")
	#pragma comment(linker, "/EXPORT:HP_Server_SetFreeBufferObjPool=_HP_Server_SetFreeBufferObjPool@8")
	#pragma comment(linker, "/EXPORT:HP_Server_SetFreeSocketObjHold=_HP_Server_SetFreeSocketObjHold@8")
	#pragma comment(linker, "/EXPORT:HP_Server_SetFreeSocketObjLockTime=_HP_Server_SetFreeSocketObjLockTime@8")
	#pragma comment(linker, "/EXPORT:HP_Server_SetFreeSocketObjPool=_HP_Server_SetFreeSocketObjPool@8")
	#pragma comment(linker, "/EXPORT:HP_Server_SetMaxShutdownWaitTime=_HP_Server_SetMaxShutdownWaitTime@8")
	#pragma comment(linker, "/EXPORT:HP_Server_SetMarkSilence=_HP_Server_SetMarkSilence@8")
	#pragma comment(linker, "/EXPORT:HP_Server_SetRecvPolicy=_HP_Server_SetRecvPolicy@8")
	#pragma comment(linker, "/EXPORT:HP_Server_SetSendPolicy=_HP_Server_SetSendPolicy@8")
	#pragma comment(linker, "/EXPORT:HP_Server_SetWorkerThreadCount=_HP_Server_SetWorkerThreadCount@8")
	#pragma comment(linker, "/EXPORT:HP_Server_Start=_HP_Server_Start@12")
	#pragma comment(linker, "/EXPORT:HP_Server_Stop=_HP_Server_Stop@4")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Agent_OnClose=_HP_Set_FN_Agent_OnClose@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Agent_OnConnect=_HP_Set_FN_Agent_OnConnect@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Agent_OnError=_HP_Set_FN_Agent_OnError@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Agent_OnPrepareConnect=_HP_Set_FN_Agent_OnPrepareConnect@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Agent_OnPullReceive=_HP_Set_FN_Agent_OnPullReceive@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Agent_OnReceive=_HP_Set_FN_Agent_OnReceive@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Agent_OnSend=_HP_Set_FN_Agent_OnSend@8")
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Agent_OnShutdown=_HP_Set_FN_Agent_OnShutdown@8")
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
	#pragma comment(linker, "/EXPORT:HP_Set_FN_Server_OnShutdown=_HP_Set_FN_Server_OnShutdown@8")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_GetKeepAliveInterval=_HP_TcpAgent_GetKeepAliveInterval@4")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_GetKeepAliveTime=_HP_TcpAgent_GetKeepAliveTime@4")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_GetSocketBufferSize=_HP_TcpAgent_GetSocketBufferSize@4")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_IsReuseAddress=_HP_TcpAgent_IsReuseAddress@4")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_SendSmallFile=_HP_TcpAgent_SendSmallFile@20")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_SetKeepAliveInterval=_HP_TcpAgent_SetKeepAliveInterval@8")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_SetKeepAliveTime=_HP_TcpAgent_SetKeepAliveTime@8")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_SetReuseAddress=_HP_TcpAgent_SetReuseAddress@8")
	#pragma comment(linker, "/EXPORT:HP_TcpAgent_SetSocketBufferSize=_HP_TcpAgent_SetSocketBufferSize@8")
	#pragma comment(linker, "/EXPORT:HP_TcpClient_GetKeepAliveInterval=_HP_TcpClient_GetKeepAliveInterval@4")
	#pragma comment(linker, "/EXPORT:HP_TcpClient_GetKeepAliveTime=_HP_TcpClient_GetKeepAliveTime@4")
	#pragma comment(linker, "/EXPORT:HP_TcpClient_GetSocketBufferSize=_HP_TcpClient_GetSocketBufferSize@4")
	#pragma comment(linker, "/EXPORT:HP_TcpClient_SendSmallFile=_HP_TcpClient_SendSmallFile@16")
	#pragma comment(linker, "/EXPORT:HP_TcpClient_SetKeepAliveInterval=_HP_TcpClient_SetKeepAliveInterval@8")
	#pragma comment(linker, "/EXPORT:HP_TcpClient_SetKeepAliveTime=_HP_TcpClient_SetKeepAliveTime@8")
	#pragma comment(linker, "/EXPORT:HP_TcpClient_SetSocketBufferSize=_HP_TcpClient_SetSocketBufferSize@8")
	#pragma comment(linker, "/EXPORT:HP_TcpPullAgent_Fetch=_HP_TcpPullAgent_Fetch@16")
	#pragma comment(linker, "/EXPORT:HP_TcpPullAgent_Peek=_HP_TcpPullAgent_Peek@16")
	#pragma comment(linker, "/EXPORT:HP_TcpPullClient_Fetch=_HP_TcpPullClient_Fetch@12")
	#pragma comment(linker, "/EXPORT:HP_TcpPullClient_Peek=_HP_TcpPullClient_Peek@12")
	#pragma comment(linker, "/EXPORT:HP_TcpPullServer_Fetch=_HP_TcpPullServer_Fetch@16")
	#pragma comment(linker, "/EXPORT:HP_TcpPullServer_Peek=_HP_TcpPullServer_Peek@16")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_GetAcceptSocketCount=_HP_TcpServer_GetAcceptSocketCount@4")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_GetKeepAliveInterval=_HP_TcpServer_GetKeepAliveInterval@4")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_GetKeepAliveTime=_HP_TcpServer_GetKeepAliveTime@4")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_GetSocketBufferSize=_HP_TcpServer_GetSocketBufferSize@4")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_GetSocketListenQueue=_HP_TcpServer_GetSocketListenQueue@4")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_SendSmallFile=_HP_TcpServer_SendSmallFile@20")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_SetAcceptSocketCount=_HP_TcpServer_SetAcceptSocketCount@8")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_SetKeepAliveInterval=_HP_TcpServer_SetKeepAliveInterval@8")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_SetKeepAliveTime=_HP_TcpServer_SetKeepAliveTime@8")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_SetSocketBufferSize=_HP_TcpServer_SetSocketBufferSize@8")
	#pragma comment(linker, "/EXPORT:HP_TcpServer_SetSocketListenQueue=_HP_TcpServer_SetSocketListenQueue@8")
	#pragma comment(linker, "/EXPORT:HP_UdpCast_GetBindAdddress=_HP_UdpCast_GetBindAdddress@4")
	#pragma comment(linker, "/EXPORT:HP_UdpCast_GetCastMode=_HP_UdpCast_GetCastMode@4")
	#pragma comment(linker, "/EXPORT:HP_UdpCast_GetMaxDatagramSize=_HP_UdpCast_GetMaxDatagramSize@4")
	#pragma comment(linker, "/EXPORT:HP_UdpCast_GetMultiCastTtl=_HP_UdpCast_GetMultiCastTtl@4")
	#pragma comment(linker, "/EXPORT:HP_UdpCast_GetRemoteAddress=_HP_UdpCast_GetRemoteAddress@16")
	#pragma comment(linker, "/EXPORT:HP_UdpCast_IsMultiCastLoop=_HP_UdpCast_IsMultiCastLoop@4")
	#pragma comment(linker, "/EXPORT:HP_UdpCast_IsReuseAddress=_HP_UdpCast_IsReuseAddress@4")
	#pragma comment(linker, "/EXPORT:HP_UdpCast_SetBindAdddress=_HP_UdpCast_SetBindAdddress@8")
	#pragma comment(linker, "/EXPORT:HP_UdpCast_SetCastMode=_HP_UdpCast_SetCastMode@8")
	#pragma comment(linker, "/EXPORT:HP_UdpCast_SetMaxDatagramSize=_HP_UdpCast_SetMaxDatagramSize@8")
	#pragma comment(linker, "/EXPORT:HP_UdpCast_SetMultiCastLoop=_HP_UdpCast_SetMultiCastLoop@8")
	#pragma comment(linker, "/EXPORT:HP_UdpCast_SetMultiCastTtl=_HP_UdpCast_SetMultiCastTtl@8")
	#pragma comment(linker, "/EXPORT:HP_UdpCast_SetReuseAddress=_HP_UdpCast_SetReuseAddress@8")
	#pragma comment(linker, "/EXPORT:HP_UdpClient_GetDetectAttempts=_HP_UdpClient_GetDetectAttempts@4")
	#pragma comment(linker, "/EXPORT:HP_UdpClient_GetDetectInterval=_HP_UdpClient_GetDetectInterval@4")
	#pragma comment(linker, "/EXPORT:HP_UdpClient_GetMaxDatagramSize=_HP_UdpClient_GetMaxDatagramSize@4")
	#pragma comment(linker, "/EXPORT:HP_UdpClient_SetDetectAttempts=_HP_UdpClient_SetDetectAttempts@8")
	#pragma comment(linker, "/EXPORT:HP_UdpClient_SetDetectInterval=_HP_UdpClient_SetDetectInterval@8")
	#pragma comment(linker, "/EXPORT:HP_UdpClient_SetMaxDatagramSize=_HP_UdpClient_SetMaxDatagramSize@8")
	#pragma comment(linker, "/EXPORT:HP_UdpServer_GetDetectAttempts=_HP_UdpServer_GetDetectAttempts@4")
	#pragma comment(linker, "/EXPORT:HP_UdpServer_GetDetectInterval=_HP_UdpServer_GetDetectInterval@4")
	#pragma comment(linker, "/EXPORT:HP_UdpServer_GetMaxDatagramSize=_HP_UdpServer_GetMaxDatagramSize@4")
	#pragma comment(linker, "/EXPORT:HP_UdpServer_GetPostReceiveCount=_HP_UdpServer_GetPostReceiveCount@4")
	#pragma comment(linker, "/EXPORT:HP_UdpServer_SetDetectAttempts=_HP_UdpServer_SetDetectAttempts@8")
	#pragma comment(linker, "/EXPORT:HP_UdpServer_SetDetectInterval=_HP_UdpServer_SetDetectInterval@8")
	#pragma comment(linker, "/EXPORT:HP_UdpServer_SetMaxDatagramSize=_HP_UdpServer_SetMaxDatagramSize@8")
	#pragma comment(linker, "/EXPORT:HP_UdpServer_SetPostReceiveCount=_HP_UdpServer_SetPostReceiveCount@8")
	#pragma comment(linker, "/EXPORT:SYS_GetLastError=_SYS_GetLastError@0")
	#pragma comment(linker, "/EXPORT:SYS_GetSocketOption=_SYS_GetSocketOption@20")
	#pragma comment(linker, "/EXPORT:SYS_IoctlSocket=_SYS_IoctlSocket@12")
	#pragma comment(linker, "/EXPORT:SYS_SetSocketOption=_SYS_SetSocketOption@20")
	#pragma comment(linker, "/EXPORT:SYS_WSAGetLastError=_SYS_WSAGetLastError@0")
	#pragma comment(linker, "/EXPORT:SYS_WSAIoctl=_SYS_WSAIoctl@28")
#endif

class C_HP_Object
{
public:

	inline static IServer* ToServer(HP_Server pServer)
	{
		return (IServer*)((char*)pServer + ((C_HP_Object*)pServer)->offset);
	}

	inline static IAgent* ToAgent(HP_Agent pAgent)
	{
		return (IAgent*)((char*)pAgent + ((C_HP_Object*)pAgent)->offset);
	}

	inline static IClient* ToClient(HP_Client pClient)
	{
		return (IClient*)((char*)pClient + ((C_HP_Object*)pClient)->offset);
	}

	inline static IPullSocket* ToPullSocket(HP_PullSocket pPullSocket)
	{
		return (IPullSocket*)((char*)pPullSocket + sizeof(C_HP_Object));
	}

	inline static IPullClient* ToPullClient(HP_PullClient pPullClient)
	{
		return (IPullClient*)((char*)pPullClient + sizeof(C_HP_Object));
	}

	inline static HP_Server FromServer(IServer* pServer)
	{
		C_HP_Object* pResult = (C_HP_Object*)((char*)pServer - sizeof(C_HP_Object));

		if(pResult->offset != sizeof(C_HP_Object))
			pResult = (C_HP_Object*)((char*)pResult - sizeof(IPullSocket));

		return (HP_Object)pResult;
	}

	inline static HP_Agent FromAgent(IAgent* pAgent)
	{
		C_HP_Object* pResult = (C_HP_Object*)((char*)pAgent - sizeof(C_HP_Object));

		if(pResult->offset != sizeof(C_HP_Object))
			pResult = (C_HP_Object*)((char*)pResult - sizeof(IPullSocket));

		return (HP_Object)pResult;
	}

	inline static HP_Client FromClient(IClient* pClient)
	{
		C_HP_Object* pResult = (C_HP_Object*)((char*)pClient - sizeof(C_HP_Object));

		if(pResult->offset != sizeof(C_HP_Object))
			pResult = (C_HP_Object*)((char*)pResult - sizeof(IPullClient));

		return (HP_Object)pResult;
	}

	inline static HP_PullSocket FromPullSocket(IPullSocket* pPullSocket)
	{
		return (HP_PullSocket)((char*)pPullSocket - sizeof(IPullSocket));
	}

	inline static HP_PullClient FromPullClient(IPullClient* pPullClient)
	{
		return (HP_PullClient)((char*)pPullClient - sizeof(IPullClient));
	}

public:

	inline static ITcpServer* ToTcpServer(HP_TcpServer pServer)
	{
		return (ITcpServer*)ToServer(pServer);
	}

	inline static IUdpServer* ToUdpServer(HP_UdpServer pServer)
	{
		return (IUdpServer*)ToServer(pServer);
	}

	inline static ITcpAgent* ToTcpAgent(HP_TcpAgent pAgent)
	{
		return (ITcpAgent*)ToAgent(pAgent);
	}

	inline static ITcpClient* ToTcpClient(HP_TcpClient pClient)
	{
		return (ITcpClient*)ToClient(pClient);
	}

	inline static IUdpClient* ToUdpClient(HP_UdpClient pClient)
	{
		return (IUdpClient*)ToClient(pClient);
	}

	inline static IUdpCast* ToUdpCast(HP_UdpCast pCast)
	{
		return (IUdpCast*)ToClient(pCast);
	}

protected:

	C_HP_Object(int k = 0) : offset(k + sizeof(C_HP_Object))	{}

	virtual ~C_HP_Object()										{}

private:

	size_t offset;
};

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

	virtual EnHandleResult OnShutdown()
	{
		return	(m_fnOnShutdown)
			? (EnHandleResult)m_fnOnShutdown()
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
		, m_fnOnShutdown		(nullptr)
	{
	}

public:
	HP_FN_Server_OnPrepareListen	m_fnOnPrepareListen	;
	HP_FN_Server_OnAccept			m_fnOnAccept		;
	HP_FN_Server_OnSend				m_fnOnSend			;
	HP_FN_Server_OnReceive			m_fnOnReceive		;
	HP_FN_Server_OnPullReceive		m_fnOnPullReceive	;
	HP_FN_Server_OnClose			m_fnOnClose			;
	HP_FN_Server_OnError			m_fnOnError			;
	HP_FN_Server_OnShutdown			m_fnOnShutdown		;
};

class C_HP_AgentListener : public IAgentListener
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

	virtual EnHandleResult OnShutdown()
	{
		return	(m_fnOnShutdown)
			? (EnHandleResult)m_fnOnShutdown()
			: HR_IGNORE;
	}

public:
	C_HP_AgentListener()
		: m_fnOnPrepareConnect	(nullptr)
		, m_fnOnConnect			(nullptr)
		, m_fnOnSend			(nullptr)
		, m_fnOnReceive			(nullptr)
		, m_fnOnPullReceive		(nullptr)
		, m_fnOnClose			(nullptr)
		, m_fnOnError			(nullptr)
		, m_fnOnShutdown		(nullptr)
	{
	}

public:
	HP_FN_Agent_OnPrepareConnect	m_fnOnPrepareConnect;
	HP_FN_Agent_OnConnect			m_fnOnConnect		;
	HP_FN_Agent_OnSend				m_fnOnSend			;
	HP_FN_Agent_OnReceive			m_fnOnReceive		;
	HP_FN_Agent_OnPullReceive		m_fnOnPullReceive	;
	HP_FN_Agent_OnClose				m_fnOnClose			;
	HP_FN_Agent_OnError				m_fnOnError			;
	HP_FN_Agent_OnShutdown			m_fnOnShutdown		;
};

class C_HP_ClientListener : public IClientListener
{
public:
	virtual EnHandleResult OnPrepareConnect(IClient* pClient, UINT_PTR socket)
	{
		return	(m_fnOnPrepareConnect)
			? (EnHandleResult)m_fnOnPrepareConnect(C_HP_Object::FromClient(pClient), socket)
			: HR_IGNORE;
	}

	virtual EnHandleResult OnConnect(IClient* pClient)
	{
		ASSERT(m_fnOnConnect);

		return	(m_fnOnConnect)
			? (EnHandleResult)m_fnOnConnect(C_HP_Object::FromClient(pClient))
			: HR_IGNORE;
	}

	virtual EnHandleResult OnSend(IClient* pClient, const BYTE* pData, int iLength)
	{
		return	(m_fnOnSend)
			? (EnHandleResult)m_fnOnSend(C_HP_Object::FromClient(pClient), pData, iLength)
			: HR_IGNORE;
	}

	virtual EnHandleResult OnReceive(IClient* pClient, const BYTE* pData, int iLength)
	{
		ASSERT(m_fnOnReceive);

		return	(m_fnOnReceive)
			? (EnHandleResult)m_fnOnReceive(C_HP_Object::FromClient(pClient), pData, iLength)
			: HR_IGNORE;
	}

	virtual EnHandleResult OnReceive(IClient* pClient, int iLength)
	{
		ASSERT(m_fnOnPullReceive);

		return	(m_fnOnPullReceive)
			? (EnHandleResult)m_fnOnPullReceive(C_HP_Object::FromClient(pClient), iLength)
			: HR_IGNORE;
	}

	virtual EnHandleResult OnClose(IClient* pClient)
	{
		ASSERT(m_fnOnClose);

		return	(m_fnOnClose)
			? (EnHandleResult)m_fnOnClose(C_HP_Object::FromClient(pClient))
			: HR_IGNORE;
	}

	virtual EnHandleResult OnError(IClient* pClient, EnSocketOperation enOperation, int iErrorCode)
	{
		ASSERT(m_fnOnError);

		return	(m_fnOnError)
			? (EnHandleResult)m_fnOnError(C_HP_Object::FromClient(pClient), (En_HP_SocketOperation)enOperation, iErrorCode)
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
	HP_FN_Client_OnPrepareConnect	m_fnOnPrepareConnect;
	HP_FN_Client_OnConnect			m_fnOnConnect		;
	HP_FN_Client_OnSend				m_fnOnSend			;
	HP_FN_Client_OnReceive			m_fnOnReceive		;
	HP_FN_Client_OnPullReceive		m_fnOnPullReceive	;
	HP_FN_Client_OnClose			m_fnOnClose			;
	HP_FN_Client_OnError			m_fnOnError			;
};

class C_HP_TcpServer : public C_HP_Object, public CTcpServer
{
public:
	C_HP_TcpServer(ITcpServerListener* pListener) : CTcpServer(pListener) {}
};

class C_HP_TcpPullServer : public C_HP_Object, public CTcpPullServer
{
public:
	C_HP_TcpPullServer(ITcpServerListener* pListener) : C_HP_Object(sizeof(IPullSocket)), CTcpPullServer(pListener) {}
};

class C_HP_UdpServer : public C_HP_Object, public CUdpServer
{
public:
	C_HP_UdpServer(IUdpServerListener* pListener) : CUdpServer(pListener) {}
};

class C_HP_TcpAgent : public C_HP_Object, public CTcpAgent
{
public:
	C_HP_TcpAgent(ITcpAgentListener* pListener) : CTcpAgent(pListener) {}
};

class C_HP_TcpPullAgent : public C_HP_Object, public CTcpPullAgent
{
public:
	C_HP_TcpPullAgent(ITcpAgentListener* pListener) : C_HP_Object(sizeof(IPullSocket)), CTcpPullAgent(pListener) {}
};

class C_HP_TcpClient : public C_HP_Object, public CTcpClient
{
public:
	C_HP_TcpClient(ITcpClientListener* pListener) : CTcpClient(pListener) {}
};

class C_HP_TcpPullClient : public C_HP_Object, public CTcpPullClient
{
public:
	C_HP_TcpPullClient(ITcpClientListener* pListener) : C_HP_Object(sizeof(IPullClient)), CTcpPullClient(pListener) {}
};

class C_HP_UdpClient : public C_HP_Object, public CUdpClient
{
public:
	C_HP_UdpClient(IUdpClientListener* pListener) : CUdpClient(pListener) {}
};

class C_HP_UdpCast : public C_HP_Object, public CUdpCast
{
public:
	C_HP_UdpCast(IUdpCastListener* pListener) : CUdpCast(pListener) {}
};

/****************************************************/
/************** HPSocket4C.dll 导出函数 **************/

HPSOCKET_API HP_TcpServer __stdcall Create_HP_TcpServer(HP_TcpServerListener pListener)
{
	return (HP_TcpServer)(new C_HP_TcpServer((ITcpServerListener*)pListener));
}

HPSOCKET_API HP_TcpAgent __stdcall Create_HP_TcpAgent(HP_TcpAgentListener pListener)
{
	return (HP_TcpAgent)(new C_HP_TcpAgent((ITcpAgentListener*)pListener));
}

HPSOCKET_API HP_TcpClient __stdcall Create_HP_TcpClient(HP_TcpClientListener pListener)
{
	return (HP_TcpClient)(new C_HP_TcpClient((ITcpClientListener*)pListener));
}

HPSOCKET_API HP_TcpPullServer __stdcall Create_HP_TcpPullServer(HP_TcpPullServerListener pListener)
{
	return (HP_TcpPullServer)(new C_HP_TcpPullServer((ITcpServerListener*)pListener));
}

HPSOCKET_API HP_TcpPullAgent __stdcall Create_HP_TcpPullAgent(HP_TcpPullAgentListener pListener)
{
	return (HP_TcpPullAgent)(new C_HP_TcpPullAgent((ITcpAgentListener*)pListener));
}

HPSOCKET_API HP_TcpPullClient __stdcall Create_HP_TcpPullClient(HP_TcpPullClientListener pListener)
{
	return (HP_TcpPullClient)(new C_HP_TcpPullClient((ITcpClientListener*)pListener));
}

HPSOCKET_API HP_UdpServer __stdcall Create_HP_UdpServer(HP_UdpServerListener pListener)
{
	return (HP_UdpServer)(new C_HP_UdpServer((IUdpServerListener*)pListener));
}

HPSOCKET_API HP_UdpClient __stdcall Create_HP_UdpClient(HP_UdpClientListener pListener)
{
	return (HP_UdpClient)(new C_HP_UdpClient((IUdpClientListener*)pListener));
}

HPSOCKET_API HP_UdpCast __stdcall Create_HP_UdpCast(HP_UdpCastListener pListener)
{
	return (HP_UdpCast)(new C_HP_UdpCast((IUdpCastListener*)pListener));
}

HPSOCKET_API void __stdcall Destroy_HP_TcpServer(HP_TcpServer pServer)
{
	delete (C_HP_TcpServer*)pServer;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpAgent(HP_TcpAgent pAgent)
{
	delete (C_HP_TcpAgent*)pAgent;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpClient(HP_TcpClient pClient)
{
	delete (C_HP_TcpClient*)pClient;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpPullServer(HP_TcpPullServer pServer)
{
	delete (C_HP_TcpPullServer*)pServer;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpPullAgent(HP_TcpPullAgent pAgent)
{
	delete (C_HP_TcpPullAgent*)pAgent;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpPullClient(HP_TcpPullClient pClient)
{
	delete (C_HP_TcpPullClient*)pClient;
}

HPSOCKET_API void __stdcall Destroy_HP_UdpServer(HP_UdpServer pServer)
{
	delete (C_HP_UdpServer*)pServer;
}

HPSOCKET_API void __stdcall Destroy_HP_UdpClient(HP_UdpClient pClient)
{
	delete (C_HP_UdpClient*)pClient;
}

HPSOCKET_API void __stdcall Destroy_HP_UdpCast(HP_UdpCast pCast)
{
	delete (C_HP_UdpCast*)pCast;
}

HPSOCKET_API HP_TcpServerListener __stdcall Create_HP_TcpServerListener()
{
	return (HP_TcpServerListener)(new C_HP_ServerListener);
}

HPSOCKET_API HP_TcpAgentListener __stdcall Create_HP_TcpAgentListener()
{
	return (HP_TcpAgentListener)(new C_HP_AgentListener);
}

HPSOCKET_API HP_TcpClientListener __stdcall Create_HP_TcpClientListener()
{
	return (HP_TcpClientListener)(new C_HP_ClientListener);
}

HPSOCKET_API HP_TcpPullServerListener __stdcall Create_HP_TcpPullServerListener()
{
	return (HP_TcpPullServerListener)(new C_HP_ServerListener);
}

HPSOCKET_API HP_TcpPullAgentListener __stdcall Create_HP_TcpPullAgentListener()
{
	return (HP_TcpPullAgentListener)(new C_HP_AgentListener);
}

HPSOCKET_API HP_TcpPullClientListener __stdcall Create_HP_TcpPullClientListener()
{
	return (HP_TcpPullClientListener)(new C_HP_ClientListener);
}

HPSOCKET_API HP_UdpServerListener __stdcall Create_HP_UdpServerListener()
{
	return (HP_UdpServerListener)(new C_HP_ServerListener);
}

HPSOCKET_API HP_UdpClientListener __stdcall Create_HP_UdpClientListener()
{
	return (HP_UdpClientListener)(new C_HP_ClientListener);
}

HPSOCKET_API HP_UdpCastListener __stdcall Create_HP_UdpCastListener()
{
	return (HP_UdpCastListener)(new C_HP_ClientListener);
}

HPSOCKET_API void __stdcall Destroy_HP_TcpServerListener(HP_TcpServerListener pListener)
{
	delete (C_HP_ServerListener*)pListener;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpAgentListener(HP_TcpAgentListener pListener)
{
	delete (C_HP_AgentListener*)pListener;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpClientListener(HP_TcpClientListener pListener)
{
	delete (C_HP_ClientListener*)pListener;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpPullServerListener(HP_TcpPullServerListener pListener)
{
	delete (C_HP_ServerListener*)pListener;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpPullAgentListener(HP_TcpPullAgentListener pListener)
{
	delete (C_HP_AgentListener*)pListener;
}

HPSOCKET_API void __stdcall Destroy_HP_TcpPullClientListener(HP_TcpPullClientListener pListener)
{
	delete (C_HP_ClientListener*)pListener;
}

HPSOCKET_API void __stdcall Destroy_HP_UdpServerListener(HP_UdpServerListener pListener)
{
	delete (C_HP_ServerListener*)pListener;
}

HPSOCKET_API void __stdcall Destroy_HP_UdpClientListener(HP_UdpClientListener pListener)
{
	delete (C_HP_ClientListener*)pListener;
}

HPSOCKET_API void __stdcall Destroy_HP_UdpCastListener(HP_UdpCastListener pListener)
{
	delete (C_HP_ClientListener*)pListener;
}

/**********************************************************************************/
/***************************** Server 回调函数设置方法 *****************************/

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnPrepareListen(HP_ServerListener pListener, HP_FN_Server_OnPrepareListen fn)
{
	((C_HP_ServerListener*)pListener)->m_fnOnPrepareListen = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnAccept(HP_ServerListener pListener, HP_FN_Server_OnAccept fn)
{
	((C_HP_ServerListener*)pListener)->m_fnOnAccept = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnSend(HP_ServerListener pListener, HP_FN_Server_OnSend fn)
{
	((C_HP_ServerListener*)pListener)->m_fnOnSend = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnReceive(HP_ServerListener pListener, HP_FN_Server_OnReceive fn)
{
	((C_HP_ServerListener*)pListener)->m_fnOnReceive = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnPullReceive(HP_ServerListener pListener, HP_FN_Server_OnPullReceive fn)
{
	((C_HP_ServerListener*)pListener)->m_fnOnPullReceive = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnClose(HP_ServerListener pListener, HP_FN_Server_OnClose fn)
{
	((C_HP_ServerListener*)pListener)->m_fnOnClose = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnError(HP_ServerListener pListener, HP_FN_Server_OnError fn)
{
	((C_HP_ServerListener*)pListener)->m_fnOnError = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnShutdown(HP_ServerListener pListener, HP_FN_Server_OnShutdown fn)
{
	((C_HP_ServerListener*)pListener)->m_fnOnShutdown = fn;
}

/**********************************************************************************/
/***************************** Agent 回调函数设置方法 *****************************/

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnPrepareConnect(HP_AgentListener pListener, HP_FN_Agent_OnPrepareConnect fn)
{
	((C_HP_AgentListener*)pListener)->m_fnOnPrepareConnect = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnConnect(HP_AgentListener pListener, HP_FN_Agent_OnConnect fn)
{
	((C_HP_AgentListener*)pListener)->m_fnOnConnect = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnSend(HP_AgentListener pListener, HP_FN_Agent_OnSend fn)
{
	((C_HP_AgentListener*)pListener)->m_fnOnSend = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnReceive(HP_AgentListener pListener, HP_FN_Agent_OnReceive fn)
{
	((C_HP_AgentListener*)pListener)->m_fnOnReceive = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnPullReceive(HP_AgentListener pListener, HP_FN_Agent_OnPullReceive fn)
{
	((C_HP_AgentListener*)pListener)->m_fnOnPullReceive = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnClose(HP_AgentListener pListener, HP_FN_Agent_OnClose fn)
{
	((C_HP_AgentListener*)pListener)->m_fnOnClose = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnError(HP_AgentListener pListener, HP_FN_Agent_OnError fn)
{
	((C_HP_AgentListener*)pListener)->m_fnOnError = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Agent_OnShutdown(HP_AgentListener pListener, HP_FN_Agent_OnShutdown fn)
{
	((C_HP_AgentListener*)pListener)->m_fnOnShutdown = fn;
}

/**********************************************************************************/
/***************************** Client 回调函数设置方法 *****************************/

HPSOCKET_API void __stdcall HP_Set_FN_Client_OnPrepareConnect(HP_ClientListener pListener, HP_FN_Client_OnPrepareConnect fn)
{
	((C_HP_ClientListener*)pListener)->m_fnOnPrepareConnect = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Client_OnConnect(HP_ClientListener pListener, HP_FN_Client_OnConnect fn)
{
	((C_HP_ClientListener*)pListener)->m_fnOnConnect = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Client_OnSend(HP_ClientListener pListener, HP_FN_Client_OnSend fn)
{
	((C_HP_ClientListener*)pListener)->m_fnOnSend = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Client_OnReceive(HP_ClientListener pListener, HP_FN_Client_OnReceive fn)
{
	((C_HP_ClientListener*)pListener)->m_fnOnReceive = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Client_OnPullReceive(HP_ClientListener pListener, HP_FN_Client_OnPullReceive fn)
{
	((C_HP_ClientListener*)pListener)->m_fnOnPullReceive = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Client_OnClose(HP_ClientListener pListener, HP_FN_Client_OnClose fn)
{
	((C_HP_ClientListener*)pListener)->m_fnOnClose = fn;
}

HPSOCKET_API void __stdcall HP_Set_FN_Client_OnError(HP_ClientListener pListener, HP_FN_Client_OnError fn)
{
	((C_HP_ClientListener*)pListener)->m_fnOnError = fn;
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

HPSOCKET_API BOOL __stdcall HP_Server_SendPackets(HP_Server pServer, HP_CONNID dwConnID, const WSABUF pBuffers[], int iCount)
{
	return C_HP_Object::ToServer(pServer)->SendPackets(dwConnID, pBuffers, iCount);
}

HPSOCKET_API BOOL __stdcall HP_Server_Disconnect(HP_Server pServer, HP_CONNID dwConnID, BOOL bForce)
{
	return C_HP_Object::ToServer(pServer)->Disconnect(dwConnID, bForce);
}

HPSOCKET_API BOOL __stdcall HP_Server_DisconnectLongConnections(HP_Server pServer, DWORD dwPeriod, BOOL bForce)
{
	return C_HP_Object::ToServer(pServer)->DisconnectLongConnections(dwPeriod, bForce);
}

HPSOCKET_API BOOL __stdcall HP_Server_DisconnectSilenceConnections(HP_Server pServer, DWORD dwPeriod, BOOL bForce)
{
	return C_HP_Object::ToServer(pServer)->DisconnectSilenceConnections(dwPeriod, bForce);
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

HPSOCKET_API BOOL __stdcall HP_Server_GetAllConnectionIDs(HP_Server pServer, HP_CONNID pIDs[], DWORD* pdwCount)
{
	return C_HP_Object::ToServer(pServer)->GetAllConnectionIDs(pIDs, *pdwCount);
}

HPSOCKET_API BOOL __stdcall HP_Server_GetConnectPeriod(HP_Server pServer, HP_CONNID dwConnID, DWORD* pdwPeriod)
{
	return C_HP_Object::ToServer(pServer)->GetConnectPeriod(dwConnID, *pdwPeriod);
}

HPSOCKET_API BOOL __stdcall HP_Server_GetSilencePeriod(HP_Server pServer, HP_CONNID dwConnID, DWORD* pdwPeriod)
{
	return C_HP_Object::ToServer(pServer)->GetSilencePeriod(dwConnID, *pdwPeriod);
}

HPSOCKET_API BOOL __stdcall HP_Server_GetListenAddress(HP_Server pServer, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToServer(pServer)->GetListenAddress(lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API BOOL __stdcall HP_Server_GetRemoteAddress(HP_Server pServer, HP_CONNID dwConnID, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToServer(pServer)->GetRemoteAddress(dwConnID, lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API void __stdcall HP_Server_SetSendPolicy(HP_Server pServer, En_HP_SendPolicy enSendPolicy)
{
	C_HP_Object::ToServer(pServer)->SetSendPolicy((EnSendPolicy)enSendPolicy);
}

HPSOCKET_API void __stdcall HP_Server_SetRecvPolicy(HP_Server pServer, En_HP_RecvPolicy enRecvPolicy)
{
	C_HP_Object::ToServer(pServer)->SetRecvPolicy((EnRecvPolicy)enRecvPolicy);
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

HPSOCKET_API void __stdcall HP_Server_SetMarkSilence(HP_Server pServer, BOOL bMarkSilence)
{
	C_HP_Object::ToServer(pServer)->SetMarkSilence(bMarkSilence);
}

HPSOCKET_API En_HP_SendPolicy __stdcall HP_Server_GetSendPolicy(HP_Server pServer)
{
	return (En_HP_SendPolicy)C_HP_Object::ToServer(pServer)->GetSendPolicy();
}

HPSOCKET_API En_HP_RecvPolicy __stdcall HP_Server_GetRecvPolicy(HP_Server pServer)
{
	return (En_HP_RecvPolicy)C_HP_Object::ToServer(pServer)->GetRecvPolicy();
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

HPSOCKET_API BOOL __stdcall HP_Server_IsMarkSilence(HP_Server pServer)
{
	return C_HP_Object::ToServer(pServer)->IsMarkSilence();
}

/**********************************************************************************/
/******************************* TCP Server 操作方法 *******************************/

HPSOCKET_API BOOL __stdcall HP_TcpServer_SendSmallFile(HP_Server pServer, HP_CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail)
{
	return C_HP_Object::ToTcpServer(pServer)->SendSmallFile(dwConnID, lpszFileName, pHead, pTail);
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

HPSOCKET_API void __stdcall HP_UdpServer_SetPostReceiveCount(HP_UdpServer pServer, DWORD dwPostReceiveCount)
{
	C_HP_Object::ToUdpServer(pServer)->SetPostReceiveCount(dwPostReceiveCount);
}

HPSOCKET_API DWORD __stdcall HP_UdpServer_GetPostReceiveCount(HP_UdpServer pServer)
{
	return C_HP_Object::ToUdpServer(pServer)->GetPostReceiveCount();
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

HPSOCKET_API BOOL __stdcall HP_Agent_SendPackets(HP_Agent pAgent, HP_CONNID dwConnID, const WSABUF pBuffers[], int iCount)
{
	return C_HP_Object::ToAgent(pAgent)->SendPackets(dwConnID, pBuffers, iCount);
}

HPSOCKET_API BOOL __stdcall HP_Agent_Disconnect(HP_Agent pAgent, HP_CONNID dwConnID, BOOL bForce)
{
	return C_HP_Object::ToAgent(pAgent)->Disconnect(dwConnID, bForce);
}

HPSOCKET_API BOOL __stdcall HP_Agent_DisconnectLongConnections(HP_Agent pAgent, DWORD dwPeriod, BOOL bForce)
{
	return C_HP_Object::ToAgent(pAgent)->DisconnectLongConnections(dwPeriod, bForce);
}

HPSOCKET_API BOOL __stdcall HP_Agent_DisconnectSilenceConnections(HP_Agent pAgent, DWORD dwPeriod, BOOL bForce)
{
	return C_HP_Object::ToAgent(pAgent)->DisconnectSilenceConnections(dwPeriod, bForce);
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

HPSOCKET_API BOOL __stdcall HP_Agent_GetAllConnectionIDs(HP_Agent pAgent, HP_CONNID pIDs[], DWORD* pdwCount)
{
	return C_HP_Object::ToAgent(pAgent)->GetAllConnectionIDs(pIDs, *pdwCount);
}

HPSOCKET_API BOOL __stdcall HP_Agent_GetConnectPeriod(HP_Agent pAgent, HP_CONNID dwConnID, DWORD* pdwPeriod)
{
	return C_HP_Object::ToAgent(pAgent)->GetConnectPeriod(dwConnID, *pdwPeriod);
}

HPSOCKET_API BOOL __stdcall HP_Agent_GetSilencePeriod(HP_Agent pAgent, HP_CONNID dwConnID, DWORD* pdwPeriod)
{
	return C_HP_Object::ToAgent(pAgent)->GetSilencePeriod(dwConnID, *pdwPeriod);
}

HPSOCKET_API BOOL __stdcall HP_Agent_GetLocalAddress(HP_Agent pAgent, HP_CONNID dwConnID, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToAgent(pAgent)->GetLocalAddress(dwConnID, lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API BOOL __stdcall HP_Agent_GetRemoteAddress(HP_Agent pAgent, HP_CONNID dwConnID, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToAgent(pAgent)->GetRemoteAddress(dwConnID, lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API void __stdcall HP_Agent_SetSendPolicy(HP_Agent pAgent, En_HP_SendPolicy enSendPolicy)
{
	C_HP_Object::ToAgent(pAgent)->SetSendPolicy((EnSendPolicy)enSendPolicy);
}

HPSOCKET_API void __stdcall HP_Agent_SetRecvPolicy(HP_Agent pAgent, En_HP_RecvPolicy enRecvPolicy)
{
	C_HP_Object::ToAgent(pAgent)->SetRecvPolicy((EnRecvPolicy)enRecvPolicy);
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

HPSOCKET_API void __stdcall HP_Agent_SetMarkSilence(HP_Agent pAgent, BOOL bMarkSilence)
{
	C_HP_Object::ToAgent(pAgent)->SetMarkSilence(bMarkSilence);
}

HPSOCKET_API En_HP_SendPolicy __stdcall HP_Agent_GetSendPolicy(HP_Agent pAgent)
{
	return (En_HP_SendPolicy)C_HP_Object::ToAgent(pAgent)->GetSendPolicy();
}

HPSOCKET_API En_HP_RecvPolicy __stdcall HP_Agent_GetRecvPolicy(HP_Agent pAgent)
{
	return (En_HP_RecvPolicy)C_HP_Object::ToAgent(pAgent)->GetRecvPolicy();
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

HPSOCKET_API BOOL __stdcall HP_Agent_IsMarkSilence(HP_Agent pAgent)
{
	return C_HP_Object::ToAgent(pAgent)->IsMarkSilence();
}

/**********************************************************************************/
/******************************* TCP Agent 操作方法 *******************************/

HPSOCKET_API BOOL __stdcall HP_TcpAgent_SendSmallFile(HP_Agent pAgent, HP_CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail)
{
	return C_HP_Object::ToTcpAgent(pAgent)->SendSmallFile(dwConnID, lpszFileName, pHead, pTail);
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

HPSOCKET_API BOOL __stdcall HP_Client_SendPackets(HP_Client pClient, const WSABUF pBuffers[], int iCount)
{
	return C_HP_Object::ToClient(pClient)->SendPackets(pBuffers, iCount);
}

/******************************************************************************/
/***************************** Client 属性访问方法 *****************************/

HPSOCKET_API void __stdcall HP_Client_SetExtra(HP_Client pClient, PVOID pExtra)
{
	C_HP_Object::ToClient(pClient)->SetExtra(pExtra);
}

HPSOCKET_API PVOID __stdcall HP_Client_GetExtra(HP_Client pClient)
{
	return C_HP_Object::ToClient(pClient)->GetExtra();
}

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

HPSOCKET_API BOOL __stdcall HP_Client_GetLocalAddress(HP_Client pClient, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
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
/******************************* TCP Client 操作方法 *******************************/

HPSOCKET_API BOOL __stdcall HP_TcpClient_SendSmallFile(HP_Client pClient, LPCTSTR lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail)
{
	return C_HP_Object::ToTcpClient(pClient)->SendSmallFile(lpszFileName, pHead, pTail);
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

/**********************************************************************************/
/****************************** UDP Cast 属性访问方法 ******************************/

HPSOCKET_API void __stdcall HP_UdpCast_SetMaxDatagramSize(HP_UdpCast pCast, DWORD dwMaxDatagramSize)
{
	C_HP_Object::ToUdpCast(pCast)->SetMaxDatagramSize(dwMaxDatagramSize);
}

HPSOCKET_API DWORD __stdcall HP_UdpCast_GetMaxDatagramSize(HP_UdpCast pCast)
{
	return C_HP_Object::ToUdpCast(pCast)->GetMaxDatagramSize();
}

HPSOCKET_API BOOL __stdcall HP_UdpCast_GetRemoteAddress(HP_UdpCast pCast, TCHAR lpszAddress[], int* piAddressLen, USHORT* pusPort)
{
	return C_HP_Object::ToUdpCast(pCast)->GetRemoteAddress(lpszAddress, *piAddressLen, *pusPort);
}

HPSOCKET_API void __stdcall HP_UdpCast_SetBindAdddress(HP_UdpCast pCast, LPCTSTR pszBindAddress)
{
	C_HP_Object::ToUdpCast(pCast)->SetBindAdddress(pszBindAddress);
}

HPSOCKET_API LPCTSTR __stdcall HP_UdpCast_GetBindAdddress(HP_UdpCast pCast)
{
	return C_HP_Object::ToUdpCast(pCast)->GetBindAdddress();
}

HPSOCKET_API void __stdcall HP_UdpCast_SetReuseAddress(HP_UdpCast pCast, BOOL bReuseAddress)
{
	C_HP_Object::ToUdpCast(pCast)->SetReuseAddress(bReuseAddress);
}

HPSOCKET_API BOOL __stdcall HP_UdpCast_IsReuseAddress(HP_UdpCast pCast)
{
	return C_HP_Object::ToUdpCast(pCast)->IsReuseAddress();
}

HPSOCKET_API void __stdcall HP_UdpCast_SetCastMode(HP_UdpCast pCast, En_HP_CastMode enCastMode)
{
	C_HP_Object::ToUdpCast(pCast)->SetCastMode((EnCastMode)enCastMode);
}

HPSOCKET_API En_HP_CastMode __stdcall HP_UdpCast_GetCastMode(HP_UdpCast pCast)
{
	return (En_HP_CastMode)C_HP_Object::ToUdpCast(pCast)->GetCastMode();
}

HPSOCKET_API void __stdcall HP_UdpCast_SetMultiCastTtl(HP_UdpCast pCast, int iMCTtl)
{
	C_HP_Object::ToUdpCast(pCast)->SetMultiCastTtl(iMCTtl);
}

HPSOCKET_API int __stdcall HP_UdpCast_GetMultiCastTtl(HP_UdpCast pCast)
{
	return C_HP_Object::ToUdpCast(pCast)->GetMultiCastTtl();
}

HPSOCKET_API void __stdcall HP_UdpCast_SetMultiCastLoop(HP_UdpCast pCast, BOOL bMCLoop)
{
	C_HP_Object::ToUdpCast(pCast)->SetMultiCastLoop(bMCLoop);
}

HPSOCKET_API BOOL __stdcall HP_UdpCast_IsMultiCastLoop(HP_UdpCast pCast)
{
	return C_HP_Object::ToUdpCast(pCast)->IsMultiCastLoop();
}

/***************************************************************************************/
/***************************** TCP Pull Server 组件操作方法 *****************************/

HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullServer_Fetch(HP_TcpPullServer pServer, HP_CONNID dwConnID, BYTE* pData, int iLength)
{
	return (En_HP_FetchResult)C_HP_Object::ToPullSocket(pServer)->Fetch(dwConnID, pData, iLength);
}

HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullServer_Peek(HP_TcpPullServer pServer, HP_CONNID dwConnID, BYTE* pData, int iLength)
{
	return (En_HP_FetchResult)C_HP_Object::ToPullSocket(pServer)->Peek(dwConnID, pData, iLength);
}

/***************************************************************************************/
/***************************** TCP Pull Server 属性访问方法 *****************************/

/***************************************************************************************/
/***************************** TCP Pull Agent 组件操作方法 *****************************/

HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullAgent_Fetch(HP_TcpPullAgent pAgent, HP_CONNID dwConnID, BYTE* pData, int iLength)
{
	return (En_HP_FetchResult)C_HP_Object::ToPullSocket(pAgent)->Fetch(dwConnID, pData, iLength);
}

HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullAgent_Peek(HP_TcpPullAgent pAgent, HP_CONNID dwConnID, BYTE* pData, int iLength)
{
	return (En_HP_FetchResult)C_HP_Object::ToPullSocket(pAgent)->Peek(dwConnID, pData, iLength);
}

/***************************************************************************************/
/***************************** TCP Pull Agent 属性访问方法 *****************************/

/***************************************************************************************/
/***************************** TCP Pull Client 组件操作方法 *****************************/

HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullClient_Fetch(HP_TcpPullClient pClient, BYTE* pData, int iLength)
{
	return (En_HP_FetchResult)C_HP_Object::ToPullClient(pClient)->Fetch(pData, iLength);
}

HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullClient_Peek(HP_TcpPullClient pClient, BYTE* pData, int iLength)
{
	return (En_HP_FetchResult)C_HP_Object::ToPullClient(pClient)->Peek(pData, iLength);
}

/***************************************************************************************/
/***************************** TCP Pull Client 属性访问方法 *****************************/

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

HPSOCKET_API int __stdcall SYS_WSAGetLastError()
{
	return ::WSAGetLastError();
}

HPSOCKET_API int __stdcall SYS_SetSocketOption(SOCKET sock, int level, int name, LPVOID val, int len)
{
	return ::SSO_SetSocketOption(sock, level, name, val, len);
}

HPSOCKET_API int __stdcall SYS_GetSocketOption(SOCKET sock, int level, int name, LPVOID val, int* len)
{
	return ::SSO_GetSocketOption(sock, level, name, val, len);
}

HPSOCKET_API int __stdcall SYS_IoctlSocket(SOCKET sock, long cmd, u_long* arg)
{
	return ::SSO_IoctlSocket(sock, cmd, arg);
}

HPSOCKET_API int __stdcall SYS_WSAIoctl(SOCKET sock, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer, LPDWORD lpcbBytesReturned)
{
	return ::SSO_WSAIoctl(sock, dwIoControlCode, lpvInBuffer, cbInBuffer, lpvOutBuffer, cbOutBuffer, lpcbBytesReturned);
}
