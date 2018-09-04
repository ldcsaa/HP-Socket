
// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"

#ifdef _WIN64
	#ifdef _DEBUG
		#pragma comment(lib, "../../../Bin/HPSocket/x64/HPSocket_UD.lib")
	#else
		#pragma comment(lib, "../../../Bin/HPSocket/x64/HPSocket_U.lib")
	#endif
#else
	#ifdef _DEBUG
		#pragma comment(lib, "../../../Bin/HPSocket/x86/HPSocket_UD.lib")
	#else
		#pragma comment(lib, "../../../Bin/HPSocket/x86/HPSocket_U.lib")
	#endif
#endif

// CServerDlg dialog

#define AGENT_NAME		_T("*A*")
#define SERVER_NAME		_T("*S*")

#define DEFAULT_ADDRESS	_T("0.0.0.0")
#define DEFAULT_PORT	_T("5555")

#ifdef _DEBUG
	#define DEF_SHOW_LOG	BST_CHECKED
#else
	#define DEF_SHOW_LOG	BST_UNCHECKED
#endif

#define CONNECT_TO_SERVER_WAIT_TIME		10000
#define SILENCE_CONNECTION_CHECK		60000
#define SILENCE_CONNECTION_DURATION		90000

#define DEF_HTTP_PORT	80
#define DEF_HTTPS_PORT	443
#define HEADER_SEP		"\r\n"
#define HTTPS_FLAG		"CONNECT"
#define CHUNK_FLAG		"chunked"
#define HTTP_SCHEME		"http://"
#define HTTPS_SCHEME	"https://"
#define HTTP_1_1		"HTTP/1.1"
#define HEAD_HOST		"Host"
#define HEAD_CONN		"Connection"
#define HEAD_PROXY_CONN	"Proxy-Connection"
#define HEAD_PROXY_AUTH	"Prxoy-Authenticate"
#define HTTPS_RESP		"HTTP/1.1 200 Connection Established\r\n\r\n"
#define IE_OPT_DIALOG	"rundll32.exe shell32.dll, Control_RunDLL inetcpl.cpl, , 4"

static int HEADER_SEP_LEN		= (int)strlen(HEADER_SEP);
static int HTTP_SCHEME_LEN		= (int)strlen(HTTP_SCHEME);
static int HTTPS_SCHEME_LEN		= (int)strlen(HTTPS_SCHEME);
static int HEAD_HOST_LEN		= (int)strlen(HEAD_HOST);
static int HEAD_CONN_LEN		= (int)strlen(HEAD_CONN);
static int HEAD_PROXY_CONN_LEN	= (int)strlen(HEAD_PROXY_CONN);
static int HEAD_PROXY_AUTH_LEN	= (int)strlen(HEAD_PROXY_AUTH);
static int HTTPS_RESP_LEN		= (int)strlen(HTTPS_RESP);

CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CServerDlg::IDD, pParent)
	, m_pServerListener(new CHttpServerListenerImpl(this))
	, m_pAgentListener(new CTcpAgentListenerImpl(this))
	, m_Server(m_pServerListener.get())
	, m_Agent(m_pAgentListener.get())
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_START, m_Start);
	DDX_Control(pDX, IDC_STOP, m_Stop);
	DDX_Control(pDX, IDC_BIND_ADDRESS, m_BindAddress);
	DDX_Control(pDX, IDC_PORT, m_Port);
	DDX_Control(pDX, IDC_OPTIONS, m_Options);
	DDX_Control(pDX, IDC_SHOWLOG, m_ShowLog);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CServerDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CServerDlg::OnBnClickedStop)
	ON_MESSAGE(USER_INFO_MSG, OnUserInfoMsg)
	ON_WM_VKEYTOITEM()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_OPTIONS, &CServerDlg::OnBnClickedOptions)
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

	m_BindAddress.SetWindowText(DEFAULT_ADDRESS);
	m_Port.SetWindowText(DEFAULT_PORT);
	m_ShowLog.SetCheck(DEF_SHOW_LOG);

	::SetMainWnd(this);
	::SetInfoList(&m_Info);
	SetAppState(ST_STOPPED);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CServerDlg::OnClose()
{
	/*
	if(m_Server->GetState() != SS_STOPED)
	{
		this->MessageBox(_T("stop IOCP Server first, pls !"), _T("forbiddden"));
		return;
	}
	*/

	::SetMainWnd(nullptr);
	__super::OnClose();

	if(m_Server->HasStarted())
		m_Server->Stop();

	if(m_Agent->HasStarted())
		m_Agent->Stop();
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
	if(m_enState == state)
		return;

	m_enState = state;

	if(this->GetSafeHwnd() == nullptr)
		return;

	m_Start.EnableWindow(m_enState == ST_STOPPED);
	m_Stop.EnableWindow(m_enState == ST_STARTED);
	m_BindAddress.EnableWindow(m_enState == ST_STOPPED);
	m_Port.EnableWindow(m_enState == ST_STOPPED);
	m_ShowLog.EnableWindow(m_enState == ST_STOPPED);
}

