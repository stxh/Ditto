// FocusHighlightDlg.cpp : WTL dialog implementation

#include "stdafx.h"
#include "resource.h"
#include "FocusHighlightDlg.h"

CFocusHighlightDlg::CFocusHighlightDlg()
{
}

LRESULT CFocusHighlightDlg::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
    m_activeEdit.Attach(GetDlgItem(IDC_EDIT_ACTIVE));
    m_focusEdit.Attach(GetDlgItem(IDC_EDIT_FOCUS));
    m_trackFocusChangesCheck.Attach(GetDlgItem(IDC_CHECK_TRACK_FOCUS_CHANGES));

    StayOnTop();

    m_trackFocusChangesCheck.SetCheck(BST_CHECKED);
    SetTimer(1, 2000);

    CenterWindow();

    return TRUE;
}

LRESULT CFocusHighlightDlg::OnTimer(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
    if (wParam == 1 && m_trackFocusChangesCheck.GetCheck() != 0)
    {
        m_tracker.TrackActiveWnd();

        CString cs;
        cs.Format(_T("0x%08x - %s"), m_tracker.ActiveWnd(), m_tracker.ActiveWndName());
        m_activeEdit.SetWindowText(cs);

        cs.Format(_T("0x%08x - %s"), m_tracker.FocusWnd(), m_tracker.FocusWndName());
        m_focusEdit.SetWindowText(cs);
    }

    bHandled = TRUE;
    return 0;
}

LRESULT CFocusHighlightDlg::OnHighlightActive(WORD, WORD, HWND, BOOL&)
{
    HighlightWindow(m_tracker.ActiveWnd());
    return 0;
}

LRESULT CFocusHighlightDlg::OnHighlightFocus(WORD, WORD, HWND, BOOL&)
{
    HighlightWindow(m_tracker.FocusWnd());
    return 0;
}

LRESULT CFocusHighlightDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND, BOOL&)
{
    KillTimer(1);
    EndDialog(wID);
    return 0;
}

void CFocusHighlightDlg::StayOnTop()
{
    CRect rect;
    GetWindowRect(rect);
    SetWindowPos(HWND_TOPMOST, rect, SWP_SHOWWINDOW);
}

void CFocusHighlightDlg::HighlightWindow(HWND hWnd)
{
    if (!::IsWindow(hWnd))
        return;

    CRect cr;
    ::GetWindowRect(hWnd, &cr);
    ::ScreenToClient(hWnd, &cr.TopLeft());
    ::ScreenToClient(hWnd, &cr.BottomRight());

    HDC hdc = ::GetWindowDC(hWnd);
    if (hdc == NULL)
        return;

    for (int i = 0; i < 10; i++)
    {
        ::DrawFocusRect(hdc, cr);
        ::Sleep(200);
    }

    ::ReleaseDC(hWnd, hdc);
}
