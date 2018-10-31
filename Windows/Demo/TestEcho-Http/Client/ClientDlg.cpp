
// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"


// CClientDlg dialog

#define DEFAULT_PATH	_T("req/path?p1=v1&p2=v2")
#define DEFAULT_ADDRESS	_T("127.0.0.1")
#define DEFAULT_PORT	_T("8080")

CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	VERIFY(g_CookieMgr.LoadFromFile(CT2A(g_lpszDefaultCookieFile), FALSE) || ::GetLastError() == ERROR_FILE_NOT_FOUND);
}

CClientDlg::~CClientDlg()
{
	g_CookieMgr.SaveToFile(CT2A(g_lpszDefaultCookieFile), TRUE);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SEND, m_Send);
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_ADDRESS, m_Address);
	DDX_Control(pDX, IDC_PORT, m_Port);
	DDX_Control(pDX, IDC_USE_COOKIE, m_UseCookie);
	DDX_Control(pDX, IDC_ASYNC, m_Async);
	DDX_Control(pDX, IDC_START, m_Start);
	DDX_Control(pDX, IDC_STOP, m_Stop);
	DDX_Control(pDX, IDC_METHOD, m_Method);
	DDX_Control(pDX, IDC_SCHEMA, m_Schema);
	DDX_Control(pDX, IDC_PATH, m_Path);
	DDX_Control(pDX, IDC_HEADER_NAME, m_HeaderName);
	DDX_Control(pDX, IDC_HEADER_VALUE, m_HeaderValue);
	DDX_Control(pDX, IDC_HEADER_ADD, m_HeaderAdd);
	DDX_Control(pDX, IDC_HEADERS, m_Headers);
	DDX_Control(pDX, IDC_BODY, m_Body);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, &CClientDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_START, &CClientDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CClientDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_HEADER_ADD, &CClientDlg::OnBnClickedHeaderAdd)
	ON_CBN_SELCHANGE(IDC_METHOD, &CClientDlg::OnCbnSelchangeMethod)
	ON_MESSAGE(USER_INFO_MSG, OnUserInfoMsg)
	ON_WM_VKEYTOITEM()
END_MESSAGE_MAP()


// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_Method.SetCurSel(3);
	m_Schema.SetCurSel(0);

	m_Path.SetWindowText(DEFAULT_PATH);
	m_Address.SetWindowText(DEFAULT_ADDRESS);
	m_Port.SetWindowText(DEFAULT_PORT);
	m_UseCookie.SetCheck(BST_CHECKED);
	m_Async.SetCheck(BST_CHECKED);

	::SetMainWnd(this);
	::SetInfoList(&m_Info);
	SetAppState(ST_STOPPED);

	m_bWebSocket = FALSE;
	m_bUseCookie = FALSE;
	m_bAsyncConn = FALSE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientDlg::OnPaint()
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
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CClientDlg::PreTranslateMessage(MSG* pMsg)
{
	if (
			pMsg->message == WM_KEYDOWN		
			&&(	pMsg->wParam == VK_ESCAPE	 
			||	pMsg->wParam == VK_CANCEL	
			||	(pMsg->wParam == VK_RETURN && pMsg->hwnd == this->GetSafeHwnd())
		))
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CClientDlg::SetAppState(EnAppState state)
{
	m_enState = state;

	if(this->GetSafeHwnd() == nullptr)
		return;

	m_UseCookie.EnableWindow(m_enState == ST_STOPPED);
	m_Async.EnableWindow(m_enState == ST_STOPPED);
	m_Start.EnableWindow(m_enState == ST_STOPPED);
	m_Stop.EnableWindow(m_enState == ST_STARTED);
	m_Send.EnableWindow(m_enState == ST_STARTED);
	m_Schema.EnableWindow(m_enState == ST_STOPPED);
	m_Address.EnableWindow(m_enState == ST_STOPPED);
	m_Port.EnableWindow(m_enState == ST_STOPPED);
	m_Body.EnableWindow(m_Method.GetCurSel() <= 2 || m_bWebSocket);
	m_Path.EnableWindow(m_Method.GetCurSel() != 8);
}

void CClientDlg::OnBnClickedHeaderAdd()
{
	CString strName;
	CString strValue;

	m_HeaderName.GetWindowText(strName);
	m_HeaderValue.GetWindowText(strValue);

	strName.Trim();
	strValue.Trim();

	if(!strName.IsEmpty())
	{
		if(strName == _T("Content-Length"))
		{
			MessageBox(_T("'Content-Length' header can't be specified!"), _T("Add Header"), MB_OK | MB_ICONINFORMATION);
			m_HeaderName.SetFocus();

			return;
		}

		CString strHeader;
		strHeader.Format(_T("%s: %s"), strName, strValue);
		m_Headers.AddString(strHeader);

		m_HeaderName.SetWindowText(_T(""));
		m_HeaderValue.SetWindowText(_T(""));
	}
}

void CClientDlg::OnBnClickedSend()
{
	m_bWebSocket ? SendWebSocket() : SendHttp();
}

void CClientDlg::SendHttp()
{
	USES_CONVERSION;

	CString strMethod;
	CString strSchema;
	CString strAddress;
	CString strPort;
	CString strPath;

	m_Method.GetWindowText(strMethod);
	m_Schema.GetWindowText(strSchema);
	m_Address.GetWindowText(strAddress);
	m_Port.GetWindowText(strPort);

	if(m_Method.GetCurSel() != 8)
	{
		m_Path.GetWindowText(strPath);
		strPath.Trim();

		if(strPath.IsEmpty() || strPath.GetAt(0) != '/')
			strPath.Insert(0, '/');
	}

	THttpHeaderMap headers;

	int iCount = m_Headers.GetCount();

	for(int i = 0; i < iCount; i++)
	{
		CString strHeader;
		m_Headers.GetText(i, strHeader);

		int j = 0;
		CString strName  = strHeader.Tokenize(_T(": "), j);
		CString strValue = strHeader.Mid(j + 1);

		headers.emplace(THttpHeaderMap::value_type(T2A(strName), T2A(strValue)));
	}

	CStringA strBodyA;
	CStringA strPathA;

	if(m_Method.GetCurSel() <= 2)
	{
		CString strBody;
		m_Body.GetWindowText(strBody);

		strBodyA = T2A(strBody);
	}
	else if(m_Method.GetCurSel() == 8)
	{
		THttpHeaderMapCI it = headers.find("Host");

		if(it != headers.end() && !it->second.IsEmpty())
			strPathA = it->second;
		else
		{
			CString strHost;
			strHost.Format(_T("%s:%s"), strAddress, strPort);
			strPathA = strHost;
		}
	}

	if(strPathA.IsEmpty())
		strPathA = T2A(strPath);

	DWORD dwIndex	= 0;
	DWORD dwSize	= (DWORD)headers.size();
	unique_ptr<THeader[]> szHeaders(new THeader[dwSize]);

	for(THttpHeaderMapCI it = headers.begin(), end = headers.end(); it != end; ++it, ++dwIndex)
	{
		szHeaders[dwIndex].name  = it->first;
		szHeaders[dwIndex].value = it->second;
	}

	CString strContent;
	strContent.Format(_T("[%s] %s://%s:%s%s"), strMethod, strSchema, strAddress, strPort, strPath);

	if(m_pClient->SendRequest(T2A(strMethod), strPathA, szHeaders.get(), dwSize, (const BYTE*)(LPCSTR)strBodyA, strBodyA.GetLength()))
		::LogSend(m_pClient->GetConnectionID(), strContent);
	else
		::LogSendFail(m_pClient->GetConnectionID(), ::GetLastError(), ::GetSocketErrorDesc(SE_DATA_SEND));
}

void CClientDlg::SendWebSocket()
{
	static LPCSTR CLOSE_FLAG	 = "$close";
	static const BYTE MASK_KEY[] = {0x1, 0x2, 0x3, 0x4};

	USES_CONVERSION;

	CString strBody;
	m_Body.GetWindowText(strBody);

	CStringA strBodyA = T2A(strBody);

	if(strBodyA.CompareNoCase(CLOSE_FLAG) != 0)
		m_pClient->SendWSMessage(TRUE, 0, 0x1, MASK_KEY, (BYTE*)(LPCSTR)strBodyA, strBodyA.GetLength());
	else
	{
		m_pClient->SendWSMessage(TRUE, 0, 0x8, MASK_KEY, nullptr, 0);
		m_Body.SetWindowText(_T(""));
	}
}

void CClientDlg::OnBnClickedStart()
{
	SetAppState(ST_STARTING);

	CString strAddress;
	CString strPort;

	m_Address.GetWindowText(strAddress);
	m_Port.GetWindowText(strPort);

	USHORT usPort	= (USHORT)_ttoi(strPort);
	BOOL isHttp		= m_Schema.GetCurSel() == 0;

	m_bUseCookie	= m_UseCookie.GetCheck() == BST_CHECKED;
	m_bAsyncConn	= m_Async.GetCheck() == BST_CHECKED;

	if(isHttp)
		m_pClient.reset((IHttpClient*)(new CHttpClient(this)));
	else
	{
		CHttpsClient* pClient = new CHttpsClient(this);
		VERIFY(pClient->SetupSSLContext(SSL_VM_NONE, g_c_lpszPemCertFile, g_c_lpszPemKeyFile, g_c_lpszKeyPasswod, g_c_lpszCAPemCertFileOrPath));
		m_pClient.reset((IHttpClient*)pClient);
	}

	m_pClient->SetUseCookie(m_bUseCookie);

	::LogClientStarting(strAddress, usPort);

	if(m_pClient->Start(strAddress, usPort, m_bAsyncConn))
	{

	}
	else
	{
		::LogClientStartFail(m_pClient->GetLastError(), m_pClient->GetLastErrorDesc());
		SetAppState(ST_STOPPED);
	}
}

void CClientDlg::OnBnClickedStop()
{
	SetAppState(ST_STOPPING);

	if(m_pClient->Stop())
		::LogClientStopping(m_pClient->GetConnectionID());
	else
		ASSERT(FALSE);
}

void CClientDlg::OnCbnSelchangeMethod()
{
	m_Body.EnableWindow(m_Method.GetCurSel() <= 2 || m_bWebSocket);
	m_Path.EnableWindow(m_Method.GetCurSel() != 8);
}

int CClientDlg::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex)
{
	if(nKey == 'C')
		pListBox->ResetContent();
	else if(nKey == 'D' && pListBox == &m_Headers)
	{
		int index = pListBox->GetCurSel();
		if(index != LB_ERR)
		{
			pListBox->DeleteString(index);

			int count = pListBox->GetCount();
			if(index < count)
				pListBox->SetCurSel(index);
			else if(index > 0)
				pListBox->SetCurSel(index - 1);
		}
	}

	return __super::OnVKeyToItem(nKey, pListBox, nIndex);
}

