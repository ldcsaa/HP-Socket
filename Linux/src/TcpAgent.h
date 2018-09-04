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
 
#pragma once

#include "SocketHelper.h"
#include "./common/GeneralHelper.h"
#include "./common/IODispatcher.h"

class CTcpAgent : public ITcpAgent, private CIOHandler
{
public:
	virtual BOOL Start	(LPCTSTR lpszBindAddress = nullptr, BOOL bAsyncConnect = TRUE);
	virtual BOOL Stop	();
	virtual BOOL Connect(LPCTSTR lpszRemoteAddress, USHORT usPort, CONNID* pdwConnID = nullptr, PVOID pExtra = nullptr, USHORT usLocalPort = 0);
	virtual BOOL Send	(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset = 0);
	virtual BOOL SendSmallFile	(CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr);
	virtual BOOL SendPackets	(CONNID dwConnID, const WSABUF pBuffers[], int iCount)	{return DoSendPackets(dwConnID, pBuffers, iCount);}
	virtual BOOL PauseReceive	(CONNID dwConnID, BOOL bPause = TRUE);
	virtual BOOL			HasStarted					()	{return m_enState == SS_STARTED || m_enState == SS_STARTING;}
	virtual EnServiceState	GetState					()	{return m_enState;}
	virtual BOOL			Disconnect					(CONNID dwConnID, BOOL bForce = TRUE);
	virtual BOOL			DisconnectLongConnections	(DWORD dwPeriod, BOOL bForce = TRUE);
	virtual BOOL			DisconnectSilenceConnections(DWORD dwPeriod, BOOL bForce = TRUE);
	virtual BOOL			GetLocalAddress				(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);
	virtual BOOL			GetRemoteAddress			(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);
	virtual BOOL			GetRemoteHost				(CONNID dwConnID, TCHAR lpszHost[], int& iHostLen, USHORT& usPort);

	virtual BOOL IsConnected			(CONNID dwConnID);
	virtual BOOL IsPauseReceive			(CONNID dwConnID, BOOL& bPaused);
	virtual BOOL GetPendingDataLength	(CONNID dwConnID, int& iPending);
	virtual DWORD GetConnectionCount	();
	virtual BOOL GetAllConnectionIDs	(CONNID pIDs[], DWORD& dwCount);
	virtual BOOL GetConnectPeriod		(CONNID dwConnID, DWORD& dwPeriod);
	virtual BOOL GetSilencePeriod		(CONNID dwConnID, DWORD& dwPeriod);
	virtual EnSocketError GetLastError	()	{return m_enLastError;}
	virtual LPCTSTR GetLastErrorDesc	()	{return ::GetSocketErrorDesc(m_enLastError);}

private:
	virtual BOOL OnBeforeProcessIo(PVOID pv, UINT events)			override;
	virtual VOID OnAfterProcessIo(PVOID pv, UINT events, BOOL rs)	override;
	virtual VOID OnCommand(TDispCommand* pCmd)						override;
	virtual BOOL OnReadyRead(PVOID pv, UINT events)					override;
	virtual BOOL OnReadyWrite(PVOID pv, UINT events)				override;
	virtual BOOL OnHungUp(PVOID pv, UINT events)					override;
	virtual BOOL OnError(PVOID pv, UINT events)						override;
	virtual VOID OnDispatchThreadEnd(THR_ID tid)					override;

#ifdef _SSL_SUPPORT
	virtual BOOL SetupSSLContext	(int iVerifyMode = SSL_VM_NONE, LPCTSTR lpszPemCertFile = nullptr, LPCTSTR lpszPemKeyFile = nullptr, LPCTSTR lpszKeyPasswod = nullptr, LPCTSTR lpszCAPemCertFileOrPath = nullptr)	{return FALSE;}
	virtual void CleanupSSLContext	()																																													{}
#endif

public:
	virtual BOOL IsSecure				() {return FALSE;}

	virtual BOOL SetConnectionExtra(CONNID dwConnID, PVOID pExtra);
	virtual BOOL GetConnectionExtra(CONNID dwConnID, PVOID* ppExtra);

