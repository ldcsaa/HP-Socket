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
 
#include "SSLHelper.h"
#include "SocketHelper.h"
#include "common/FileHelper.h"

#ifdef _SSL_SUPPORT

#include "openssl/ssl.h"
#include "openssl/err.h"
#include "openssl/engine.h"
#include "openssl/x509v3.h"

#if OPENSSL_VERSION_NUMBER < OPENSSL_VERSION_1_1_0
	int CSSLInitializer::sm_iLockNum			= 0;
	CSimpleRWLock* CSSLInitializer::sm_pcsLocks	= nullptr;
#endif

CSSLInitializer CSSLInitializer::sm_instance;

const DWORD CSSLSessionPool::DEFAULT_ITEM_CAPACITY		= CItemPool::DEFAULT_ITEM_CAPACITY;
const DWORD CSSLSessionPool::DEFAULT_ITEM_POOL_SIZE		= CItemPool::DEFAULT_POOL_SIZE;
const DWORD CSSLSessionPool::DEFAULT_ITEM_POOL_HOLD		= CItemPool::DEFAULT_POOL_HOLD;
const DWORD CSSLSessionPool::DEFAULT_SESSION_LOCK_TIME	= 15 * 1000;
const DWORD CSSLSessionPool::DEFAULT_SESSION_POOL_SIZE	= 600;
const DWORD CSSLSessionPool::DEFAULT_SESSION_POOL_HOLD	= 600;

CSSLInitializer::CSSLInitializer()
{
#if OPENSSL_VERSION_NUMBER < OPENSSL_VERSION_1_1_0
	sm_iLockNum = CRYPTO_num_locks();

	if(sm_iLockNum > 0)
		sm_pcsLocks = new CSimpleRWLock[sm_iLockNum];
/*
#ifdef _DEBUG
	CRYPTO_malloc_debug_init();
	CRYPTO_dbg_set_options(V_CRYPTO_MDEBUG_ALL);
	CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);
#endif
*/
	CRYPTO_set_locking_callback			(&ssl_lock_callback);
	CRYPTO_set_dynlock_create_callback	(&ssl_lock_dyn_create_callback);
	CRYPTO_set_dynlock_destroy_callback	(&ssl_lock_dyn_destroy_callback);
	CRYPTO_set_dynlock_lock_callback	(&ssl_lock_dyn_callback);

	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();
#else
	OPENSSL_init_ssl(OPENSSL_INIT_SSL_DEFAULT, nullptr);
#endif
}

CSSLInitializer::~CSSLInitializer()
{
	CleanupThreadState();

#if OPENSSL_VERSION_NUMBER < OPENSSL_VERSION_1_1_0
	CONF_modules_free();
	ENGINE_cleanup();
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();
	ERR_free_strings();
#if OPENSSL_VERSION_NUMBER >= OPENSSL_VERSION_1_0_2
	SSL_COMP_free_compression_methods();
#endif

	CRYPTO_set_locking_callback			(nullptr);
	CRYPTO_set_dynlock_create_callback	(nullptr);
	CRYPTO_set_dynlock_destroy_callback	(nullptr);
	CRYPTO_set_dynlock_lock_callback	(nullptr);

	if(sm_iLockNum > 0)
	{
		delete[] sm_pcsLocks;

		sm_pcsLocks = nullptr;
		sm_iLockNum = 0;
	}
#endif
}

void CSSLInitializer::CleanupThreadState(THR_ID dwThreadID)
{
#if OPENSSL_VERSION_NUMBER < OPENSSL_VERSION_1_1_0
	CRYPTO_THREADID tid = {nullptr, dwThreadID};
	
	CRYPTO_THREADID_current(&tid);
	ERR_remove_thread_state(&tid);
#else
	OPENSSL_thread_stop();
#endif
}

#if OPENSSL_VERSION_NUMBER < OPENSSL_VERSION_1_1_0

