/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 5.0.1
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

/******************************************************************************
Module:  HPSocket

Usage:
		����һ��
		--------------------------------------------------------------------------------------
		0. Ӧ�ó������ HPTypeDef.h / SocketInterface.h / HPSocket.h ͷ�ļ�
		1. ���� HP_Create_Xxx() �������� HPSocket ����
		2. ʹ����Ϻ���� HP_Destroy_Xxx() �������� HPSocket ����

		��������
		--------------------------------------------------------------------------------------
		0. Ӧ�ó������ SocketInterface.h �� HPSocket.h ͷ�ļ�
		1. ���� CXxxPtr ����ָ�룬ͨ������ָ��ʹ�� HPSocket ����

Release:
		<-- ��̬���ӿ� -->
		1. x86/HPSocket.dll				- (32λ/MBCS/Release)
		2. x86/HPSocket_D.dll			- (32λ/MBCS/DeBug)
		3. x86/HPSocket_U.dll			- (32λ/UNICODE/Release)
		4. x86/HPSocket_UD.dll			- (32λ/UNICODE/DeBug)
		5. x64/HPSocket.dll				- (64λ/MBCS/Release)
		6. x64/HPSocket_D.dll			- (64λ/MBCS/DeBug)
		7. x64/HPSocket_U.dll			- (64λ/UNICODE/Release)
		8. x64/HPSocket_UD.dll			- (64λ/UNICODE/DeBug)

		<-- ��̬���ӿ� -->
		!!ע��!!��ʹ�� HPSocket ��̬��ʱ����Ҫ�ڹ��������ж���Ԥ����� -> HPSOCKET_STATIC_LIB
		1. x86/static/HPSocket.lib		- (32λ/MBCS/Release)
		2. x86/static/HPSocket_D.lib	- (32λ/MBCS/DeBug)
		3. x86/static/HPSocket_U.lib	- (32λ/UNICODE/Release)
		4. x86/static/HPSocket_UD.lib	- (32λ/UNICODE/DeBug)
		5. x64/static/HPSocket.lib		- (64λ/MBCS/Release)
		6. x64/static/HPSocket_D.lib	- (64λ/MBCS/DeBug)
		7. x64/static/HPSocket_U.lib	- (64λ/UNICODE/Release)
		8. x64/static/HPSocket_UD.lib	- (64λ/UNICODE/DeBug)

******************************************************************************/

#pragma once

/**************************************************/
/*********** imports / exports HPSocket ***********/

#ifdef HPSOCKET_STATIC_LIB
	#define HPSOCKET_API EXTERN_C
#else
	#ifdef HPSOCKET_EXPORTS
		#define HPSOCKET_API EXTERN_C __declspec(dllexport)
	#else
		#define HPSOCKET_API EXTERN_C __declspec(dllimport)
	#endif
#endif

#include "SocketInterface.h"

/*****************************************************************************************************************************************************/
/****************************************************************** TCP/UDP Exports ******************************************************************/
/*****************************************************************************************************************************************************/

/**************************************************/
/************** HPSocket ��������ָ�� **************/

template<class T, class _Listener, class _Creator> class CHPSocketPtr
{
public:
	CHPSocketPtr(_Listener* pListener)
	{
		m_pObj = _Creator::Create(pListener);
	}

	CHPSocketPtr() : m_pObj(nullptr)
	{

	}

	~CHPSocketPtr()
	{
		Reset();
	}

public:
	CHPSocketPtr& Reset(T* pObj = nullptr)
	{
		if(pObj != m_pObj)
		{
			if(m_pObj)
				_Creator::Destroy(m_pObj);

			m_pObj = pObj;
		}

		return *this;
	}

	CHPSocketPtr& Attach(T* pObj)
	{
		return Reset(pObj);
	}

	T* Detach()
	{
		T* pObj	= m_pObj;
		m_pObj	= nullptr;

		return pObj;
	}

	BOOL IsValid	()	const	{return m_pObj != nullptr	;}
	T* Get			()	const	{return m_pObj				;}
	T* operator ->	()	const	{return m_pObj				;}
	operator T*		()	const	{return m_pObj				;}

	CHPSocketPtr& operator = (T* pObj)	{return Reset(pObj)	;}

private:
	CHPSocketPtr(const CHPSocketPtr&);
	CHPSocketPtr& operator = (const CHPSocketPtr&);

private:
	T* m_pObj;
};

