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

/******************************************************************************
Module:  HPSocket

Usage:
		方法一：
		--------------------------------------------------------------------------------------
		0. 应用程序包含 HPTypeDef.h / SocketInterface.h / HPSocket.h 头文件
		1. 调用 HP_Create_Xxx() 函数创建 HPSocket 对象
		2. 使用完毕后调用 HP_Destroy_Xxx() 函数销毁 HPSocket 对象

		方法二：
		--------------------------------------------------------------------------------------
		0. 应用程序包含 SocketInterface.h 和 HPSocket.h 头文件
		1. 创建 CXxxPtr 智能指针，通过智能指针使用 HPSocket 对象

Release:
		<-- 动态链接库 -->
		1. x86/HPSocket.dll				- (32位/MBCS/Release)
		2. x86/HPSocket_D.dll			- (32位/MBCS/DeBug)
		3. x86/HPSocket_U.dll			- (32位/UNICODE/Release)
		4. x86/HPSocket_UD.dll			- (32位/UNICODE/DeBug)
		5. x64/HPSocket.dll				- (64位/MBCS/Release)
		6. x64/HPSocket_D.dll			- (64位/MBCS/DeBug)
		7. x64/HPSocket_U.dll			- (64位/UNICODE/Release)
		8. x64/HPSocket_UD.dll			- (64位/UNICODE/DeBug)

		<-- 静态链接库 -->
		!!注意!!：使用 HPSocket 静态库时，需要在工程属性中定义预处理宏 -> HPSOCKET_STATIC_LIB
		1. x86/static/HPSocket.lib		- (32位/MBCS/Release)
		2. x86/static/HPSocket_D.lib	- (32位/MBCS/DeBug)
		3. x86/static/HPSocket_U.lib	- (32位/UNICODE/Release)
		4. x86/static/HPSocket_UD.lib	- (32位/UNICODE/DeBug)
		5. x64/static/HPSocket.lib		- (64位/MBCS/Release)
		6. x64/static/HPSocket_D.lib	- (64位/MBCS/DeBug)
		7. x64/static/HPSocket_U.lib	- (64位/UNICODE/Release)
		8. x64/static/HPSocket_UD.lib	- (64位/UNICODE/DeBug)

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
/************** HPSocket 对象智能指针 **************/

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
/**************** HPSocket 导出函数 ****************/

// 创建 ITcpServer 对象
HPSOCKET_API ITcpServer* HP_Create_TcpServer(ITcpServerListener* pListener);
// 创建 ITcpAgent 对象
HPSOCKET_API ITcpAgent* HP_Create_TcpAgent(ITcpAgentListener* pListener);
// 创建 ITcpClient 对象
HPSOCKET_API ITcpClient* HP_Create_TcpClient(ITcpClientListener* pListener);
// 创建 ITcpPullServer 对象
HPSOCKET_API ITcpPullServer* HP_Create_TcpPullServer(ITcpServerListener* pListener);
// 创建 ITcpPullAgent 对象
HPSOCKET_API ITcpPullAgent* HP_Create_TcpPullAgent(ITcpAgentListener* pListener);
// 创建 ITcpPullClient 对象
HPSOCKET_API ITcpPullClient* HP_Create_TcpPullClient(ITcpClientListener* pListener);
// 创建 ITcpPackServer 对象
HPSOCKET_API ITcpPackServer* HP_Create_TcpPackServer(ITcpServerListener* pListener);
// 创建 ITcpPackAgent 对象
HPSOCKET_API ITcpPackAgent* HP_Create_TcpPackAgent(ITcpAgentListener* pListener);
// 创建 ITcpPackClient 对象
HPSOCKET_API ITcpPackClient* HP_Create_TcpPackClient(ITcpClientListener* pListener);
// 创建 IUdpServer 对象
HPSOCKET_API IUdpServer* HP_Create_UdpServer(IUdpServerListener* pListener);
// 创建 IUdpClient 对象
HPSOCKET_API IUdpClient* HP_Create_UdpClient(IUdpClientListener* pListener);
// 创建 IUdpCast 对象
HPSOCKET_API IUdpCast* HP_Create_UdpCast(IUdpCastListener* pListener);