LRESULT CClientDlg::OnUserInfoMsg(WPARAM wp, LPARAM lp)
{
	info_msg* msg = (info_msg*)wp;

	::LogInfoMsg(msg);

	return 0;
}

EnHandleResult CClientDlg::OnConnect(ITcpClient* pSender, CONNID dwConnID)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	pSender->GetLocalAddress(szAddress, iAddressLen, usPort);
	::PostOnConnect(dwConnID, szAddress, usPort);

	return HR_OK;
}

EnHandleResult CClientDlg::OnHandShake(ITcpClient* pSender, CONNID dwConnID)
{
	::PostOnHandShake(dwConnID);

	SetAppState(ST_STARTED);
	return HR_OK;
}

EnHandleResult CClientDlg::OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength);
	return HR_OK;
}

EnHandleResult CClientDlg::OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnReceive(dwConnID, pData, iLength);
	return HR_OK;
}

EnHandleResult CClientDlg::OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID)		:
	::PostOnError(dwConnID, enOperation, iErrorCode)	;

	m_bWebSocket = FALSE;
	SetAppState(ST_STOPPED);

	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHttpParseResult CClientDlg::OnMessageBegin(IHttpClient* pSender, CONNID dwConnID)
{
	::PostOnMessageBegin(dwConnID);
	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnStatusLine(IHttpClient* pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)
{
	::PostOnStatusLine(dwConnID, usStatusCode, lpszDesc);
	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnHeader(IHttpClient* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
{
	::PostOnHeader(dwConnID, lpszName, lpszValue);
	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnHeadersComplete(IHttpClient* pSender, CONNID dwConnID)
{
	CStringA strSummary = GetHeaderSummary(pSender, "    ", 0, TRUE);
	::PostOnHeadersComplete(dwConnID, strSummary);

	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnBody(IHttpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnBody(dwConnID, pData, iLength);
	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnChunkHeader(IHttpClient* pSender, CONNID dwConnID, int iLength)
{
	::PostOnChunkHeader(dwConnID, iLength);
	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnChunkComplete(IHttpClient* pSender, CONNID dwConnID)
{
	::PostOnChunkComplete(dwConnID);
	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnMessageComplete(IHttpClient* pSender, CONNID dwConnID)
{
	::PostOnMessageComplete(dwConnID);

	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnUpgrade(IHttpClient* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
{
	::PostOnUpgrade(dwConnID, enUpgradeType);

	if(enUpgradeType == HUT_WEB_SOCKET)
	{
		m_bWebSocket = TRUE;
		OnCbnSelchangeMethod();
	}

	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnParseError(IHttpClient* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
{
	::PostOnParseError(dwConnID, iErrorCode, lpszErrorDesc);
	return HPR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHandleResult CClientDlg::OnWSMessageHeader(IHttpClient* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
{
	::PostOnWSMessageHeader(dwConnID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen);

	return HR_OK;
}

EnHandleResult CClientDlg::OnWSMessageBody(IHttpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnWSMessageBody(dwConnID, pData, iLength);

	return HR_OK;
}

EnHandleResult CClientDlg::OnWSMessageComplete(IHttpClient* pSender, CONNID dwConnID)
{
	::PostOnWSMessageComplete(dwConnID);

	BYTE iOperationCode;

	VERIFY(pSender->GetWSMessageState(nullptr, nullptr, &iOperationCode, nullptr, nullptr, nullptr));

	if(iOperationCode == 0x8)
		return HR_ERROR;

	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

CStringA CClientDlg::GetHeaderSummary(IHttpClient* pHttpClient, LPCSTR lpszSep, int iSepCount, BOOL bWithContentLength)
{
	CStringA SEP1;

	for(int i = 0; i < iSepCount; i++)
		SEP1 += lpszSep;

	CStringA SEP2(SEP1);
	SEP2 += lpszSep;

	CStringA strResult;

	strResult.AppendFormat("%s[Status Fields]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%13s: %u%s", SEP2, "Status Code", pHttpClient->GetStatusCode(), CRLF);

	DWORD dwHeaderCount = 0;
	pHttpClient->GetAllHeaders(nullptr, dwHeaderCount);

	strResult.AppendFormat("%s[Response Headers]%s", SEP1, CRLF);

	if(dwHeaderCount == 0)
		strResult.AppendFormat("%s(no header)%s", SEP2, CRLF);
	else
	{
		unique_ptr<THeader[]> headers(new THeader[dwHeaderCount]);
		VERIFY(pHttpClient->GetAllHeaders(headers.get(), dwHeaderCount));

		for(DWORD i = 0; i < dwHeaderCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, headers[i].name, headers[i].value, CRLF);
	}

	DWORD dwCookieCount = 0;
	pHttpClient->GetAllCookies(nullptr, dwCookieCount);

	strResult.AppendFormat("%s[Cookies]%s", SEP1, CRLF);

	if(dwCookieCount == 0)
		strResult.AppendFormat("%s(no cookie)%s", SEP2, CRLF);
	else
	{
		unique_ptr<TCookie[]> cookies(new TCookie[dwCookieCount]);
		VERIFY(pHttpClient->GetAllCookies(cookies.get(), dwCookieCount));

		for(DWORD i = 0; i < dwCookieCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, cookies[i].name, cookies[i].value, CRLF);
	}

	CStringA strVersion;
	::HttpVersionToString((EnHttpVersion)pHttpClient->GetVersion(), strVersion);
	EnHttpUpgradeType enUpgType	= pHttpClient->GetUpgradeType();
	LPCSTR lpszUpgrade			= enUpgType != HUT_NONE ? "true" : "false";
	LPCSTR lpszKeepAlive		= pHttpClient->IsKeepAlive() ? "true" : "false";

	strResult.AppendFormat("%s[Basic Info]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "Version", strVersion, CRLF);
	strResult.AppendFormat("%s%13s: %u%s", SEP2, "Status Code", pHttpClient->GetStatusCode(), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsUpgrade", lpszUpgrade, CRLF);
	if(enUpgType != HUT_NONE)
		strResult.AppendFormat("%s%13s: %d%s", SEP2, "UpgradeType", enUpgType, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsKeepAlive", lpszKeepAlive, CRLF);
	if(bWithContentLength)
		strResult.AppendFormat("%s%13s: %lld%s", SEP2, "ContentLength", pHttpClient->GetContentLength(), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "ContentType", pHttpClient->GetContentType(), CRLF);
 
	return strResult;
}
