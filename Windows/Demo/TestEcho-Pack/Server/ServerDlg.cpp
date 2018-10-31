
// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"


// CServerDlg dialog

const LPCTSTR CServerDlg::ADDRESS	= _T("0.0.0.0");
const USHORT CServerDlg::PORT		= 5555;
CServerDlg* CServerDlg::m_spThis	= nullptr;

CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_spThis = this;

	// 创建监听器对象
	m_pListener	= ::Create_HP_TcpPackServerListener();
	// 创建 Socket 对象
	m_pServer		= ::Create_HP_TcpPackServer(m_pListener);
	// 设置 Socket 监听器回调函数
	::HP_Set_FN_Server_OnPrepareListen(m_pListener, OnPrepareListen);
	::HP_Set_FN_Server_OnAccept(m_pListener, OnAccept);
	::HP_Set_FN_Server_OnSend(m_pListener, OnSend);
	::HP_Set_FN_Server_OnReceive(m_pListener, OnReceive);
	::HP_Set_FN_Server_OnClose(m_pListener, OnClose);
	::HP_Set_FN_Server_OnShutdown(m_pListener, OnShutdown);
}

CServerDlg::~CServerDlg()
{
	// 销毁 Socket 对象
	::Destroy_HP_TcpPackServer(m_spThis->m_pServer);
	// 销毁监听器对象
	::Destroy_HP_TcpServerListener(m_pListener);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_START, m_Start);
	DDX_Control(pDX, IDC_STOP, m_Stop);
	DDX_Control(pDX, IDC_ADDRESS, m_Address);
	DDX_Control(pDX, IDC_CONN_ID, m_ConnID);
	DDX_Control(pDX, IDC_DISCONNECT, m_DisConn);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CServerDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CServerDlg::OnBnClickedStop)
	ON_MESSAGE(USER_INFO_MSG, OnUserInfoMsg)
	ON_BN_CLICKED(IDC_DISCONNECT, &CServerDlg::OnBnClickedDisconnect)
	ON_EN_CHANGE(IDC_CONN_ID, &CServerDlg::OnEnChangeConnId)
	ON_WM_VKEYTOITEM()
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
	strTitle.Format(_T("%s - (%s:%d)"), strOriginTitle, ADDRESS, PORT);
	SetWindowText(strTitle);

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
	m_Address.EnableWindow(m_enState == ST_STOPPED);
	m_DisConn.EnableWindow(m_enState == ST_STARTED && m_ConnID.GetWindowTextLength() > 0);
}

void CServerDlg::OnBnClickedStart()
{
	m_Address.GetWindowText(m_strAddress);
	m_strAddress.Trim();

	SetAppState(ST_STARTING);
	
	::HP_TcpPackServer_SetMaxPackSize(m_spThis->m_pServer, 0xFFF);
	::HP_TcpPackServer_SetPackHeaderFlag(m_spThis->m_pServer, 0x169);

	if(::HP_Server_Start(m_spThis->m_pServer, ADDRESS, PORT))
	{
		::LogServerStart(ADDRESS, PORT);
		SetAppState(ST_STARTED);
	}
	else
	{
		::LogServerStartFail(::HP_Server_GetLastError(m_spThis->m_pServer), ::HP_Server_GetLastErrorDesc(m_spThis->m_pServer));
		SetAppState(ST_STOPPED);
	}
}

void CServerDlg::OnBnClickedStop()
{
	SetAppState(ST_STOPPING);

	if(::HP_Server_Stop(m_spThis->m_pServer))
	{
		::LogServerStop();
		SetAppState(ST_STOPPED);
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CServerDlg::OnBnClickedDisconnect()
{
	CString strConnID;
	m_ConnID.GetWindowText(strConnID);
	HP_CONNID dwConnID = (HP_CONNID)_ttoi(strConnID);

	if(::HP_Server_Disconnect(m_spThis->m_pServer, dwConnID, TRUE))
		::LogDisconnect(dwConnID);
	else
		::LogDisconnectFail(dwConnID);
}

void CServerDlg::OnEnChangeConnId()
{
	m_DisConn.EnableWindow(m_enState == ST_STARTED && m_ConnID.GetWindowTextLength() > 0);
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

En_HP_HandleResult CServerDlg::OnPrepareListen(HP_Server pSender, SOCKET soListen)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	::HP_Server_GetListenAddress(pSender, szAddress, &iAddressLen, &usPort);
	::PostOnPrepareListen(szAddress, usPort);
	return HR_OK;
}

En_HP_HandleResult CServerDlg::OnAccept(HP_Server pSender, HP_CONNID dwConnID, SOCKET soClient)
{
	BOOL bPass = TRUE;
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	::HP_Server_GetRemoteAddress(pSender, dwConnID, szAddress, &iAddressLen, &usPort);

	if(!m_spThis->m_strAddress.IsEmpty())
	{
		if(m_spThis->m_strAddress.CompareNoCase(szAddress) == 0)
			bPass = FALSE;
	}

	::PostOnAccept(dwConnID, szAddress, usPort, bPass);

	return bPass ? HR_OK : HR_ERROR;
}

En_HP_HandleResult CServerDlg::OnSend(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength);
	return HR_OK;
}

En_HP_HandleResult CServerDlg::OnReceive(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnReceive(dwConnID, pData, iLength);

	if(!::HP_Server_Send(pSender, dwConnID, pData, iLength))
		return HR_ERROR;

	return HR_OK;
}

En_HP_HandleResult CServerDlg::OnClose(HP_Server pSender, HP_CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID):
	::PostOnError(dwConnID, enOperation, iErrorCode);

	return HR_OK;
}

En_HP_HandleResult CServerDlg::OnShutdown(HP_Server pSender)
{
	::PostOnShutdown();

	return HR_OK;
}
