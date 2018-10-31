
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
CServerDlg* CServerDlg::m_spThis		= nullptr;

CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_spThis = this;

	// 创建监听器对象
	m_HttpServerListener = ::Create_HP_HttpServerListener();

	// 创建 HTTP Server 对象
	m_HttpServer = ::Create_HP_HttpServer(m_HttpServerListener);
	m_HttpsServer = ::Create_HP_HttpsServer(m_HttpServerListener);

	// 初始化 SSL 环境参数
	VERIFY(::HP_SSLServer_SetupSSLContext(m_HttpsServer, SSL_VM_NONE, g_s_lpszPemCertFile, g_s_lpszPemKeyFile, g_s_lpszKeyPasswod, g_s_lpszCAPemCertFileOrPath, SIN_ServerNameCallback));
	SPECIAL_SERVER_INDEX = ::HP_SSLServer_AddSSLContext(m_HttpsServer, SSL_VM_NONE, g_s_lpszPemCertFile2, g_s_lpszPemKeyFile2, g_s_lpszKeyPasswod2, g_s_lpszCAPemCertFileOrPath2);
	VERIFY(SPECIAL_SERVER_INDEX > 0);

	// 设置 HTTP 监听器回调函数
	::HP_Set_FN_HttpServer_OnPrepareListen(m_HttpServerListener, OnPrepareListen);
	::HP_Set_FN_HttpServer_OnAccept(m_HttpServerListener, OnAccept);
	::HP_Set_FN_HttpServer_OnHandShake(m_HttpServerListener, OnHandShake);
	::HP_Set_FN_HttpServer_OnSend(m_HttpServerListener, OnSend);
	::HP_Set_FN_HttpServer_OnReceive(m_HttpServerListener, OnReceive);
	::HP_Set_FN_HttpServer_OnClose(m_HttpServerListener, OnClose);
	::HP_Set_FN_HttpServer_OnShutdown(m_HttpServerListener, OnShutdown);

	::HP_Set_FN_HttpServer_OnMessageBegin(m_HttpServerListener, OnMessageBegin);
	::HP_Set_FN_HttpServer_OnRequestLine(m_HttpServerListener, OnRequestLine);
	::HP_Set_FN_HttpServer_OnHeader(m_HttpServerListener, OnHeader);
	::HP_Set_FN_HttpServer_OnHeadersComplete(m_HttpServerListener, OnHeadersComplete);
	::HP_Set_FN_HttpServer_OnBody(m_HttpServerListener, OnBody);
	::HP_Set_FN_HttpServer_OnChunkHeader(m_HttpServerListener, OnChunkHeader);
	::HP_Set_FN_HttpServer_OnChunkComplete(m_HttpServerListener, OnChunkComplete);
	::HP_Set_FN_HttpServer_OnMessageComplete(m_HttpServerListener, OnMessageComplete);
	::HP_Set_FN_HttpServer_OnUpgrade(m_HttpServerListener, OnUpgrade);
	::HP_Set_FN_HttpServer_OnParseError(m_HttpServerListener, OnParseError);

	::HP_Set_FN_HttpServer_OnWSMessageHeader(m_HttpServerListener, OnWSMessageHeader);
	::HP_Set_FN_HttpServer_OnWSMessageBody(m_HttpServerListener, OnWSMessageBody);
	::HP_Set_FN_HttpServer_OnWSMessageComplete(m_HttpServerListener, OnWSMessageComplete);
}

CServerDlg::~CServerDlg()
{
	// 清理 SSL 运行环境（可选，删除 m_HttpsServer 时会自动清理）
	::HP_SSLServer_CleanupSSLContext(m_HttpsServer);

	// 销毁 HTTP 对象
	::Destroy_HP_HttpServer(m_HttpServer);
	::Destroy_HP_HttpsServer(m_HttpsServer);

	// 销毁监听器对象
	::Destroy_HP_HttpServerListener(m_HttpServerListener);
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
	if(::SYS_IsIPAddress(lpszServerName, nullptr))
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
	CString strBindAddr;
	m_BindAddr.GetWindowText(strBindAddr);
	strBindAddr.Trim();

	if(strBindAddr.IsEmpty())
		strBindAddr = DEF_ADDRESS;

	SetAppState(ST_STARTING);

	if(::HP_Server_Start(m_HttpServer, strBindAddr, HTTP_PORT))
	{
		::LogServerStart(strBindAddr, HTTP_PORT, HTTP_NAME);

		if(::HP_Server_Start(m_HttpsServer, strBindAddr, HTTPS_PORT))
		{
			::LogServerStart(strBindAddr, HTTPS_PORT, HTTPS_NAME);
			SetAppState(ST_STARTED);
		}
		else
		{
			::HP_Server_Stop(m_HttpServer);

			::LogServerStartFail(::HP_Server_GetLastError(m_HttpsServer), ::HP_Server_GetLastErrorDesc(m_HttpsServer), HTTPS_NAME);
			SetAppState(ST_STOPPED);
		}
	}
	else
	{
		::LogServerStartFail(::HP_Server_GetLastError(m_HttpServer), ::HP_Server_GetLastErrorDesc(m_HttpServer), HTTP_NAME);
		SetAppState(ST_STOPPED);
	}
}

