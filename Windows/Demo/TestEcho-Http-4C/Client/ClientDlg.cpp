
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

CClientDlg* CClientDlg::m_spThis	= nullptr;

CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_spThis = this;

	// 加载 Cookie
	VERIFY(::HP_HttpCookie_MGR_LoadFromFile(CT2A(g_lpszDefaultCookieFile), FALSE) || ::GetLastError() == ERROR_FILE_NOT_FOUND);

	// 创建监听器对象
	m_HttpClientListener = ::Create_HP_HttpClientListener();
	m_HttpClient		 = nullptr;

	// 设置 HTTP 监听器回调函数
	::HP_Set_FN_HttpClient_OnConnect(m_HttpClientListener, OnConnect);
	::HP_Set_FN_HttpClient_OnHandShake(m_HttpClientListener, OnHandShake);
	::HP_Set_FN_HttpClient_OnSend(m_HttpClientListener, OnSend);
	::HP_Set_FN_HttpClient_OnReceive(m_HttpClientListener, OnReceive);
	::HP_Set_FN_HttpClient_OnClose(m_HttpClientListener, OnClose);

	::HP_Set_FN_HttpClient_OnMessageBegin(m_HttpClientListener, OnMessageBegin);
	::HP_Set_FN_HttpClient_OnStatusLine(m_HttpClientListener, OnStatusLine);
	::HP_Set_FN_HttpClient_OnHeader(m_HttpClientListener, OnHeader);
	::HP_Set_FN_HttpClient_OnHeadersComplete(m_HttpClientListener, OnHeadersComplete);
	::HP_Set_FN_HttpClient_OnBody(m_HttpClientListener, OnBody);
	::HP_Set_FN_HttpClient_OnChunkHeader(m_HttpClientListener, OnChunkHeader);
	::HP_Set_FN_HttpClient_OnChunkComplete(m_HttpClientListener, OnChunkComplete);
	::HP_Set_FN_HttpClient_OnMessageComplete(m_HttpClientListener, OnMessageComplete);
	::HP_Set_FN_HttpClient_OnUpgrade(m_HttpClientListener, OnUpgrade);
	::HP_Set_FN_HttpClient_OnParseError(m_HttpClientListener, OnParseError);

	::HP_Set_FN_HttpClient_OnWSMessageHeader(m_HttpClientListener, OnWSMessageHeader);
	::HP_Set_FN_HttpClient_OnWSMessageBody(m_HttpClientListener, OnWSMessageBody);
	::HP_Set_FN_HttpClient_OnWSMessageComplete(m_HttpClientListener, OnWSMessageComplete);
}

