/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Author	: Bruce Liang
 * Website	: https://github.com/ldcsaa
 * Project	: https://github.com/ldcsaa/HP-Socket
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

#include "SocketHelper.h"
#include "./common/GeneralHelper.h"
#include "./common/IODispatcher.h"

#ifdef _UDP_SUPPORT

class CUdpNode : public IUdpNode, private CIOHandler
{
public:
	virtual BOOL Start	(LPCTSTR lpszBindAddress = nullptr, USHORT usPort = 0, EnCastMode enCastMode = CM_UNICAST, LPCTSTR lpszCastAddress = nullptr);
	virtual BOOL Stop	();
	virtual BOOL Send	(LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pBuffer, int iLength, int iOffset = 0);
	virtual BOOL SendPackets	(LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const WSABUF pBuffers[], int iCount);
	virtual BOOL SendCast		(const BYTE* pBuffer, int iLength, int iOffset = 0);
	virtual BOOL SendCastPackets(const WSABUF pBuffers[], int iCount);
	virtual BOOL Wait			(DWORD dwMilliseconds = INFINITE) {return m_evWait.WaitFor(dwMilliseconds, WAIT_FOR_STOP_PREDICATE);}

	virtual BOOL			HasStarted			()	{return m_enState == SS_STARTED || m_enState == SS_STARTING;}
	virtual EnServiceState	GetState			()	{return m_enState;}
	virtual BOOL			GetLocalAddress		(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);
	virtual BOOL			GetCastAddress		(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);

	virtual EnSocketError GetLastError	()	{return m_enLastError;}
	virtual LPCTSTR GetLastErrorDesc	()	{return ::GetSocketErrorDesc(m_enLastError);}
	virtual BOOL GetPendingDataLength	(int& iPending);

private:
	virtual BOOL OnBeforeProcessIo(const TDispContext* pContext, PVOID pv, UINT events)			override;
	virtual VOID OnAfterProcessIo(const TDispContext* pContext, PVOID pv, UINT events, BOOL rs)	override;
	virtual VOID OnCommand(const TDispContext* pContext, TDispCommand* pCmd)					override;
	virtual BOOL OnReadyRead(const TDispContext* pContext, PVOID pv, UINT events)				override;
	virtual BOOL OnReadyWrite(const TDispContext* pContext, PVOID pv, UINT events)				override;
	virtual BOOL OnHungUp(const TDispContext* pContext, PVOID pv, UINT events)					override;
	virtual BOOL OnError(const TDispContext* pContext, PVOID pv, UINT events)					override;
	virtual VOID OnDispatchThreadStart(THR_ID tid)												override;
	virtual VOID OnDispatchThreadEnd(THR_ID tid)												override;

public:
	virtual void SetReuseAddressPolicy	(EnReuseAddressPolicy enReusePolicy){ENSURE_HAS_STOPPED(); ASSERT(m_enReusePolicy == enReusePolicy);}
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

	EnHandleResult FireSend(TNodeBufferObj* pBufferObj);
	EnHandleResult FireReceive(const HP_SOCKADDR* pRemoteAddr, const BYTE* pData, int iLength);
	EnHandleResult FireError(const HP_SOCKADDR*	pRemoteAddr, const BYTE* pData, int iLength, EnSocketOperation enOperation, int iErrorCode);

	void SetLastError(EnSocketError code, LPCSTR func, int ec);
	virtual BOOL CheckParams();
	virtual void PrepareStart();
	virtual void Reset();

	virtual void OnWorkerThreadStart(THR_ID dwThreadID) {}
	virtual void OnWorkerThreadEnd(THR_ID dwThreadID) {}

