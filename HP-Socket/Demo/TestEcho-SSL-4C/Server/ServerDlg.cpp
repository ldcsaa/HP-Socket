
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

	// 初始化 SSL 全局环境参数
	::HP_SSL_Initialize(HP_SSL_SM_SERVER, g_s_iVerifyMode, g_s_lpszPemCertFile, g_s_lpszPemKeyFile, g_s_lpszKeyPasswod, g_s_lpszCAPemCertFileOrPath);
	VERIFY(::HP_SSL_IsValid());

	// 创建监听器对象
	m_pListener	= ::Create_HP_TcpPullServerListener();
	// 创建 Socket 对象
	m_pServer		= ::Create_HP_SSLPullServer(m_pListener);
	// 设置 Socket 监听器回调函数
	::HP_Set_FN_Server_OnPrepareListen(m_pListener, OnPrepareListen);
	::HP_Set_FN_Server_OnAccept(m_pListener, OnAccept);
	::HP_Set_FN_Server_OnHandShake(m_pListener, OnHandShake);
	::HP_Set_FN_Server_OnSend(m_pListener, OnSend);
	::HP_Set_FN_Server_OnPullReceive(m_pListener, OnReceive);
	::HP_Set_FN_Server_OnClose(m_pListener, OnClose);
	::HP_Set_FN_Server_OnShutdown(m_pListener, OnShutdown);
}

CServerDlg::~CServerDlg()
{
	// 销毁 Socket 对象
	::Destroy_HP_SSLPullServer(m_spThis->m_pServer);
	// 销毁监听器对象
	::Destroy_HP_TcpPullServerListener(m_pListener);

	// 清理 SSL 全局运行环境
	::HP_SSL_Cleanup();
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_START, m_Start);
	DDX_Control(pDX, IDC_STOP, m_Stop);
	DDX_Control(pDX, IDC_ADDRESS, m_Address);
	DDX_Control(pDX, IDC_CONN_ID, m_HP_CONNID);
	DDX_Control(pDX, IDC_DISCONNECT, m_DisConn);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CServerDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CServerDlg::OnBnClickedStop)
	ON_MESSAGE(USER_INFO_MSG, OnUserInfoMsg)
	ON_BN_CLICKED(IDC_DISCONNECT, &CServerDlg::OnBnClickedDisconnect)
	ON_EN_CHANGE(IDC_CONN_ID, &CServerDlg::OnEnChangeHP_CONNID)
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
	m_DisConn.EnableWindow(m_enState == ST_STARTED && m_HP_CONNID.GetWindowTextLength() > 0);
}

void CServerDlg::OnBnClickedStart()
{
	m_Address.GetWindowText(m_strAddress);
	m_strAddress.Trim();

	SetAppState(ST_STARTING);
	//m_Server->SetSocketBufferSize(64);
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
	CString strHP_CONNID;
	m_HP_CONNID.GetWindowText(strHP_CONNID);
	HP_CONNID dwHP_CONNID = (HP_CONNID)_ttoi(strHP_CONNID);

	if(::HP_Server_Disconnect(m_spThis->m_pServer, dwHP_CONNID, TRUE))
		::LogDisconnect(dwHP_CONNID);
	else
		::LogDisconnectFail(dwHP_CONNID);
}

void CServerDlg::OnEnChangeHP_CONNID()
{
	m_DisConn.EnableWindow(m_enState == ST_STARTED && m_HP_CONNID.GetWindowTextLength() > 0);
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
	
	::HP_Server_GetListenAddress(m_spThis->m_pServer, szAddress, &iAddressLen, &usPort);
	::PostOnPrepareListen(szAddress, usPort);
	return HP_HR_OK;
}

En_HP_HandleResult CServerDlg::OnAccept(HP_CONNID dwHP_CONNID, SOCKET soClient)
{
	BOOL bPass = TRUE;
	TCHAR szAddress[40];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	::HP_Server_GetRemoteAddress(m_spThis->m_pServer, dwHP_CONNID, szAddress, &iAddressLen, &usPort);

	if(!m_spThis->m_strAddress.IsEmpty())
	{
		if(m_spThis->m_strAddress.CompareNoCase(szAddress) == 0)
			bPass = FALSE;
	}

	::PostOnAccept(dwHP_CONNID, szAddress, usPort, bPass);

	if(bPass) ::HP_Server_SetConnectionExtra(m_spThis->m_pServer, dwHP_CONNID, new TPkgInfo(true, sizeof(TPkgHeader)));

	return bPass ? HP_HR_OK : HP_HR_ERROR;
}

En_HP_HandleResult CServerDlg::OnHandShake(HP_CONNID dwConnID)
{
	::PostOnHandShake(dwConnID);
	return HP_HR_OK;
}

En_HP_HandleResult CServerDlg::OnSend(HP_CONNID dwHP_CONNID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwHP_CONNID, pData, iLength);
	return HP_HR_OK;
}

En_HP_HandleResult CServerDlg::OnReceive(HP_CONNID dwHP_CONNID, int iLength)
{
	TPkgInfo* pInfo = m_spThis->FindPkgInfo(dwHP_CONNID);

	if(pInfo != nullptr)
	{
		int required = pInfo->length;
		int remain = iLength;

		while(remain >= required)
		{
			remain -= required;
			CBufferPtr buffer(required);

			En_HP_FetchResult result = ::HP_TcpPullServer_Fetch(m_spThis->m_pServer, dwHP_CONNID, buffer, (int)buffer.Size());
			if(result == HP_FR_OK)
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

				::PostOnReceive(dwHP_CONNID, buffer, (int)buffer.Size());

				if(!::HP_Server_Send(m_spThis->m_pServer, dwHP_CONNID, buffer, (int)buffer.Size()))
					return HP_HR_ERROR;
			}
		}
	}
	
	return HP_HR_OK;
}

En_HP_HandleResult CServerDlg::OnClose(HP_CONNID dwHP_CONNID, En_HP_SocketOperation enOperation, int iErrorCode)
{
	iErrorCode == HP_SE_OK ? ::PostOnClose(dwHP_CONNID):
	::PostOnError(dwHP_CONNID, enOperation, iErrorCode);

	m_spThis->RemovePkgInfo(dwHP_CONNID);

	return HP_HR_OK;
}

En_HP_HandleResult CServerDlg::OnShutdown()
{
	::PostOnShutdown();

	return HP_HR_OK;
}

TPkgInfo* CServerDlg::FindPkgInfo(HP_CONNID dwHP_CONNID)
{
	PVOID pInfo = nullptr;
	::HP_Server_GetConnectionExtra(m_spThis->m_pServer, dwHP_CONNID, &pInfo);

	return (TPkgInfo*)pInfo;
}

void CServerDlg::RemovePkgInfo(HP_CONNID dwHP_CONNID)
{
	//CCriSecLock locallock(m_spThis->m_csPkgInfo);

	TPkgInfo* pInfo = FindPkgInfo(dwHP_CONNID);
	ASSERT(pInfo != nullptr);

	delete pInfo;
}