/**************************************************/
/**************** HPSocket �������� ****************/

// ���� ITcpServer ����
HPSOCKET_API ITcpServer* HP_Create_TcpServer(ITcpServerListener* pListener);
// ���� ITcpAgent ����
HPSOCKET_API ITcpAgent* HP_Create_TcpAgent(ITcpAgentListener* pListener);
// ���� ITcpClient ����
HPSOCKET_API ITcpClient* HP_Create_TcpClient(ITcpClientListener* pListener);
// ���� ITcpPullServer ����
HPSOCKET_API ITcpPullServer* HP_Create_TcpPullServer(ITcpServerListener* pListener);
// ���� ITcpPullAgent ����
HPSOCKET_API ITcpPullAgent* HP_Create_TcpPullAgent(ITcpAgentListener* pListener);
// ���� ITcpPullClient ����
HPSOCKET_API ITcpPullClient* HP_Create_TcpPullClient(ITcpClientListener* pListener);
// ���� ITcpPackServer ����
HPSOCKET_API ITcpPackServer* HP_Create_TcpPackServer(ITcpServerListener* pListener);
// ���� ITcpPackAgent ����
HPSOCKET_API ITcpPackAgent* HP_Create_TcpPackAgent(ITcpAgentListener* pListener);
// ���� ITcpPackClient ����
HPSOCKET_API ITcpPackClient* HP_Create_TcpPackClient(ITcpClientListener* pListener);
// ���� IUdpServer ����
HPSOCKET_API IUdpServer* HP_Create_UdpServer(IUdpServerListener* pListener);
// ���� IUdpClient ����
HPSOCKET_API IUdpClient* HP_Create_UdpClient(IUdpClientListener* pListener);
// ���� IUdpCast ����
HPSOCKET_API IUdpCast* HP_Create_UdpCast(IUdpCastListener* pListener);

// ���� ITcpServer ����
HPSOCKET_API void HP_Destroy_TcpServer(ITcpServer* pServer);
// ���� ITcpAgent ����
HPSOCKET_API void HP_Destroy_TcpAgent(ITcpAgent* pAgent);
// ���� ITcpClient ����
HPSOCKET_API void HP_Destroy_TcpClient(ITcpClient* pClient);
// ���� ITcpPullServer ����
HPSOCKET_API void HP_Destroy_TcpPullServer(ITcpPullServer* pServer);
// ���� ITcpPullAgent ����
HPSOCKET_API void HP_Destroy_TcpPullAgent(ITcpPullAgent* pAgent);
// ���� ITcpPullClient ����
HPSOCKET_API void HP_Destroy_TcpPullClient(ITcpPullClient* pClient);
// ���� ITcpPackServer ����
HPSOCKET_API void HP_Destroy_TcpPackServer(ITcpPackServer* pServer);
// ���� ITcpPackAgent ����
HPSOCKET_API void HP_Destroy_TcpPackAgent(ITcpPackAgent* pAgent);
// ���� ITcpPackClient ����
HPSOCKET_API void HP_Destroy_TcpPackClient(ITcpPackClient* pClient);
// ���� IUdpServer ����
HPSOCKET_API void HP_Destroy_UdpServer(IUdpServer* pServer);
// ���� IUdpClient ����
HPSOCKET_API void HP_Destroy_UdpClient(IUdpClient* pClient);
// ���� IUdpCast ����
HPSOCKET_API void HP_Destroy_UdpCast(IUdpCast* pCast);

// ITcpServer ���󴴽���
struct TcpServer_Creator
{
	static ITcpServer* Create(ITcpServerListener* pListener)
	{
		return HP_Create_TcpServer(pListener);
	}

	static void Destroy(ITcpServer* pServer)
	{
		HP_Destroy_TcpServer(pServer);
	}
};

