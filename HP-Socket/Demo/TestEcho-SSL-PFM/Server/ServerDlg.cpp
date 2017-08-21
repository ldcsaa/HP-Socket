
// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include "../../../../Common/Src/WaitFor.h"
#include "../../../../Common/Src/SysHelper.h"

#ifdef _WIN64
	#ifdef _DEBUG
		#pragma comment(lib, "../../../Bin/HPSocket/x64/static/HPSocket-SSL_UD.lib")
	#else
		#pragma comment(lib, "../../../Bin/HPSocket/x64/static/HPSocket-SSL_U.lib")
	#endif
#else
	#ifdef _DEBUG
		#pragma comment(lib, "../../../Bin/HPSocket/x86/static/HPSocket-SSL_UD.lib")
	#else
		#pragma comment(lib, "../../../Bin/HPSocket/x86/static/HPSocket-SSL_U.lib")
	#endif
#endif

// CServerDlg dialog

#define DEFAULT_ADDRESS	_T("0.0.0.0")
#define DEFAULT_PORT	_T("5555")

CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CServerDlg::IDD, pParent), m_Server(this)
	, m_lClientCount(0L)
{
	VERIFY(m_Server->SetupSSLContext(g_s_iVerifyMode, g_s_lpszPemCertFile, g_s_lpszPemKeyFile, g_s_lpszKeyPasswod, g_s_lpszCAPemCertFileOrPath));

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_START, m_Start);
	DDX_Control(pDX, IDC_STOP, m_Stop);
	DDX_Control(pDX, IDC_PORT, m_Port);
	DDX_Control(pDX, IDC_SEND_POLICY, m_SendPolicy);
	DDX_Control(pDX, IDC_THREAD_COUNT, m_ThreadCount);
	DDX_Control(pDX, IDC_MAX_CONN_COUNT, m_MaxConnCount);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CServerDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CServerDlg::OnBnClickedStop)
	ON_MESSAGE(USER_INFO_MSG, OnUserInfoMsg)
	ON_WM_VKEYTOITEM()
	ON_WM_CLOSE()
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
	m_SendPolicy.SetCurSel(0);
	m_ThreadCount.SetCurSel(0);
	m_MaxConnCount.SetCurSel(0);
	m_Port.SetWindowText(DEFAULT_PORT);

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
	m_Port.EnableWindow(m_enState == ST_STOPPED);
	m_SendPolicy.EnableWindow(m_enState == ST_STOPPED);
	m_ThreadCount.EnableWindow(m_enState == ST_STOPPED);
	m_MaxConnCount.EnableWindow(m_enState == ST_STOPPED);
}

void CServerDlg::OnBnClickedStart()
{
	CString strPort;
	m_Port.GetWindowText(strPort);
	USHORT usPort = (USHORT)_ttoi(strPort);

	if(usPort == 0)
	{
		MessageBox(_T("Listen Port invalid, pls check!"), _T("Params Error"), MB_OK);
		m_Port.SetFocus();
		return;
	}

	EnSendPolicy enSendPolicy = (EnSendPolicy)m_SendPolicy.GetCurSel();

	CString strThreadCount;
	m_ThreadCount.GetWindowText(strThreadCount);
	int iThreadCount = _ttoi(strThreadCount);

	if(iThreadCount == 0)
		iThreadCount = min((::SysGetNumberOfProcessors() * 2 + 2), 500);
	else if(iThreadCount < 0 || iThreadCount > 500)
	{
		m_ThreadCount.SetFocus();
		return;
	}

	CString strMaxConnCount;
	m_MaxConnCount.GetWindowText(strMaxConnCount);
	int iMaxConnCount = _ttoi(strMaxConnCount);

	if(iMaxConnCount == 0)
		iMaxConnCount = 10;

	iMaxConnCount *= 1000;

	SetAppState(ST_STARTING);

	Reset();

	m_Server->SetSendPolicy(enSendPolicy);
	m_Server->SetWorkerThreadCount(iThreadCount);
	m_Server->SetMaxConnectionCount(iMaxConnCount);

	//m_Server->SetFreeSocketObjPool(500);
	//m_Server->SetFreeSocketObjHold(1500);
	//m_Server->SetFreeBufferObjPool(2000);
	//m_Server->SetFreeBufferObjHold(6000);
	//m_Server->SetSocketListenQueue(2000);
	//m_Server->SetAcceptSocketCount(2000);


	if(m_Server->Start(DEFAULT_ADDRESS, usPort))
	{
		::LogServerStart(DEFAULT_ADDRESS, usPort);
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

	if(m_Server->Stop())
	{
		::LogServerStop();
		SetAppState(ST_STOPPED);
	}
	else
	{
		ASSERT(FALSE);
	}
}

int CServerDlg::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex)
{
	if(nKey == 'C')
		pListBox->ResetContent();
	else if(nKey == 'R')
	{
		Reset();

		CString strMsg;
		strMsg.Format(	_T(" *** Reset Statics: CC -  %u, TS - %lld, TR - %lld"),
						m_lClientCount, m_llTotalSent, m_llTotalReceived);

		::LogMsg(strMsg);
	}

	return __super::OnVKeyToItem(nKey, pListBox, nIndex);
}

