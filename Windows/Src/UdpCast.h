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
#include "../Common/Src/BufferPtr.h"
#include "../Common/Src/BufferPool.h"
#include "../Common/Src/CriticalSection.h"

class CUdpCast : public IUdpCast
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
	virtual BOOL GetPendingDataLength	(int& iPending) {iPending = m_iPending; return HasStarted();}
	virtual BOOL IsPauseReceive			(BOOL& bPaused) {bPaused = m_bPaused; return HasStarted();}
	virtual BOOL IsConnected			()				{return m_bConnected;}

public:
	virtual BOOL IsSecure				() {return FALSE;}

	virtual void SetMaxDatagramSize		(DWORD dwMaxDatagramSize)		{m_dwMaxDatagramSize	= dwMaxDatagramSize;}
	virtual void SetFreeBufferPoolSize	(DWORD dwFreeBufferPoolSize)	{m_dwFreeBufferPoolSize	= dwFreeBufferPoolSize;}
	virtual void SetFreeBufferPoolHold	(DWORD dwFreeBufferPoolHold)	{m_dwFreeBufferPoolHold	= dwFreeBufferPoolHold;}
	virtual void SetReuseAddress		(BOOL bReuseAddress)			{m_bReuseAddress		= bReuseAddress;}
	virtual void SetCastMode			(EnCastMode enCastMode)			{m_enCastMode			= enCastMode;}
	virtual void SetMultiCastTtl		(int iMCTtl)					{m_iMCTtl				= iMCTtl;}
	virtual void SetMultiCastLoop		(BOOL bMCLoop)					{m_bMCLoop				= bMCLoop;}
	virtual void SetExtra				(PVOID pExtra)					{m_pExtra				= pExtra;}						

	virtual DWORD GetMaxDatagramSize	()	{return m_dwMaxDatagramSize;}
	virtual DWORD GetFreeBufferPoolSize	()	{return m_dwFreeBufferPoolSize;}
	virtual DWORD GetFreeBufferPoolHold	()	{return m_dwFreeBufferPoolHold;}
	virtual BOOL  IsReuseAddress		()	{return m_bReuseAddress;}
	virtual EnCastMode GetCastMode		()	{return m_enCastMode;}
	virtual int GetMultiCastTtl			()	{return m_iMCTtl;}
	virtual BOOL IsMultiCastLoop		()	{return m_bMCLoop;}
	virtual PVOID GetExtra				()	{return m_pExtra;}

	virtual BOOL GetRemoteAddress(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort)
	{
		ADDRESS_FAMILY usFamily;
		return ::sockaddr_IN_2_A(m_remoteAddr, usFamily, lpszAddress, iAddressLen, usPort);
	}

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

	virtual void OnWorkerThreadEnd(DWORD dwThreadID) {}

protected:
	void SetReserved	(PVOID pReserved)	{m_pReserved = pReserved;}						
	PVOID GetReserved	()					{return m_pReserved;}
	BOOL GetRemoteHost	(LPCSTR* lpszHost, USHORT* pusPort = nullptr);

private:
	void SetRemoteHost	(LPCTSTR lpszHost, USHORT usPort);
	void SetConnected	(BOOL bConnected = TRUE) {m_bConnected = bConnected; if(bConnected) m_enState = SS_STARTED;}

	BOOL CheckStarting();
	BOOL CheckStoping(DWORD dwCurrentThreadID);
	BOOL CreateClientSocket(LPCTSTR lpszRemoteAddress, USHORT usPort, LPCTSTR lpszBindAddress, HP_SOCKADDR& bindAddr);
	BOOL BindClientSocket(HP_SOCKADDR& bindAddr);
	BOOL ConnectToGroup(const HP_SOCKADDR& bindAddr);
	BOOL SetMultiCastSocketOptions(const HP_SOCKADDR& bindAddr);
	BOOL CreateWorkerThread();
	BOOL ProcessNetworkEvent();
	BOOL ReadData();
	BOOL SendData();
	TItem* GetSendBuffer();
	int SendInternal(const BYTE* pBuffer, int iLength);
	void WaitForWorkerThreadEnd(DWORD dwCurrentThreadID);

	BOOL HandleError(WSANETWORKEVENTS& events);
	BOOL HandleRead(WSANETWORKEVENTS& events);
	BOOL HandleWrite(WSANETWORKEVENTS& events);
	BOOL HandleConnect(WSANETWORKEVENTS& events);
	BOOL HandleClose(WSANETWORKEVENTS& events);

	static UINT WINAPI WorkerThreadProc(LPVOID pv);

public:
	CUdpCast(IUdpCastListener* pListener)
	: m_pListener			(pListener)
	, m_lsSend				(m_itPool)
	, m_soClient			(INVALID_SOCKET)
	, m_evSocket			(nullptr)
	, m_dwConnID			(0)
	, m_usPort				(0)
	, m_hWorker				(nullptr)
	, m_dwWorkerID			(0)
	, m_bPaused				(FALSE)
	, m_iPending			(0)
	, m_bConnected			(FALSE)
	, m_enLastError			(SE_OK)
	, m_enState				(SS_STOPPED)
	, m_pExtra				(nullptr)
	, m_pReserved			(nullptr)
	, m_dwMaxDatagramSize	(DEFAULT_UDP_MAX_DATAGRAM_SIZE)
	, m_dwFreeBufferPoolSize(DEFAULT_CLIENT_FREE_BUFFER_POOL_SIZE)
	, m_dwFreeBufferPoolHold(DEFAULT_CLIENT_FREE_BUFFER_POOL_HOLD)
	, m_bReuseAddress		(FALSE)
	, m_iMCTtl				(1)
	, m_bMCLoop				(FALSE)
	, m_enCastMode			(CM_MULTICAST)
	, m_castAddr			(AF_UNSPEC, TRUE)
	, m_remoteAddr			(AF_UNSPEC, TRUE)
	{
		ASSERT(sm_wsSocket.IsValid());
		ASSERT(m_pListener);
	}

	virtual ~CUdpCast()
	{
		Stop();
	}

private:
	static const CInitSocket sm_wsSocket;

private:
	IUdpCastListener*	m_pListener;
	TClientCloseContext m_ccContext;

	SOCKET				m_soClient;
	HANDLE				m_evSocket;
	CONNID				m_dwConnID;

	BOOL				m_bReuseAddress;
	DWORD				m_dwMaxDatagramSize;
	DWORD				m_dwFreeBufferPoolSize;
	DWORD				m_dwFreeBufferPoolHold;

	int					m_iMCTtl;
	BOOL				m_bMCLoop;
	EnCastMode			m_enCastMode;

	HANDLE				m_hWorker;
	UINT				m_dwWorkerID;

	EnSocketError		m_enLastError;
	volatile BOOL		m_bConnected;
	volatile EnServiceState	m_enState;

	PVOID				m_pExtra;
	PVOID				m_pReserved;

	HP_SOCKADDR			m_castAddr;
	HP_SOCKADDR			m_remoteAddr;

	CBufferPtr			m_rcBuffer;

protected:
	CStringA			m_strHost;
	USHORT				m_usPort;

	CItemPool			m_itPool;

private:
	CSpinGuard			m_csState;

	CCriSec				m_csSend;
	TItemList			m_lsSend;

	CEvt				m_evBuffer;
	CEvt				m_evWorker;

	volatile int		m_iPending;
	volatile BOOL		m_bPaused;
};