// ITcpAgent ���󴴽���
struct TcpAgent_Creator
{
	static ITcpAgent* Create(ITcpAgentListener* pListener)
	{
		return HP_Create_TcpAgent(pListener);
	}

	static void Destroy(ITcpAgent* pAgent)
	{
		HP_Destroy_TcpAgent(pAgent);
	}
};

// ITcpClient ���󴴽���
struct TcpClient_Creator
{
	static ITcpClient* Create(ITcpClientListener* pListener)
	{
		return HP_Create_TcpClient(pListener);
	}

	static void Destroy(ITcpClient* pClient)
	{
		HP_Destroy_TcpClient(pClient);
	}
};

// ITcpPullServer ���󴴽���
struct TcpPullServer_Creator
{
	static ITcpPullServer* Create(ITcpServerListener* pListener)
	{
		return HP_Create_TcpPullServer(pListener);
	}

	static void Destroy(ITcpPullServer* pServer)
	{
		HP_Destroy_TcpPullServer(pServer);
	}
};

// ITcpPullAgent ���󴴽���
struct TcpPullAgent_Creator
{
	static ITcpPullAgent* Create(ITcpAgentListener* pListener)
	{
		return HP_Create_TcpPullAgent(pListener);
	}

	static void Destroy(ITcpPullAgent* pAgent)
	{
		HP_Destroy_TcpPullAgent(pAgent);
	}
};

// ITcpPullClient ���󴴽���
struct TcpPullClient_Creator
{
	static ITcpPullClient* Create(ITcpClientListener* pListener)
	{
		return HP_Create_TcpPullClient(pListener);
	}

	static void Destroy(ITcpPullClient* pClient)
	{
		HP_Destroy_TcpPullClient(pClient);
	}
};

// ITcpPackServer ���󴴽���
struct TcpPackServer_Creator
{
	static ITcpPackServer* Create(ITcpServerListener* pListener)
	{
		return HP_Create_TcpPackServer(pListener);
	}

	static void Destroy(ITcpPackServer* pServer)
	{
		HP_Destroy_TcpPackServer(pServer);
	}
};

// ITcpPackAgent ���󴴽���
struct TcpPackAgent_Creator
{
	static ITcpPackAgent* Create(ITcpAgentListener* pListener)
	{
		return HP_Create_TcpPackAgent(pListener);
	}

	static void Destroy(ITcpPackAgent* pAgent)
	{
		HP_Destroy_TcpPackAgent(pAgent);
	}
};

// ITcpPackClient ���󴴽���
struct TcpPackClient_Creator
{
	static ITcpPackClient* Create(ITcpClientListener* pListener)
	{
		return HP_Create_TcpPackClient(pListener);
	}

	static void Destroy(ITcpPackClient* pClient)
	{
		HP_Destroy_TcpPackClient(pClient);
	}
};

// IUdpServer ���󴴽���
struct UdpServer_Creator
{
	static IUdpServer* Create(IUdpServerListener* pListener)
	{
		return HP_Create_UdpServer(pListener);
	}

	static void Destroy(IUdpServer* pServer)
	{
		HP_Destroy_UdpServer(pServer);
	}
};

// IUdpClient ���󴴽���
struct UdpClient_Creator
{
	static IUdpClient* Create(IUdpClientListener* pListener)
	{
		return HP_Create_UdpClient(pListener);
	}

	static void Destroy(IUdpClient* pClient)
	{
		HP_Destroy_UdpClient(pClient);
	}
};

// IUdpClient ���󴴽���
struct UdpCast_Creator
{
	static IUdpCast* Create(IUdpCastListener* pListener)
	{
		return HP_Create_UdpCast(pListener);
	}

	static void Destroy(IUdpCast* pCast)
	{
		HP_Destroy_UdpCast(pCast);
	}
};

