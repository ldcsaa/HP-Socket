
// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"


// CClientDlg dialog

#define DEFAULT_CONTENT		_T("text to be sent")
#define DEFAULT_CAST_ADDR	_T("233.0.0.1")
#define DEFAULT_BIND_ADDR	_T("0.0.0.0")
#define DEFAULT_PORT		_T("5555")
#define DEFAULT_TTL			_T("1")


CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent), m_Cast(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONTENT, m_Content);
	DDX_Control(pDX, IDC_SEND, m_Send);
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_PORT, m_Port);
	DDX_Control(pDX, IDC_START, m_Start);
	DDX_Control(pDX, IDC_STOP, m_Stop);
	DDX_Control(pDX, IDC_CAST_ADDRESS, m_CastAddr);
	DDX_Control(pDX, IDC_BIND_ADDRESS, m_BindAddr);
	DDX_Control(pDX, IDC_TTL, m_TTL);
	DDX_Control(pDX, IDC_IP_LOOP, m_IPLoop);
	DDX_Control(pDX, IDC_REUSE_ADDR, m_ReuseAddr);
	DDX_Control(pDX, IDC_MULTI_MODE, m_CastMode);
	DDX_Control(pDX, IDC_BROAD_MODE, m_CastMode2);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_CONTENT, &CClientDlg::OnEnChangeContent)
	ON_BN_CLICKED(IDC_SEND, &CClientDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_START, &CClientDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CClientDlg::OnBnClickedStop)
	ON_MESSAGE(USER_INFO_MSG, OnUserInfoMsg)
	ON_WM_VKEYTOITEM()
	ON_BN_CLICKED(IDC_MULTI_MODE, &CClientDlg::OnBnClickedMultiMode)
	ON_BN_CLICKED(IDC_BROAD_MODE, &CClientDlg::OnBnClickedBroadMode)
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

	m_Content.SetWindowText(DEFAULT_CONTENT);
	m_CastAddr.SetWindowText(DEFAULT_CAST_ADDR);
	m_BindAddr.SetWindowText(DEFAULT_BIND_ADDR);
	m_Port.SetWindowText(DEFAULT_PORT);
	m_TTL.SetWindowText(DEFAULT_TTL);
	m_CastMode.SetCheck(BST_CHECKED);
	m_IPLoop.SetCheck(BST_CHECKED);
	m_ReuseAddr.SetCheck(BST_UNCHECKED);

	::SetMainWnd(this);
	::SetInfoList(&m_Info);
	SetAppState(ST_STOPPED);

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
			||	pMsg->wParam == VK_RETURN	
		))
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CClientDlg::SetAppState(EnAppState state)
{
	m_enState = state;

	if(this->GetSafeHwnd() == nullptr)
		return;

	m_Start.EnableWindow(m_enState == ST_STOPPED);
	m_Stop.EnableWindow(m_enState == ST_STARTED);
	m_Send.EnableWindow(m_enState == ST_STARTED && m_Content.GetWindowTextLength() > 0);
	m_CastAddr.EnableWindow(m_enState == ST_STOPPED && m_CastMode.GetCheck() == BST_CHECKED);
	m_BindAddr.EnableWindow(m_enState == ST_STOPPED);
	m_Port.EnableWindow(m_enState == ST_STOPPED);
	m_TTL.EnableWindow(m_enState == ST_STOPPED);
	m_CastMode.EnableWindow(m_enState == ST_STOPPED);
	m_CastMode2.EnableWindow(m_enState == ST_STOPPED);
	m_IPLoop.EnableWindow(m_enState == ST_STOPPED);
	m_ReuseAddr.EnableWindow(m_enState == ST_STOPPED);
}

void CClientDlg::OnEnChangeContent()
{
	m_Send.EnableWindow(m_enState == ST_STARTED && m_Content.GetWindowTextLength() > 0);
}

void CClientDlg::OnBnClickedMultiMode()
{
	m_CastAddr.EnableWindow(m_enState == ST_STOPPED && m_CastMode.GetCheck() == BST_CHECKED);
}

void CClientDlg::OnBnClickedBroadMode()
{
	m_CastAddr.EnableWindow(m_enState == ST_STOPPED && m_CastMode.GetCheck() == BST_CHECKED);
}

void CClientDlg::OnBnClickedSend()
{
	USES_CONVERSION;

	CString strContent;
	m_Content.GetWindowText(strContent);

	LPSTR lpszContent = T2A((LPTSTR)(LPCTSTR)strContent);
	int iLen = (int)strlen(lpszContent);

	if(m_Cast.Send((LPBYTE)lpszContent, iLen))
		::LogSend(m_Cast.GetConnectionID(), strContent);
	else
		::LogSendFail(m_Cast.GetConnectionID(), ::GetLastError(), ::GetSocketErrorDesc(SE_DATA_SEND));
}


void CClientDlg::OnBnClickedStart()
{
	SetAppState(ST_STARTING);


	CString strCastAddress;
	CString strBindAddress;
	CString strPort;
	CString strTTL;

	m_CastAddr.GetWindowText(strCastAddress);
	m_BindAddr.GetWindowText(strBindAddress);
	m_Port.GetWindowText(strPort);
	m_TTL.GetWindowText(strTTL);

	USHORT usPort		= (USHORT)_ttoi(strPort);
	int iTTL			= _ttoi(strTTL);
	EnCastMode enMode	= (m_CastMode.GetCheck() == BST_CHECKED) ? CM_MULTICAST : CM_BROADCAST;
	BOOL bIPLoop		= (m_IPLoop.GetCheck() == BST_CHECKED) ? TRUE : FALSE;
	BOOL bReuseAddr		= (m_ReuseAddr.GetCheck() == BST_CHECKED) ? TRUE : FALSE;

	// OPTIONS ...
	m_Cast.SetCastMode(enMode);
	m_Cast.SetMultiCastLoop(bIPLoop);
	m_Cast.SetReuseAddress(bReuseAddr);
	m_Cast.SetMultiCastTtl(iTTL);

	::LogClientStarting(strCastAddress, usPort);

	if(m_Cast.Start(strCastAddress, usPort, FALSE, strBindAddress))
	{

	}
	else
	{
		::LogClientStartFail(m_Cast.GetLastError(), m_Cast.GetLastErrorDesc());
		SetAppState(ST_STOPPED);
	}
}


void CClientDlg::OnBnClickedStop()
{
	SetAppState(ST_STOPPING);

	if(m_Cast.Stop())
		::LogClientStopping(m_Cast.GetConnectionID());
	else
		ASSERT(FALSE);
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

EnHandleResult CClientDlg::OnPrepareConnect(IUdpCast* pSender, CONNID dwConnID, SOCKET socket)
{
	return HR_OK;
}

EnHandleResult CClientDlg::OnConnect(IUdpCast* pSender, CONNID dwConnID)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	pSender->GetLocalAddress(szAddress, iAddressLen, usPort);

	::PostOnConnect(dwConnID, szAddress, usPort);
	SetAppState(ST_STARTED);

	return HR_OK;
}

EnHandleResult CClientDlg::OnSend(IUdpCast* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength);
	return HR_OK;
}

EnHandleResult CClientDlg::OnReceive(IUdpCast* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	(pSender)->GetRemoteAddress(szAddress, iAddressLen, usPort);

	::PostOnReceiveCast(dwConnID, szAddress, usPort, pData, iLength);
	return HR_OK;
}

EnHandleResult CClientDlg::OnClose(IUdpCast* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID)		:
	::PostOnError(dwConnID, enOperation, iErrorCode)	;

	SetAppState(ST_STOPPED);

	return HR_OK;
}
