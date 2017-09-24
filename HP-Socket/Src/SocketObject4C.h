/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 5.0.2
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
 
#pragma once

#include "HPSocket4C.h"
#include "SocketInterface.h"

class C_HP_Object
{
public:
	template<class T> static inline HP_Object FromFirst(T* pFirst)
	{
		return (HP_Object)((char*)pFirst - first);
	}

	template<class T> static inline T* ToFirst(HP_Object pObject)
	{
		return (T*)((char*)pObject + first);
	}

	template<size_t offset, class T> static inline HP_Object FromSecond(T* pSecond)
	{
		return (C_HP_Object*)((char*)pSecond - first - offset);

		/*
		C_HP_Object* pObj = (C_HP_Object*)((char*)pSecond - first - offset);

		if(pObj->second != first)
		{
			pObj = (C_HP_Object*)((char*)pObj - sizeof(HP_Object));
			ASSERT(pObj->second == first + sizeof(HP_Object));
		}

		return (HP_Object)pObj;
		*/
	}

	template<class T> static inline T* ToSecond(HP_Object pObject)
	{
		return (T*)((char*)pObject + ((C_HP_Object*)pObject)->second);
	}

public:
	C_HP_Object(int offset = 0) : second(first + offset) {}
	virtual ~C_HP_Object() {}

private:
	static const size_t first = (sizeof(PVOID) + sizeof(size_t));
	size_t second;
};

template<class T, class L, int offset = 0> class C_HP_ObjectT : private C_HP_Object, public T
{
public:
	C_HP_ObjectT(L* pListener)	: C_HP_Object(offset), T(pListener)	{}
};