// ITcpServer ��������ָ��
typedef CHPSocketPtr<ITcpServer, ITcpServerListener, TcpServer_Creator>			CTcpServerPtr;
// ITcpAgent ��������ָ��
typedef CHPSocketPtr<ITcpAgent, ITcpAgentListener, TcpAgent_Creator>			CTcpAgentPtr;
// ITcpClient ��������ָ��
typedef CHPSocketPtr<ITcpClient, ITcpClientListener, TcpClient_Creator>			CTcpClientPtr;
// ITcpPullServer ��������ָ��
typedef CHPSocketPtr<ITcpPullServer, ITcpServerListener, TcpPullServer_Creator>	CTcpPullServerPtr;
// ITcpPullAgent ��������ָ��
typedef CHPSocketPtr<ITcpPullAgent, ITcpAgentListener, TcpPullAgent_Creator>	CTcpPullAgentPtr;
// ITcpPullClient ��������ָ��
typedef CHPSocketPtr<ITcpPullClient, ITcpClientListener, TcpPullClient_Creator>	CTcpPullClientPtr;
// ITcpPackServer ��������ָ��
typedef CHPSocketPtr<ITcpPackServer, ITcpServerListener, TcpPackServer_Creator>	CTcpPackServerPtr;
// ITcpPackAgent ��������ָ��
typedef CHPSocketPtr<ITcpPackAgent, ITcpAgentListener, TcpPackAgent_Creator>	CTcpPackAgentPtr;
// ITcpPackClient ��������ָ��
typedef CHPSocketPtr<ITcpPackClient, ITcpClientListener, TcpPackClient_Creator>	CTcpPackClientPtr;
// IUdpServer ��������ָ��
typedef CHPSocketPtr<IUdpServer, IUdpServerListener, UdpServer_Creator>			CUdpServerPtr;
// IUdpClient ��������ָ��
typedef CHPSocketPtr<IUdpClient, IUdpClientListener, UdpClient_Creator>			CUdpClientPtr;
// IUdpCast ��������ָ��
typedef CHPSocketPtr<IUdpCast, IUdpCastListener, UdpCast_Creator>				CUdpCastPtr;

/*****************************************************************************************************************************************************/
/******************************************************************** HTTP Exports *******************************************************************/
/*****************************************************************************************************************************************************/

// ���� IHttpServer ����
HPSOCKET_API IHttpServer* HP_Create_HttpServer(IHttpServerListener* pListener);
// ���� IHttpAgent ����
HPSOCKET_API IHttpAgent* HP_Create_HttpAgent(IHttpAgentListener* pListener);
// ���� IHttpClient ����
HPSOCKET_API IHttpClient* HP_Create_HttpClient(IHttpClientListener* pListener);
// ���� IHttpSyncClient ����
HPSOCKET_API IHttpSyncClient* HP_Create_HttpSyncClient(IHttpClientListener* pListener);

// ���� IHttpServer ����
HPSOCKET_API void HP_Destroy_HttpServer(IHttpServer* pServer);
// ���� IHttpAgent ����
HPSOCKET_API void HP_Destroy_HttpAgent(IHttpAgent* pAgent);
// ���� IHttpClient ����
HPSOCKET_API void HP_Destroy_HttpClient(IHttpClient* pClient);
// ���� IHttpSyncClient ����
HPSOCKET_API void HP_Destroy_HttpSyncClient(IHttpSyncClient* pClient);

// IHttpServer ���󴴽���
struct HttpServer_Creator
{
	static IHttpServer* Create(IHttpServerListener* pListener)
	{
		return HP_Create_HttpServer(pListener);
	}

	static void Destroy(IHttpServer* pServer)
	{
		HP_Destroy_HttpServer(pServer);
	}
};

// IHttpAgent ���󴴽���
struct HttpAgent_Creator
{
	static IHttpAgent* Create(IHttpAgentListener* pListener)
	{
		return HP_Create_HttpAgent(pListener);
	}

	static void Destroy(IHttpAgent* pAgent)
	{
		HP_Destroy_HttpAgent(pAgent);
	}
};

// IHttpClient ���󴴽���
struct HttpClient_Creator
{
	static IHttpClient* Create(IHttpClientListener* pListener)
	{
		return HP_Create_HttpClient(pListener);
	}

