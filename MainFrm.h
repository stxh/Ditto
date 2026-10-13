#pragma once

#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlcrack.h>
#include <atlmisc.h>
#include <atlctrls.h>
#include <atlctrlx.h>

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

    class CMainFrame : public CFrameWindowImpl<CMainFrame>,
                       public CUpdateUI<CMainFrame>,
                       public CMessageFilter,
                       public CIdleHandler
{
public:
    CMainFrame();

    DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnIdle();

    BEGIN_MSG_MAP(CMainFrame)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_CLOSE(OnClose)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_TIMER(OnTimer)
        MSG_WM_SETTINGCHANGE(OnWinIniChange)
        MSG_WM_SETFOCUS(OnSetFocus)
        COMMAND_ID_HANDLER(ID_FIRST_OPTION, OnFirstOption)
        COMMAND_ID_HANDLER(ID_FIRST_EXIT, OnFirstExit)
        COMMAND_ID_HANDLER(ID_FIRST_SHOWQUICKPASTE, OnFirstShowquickpaste)
        COMMAND_ID_HANDLER(ID_FIRST_TOGGLECONNECTCV, OnFirstToggleConnectCV)
        COMMAND_ID_HANDLER(ID_FIRST_IMPORT, OnFirstImport)
        COMMAND_ID_HANDLER(ID_FIRST_NEWCLIP, OnFirstNewclip)
        COMMAND_ID_HANDLER(ID_FIRST_GLOBALHOTKEYS, OnFirstGlobalhotkeys)
        COMMAND_ID_HANDLER(ID_FIRST_DELETECLIPDATA, OnFirstDeleteclipdata)
        COMMAND_ID_HANDLER(ID_FIRST_SAVECURRENTCLIPBOARD, OnFirstSavecurrentclipboard)
        COMMAND_ID_HANDLER(ID_FIRST_FIXUPSTICKYCLIPORDER, OnFirstFixupstickycliporder)
        COMMAND_ID_HANDLER(ID_FIRST_SHOWSTARTUPMESSAGE, OnFirstShowstartupmessage)
        COMMAND_ID_HANDLER(ID_FIRST_BACKUPDATABASE, OnFirstBackupdatabase)
        COMMAND_ID_HANDLER(ID_FIRST_RESTOREDATABASE, OnFirstRestoredatabase)
        COMMAND_ID_HANDLER(ID_FIRST_DELETEALLNONUSEDCLIPS, OnFirstDeleteallnonusedclips)
        COMMAND_ID_HANDLER(ID_FIRST_HELP, OnFirstHelp)
        MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
        MESSAGE_HANDLER(WM_SHOW_TRAY_ICON, OnShowTrayIcon)
        MESSAGE_HANDLER(WM_CLIPBOARD_COPIED, OnClipboardCopied)
        MESSAGE_HANDLER(WM_ADD_TO_DATABASE_FROM_SOCKET, OnAddToDatabaseFromSocket)
        MESSAGE_HANDLER(WM_SEND_RECIEVE_ERROR, OnErrorOnSendRecieve)
        MESSAGE_HANDLER(WM_SHOW_ERROR_MSG, OnErrorMsg)
        MESSAGE_HANDLER(WM_SET_CONNECTED, OnSetConnected)
        MESSAGE_HANDLER(WM_OPEN_CLOSE_WINDOW, OnOpenCloseWindow)
        MESSAGE_HANDLER(WM_LOAD_ClIP_ON_CLIPBOARD, OnLoadClipOnClipboard)
        MESSAGE_HANDLER(WM_GLOBAL_CLIPS_CLOSED, OnGlobalClipsClosed)
        MESSAGE_HANDLER(WM_OPTIONS_CLOSED, OnOptionsClosed)
        MESSAGE_HANDLER(WM_SHOW_OPTIONS, OnShowOptions)
        MESSAGE_HANDLER(WM_DELETE_CLIPS_CLOSED, OnDeleteClipDataClosed)
        MESSAGE_HANDLER(WM_SAVE_CLIPBOARD, OnSaveClipboardMessage)
        MESSAGE_HANDLER(WM_READD_TASKBAR_ICON, OnReAddTaskBarIcon)
        MESSAGE_HANDLER(WM_REOPEN_DATABASE, OnReOpenDatabase)
        MESSAGE_HANDLER(WM_SHOW_MSG_WINDOW, OnShowMsgWindow)
        MESSAGE_HANDLER(WM_SHOW_DITTO_GROUP, OnShowDittoGroup)
        MESSAGE_HANDLER(WM_DISPLAYCHANGE, OnResolutionChange)
        MESSAGE_HANDLER(WM_TRAYNOTIFY, OnTrayNotification)
        MESSAGE_HANDLER(WM_PLAIN_TEXT_PASTE, OnPlainTextPaste)
        MESSAGE_HANDLER(WM_BACKUP_DB, OnBackupDb)
        MESSAGE_HANDLER(WM_RESTORE_DB, OnRestoreDb)
        MESSAGE_HANDLER(WM_PASTE_CLIP, OnPasteClip)
        MESSAGE_HANDLER(WM_EDIT_CLIP, OnEditClip)
        CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
        CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
    END_MSG_MAP()

    BEGIN_UPDATE_UI_MAP(CMainFrame)
        UPDATE_ELEMENT(ID_FIRST_TOGGLECONNECTCV)
        UPDATE_ELEMENT(ID_FIRST_SHOWSTARTUPMESSAGE)
    END_UPDATE_UI_MAP()

public:
    CQuickPaste m_quickPaste;
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
    HWND m_pGlobalClips;
    HWND m_pDeleteClips;
    HWND m_pOptions;
    int m_doubleClickGroupId;
    DWORD m_doubleClickGroupStartTime;
    CPowerManager m_PowerManager;
    int m_startupScreenWidth;
    int m_startupScreenHeight;
    CRichEditCtrlEx m_richEditTextConverter;

    BOOL ResetKillDBTimer();
    void DoDittoCopyBufferPaste(int nCopyBuffer);
    void DoFirstTenPositionsPaste(int nPos);
    void PasteOrShowGroup(int dbId, BOOL updateClipTime, BOOL activeTarget, BOOL sendPaste, bool pastedFromGroup);
    void StartKeyModifierTimer();
    bool PasteQuickPasteEntry(CString csQuickPaste);
    bool SaveQuickPasteEntry(CString csQuickPaste, CClipList* pClipList);
    void ShowErrorMessage(CString csTitle, CString csMessage);
    bool CloseAllOpenDialogs();
    void DoTextOnlyPaste();
    void RefreshShowInTaskBar();
    void ShowEditWnd(CClipIDs& Ids);
    void ShowQPasteWithActiveWindowCheck();

    LRESULT OnHotKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnShowTrayIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnClipboardCopied(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnAddToDatabaseFromSocket(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnErrorOnSendRecieve(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnErrorMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSetConnected(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnOpenCloseWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnLoadClipOnClipboard(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnGlobalClipsClosed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnDeleteClipDataClosed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnOptionsClosed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnShowOptions(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSaveClipboardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnReAddTaskBarIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnReOpenDatabase(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnShowMsgWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnShowDittoGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnResolutionChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnTrayNotification(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnPlainTextPaste(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnWinIniChange(UINT nFlags, LPCTSTR lpszSection);
    LRESULT OnRestoreDb(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnBackupDb(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnPasteClip(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnEditClip(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnClose();
    void OnDestroy();
    void OnTimer(UINT_PTR nIDEvent);
    void OnSetFocus(HWND hWndOldFocus);
    LRESULT OnFirstOption(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnFirstExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnFirstShowquickpaste(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnFirstToggleConnectCV(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnFirstImport(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnFirstNewclip(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnFirstGlobalhotkeys(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnFirstDeleteclipdata(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnFirstSavecurrentclipboard(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnFirstFixupstickycliporder(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnFirstShowstartupmessage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnFirstBackupdatabase(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnFirstRestoredatabase(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnFirstDeleteallnonusedclips(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnFirstHelp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};
