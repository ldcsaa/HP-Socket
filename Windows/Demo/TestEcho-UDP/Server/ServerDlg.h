
// ServerDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "../../../Src/UdpServer.h"
#include "../../Global/helper.h"

// CServerDlg dialog
class CServerDlg : public CDialogEx, public CUdpServerListener
{
// Construction
public:
	CServerDlg(CWnd* pParent = NULL);	// standard constructor

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
	afx_msg void OnBnClickedDisconnect();
	afx_msg void OnEnChangeConnId();
	afx_msg LRESULT CServerDlg::OnUserInfoMsg(WPARAM wp, LPARAM lp);
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	DECLARE_MESSAGE_MAP()
public:
	void SetAppState(EnAppState state);
private:
	virtual EnHandleResult OnPrepareListen(IUdpServer* pSender, SOCKET soListen);
	virtual EnHandleResult OnAccept(IUdpServer* pSender, CONNID dwConnID, UINT_PTR pSockAddr);
	virtual EnHandleResult OnSend(IUdpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnReceive(IUdpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnClose(IUdpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult OnShutdown(IUdpServer* pSender);

private:
	CListBox m_Info;
	CButton m_Start;
	CButton m_Stop;
	CEdit m_BindAddress;
	CEdit m_RejectAddress;
	CEdit m_ConnID;
	CButton m_DisConn;

private:
	static const USHORT PORT;
	static const LPCTSTR DEF_ADDRESS;

	CString m_strRejectAddress;
	EnAppState m_enState;
	CUdpServer m_Server;
};
