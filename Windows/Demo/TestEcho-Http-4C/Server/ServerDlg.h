
// ServerDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "../../../Src/HPSocket4C-SSL.h"
#include "../../Global/helper.h"


// CServerDlg dialog
class CServerDlg : public CDialogEx
{
// Construction
public:
	CServerDlg(CWnd* pParent = NULL);	// standard constructor
	~CServerDlg();						// standard destructor

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
	afx_msg void OnBnClickedRelease();
	afx_msg void OnEnChangeConnId();
	afx_msg LRESULT CServerDlg::OnUserInfoMsg(WPARAM wp, LPARAM lp);
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	DECLARE_MESSAGE_MAP()
public:
	void SetAppState(EnAppState state);

	static int CALLBACK SIN_ServerNameCallback(LPCTSTR lpszServerName);

private:
	static En_HP_HandleResult __stdcall OnPrepareListen(HP_Server pSender, SOCKET soListen);
	static En_HP_HandleResult __stdcall OnAccept(HP_Server pSender, HP_CONNID dwConnID, SOCKET soClient);
	static En_HP_HandleResult __stdcall OnHandShake(HP_Server pSender, HP_CONNID dwConnID);
	static En_HP_HandleResult __stdcall OnReceive(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
	static En_HP_HandleResult __stdcall OnSend(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
	static En_HP_HandleResult __stdcall OnClose(HP_Server pSender, HP_CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	static En_HP_HandleResult __stdcall OnShutdown(HP_Server pSender);

	static En_HP_HttpParseResult __stdcall OnMessageBegin(HP_HttpServer pSender, HP_CONNID dwConnID);
	static En_HP_HttpParseResult __stdcall OnRequestLine(HP_HttpServer pSender, HP_CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl);
	static En_HP_HttpParseResult __stdcall OnHeader(HP_HttpServer pSender, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue);
	static En_HP_HttpParseResult __stdcall OnHeadersComplete(HP_HttpServer pSender, HP_CONNID dwConnID);
	static En_HP_HttpParseResult __stdcall OnBody(HP_HttpServer pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
	static En_HP_HttpParseResult __stdcall OnChunkHeader(HP_HttpServer pSender, HP_CONNID dwConnID, int iLength);
	static En_HP_HttpParseResult __stdcall OnChunkComplete(HP_HttpServer pSender, HP_CONNID dwConnID);
	static En_HP_HttpParseResult __stdcall OnMessageComplete(HP_HttpServer pSender, HP_CONNID dwConnID);
	static En_HP_HttpParseResult __stdcall OnUpgrade(HP_HttpServer pSender, HP_CONNID dwConnID, EnHttpUpgradeType enUpgradeType);
	static En_HP_HttpParseResult __stdcall OnParseError(HP_HttpServer pSender, HP_CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc);

	static En_HP_HandleResult __stdcall OnWSMessageHeader(HP_HttpServer pSender, HP_CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen);
	static En_HP_HandleResult __stdcall OnWSMessageBody(HP_HttpServer pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
	static En_HP_HandleResult __stdcall OnWSMessageComplete(HP_HttpServer pSender, HP_CONNID dwConnID);

private:
	CStringA GetHeaderSummary(HP_HttpServer pSender, HP_CONNID dwConnID, LPCSTR lpszSep = "  ", int iSepCount = 0, BOOL bWithContentLength = TRUE);
	LPCTSTR GetSenderName(HP_HttpServer pSender);

private:
	CListBox m_Info;
	CButton m_Start;
	CButton m_Stop;
	CEdit m_ConnID;
	CButton m_DisConn;
	CButton m_Release;
	CButton m_RadioHttp;
	CButton m_RadioHttps;
	CEdit m_BindAddr;

	EnAppState m_enState;

private:
	static const LPCTSTR DEF_ADDRESS;
	static const USHORT HTTP_PORT;
	static const USHORT HTTPS_PORT;

	static CServerDlg* m_spThis;

	HP_HttpServer m_HttpServer;
	HP_HttpsServer m_HttpsServer;

	HP_HttpServerListener m_HttpServerListener;
};
