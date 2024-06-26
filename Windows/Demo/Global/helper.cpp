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
	"MIIDszCCApugAwIBAgIBATANBgkqhkiG9w0BAQsFADB7MQswCQYDVQQGEwJDTjEL\n"
	"MAkGA1UECAwCR0QxCzAJBgNVBAcMAkdaMQwwCgYDVQQKDANTU1QxDzANBgNVBAsM\n"
	"Bkplc3NtYTETMBEGA1UEAwwKamVzc21hLm9yZzEeMBwGCSqGSIb3DQEJARYPbGRj\n"
	"c2FhQDIxY24uY29tMCAXDTI0MDYyNjA1MjUwOFoYDzIyNDMwNzA5MDUyNTA4WjBu\n"
	"MQswCQYDVQQGEwJDTjELMAkGA1UECAwCR0QxDDAKBgNVBAoMA1NTVDEPMA0GA1UE\n"
	"CwwGSmVzc21hMRMwEQYDVQQDDApqZXNzbWEub3JnMR4wHAYJKoZIhvcNAQkBFg9s\n"
	"ZGNzYWFAMjFjbi5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCD\n"
	"+MyrJEKCheRoOpMRjR78S8hr9W7XN0/EZWyVKwXRT7EE0aGiQdH/W2a+qpWRMa6E\n"
	"Qi47zdBnt0P8ZoFiItQhuhwUJ064afpVoaHHX25UdbF8r+sRTofadughETBBj2Cf\n"
	"qh0ia6EOB0QvpJpywWmGZPoMtypjbUiTb/YGOJh2qsVr67MN/E48vt7qt0VxF9SE\n"
	"pucvqhraTBljWCeRVCae2c0yBSpq/n+7NhamK7+g3xxCKWRz4pN3wrIoEsXTboTh\n"
	"z940caDgthCc23VJ080DN44jZg6c87huKIuxbebJqw2HCM4DwrW+OSzTLszpFAXZ\n"
	"yarllOzWnBut20zmYnl1AgMBAAGjTTBLMAkGA1UdEwQCMAAwHQYDVR0OBBYEFJ5E\n"
	"RJmJ4pUzEbcU9Yge6nr0oi51MB8GA1UdIwQYMBaAFN49z48DywmoD4cNTQgC6nn2\n"
	"QJoUMA0GCSqGSIb3DQEBCwUAA4IBAQBpoSFfDDDKMAy95tSROpYu5WSWQXe6B7kl\n"
	"PGJAF6mWe/4b7jHQqDUVkEmFmbMWUAtpTC3P01TrV77dhIosAnC/B76fb7Pto8W4\n"
	"cjGpWAT0sSegZuhnLtguTGlnR0vVSh/yRRDEtjN8loWpu3BLWVHYOKnn62QGfY0B\n"
	"sRGrfZsKvwB+1w+HOvGopnWv6UYwrzEKthjPMR65rOsoManOv24ua8baJmq0gqF9\n"
	"752kD8n703uWUBx79/QlNIPMZC1iUIi1mEjyrTgSag6+3sWAIKihaoF/Nf9d01nw\n"
	"iL16EIT5dJ0QJWDCeIxhuTZckw+gL1pBeQU7pqzKHPnvo+8GBnTG\n"
	"-----END CERTIFICATE-----\n";