CClientDlg::~CClientDlg()
{
	// 销毁 HTTP 对象
	if(m_HttpClient != nullptr)
		::Destroy_HP_HttpClient(m_HttpClient);

	// 销毁监听器对象
	::Destroy_HP_HttpClientListener(m_HttpClientListener);

	// 保存 Coookie
	::HP_HttpCookie_MGR_SaveToFile(CT2A(g_lpszDefaultCookieFile), TRUE);
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

	if(HP_HttpClient_SendRequest(m_HttpClient, T2A(strMethod), strPathA, szHeaders.get(), dwSize, (const BYTE*)(LPCSTR)strBodyA, strBodyA.GetLength()))
		::LogSend(::HP_Client_GetConnectionID(m_HttpClient), strContent);
	else
		::LogSendFail(::HP_Client_GetConnectionID(m_HttpClient), ::SYS_GetLastError(), ::HP_GetSocketErrorDesc(SE_DATA_SEND));
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
		::HP_HttpClient_SendWSMessage(m_HttpClient, TRUE, 0, 0x1, MASK_KEY, (BYTE*)(LPCSTR)strBodyA, strBodyA.GetLength(), 0);
	else
	{
		::HP_HttpClient_SendWSMessage(m_HttpClient, TRUE, 0, 0x8, MASK_KEY, nullptr, 0, 0);
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

	if(m_HttpClient != nullptr)
		::Destroy_HP_HttpClient(m_HttpClient);

	if(isHttp)
		m_HttpClient = ::Create_HP_HttpClient(m_HttpClientListener);
	else
	{
		m_HttpClient = ::Create_HP_HttpsClient(m_HttpClientListener);
		VERIFY(::HP_SSLClient_SetupSSLContext(m_HttpClient, SSL_VM_NONE, g_c_lpszPemCertFile, g_c_lpszPemKeyFile, g_c_lpszKeyPasswod, g_c_lpszCAPemCertFileOrPath));
	}

	::HP_HttpClient_SetUseCookie(m_HttpClient, m_bUseCookie);

	::LogClientStarting(strAddress, usPort);

	if(::HP_Client_Start(m_HttpClient, strAddress, usPort, m_bAsyncConn))
	{

	}
	else
	{
		::LogClientStartFail(::HP_Client_GetLastError(m_HttpClient), ::HP_Client_GetLastErrorDesc(m_HttpClient));
		SetAppState(ST_STOPPED);
	}
}

void CClientDlg::OnBnClickedStop()
{
	SetAppState(ST_STOPPING);

	if(::HP_Client_Stop(m_HttpClient))
		::LogClientStopping(::HP_Client_GetConnectionID(m_HttpClient));
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

EnHandleResult CClientDlg::OnConnect(HP_Client pSender, CONNID dwConnID)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	::HP_Client_GetLocalAddress(pSender, szAddress, &iAddressLen, &usPort);
	::PostOnConnect(dwConnID, szAddress, usPort);

	return HR_OK;
}

EnHandleResult CClientDlg::OnHandShake(HP_Client pSender, CONNID dwConnID)
{
	::PostOnHandShake(dwConnID);

	m_spThis->SetAppState(ST_STARTED);
	return HR_OK;
}

EnHandleResult CClientDlg::OnSend(HP_Client pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength);
	return HR_OK;
}

EnHandleResult CClientDlg::OnReceive(HP_Client pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnReceive(dwConnID, pData, iLength);
	return HR_OK;
}

