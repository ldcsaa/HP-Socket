/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.4.2
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

#include "SocketHelper.h"
#include "../../Common/Src/Event.h"
#include "../../Common/Src/RWLock.h"
#include "../../Common/Src/STLHelper.h"
#include "../../Common/Src/RingBuffer.h"
#include "../../Common/Src/PrivateHeap.h"

class CTcpServer : public ITcpServer
{
public:
	CTcpServer(ITcpServerListener* psoListener)
	: m_psoListener				(psoListener)
	, m_hCompletePort			(nullptr)
	, m_soListen				(INVALID_SOCKET)
	, m_iRemainAcceptSockets	(0)
	, m_pfnAcceptEx				(nullptr)
	, m_pfnGetAcceptExSockaddrs	(nullptr)
	, m_pfnDisconnectEx			(nullptr)
	, m_enLastError				(SE_OK)
	, m_enState					(SS_STOPPED)
	, m_enSendPolicy			(SP_PACK)
	, m_dwWorkerThreadCount		(DEFAULT_WORKER_THREAD_COUNT)
	, m_dwSocketListenQueue		(DEFAULT_TCP_SERVER_SOCKET_LISTEN_QUEUE)
	, m_dwAcceptSocketCount		(DEFAULT_TCP_SERVER_ACCEPT_SOCKET_COUNT)
	, m_dwSocketBufferSize		(DEFAULT_TCP_SOCKET_BUFFER_SIZE)
	, m_dwFreeSocketObjLockTime	(DEFAULT_FREE_SOCKETOBJ_LOCK_TIME)
	, m_dwFreeSocketObjPool		(DEFAULT_FREE_SOCKETOBJ_POOL)
	, m_dwFreeBufferObjPool		(DEFAULT_FREE_BUFFEROBJ_POOL)
	, m_dwFreeSocketObjHold		(DEFAULT_FREE_SOCKETOBJ_HOLD)
	, m_dwFreeBufferObjHold		(DEFAULT_FREE_BUFFEROBJ_HOLD)
	, m_dwKeepAliveTime			(DEFALUT_TCP_KEEPALIVE_TIME)
	, m_dwKeepAliveInterval		(DEFALUT_TCP_KEEPALIVE_INTERVAL)
	, m_bMarkSilence			(FALSE)
	{
		ASSERT(m_wsSocket.IsValid());
		ASSERT(m_psoListener);

		Reset(FALSE);
	}

	virtual ~CTcpServer()
	{
		if(HasStarted())
			Stop();
	}

public:
	virtual BOOL Start	(LPCTSTR pszBindAddress, USHORT usPort);
	virtual BOOL Stop	();
	virtual BOOL Send	(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset = 0);
	virtual BOOL SendPackets	(CONNID dwConnID, const WSABUF pBuffers[], int iCount);
	virtual BOOL SendSmallFile	(CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr);
	virtual BOOL			HasStarted					()	{return m_enState == SS_STARTED || m_enState == SS_STARTING;}
	virtual EnServiceState	GetState					()	{return m_enState;}
	virtual BOOL			Disconnect					(CONNID dwConnID, BOOL bForce = TRUE);
	virtual BOOL			DisconnectLongConnections	(DWORD dwPeriod, BOOL bForce = TRUE);
	virtual BOOL			DisconnectSilenceConnections(DWORD dwPeriod, BOOL bForce = TRUE);
	virtual BOOL			GetListenAddress			(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);
	virtual BOOL			GetRemoteAddress			(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);
	
	virtual BOOL GetPendingDataLength	(CONNID dwConnID, int& iPending);
	virtual DWORD GetConnectionCount	();
	virtual BOOL GetAllConnectionIDs	(CONNID pIDs[], DWORD& dwCount);
	virtual BOOL GetConnectPeriod		(CONNID dwConnID, DWORD& dwPeriod);
	virtual BOOL GetSilencePeriod		(CONNID dwConnID, DWORD& dwPeriod);
	virtual EnSocketError GetLastError	()	{return m_enLastError;}
	virtual LPCTSTR	GetLastErrorDesc	()	{return ::GetSocketErrorDesc(m_enLastError);}

public:
	virtual BOOL SetConnectionExtra(CONNID dwConnID, PVOID pExtra);
	virtual BOOL GetConnectionExtra(CONNID dwConnID, PVOID* ppExtra);

