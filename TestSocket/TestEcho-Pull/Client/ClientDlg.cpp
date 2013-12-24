
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


CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent), m_Client(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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
	SetAppState(ST_STOPED);

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

	m_Async.EnableWindow(m_enState == ST_STOPED);
	m_Start.EnableWindow(m_enState == ST_STOPED);
	m_Stop.EnableWindow(m_enState == ST_STARTED);
	m_Send.EnableWindow(m_enState == ST_STARTED);
	m_Address.EnableWindow(m_enState == ST_STOPED);
	m_Port.EnableWindow(m_enState == ST_STOPED);
}

void CClientDlg::OnBnClickedSend()
{
	USES_CONVERSION;

	static DWORD SEQ = 0;

	CString strContent;
	m_Content.GetWindowText(strContent);

	smart_simple_ptr<CBufferPtr> buffer = ::GeneratePkgBuffer(++SEQ, _T("ÉËÉñÐ¡¹ÖÊÞ"), 23, strContent);

	if(m_Client->Send(m_Client->GetConnectionID(), buffer->Ptr(), (int)buffer->Size()))
		::LogSend(m_Client->GetConnectionID(), strContent);
	else
		::LogSendFail(m_Client->GetConnectionID(), m_Client->GetLastError(), m_Client->GetLastErrorDesc());
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
	//m_Client->SetSocketBufferSize(5);
	if(m_Client->Start(strAddress, usPort, m_bAsyncConn))
	{

	}
	else
	{
		::LogClientStartFail(m_Client->GetLastError(), m_Client->GetLastErrorDesc());
		SetAppState(ST_STOPED);
	}
}


void CClientDlg::OnBnClickedStop()
{
	SetAppState(ST_STOPING);

	if(m_Client->Stop())
		::LogClientStopping(m_Client->GetConnectionID());
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

ISocketListener::EnHandleResult CClientDlg::OnConnect(CONNID dwConnID)
{
	TCHAR szAddress[40];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	m_Client->GetLocalAddress(szAddress, iAddressLen, usPort);

	::PostOnConnect(dwConnID, szAddress, usPort);
	SetAppState(ST_STARTED);

	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CClientDlg::OnSend(CONNID dwConnID, const BYTE* pData, int iLength)
{
	//static int t = 0;
	//if(++t % 3 == 0) return ISocketListener::HR_ERROR;

	::PostOnSend(dwConnID, pData, iLength);
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CClientDlg::OnReceive(CONNID dwConnID, int iLength)
{
	int required = m_pkgInfo.length;
	int remain = iLength;

	while(remain >= required)
	{
		remain -= required;
		CBufferPtr buffer(required);

		IPullSocket::EnFetchResult result = m_Client->Fetch(dwConnID, buffer, (int)buffer.Size());
		if(result == IPullSocket::FR_OK)
		{
			if(m_pkgInfo.is_header)
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

			m_pkgInfo.is_header	= !m_pkgInfo.is_header;
			m_pkgInfo.length	= required;

			::PostOnReceive(dwConnID, buffer, (int)buffer.Size());
		}
	}

	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CClientDlg::OnClose(CONNID dwConnID)
{
	::PostOnClose(dwConnID);
	SetAppState(ST_STOPED);
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CClientDlg::OnError(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	::PostOnError(dwConnID, enOperation, iErrorCode);
	SetAppState(ST_STOPED);
	return ISocketListener::HR_OK;
}
