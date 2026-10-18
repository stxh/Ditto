#if !defined(AFX_CLIPBOARDVIEWER_H__67418FB6_6048_48FA_86D4_F412CACC41B1__INCLUDED_)
#define AFX_CLIPBOARDVIEWER_H__67418FB6_6048_48FA_86D4_F412CACC41B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define TIMER_ENSURE_VIEWER_IN_CHAIN	6
#define TIMER_DRAW_CLIPBOARD			7
#define TIMER_PING						8

class CClipboardViewer : public CWindowImpl<CClipboardViewer>
{
public:
	DECLARE_WND_CLASS(_T("Ditto_ClipboardViewer"))

// Construction
public:
	CClipboardViewer(CCopyThread* pHandler);
	virtual ~CClipboardViewer();

// Implementation
public:
	void Create();

	HWND	m_hNextClipboardViewer;
	bool	m_bCalling_SetClipboardViewer;
	bool	m_bPinging;
	bool	m_bConnect;
	bool	m_bIsConnected;
	bool	m_connectOnStartup;
	CString m_activeWindow;
	CString m_activeWindowTitle;

	// m_pHandler->OnClipboardChange is called when the clipboard changes.
	CCopyThread*	m_pHandler;

	void Connect();    // connects as a clipboard viewer
	void Disconnect(bool bSendPing = true); // disconnects as a clipboard viewer

	void SendPing();

	bool GetConnect()				{ return m_bConnect; }
	void SetConnect(bool bConnect);
	void SetEnsureConnectedTimer();
	bool ValidActiveWnd();

	bool GetIgnoreClipboardChange();

	DWORD m_dwLastCopy;

	BEGIN_MSG_MAP(CClipboardViewer)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CHANGECBCHAIN(OnChangeCbChain)
		MSG_WM_DRAWCLIPBOARD(OnDrawClipboard)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER(WM_SETCONNECT, OnSetConnect)
		MESSAGE_HANDLER(WM_CLIPBOARDUPDATE, OnClipboardChange)
	END_MSG_MAP()

protected:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnChangeCbChain(HWND hWndRemove, HWND hWndAfter);
	void OnDrawClipboard();
	void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnSetConnect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClipboardChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_CLIPBOARDVIEWER_H__67418FB6_6048_48FA_86D4_F412CACC41B1__INCLUDED_)
