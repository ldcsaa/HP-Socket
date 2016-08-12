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
 
#include "stdafx.h"
#include "SSLHelper.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "openssl/engine.h"
#include "openssl/x509v3.h"
#include "../../Common/Src/WaitFor.h"

#include <atlpath.h>

CSSLContext CSSLContext::sm_Instance;
CSSLContext& g_SSL = CSSLContext::getInstance();

const DWORD CSSLSessionPool::DEFAULT_ITEM_CAPACITY		= CItemPool::DEFAULT_ITEM_CAPACITY;
const DWORD CSSLSessionPool::DEFAULT_ITEM_POOL_SIZE		= CItemPool::DEFAULT_POOL_SIZE;
const DWORD CSSLSessionPool::DEFAULT_ITEM_POOL_HOLD		= CItemPool::DEFAULT_POOL_HOLD;
const DWORD CSSLSessionPool::DEFAULT_SESSION_LOCK_TIME	= 10 * 1000;
const DWORD CSSLSessionPool::DEFAULT_SESSION_POOL_SIZE	= 150;
const DWORD CSSLSessionPool::DEFAULT_SESSION_POOL_HOLD	= 600;

BOOL CSSLContext::Initialize(EnSSLSessionMode enSessionMode, int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath)
{
	ASSERT(!IsValid());

	if(IsValid())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	CreateContext();

	m_enSessionMode = enSessionMode;

	if(!LoadCertAndKey(iVerifyMode, lpszPemCertFile, lpszPemKeyFile, lpszKeyPasswod, lpszCAPemCertFileOrPath))
	{
		Cleanup();
		return FALSE;
	}

	return m_bValid = TRUE;
}

void CSSLContext::CreateContext()
{
	m_iLockNum = ::CRYPTO_num_locks();

	if(m_iLockNum > 0)
		m_pcsLocks = new CSimpleRWLock[m_iLockNum];
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
}

BOOL CSSLContext::LoadCertAndKey(int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath)
{
	USES_CONVERSION;

	m_sslCtx = SSL_CTX_new(SSLv23_method());
	SSL_CTX_set_verify(m_sslCtx, iVerifyMode, nullptr);

	if(lpszCAPemCertFileOrPath != nullptr)
	{
		LPCTSTR lpszCAPemCertFile = nullptr;
		LPCTSTR lpszCAPemCertPath = nullptr;

		if(!ATLPath::FileExists(lpszCAPemCertFileOrPath))
		{
			::SetLastError(ERROR_FILE_NOT_FOUND);
			return FALSE;
		}

		if(!ATLPath::IsDirectory(lpszCAPemCertFileOrPath))
			lpszCAPemCertFile = lpszCAPemCertFileOrPath;
		else
			lpszCAPemCertPath = lpszCAPemCertFileOrPath;

		if(!SSL_CTX_load_verify_locations(m_sslCtx, T2CA(lpszCAPemCertFile), T2CA(lpszCAPemCertPath)))
		{
			::SetLastError(ERROR_INVALID_DATA);
			return FALSE;
		}

		if(!SSL_CTX_set_default_verify_paths(m_sslCtx))
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

			SSL_CTX_set_client_CA_list(m_sslCtx, caCertNames);
		}
	}

	if(lpszPemCertFile != nullptr)
	{
		if(	!ATLPath::FileExists(lpszPemCertFile)	||
			ATLPath::IsDirectory(lpszPemCertFile)	)
		{
			::SetLastError(ERROR_FILE_NOT_FOUND);
			return FALSE;
		}

		if(	lpszPemKeyFile == nullptr				||
			!ATLPath::FileExists(lpszPemKeyFile)	||
			ATLPath::IsDirectory(lpszPemKeyFile)	)
		{
			::SetLastError(ERROR_FILE_NOT_FOUND);
			return FALSE;
		}
		
		if(lpszKeyPasswod != nullptr)
			SSL_CTX_set_default_passwd_cb_userdata(m_sslCtx, (void*)T2CA(lpszKeyPasswod));

		if(!SSL_CTX_use_PrivateKey_file(m_sslCtx, T2CA(lpszPemKeyFile), SSL_FILETYPE_PEM))
		{
			::SetLastError(ERROR_INVALID_PASSWORD);
			return FALSE;
		}

		if(!SSL_CTX_use_certificate_chain_file(m_sslCtx, T2CA(lpszPemCertFile)))
		{
			::SetLastError(ERROR_INVALID_DATA);
			return FALSE;
		}

		if(!SSL_CTX_check_private_key(m_sslCtx))
		{
			::SetLastError(ERROR_INVALID_ACCESS);
			return FALSE;
		}
	}

	return TRUE;
}

void CSSLContext::Cleanup()
{
	m_bValid = FALSE;

	if(m_sslCtx != nullptr)
	{
		SSL_CTX_free(m_sslCtx);
		m_sslCtx = nullptr;
	}

	CleanupThreadState();
	CleanupGlobalState();

	CRYPTO_set_locking_callback			(nullptr);
	CRYPTO_set_dynlock_create_callback	(nullptr);
	CRYPTO_set_dynlock_destroy_callback	(nullptr);
	CRYPTO_set_dynlock_lock_callback	(nullptr);

	if(m_iLockNum > 0)
	{
		delete[] m_pcsLocks;

		m_pcsLocks = nullptr;
		m_iLockNum = 0;
	}
}

