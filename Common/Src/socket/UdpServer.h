/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.1.3
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
#include "../PrivateHeap.h"
#include "../Event.h"
#include "../RWLock.h"
#include "../CriticalSection.h"
#include "../STLHelper.h"

class CUdpServer : public IUdpServer
{
public:
	static const DWORD MAX_WORKER_THREAD_COUNT;

	static const DWORD DEFAULT_WORKER_THREAD_COUNT;
	static const DWORD DEFAULT_FREE_SOCKETOBJ_LOCK_TIME;
	static const DWORD DEFAULT_FREE_SOCKETOBJ_POOL;
	static const DWORD DEFAULT_FREE_SOCKETOBJ_HOLD;
	static const DWORD DEFAULT_FREE_BUFFEROBJ_POOL;
	static const DWORD DEFAULT_FREE_BUFFEROBJ_HOLD;
	static const DWORD DEFAULT_MAX_DATAGRAM_SIZE;
	static const DWORD DEFAULT_DETECT_ATTEMPTS;
	static const DWORD DEFAULT_DETECT_INTERVAL;
	static const DWORD DEFAULT_MAX_SHUTDOWN_WAIT_TIME;

public:
	CUdpServer(IUdpServerListener* psoListener)
	: m_psoListener				(psoListener)
	, m_hCompletePort			(nullptr)
	, m_soListen				(INVALID_SOCKET)
	, m_enLastError				(SE_OK)
	, m_enState					(SS_STOPED)
	, m_hDetector				(nullptr)
	, m_dwWorkerThreadCount		(DEFAULT_WORKER_THREAD_COUNT)
	, m_dwFreeSocketObjLockTime	(DEFAULT_FREE_SOCKETOBJ_LOCK_TIME)
	, m_dwFreeSocketObjPool		(DEFAULT_FREE_SOCKETOBJ_POOL)
	, m_dwFreeBufferObjPool		(DEFAULT_FREE_BUFFEROBJ_POOL)
	, m_dwFreeSocketObjHold		(DEFAULT_FREE_SOCKETOBJ_HOLD)
	, m_dwFreeBufferObjHold		(DEFAULT_FREE_BUFFEROBJ_HOLD)
	, m_dwMaxDatagramSize		(DEFAULT_MAX_DATAGRAM_SIZE)
	, m_dwDetectAttempts		(DEFAULT_DETECT_ATTEMPTS)
	, m_dwDetectInterval		(DEFAULT_DETECT_INTERVAL)
	, m_dwMaxShutdownWaitTime	(DEFAULT_MAX_SHUTDOWN_WAIT_TIME)
	{
		ASSERT(m_wsSocket.IsValid());
		ASSERT(m_psoListener);

		Reset();
	}

	virtual ~CUdpServer()
	{
		if(HasStarted())
			Stop();
	}

public:
	virtual BOOL Start	(LPCTSTR pszBindAddress, USHORT usPort);
	virtual BOOL Stop	();
	virtual BOOL Send	(CONNID dwConnID, const BYTE* pBuffer, int iLength);
	virtual BOOL			HasStarted					()	{return m_enState == SS_STARTED || m_enState == SS_STARTING;}
	virtual EnServiceState	GetState					()	{return m_enState;}
	virtual EnServerError	GetLastError				()	{return m_enLastError;}
	virtual BOOL			Disconnect					(CONNID dwConnID, BOOL bForce = TRUE);
	virtual BOOL			DisconnectLongConnections	(DWORD dwPeriod, BOOL bForce = TRUE);
	virtual BOOL			GetListenAddress			(LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort);
	virtual BOOL			GetClientAddress			(CONNID dwConnID, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort);

	virtual LPCTSTR	GetLastErrorDesc	();
	virtual DWORD	GetConnectionCount	();
	virtual BOOL	GetConnectPeriod	(CONNID dwConnID, DWORD& dwPeriod);

public:
	virtual BOOL SetConnectionExtra(CONNID dwConnID, PVOID pExtra);
	virtual BOOL GetConnectionExtra(CONNID dwConnID, PVOID* ppExtra);