void CServerDlg::OnBnClickedStart()
{
	CString strBindAddress;
	m_BindAddress.GetWindowText(strBindAddress);
	strBindAddress.Trim();

	if(strBindAddress.IsEmpty())
		strBindAddress = DEFAULT_ADDRESS;

	CString strPort;
	m_Port.GetWindowText(strPort);
	USHORT usPort = (USHORT)_ttoi(strPort);

	if(usPort == 0)
	{
		MessageBox(_T("Listen Port invalid, pls check!"), _T("Params Error"), MB_OK);
		m_Port.SetFocus();
		return;
	}

	m_bLog = (m_ShowLog.GetCheck() == BST_CHECKED);

	SetAppState(ST_STARTING);

	//m_Server->SetFreeSocketObjPool(500);
	//m_Server->SetFreeSocketObjHold(1500);
	//m_Server->SetFreeBufferObjPool(2000);
	//m_Server->SetFreeBufferObjHold(6000);
	//m_Server->SetAcceptSocketCount(50);
	//m_Server->SetSendPolicy(SP_DIRECT);
	//m_Agent->SetSendPolicy(SP_DIRECT);

	m_Server->SetWorkerThreadCount(3 * m_Agent->GetWorkerThreadCount());

	if(m_Server->Start(strBindAddress, usPort))
	{
		VERIFY(m_Agent->Start(nullptr, TRUE));

		::LogServerStart(strBindAddress, usPort);
		SetAppState(ST_STARTED);
	}
	else
	{
		::LogServerStartFail(m_Server->GetLastError(), m_Server->GetLastErrorDesc());
		SetAppState(ST_STOPPED);
	}
}

void CServerDlg::OnBnClickedStop()
{
	SetAppState(ST_STOPPING);
	::LogServerStop();

	VERIFY(m_Server->Stop());
	VERIFY(m_Agent->Stop());

	SetAppState(ST_STOPPED);
}