	static void Destroy(IHttpClient* pClient)
	{
		HP_Destroy_HttpClient(pClient);
	}
};

// IHttpSyncClient ���󴴽���
struct HttpSyncClient_Creator
{
	static IHttpSyncClient* Create(IHttpClientListener* pListener)
	{
		return HP_Create_HttpSyncClient(pListener);
	}

	static void Destroy(IHttpSyncClient* pClient)
	{
		HP_Destroy_HttpSyncClient(pClient);
	}
};

// IHttpServer ��������ָ��
typedef CHPSocketPtr<IHttpServer, IHttpServerListener, HttpServer_Creator>			CHttpServerPtr;
// IHttpAgent ��������ָ��
typedef CHPSocketPtr<IHttpAgent, IHttpAgentListener, HttpAgent_Creator>				CHttpAgentPtr;
// IHttpClient ��������ָ��
typedef CHPSocketPtr<IHttpClient, IHttpClientListener, HttpClient_Creator>			CHttpClientPtr;
// IHttpSyncClient ��������ָ��
typedef CHPSocketPtr<IHttpSyncClient, IHttpClientListener, HttpSyncClient_Creator>	CHttpSyncClientPtr;

/**************************************************************************/
/*************************** HTTP Cookie ������ **************************/

/* ���ļ����� Cookie */
HPSOCKET_API BOOL HP_HttpCookie_MGR_LoadFromFile(LPCSTR lpszFile, BOOL bKeepExists = TRUE);
/* ���� Cookie ���ļ� */
HPSOCKET_API BOOL HP_HttpCookie_MGR_SaveToFile(LPCSTR lpszFile, BOOL bKeepExists = TRUE);
/* ���� Cookie */
HPSOCKET_API BOOL HP_HttpCookie_MGR_ClearCookies(LPCSTR lpszDomain = nullptr, LPCSTR lpszPath = nullptr);
/* ������� Cookie */
HPSOCKET_API BOOL HP_HttpCookie_MGR_RemoveExpiredCookies(LPCSTR lpszDomain = nullptr, LPCSTR lpszPath = nullptr);
/* ���� Cookie */
HPSOCKET_API BOOL HP_HttpCookie_MGR_SetCookie(LPCSTR lpszName, LPCSTR lpszValue, LPCSTR lpszDomain, LPCSTR lpszPath, int iMaxAge = -1, BOOL bHttpOnly = FALSE, BOOL bSecure = FALSE, int enSameSite = 0, BOOL bOnlyUpdateValueIfExists = TRUE);
/* ɾ�� Cookie */
HPSOCKET_API BOOL HP_HttpCookie_MGR_DeleteCookie(LPCSTR lpszDomain, LPCSTR lpszPath, LPCSTR lpszName);
/* �����Ƿ���������� Cookie */
HPSOCKET_API void HP_HttpCookie_MGR_SetEnableThirdPartyCookie(BOOL bEnableThirdPartyCookie = TRUE);
/* ����Ƿ���������� Cookie */
HPSOCKET_API BOOL HP_HttpCookie_MGR_IsEnableThirdPartyCookie();

/* Cookie expires �ַ���ת��Ϊ���� */
HPSOCKET_API BOOL HP_HttpCookie_HLP_ParseExpires(LPCSTR lpszExpires, __time64_t& tmExpires);
/* ����ת��Ϊ Cookie expires �ַ��� */
HPSOCKET_API BOOL HP_HttpCookie_HLP_MakeExpiresStr(char lpszBuff[], int& iBuffLen, __time64_t tmExpires);
/* ���� Cookie �ַ��� */
HPSOCKET_API BOOL HP_HttpCookie_HLP_ToString(char lpszBuff[], int& iBuffLen, LPCSTR lpszName, LPCSTR lpszValue, LPCSTR lpszDomain, LPCSTR lpszPath, int iMaxAge /*= -1*/, BOOL bHttpOnly /*= FALSE*/, BOOL bSecure /*= FALSE*/, int enSameSite /*= 0*/);
/* ��ȡ��ǰ UTC ʱ�� */
HPSOCKET_API __time64_t HP_HttpCookie_HLP_CurrentUTCTime();
/* Max-Age -> expires */
HPSOCKET_API __time64_t HP_HttpCookie_HLP_MaxAgeToExpires(int iMaxAge);
/* expires -> Max-Age */
HPSOCKET_API int HP_HttpCookie_HLP_ExpiresToMaxAge(__time64_t tmExpires);

