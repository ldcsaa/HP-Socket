/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.5.4
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
 
#pragma once

#include "HPSocket4C.h"
#include "SocketInterface.h"

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

	inline static IPackSocket* ToPackSocket(HP_PackSocket pPackSocket)
	{
		return (IPackSocket*)((char*)pPackSocket + sizeof(C_HP_Object));
	}

	inline static IPackClient* ToPackClient(HP_PackClient pPackClient)
	{
		return (IPackClient*)((char*)pPackClient + sizeof(C_HP_Object));
	}

	inline static HP_Server FromServer(IServer* pServer)
	{
		C_HP_Object* pResult = (C_HP_Object*)((char*)pServer - sizeof(C_HP_Object));

		if(pResult->offset != sizeof(C_HP_Object))
			pResult = (C_HP_Object*)((char*)pResult - sizeof(HP_Object));

		return (HP_Object)pResult;
	}

	inline static HP_Agent FromAgent(IAgent* pAgent)
	{
		C_HP_Object* pResult = (C_HP_Object*)((char*)pAgent - sizeof(C_HP_Object));

		if(pResult->offset != sizeof(C_HP_Object))
			pResult = (C_HP_Object*)((char*)pResult - sizeof(HP_Object));

		return (HP_Object)pResult;
	}

	inline static HP_Client FromClient(IClient* pClient)
	{
		C_HP_Object* pResult = (C_HP_Object*)((char*)pClient - sizeof(C_HP_Object));

		if(pResult->offset != sizeof(C_HP_Object))
			pResult = (C_HP_Object*)((char*)pResult - sizeof(HP_Object));

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

	inline static HP_PackSocket FromPackSocket(IPackSocket* pPackSocket)
	{
		return (HP_PackSocket)((char*)pPackSocket - sizeof(IPackSocket));
	}

	inline static HP_PackClient FromPackClient(IPackClient* pPackClient)
	{
		return (HP_PackClient)((char*)pPackClient - sizeof(IPackClient));
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

	virtual EnHandleResult OnHandShake(CONNID dwConnID)
	{
#ifdef _SSL_SUPPORT
		ASSERT(m_fnOnHandShake);
#endif

		return	(m_fnOnHandShake)
				? (EnHandleResult)m_fnOnHandShake(dwConnID)
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

	virtual EnHandleResult OnClose(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
	{
		ASSERT(m_fnOnClose);

		return	(m_fnOnClose)
				? (EnHandleResult)m_fnOnClose(dwConnID, (En_HP_SocketOperation)enOperation, iErrorCode)
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
		, m_fnOnHandShake		(nullptr)
		, m_fnOnSend			(nullptr)
		, m_fnOnReceive			(nullptr)
		, m_fnOnPullReceive		(nullptr)
		, m_fnOnClose			(nullptr)
		, m_fnOnShutdown		(nullptr)
	{
	}

public:
	HP_FN_Server_OnPrepareListen	m_fnOnPrepareListen	;
	HP_FN_Server_OnAccept			m_fnOnAccept		;
	HP_FN_Server_OnHandShake		m_fnOnHandShake		;
	HP_FN_Server_OnSend				m_fnOnSend			;
	HP_FN_Server_OnReceive			m_fnOnReceive		;
	HP_FN_Server_OnPullReceive		m_fnOnPullReceive	;
	HP_FN_Server_OnClose			m_fnOnClose			;
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

	virtual EnHandleResult OnHandShake(CONNID dwConnID)
	{
#ifdef _SSL_SUPPORT
		ASSERT(m_fnOnHandShake);
#endif

		return	(m_fnOnHandShake)
				? (EnHandleResult)m_fnOnHandShake(dwConnID)
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

	virtual EnHandleResult OnClose(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
	{
		ASSERT(m_fnOnClose);

		return	(m_fnOnClose)
				? (EnHandleResult)m_fnOnClose(dwConnID, (En_HP_SocketOperation)enOperation, iErrorCode)
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
		, m_fnOnHandShake		(nullptr)
		, m_fnOnSend			(nullptr)
		, m_fnOnReceive			(nullptr)
		, m_fnOnPullReceive		(nullptr)
		, m_fnOnClose			(nullptr)
		, m_fnOnShutdown		(nullptr)
	{
	}

public:
	HP_FN_Agent_OnPrepareConnect	m_fnOnPrepareConnect;
	HP_FN_Agent_OnConnect			m_fnOnConnect		;
	HP_FN_Agent_OnHandShake			m_fnOnHandShake		;
	HP_FN_Agent_OnSend				m_fnOnSend			;
	HP_FN_Agent_OnReceive			m_fnOnReceive		;
	HP_FN_Agent_OnPullReceive		m_fnOnPullReceive	;
	HP_FN_Agent_OnClose				m_fnOnClose			;
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

	virtual EnHandleResult OnHandShake(IClient* pClient)
	{
#ifdef _SSL_SUPPORT
		ASSERT(m_fnOnHandShake);
#endif

		return	(m_fnOnHandShake)
				? (EnHandleResult)m_fnOnHandShake(C_HP_Object::FromClient(pClient))
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

	virtual EnHandleResult OnClose(IClient* pClient, EnSocketOperation enOperation, int iErrorCode)
	{
		ASSERT(m_fnOnClose);

		return	(m_fnOnClose)
				? (EnHandleResult)m_fnOnClose(C_HP_Object::FromClient(pClient), (En_HP_SocketOperation)enOperation, iErrorCode)
				: HR_IGNORE;
	}

public:
	C_HP_ClientListener()
		: m_fnOnPrepareConnect	(nullptr)
		, m_fnOnConnect			(nullptr)
		, m_fnOnHandShake		(nullptr)
		, m_fnOnSend			(nullptr)
		, m_fnOnReceive			(nullptr)
		, m_fnOnPullReceive		(nullptr)
		, m_fnOnClose			(nullptr)
	{
	}

public:
	HP_FN_Client_OnPrepareConnect	m_fnOnPrepareConnect;
	HP_FN_Client_OnConnect			m_fnOnConnect		;
	HP_FN_Client_OnHandShake		m_fnOnHandShake		;
	HP_FN_Client_OnSend				m_fnOnSend			;
	HP_FN_Client_OnReceive			m_fnOnReceive		;
	HP_FN_Client_OnPullReceive		m_fnOnPullReceive	;
	HP_FN_Client_OnClose			m_fnOnClose			;
};
