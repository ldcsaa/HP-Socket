
// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"


// CServerDlg dialog

const CString SPECIAL_SERVER_NAME		= _T("hpsocket.org");
int SPECIAL_SERVER_INDEX				= -1;

const LPCTSTR CServerDlg::DEF_ADDRESS	= _T("0.0.0.0");
const USHORT CServerDlg::HTTP_PORT		= 8080;
const USHORT CServerDlg::HTTPS_PORT		= 8443;

CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServerDlg::IDD, pParent)
	, m_HttpListener(HTTP_NAME)
	, m_HttpsListener(HTTPS_NAME)
	, m_HttpServer(&m_HttpListener)
	, m_HttpsServer(&m_HttpsListener)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_START, m_Start);
	DDX_Control(pDX, IDC_STOP, m_Stop);
	DDX_Control(pDX, IDC_CONN_ID, m_ConnID);
	DDX_Control(pDX, IDC_DISCONNECT, m_DisConn);
	DDX_Control(pDX, IDC_RELEASE, m_Release);
	DDX_Control(pDX, IDC_HTTP, m_RadioHttp);
	DDX_Control(pDX, IDC_HTTPS, m_RadioHttps);
	DDX_Control(pDX, IDC_BIND_ADDR, m_BindAddr);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_VKEYTOITEM()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(USER_INFO_MSG, OnUserInfoMsg)
	ON_BN_CLICKED(IDC_START, &CServerDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CServerDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_DISCONNECT, &CServerDlg::OnBnClickedDisconnect)
	ON_EN_CHANGE(IDC_CONN_ID, &CServerDlg::OnEnChangeConnId)
	ON_BN_CLICKED(IDC_RELEASE, &CServerDlg::OnBnClickedRelease)
END_MESSAGE_MAP()


// CServerDlg message handlers

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	CString strTitle;
	CString strOriginTitle;
	GetWindowText(strOriginTitle);
	strTitle.Format(_T("%s - (%d/%d)"), strOriginTitle, HTTP_PORT, HTTPS_PORT);
	SetWindowText(strTitle);
	m_RadioHttp.SetCheck(BST_CHECKED);

	m_BindAddr.SetWindowText(DEF_ADDRESS);

	::SetMainWnd(this);
	::SetInfoList(&m_Info);
	SetAppState(ST_STOPPED);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CServerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (
			pMsg->message == WM_KEYDOWN		
			&&(	pMsg->wParam == VK_ESCAPE	 
			||	pMsg->wParam == VK_CANCEL	
			||	pMsg->wParam == VK_RETURN	
		))
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CServerDlg::SetAppState(EnAppState state)
{
	m_enState = state;

	if(this->GetSafeHwnd() == nullptr)
		return;

	m_Start.EnableWindow(m_enState == ST_STOPPED);
	m_Stop.EnableWindow(m_enState == ST_STARTED);
	m_RadioHttp.EnableWindow(m_enState == ST_STARTED && m_ConnID.GetWindowTextLength() > 0);
	m_RadioHttps.EnableWindow(m_enState == ST_STARTED && m_ConnID.GetWindowTextLength() > 0);
	m_DisConn.EnableWindow(m_enState == ST_STARTED && m_ConnID.GetWindowTextLength() > 0);
	m_Release.EnableWindow(m_enState == ST_STARTED && m_ConnID.GetWindowTextLength() > 0);
	m_BindAddr.EnableWindow(m_enState == ST_STOPPED);
}

int CALLBACK CServerDlg::SIN_ServerNameCallback(LPCTSTR lpszServerName)
{
	if(::IsIPAddress(lpszServerName))
		return 0;

	int len  = lstrlen(lpszServerName);
	int diff = len - SPECIAL_SERVER_NAME.GetLength();

	if(diff < 0)
		return 0;

	if(SPECIAL_SERVER_NAME.CompareNoCase(lpszServerName + diff) != 0)
		return 0;

	return SPECIAL_SERVER_INDEX;
}