	BOOL DoSend(const HP_SOCKADDR& addrRemote, const BYTE* pBuffer, int iLength, int iOffset = 0);
	BOOL DoSendPackets(const HP_SOCKADDR& addrRemote, const WSABUF pBuffers[], int iCount);
	int SendInternal(const HP_SOCKADDR& addrRemote, TNodeBufferObjPtr& bufPtr);

private:
	BOOL CheckStarting();
	BOOL CheckStoping();
	BOOL ParseBindAddr(LPCTSTR lpszBindAddress, USHORT usPort, LPCTSTR lpszCastAddress, HP_SOCKADDR& bindAddr);
	BOOL CreateListenSocket(const HP_SOCKADDR& bindAddr);
	BOOL CreateWorkerThreads();
	BOOL StartAccept();

	void CloseListenSocket();
	void WaitForWorkerThreadEnd();
	void ReleaseFreeBuffer();

	int GenerateBufferIndex(const HP_SOCKADDR& addrRemote);

private:
	BOOL HandleReceive(const TDispContext* pContext, int flag = 0);
	BOOL HandleSend(const TDispContext* pContext, int flag = 0, int rd = 0);
	BOOL HandleClose(int idx, TNodeBufferObj* pBufferObj, EnSocketOperation enOperation, int iErrorCode);

	VOID HandleCmdSend(int idx, int flag);

	BOOL SendItem(int idx, TNodeBufferObjList& sndBuff, TNodeBufferObj* pBufferObj, BOOL& bBlocked);

private:
	BOOL IsValid			()			{return m_enState == SS_STARTED;}
	BOOL IsPending			(int idx)	{return m_sndBuffs[idx].Length() > 0;}

public:
	CUdpNode(IUdpNodeListener* pListener)
	: m_pListener				(pListener)
	, m_iSending				(0)
	, m_enLastError				(SE_OK)
	, m_enState					(SS_STOPPED)
	, m_enReusePolicy			(RAP_ADDR_AND_PORT)
	, m_dwWorkerThreadCount		(DEFAULT_WORKER_THREAD_COUNT)
	, m_dwFreeBufferPoolSize	(DEFAULT_FREE_BUFFEROBJ_POOL)
	, m_dwFreeBufferPoolHold	(DEFAULT_FREE_BUFFEROBJ_HOLD)
	, m_dwPostReceiveCount		(DEFAULT_UDP_POST_RECEIVE_COUNT)
	, m_dwMaxDatagramSize		(DEFAULT_UDP_MAX_DATAGRAM_SIZE)
	, m_pExtra					(nullptr)
	, m_iMCTtl					(1)
	, m_bMCLoop					(FALSE)
	, m_enCastMode				(CM_UNICAST)
	, m_castAddr				(AF_UNSPEC, TRUE)
	, m_localAddr				(AF_UNSPEC, TRUE)
	{
		ASSERT(m_pListener);
	}

	virtual ~CUdpNode()
	{
		ENSURE_STOP();
	}

private:
	EnReuseAddressPolicy m_enReusePolicy;
	DWORD m_dwWorkerThreadCount;
	DWORD m_dwFreeBufferPoolSize;
	DWORD m_dwFreeBufferPoolHold;
	DWORD m_dwPostReceiveCount;
	DWORD m_dwMaxDatagramSize;
	PVOID m_pExtra;

	int					m_iMCTtl;
	BOOL				m_bMCLoop;
	EnCastMode			m_enCastMode;

private:
	CSEM				m_evWait;

	HP_SOCKADDR			m_castAddr;
	HP_SOCKADDR			m_localAddr;

	CNodeBufferObjPool	m_bfObjPool;
	CNodeCriSecs		m_csSends;
	TNodeBufferObjLists	m_sndBuffs;

	IUdpNodeListener*	m_pListener;
	ListenSocketsPtr	m_soListens;
	EnServiceState		m_enState;
	EnSocketError		m_enLastError;

	CReceiveBuffersPtr	m_rcBuffers;

	CRWLock				m_lcState;

	volatile long		m_iSending;

	CIODispatcher		m_ioDispatcher;
};

#endif
