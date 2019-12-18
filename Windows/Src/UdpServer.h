/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Author	: Bruce Liang
 * Website	: https://github.com/ldcsaa
 * Project	: https://github.com/ldcsaa/HP-Socket/HP-Socket
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 44636872, 75375912
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

#include "MiscHelper.h"
#include "../Common/Src/Event.h"
#include "../Common/Src/RWLock.h"
#include "../Common/Src/STLHelper.h"
#include "../Common/Src/RingBuffer.h"
#include "../Common/Src/PrivateHeap.h"

#ifdef _UDP_SUPPORT

class CUdpServer : public IUdpServer
{
public:
	virtual BOOL Start	(LPCTSTR lpszBindAddress, USHORT usPort);
	virtual BOOL Stop	();
	virtual BOOL Send	(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset = 0);
	virtual BOOL SendPackets	(CONNID dwConnID, const WSABUF pBuffers[], int iCount);
	virtual BOOL PauseReceive	(CONNID dwConnID, BOOL bPause = TRUE);
	virtual BOOL Wait			(DWORD dwMilliseconds = INFINITE) {return m_evWait.Wait(dwMilliseconds);}
	virtual BOOL			HasStarted					()	{return m_enState == SS_STARTED || m_enState == SS_STARTING;}
	virtual EnServiceState	GetState					()	{return m_enState;}
	virtual BOOL			Disconnect					(CONNID dwConnID, BOOL bForce = TRUE);
	virtual BOOL			DisconnectLongConnections	(DWORD dwPeriod, BOOL bForce = TRUE);
	virtual BOOL			DisconnectSilenceConnections(DWORD dwPeriod, BOOL bForce = TRUE);
	virtual BOOL			GetListenAddress			(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);
	virtual BOOL			GetLocalAddress				(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);
	virtual BOOL			GetRemoteAddress			(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);

	virtual BOOL IsConnected			(CONNID dwConnID);
	virtual BOOL IsPauseReceive			(CONNID dwConnID, BOOL& bPaused);
	virtual BOOL GetPendingDataLength	(CONNID dwConnID, int& iPending);
	virtual DWORD GetConnectionCount	();
	virtual BOOL GetAllConnectionIDs	(CONNID pIDs[], DWORD& dwCount);
	virtual BOOL GetConnectPeriod		(CONNID dwConnID, DWORD& dwPeriod);
	virtual BOOL GetSilencePeriod		(CONNID dwConnID, DWORD& dwPeriod);
	virtual EnSocketError GetLastError	()	{return m_enLastError;}
	virtual LPCTSTR GetLastErrorDesc	()	{return ::GetSocketErrorDesc(m_enLastError);}

public:
	virtual BOOL IsSecure				() {return FALSE;}

	virtual BOOL SetConnectionExtra(CONNID dwConnID, PVOID pExtra);
	virtual BOOL GetConnectionExtra(CONNID dwConnID, PVOID* ppExtra);

