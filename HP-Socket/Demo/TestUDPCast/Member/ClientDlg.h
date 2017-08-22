
// ClientDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "../../../Src/UdpCast.h"
#include "../../Global/helper.h"


// CClientDlg dialog
class CClientDlg : public CDialogEx, public CUdpCastListener
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
	afx_msg void OnEnChangeContent();
	afx_msg void OnBnClickedMultiMode();
	afx_msg void OnBnClickedBroadMode();
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg LRESULT OnUserInfoMsg(WPARAM wp, LPARAM lp);
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	DECLARE_MESSAGE_MAP()
public:
	void SetAppState(EnAppState state);
private:
	virtual EnHandleResult OnPrepareConnect(IUdpCast* pSender, CONNID dwConnID, SOCKET socket);
	virtual EnHandleResult OnSend(IUdpCast* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnReceive(IUdpCast* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnClose(IUdpCast* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult OnConnect(IUdpCast* pSender, CONNID dwConnID);
private:
	CEdit m_Content;
	CButton m_Send;
	CListBox m_Info;
	CEdit m_CastAddr;
	CEdit m_Port;
	CEdit m_BindAddr;
	CEdit m_TTL;
	CButton m_IPLoop;
	CButton m_ReuseAddr;
	CButton m_CastMode;
	CButton m_CastMode2;
	CButton m_Start;
	CButton m_Stop;

	EnAppState m_enState;
	CUdpCast m_Cast;
};
