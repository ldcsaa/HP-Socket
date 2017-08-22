
// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"


// CClientDlg dialog

#define DEFAULT_CONTENT	_T("text to be sent")
#define DEFAULT_ADDRESS	_T("127.0.0.1")
#define DEFAULT_PORT	_T("5555")

CClientDlg* CClientDlg::m_spThis = nullptr;

CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_spThis = this;

	// 创建监听器对象
	m_pListener	= ::Create_HP_TcpPullClientListener();
	// 创建 Socket 对象
	m_pClient	= ::Create_HP_TcpPullClient(m_pListener);
	// 设置 Socket 监听器回调函数
	::HP_Set_FN_Client_OnConnect(m_pListener, OnConnect);
	::HP_Set_FN_Client_OnSend(m_pListener, OnSend);
	::HP_Set_FN_Client_OnPullReceive(m_pListener, OnReceive);
	::HP_Set_FN_Client_OnClose(m_pListener, OnClose);
}

CClientDlg::~CClientDlg()
{
	// 销毁 Socket 对象
	::Destroy_HP_TcpPullClient(m_pClient);
	// 销毁监听器对象
	::Destroy_HP_TcpPullClientListener(m_pListener);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONTENT, m_Content);
	DDX_Control(pDX, IDC_SEND, m_Send);
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_ADDRESS, m_Address);
	DDX_Control(pDX, IDC_PORT, m_Port);
	DDX_Control(pDX, IDC_ASYNC, m_Async);
	DDX_Control(pDX, IDC_START, m_Start);
	DDX_Control(pDX, IDC_STOP, m_Stop);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, &CClientDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_START, &CClientDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CClientDlg::OnBnClickedStop)
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

	m_Content.SetWindowText(DEFAULT_CONTENT);
	m_Address.SetWindowText(DEFAULT_ADDRESS);
	m_Port.SetWindowText(DEFAULT_PORT);
	m_Async.SetCheck(BST_CHECKED);

	::SetMainWnd(this);
	::SetInfoList(&m_Info);
	SetAppState(ST_STOPPED);

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

	m_Async.EnableWindow(m_enState == ST_STOPPED);
	m_Start.EnableWindow(m_enState == ST_STOPPED);
	m_Stop.EnableWindow(m_enState == ST_STARTED);
	m_Send.EnableWindow(m_enState == ST_STARTED);
	m_Address.EnableWindow(m_enState == ST_STOPPED);
	m_Port.EnableWindow(m_enState == ST_STOPPED);
}

void CClientDlg::OnBnClickedSend()
{
	USES_CONVERSION;

	static DWORD SEQ = 0;

	CString strContent;
	m_Content.GetWindowText(strContent);

	smart_simple_ptr<CBufferPtr> buffer = ::GeneratePkgBuffer(++SEQ, _T("伤神小怪兽"), 23, strContent);

	if(::HP_Client_Send(m_pClient, buffer->Ptr(), (int)buffer->Size()))
		::LogSend(::HP_Client_GetConnectionID(m_pClient), strContent);
	else
		::LogSendFail(::HP_Client_GetConnectionID(m_pClient), ::SYS_GetLastError(), ::HP_GetSocketErrorDesc(SE_DATA_SEND));
}

void CClientDlg::OnBnClickedStart()
{
	SetAppState(ST_STARTING);

	CString strAddress;
	CString strPort;

	m_Address.GetWindowText(strAddress);
	m_Port.GetWindowText(strPort);

	USHORT usPort	= (USHORT)_ttoi(strPort);
	m_bAsyncConn	= m_Async.GetCheck();

	m_pkgInfo.Reset();

	::LogClientStarting(strAddress, usPort);

	if(::HP_Client_Start(m_pClient, strAddress, usPort, m_bAsyncConn))
	{

	}
	else
	{
		::LogClientStartFail(::HP_Client_GetLastError(m_pClient), HP_Client_GetLastErrorDesc(m_pClient));
		SetAppState(ST_STOPPED);
	}
}

void CClientDlg::OnBnClickedStop()
{
	SetAppState(ST_STOPPING);

	if(::HP_Client_Stop(m_pClient))
		::LogClientStopping(::HP_Client_GetConnectionID(m_pClient));
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

En_HP_HandleResult CClientDlg::OnConnect(HP_Client pSender, HP_CONNID dwConnID)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	::HP_Client_GetLocalAddress(pSender, szAddress, &iAddressLen, &usPort);

	::PostOnConnect(dwConnID, szAddress, usPort);
	m_spThis->SetAppState(ST_STARTED);

	return HR_OK;
}

En_HP_HandleResult CClientDlg::OnSend(HP_Client pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength);
	return HR_OK;
}

En_HP_HandleResult CClientDlg::OnReceive(HP_Client pSender, HP_CONNID dwConnID, int iLength)
{
	int required = m_spThis->m_pkgInfo.length;
	int remain = iLength;

	while(remain >= required)
	{
		remain -= required;
		CBufferPtr buffer(required);

		En_HP_FetchResult result = ::HP_TcpPullClient_Fetch(pSender, buffer, (int)buffer.Size());
		if(result == FR_OK)
		{
			if(m_spThis->m_pkgInfo.is_header)
			{
				TPkgHeader* pHeader = (TPkgHeader*)buffer.Ptr();
				TRACE("[Client] head -> seq: %d, body_len: %d\n", pHeader->seq, pHeader->body_len);

				required = pHeader->body_len;
			}
			else
			{
				TPkgBody* pBody = (TPkgBody*)buffer.Ptr();
				TRACE("[Client] body -> name: %s, age: %d, desc: %s\n", pBody->name, pBody->age, pBody->desc);

				required = sizeof(TPkgHeader);
			}

			m_spThis->m_pkgInfo.is_header	= !m_spThis->m_pkgInfo.is_header;
			m_spThis->m_pkgInfo.length		= required;

			::PostOnReceive(dwConnID, buffer, (int)buffer.Size());
		}
	}

	return HR_OK;
}

En_HP_HandleResult CClientDlg::OnClose(HP_Client pSender, HP_CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID):
	::PostOnError(dwConnID, enOperation, iErrorCode);

	m_spThis->SetAppState(ST_STOPPED);

	return HR_OK;
}
