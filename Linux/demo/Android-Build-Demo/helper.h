#pragma once

#include "src/common/GeneralHelper.h"

#ifdef _USE_HP_LIB
#include <hpsocket/HPTypeDef.h>
#else
#include "src/HPTypeDef.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/socket.h>

#define EVT_ON_SEND				_T("OnSend")
#define EVT_ON_RECEIVE			_T("OnReceive")
#define EVT_ON_CLOSE			_T("OnClose")
#define EVT_ON_ERROR			_T("OnError")
#define EVT_ON_PREPARE_CONNECT	_T("OnPrepareConnect")
#define EVT_ON_PREPARE_LISTEN	_T("OnPrepareListen")
#define EVT_ON_ACCEPT			_T("OnAccept")
#define EVT_ON_CONNECT			_T("OnConnect")
#define EVT_ON_HAND_SHAKE		_T("OnHandShake")
#define EVT_ON_SHUTDOWN			_T("OnShutdown")
#define EVT_ON_END_TEST			_T("END TEST")
#define EVT_ON_STAT_TEST		_T("STAT TEST")

#define EVT_ON_MESSAGE_BEGIN	_T("OnMessageBegin")
#define EVT_ON_REQUEST_LINE		_T("OnRequestLine")
#define EVT_ON_STATUS_LINE		_T("OnStatusLine")
#define EVT_ON_HEADER			_T("OnHeader")
#define EVT_ON_HEADERS_COMPLETE	_T("OnHeadersComplete")
#define EVT_ON_BODY				_T("OnBody")
#define EVT_ON_CHUNK_HEADER		_T("OnChunkHeader")
#define EVT_ON_CHUNK_COMPLETE	_T("OnChunkComplete")
#define EVT_ON_MESSAGE_COMPLETE	_T("OnMessageComplete")
#define EVT_ON_UPGRADE			_T("OnUpgrade")
#define EVT_ON_PARSE_ERROR		_T("OnParseError")

#define EVT_ON_WS_MSG_HEADER	_T("OnWSMessageHeader")
#define EVT_ON_WS_MSG_BODY		_T("OnWSMessageBody")
#define EVT_ON_WS_MSG_COMPLETE	_T("OnWSMessageComplete")

#define EVT_ON_UNCOMPRESS_BODY		_T("Uncompress Body")
#define EVT_ON_UNCOMPRESS_BODY_FAIL	_T("Uncompress Body Fail")

#define IPV4_LOOPBACK_ADDRESS	_T("127.0.0.1")
#define IPV6_LOOPBACK_ADDRESS	_T("::1")
#define IPV4_ANY_ADDRESS		_T("0.0.0.0")
#define IPV6_ANY_ADDRESS		_T("::")
#define DEF_BROAD_CAST_ADDRESS	_T("233.0.0.1")
#define DEF_TCP_UDP_PORT		5555
#define DEF_HTTP_PORT			8080
#define DEF_HTTPS_PORT			8443

#define TCP_KEEPALIVE_TIME		(30 * 1000)
#define UDP_DETECT_ATTEMPTS		3

enum EnAppState
{
	ST_STARTING, ST_STARTED, ST_CONNECTING, ST_CONNECTED, ST_STOPPING, ST_STOPPED
};

struct app_arg
{
	static char OPTIONS[];

	// -a
	CString remote_addr;
	// -p
	USHORT port;
	// -b
	CString bind_addr;
	// -d
	USHORT local_port;
	// -j
	CString reject_addr;
	// -n
	bool async;
	// -t
	DWORD thread_count;
	// -e
	DWORD test_times;
	// -i
	DWORD test_interval;
	// -c
	DWORD conn_count;
	// -l
	DWORD data_length;
	// -s
	EnSendPolicy send_policy;
	// -m
	DWORD max_conn;
	// -q
	bool keep_alive;

	// -o
	EnCastMode cast_mode;
	// -r
	bool reuse_addr;
	// -u
	bool ip_loop;
	// -k
	int ttl;

	// -x
	USHORT http_port;
	// -y
	USHORT https_port;
	// -z
	bool http_use_cookie;
	// -w
	bool http_with_listener;

public:
	void ParseArgs(int argc, char* const argv[]);
	void ShowPFMTestArgs(BOOL bAgent);
	static void PrintUsage();
	static void PrintVersion();
public:
	app_arg();
	~app_arg();
};

extern app_arg g_app_arg;

class CAppSignalHandler
{
public:

	CAppSignalHandler(const vector<int>&& signals, BOOL bIgnoreSigInt = TRUE)
	: m_bIgnoreSigInt(bIgnoreSigInt)
	{
		sigset_t ss;
		sigemptyset(&ss);

		for(size_t i = 0; i < signals.size(); i++)
			sigaddset(&ss, signals[i]);

		sh.Setup(this, &CAppSignalHandler::handle_sig, &ss);
	}

	int WaitForExit()
	{
		int rs = evt.Wait();
		
		PRINTLN(" bye ~ bye ~");

		return rs;
	}

private:

	void handle_sig(const siginfo_t* pSigInfo)
	{
		if(pSigInfo->si_signo == SIGINT)
			m_bIgnoreSigInt || evt.Set();
	}

private:
	CSignalHandler<CAppSignalHandler> sh;
	CPipeEvent evt;

	BOOL m_bIgnoreSigInt;
};

class CTermAttrInitializer
{
public:
	CTermAttrInitializer(tcflag_t l_mask = 0)
	{
		tcgetattr(STDIN_FILENO, &old_attr);

		termios new_attr = old_attr;

		new_attr.c_lflag	&= ~l_mask;
		new_attr.c_cc[VERASE]= 0x08;
		new_attr.c_cc[VTIME] = 0;
		new_attr.c_cc[VMIN]  = 1;

		tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_attr);
	}

	~CTermAttrInitializer() {tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_attr);}

private:
	termios old_attr;
};

class CCommandParser
{
public:
	using CMD_FUNC = void (*)(CCommandParser*);

	enum EnAppType {AT_SERVER, AT_AGENT, AT_CLIENT};
	enum EnCmdType {CT_START = 0, CT_STOP, CT_STATUS, CT_CONNECT, CT_SEND, CT_PAUSE, CT_KICK, CT_KICK_L, CT_KICK_S, CT_STAT, CT_MAX};

protected:

	struct TCmdNameFunc
	{
		LPCTSTR name;
		CMD_FUNC func;
	};

public:
	BOOL Run();
	void PrintStatus(EnServiceState enStatus, LPCTSTR lpszName = nullptr);

public:
	CCommandParser(EnAppType enAppType, CMD_FUNC fnCmds[CT_MAX]);
	virtual ~CCommandParser() {}

private:
	BOOL WaitForExit();
	void WorkerProc(PVOID pv);
	void Parse(LPTSTR lpszLine, SSIZE_T nSize);

protected:
	virtual void ParseCmdArgs(EnCmdType type, LPTSTR lpszArg);
	virtual void Reset();

	void PrintUsage();
	virtual void PrintCmdUsage();
	virtual CString GetCmdUsage(EnCmdType type);


public:
	BOOL	m_bFlag;
	CONNID	m_dwConnID;
	CString	m_strMessage;
	CString	m_strRemoteAddr;
	USHORT	m_usRemotePort;
	DWORD	m_dwSeconds;

protected:
	EnAppType	 m_enAppType;
	TCmdNameFunc m_szCmdNameFuncs[CT_MAX];

	CThread<CCommandParser, VOID, VOID> m_thWorker;
};

#ifdef _NEED_HTTP

class CHttpCommandParser : public CCommandParser
{
	using __super = CCommandParser;

public:
	CHttpCommandParser(EnAppType enAppType, CMD_FUNC fnCmds[CT_MAX])
	: __super(enAppType, fnCmds)
	{
	}

protected:
	virtual void ParseCmdArgs(EnCmdType type, LPTSTR lpszArg) override;
	virtual void Reset() override;

	virtual void PrintCmdUsage() override;
	virtual CString GetCmdUsage(EnCmdType type) override;

private:
	BOOL ParseCmdOptions(LPCTSTR lpszArg, LPCTSTR lpszOptions);

public:
	BOOL	m_bHttps;
	CString m_strPath;
	CString m_strMethod;
	CString m_strData;
	CString m_strFilePath;
	vector<CString> m_vtHeaders;
};

#endif

struct server_statistics_info
{
	volatile LONG m_lClientCount;
	volatile LONGLONG m_llTotalReceived;
	volatile LONGLONG m_llTotalSent;

	CCriSec m_cs;

	void Reset(BOOL bResetClientCount = TRUE);
	void CheckClientCount();
	void CheckStatistics();
	void AddTotalRecv(int iLength);
	void AddTotalSend(int iLength);

};

struct client_statistics_info
{
	volatile LONGLONG m_llTotalReceived;
	volatile LONGLONG m_llTotalSent;
	LONGLONG m_llExpectReceived;
	DWORD m_dwBeginTickCount;
	DWORD m_dwTimeconsuming;

	volatile int m_iConnected;

	void Reset();
	void StartTest();
	void CheckStatistics(BOOL bCheckSend = TRUE);
	void AddTotalRecv(int iLength);
	void AddTotalSend(int iLength);

