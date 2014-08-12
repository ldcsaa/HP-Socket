/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.2.3
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
#include "../../Common/Src/PrivateHeap.h"
#include "../../Common/Src/Event.h"
#include "../../Common/Src/RWLock.h"
#include "../../Common/Src/CriticalSection.h"
#include "../../Common/Src/STLHelper.h"

class CTcpAgent : public ITcpAgent
{
public:
	CTcpAgent(ITcpAgentListener* psoListener)
	: m_psoListener				(psoListener)
	, m_hCompletePort			(nullptr)
	, m_pfnConnectEx			(nullptr)
	, m_pfnDisconnectEx			(nullptr)
	, m_enLastError				(SE_OK)
	, m_enState					(SS_STOPED)
	, m_bAsyncConnect			(TRUE)
	, m_enSendPolicy			(SP_PACK)
	, m_enRecvPolicy			(RP_SERIAL)
	, m_dwWorkerThreadCount		(DEFAULT_WORKER_THREAD_COUNT)
	, m_dwSocketBufferSize		(DEFAULT_TCP_SOCKET_BUFFER_SIZE)
	, m_dwFreeSocketObjLockTime	(DEFAULT_FREE_SOCKETOBJ_LOCK_TIME)
	, m_dwFreeSocketObjPool		(DEFAULT_FREE_SOCKETOBJ_POOL)
	, m_dwFreeBufferObjPool		(DEFAULT_FREE_BUFFEROBJ_POOL)
	, m_dwFreeSocketObjHold		(DEFAULT_FREE_SOCKETOBJ_HOLD)
	, m_dwFreeBufferObjHold		(DEFAULT_FREE_BUFFEROBJ_HOLD)
	, m_dwKeepAliveTime			(DEFALUT_TCP_KEEPALIVE_TIME)
	, m_dwKeepAliveInterval		(DEFALUT_TCP_KEEPALIVE_INTERVAL)
	, m_dwMaxShutdownWaitTime	(DEFAULT_MAX_SHUTDOWN_WAIT_TIME)
	, m_bReuseAddress			(FALSE)
	{
		ASSERT(m_wsSocket.IsValid());
		ASSERT(m_psoListener);

		Reset();
	}

	virtual ~CTcpAgent()
	{
		if(HasStarted())
			Stop();
	}

public:
	virtual BOOL Start	(LPCTSTR pszBindAddress = nullptr, BOOL bAsyncConnect = TRUE);
	virtual BOOL Stop	();
	virtual BOOL Connect(LPCTSTR pszRemoteAddress, USHORT usPort, CONNID* pdwConnID = nullptr);
	virtual BOOL Send	(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset = 0);
	virtual BOOL SendPackets	(CONNID dwConnID, const WSABUF pBuffers[], int iCount);
	virtual BOOL SendSmallFile	(CONNID dwConnID, LPCTSTR lpszFileName, const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr);
	virtual BOOL			HasStarted					()	{return m_enState == SS_STARTED || m_enState == SS_STARTING;}
	virtual EnServiceState	GetState					()	{return m_enState;}
	virtual BOOL			Disconnect					(CONNID dwConnID, BOOL bForce = TRUE);
	virtual BOOL			DisconnectLongConnections	(DWORD dwPeriod, BOOL bForce = TRUE);
	virtual BOOL			GetLocalAddress				(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);
	virtual BOOL			GetRemoteAddress			(CONNID dwConnID, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);
	
	virtual BOOL GetPendingDataLength	(CONNID dwConnID, int& iPending);
	virtual DWORD GetConnectionCount	();
	virtual BOOL GetAllConnectionIDs	(CONNID pIDs[], DWORD& dwCount);
	virtual BOOL GetConnectPeriod		(CONNID dwConnID, DWORD& dwPeriod);
	virtual EnSocketError GetLastError	()	{return m_enLastError;}
	virtual LPCTSTR GetLastErrorDesc	()	{return ::GetSocketErrorDesc(m_enLastError);}

public:
	virtual BOOL SetConnectionExtra(CONNID dwConnID, PVOID pExtra);
	virtual BOOL GetConnectionExtra(CONNID dwConnID, PVOID* ppExtra);

