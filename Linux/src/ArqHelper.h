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

#include "HPTypeDef.h"

#ifdef _UDP_SUPPORT

#include "SocketHelper.h"

#include "common/FuncHelper.h"
#include "common/BufferPool.h"
#include "common/IODispatcher.h"
#include "common/kcp/ikcp.h"

#include <memory>

using namespace std;

#define DEFAULT_ARQ_NO_DELAY			FALSE
#define DEFAULT_ARQ_TURNOFF_NC			FALSE
#define DEFAULT_ARQ_FLUSH_INTERVAL		60
#define DEFAULT_ARQ_RESEND_BY_ACKS		0
#define DEFAULT_ARQ_SEND_WND_SIZE		128
#define DEFAULT_ARQ_RECV_WND_SIZE		512
#define DEFAULT_ARQ_MIN_RTO				30
#define DEFAULT_ARQ_FAST_LIMIT			5
#define DEFAULT_ARQ_MAX_TRANS_UNIT		DEFAULT_UDP_MAX_DATAGRAM_SIZE
#define DEFAULT_ARQ_MAX_MSG_SIZE		DEFAULT_BUFFER_CACHE_CAPACITY
#define DEFAULT_ARQ_HANND_SHAKE_TIMEOUT	5000

#define KCP_HEADER_SIZE					24
#define KCP_MIN_RECV_WND				128

#define ARQ_MAX_HANDSHAKE_INTERVAL		2000

using Fn_ArqOutputProc = int (*)(const char* pBuffer, int iLength, IKCPCB* kcp, LPVOID pv);

DWORD GenerateConversationID();

/************************************************************************
名称：ARQ 握手状态
描述：标识当前连接的 ARQ 握手状态
************************************************************************/
enum EnArqHandShakeStatus
{
	ARQ_HSS_INIT	= 0,	// 初始状态
	ARQ_HSS_PROC	= 1,	// 正在握手
	ARQ_HSS_SUCC	= 2,	// 握手成功
};

struct TArqCmd
{
public:
	static const UINT16 MAGIC			= 0xBB4F;
	static const UINT8	CMD_HANDSHAKE	= 0x01;
	static const UINT8	FLAG_COMPLETE	= 0x01;
	static const int	PACKAGE_LENGTH	= 12;

public:
	UINT16 magic;
	UINT8 cmd;
	UINT8 flag;
	DWORD selfID;
	DWORD peerID;

public:
	static BYTE* MakePackage(UINT8 cmd, UINT8 flag, DWORD selfID, DWORD peerID, UINT16 magic = MAGIC)
	{
		BYTE* buff = new BYTE[PACKAGE_LENGTH];

		*((UINT16*)(buff + 0))	= magic;
		*((UINT8*)(buff + 2))	= cmd;
		*((UINT8*)(buff + 3))	= flag;
		*((DWORD*)(buff + 4))	= selfID;
		*((DWORD*)(buff + 8))	= peerID;

		return buff;
	}

	BYTE* MakePackage()
	{
		return MakePackage(cmd, flag, selfID, peerID, magic);
	}

	BOOL Parse(const BYTE buff[PACKAGE_LENGTH])
	{
		magic	= *((UINT16*)(buff + 0));
		cmd		= *((UINT8*) (buff + 2));
		flag	= *((UINT8*) (buff + 3));
		selfID	= *((DWORD*) (buff + 4));
		peerID	= *((DWORD*) (buff + 8));

		return IsValid();
	}

	BOOL IsValid()	{return (magic == MAGIC && cmd == CMD_HANDSHAKE && (flag & 0xFE) == 0);}

public:
	TArqCmd()
	{
		::ZeroMemory(this, sizeof(TArqCmd));
	}

	TArqCmd(UINT8 c, UINT8 f, DWORD sid, DWORD pid)
	: magic	(MAGIC)
	, cmd	(c)
	, flag	(f)
	, selfID(sid)
	, peerID(pid)
	{

	}
};

