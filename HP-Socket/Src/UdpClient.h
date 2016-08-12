/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.5.3
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
#include "../../Common/Src/bufferptr.h"
#include "../../Common/Src/bufferpool.h"
#include "../../Common/Src/CriticalSection.h"

class CUdpClient : public IUdpClient
{
public:
	virtual BOOL Start	(LPCTSTR lpszRemoteAddress, USHORT usPort, BOOL bAsyncConnect = TRUE, LPCTSTR lpszBindAddress = nullptr);
	virtual BOOL Stop	();
	virtual BOOL Send	(const BYTE* pBuffer, int iLength, int iOffset = 0);
	virtual BOOL			SendPackets			(const WSABUF pBuffers[], int iCount);
	virtual BOOL			HasStarted			()	{return m_enState == SS_STARTED || m_enState == SS_STARTING;}
	virtual EnServiceState	GetState			()	{return m_enState;}
	virtual CONNID			GetConnectionID		()	{return m_dwConnID;};
	virtual BOOL			GetLocalAddress		(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);
	virtual BOOL GetPendingDataLength			(int& iPending) {iPending = m_iPending; return HasStarted();}
	virtual EnSocketError	GetLastError		()	{return m_enLastError;}
	virtual LPCTSTR			GetLastErrorDesc	()	{return ::GetSocketErrorDesc(m_enLastError);}

public:
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
	virtual EnHandleResult FirePrepareConnect(IClient* pClient, SOCKET socket)
		{return m_psoListener->OnPrepareConnect(pClient, socket);}
	virtual EnHandleResult FireConnect(IClient* pClient)
		{return m_psoListener->OnConnect(pClient);}
	virtual EnHandleResult FireSend(IClient* pClient, const BYTE* pData, int iLength)
		{return m_psoListener->OnSend(pClient, pData, iLength);}
	virtual EnHandleResult FireReceive(IClient* pClient, const BYTE* pData, int iLength)
		{return m_psoListener->OnReceive(pClient, pData, iLength);}
	virtual EnHandleResult FireReceive(IClient* pClient, int iLength)
		{return m_psoListener->OnReceive(pClient, iLength);}
	virtual EnHandleResult FireClose(IClient* pClient, EnSocketOperation enOperation, int iErrorCode)
		{return m_psoListener->OnClose(pClient, enOperation, iErrorCode);}

	void SetLastError(EnSocketError code, LPCSTR func, int ec);
	virtual BOOL CheckParams();
	virtual void PrepareStart();
	virtual void Reset();

	virtual void OnWorkerThreadEnd(DWORD dwThreadID) {}

protected:
	void SetReserved	(PVOID pReserved)	{m_pReserved = pReserved;}						
	PVOID GetReserved	()					{return m_pReserved;}

private:
	BOOL CheckStarting();
	BOOL CheckStoping(DWORD dwCurrentThreadID);
	BOOL CreateClientSocket();
	BOOL BindClientSocket(LPCTSTR lpszBindAddress);
	BOOL ConnectToServer(LPCTSTR lpszRemoteAddress, USHORT usPort);
	BOOL CreateWorkerThread();
	BOOL CreateDetectorThread();
	BOOL ProcessNetworkEvent();
	BOOL ReadData();
	BOOL SendData();
	TItem* GetSendBuffer();
	int SendInternal(const BYTE* pBuffer, int iLength);
	void WaitForWorkerThreadEnd(DWORD dwCurrentThreadID);
	void WaitForDetectorThreadEnd(DWORD dwCurrentThreadID);

	BOOL HandleError(WSANETWORKEVENTS& events);
	BOOL HandleRead(WSANETWORKEVENTS& events);
	BOOL HandleWrite(WSANETWORKEVENTS& events);
	BOOL HandleConnect(WSANETWORKEVENTS& events);
	BOOL HandleClose(WSANETWORKEVENTS& events);

	int DetectConnection	();
	BOOL NeedDetectorThread	() {return m_dwDetectAttempts > 0 && m_dwDetectInterval > 0;}

	static UINT WINAPI WorkerThreadProc(LPVOID pv);
	static UINT WINAPI DetecotrThreadProc(LPVOID pv);

public:
	CUdpClient(IUdpClientListener* psoListener)
	: m_psoListener			(psoListener)
	, m_lsSend				(m_itPool)
	, m_soClient			(INVALID_SOCKET)
	, m_evSocket			(nullptr)
	, m_dwConnID			(0)
	, m_hWorker				(nullptr)
	, m_dwWorkerID			(0)
	, m_hDetector			(nullptr)
	, m_dwDetectorID		(0)
	, m_bAsyncConnect		(FALSE)
	, m_iPending			(0)
	, m_enState				(SS_STOPPED)
	, m_enLastError			(SE_OK)
	, m_dwDetectFails		(0)
	, m_pExtra				(nullptr)
	, m_pReserved			(nullptr)
	, m_dwMaxDatagramSize	(DEFAULT_UDP_MAX_DATAGRAM_SIZE)
	, m_dwFreeBufferPoolSize(DEFAULT_CLIENT_FREE_BUFFER_POOL_SIZE)
	, m_dwFreeBufferPoolHold(DEFAULT_CLIENT_FREE_BUFFER_POOL_HOLD)
	, m_dwDetectAttempts	(DEFAULT_UDP_DETECT_ATTEMPTS)
	, m_dwDetectInterval	(DEFAULT_UDP_DETECT_INTERVAL)
	{
		ASSERT(m_psoListener);
	}

	virtual ~CUdpClient()
	{
		Stop();
	}

private:
	CInitSocket			m_wsSocket;

private:
	IUdpClientListener*	m_psoListener;
	TClientCloseContext m_ccContext;

	BOOL				m_bAsyncConnect;
	SOCKET				m_soClient;
	HANDLE				m_evSocket;
	CONNID				m_dwConnID;

	DWORD				m_dwMaxDatagramSize;
	DWORD				m_dwFreeBufferPoolSize;
	DWORD				m_dwFreeBufferPoolHold;
	DWORD				m_dwDetectAttempts;
	DWORD				m_dwDetectInterval;

	HANDLE				m_hWorker;
	HANDLE				m_hDetector;
	UINT				m_dwWorkerID;
	UINT				m_dwDetectorID;

	volatile EnServiceState	m_enState;
	EnSocketError		m_enLastError;

	PVOID				m_pExtra;
	PVOID				m_pReserved;

	CBufferPtr			m_rcBuffer;

protected:
	CItemPool			m_itPool;

private:
	CSpinGuard			m_csState;

	CCriSec				m_csSend;
	TItemList			m_lsSend;

	CEvt				m_evBuffer;
	CEvt				m_evWorker;
	CEvt				m_evDetector;

	volatile int		m_iPending;
	volatile DWORD		m_dwDetectFails;
};
