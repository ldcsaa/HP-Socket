
// ClientDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "../../../Src/HPSocket4C.h"
#include "../../Global/helper.h"


// CClientDlg dialog
class CClientDlg : public CDialogEx
{
// Construction
public:
	CClientDlg(CWnd* pParent = NULL);	// standard constructor
	~CClientDlg();						// standard destructor

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
	afx_msg void OnBnClickedStop();
	afx_msg LRESULT OnUserInfoMsg(WPARAM wp, LPARAM lp);
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	DECLARE_MESSAGE_MAP()

public:
	void SetAppState(EnAppState state);

private:
	static En_HP_HandleResult __stdcall OnConnect(HP_Client pSender, HP_CONNID dwConnID);
	static En_HP_HandleResult __stdcall OnSend(HP_Client pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
	static En_HP_HandleResult __stdcall OnReceive(HP_Client pSender, HP_CONNID dwConnID, int iLength);
	static En_HP_HandleResult __stdcall OnClose(HP_Client pSender, HP_CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode);

private:
	CEdit m_Content;
	CButton m_Send;
	CListBox m_Info;
	CEdit m_Address;
	CEdit m_Port;
	CButton m_Async;
	CButton m_Start;
	CButton m_Stop;

	EnAppState m_enState;
	BOOL m_bAsyncConn;

	TPkgInfo m_pkgInfo;

private:
	static CClientDlg* m_spThis;

	HP_TcpPullClient m_pClient;
	HP_TcpPullClientListener m_pListener;
};