void CServerDlg::OnBnClickedStart()
{
	SetAppState(ST_STARTING);

	m_HttpsServer.CleanupSSLContext();
	if(m_HttpsServer.SetupSSLContext(SSL_VM_NONE, g_s_lpszPemCertFile, g_s_lpszPemKeyFile, g_s_lpszKeyPasswod, g_s_lpszCAPemCertFileOrPath, SIN_ServerNameCallback))
		SPECIAL_SERVER_INDEX = m_HttpsServer.AddSSLContext(SSL_VM_NONE, g_s_lpszPemCertFile2, g_s_lpszPemKeyFile2, g_s_lpszKeyPasswod2, g_s_lpszCAPemCertFileOrPath2);
	else
	{
		::LogServerStartFail(::GetLastError(), _T("initialize SSL env fail"));
		SetAppState(ST_STOPPED);

		return;
	}

	CString strBindAddr;
	m_BindAddr.GetWindowText(strBindAddr);
	strBindAddr.Trim();

	if(strBindAddr.IsEmpty())
		strBindAddr = DEF_ADDRESS;

	if(m_HttpServer.Start(strBindAddr, HTTP_PORT))
	{
		::LogServerStart(strBindAddr, HTTP_PORT, HTTP_NAME);

		if(m_HttpsServer.Start(strBindAddr, HTTPS_PORT))
		{
			::LogServerStart(strBindAddr, HTTPS_PORT, HTTPS_NAME);
			SetAppState(ST_STARTED);
		}
		else
		{
			m_HttpServer.Stop();

			::LogServerStartFail(m_HttpsServer.GetLastError(), m_HttpsServer.GetLastErrorDesc(), HTTPS_NAME);
			SetAppState(ST_STOPPED);
		}
	}
	else
	{
		::LogServerStartFail(m_HttpServer.GetLastError(), m_HttpServer.GetLastErrorDesc(), HTTP_NAME);
		SetAppState(ST_STOPPED);
	}
}

void CServerDlg::OnBnClickedStop()
{
	SetAppState(ST_STOPPING);

	if(!m_HttpsServer.Stop())
		ASSERT(FALSE);

	::LogServerStop(HTTPS_NAME);

	if(!m_HttpServer.Stop())
		ASSERT(FALSE);

	::LogServerStop(HTTP_NAME);

	SetAppState(ST_STOPPED);

}

void CServerDlg::OnBnClickedDisconnect()
{
	CString strConnID;
	m_ConnID.GetWindowText(strConnID);
	CONNID dwConnID = (CONNID)_ttoi(strConnID);

	BOOL bHttp = m_RadioHttp.GetCheck() == BST_CHECKED;
	LPCTSTR lpszName = bHttp ? HTTP_NAME : HTTPS_NAME;

	IHttpServer* pServer = bHttp ? (IHttpServer*)&m_HttpServer : (IHttpServer*)&m_HttpsServer;

	if(pServer->Disconnect(dwConnID))
		::LogDisconnect(dwConnID, lpszName);
	else
		::LogDisconnectFail(dwConnID, lpszName);
}

void CServerDlg::OnBnClickedRelease()
{
	CString strConnID;
	m_ConnID.GetWindowText(strConnID);
	CONNID dwConnID = (CONNID)_ttoi(strConnID);

	BOOL bHttp = m_RadioHttp.GetCheck() == BST_CHECKED;
	LPCTSTR lpszName = bHttp ? HTTP_NAME : HTTPS_NAME;

	IHttpServer* pServer = bHttp ? (IHttpServer*)&m_HttpServer : (IHttpServer*)&m_HttpsServer;

	if(pServer->Release(dwConnID))
		::LogRelease(dwConnID, lpszName);
	else
		::LogReleaseFail(dwConnID, lpszName);
}

void CServerDlg::OnEnChangeConnId()
{
	m_RadioHttp.EnableWindow(m_enState == ST_STARTED && m_ConnID.GetWindowTextLength() > 0);
	m_RadioHttps.EnableWindow(m_enState == ST_STARTED && m_ConnID.GetWindowTextLength() > 0);
	m_DisConn.EnableWindow(m_enState == ST_STARTED && m_ConnID.GetWindowTextLength() > 0);
	m_Release.EnableWindow(m_enState == ST_STARTED && m_ConnID.GetWindowTextLength() > 0);
}

