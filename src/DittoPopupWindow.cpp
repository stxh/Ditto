#include "stdafx.h"
#include "CP_Main.h"
#include "DittoPopupWindow.h"


CDittoPopupWindow::CDittoPopupWindow()
{
	m_groupId = -1;
}


CDittoPopupWindow::~CDittoPopupWindow()
{
}

int CDittoPopupWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWndEx::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	CWndEx::SetCaptionOn(CAPTION_TOP, false, CGetSetOptions::m_Theme.GetCaptionSize(), CGetSetOptions::m_Theme.GetCaptionFontSize());
		
	m_font.CreateFont(-m_DittoWindow.m_dpi.Scale(12), 0, 0, 0, 400, 0, 0, 0, DEFAULT_CHARSET, 3, 2, 1, 34, _T("MS Sans Serif"));
	CRect rcZero(0, 0, 0, 0);
	m_textLabel.Create(m_hWnd, rcZero, _T("test"), WS_CHILD | WS_VISIBLE, 0, 0xffff);
	m_textLabel.SetFont(m_font);
	

	m_progressWnd.Create(m_hWnd, rcZero, NULL, WS_CHILD | PBS_SMOOTH | WS_VISIBLE, 0, 2);

	m_progressWnd.SetRange(0, 100);

	SetWindowText(_T("Running Ditto Updates"));

	//m_textLabel.SetWindowText(_T("test"));

	return 0;
}

void CDittoPopupWindow::UpdateText(CString text)
{
	m_textLabel.SetWindowText(text);

	CRect rect;
	m_textLabel.GetWindowRect(&rect);
	ScreenToClient(&rect);
	InvalidateRect(&rect);
	UpdateWindow();

	PumpMessages();
}


void CDittoPopupWindow::SetProgressBarPercent(int percent)
{
	if(::IsWindowVisible(m_progressWnd.m_hWnd) == FALSE)
	{
		m_progressWnd.ShowWindow(SW_SHOW);

		CRect size;
		GetClientRect(size);
		DoSize(size.Width(), size.Height());
	}
	m_progressWnd.SetPos(percent);
	PumpMessages();
}

void CDittoPopupWindow::HideProgressBar()
{
	m_progressWnd.ShowWindow(SW_HIDE);
	PumpMessages();

	CRect size;
	GetClientRect(size);
	DoSize(size.Width(), size.Height());
}

LRESULT CDittoPopupWindow::OnSize(UINT nType, CSize size)
{
	DoSize(size.cx, size.cy);
	return 0;
}

void CDittoPopupWindow::DoSize(int cx, int cy)
{
	if(m_textLabel.m_hWnd != NULL)
	{
		int bottom = 0;
		if(::IsWindowVisible(m_progressWnd.m_hWnd))
		{
			bottom = 50;
		}
		m_textLabel.MoveWindow(10, 10, cx-20, cy-bottom);
		this->Invalidate();
	}

	if(m_progressWnd.m_hWnd != NULL)
	{
		m_progressWnd.MoveWindow(10, cy-40, cx-20, 30);
	}	
}

LRESULT CDittoPopupWindow::OnCtlColorStatic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (::GetDlgCtrlID((HWND)lParam) == 0xffff)
	{
		::SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)::GetStockObject(NULL_BRUSH);
	}

	bHandled = FALSE;
	return 0;
}

void CDittoPopupWindow::PumpMessages()
{
	int nLoops = 0;
	MSG msg;
	while (::PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		nLoops++;
		if (nLoops > 100)
			break;
	}
}

void CDittoPopupWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_groupId > 0)
	{
		CWindow wndParent = GetParent();
		if(wndParent.m_hWnd != NULL)
		{
			wndParent.PostMessage(WM_SHOW_DITTO_GROUP, m_groupId, 0);
		}
	}
}