	virtual void SetReuseAddressPolicy		(EnReuseAddressPolicy enReusePolicy)	{ENSURE_HAS_STOPPED(); m_enReusePolicy		= enReusePolicy;}
	virtual void SetSendPolicy				(EnSendPolicy enSendPolicy)				{ENSURE_HAS_STOPPED(); m_enSendPolicy		= enSendPolicy;}
	virtual void SetOnSendSyncPolicy		(EnOnSendSyncPolicy enOnSendSyncPolicy)	{ENSURE_HAS_STOPPED(); m_enOnSendSyncPolicy	= enOnSendSyncPolicy;}
	virtual void SetMaxConnectionCount		(DWORD dwMaxConnectionCount)	{ENSURE_HAS_STOPPED(); m_dwMaxConnectionCount		= dwMaxConnectionCount;}
	virtual void SetWorkerThreadCount		(DWORD dwWorkerThreadCount)		{ENSURE_HAS_STOPPED(); m_dwWorkerThreadCount		= dwWorkerThreadCount;}
	virtual void SetFreeSocketObjLockTime	(DWORD dwFreeSocketObjLockTime)	{ENSURE_HAS_STOPPED(); m_dwFreeSocketObjLockTime	= dwFreeSocketObjLockTime;}
	virtual void SetFreeSocketObjPool		(DWORD dwFreeSocketObjPool)		{ENSURE_HAS_STOPPED(); m_dwFreeSocketObjPool		= dwFreeSocketObjPool;}
	virtual void SetFreeBufferObjPool		(DWORD dwFreeBufferObjPool)		{ENSURE_HAS_STOPPED(); m_dwFreeBufferObjPool		= dwFreeBufferObjPool;}
	virtual void SetFreeSocketObjHold		(DWORD dwFreeSocketObjHold)		{ENSURE_HAS_STOPPED(); m_dwFreeSocketObjHold		= dwFreeSocketObjHold;}
	virtual void SetFreeBufferObjHold		(DWORD dwFreeBufferObjHold)		{ENSURE_HAS_STOPPED(); m_dwFreeBufferObjHold		= dwFreeBufferObjHold;}
	virtual void SetMaxDatagramSize			(DWORD dwMaxDatagramSize)		{ENSURE_HAS_STOPPED(); m_dwMaxDatagramSize			= dwMaxDatagramSize;}
	virtual void SetPostReceiveCount		(DWORD dwPostReceiveCount)		{ENSURE_HAS_STOPPED(); m_dwPostReceiveCount			= dwPostReceiveCount;}
	virtual void SetDetectAttempts			(DWORD dwDetectAttempts)		{ENSURE_HAS_STOPPED(); m_dwDetectAttempts			= dwDetectAttempts;}
	virtual void SetDetectInterval			(DWORD dwDetectInterval)		{ENSURE_HAS_STOPPED(); m_dwDetectInterval			= dwDetectInterval;}
	virtual void SetMarkSilence				(BOOL bMarkSilence)				{ENSURE_HAS_STOPPED(); m_bMarkSilence				= bMarkSilence;}