	virtual void SetWorkerThreadCount		(DWORD dwWorkerThreadCount)		{m_dwWorkerThreadCount		= dwWorkerThreadCount;}
	virtual void SetFreeSocketObjLockTime	(DWORD dwFreeSocketObjLockTime)	{m_dwFreeSocketObjLockTime	= dwFreeSocketObjLockTime;}
	virtual void SetFreeSocketObjPool		(DWORD dwFreeSocketObjPool)		{m_dwFreeSocketObjPool		= dwFreeSocketObjPool;}
	virtual void SetFreeBufferObjPool		(DWORD dwFreeBufferObjPool)		{m_dwFreeBufferObjPool		= dwFreeBufferObjPool;}
	virtual void SetFreeSocketObjHold		(DWORD dwFreeSocketObjHold)		{m_dwFreeSocketObjHold		= dwFreeSocketObjHold;}
	virtual void SetFreeBufferObjHold		(DWORD dwFreeBufferObjHold)		{m_dwFreeBufferObjHold		= dwFreeBufferObjHold;}
	virtual void SetMaxDatagramSize			(DWORD dwMaxDatagramSize)		{m_dwMaxDatagramSize		= dwMaxDatagramSize;}
	virtual void SetDetectAttempts			(DWORD dwDetectAttempts)		{m_dwDetectAttempts			= dwDetectAttempts;}
	virtual void SetDetectInterval			(DWORD dwDetectInterval)		{m_dwDetectInterval			= dwDetectInterval;}
	virtual void SetMaxShutdownWaitTime		(DWORD dwMaxShutdownWaitTime)	{m_dwMaxShutdownWaitTime	= dwMaxShutdownWaitTime;}