	virtual void SetSendPolicy				(EnSendPolicy enSendPolicy)		{m_enSendPolicy				= enSendPolicy;}
	virtual void SetRecvPolicy				(EnRecvPolicy enRecvPolicy)		{m_enRecvPolicy				= enRecvPolicy;}
	virtual void SetWorkerThreadCount		(DWORD dwWorkerThreadCount)		{m_dwWorkerThreadCount		= dwWorkerThreadCount;}
	virtual void SetSocketBufferSize		(DWORD dwSocketBufferSize)		{m_dwSocketBufferSize		= dwSocketBufferSize;}
	virtual void SetFreeSocketObjLockTime	(DWORD dwFreeSocketObjLockTime)	{m_dwFreeSocketObjLockTime	= dwFreeSocketObjLockTime;}
	virtual void SetFreeSocketObjPool		(DWORD dwFreeSocketObjPool)		{m_dwFreeSocketObjPool		= dwFreeSocketObjPool;}
	virtual void SetFreeBufferObjPool		(DWORD dwFreeBufferObjPool)		{m_dwFreeBufferObjPool		= dwFreeBufferObjPool;}
	virtual void SetFreeSocketObjHold		(DWORD dwFreeSocketObjHold)		{m_dwFreeSocketObjHold		= dwFreeSocketObjHold;}
	virtual void SetFreeBufferObjHold		(DWORD dwFreeBufferObjHold)		{m_dwFreeBufferObjHold		= dwFreeBufferObjHold;}
	virtual void SetKeepAliveTime			(DWORD dwKeepAliveTime)			{m_dwKeepAliveTime			= dwKeepAliveTime;}
	virtual void SetKeepAliveInterval		(DWORD dwKeepAliveInterval)		{m_dwKeepAliveInterval		= dwKeepAliveInterval;}
	virtual void SetMaxShutdownWaitTime		(DWORD dwMaxShutdownWaitTime)	{m_dwMaxShutdownWaitTime	= dwMaxShutdownWaitTime;}
	virtual void SetReuseAddress			(BOOL bReuseAddress)			{m_bReuseAddress			= bReuseAddress;}

	virtual EnSendPolicy GetSendPolicy		()	{return m_enSendPolicy;}
	virtual EnRecvPolicy GetRecvPolicy		()	{return m_enRecvPolicy;}
	virtual DWORD GetWorkerThreadCount		()	{return m_dwWorkerThreadCount;}
	virtual DWORD GetSocketBufferSize		()	{return m_dwSocketBufferSize;}
	virtual DWORD GetFreeSocketObjLockTime	()	{return m_dwFreeSocketObjLockTime;}
	virtual DWORD GetFreeSocketObjPool		()	{return m_dwFreeSocketObjPool;}
	virtual DWORD GetFreeBufferObjPool		()	{return m_dwFreeBufferObjPool;}
	virtual DWORD GetFreeSocketObjHold		()	{return m_dwFreeSocketObjHold;}
	virtual DWORD GetFreeBufferObjHold		()	{return m_dwFreeBufferObjHold;}
	virtual DWORD GetKeepAliveTime			()	{return m_dwKeepAliveTime;}
	virtual DWORD GetKeepAliveInterval		()	{return m_dwKeepAliveInterval;}
	virtual DWORD GetMaxShutdownWaitTime	()	{return m_dwMaxShutdownWaitTime;}
	virtual BOOL  IsReuseAddress			()	{return m_bReuseAddress;}

protected:
	virtual EnHandleResult FireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength);
	virtual EnHandleResult FireReceive(TSocketObj* pSocketObj, int iLength);
	virtual EnHandleResult FireClose(TSocketObj* pSocketObj);
	virtual EnHandleResult FireError(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode);

	virtual EnHandleResult FirePrepareConnect(CONNID dwConnID, SOCKET socket)
		{return m_psoListener->OnPrepareConnect(dwConnID, socket);}
	virtual EnHandleResult FireConnect(CONNID dwConnID)
		{return m_psoListener->OnConnect(dwConnID);}
	virtual EnHandleResult FireSend(CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_psoListener->OnSend(dwConnID, pData, iLength);}
	virtual EnHandleResult FireAgentShutdown()
		{return m_psoListener->OnAgentShutdown();}

	virtual BOOL CheckParams();