struct TArqAttr
{
	BOOL	bNoDelay;
	BOOL	bTurnoffNc;
	DWORD	dwResendByAcks;
	DWORD	dwFlushInterval;
	DWORD	dwSendWndSize;
	DWORD	dwRecvWndSize;
	DWORD	dwMinRto;
	DWORD	dwMtu;
	DWORD	dwFastLimit;
	DWORD	dwMaxMessageSize;
	DWORD	dwHandShakeTimeout;

public:
	TArqAttr( BOOL no_delay				= DEFAULT_ARQ_NO_DELAY
			, BOOL turnoff_nc			= DEFAULT_ARQ_TURNOFF_NC
			, DWORD resend_by_acks		= DEFAULT_ARQ_RESEND_BY_ACKS
			, DWORD flush_interval		= DEFAULT_ARQ_FLUSH_INTERVAL
			, DWORD send_wnd_size		= DEFAULT_ARQ_SEND_WND_SIZE
			, DWORD recv_wnd_size		= DEFAULT_ARQ_RECV_WND_SIZE
			, DWORD min_rto				= DEFAULT_ARQ_MIN_RTO
			, DWORD mtu					= DEFAULT_ARQ_MAX_TRANS_UNIT
			, DWORD fast_limit			= DEFAULT_ARQ_FAST_LIMIT
			, DWORD max_msg_size		= DEFAULT_ARQ_MAX_MSG_SIZE
			, DWORD hand_shake_timeout	= DEFAULT_ARQ_HANND_SHAKE_TIMEOUT
			)
	: bNoDelay			(no_delay)
	, bTurnoffNc		(turnoff_nc)
	, dwResendByAcks	(resend_by_acks)
	, dwFlushInterval	(flush_interval)
	, dwSendWndSize		(send_wnd_size)
	, dwRecvWndSize		(recv_wnd_size)
	, dwMinRto			(min_rto)
	, dwMtu				(mtu)
	, dwFastLimit		(fast_limit)
	, dwMaxMessageSize	(max_msg_size)
	, dwHandShakeTimeout(hand_shake_timeout)
	{
		ASSERT(IsValid());
	}

	BOOL IsValid() const
	{
		return 	((int)dwResendByAcks >= 0)																				&&
				((int)dwFlushInterval > 0)																				&&
				((int)dwSendWndSize > 0)																				&&
				((int)dwRecvWndSize > 0)																				&&
				((int)dwMinRto > 0)																						&&
				((int)dwFastLimit >= 0)																					&&
				((int)dwHandShakeTimeout > 2 * (int)dwMinRto)															&&
				((int)dwMtu >= 3 * KCP_HEADER_SIZE && dwMtu <= MAXIMUM_UDP_MAX_DATAGRAM_SIZE)							&&
				((int)dwMaxMessageSize > 0 && dwMaxMessageSize < ((KCP_MIN_RECV_WND - 1) * (dwMtu - KCP_HEADER_SIZE)))	;
	}

};

