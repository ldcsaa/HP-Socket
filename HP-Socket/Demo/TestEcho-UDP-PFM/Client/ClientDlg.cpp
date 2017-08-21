
// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
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

// CClientDlg dialog

#define DEFAULT_ADDRESS	_T("127.0.0.1")
#define DEFAULT_PORT	_T("5555")

CClientDlg::CClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_ADDRESS, m_Address);
	DDX_Control(pDX, IDC_PORT, m_Port);
	DDX_Control(pDX, IDC_START, m_Start);
	DDX_Control(pDX, IDC_STOP, m_Stop);
	DDX_Control(pDX, IDC_TEST_TIMES, m_TestTimes);
	DDX_Control(pDX, IDC_SOCKET_COUNT, m_SocketCount);
	DDX_Control(pDX, IDC_DATA_LEN, m_DataLen);
	DDX_Control(pDX, IDC_TEST_TIMES_INTERV, m_TestInterv);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CClientDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CClientDlg::OnBnClickedStop)
	ON_MESSAGE(USER_INFO_MSG, OnUserInfoMsg)
	ON_WM_VKEYTOITEM()
	ON_WM_CLOSE()
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

	m_TestTimes.SetCurSel(5);
	m_TestInterv.SetCurSel(1);
	m_SocketCount.SetCurSel(5);
	m_DataLen.SetCurSel(5);
	m_Address.SetWindowText(DEFAULT_ADDRESS);
	m_Port.SetWindowText(DEFAULT_PORT);

	::SetMainWnd(this);
	::SetInfoList(&m_Info);
	SetAppState(ST_STOPPED);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientDlg::OnClose()
{
	::SetMainWnd(nullptr);
	__super::OnClose();
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
			||	pMsg->wParam == VK_RETURN	
		))
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CClientDlg::SetAppState(EnAppState state)
{
	if(m_enState == state)
		return;

	m_enState = state;

	if(this->GetSafeHwnd() == nullptr)
		return;

	m_Start.EnableWindow(m_enState == ST_STOPPED);
	m_Stop.EnableWindow(m_enState == ST_STARTED);
	m_Address.EnableWindow(m_enState == ST_STOPPED);
	m_Port.EnableWindow(m_enState == ST_STOPPED);
	m_TestTimes.EnableWindow(m_enState == ST_STOPPED);
	m_TestInterv.EnableWindow(m_enState == ST_STOPPED);
	m_SocketCount.EnableWindow(m_enState == ST_STOPPED);
	m_DataLen.EnableWindow(m_enState == ST_STOPPED);
}

BOOL CClientDlg::CheckParams()
{
	BOOL isOK = TRUE;

	if(m_strAddress.IsEmpty())
	{
		m_Address.SetFocus();
		isOK = FALSE;
	}
	else if(m_usPort == 0)
	{
		m_Port.SetFocus();
		isOK = FALSE;
	}
	else if(m_iTestTimes <= 0)
	{
		m_TestTimes.SetFocus();
		isOK = FALSE;
	}
	else if(m_iTestInterv < 0)
	{
		m_TestInterv.SetFocus();
		isOK = FALSE;
	}
	else if(m_iSocketCount <= 0)
	{
		m_SocketCount.SetFocus();
		isOK = FALSE;
	}
	else if(m_iDataLen <= 0)
	{
		m_DataLen.SetFocus();
		isOK = FALSE;
	}

	if(!isOK)
		MessageBox(_T("One or more settings invalid, pls check!"), _T("Params Error"), MB_OK);

	return isOK;
}