	virtual void SetSendPolicy				(EnSendPolicy enSendPolicy)		{m_enSendPolicy				= enSendPolicy;}
	virtual void SetMaxConnectionCount		(DWORD dwMaxConnectionCount)	{m_dwMaxConnectionCount		= dwMaxConnectionCount;}
	virtual void SetWorkerThreadCount		(DWORD dwWorkerThreadCount)		{m_dwWorkerThreadCount		= dwWorkerThreadCount;}
	virtual void SetSocketBufferSize		(DWORD dwSocketBufferSize)		{m_dwSocketBufferSize		= dwSocketBufferSize;}
	virtual void SetFreeSocketObjLockTime	(DWORD dwFreeSocketObjLockTime)	{m_dwFreeSocketObjLockTime	= dwFreeSocketObjLockTime;}
	virtual void SetFreeSocketObjPool		(DWORD dwFreeSocketObjPool)		{m_dwFreeSocketObjPool		= dwFreeSocketObjPool;}
	virtual void SetFreeBufferObjPool		(DWORD dwFreeBufferObjPool)		{m_dwFreeBufferObjPool		= dwFreeBufferObjPool;}
	virtual void SetFreeSocketObjHold		(DWORD dwFreeSocketObjHold)		{m_dwFreeSocketObjHold		= dwFreeSocketObjHold;}
	virtual void SetFreeBufferObjHold		(DWORD dwFreeBufferObjHold)		{m_dwFreeBufferObjHold		= dwFreeBufferObjHold;}
	virtual void SetKeepAliveTime			(DWORD dwKeepAliveTime)			{m_dwKeepAliveTime			= dwKeepAliveTime;}
	virtual void SetKeepAliveInterval		(DWORD dwKeepAliveInterval)		{m_dwKeepAliveInterval		= dwKeepAliveInterval;}
	virtual void SetReuseAddress			(BOOL bReuseAddress)			{m_bReuseAddress			= bReuseAddress;}
	virtual void SetMarkSilence				(BOOL bMarkSilence)				{m_bMarkSilence				= bMarkSilence;}

