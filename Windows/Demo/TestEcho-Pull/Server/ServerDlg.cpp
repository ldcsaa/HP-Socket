
// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"


// CServerDlg dialog

const LPCTSTR CServerDlg::ADDRESS	= _T("0.0.0.0");
const USHORT CServerDlg::PORT		= 5555;

CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServerDlg::IDD, pParent), m_Server(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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
	//m_Server->SetSocketBufferSize(64);
	if(m_Server->Start(ADDRESS, PORT))
	{
		::LogServerStart(ADDRESS, PORT);
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

void CServerDlg::OnBnClickedDisconnect()
{
	CString strConnID;
	m_ConnID.GetWindowText(strConnID);
	CONNID dwConnID = (CONNID)_ttoi(strConnID);

	if(m_Server->Disconnect(dwConnID))
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

EnHandleResult CServerDlg::OnPrepareListen(ITcpServer* pSender, SOCKET soListen)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;
	
	pSender->GetListenAddress(szAddress, iAddressLen, usPort);
	::PostOnPrepareListen(szAddress, usPort);
	return HR_OK;
}

EnHandleResult CServerDlg::OnAccept(ITcpServer* pSender, CONNID dwConnID, SOCKET soClient)
{
	BOOL bPass = TRUE;
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	pSender->GetRemoteAddress(dwConnID, szAddress, iAddressLen, usPort);

	if(!m_strAddress.IsEmpty())
	{
		if(m_strAddress.CompareNoCase(szAddress) == 0)
			bPass = FALSE;
	}

	::PostOnAccept(dwConnID, szAddress, usPort, bPass);

	if(bPass) pSender->SetConnectionExtra(dwConnID, new TPkgInfo(true, sizeof(TPkgHeader)));

	return bPass ? HR_OK : HR_ERROR;
}

EnHandleResult CServerDlg::OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength);
	return HR_OK;
}

EnHandleResult CServerDlg::OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength)
{
	TPkgInfo* pInfo			= FindPkgInfo(pSender, dwConnID);
	ITcpPullServer* pServer	= ITcpPullServer::FromS(pSender);

	if(pInfo != nullptr)
	{
		int required = pInfo->length;
		int remain = iLength;

		while(remain >= required)
		{
			remain -= required;
			CBufferPtr buffer(required);

			EnFetchResult result = pServer->Fetch(dwConnID, buffer, (int)buffer.Size());
			if(result == FR_OK)
			{
				if(pInfo->is_header)
				{
					TPkgHeader* pHeader = (TPkgHeader*)buffer.Ptr();
					TRACE("[Server] head -> seq: %d, body_len: %d\n", pHeader->seq, pHeader->body_len);

					required = pHeader->body_len;
				}
				else
				{
					TPkgBody* pBody = (TPkgBody*)(BYTE*)buffer;
					TRACE("[Server] body -> name: %s, age: %d, desc: %s\n", pBody->name, pBody->age, pBody->desc);

					required = sizeof(TPkgHeader);
				}

				pInfo->is_header = !pInfo->is_header;
				pInfo->length	 = required;

				::PostOnReceive(dwConnID, buffer, (int)buffer.Size());

				if(!pSender->Send(dwConnID, buffer, (int)buffer.Size()))
					return HR_ERROR;
			}
		}
	}
	
	return HR_OK;
}

EnHandleResult CServerDlg::OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID)	:
	::PostOnError(dwConnID, enOperation, iErrorCode);

	RemovePkgInfo(pSender, dwConnID);

	return HR_OK;
}

EnHandleResult CServerDlg::OnShutdown(ITcpServer* pSender)
{
	::PostOnShutdown();

	return HR_OK;
}

TPkgInfo* CServerDlg::FindPkgInfo(ITcpServer* pSender, CONNID dwConnID)
{
	PVOID pInfo = nullptr;

	pSender->GetConnectionExtra(dwConnID, &pInfo);

	return (TPkgInfo*)pInfo;
}

void CServerDlg::RemovePkgInfo(ITcpServer* pSender, CONNID dwConnID)
{
	TPkgInfo* pInfo = FindPkgInfo(pSender, dwConnID);
	ASSERT(pInfo != nullptr);

	delete pInfo;
}