void CSSLInitializer::ssl_lock_callback(int mode, int n, const char *file, int line)
{
	mode & CRYPTO_LOCK
	? (mode & CRYPTO_READ
		? sm_pcsLocks[n].lock_shared()
		: sm_pcsLocks[n].lock())
	: (mode & CRYPTO_READ
		? sm_pcsLocks[n].unlock_shared()
		: sm_pcsLocks[n].unlock());
}

CRYPTO_dynlock_value* CSSLInitializer::ssl_lock_dyn_create_callback(const char *file, int line)
 {
	 return new DynamicLock;
 }

void CSSLInitializer::ssl_lock_dyn_callback(int mode, CRYPTO_dynlock_value* l, const char *file, int line)
{
	mode & CRYPTO_LOCK
	? (mode & CRYPTO_READ
		? l->cs.lock_shared()
		: l->cs.lock())
	: (mode & CRYPTO_READ
		? l->cs.unlock_shared()
		: l->cs.unlock());
}

void CSSLInitializer::ssl_lock_dyn_destroy_callback(CRYPTO_dynlock_value* l, const char *file, int line)
{
	delete l;
}

#endif

BOOL CSSLContext::Initialize(EnSSLSessionMode enSessionMode, int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath, HP_Fn_SNI_ServerNameCallback fnServerNameCallback)
{
	ASSERT(!IsValid());

	if(IsValid())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	m_enSessionMode	= enSessionMode;

	if(AddContext(iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath) == 0)
		m_sslCtx = GetContext(0);
	else
	{
		Cleanup();
		return FALSE;
	}

	SetServerNameCallback(fnServerNameCallback);

	return TRUE;
}

int CSSLContext::AddServerContext(int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath)
{
	ASSERT(IsValid());

	if(!IsValid())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	if(m_enSessionMode != SSL_SM_SERVER)
	{
		::SetLastError(ERROR_INVALID_OPERATION);
		return FALSE;
	}

	return AddContext(iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath);
}

int CSSLContext::AddContext(int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath)
{
	int iIndex		= -1;
	SSL_CTX* sslCtx	= SSL_CTX_new(SSLv23_method());

	SSL_CTX_set_quiet_shutdown(sslCtx, 1);
	SSL_CTX_set_verify(sslCtx, iVerifyMode, nullptr);
	SSL_CTX_set_cipher_list(sslCtx, "ALL:!aNULL:!eNULL");

	if(m_enSessionMode == SSL_SM_SERVER)
	{
		static volatile ULONG s_session_id_context = 0;
		ULONG session_id_context = ::InterlockedIncrement(&s_session_id_context);

		SSL_CTX_set_session_id_context(sslCtx, (BYTE*)&session_id_context, sizeof(session_id_context));
	}

	if(!LoadCertAndKey(sslCtx, iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath))
		SSL_CTX_free(sslCtx);
	else
	{
		iIndex = (int)m_lsSslCtxs.size();
		m_lsSslCtxs.push_back(sslCtx);
	}
	
	return iIndex;
}

