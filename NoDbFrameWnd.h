#pragma once
#include "NTray.h"
#include "HotKeys.h"

class COptionsSheet;

class CNoDbFrameWnd : public CFrameWindowImpl<CNoDbFrameWnd>
{
public:
	DECLARE_FRAME_WND_CLASS_EX(_T("DittoNoDbFrame"), IDR_MAINFRAME_NO_DB, 0, COLOR_WINDOW)

public:
	CNoDbFrameWnd();

	CTrayNotifyIcon m_trayIcon;
	COptionsSheet* m_pOptions;
	CHotKey* m_pDittoHotKey; // activate ditto's qpaste window
	CHotKey* m_pDittoHotKey2; // activate ditto's qpaste window
	CHotKey* m_pDittoHotKey3; // activate ditto's qpaste window

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

