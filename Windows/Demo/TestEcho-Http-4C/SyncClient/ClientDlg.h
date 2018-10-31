
// ClientDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "../../../Src/HPSocket4C-SSL.h"
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
	afx_msg void OnBnClickedHeaderAdd();
	afx_msg void OnBnClickedSend();

	void SendHttp();
	void SendWebSocket();

	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg LRESULT OnUserInfoMsg(WPARAM wp, LPARAM lp);
	afx_msg void OnCbnSelchangeMethod();
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	DECLARE_MESSAGE_MAP()

private:
	void SetAppState(EnAppState state);
	BOOL CheckStarted(BOOL bRestart = TRUE);
	static CStringA GetHeaderSummary(HP_HttpSyncClient pSender, LPCSTR lpszSep = "  ", int iSepCount = 0, BOOL bWithContentLength = TRUE);

private:
	static En_HP_HandleResult __stdcall OnConnect(HP_Client pSender, CONNID dwConnID);
	static En_HP_HandleResult __stdcall OnClose(HP_Client pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);

	static En_HP_HttpParseResult __stdcall OnHeader(HP_HttpClient pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue);
	static En_HP_HttpParseResult __stdcall OnHeadersComplete(HP_HttpClient pSender, CONNID dwConnID);
	static En_HP_HttpParseResult __stdcall OnBody(HP_HttpClient pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	static En_HP_HttpParseResult __stdcall OnChunkHeader(HP_HttpClient pSender, CONNID dwConnID, int iLength);
	static En_HP_HttpParseResult __stdcall OnChunkComplete(HP_HttpClient pSender, CONNID dwConnID);
	static En_HP_HttpParseResult __stdcall OnMessageComplete(HP_HttpClient pSender, CONNID dwConnID);
	static En_HP_HttpParseResult __stdcall OnUpgrade(HP_HttpClient pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType);

	static En_HP_HandleResult __stdcall OnWSMessageHeader(HP_HttpClient pSender, HP_CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen);
	static En_HP_HandleResult __stdcall OnWSMessageBody(HP_HttpClient pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
	static En_HP_HandleResult __stdcall OnWSMessageComplete(HP_HttpClient pSender, HP_CONNID dwConnID);

private:
	CButton m_Send;
	CListBox m_Info;
	CEdit m_Address;
	CEdit m_Port;
	CButton m_UseCookie;
	CButton m_Listener;
	CButton m_Start;
	CButton m_Stop;
	CComboBox m_Method;
	CComboBox m_Schema;
	CEdit m_Path;
	CEdit m_HeaderName;
	CEdit m_HeaderValue;
	CButton m_HeaderAdd;
	CListBox m_Headers;
	CEdit m_Body;

	BOOL m_bUseCookie;
	BOOL m_bListener;
	BOOL m_bWebSocket;
	EnAppState m_enState;

	static CClientDlg* m_spThis;

	HP_HttpClientListener m_HttpClientListener;
	HP_HttpSyncClient m_HttpSyncClient;
};