LPCSTR g_c_lpszPemKey =
	"-----BEGIN ENCRYPTED PRIVATE KEY-----\n"
	"MIIFLTBXBgkqhkiG9w0BBQ0wSjApBgkqhkiG9w0BBQwwHAQIK2UJW9QXIj4CAggA\n"
	"MAwGCCqGSIb3DQIJBQAwHQYJYIZIAWUDBAEqBBCDDZQLhAdT91jd6v/5H0+GBIIE\n"
	"0PH6tKl+nPi8sU0ryjxDIrHwrT/ZFah+3TAHGE/YFAOZnzRyCFHQTvUZX4p8eSmw\n"
	"WOpt5NBUPJ3mT0Ctt7lGBRy4AXSyBrFSamlTruM3P1e3ijluYjMbweZFfCWPq8c/\n"
	"jPjbcUkXe6mD96aPSTt/jIunexS8AKovu8c/bFLyTLDk38lATc+GnXQQJ0KiXCRu\n"
	"vpjVSKcv2Br6cWqaNTZ71FvH1RmSD6K6givc0w65pKruHYTMApIRR8YC5Y0vx0gD\n"
	"6nS12LV/EJEtxTfZFlrzZTRWZISPIzYGuTfS+3bPePlxpbwzhN6vmvgjKhdk+3lN\n"
	"3W3ZfqODNhoOKG+mG5Fdj7vR2PU1UND6UUd3+FrzWkXikmalAAwKzRLnyTR1T2rl\n"
	"RhM0Qe/HZianeEQTHpCw27gOz1OMw2EKfIEHM6W2BKGOTY5ls5dqgMfP1ZoQUrOr\n"
	"59tJo4GpWYFGCuHhTEa5OS/gsgnzymGrkuEwPsdSQaBdzV7lFGTv2/ryKX+vNm9V\n"
	"CmKw0nHzOVP19+WL4vPDtbRnLUk8KV9Mg7PdSbGbNcMmTEBk8ju8OvjIUIWZbRTa\n"
	"n5C6fhD1DYZcczmlCILYgXyJISu7EDf3z9cKRAf5VbRAedDMB/xHWmrmlxUJ37Kt\n"
	"tVgaCD0U6Q3q+3y6OOwugc8UbSo4yA/DbLlG0/U7afwQaNxTLa4HGBQljpoNStIt\n"
	"Vgfy2olqHXaf2doSQtsYEl9MHa6neuGfZQMtonDkejnx4KKU+cMhe+KijEUwieYx\n"
	"7aoPB71b82XODquDPAL5zOegj0eYgKn5iXyOx5W44S34zfclxtxxgfsDJ3qJ9qoL\n"
	"sSenrQ3xAYHJSZRcqEgO31XhoEnkyt1V7G0Bk4/GUMD6uQudr3nsw/ulJpAlNK15\n"
	"ZxTSKWrtwOWdwcTj6B14K6wcqMFVNF1Ydbv/qp0b5q5S/orYHzRIPcFmdOAIsjyO\n"
	"6na7+D31BH/4pf+TASBNqRNRw5CBqNcGcfiXk11AywxUnmD5ZvC/C0pTpTD/9qC4\n"
	"LucWJ0sNAtPq8suFjKqQ+wMvq3rUh050NRm2cm2nUJLxafTnr0v3+kKYbVW8pSWB\n"
	"NMelZMVGF1MDYBujg8Mw/xuMhPeLozCZeKmo7eu7aDMXzQMZLfAEJAzU9Du8H4nq\n"
	"GgQVUgEkS5rdbjZGkHP0FuM8m8lueKEPDYwHCJv9Be5Z/uxp9OO/Lmdlha0J7gJu\n"
	"pihNkAYVxRst96b5okXKooYi/TZxAdThoPYH28VwinGR1I3/8I3M5DbUPIgHhDeB\n"
	"ga3u7jt7ZNDUgavukUD0S7WioRb5ooXrXGZ1xmzKLCmMdCDC5S32fQS0wRGfVoMl\n"
	"hWbaT+0uak+fOpqVRxSNyE3Ek788ua5iPHaTSXJSoe5lv7OQKDSZ/+wFeLmDPf4M\n"
	"BHL2gBLD6RNkz5cWgy14sQcJKNAnyptU4EGPyURZcB8APtB/ITAS2Az/JSxvSBgq\n"
	"g/L1FujnP2QEpWpVKkTNxsF867bUPN34KrlPKYjNqcKA2pD4fkFoKSeeNtOEWa++\n"
	"d6q9y+mDD97SnIFAAhDFlukzXtyl4MU6uiqRldFiuEt3KzvV19n8M+NyyYIFhfdg\n"
	"6TkYEbMJPQ/Y3EGNmyMqbFdJzrdl/B8pr7JQnikTfUZZ\n"
	"-----END ENCRYPTED PRIVATE KEY-----\n";

