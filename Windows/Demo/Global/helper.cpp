#include "stdafx.h"
#include "helper.h"

#include <ws2tcpip.h>

#if !defined(stscanf_s)
	#ifdef _UNICODE
		#define stscanf_s	swscanf_s
	#else
		#define stscanf_s	sscanf_s
	#endif
#endif


CWnd*		g_pMainWnd;
CListBox*	g_pInfoList;

info_msg* info_msg::Construct(CONNID dwConnID, LPCTSTR lpszEvent, int iContentLength, LPCTSTR lpszContent, LPCTSTR lpszName)
{
	return new info_msg(dwConnID, lpszEvent, iContentLength, lpszContent, lpszName);
}

void info_msg::Destruct(info_msg* pMsg)
{
	delete pMsg;
}

info_msg::info_msg(CONNID dwConnID, LPCTSTR lpszEvent, int iContentLength, LPCTSTR lpszContent, LPCTSTR lpszName)
	: connID(dwConnID), evt(lpszEvent), contentLength(iContentLength), content(lpszContent), name(nullptr)
{
	if(lpszName)
	{
		int len = lstrlen(lpszName);

		if(len > 0)
		{
			name = new TCHAR[len + 1];
			memcpy((LPSTR)name, lpszName, (len + 1) * sizeof(TCHAR));
		}
	}
}

info_msg::~info_msg()
{
	if(name)
		delete[] name;

	if(contentLength > 0)
		delete[] content;
}

void SetMainWnd(CWnd* pWnd)
{
	g_pMainWnd = pWnd;
}

void SetInfoList(CListBox* pInfoList)
{
	g_pInfoList = pInfoList;
}

inline CString SafeString(LPCTSTR lpszName)
{
	CString str(lpszName);

	if(lpszName) str.AppendChar(' ');

	return str;
}

inline CString SafeString2(LPCTSTR lpszName)
{
	CString str(lpszName);

	if(lpszName) str.Append(_T(" #"));

	return str;
}

void LogServerStart(LPCTSTR lpszAddress, USHORT port, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %sServer Start OK --> (%s#%d)"), SafeString(lpszName), lpszAddress, port);
	LogMsg(msg);
}

void LogServerStartFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %sServer Start Fail --> %s (%d) [%d]"), SafeString(lpszName), lpszDesc, code, ::GetLastError());
	LogMsg(msg);
}

void LogServerStop(LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %sServer Stop"), SafeString(lpszName));

	LogMsg(msg);
}

void LogServerStopFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %sServer Stop Fail --> %s (%d)"), SafeString(lpszName), lpszDesc, code);
	LogMsg(msg);
}

void LogClientStart(LPCTSTR lpszAddress, USHORT port, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %sClient Start OK --> (%s#%d)"), SafeString(lpszName), lpszAddress, port);
	LogMsg(msg);
}

void LogStarting(LPCTSTR lpszAddress, USHORT port, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %sStarting ... --> (%s#%d)"), SafeString(lpszName), lpszAddress, port);
	LogMsg(msg);
}

void LogClientStarting(LPCTSTR lpszAddress, USHORT port, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %sClient Starting ... --> (%s#%d)"), SafeString(lpszName), lpszAddress, port);
	LogMsg(msg);
}

void LogClientStartFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %sClient Start Fail --> %s (%d) [%d]"), SafeString(lpszName), lpszDesc, code, ::GetLastError());
	LogMsg(msg);
}

void LogStartFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %sStart Fail --> %s (%d) [%d]"), SafeString(lpszName), lpszDesc, code, ::GetLastError());
	LogMsg(msg);
}

void LogClientStopping(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %sClient Stopping ... --> (%Iu)"), SafeString(lpszName), dwConnID);
	LogMsg(msg);
}

void LogStopping(LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %sStopping ..."), SafeString(lpszName));
	LogMsg(msg);
}

void LogClientStopFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %sClient Stop Fail --> %s (%d)"), SafeString(lpszName), lpszDesc, code);
	LogMsg(msg);
}

void LogClientSendFail(int iSequence, int iSocketIndex, DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %sClient Send Fail [SOCK: %d, SEQ: %d] --> %s (%d)"), SafeString(lpszName), iSocketIndex, iSequence, lpszDesc, code);
	LogMsg(msg);
}

void LogSend(CONNID dwConnID, LPCTSTR lpszContent, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %s(%Iu) Send OK --> %s"), SafeString2(lpszName), dwConnID, lpszContent);
	LogMsg(msg);
}

void LogSend(LPCTSTR lpszContent, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %sSend OK --> %s"), SafeString(lpszName), lpszContent);
	LogMsg(msg);
}

void LogSendFail(CONNID dwConnID, DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %s(%Iu) Send Fail --> %s (%d)"), SafeString2(lpszName), dwConnID, lpszDesc, code, lpszName);
	LogMsg(msg);
}

void LogSendFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %sSend Fail --> %s (%d)"), SafeString(lpszName), lpszDesc, code, lpszName);
	LogMsg(msg);
}

void LogDisconnect(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %s(%Iu) Disconnect OK"), SafeString2(lpszName), dwConnID);
	LogMsg(msg);
}

void LogDisconnectFail(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %s(%Iu) Disconnect Fail"), SafeString2(lpszName), dwConnID, lpszName);
	LogMsg(msg);
}

void LogRelease(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %s(%Iu) Release OK"), SafeString2(lpszName), dwConnID);
	LogMsg(msg);
}

void LogReleaseFail(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %s(%Iu) Release Fail"), SafeString2(lpszName), dwConnID);
	LogMsg(msg);
}

void LogDetect(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %s(%Iu) Detect Connection OK"), SafeString2(lpszName), dwConnID);
	LogMsg(msg);
}

void LogDetectFail(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("$ %s(%Iu) Detect Connection Fail"), SafeString2(lpszName), dwConnID);
	LogMsg(msg);
}

void LogOnConnect(CONNID dwConnID, const CString& strAddress, USHORT usPort, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("local address: %s#%d"), strAddress, usPort);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CONNECT, content_len, lpszContent, lpszName);

	LogInfoMsg(msg);
}

void LogOnConnect2(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("  > [ %s%Iu, %s ]"), SafeString2(lpszName), dwConnID, EVT_ON_CONNECT);
	LogMsg(msg);
}

