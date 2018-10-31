
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
#define CLEANER_THREAD_CHECK_INTERVAL	5000
#define CONNECTION_RELEASE_DELAY		3000
#define SILENCE_CONNECTION_CHECK		60000
#define SILENCE_CONNECTION_DURATION		90000

#define DEF_HTTP_PORT	80
#define DEF_HTTPS_PORT	443
#define HTTPS_FLAG		"CONNECT"
#define HTTP_SCHEME		"http://"
#define HTTP_1_1		"HTTP/1.1"
#define HEAD_HOST		"Host:"
#define HEAD_CONN		"Connection:"
#define HEAD_PROXY_CONN	"Proxy-Connection:"
#define HEAD_PROXY_AUTH	"Prxoy-Authenticate:"
#define HTTPS_RESP		"HTTP/1.1 200 Connection Established"
#define IE_OPT_DIALOG	"rundll32.exe shell32.dll, Control_RunDLL inetcpl.cpl, , 4"

static int HTTP_SCHEME_LEN		= (int)strlen(HTTP_SCHEME);
static int HEAD_HOST_LEN		= (int)strlen(HEAD_HOST);
static int HEAD_CONN_LEN		= (int)strlen(HEAD_CONN);
static int HEAD_PROXY_CONN_LEN	= (int)strlen(HEAD_PROXY_CONN);
static int HEAD_PROXY_AUTH_LEN	= (int)strlen(HEAD_PROXY_AUTH);
static int HTTPS_RESP_LEN		= (int)strlen(HTTPS_RESP);

CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CServerDlg::IDD, pParent)
	, m_pServerListener(new CTcpServerListenerImpl(this))
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
		dwRetValue = ::WaitForSingleObject(m_evCleaner, CLEANER_THREAD_CHECK_INTERVAL);

		if(dwRetValue != WAIT_TIMEOUT)
		{
			ASSERT(dwRetValue == WAIT_OBJECT_0);
			break;
		}
		else
		{
			KillServerDyingConnections();
			DisconnectAgentSilenceConnections();
		}
	}

	ReleaseServerDyingConnections();

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

void CServerDlg::KillServerDyingConnections()
{
	TDyingConnection* pDyingConn		= nullptr;
	TDyingConnection* pFirstDyingConn	= nullptr;
	DWORD now							= ::TimeGetTime();

	while(m_lsDyingQueue.UnsafePeekFront(&pDyingConn))
	{
		if((int)(now - pDyingConn->killTime) < CONNECTION_RELEASE_DELAY)
			break;

		BOOL bDisconnect = TRUE;
		BOOL bDestruct	 = TRUE;

		VERIFY(m_lsDyingQueue.UnsafePopFront(&pDyingConn));

		int iPending;
		if(!m_Server->GetPendingDataLength(pDyingConn->connID, iPending))
			bDisconnect = FALSE;
		else if(iPending > 0)
		{
			bDisconnect	= FALSE;
			bDestruct	= FALSE;
		}

		if(bDisconnect)
			m_Server->Disconnect(pDyingConn->connID, TRUE);

		if(bDestruct)
		{
			TDyingConnection::Destruct(pDyingConn);

			if(pFirstDyingConn == pDyingConn)
				pFirstDyingConn = nullptr;
		}
		else
		{
			m_lsDyingQueue.PushBack(pDyingConn);

			if(pFirstDyingConn == nullptr)
				pFirstDyingConn = pDyingConn;
			else if(pFirstDyingConn == pDyingConn)
				break;
		}
	}
}

void CServerDlg::ReleaseServerDyingConnections()
{
	TDyingConnection* pDyingConn = nullptr;

	while(m_lsDyingQueue.UnsafePopFront(&pDyingConn))
		TDyingConnection::Destruct(pDyingConn);

	VERIFY(m_lsDyingQueue.IsEmpty());
}

void CServerDlg::DisconnectAgentSilenceConnections()
{
	static DWORD last	= ::TimeGetTime();
	DWORD now			= ::TimeGetTime();

	if((int)(now - last) >= SILENCE_CONNECTION_CHECK)
	{
		m_Agent->DisconnectSilenceConnections(SILENCE_CONNECTION_DURATION);
		last = now;
	}
}

EnHandleResult CTcpServerListenerImpl::OnPrepareListen(ITcpServer* pSender, SOCKET soListen)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	m_pDlg->m_Server->GetListenAddress(szAddress, iAddressLen, usPort);
	::PostOnPrepareListen(szAddress, usPort, SERVER_NAME);

	VERIFY(m_pDlg->m_thCleaner.Start(m_pDlg, &CServerDlg::CleanerThreadProc));

	return HR_OK;
}