	virtual void SetSendPolicy				(EnSendPolicy enSendPolicy)		{m_enSendPolicy				= enSendPolicy;}
	virtual void SetWorkerThreadCount		(DWORD dwWorkerThreadCount)		{m_dwWorkerThreadCount		= dwWorkerThreadCount;}
	virtual void SetSocketListenQueue		(DWORD dwSocketListenQueue)		{m_dwSocketListenQueue		= dwSocketListenQueue;}
	virtual void SetAcceptSocketCount		(DWORD dwAcceptSocketCount)		{m_dwAcceptSocketCount		= dwAcceptSocketCount;}
	virtual void SetSocketBufferSize		(DWORD dwSocketBufferSize)		{m_dwSocketBufferSize		= dwSocketBufferSize;}
	virtual void SetFreeSocketObjLockTime	(DWORD dwFreeSocketObjLockTime)	{m_dwFreeSocketObjLockTime	= dwFreeSocketObjLockTime;}
	virtual void SetFreeSocketObjPool		(DWORD dwFreeSocketObjPool)		{m_dwFreeSocketObjPool		= dwFreeSocketObjPool;}
	virtual void SetFreeBufferObjPool		(DWORD dwFreeBufferObjPool)		{m_dwFreeBufferObjPool		= dwFreeBufferObjPool;}
	virtual void SetFreeSocketObjHold		(DWORD dwFreeSocketObjHold)		{m_dwFreeSocketObjHold		= dwFreeSocketObjHold;}
	virtual void SetFreeBufferObjHold		(DWORD dwFreeBufferObjHold)		{m_dwFreeBufferObjHold		= dwFreeBufferObjHold;}
	virtual void SetKeepAliveTime			(DWORD dwKeepAliveTime)			{m_dwKeepAliveTime			= dwKeepAliveTime;}
	virtual void SetKeepAliveInterval		(DWORD dwKeepAliveInterval)		{m_dwKeepAliveInterval		= dwKeepAliveInterval;}
	virtual void SetMarkSilence				(BOOL bMarkSilence)				{m_bMarkSilence				= bMarkSilence;}

	virtual EnSendPolicy GetSendPolicy		()	{return m_enSendPolicy;}
	virtual DWORD GetWorkerThreadCount		()	{return m_dwWorkerThreadCount;}
	virtual DWORD GetSocketListenQueue		()	{return m_dwSocketListenQueue;}
	virtual DWORD GetAcceptSocketCount		()	{return m_dwAcceptSocketCount;}
	virtual DWORD GetSocketBufferSize		()	{return m_dwSocketBufferSize;}
	virtual DWORD GetFreeSocketObjLockTime	()	{return m_dwFreeSocketObjLockTime;}
	virtual DWORD GetFreeSocketObjPool		()	{return m_dwFreeSocketObjPool;}
	virtual DWORD GetFreeBufferObjPool		()	{return m_dwFreeBufferObjPool;}
	virtual DWORD GetFreeSocketObjHold		()	{return m_dwFreeSocketObjHold;}
	virtual DWORD GetFreeBufferObjHold		()	{return m_dwFreeBufferObjHold;}
	virtual DWORD GetKeepAliveTime			()	{return m_dwKeepAliveTime;}
	virtual DWORD GetKeepAliveInterval		()	{return m_dwKeepAliveInterval;}
	virtual BOOL  IsMarkSilence				()	{return m_bMarkSilence;}

protected:
	virtual EnHandleResult FirePrepareListen(SOCKET soListen)
		{return m_psoListener->OnPrepareListen(soListen);}
	virtual EnHandleResult FireAccept(TSocketObj* pSocketObj)
		{return m_psoListener->OnAccept(pSocketObj->connID, pSocketObj->socket);}
	virtual EnHandleResult FireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return m_psoListener->OnReceive(pSocketObj->connID, pData, iLength);}
	virtual EnHandleResult FireReceive(TSocketObj* pSocketObj, int iLength)
		{return m_psoListener->OnReceive(pSocketObj->connID, iLength);}
	virtual EnHandleResult FireSend(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return m_psoListener->OnSend(pSocketObj->connID, pData, iLength);}
	virtual EnHandleResult FireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
		{return m_psoListener->OnClose(pSocketObj->connID, enOperation, iErrorCode);}
	virtual EnHandleResult FireShutdown()
		{return m_psoListener->OnShutdown();}

	void SetLastError(EnSocketError code, LPCSTR func, int ec);
	virtual BOOL CheckParams();
	virtual void PrepareStart();
	virtual void Reset(BOOL bAll = TRUE);