void CServerDlg::OnBnClickedStop()
{
	SetAppState(ST_STOPPING);

	if(!::HP_Server_Stop(m_HttpsServer))
		ASSERT(FALSE);

	::LogServerStop(HTTPS_NAME);

	if(!::HP_Server_Stop(m_HttpServer))
		ASSERT(FALSE);

	::LogServerStop(HTTP_NAME);

	SetAppState(ST_STOPPED);

}

void CServerDlg::OnBnClickedDisconnect()
{
	CString strConnID;
	m_ConnID.GetWindowText(strConnID);
	HP_CONNID dwConnID = (HP_CONNID)_ttoi(strConnID);

	BOOL bHttp = m_RadioHttp.GetCheck() == BST_CHECKED;
	LPCTSTR lpszName = bHttp ? HTTP_NAME : HTTPS_NAME;

	HP_HttpServer pServer = bHttp ? m_HttpServer : m_HttpsServer;

	if(::HP_Server_Disconnect(pServer, dwConnID, TRUE))
		::LogDisconnect(dwConnID, lpszName);
	else
		::LogDisconnectFail(dwConnID, lpszName);
}

void CServerDlg::OnBnClickedRelease()
{
	CString strConnID;
	m_ConnID.GetWindowText(strConnID);
	HP_CONNID dwConnID = (HP_CONNID)_ttoi(strConnID);

	BOOL bHttp = m_RadioHttp.GetCheck() == BST_CHECKED;
	LPCTSTR lpszName = bHttp ? HTTP_NAME : HTTPS_NAME;

	HP_HttpServer pServer = bHttp ? m_HttpServer : m_HttpsServer;

	if(::HP_HttpServer_Release(pServer, dwConnID))
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

EnHandleResult CServerDlg::OnPrepareListen(HP_Server pSender, SOCKET soListen)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;
	
	::HP_Server_GetListenAddress(pSender, szAddress, &iAddressLen, &usPort);
	::PostOnPrepareListen(szAddress, usPort, m_spThis->GetSenderName(pSender));

	return HR_OK;
}

EnHandleResult CServerDlg::OnAccept(HP_Server pSender, HP_CONNID dwConnID, SOCKET soClient)
{
	BOOL bPass = TRUE;
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	::HP_Server_GetRemoteAddress(pSender, dwConnID, szAddress, &iAddressLen, &usPort);

	::PostOnAccept(dwConnID, szAddress, usPort, bPass, m_spThis->GetSenderName(pSender));

	return bPass ? HR_OK : HR_ERROR;
}

EnHandleResult CServerDlg::OnHandShake(HP_Server pSender, HP_CONNID dwConnID)
{
	::PostOnHandShake(dwConnID, m_spThis->GetSenderName(pSender));
	return HR_OK;
}

EnHandleResult CServerDlg::OnReceive(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnReceive(dwConnID, pData, iLength, m_spThis->GetSenderName(pSender));

	if(::HP_Server_Send(pSender, dwConnID, pData, iLength))
		return HR_OK;
	else
		return HR_ERROR;
}

EnHandleResult CServerDlg::OnSend(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength, m_spThis->GetSenderName(pSender));
	return HR_OK;
}

EnHandleResult CServerDlg::OnClose(HP_Server pSender, HP_CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID, m_spThis->GetSenderName(pSender))		:
	::PostOnError(dwConnID, enOperation, iErrorCode, m_spThis->GetSenderName(pSender))	;

	CBufferPtr* pBuffer = nullptr;
	::HP_Server_GetConnectionExtra(pSender, dwConnID, (PVOID*)&pBuffer);

	if(pBuffer) delete pBuffer;

	return HR_OK;
}