BOOL CSSLContext::LoadCertAndKey(SSL_CTX* sslCtx, int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath)
{
	USES_CONVERSION;

	if(lpszCAPemCertFileOrPath != nullptr)
	{
		LPCTSTR lpszCAPemCertFile = nullptr;
		LPCTSTR lpszCAPemCertPath = nullptr;

		CFile fCAPemCertFile(lpszCAPemCertFileOrPath, O_RDONLY | O_CLOEXEC);

		if(!fCAPemCertFile.IsExist())
		{
			::SetLastError(ERROR_FILE_NOT_FOUND);
			return FALSE;
		}

		if(fCAPemCertFile.IsFile())
			lpszCAPemCertFile = lpszCAPemCertFileOrPath;
		else if(fCAPemCertFile.IsDirectory())
			lpszCAPemCertPath = lpszCAPemCertFileOrPath;
		else
		{
			::SetLastError(ERROR_BAD_FILE_TYPE);
			return FALSE;
		}

		if(!SSL_CTX_load_verify_locations(sslCtx, T2CA(lpszCAPemCertFile), T2CA(lpszCAPemCertPath)))
		{
			::SetLastError(ERROR_INVALID_DATA);
			return FALSE;
		}

		if(!SSL_CTX_set_default_verify_paths(sslCtx))
		{
			::SetLastError(ERROR_FUNCTION_FAILED);
			return FALSE;
		}

		if(m_enSessionMode == SSL_SM_SERVER && iVerifyMode & SSL_VM_PEER)
		{
			STACK_OF(X509_NAME)* caCertNames = SSL_load_client_CA_file(T2CA(lpszCAPemCertFileOrPath));

			if(caCertNames == nullptr)
			{
				::SetLastError(ERROR_EMPTY);
				return FALSE;
			}

			SSL_CTX_set_client_CA_list(sslCtx, caCertNames);
		}
	}

	if(lpszPemCertFile != nullptr)
	{
		CFile fPemCertFile(lpszPemCertFile, O_RDONLY | O_CLOEXEC);

		if(!fPemCertFile.IsFile())
		{
			::SetLastError(ERROR_FILE_NOT_FOUND);
			return FALSE;
		}

		if(lpszPemKeyFile == nullptr)
		{
			::SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		CFile fPemKeyFile(lpszPemKeyFile, O_RDONLY | O_CLOEXEC);

		if(!fPemKeyFile.IsFile())
		{
			::SetLastError(ERROR_FILE_NOT_FOUND);
			return FALSE;
		}
		
		if(lpszKeyPasswod != nullptr)
			SSL_CTX_set_default_passwd_cb_userdata(sslCtx, (void*)T2CA(lpszKeyPasswod));

		if(!SSL_CTX_use_PrivateKey_file(sslCtx, T2CA(lpszPemKeyFile), SSL_FILETYPE_PEM))
		{
			::SetLastError(ERROR_INVALID_PASSWORD);
			return FALSE;
		}

		if(!SSL_CTX_use_certificate_chain_file(sslCtx, T2CA(lpszPemCertFile)))
		{
			::SetLastError(ERROR_INVALID_DATA);
			return FALSE;
		}

		if(!SSL_CTX_check_private_key(sslCtx))
		{
			::SetLastError(ERROR_INVALID_ACCESS);
			return FALSE;
		}
	}

	return TRUE;
}

void CSSLContext::Cleanup()
{
	if(IsValid())
	{
		int iCount = (int)m_lsSslCtxs.size();

		for(int i = 0; i < iCount; i++)
			SSL_CTX_free(m_lsSslCtxs[i]);

		m_lsSslCtxs.clear();
		m_sslCtx = nullptr;
	}

	m_fnServerNameCallback = nullptr;

	RemoveThreadLocalState();
}

void CSSLContext::SetServerNameCallback(Fn_SNI_ServerNameCallback fn)
{
	if(m_enSessionMode != SSL_SM_SERVER)
		return;

	m_fnServerNameCallback = fn;

	if(m_fnServerNameCallback == nullptr)
		return;

	VERIFY(SSL_CTX_set_tlsext_servername_callback(m_sslCtx, InternalServerNameCallback));
	VERIFY(SSL_CTX_set_tlsext_servername_arg(m_sslCtx, this));
}

int CSSLContext::InternalServerNameCallback(SSL* ssl, int* ad, void* arg)
{
	USES_CONVERSION;

	CSSLContext* pThis = (CSSLContext*)arg;
	ASSERT(pThis->m_fnServerNameCallback != nullptr);

	const char* lpszServerName = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name);

	if(lpszServerName == nullptr)
		return SSL_TLSEXT_ERR_NOACK;

	int iIndex = pThis->m_fnServerNameCallback(A2CT(lpszServerName));

	if(iIndex == 0)
		return SSL_TLSEXT_ERR_OK;

	if(iIndex < 0)
	{
		::SetLastError(ERROR_INVALID_NAME);
		return SSL_TLSEXT_ERR_ALERT_FATAL;
	}

	SSL_CTX* sslCtx = pThis->GetContext(iIndex);

	if(sslCtx == nullptr)
	{
		::SetLastError(ERROR_INVALID_INDEX);
		return SSL_TLSEXT_ERR_ALERT_FATAL;
	}

	SSL_set_SSL_CTX(ssl, sslCtx);

	return SSL_TLSEXT_ERR_OK;
}

