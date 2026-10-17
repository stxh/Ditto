#pragma once
#include "..\resource.h"
#include "NTray.h"
#include "HotKeys.h"
#include "TrayWnd.h"
#include "Misc.h"

class COptionsSheet;

class CNoDbFrameWnd : public CFrameWindowImpl<CNoDbFrameWnd>
{
public:
	DECLARE_FRAME_WND_CLASS(_T("DittoNoDbFrame"), IDR_MAINFRAME)

public:
	CNoDbFrameWnd();

	CTrayNotifyIcon m_trayIcon;
	COptionsSheet* m_pOptions;
	CHotKey* m_pDittoHotKey; // activate ditto's qpaste window
	CHotKey* m_pDittoHotKey2; // activate ditto's qpaste window
	CHotKey* m_pDittoHotKey3; // activate ditto's qpaste window

	BEGIN_MSG_MAP(CNoDbFrameWnd)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		COMMAND_ID_HANDLER(ID_FIRST_OPTIONS, OnFirstOptions)
		COMMAND_ID_HANDLER(ID_FIRST_EXIT_NO_DB, OnFirstExitNoDb)
		MESSAGE_HANDLER(WM_TRAYNOTIFY, OnTrayNotification)
		MESSAGE_HANDLER(WM_OPTIONS_CLOSED, OnOptionsClosed)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
		CHAIN_MSG_MAP(CFrameWindowImpl<CNoDbFrameWnd>)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFirstOptions(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFirstExitNoDb(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnTrayNotification(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void ShowNoDbMessage();
	void TryOpenDatabase();
	LRESULT OnOptionsClosed(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHotKey(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