/*****************************************************************************************************************************************************/
/*************************************************************** Global Function Exports *************************************************************/
/*****************************************************************************************************************************************************/

// ��ȡ HPSocket �汾�ţ�4 ���ֽڷֱ�Ϊ�����汾�ţ��Ӱ汾�ţ������汾�ţ�������ţ�
HPSOCKET_API DWORD HP_GetHPSocketVersion();

// ��ȡ���������ı�
HPSOCKET_API LPCTSTR HP_GetSocketErrorDesc(EnSocketError enCode);
// ����ϵͳ�� GetLastError() ������ȡϵͳ�������
HPSOCKET_API DWORD SYS_GetLastError	();
// ����ϵͳ�� WSAGetLastError() ������ȡϵͳ�������
HPSOCKET_API int SYS_WSAGetLastError();
// ����ϵͳ�� setsockopt()
HPSOCKET_API int SYS_SetSocketOption(SOCKET sock, int level, int name, LPVOID val, int len);
// ����ϵͳ�� getsockopt()
HPSOCKET_API int SYS_GetSocketOption(SOCKET sock, int level, int name, LPVOID val, int* len);
// ����ϵͳ�� ioctlsocket()
HPSOCKET_API int SYS_IoctlSocket(SOCKET sock, long cmd, u_long* arg);
// ����ϵͳ�� WSAIoctl()
HPSOCKET_API int SYS_WSAIoctl(SOCKET sock, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer, LPDWORD lpcbBytesReturned);

// ���� socket ѡ�IPPROTO_TCP -> TCP_NODELAY
HPSOCKET_API int SYS_SSO_NoDelay(SOCKET sock, BOOL bNoDelay);
// ���� socket ѡ�SOL_SOCKET -> SO_DONTLINGER
HPSOCKET_API int SYS_SSO_DontLinger(SOCKET sock, BOOL bDont);
// ���� socket ѡ�SOL_SOCKET -> SO_LINGER
HPSOCKET_API int SYS_SSO_Linger(SOCKET sock, USHORT l_onoff, USHORT l_linger);
// ���� socket ѡ�SOL_SOCKET -> SO_RCVBUF
HPSOCKET_API int SYS_SSO_RecvBuffSize(SOCKET sock, int size);
// ���� socket ѡ�SOL_SOCKET -> SO_SNDBUF
HPSOCKET_API int SYS_SSO_SendBuffSize(SOCKET sock, int size);
// ���� socket ѡ�SOL_SOCKET -> SO_REUSEADDR
HPSOCKET_API int SYS_SSO_ReuseAddress(SOCKET sock, BOOL bReuse);

// ��ȡ SOCKET ���ص�ַ��Ϣ
HPSOCKET_API BOOL SYS_GetSocketLocalAddress(SOCKET socket, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);
// ��ȡ SOCKET Զ�̵�ַ��Ϣ
HPSOCKET_API BOOL SYS_GetSocketRemoteAddress(SOCKET socket, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);

/* ö������ IP ��ַ */
HPSOCKET_API BOOL SYS_EnumHostIPAddresses(LPCTSTR lpszHost, EnIPAddrType enType, LPTIPAddr** lpppIPAddr, int& iIPAddrCount);
/* �ͷ� LPTIPAddr* */
HPSOCKET_API BOOL SYS_FreeHostIPAddresses(LPTIPAddr* lppIPAddr);
/* ����ַ����Ƿ���� IP ��ַ��ʽ */
HPSOCKET_API BOOL SYS_IsIPAddress(LPCTSTR lpszAddress, EnIPAddrType* penType = nullptr);
/* ͨ����������ȡ IP ��ַ */
HPSOCKET_API BOOL SYS_GetIPAddress(LPCTSTR lpszHost, TCHAR lpszIP[], int& iIPLenth, EnIPAddrType& enType);

