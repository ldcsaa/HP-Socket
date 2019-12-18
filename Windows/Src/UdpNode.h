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

#ifdef _UDP_SUPPORT

class CUdpNode : public IUdpNode
{
public:
	virtual BOOL Start(LPCTSTR lpszBindAddress = nullptr, USHORT usPort = 0, EnCastMode enCastMode = CM_UNICAST, LPCTSTR lpszCastAddress = nullptr);
	virtual BOOL Stop();
	virtual BOOL Send(LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pBuffer, int iLength, int iOffset = 0);
	virtual BOOL SendPackets(LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const WSABUF pBuffers[], int iCount);
	virtual BOOL SendCast(const BYTE* pBuffer, int iLength, int iOffset = 0);
	virtual BOOL SendCastPackets(const WSABUF pBuffers[], int iCount);
	virtual BOOL Wait(DWORD dwMilliseconds = INFINITE) {return m_evWait.Wait(dwMilliseconds);}

	virtual BOOL			HasStarted			()	{return m_enState == SS_STARTED || m_enState == SS_STARTING;}
	virtual EnServiceState	GetState			()	{return m_enState;}
	virtual BOOL			GetLocalAddress		(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);
	virtual BOOL			GetCastAddress		(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);

	virtual BOOL GetPendingDataLength	(int& iPending) {iPending = m_iPending; return HasStarted();}
	virtual EnSocketError GetLastError	()	{return m_enLastError;}
	virtual LPCTSTR GetLastErrorDesc	()	{return ::GetSocketErrorDesc(m_enLastError);}

public:
	virtual void SetReuseAddressPolicy	(EnReuseAddressPolicy enReusePolicy){ENSURE_HAS_STOPPED(); m_enReusePolicy			= enReusePolicy;}
	virtual void SetWorkerThreadCount	(DWORD dwWorkerThreadCount)			{ENSURE_HAS_STOPPED(); m_dwWorkerThreadCount	= dwWorkerThreadCount;}
	virtual void SetFreeBufferPoolSize	(DWORD dwFreeBufferPoolSize)		{ENSURE_HAS_STOPPED(); m_dwFreeBufferPoolSize	= dwFreeBufferPoolSize;}
	virtual void SetFreeBufferPoolHold	(DWORD dwFreeBufferPoolHold)		{ENSURE_HAS_STOPPED(); m_dwFreeBufferPoolHold	= dwFreeBufferPoolHold;}
	virtual void SetPostReceiveCount	(DWORD dwPostReceiveCount)			{ENSURE_HAS_STOPPED(); m_dwPostReceiveCount		= dwPostReceiveCount;}
	virtual void SetMaxDatagramSize		(DWORD dwMaxDatagramSize)			{ENSURE_HAS_STOPPED(); m_dwMaxDatagramSize		= dwMaxDatagramSize;}
	virtual void SetMultiCastTtl		(int iMCTtl)						{ENSURE_HAS_STOPPED(); m_iMCTtl					= iMCTtl;}
	virtual void SetMultiCastLoop		(BOOL bMCLoop)						{ENSURE_HAS_STOPPED(); m_bMCLoop				= bMCLoop;}
	virtual void SetExtra				(PVOID pExtra)						{m_pExtra										= pExtra;}						

	virtual EnReuseAddressPolicy GetReuseAddressPolicy	()	{return m_enReusePolicy;}
	virtual DWORD GetWorkerThreadCount	()	{return m_dwWorkerThreadCount;}
	virtual DWORD GetFreeBufferPoolSize	()	{return m_dwFreeBufferPoolSize;}
	virtual DWORD GetFreeBufferPoolHold	()	{return m_dwFreeBufferPoolHold;}
	virtual DWORD GetPostReceiveCount	()	{return m_dwPostReceiveCount;}
	virtual DWORD GetMaxDatagramSize	()	{return m_dwMaxDatagramSize;}
	virtual EnCastMode GetCastMode		()	{return m_enCastMode;}
	virtual int GetMultiCastTtl			()	{return m_iMCTtl;}
	virtual BOOL IsMultiCastLoop		()	{return m_bMCLoop;}
	virtual PVOID GetExtra				()	{return m_pExtra;}

protected:
	EnHandleResult FirePrepareListen(SOCKET soListen)
		{return m_pListener->OnPrepareListen(this, soListen);}
	EnHandleResult FireShutdown()
		{return m_pListener->OnShutdown(this);}

	EnHandleResult FireSend(TUdpBufferObj* pBufferObj);
	EnHandleResult FireReceive(TUdpBufferObj* pBufferObj);
	EnHandleResult FireError(TUdpBufferObj* pBufferObj, int iErrorCode);

	void ProcessReceiveBufferObj(TUdpBufferObj* pBufferObj)
		{TRIGGER(FireReceive(pBufferObj));}

	void SetLastError(EnSocketError code, LPCSTR func, int ec);
	virtual BOOL CheckParams();
	virtual void PrepareStart();
	virtual void Reset();

	virtual void OnWorkerThreadStart(THR_ID dwThreadID) {}
	virtual void OnWorkerThreadEnd(THR_ID dwThreadID) {}

