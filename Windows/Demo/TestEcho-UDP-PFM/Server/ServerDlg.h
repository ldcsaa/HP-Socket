
// ServerDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "../../../Src/HPSocket.h"
#include "../../Global/helper.h"

// CServerDlg dialog
class CServerDlg : public CDialogEx, public CUdpServerListener
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
	afx_msg void OnBnClickedStatistics();
	afx_msg LRESULT CServerDlg::OnUserInfoMsg(WPARAM wp, LPARAM lp);
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
public:
	void SetAppState(EnAppState state);
	void Statistics();
	void Reset(BOOL bResetClientCount = TRUE);
private:
	virtual EnHandleResult OnPrepareListen(IUdpServer* pSender, SOCKET soListen);
	virtual EnHandleResult OnSend(IUdpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnReceive(IUdpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnClose(IUdpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult OnAccept(IUdpServer* pSender, CONNID dwConnID, UINT_PTR pSockAddr);
	virtual EnHandleResult OnShutdown(IUdpServer* pSender);

private:
	CListBox m_Info;
	CEdit m_BindAddress;
	CEdit m_Port;
	CComboBox m_SendPolicy;
	CComboBox m_MaxConnCount;
	CComboBox m_RecvCount;
	CButton m_Start;
	CButton m_Stop;
	CButton m_Statistics;
	EnAppState m_enState;

	volatile LONGLONG m_llTotalReceived;
	volatile LONGLONG m_llTotalSent;
	volatile LONG m_lClientCount;

	CCriSec m_cs;
	CUdpServerPtr m_Server;
};