LPCSTR g_c_lpszCAPemCert =
	"-----BEGIN CERTIFICATE-----\n"
	"MIID2TCCAsGgAwIBAgIUM8TTtPU+ejzffYXCcs/zZsU7OuIwDQYJKoZIhvcNAQEL\n"
	"BQAwezELMAkGA1UEBhMCQ04xCzAJBgNVBAgMAkdEMQswCQYDVQQHDAJHWjEMMAoG\n"
	"A1UECgwDU1NUMQ8wDQYDVQQLDAZKZXNzbWExEzARBgNVBAMMCmplc3NtYS5vcmcx\n"
	"HjAcBgkqhkiG9w0BCQEWD2xkY3NhYUAyMWNuLmNvbTAgFw0yNDA2MjYwNTA0NDNa\n"
	"GA8yMjcwMTEyNDA1MDQ0M1owezELMAkGA1UEBhMCQ04xCzAJBgNVBAgMAkdEMQsw\n"
	"CQYDVQQHDAJHWjEMMAoGA1UECgwDU1NUMQ8wDQYDVQQLDAZKZXNzbWExEzARBgNV\n"
	"BAMMCmplc3NtYS5vcmcxHjAcBgkqhkiG9w0BCQEWD2xkY3NhYUAyMWNuLmNvbTCC\n"
	"ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAML+v79+aLQt0Za0dTIZHI5B\n"
	"NDs0g5G8bhdOTlW/kNWflaziZ3GY6d6nJSkQ5e29kyFKxlOD6Gls6bOJ86U71u4R\n"
	"bCmoFvRTDH4q2cJ/+PbiioLpNveDG6lnRCs9JNRQoJrkpRo6urnVnAdsIf6UFjLI\n"
	"dlByNMPGYJ0V8/oKJG5Vu5gcbZV0jVA5+tswkH/zquexEXoKvp18mcwl+pNc/LwW\n"
	"0WnGj0uoJjxHg4GsS78PASjhxMR/2d/1OpgPauldFaNHjVPtaLqJnuejwA6M6Sz8\n"
	"iFPybAQAMpHL9W8kf08jtbnFvnm4ibUkQL5h+OJoIEQa9AVZOSoFG2/g5Zcn8X8C\n"
	"AwEAAaNTMFEwHQYDVR0OBBYEFN49z48DywmoD4cNTQgC6nn2QJoUMB8GA1UdIwQY\n"
	"MBaAFN49z48DywmoD4cNTQgC6nn2QJoUMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZI\n"
	"hvcNAQELBQADggEBALJnYrYBSZLyYX14FQ04zxG3AX0CtQzNOOa7LDrr+H8Ly+nK\n"
	"qS87gg2njMVZH1zM2demtMwydR/F2Ui8ggaduMvc9h5YgQKEwYl8KarJEY03oZoe\n"
	"zbQGBxCXpDOtMs1vujzcl/iZbSzwEDF3g4la5U8q4MlmfGFKz9CJbvoxecqYA206\n"
	"nNbW2XZsW/xMiQv6iAw5iP/LOR9HAyxcvXIsL790nfcgnTYLmyP254Dj4outc6R+\n"
	"PA+f/c1FvkbUBTR5vJt2tsvHcNU218rY2hyOIhDmZeUWprqBO19sUk3scLbVPr3+\n"
	"WEWEl2XaCekKuPtAnMgVQuFsocXGyiuIhkOe5Z4=\n"
	"-----END CERTIFICATE-----\n";