// 销毁 ITcpServer 对象
HPSOCKET_API void HP_Destroy_TcpServer(ITcpServer* pServer);
// 销毁 ITcpAgent 对象
HPSOCKET_API void HP_Destroy_TcpAgent(ITcpAgent* pAgent);
// 销毁 ITcpClient 对象
HPSOCKET_API void HP_Destroy_TcpClient(ITcpClient* pClient);
// 销毁 ITcpPullServer 对象
HPSOCKET_API void HP_Destroy_TcpPullServer(ITcpPullServer* pServer);
// 销毁 ITcpPullAgent 对象
HPSOCKET_API void HP_Destroy_TcpPullAgent(ITcpPullAgent* pAgent);
// 销毁 ITcpPullClient 对象
HPSOCKET_API void HP_Destroy_TcpPullClient(ITcpPullClient* pClient);
// 销毁 ITcpPackServer 对象
HPSOCKET_API void HP_Destroy_TcpPackServer(ITcpPackServer* pServer);
// 销毁 ITcpPackAgent 对象
HPSOCKET_API void HP_Destroy_TcpPackAgent(ITcpPackAgent* pAgent);
// 销毁 ITcpPackClient 对象
HPSOCKET_API void HP_Destroy_TcpPackClient(ITcpPackClient* pClient);
// 销毁 IUdpServer 对象
HPSOCKET_API void HP_Destroy_UdpServer(IUdpServer* pServer);
// 销毁 IUdpClient 对象
HPSOCKET_API void HP_Destroy_UdpClient(IUdpClient* pClient);
// 销毁 IUdpCast 对象
HPSOCKET_API void HP_Destroy_UdpCast(IUdpCast* pCast);

// ITcpServer 对象创建器
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

// ITcpAgent 对象创建器
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

// ITcpClient 对象创建器
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

// ITcpPullServer 对象创建器
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

// ITcpPullAgent 对象创建器
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

// ITcpPullClient 对象创建器
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

// ITcpPackServer 对象创建器
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

// ITcpPackAgent 对象创建器
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

// ITcpPackClient 对象创建器
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

// IUdpServer 对象创建器
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

// IUdpClient 对象创建器
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

// IUdpClient 对象创建器
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

// ITcpServer 对象智能指针
typedef CHPSocketPtr<ITcpServer, ITcpServerListener, TcpServer_Creator>			CTcpServerPtr;
// ITcpAgent 对象智能指针
typedef CHPSocketPtr<ITcpAgent, ITcpAgentListener, TcpAgent_Creator>			CTcpAgentPtr;
// ITcpClient 对象智能指针
typedef CHPSocketPtr<ITcpClient, ITcpClientListener, TcpClient_Creator>			CTcpClientPtr;
// ITcpPullServer 对象智能指针
typedef CHPSocketPtr<ITcpPullServer, ITcpServerListener, TcpPullServer_Creator>	CTcpPullServerPtr;
// ITcpPullAgent 对象智能指针
typedef CHPSocketPtr<ITcpPullAgent, ITcpAgentListener, TcpPullAgent_Creator>	CTcpPullAgentPtr;
// ITcpPullClient 对象智能指针
typedef CHPSocketPtr<ITcpPullClient, ITcpClientListener, TcpPullClient_Creator>	CTcpPullClientPtr;
// ITcpPackServer 对象智能指针
typedef CHPSocketPtr<ITcpPackServer, ITcpServerListener, TcpPackServer_Creator>	CTcpPackServerPtr;
// ITcpPackAgent 对象智能指针
typedef CHPSocketPtr<ITcpPackAgent, ITcpAgentListener, TcpPackAgent_Creator>	CTcpPackAgentPtr;
// ITcpPackClient 对象智能指针
typedef CHPSocketPtr<ITcpPackClient, ITcpClientListener, TcpPackClient_Creator>	CTcpPackClientPtr;
// IUdpServer 对象智能指针
typedef CHPSocketPtr<IUdpServer, IUdpServerListener, UdpServer_Creator>			CUdpServerPtr;
// IUdpClient 对象智能指针
typedef CHPSocketPtr<IUdpClient, IUdpClientListener, UdpClient_Creator>			CUdpClientPtr;
// IUdpCast 对象智能指针
typedef CHPSocketPtr<IUdpCast, IUdpCastListener, UdpCast_Creator>				CUdpCastPtr;

