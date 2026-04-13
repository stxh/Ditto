#pragma once

//#include "SystemTray.h"
#include "QuickPaste.h"
#include "ToolTipEx.h"
#include "MainFrmThread.h"
#include "ClipboardSaveRestore.h"
#include "PowerManager.h"
#include "DittoPopupWindow.h"
#include "NTray.h"

#define CLOSE_WINDOW_TIMER				1	
#define HIDE_ICON_TIMER					2
#define REMOVE_OLD_ENTRIES_TIMER		3
#define REMOVE_OLD_TEMP_FILES			6
#define END_DITTO_BUFFER_CLIPBOARD_TIMER	7
#define KEY_STATE_MODIFIERS				8
#define ACTIVE_WINDOW_TIMER				9
#define TEXT_ONLY_PASTE					11
#define READ_RANDOM_DB_FILE				12
#define GROUP_DOUBLE_CLICK				13
#define CLOSE_POPUP_MSG_WND				14
#define SCREEN_RESOLUTION_CHANGED		15
#define DELAYED_SHOW_DITTO_TIMER		16
#define SET_WINDOWS_THEME_TIMER			17
#define CLOSE_NO_DB_WINDOW_TIMER        18

class COptionsSheet;

class CMainFrame: public CFrameWindowImpl<CMainFrame>
{
public:
    CMainFrame();
protected:
    DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)
    // Attributes
public:

    // Operations
public:

    BOOL ResetKillDBTimer();

// Overrides
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    // Implementation
public:
    virtual ~CMainFrame();
    #ifdef _DEBUG
        virtual void AssertValid()const;
        virtual void Dump(CDumpContext &dc)const;
    #endif 

    CQuickPaste m_quickPaste;
    //CSystemTray m_TrayIcon;
	CTrayNotifyIcon m_trayIcon;
    ULONG m_ulCopyGap;
    CString m_csKeyboardPaste;
    CAlphaBlend m_Transparency;
    BYTE m_keyStateModifiers;
    DWORD m_startKeyStateTime;
    bool m_bMovedSelectionMoveKeyState;
    short m_keyModifiersTimerCount;
    HWND m_tempFocusWnd;
    CMainFrmThread m_thread;
	CDialog *m_pGlobalClips;
	CDialog *m_pDeleteClips;
	COptionsSheet *m_pOptions;
	int m_doubleClickGroupId;
	DWORD m_doubleClickGroupStartTime;
	CPowerManager m_PowerManager;
	int m_startupScreenWidth;
	int m_startupScreenHeight;
    CRichEditCtrlEx m_richEditTextConverter;

    void DoDittoCopyBufferPaste(int nCopyBuffer);
    void DoFirstTenPositionsPaste(int nPos);
	void PasteOrShowGroup(int dbId, BOOL updateClipTime, BOOL activeTarget, BOOL sendPaste, bool pastedFromGroup);

	void StartKeyModifierTimer();

	bool PasteQuickPasteEntry(CString csQuickPaste);
    bool SaveQuickPasteEntry(CString csQuickPaste, CClipList *pClipList);
    void ShowErrorMessage(CString csTitle, CString csMessage);
    bool CloseAllOpenDialogs();
	void DoTextOnlyPaste();
	void RefreshShowInTaskBar();

    void ShowEditWnd(CClipIDs &Ids);


    // Generated message map functions
protected:
    //{{AFX_MSG(CMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
void OnFirstOption();
    void OnFirstExit();
    void OnChangeCbChain(HWND hWndRemove, HWND hWndAfter);
    void OnDrawClipboard();
    void OnTimer(UINT_PTR nIDEvent);
    void OnFirstShowquickpaste();
    void OnFirstToggleConnectCV();
    void OnUpdateFirstToggleConnectCV(CCmdUI *pCmdUI);
    void OnFirstHelp();
    // End of message declarations
    LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);	void ShowQPasteWithActiveWindowCheck();
