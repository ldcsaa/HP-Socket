#include "stdafx.h"
#include "helper.h"

CWnd*		g_pMainWnd;
CListBox*	g_pInfoList;

info_msg* info_msg::Construct(CONNID dwConnID, LPCTSTR lpszEvent, int iContentLength, LPCTSTR lpszContent)
{
	return new info_msg(dwConnID, lpszEvent, iContentLength, lpszContent);
}

void info_msg::Destruct(info_msg* pMsg)
{
	delete pMsg;
}

info_msg::info_msg(CONNID dwConnID, LPCTSTR lpszEvent, int iContentLength, LPCTSTR lpszContent)
	: connID(dwConnID), evt(lpszEvent), contentLength(iContentLength), content(lpszContent)
{

}

info_msg::~info_msg()
{
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

void LogServerStart(LPCTSTR lpszAddress, USHORT port)
{
	CString msg;
	msg.Format(_T("$ Server Start OK --> (%s : %d)"), lpszAddress, port);
	LogMsg(msg);
}

void LogServerStartFail(DWORD code, LPCTSTR lpszDesc)
{
	CString msg;
	msg.Format(_T("$ Server Start Fail --> %s (%d)"), lpszDesc, code);
	LogMsg(msg);
}

void LogServerStop()
{
	CString msg = _T("$ Server Stop");
	LogMsg(msg);
}

void LogServerStopFail(DWORD code, LPCTSTR lpszDesc)
{
	CString msg;
	msg.Format(_T("$ Server Stop Fail --> %s (%d)"), lpszDesc, code);
	LogMsg(msg);
}

void LogClientStart(LPCTSTR lpszAddress, USHORT port)
{
	CString msg;
	msg.Format(_T("$ Client Start OK --> (%s : %d)"), lpszAddress, port);
	LogMsg(msg);
}

void LogClientStarting(LPCTSTR lpszAddress, USHORT port)
{
	CString msg;
	msg.Format(_T("$ Client Starting ... --> (%s : %d)"), lpszAddress, port);
	LogMsg(msg);
}

void LogClientStartFail(DWORD code, LPCTSTR lpszDesc)
{
	CString msg;
	msg.Format(_T("$ Client Start Fail --> %s (%d)"), lpszDesc, code);
	LogMsg(msg);
}

void LogClientStopping(CONNID dwConnID)
{
	CString msg;
	msg.Format(_T("$ Client Stopping ... --> (%Iu)"), dwConnID);
	LogMsg(msg);
}

void LogClientStopFail(DWORD code, LPCTSTR lpszDesc)
{
	CString msg;
	msg.Format(_T("$ Client Stop Fail --> %s (%d)"), lpszDesc, code);
	LogMsg(msg);
}

void LogClientSendFail(int iSequence, int iSocketIndex, DWORD code, LPCTSTR lpszDesc)
{
	CString msg;
	msg.Format(_T("$ Client Send Fail [SOCK: %d, SEQ: %d] --> %s (%d)"), iSocketIndex, iSequence, lpszDesc, code);
	LogMsg(msg);
}

void LogSend(CONNID dwConnID, LPCTSTR lpszContent)
{
	CString msg;
	msg.Format(_T("$ (%Iu) Send OK --> %s"), dwConnID, lpszContent);
	LogMsg(msg);
}

void LogSendFail(CONNID dwConnID, DWORD code, LPCTSTR lpszDesc)
{
	CString msg;
	msg.Format(_T("$ (%Iu) Send Fail --> %s (%d)"), dwConnID, lpszDesc, code);
	LogMsg(msg);
}

void LogDisconnect(CONNID dwConnID)
{
	CString msg;
	msg.Format(_T("$ (%Iu) Disconnect OK"), dwConnID);
	LogMsg(msg);
}

void LogDisconnectFail(CONNID dwConnID)
{
	CString msg;
	msg.Format(_T("$ (%Iu) Disconnect Fail"), dwConnID);
	LogMsg(msg);
}

void LogDetect(CONNID dwConnID)
{
	CString msg;
	msg.Format(_T("$ (%Iu) Detect Connection OK"), dwConnID);
	LogMsg(msg);
}

void LogDetectFail(CONNID dwConnID)
{
	CString msg;
	msg.Format(_T("$ (%Iu) Detect Connection Fail"), dwConnID);
	LogMsg(msg);
}

void LogOnConnect(CONNID dwConnID, const CString& strAddress, USHORT usPort)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("local address: %s:%d"), strAddress, usPort);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CONNECT, content_len, lpszContent);

	LogInfoMsg(msg);
}