	virtual EnSendPolicy GetSendPolicy		()	{return m_enSendPolicy;}
	virtual DWORD GetMaxConnectionCount		()	{return m_dwMaxConnectionCount;}
	virtual DWORD GetWorkerThreadCount		()	{return m_dwWorkerThreadCount;}
	virtual DWORD GetSocketBufferSize		()	{return m_dwSocketBufferSize;}
	virtual DWORD GetFreeSocketObjLockTime	()	{return m_dwFreeSocketObjLockTime;}
	virtual DWORD GetFreeSocketObjPool		()	{return m_dwFreeSocketObjPool;}
	virtual DWORD GetFreeBufferObjPool		()	{return m_dwFreeBufferObjPool;}
	virtual DWORD GetFreeSocketObjHold		()	{return m_dwFreeSocketObjHold;}
	virtual DWORD GetFreeBufferObjHold		()	{return m_dwFreeBufferObjHold;}
	virtual DWORD GetKeepAliveTime			()	{return m_dwKeepAliveTime;}
	virtual DWORD GetKeepAliveInterval		()	{return m_dwKeepAliveInterval;}
	virtual BOOL  IsReuseAddress			()	{return m_bReuseAddress;}
	virtual BOOL  IsMarkSilence				()	{return m_bMarkSilence;}

protected:
	virtual EnHandleResult FirePrepareConnect(CONNID dwConnID, SOCKET socket)
		{return DoFirePrepareConnect(dwConnID, socket);}
	virtual EnHandleResult FireConnect(TAgentSocketObj* pSocketObj)
		{
			EnHandleResult rs		= DoFireConnect(pSocketObj);
			if(rs != HR_ERROR) rs	= FireHandShake(pSocketObj);
			return rs;
		}
	virtual EnHandleResult FireHandShake(TAgentSocketObj* pSocketObj)
		{return DoFireHandShake(pSocketObj);}
	virtual EnHandleResult FireReceive(TAgentSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return DoFireReceive(pSocketObj, pData, iLength);}
	virtual EnHandleResult FireReceive(TAgentSocketObj* pSocketObj, int iLength)
		{return DoFireReceive(pSocketObj, iLength);}
	virtual EnHandleResult FireSend(TAgentSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return DoFireSend(pSocketObj, pData, iLength);}
	virtual EnHandleResult FireClose(TAgentSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
		{return DoFireClose(pSocketObj, enOperation, iErrorCode);}
	virtual EnHandleResult FireShutdown()
		{return DoFireShutdown();}

	virtual EnHandleResult DoFirePrepareConnect(CONNID dwConnID, SOCKET socket)
		{return m_pListener->OnPrepareConnect(this, dwConnID, socket);}
	virtual EnHandleResult DoFireConnect(TAgentSocketObj* pSocketObj)
		{return m_pListener->OnConnect(this, pSocketObj->connID);}
	virtual EnHandleResult DoFireHandShake(TAgentSocketObj* pSocketObj)
		{return m_pListener->OnHandShake(this, pSocketObj->connID);}
	virtual EnHandleResult DoFireReceive(TAgentSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return m_pListener->OnReceive(this, pSocketObj->connID, pData, iLength);}
	virtual EnHandleResult DoFireReceive(TAgentSocketObj* pSocketObj, int iLength)
		{return m_pListener->OnReceive(this, pSocketObj->connID, iLength);}
	virtual EnHandleResult DoFireSend(TAgentSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return m_pListener->OnSend(this, pSocketObj->connID, pData, iLength);}
	virtual EnHandleResult DoFireClose(TAgentSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
		{return m_pListener->OnClose(this, pSocketObj->connID, enOperation, iErrorCode);}
	virtual EnHandleResult DoFireShutdown()
		{return m_pListener->OnShutdown(this);}

	void SetLastError(EnSocketError code, LPCSTR func, int ec);
	virtual BOOL CheckParams();
	virtual void PrepareStart();
	virtual void Reset();

	virtual void OnWorkerThreadEnd(THR_ID tid) {}

	BOOL DoSendPackets(CONNID dwConnID, const WSABUF pBuffers[], int iCount);
	BOOL DoSendPackets(TAgentSocketObj* pSocketObj, const WSABUF pBuffers[], int iCount);
	TAgentSocketObj* FindSocketObj(CONNID dwConnID);
	BOOL GetRemoteHost(CONNID dwConnID, LPCSTR* lpszHost, USHORT* pusPort = nullptr);

protected:
	BOOL SetConnectionExtra(TAgentSocketObj* pSocketObj, PVOID pExtra);
	BOOL GetConnectionExtra(TAgentSocketObj* pSocketObj, PVOID* ppExtra);
	BOOL SetConnectionReserved(CONNID dwConnID, PVOID pReserved);
	BOOL GetConnectionReserved(CONNID dwConnID, PVOID* ppReserved);
	BOOL SetConnectionReserved(TAgentSocketObj* pSocketObj, PVOID pReserved);
	BOOL GetConnectionReserved(TAgentSocketObj* pSocketObj, PVOID* ppReserved);
	BOOL SetConnectionReserved2(CONNID dwConnID, PVOID pReserved2);
	BOOL GetConnectionReserved2(CONNID dwConnID, PVOID* ppReserved2);
	BOOL SetConnectionReserved2(TAgentSocketObj* pSocketObj, PVOID pReserved2);
	BOOL GetConnectionReserved2(TAgentSocketObj* pSocketObj, PVOID* ppReserved2);

private:
	BOOL CheckStarting();
	BOOL CheckStoping();
	BOOL ParseBindAddress(LPCTSTR lpszBindAddress);
	BOOL CreateWorkerThreads();

	void DisconnectClientSocket();
	void WaitForClientSocketClose();
	void ReleaseClientSocket();
	void ReleaseFreeSocket();
	void WaitForWorkerThreadEnd();

	TAgentSocketObj* GetFreeSocketObj(CONNID dwConnID, SOCKET soClient);
	TAgentSocketObj* CreateSocketObj();
	void AddFreeSocketObj	(TAgentSocketObj* pSocketObj, EnSocketCloseFlag enFlag = SCF_NONE, EnSocketOperation enOperation = SO_UNKNOWN, int iErrorCode = 0);
	void DeleteSocketObj	(TAgentSocketObj* pSocketObj);
	BOOL InvalidSocketObj	(TAgentSocketObj* pSocketObj);
	void ReleaseGCSocketObj	(BOOL bForce = FALSE);
	void AddClientSocketObj	(CONNID dwConnID, TAgentSocketObj* pSocketObj, const HP_SOCKADDR& remoteAddr, LPCTSTR lpszRemoteAddress, PVOID pExtra);
	void CloseClientSocketObj(TAgentSocketObj* pSocketObj, EnSocketCloseFlag enFlag = SCF_NONE, EnSocketOperation enOperation = SO_UNKNOWN, int iErrorCode = 0, int iShutdownFlag = SHUT_WR);

private:
	int CreateClientSocket(LPCTSTR lpszRemoteAddress, USHORT usPort, USHORT usLocalPort, SOCKET& soClient, HP_SOCKADDR& addr);
	int PrepareConnect	(CONNID& dwConnID, SOCKET soClient);
	int ConnectToServer	(CONNID dwConnID, LPCTSTR lpszRemoteAddress, SOCKET soClient, const HP_SOCKADDR& addr, PVOID pExtra);

	VOID HandleCmdSend		(CONNID dwConnID);
	VOID HandleCmdUnpause	(CONNID dwConnID);
	VOID HandleCmdDisconnect(CONNID dwConnID, BOOL bForce);
	BOOL HandleConnect		(TAgentSocketObj* pSocketObj, UINT events);
	BOOL HandleReceive		(TAgentSocketObj* pSocketObj, int flag);
	BOOL HandleSend			(TAgentSocketObj* pSocketObj, int flag);
	BOOL HandleClose		(TAgentSocketObj* pSocketObj, EnSocketCloseFlag enFlag, UINT events);

	int SendInternal	(TAgentSocketObj* pSocketObj, const WSABUF pBuffers[], int iCount);
	BOOL SendItem		(TAgentSocketObj* pSocketObj, TItem* pItem);

public:
	CTcpAgent(ITcpAgentListener* pListener)
	: m_pListener				(pListener)
	, m_enLastError				(SE_OK)
	, m_enState					(SS_STOPPED)
	, m_bAsyncConnect			(TRUE)
	, m_enSendPolicy			(SP_PACK)
	, m_dwMaxConnectionCount	(DEFAULT_MAX_CONNECTION_COUNT)
	, m_dwWorkerThreadCount		(DEFAULT_WORKER_THREAD_COUNT)
	, m_dwSocketBufferSize		(DEFAULT_TCP_SOCKET_BUFFER_SIZE)
	, m_dwFreeSocketObjLockTime	(DEFAULT_FREE_SOCKETOBJ_LOCK_TIME)
	, m_dwFreeSocketObjPool		(DEFAULT_FREE_SOCKETOBJ_POOL)
	, m_dwFreeBufferObjPool		(DEFAULT_FREE_BUFFEROBJ_POOL)
	, m_dwFreeSocketObjHold		(DEFAULT_FREE_SOCKETOBJ_HOLD)
	, m_dwFreeBufferObjHold		(DEFAULT_FREE_BUFFEROBJ_HOLD)
	, m_dwKeepAliveTime			(DEFALUT_TCP_KEEPALIVE_TIME)
	, m_dwKeepAliveInterval		(DEFALUT_TCP_KEEPALIVE_INTERVAL)
	, m_bReuseAddress			(FALSE)
	, m_bMarkSilence			(TRUE)
	, m_soAddr					(AF_UNSPEC, TRUE)
	{
		ASSERT(m_pListener);
	}

	virtual ~CTcpAgent()
	{
		Stop();
	}

private:
	EnSendPolicy m_enSendPolicy;
	DWORD m_dwMaxConnectionCount;
	DWORD m_dwWorkerThreadCount;
	DWORD m_dwSocketBufferSize;
	DWORD m_dwFreeSocketObjLockTime;
	DWORD m_dwFreeSocketObjPool;
	DWORD m_dwFreeBufferObjPool;
	DWORD m_dwFreeSocketObjHold;
	DWORD m_dwFreeBufferObjHold;
	DWORD m_dwKeepAliveTime;
	DWORD m_dwKeepAliveInterval;
	BOOL  m_bReuseAddress;
	BOOL  m_bMarkSilence;

private:
	ITcpAgentListener*		m_pListener;
	BOOL					m_bAsyncConnect;
	EnServiceState			m_enState;
	EnSocketError			m_enLastError;
	HP_SOCKADDR				m_soAddr;

	CPrivateHeap			m_phSocket;
	CBufferObjPool			m_bfObjPool;

	CSpinGuard				m_csState;

	TAgentSocketObjPtrPool	m_bfActiveSockets;
	
	TAgentSocketObjPtrList	m_lsFreeSocket;
	TAgentSocketObjPtrQueue	m_lsGCSocket;
	TReceiveBufferMap		m_rcBufferMap;

	CIODispatcher			m_ioDispatcher;
};