EnHandleResult CClientDlg::OnClose(HP_Client pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID)		:
	::PostOnError(dwConnID, enOperation, iErrorCode)	;

	m_spThis->m_bWebSocket = FALSE;
	m_spThis->SetAppState(ST_STOPPED);
	
	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHttpParseResult CClientDlg::OnMessageBegin(HP_HttpClient pSender, CONNID dwConnID)
{
	::PostOnMessageBegin(dwConnID);
	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnStatusLine(HP_HttpClient pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)
{
	::PostOnStatusLine(dwConnID, usStatusCode, lpszDesc);
	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnHeader(HP_HttpClient pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
{
	::PostOnHeader(dwConnID, lpszName, lpszValue);
	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnHeadersComplete(HP_HttpClient pSender, CONNID dwConnID)
{
	CStringA strSummary = GetHeaderSummary(pSender, "    ", 0, TRUE);
	::PostOnHeadersComplete(dwConnID, strSummary);

	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnBody(HP_HttpClient pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnBody(dwConnID, pData, iLength);
	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnChunkHeader(HP_HttpClient pSender, CONNID dwConnID, int iLength)
{
	::PostOnChunkHeader(dwConnID, iLength);
	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnChunkComplete(HP_HttpClient pSender, CONNID dwConnID)
{
	::PostOnChunkComplete(dwConnID);
	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnMessageComplete(HP_HttpClient pSender, CONNID dwConnID)
{
	::PostOnMessageComplete(dwConnID);

	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnUpgrade(HP_HttpClient pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
{
	::PostOnUpgrade(dwConnID, enUpgradeType);

	if(enUpgradeType == HUT_WEB_SOCKET)
	{
		m_spThis->m_bWebSocket = TRUE;
		m_spThis->OnCbnSelchangeMethod();
	}

	return HPR_OK;
}

EnHttpParseResult CClientDlg::OnParseError(HP_HttpClient pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
{
	::PostOnParseError(dwConnID, iErrorCode, lpszErrorDesc);
	return HPR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

EnHandleResult CClientDlg::OnWSMessageHeader(HP_HttpClient pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
{
	::PostOnWSMessageHeader(dwConnID, bFinal, iReserved, iOperationCode, lpszMask, ullBodyLen);

	return HR_OK;
}

EnHandleResult CClientDlg::OnWSMessageBody(HP_HttpClient pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnWSMessageBody(dwConnID, pData, iLength);

	return HR_OK;
}

EnHandleResult CClientDlg::OnWSMessageComplete(HP_HttpClient pSender, CONNID dwConnID)
{
	::PostOnWSMessageComplete(dwConnID);

	BYTE iOperationCode;

	VERIFY(::HP_HttpClient_GetWSMessageState(pSender, nullptr, nullptr, &iOperationCode, nullptr, nullptr, nullptr));

	if(iOperationCode == 0x8)
		return HR_ERROR;

	return HR_OK;
}

// ------------------------------------------------------------------------------------------------------------- //

CStringA CClientDlg::GetHeaderSummary(HP_HttpClient pSender, LPCSTR lpszSep, int iSepCount, BOOL bWithContentLength)
{
	CStringA SEP1;

	for(int i = 0; i < iSepCount; i++)
		SEP1 += lpszSep;

	CStringA SEP2(SEP1);
	SEP2 += lpszSep;

	CStringA strResult;

	strResult.AppendFormat("%s[Status Fields]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%13s: %u%s", SEP2, "Status Code", ::HP_HttpClient_GetStatusCode(pSender), CRLF);

	DWORD dwHeaderCount = 0;
	::HP_HttpClient_GetAllHeaders(pSender, nullptr, &dwHeaderCount);

	strResult.AppendFormat("%s[Response Headers]%s", SEP1, CRLF);

	if(dwHeaderCount == 0)
		strResult.AppendFormat("%s(no header)%s", SEP2, CRLF);
	else
	{
		unique_ptr<THeader[]> headers(new THeader[dwHeaderCount]);
		VERIFY(::HP_HttpClient_GetAllHeaders(pSender, headers.get(), &dwHeaderCount));

		for(DWORD i = 0; i < dwHeaderCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, headers[i].name, headers[i].value, CRLF);
	}

	DWORD dwCookieCount = 0;
	::HP_HttpClient_GetAllCookies(pSender, nullptr, &dwCookieCount);

	strResult.AppendFormat("%s[Cookies]%s", SEP1, CRLF);

	if(dwCookieCount == 0)
		strResult.AppendFormat("%s(no cookie)%s", SEP2, CRLF);
	else
	{
		unique_ptr<TCookie[]> cookies(new TCookie[dwCookieCount]);
		VERIFY(::HP_HttpClient_GetAllCookies(pSender, cookies.get(), &dwCookieCount));

		for(DWORD i = 0; i < dwCookieCount; i++)
			strResult.AppendFormat("%s%s: %s%s", SEP2, cookies[i].name, cookies[i].value, CRLF);
	}

	CStringA strVersion;
	::HttpVersionToString((EnHttpVersion)::HP_HttpClient_GetVersion(pSender), strVersion);
	EnHttpUpgradeType enUpgType	= ::HP_HttpClient_GetUpgradeType(pSender);
	LPCSTR lpszUpgrade			= enUpgType != HUT_NONE ? "true" : "false";
	LPCSTR lpszKeepAlive		= ::HP_HttpClient_IsKeepAlive(pSender) ? "true" : "false";

	strResult.AppendFormat("%s[Basic Info]%s", SEP1, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "Version", strVersion, CRLF);
	strResult.AppendFormat("%s%13s: %u%s", SEP2, "Status Code", ::HP_HttpClient_GetStatusCode(pSender), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsUpgrade", lpszUpgrade, CRLF);
	if(enUpgType != HUT_NONE)
		strResult.AppendFormat("%s%13s: %d%s", SEP2, "UpgradeType", enUpgType, CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "IsKeepAlive", lpszKeepAlive, CRLF);
	if(bWithContentLength)
		strResult.AppendFormat("%s%13s: %lld%s", SEP2, "ContentLength", ::HP_HttpClient_GetContentLength(pSender), CRLF);
	strResult.AppendFormat("%s%13s: %s%s", SEP2, "ContentType", ::HP_HttpClient_GetContentType(pSender), CRLF);
 
	return strResult;
}