	void TermConnected();
	void AddConnected();
	int GetConnected();
};

struct info_msg
{
	LPCTSTR name;
	CONNID connID;
	LPCTSTR evt;
	int contentLength;
	LPCTSTR content;

	static info_msg* Construct(CONNID dwConnID, LPCTSTR lpszEvent, int iContentLength = 0, LPCTSTR lpszContent = nullptr, LPCTSTR lpszName = nullptr);
	static void Destruct(info_msg* pMsg);

private:
	info_msg(CONNID dwConnID, LPCTSTR lpszEvent, int iContentLength = 0, LPCTSTR lpszContent = nullptr, LPCTSTR lpszName = nullptr);
	~info_msg();
};

struct TPkgHeader 
{
	DWORD seq;
	int body_len;
};

struct TPkgBody 
{
	char name[30];
	short age;
	char desc[1];
};

struct TPkgInfo
{
	bool is_header;
	int length;

	TPkgInfo(bool header = true, int len = sizeof(TPkgHeader)) : is_header(header), length(len) {}
	void Reset() {is_header = true, length = sizeof(TPkgHeader);}
	~TPkgInfo() {}
};

inline TPkgInfo* ConstructPkgInfo()
{
	return new TPkgInfo(true, sizeof(TPkgHeader));
}

inline void DestructPkgInfo(TPkgInfo* pInfo)
{
	delete pInfo;
}

template<class T, typename = enable_if_t<!is_void<decay_t<T>>::value>>
inline TPkgInfo* CreatePkgInfo(T* pSender, CONNID dwConnID)
{
	TPkgInfo* pInfo = ConstructPkgInfo();
	pSender->SetConnectionExtra(dwConnID, pInfo);

	return pInfo;
}

template<class T, typename = enable_if_t<!is_void<decay_t<T>>::value>>
inline TPkgInfo* FindPkgInfo(T* pSender, CONNID dwConnID)
{
	PVOID pInfo = nullptr;
	pSender->GetConnectionExtra(dwConnID, &pInfo);

	return (TPkgInfo*)pInfo;
}

template<class T, typename = enable_if_t<!is_void<decay_t<T>>::value>>
inline void RemovePkgInfo(T* pSender, CONNID dwConnID)
{
	TPkgInfo* pInfo = FindPkgInfo(pSender, dwConnID);
	ASSERT(pInfo != nullptr);

	DestructPkgInfo(pInfo);
}

CBufferPtr* GeneratePkgBuffer(DWORD seq, LPCTSTR lpszName, short age, LPCTSTR lpszDesc);
CBufferPtr* GeneratePkgBuffer(const TPkgHeader& header, const TPkgBody& body);

BOOL SplitStr(LPCTSTR pszSrc, vector<CString>& vtItem, LPCTSTR pszSepectors = nullptr, LPCTSTR pszQMarks = nullptr);

sa_family_t GuessAddrFamily(LPCTSTR lpszAddress);
LPCTSTR GetLoopbackAddress(LPCTSTR lpszLikeAddress);
LPCTSTR GetAnyAddress(LPCTSTR lpszLikeAddress);