LRESULT CServerDlg::OnUserInfoMsg(WPARAM wp, LPARAM lp)
{
	info_msg* msg = (info_msg*)wp;

	::LogInfoMsg(msg);

	return 0;
}

EnHandleResult CServerDlg::OnPrepareListen(ITcpServer* pSender, SOCKET soListen)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	pSender->GetListenAddress(szAddress, iAddressLen, usPort);
	::PostOnPrepareListen(szAddress, usPort);
	return HR_OK;
}

EnHandleResult CServerDlg::OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
#ifdef _DEBUG2
	::PostOnSend(dwConnID, pData, iLength);
#endif

#if (_WIN32_WINNT <= _WIN32_WINNT_WS03)
	::InterlockedExchangeAdd((volatile LONG*)&m_llTotalSent, iLength);
#else
	::InterlockedExchangeAdd64(&m_llTotalSent, iLength);
#endif

	return HR_OK;
}

EnHandleResult CServerDlg::OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
#ifdef _DEBUG2
	::PostOnReceive(dwConnID, pData, iLength);
#endif

#if (_WIN32_WINNT <= _WIN32_WINNT_WS03)
	::InterlockedExchangeAdd((volatile LONG*)&m_llTotalReceived, iLength);
#else
	::InterlockedExchangeAdd64(&m_llTotalReceived, iLength);
#endif

	if(pSender->Send(dwConnID, pData, iLength))
		return HR_OK;
	else
		return HR_ERROR;
}

EnHandleResult CServerDlg::OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID)	:
	::PostOnError(dwConnID, enOperation, iErrorCode);

	Statistics();

	return HR_OK;
}

EnHandleResult CServerDlg::OnAccept(ITcpServer* pSender, CONNID dwConnID, SOCKET soClient)
{
	::PostOnAccept2(dwConnID);
	return HR_OK;
}

EnHandleResult CServerDlg::OnHandShake(ITcpServer* pSender, CONNID dwConnID)
{
	if(m_lClientCount == 0)
	{
		CCriSecLock lock(m_cs);

		if(m_lClientCount == 0)
		{
			Reset(FALSE);
		}
	}

	::InterlockedIncrement(&m_lClientCount);
	::PostOnHandShake(dwConnID);

	return HR_OK;
}

EnHandleResult CServerDlg::OnShutdown(ITcpServer* pSender)
{
	::PostOnShutdown();
	return HR_OK;
}

void CServerDlg::Statistics()
{
	if(m_lClientCount > 0)
	{
		CCriSecLock lock(m_cs);

		if(m_lClientCount > 0)
		{
			::InterlockedDecrement(&m_lClientCount);

			if(m_lClientCount == 0)
			{
				::WaitWithMessageLoop(600L);
				::PostServerStatics((LONGLONG)m_llTotalSent, (LONGLONG)m_llTotalReceived);
			}
		}
	}
}

void CServerDlg::Reset(BOOL bResetClientCount)
{
	if(bResetClientCount)
		m_lClientCount	= 0L;

	m_llTotalSent		= 0L;
	m_llTotalReceived	= 0L;
}