LPCSTR g_s_lpszPemCert =
	"-----BEGIN CERTIFICATE-----\n"
	"MIIEJjCCAw6gAwIBAgIBAzANBgkqhkiG9w0BAQsFADB7MQswCQYDVQQGEwJDTjEL\n"
	"MAkGA1UECAwCR0QxCzAJBgNVBAcMAkdaMQwwCgYDVQQKDANTU1QxDzANBgNVBAsM\n"
	"Bkplc3NtYTETMBEGA1UEAwwKamVzc21hLm9yZzEeMBwGCSqGSIb3DQEJARYPbGRj\n"
	"c2FhQDIxY24uY29tMCAXDTI0MDYyNjA1MTY1NFoYDzIyNDMwNzA5MDUxNjU0WjBu\n"
	"MQswCQYDVQQGEwJDTjELMAkGA1UECAwCR0QxDDAKBgNVBAoMA1NTVDEPMA0GA1UE\n"
	"CwwGSmVzc21hMRMwEQYDVQQDDApqZXNzbWEub3JnMR4wHAYJKoZIhvcNAQkBFg9s\n"
	"ZGNzYWFAMjFjbi5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC7\n"
	"x3ilLjZtH2ZKuofj4FpVl/IF2yDI5503YQbwllxp7kNEaqTyjJUgmLlZBbwHQzTD\n"
	"xfPk/nZ/m3xUHsVjwXMZqNNufgtSLaBoK4CvBAOTkELphIOZdJYPpuaU66W0phjG\n"
	"VM2R4EFm/rTXddZ7N6sq3fYEL0bxqUJ6fW/+0QhdNSwfdevdAHgOmGkrTj5rILJ8\n"
	"A7FwbkcuV5vBWZ+9ZhNG4csqAjH5LLLCn5hJdhE9WqUp+slfIuXE5vZGDpCQrcc5\n"
	"I1qWt8VNfdwzaBDL/hXl7pAiVpZRvQxyJgbUMLr2QrYFwrPkgpncU7R7AyT/C0tO\n"
	"vgPVZb+IGqbf+NrbHEk3AgMBAAGjgb8wgbwwHwYDVR0jBBgwFoAU3j3PjwPLCagP\n"
	"hw1NCALqefZAmhQwCQYDVR0TBAIwADALBgNVHQ8EBAMCBPAwYgYDVR0RBFswWYIJ\n"
	"bG9jYWxob3N0ggpqZXNzbWEub3JnggwqLmplc3NtYS5vcmeCCmplc3NtYS5jb22C\n"
	"DCouamVzc21hLmNvbYIKamVzc21hLm5ldIIMKi5qZXNzbWEubmV0MB0GA1UdDgQW\n"
	"BBRZ97VSkfue5s8/OkYvUe+lXUgsHzANBgkqhkiG9w0BAQsFAAOCAQEAvM1QrhTL\n"
	"/I1ws4fozKQlfmd3S0AdfFJX4BMTbp88proWpjrNZt6ZJ3EETHKcXu0z4gn3jWy6\n"
	"1d8bDCytYQXjpG3Na7Ym5jc/E7wc+XUCUxlW41G/HnaLaIVccmRbyxMOWkr3yUX1\n"
	"tc8rxUSKWzZBmYtJ49QzIvNzDuoLklE44g8XuqsZZlOZ2wRWJxc/hDG0MkKhRnc1\n"
	"mqeaoY/79QZNE1RvX/aRRJoSl7NQ00/rMP8MU6OMzPvbIsMVK2uT+BVZG0RZJXaG\n"
	"ikQJvxYZrDVZdRZL6tWPtS2wI49KkzGHNH4S1Fni/dDq3P2rxzisMY1gtKQLeVYY\n"
	"eTQDDybjTWWiTg==\n"
	"-----END CERTIFICATE-----\n";

