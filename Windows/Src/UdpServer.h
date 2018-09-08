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
#include "../Common/Src/Event.h"
#include "../Common/Src/RWLock.h"
#include "../Common/Src/STLHelper.h"
#include "../Common/Src/RingBuffer.h"
#include "../Common/Src/PrivateHeap.h"

class CUdpServer : public IUdpServer
{
public:
	virtual BOOL Start	(LPCTSTR lpszBindAddress, USHORT usPort);
	virtual BOOL Stop	();
	virtual BOOL Send	(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset = 0);
	virtual BOOL SendPackets	(CONNID dwConnID, const WSABUF pBuffers[], int iCount);
	virtual BOOL PauseReceive	(CONNID dwConnID, BOOL bPause = TRUE);
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

	virtual void SetSendPolicy				(EnSendPolicy enSendPolicy)		{m_enSendPolicy				= enSendPolicy;}
	virtual void SetMaxConnectionCount		(DWORD dwMaxConnectionCount)	{m_dwMaxConnectionCount		= dwMaxConnectionCount;}
	virtual void SetWorkerThreadCount		(DWORD dwWorkerThreadCount)		{m_dwWorkerThreadCount		= dwWorkerThreadCount;}
	virtual void SetFreeSocketObjLockTime	(DWORD dwFreeSocketObjLockTime)	{m_dwFreeSocketObjLockTime	= dwFreeSocketObjLockTime;}
	virtual void SetFreeSocketObjPool		(DWORD dwFreeSocketObjPool)		{m_dwFreeSocketObjPool		= dwFreeSocketObjPool;}
	virtual void SetFreeBufferObjPool		(DWORD dwFreeBufferObjPool)		{m_dwFreeBufferObjPool		= dwFreeBufferObjPool;}
	virtual void SetFreeSocketObjHold		(DWORD dwFreeSocketObjHold)		{m_dwFreeSocketObjHold		= dwFreeSocketObjHold;}
	virtual void SetFreeBufferObjHold		(DWORD dwFreeBufferObjHold)		{m_dwFreeBufferObjHold		= dwFreeBufferObjHold;}
	virtual void SetMaxDatagramSize			(DWORD dwMaxDatagramSize)		{m_dwMaxDatagramSize		= dwMaxDatagramSize;}
	virtual void SetPostReceiveCount		(DWORD dwPostReceiveCount)		{m_dwPostReceiveCount		= dwPostReceiveCount;}
	virtual void SetDetectAttempts			(DWORD dwDetectAttempts)		{m_dwDetectAttempts			= dwDetectAttempts;}
	virtual void SetDetectInterval			(DWORD dwDetectInterval)		{m_dwDetectInterval			= dwDetectInterval;}
	virtual void SetMarkSilence				(BOOL bMarkSilence)				{m_bMarkSilence				= bMarkSilence;}