EnHandleResult CTcpServerListenerImpl::OnAccept(ITcpServer* pSender, CONNID dwConnID, SOCKET soClient)
{
	//if(m_pDlg->m_bLog) ::PostOnAccept2(dwConnID, SERVER_NAME);

	m_pDlg->m_Server->SetConnectionExtra(dwConnID, THttpContext::Construct(dwConnID));

	return HR_OK;
}

EnHandleResult CTcpServerListenerImpl::OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	//if(m_pDlg->m_bLog) ::PostOnSend(dwConnID, pData, iLength, SERVER_NAME);

	return HR_OK;
}

EnHandleResult CTcpServerListenerImpl::OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	//if(m_pDlg->m_bLog) ::PostOnReceive(dwConnID, pData, iLength, SERVER_NAME);

	EnHandleResult rs		= HR_OK;
	THttpContext* pContext	= nullptr;

	m_pDlg->m_Server->GetConnectionExtra(dwConnID, (PVOID*)&pContext);
	ASSERT(pContext);

	if(!pContext->parsed)
		rs = ParseRequest(pContext, pData, iLength);
	else if(pContext->agentConnID != 0)
		m_pDlg->m_Agent->Send(pContext->agentConnID, pData, iLength);

	return rs;
}

EnHandleResult CTcpServerListenerImpl::OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
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

EnHandleResult CTcpServerListenerImpl::OnShutdown(ITcpServer* pSender)
{
	m_pDlg->WaitForCleanerThreadEnd();

	::PostOnShutdown(SERVER_NAME);

	return HR_OK;
}

void CTcpServerListenerImpl::DetachConnInfo(CONNID dwConnID)
{
	THttpContext* pContext = nullptr;

	m_pDlg->m_Server->GetConnectionExtra(dwConnID, (PVOID*)&pContext);
	ASSERT(pContext);

	pContext->serverConnID = 0;

	if(pContext->agentConnID != 0)
		m_pDlg->m_Agent->Disconnect(pContext->agentConnID);

	pContext->DecRef();
}

EnHandleResult CTcpServerListenerImpl::ParseRequest(THttpContext* pContext, const BYTE* pData, int iLength)
{
	CStringA& strHeader	= pContext->header;
	int iPreLength		= strHeader.GetLength();

	strHeader.Append((LPCSTR)pData, iLength);

	if(pContext->separator[0] == 0)
	{
		int p = strHeader.Find('\n');

		if(p < 0)		return HR_OK;
		else if(p == 0)	return HR_ERROR;

		int i = 0;

		if(strHeader.GetAt(p - 1) == '\r')
			pContext->separator[i++] = '\r';

		pContext->separator[i] = '\n';
	}

	CStringA strFlag(pContext->separator);
	strFlag.Append(pContext->separator);

	int iFlagLength	= strFlag.GetLength();
	int iStart		= max(0, iPreLength - iFlagLength);
	int iFlagPos	= strHeader.Find(strFlag, iStart);

	ASSERT(iFlagPos != 0);
	if(iFlagPos < 0) return HR_OK;

	int iLength2		= iLength - (iFlagPos + iFlagLength - iPreLength);
	const BYTE* pData2	= pData + (iLength - iLength2);

	strHeader.Truncate(iFlagPos);

	CStringA strData;

	if(!ParseConnectionInfo(pContext, strData))
		return HR_ERROR;

	return ConnectToServer(pContext, strData, pData2, iLength2);
}