void LogOnConnect2(CONNID dwConnID)
{
	CString msg;
	msg.Format(_T("  > [ %Iu, %s ]"), dwConnID, EVT_ON_CONNECT);
	LogMsg(msg);
}

void PostOnSend(CONNID dwConnID, const BYTE* pData, int iLength)
{
	/*
	LPTSTR lpszContent = nullptr;
	int content_len = ::BytesToHex(pData, iLength, &lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_SEND, content_len, lpszContent);
	*/

	LPTSTR lpszContent = new TCHAR[20];
	wsprintf(lpszContent, _T("(%d bytes)"), iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_SEND, content_len, lpszContent);

	PostInfoMsg(msg);
}

void PostOnReceive(CONNID dwConnID, const BYTE* pData, int iLength)
{
	/*
	LPTSTR lpszContent = nullptr;
	int content_len = ::BytesToHex(pData, iLength, &lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_RECEIVE, content_len, lpszContent);
	*/

	LPTSTR lpszContent = new TCHAR[20];
	wsprintf(lpszContent, _T("(%d bytes)"), iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_RECEIVE, content_len, lpszContent);

	PostInfoMsg(msg);
}

void PostOnClose(CONNID dwConnID)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CLOSE, 0, nullptr);

	PostInfoMsg(msg);
}

void PostOnError(CONNID dwConnID, int enOperation, int iErrorCode)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("OP: %d, CODE: %d"), enOperation, iErrorCode);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_ERROR, content_len, lpszContent);

	PostInfoMsg(msg);
}

void PostOnAccept(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, BOOL bPass)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("%s (%s:%d)"), bPass ? _T("PASS") : _T("REJECT"), lpszAddress, usPort);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_ACCEPT, content_len, lpszContent);

	PostInfoMsg(msg);
}

void PostOnAccept2(CONNID dwConnID)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_ACCEPT, 0, nullptr);

	PostInfoMsg(msg);
}

void PostOnPrepareListen(LPCTSTR lpszAddress, USHORT usPort)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("bind address: %s:%d"), lpszAddress, usPort);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(0, EVT_ON_PREPARE_LISTEN, content_len, lpszContent);

	LogInfoMsg(msg);
}

void PostOnPrepareConnect(CONNID dwConnID)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_PREPARE_CONNECT, 0, nullptr);

	LogInfoMsg(msg);
}

void PostOnConnect(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("local address: %s:%d"), lpszAddress, usPort);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CONNECT, content_len, lpszContent);

	PostInfoMsg(msg);
}

void PostOnShutdown()
{
	info_msg* msg = info_msg::Construct(0, EVT_ON_SHUTDOWN, 0, nullptr);

	PostInfoMsg(msg);
}

void PostServerStatics(const LONGLONG& llTotalSent, const LONGLONG& llTotalReceived)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T(" *** Summary: send - %I64d, recv - %I64d"), llTotalSent, llTotalReceived);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(0, EVT_ON_END_TEST, content_len, lpszContent);

	PostInfoMsg(msg);
}

void PostTimeConsuming(DWORD dwTickCount)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("Total Time Consuming: %u"), dwTickCount);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(0, EVT_ON_END_TEST, content_len, lpszContent);

	PostInfoMsg(msg);
}

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

	LogMsg(msg);

	info_msg::Destruct(pInfoMsg);
}

void LogMsg(const CString& msg)
{
	if(!g_pInfoList || !g_pInfoList->GetSafeHwnd())
		return;

	int iCurIndex	= g_pInfoList->GetCurSel();
	int iCount		= g_pInfoList->GetCount();
	BOOL bCurLast	= (iCurIndex != LB_ERR && iCurIndex == iCount - 1);

	g_pInfoList->SetRedraw(FALSE);

	if(iCount > MAX_LOG_RECORD_LENGT)
		g_pInfoList->DeleteString(0);

	g_pInfoList->AddString(msg);

	iCount = g_pInfoList->GetCount();

	if(bCurLast)
		g_pInfoList->SetCurSel(iCount - 1);

	iCurIndex = g_pInfoList->GetCurSel();
	if(iCurIndex == LB_ERR)
	{
		iCurIndex = 0;
		g_pInfoList->SetCurSel(iCurIndex);
	}

	g_pInfoList->SetAnchorIndex(iCurIndex);
	g_pInfoList->SetRedraw(TRUE);
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