void CClientDlg::OnBnClickedStart()
{
	CString strAddress;
	CString strPort;
	CString strTestTimes;
	CString strTestInterv;
	CString strSocketCount;
	CString strDataLen;

	m_Address.GetWindowText(strAddress);
	m_Port.GetWindowText(strPort);
	m_TestTimes.GetWindowText(strTestTimes);
	m_TestInterv.GetWindowText(strTestInterv);
	m_SocketCount.GetWindowText(strSocketCount);
	m_DataLen.GetWindowText(strDataLen);

	m_strAddress	= strAddress.Trim();
	m_usPort		= (USHORT)_ttoi(strPort);
	m_iTestTimes	= _ttoi(strTestTimes);
	m_iTestInterv	= _ttoi(strTestInterv);
	m_iSocketCount	= _ttoi(strSocketCount);
	m_iDataLen		= _ttoi(strDataLen);

	if(!CheckParams())
		return;

	SetAppState(ST_STARTING);

	m_dwBeginTickCount	= 0;
	m_dwTimeconsuming	= 0;
	m_llTotalReceived	= 0;
	m_llTotalSent		= 0;
	m_llExpectReceived	= (LONGLONG)m_iTestTimes * (LONGLONG)m_iSocketCount * (LONGLONG)m_iDataLen;

	m_vtClients.Clear();

	for(int i = 0; i < m_iSocketCount; i++)
	{
		smart_simple_ptr<CUdpClientPtr> pSocket = new CUdpClientPtr(this);

		/*
		(*pSocket)->SetDetectAttempts(0);
		(*pSocket)->SetDetectInterval(0);
		*/

		if((*pSocket)->Start(m_strAddress, m_usPort))
			m_vtClients->push_back(pSocket.release());
		else
		{
			::LogClientStartFail((*pSocket)->GetLastError(), (*pSocket)->GetLastErrorDesc());
			m_vtClients.Clear();
			SetAppState(ST_STOPPED);
			return;
		}
	}

	::LogClientStart(m_strAddress, m_usPort);

	DWORD dwSendDelay = 3;
	CString strMsg;

	strMsg.Format(_T(" *** willing to send data after %d seconds ..."), dwSendDelay);
	::LogMsg(strMsg);

	::WaitWithMessageLoop(dwSendDelay * 1000);
	m_sendBuffer.Malloc(m_iDataLen, true);
	memset(m_sendBuffer, 'X', m_sendBuffer.Size());

	SetAppState(ST_STARTED);

	::LogMsg(_T(" *** Go Now !"));

	m_dwBeginTickCount = ::TimeGetTime();

	BOOL bTerminated = FALSE;
	for(int i = 0; i < m_iTestTimes; i++)
	{
		for(int j = 0; j < m_iSocketCount; j++)
		{
			CUdpClientPtr* pSocket = m_vtClients[j];
			if(!(*pSocket)->Send(m_sendBuffer, (int)m_sendBuffer.Size()))
			{
				::LogClientSendFail(i + 1, j + 1, ::SYS_GetLastError(), ::HP_GetSocketErrorDesc(SE_DATA_SEND));
				bTerminated = TRUE;
				break;
			}
		}

		if(bTerminated)
			break;

		if(m_iTestInterv > 0 && i + 1 < m_iTestTimes)
			::WaitWithMessageLoop(m_iTestInterv);
	}

	m_sendBuffer.Free();
}

void CClientDlg::OnBnClickedStop()
{
	SetAppState(ST_STOPPING);

	for(size_t i = 0; i < m_vtClients.Size(); i++)
	{
		CUdpClientPtr* pSocket = m_vtClients[i];

		if((*pSocket)->Stop())
			::LogClientStopping((DWORD)i + 1);
		//else
		//	ASSERT(FALSE);
	}

	::WaitWithMessageLoop(100L);

	CString strMsg;
	strMsg.Format(	_T(" *** Summary: expect -  %lld, send - %lld, recv - %lld"),
					m_llExpectReceived, m_llTotalSent, m_llTotalReceived);

	::LogMsg(strMsg);

	if(m_llExpectReceived == m_llTotalSent && m_llTotalSent == m_llTotalReceived)
		strMsg.Format(_T(" *** Success: time consuming -  %u millisecond !"), m_dwTimeconsuming);
	else
		strMsg.Format(_T(" *** Fail: manual terminated ? (or data lost)"));

	::LogMsg(strMsg);

	SetAppState(ST_STOPPED);
}

int CClientDlg::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex)
{
	if(nKey == 'C')
		pListBox->ResetContent();

	return __super::OnVKeyToItem(nKey, pListBox, nIndex);
}

LRESULT CClientDlg::OnUserInfoMsg(WPARAM wp, LPARAM lp)
{
	info_msg* msg = (info_msg*)wp;

	::LogInfoMsg(msg);

	return 0;
}

EnHandleResult CClientDlg::OnPrepareConnect(IUdpClient* pSender, CONNID dwConnID, SOCKET socket)
{
	return HR_OK;
}

EnHandleResult CClientDlg::OnSend(IUdpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
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

EnHandleResult CClientDlg::OnReceive(IUdpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
#ifdef _DEBUG
	::PostOnReceive(dwConnID, pData, iLength);
#endif

#if (_WIN32_WINNT <= _WIN32_WINNT_WS03)
	::InterlockedExchangeAdd((volatile LONG*)&m_llTotalReceived, iLength);
#else
	::InterlockedExchangeAdd64(&m_llTotalReceived, iLength);
#endif

	if(m_llTotalReceived == m_llExpectReceived)
	{
		m_dwTimeconsuming = ::GetTimeGap32(m_dwBeginTickCount);
		::PostTimeConsuming(m_dwTimeconsuming);
	}

	ASSERT(m_llTotalReceived <= m_llExpectReceived);
	return HR_OK;
}

EnHandleResult CClientDlg::OnClose(IUdpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID)		:
	::PostOnError(dwConnID, enOperation, iErrorCode)	;

	return HR_OK;
}

EnHandleResult CClientDlg::OnConnect(IUdpClient* pSender, CONNID dwConnID)
{
	::PostOnConnect3(dwConnID);
	return HR_OK;
}