private:
	EnHandleResult TriggerFireAccept(TSocketObj* pSocketObj);
	EnHandleResult TriggerFireReceive(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
	EnHandleResult TriggerFireSend(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
	EnHandleResult TriggerFireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode);

protected:
	BOOL SetConnectionExtra(TSocketObj* pSocketObj, PVOID pExtra);
	BOOL GetConnectionExtra(TSocketObj* pSocketObj, PVOID* ppExtra);
	BOOL SetConnectionReserved(CONNID dwConnID, PVOID pReserved);
	BOOL GetConnectionReserved(CONNID dwConnID, PVOID* ppReserved);
	BOOL SetConnectionReserved(TSocketObj* pSocketObj, PVOID pReserved);
	BOOL GetConnectionReserved(TSocketObj* pSocketObj, PVOID* ppReserved);

private:
	BOOL CheckStarting();
	BOOL CheckStoping();
	BOOL CreateListenSocket(LPCTSTR pszBindAddress, USHORT usPort);
	BOOL CreateCompletePort();
	BOOL CreateWorkerThreads();
	BOOL StartAccept();

	void CloseListenSocket();
	void WaitForAcceptSocketClose();
	void DisconnectClientSocket();
	void WaitForClientSocketClose();
	void ReleaseClientSocket();
	void ReleaseFreeSocket();
	void ReleaseFreeBuffer();
	void WaitForWorkerThreadEnd();
	void CloseCompletePort();

	TBufferObj*	GetFreeBufferObj(int iLen = 0);
	TSocketObj*	GetFreeSocketObj(CONNID dwConnID, SOCKET soClient);
	void		AddFreeBufferObj(TBufferObj* pBufferObj);
	void		AddFreeSocketObj(TSocketObj* pSocketObj, EnSocketCloseFlag enFlag = SCF_NONE, EnSocketOperation enOperation = SO_UNKNOWN, int iErrorCode = 0);
	TBufferObj*	CreateBufferObj();
	TSocketObj*	CreateSocketObj();
	void		DeleteBufferObj(TBufferObj* pBufferObj);
	void		DeleteSocketObj(TSocketObj* pSocketObj);
	BOOL		InvalidSocketObj(TSocketObj* pSocketObj);
	void		ReleaseGCSocketObj(BOOL bForce = FALSE);

	void		AddClientSocketObj(CONNID dwConnID, TSocketObj* pSocketObj);
	void		CloseClientSocketObj(TSocketObj* pSocketObj, EnSocketCloseFlag enFlag = SCF_NONE, EnSocketOperation enOperation = SO_UNKNOWN, int iErrorCode = 0, int iShutdownFlag = SD_SEND);
	TSocketObj* FindSocketObj(CONNID dwConnID);

private:
	static UINT WINAPI WorkerThreadProc(LPVOID pv);

	EnIocpAction CheckIocpCommand(OVERLAPPED* pOverlapped, DWORD dwBytes, ULONG_PTR ulCompKey);

	void ForceDisconnect(CONNID dwConnID);
	void HandleIo		(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj, DWORD dwBytes, DWORD dwErrorCode);
	void HandleError	(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj, DWORD dwErrorCode);
	void HandleAccept	(SOCKET soListen, TBufferObj* pBufferObj);
	void HandleSend		(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj);
	void HandleReceive	(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj);

	int SendInternal(TSocketObj* pSocketObj, const WSABUF pBuffers[], int iCount);
	int SendPack	(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength);
	int SendSafe	(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength);
	int SendDirect	(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength);
	int CatAndPost	(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength, BOOL isPostSend);

	BOOL DoAccept	();
	int DoReceive	(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj);

	int DoSend		(CONNID dwConnID);
	int DoSend		(TSocketObj* pSocketObj);
	int DoSendPack	(TSocketObj* pSocketObj);
	int DoSendSafe	(TSocketObj* pSocketObj);
	int SendItem	(TSocketObj* pSocketObj);

	void CheckError	(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode);

private:
	EnSendPolicy m_enSendPolicy;
	DWORD m_dwWorkerThreadCount;
	DWORD m_dwSocketListenQueue;
	DWORD m_dwAcceptSocketCount;
	DWORD m_dwSocketBufferSize;
	DWORD m_dwFreeSocketObjLockTime;
	DWORD m_dwFreeSocketObjPool;
	DWORD m_dwFreeBufferObjPool;
	DWORD m_dwFreeSocketObjHold;
	DWORD m_dwFreeBufferObjHold;
	DWORD m_dwKeepAliveTime;
	DWORD m_dwKeepAliveInterval;
	BOOL  m_bMarkSilence;

private:
	CInitSocket					m_wsSocket;
	LPFN_ACCEPTEX				m_pfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS	m_pfnGetAcceptExSockaddrs;
	LPFN_DISCONNECTEX			m_pfnDisconnectEx;

private:
	ITcpServerListener*	m_psoListener;
	SOCKET				m_soListen;
	HANDLE				m_hCompletePort;
	EnServiceState		m_enState;
	EnSocketError		m_enLastError;

	vector<HANDLE>		m_vtWorkerThreads;

	CPrivateHeap		m_phBuffer;
	CPrivateHeap		m_phSocket;
	CItemPool			m_itPool;

	CSpinGuard			m_csState;

	CRWLock				m_csClientSocket;
	TSocketObjPtrMap	m_mpClientSocket;

	TBufferObjPtrList	m_lsFreeBuffer;
	TSocketObjPtrList	m_lsFreeSocket;
	TSocketObjPtrQueue	m_lsGCSocket;

	volatile long		m_iRemainAcceptSockets;
};
