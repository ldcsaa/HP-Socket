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

class CUdpClient : public IUdpClient
{
public:
	virtual BOOL Start	(LPCTSTR lpszRemoteAddress, USHORT usPort, BOOL bAsyncConnect = TRUE, LPCTSTR lpszBindAddress = nullptr, USHORT usLocalPort = 0);
	virtual BOOL Stop	();
	virtual BOOL Send	(const BYTE* pBuffer, int iLength, int iOffset = 0);
	virtual BOOL SendPackets	(const WSABUF pBuffers[], int iCount);
	virtual BOOL PauseReceive	(BOOL bPause = TRUE);
	virtual BOOL			HasStarted			()	{return m_enState == SS_STARTED || m_enState == SS_STARTING;}
	virtual EnServiceState	GetState			()	{return m_enState;}
	virtual CONNID			GetConnectionID		()	{return m_dwConnID;};
	virtual EnSocketError	GetLastError		()	{return m_enLastError;}
	virtual LPCTSTR			GetLastErrorDesc	()	{return ::GetSocketErrorDesc(m_enLastError);}

	virtual BOOL GetLocalAddress		(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);
	virtual BOOL GetRemoteHost			(TCHAR lpszHost[], int& iHostLen, USHORT& usPort);
	virtual BOOL GetPendingDataLength	(int& iPending) {iPending = m_lsSend.Length(); return HasStarted();}
	virtual BOOL IsPauseReceive			(BOOL& bPaused) {bPaused = m_bPaused; return HasStarted();}
	virtual BOOL IsConnected			()				{return m_bConnected;}

public:
	virtual BOOL IsSecure				() {return FALSE;}

	virtual void SetMaxDatagramSize		(DWORD dwMaxDatagramSize)		{m_dwMaxDatagramSize	= dwMaxDatagramSize;}
	virtual void SetDetectAttempts		(DWORD dwDetectAttempts)		{m_dwDetectAttempts		= dwDetectAttempts;}
	virtual void SetDetectInterval		(DWORD dwDetectInterval)		{m_dwDetectInterval		= dwDetectInterval;}
	virtual void SetFreeBufferPoolSize	(DWORD dwFreeBufferPoolSize)	{m_dwFreeBufferPoolSize = dwFreeBufferPoolSize;}
	virtual void SetFreeBufferPoolHold	(DWORD dwFreeBufferPoolHold)	{m_dwFreeBufferPoolHold = dwFreeBufferPoolHold;}
	virtual void SetExtra				(PVOID pExtra)					{m_pExtra				= pExtra;}						

	virtual DWORD GetMaxDatagramSize	()	{return m_dwMaxDatagramSize;}
	virtual DWORD GetDetectAttempts		()	{return m_dwDetectAttempts;}
	virtual DWORD GetDetectInterval		()	{return m_dwDetectInterval;}
	virtual DWORD GetFreeBufferPoolSize	()	{return m_dwFreeBufferPoolSize;}
	virtual DWORD GetFreeBufferPoolHold	()	{return m_dwFreeBufferPoolHold;}
	virtual PVOID GetExtra				()	{return m_pExtra;}

protected:
	virtual EnHandleResult FirePrepareConnect(SOCKET socket)
		{return m_pListener->OnPrepareConnect(this, m_dwConnID, socket);}
	virtual EnHandleResult FireConnect()
		{
			EnHandleResult rs		= m_pListener->OnConnect(this, m_dwConnID);
			if(rs != HR_ERROR) rs	= FireHandShake();
			return rs;
		}
	virtual EnHandleResult FireHandShake()
		{return m_pListener->OnHandShake(this, m_dwConnID);}
	virtual EnHandleResult FireSend(const BYTE* pData, int iLength)
		{return m_pListener->OnSend(this, m_dwConnID, pData, iLength);}
	virtual EnHandleResult FireReceive(const BYTE* pData, int iLength)
		{return m_pListener->OnReceive(this, m_dwConnID, pData, iLength);}
	virtual EnHandleResult FireReceive(int iLength)
		{return m_pListener->OnReceive(this, m_dwConnID, iLength);}
	virtual EnHandleResult FireClose(EnSocketOperation enOperation, int iErrorCode)
		{return m_pListener->OnClose(this, m_dwConnID, enOperation, iErrorCode);}

	void SetLastError(EnSocketError code, LPCSTR func, int ec);
	virtual BOOL CheckParams();
	virtual void PrepareStart();
	virtual void Reset();