	virtual EnSendPolicy GetSendPolicy		()	{return m_enSendPolicy;}
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
		{return m_pListener->OnPrepareListen(this, soListen);}
	virtual EnHandleResult FireAccept(TUdpSocketObj* pSocketObj)
		{
			pSocketObj->sndBuffSize	= m_dwMaxDatagramSize;
			EnHandleResult rs		= m_pListener->OnAccept(this, pSocketObj->connID, (UINT_PTR)(&pSocketObj->remoteAddr));
			if(rs != HR_ERROR) rs	= FireHandShake(pSocketObj);
			return rs;
		}
	virtual EnHandleResult FireHandShake(TUdpSocketObj* pSocketObj)
		{return m_pListener->OnHandShake(this, pSocketObj->connID);}
	virtual EnHandleResult FireReceive(TUdpSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return m_pListener->OnReceive(this, pSocketObj->connID, pData, iLength);}
	virtual EnHandleResult FireReceive(TUdpSocketObj* pSocketObj, int iLength)
		{return m_pListener->OnReceive(this, pSocketObj->connID, iLength);}
	virtual EnHandleResult FireSend(TUdpSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return m_pListener->OnSend(this, pSocketObj->connID, pData, iLength);}
	virtual EnHandleResult FireClose(TUdpSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
		{return m_pListener->OnClose(this, pSocketObj->connID, enOperation, iErrorCode);}
	virtual EnHandleResult FireShutdown()
		{return m_pListener->OnShutdown(this);}

	void SetLastError(EnSocketError code, LPCSTR func, int ec);
	virtual BOOL CheckParams();
	virtual void PrepareStart();
	virtual void Reset();

	virtual void OnWorkerThreadEnd(DWORD dwThreadID) {}

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
	static UINT WINAPI WorkerThreadProc(LPVOID pv);
	static UINT WINAPI DetecotrThreadProc(LPVOID pv);

private:
	BOOL CheckStarting();
	BOOL CheckStoping();
	BOOL CreateListenSocket(LPCTSTR lpszBindAddress, USHORT usPort);
	BOOL CreateCompletePort();
	BOOL CreateWorkerThreads();
	BOOL CreateDetectorThread();
	BOOL StartAccept();

	void CloseListenSocket();
	void WaitForPostReceiveRelease();
	void DisconnectClientSocket();
	void WaitForClientSocketClose();
	void ReleaseClientSocket();
	void ReleaseFreeSocket();
	void ReleaseFreeBuffer();
	void WaitForWorkerThreadEnd();
	void WaitForDetectorThreadEnd();
	void CloseCompletePort();

	TUdpBufferObj*	GetFreeBufferObj(int iLen = -1);
	TUdpSocketObj*	GetFreeSocketObj(CONNID dwConnID);
	void			AddFreeBufferObj(TUdpBufferObj* pBufferObj);
	void			AddFreeSocketObj(CONNID dwConnID, EnSocketCloseFlag enFlag = SCF_NONE, EnSocketOperation enOperation = SO_UNKNOWN, int iErrorCode = 0);
	void			AddFreeSocketObj(TUdpSocketObj* pSocketObj, EnSocketCloseFlag enFlag = SCF_NONE, EnSocketOperation enOperation = SO_UNKNOWN, int iErrorCode = 0);
	TUdpSocketObj*	CreateSocketObj();
	void			DeleteSocketObj(TUdpSocketObj* pSocketObj);
	BOOL			InvalidSocketObj(TUdpSocketObj* pSocketObj);
	void			ReleaseGCSocketObj(BOOL bForce = FALSE);

	void			AddClientSocketObj(CONNID dwConnID, TUdpSocketObj* pSocketObj, const HP_SOCKADDR& remoteAddr);
	void			CloseClientSocketObj(TUdpSocketObj* pSocketObj, EnSocketCloseFlag enFlag = SCF_NONE, EnSocketOperation enOperation = SO_UNKNOWN, int iErrorCode = 0);
	TUdpSocketObj*	FindSocketObj(CONNID dwConnID);

	CONNID			FindConnectionID(const HP_SOCKADDR* pAddr);

private:
	EnIocpAction CheckIocpCommand(OVERLAPPED* pOverlapped, DWORD dwBytes, ULONG_PTR ulCompKey);

	void ForceDisconnect(CONNID dwConnID);
	void HandleIo		(CONNID dwConnID, TUdpBufferObj* pBufferObj, DWORD dwBytes, DWORD dwErrorCode);
	void HandleError	(CONNID dwConnID, TUdpBufferObj* pBufferObj, DWORD dwErrorCode);
	void HandleZeroBytes(CONNID dwConnID, TUdpBufferObj* pBufferObj);
	CONNID HandleAccept	(TUdpBufferObj* pBufferObj);
	void HandleSend		(CONNID dwConnID, TUdpBufferObj* pBufferObj);
	void HandleReceive	(CONNID dwConnID, TUdpBufferObj* pBufferObj);

	int SendInternal(CONNID dwConnID, const BYTE* pBuffer, int iLength);
	int SendPack	(TUdpSocketObj* pSocketObj, const BYTE* pBuffer, int iLength);
	int SendSafe	(TUdpSocketObj* pSocketObj, const BYTE* pBuffer, int iLength);
	int SendDirect	(TUdpSocketObj* pSocketObj, const BYTE* pBuffer, int iLength);
	int CatAndPost	(TUdpSocketObj* pSocketObj, const BYTE* pBuffer, int iLength);

	int DoReceive	(TUdpBufferObj* pBufferObj);

	int DoSend		(CONNID dwConnID);
	int DoSend		(TUdpSocketObj* pSocketObj);
	int DoSendPack	(TUdpSocketObj* pSocketObj);
	int DoSendSafe	(TUdpSocketObj* pSocketObj);
	int SendItem	(TUdpSocketObj* pSocketObj);

	void DetectConnections	();
	BOOL SendDetectPackage	(CONNID dwConnID, TUdpSocketObj* pSocketObj);
	BOOL NeedDetectorThread	() {return m_dwDetectAttempts > 0 && m_dwDetectInterval > 0;}

public:
	CUdpServer(IUdpServerListener* pListener)
	: m_pListener				(pListener)
	, m_hCompletePort			(nullptr)
	, m_soListen				(INVALID_SOCKET)
	, m_iRemainPostReceives		(0)
	, m_enLastError				(SE_OK)
	, m_enState					(SS_STOPPED)
	, m_usFamily				(AF_UNSPEC)
	, m_hDetector				(nullptr)
	, m_enSendPolicy			(SP_PACK)
	, m_dwMaxConnectionCount	(DEFAULT_MAX_CONNECTION_COUNT)
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
	{
		ASSERT(sm_wsSocket.IsValid());
		ASSERT(m_pListener);
	}

	virtual ~CUdpServer()
	{
		Stop();
	}

private:
	EnSendPolicy m_enSendPolicy;
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

private:
	static const CInitSocket sm_wsSocket;

	ADDRESS_FAMILY			m_usFamily;

private:
	IUdpServerListener*		m_pListener;
	SOCKET					m_soListen;
	HANDLE					m_hCompletePort;
	EnServiceState			m_enState;
	EnSocketError			m_enLastError;

	HANDLE					m_hDetector;
	vector<HANDLE>			m_vtWorkerThreads;

	CPrivateHeap			m_phSocket;
	CUdpBufferObjPool		m_bfObjPool;

	CSpinGuard				m_csState;

	CCriSec					m_csAccept;
	CEvt					m_evDetector;

	TUdpSocketObjPtrPool	m_bfActiveSockets;

	CSimpleRWLock			m_csClientSocket;
	TSockAddrMap			m_mpClientAddr;

	TUdpSocketObjPtrList	m_lsFreeSocket;
	TUdpSocketObjPtrQueue	m_lsGCSocket;

	volatile long			m_iRemainPostReceives;
};