void LogOnConnect3(CONNID dwConnID, const CString& strAddress, USHORT usPort, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("remote address: %s#%d"), strAddress, usPort);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CONNECT, content_len, lpszContent, lpszName);

	LogInfoMsg(msg);
}

void LogOnHandShake2(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("  > [ %s%Iu, %s ]"), SafeString2(lpszName), dwConnID, EVT_ON_HAND_SHAKE);
	LogMsg(msg);
}

void LogOnClose(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CLOSE, 0, nullptr, lpszName);

	LogInfoMsg(msg);
}

void PostOnSend(CONNID dwConnID, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[20];
	wsprintf(lpszContent, _T("(%d bytes)"), iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_SEND, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnSendTo(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("<%s#%d> (%d bytes)"), lpszAddress, usPort, iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_SEND, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnReceive(CONNID dwConnID, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[20];
	wsprintf(lpszContent, _T("(%d bytes)"), iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_RECEIVE, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnReceiveFrom(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("<%s#%d> (%d bytes)"), lpszAddress, usPort, iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_RECEIVE, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnReceiveCast(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("<%s#%d> (%d bytes)"), lpszAddress, usPort, iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_RECEIVE, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnClose(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CLOSE, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnError(CONNID dwConnID, int enOperation, int iErrorCode, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("OP: %d, CODE: %d"), enOperation, iErrorCode);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_ERROR, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnError2(CONNID dwConnID, int enOperation, int iErrorCode, LPCTSTR lpszAddress, USHORT usPort, const BYTE* pBuffer, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[150];
	wsprintf(lpszContent, _T("<%s#%d> OP: %d, CODE: %d (DATA: 0x%X, LEN: %d>"), lpszAddress, usPort, enOperation, iErrorCode, pBuffer, iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_ERROR, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnAccept(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, BOOL bPass, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("%s (%s#%d)"), bPass ? _T("PASS") : _T("REJECT"), lpszAddress, usPort);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_ACCEPT, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnAccept2(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_ACCEPT, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnHandShake(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_HAND_SHAKE, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnPrepareListen(LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("bind address: %s#%d"), lpszAddress, usPort);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(0, EVT_ON_PREPARE_LISTEN, content_len, lpszContent, lpszName);

	LogInfoMsg(msg);
}

void PostOnPrepareConnect(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_PREPARE_CONNECT, 0, nullptr, lpszName);

	LogInfoMsg(msg);
}

void PostOnConnect(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("local address: %s#%d"), lpszAddress, usPort);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CONNECT, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnConnect2(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("remote address: %s#%d"), lpszAddress, usPort);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CONNECT, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnConnect3(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CONNECT, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnShutdown(LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(0, EVT_ON_SHUTDOWN, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostServerStatics(const LONGLONG& llTotalSent, const LONGLONG& llTotalReceived, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T(" *** Summary: send - %I64d, recv - %I64d"), llTotalSent, llTotalReceived);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(0, EVT_ON_END_TEST, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostTimeConsuming(DWORD dwTickCount, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("Total Time Consuming: %u"), dwTickCount);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(0, EVT_ON_END_TEST, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

#ifdef _NEED_HTTP

void PostOnMessageBegin(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_MESSAGE_BEGIN, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnRequestLine(CONNID dwConnID, LPCSTR lpszMethod, USHORT usUrlFieldSet, LPCSTR lpszUrl, LPCTSTR lpszName)
{
	USES_CONVERSION;

	int content_len		= (int)(strlen(lpszMethod) + strlen(lpszUrl) + 20);
	LPTSTR lpszContent	= new TCHAR[content_len];

	wsprintf(lpszContent, _T("[%s/0x%02X] : %s"), A2T(lpszMethod), usUrlFieldSet, A2T(lpszUrl));
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_REQUEST_LINE, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnStatusLine(CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc, LPCTSTR lpszName)
{
	USES_CONVERSION;

	int content_len		= (int)(strlen(lpszDesc) + 10);
	LPTSTR lpszContent	= new TCHAR[content_len];

	wsprintf(lpszContent, _T("(%u) : %s"), usStatusCode, A2T(lpszDesc));
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_STATUS_LINE, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnHeader(CONNID dwConnID, LPCSTR lpszHeaderName, LPCSTR lpszHeaderValue, LPCTSTR lpszName)
{
	USES_CONVERSION;

	int content_len		= (int)(strlen(lpszHeaderName) + strlen(lpszHeaderValue) + 10);
	LPTSTR lpszContent	= new TCHAR[content_len];

	wsprintf(lpszContent, _T("%s: %s"), A2T(lpszHeaderName), A2T(lpszHeaderValue));
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_HEADER, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnHeadersComplete(CONNID dwConnID, LPCSTR lpszSummary, LPCTSTR lpszName)
{
	USES_CONVERSION;

	static LPCTSTR PREFIX = _T("* * * * * * * * * Summary * * * * * * * * *\r\n");
	static int PREFIX_LEN = lstrlen(PREFIX);

	LPCTSTR lpszSummaryT = A2CT(lpszSummary);


	int content_len		= lstrlen(lpszSummaryT) + PREFIX_LEN + 1;
	LPTSTR lpszContent	= new TCHAR[content_len];

	memcpy(lpszContent, PREFIX, PREFIX_LEN * sizeof(TCHAR));
	memcpy(lpszContent + PREFIX_LEN, lpszSummaryT, (content_len - PREFIX_LEN) * sizeof(TCHAR));

	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_HEADERS_COMPLETE, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnBody(CONNID dwConnID, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[20];
	wsprintf(lpszContent, _T("(%d bytes)"), iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_BODY, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnChunkHeader(CONNID dwConnID, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[20];
	wsprintf(lpszContent, _T("(%d bytes)"), iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CHUNK_HEADER, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnChunkComplete(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CHUNK_COMPLETE, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnMessageComplete(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_MESSAGE_COMPLETE, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnUpgrade(CONNID dwConnID, EnHttpUpgradeType enUpgradeType, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[20];
	wsprintf(lpszContent, _T("(type: %d)"), enUpgradeType);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_UPGRADE, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnParseError(CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc, LPCTSTR lpszName)
{
	USES_CONVERSION;

	int content_len		= (int)(strlen(lpszErrorDesc) + 10);
	LPTSTR lpszContent	= new TCHAR[content_len];

	wsprintf(lpszContent, _T("(%i) : %s"), iErrorCode, A2T(lpszErrorDesc));
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_PARSE_ERROR, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnWSMessageHeader(CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("(fin: %d, rsv: 0x%x, oc: 0x%x, mask: %d, len: %I64d)"), bFinal, iReserved, iOperationCode, lpszMask ? 1 : 0, ullBodyLen);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_WS_MSG_HEADER, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnWSMessageBody(CONNID dwConnID, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[20];
	wsprintf(lpszContent, _T("(%d bytes)"), iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_WS_MSG_BODY, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnWSMessageComplete(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_WS_MSG_COMPLETE, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostUncompressBody(CONNID dwConnID, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[20];
	wsprintf(lpszContent, _T("(%d bytes)"), iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_UNCOMPRESS_BODY, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostUncompressBodyFail(CONNID dwConnID, int iResult, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[20];
	wsprintf(lpszContent, _T("(rs: %d)"), iResult);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_UNCOMPRESS_BODY_FAIL, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

#endif

void PostInfoMsg(info_msg* msg)
{
	if(	g_pMainWnd == nullptr								||
		g_pMainWnd->GetSafeHwnd() == nullptr				||
		!g_pMainWnd->PostMessage(USER_INFO_MSG, (WPARAM)msg	))
		info_msg::Destruct(msg);
}

void LogInfoMsg(info_msg* pInfoMsg)
{
	CString msg;

	if(pInfoMsg->name)
	{
		if(pInfoMsg->connID > 0)
		{
			if(pInfoMsg->contentLength > 0)
				msg.Format(_T("  > [ %s #%Iu, %s ] -> %s"), pInfoMsg->name, pInfoMsg->connID, pInfoMsg->evt, pInfoMsg->content);
			else
				msg.Format(_T("  > [ %s #%Iu, %s ]"), pInfoMsg->name, pInfoMsg->connID, pInfoMsg->evt);
		}
		else
		{
			if(pInfoMsg->contentLength > 0)
				msg.Format(_T("  > [ %s - %s ] -> %s"), pInfoMsg->name, pInfoMsg->evt, pInfoMsg->content);
			else
				msg.Format(_T("  > [ %s - %s ]"), pInfoMsg->name, pInfoMsg->evt);
		}
	}
	else
	{
		if(pInfoMsg->connID > 0)
		{
			if(pInfoMsg->contentLength > 0)
				msg.Format(_T("  > [ %Iu, %s ] -> %s"), pInfoMsg->connID, pInfoMsg->evt, pInfoMsg->content);
			else
				msg.Format(_T("  > [ %Iu, %s ]"), pInfoMsg->connID, pInfoMsg->evt);
		}
		else
		{
			if(pInfoMsg->contentLength > 0)
				msg.Format(_T("  > [ %s ] -> %s"), pInfoMsg->evt, pInfoMsg->content);
			else
				msg.Format(_T("  > [ %s ]"), pInfoMsg->evt);
		}
	}

	LogMsg(msg);

	info_msg::Destruct(pInfoMsg);
}

void LogMsg(const CString& msg)
{
	if(!g_pInfoList || !g_pInfoList->GetSafeHwnd())
		return;

	g_pInfoList->SetRedraw(FALSE);

	int iCurIndex	= g_pInfoList->GetCurSel();

	BOOL bFirst		= TRUE;
	int iStart		= 0;
	int iAdd		= 0;

	while(true)
	{
		CString item = msg.Tokenize(_T("\r\n"), iStart);

		if(iStart == -1)
			break;

		if(bFirst)
			bFirst = FALSE;
		else
			item.Insert(0, _T("      | "));

		g_pInfoList->AddString(item);
		++iAdd;
	}

	int iCount		= g_pInfoList->GetCount();
	BOOL bCurLast	= (iCurIndex == LB_ERR || iCurIndex + iAdd == iCount - 1);

	int iDec = 0;
	while(g_pInfoList->GetCount() > MAX_LOG_RECORD_LENGTH)
	{
		g_pInfoList->DeleteString(0);
		++iDec;
	}

	if(bCurLast)
	{
		iCurIndex = iCount - iDec - 1;
		g_pInfoList->SetCurSel(iCurIndex);
	}
	else if(iCurIndex < iDec)
	{
		iCurIndex = 0;
		g_pInfoList->SetCurSel(iCurIndex);
	}
	else
		iCurIndex = g_pInfoList->GetCurSel();

	g_pInfoList->SetAnchorIndex(iCurIndex);
	g_pInfoList->SetRedraw(TRUE);
}

ADDRESS_FAMILY GuessAddrFamily(LPCTSTR lpszAddress)
{
	if (!lpszAddress || lpszAddress[0] == 0)
		return AF_UNSPEC;

	if(::StrChr(lpszAddress, ':'))
		return AF_INET6;

	TCHAR c;
	int arr[4];

	if(stscanf_s(lpszAddress, _T("%d.%d.%d.%d%c"), &arr[0], &arr[1], &arr[2], &arr[3], &c, 1) != 4)
		return AF_UNSPEC;

	for(int i = 0; i < 4; i++)
	{
		if(arr[i] < 0 || arr[i] > 255)
			return AF_UNSPEC;
	}

	return AF_INET;
}

LPCTSTR GetLoopbackAddress(LPCTSTR lpszLikeAddress)
{
	ADDRESS_FAMILY f = GuessAddrFamily(lpszLikeAddress);

	if(f == AF_INET)
		return IPV4_LOOPBACK_ADDRESS;
	if(f == AF_INET6)
		return IPV6_LOOPBACK_ADDRESS;

	return nullptr;
}

LPCTSTR GetAnyAddress(LPCTSTR lpszLikeAddress)
{
	ADDRESS_FAMILY f = GuessAddrFamily(lpszLikeAddress);

	if(f == AF_INET)
		return IPV4_ANY_ADDRESS;
	if(f == AF_INET6)
		return IPV6_ANY_ADDRESS;

	return nullptr;
}

CBufferPtr* GeneratePkgBuffer(DWORD seq, LPCTSTR lpszName, short age, LPCTSTR lpszDesc)
{
	USES_CONVERSION;

	LPCSTR name = T2A((LPTSTR)lpszName);
	LPCSTR desc = T2A((LPTSTR)lpszDesc);
	int desc_len = (int)strlen(desc) + 1;
	int body_len = offsetof(TPkgBody, desc) + desc_len;

	TPkgBody* pBody = (TPkgBody*)_alloca(body_len);
	memset(pBody, 0, body_len);

	pBody->age = age;
	strcpy(pBody->name, name);
	strcpy(pBody->desc, desc);

	TPkgHeader header;
	header.seq = seq;
	header.body_len = body_len;

	return GeneratePkgBuffer(header, *pBody);
}

CBufferPtr* GeneratePkgBuffer(const TPkgHeader& header, const TPkgBody& body)
{
	int header_len	= sizeof(TPkgHeader);
	int body_len	= header.body_len;
	
	CBufferPtr* pBuffer = new CBufferPtr(header_len + body_len);

	memcpy(pBuffer->Ptr(), (BYTE*)&header, header_len);
	memcpy(pBuffer->Ptr() + header_len, (BYTE*)&body, body_len);

	return pBuffer;
}

LPCTSTR g_lpszDefaultCookieFile = GetDefaultCookieFile();

LPCTSTR GetDefaultCookieFile()
{
	static TCHAR c_szCookieFile[MAX_PATH] = {0};

	if(c_szCookieFile[0] == 0)
	{
		CString strName;
		LPTSTR lpszName = strName.GetBuffer(MAX_PATH);

		DWORD rs = ::GetModuleFileName(nullptr, lpszName, MAX_PATH);
		VERIFY(rs > 0 && rs < MAX_PATH);
		
		strName.ReleaseBuffer();
		strName.Append(_T(".cki"));

		lstrcpy(c_szCookieFile, strName);
	}

	return c_szCookieFile;
}

BOOL SetCurrentPathToModulePath(HMODULE hModule)
{
	TCHAR szPath[MAX_PATH];
	if(::GetModuleFileName(hModule, szPath, MAX_PATH))
	{
		TCHAR drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
		_tsplitpath(szPath, drive, dir, fname, ext);
		lstrcpy(szPath, drive);
		lstrcat(szPath, dir);

		return ::SetCurrentDirectory(szPath);
	}

	return FALSE;
}

#ifdef _NEED_SSL

#include "../../Src/Common/FuncHelper.h"

#define SSL_CERT_RELATIVE_PATH		_T("\\..\\..\\ssl-cert\\")

LPCSTR g_c_lpszPemCert =
	"-----BEGIN CERTIFICATE-----\n"
	"MIID6TCCAtGgAwIBAgIDAIFpMA0GCSqGSIb3DQEBCwUAMH8xCzAJBgNVBAYTAkNO\n"
	"MQswCQYDVQQIDAJHRDELMAkGA1UEBwwCR1oxDDAKBgNVBAoMA1NTVDEPMA0GA1UE\n"
	"CwwGSmVzc01BMRcwFQYDVQQDDA53d3cuamVzc21hLm9yZzEeMBwGCSqGSIb3DQEJ\n"
	"ARYPbGRjc2FhQDIxY24uY29tMB4XDTE2MDEwMTAwMDAwMFoXDTI2MDEwMTAwMDAw\n"
	"MFowcjELMAkGA1UEBhMCQ04xCzAJBgNVBAgMAkdEMQwwCgYDVQQKDANTU1QxDzAN\n"
	"BgNVBAsMBkplc3NNQTEXMBUGA1UEAwwOd3d3Lmplc3NtYS5vcmcxHjAcBgkqhkiG\n"
	"9w0BCQEWD2xkY3NhYUAyMWNuLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC\n"
	"AQoCggEBAOFlDuWj95Nov+48W0ZaUp69GVB+U3orPuVm+fU5gXgl0tJbNbXS0GOM\n"
	"MspmMB0WxciTfj+t/WufpcHKzryHwrbqmMgrZiSyrp4ulA2mQuTPR32vgQ3/2eBs\n"
	"O884GmKOUbiaORgHGBoRAdOnoWarM7B/igS6WGqPetaUz12QAv2vzw9Qn0xxIBpr\n"
	"SVJumTo9HtOvuRj0BNXMRRYp+lfX2+96cftidyU1q+2Y2v3Fedzlo8Lr7m1ZkPcp\n"
	"ugQDz3R0kXvigDPAKhbTj6eAXJ/hcPOCGQ5RTIZJH4JnJ2jKw5lTjJ6y0rbt9Jq9\n"
	"NuIqz3GORHYWY4xBmbdTZy0BrbQOePECAwEAAaN7MHkwCQYDVR0TBAIwADAsBglg\n"
	"hkgBhvhCAQ0EHxYdT3BlblNTTCBHZW5lcmF0ZWQgQ2VydGlmaWNhdGUwHQYDVR0O\n"
	"BBYEFHYUdE6SbzXIWA8m3TwdgUJfAT6+MB8GA1UdIwQYMBaAFPx+7pAUh/GVC8xa\n"
	"OXmmZ8GMvpyIMA0GCSqGSIb3DQEBCwUAA4IBAQCZWaDrHbRHNNEaCVVFl5WCNqyd\n"
	"y1MpI2iErmfiCTNiG4v9wImTGWIWRHc3dZowniwSb43OuuhlpdWCxeal9nTL6iCP\n"
	"B486EMOsVWcfUZkPeK4cBgRXbHTmjxfmQEHHA6NTQdRc7LGq4tjhYs8zmxBVI2HO\n"
	"uIp0Qbj/Mb2j8r13IC91Z5Ontq0mQTIg0vvffZsqvcjs3Mmrxr2iX/EYpKNR2DP+\n"
	"9msWTZH734tZ2I7vORZOprpdAwELOyVLXkKIcP9qkRPAcFNk0Fl5peBQtBuP1ky5\n"
	"kUzWMQq2eoy73miPRnQ8ZjZ/3xTXhMKJ6bCn3tbWrjvc1TwpobfwyfsFH9sq\n"
	"-----END CERTIFICATE-----\n";

LPCSTR g_c_lpszPemKey =
	"-----BEGIN RSA PRIVATE KEY-----\n"
	"Proc-Type: 4,ENCRYPTED\n"
	"DEK-Info: AES-256-CBC,5F6ACADA6B6B1238FD34A0EC0C5EC2ED\n"
	"\n"
	"F0JylDtptlBaFpbCP9Qa2pRf5z8sBbUta6G+rBwuWn7GxU//Nub49/Mf0uRm3YTN\n"
	"PmCbIQKRyn/u92Yk6cJ9vLCiWe89pa+qlv+kF9PWmYo8Lt6dvtSr1TNAjelGvQjC\n"
	"HAi58BHoXXIRrZJQKjzFKGcwhWExCAOct5UDuHu64IvWg+LDccd+tTU/kdKEI9QY\n"
	"3POohP6etBBsFIoJFd23yxxNrvxK/v0as7EI/SeFzv7Mfn7haL4LxEsN+9XPN/Ap\n"
	"U0IxOT84dMgtIsvW2gvit+qNuaSvrocyEOr5uEfpoS58/yyb0VyNg94itkc6IK3X\n"
	"Yeokm2YOJk/4yMoZAeNMMDdR82csCzpTTHurMIZLgtr+SO1ChtreKm0XQB8QNyJ+\n"
	"CouAAHOBsxTPgVY/QS1TDrmEZv7cHOAvJp7BYvKroD/pi5Wu2IHAlIEXhdW3Evwn\n"
	"jyzUgMUHdXJIcngkkopYEoTPVEXsYX6aHvPJI+jDDKIfDM75SOOw1z7Divsb77I8\n"
	"n1jMYDUH4i2g2BixfwFRPxZ6NnRLtqM+TD4yecu3zsqvs0UIlrZFuVIl98Guqa1H\n"
	"tJKCxWEaJNb5YZg8GgdZFXYWXUN/HtFqWJelhGcjy48RjOy7LHJ30fkXV07b2s6M\n"
	"XcXj9g4jRRyAiZ/kmTqs+Oj6hn+iI4vdGumc2sG0+ZkUwDkj3zfn1N1+J/htVM1z\n"
	"Y2kHpKGZzQZ4eriy6CM++FrZ9CSNIPqh5AzayPuMxvEtd27vBnO8nKMwThbhGPtj\n"
	"fd7uPPzfG/W7Zo55lYL5opEzvOdULWm8+31ojitN7GSjsFW46HWoyYfTOlTAzP5+\n"
	"yPlUGxUyBhlqeXnQNTiLtRb22I2FF41HYH7GsYcxonHw3rKCcJedzL5BMg6Ic2xN\n"
	"ZxwXdj20rkugtcSAPxp3OGrmBiCX0wYiZaSgNg5zqI1SIhiWzkq64xoiIFlmxGEo\n"
	"Q/Vv7mpRmf1X0eTjiTYz4tYU6TPgtAqLLpmVNbozPnb5eGaMDbRlJnGJjqlXzPAw\n"
	"7qUhsb/KZ/k7yZLE+rseicwtAYA0th9A2tpc0CY3/EiL0w5E3zR0SPMinCJeO0rJ\n"
	"91njgDPxJ5lPXC04Dlgt/a1WK6D/yQ2kpIiNBUIlGZtr+nD9GqPYiGTqND53TL5f\n"
	"PJrr1PibL2l3fyhNDgNhgmY1CGpB6hNkqPOpit+XhGq6X0PlELcj7zYN3acW47jp\n"
	"KVm2lT1AXhzqemTWqPQL1+3s8ix0nGVNlmzsOFHqCkmr7FZb76hD5YTtGkOU/Kn3\n"
	"I5GqS5GHSCAVJnJWuFk2Y8qqg69mJREcoKThoYsuo8hw9JqZM34JIqqT029Nj35B\n"
	"xau4hSXkmHuUCGpwbr0yvsz3ikRfM6y0cT3T6oBiWgfYktpX//89YbyYGcQX7tK0\n"
	"aaHW2zvAfMyCWT0CxRen5GAyVeb+WFBdo+xD0HpQcRFtByiNNXmm4kJAoZujeHpe\n"
	"ZWcu3xkVcTqWMixAYHcDv3L0EXUnVeWplovRh1+OSm7YvdaLL3cctHoW3a8Kzdc1\n"
	"rtoGVI+f4xSEDheB5P4NX6UYbDLIGMPPuR0F5EliL+vmyJcEMuLY8KTPJ0uwf7Ns\n"
	"-----END RSA PRIVATE KEY-----\n";

LPCSTR g_c_lpszCAPemCert =
	"-----BEGIN CERTIFICATE-----\n"
	"MIID0TCCArmgAwIBAgIJALFGobpzN5MdMA0GCSqGSIb3DQEBCwUAMH8xCzAJBgNV\n"
	"BAYTAkNOMQswCQYDVQQIDAJHRDELMAkGA1UEBwwCR1oxDDAKBgNVBAoMA1NTVDEP\n"
	"MA0GA1UECwwGSmVzc01BMRcwFQYDVQQDDA53d3cuamVzc21hLm9yZzEeMBwGCSqG\n"
	"SIb3DQEJARYPbGRjc2FhQDIxY24uY29tMB4XDTE2MDQwMTE1MDIwMFoXDTI0MDYx\n"
	"ODE1MDIwMFowfzELMAkGA1UEBhMCQ04xCzAJBgNVBAgMAkdEMQswCQYDVQQHDAJH\n"
	"WjEMMAoGA1UECgwDU1NUMQ8wDQYDVQQLDAZKZXNzTUExFzAVBgNVBAMMDnd3dy5q\n"
	"ZXNzbWEub3JnMR4wHAYJKoZIhvcNAQkBFg9sZGNzYWFAMjFjbi5jb20wggEiMA0G\n"
	"CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDht6llexLtFkV8ijjdJGaHXXQysWOJ\n"
	"UM/YQFYP52nviurJSpMbWSXnuaDlfidk76B66Np5mlnN5BiHqbBj34GCVKz5VQtx\n"
	"3kMY1y30YWyiHAEZiV3PLQc8/A9MnJM/q/mHaulmTuJi8A85TWadqUNXgiaIMkqz\n"
	"bKaauR1/GCxXuEVroqtyR99RCWhfakTz04KfIbt83QR0imWC6uhmvD/DXJ03XFzd\n"
	"XkK5aNp+ef1sBQgFKjeXV6EMuq+UgEDPXlCDUJAqsZt6W/ohrCAHWQYZ/RSvvaMJ\n"
	"O7aWROGAC/lh6ATOIbFlGVppw6zUGdIDkB5FVF1MC7CyDndncFrY+OJzAgMBAAGj\n"
	"UDBOMB0GA1UdDgQWBBT8fu6QFIfxlQvMWjl5pmfBjL6ciDAfBgNVHSMEGDAWgBT8\n"
	"fu6QFIfxlQvMWjl5pmfBjL6ciDAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUA\n"
	"A4IBAQDI+f6GMBJxRJNKrgbUYLD1U6LWEQJQ50g2NxGy0j+TL6oypoo/kyME3tOR\n"
	"EmXEDzytGcSaQ78xYcg97UQd8OhXYQr0qwZ/JLarmhCVK/bfbGTIn4Mk4ZgDqcOU\n"
	"46jsJeEZwUSrrq7svKO5d7+wV0VGPO+Ww4yzRCPwm2puXFY1+KpTxYX31+wwMB8p\n"
	"7GuJEDgV08qzLfcBAfSFFYiOHL3tJ+XNKFNRqigjeYrWuAMphOhpYfYnU0d0upe8\n"
	"wWx9Unm8qSkc7hiS/vvs1v7Pv1sqMFRBoaKOTqZ7Wz/5AySGPQjeMV/atmArDEkx\n"
	"z58OEgTzg1J/Keztxwj7I2KnYHyH\n"
	"-----END CERTIFICATE-----\n";

LPCSTR g_s_lpszPemCert =
	"-----BEGIN CERTIFICATE-----\n"
	"MIID6TCCAtGgAwIBAgIDAIFoMA0GCSqGSIb3DQEBCwUAMH8xCzAJBgNVBAYTAkNO\n"
	"MQswCQYDVQQIDAJHRDELMAkGA1UEBwwCR1oxDDAKBgNVBAoMA1NTVDEPMA0GA1UE\n"
	"CwwGSmVzc01BMRcwFQYDVQQDDA53d3cuamVzc21hLm9yZzEeMBwGCSqGSIb3DQEJ\n"
	"ARYPbGRjc2FhQDIxY24uY29tMB4XDTE2MDEwMTAwMDAwMFoXDTI2MDEwMTAwMDAw\n"
	"MFowcjELMAkGA1UEBhMCQ04xCzAJBgNVBAgMAkdEMQwwCgYDVQQKDANTU1QxDzAN\n"
	"BgNVBAsMBkplc3NNQTEXMBUGA1UEAwwOd3d3Lmplc3NtYS5vcmcxHjAcBgkqhkiG\n"
	"9w0BCQEWD2xkY3NhYUAyMWNuLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC\n"
	"AQoCggEBALEOPuYKEl/iN3ub/QNaaqo0Hynk62znTp5kXj0zQAqxLxwHU117xHBk\n"
	"0LCZux0LnDFl9JAlilvqfeaXRWmQd6hrvV+1mNpnIe2xfyNWwa37QDNjXB7mEg3V\n"
	"O/dN9KeVxq9RlH/dQJ7qZlYy4i7wznYMUlz38378wYoOvdAQBX1UrdjWA5T4q0Da\n"
	"cRRAV0fIp45hBarp6T1nhDEvttU8yud2lIEV9jWblkyGIhU4FisGOIRRGhnKzCAh\n"
	"CYSZ59mq8Wf4FawMyz/H4noHcWWJu4ID1oAE+VFGXKm78qjp/T71/myn5lcGquTx\n"
	"oSoJrx7og+529eaJwgeJd2JF7OQ5NysCAwEAAaN7MHkwCQYDVR0TBAIwADAsBglg\n"
	"hkgBhvhCAQ0EHxYdT3BlblNTTCBHZW5lcmF0ZWQgQ2VydGlmaWNhdGUwHQYDVR0O\n"
	"BBYEFO0CmyFtrGLrkNkkA62QPzSp8vgJMB8GA1UdIwQYMBaAFPx+7pAUh/GVC8xa\n"
	"OXmmZ8GMvpyIMA0GCSqGSIb3DQEBCwUAA4IBAQBT1JGXn7HMyNHZntM4kCp7kwN9\n"
	"HSpcbQxbWFtk+l8ANP9SqCIEd4xc4rAaUDDDMdD2S0wF9iio1MA1KC5OoJ5Dqw6C\n"
	"yTa08LRowlmnw+NNZC2xzaSRmzLGlPDHNauZ/G2ScdWsqR5v2+pKF7bYxAajRkk5\n"
	"oqRRDI6X7vHsvX/kqTn9Prk0sFmU0EU64az2iUe/MG9v1WY8eOV+tTEq29GuYYjJ\n"
	"E3H8vaeq1fTB0vYgT5xoCc36xVXju/3l17CE12n5of004u4eRi3/T0wbp9WgG2ns\n"
	"wjetaYeIAWS/AJQzIK22tztGAbAk6kTgsO5YLx2h6GLGpytDXWvgzr2PdEu1\n"
	"-----END CERTIFICATE-----\n";

LPCSTR g_s_lpszPemKey =
	"-----BEGIN RSA PRIVATE KEY-----\n"
	"Proc-Type: 4,ENCRYPTED\n"
	"DEK-Info: AES-256-CBC,107800C9B21513D8F642A394FAD9047E\n"
	"\n"
	"bmaTCBYAzKzpUrdaT6sb/dbH5o3JbFu2d8s1NfKhghYUG/RWgzx5H8QWSsh1Ll+8\n"
	"701HwSwauwbKaBWzWP5bj4mjw9hXcva8l1QWeuW+Q0GsE7uWNZGFQZbCxER/9jOu\n"
	"36KMlx0twcIEGs9PNwnY7d9oS/BMVpiUqhL8sPihMLCFK0fm6VGjMrfdIZJuZXEv\n"
	"BKt+c0UPkkPmivZjSm6iTPKI6VpGeyg0sM/uZr+mRj7IOAD6vsSQnCfUOq00Rm14\n"
	"ABrer8dCWJDKrx1iLq7tFojII+JcB7oBy+8KC/t0MlqmIP2jx93/MVW5lbZ3mj2v\n"
	"BanVQvklHTLe/WHjoaltqBEKN0sIzfyed6qWudKHUa8yjAr0PS4rsTbO0S4fFz+A\n"
	"KFWEs8fuXsanFHt5akWDTMiw1n4HaiioT4jjhWRRIKGEndztyGMklShYAdSTqsZO\n"
	"vzPO+xqLlg6wOgCGZ3Kl/c243XyLMCI+lJlzu3ccqzQJbNfPADXUcxSTqaEByWG3\n"
	"YIXttKY5WFBcNjf9B3c1qfJMt3nGY2QewnL+rNm/qpose/AsFS+2aqMeRw7fYh3R\n"
	"O+H+G+0gxrSK0E3WiewMIvUqFI6W300MWx6O7d87OkbfwMLpLPumsfLKswOKaAIF\n"
	"p0RxSABSe5ixqnErsoiYGNf3bln4tsQ+v5jYUY+Gn/9mVTPEYs6uwC78UW5Gj+Za\n"
	"lB3V8aRxQjWBerhLhf3iCmkkOVaG5JvVJDmvwo6iBRZ4NNzkAqOJm/UvfWoKN91b\n"
	"riHAWNayqvOfzaK6KYiJlOR9D779pK6xL+gZ0zo3rIA2yzs5MJAvJmaq6SQmQFl1\n"
	"Vzhnf3hIGJFtJYQJhlr36pkPgG9jVdJPhZisY9EsIAHvDbbLlBNexSmZc2RZmsVL\n"
	"VG6WqOm85DzuZxGULK/CnXCLpBMKWf9y5YPJrLkuwQILrr2btnAivPfUU80S3E0c\n"
	"VslBodCkSGVx3iQoAVJv2+6ZNMRNLVzpLnlaVtHl/gvw+InJlXmzBlZQFKOCaOny\n"
	"sz67qKmK1KWui2G3AzuvTEL9O++FVO+irpSNSLozxif/erLJl7R60pBkrWwd6jGB\n"
	"8qQv7ni+xIeXDBLSu26zPunLR/2PhrrCv49KmqNmF7sSNx6xmcc3CfgEy+5+Tm97\n"
	"KPE4JlaH/ZW61TlzXojbZMaiJltkFkpagFgm2bXrN1nMk2d6R+Yfe78lSvPKoB8k\n"
	"1m7LY1lNXFZJh2hxQRhdV9oTokFahUSitQCg1S3eASuE2jFRRL4jrh4LcBWUHZnJ\n"
	"OOAwBDOF4RfGyfiDs0IlrnLvK3a2KorlHfvUyj+cwkRmgI8sWgkJuNpz1mKDn5UZ\n"
	"yGn2JczHY0CiInRGgwMnS1Z/WpJZYGNh7pP7Hm1RnzVslkWSJGaKJv9jDNUCyy8R\n"
	"L+nwUSd6m6r8YWcs3zdBYsDHGOO5MgAU5Q8yuLbEgaK5NJSJgGkm+ya5oB88Y6T0\n"
	"JcL/u5xKw4kJiKgiLr3zrWe5qV6+lpH7fGKy7fucJWltDkfQSD3Dp0b64TW4FuQq\n"
	"VQhDR+KykLK6QxM2x3/maMfG578Ku1F6g4RboDR/6uCFu6hQuCoNYBk/QB0XpY13\n"
	"-----END RSA PRIVATE KEY-----\n";

LPCSTR g_s_lpszCAPemCert =
	"-----BEGIN CERTIFICATE-----\n"
	"MIID0TCCArmgAwIBAgIJALFGobpzN5MdMA0GCSqGSIb3DQEBCwUAMH8xCzAJBgNV\n"
	"BAYTAkNOMQswCQYDVQQIDAJHRDELMAkGA1UEBwwCR1oxDDAKBgNVBAoMA1NTVDEP\n"
	"MA0GA1UECwwGSmVzc01BMRcwFQYDVQQDDA53d3cuamVzc21hLm9yZzEeMBwGCSqG\n"
	"SIb3DQEJARYPbGRjc2FhQDIxY24uY29tMB4XDTE2MDQwMTE1MDIwMFoXDTI0MDYx\n"
	"ODE1MDIwMFowfzELMAkGA1UEBhMCQ04xCzAJBgNVBAgMAkdEMQswCQYDVQQHDAJH\n"
	"WjEMMAoGA1UECgwDU1NUMQ8wDQYDVQQLDAZKZXNzTUExFzAVBgNVBAMMDnd3dy5q\n"
	"ZXNzbWEub3JnMR4wHAYJKoZIhvcNAQkBFg9sZGNzYWFAMjFjbi5jb20wggEiMA0G\n"
	"CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDht6llexLtFkV8ijjdJGaHXXQysWOJ\n"
	"UM/YQFYP52nviurJSpMbWSXnuaDlfidk76B66Np5mlnN5BiHqbBj34GCVKz5VQtx\n"
	"3kMY1y30YWyiHAEZiV3PLQc8/A9MnJM/q/mHaulmTuJi8A85TWadqUNXgiaIMkqz\n"
	"bKaauR1/GCxXuEVroqtyR99RCWhfakTz04KfIbt83QR0imWC6uhmvD/DXJ03XFzd\n"
	"XkK5aNp+ef1sBQgFKjeXV6EMuq+UgEDPXlCDUJAqsZt6W/ohrCAHWQYZ/RSvvaMJ\n"
	"O7aWROGAC/lh6ATOIbFlGVppw6zUGdIDkB5FVF1MC7CyDndncFrY+OJzAgMBAAGj\n"
	"UDBOMB0GA1UdDgQWBBT8fu6QFIfxlQvMWjl5pmfBjL6ciDAfBgNVHSMEGDAWgBT8\n"
	"fu6QFIfxlQvMWjl5pmfBjL6ciDAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUA\n"
	"A4IBAQDI+f6GMBJxRJNKrgbUYLD1U6LWEQJQ50g2NxGy0j+TL6oypoo/kyME3tOR\n"
	"EmXEDzytGcSaQ78xYcg97UQd8OhXYQr0qwZ/JLarmhCVK/bfbGTIn4Mk4ZgDqcOU\n"
	"46jsJeEZwUSrrq7svKO5d7+wV0VGPO+Ww4yzRCPwm2puXFY1+KpTxYX31+wwMB8p\n"
	"7GuJEDgV08qzLfcBAfSFFYiOHL3tJ+XNKFNRqigjeYrWuAMphOhpYfYnU0d0upe8\n"
	"wWx9Unm8qSkc7hiS/vvs1v7Pv1sqMFRBoaKOTqZ7Wz/5AySGPQjeMV/atmArDEkx\n"
	"z58OEgTzg1J/Keztxwj7I2KnYHyH\n"
	"-----END CERTIFICATE-----\n";

CString g_c_strCAPemCertFileOrPath;
CString g_c_strPemCertFile;
CString g_c_strPemKeyFile;

CString g_s_strCAPemCertFileOrPath;
CString g_s_strPemCertFile;
CString g_s_strPemKeyFile;

int g_c_iVerifyMode					= SSL_VM_PEER;
LPCTSTR g_c_lpszCAPemCertFileOrPath	= _T("ca.crt");
LPCTSTR g_c_lpszPemCertFile			= _T("client.cer");
LPCTSTR g_c_lpszPemKeyFile			= _T("client.key");
LPCTSTR g_c_lpszKeyPasswod			= _T("123456");

int g_s_iVerifyMode					= SSL_VM_PEER | SSL_VM_FAIL_IF_NO_PEER_CERT;
LPCTSTR g_s_lpszCAPemCertFileOrPath	= _T("ca.crt");
LPCTSTR g_s_lpszPemCertFile			= _T("server.cer");
LPCTSTR g_s_lpszPemKeyFile			= _T("server.key");
LPCTSTR g_s_lpszKeyPasswod			= _T("123456");

CString g_c_strCAPemCertFileOrPath2;
CString g_c_strPemCertFile2;
CString g_c_strPemKeyFile2;

CString g_s_strCAPemCertFileOrPath2;
CString g_s_strPemCertFile2;
CString g_s_strPemKeyFile2;

int g_c_iVerifyMode2					= SSL_VM_PEER;
LPCTSTR g_c_lpszCAPemCertFileOrPath2	= _T("ca2.crt");
LPCTSTR g_c_lpszPemCertFile2			= _T("client2.cer");
LPCTSTR g_c_lpszPemKeyFile2				= _T("client2.key");
LPCTSTR g_c_lpszKeyPasswod2				= _T("ppmm");

int g_s_iVerifyMode2					= SSL_VM_PEER | SSL_VM_FAIL_IF_NO_PEER_CERT;
LPCTSTR g_s_lpszCAPemCertFileOrPath2	= _T("ca2.crt");
LPCTSTR g_s_lpszPemCertFile2			= _T("server2.cer");
LPCTSTR g_s_lpszPemKeyFile2				= _T("server2.key");
LPCTSTR g_s_lpszKeyPasswod2				= _T("ppmm");

BOOL InitSSLParams();
BOOL g_SSLParams = InitSSLParams();

BOOL InitSSLParams()
{
	::SetCurrentPathToModulePath();

	CString strPath;
	::GetCurrentDirectory(MAX_PATH, strPath.GetBuffer(MAX_PATH));
	strPath.ReleaseBuffer();
	strPath += SSL_CERT_RELATIVE_PATH;

	if(g_c_lpszPemCertFile)
	{
		g_c_strPemCertFile	= strPath + g_c_lpszPemCertFile;
		g_c_lpszPemCertFile	= g_c_strPemCertFile;
	}

	if(g_c_lpszPemKeyFile)
	{
		g_c_strPemKeyFile	= strPath + g_c_lpszPemKeyFile;
		g_c_lpszPemKeyFile	= g_c_strPemKeyFile;
	}

	if(g_c_lpszCAPemCertFileOrPath)
	{
		g_c_strCAPemCertFileOrPath	= strPath + g_c_lpszCAPemCertFileOrPath;
		g_c_lpszCAPemCertFileOrPath	= g_c_strCAPemCertFileOrPath;
	}

	if(g_s_lpszPemCertFile)
	{
		g_s_strPemCertFile	= strPath + g_s_lpszPemCertFile;
		g_s_lpszPemCertFile	= g_s_strPemCertFile;
	}

	if(g_s_lpszPemKeyFile)
	{
		g_s_strPemKeyFile	= strPath + g_s_lpszPemKeyFile;
		g_s_lpszPemKeyFile	= g_s_strPemKeyFile;
	}

	if(g_s_lpszCAPemCertFileOrPath)
	{
		g_s_strCAPemCertFileOrPath	= strPath + g_s_lpszCAPemCertFileOrPath;
		g_s_lpszCAPemCertFileOrPath	= g_s_strCAPemCertFileOrPath;
	}

	if(g_c_lpszPemCertFile2)
	{
		g_c_strPemCertFile2  = strPath + g_c_lpszPemCertFile2;
		g_c_lpszPemCertFile2 = g_c_strPemCertFile2;
	}

	if(g_c_lpszPemKeyFile2)
	{
		g_c_strPemKeyFile2	= strPath + g_c_lpszPemKeyFile2;
		g_c_lpszPemKeyFile2	= g_c_strPemKeyFile2;
	}

	if(g_c_lpszCAPemCertFileOrPath2)
	{
		g_c_strCAPemCertFileOrPath2  = strPath + g_c_lpszCAPemCertFileOrPath2;
		g_c_lpszCAPemCertFileOrPath2 = g_c_strCAPemCertFileOrPath2;
	}

	if(g_s_lpszPemCertFile2)
	{
		g_s_strPemCertFile2  = strPath + g_s_lpszPemCertFile2;
		g_s_lpszPemCertFile2 = g_s_strPemCertFile2;
	}

	if(g_s_lpszPemKeyFile2)
	{
		g_s_strPemKeyFile2	= strPath + g_s_lpszPemKeyFile2;
		g_s_lpszPemKeyFile2	= g_s_strPemKeyFile2;
	}

	if(g_s_lpszCAPemCertFileOrPath2)
	{
		g_s_strCAPemCertFileOrPath2  = strPath + g_s_lpszCAPemCertFileOrPath2;
		g_s_lpszCAPemCertFileOrPath2 = g_s_strCAPemCertFileOrPath2;
	}

	return TRUE;
}

#endif

#ifdef _NEED_HTTP

CStringA& HttpVersionToString(EnHttpVersion enVersion, CStringA& strResult)
{
	strResult.Format("HTTP/%d.%d", LOBYTE(enVersion), HIBYTE(enVersion));
	return strResult;
}

CStringA& MakeSecWebSocketAccept(LPCSTR lpszKey, CStringA& strAccept)
{
	CStringA strKey(lpszKey);
	strKey.Append(HTTP_WEB_SOCKET_SEC_SALT);

	_SHA1_CTX ctx;
	BYTE buf[SHA1_BLOCK_SIZE];

	::sha1_init(&ctx);
	::sha1_update(&ctx, (BYTE*)(LPCSTR)strKey, strKey.GetLength());
	::sha1_final(&ctx, buf);

	BYTE* lpszAccept = (BYTE*)strAccept.GetBuffer(SHA1_BLOCK_SIZE * 4 / 3 + 4);

	int len = (int)::base64_encode(buf, lpszAccept, SHA1_BLOCK_SIZE, FALSE);
	strAccept.ReleaseBufferSetLength(len);

	return strAccept;
}

#endif