private:
	BOOL CheckStarting();
	BOOL CheckStoping();
	BOOL ParseBindAddress(LPCTSTR pszBindAddress, BOOL bAsyncConnect);
	BOOL CreateCompletePort();
	BOOL CreateWorkerThreads();

	void DisconnectClientSocket();
	void WaitForClientSocketClose();
	void ReleaseClientSocket();
	void ReleaseFreeSocket();
	void ReleaseFreeBuffer();
	void WaitForWorkerThreadEnd();
	void CloseCompletePort();

	void Reset();

	void		CompressFreeBuffer(size_t size);
	void		CompressFreeSocket(size_t size, BOOL bForce = FALSE);
	TBufferObj*	GetFreeBufferObj(int iLen = 0);
	TSocketObj*	GetFreeSocketObj(CONNID dwConnID, SOCKET soClient);
	void		AddFreeBufferObj(TBufferObj* pBufferObj);
	void		AddFreeSocketObj(TSocketObj* pSocketObj, EnSocketCloseFlag enFlag = SCF_NONE, EnSocketOperation enOperation = SO_UNKNOWN, int iErrorCode = 0);
	TBufferObj*	CreateBufferObj();
	TSocketObj*	CreateSocketObj();
	void		DeleteBufferObj(TBufferObj* pBufferObj);
	void		DeleteSocketObj(TSocketObj* pSocketObj);
	BOOL		InvalidSocketObj(TSocketObj* pSocketObj);

	void		AddClientSocketObj(CONNID dwConnID, TSocketObj* pSocketObj);
	void		CloseClientSocketObj(TSocketObj* pSocketObj, EnSocketCloseFlag enFlag = SCF_NONE, EnSocketOperation enOperation = SO_UNKNOWN, int iErrorCode = 0, int iShutdownFlag = SD_SEND);
	TSocketObj* FindSocketObj(CONNID dwConnID);

private:
	void SetLastError(EnSocketError code, LPCSTR func, int ec);

private:
	static UINT WINAPI WorkerThreadProc(LPVOID pv);

	EnIocpAction CheckIocpCommand(OVERLAPPED* pOverlapped, DWORD dwBytes, ULONG_PTR ulCompKey);

	DWORD CreateClientSocket(SOCKET& soClient);
	DWORD ConnectToServer	(CONNID dwConnID, SOCKET& soClient, LPCTSTR pszRemoteAddress, USHORT usPort);
	void ForceDisconnect	(CONNID dwConnID);

	void HandleIo			(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj, DWORD dwBytes, DWORD dwErrorCode);
	void HandleError		(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj, DWORD dwErrorCode);
	void HandleConnect		(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj);
	void HandleSend			(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj);
	void HandleReceive		(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj);

	void TriggerFireSend	(CONNID dwConnID, TBufferObj* pBufferObj);

	int SendInternal(TSocketObj* pSocketObj, const WSABUF pBuffers[], int iCount);
	int SendPack	(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength);
	int SendSafe	(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength);
	int SendDirect	(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength);
	int CatAndPost	(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength, BOOL isPostSend);

	int DoConnect	(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj);
	int DoReceive	(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj);

	int DoSend		(CONNID dwConnID);
	int DoSend		(TSocketObj* pSocketObj);
	int DoSendPack	(TSocketObj* pSocketObj);
	int DoSendSafe	(TSocketObj* pSocketObj);
	int SendItem	(TSocketObj* pSocketObj);

	void CheckError	(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode);

private:
	EnSendPolicy m_enSendPolicy;
	EnRecvPolicy m_enRecvPolicy;
	DWORD m_dwWorkerThreadCount;
	DWORD m_dwSocketBufferSize;
	DWORD m_dwFreeSocketObjLockTime;
	DWORD m_dwFreeSocketObjPool;
	DWORD m_dwFreeBufferObjPool;
	DWORD m_dwFreeSocketObjHold;
	DWORD m_dwFreeBufferObjHold;
	DWORD m_dwKeepAliveTime;
	DWORD m_dwKeepAliveInterval;
	DWORD m_dwMaxShutdownWaitTime;
	BOOL  m_bReuseAddress;

private:
	CInitSocket			m_wsSocket;
	LPFN_CONNECTEX		m_pfnConnectEx;
	LPFN_DISCONNECTEX	m_pfnDisconnectEx;

private:
	ITcpAgentListener*	m_psoListener;
	BOOL				m_bAsyncConnect;
	HANDLE				m_hCompletePort;
	EnServiceState		m_enState;
	EnSocketError		m_enLastError;
	SOCKADDR_IN			m_soAddrIN;
	vector<HANDLE>		m_vtWorkerThreads;

	CPrivateHeap		m_phBuffer;
	CPrivateHeap		m_phSocket;
	CItemPool			m_itPool;

	CRWLock				m_csClientSocket;
	TSocketObjPtrMap	m_mpClientSocket;

	CCriSec				m_csFreeBuffer;
	TBufferObjPtrList	m_lsFreeBuffer;

	CCriSec				m_csFreeSocket;
	TSocketObjPtrList	m_lsFreeSocket;
};