LPCSTR g_s_lpszPemKey =
	"-----BEGIN ENCRYPTED PRIVATE KEY-----\n"
	"MIIFHTBXBgkqhkiG9w0BBQ0wSjApBgkqhkiG9w0BBQwwHAQIgAqUxS2ufB0CAggA\n"
	"MAwGCCqGSIb3DQIJBQAwHQYJYIZIAWUDBAEqBBD8E9f397mpmzmM9zYTWt2DBIIE\n"
	"wIo2i0YH7cT/WCLmSnVvpsbayeo0mbVUFS4xp2VerQUS+UXHlrrOFeKU8NYfG6SS\n"
	"m+3PXksvUlDV9iGT0p/w2Fm1GVKhE7UAeTJkff7KWi2pc926DUbYxhFOUNyCnrQS\n"
	"pWCdV6M/243A0kHg6kIlSblsXjzKMC6eSlh2FGa7M7my9A+nu0WGOqvazkOm/8jk\n"
	"h20xmB3t/Qa8NQp59E9oLXs5+CokIKL17+PmtBKeKpQBxAUtFjcqRAugIiNpPjOS\n"
	"K99cY/Tt1C8ugMIsFH1/4HzFDYiSDRZ630dKOzBruprHVkIvQXI1CW6edPlfFZKG\n"
	"yHeH95kigPVWUxWwluwALVmlPwD4h/GWHGOS7HH6x4ubStogjmC2B9f4VQCd6YRu\n"
	"lee3cdDvdqLoRCoU48SoM/+RMR3NpF508ulWhKrDZ4eGcqdUYrx3rWVyOrmfMJDe\n"
	"kfckGKnhCA1hL3wb/HQuAs2st2lDKBwsYIsOc8UXhueRFHKk+W0O/5kureMPF14O\n"
	"DAxqAW3meHq7CLY8WuqatptIrJVDT8Wkbx47tXhLapTwlI0Cbf2AGfJIWSGmM+VF\n"
	"I5l13pW2tycQnAXzSdd0y9wE0df/EoyXfIJeEfBNkzVhkIIC3KmOiI8BCnei6UR9\n"
	"jun4+6aeFyfGybJ04ybixXyFsCsVUa5QcnhPwJvC8QsVFpuzOttQ5cf4Xn57DyxT\n"
	"4CzWieJ6iQpfAJahRcj/4O6KRmWuMPpkK8XsCgzOYM1MxETq4HbqXEp68KiT6Q9r\n"
	"jEAmbfZ8NdPvAPZ/iXKtA/eaMDDy6EbzDscUBg/TSxsF286F+wH2kXvkbwL2E/zh\n"
	"LsTcjsGUdNKxlDJaivi7dDbSzxzvcDYRh+8Bd/vOw2gJF2ohwXXp3GKTVu71LS+b\n"
	"YruQ55Lauh761ziI/z7qZw9ko8erb5vcsqLh9duqtxTBnQEd05ufFhNnXk4Vq8Cp\n"
	"hr30Qy5sJ7TUuAVs2RSuGHd0Q5l8NGLjQwtkDx5ofizZKQOMWTq8S3IA98QyFka0\n"
	"e+XaGGQ/KZJciIoCkuzAX4mn/aIffMldQIEg5ybslBc326SdTe52ex5YlmUuyvbO\n"
	"zDotjC/eeQEFvq6Xb14N+u7mp8xL5Dlro79aL3VNNGa3lgKP6lWtMjgcyZrWMdc4\n"
	"xaV0sVbfRO8Pj4y3cZGXol529zSNSIc7wT2/kyvF6RgJEcluAIPAJ8ea6CXKqDfe\n"
	"sYZDL1emVoKMoFy3YsnEI+py2xxSsU4pjGPanZZaVfrDfw3qnQWPovfOx5fVc6Om\n"
	"U55o3nbR5wtjPlQmcdVlT/fo7m/OUu3UgdjyLFcljeezJGeskJ3PMYbSsi7Mf+yF\n"
	"/BEW6IfqicjG9TTMGzNACHH0iqAzW6lrC60UXMRIXrs/segt3+r5JqfRH58TMBR+\n"
	"O5xk6sjOL4uqTsJRmbMapHf4tPxli8PdvNYN+YTQ4tlwazrckwgC5TJ3GA5JM3Ox\n"
	"ZQIIKf1joemxq/VT2IpsqwMY67KC4OUOiRy471guGdljFarACe8rzZh8BHONveJd\n"
	"XDgM0oPBkR9z4BGlfbBgAG0bIRNSXp8yGaZMiuIHbI8I4TnR84KgyUUscsIlH03A\n"
	"8PQL73vd5pU4jC6WdOaXwkI=\n"
	"-----END ENCRYPTED PRIVATE KEY-----\n";

