
// NodeDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "../../../Src/UdpNode.h"
#include "../../Global/helper.h"


// CNodeDlg dialog
class CNodeDlg : public CDialogEx, public CUdpNodeListener
{
// Construction
public:
	CNodeDlg(CWnd* pParent = NULL);	// standard constructor

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
	afx_msg void OnBnClickedUniMode();
	afx_msg void OnBnClickedMultiMode();
	afx_msg void OnBnClickedBroadMode();
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedSendCast();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg LRESULT OnUserInfoMsg(WPARAM wp, LPARAM lp);
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	DECLARE_MESSAGE_MAP()
public:
	void SetAppState(EnAppState state);
private:
	virtual EnHandleResult OnPrepareListen(IUdpNode* pSender, SOCKET soListen);
	virtual EnHandleResult OnSend(IUdpNode* pSender, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pData, int iLength);
	virtual EnHandleResult OnReceive(IUdpNode* pSender, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pData, int iLength);
	virtual EnHandleResult OnError(IUdpNode* pSender, EnSocketOperation enOperation, int iErrorCode, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pBuffer, int iLength);
	virtual EnHandleResult OnShutdown(IUdpNode* pSender);
private:
	CEdit m_Content;
	CButton m_Send;
	CButton m_SendCast;
	CListBox m_Info;
	CEdit m_DestAddr;
	CEdit m_DestPort;
	CEdit m_CastAddr;
	CEdit m_Port;
	CEdit m_BindAddr;
	CEdit m_TTL;
	CButton m_IPLoop;
	CButton m_ReuseAddr;
	CButton m_CastMode;
	CButton m_CastMode2;
	CButton m_CastMode3;
	CButton m_Start;
	CButton m_Stop;

	EnAppState m_enState;
	CUdpNode m_Node;
};