/*****************************************************************************************************************************************************/
/******************************************************************** HTTP Exports *******************************************************************/
/*****************************************************************************************************************************************************/

// 创建 IHttpServer 对象
HPSOCKET_API IHttpServer* HP_Create_HttpServer(IHttpServerListener* pListener);
// 创建 IHttpAgent 对象
HPSOCKET_API IHttpAgent* HP_Create_HttpAgent(IHttpAgentListener* pListener);
// 创建 IHttpClient 对象
HPSOCKET_API IHttpClient* HP_Create_HttpClient(IHttpClientListener* pListener);
// 创建 IHttpSyncClient 对象
HPSOCKET_API IHttpSyncClient* HP_Create_HttpSyncClient(IHttpClientListener* pListener);

// 销毁 IHttpServer 对象
HPSOCKET_API void HP_Destroy_HttpServer(IHttpServer* pServer);
// 销毁 IHttpAgent 对象
HPSOCKET_API void HP_Destroy_HttpAgent(IHttpAgent* pAgent);
// 销毁 IHttpClient 对象
HPSOCKET_API void HP_Destroy_HttpClient(IHttpClient* pClient);
// 销毁 IHttpSyncClient 对象
HPSOCKET_API void HP_Destroy_HttpSyncClient(IHttpSyncClient* pClient);

// IHttpServer 对象创建器
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

// IHttpAgent 对象创建器
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

// IHttpClient 对象创建器
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

// IHttpSyncClient 对象创建器
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

// IHttpServer 对象智能指针
typedef CHPSocketPtr<IHttpServer, IHttpServerListener, HttpServer_Creator>			CHttpServerPtr;
// IHttpAgent 对象智能指针
typedef CHPSocketPtr<IHttpAgent, IHttpAgentListener, HttpAgent_Creator>				CHttpAgentPtr;
// IHttpClient 对象智能指针
typedef CHPSocketPtr<IHttpClient, IHttpClientListener, HttpClient_Creator>			CHttpClientPtr;
// IHttpSyncClient 对象智能指针
typedef CHPSocketPtr<IHttpSyncClient, IHttpClientListener, HttpSyncClient_Creator>	CHttpSyncClientPtr;

/**************************************************************************/
/*************************** HTTP Cookie 管理方法 **************************/

/* 从文件加载 Cookie */
HPSOCKET_API BOOL HP_HttpCookie_MGR_LoadFromFile(LPCSTR lpszFile, BOOL bKeepExists = TRUE);
/* 保存 Cookie 到文件 */
HPSOCKET_API BOOL HP_HttpCookie_MGR_SaveToFile(LPCSTR lpszFile, BOOL bKeepExists = TRUE);
/* 清理 Cookie */
HPSOCKET_API BOOL HP_HttpCookie_MGR_ClearCookies(LPCSTR lpszDomain = nullptr, LPCSTR lpszPath = nullptr);
/* 清理过期 Cookie */
HPSOCKET_API BOOL HP_HttpCookie_MGR_RemoveExpiredCookies(LPCSTR lpszDomain = nullptr, LPCSTR lpszPath = nullptr);
/* 设置 Cookie */
HPSOCKET_API BOOL HP_HttpCookie_MGR_SetCookie(LPCSTR lpszName, LPCSTR lpszValue, LPCSTR lpszDomain, LPCSTR lpszPath, int iMaxAge = -1, BOOL bHttpOnly = FALSE, BOOL bSecure = FALSE, int enSameSite = 0, BOOL bOnlyUpdateValueIfExists = TRUE);
/* 删除 Cookie */
HPSOCKET_API BOOL HP_HttpCookie_MGR_DeleteCookie(LPCSTR lpszDomain, LPCSTR lpszPath, LPCSTR lpszName);
/* 设置是否允许第三方 Cookie */
HPSOCKET_API void HP_HttpCookie_MGR_SetEnableThirdPartyCookie(BOOL bEnableThirdPartyCookie = TRUE);
/* 检查是否允许第三方 Cookie */
HPSOCKET_API BOOL HP_HttpCookie_MGR_IsEnableThirdPartyCookie();

