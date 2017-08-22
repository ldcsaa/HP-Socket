
// ServerDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "../../../../Common/Src/RingBuffer.h"
#include "../../../../Common/Src/WaitFor.h"
#include "../../../../Common/Src/Thread.h"
#include "../../../../Common/Src/Event.h"
#include "../../../Src/HPSocket.h"
#include "../../Global/helper.h"

struct TDyingConnection
{
	CONNID			connID;
	DWORD			killTime;

	TDyingConnection(CONNID id, DWORD kt = 0)
	: connID		(id)
	, killTime		(kt == 0 ? ::TimeGetTime() : kt)
	{

	}

	static TDyingConnection* Construct(CONNID id, DWORD kt = 0)	{return new TDyingConnection(id, kt);}
	static void Destruct(TDyingConnection* pObj)				{if(pObj) delete pObj;}

};

struct THttpContext
{
	volatile LONG	ref;
	volatile CONNID	serverConnID;
	volatile CONNID	agentConnID;

	char		separator[3];
	BOOL		parsed;
	BOOL		https;
	BOOL		http11;
	CStringA	header;
	CStringA	host;
	USHORT		port;

	CEvt		  evt;

	static THttpContext* Construct(CONNID svrConnID) {return new THttpContext(svrConnID);}
	static void Destruct(THttpContext* pObj) {delete pObj;}

	LONG IncRef() {return ::InterlockedIncrement(&ref);}

	LONG DecRef()
	{
		LONG val = ::InterlockedDecrement(&ref);
		ASSERT(val >= 0);

		if(val == 0)
			Destruct(this);

		return val;
	}

private:
	THttpContext	(CONNID svrConnID)
	: ref			(1L)
	, serverConnID	(svrConnID)
	, agentConnID	(0)
	, port			(0)
	, parsed		(FALSE)
	, https			(FALSE)
	, http11		(FALSE)
	{
		ZeroMemory(separator, sizeof(separator) / sizeof(char));
	}

};

class CServerDlg;

class CTcpServerListenerImpl : public CTcpServerListener
{
public:
	CTcpServerListenerImpl(CServerDlg* pDlg) : m_pDlg(pDlg)
	{
		
	}

private:
	virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen);
	virtual EnHandleResult OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult OnAccept(ITcpServer* pSender, CONNID dwConnID, SOCKET soClient);
	virtual EnHandleResult OnShutdown(ITcpServer* pSender);

private:
	void DetachConnInfo(CONNID dwConnID);
	EnHandleResult ParseRequest(THttpContext* pContext, const BYTE* pData, int iLength);
	BOOL ParseConnectionInfo(THttpContext* pContext, CStringA& strData);
	void ParseHost(THttpContext* pContext, const CStringA& strFullHost);

	EnHandleResult ConnectToServer(THttpContext* pContext, const CStringA& strData, const BYTE* pData2, int iLength2);


public:
	CServerDlg* m_pDlg;
};

class CTcpAgentListenerImpl : public CTcpAgentListener
{
public:
	CTcpAgentListenerImpl(CServerDlg* pDlg) : m_pDlg(pDlg)
	{

	}

private:
	virtual EnHandleResult OnSend(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnClose(ITcpAgent* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult OnConnect(ITcpAgent* pSender, CONNID dwConnID);
	virtual EnHandleResult OnShutdown(ITcpAgent* pSender);

private:
	void DetachConnInfo(CONNID dwConnID);

public:
	CServerDlg* m_pDlg;
};

// CServerDlg dialog
class CServerDlg : public CDialogEx
{
// Construction
public:
	CServerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SERVER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg LRESULT CServerDlg::OnUserInfoMsg(WPARAM wp, LPARAM lp);
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	afx_msg void OnBnClickedOptions();
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()

public:
	void SetAppState(EnAppState state);

	void KillServerDyingConnections();
	void ReleaseServerDyingConnections();
	void DisconnectAgentSilenceConnections();

	UINT CleanerThreadProc(PVOID pv = nullptr);
	void WaitForCleanerThreadEnd();

private:
	CListBox m_Info;
	CEdit m_BindAddress;
	CEdit m_Port;
	CButton m_Start;
	CButton m_Stop;
	CButton m_Options;
	CButton m_ShowLog;
	EnAppState m_enState;

public:
	unique_ptr<CTcpServerListenerImpl>	m_pServerListener;
	unique_ptr<CTcpAgentListenerImpl>	m_pAgentListener;
	CTcpServerPtr	m_Server;
	CTcpAgentPtr	m_Agent;

	BOOL m_bLog;

	CEvt						m_evCleaner;
	CThread<CServerDlg>			m_thCleaner;
	CCASQueue<TDyingConnection>	m_lsDyingQueue;
};