template<class T, class S> class CArqSessionT
{
public:
	CArqSessionT* Renew(T* pContext, S* pSocket, const TArqAttr& attr, DWORD dwPeerConvID = 0)
	{
		m_pContext		= pContext;
		m_pSocket		= pSocket;
		m_dwSelfConvID	= ::GenerateConversationID();

		DoRenew(attr, dwPeerConvID);
		RenewExtra(attr);

		m_dwCreateTime	= ::TimeGetTime();
		m_dwHSNextTime	= m_dwCreateTime;
		m_dwHSSndCount	= 0;
		m_bHSComplete	= FALSE;
		m_enStatus		= ARQ_HSS_PROC;

		Check();

		return this;
	}

	BOOL Reset()
	{
		if(!IsValid())
			return FALSE;

		{
			CReentrantCriSecLock locallock(m_cs);

			if(!IsValid())
				return FALSE;

			m_enStatus	= ARQ_HSS_INIT;

			DoReset();
		}

		ResetExtra();

		return TRUE;
	}

	BOOL Check()
	{
		if(IsReady())
		{
			if(m_bHSComplete || DoHandShake())
				return Flush();
			else
				return FALSE;
		}
		else if(IsHandShaking())
			return DoHandShake();
		else
		{
			::SetLastError(ERROR_INVALID_STATE);
			return FALSE;
		}
	}

	BOOL DoHandShake()
	{
		if(!IsValid())
		{
			::SetLastError(ERROR_INVALID_STATE);
			return FALSE;
		}

		unique_ptr<BYTE[]> bufCmdPtr;

		{
			CReentrantCriSecLock locallock(m_cs);

			if(!IsValid())
			{
				::SetLastError(ERROR_INVALID_STATE);
				return FALSE;
			}

			DWORD dwCurrent = ::TimeGetTime();

			if(::GetTimeGap32(m_dwCreateTime, dwCurrent) > m_pContext->GetHandShakeTimeout())
			{
				::SetLastError(ERROR_TIMEOUT);
				return FALSE;
			}

			if((int)(::GetTimeGap32(m_dwHSNextTime, dwCurrent)) < 0)
				return TRUE;

			m_dwHSNextTime	= dwCurrent + MIN(m_kcp->interval * (++m_dwHSSndCount), ARQ_MAX_HANDSHAKE_INTERVAL);
			UINT8 iFlag		= IsReady() ? TArqCmd::FLAG_COMPLETE : 0;

			bufCmdPtr.reset(TArqCmd::MakePackage(TArqCmd::CMD_HANDSHAKE, iFlag, m_dwSelfConvID, m_dwPeerConvID));
		}

		return m_pContext->DoSend(m_pSocket, bufCmdPtr.get(), TArqCmd::PACKAGE_LENGTH);
	}

	BOOL Flush(BOOL bForce = FALSE)
	{
		if(!IsReady())
		{
			::SetLastError(ERROR_INVALID_STATE);
			return FALSE;
		}

		{
			CReentrantCriSecTryLock locallock(m_cs);

			if(locallock.IsValid())
			{
				if(!IsReady())
				{
					::SetLastError(ERROR_INVALID_STATE);
					return FALSE;
				}

				if(bForce)
					::ikcp_flush(m_kcp);
				else
					::ikcp_update(m_kcp, ::TimeGetTime());
			}
		}

		return TRUE;
	}

	int Send(const BYTE* pBuffer, int iLength)
	{
		if(!IsReady())
			return ERROR_INVALID_STATE;

		int rs = NO_ERROR;

		{
			CReentrantCriSecLock locallock(m_cs);

			if(!IsReady())
				return ERROR_INVALID_STATE;

			rs = ::ikcp_send(m_kcp, (const char*)pBuffer, iLength);
			if(rs != NO_ERROR) rs = ERROR_INCORRECT_SIZE;
		}

		if(rs == NO_ERROR)
			Flush(TRUE);

		return rs;
	}

	int GetWaitingSend()
	{
		if(!IsValid())
		{
			::SetLastError(ERROR_INVALID_STATE);
			return -1;
		}

		CReentrantCriSecLock locallock(m_cs);

		if(!IsValid())
		{
			::SetLastError(ERROR_INVALID_STATE);
			return -1;
		}

		return ::ikcp_waitsnd(m_kcp);
	}

	EnHandleResult Receive(const BYTE* pData, int iLength, BYTE* pBuffer, int iCapacity)
	{
		if(iLength >= KCP_HEADER_SIZE)
			return ReceiveArq(pData, iLength, pBuffer, iCapacity);
		else if(iLength == TArqCmd::PACKAGE_LENGTH)
			return ReceiveHandShake(pData);
		else
		{
			::WSASetLastError(ERROR_INVALID_DATA);
			return HR_ERROR;
		}
	}

	EnHandleResult ReceiveHandShake(const BYTE* pBuffer)
	{
		TArqCmd cmd;
		cmd.Parse(pBuffer);

		if(!cmd.IsValid())
		{
			::WSASetLastError(ERROR_INVALID_DATA);
			return HR_ERROR;
		}

		{
			CReentrantCriSecLock locallock(m_cs);

			if(!IsValid())
			{
				::WSASetLastError(ERROR_INVALID_STATE);
				return HR_ERROR;
			}
			
			if(IsReady())
			{
				BOOL bReset = FALSE;

				if(cmd.selfID != m_dwPeerConvID)
					bReset = TRUE;
				else if(cmd.peerID != m_dwSelfConvID)
				{
					if(cmd.peerID != 0)
						bReset = TRUE;
					else
					{
						if(::GetTimeGap32(m_dwCreateTime) > 2 * m_pContext->GetHandShakeTimeout())
							bReset = TRUE;
					}
				}

				if(bReset)
				{
					::WSASetLastError(ERROR_CONNRESET);
					return HR_ERROR;
				}
			}
			else
			{
				if(m_dwPeerConvID == 0)
				{
					m_dwPeerConvID = cmd.selfID;
					m_dwHSNextTime = ::TimeGetTime();
					m_dwHSSndCount = 0;
				}
				else if(cmd.selfID != m_dwPeerConvID)
				{
					::WSASetLastError(ERROR_CONNRESET);
					return HR_ERROR;
				}

				if(cmd.peerID == m_dwSelfConvID)
				{
					m_enStatus = ARQ_HSS_SUCC;
					return m_pContext->DoFireHandShake(m_pSocket);
				}
				else if(cmd.peerID != 0)
				{
					::WSASetLastError(ERROR_CONNRESET);
					return HR_ERROR;
				}
			}
		}

		if(!m_bHSComplete && cmd.flag == TArqCmd::FLAG_COMPLETE)
			m_bHSComplete = TRUE;

		DoHandShake();

		return HR_OK;
	}

	EnHandleResult ReceiveArq(const BYTE* pData, int iLength, BYTE* pBuffer, int iCapacity)
	{
		if(!IsReady()) return HR_IGNORE;

		{
			CReentrantCriSecLock locallock(m_cs);

			if(!IsReady())
			{
				::WSASetLastError(ERROR_INVALID_STATE);
				return HR_ERROR;
			}

			if(iLength < KCP_HEADER_SIZE)
			{
				::WSASetLastError(ERROR_INVALID_DATA);
				return HR_ERROR;
			}

			int rs = ::ikcp_input(m_kcp, (const char*)pData, iLength);

			if(rs != NO_ERROR)
			{
				::WSASetLastError(ERROR_INVALID_DATA);
				return HR_ERROR;
			}

			while(TRUE)
			{
				int iRead = ::ikcp_recv(m_kcp, (char*)pBuffer, iCapacity);

				if(iRead >= 0)
				{
					EnHandleResult result = m_pContext->DoFireReceive(m_pSocket, pBuffer, iRead);

					if(result == HR_ERROR)
						return result;
				}
				else if(iRead == -3)
				{
					::WSASetLastError(ERROR_INCORRECT_SIZE);
					return HR_ERROR;
				}
				else
					break;
			}
		}

		Flush(TRUE);

		return HR_OK;
	}

private:
	void DoRenew(const TArqAttr& attr, DWORD dwPeerConvID = 0)
	{
		ASSERT(attr.IsValid());

		DoReset();

		m_dwPeerConvID	= dwPeerConvID;
		m_kcp			= ::ikcp_create(m_dwSelfConvID, m_pSocket);

		::ikcp_nodelay(m_kcp, attr.bNoDelay ? 1 : 0, (int)attr.dwFlushInterval, (int)attr.dwResendByAcks, attr.bTurnoffNc ? 1 : 0);
		::ikcp_wndsize(m_kcp, (int)attr.dwSendWndSize, (int)attr.dwRecvWndSize);
		::ikcp_setmtu(m_kcp, attr.dwMtu);

		m_kcp->rx_minrto	= (int)attr.dwMinRto;
		m_kcp->fastlimit	= (int)attr.dwFastLimit;
		m_kcp->output		= m_pContext->GetArqOutputProc();
	}

	void DoReset()
	{
		if(m_kcp != nullptr)
		{
			::ikcp_release(m_kcp);
			m_kcp = nullptr;
		}
	}

public:
	BOOL		IsValid()		const	{return GetStatus() != ARQ_HSS_INIT;}
	BOOL		IsHandShaking()	const	{return GetStatus() == ARQ_HSS_PROC;}
	BOOL		IsReady()		const	{return GetStatus() == ARQ_HSS_SUCC;}
	IKCPCB*		GetKcp()				{return m_kcp;}
	DWORD		GetConvID()		const	{if(!IsValid()) return 0; return m_kcp->conv;}
	DWORD		GetSelfConvID()	const	{return m_dwSelfConvID;}
	DWORD		GetPeerConvID()	const	{return m_dwPeerConvID;}
	
	CReentrantCriSec&		GetLock()			{return m_cs;}
	EnArqHandShakeStatus	GetStatus()	const	{return m_enStatus;}

protected:
	virtual void RenewExtra(const TArqAttr& attr) {}
	virtual void ResetExtra() {}

public:
	CArqSessionT()
	: m_pContext	(nullptr)
	, m_pSocket		(nullptr)
	, m_kcp			(nullptr)
	, m_enStatus	(ARQ_HSS_INIT)
	, m_dwSelfConvID(0)
	, m_dwPeerConvID(0)
	, m_dwCreateTime(0)
	, m_dwHSNextTime(0)
	, m_dwHSSndCount(0)
	, m_bHSComplete	(FALSE)
	{

	}

	virtual ~CArqSessionT()
	{
		Reset();
	}

	static CArqSessionT* Construct()
		{return new CArqSessionT();}

	static void Destruct(CArqSessionT* pSession)
		{if(pSession) delete pSession;}

protected:
	T*		m_pContext;
	S*		m_pSocket;

private:
	BOOL	m_bHSComplete;
	DWORD	m_dwHSNextTime;
	DWORD	m_dwHSSndCount;
	DWORD	m_dwCreateTime;
	DWORD	m_dwSelfConvID;
	DWORD	m_dwPeerConvID;
	EnArqHandShakeStatus m_enStatus;

	CReentrantCriSec m_cs;
	IKCPCB*			 m_kcp;
};