void CServerDlg::OnBnClickedOptions()
{
	::WinExec(IE_OPT_DIALOG, 0);
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

UINT CServerDlg::CleanerThreadProc(PVOID pv)
{
	TRACE("---------------> HTTP Cleaner Thread 0x%08X started <---------------\n", ::GetCurrentThreadId());

	DWORD dwRetValue = 0;

	while(m_Server->HasStarted() || m_Agent->HasStarted())
	{
		dwRetValue = ::WaitForSingleObject(m_evCleaner, SILENCE_CONNECTION_CHECK);

		if(dwRetValue == WAIT_TIMEOUT)
			m_Agent->DisconnectSilenceConnections(SILENCE_CONNECTION_DURATION);
		else
		{
			ASSERT(dwRetValue == WAIT_OBJECT_0);
			break;
		}
	}

	TRACE("---------------> HTTP Cleaner Thread 0x%08X stoped <---------------\n", ::GetCurrentThreadId());

	return 0;
}

void CServerDlg::WaitForCleanerThreadEnd()
{
	if(m_thCleaner.IsRunning())
	{
		m_evCleaner.Set();
		VERIFY(m_thCleaner.Join(TRUE));
		m_evCleaner.Reset();
	}
}

EnHandleResult CHttpServerListenerImpl::OnPrepareListen(ITcpServer* pSender, SOCKET soListen)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	m_pDlg->m_Server->GetListenAddress(szAddress, iAddressLen, usPort);
	::PostOnPrepareListen(szAddress, usPort, SERVER_NAME);

	VERIFY(m_pDlg->m_thCleaner.Start(m_pDlg, &CServerDlg::CleanerThreadProc));

	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnAccept(ITcpServer* pSender, CONNID dwConnID, SOCKET soClient)
{
	//if(m_pDlg->m_bLog) ::PostOnAccept2(dwConnID, SERVER_NAME);

	m_pDlg->m_Server->SetConnectionExtra(dwConnID, THttpContext::Construct(dwConnID));

	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	//if(m_pDlg->m_bLog) ::PostOnSend(dwConnID, pData, iLength, SERVER_NAME);

	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	//if(m_pDlg->m_bLog) ::PostOnReceive(dwConnID, pData, iLength, SERVER_NAME);

	THttpContext* pContext = nullptr;

	m_pDlg->m_Server->GetConnectionExtra(dwConnID, (PVOID*)&pContext);
	ASSERT(pContext);

	m_pDlg->m_Agent->Send(pContext->agentConnID, pData, iLength);

	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	/*
	if(m_pDlg->m_bLog)
	{
		iErrorCode == SE_OK ? ::PostOnClose(dwConnID, SERVER_NAME)	:
		::PostOnError(dwConnID, enOperation, iErrorCode, SERVER_NAME);
	}
	*/

	DetachConnInfo(dwConnID);

	return HR_OK;
}

EnHandleResult CHttpServerListenerImpl::OnShutdown(ITcpServer* pSender)
{
	m_pDlg->WaitForCleanerThreadEnd();

	::PostOnShutdown(SERVER_NAME);

	return HR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnRequestLine(IHttpServer* pSender, CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)
{
	THttpContext* pContext = nullptr;

	m_pDlg->m_Server->GetConnectionExtra(dwConnID, (PVOID*)&pContext);
	ASSERT(pContext);

	pContext->method = lpszMethod;

	if(_stricmp(lpszMethod, HTTPS_FLAG) == 0)
		pContext->https = TRUE;

	if(pContext->https)
		pContext->path.Empty();
	else
	{
		LPCSTR lpszPath = lpszUrl;

		if(lpszUrl[0] != '/')
		{
			int iLen = (int)strlen(lpszUrl);
			lpszPath = strchr(lpszUrl + min(iLen, 8), '/');

			if(!lpszPath) lpszPath = lpszUrl;
		}

		pContext->path = lpszPath;
	}

	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnHeadersComplete(IHttpServer* pSender, CONNID dwConnID)
{
	EnHttpParseResult rs	= HPR_OK;
	THttpContext* pContext	= nullptr;

	m_pDlg->m_Server->GetConnectionExtra(dwConnID, (PVOID*)&pContext);
	ASSERT(pContext);

	if(!CheckSameHost(pContext))
	{
		if(!ConnectToServer(pContext))
			return HPR_ERROR;
	}

	if(pContext->agentConnID != 0)
		DispatchHeaders(pContext);

	return rs;
}

EnHttpParseResult CHttpServerListenerImpl::OnBody(IHttpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	THttpContext* pContext = nullptr;

	m_pDlg->m_Server->GetConnectionExtra(dwConnID, (PVOID*)&pContext);
	ASSERT(pContext);

	m_pDlg->m_Agent->Send(pContext->agentConnID, pData, iLength);

	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnChunkHeader(IHttpServer* pSender, CONNID dwConnID, int iLength)
{
	THttpContext* pContext = nullptr;

	m_pDlg->m_Server->GetConnectionExtra(dwConnID, (PVOID*)&pContext);
	ASSERT(pContext);

	CStringA strData;
	strData.Format("%s%X%s", HEADER_SEP, iLength, HEADER_SEP);

	m_pDlg->m_Agent->Send(pContext->agentConnID, (const BYTE*)(LPCSTR)strData, strData.GetLength());

	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnChunkComplete(IHttpServer* pSender, CONNID dwConnID)
{
	THttpContext* pContext = nullptr;

	m_pDlg->m_Server->GetConnectionExtra(dwConnID, (PVOID*)&pContext);
	ASSERT(pContext);

	m_pDlg->m_Agent->Send(pContext->agentConnID, (const BYTE*)HEADER_SEP, HEADER_SEP_LEN);

	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnMessageComplete(IHttpServer* pSender, CONNID dwConnID)
{
	return HPR_OK;
}

EnHttpParseResult CHttpServerListenerImpl::OnUpgrade(IHttpServer* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
{
	return (enUpgradeType == HUT_HTTP_TUNNEL) ? HPR_OK : HPR_ERROR;
}

EnHttpParseResult CHttpServerListenerImpl::OnParseError(IHttpServer* pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
{
	return HPR_OK;
}

BOOL CHttpServerListenerImpl::CheckSameHost(THttpContext* pContext)
{
	BOOL isOK = FALSE;

	CStringA strHost;
	USHORT usPort = 0;

	if(GetHost(pContext->serverConnID, pContext->https, strHost, usPort))
	{
		if(pContext->port == usPort && pContext->host.CompareNoCase(strHost) == 0)
			isOK = TRUE;
		else
		{
			pContext->host = strHost;
			pContext->port = usPort;
		}
	}

	return isOK;
}

BOOL CHttpServerListenerImpl::GetHost(CONNID dwConnID, BOOL bHttps, CStringA& strHost, USHORT& usPort)
{
	strHost.Empty();
	usPort = 0;

	BOOL isOK		= FALSE;
	LPCSTR lpszHost	= m_pDlg->m_Server->GetHost(dwConnID);

	if(lpszHost)
	{
		int iPos			 = 0;
		CStringA strFullHost = lpszHost;

		for(int k = 0; k < 2; k++)
		{
			CStringA strToken = strFullHost.Tokenize(":", iPos);

			if(strToken.IsEmpty())
				break;

			if(k == 0)
				strHost = strToken;
			else if(k == 1)
				usPort = atoi(strToken);
		}

		isOK = TRUE;
	}
	else
	{
		lpszHost = m_pDlg->m_Server->GetUrlField(dwConnID, HUF_HOST);

		if(lpszHost)
		{
			strHost			= lpszHost;
			LPCSTR lpszPort	= m_pDlg->m_Server->GetUrlField(dwConnID, HUF_PORT);

			if(lpszPort)
				usPort = atoi(lpszPort);

			isOK = TRUE;
		}
	}

	if(isOK && usPort == 0)
		usPort = bHttps ? DEF_HTTPS_PORT : DEF_HTTP_PORT;

	ASSERT(isOK);
	return isOK;
}

BOOL CHttpServerListenerImpl::ConnectToServer(THttpContext* pContext)
{
	if(pContext->host.IsEmpty() || pContext->port == 0)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CONNID dwAgentID		= 0;
	CONNID dwCurrentAgentID = pContext->agentConnID;

	{
		CCriSecLock locallock(pContext->cs);

		if(pContext->closed)
			return FALSE;

		pContext->IncRef();

		if(!m_pDlg->m_Agent->Connect(CA2T(pContext->host), pContext->port, &dwAgentID))
		{
			::PostOnError(dwAgentID, SO_CONNECT, ::SYS_GetLastError(), AGENT_NAME);

			pContext->DecRef();
			return FALSE;
		}

		pContext->agentConnID = dwAgentID;
	}

	VERIFY(m_pDlg->m_Agent->SetConnectionExtra(dwAgentID, pContext));

	if(dwCurrentAgentID != 0)
		m_pDlg->m_Agent->Disconnect(dwCurrentAgentID);

	DWORD dwRetValue = ::WaitForSingleObject(pContext->evt, CONNECT_TO_SERVER_WAIT_TIME);

	if(dwRetValue != WAIT_TIMEOUT)
		ASSERT(dwRetValue == WAIT_OBJECT_0);
	else
	{
		if(pContext->agentConnID != 0)
			m_pDlg->m_Agent->Disconnect(pContext->agentConnID);

		return FALSE;
	}

	return TRUE;
}

void CHttpServerListenerImpl::DispatchHeaders(THttpContext* pContext)
{
	CONNID dwConnID = pContext->serverConnID;

	if(pContext->https)
		m_pDlg->m_Server->Send(dwConnID, (const BYTE*)HTTPS_RESP, HTTPS_RESP_LEN);
	else
	{
		CStringA strHeaders;	

		USHORT usVersion = m_pDlg->m_Server->GetVersion(dwConnID);

		strHeaders.AppendFormat("%s %s HTTP/%d.%d%s", pContext->method, pContext->path, LOBYTE(usVersion), HIBYTE(usVersion), HEADER_SEP);

		DWORD dwHeaderCount = 0;
		m_pDlg->m_Server->GetAllHeaders(dwConnID, nullptr, dwHeaderCount);

		if(dwHeaderCount > 0)
		{
			unique_ptr<THeader[]> headers(new THeader[dwHeaderCount]);
			m_pDlg->m_Server->GetAllHeaders(dwConnID, headers.get(), dwHeaderCount);

			for( DWORD i = 0; i < dwHeaderCount; i++)
			{
				if(_stricmp(headers[i].name, HEAD_PROXY_CONN) == 0)
				{
					LPCSTR lpszVal;
					if(!m_pDlg->m_Server->GetHeader(dwConnID, HEAD_CONN, &lpszVal))
					{
						strHeaders.AppendFormat("%s: %s%s", HEAD_CONN, headers[i].value, HEADER_SEP);
						continue;
					}
				}
				else if(_stricmp(headers[i].name, HEAD_PROXY_AUTH) == 0)
					continue;

				strHeaders.AppendFormat("%s: %s%s", headers[i].name, headers[i].value, HEADER_SEP);
			}
		}

		LPCSTR lpszTrans = m_pDlg->m_Server->GetTransferEncoding(dwConnID);

		if(lpszTrans && _stricmp(lpszTrans, CHUNK_FLAG) == 0)
			pContext->chunk = TRUE;
		else
			strHeaders.Append(HEADER_SEP);

		m_pDlg->m_Agent->Send(pContext->agentConnID, (const BYTE*)(LPCSTR)strHeaders, strHeaders.GetLength());
	}
}

void CHttpServerListenerImpl::DetachConnInfo(CONNID dwConnID)
{
	THttpContext* pContext = nullptr;

	m_pDlg->m_Server->GetConnectionExtra(dwConnID, (PVOID*)&pContext);
	ASSERT(pContext);

	pContext->serverConnID = 0;

	if(pContext->agentConnID != 0)
		m_pDlg->m_Agent->Disconnect(pContext->agentConnID);

	pContext->DecRef();
}

EnHandleResult CTcpAgentListenerImpl::OnConnect(ITcpAgent* pSender, CONNID dwConnID)
{
	if(m_pDlg->m_bLog)
	{
		TCHAR szHost[80];
		int iAddressLen = sizeof(szHost) / sizeof(TCHAR);
		USHORT usPort;

		m_pDlg->m_Agent->GetRemoteHost(dwConnID, szHost, iAddressLen, usPort);

		::PostOnConnect2(dwConnID, szHost, usPort, AGENT_NAME);
	}

	THttpContext* pContext = nullptr;

	do 
	{
		VERIFY(m_pDlg->m_Agent->GetConnectionExtra(dwConnID, (PVOID*)&pContext));

		if(pContext)
		{
			pContext->evt.Set();
			break;
		}

		::SwitchToThread();

	} while(!pContext);

	return HR_OK;
}

EnHandleResult CTcpAgentListenerImpl::OnSend(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	if(m_pDlg->m_bLog) ::PostOnSend(dwConnID, pData, iLength, AGENT_NAME);

	return HR_OK;
}

EnHandleResult CTcpAgentListenerImpl::OnReceive(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	if(m_pDlg->m_bLog) ::PostOnReceive(dwConnID, pData, iLength, AGENT_NAME);

	THttpContext* pContext = nullptr;

	m_pDlg->m_Agent->GetConnectionExtra(dwConnID, (PVOID*)&pContext);
	ASSERT(pContext);

	if(pContext->serverConnID != 0 && pContext->agentConnID == dwConnID)
		m_pDlg->m_Server->Send(pContext->serverConnID, pData, iLength);

	return HR_OK;
}

EnHandleResult CTcpAgentListenerImpl::OnClose(ITcpAgent* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	if(m_pDlg->m_bLog)
	{
		iErrorCode == SE_OK ? ::PostOnClose(dwConnID, AGENT_NAME)	:
		::PostOnError(dwConnID, enOperation, iErrorCode, AGENT_NAME);
	}

	DetachConnInfo(dwConnID);

	return HR_OK;
}

EnHandleResult CTcpAgentListenerImpl::OnShutdown(ITcpAgent* pSender)
{
	::PostOnShutdown(AGENT_NAME);

	return HR_OK;
}

void CTcpAgentListenerImpl::DetachConnInfo(CONNID dwConnID)
{
	THttpContext* pContext = nullptr;

	do 
	{
		VERIFY(m_pDlg->m_Agent->GetConnectionExtra(dwConnID, (PVOID*)&pContext));

		if(pContext)
			break;

		::SwitchToThread();

	} while(!pContext);

	{
		CCriSecLock locallock(pContext->cs);

		if(pContext->agentConnID == dwConnID)
		{
			pContext->closed	  = TRUE;
			pContext->agentConnID = 0;

			if(pContext->serverConnID != 0)
				m_pDlg->m_Server->Release(pContext->serverConnID);
		}
	}

	pContext->DecRef();
}
