
// ClientDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "../../../Common/Src/socket/HPSocket.h"
#include "../../Global/helper.h"


// CClientDlg dialog
class CClientDlg : public CDialogEx, public CTcpClientListener
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
	virtual ISocketListener::EnHandleResult OnPrepareSocket(CONNID dwConnID, SOCKET socket);
	virtual ISocketListener::EnHandleResult OnSend(CONNID dwConnID, const BYTE* pData, int iLength);
	virtual ISocketListener::EnHandleResult OnReceive(CONNID dwConnID, const BYTE* pData, int iLength);
	virtual ISocketListener::EnHandleResult OnClose(CONNID dwConnID);
	virtual ISocketListener::EnHandleResult OnError(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	virtual ISocketListener::EnHandleResult OnConnect(CONNID dwConnID);
private:
	BOOL CheckParams();
private:
	CComboBox m_TestTimes;
	CComboBox m_ThreadCount;
	CComboBox m_TestInterv;
	CComboBox m_ThreadInterv;
	CComboBox m_DataLen;
	CListBox m_Info;
	CEdit m_Address;
	CEdit m_Port;
	CButton m_Start;
	CButton m_Stop;

	CString m_strAddress;
	USHORT m_usPort;
	int m_iTestTimes;
	int m_iTestInterv;
	int m_iThreadCount;
	int m_iThreadInterv;
	int m_iDataLen;

	volatile LONGLONG m_llTotalReceived;
	volatile LONGLONG m_llTotalSent;
	LONGLONG m_llExpectReceived;
	DWORD m_dwBeginTickCount;
	DWORD m_dwTimeconsuming;

	EnAppState m_enState;
	CBufferPtr m_sendBuffer;

	VectorWrapper<vector<CTcpClientWrapper*>, PtrSet_Cleaner<vector<CTcpClientWrapper*>>> m_vtClients;

};