template<class T, class S> class CArqSessionPoolT;

template<class T, class S> class CArqSessionExT : public CArqSessionT<T, S>
{
	using __super = CArqSessionT<T, S>;

	using __super::Reset;

	friend class CArqSessionPoolT<T, S>;

public:
	DWORD GetFreeTime	()	const	{return m_dwFreeTime;}
	FD GetTimer			()	const	{return m_fdTimer;}

protected:
	virtual void RenewExtra(const TArqAttr& attr)
	{
		m_fdTimer = m_ioDispatcher.AddTimer(attr.dwFlushInterval, this);
		ASSERT(IS_VALID_FD(m_fdTimer));
	}

	virtual void ResetExtra()
	{
		m_ioDispatcher.DelTimer(m_fdTimer);

		m_dwFreeTime = ::TimeGetTime();
		m_fdTimer	 = INVALID_FD;
	}

public:
	CArqSessionExT(CIODispatcher& ioDispatcher)
	: m_ioDispatcher(ioDispatcher)
	, m_fdTimer		(INVALID_FD)
	, m_dwFreeTime	(0)
	{

	}

	virtual ~CArqSessionExT()
	{
		Reset();
	}

	static CArqSessionExT* Construct(CIODispatcher& ioDispatcher)
		{return new CArqSessionExT(ioDispatcher);}

	static void Destruct(CArqSessionExT* pSession)
		{if(pSession) delete pSession;}

private:
	CIODispatcher& m_ioDispatcher;

	FD		m_fdTimer;
	DWORD	m_dwFreeTime;
};