int CServerDlg::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex)
{
	if(nKey == 'C')
		pListBox->ResetContent();

	return __super::OnVKeyToItem(nKey, pListBox, nIndex);
}

LRESULT CServerDlg::OnUserInfoMsg(WPARAM wp, LPARAM lp)
{
	info_msg* msg = (info_msg*)wp;

	::LogInfoMsg(msg);

	return 0;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHandleResult CHttpServerListenerImpl::OnPrepareListen(ITcpServer* pSender, SOCKET soListen)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;
	
	pSender->GetListenAddress(szAddress, iAddressLen, usPort);
	::PostOnPrepareListen(szAddress, usPort, m_strName);

	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnAccept(ITcpServer* pSender, CONNID dwConnID, SOCKET soClient)
{
	BOOL bPass = TRUE;
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	pSender->GetRemoteAddress(dwConnID, szAddress, iAddressLen, usPort);

	::PostOnAccept(dwConnID, szAddress, usPort, bPass, m_strName);

	return bPass ? HR_OK : HR_ERROR;
}

EnHandleResult CHttpServerListenerImpl::OnHandShake(ITcpServer* pSender, CONNID dwConnID)
{
	::PostOnHandShake(dwConnID, m_strName);
	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnReceive(dwConnID, pData, iLength, m_strName);

	if(pSender->Send(dwConnID, pData, iLength))
		return HR_OK;
	else
		return HR_ERROR;
}

EnHandleResult CHttpServerListenerImpl::OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength, m_strName);
	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID, m_strName)	:
	::PostOnError(dwConnID, enOperation, iErrorCode, m_strName)	;

	CBufferPtr* pBuffer = nullptr;
	pSender->GetConnectionExtra(dwConnID, (PVOID*)&pBuffer);

	if(pBuffer) delete pBuffer;

	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnShutdown(ITcpServer* pSender)
{
	::PostOnShutdown(m_strName);
	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHttpParseResult CHttpServerListenerImpl::OnMessageBegin(IHttpServer* pSender, CONNID dwConnID)
{
	::PostOnMessageBegin(dwConnID, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnRequestLine(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)
{
	::PostOnRequestLine(dwConnID, lpszMethod, pSender->GetUrlFieldSet(dwConnID), lpszUrl, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnHeader(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
{
	::PostOnHeader(dwConnID, lpszName, lpszValue, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnHeadersComplete(IHttpServer* pSender, CONNID dwConnID)
{
	CStringA strSummary = GetHeaderSummary(pSender, dwConnID, "    ", 0, TRUE);
	::PostOnHeadersComplete(dwConnID, strSummary, m_strName);

	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnBody(IHttpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnBody(dwConnID, pData, iLength, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnChunkHeader(IHttpServer* pSender, CONNID dwConnID, int iLength)
{
	::PostOnChunkHeader(dwConnID, iLength, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnChunkComplete(IHttpServer* pSender, CONNID dwConnID)
{
	::PostOnChunkComplete(dwConnID, m_strName);
	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnMessageComplete(IHttpServer* pSender, CONNID dwConnID)
{
	::PostOnMessageComplete(dwConnID, m_strName);

	if(pSender->IsUpgrade(dwConnID))
		return HPR_OK;

	CStringA strBody = GetHeaderSummary(pSender, dwConnID, "    ", 0, FALSE);
	int iBodyLength  = strBody.GetLength();
	BOOL bSkipBody	 = FALSE;

	if(strcmp(pSender->GetMethod(dwConnID), HTTP_METHOD_HEAD) == 0)
		bSkipBody = TRUE;

	CStringA strContentLength;
	strContentLength.Format("%u", iBodyLength);

	DWORD dwSeq				= 1;
	LPCSTR lpszReqSequence	= nullptr;
	CStringA strSeq;

	if(pSender->GetCookie(dwConnID, "__reqSequence_1", &lpszReqSequence))
		dwSeq += atoi(lpszReqSequence);

	strSeq.Format("%u", dwSeq);

	CStringA strSeqCookie1 = CCookie::ToString("__reqSequence_1", strSeq, nullptr, nullptr, -1, TRUE, TRUE, CCookie::SS_LAX);

	dwSeq			= 1;
	lpszReqSequence	= nullptr;

	if(pSender->GetCookie(dwConnID, "__reqSequence_2", &lpszReqSequence))
		dwSeq += atoi(lpszReqSequence);

	strSeq.Format("%u", dwSeq);

	CStringA strSeqCookie2 = CCookie::ToString("__reqSequence_2", strSeq, nullptr, "/", 300, FALSE, FALSE, CCookie::SS_NONE);

	THeader header[] = {{"Content-Type", "text/plain"}, {"Content-Length", strContentLength}, {"Set-Cookie", strSeqCookie1}, {"Set-Cookie", strSeqCookie2}};
	int iHeaderCount = sizeof(header) / sizeof(THeader);

	if(bSkipBody)
	{
		strBody.Empty();
		iBodyLength = 0;
	}

	pSender->SendResponse(	dwConnID,
							HSC_OK,
							"HP Http Server OK",
							header, iHeaderCount,
							(const BYTE*)(LPCSTR)strBody,
							iBodyLength);

	if(!pSender->IsKeepAlive(dwConnID))
		pSender->Release(dwConnID);

	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnUpgrade(IHttpServer* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
{
	::PostOnUpgrade(dwConnID, enUpgradeType, m_strName);

	if(enUpgradeType == HUT_HTTP_TUNNEL)
	{
		pSender->SendResponse(dwConnID, HSC_OK, "Connection Established");
	}
	else if(enUpgradeType == HUT_WEB_SOCKET)
	{
		THeader header[] = {{"Connection", UPGRADE_HEADER},
							{UPGRADE_HEADER, WEB_SOCKET_HEADER_VALUE},
							{nullptr, nullptr},
							{nullptr, nullptr}};

		LPCSTR lpszAccept = nullptr;
		
		if(!pSender->GetHeader(dwConnID, "Sec-WebSocket-Key", &lpszAccept))
			return HPR_ERROR;

		CStringA strAccept;
		::MakeSecWebSocketAccept(lpszAccept, strAccept);

		header[2].name	= "Sec-WebSocket-Accept";
		header[2].value	= strAccept;

		LPCSTR lpszProtocol = nullptr;

		if(pSender->GetHeader(dwConnID, "Sec-WebSocket-Protocol", &lpszProtocol))
		{
			int i = 0;
			CStringA strProtocol(lpszProtocol);
			CStringA strFirst = strProtocol.Tokenize(", ", i);

			if(!strFirst.IsEmpty())
			{
				header[3].name	= "Sec-WebSocket-Protocol";
				header[3].value	= strFirst;
			}
		}

		int iHeaderCount = sizeof(header) / sizeof(THeader);

		pSender->SendResponse(dwConnID, HSC_SWITCHING_PROTOCOLS, nullptr, header, iHeaderCount);
		pSender->SetConnectionExtra(dwConnID, new CBufferPtr);
	}
	else
		ASSERT(FALSE);

	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnParseError(IHttpServer* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
{
	::PostOnParseError(dwConnID, iErrorCode, lpszErrorDesc, m_strName);
	return HPR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHandleResult CHttpServerListenerImpl::OnWSMessageHeader(IHttpServer* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
{
	::PostOnWSMessageHeader(dwConnID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen, m_strName);

	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnWSMessageBody(IHttpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnWSMessageBody(dwConnID, pData, iLength, m_strName);

	CBufferPtr* pBuffer = nullptr;
	pSender->GetConnectionExtra(dwConnID, (PVOID*)&pBuffer);
	VERIFY(pBuffer);

	pBuffer->Cat(pData, iLength);

	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnWSMessageComplete(IHttpServer* pSender, CONNID dwConnID)
{
	::PostOnWSMessageComplete(dwConnID, m_strName);

	CBufferPtr* pBuffer = nullptr;
	pSender->GetConnectionExtra(dwConnID, (PVOID*)&pBuffer);
	VERIFY(pBuffer);

	BOOL bFinal;
	BYTE iReserved, iOperationCode;

	VERIFY(pSender->GetWSMessageState(dwConnID, &bFinal, &iReserved, &iOperationCode, nullptr, nullptr, nullptr));

	pSender->SendWSMessage(dwConnID, bFinal, iReserved, iOperationCode, nullptr, pBuffer->Ptr(), (int)pBuffer->Size());
	pBuffer->Free();

	if(iOperationCode == 0x8)
		pSender->Disconnect(dwConnID);

	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

CStringA CHttpServerListenerImpl::GetHeaderSummary(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszSep, int iSepCount, BOOL bWithContentLength)
{
	CStringA SEP1;

	for(int i = 0; i < iSepCount; i++)
		SEP1 += lpszSep;

	CStringA SEP2(SEP1);
	SEP2 += lpszSep;

	CStringA strResult;

	USHORT usUrlFieldSet = pSender->GetUrlFieldSet(dwConnID);

	strResult.AppendFormat("%s[URL Fields]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "SCHEMA", pSender->GetUrlField(dwConnID, HUF_SCHEMA), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "HOST", pSender->GetUrlField(dwConnID, HUF_HOST), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "PORT", pSender->GetUrlField(dwConnID, HUF_PORT), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "PATH", pSender->GetUrlField(dwConnID, HUF_PATH), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "QUERY", pSender->GetUrlField(dwConnID, HUF_QUERY), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "FRAGMENT", pSender->GetUrlField(dwConnID, HUF_FRAGMENT), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "USERINFO", pSender->GetUrlField(dwConnID, HUF_USERINFO), CRLF);	

	DWORD dwHeaderCount = 0;
	pSender->GetAllHeaders(dwConnID, nullptr, dwHeaderCount);

	strResult.AppendFormat("%s[Request Headers]%s", SEP1, CRLF);

	if(dwHeaderCount == 0)
		strResult.AppendFormat("%s(no header)%s", SEP2, CRLF);
	else
	{
		unique_ptr<THeader[]> headers(new THeader[dwHeaderCount]);
		VERIFY(pSender->GetAllHeaders(dwConnID, headers.get(), dwHeaderCount));

		for(DWORD i = 0; i < dwHeaderCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, headers[i].name, headers[i].value, CRLF);
	}

	DWORD dwCookieCount = 0;
	pSender->GetAllCookies(dwConnID, nullptr, dwCookieCount);

	strResult.AppendFormat("%s[Cookies]%s", SEP1, CRLF);

	if(dwCookieCount == 0)
		strResult.AppendFormat("%s(no cookie)%s", SEP2, CRLF);
	else
	{
		unique_ptr<TCookie[]> cookies(new TCookie[dwCookieCount]);
		VERIFY(pSender->GetAllCookies(dwConnID, cookies.get(), dwCookieCount));

		for(DWORD i = 0; i < dwCookieCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, cookies[i].name, cookies[i].value, CRLF);
	}

	CStringA strVersion;
	::HttpVersionToString((EnHttpVersion)pSender->GetVersion(dwConnID), strVersion);
	EnHttpUpgradeType enUpgType	= pSender->GetUpgradeType(dwConnID);
	LPCSTR lpszUpgrade			= enUpgType != HUT_NONE ? "true" : "false";
	LPCSTR lpszKeepAlive		= pSender->IsKeepAlive(dwConnID) ? "true" : "false";

	strResult.AppendFormat("%s[Basic Info]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "Version", strVersion, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "Method", pSender->GetMethod(dwConnID), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsUpgrade", lpszUpgrade, CRLF);
	if(enUpgType != HUT_NONE)
		strResult.AppendFormat("%s%13s: %d%s", SEP2, "UpgradeType", enUpgType, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsKeepAlive", lpszKeepAlive, CRLF);
	if(bWithContentLength)
		strResult.AppendFormat("%s%13s: %lld%s", SEP2, "ContentLength", pSender->GetContentLength(dwConnID), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "ContentType", pSender->GetContentType(dwConnID), CRLF);
 
	return strResult;
}