SSL_CTX* CSSLContext::GetContext(int i) const
{
	SSL_CTX* sslCtx = nullptr;

	if(i >= 0 && i < (int)m_lsSslCtxs.size())
		sslCtx = m_lsSslCtxs[i];

	return sslCtx;
}

BOOL CSSLSession::WriteRecvChannel(const BYTE* pData, int iLength)
{
	ASSERT(pData && iLength > 0);

	BOOL isOK = TRUE;
	int bytes = BIO_write(m_bioRecv, pData, iLength);

	if(bytes > 0)
		ASSERT(bytes == iLength);
	else if(!BIO_should_retry(m_bioRecv))
		isOK = FALSE;

	return isOK;
}

BOOL CSSLSession::ReadRecvChannel()
{
	BOOL isOK = TRUE;
	int bytes = SSL_read(m_ssl, m_bufRecv.buf, m_pitRecv->Capacity());

	if(bytes > 0)
		m_bufRecv.len = bytes;
	else if(!IsFatalError(bytes))
		m_bufRecv.len = 0;
	else
		isOK = FALSE;

	if(isOK && m_enStatus == SSL_HSS_PROC && SSL_is_init_finished(m_ssl))
		m_enStatus = SSL_HSS_SUCC;

	return isOK;
}

BOOL CSSLSession::WriteSendChannel(const BYTE* pData, int iLength)
{
	ASSERT(IsReady());
	ASSERT(pData && iLength > 0);

	BOOL isOK = TRUE;
	int bytes = SSL_write(m_ssl, pData, iLength);

	if(bytes > 0)
		ASSERT(bytes == iLength);
	else if(IsFatalError(bytes))
		isOK = FALSE;

	return isOK;
}

BOOL CSSLSession::WriteSendChannel(const WSABUF pBuffers[], int iCount)
{
	ASSERT(pBuffers && iCount > 0);

	BOOL isOK = TRUE;

	for(int i = 0; i < iCount; i++)
	{
		const WSABUF& buffer = pBuffers[i];

		if(buffer.len > 0)
		{
			if(!WriteSendChannel((const BYTE*)buffer.buf, buffer.len))
			{
				isOK = FALSE;
				break;
			}
		}
	}

	return isOK;
}

BOOL CSSLSession::ReadSendChannel()
{
	if(BIO_pending(m_bioSend) == 0)
	{
		m_bufSend.len = 0;
		return TRUE;
	}

	BOOL isOK = TRUE;
	int bytes = BIO_read(m_bioSend, m_bufSend.buf, m_pitSend->Capacity());

	if(bytes > 0)
		m_bufSend.len = bytes;
	else if(BIO_should_retry(m_bioSend))
		m_bufSend.len = 0;
	else
		isOK = FALSE;

	return isOK;
}

CSSLSession* CSSLSession::Renew(const CSSLContext& sslCtx, LPCSTR lpszHostName)
{
	ASSERT(!IsValid());

	m_ssl		= SSL_new(sslCtx.GetDefaultContext());
	m_bioSend	= BIO_new(BIO_s_mem());
	m_bioRecv	= BIO_new(BIO_s_mem());

	SSL_set_bio(m_ssl, m_bioRecv, m_bioSend);

	if(sslCtx.GetSessionMode() == SSL_SM_SERVER)
		SSL_accept(m_ssl);
	else
	{
		USES_CONVERSION;

		if(lpszHostName && lpszHostName[0] != 0 && !::IsIPAddress(A2CT(lpszHostName)))
			SSL_set_tlsext_host_name(m_ssl, lpszHostName);

		SSL_connect(m_ssl);
	}

	m_pitSend		= m_itPool.PickFreeItem();
	m_pitRecv		= m_itPool.PickFreeItem();
	m_bufSend.buf	= m_pitSend->Ptr();
	m_bufRecv.buf	= m_pitRecv->Ptr();
	m_enStatus		= SSL_HSS_PROC;

	return this;
}