LRESULT OnShowTrayIcon(WPARAM wParam, LPARAM lParam);
    LRESULT OnClipboardCopied(WPARAM wParam, LPARAM lParam);
    LRESULT OnAddToDatabaseFromSocket(WPARAM wParam, LPARAM lParam);
    LRESULT OnErrorOnSendRecieve(WPARAM wParam, LPARAM lParam);
 	LRESULT OnErrorMsg(WPARAM wParam, LPARAM lParam);
    LRESULT OnSetConnected(WPARAM wParam, LPARAM lParam);
 	LRESULT OnOpenCloseWindow(WPARAM wParam, LPARAM lParam);
 	LRESULT OnLoadClipOnClipboard(WPARAM wParam, LPARAM lParam);
 	LRESULT OnGlobalClipsClosed(WPARAM wParam, LPARAM lParam);
 	LRESULT OnDeleteClipDataClosed(WPARAM wParam, LPARAM lParam);
 	LRESULT OnOptionsClosed(WPARAM wParam, LPARAM lParam);
 	LRESULT OnShowOptions(WPARAM wParam, LPARAM lParam);
 	LRESULT OnSaveClipboardMessage(WPARAM wParam, LPARAM lParam);
 	LRESULT OnReAddTaskBarIcon(WPARAM wParam, LPARAM lParam);
    
    // WTL Message Map
    BEGIN_MSG_MAP(CMainFrame)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_CLOSE(OnClose)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_TIMER(OnTimer)
        MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
        MESSAGE_HANDLER(WM_SHOWTRAYICON, OnShowTrayIcon)
        MESSAGE_HANDLER(WM_CLIPBOARDCOPIED, OnClipboardCopied)
        MESSAGE_HANDLER(WM_ADDTODATABASEFROMSOCKET, OnAddToDatabaseFromSocket)
        MESSAGE_HANDLER(WM_ERRORONSENDRECIEVE, OnErrorOnSendRecieve)
        MESSAGE_HANDLER(WM_ERRORMSG, OnErrorMsg)
        MESSAGE_HANDLER(WM_SETCONNECTED, OnSetConnected)
        MESSAGE_HANDLER(WM_OPENCLOSEWINDOW, OnOpenCloseWindow)
        MESSAGE_HANDLER(WM_LOADCLIPONCLIPBOARD, OnLoadClipOnClipboard)
        MESSAGE_HANDLER(WM_GLOBALCLIPSCLOSED, OnGlobalClipsClosed)
        MESSAGE_HANDLER(WM_DELETECLIPDATACLOSED, OnDeleteClipDataClosed)
        MESSAGE_HANDLER(WM_OPTIONSCLOSED, OnOptionsClosed)
        MESSAGE_HANDLER(WM_SHOWOPTIONS, OnShowOptions)
        MESSAGE_HANDLER(WM_SAVECLIPBOARDMESSAGE, OnSaveClipboardMessage)
        MESSAGE_HANDLER(WM_READDTASKBARICON, OnReAddTaskBarIcon)
        CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
    END_MSG_MAP()
    
public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    void OnClose();
    void OnFirstImport();
    void OnDestroy();
    void OnFirstNewclip();
 	void OnFirstGlobalhotkeys();
 	void OnFirstDeleteclipdata();
 	void OnFirstSavecurrentclipboard();
 	LRESULT OnReOpenDatabase(WPARAM wParam, LPARAM lParam);
 	LRESULT OnShowMsgWindow(WPARAM wParam, LPARAM lParam);
 	LRESULT OnShowDittoGroup(WPARAM wParam, LPARAM lParam);
 	void OnFirstFixupstickycliporder();
 	LRESULT OnResolutionChange(WPARAM wParam, LPARAM lParam);
 	LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);	afx_msg LRESULT OnPlainTextPaste(WPARAM wParam, LPARAM lParam);
	afx_msg void OnWinIniChange(LPCTSTR lpszSection);
	afx_msg void OnFirstShowstartupmessage();
	afx_msg void OnUpdateFirstShowstartupmessage(CCmdUI *pCmdUI);
	afx_msg void OnFirstBackupdatabase();
	afx_msg void OnFirstRestoredatabase();
	afx_msg LRESULT OnRestoreDb(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBackupDb(WPARAM wParam, LPARAM lParam);
	afx_msg void OnFirstDeleteallnonusedclips();
    afx_msg LRESULT OnPasteClip(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEditClip(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
};
