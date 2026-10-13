// TrayWnd.cpp : implementation file
//

#include "stdafx.h"
#include "CP_Main.h"
#include "TrayWnd.h"


// CTrayWnd

UINT WM_TASKBARCREATED = RegisterWindowMessage(_T("TaskbarCreated"));

CTrayWnd::CTrayWnd()
{
}

CTrayWnd::~CTrayWnd()
{
}

LRESULT CTrayWnd::OnTaskBarCreated(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(theApp.m_pMainFrame != NULL)
	{
		theApp.m_pMainFrame->PostMessage(WM_READD_TASKBAR_ICON, 0, 0);
	}
	
	return TRUE;
}


