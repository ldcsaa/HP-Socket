
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
#define LOCAL_ADDRESS	_T("0.0.0.0")

// link: ..\..\Bin\HPSocket\x86\HPSocket_UD.lib

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

CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent), m_Agent(this)
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
	DDX_Control(pDX, IDC_START, m_Start);
	DDX_Control(pDX, IDC_CONNECT, m_Connect);
	DDX_Control(pDX, IDC_STOP, m_Stop);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, &CClientDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_START, &CClientDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_CONNECT, &CClientDlg::OnBnClickedConnect)
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

	m_Connect.SetWindowText(m_enState == ST_CONNECTED ? _T("Dis Connect") : _T("Connect"));

	m_Start.EnableWindow(m_enState == ST_STOPPED);
	m_Connect.EnableWindow(m_enState == ST_STARTED || m_enState == ST_CONNECTED);
	m_Stop.EnableWindow(m_enState == ST_STARTED || m_enState == ST_CONNECTED);
	m_Send.EnableWindow(m_enState == ST_CONNECTED);
	m_Address.EnableWindow(m_enState == ST_STOPPED);
	m_Port.EnableWindow(m_enState == ST_STOPPED);
}

void CClientDlg::OnBnClickedSend()
{
	USES_CONVERSION;

	static DWORD SEQ = 0;

	CString strContent;
	m_Content.GetWindowText(strContent);

	smart_simple_ptr<CBufferPtr> buffer = ::GeneratePkgBuffer(++SEQ, _T("ÉËÉñÐ¡¹ÖÊÞ"), 23, strContent);

	if(m_Agent->Send(m_dwConnID, buffer->Ptr(), (int)buffer->Size()))
		::LogSend(m_dwConnID, strContent);
	else
		::LogSendFail(m_dwConnID, SE_DATA_SEND, ::HP_GetSocketErrorDesc(SE_DATA_SEND));
}


void CClientDlg::OnBnClickedStart()
{
	SetAppState(ST_STARTING);

	CString strPort;

	m_Address.GetWindowText(m_strAddress);
	m_Port.GetWindowText(strPort);

	m_usPort	= (USHORT)_ttoi(strPort);
	m_dwConnID	= 0;

	m_pkgInfo.Reset();

	::LogClientStarting(m_strAddress, m_usPort);
	
	if(m_Agent->Start(LOCAL_ADDRESS, TRUE))
	{
		SetAppState(ST_STARTED);
	}
	else
	{
		::LogClientStartFail(m_Agent->GetLastError(), m_Agent->GetLastErrorDesc());
		SetAppState(ST_STOPPED);
	}
}

void CClientDlg::OnBnClickedConnect()
{
	EnAppState enState = m_enState;

	SetAppState(ST_CONNECTING);

	if(enState != ST_CONNECTED)
	{
		if(m_Agent->Connect(m_strAddress, m_usPort, &m_dwConnID))
		{
		}
		else
		{
			::LogClientStartFail(::SYS_GetLastError(), ::HP_GetSocketErrorDesc(SE_CONNECT_SERVER));
			SetAppState(ST_STARTED);
		}
	}
	else
	{
		m_Agent->Disconnect(m_dwConnID, TRUE);
	}
}

void CClientDlg::OnBnClickedStop()
{
	SetAppState(ST_STOPPING);

	if(!m_Agent->Stop())
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

EnHandleResult CClientDlg::OnConnect(ITcpAgent* pSender, CONNID dwConnID)
{
	TCHAR szAddress[50];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	pSender->GetLocalAddress(dwConnID, szAddress, iAddressLen, usPort);

	::PostOnConnect(dwConnID, szAddress, usPort);
	SetAppState(ST_CONNECTED);

	return HR_OK;
}

EnHandleResult CClientDlg::OnSend(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	::PostOnSend(dwConnID, pData, iLength);
	return HR_OK;
}

EnHandleResult CClientDlg::OnReceive(ITcpAgent* pSender, CONNID dwConnID, int iLength)
{
	ITcpPullAgent* pAgent = ITcpPullAgent::FromS(pSender);

	int required = m_pkgInfo.length;
	int remain	 = iLength;

	while(remain >= required)
	{
		remain -= required;
		CBufferPtr buffer(required);

		EnFetchResult result = pAgent->Fetch(dwConnID, buffer, (int)buffer.Size());
		if(result == FR_OK)
		{
			if(m_pkgInfo.is_header)
			{
				TPkgHeader* pHeader = (TPkgHeader*)buffer.Ptr();
				TRACE("[Agent] head -> seq: %d, body_len: %d\n", pHeader->seq, pHeader->body_len);

				required = pHeader->body_len;
			}
			else
			{
				TPkgBody* pBody = (TPkgBody*)buffer.Ptr();
				TRACE("[Agent] body -> name: %s, age: %d, desc: %s\n", pBody->name, pBody->age, pBody->desc);

				required = sizeof(TPkgHeader);
			}

			m_pkgInfo.is_header	= !m_pkgInfo.is_header;
			m_pkgInfo.length	= required;

			::PostOnReceive(dwConnID, buffer, (int)buffer.Size());
		}
	}

	return HR_OK;
}

EnHandleResult CClientDlg::OnClose(ITcpAgent* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID)	:
	::PostOnError(dwConnID, enOperation, iErrorCode);

	if(m_enState != ST_STOPPING)
		SetAppState(ST_STARTED);

	return HR_OK;
}

EnHandleResult CClientDlg::OnShutdown(ITcpAgent* pSender)
{
	::PostOnShutdown();
	SetAppState(ST_STOPPED);

	return HR_OK;
}
