
// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include "../../../Common/Src/WaitFor.h"

#ifdef _WIN64
		#ifdef _DEBUG
			#pragma comment(lib, "../../Bin/HPSocket/x64/HPSocket_UD.lib")
		#else
			#pragma comment(lib, "../../Bin/HPSocket/x64/HPSocket_U.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib, "../../Bin/HPSocket/x86/HPSocket_UD.lib")
		#else
			#pragma comment(lib, "../../Bin/HPSocket/x86/HPSocket_U.lib")
		#endif
#endif

// CServerDlg dialog

const LPCTSTR CServerDlg::ADDRESS = _T("0.0.0.0");
const USHORT CServerDlg::PORT = 5555;


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
	GetWindowText(strTitle);
	strTitle.Format(_T("%s - (%s:%d)"), strTitle, ADDRESS, PORT);
	SetWindowText(strTitle);

	::SetMainWnd(this);
	::SetInfoList(&m_Info);
	SetAppState(ST_STOPED);

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

	m_Start.EnableWindow(m_enState == ST_STOPED);
	m_Stop.EnableWindow(m_enState == ST_STARTED);
}

void CServerDlg::OnBnClickedStart()
{
	Reset();
	SetAppState(ST_STARTING);

	//m_Server->SetFreeSocketObjPool(500);
	//m_Server->SetFreeSocketObjHold(1500);
	//m_Server->SetFreeBufferObjPool(2000);
	//m_Server->SetFreeBufferObjHold(6000);
	//m_Server->SetAcceptSocketCount(50);

	if(m_Server->Start(ADDRESS, PORT))
	{
		::LogServerStart(ADDRESS, PORT);
		SetAppState(ST_STARTED);
	}
	else
	{
		::LogServerStartFail(m_Server->GetLastError(), m_Server->GetLastErrorDesc());
		SetAppState(ST_STOPED);
	}
}

void CServerDlg::OnBnClickedStop()
{
	SetAppState(ST_STOPING);

	if(m_Server->Stop())
	{
		::LogServerStop();
		SetAppState(ST_STOPED);
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

ISocketListener::EnHandleResult CServerDlg::OnPrepareListen(SOCKET soListen)
{
	TCHAR szAddress[40];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	m_Server->GetListenAddress(szAddress, iAddressLen, usPort);
	::PostOnPrepareListen(szAddress, usPort);
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CServerDlg::OnSend(CONNID dwConnID, const BYTE* pData, int iLength)
{
#ifdef _DEBUG
	::PostOnSend(dwConnID, pData, iLength);
#endif

#if (WINVER < 0x0502)
	::InterlockedExchangeAdd((volatile LONG*)&m_llTotalSent, iLength);
#else
	::InterlockedExchangeAdd64(&m_llTotalSent, iLength);
#endif

	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CServerDlg::OnReceive(CONNID dwConnID, const BYTE* pData, int iLength)
{
#ifdef _DEBUG
	::PostOnReceive(dwConnID, pData, iLength);
#endif

#if (WINVER < 0x0502)
	::InterlockedExchangeAdd((volatile LONG*)&m_llTotalReceived, iLength);
#else
	::InterlockedExchangeAdd64(&m_llTotalReceived, iLength);
#endif

	if(m_Server->Send(dwConnID, pData, iLength))
		return ISocketListener::HR_OK;
	else
		return ISocketListener::HR_ERROR;
}

ISocketListener::EnHandleResult CServerDlg::OnClose(CONNID dwConnID)
{
	::PostOnClose(dwConnID);
	Statistics();

	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CServerDlg::OnError(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	::PostOnError(dwConnID, enOperation, iErrorCode);
	Statistics();

	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CServerDlg::OnAccept(CONNID dwConnID, SOCKET soClient)
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

	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CServerDlg::OnServerShutdown()
{
	::PostOnShutdown();
	return ISocketListener::HR_OK;
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