void CSSLContext::CleanupGlobalState()
{
	CONF_modules_free();
	ENGINE_cleanup();
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();
	ERR_free_strings();
	SSL_COMP_free_compression_methods();
}

void CSSLContext::CleanupThreadState()
{
	ERR_remove_thread_state(0);
}

void CSSLContext::ssl_lock_callback(int mode, int n, const char *file, int line)
{
	mode & CRYPTO_LOCK
	? (mode & CRYPTO_READ
		? g_SSL.m_pcsLocks[n].WaitToRead()
		: g_SSL.m_pcsLocks[n].WaitToWrite())
	: (mode & CRYPTO_READ
		? g_SSL.m_pcsLocks[n].ReadDone()
		: g_SSL.m_pcsLocks[n].WriteDone());
}

CRYPTO_dynlock_value* CSSLContext::ssl_lock_dyn_create_callback(const char *file, int line)
 {
	 return new DynamicLock;
 }

void CSSLContext::ssl_lock_dyn_callback(int mode, CRYPTO_dynlock_value* l, const char *file, int line)
{
	mode & CRYPTO_LOCK
	? (mode & CRYPTO_READ
		? l->cs.WaitToRead()
		: l->cs.WaitToWrite())
	: (mode & CRYPTO_READ
		? l->cs.ReadDone()
		: l->cs.WriteDone());
}

void CSSLContext::ssl_lock_dyn_destroy_callback(CRYPTO_dynlock_value* l, const char *file, int line)
{
	delete l;
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

		if(!WriteSendChannel((const BYTE*)buffer.buf, buffer.len))
		{
			isOK = FALSE;
			break;
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

CSSLSession* CSSLSession::Renew()
{
	ASSERT(!IsValid());

	m_ssl		= SSL_new(g_SSL.GetContext());
	m_bioSend	= BIO_new(BIO_s_mem());
	m_bioRecv	= BIO_new(BIO_s_mem());

	SSL_set_bio(m_ssl, m_bioRecv, m_bioSend);

	g_SSL.GetSessionMode() == SSL_SM_CLIENT
	? SSL_connect(m_ssl) : SSL_accept(m_ssl);

	m_pitSend		= m_itPool.PickFreeItem();
	m_pitRecv		= m_itPool.PickFreeItem();
	m_bufSend.buf	= (char*)m_pitSend->Ptr();
	m_bufRecv.buf	= (char*)m_pitRecv->Ptr();
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
	int iCode	= iErrorCode;

	for(; iCode != SSL_ERROR_NONE; i++)
	{
#ifdef _DEBUG
		ERR_error_string_n(iCode, szBuffer, sizeof(szBuffer));
		TRACE("	> SSL Error: %d - %s\n", iCode, szBuffer);
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

CSSLSession* CSSLSessionPool::PickFreeSession()
{
	DWORD dwIndex;
	CSSLSession* pSession = nullptr;

	if(m_lsFreeSession.TryLock(&pSession, dwIndex))
	{
		if(::GetTimeGap32(pSession->GetFreeTime()) >= m_dwSessionLockTime)
			m_lsFreeSession.ReleaseLock(nullptr, dwIndex);
		else
		{
			m_lsFreeSession.ReleaseLock(pSession, dwIndex);
			pSession = nullptr;
		}
	}

	if(!pSession) pSession = new CSSLSession(m_itPool);

	ASSERT(pSession);
	return pSession->Renew();
}

void CSSLSessionPool::PutFreeSession(CSSLSession* pSession)
{
	if(pSession->Reset())
	{
		if(!m_lsFreeSession.TryPut(pSession))
		{
			m_lsGCSession.PushBack(pSession);

			if(m_lsGCSession.Size() > m_dwSessionPoolSize)
				ReleaseGCSession();
		}
	}
}

void CSSLSessionPool::ReleaseGCSession(BOOL bForce)
{
	CSSLSession* pSession	= nullptr;
	DWORD now				= ::TimeGetTime();

	while(m_lsGCSession.PopFront(&pSession))
	{
		if(bForce || (now - pSession->GetFreeTime()) >= m_dwSessionLockTime)
			delete pSession;
		else
		{
			m_lsGCSession.PushBack(pSession);
			break;
		}
	}
}

void CSSLSessionPool::Prepare()
{
	m_itPool.Prepare();
	m_lsFreeSession.Reset(m_dwSessionPoolHold);
}

void CSSLSessionPool::Clear()
{
	CSSLSession* pSession = nullptr;

	while(m_lsFreeSession.TryGet(&pSession))
		delete pSession;

	VERIFY(m_lsFreeSession.IsEmpty());
	m_lsFreeSession.Reset();

	ReleaseGCSession(TRUE);
	VERIFY(m_lsGCSession.IsEmpty());

	m_itPool.Clear();
}