	virtual void OnWorkerThreadEnd(THR_ID tid) {}

protected:
	void SetReserved	(PVOID pReserved)	{m_pReserved = pReserved;}						
	PVOID GetReserved	()					{return m_pReserved;}
	BOOL GetRemoteHost	(LPCSTR* lpszHost, USHORT* pusPort = nullptr);

private:
	void SetRemoteHost	(LPCTSTR lpszHost, USHORT usPort);
	void SetConnected	(BOOL bConnected = TRUE) {m_bConnected = bConnected; if(bConnected) m_enState = SS_STARTED;}

	BOOL CheckStarting();
	BOOL CheckStoping();
	BOOL CreateClientSocket(LPCTSTR lpszRemoteAddress, HP_SOCKADDR& addrRemote, USHORT usPort, LPCTSTR lpszBindAddress, HP_SOCKADDR& addrBind);
	BOOL BindClientSocket(const HP_SOCKADDR& addrBind, const HP_SOCKADDR& addrRemote, USHORT usLocalPort);
	BOOL ConnectToServer(const HP_SOCKADDR& addrRemote, BOOL bAsyncConnect);
	BOOL CreateWorkerThread();
	BOOL ProcessNetworkEvent(SHORT events);
	BOOL ReadData();
	BOOL SendData();
	BOOL DoSendData(TItem* pItem);
	int SendInternal(TItemPtr& itPtr);
	void WaitForWorkerThreadEnd();

	BOOL HandleConnect	(SHORT events);
	BOOL HandleClose	(SHORT events);
	BOOL HandleRead		(SHORT events);
	BOOL HandleWrite	(SHORT events);

	BOOL CheckConnection	();
	BOOL DetectConnection	();
	BOOL IsNeedDetect		() {return m_dwDetectAttempts > 0 && m_dwDetectInterval > 0;}

	UINT WINAPI WorkerThreadProc(LPVOID pv);

public:
	CUdpClient(IUdpClientListener* pListener)
	: m_pListener			(pListener)
	, m_lsSend				(m_itPool)
	, m_soClient			(INVALID_SOCKET)
	, m_nEvents				(0)
	, m_dwConnID			(0)
	, m_usPort				(0)
	, m_bPaused				(FALSE)
	, m_bConnected			(FALSE)
	, m_enLastError			(SE_OK)
	, m_enState				(SS_STOPPED)
	, m_dwDetectFails		(0)
	, m_pExtra				(nullptr)
	, m_pReserved			(nullptr)
	, m_dwMaxDatagramSize	(DEFAULT_UDP_MAX_DATAGRAM_SIZE)
	, m_dwFreeBufferPoolSize(DEFAULT_CLIENT_FREE_BUFFER_POOL_SIZE)
	, m_dwFreeBufferPoolHold(DEFAULT_CLIENT_FREE_BUFFER_POOL_HOLD)
	, m_dwDetectAttempts	(DEFAULT_UDP_DETECT_ATTEMPTS)
	, m_dwDetectInterval	(DEFAULT_UDP_DETECT_INTERVAL)
	{
		ASSERT(m_pListener);
	}

	virtual ~CUdpClient()
	{
		Stop();
	}

private:
	IUdpClientListener*	m_pListener;
	TClientCloseContext m_ccContext;

	SOCKET				m_soClient;
	SHORT				m_nEvents;
	CONNID				m_dwConnID;

	DWORD				m_dwMaxDatagramSize;
	DWORD				m_dwFreeBufferPoolSize;
	DWORD				m_dwFreeBufferPoolHold;
	DWORD				m_dwDetectAttempts;
	DWORD				m_dwDetectInterval;

	EnSocketError		m_enLastError;
	volatile BOOL		m_bConnected;
	volatile EnServiceState	m_enState;

	PVOID				m_pExtra;
	PVOID				m_pReserved;

	CBufferPtr			m_rcBuffer;

protected:
	CStringA			m_strHost;
	USHORT				m_usPort;

	CItemPool			m_itPool;

private:
	CSpinGuard			m_csState;

	CCriSec				m_csSend;
	TItemListExV		m_lsSend;

	CEvt				m_evSend;
	CEvt				m_evRecv;
	CEvt				m_evStop;
	CTimerEvent			m_evDetect;

	DWORD				m_dwDetectFails;
	volatile BOOL		m_bPaused;

	CThread<CUdpClient, VOID, UINT> m_thWorker;
};