/* 64 λ�����ֽ���ת�����ֽ��� */
HPSOCKET_API ULONGLONG SYS_NToH64(ULONGLONG value);
/* 64 λ�����ֽ���ת�����ֽ��� */
HPSOCKET_API ULONGLONG SYS_HToN64(ULONGLONG value);

// CP_XXX -> UNICODE
HPSOCKET_API BOOL SYS_CodePageToUnicode(int iCodePage, const char szSrc[], WCHAR szDest[], int& iDestLength);
// UNICODE -> CP_XXX
HPSOCKET_API BOOL SYS_UnicodeToCodePage(int iCodePage, const WCHAR szSrc[], char szDest[], int& iDestLength);
// GBK -> UNICODE
HPSOCKET_API BOOL SYS_GbkToUnicode(const char szSrc[], WCHAR szDest[], int& iDestLength);
// UNICODE -> GBK
HPSOCKET_API BOOL SYS_UnicodeToGbk(const WCHAR szSrc[], char szDest[], int& iDestLength);
// UTF8 -> UNICODE
HPSOCKET_API BOOL SYS_Utf8ToUnicode(const char szSrc[], WCHAR szDest[], int& iDestLength);
// UNICODE -> UTF8
HPSOCKET_API BOOL SYS_UnicodeToUtf8(const WCHAR szSrc[], char szDest[], int& iDestLength);
// GBK -> UTF8
HPSOCKET_API BOOL SYS_GbkToUtf8(const char szSrc[], char szDest[], int& iDestLength);
// UTF8 -> GBK
HPSOCKET_API BOOL SYS_Utf8ToGbk(const char szSrc[], char szDest[], int& iDestLength);

// ��ͨѹ��������ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
HPSOCKET_API int SYS_Compress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// �߼�ѹ��������ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
//��Ĭ�ϲ�����iLevel -> -1��iMethod -> 8��iWindowBits -> 15��iMemLevel -> 8��iStrategy -> 0��
HPSOCKET_API int SYS_CompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iLevel = -1, int iMethod = 8, int iWindowBits = 15, int iMemLevel = 8, int iStrategy = 0);
// ��ͨ��ѹ������ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
HPSOCKET_API int SYS_Uncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// �߼���ѹ������ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
//��Ĭ�ϲ�����iWindowBits -> 15��
HPSOCKET_API int SYS_UncompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iWindowBits = 15);
// �Ʋ�ѹ���������
HPSOCKET_API DWORD SYS_GuessCompressBound(DWORD dwSrcLen, BOOL bGZip = FALSE);

// Gzip ѹ��������ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
HPSOCKET_API int SYS_GZipCompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// Gzip ��ѹ������ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
HPSOCKET_API int SYS_GZipUncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// �Ʋ� Gzip ��ѹ������ȣ�������� 0 �򲻺���ֵ��˵���������ݲ�����Ч�� Gzip ��ʽ��
HPSOCKET_API DWORD SYS_GZipGuessUncompressBound(const BYTE* lpszSrc, DWORD dwSrcLen);

// ���� Base64 ����󳤶�
HPSOCKET_API DWORD SYS_GuessBase64EncodeBound(DWORD dwSrcLen);
// ���� Base64 ����󳤶�
HPSOCKET_API DWORD SYS_GuessBase64DecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen);
// Base64 ���루����ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
HPSOCKET_API int SYS_Base64Encode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// Base64 ���루����ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
HPSOCKET_API int SYS_Base64Decode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);

// ���� URL ����󳤶�
HPSOCKET_API DWORD SYS_GuessUrlEncodeBound(const BYTE* lpszSrc, DWORD dwSrcLen);
// ���� URL ����󳤶�
HPSOCKET_API DWORD SYS_GuessUrlDecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen);
// URL ���루����ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
HPSOCKET_API int SYS_UrlEncode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// URL ���루����ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
HPSOCKET_API int SYS_UrlDecode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
