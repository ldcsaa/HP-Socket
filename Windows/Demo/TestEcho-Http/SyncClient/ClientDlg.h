
// ClientDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "../../../Src/HttpClient.h"
#include "../../Global/helper.h"


// CClientDlg dialog
class CClientDlg : public CDialogEx, public CHttpSyncClientListener
{
// Construction
public:
	CClientDlg(CWnd* pParent = NULL);	// standard constructor
	~CClientDlg();

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
	static CStringA GetHeaderSummary(IHttpSyncClient* pHttpClient, LPCSTR lpszSep = "  ", int iSepCount = 0, BOOL bWithContentLength = TRUE);

private:
	virtual EnHandleResult OnConnect(ITcpClient* pSender, CONNID dwConnID);
	virtual EnHandleResult OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);

	virtual EnHttpParseResult OnHeader(IHttpClient* pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue);
	virtual EnHttpParseResult OnHeadersComplete(IHttpClient* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnBody(IHttpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHttpParseResult OnChunkHeader(IHttpClient* pSender, CONNID dwConnID, int iLength);
	virtual EnHttpParseResult OnChunkComplete(IHttpClient* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnMessageComplete(IHttpClient* pSender, CONNID dwConnID);
	virtual EnHttpParseResult OnUpgrade(IHttpClient* pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType);

	virtual EnHandleResult OnWSMessageHeader(IHttpClient* pSender, CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen);
	virtual EnHandleResult OnWSMessageBody(IHttpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnWSMessageComplete(IHttpClient* pSender, CONNID dwConnID);


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

	unique_ptr<IHttpSyncClient> m_pClient;
};
