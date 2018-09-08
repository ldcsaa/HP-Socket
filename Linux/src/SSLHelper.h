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

#include "HPTypeDef.h"
#include "common/BufferPool.h"

#ifdef _SSL_SUPPORT

#include "openssl/ssl.h"

#define OPENSSL_VERSION_1_0_2	0x10002000L
#define OPENSSL_VERSION_1_1_0	0x10100000L

/************************************************************************
名称：SSL 握手状态
描述：标识当前连接的 SSL 握手状态
************************************************************************/
enum EnSSLHandShakeStatus
{
	SSL_HSS_INIT	= 0,	// 初始状态
	SSL_HSS_PROC	= 1,	// 正在握手
	SSL_HSS_SUCC	= 2,	// 握手成功
};

#if OPENSSL_VERSION_NUMBER < OPENSSL_VERSION_1_1_0

/* SSL CRYPTO DYNLOCK 结构 */
typedef struct CRYPTO_dynlock_value
{
	CSimpleRWLock cs;
} DynamicLock;

#endif

class CSSLInitializer
{
public:

	static void CleanupThreadState(THR_ID dwThreadID = 0);

private:

	CSSLInitializer();
	~CSSLInitializer();

private:

#if OPENSSL_VERSION_NUMBER < OPENSSL_VERSION_1_1_0
	static void ssl_lock_callback(int mode, int n, const char *file, int line);
	static CRYPTO_dynlock_value* ssl_lock_dyn_create_callback(const char *file, int line);
	static void ssl_lock_dyn_callback(int mode, CRYPTO_dynlock_value* l, const char *file, int line);
	static void ssl_lock_dyn_destroy_callback(CRYPTO_dynlock_value* l, const char *file, int line);
#endif

private:

#if OPENSSL_VERSION_NUMBER < OPENSSL_VERSION_1_1_0
	static int				sm_iLockNum;
	static CSimpleRWLock*	sm_pcsLocks;
#endif

	static CSSLInitializer sm_instance;

};

/************************************************************************
名称：SSL Context
描述：初始化和清理 SSL 运行环境
************************************************************************/
class CSSLContext
{
public:

	/*
	* 名称：初始化 SSL 环境参数
	* 描述：SSL 环境参数必须在 SSL 通信组件启动前完成初始化，否则启动失败
	*		
	* 参数：		enSessionMode			-- SSL 工作模式（参考 EnSSLSessionMode）
	*			iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
	*			lpszPemCertFile			-- 证书文件（客户端可选）
	*			lpszPemKeyFile			-- 私钥文件（客户端可选）
	*			lpszKeyPasswod			-- 私钥密码（没有密码则为空）
	*			lpszCAPemCertFileOrPath	-- CA 证书文件或目录（单向验证或客户端可选）
	*			fnServerNameCallback	-- SNI 回调函数指针（可选，只用于服务端）
	*
	* 返回值：	TRUE	-- 成功
	*			FALSE	-- 失败，可通过 ::GetLastError() 获取失败原因
	*/
	BOOL Initialize(EnSSLSessionMode enSessionMode, int iVerifyMode = SSL_VM_NONE, LPCTSTR lpszPemCertFile = nullptr, LPCTSTR lpszPemKeyFile = nullptr, LPCTSTR lpszKeyPasswod = nullptr, LPCTSTR lpszCAPemCertFileOrPath = nullptr, Fn_SNI_ServerNameCallback fnServerNameCallback = nullptr);

	/*
	* 名称：增加 SNI 主机证书（只用于服务端）
	* 描述：SSL 服务端在 Initialize() 成功后可以调用本方法增加多个 SNI 主机证书
	*		
	* 参数：		iVerifyMode				-- SSL 验证模式（参考 EnSSLVerifyMode）
	*			lpszPemCertFile			-- 证书文件
	*			lpszPemKeyFile			-- 私钥文件
	*			lpszKeyPasswod			-- 私钥密码（没有密码则为空）
	*			lpszCAPemCertFileOrPath	-- CA 证书文件或目录（单向验证可选）
	*
	* 返回值：	正数		-- 成功，并返回 SNI 主机证书对应的索引，该索引用于在 SNI 回调函数中定位 SNI 主机
	*			负数		-- 失败，可通过 ::GetLastError() 获取失败原因
	*/
	int AddServerContext(int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod = nullptr, LPCTSTR lpszCAPemCertFileOrPath = nullptr);

	/*
	* 名称：清理 SSL 运行环境
	* 描述：清理 SSL 运行环境，回收 SSL 相关内存
	*		1、CSSLContext 的析构函数会自动调用本方法
	*		2、当要重新设置 SSL 环境参数时，需要先调用本方法清理原先的环境参数
	*		
	* 参数：	无
	* 
	* 返回值：无
	*/
	void Cleanup();

