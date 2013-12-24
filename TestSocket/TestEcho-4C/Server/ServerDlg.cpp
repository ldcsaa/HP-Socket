
// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"


// CServerDlg dialog

const LPCTSTR CServerDlg::ADDRESS = _T("0.0.0.0");
const USHORT CServerDlg::PORT = 5555;


CServerDlg* CServerDlg::m_spThis					= nullptr;
HP_TcpPullServer CServerDlg::m_spServer				= nullptr;
HP_TcpPullServerListener CServerDlg::m_spListener	= nullptr;

CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_spThis		= this;
	// 创建监听器对象
	m_spListener	= ::Create_HP_TcpPullServerListener();
	// 创建 Socket 对象
	m_spServer		= ::Create_HP_TcpPullServer(m_spListener);
	// 设置 Socket 监听器回调函数
	::HP_Set_FN_Server_OnPrepareListen(m_spListener, OnPrepareListen);
	::HP_Set_FN_Server_OnAccept(m_spListener, OnAccept);
	::HP_Set_FN_Server_OnSend(m_spListener, OnSend);
	::HP_Set_FN_Server_OnPullReceive(m_spListener, OnReceive);
	::HP_Set_FN_Server_OnClose(m_spListener, OnClose);
	::HP_Set_FN_Server_OnError(m_spListener, OnError);
	::HP_Set_FN_Server_OnServerShutdown(m_spListener, OnServerShutdown);
}

CServerDlg::~CServerDlg()
{
	// 销毁 Socket 对象
	::Destroy_HP_TcpPullServer(m_spServer);
	// 销毁监听器对象
	::Destroy_HP_TcpPullServerListener(m_spListener);
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
	if(::HP_Server_Start(m_spServer, ADDRESS, PORT))
	{
		::LogServerStart(ADDRESS, PORT);
		SetAppState(ST_STARTED);
	}
	else
	{
		::LogServerStartFail(::HP_Server_GetLastError(m_spServer), ::HP_Server_GetLastErrorDesc(m_spServer));
		SetAppState(ST_STOPED);
	}
}

void CServerDlg::OnBnClickedStop()
{
	SetAppState(ST_STOPING);

	if(::HP_Server_Stop(m_spServer))
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

	if(::HP_Server_Disconnect(m_spServer, dwConnID, TRUE))
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

En_HP_HandleResult CServerDlg::OnPrepareListen(SOCKET soListen)
{
	TCHAR szAddress[40];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;
	
	::HP_Server_GetListenAddress(m_spServer, szAddress, &iAddressLen, &usPort);
	::PostOnPrepareListen(szAddress, usPort);
	return HP_HR_OK;
}

En_HP_HandleResult CServerDlg::OnAccept(CONNID dwConnID, SOCKET soClient)
{
	BOOL bPass = TRUE;
	TCHAR szAddress[40];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	::HP_Server_GetClientAddress(m_spServer, dwConnID, szAddress, &iAddressLen, &usPort);

	if(!m_spThis->m_strAddress.IsEmpty())
	{
		if(m_spThis->m_strAddress.CompareNoCase(szAddress) == 0)
			bPass = FALSE;
	}

	::PostOnAccept(dwConnID, szAddress, usPort, bPass);

	//if(bPass) m_mpPkgInfo[dwConnID] = new TPkgInfo(true, sizeof(TPkgHeader));
	if(bPass) ::HP_Server_SetConnectionExtra(m_spServer, dwConnID, new TPkgInfo(true, sizeof(TPkgHeader)));

	return bPass ? HP_HR_OK : HP_HR_ERROR;
}

En_HP_HandleResult CServerDlg::OnSend(CONNID dwConnID, const BYTE* pData, int iLength)
{
	//static int t = 0;
	//if(++t % 3 == 0) return ISocketListener::HR_ERROR;

	::PostOnSend(dwConnID, pData, iLength);
	return HP_HR_OK;
}

En_HP_HandleResult CServerDlg::OnReceive(CONNID dwConnID, int iLength)
{
	TPkgInfo* pInfo = m_spThis->FindPkgInfo(dwConnID);

	if(pInfo != nullptr)
	{
		int required = pInfo->length;
		int remain = iLength;

		while(remain >= required)
		{
			remain -= required;
			CBufferPtr buffer(required);

			En_HP_FetchResult result = ::HP_TcpPullServer_Fetch(m_spServer, dwConnID, buffer, (int)buffer.Size());
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

				if(!::HP_Server_Send(m_spServer, dwConnID, buffer, (int)buffer.Size()))
					return HP_HR_ERROR;
			}
		}
	}
	
	return HP_HR_OK;
}

En_HP_HandleResult CServerDlg::OnClose(CONNID dwConnID)
{
	::PostOnClose(dwConnID);
	m_spThis->RemovePkgInfo(dwConnID);

	return HP_HR_OK;
}

En_HP_HandleResult CServerDlg::OnError(CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode)
{
	::PostOnError(dwConnID, enOperation, iErrorCode);
	m_spThis->RemovePkgInfo(dwConnID);

	return HP_HR_OK;
}

En_HP_HandleResult CServerDlg::OnServerShutdown()
{
	::PostOnShutdown();
	//::ClearPtrMap(m_mpPkgInfo);

	return HP_HR_OK;
}

TPkgInfo* CServerDlg::FindPkgInfo(CONNID dwConnID)
{
	PVOID pInfo = nullptr;

	::HP_Server_GetConnectionExtra(m_spServer, dwConnID, &pInfo);

	/*
	auto it = m_mpPkgInfo.find(dwConnID);

	if(it != m_mpPkgInfo.end())
		pInfo = it->second;
	*/

	return (TPkgInfo*)pInfo;
}

void CServerDlg::RemovePkgInfo(CONNID dwConnID)
{
	CCriSecLock locallock(m_spThis->m_csPkgInfo);

	PVOID pInfo = nullptr;

	if(::HP_Server_GetConnectionExtra(m_spServer, dwConnID, &pInfo) && pInfo != nullptr)
	{
		::HP_Server_SetConnectionExtra(m_spServer, dwConnID, nullptr);
		delete (TPkgInfo*)pInfo;
	}

	/*
	TPkgInfo* pInfo = FindPkgInfo(dwConnID);

	if(pInfo != nullptr)
	{
		m_mpPkgInfo.erase(dwConnID);
		delete pInfo;
	}
	*/
}
