
// NodeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Node.h"
#include "NodeDlg.h"
#include "afxdialogex.h"


// CNodeDlg dialog

#define DEFAULT_CONTENT		_T("text to be sent")
#define DEFAULT_DEST_ADDR	_T("127.0.0.1")
#define DEFAULT_CAST_ADDR	_T("233.0.0.1")
#define DEFAULT_BIND_ADDR	_T("0.0.0.0")
#define DEFAULT_DEST_PORT	_T("6666")
#define DEFAULT_PORT		_T("5555")
#define DEFAULT_TTL			_T("1")


CNodeDlg::CNodeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNodeDlg::IDD, pParent), m_Node(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONTENT, m_Content);
	DDX_Control(pDX, IDC_SEND, m_Send);
	DDX_Control(pDX, IDC_SEND_CAST, m_SendCast);
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_DEST_PORT, m_DestPort);
	DDX_Control(pDX, IDC_PORT, m_Port);
	DDX_Control(pDX, IDC_START, m_Start);
	DDX_Control(pDX, IDC_STOP, m_Stop);
	DDX_Control(pDX, IDC_DEST_ADDRESS, m_DestAddr);
	DDX_Control(pDX, IDC_CAST_ADDRESS, m_CastAddr);
	DDX_Control(pDX, IDC_BIND_ADDRESS, m_BindAddr);
	DDX_Control(pDX, IDC_TTL, m_TTL);
	DDX_Control(pDX, IDC_IP_LOOP, m_IPLoop);
	DDX_Control(pDX, IDC_REUSE_ADDR, m_ReuseAddr);
	DDX_Control(pDX, IDC_UNI_MODE, m_CastMode);
	DDX_Control(pDX, IDC_MULTI_MODE, m_CastMode2);
	DDX_Control(pDX, IDC_BROAD_MODE, m_CastMode3);
}

BEGIN_MESSAGE_MAP(CNodeDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_CONTENT, &CNodeDlg::OnEnChangeContent)
	ON_BN_CLICKED(IDC_SEND, &CNodeDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_SEND_CAST, &CNodeDlg::OnBnClickedSendCast)
	ON_BN_CLICKED(IDC_START, &CNodeDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CNodeDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_UNI_MODE, &CNodeDlg::OnBnClickedUniMode)
	ON_BN_CLICKED(IDC_MULTI_MODE, &CNodeDlg::OnBnClickedMultiMode)
	ON_BN_CLICKED(IDC_BROAD_MODE, &CNodeDlg::OnBnClickedBroadMode)
	ON_MESSAGE(USER_INFO_MSG, OnUserInfoMsg)
	ON_WM_VKEYTOITEM()
END_MESSAGE_MAP()


// CNodeDlg message handlers

BOOL CNodeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_Content.SetWindowText(DEFAULT_CONTENT);
	m_DestAddr.SetWindowText(DEFAULT_DEST_ADDR);
	m_CastAddr.SetWindowText(DEFAULT_CAST_ADDR);
	m_BindAddr.SetWindowText(DEFAULT_BIND_ADDR);
	m_DestPort.SetWindowText(DEFAULT_DEST_PORT);
	m_Port.SetWindowText(DEFAULT_PORT);
	m_TTL.SetWindowText(DEFAULT_TTL);
	m_CastMode.SetCheck(BST_CHECKED);
	m_IPLoop.SetCheck(BST_CHECKED);
	m_ReuseAddr.SetCheck(BST_CHECKED);

	::SetMainWnd(this);
	::SetInfoList(&m_Info);
	SetAppState(ST_STOPPED);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNodeDlg::OnPaint()
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
HCURSOR CNodeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CNodeDlg::PreTranslateMessage(MSG* pMsg)
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

void CNodeDlg::SetAppState(EnAppState state)
{
	m_enState = state;

	if(this->GetSafeHwnd() == nullptr)
		return;

	m_Start.EnableWindow(m_enState == ST_STOPPED);
	m_Stop.EnableWindow(m_enState == ST_STARTED);
	m_Send.EnableWindow(m_enState == ST_STARTED);
	m_SendCast.EnableWindow(m_enState == ST_STARTED && m_CastMode.GetCheck() != BST_CHECKED);
	m_CastAddr.EnableWindow(m_enState == ST_STOPPED && m_CastMode.GetCheck() != BST_CHECKED);
	m_BindAddr.EnableWindow(m_enState == ST_STOPPED);
	m_Port.EnableWindow(m_enState == ST_STOPPED);
	m_TTL.EnableWindow(m_enState == ST_STOPPED);
	m_CastMode.EnableWindow(m_enState == ST_STOPPED);
	m_CastMode2.EnableWindow(m_enState == ST_STOPPED);
	m_CastMode3.EnableWindow(m_enState == ST_STOPPED);
	m_IPLoop.EnableWindow(m_enState == ST_STOPPED);
	m_ReuseAddr.EnableWindow(m_enState == ST_STOPPED);
}

void CNodeDlg::OnEnChangeContent()
{
	m_Send.EnableWindow(m_enState == ST_STARTED);
	m_SendCast.EnableWindow(m_enState == ST_STARTED && m_CastMode.GetCheck() != BST_CHECKED);
}

void CNodeDlg::OnBnClickedUniMode()
{
	m_SendCast.EnableWindow(m_enState == ST_STARTED && m_CastMode.GetCheck() != BST_CHECKED);
	m_CastAddr.EnableWindow(m_enState == ST_STOPPED && m_CastMode.GetCheck() != BST_CHECKED);
}