	/* 获取 SSL 运行环境 SSL_CTX 对象 */
	SSL_CTX* GetContext				(int i) const;
	/* 获取 SSL 运行环境默认 SSL_CTX 对象 */
	SSL_CTX* GetDefaultContext		()		const	{return m_sslCtx;}
	/* 获取 SSL 运行环境的配置模式，配置模式参考：EnSSLSessionMode */
	EnSSLSessionMode GetSessionMode	()		const	{return m_enSessionMode;}
	/* 检查 SSL 运行环境是否初始化完成 */
	BOOL IsValid					()		const	{return m_sslCtx != nullptr;}

public:
	
		/*
	* 名称：清理线程局部环境 SSL 资源
	* 描述：任何一个操作 SSL 的线程，在通信结束时都需要清理线程局部环境 SSL 资源
	*		1、主线程和 HP-Socket 工作线程在通信结束时会自动清理线程局部环境 SSL 资源。因此，一般情况下不必手工调用本方法
	*		2、特殊情况下，当自定义线程参与 HP-Socket 通信操作并检查到 SSL 内存泄漏时，需在每次通信结束时自定义线程调用本方法
	*		
	* 参数：		dwThreadID	-- 线程 ID（0：当前线程）
	* 
	* 返回值：无
	*/
	static void RemoveThreadLocalState(THR_ID dwThreadID = 0)	{CSSLInitializer::CleanupThreadState(dwThreadID);}

public:

	CSSLContext()
	: m_enSessionMode		(SSL_SM_SERVER)
	, m_sslCtx				(nullptr)
	, m_fnServerNameCallback(nullptr)
	{

	}

	~CSSLContext() {Cleanup();}

private:

	void SetServerNameCallback(Fn_SNI_ServerNameCallback fn);
	int AddContext(int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath);
	BOOL LoadCertAndKey(SSL_CTX* sslCtx, int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath);

private:

	static int InternalServerNameCallback(SSL* ssl, int* ad, void* arg);

private:

	EnSSLSessionMode	m_enSessionMode;
	vector<SSL_CTX*>	m_lsSslCtxs;
	SSL_CTX*			m_sslCtx;

	Fn_SNI_ServerNameCallback m_fnServerNameCallback;
};

class CSSLSession
{
public:

	BOOL WriteRecvChannel(const BYTE* pData, int iLength);
	BOOL ReadRecvChannel();

	BOOL WriteSendChannel(const BYTE* pData, int iLength);
	BOOL WriteSendChannel(const WSABUF pBuffers[], int iCount);
	BOOL ReadSendChannel();

	const WSABUF& GetRecvBuffer()	const	{return m_bufRecv;}
	const WSABUF& GetSendBuffer()	const	{return m_bufSend;}

	CSSLSession*			Renew(const CSSLContext& sslCtx, LPCSTR lpszHostName = nullptr);
	BOOL					Reset();
	BOOL					IsValid()		const	{return GetStatus() != SSL_HSS_INIT;}
	BOOL					IsHandShaking()	const	{return GetStatus() == SSL_HSS_PROC;}
	BOOL					IsReady()		const	{return GetStatus() == SSL_HSS_SUCC;}
	EnSSLHandShakeStatus	GetStatus()		const	{return m_enStatus;}
	DWORD					GetFreeTime()	const	{return m_dwFreeTime;}
	CCriSec&				GetSendLock()			{return m_csSend;}

private:

	BOOL IsFatalError(int iBytes);

public:

	CSSLSession(CItemPool& itPool)
	: m_enStatus(SSL_HSS_INIT)
	, m_itPool	(itPool)
	, m_ssl		(nullptr)
	, m_bioSend	(nullptr)
	, m_bioRecv	(nullptr)
	, m_pitSend	(nullptr)
	, m_pitRecv	(nullptr)
	{

	}

	~CSSLSession()
	{
		Reset();
	}

	static CSSLSession* Construct(CItemPool& itPool)
		{return new CSSLSession(itPool);}

	static void Destruct(CSSLSession* pSession)
		{if(pSession) delete pSession;}

private:
	CItemPool&				m_itPool;
	CCriSec					m_csSend;

	DWORD					m_dwFreeTime;
	EnSSLHandShakeStatus	m_enStatus;

	SSL* m_ssl;
	BIO* m_bioSend;
	BIO* m_bioRecv;

	TItem*		m_pitSend;
	TItem*		m_pitRecv;
	WSABUF		m_bufSend;
	WSABUF		m_bufRecv;
};

class CSSLSessionPool
{
	typedef CRingPool<CSSLSession>	TSSLSessionList;
	typedef CCASQueue<CSSLSession>	TSSLSessionQueue;

public:
	CSSLSession*	PickFreeSession	(LPCSTR lpszHostName = nullptr);
	void			PutFreeSession	(CSSLSession* pSession);

	void			Prepare			();
	void			Clear			();

private:
	void ReleaseGCSession	(BOOL bForce = FALSE);

public:
	void SetItemCapacity	(DWORD dwItemCapacity)		{m_itPool.SetItemCapacity(dwItemCapacity);}
	void SetItemPoolSize	(DWORD dwItemPoolSize)		{m_itPool.SetPoolSize(dwItemPoolSize);}
	void SetItemPoolHold	(DWORD dwItemPoolHold)		{m_itPool.SetPoolHold(dwItemPoolHold);}

