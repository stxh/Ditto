#include "stdafx.h"
#include "NoDbFrameWnd.h"
#include "resource.h"
#include "OptionsSheet.h"
#include "DatabaseUtilities.h"
#include "Options.h"
#include "CP_Main.h"
#include "Misc.h"

#define TIMER_OPEN_DB 1
#define TIMER_ERROR_MSG 2

CNoDbFrameWnd::CNoDbFrameWnd()
{
	m_pOptions = NULL;
	m_pDittoHotKey = NULL;
	m_pDittoHotKey2 = NULL;
	m_pDittoHotKey3 = NULL;
}

LRESULT CNoDbFrameWnd::OnCreate(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	////Center the main window so message boxes are in the center
	CRect rcScreen = DefaultMonitorRect();
	CPoint cpCenter = rcScreen.CenterPoint();
	MoveWindow(cpCenter.x, cpCenter.x, 1, 1);

	SetWindowText(_T("DittoWTL"));

	m_trayIcon.Create(this, IDR_MENU_NO_DB, _T("DittoWTL"), CTrayNotifyIcon::LoadIcon(IDI_MAINFRAME_NO_DB), WM_TRAYNOTIFY, 0, 1);
	m_trayIcon.SetDefaultMenuItem(ID_FIRST_OPTIONS, FALSE);
	m_trayIcon.MinimiseToTray(m_hWnd);

	SetTimer(TIMER_OPEN_DB, 15000, NULL);
	SetTimer(TIMER_ERROR_MSG, 180000, NULL);

	g_HotKeys.Init(m_hWnd);

	m_pDittoHotKey = new CHotKey(CString("DittoHotKey"), 704); //704 is ctrl-tilda
	m_pDittoHotKey2 = new CHotKey(CString("DittoHotKey2"));
	m_pDittoHotKey3 = new CHotKey(CString("DittoHotKey3"));

	g_HotKeys.RegisterAll();

	bHandled = FALSE;
	return 0;
}

LRESULT CNoDbFrameWnd::OnFirstOptions(WORD, WORD, HWND, BOOL&)
{
	if (m_pOptions != NULL)
	{
		::SetForegroundWindow(m_pOptions->m_hWnd);
	}
	else
	{
		m_pOptions = new COptionsSheet(_T(""));

		if (m_pOptions != NULL)
		{
			m_pOptions->SetNotifyWnd(m_hWnd);
			m_pOptions->Create();
			m_pOptions->ShowWindow(SW_SHOW);
		}
	}

	return 0;
}

LRESULT CNoDbFrameWnd::OnFirstExitNoDb(WORD, WORD, HWND, BOOL&)
{
	SendMessage(WM_CLOSE, 0, 0);
	return 0;
}

LRESULT CNoDbFrameWnd::OnTrayNotification(UINT, WPARAM wParam, LPARAM lParam, BOOL&)
{
	m_trayIcon.OnTrayNotification(wParam, lParam);
	return 0L;
}

LRESULT CNoDbFrameWnd::OnTimer(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
	UINT_PTR nIDEvent = static_cast<UINT_PTR>(wParam);
	switch (nIDEvent)
	{
	case TIMER_OPEN_DB:
		TryOpenDatabase();
		break;
	case TIMER_ERROR_MSG:
		KillTimer(TIMER_ERROR_MSG);
		ShowNoDbMessage();
		break;
	}

	bHandled = FALSE;
	return 0;
}

void CNoDbFrameWnd::ShowNoDbMessage()
{
	CString msg = theApp.m_Language.GetString(_T("StartupNoDbMsg"), _T("Ditto was unable to open its database, waiting until it can be opened. Update the path in Options if needed. Path: "));
	msg += StrF(_T(" %s"), CGetSetOptions::GetDBPath());
	m_trayIcon.SetBalloonDetails(msg, _T("DittoWTL"), CTrayNotifyIcon::BalloonStyle::Info, CGetSetOptions::GetBalloonTimeout());
}

void CNoDbFrameWnd::TryOpenDatabase()
{
	if (IsDatabaseOpen() ||
		CheckDBExists(CGetSetOptions::GetDBPath()))
	{
		g_HotKeys.Remove(m_pDittoHotKey);
		delete m_pDittoHotKey;
		m_pDittoHotKey = NULL;

		g_HotKeys.Remove(m_pDittoHotKey2);
		delete m_pDittoHotKey2;
		m_pDittoHotKey2 = NULL;

		g_HotKeys.Remove(m_pDittoHotKey3);
		delete m_pDittoHotKey3;
		m_pDittoHotKey3 = NULL;

		KillTimer(TIMER_OPEN_DB);
		KillTimer(TIMER_ERROR_MSG);
		m_trayIcon.Hide();

		theApp.CreateMainWnd();
	}
}

LRESULT CNoDbFrameWnd::OnOptionsClosed(UINT, WPARAM wParam, LPARAM lParam, BOOL&)
{
	delete m_pOptions;
	m_pOptions = NULL;

	TryOpenDatabase();

	return TRUE;
}

LRESULT CNoDbFrameWnd::OnHotKey(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	const UINT nHotKeyId = static_cast<UINT>(wParam);
	if (m_pDittoHotKey && nHotKeyId == m_pDittoHotKey->m_Atom ||
		m_pDittoHotKey2 && nHotKeyId == m_pDittoHotKey2->m_Atom ||
		m_pDittoHotKey3 && nHotKeyId == m_pDittoHotKey3->m_Atom)
	{
		ShowNoDbMessage();
	}

	bHandled = FALSE;
	return 0;
}