LPCSTR g_s_lpszCAPemCert =
	"-----BEGIN CERTIFICATE-----\n"
	"MIID2TCCAsGgAwIBAgIUM8TTtPU+ejzffYXCcs/zZsU7OuIwDQYJKoZIhvcNAQEL\n"
	"BQAwezELMAkGA1UEBhMCQ04xCzAJBgNVBAgMAkdEMQswCQYDVQQHDAJHWjEMMAoG\n"
	"A1UECgwDU1NUMQ8wDQYDVQQLDAZKZXNzbWExEzARBgNVBAMMCmplc3NtYS5vcmcx\n"
	"HjAcBgkqhkiG9w0BCQEWD2xkY3NhYUAyMWNuLmNvbTAgFw0yNDA2MjYwNTA0NDNa\n"
	"GA8yMjcwMTEyNDA1MDQ0M1owezELMAkGA1UEBhMCQ04xCzAJBgNVBAgMAkdEMQsw\n"
	"CQYDVQQHDAJHWjEMMAoGA1UECgwDU1NUMQ8wDQYDVQQLDAZKZXNzbWExEzARBgNV\n"
	"BAMMCmplc3NtYS5vcmcxHjAcBgkqhkiG9w0BCQEWD2xkY3NhYUAyMWNuLmNvbTCC\n"
	"ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAML+v79+aLQt0Za0dTIZHI5B\n"
	"NDs0g5G8bhdOTlW/kNWflaziZ3GY6d6nJSkQ5e29kyFKxlOD6Gls6bOJ86U71u4R\n"
	"bCmoFvRTDH4q2cJ/+PbiioLpNveDG6lnRCs9JNRQoJrkpRo6urnVnAdsIf6UFjLI\n"
	"dlByNMPGYJ0V8/oKJG5Vu5gcbZV0jVA5+tswkH/zquexEXoKvp18mcwl+pNc/LwW\n"
	"0WnGj0uoJjxHg4GsS78PASjhxMR/2d/1OpgPauldFaNHjVPtaLqJnuejwA6M6Sz8\n"
	"iFPybAQAMpHL9W8kf08jtbnFvnm4ibUkQL5h+OJoIEQa9AVZOSoFG2/g5Zcn8X8C\n"
	"AwEAAaNTMFEwHQYDVR0OBBYEFN49z48DywmoD4cNTQgC6nn2QJoUMB8GA1UdIwQY\n"
	"MBaAFN49z48DywmoD4cNTQgC6nn2QJoUMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZI\n"
	"hvcNAQELBQADggEBALJnYrYBSZLyYX14FQ04zxG3AX0CtQzNOOa7LDrr+H8Ly+nK\n"
	"qS87gg2njMVZH1zM2demtMwydR/F2Ui8ggaduMvc9h5YgQKEwYl8KarJEY03oZoe\n"
	"zbQGBxCXpDOtMs1vujzcl/iZbSzwEDF3g4la5U8q4MlmfGFKz9CJbvoxecqYA206\n"
	"nNbW2XZsW/xMiQv6iAw5iP/LOR9HAyxcvXIsL790nfcgnTYLmyP254Dj4outc6R+\n"
	"PA+f/c1FvkbUBTR5vJt2tsvHcNU218rY2hyOIhDmZeUWprqBO19sUk3scLbVPr3+\n"
	"WEWEl2XaCekKuPtAnMgVQuFsocXGyiuIhkOe5Z4=\n"
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
LPCTSTR g_c_lpszKeyPasswod2				= _T("123456");

int g_s_iVerifyMode2					= SSL_VM_PEER | SSL_VM_FAIL_IF_NO_PEER_CERT;
LPCTSTR g_s_lpszCAPemCertFileOrPath2	= _T("ca2.crt");
LPCTSTR g_s_lpszPemCertFile2			= _T("server2.cer");
LPCTSTR g_s_lpszPemKeyFile2				= _T("server2.key");
LPCTSTR g_s_lpszKeyPasswod2				= _T("123456");

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