EnHandleResult CServerDlg::OnShutdown(HP_Server pSender)
{
	::PostOnShutdown(m_spThis->GetSenderName(pSender));
	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHttpParseResult CServerDlg::OnMessageBegin(HP_HttpServer pSender, HP_CONNID dwConnID)
{
	::PostOnMessageBegin(dwConnID, m_spThis->GetSenderName(pSender));
	return HPR_OK;
}

EnHttpParseResult CServerDlg::OnRequestLine(HP_HttpServer pSender, HP_CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)
{
	::PostOnRequestLine(dwConnID, lpszMethod, ::HP_HttpServer_GetUrlFieldSet(pSender, dwConnID), lpszUrl, m_spThis->GetSenderName(pSender));
	return HPR_OK;
}

EnHttpParseResult CServerDlg::OnHeader(HP_HttpServer pSender, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
{
	::PostOnHeader(dwConnID, lpszName, lpszValue, m_spThis->GetSenderName(pSender));
	return HPR_OK;
}

EnHttpParseResult CServerDlg::OnHeadersComplete(HP_HttpServer pSender, HP_CONNID dwConnID)
{
	CStringA strSummary = m_spThis->GetHeaderSummary(pSender, dwConnID, "    ", 0, TRUE);
	::PostOnHeadersComplete(dwConnID, strSummary, m_spThis->GetSenderName(pSender));

	return HPR_OK;
}

EnHttpParseResult CServerDlg::OnBody(HP_HttpServer pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnBody(dwConnID, pData, iLength, m_spThis->GetSenderName(pSender));
	return HPR_OK;
}

EnHttpParseResult CServerDlg::OnChunkHeader(HP_HttpServer pSender, HP_CONNID dwConnID, int iLength)
{
	::PostOnChunkHeader(dwConnID, iLength, m_spThis->GetSenderName(pSender));
	return HPR_OK;
}

EnHttpParseResult CServerDlg::OnChunkComplete(HP_HttpServer pSender, HP_CONNID dwConnID)
{
	::PostOnChunkComplete(dwConnID, m_spThis->GetSenderName(pSender));
	return HPR_OK;
}

EnHttpParseResult CServerDlg::OnMessageComplete(HP_HttpServer pSender, HP_CONNID dwConnID)
{
	::PostOnMessageComplete(dwConnID, m_spThis->GetSenderName(pSender));

	if(::HP_HttpServer_IsUpgrade(pSender, dwConnID))
		return HPR_OK;

	CStringA strBody = m_spThis->GetHeaderSummary(pSender, dwConnID, "    ", 0, FALSE);
	int iBodyLength  = strBody.GetLength();
	BOOL bSkipBody	 = FALSE;

	if(strcmp(::HP_HttpServer_GetMethod(pSender, dwConnID), HTTP_METHOD_HEAD) == 0)
		bSkipBody = TRUE;

	CStringA strContentLength;
	strContentLength.Format("%u", iBodyLength);

	DWORD dwSeq				= 1;
	int iSize				= MAX_PATH;
	LPCSTR lpszReqSequence	= nullptr;
	CStringA strSeq;

	if(::HP_HttpServer_GetCookie(pSender, dwConnID, "__reqSequence_1", &lpszReqSequence))
		dwSeq += atoi(lpszReqSequence);

	strSeq.Format("%u", dwSeq);

	CStringA strSeqCookie1;
	LPSTR lpszSeqCookie = strSeqCookie1.GetBuffer(iSize);
	VERIFY(::HP_HttpCookie_HLP_ToString(lpszSeqCookie, &iSize, "__reqSequence_1", strSeq, nullptr, nullptr, -1, TRUE, TRUE, 2));
	strSeqCookie1.ReleaseBuffer();

	dwSeq			= 1;
	iSize			= MAX_PATH;
	lpszReqSequence	= nullptr;

	if(::HP_HttpServer_GetCookie(pSender, dwConnID, "__reqSequence_2", &lpszReqSequence))
		dwSeq += atoi(lpszReqSequence);

	strSeq.Format("%u", dwSeq);

	CStringA strSeqCookie2;
	lpszSeqCookie = strSeqCookie2.GetBuffer(iSize);
	VERIFY(::HP_HttpCookie_HLP_ToString(lpszSeqCookie, &iSize, "__reqSequence_2", strSeq, nullptr, "/", 300, FALSE, FALSE, 0));
	strSeqCookie2.ReleaseBuffer();

	THeader header[] = {{"Content-Type", "text/plain"}, {"Content-Length", strContentLength}, {"Set-Cookie", strSeqCookie1}, {"Set-Cookie", strSeqCookie2}};
	int iHeaderCount = sizeof(header) / sizeof(THeader);

	if(bSkipBody)
	{
		strBody.Empty();
		iBodyLength = 0;
	}

	::HP_HttpServer_SendResponse(pSender,
								dwConnID, HSC_OK,
								"HP Http Server OK",
								header, iHeaderCount,
								(const BYTE*)(LPCSTR)strBody,
								iBodyLength);

	if(!::HP_HttpServer_IsKeepAlive(pSender, dwConnID))
		::HP_HttpServer_Release(pSender, dwConnID);

	return HPR_OK;
}

EnHttpParseResult CServerDlg::OnUpgrade(HP_HttpServer pSender, HP_CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
{
	::PostOnUpgrade(dwConnID, enUpgradeType, m_spThis->GetSenderName(pSender));

	if(enUpgradeType == HUT_HTTP_TUNNEL)
	{
		::HP_HttpServer_SendResponse(pSender, dwConnID, HSC_OK, "Connection Established", nullptr, 0, nullptr, 0);
	}
	else if(enUpgradeType == HUT_WEB_SOCKET)
	{
		THeader header[] = {{"Connection", UPGRADE_HEADER},
							{UPGRADE_HEADER, WEB_SOCKET_HEADER_VALUE},
							{nullptr, nullptr},
							{nullptr, nullptr}};

		LPCSTR lpszAccept = nullptr;
		
		if(!::HP_HttpServer_GetHeader(pSender, dwConnID, "Sec-WebSocket-Key", &lpszAccept))
			return HPR_ERROR;

		CStringA strAccept;
		::MakeSecWebSocketAccept(lpszAccept, strAccept);

		header[2].name	= "Sec-WebSocket-Accept";
		header[2].value	= strAccept;

		LPCSTR lpszProtocol = nullptr;

		if(::HP_HttpServer_GetHeader(pSender, dwConnID, "Sec-WebSocket-Protocol", &lpszProtocol))
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

		::HP_HttpServer_SendResponse(pSender, dwConnID, HSC_SWITCHING_PROTOCOLS, nullptr, header, iHeaderCount, nullptr, 0);
		::HP_Server_SetConnectionExtra(pSender, dwConnID, new CBufferPtr);
	}
	else
		ASSERT(FALSE);

	return HPR_OK;
}

EnHttpParseResult CServerDlg::OnParseError(HP_HttpServer pSender, HP_CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
{
	::PostOnParseError(dwConnID, iErrorCode, lpszErrorDesc, m_spThis->GetSenderName(pSender));
	return HPR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

En_HP_HandleResult CServerDlg::OnWSMessageHeader(HP_HttpServer pSender, HP_CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
{
	::PostOnWSMessageHeader(dwConnID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen, m_spThis->GetSenderName(pSender));

	return HR_OK;
}

En_HP_HandleResult CServerDlg::OnWSMessageBody(HP_HttpServer pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnWSMessageBody(dwConnID, pData, iLength, m_spThis->GetSenderName(pSender));

	CBufferPtr* pBuffer = nullptr;
	::HP_Server_GetConnectionExtra(pSender, dwConnID, (PVOID*)&pBuffer);
	VERIFY(pBuffer);

	pBuffer->Cat(pData, iLength);

	return HR_OK;
}

En_HP_HandleResult CServerDlg::OnWSMessageComplete(HP_HttpServer pSender, HP_CONNID dwConnID)
{
	::PostOnWSMessageComplete(dwConnID, m_spThis->GetSenderName(pSender));

	CBufferPtr* pBuffer = nullptr;
	::HP_Server_GetConnectionExtra(pSender, dwConnID, (PVOID*)&pBuffer);
	VERIFY(pBuffer);

	BOOL bFinal;
	BYTE iReserved, iOperationCode;

	VERIFY(::HP_HttpServer_GetWSMessageState(pSender, dwConnID, &bFinal, &iReserved, &iOperationCode, nullptr, nullptr, nullptr));

	::HP_HttpServer_SendWSMessage(pSender, dwConnID, bFinal, iReserved, iOperationCode, nullptr, pBuffer->Ptr(), (int)pBuffer->Size(), 0);
	pBuffer->Free();

	if(iOperationCode == 0x8)
		::HP_Server_Disconnect(pSender, dwConnID, TRUE);

	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

CStringA CServerDlg::GetHeaderSummary(HP_HttpServer pSender, HP_CONNID dwConnID, LPCSTR lpszSep, int iSepCount, BOOL bWithContentLength)
{
	CStringA SEP1;

	for(int i = 0; i < iSepCount; i++)
		SEP1 += lpszSep;

	CStringA SEP2(SEP1);
	SEP2 += lpszSep;

	CStringA strResult;

	USHORT usUrlFieldSet = ::HP_HttpServer_GetUrlFieldSet(pSender, dwConnID);

	strResult.AppendFormat("%s[URL Fields]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "SCHEMA", ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_SCHEMA), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "HOST", ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_HOST), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "PORT", ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_PORT), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "PATH", ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_PATH), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "QUERY", ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_QUERY), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "FRAGMENT", ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_FRAGMENT), CRLF);
	strResult.AppendFormat("%s%8s: %s%s", SEP2, "USERINFO", ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_USERINFO), CRLF);	

	DWORD dwHeaderCount = 0;
	::HP_HttpServer_GetAllHeaders(pSender, dwConnID, nullptr, &dwHeaderCount);

	strResult.AppendFormat("%s[Request Headers]%s", SEP1, CRLF);

	if(dwHeaderCount == 0)
		strResult.AppendFormat("%s(no header)%s", SEP2, CRLF);
	else
	{
		unique_ptr<THeader[]> headers(new THeader[dwHeaderCount]);
		VERIFY(::HP_HttpServer_GetAllHeaders(pSender, dwConnID, headers.get(), &dwHeaderCount));

		for(DWORD i = 0; i < dwHeaderCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, headers[i].name, headers[i].value, CRLF);
	}

	DWORD dwCookieCount = 0;
	::HP_HttpServer_GetAllCookies(pSender, dwConnID, nullptr, &dwCookieCount);

	strResult.AppendFormat("%s[Cookies]%s", SEP1, CRLF);

	if(dwCookieCount == 0)
		strResult.AppendFormat("%s(no cookie)%s", SEP2, CRLF);
	else
	{
		unique_ptr<TCookie[]> cookies(new TCookie[dwCookieCount]);
		VERIFY(::HP_HttpServer_GetAllCookies(pSender, dwConnID, cookies.get(), &dwCookieCount));

		for(DWORD i = 0; i < dwCookieCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, cookies[i].name, cookies[i].value, CRLF);
	}

	CStringA strVersion;
	::HttpVersionToString((EnHttpVersion)::HP_HttpServer_GetVersion(pSender, dwConnID), strVersion);
	EnHttpUpgradeType enUpgType	= ::HP_HttpServer_GetUpgradeType(pSender, dwConnID);
	LPCSTR lpszUpgrade			= enUpgType != HUT_NONE ? "true" : "false";
	LPCSTR lpszKeepAlive		= ::HP_HttpServer_IsKeepAlive(pSender, dwConnID) ? "true" : "false";

	strResult.AppendFormat("%s[Basic Info]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "Version", strVersion, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "Method", ::HP_HttpServer_GetMethod(pSender, dwConnID), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsUpgrade", lpszUpgrade, CRLF);
	if(enUpgType != HUT_NONE)
		strResult.AppendFormat("%s%13s: %d%s", SEP2, "UpgradeType", enUpgType, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsKeepAlive", lpszKeepAlive, CRLF);
	if(bWithContentLength)
		strResult.AppendFormat("%s%13s: %lld%s", SEP2, "ContentLength", ::HP_HttpServer_GetContentLength(pSender, dwConnID), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "ContentType", ::HP_HttpServer_GetContentType(pSender, dwConnID), CRLF);
 
	return strResult;
}

LPCTSTR CServerDlg::GetSenderName(HP_HttpServer pSender)
{
	return pSender == m_HttpServer ? HTTP_NAME : (pSender == m_HttpsServer ? HTTPS_NAME : nullptr);
}