	void SetSessionLockTime	(DWORD dwSessionLockTime)	{m_dwSessionLockTime = dwSessionLockTime;}
	void SetSessionPoolSize	(DWORD dwSessionPoolSize)	{m_dwSessionPoolSize = dwSessionPoolSize;}
	void SetSessionPoolHold	(DWORD dwSessionPoolHold)	{m_dwSessionPoolHold = dwSessionPoolHold;}

	DWORD GetItemCapacity	()	{return m_itPool.GetItemCapacity();}
	DWORD GetItemPoolSize	()	{return m_itPool.GetPoolSize();}
	DWORD GetItemPoolHold	()	{return m_itPool.GetPoolHold();}

	DWORD GetSessionLockTime()	{return m_dwSessionLockTime;}
	DWORD GetSessionPoolSize()	{return m_dwSessionPoolSize;}
	DWORD GetSessionPoolHold()	{return m_dwSessionPoolHold;}

public:
	CSSLSessionPool(const CSSLContext& sslCtx,
					DWORD dwPoolSize = DEFAULT_SESSION_POOL_SIZE,
					DWORD dwPoolHold = DEFAULT_SESSION_POOL_HOLD,
					DWORD dwLockTime = DEFAULT_SESSION_LOCK_TIME)
	: m_sslCtx(sslCtx)
	, m_dwSessionPoolSize(dwPoolSize)
	, m_dwSessionPoolHold(dwPoolHold)
	, m_dwSessionLockTime(dwLockTime)
	{

	}

	~CSSLSessionPool()	{Clear();}

	DECLARE_NO_COPY_CLASS(CSSLSessionPool)

public:
	static const DWORD DEFAULT_ITEM_CAPACITY;
	static const DWORD DEFAULT_ITEM_POOL_SIZE;
	static const DWORD DEFAULT_ITEM_POOL_HOLD;
	static const DWORD DEFAULT_SESSION_LOCK_TIME;
	static const DWORD DEFAULT_SESSION_POOL_SIZE;
	static const DWORD DEFAULT_SESSION_POOL_HOLD;

private:
	CItemPool			m_itPool;
	const CSSLContext&	m_sslCtx;

	DWORD				m_dwSessionLockTime;
	DWORD				m_dwSessionPoolSize;
	DWORD				m_dwSessionPoolHold;

	TSSLSessionList		m_lsFreeSession;
	TSSLSessionQueue	m_lsGCSession;
};

template<class T, class S> EnHandleResult ProcessHandShake(T* pThis, S* pSocketObj, CSSLSession* pSession)
{
	EnHandleResult result = HR_OK;

	CCriSecLock locallock(pSession->GetSendLock());

	while(TRUE)
	{
		VERIFY(pSession->ReadSendChannel());
		const WSABUF& buffer = pSession->GetSendBuffer();

		if(buffer.len == 0)
			break;

		if(!pThis->DoSendPackets(pSocketObj, &buffer, 1))
		{
			result = HR_ERROR;
			break;
		}
	}

	return result;
}

template<class T, class S> EnHandleResult ProcessReceive(T* pThis, S* pSocketObj, CSSLSession* pSession, const BYTE* pData, int iLength)
{
	if(!pSession->WriteRecvChannel(pData, iLength))
		return HR_ERROR;

	EnHandleResult result			= HR_OK;
	EnSSLHandShakeStatus enStatus	= pSession->GetStatus();

	while(TRUE)
	{
		if(!pSession->ReadRecvChannel())
			return HR_ERROR;

		if(enStatus == SSL_HSS_PROC && pSession->IsReady())
		{
			result = ProcessHandShake(pThis, pSocketObj, pSession);

			if(result == HR_ERROR)
				break;

			enStatus = SSL_HSS_SUCC;
			result	 = pThis->DoFireHandShake(pSocketObj);

			if(result == HR_ERROR)
				break;
		}

		const WSABUF& buffer = pSession->GetRecvBuffer();

		if(buffer.len == 0)
			break;

		result = pThis->DoFireReceive(pSocketObj, (const BYTE*)buffer.buf, buffer.len);

		if(result == HR_ERROR)
			break;
	}

	if(result != HR_ERROR && pSession->IsHandShaking())
		result = ::ProcessHandShake(pThis, pSocketObj, pSession);

	return result;
}

template<class T, class S> BOOL ProcessSend(T* pThis, S* pSocketObj, CSSLSession* pSession, const WSABUF * pBuffers, int iCount)
{
	if(pSession == nullptr || !pSession->IsReady())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	CCriSecLock locallock(pSession->GetSendLock());

	if(!pSession->IsReady())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	VERIFY(pSession->WriteSendChannel(pBuffers, iCount));

	while(TRUE)
	{
		VERIFY(pSession->ReadSendChannel());
		const WSABUF& buffer = pSession->GetSendBuffer();

		if(buffer.len == 0)
			break;

		if(!pThis->DoSendPackets(pSocketObj, &buffer, 1))
			return FALSE;
	}

	return TRUE;
}

#endif