	virtual DWORD GetWorkerThreadCount		()	{return m_dwWorkerThreadCount;}
	virtual DWORD GetFreeSocketObjLockTime	()	{return m_dwFreeSocketObjLockTime;}
	virtual DWORD GetFreeSocketObjPool		()	{return m_dwFreeSocketObjPool;}
	virtual DWORD GetFreeBufferObjPool		()	{return m_dwFreeBufferObjPool;}
	virtual DWORD GetFreeSocketObjHold		()	{return m_dwFreeSocketObjHold;}
	virtual DWORD GetFreeBufferObjHold		()	{return m_dwFreeBufferObjHold;}
	virtual DWORD GetMaxDatagramSize		()	{return m_dwMaxDatagramSize;}
	virtual DWORD GetDetectAttempts			()	{return m_dwDetectAttempts;}
	virtual DWORD GetDetectInterval			()	{return m_dwDetectInterval;}
	virtual DWORD GetMaxShutdownWaitTime	()	{return m_dwMaxShutdownWaitTime;}

protected:
	virtual ISocketListener::EnHandleResult FirePrepareListen(SOCKET soListen)
		{return m_psoListener->OnPrepareListen(soListen);}
	virtual ISocketListener::EnHandleResult FireAccept(CONNID dwConnID, const SOCKADDR_IN* pSockAddr)
		{return m_psoListener->OnAccept(dwConnID, pSockAddr);}
	virtual ISocketListener::EnHandleResult FireSend(CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_psoListener->OnSend(dwConnID, pData, iLength);}
	virtual ISocketListener::EnHandleResult FireReceive(CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_psoListener->OnReceive(dwConnID, pData, iLength);}
	virtual ISocketListener::EnHandleResult FireReceive(CONNID dwConnID, int iLength)
		{return m_psoListener->OnReceive(dwConnID, iLength);}
	virtual ISocketListener::EnHandleResult FireClose(CONNID dwConnID)
		{return m_psoListener->OnClose(dwConnID);}
	virtual ISocketListener::EnHandleResult FireError(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
		{return m_psoListener->OnError(dwConnID, enOperation, iErrorCode);}
	virtual ISocketListener::EnHandleResult FireServerShutdown()
		{return m_psoListener->OnServerShutdown();}

	virtual BOOL CheckParams();

private:
	BOOL CheckStarting();
	BOOL CheckStoping();
	BOOL CreateListenSocket(LPCTSTR pszBindAddress, USHORT usPort);
	BOOL CreateCompletePort();
	BOOL CreateWorkerThreads();
	BOOL CreateDetectorThread();
	BOOL StartAccept();

	void CloseListenSocket();
	void DisconnectClientSocket();
	void WaitForClientSocketClose();
	void ReleaseClientSocket();
	void ReleaseFreeSocket();
	void CompressFreeSocket(size_t size, BOOL bForce = FALSE);
	void ReleaseFreeBuffer();
	void CompressFreeBuffer(size_t size);
	void WaitForWorkerThreadEnd();
	void WaitForDetectorThreadEnd();
	void TerminateWorkerThread();
	void CloseCompletePort();

	void Reset();

	TUdpBufferObj*	GetFreeBufferObj(int iLen = -1);
	TUdpSocketObj*	GetFreeSocketObj(CONNID dwConnID);
	void			AddFreeBufferObj(TUdpBufferObj* pBufferObj);
	void			AddFreeSocketObj(CONNID dwConnID, EnSocketCloseFlag enFlag = SCF_NONE, EnSocketOperation enOperation = SO_UNKNOWN, int iErrorCode = 0);
	TUdpBufferObj*	CreateBufferObj();
	TUdpSocketObj*	CreateSocketObj();
	void			DeleteBufferObj(TUdpBufferObj* pBufferObj);
	void			DeleteSocketObj(TUdpSocketObj* pSocketObj);

	void			AddClientSocketObj(CONNID dwConnID, TUdpSocketObj* pSocketObj);
	void			CloseClientSocketObj(TUdpSocketObj* pSocketObj, EnSocketCloseFlag enFlag = SCF_NONE, EnSocketOperation enOperation = SO_UNKNOWN, int iErrorCode = 0);
	TUdpSocketObj*	FindSocketObj(CONNID dwConnID);

	CONNID			FindConnectionID(SOCKADDR_IN* pAddr);

private:
	void SetLastError(EnServerError code, LPCSTR func, int ec);

private:
	static UINT WINAPI WorkerThreadProc(LPVOID pv);
	static UINT WINAPI DetecotrThreadProc(LPVOID pv);

	int CheckSpecialIndative(OVERLAPPED* pOverlapped, DWORD dwBytes, ULONG_PTR ulCompKey);
	void ForceDisconnect	(CONNID dwConnID);
	void HandleIo			(CONNID dwConnID, TUdpBufferObj* pBufferObj, DWORD dwBytes, DWORD dwErrorCode);
	void HandleError		(CONNID dwConnID, TUdpBufferObj* pBufferObj, DWORD dwErrorCode);
	void HandleZeroBytes	(CONNID dwConnID, TUdpBufferObj* pBufferObj);
	CONNID HandleAccept		(TUdpBufferObj* pBufferObj);
	void HandleSend			(CONNID dwConnID, TUdpBufferObj* pBufferObj);
	void HandleReceive		(CONNID dwConnID, TUdpBufferObj* pBufferObj);
	BOOL DoAccept			();
	BOOL DoSend				(TUdpSocketObj* pSocketObj, const BYTE* pBuffer, int iLen);
	BOOL DoReceive			(TUdpBufferObj* pBufferObj);

	void DetectConnections	();
	BOOL NeedDetectorThread	() {return m_dwDetectAttempts > 0 && m_dwDetectInterval > 0;}

private:
	CInitSocket			m_wsSocket;

private:
	CPrivateHeap		m_phSocket;
	CPrivateHeap		m_phBuffer;

	CEvt				m_evDetector;
	HANDLE				m_hDetector;

	CCriSec				m_csAccept;
	CCriSec				m_csFreeBuffer;
	CCriSec				m_csFreeSocket;
	CRWLock				m_csClientSocket;

	TUdpBufferObjPtrList	m_lsFreeBuffer;
	TUdpSocketObjPtrList	m_lsFreeSocket;
	TUdpSocketObjPtrMap		m_mpClientSocket;
	TSockAddrMap			m_mpClientAddr;

	vector<HANDLE>			m_vtWorkerThreads;

	volatile EnServiceState	m_enState;
	volatile CONNID			m_dwConnID;

	EnServerError	m_enLastError;

	SOCKET			m_soListen;
	HANDLE			m_hCompletePort;

	IUdpServerListener*	m_psoListener;

private:
	DWORD m_dwWorkerThreadCount;
	DWORD m_dwFreeSocketObjLockTime;
	DWORD m_dwFreeSocketObjPool;
	DWORD m_dwFreeBufferObjPool;
	DWORD m_dwFreeSocketObjHold;
	DWORD m_dwFreeBufferObjHold;
	DWORD m_dwMaxDatagramSize;
	DWORD m_dwDetectAttempts;
	DWORD m_dwDetectInterval;
	DWORD m_dwMaxShutdownWaitTime;
};