/* Cookie expires 字符串转换为整数 */
HPSOCKET_API BOOL HP_HttpCookie_HLP_ParseExpires(LPCSTR lpszExpires, __time64_t& tmExpires);
/* 整数转换为 Cookie expires 字符串 */
HPSOCKET_API BOOL HP_HttpCookie_HLP_MakeExpiresStr(char lpszBuff[], int& iBuffLen, __time64_t tmExpires);
/* 生成 Cookie 字符串 */
HPSOCKET_API BOOL HP_HttpCookie_HLP_ToString(char lpszBuff[], int& iBuffLen, LPCSTR lpszName, LPCSTR lpszValue, LPCSTR lpszDomain, LPCSTR lpszPath, int iMaxAge /*= -1*/, BOOL bHttpOnly /*= FALSE*/, BOOL bSecure /*= FALSE*/, int enSameSite /*= 0*/);
/* 获取当前 UTC 时间 */
HPSOCKET_API __time64_t HP_HttpCookie_HLP_CurrentUTCTime();
/* Max-Age -> expires */
HPSOCKET_API __time64_t HP_HttpCookie_HLP_MaxAgeToExpires(int iMaxAge);
/* expires -> Max-Age */
HPSOCKET_API int HP_HttpCookie_HLP_ExpiresToMaxAge(__time64_t tmExpires);

/*****************************************************************************************************************************************************/
/*************************************************************** Global Function Exports *************************************************************/
/*****************************************************************************************************************************************************/

// 获取 HPSocket 版本号（4 个字节分别为：主版本号，子版本号，修正版本号，构建编号）
HPSOCKET_API DWORD HP_GetHPSocketVersion();

// 获取错误描述文本
HPSOCKET_API LPCTSTR HP_GetSocketErrorDesc(EnSocketError enCode);
// 调用系统的 GetLastError() 方法获取系统错误代码
HPSOCKET_API DWORD SYS_GetLastError	();
// 调用系统的 WSAGetLastError() 方法获取系统错误代码
HPSOCKET_API int SYS_WSAGetLastError();
// 调用系统的 setsockopt()
HPSOCKET_API int SYS_SetSocketOption(SOCKET sock, int level, int name, LPVOID val, int len);
// 调用系统的 getsockopt()
HPSOCKET_API int SYS_GetSocketOption(SOCKET sock, int level, int name, LPVOID val, int* len);
// 调用系统的 ioctlsocket()
HPSOCKET_API int SYS_IoctlSocket(SOCKET sock, long cmd, u_long* arg);
// 调用系统的 WSAIoctl()
HPSOCKET_API int SYS_WSAIoctl(SOCKET sock, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer, LPDWORD lpcbBytesReturned);

// 设置 socket 选项：IPPROTO_TCP -> TCP_NODELAY
HPSOCKET_API int SYS_SSO_NoDelay(SOCKET sock, BOOL bNoDelay);
// 设置 socket 选项：SOL_SOCKET -> SO_DONTLINGER
HPSOCKET_API int SYS_SSO_DontLinger(SOCKET sock, BOOL bDont);
// 设置 socket 选项：SOL_SOCKET -> SO_LINGER
HPSOCKET_API int SYS_SSO_Linger(SOCKET sock, USHORT l_onoff, USHORT l_linger);
// 设置 socket 选项：SOL_SOCKET -> SO_RCVBUF
HPSOCKET_API int SYS_SSO_RecvBuffSize(SOCKET sock, int size);
// 设置 socket 选项：SOL_SOCKET -> SO_SNDBUF
HPSOCKET_API int SYS_SSO_SendBuffSize(SOCKET sock, int size);
// 设置 socket 选项：SOL_SOCKET -> SO_REUSEADDR
HPSOCKET_API int SYS_SSO_ReuseAddress(SOCKET sock, BOOL bReuse);