	BOOL DoSend(HP_SOCKADDR& addrRemote, const BYTE* pBuffer, int iLength, int iOffset = 0);
	BOOL DoSendPackets(HP_SOCKADDR& addrRemote, const WSABUF pBuffers[], int iCount);
	int SendInternal(HP_SOCKADDR& addrRemote, TUdpBufferObjPtr& bufPtr);

private:
	friend void ContinueReceiveFrom<>(CUdpNode* pThis, TUdpBufferObj* pBufferObj);

	static UINT WINAPI WorkerThreadProc(LPVOID pv);

	BOOL CheckStarting();
	BOOL CheckStoping();
	BOOL CreateListenSocket(LPCTSTR lpszBindAddress, USHORT usPort, LPCTSTR lpszCastAddress);
	BOOL CreateListenSocket(LPCTSTR lpszBindAddress, USHORT usPort, LPCTSTR lpszCastAddress, HP_SOCKADDR& bindAddr);
	BOOL BindListenSocket(HP_SOCKADDR& bindAddr);
	BOOL ConnectToGroup(const HP_SOCKADDR& bindAddr);
	BOOL CreateCompletePort();
	BOOL CreateWorkerThreads();
	BOOL StartAccept();

	void CloseListenSocket();
	void WaitForPostReceiveRelease();
	void WaitForWorkerThreadEnd();
	void ReleaseFreeBuffer();
	void CloseCompletePort();

	TUdpBufferObj*	GetFreeBufferObj(int iLen = -1);
	void			AddFreeBufferObj(TUdpBufferObj* pBufferObj);

	EnIocpAction CheckIocpCommand(OVERLAPPED* pOverlapped, DWORD dwBytes, ULONG_PTR ulCompKey);
	void HandleIo(TUdpBufferObj* pBufferObj, DWORD dwBytes, DWORD dwErrorCode);
	void HandleError(TUdpBufferObj* pBufferObj, DWORD dwErrorCode);
	void HandleSend(TUdpBufferObj* pBufferObj);
	void HandleReceive(TUdpBufferObj* pBufferObj);

	int ProcessReceive(TUdpBufferObj* pBufferObj);
	int ProcessSend(TUdpBufferObj* pBufferObj);
	int ProcessSend();

	BOOL IsCanSend			()	{return m_iSending <= GetSendBufferSize();}
	int GetSendBufferSize	()	{return (32 * DEFAULT_SOCKET_SNDBUFF_SIZE);}

	BOOL IsValid			()	{return m_enState == SS_STARTED;}
	BOOL IsPending			()	{return m_iPending > 0;}

	SOCKET GetListenSocket	()	{return m_soListen;}

private:

public:
	CUdpNode(IUdpNodeListener* pListener)
	: m_pListener				(pListener)
	, m_sndBuff					(m_bfObjPool)
	, m_hCompletePort			(nullptr)
	, m_soListen				(INVALID_SOCKET)
	, m_iRemainPostReceives		(0)
	, m_iPending				(0)
	, m_iSending				(0)
	, m_enLastError				(SE_OK)
	, m_enState					(SS_STOPPED)
	, m_enReusePolicy			(RAP_ADDR_ONLY)
	, m_dwWorkerThreadCount		(DEFAULT_WORKER_THREAD_COUNT)
	, m_dwFreeBufferPoolSize	(DEFAULT_FREE_BUFFEROBJ_POOL)
	, m_dwFreeBufferPoolHold	(DEFAULT_FREE_BUFFEROBJ_HOLD)
	, m_dwMaxDatagramSize		(DEFAULT_UDP_MAX_DATAGRAM_SIZE)
	, m_dwPostReceiveCount		(DEFAULT_UDP_POST_RECEIVE_COUNT)
	, m_pExtra					(nullptr)
	, m_iMCTtl					(1)
	, m_bMCLoop					(FALSE)
	, m_enCastMode				(CM_UNICAST)
	, m_castAddr				(AF_UNSPEC, TRUE)
	, m_localAddr				(AF_UNSPEC, TRUE)
	, m_evWait					(TRUE, TRUE)
	{
		ASSERT(sm_wsSocket.IsValid());
		ASSERT(m_pListener);
	}

	virtual ~CUdpNode()
	{
		ENSURE_STOP();
	}

private:
	static const CInitSocket& sm_wsSocket;

	CEvt m_evWait;

	EnReuseAddressPolicy m_enReusePolicy;
	DWORD m_dwWorkerThreadCount;
	DWORD m_dwFreeBufferPoolSize;
	DWORD m_dwFreeBufferPoolHold;
	DWORD m_dwMaxDatagramSize;
	DWORD m_dwPostReceiveCount;
	PVOID m_pExtra;

	int					m_iMCTtl;
	BOOL				m_bMCLoop;
	EnCastMode			m_enCastMode;
	HP_SOCKADDR			m_castAddr;
	HP_SOCKADDR			m_localAddr;

	CUdpBufferObjPool	m_bfObjPool;
	TUdpBufferObjList	m_sndBuff;

	IUdpNodeListener*	m_pListener;
	SOCKET				m_soListen;
	HANDLE				m_hCompletePort;
	EnServiceState		m_enState;
	EnSocketError		m_enLastError;

	vector<HANDLE>		m_vtWorkerThreads;

	CSpinGuard			m_csState;

	volatile long		m_iPending;
	volatile long		m_iSending;
	volatile long		m_iRemainPostReceives;
};

#endif