void LogServerStart(LPCTSTR lpszAddress, USHORT port, LPCTSTR lpszName = nullptr);
void LogServerStartFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName = nullptr);
void LogServerStop(LPCTSTR lpszName = nullptr);
void LogServerStopFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName = nullptr);
void LogAgentStart(LPCTSTR lpszAddress, BOOL bAsync, LPCTSTR lpszName = nullptr);
void LogAgentStarting(LPCTSTR lpszAddress, BOOL bAsync, LPCTSTR lpszName = nullptr);
void LogAgentStartFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName = nullptr);
void LogAgentStopping(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void LogAgentStop(LPCTSTR lpszName = nullptr);
void LogAgentStopFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName = nullptr);
void LogAgentSendFail(int iSequence, int iSocketIndex, DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName = nullptr);
void LogClientStart(LPCTSTR lpszAddress, USHORT port, LPCTSTR lpszName = nullptr);
void LogClientStarting(LPCTSTR lpszAddress, USHORT port, LPCTSTR lpszName = nullptr);
void LogClientStartFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName = nullptr);
void LogClientStopping(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void LogClientStop(LPCTSTR lpszName = nullptr);
void LogClientStopFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName = nullptr);
void LogClientSendFail(int iSequence, int iSocketIndex, DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName = nullptr);
void LogSend(CONNID dwConnID, LPCTSTR lpszContent, LPCTSTR lpszName = nullptr);
void LogSending(CONNID dwConnID, LPCTSTR lpszContent, LPCTSTR lpszName = nullptr);
void LogSendFail(CONNID dwConnID, DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName = nullptr);
void LogDisconnect(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void LogDisconnectFail(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void LogDisconnect2(CONNID dwConnID, BOOL bForce, LPCTSTR lpszName = nullptr);
void LogDisconnectFail2(CONNID dwConnID, BOOL bForce, LPCTSTR lpszName = nullptr);
void LogDisconnectLong(DWORD dwSeconds, BOOL bForce, LPCTSTR lpszName = nullptr);
void LogDisconnectFailLong(DWORD dwSeconds, BOOL bForce, LPCTSTR lpszName = nullptr);
void LogPause(CONNID dwConnID, BOOL bPause, LPCTSTR lpszName = nullptr);
void LogPauseFail(CONNID dwConnID, BOOL bPause, LPCTSTR lpszName = nullptr);
void LogConnect(LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName = nullptr);
void LogConnectFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName = nullptr);
void LogRelease(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void LogReleaseFail(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void LogDetect(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void LogDetectFail(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void LogOnConnect(CONNID dwConnID, const CString& strAddress, USHORT usPort, LPCTSTR lpszName = nullptr);
void LogOnConnect2(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void LogOnConnect3(CONNID dwConnID, const CString& strAddress, USHORT usPort, LPCTSTR lpszName = nullptr);
void LogOnHandShake2(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void LogOnClose(CONNID dwConnID, LPCTSTR lpszName = nullptr);

void PostOnSend(CONNID dwConnID, const BYTE* pData, int iLength, LPCTSTR lpszName = nullptr);
void PostOnReceive(CONNID dwConnID, const BYTE* pData, int iLength, LPCTSTR lpszName = nullptr);
void PostOnReceiveCast(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, const BYTE* pData, int iLength, LPCTSTR lpszName = nullptr);
void PostOnClose(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void PostOnError(CONNID dwConnID, int enOperation, int iErrorCode, LPCTSTR lpszName = nullptr);
void PostOnAccept(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, BOOL bPass, LPCTSTR lpszName = nullptr);
void PostOnAccept2(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void PostOnHandShake(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void PostOnPrepareListen(LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName = nullptr);
void PostOnPrepareConnect(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void PostOnConnect(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName = nullptr);
void PostOnConnect2(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName = nullptr);
void PostOnConnect3(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void PostOnShutdown(LPCTSTR lpszName = nullptr);
void PostServerStatics(const LONGLONG& llTotalSent, const LONGLONG& llTotalReceived, LPCTSTR lpszName = nullptr);
void PostServerTemporaryStatics(const LONGLONG& llTotalSent, const LONGLONG& llTotalReceived, LPCTSTR lpszName = nullptr);
void PostTimeConsuming(DWORD dwTickCount, LPCTSTR lpszName = nullptr);

#ifdef _NEED_HTTP
void PostOnMessageBegin(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void PostOnRequestLine(CONNID dwConnID, LPCSTR lpszMethod, USHORT usUrlFieldSet, LPCSTR lpszUrl, LPCTSTR lpszName = nullptr);
void PostOnStatusLine(CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc, LPCTSTR lpszName = nullptr);
void PostOnHeader(CONNID dwConnID, LPCSTR lpszHeaderName, LPCSTR lpszHeaderValue, LPCTSTR lpszName = nullptr);
void PostOnHeadersComplete(CONNID dwConnID, LPCSTR lpszSummary, LPCTSTR lpszName = nullptr);
void PostOnBody(CONNID dwConnID, const BYTE* pData, int iLength, LPCTSTR lpszName = nullptr);
void PostOnChunkHeader(CONNID dwConnID, int iLength, LPCTSTR lpszName = nullptr);
void PostOnChunkComplete(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void PostOnMessageComplete(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void PostOnUpgrade(CONNID dwConnID, EnHttpUpgradeType enUpgradeType, LPCTSTR lpszName = nullptr);
void PostOnParseError(CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc, LPCTSTR lpszName = nullptr);

void PostOnWSMessageHeader(CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen, LPCTSTR lpszName = nullptr);
void PostOnWSMessageBody(CONNID dwConnID, const BYTE* pData, int iLength, LPCTSTR lpszName = nullptr);
void PostOnWSMessageComplete(CONNID dwConnID, LPCTSTR lpszName = nullptr);

void PostUncompressBody(CONNID dwConnID, int iLength, LPCTSTR lpszName = nullptr);
void PostUncompressBodyFail(CONNID dwConnID, int iResult, LPCTSTR lpszName = nullptr);
#endif

void PostInfoMsg(info_msg* msg);
void LogInfoMsg(info_msg* pInfoMsg);
void LogMsg(const CString& msg);

extern LPCTSTR g_lpszDefaultCookieFile;

LPCTSTR GetDefaultCookieFile();

#ifdef _NEED_SSL

extern int g_c_iVerifyMode;
extern BOOL g_c_bNeedClientVerification;
extern LPCTSTR g_c_lpszCAPemCertFileOrPath;
extern LPCTSTR g_c_lpszPemCertFile;
extern LPCTSTR g_c_lpszPemKeyFile;
extern LPCTSTR g_c_lpszKeyPasswod;

extern int g_s_iVerifyMode;
extern BOOL g_s_bNeedClientVerification;
extern LPCTSTR g_s_lpszCAPemCertFileOrPath;
extern LPCTSTR g_s_lpszPemCertFile;
extern LPCTSTR g_s_lpszPemKeyFile;
extern LPCTSTR g_s_lpszKeyPasswod;

extern int g_c_iVerifyMode2;
extern BOOL g_c_bNeedClientVerification2;
extern LPCTSTR g_c_lpszCAPemCertFileOrPath2;
extern LPCTSTR g_c_lpszPemCertFile2;
extern LPCTSTR g_c_lpszPemKeyFile2;
extern LPCTSTR g_c_lpszKeyPasswod2;

extern int g_s_iVerifyMode2;
extern BOOL g_s_bNeedClientVerification2;
extern LPCTSTR g_s_lpszCAPemCertFileOrPath2;
extern LPCTSTR g_s_lpszPemCertFile2;
extern LPCTSTR g_s_lpszPemKeyFile2;
extern LPCTSTR g_s_lpszKeyPasswod2;

#endif

#ifdef _NEED_HTTP

#include "src/common/crypto/Crypto.h"

#define HTTP_NAME					_T("http")
#define HTTPS_NAME					_T("https")
#define STR_HTTP_SCHEMA				"http://"
#define STR_HTTPS_SCHEMA			"https://"
#define CRLF						"\r\n"
#define NV_SEPARATOR_CHAR			'='
#define HEADER_SEPARATOR			": "
#define COOKIE_TOKENIZE				"; "
#define STR_HTTP_1_0				"HTTP/1.0"
#define STR_HTTP_1_1				"HTTP/1.1"
#define HOST_HEADER					"Host"
#define COOKIE_HEADER				"Cookie"
#define SET_COOKIE_HEADER			"Set-Cookie"
#define CONTENT_TYPE_HEADER			"Content-Type"
#define CONTENT_LENGTH_HEADER		"Content-Length"
#define TRANSFER_ENCODING_HEADER	"Transfer-Encoding"
#define UPGRADE_HEADER				"Upgrade"
#define WEB_SOCKET_HEADER_VALUE		"WebSocket"

#define HTTP_METHOD_POST			"POST"
#define HTTP_METHOD_PUT				"PUT"
#define HTTP_METHOD_PATCH			"PATCH"
#define HTTP_METHOD_GET				"GET"
#define HTTP_METHOD_DELETE			"DELETE"
#define HTTP_METHOD_HEAD			"HEAD"
#define HTTP_METHOD_TRACE			"TRACE"
#define HTTP_METHOD_OPTIONS			"OPTIONS"
#define HTTP_METHOD_CONNECT			"CONNECT"

#define HTTP_WEB_SOCKET_SEC_SALT	"258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

extern LPCSTR HTTP_WEB_SOCKET_CLOSE_FLAG;
extern const BYTE HTTP_WEB_SOCKET_MASK_KEY[];


struct THttpHeader
{
	CStringA strName;
	CStringA strValue;

	THttpHeader(LPCTSTR lpszName, LPCTSTR lpszValue) : strName(lpszName), strValue(lpszValue) {}

	struct hash
	{
		size_t operator() (const CStringA& str) const
		{
			return hash_value(str);
		}
	};

	struct equal_to
	{
		bool operator () (const CStringA& strA, const CStringA& strB) const
		{
			return strA == strB;
		}
	};

};

typedef unordered_multimap<CStringA, CStringA,
		THttpHeader::hash, THttpHeader::equal_to>	THttpHeaderMap;
typedef THttpHeaderMap::const_iterator				THttpHeaderMapCI;
typedef THttpHeaderMap::iterator					THttpHeaderMapI;

CStringA& HttpVersionToString(EnHttpVersion enVersion, CStringA& strResult);
CStringA& MakeSecWebSocketAccept(LPCSTR lpszKey, CStringA& strAccept);

#endif
