
// ClientDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "../../../Src/HPSocket.h"
#include "../../Global/helper.h"


// CClientDlg dialog
class CClientDlg : public CDialogEx, public CTcpPullAgentListener
{
// Construction
public:
	CClientDlg(CWnd* pParent = NULL);	// standard constructor

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
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedStop();
	afx_msg LRESULT OnUserInfoMsg(WPARAM wp, LPARAM lp);
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	DECLARE_MESSAGE_MAP()
public:
	void SetAppState(EnAppState state);
private:
	virtual EnHandleResult OnSend(ITcpAgent* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnReceive(ITcpAgent* pSender, CONNID dwConnID, int iLength);
	virtual EnHandleResult OnClose(ITcpAgent* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult OnConnect(ITcpAgent* pSender, CONNID dwConnID);
	virtual EnHandleResult OnShutdown(ITcpAgent* pSender);
private:
	CEdit m_Content;
	CButton m_Send;
	CListBox m_Info;
	CEdit m_Address;
	CEdit m_Port;
	CButton m_Start;
	CButton m_Connect;
	CButton m_Stop;

	EnAppState m_enState;
	CString m_strAddress;
	USHORT m_usPort;
	CONNID m_dwConnID;

	TPkgInfo m_pkgInfo;
	CTcpPullAgentPtr m_Agent;
};