// 获取 SOCKET 本地地址信息
HPSOCKET_API BOOL SYS_GetSocketLocalAddress(SOCKET socket, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);
// 获取 SOCKET 远程地址信息
HPSOCKET_API BOOL SYS_GetSocketRemoteAddress(SOCKET socket, TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);

/* 枚举主机 IP 地址 */
HPSOCKET_API BOOL SYS_EnumHostIPAddresses(LPCTSTR lpszHost, EnIPAddrType enType, LPTIPAddr** lpppIPAddr, int& iIPAddrCount);
/* 释放 LPTIPAddr* */
HPSOCKET_API BOOL SYS_FreeHostIPAddresses(LPTIPAddr* lppIPAddr);
/* 检查字符串是否符合 IP 地址格式 */
HPSOCKET_API BOOL SYS_IsIPAddress(LPCTSTR lpszAddress, EnIPAddrType* penType = nullptr);
/* 通过主机名获取 IP 地址 */
HPSOCKET_API BOOL SYS_GetIPAddress(LPCTSTR lpszHost, TCHAR lpszIP[], int& iIPLenth, EnIPAddrType& enType);

/* 64 位网络字节序转主机字节序 */
HPSOCKET_API ULONGLONG SYS_NToH64(ULONGLONG value);
/* 64 位主机字节序转网络字节序 */
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

// 普通压缩（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
HPSOCKET_API int SYS_Compress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// 高级压缩（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
//（默认参数：iLevel -> -1，iMethod -> 8，iWindowBits -> 15，iMemLevel -> 8，iStrategy -> 0）
HPSOCKET_API int SYS_CompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iLevel = -1, int iMethod = 8, int iWindowBits = 15, int iMemLevel = 8, int iStrategy = 0);
// 普通解压（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
HPSOCKET_API int SYS_Uncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// 高级解压（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
//（默认参数：iWindowBits -> 15）
HPSOCKET_API int SYS_UncompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iWindowBits = 15);
// 推测压缩结果长度
HPSOCKET_API DWORD SYS_GuessCompressBound(DWORD dwSrcLen, BOOL bGZip = FALSE);

// Gzip 压缩（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
HPSOCKET_API int SYS_GZipCompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// Gzip 解压（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
HPSOCKET_API int SYS_GZipUncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// 推测 Gzip 解压结果长度（如果返回 0 或不合理值则说明输入内容并非有效的 Gzip 格式）
HPSOCKET_API DWORD SYS_GZipGuessUncompressBound(const BYTE* lpszSrc, DWORD dwSrcLen);

// 计算 Base64 编码后长度
HPSOCKET_API DWORD SYS_GuessBase64EncodeBound(DWORD dwSrcLen);
// 计算 Base64 解码后长度
HPSOCKET_API DWORD SYS_GuessBase64DecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen);
// Base64 编码（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
HPSOCKET_API int SYS_Base64Encode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// Base64 解码（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
HPSOCKET_API int SYS_Base64Decode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);

// 计算 URL 编码后长度
HPSOCKET_API DWORD SYS_GuessUrlEncodeBound(const BYTE* lpszSrc, DWORD dwSrcLen);
// 计算 URL 解码后长度
HPSOCKET_API DWORD SYS_GuessUrlDecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen);
// URL 编码（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
HPSOCKET_API int SYS_UrlEncode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// URL 解码（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
HPSOCKET_API int SYS_UrlDecode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