BOOL CSSLSession::Reset()
{
	BOOL isOK = FALSE;

	if(IsValid())
	{
		CCriSecLock locallock(m_csSend);

		if(IsValid())
		{
			m_enStatus = SSL_HSS_INIT;

			SSL_shutdown(m_ssl);
			SSL_free(m_ssl);

			m_itPool.PutFreeItem(m_pitSend);
			m_itPool.PutFreeItem(m_pitRecv);

			m_pitSend	= nullptr;
			m_pitRecv	= nullptr;
			m_ssl		= nullptr;
			m_bioSend	= nullptr;
			m_bioRecv	= nullptr;
			m_dwFreeTime= ::TimeGetTime();

			isOK = TRUE;
		}
	}

	ERR_clear_error();

	return isOK;
}

inline BOOL CSSLSession::IsFatalError(int iBytes)
{
	int iErrorCode = SSL_get_error(m_ssl, iBytes);

	if(	iErrorCode == SSL_ERROR_NONE			||
		iErrorCode == SSL_ERROR_WANT_READ		||
		iErrorCode == SSL_ERROR_WANT_WRITE		||
		iErrorCode == SSL_ERROR_WANT_CONNECT	||
		iErrorCode == SSL_ERROR_WANT_ACCEPT		)
		return FALSE;

#ifdef _DEBUG
	char szBuffer[512];
#endif

	int i		= 0;
	ULONG iCode	= iErrorCode;

	for(; iCode != SSL_ERROR_NONE; i++)
	{
#ifdef _DEBUG
		ERR_error_string_n(iCode, szBuffer, sizeof(szBuffer));
		TRACE("	> SSL Error: %ld - %s\n", iCode, szBuffer);
#endif

		iCode = ERR_get_error();
	}

	if(iErrorCode == SSL_ERROR_SYSCALL && i == 1)
	{
		//ERR_clear_error();
		return FALSE;
	}

	return TRUE;
}

CSSLSession* CSSLSessionPool::PickFreeSession(LPCSTR lpszHostName)
{
	DWORD dwIndex;
	CSSLSession* pSession = nullptr;

	if(m_lsFreeSession.TryLock(&pSession, dwIndex))
	{
		if(::GetTimeGap32(pSession->GetFreeTime()) >= m_dwSessionLockTime)
			VERIFY(m_lsFreeSession.ReleaseLock(nullptr, dwIndex));
		else
		{
			VERIFY(m_lsFreeSession.ReleaseLock(pSession, dwIndex));
			pSession = nullptr;
		}
	}

	if(!pSession) pSession = CSSLSession::Construct(m_itPool);

	ASSERT(pSession);
	return pSession->Renew(m_sslCtx, lpszHostName);
}

void CSSLSessionPool::PutFreeSession(CSSLSession* pSession)
{
	if(pSession->Reset())
	{
		ReleaseGCSession();

		if(!m_lsFreeSession.TryPut(pSession))
			m_lsGCSession.PushBack(pSession);
	}
}

void CSSLSessionPool::ReleaseGCSession(BOOL bForce)
{
	::ReleaseGCObj(m_lsGCSession, m_dwSessionLockTime, bForce);
}

void CSSLSessionPool::Prepare()
{
	m_itPool.Prepare();
	m_lsFreeSession.Reset(m_dwSessionPoolSize);
}

void CSSLSessionPool::Clear()
{
	CSSLSession* pSession = nullptr;

	while(m_lsFreeSession.TryGet(&pSession))
		CSSLSession::Destruct(pSession);

	VERIFY(m_lsFreeSession.IsEmpty());
	m_lsFreeSession.Reset();

	ReleaseGCSession(TRUE);
	VERIFY(m_lsGCSession.IsEmpty());

	m_itPool.Clear();
}

#endif