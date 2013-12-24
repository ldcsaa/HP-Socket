
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
	GetWindowText(strTitle);
	strTitle.Format(_T("%s - (%s:%d)"), strTitle, ADDRESS, PORT);
	SetWindowText(strTitle);

	::SetMainWnd(this);
	::SetInfoList(&m_Info);
	SetAppState(ST_STOPED);

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

	m_Start.EnableWindow(m_enState == ST_STOPED);
	m_Stop.EnableWindow(m_enState == ST_STARTED);
	m_Address.EnableWindow(m_enState == ST_STOPED);
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

ISocketListener::EnHandleResult CServerDlg::OnPrepareListen(SOCKET soListen)
{
	TCHAR szAddress[40];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;
	
	m_Server->GetListenAddress(szAddress, iAddressLen, usPort);
	::PostOnPrepareListen(szAddress, usPort);
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CServerDlg::OnAccept(CONNID dwConnID, SOCKET soClient)
{
	BOOL bPass = TRUE;
	TCHAR szAddress[40];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	m_Server->GetClientAddress(dwConnID, szAddress, iAddressLen, usPort);

	if(!m_strAddress.IsEmpty())
	{
		if(m_strAddress.CompareNoCase(szAddress) == 0)
			bPass = FALSE;
	}

	::PostOnAccept(dwConnID, szAddress, usPort, bPass);

	if(bPass) m_Server->SetConnectionExtra(dwConnID, new TPkgInfo(true, sizeof(TPkgHeader)));

	return bPass ? ISocketListener::HR_OK : ISocketListener::HR_ERROR;
}

ISocketListener::EnHandleResult CServerDlg::OnSend(CONNID dwConnID, const BYTE* pData, int iLength)
{
	//static int t = 0;
	//if(++t % 3 == 0) return ISocketListener::HR_ERROR;

	::PostOnSend(dwConnID, pData, iLength);
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CServerDlg::OnReceive(CONNID dwConnID, int iLength)
{
	TPkgInfo* pInfo = FindPkgInfo(dwConnID);

	if(pInfo != nullptr)
	{
		int required = pInfo->length;
		int remain = iLength;

		while(remain >= required)
		{
			remain -= required;
			CBufferPtr buffer(required);

			IPullSocket::EnFetchResult result = m_Server->Fetch(dwConnID, buffer, (int)buffer.Size());
			if(result == IPullSocket::FR_OK)
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

				if(!m_Server->Send(dwConnID, buffer, (int)buffer.Size()))
					return ISocketListener::HR_ERROR;
			}
		}
	}
	
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CServerDlg::OnClose(CONNID dwConnID)
{
	::PostOnClose(dwConnID);
	RemovePkgInfo(dwConnID);

	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CServerDlg::OnError(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	::PostOnError(dwConnID, enOperation, iErrorCode);
	RemovePkgInfo(dwConnID);

	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CServerDlg::OnServerShutdown()
{
	::PostOnShutdown();

	return ISocketListener::HR_OK;
}

TPkgInfo* CServerDlg::FindPkgInfo(CONNID dwConnID)
{
	PVOID pInfo = nullptr;

	m_Server->GetConnectionExtra(dwConnID, &pInfo);

	return (TPkgInfo*)pInfo;
}

void CServerDlg::RemovePkgInfo(CONNID dwConnID)
{
	//CCriSecLock locallock(m_csPkgInfo);

	TPkgInfo* pInfo = FindPkgInfo(dwConnID);
	ASSERT(pInfo != nullptr);

	delete pInfo;
}