void CNodeDlg::OnBnClickedMultiMode()
{
	m_SendCast.EnableWindow(m_enState == ST_STARTED && m_CastMode.GetCheck() != BST_CHECKED);
	m_CastAddr.EnableWindow(m_enState == ST_STOPPED && m_CastMode.GetCheck() != BST_CHECKED);
}

void CNodeDlg::OnBnClickedBroadMode()
{
	m_SendCast.EnableWindow(m_enState == ST_STARTED && m_CastMode.GetCheck() != BST_CHECKED);
	m_CastAddr.EnableWindow(m_enState == ST_STOPPED && m_CastMode.GetCheck() != BST_CHECKED);
}

void CNodeDlg::OnBnClickedSend()
{
	USES_CONVERSION;

	CString strAddress;
	CString strPort;

	m_DestAddr.GetWindowText(strAddress);
	m_DestPort.GetWindowText(strPort);

	USHORT usPort = (USHORT)_ttoi(strPort);

	CString strContent;
	m_Content.GetWindowText(strContent);

	LPSTR lpszContent = T2A((LPTSTR)(LPCTSTR)strContent);
	int iLen = (int)strlen(lpszContent);

	if(m_Node.Send(strAddress, usPort, (LPBYTE)lpszContent, iLen))
		::LogSend(strContent);
	else
		::LogSendFail(::GetLastError(), ::GetSocketErrorDesc(SE_DATA_SEND));
}


void CNodeDlg::OnBnClickedSendCast()
{
	USES_CONVERSION;

	CString strContent;
	m_Content.GetWindowText(strContent);

	LPSTR lpszContent = T2A((LPTSTR)(LPCTSTR)strContent);
	int iLen = (int)strlen(lpszContent);

	if(m_Node.SendCast((LPBYTE)lpszContent, iLen))
		::LogSend(strContent);
	else
		::LogSendFail(::GetLastError(), ::GetSocketErrorDesc(SE_DATA_SEND));
}

void CNodeDlg::OnBnClickedStart()
{
	SetAppState(ST_STARTING);


	CString strDestAddress;
	CString strCastAddress;
	CString strBindAddress;
	CString strDestPort;
	CString strPort;
	CString strTTL;

	m_DestAddr.GetWindowText(strDestAddress);
	m_CastAddr.GetWindowText(strCastAddress);
	m_BindAddr.GetWindowText(strBindAddress);
	m_DestPort.GetWindowText(strDestPort);
	m_Port.GetWindowText(strPort);
	m_TTL.GetWindowText(strTTL);

	USHORT usDestPort	= (USHORT)_ttoi(strDestPort);
	USHORT usPort		= (USHORT)_ttoi(strPort);
	int iTTL			= _ttoi(strTTL);
	EnCastMode enMode	= (m_CastMode.GetCheck() == BST_CHECKED) ? CM_UNICAST : (m_CastMode2.GetCheck() == BST_CHECKED) ? CM_MULTICAST : CM_BROADCAST;
	BOOL bIPLoop		= (m_IPLoop.GetCheck() == BST_CHECKED) ? TRUE : FALSE;
	BOOL bReuseAddr		= (m_ReuseAddr.GetCheck() == BST_CHECKED) ? TRUE : FALSE;

	// OPTIONS ...
	m_Node.SetReuseAddressPolicy(bReuseAddr ? RAP_ADDR_AND_PORT : RAP_NONE);
	m_Node.SetMultiCastLoop(bIPLoop);
	m_Node.SetMultiCastTtl(iTTL);

	::LogStarting(strBindAddress, usPort);

	if(m_Node.Start(strBindAddress, usPort, enMode, strCastAddress))
	{
		SetAppState(ST_STARTED);
	}
	else
	{
		::LogStartFail(m_Node.GetLastError(), m_Node.GetLastErrorDesc());
		SetAppState(ST_STOPPED);
	}
}


void CNodeDlg::OnBnClickedStop()
{
	SetAppState(ST_STOPPING);

	if(m_Node.Stop())
		::LogStopping();
	else
		ASSERT(FALSE);
}

int CNodeDlg::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex)
{
	if(nKey == 'C')
		pListBox->ResetContent();

	return __super::OnVKeyToItem(nKey, pListBox, nIndex);
}

LRESULT CNodeDlg::OnUserInfoMsg(WPARAM wp, LPARAM lp)
{
	info_msg* msg = (info_msg*)wp;

	::LogInfoMsg(msg);

	return 0;
}

EnHandleResult CNodeDlg::OnPrepareListen(IUdpNode* pSender, SOCKET soListen)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	pSender->GetLocalAddress(szAddress, iAddressLen, usPort);

	::PostOnPrepareListen(szAddress, usPort);

	return HR_OK;
}

EnHandleResult CNodeDlg::OnSend(IUdpNode* pSender, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pData, int iLength)
{
	::PostOnSendTo(0, lpszRemoteAddress, usRemotePort, pData, iLength);
	return HR_IGNORE;
}

EnHandleResult CNodeDlg::OnReceive(IUdpNode* pSender, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pData, int iLength)
{
	::PostOnReceiveFrom(0, lpszRemoteAddress, usRemotePort, pData, iLength);

	//pSender->Send(lpszRemoteAddress, usRemotePort, pData, iLength);

	return HR_IGNORE;
}

EnHandleResult CNodeDlg::OnError(IUdpNode* pSender, EnSocketOperation enOperation, int iErrorCode, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pBuffer, int iLength)
{
	::PostOnError2(0, enOperation, iErrorCode, lpszRemoteAddress, usRemotePort, pBuffer, iLength);
	return HR_IGNORE;
}

EnHandleResult CNodeDlg::OnShutdown(IUdpNode* pSender)
{
	::PostOnShutdown();
	SetAppState(ST_STOPPED);

	return HR_IGNORE;
}
