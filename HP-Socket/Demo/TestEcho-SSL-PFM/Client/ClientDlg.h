
// ClientDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "../../../Src/HPSocket-SSL.h"
#include "../../Global/helper.h"


// CClientDlg dialog
class CClientDlg : public CDialogEx, public CTcpAgentListener
{
// Construction
public:
	CClientDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg LRESULT OnUserInfoMsg(WPARAM wp, LPARAM lp);
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);

	DECLARE_MESSAGE_MAP()
public:
	void SetAppState(EnAppState state);
private:
	virtual EnHandleResult OnPrepareConnect(ITcpAgent* pSender, CONNID dwConnID, SOCKET socket);
	virtual EnHandleResult OnSend(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnClose(ITcpAgent* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult OnConnect(ITcpAgent* pSender, CONNID dwConnID);
	virtual EnHandleResult OnHandShake(ITcpAgent* pSender, CONNID dwConnID);
	virtual EnHandleResult OnShutdown(ITcpAgent* pSender);
private:
	BOOL CheckParams();
private:
	CComboBox m_TestTimes;
	CComboBox m_SocketCount;
	CComboBox m_TestInterv;
	CComboBox m_ThreadCount;
	CComboBox m_MaxConnCount;
	CComboBox m_DataLen;
	CComboBox m_SendPolicy;
	CListBox m_Info;
	CEdit m_Address;
	CEdit m_Port;
	CButton m_Start;
	CButton m_Stop;

	CString m_strAddress;
	USHORT m_usPort;
	int m_iTestTimes;
	int m_iTestInterv;
	int m_iSocketCount;
	int m_iThreadCount;
	int m_iDataLen;

	volatile LONGLONG m_llTotalReceived;
	volatile LONGLONG m_llTotalSent;
	LONGLONG m_llExpectReceived;
	DWORD m_dwBeginTickCount;
	DWORD m_dwTimeconsuming;

	EnAppState m_enState;
	CBufferPtr m_sendBuffer;

	volatile BOOL	m_hasError;
	CCriSec			m_cs;
	vector<CONNID>	m_connIDs;

	CSSLAgentPtr		m_Agent;
};
