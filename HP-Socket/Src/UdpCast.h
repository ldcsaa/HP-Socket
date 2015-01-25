/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.3.1
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

class CUdpCast : public IUdpCast
{
public:
	virtual BOOL Start	(LPCTSTR pszRemoteAddress, USHORT usPort, BOOL bAsyncConnect = FALSE);
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
	virtual void SetFreeBufferPoolSize	(DWORD dwFreeBufferPoolSize)	{m_dwFreeBufferPoolSize	= dwFreeBufferPoolSize;}
	virtual void SetFreeBufferPoolHold	(DWORD dwFreeBufferPoolHold)	{m_dwFreeBufferPoolHold	= dwFreeBufferPoolHold;}
	virtual void SetReuseAddress		(BOOL bReuseAddress)			{m_bReuseAddress		= bReuseAddress;}
	virtual void SetBindAdddress		(LPCTSTR pszBindAddress)		{m_strBindAddress		= pszBindAddress;}
	virtual void SetCastMode			(EnCastMode enCastMode)			{m_enCastMode			= enCastMode;}
	virtual void SetMultiCastTtl		(int iMCTtl)					{m_iMCTtl				= iMCTtl;}
	virtual void SetMultiCastLoop		(BOOL bMCLoop)					{m_bMCLoop				= bMCLoop;}
	virtual void SetExtra				(PVOID pExtra)					{m_pExtra				= pExtra;}						

	virtual DWORD GetMaxDatagramSize	()	{return m_dwMaxDatagramSize;}
	virtual DWORD GetFreeBufferPoolSize	()	{return m_dwFreeBufferPoolSize;}
	virtual DWORD GetFreeBufferPoolHold	()	{return m_dwFreeBufferPoolHold;}
	virtual BOOL  IsReuseAddress		()	{return m_bReuseAddress;}
	virtual LPCTSTR GetBindAdddress		()	{return (LPCTSTR)m_strBindAddress;}
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
	virtual EnHandleResult FireClose(IClient* pClient)
		{return m_psoListener->OnClose(pClient);}
	virtual EnHandleResult FireError(IClient* pClient, EnSocketOperation enOperation, int iErrorCode)
		{return m_psoListener->OnError(pClient, enOperation, iErrorCode);}

	virtual BOOL CheckParams();
	virtual void Reset(BOOL bAll = TRUE);

private:
	BOOL CheckStarting();
	BOOL CheckStoping();
	BOOL CreateClientSocket();
	BOOL ConnectToGroup(LPCTSTR pszRemoteAddress, USHORT usPort);
	BOOL CreateWorkerThread();
	BOOL CreateDetectorThread();
	BOOL ProcessNetworkEvent();
	BOOL ReadData();
	BOOL SendData();
	TItem* GetSendBuffer();
	int SendInternal(const BYTE* pBuffer, int iLength, EnSocketError& enCode);
	void WaitForWorkerThreadEnd(DWORD dwCurrentThreadID);

	BOOL HandleError();
	BOOL HandleRead(WSANETWORKEVENTS& events);
	BOOL HandleWrite(WSANETWORKEVENTS& events);
	BOOL HandleConnect(WSANETWORKEVENTS& events);
	BOOL HandleClosse(WSANETWORKEVENTS& events);

	void SetLastError(EnSocketError code, LPCSTR func, int ec);

	static 
#ifndef _WIN32_WCE
	UINT
#else
	DWORD
#endif
	 WINAPI WorkerThreadProc(LPVOID pv);

public:
	CUdpCast(IUdpCastListener* psoListener)
	: m_psoListener			(psoListener)
	, m_lsSend				(m_itPool)
	, m_soClient			(INVALID_SOCKET)
	, m_evSocket			(nullptr)
	, m_dwConnID			(0)
	, m_hWorker				(nullptr)
	, m_dwWorkerID			(0)
	, m_iPending			(0)
	, m_enState				(SS_STOPPED)
	, m_enLastError			(SE_OK)
	, m_pExtra				(nullptr)
	, m_dwMaxDatagramSize	(DEFAULT_UDP_MAX_DATAGRAM_SIZE)
	, m_dwFreeBufferPoolSize(DEFAULT_CLIENT_FREE_BUFFER_POOL_SIZE)
	, m_dwFreeBufferPoolHold(DEFAULT_CLIENT_FREE_BUFFER_POOL_HOLD)
	, m_bReuseAddress		(FALSE)
	, m_iMCTtl				(1)
	, m_bMCLoop				(FALSE)
	, m_enCastMode			(CM_MULTICAST)
	, m_strBindAddress		(DEFAULT_BIND_ADDRESS)
	{
		ASSERT(m_psoListener);
		Reset(FALSE);
	}

	virtual ~CUdpCast()	{if(HasStarted()) Stop();}

private:
	CInitSocket			m_wsSocket;

private:
	IUdpCastListener*	m_psoListener;

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
	CString				m_strBindAddress;

	HANDLE				m_hWorker;

#ifndef _WIN32_WCE
	UINT				m_dwWorkerID;
#else
	DWORD				m_dwWorkerID;
#endif

	EnServiceState		m_enState;
	EnSocketError		m_enLastError;

	PVOID				m_pExtra;

	SOCKADDR_IN			m_castAddr;
	SOCKADDR_IN			m_remoteAddr;

	CBufferPtr			m_rcBuffer;

protected:
	CItemPool			m_itPool;

	CSpinGuard			m_csState;

private:
	CCriSec				m_csSend;
	TItemList			m_lsSend;

	CEvt				m_evBuffer;
	CEvt				m_evWorker;
	CEvt				m_evDetector;

	volatile int		m_iPending;
};
