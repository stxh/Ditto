#pragma once

// Tray notify message, shared by CMainFrame and CNoDbFrameWnd maps.
#ifndef WM_TRAYNOTIFY
#define WM_TRAYNOTIFY (WM_USER + 100)
#endif

// Registered "TaskbarCreated" message, defined in TrayWnd.cpp.
extern UINT WM_TASKBARCREATED;

// Message-only window that relays TaskbarCreated to the main frame.
class CTrayWnd : public CWindowImpl<CTrayWnd>
{
public:
	DECLARE_WND_CLASS_EX(_T("Ditto_TrayWnd"), 0, 0)

	CTrayWnd();
	~CTrayWnd();

	BEGIN_MSG_MAP(CTrayWnd)
		MESSAGE_HANDLER(WM_TASKBARCREATED, OnTaskBarCreated)
	END_MSG_MAP()

	LRESULT OnTaskBarCreated(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