BOOL CTcpServerListenerImpl::ParseConnectionInfo(THttpContext* pContext, CStringA& strData)
{
	CStringA& strHeader = pContext->header;

	int i		= 0;
	int iLine	= 0;

	CStringA strLine;
	//CStringA strFullHost;

	BOOL bHasConnHeader = FALSE;
	CStringA strProxyConnHeader;

	strData.Empty();

	while(TRUE) 
	{
		strLine = strHeader.Tokenize(pContext->separator, iLine);

		if(iLine < 0 || strLine.IsEmpty())
			break;

		int iField = 0;

		if(i == 0)
		{
			for(int j = 0; j < 3; j++)
			{
				CStringA strField = strLine.Tokenize(" ", iField);

				if(strField.IsEmpty())
					goto OUTER;

				if(j == 0)
					pContext->https = (strField.CompareNoCase(HTTPS_FLAG) == 0);
				else if(j == 2)
					pContext->http11 = (strField.CompareNoCase(HTTP_1_1) == 0);
				else if(j == 1)
				{
					CStringA strFullHost;

					if(pContext->https)
						strFullHost = strField;
					else
					{
						if(_strnicmp((LPCSTR)strField, HTTP_SCHEME, HTTP_SCHEME_LEN) != 0)
							continue;

						int iPos = HTTP_SCHEME_LEN;
						strFullHost = strField.Tokenize("/", iPos);
					}

					if(!strFullHost.IsEmpty())
						ParseHost(pContext, strFullHost);
				}
			}

			if(pContext->https)
			{
				strData.Append(HTTPS_RESP);
				strData.Append(pContext->separator);
				goto OUTER;
			}
			/*
			else if(pContext->http11 && !strFullHost.IsEmpty())
			{
				CStringA strSchemeHost = HTTP_SCHEME;
				strSchemeHost += strFullHost;

				strLine.Replace(strSchemeHost, "");
			}
			*/
		}
		else
		{
			if(_strnicmp((LPCSTR)strLine, HEAD_HOST, HEAD_HOST_LEN) == 0)
			{
				CStringA strFullHost = strLine.Right(strLine.GetLength() - HEAD_HOST_LEN);
				ParseHost(pContext, strFullHost.Trim());
			}
			else if(_strnicmp((LPCSTR)strLine, HEAD_CONN, HEAD_CONN_LEN) == 0)
				bHasConnHeader = TRUE;
			else if(_strnicmp((LPCSTR)strLine, HEAD_PROXY_CONN, HEAD_PROXY_CONN_LEN) == 0)
			{
				if(!bHasConnHeader)
				{
					strProxyConnHeader = strLine;
					strProxyConnHeader.Replace(HEAD_PROXY_CONN, HEAD_CONN);
				}

				strLine.Empty();
			}
			else if(_strnicmp((LPCSTR)strLine, HEAD_PROXY_AUTH, HEAD_PROXY_AUTH_LEN) == 0)
				strLine.Empty();
		}

		if(!strLine.IsEmpty())
		{
			strData.Append(strLine);
			strData.Append(pContext->separator);
		}

		++i;
	}

	if(!bHasConnHeader && !strProxyConnHeader.IsEmpty())
	{
		strData.Append(strProxyConnHeader);
		strData.Append(pContext->separator);
	}

OUTER:

	strHeader.Empty();

	BOOL isOK = !pContext->host.IsEmpty();

	if(isOK)
	{
		strData.Append(pContext->separator);
		pContext->parsed = TRUE;
	}

	return isOK;
}

void CTcpServerListenerImpl::ParseHost(THttpContext* pContext, const CStringA& strFullHost)
{
	int iPos = 0;

	for(int k = 0; k < 2; k++)
	{
		CStringA strToken = strFullHost.Tokenize(":", iPos);

		if(strToken.IsEmpty())
			break;

		if(k == 0)
			pContext->host = strToken;
		else if(k == 1)
			pContext->port = atoi(strToken);
	}

	if(pContext->port == 0)
		pContext->port = pContext->https ? DEF_HTTPS_PORT : DEF_HTTP_PORT;
}

EnHandleResult CTcpServerListenerImpl::ConnectToServer(THttpContext* pContext, const CStringA& strData, const BYTE* pData2, int iLength2)
{
	pContext->IncRef();

	CONNID dwAgentID = 0;
	if(!m_pDlg->m_Agent->Connect(CA2T(pContext->host), pContext->port, &dwAgentID))
	{
		::PostOnError(dwAgentID, SO_CONNECT, ::SYS_GetLastError(), AGENT_NAME);

		pContext->DecRef();
		return HR_ERROR;
	}

	pContext->agentConnID = dwAgentID;
	VERIFY(m_pDlg->m_Agent->SetConnectionExtra(dwAgentID, pContext));

	DWORD dwRetValue = ::WaitForSingleObject(pContext->evt, CONNECT_TO_SERVER_WAIT_TIME);

	if(dwRetValue != WAIT_TIMEOUT)
		ASSERT(dwRetValue == WAIT_OBJECT_0);
	else
	{
		if(pContext->agentConnID != 0)
			m_pDlg->m_Agent->Disconnect(pContext->agentConnID);

		return HR_ERROR;
	}

	if(pContext->https)
		m_pDlg->m_Server->Send(pContext->serverConnID, (BYTE*)(LPCSTR)strData, strData.GetLength());
	else
		m_pDlg->m_Agent->Send(pContext->agentConnID, (BYTE*)(LPCSTR)strData, strData.GetLength());

	if(iLength2 > 0)
		m_pDlg->m_Agent->Send(pContext->agentConnID, pData2, iLength2);

	return HR_OK;
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

	if(pContext->serverConnID != 0)
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

	pContext->agentConnID = 0;

	if(pContext->serverConnID != 0)
		m_pDlg->m_lsDyingQueue.PushBack(TDyingConnection::Construct(pContext->serverConnID));

	pContext->DecRef();
}
