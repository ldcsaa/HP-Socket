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

class CTcpClient : public ITcpClient
{
public:
	virtual BOOL Start	(LPCTSTR pszRemoteAddress, USHORT usPort, BOOL bAsyncConnect = FALSE);
	virtual BOOL Stop	();
	virtual BOOL Send	(const BYTE* pBuffer, int iLength, int iOffset = 0);
	virtual BOOL SendPackets	(const WSABUF pBuffers[], int iCount);
	virtual BOOL SendSmallFile	(LPCTSTR lpszFileName, const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr);
	virtual BOOL			HasStarted			()	{return m_enState == SS_STARTED || m_enState == SS_STARTING;}
	virtual EnServiceState	GetState			()	{return m_enState;}
	virtual CONNID			GetConnectionID		()	{return m_dwConnID;};
	virtual BOOL			GetLocalAddress		(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);
	virtual BOOL GetPendingDataLength			(int& iPending) {iPending = m_iPending; return HasStarted();}
	virtual EnSocketError	GetLastError		()	{return m_enLastError;}
	virtual LPCTSTR			GetLastErrorDesc	()	{return ::GetSocketErrorDesc(m_enLastError);}

public:
	virtual void SetSocketBufferSize	(DWORD dwSocketBufferSize)		{m_dwSocketBufferSize	= dwSocketBufferSize;}
	virtual void SetKeepAliveTime		(DWORD dwKeepAliveTime)			{m_dwKeepAliveTime		= dwKeepAliveTime;}
	virtual void SetKeepAliveInterval	(DWORD dwKeepAliveInterval)		{m_dwKeepAliveInterval	= dwKeepAliveInterval;}
	virtual void SetFreeBufferPoolSize	(DWORD dwFreeBufferPoolSize)	{m_dwFreeBufferPoolSize = dwFreeBufferPoolSize;}
	virtual void SetFreeBufferPoolHold	(DWORD dwFreeBufferPoolHold)	{m_dwFreeBufferPoolHold = dwFreeBufferPoolHold;}
	virtual void SetExtra				(PVOID pExtra)					{m_pExtra				= pExtra;}						

	virtual DWORD GetSocketBufferSize	()	{return m_dwSocketBufferSize;}
	virtual DWORD GetKeepAliveTime		()	{return m_dwKeepAliveTime;}
	virtual DWORD GetKeepAliveInterval	()	{return m_dwKeepAliveInterval;}
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
	BOOL ConnectToServer(LPCTSTR pszRemoteAddress, USHORT usPort);
	BOOL CreateWorkerThread();
	BOOL ProcessNetworkEvent();
	BOOL ReadData();
	BOOL SendData();
	BOOL DoSendData(TItem* pItem);
	TItem* GetSendBuffer();
	BOOL SendInternal(const WSABUF pBuffers[], int iCount, EnSocketError& enCode);
	void WaitForWorkerThreadEnd(DWORD dwCurrentThreadID);

	void SetLastError(EnSocketError code, LPCSTR func, int ec);

	static 
#ifndef _WIN32_WCE
	UINT
#else
	DWORD
#endif
	 WINAPI WorkerThreadProc(LPVOID pv);

public:
	CTcpClient(ITcpClientListener* psoListener)
	: m_psoListener			(psoListener)
	, m_lsSend				(m_itPool)
	, m_soClient			(INVALID_SOCKET)
	, m_evSocket			(nullptr)
	, m_dwConnID			(0)
	, m_hWorker				(nullptr)
	, m_dwWorkerID			(0)
	, m_bAsyncConnect		(FALSE)
	, m_iPending			(0)
	, m_enState				(SS_STOPPED)
	, m_enLastError			(SE_OK)
	, m_pExtra				(nullptr)
	, m_dwSocketBufferSize	(DEFAULT_TCP_SOCKET_BUFFER_SIZE)
	, m_dwFreeBufferPoolSize(DEFAULT_CLIENT_FREE_BUFFER_POOL_SIZE)
	, m_dwFreeBufferPoolHold(DEFAULT_CLIENT_FREE_BUFFER_POOL_HOLD)
	, m_dwKeepAliveTime		(DEFALUT_TCP_KEEPALIVE_TIME)
	, m_dwKeepAliveInterval	(DEFALUT_TCP_KEEPALIVE_INTERVAL)
	{
		ASSERT(m_psoListener);
	}

	virtual ~CTcpClient()	{if(HasStarted()) Stop();}

private:
	CInitSocket			m_wsSocket;

private:
	ITcpClientListener*	m_psoListener;
	BOOL				m_bAsyncConnect;
	SOCKET				m_soClient;
	HANDLE				m_evSocket;
	CONNID				m_dwConnID;

	DWORD				m_dwSocketBufferSize;
	DWORD				m_dwFreeBufferPoolSize;
	DWORD				m_dwFreeBufferPoolHold;
	DWORD				m_dwKeepAliveTime;
	DWORD				m_dwKeepAliveInterval;

	HANDLE				m_hWorker;

#ifndef _WIN32_WCE
	UINT				m_dwWorkerID;
#else
	DWORD
#endif					m_dwWorkerID;

	EnServiceState		m_enState;
	EnSocketError		m_enLastError;

	PVOID				m_pExtra;

	CBufferPtr			m_rcBuffer;

protected:
	CItemPool			m_itPool;

	CSpinGuard			m_csState;

private:
	CCriSec				m_csSend;
	TItemList			m_lsSend;

	CEvt				m_evBuffer;
	CEvt				m_evWorker;

	volatile int		m_iPending;
};