	virtual EnReuseAddressPolicy GetReuseAddressPolicy	()	{return m_enReusePolicy;}
	virtual EnSendPolicy GetSendPolicy					()	{return m_enSendPolicy;}
	virtual EnOnSendSyncPolicy GetOnSendSyncPolicy		()	{return m_enOnSendSyncPolicy;}
	virtual DWORD GetMaxConnectionCount		()	{return m_dwMaxConnectionCount;}
	virtual DWORD GetWorkerThreadCount		()	{return m_dwWorkerThreadCount;}
	virtual DWORD GetFreeSocketObjLockTime	()	{return m_dwFreeSocketObjLockTime;}
	virtual DWORD GetFreeSocketObjPool		()	{return m_dwFreeSocketObjPool;}
	virtual DWORD GetFreeBufferObjPool		()	{return m_dwFreeBufferObjPool;}
	virtual DWORD GetFreeSocketObjHold		()	{return m_dwFreeSocketObjHold;}
	virtual DWORD GetFreeBufferObjHold		()	{return m_dwFreeBufferObjHold;}
	virtual DWORD GetMaxDatagramSize		()	{return m_dwMaxDatagramSize;}
	virtual DWORD GetPostReceiveCount		()	{return m_dwPostReceiveCount;}
	virtual DWORD GetDetectAttempts			()	{return m_dwDetectAttempts;}
	virtual DWORD GetDetectInterval			()	{return m_dwDetectInterval;}
	virtual BOOL  IsMarkSilence				()	{return m_bMarkSilence;}

protected:
	virtual EnHandleResult FirePrepareListen(SOCKET soListen)
		{return DoFirePrepareListen(soListen);}
	virtual EnHandleResult FireAccept(TUdpSocketObj* pSocketObj)
		{
			EnHandleResult rs		= DoFireAccept(pSocketObj);
			if(rs != HR_ERROR) rs	= FireHandShake(pSocketObj);
			return rs;
		}
	virtual EnHandleResult FireHandShake(TUdpSocketObj* pSocketObj)
		{return DoFireHandShake(pSocketObj);}
	virtual EnHandleResult FireReceive(TUdpSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return DoFireReceive(pSocketObj, pData, iLength);}
	virtual EnHandleResult FireReceive(TUdpSocketObj* pSocketObj, int iLength)
		{return DoFireReceive(pSocketObj, iLength);}
	virtual EnHandleResult FireSend(TUdpSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return DoFireSend(pSocketObj, pData, iLength);}
	virtual EnHandleResult FireClose(TUdpSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
		{return DoFireClose(pSocketObj, enOperation, iErrorCode);}
	virtual EnHandleResult FireShutdown()
		{return DoFireShutdown();}

	virtual EnHandleResult DoFirePrepareListen(SOCKET soListen)
		{return m_pListener->OnPrepareListen(this, soListen);}
	virtual EnHandleResult DoFireAccept(TUdpSocketObj* pSocketObj)
		{return m_pListener->OnAccept(this, pSocketObj->connID, (UINT_PTR)(&pSocketObj->remoteAddr));}
	virtual EnHandleResult DoFireHandShake(TUdpSocketObj* pSocketObj)
		{return m_pListener->OnHandShake(this, pSocketObj->connID);}
	virtual EnHandleResult DoFireReceive(TUdpSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return m_pListener->OnReceive(this, pSocketObj->connID, pData, iLength);}
	virtual EnHandleResult DoFireReceive(TUdpSocketObj* pSocketObj, int iLength)
		{return m_pListener->OnReceive(this, pSocketObj->connID, iLength);}
	virtual EnHandleResult DoFireSend(TUdpSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return m_pListener->OnSend(this, pSocketObj->connID, pData, iLength);}
	virtual EnHandleResult DoFireClose(TUdpSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
		{return m_pListener->OnClose(this, pSocketObj->connID, enOperation, iErrorCode);}
	virtual EnHandleResult DoFireShutdown()
		{return m_pListener->OnShutdown(this);}

	void SetLastError(EnSocketError code, LPCSTR func, int ec);
	virtual BOOL CheckParams();
	virtual void PrepareStart();
	virtual void Reset();

	virtual void OnWorkerThreadStart(THR_ID dwThreadID) {}
	virtual void OnWorkerThreadEnd(THR_ID dwThreadID) {}

	TUdpSocketObj*	FindSocketObj(CONNID dwConnID);
	int				SendInternal(TUdpSocketObj* pSocketObj, TUdpBufferObjPtr& bufPtr);

	BOOL DoSend(TUdpSocketObj* pSocketObj, const BYTE* pBuffer, int iLength, int iOffset = 0);

private:
	EnHandleResult TriggerFireAccept(TUdpSocketObj* pSocketObj);
	EnHandleResult TriggerFireReceive(TUdpSocketObj* pSocketObj, TUdpBufferObj* pBufferObj);
	EnHandleResult TriggerFireSend(TUdpSocketObj* pSocketObj, TUdpBufferObj* pBufferObj);
	EnHandleResult TriggerFireClose(TUdpSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode);

protected:
	BOOL SetConnectionExtra(TUdpSocketObj* pSocketObj, PVOID pExtra);
	BOOL GetConnectionExtra(TUdpSocketObj* pSocketObj, PVOID* ppExtra);
	BOOL SetConnectionReserved(CONNID dwConnID, PVOID pReserved);
	BOOL GetConnectionReserved(CONNID dwConnID, PVOID* ppReserved);
	BOOL SetConnectionReserved(TUdpSocketObj* pSocketObj, PVOID pReserved);
	BOOL GetConnectionReserved(TUdpSocketObj* pSocketObj, PVOID* ppReserved);
	BOOL SetConnectionReserved2(CONNID dwConnID, PVOID pReserved2);
	BOOL GetConnectionReserved2(CONNID dwConnID, PVOID* ppReserved2);
	BOOL SetConnectionReserved2(TUdpSocketObj* pSocketObj, PVOID pReserved2);
	BOOL GetConnectionReserved2(TUdpSocketObj* pSocketObj, PVOID* ppReserved2);

private:
	friend void ContinueReceiveFrom<>(CUdpServer* pThis, TUdpBufferObj* pBufferObj);
	
	static UINT WINAPI WorkerThreadProc(LPVOID pv);
	static void WINAPI DetectConnectionProc(LPVOID pv, BOOLEAN bTimerFired);

private:
	BOOL CheckStarting();
	BOOL CheckStoping();
	BOOL CreateListenSocket(LPCTSTR lpszBindAddress, USHORT usPort);
	BOOL CreateCompletePort();
	BOOL CreateWorkerThreads();
	BOOL StartAccept();

	void SendCloseNotify();
	void CloseListenSocket();
	void WaitForPostReceiveRelease();
	void DisconnectClientSocket();
	void WaitForClientSocketClose();
	void ReleaseClientSocket();
	void ReleaseFreeSocket();
	void ReleaseFreeBuffer();
	void WaitForWorkerThreadEnd();
	void CloseCompletePort();

	TUdpBufferObj*	GetFreeBufferObj(int iLen = -1);
	TUdpSocketObj*	GetFreeSocketObj(CONNID dwConnID);
	void			AddFreeBufferObj(TUdpBufferObj* pBufferObj);
	void			AddFreeSocketObj(CONNID dwConnID, EnSocketCloseFlag enFlag = SCF_NONE, EnSocketOperation enOperation = SO_UNKNOWN, int iErrorCode = 0, BOOL bNotify = TRUE);
	void			AddFreeSocketObj(TUdpSocketObj* pSocketObj, EnSocketCloseFlag enFlag = SCF_NONE, EnSocketOperation enOperation = SO_UNKNOWN, int iErrorCode = 0, BOOL bNotify = TRUE);
	TUdpSocketObj*	CreateSocketObj();
	void			DeleteSocketObj(TUdpSocketObj* pSocketObj);
	BOOL			InvalidSocketObj(TUdpSocketObj* pSocketObj);
	void			ReleaseGCSocketObj(BOOL bForce = FALSE);

	void			AddClientSocketObj(CONNID dwConnID, TUdpSocketObj* pSocketObj, const HP_SOCKADDR& remoteAddr);
	void			CloseClientSocketObj(TUdpSocketObj* pSocketObj, EnSocketCloseFlag enFlag = SCF_NONE, EnSocketOperation enOperation = SO_UNKNOWN, int iErrorCode = 0, BOOL bNotify = TRUE);

	CONNID			FindConnectionID(const HP_SOCKADDR* pAddr);

private:
	EnIocpAction CheckIocpCommand(OVERLAPPED* pOverlapped, DWORD dwBytes, ULONG_PTR ulCompKey);

	void ForceDisconnect(CONNID dwConnID, BOOL bNotify = FALSE);
	void HandleIo		(CONNID dwConnID, TUdpBufferObj* pBufferObj, DWORD dwBytes, DWORD dwErrorCode);
	void HandleError	(CONNID dwConnID, TUdpBufferObj* pBufferObj, DWORD dwErrorCode);
	void HandleZeroBytes(CONNID dwConnID, TUdpBufferObj* pBufferObj);
	CONNID HandleAccept	(TUdpBufferObj* pBufferObj);
	void HandleSend		(CONNID dwConnID, TUdpBufferObj* pBufferObj);
	void HandleReceive	(CONNID dwConnID, TUdpBufferObj* pBufferObj);
	void ProcessReceive	(CONNID dwConnID, TUdpBufferObj* pBufferObj);
	void ProcessReceiveBufferObj(TUdpBufferObj* pBufferObj);

	int SendPack	(TUdpSocketObj* pSocketObj, TUdpBufferObjPtr& bufPtr);
	int SendSafe	(TUdpSocketObj* pSocketObj, TUdpBufferObjPtr& bufPtr);
	int CatAndPost	(TUdpSocketObj* pSocketObj, TUdpBufferObjPtr& bufPtr);
	int SendDirect	(TUdpSocketObj* pSocketObj, TUdpBufferObjPtr& bufPtr);

	int DoReceive	(TUdpBufferObj* pBufferObj);

	int DoSend		(CONNID dwConnID);
	int DoSend		(TUdpSocketObj* pSocketObj);
	int DoSendPack	(TUdpSocketObj* pSocketObj);
	int DoSendSafe	(TUdpSocketObj* pSocketObj);
	int SendItem	(TUdpSocketObj* pSocketObj);

	BOOL SendDetectPackage		(CONNID dwConnID, TUdpSocketObj* pSocketObj);
	BOOL IsNeedDetectConnection	()	{return m_dwDetectAttempts > 0 && m_dwDetectInterval > 0;}

	SOCKET GetListenSocket		()	{return m_soListen;}

public:
	CUdpServer(IUdpServerListener* pListener)
	: m_pListener				(pListener)
	, m_hCompletePort			(nullptr)
	, m_soListen				(INVALID_SOCKET)
	, m_iRemainPostReceives		(0)
	, m_enLastError				(SE_OK)
	, m_enState					(SS_STOPPED)
	, m_usFamily				(AF_UNSPEC)
	, m_enSendPolicy			(SP_PACK)
	, m_enOnSendSyncPolicy		(OSSP_NONE)
	, m_enReusePolicy			(RAP_ADDR_ONLY)
	, m_dwMaxConnectionCount	(DEFAULT_CONNECTION_COUNT)
	, m_dwWorkerThreadCount		(DEFAULT_WORKER_THREAD_COUNT)
	, m_dwFreeSocketObjLockTime	(DEFAULT_FREE_SOCKETOBJ_LOCK_TIME)
	, m_dwFreeSocketObjPool		(DEFAULT_FREE_SOCKETOBJ_POOL)
	, m_dwFreeBufferObjPool		(DEFAULT_FREE_BUFFEROBJ_POOL)
	, m_dwFreeSocketObjHold		(DEFAULT_FREE_SOCKETOBJ_HOLD)
	, m_dwFreeBufferObjHold		(DEFAULT_FREE_BUFFEROBJ_HOLD)
	, m_dwMaxDatagramSize		(DEFAULT_UDP_MAX_DATAGRAM_SIZE)
	, m_dwPostReceiveCount		(DEFAULT_UDP_POST_RECEIVE_COUNT)
	, m_dwDetectAttempts		(DEFAULT_UDP_DETECT_ATTEMPTS)
	, m_dwDetectInterval		(DEFAULT_UDP_DETECT_INTERVAL)
	, m_bMarkSilence			(TRUE)
	, m_evWait					(TRUE, TRUE)
	{
		ASSERT(sm_wsSocket.IsValid());
		ASSERT(m_pListener);
	}

	virtual ~CUdpServer()
	{
		ENSURE_STOP();
	}

private:
	EnReuseAddressPolicy m_enReusePolicy;
	EnSendPolicy m_enSendPolicy;
	EnOnSendSyncPolicy m_enOnSendSyncPolicy;
	DWORD m_dwMaxConnectionCount;
	DWORD m_dwWorkerThreadCount;
	DWORD m_dwFreeSocketObjLockTime;
	DWORD m_dwFreeSocketObjPool;
	DWORD m_dwFreeBufferObjPool;
	DWORD m_dwFreeSocketObjHold;
	DWORD m_dwFreeBufferObjHold;
	DWORD m_dwMaxDatagramSize;
	DWORD m_dwPostReceiveCount;
	DWORD m_dwDetectAttempts;
	DWORD m_dwDetectInterval;
	BOOL  m_bMarkSilence;

protected:
	CUdpBufferObjPool		m_bfObjPool;

private:
	static const CInitSocket& sm_wsSocket;

	CEvt					m_evWait;

	ADDRESS_FAMILY			m_usFamily;

	IUdpServerListener*		m_pListener;
	SOCKET					m_soListen;
	HANDLE					m_hCompletePort;
	EnServiceState			m_enState;
	EnSocketError			m_enLastError;

	vector<HANDLE>			m_vtWorkerThreads;

	CPrivateHeap			m_phSocket;

	CSpinGuard				m_csState;

	CCriSec					m_csAccept;

	CTimerQueue				m_tqDetect;

	TUdpSocketObjPtrPool	m_bfActiveSockets;

	CSimpleRWLock			m_csClientSocket;
	TSockAddrMap			m_mpClientAddr;

	TUdpSocketObjPtrList	m_lsFreeSocket;
	TUdpSocketObjPtrQueue	m_lsGCSocket;

	volatile long			m_iRemainPostReceives;
};

#endif