template<class T, class L, size_t offset = 0> class C_HP_ServerListenerT : public L
{
public:
	virtual EnHandleResult OnPrepareListen(T* pSender, SOCKET soListen)
	{
		return	(m_fnOnPrepareListen)
				? m_fnOnPrepareListen(C_HP_Object::FromSecond<offset>(pSender), soListen)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnAccept(T* pSender, CONNID dwConnID, UINT_PTR soClient)
	{
		return	(m_fnOnAccept)
				? m_fnOnAccept(C_HP_Object::FromSecond<offset>(pSender), dwConnID, soClient)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnHandShake(T* pSender, CONNID dwConnID)
	{
		return	(m_fnOnHandShake)
				? m_fnOnHandShake(C_HP_Object::FromSecond<offset>(pSender), dwConnID)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnSend(T* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
	{
		return	(m_fnOnSend)
				? m_fnOnSend(C_HP_Object::FromSecond<offset>(pSender), dwConnID, pData, iLength)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnReceive(T* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
	{
		ASSERT(m_fnOnReceive);

		return	(m_fnOnReceive)
				? m_fnOnReceive(C_HP_Object::FromSecond<offset>(pSender), dwConnID, pData, iLength)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnReceive(T* pSender, CONNID dwConnID, int iLength)
	{
		ASSERT(m_fnOnPullReceive);

		return	(m_fnOnPullReceive)
				? m_fnOnPullReceive(C_HP_Object::FromSecond<offset>(pSender), dwConnID, iLength)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnClose(T* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
	{
		ASSERT(m_fnOnClose);

		return	(m_fnOnClose)
				? m_fnOnClose(C_HP_Object::FromSecond<offset>(pSender), dwConnID, enOperation, iErrorCode)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnShutdown(T* pSender)
	{
		return	(m_fnOnShutdown)
				? m_fnOnShutdown(C_HP_Object::FromSecond<offset>(pSender))
				: HR_IGNORE;
	}

public:
	C_HP_ServerListenerT()
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

template<class T, class L, size_t offset = 0> class C_HP_AgentListenerT : public L
{
public:
	virtual EnHandleResult OnPrepareConnect(T* pSender, CONNID dwConnID, SOCKET socket)
	{
		return	(m_fnOnPrepareConnect)
				? m_fnOnPrepareConnect(C_HP_Object::FromSecond<offset>(pSender), dwConnID, socket)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnConnect(T* pSender, CONNID dwConnID)
	{
		return	(m_fnOnConnect)
				? m_fnOnConnect(C_HP_Object::FromSecond<offset>(pSender), dwConnID)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnHandShake(T* pSender, CONNID dwConnID)
	{
		return	(m_fnOnHandShake)
				? m_fnOnHandShake(C_HP_Object::FromSecond<offset>(pSender), dwConnID)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnSend(T* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
	{
		return	(m_fnOnSend)
				? m_fnOnSend(C_HP_Object::FromSecond<offset>(pSender), dwConnID, pData, iLength)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnReceive(T* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
	{
		ASSERT(m_fnOnReceive);

		return	(m_fnOnReceive)
				? m_fnOnReceive(C_HP_Object::FromSecond<offset>(pSender), dwConnID, pData, iLength)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnReceive(T* pSender, CONNID dwConnID, int iLength)
	{
		ASSERT(m_fnOnPullReceive);

		return	(m_fnOnPullReceive)
				? m_fnOnPullReceive(C_HP_Object::FromSecond<offset>(pSender), dwConnID, iLength)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnClose(T* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
	{
		ASSERT(m_fnOnClose);

		return	(m_fnOnClose)
				? m_fnOnClose(C_HP_Object::FromSecond<offset>(pSender), dwConnID, enOperation, iErrorCode)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnShutdown(T* pSender)
	{
		return	(m_fnOnShutdown)
				? m_fnOnShutdown(C_HP_Object::FromSecond<offset>(pSender))
				: HR_IGNORE;
	}

public:
	C_HP_AgentListenerT()
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

template<class T, class L, size_t offset = 0> class C_HP_ClientListenerT : public L
{
public:
	virtual EnHandleResult OnPrepareConnect(T* pSender, CONNID dwConnID, SOCKET socket)
	{
		return	(m_fnOnPrepareConnect)
				? m_fnOnPrepareConnect(C_HP_Object::FromSecond<offset>(pSender), dwConnID, socket)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnConnect(T* pSender, CONNID dwConnID)
	{
		return	(m_fnOnConnect)
				? m_fnOnConnect(C_HP_Object::FromSecond<offset>(pSender), dwConnID)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnHandShake(T* pSender, CONNID dwConnID)
	{
		return	(m_fnOnHandShake)
				? m_fnOnHandShake(C_HP_Object::FromSecond<offset>(pSender), dwConnID)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnSend(T* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
	{
		return	(m_fnOnSend)
				? m_fnOnSend(C_HP_Object::FromSecond<offset>(pSender), dwConnID, pData, iLength)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnReceive(T* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
	{
		ASSERT(m_fnOnReceive);

		return	(m_fnOnReceive)
				? m_fnOnReceive(C_HP_Object::FromSecond<offset>(pSender), dwConnID, pData, iLength)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnReceive(T* pSender, CONNID dwConnID, int iLength)
	{
		ASSERT(m_fnOnPullReceive);

		return	(m_fnOnPullReceive)
				? m_fnOnPullReceive(C_HP_Object::FromSecond<offset>(pSender), dwConnID, iLength)
				: HR_IGNORE;
	}

	virtual EnHandleResult OnClose(T* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
	{
		ASSERT(m_fnOnClose);

		return	(m_fnOnClose)
				? m_fnOnClose(C_HP_Object::FromSecond<offset>(pSender), dwConnID, enOperation, iErrorCode)
				: HR_IGNORE;
	}

public:
	C_HP_ClientListenerT()
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

typedef C_HP_ServerListenerT<ITcpServer, ITcpServerListener>						C_HP_TcpServerListener;
typedef C_HP_ServerListenerT<ITcpServer, ITcpServerListener, sizeof(IPullSocket)>	C_HP_TcpPullServerListener;
typedef C_HP_ServerListenerT<ITcpServer, ITcpServerListener, sizeof(IPackSocket)>	C_HP_TcpPackServerListener;

typedef C_HP_AgentListenerT<ITcpAgent, ITcpAgentListener>							C_HP_TcpAgentListener;
typedef C_HP_AgentListenerT<ITcpAgent, ITcpAgentListener, sizeof(IPullSocket)>		C_HP_TcpPullAgentListener;
typedef C_HP_AgentListenerT<ITcpAgent, ITcpAgentListener, sizeof(IPackSocket)>		C_HP_TcpPackAgentListener;

typedef C_HP_ClientListenerT<ITcpClient, ITcpClientListener>						C_HP_TcpClientListener;
typedef C_HP_ClientListenerT<ITcpClient, ITcpClientListener, sizeof(IPullClient)>	C_HP_TcpPullClientListener;
typedef C_HP_ClientListenerT<ITcpClient, ITcpClientListener, sizeof(IPackClient)>	C_HP_TcpPackClientListener;

typedef C_HP_ServerListenerT<IUdpServer, IUdpServerListener>						C_HP_UdpServerListener;
typedef C_HP_ClientListenerT<IUdpClient, IUdpClientListener>						C_HP_UdpClientListener;
typedef C_HP_ClientListenerT<IUdpCast, IUdpCastListener>							C_HP_UdpCastListener;

template<class T> class C_HP_HttpListenerT : public IHttpListenerT<T>
{
public:
	virtual EnHttpParseResult OnMessageBegin(T* pSender, CONNID dwConnID)
	{
		return	(m_fnOnMessageBegin)
				? m_fnOnMessageBegin(C_HP_Object::FromFirst(pSender), dwConnID)
				: HPR_OK;
	}

	virtual EnHttpParseResult OnRequestLine(T* pSender, CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)
	{
		return	(m_fnOnRequestLine)
				? m_fnOnRequestLine(C_HP_Object::FromFirst(pSender), dwConnID, lpszMethod, lpszUrl)
				: HPR_OK;
	}

	virtual EnHttpParseResult OnStatusLine(T* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)
	{
		return	(m_fnOnStatusLine)
				? m_fnOnStatusLine(C_HP_Object::FromFirst(pSender), dwConnID, usStatusCode, lpszDesc)
				: HPR_OK;
	}

	virtual EnHttpParseResult OnHeader(T* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
	{
		return	(m_fnOnHeader)
				? m_fnOnHeader(C_HP_Object::FromFirst(pSender), dwConnID, lpszName, lpszValue)
				: HPR_OK;
	}

	virtual EnHttpParseResult OnHeadersComplete(T* pSender, CONNID dwConnID)
	{
		ASSERT(m_fnOnHeadersComplete);

		return	(m_fnOnHeadersComplete)
				? m_fnOnHeadersComplete(C_HP_Object::FromFirst(pSender), dwConnID)
				: HPR_OK;
	}

	virtual EnHttpParseResult OnBody(T* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
	{
		ASSERT(m_fnOnBody);

		return	(m_fnOnBody)
				? m_fnOnBody(C_HP_Object::FromFirst(pSender), dwConnID, pData, iLength)
				: HPR_OK;
	}

	virtual EnHttpParseResult OnChunkHeader(T* pSender, CONNID dwConnID, int iLength)
	{
		return	(m_fnOnChunkHeader)
				? m_fnOnChunkHeader(C_HP_Object::FromFirst(pSender), dwConnID, iLength)
				: HPR_OK;
	}

	virtual EnHttpParseResult OnChunkComplete(T* pSender, CONNID dwConnID)
	{
		return	(m_fnOnChunkComplete)
				? m_fnOnChunkComplete(C_HP_Object::FromFirst(pSender), dwConnID)
				: HPR_OK;
	}

	virtual EnHttpParseResult OnMessageComplete(T* pSender, CONNID dwConnID)
	{
		ASSERT(m_fnOnMessageComplete);

		return	(m_fnOnMessageComplete)
				? m_fnOnMessageComplete(C_HP_Object::FromFirst(pSender), dwConnID)
				: HPR_OK;
	}

	virtual EnHttpParseResult OnUpgrade(T* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
	{
		return	(m_fnOnUpgrade)
				? m_fnOnUpgrade(C_HP_Object::FromFirst(pSender), dwConnID, enUpgradeType)
				: HPR_OK;
	}

	virtual EnHttpParseResult OnParseError(T* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
	{
		ASSERT(m_fnOnParseError);

		return	(m_fnOnParseError)
				? m_fnOnParseError(C_HP_Object::FromFirst(pSender), dwConnID, iErrorCode, lpszErrorDesc)
				: HPR_OK;
	}

	virtual EnHandleResult OnWSMessageHeader(T* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
	{
		return	(m_fnOnWSMessageHeader)
				? m_fnOnWSMessageHeader(C_HP_Object::FromFirst(pSender), dwConnID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen)
				: HR_OK;
	}

	virtual EnHandleResult OnWSMessageBody(T* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
	{
		return	(m_fnOnWSMessageBody)
				? m_fnOnWSMessageBody(C_HP_Object::FromFirst(pSender), dwConnID, pData, iLength)
				: HR_OK;
	}

	virtual EnHandleResult OnWSMessageComplete(T* pSender, CONNID dwConnID)
	{
		return	(m_fnOnWSMessageComplete)
				? m_fnOnWSMessageComplete(C_HP_Object::FromFirst(pSender), dwConnID)
				: HR_OK;
	}
public:
	C_HP_HttpListenerT()
	: m_fnOnMessageBegin	(nullptr)
	, m_fnOnRequestLine		(nullptr)
	, m_fnOnStatusLine		(nullptr)
	, m_fnOnHeader			(nullptr)
	, m_fnOnHeadersComplete	(nullptr)
	, m_fnOnBody			(nullptr)
	, m_fnOnChunkHeader		(nullptr)
	, m_fnOnChunkComplete	(nullptr)
	, m_fnOnMessageComplete	(nullptr)
	, m_fnOnUpgrade			(nullptr)
	, m_fnOnParseError		(nullptr)
	, m_fnOnWSMessageHeader	(nullptr)
	, m_fnOnWSMessageBody	(nullptr)
	, m_fnOnWSMessageComplete(nullptr)
	{
	}

public:
	HP_FN_Http_OnMessageBegin		m_fnOnMessageBegin		;
	HP_FN_Http_OnRequestLine		m_fnOnRequestLine		;
	HP_FN_Http_OnStatusLine			m_fnOnStatusLine		;
	HP_FN_Http_OnHeader				m_fnOnHeader			;
	HP_FN_Http_OnHeadersComplete	m_fnOnHeadersComplete	;
	HP_FN_Http_OnBody				m_fnOnBody				;
	HP_FN_Http_OnChunkHeader		m_fnOnChunkHeader		;
	HP_FN_Http_OnChunkComplete		m_fnOnChunkComplete		;
	HP_FN_Http_OnMessageComplete	m_fnOnMessageComplete	;
	HP_FN_Http_OnUpgrade			m_fnOnUpgrade			;
	HP_FN_Http_OnParseError			m_fnOnParseError		;
	HP_FN_Http_OnWSMessageHeader	m_fnOnWSMessageHeader	;
	HP_FN_Http_OnWSMessageBody		m_fnOnWSMessageBody		;
	HP_FN_Http_OnWSMessageComplete	m_fnOnWSMessageComplete	;
};

typedef C_HP_HttpListenerT<IHttpServer>		C_HP_HttpServerBaseListener1;
typedef C_HP_HttpListenerT<IHttpAgent>		C_HP_HttpAgentBaseListener1;
typedef C_HP_HttpListenerT<IHttpClient>		C_HP_HttpClientBaseListener1;

typedef C_HP_ServerListenerT<ITcpServer, ITcpServerListener, sizeof(IComplexHttpResponder)>		C_HP_HttpServerBaseListener2;
typedef C_HP_AgentListenerT<ITcpAgent, ITcpAgentListener, sizeof(IComplexHttpRequester)>		C_HP_HttpAgentBaseListener2;
typedef C_HP_ClientListenerT<ITcpClient, ITcpClientListener, sizeof(IHttpRequester)>			C_HP_HttpClientBaseListener2;

class C_HP_HttpServerListener : public IHttpServerListener
{
public:
	virtual EnHttpParseResult OnMessageBegin(IHttpServer* pSender, CONNID dwConnID)
		{return m_lsnHttp.OnMessageBegin(pSender, dwConnID);}
	virtual EnHttpParseResult OnRequestLine(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)
		{return m_lsnHttp.OnRequestLine(pSender, dwConnID, lpszMethod, lpszUrl);}
	virtual EnHttpParseResult OnStatusLine(IHttpServer* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)
		{return m_lsnHttp.OnStatusLine(pSender, dwConnID, usStatusCode, lpszDesc);}
	virtual EnHttpParseResult OnHeader(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
		{return m_lsnHttp.OnHeader(pSender, dwConnID, lpszName, lpszValue);}
	virtual EnHttpParseResult OnHeadersComplete(IHttpServer* pSender, CONNID dwConnID)
		{return m_lsnHttp.OnHeadersComplete(pSender, dwConnID);}
	virtual EnHttpParseResult OnBody(IHttpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_lsnHttp.OnBody(pSender, dwConnID, pData, iLength);}
	virtual EnHttpParseResult OnChunkHeader(IHttpServer* pSender, CONNID dwConnID, int iLength)
		{return m_lsnHttp.OnChunkHeader(pSender, dwConnID, iLength);}
	virtual EnHttpParseResult OnChunkComplete(IHttpServer* pSender, CONNID dwConnID)
		{return m_lsnHttp.OnChunkComplete(pSender, dwConnID);}
	virtual EnHttpParseResult OnMessageComplete(IHttpServer* pSender, CONNID dwConnID)
		{return m_lsnHttp.OnMessageComplete(pSender, dwConnID);}
	virtual EnHttpParseResult OnUpgrade(IHttpServer* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
		{return m_lsnHttp.OnUpgrade(pSender, dwConnID, enUpgradeType);}
	virtual EnHttpParseResult OnParseError(IHttpServer* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
		{return m_lsnHttp.OnParseError(pSender, dwConnID, iErrorCode, lpszErrorDesc);}

	virtual EnHandleResult OnWSMessageHeader(IHttpServer* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
		{return m_lsnHttp.OnWSMessageHeader(pSender, dwConnID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen);}
	virtual EnHandleResult OnWSMessageBody(IHttpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_lsnHttp.OnWSMessageBody(pSender, dwConnID, pData, iLength);}
	virtual EnHandleResult OnWSMessageComplete(IHttpServer* pSender, CONNID dwConnID)
		{return m_lsnHttp.OnWSMessageComplete(pSender, dwConnID);}

	virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, UINT_PTR soListen)
		{return m_lsnServer.OnPrepareListen(pSender, soListen);}
	virtual EnHandleResult OnAccept(ITcpServer* pSender, CONNID dwConnID, UINT_PTR soClient)
		{return m_lsnServer.OnAccept(pSender, dwConnID, soClient);}
	virtual EnHandleResult OnHandShake(ITcpServer* pSender, CONNID dwConnID)
		{return m_lsnServer.OnHandShake(pSender, dwConnID);}
	virtual EnHandleResult OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_lsnServer.OnSend(pSender, dwConnID, pData, iLength);}
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_lsnServer.OnReceive(pSender, dwConnID, pData, iLength);}
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength)
		{return m_lsnServer.OnReceive(pSender, dwConnID, iLength);}
	virtual EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
		{return m_lsnServer.OnClose(pSender, dwConnID, enOperation, iErrorCode);}
	virtual EnHandleResult OnShutdown(ITcpServer* pSender)
		{return m_lsnServer.OnShutdown(pSender);}

public:
	C_HP_HttpServerBaseListener1 m_lsnHttp;
	C_HP_HttpServerBaseListener2 m_lsnServer;
};

class C_HP_HttpAgentListener : public IHttpAgentListener
{
public:
	virtual EnHttpParseResult OnMessageBegin(IHttpAgent* pSender, CONNID dwConnID)
		{return m_lsnHttp.OnMessageBegin(pSender, dwConnID);}
	virtual EnHttpParseResult OnRequestLine(IHttpAgent* pSender, CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)
		{return m_lsnHttp.OnRequestLine(pSender, dwConnID, lpszMethod, lpszUrl);}
	virtual EnHttpParseResult OnStatusLine(IHttpAgent* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)
		{return m_lsnHttp.OnStatusLine(pSender, dwConnID, usStatusCode, lpszDesc);}
	virtual EnHttpParseResult OnHeader(IHttpAgent* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
		{return m_lsnHttp.OnHeader(pSender, dwConnID, lpszName, lpszValue);}
	virtual EnHttpParseResult OnHeadersComplete(IHttpAgent* pSender, CONNID dwConnID)
		{return m_lsnHttp.OnHeadersComplete(pSender, dwConnID);}
	virtual EnHttpParseResult OnBody(IHttpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_lsnHttp.OnBody(pSender, dwConnID, pData, iLength);}
	virtual EnHttpParseResult OnChunkHeader(IHttpAgent* pSender, CONNID dwConnID, int iLength)
		{return m_lsnHttp.OnChunkHeader(pSender, dwConnID, iLength);}
	virtual EnHttpParseResult OnChunkComplete(IHttpAgent* pSender, CONNID dwConnID)
		{return m_lsnHttp.OnChunkComplete(pSender, dwConnID);}
	virtual EnHttpParseResult OnMessageComplete(IHttpAgent* pSender, CONNID dwConnID)
		{return m_lsnHttp.OnMessageComplete(pSender, dwConnID);}
	virtual EnHttpParseResult OnUpgrade(IHttpAgent* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
		{return m_lsnHttp.OnUpgrade(pSender, dwConnID, enUpgradeType);}
	virtual EnHttpParseResult OnParseError(IHttpAgent* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
		{return m_lsnHttp.OnParseError(pSender, dwConnID, iErrorCode, lpszErrorDesc);}

	virtual EnHandleResult OnWSMessageHeader(IHttpAgent* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
		{return m_lsnHttp.OnWSMessageHeader(pSender, dwConnID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen);}
	virtual EnHandleResult OnWSMessageBody(IHttpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_lsnHttp.OnWSMessageBody(pSender, dwConnID, pData, iLength);}
	virtual EnHandleResult OnWSMessageComplete(IHttpAgent* pSender, CONNID dwConnID)
		{return m_lsnHttp.OnWSMessageComplete(pSender, dwConnID);}

	virtual EnHandleResult OnPrepareConnect(ITcpAgent* pSender, CONNID dwConnID, UINT_PTR socket)
		{return m_lsnAgent.OnPrepareConnect(pSender, dwConnID, socket);}
	virtual EnHandleResult OnConnect(ITcpAgent* pSender, CONNID dwConnID)
		{return m_lsnAgent.OnConnect(pSender, dwConnID);}
	virtual EnHandleResult OnHandShake(ITcpAgent* pSender, CONNID dwConnID)
		{return m_lsnAgent.OnHandShake(pSender, dwConnID);}
	virtual EnHandleResult OnSend(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_lsnAgent.OnSend(pSender, dwConnID, pData, iLength);}
	virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_lsnAgent.OnReceive(pSender, dwConnID, pData, iLength);}
	virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, int iLength)
		{return m_lsnAgent.OnReceive(pSender, dwConnID, iLength);}
	virtual EnHandleResult OnClose(ITcpAgent* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
		{return m_lsnAgent.OnClose(pSender, dwConnID, enOperation, iErrorCode);}
	virtual EnHandleResult OnShutdown(ITcpAgent* pSender)
		{return m_lsnAgent.OnShutdown(pSender);}

public:
	C_HP_HttpAgentBaseListener1 m_lsnHttp;
	C_HP_HttpAgentBaseListener2 m_lsnAgent;
};

class C_HP_HttpClientListener : public IHttpClientListener
{
public:
	virtual EnHttpParseResult OnMessageBegin(IHttpClient* pSender, CONNID dwConnID)
		{return m_lsnHttp.OnMessageBegin(pSender, dwConnID);}
	virtual EnHttpParseResult OnRequestLine(IHttpClient* pSender, CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)
		{return m_lsnHttp.OnRequestLine(pSender, dwConnID, lpszMethod, lpszUrl);}
	virtual EnHttpParseResult OnStatusLine(IHttpClient* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)
		{return m_lsnHttp.OnStatusLine(pSender, dwConnID, usStatusCode, lpszDesc);}
	virtual EnHttpParseResult OnHeader(IHttpClient* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
		{return m_lsnHttp.OnHeader(pSender, dwConnID, lpszName, lpszValue);}
	virtual EnHttpParseResult OnHeadersComplete(IHttpClient* pSender, CONNID dwConnID)
		{return m_lsnHttp.OnHeadersComplete(pSender, dwConnID);}
	virtual EnHttpParseResult OnBody(IHttpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_lsnHttp.OnBody(pSender, dwConnID, pData, iLength);}
	virtual EnHttpParseResult OnChunkHeader(IHttpClient* pSender, CONNID dwConnID, int iLength)
		{return m_lsnHttp.OnChunkHeader(pSender, dwConnID, iLength);}
	virtual EnHttpParseResult OnChunkComplete(IHttpClient* pSender, CONNID dwConnID)
		{return m_lsnHttp.OnChunkComplete(pSender, dwConnID);}
	virtual EnHttpParseResult OnMessageComplete(IHttpClient* pSender, CONNID dwConnID)
		{return m_lsnHttp.OnMessageComplete(pSender, dwConnID);}
	virtual EnHttpParseResult OnUpgrade(IHttpClient* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
		{return m_lsnHttp.OnUpgrade(pSender, dwConnID, enUpgradeType);}
	virtual EnHttpParseResult OnParseError(IHttpClient* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
		{return m_lsnHttp.OnParseError(pSender, dwConnID, iErrorCode, lpszErrorDesc);}

	virtual EnHandleResult OnWSMessageHeader(IHttpClient* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
		{return m_lsnHttp.OnWSMessageHeader(pSender, dwConnID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen);}
	virtual EnHandleResult OnWSMessageBody(IHttpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_lsnHttp.OnWSMessageBody(pSender, dwConnID, pData, iLength);}
	virtual EnHandleResult OnWSMessageComplete(IHttpClient* pSender, CONNID dwConnID)
		{return m_lsnHttp.OnWSMessageComplete(pSender, dwConnID);}

	virtual EnHandleResult OnPrepareConnect(ITcpClient* pSender, CONNID dwConnID, UINT_PTR socket)
		{return m_lsnClient.OnPrepareConnect(pSender, dwConnID, socket);}
	virtual EnHandleResult OnConnect(ITcpClient* pSender, CONNID dwConnID)
		{return m_lsnClient.OnConnect(pSender, dwConnID);}
	virtual EnHandleResult OnHandShake(ITcpClient* pSender, CONNID dwConnID)
		{return m_lsnClient.OnHandShake(pSender, dwConnID);}
	virtual EnHandleResult OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_lsnClient.OnSend(pSender, dwConnID, pData, iLength);}
	virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_lsnClient.OnReceive(pSender, dwConnID, pData, iLength);}
	virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, int iLength)
		{return m_lsnClient.OnReceive(pSender, dwConnID, iLength);}
	virtual EnHandleResult OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
		{return m_lsnClient.OnClose(pSender, dwConnID, enOperation, iErrorCode);}

public:
	C_HP_HttpClientBaseListener1 m_lsnHttp;
	C_HP_HttpClientBaseListener2 m_lsnClient;
};
