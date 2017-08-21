
// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include "../../../../Common/Src/WaitFor.h"

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

#define DEFAULT_ADDRESS	_T("0.0.0.0")
#define DEFAULT_PORT	_T("5555")

CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CServerDlg::IDD, pParent), m_Server(this)
	, m_lClientCount(0L)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_START, m_Start);
	DDX_Control(pDX, IDC_STOP, m_Stop);
	DDX_Control(pDX, IDC_STATISTICS, m_Statistics);
	DDX_Control(pDX, IDC_BIND_ADDRESS, m_BindAddress);
	DDX_Control(pDX, IDC_PORT, m_Port);
	DDX_Control(pDX, IDC_SEND_POLICY, m_SendPolicy);
	DDX_Control(pDX, IDC_RECV_COUNT, m_RecvCount);
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
	ON_BN_CLICKED(IDC_STATISTICS, &CServerDlg::OnBnClickedStatistics)
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

	m_MaxConnCount.SetCurSel(0);
	m_SendPolicy.SetCurSel(0);
	m_RecvCount.SetCurSel(4);
	m_BindAddress.SetWindowText(DEFAULT_ADDRESS);
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
	m_Statistics.EnableWindow(m_enState == ST_STARTED);
	m_BindAddress.EnableWindow(m_enState == ST_STOPPED);
	m_Port.EnableWindow(m_enState == ST_STOPPED);
	m_SendPolicy.EnableWindow(m_enState == ST_STOPPED);
	m_RecvCount.EnableWindow(m_enState == ST_STOPPED);
	m_MaxConnCount.EnableWindow(m_enState == ST_STOPPED);
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

	CString strRecvCount;
	m_RecvCount.GetWindowText(strRecvCount);
	DWORD dwRecvCount = (DWORD)_ttoi(strRecvCount);

	if(dwRecvCount == 0 || dwRecvCount > MAXLONG)
	{
		MessageBox(_T("Recv Count invalid, pls check!"), _T("Params Error"), MB_OK);
		m_RecvCount.SetFocus();
		return;
	}

	EnSendPolicy enSendPolicy = (EnSendPolicy)m_SendPolicy.GetCurSel();

	CString strMaxConnCount;
	m_MaxConnCount.GetWindowText(strMaxConnCount);
	int iMaxConnCount = _ttoi(strMaxConnCount);

	if(iMaxConnCount == 0)
		iMaxConnCount = 10;

	iMaxConnCount *= 1000;

	SetAppState(ST_STARTING);

	Reset();

	//m_Server->SetFreeSocketObjPool(500);
	//m_Server->SetFreeSocketObjHold(1500);
	//m_Server->SetFreeBufferObjPool(2000);
	//m_Server->SetFreeBufferObjHold(6000);
	//m_Server->SetWorkerThreadCount(10);
	//m_Server->SetDetectAttempts(0);

	m_Server->SetSendPolicy(enSendPolicy);
	m_Server->SetPostReceiveCount(dwRecvCount);
	m_Server->SetMaxConnectionCount(iMaxConnCount);

	if(m_Server->Start(strBindAddress, usPort))
	{
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


void CServerDlg::OnBnClickedStatistics()
{
	m_Server->DisconnectLongConnections(0);
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

EnHandleResult CServerDlg::OnPrepareListen(IUdpServer* pSender, SOCKET soListen)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	pSender->GetListenAddress(szAddress, iAddressLen, usPort);
	::PostOnPrepareListen(szAddress, usPort);
	return HR_OK;
}

EnHandleResult CServerDlg::OnSend(IUdpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
#ifdef _DEBUG
	::PostOnSend(dwConnID, pData, iLength);
#endif

#if (_WIN32_WINNT <= _WIN32_WINNT_WS03)
	::InterlockedExchangeAdd((volatile LONG*)&m_llTotalSent, iLength);
#else
	::InterlockedExchangeAdd64(&m_llTotalSent, iLength);
#endif

	return HR_OK;
}

EnHandleResult CServerDlg::OnReceive(IUdpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
#ifdef _DEBUG
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

EnHandleResult CServerDlg::OnClose(IUdpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID)	:
	::PostOnError(dwConnID, enOperation, iErrorCode);

	Statistics();

	return HR_OK;
}

EnHandleResult CServerDlg::OnAccept(IUdpServer* pSender, CONNID dwConnID, UINT_PTR pSockAddr)
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
	::PostOnAccept2(dwConnID);

	return HR_OK;
}

EnHandleResult CServerDlg::OnShutdown(IUdpServer* pSender)
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
