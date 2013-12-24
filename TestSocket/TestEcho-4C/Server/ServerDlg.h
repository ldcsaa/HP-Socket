
// ServerDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "../../../Common/Src/socket/HPSocket4C.h"
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
	afx_msg LRESULT CServerDlg::OnUserInfoMsg(WPARAM wp, LPARAM lp);
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	afx_msg void OnBnClickedDisconnect();
	afx_msg void OnEnChangeConnId();
	DECLARE_MESSAGE_MAP()
public:
	void SetAppState(EnAppState state);
private:
	static En_HP_HandleResult __stdcall OnPrepareListen(SOCKET soListen);
	static En_HP_HandleResult __stdcall OnAccept(CONNID dwConnID, SOCKET soClient);
	static En_HP_HandleResult __stdcall OnSend(CONNID dwConnID, const BYTE* pData, int iLength);
	static En_HP_HandleResult __stdcall OnReceive(CONNID dwConnID, int iLength);
	static En_HP_HandleResult __stdcall OnClose(CONNID dwConnID);
	static En_HP_HandleResult __stdcall OnError(CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode);
	static En_HP_HandleResult __stdcall OnServerShutdown();

private:
	TPkgInfo* FindPkgInfo(CONNID dwConnID);
	void RemovePkgInfo(CONNID dwConnID);

private:
	CListBox m_Info;
	CButton m_Start;
	CButton m_Stop;
	CEdit m_Address;
	CEdit m_ConnID;
	CButton m_DisConn;

private:
	EnAppState m_enState;
	CString m_strAddress;
	
	//CCriSec m_csPkgInfo;

private:
	static const USHORT PORT;
	static const LPCTSTR ADDRESS;

	static CServerDlg* m_spThis;
	static HP_TcpPullServer m_spServer;
	static HP_TcpPullServerListener m_spListener;
};