template<class T, class S> class CArqSessionPoolT : private CIOHandler
{
	using CArqSessionEx		= CArqSessionExT<T, S>;
	using TArqSessionList	= CRingPool<CArqSessionEx>;
	using TArqSessionQueue	= CCASQueue<CArqSessionEx>;

public:
	CArqSessionEx* PickFreeSession(T* pContext, S* pSocket, const TArqAttr& attr)
	{
		DWORD dwIndex;
		CArqSessionEx* pSession = nullptr;

		if(m_lsFreeSession.TryLock(&pSession, dwIndex))
		{
			if(::GetTimeGap32(pSession->GetFreeTime()) >= m_dwSessionLockTime)
				ENSURE(m_lsFreeSession.ReleaseLock(nullptr, dwIndex));
			else
			{
				ENSURE(m_lsFreeSession.ReleaseLock(pSession, dwIndex));
				pSession = nullptr;
			}
		}

		if(!pSession) pSession = CArqSessionEx::Construct(m_ioDispatcher);

		ASSERT(pSession);
		return (CArqSessionEx*)pSession->Renew(pContext, pSocket, attr);
	}

	void PutFreeSession(CArqSessionEx* pSession)
	{
		if(pSession->Reset())
		{
			ReleaseGCSession();

			if(!m_lsFreeSession.TryPut(pSession))
				m_lsGCSession.PushBack(pSession);
		}
	}

	void Prepare()
	{
		m_lsFreeSession.Reset(m_dwSessionPoolSize);

		m_ioDispatcher.Start(this, m_pContext->GetPostReceiveCount(), m_pContext->GetWorkerThreadCount());
	}

	void Clear()
	{
		m_ioDispatcher.Stop();

		m_lsFreeSession.Clear();

		ReleaseGCSession(TRUE);
		ENSURE(m_lsGCSession.IsEmpty());
	}

private:
	void ReleaseGCSession(BOOL bForce = FALSE)
	{
		::ReleaseGCObj(m_lsGCSession, m_dwSessionLockTime, bForce);
	}

	virtual BOOL OnReadyRead(PVOID pv, UINT events) override
	{

		if(events & _EPOLL_ALL_ERROR_EVENTS)
			return FALSE;

		CArqSessionEx* pSession = (CArqSessionEx*)pv;

		if(!pSession->Check() && pSession->IsValid() && TUdpSocketObj::IsValid(pSession->m_pSocket))
			pSession->m_pContext->Disconnect(pSession->m_pSocket->connID);

		::ReadTimer(pSession->GetTimer());

		return TRUE;
	}

public:
	void SetSessionLockTime	(DWORD dwSessionLockTime)	{m_dwSessionLockTime = dwSessionLockTime;}
	void SetSessionPoolSize	(DWORD dwSessionPoolSize)	{m_dwSessionPoolSize = dwSessionPoolSize;}
	void SetSessionPoolHold	(DWORD dwSessionPoolHold)	{m_dwSessionPoolHold = dwSessionPoolHold;}

	DWORD GetSessionLockTime()	{return m_dwSessionLockTime;}
	DWORD GetSessionPoolSize()	{return m_dwSessionPoolSize;}
	DWORD GetSessionPoolHold()	{return m_dwSessionPoolHold;}

public:
	CArqSessionPoolT(T* pContext,
					DWORD dwPoolSize = DEFAULT_SESSION_POOL_SIZE,
					DWORD dwPoolHold = DEFAULT_SESSION_POOL_HOLD,
					DWORD dwLockTime = DEFAULT_SESSION_LOCK_TIME)
	: m_pContext(pContext)
	, m_dwSessionPoolSize(dwPoolSize)
	, m_dwSessionPoolHold(dwPoolHold)
	, m_dwSessionLockTime(dwLockTime)
	{

	}

	~CArqSessionPoolT()	{Clear();}

	DECLARE_NO_COPY_CLASS(CArqSessionPoolT)

public:
	static const DWORD DEFAULT_SESSION_LOCK_TIME;
	static const DWORD DEFAULT_SESSION_POOL_SIZE;
	static const DWORD DEFAULT_SESSION_POOL_HOLD;

private:
	T*					m_pContext;

	DWORD				m_dwSessionLockTime;
	DWORD				m_dwSessionPoolSize;
	DWORD				m_dwSessionPoolHold;

	TArqSessionList		m_lsFreeSession;
	TArqSessionQueue	m_lsGCSession;

	CIODispatcher		m_ioDispatcher;
};

template<class T, class S> const DWORD CArqSessionPoolT<T, S>::DEFAULT_SESSION_LOCK_TIME	= DEFAULT_OBJECT_CACHE_LOCK_TIME;
template<class T, class S> const DWORD CArqSessionPoolT<T, S>::DEFAULT_SESSION_POOL_SIZE	= DEFAULT_OBJECT_CACHE_POOL_SIZE;
template<class T, class S> const DWORD CArqSessionPoolT<T, S>::DEFAULT_SESSION_POOL_HOLD	= DEFAULT_OBJECT_CACHE_POOL_HOLD;

#endif
