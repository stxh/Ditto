#include "stdafx.h"
#include "cp_main.h"
#include "Options.h"
#include "ProgressWnd.h"

#define _CRT_NON_CONFORMING_SWPRINTFS 1

LPCTSTR szSection = _T("Settings");
LPCTSTR szEntryX  = _T("ProgressX");
LPCTSTR szEntryY  = _T("ProgressY");

/////////////////////////////////////////////////////////////////////////////
// CProgressWnd

CProgressWnd::CProgressWnd()
{
    CommonConstruct();
}

CProgressWnd::CProgressWnd(HWND hWndParent, LPCTSTR pszTitle, BOOL bSmooth /* = FALSE */)
{
    CommonConstruct();
    m_strTitle = pszTitle;

    Create(hWndParent, pszTitle, bSmooth);
}

void CProgressWnd::CommonConstruct()
{
    m_nNumTextLines  = 4;
    m_nPrevPos       = 0;
    m_nPrevPercent   = 0;
    m_nStep          = 1;
    m_nMinValue      = 0;
    m_nMaxValue      = 100;

    m_strTitle       = _T("Progress");
    m_strCancelLabel = _T(" Cancel ");
    m_bCancelled     = FALSE;
    m_bModal         = FALSE;
    m_hWndParent     = NULL;

    m_bPersistantPosition = TRUE;   // saves and restores position automatically
}

CProgressWnd::~CProgressWnd()
{
    if (::IsWindow(m_hWnd))
        DestroyWindow();
}

BOOL CProgressWnd::Create(HWND hWndParent, LPCTSTR pszTitle, BOOL bSmooth /* = FALSE */)
{
    m_strTitle = pszTitle;
    m_hWndParent = hWndParent;

    // Get the system window message font for use in the cancel button and text area
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
    m_font.CreateFontIndirect(&(ncm.lfMessageFont));

    if (!::IsWindow(hWndParent))
        hWndParent = ::GetActiveWindow();

    // Create popup window
    HWND hWnd = Create(hWndParent,
                        CRect(0, 0, 390, 130),           // position/size - updated soon
                        pszTitle,                         // Title
                        WS_POPUP | WS_BORDER | WS_CAPTION,
                        WS_EX_DLGMODALFRAME);
    if (hWnd == NULL) return FALSE;

    // Now create the controls
    CRect TempRect(0, 0, 10, 10);

    if (!m_Text.Create(m_hWnd, TempRect, _T(""),
                       WS_CHILD | WS_VISIBLE | SS_NOPREFIX | SS_LEFTNOWORDWRAP,
                       0, PROGRESS_IDC_TEXT))
        return FALSE;

    DWORD dwProgressStyle = WS_CHILD | WS_VISIBLE;
    if (bSmooth)
        dwProgressStyle |= PBS_SMOOTH;

    if (!m_wndProgress.Create(m_hWnd, TempRect, NULL, dwProgressStyle, 0, PROGRESS_IDC_PROGRESS))
        return FALSE;

    if (!m_CancelButton.Create(m_hWnd, TempRect, m_strCancelLabel,
                               WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                               0, PROGRESS_IDC_CANCEL))
        return FALSE;

    m_CancelButton.SetFont(m_font, TRUE);
    m_Text.SetFont(m_font, TRUE);

    // Resize the whole thing according to the number of text lines, desired window
    // width and current font.
    SetWindowSize(m_nNumTextLines, 390);

    // Center and show window
    if (m_bPersistantPosition)
        GetPreviousSettings();
    else
        CenterOnParent();

    Show();

    return TRUE;
}

BOOL CProgressWnd::GoModal(LPCTSTR pszTitle /*=_T("Progress")*/, BOOL bSmooth /*=FALSE*/)
{
    if (!::IsWindow(m_hWnd) && !Create(m_hWndParent, pszTitle, bSmooth))
        return FALSE;

    // Disable main window
    if (::IsWindow(m_hWndParent))
        ::EnableWindow(m_hWndParent, FALSE);

    // Re-enable this window
    EnableWindow(TRUE);

    m_bModal = TRUE;

    return TRUE;
}

void CProgressWnd::SetWindowSize(int nNumTextLines, int nWindowWidth /*=390*/)
{
    int nMargin = 10;
    CSize EdgeSize(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));

    CRect TextRect, CancelRect, ProgressRect;
    CSize CancelSize(0, 0);

    // Set up a default size for the text area in case things go wrong
    TextRect.SetRect(nMargin, nMargin, nWindowWidth - 2 * nMargin, 100 + 2 * nMargin);

    // Get DrawText to tell us how tall the text area will be (while we're at
    // it, we'll see how big the word "Cancel" is)
    CDCHandle dc = GetDC();
    if (dc) {
        HFONT hOldFont = dc.SelectFont(m_font);
        CString str = _T("M");
        for (int i = 0; i < nNumTextLines - 1; i++) str += _T("\nM");
        dc.DrawText(str, str.GetLength(), TextRect, DT_CALCRECT | DT_NOCLIP | DT_NOPREFIX);
        TextRect.right = TextRect.left + nWindowWidth;

        CString strCancel = m_strCancelLabel + _T("  ");
        CSize textSize(0, 0);
        dc.GetTextExtent(strCancel, strCancel.GetLength(), &textSize);
        CancelSize.cx = textSize.cx + EdgeSize.cx * 4;
        CancelSize.cy = textSize.cy + EdgeSize.cy * 3;

        dc.SelectFont(hOldFont);
        ReleaseDC(dc);
    }

    // Work out how big (and where) the cancel button should be
    CancelRect.SetRect(TextRect.right - CancelSize.cx, TextRect.bottom + nMargin,
                       TextRect.right, TextRect.bottom + nMargin + CancelSize.cy);

    // Work out how big (and where) the progress control should be
    ProgressRect.SetRect(TextRect.left, CancelRect.top + EdgeSize.cy,
                         CancelRect.left - nMargin, CancelRect.bottom - EdgeSize.cy);

    // Resize the main window to fit the controls
    CSize ClientSize(nMargin + TextRect.Width() + nMargin,
                     nMargin + TextRect.Height() + nMargin + CancelRect.Height() + nMargin);

    CRect WndRect, ClientRect;
    GetWindowRect(WndRect); GetClientRect(ClientRect);
    WndRect.right = WndRect.left + WndRect.Width() - ClientRect.Width() + ClientSize.cx;
    WndRect.bottom = WndRect.top + WndRect.Height() - ClientRect.Height() + ClientSize.cy;
    MoveWindow(WndRect);

    // Now reposition the controls...
    m_wndProgress.MoveWindow(ProgressRect);
    m_CancelButton.MoveWindow(CancelRect);
    m_Text.MoveWindow(TextRect);
}

void CProgressWnd::Clear()
{ 
    SetText(_T(""));
    SetPos(0);
    m_bCancelled = FALSE; 
    m_nPrevPos = 0;

    if (::IsWindow(m_hWnd))
        UpdateWindow();
}

void CProgressWnd::Hide()
{ 
    if (!::IsWindow(m_hWnd)) 
        return;

    if (IsWindowVisible())
    {
        ShowWindow(SW_HIDE);
        ModifyStyle(WS_VISIBLE, 0);
    }
}

void CProgressWnd::Show()
{ 
    if (!::IsWindow(m_hWnd))
        return;

    if (!IsWindowVisible())
    {
        ModifyStyle(0, WS_VISIBLE);
        ShowWindow(SW_SHOWNA);
        RedrawWindow(NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE);
    }
}

void CProgressWnd::SetRange(int nLower, int nUpper, int nStep /* = 1 */)
{
    if (!::IsWindow(m_hWnd)) 
        return;

    m_wndProgress.SetRange32(nLower, nUpper);

    m_nMaxValue = nUpper;
    m_nMinValue = nLower;
    m_nStep     = nStep;

    m_wndProgress.SetStep(nStep);
}

int CProgressWnd::OffsetPos(int nPos)
{ 
    if (!::IsWindow(m_hWnd)) 
        return m_nPrevPos;

    Show();

    return SetPos(m_nPrevPos + nPos);  
}

int CProgressWnd::StepIt()
{
    if (!::IsWindow(m_hWnd)) 
        return m_nPrevPos;

    Show();

    return SetPos(m_nPrevPos + m_nStep); 
}

int CProgressWnd::SetStep(int nStep)
{
    int nOldStep = m_nStep;
    m_nStep = nStep;
    if (!::IsWindow(m_hWnd)) 
        return nOldStep;

    return m_wndProgress.SetStep(nStep); 
}

int CProgressWnd::SetPos(int nPos)
{
    if (!::IsWindow(m_hWnd)) 
        return m_nPrevPos;

    Show();

    m_nPrevPos = nPos;

    CString strTitle;
    strTitle.Format(_T("%s"), m_strTitle);
    SetWindowText(strTitle);

    return m_wndProgress.SetPos(nPos);        
}

void CProgressWnd::SetText(LPCTSTR fmt, ...)
{
    if (!::IsWindow(m_hWnd)) 
        return;

    va_list args;
    TCHAR buffer[512];

    va_start(args, fmt);
    _vstprintf(buffer, fmt, args);
    va_end(args);

    m_Text.SetWindowText(buffer);
}

/////////////////////////////////////////////////////////////////////////////
// CProgressWnd message handlers

LRESULT CProgressWnd::OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
    CDCHandle dc((HDC)wParam);

    // Fill background with the dialog face colour
    CBrush backBrush;
    backBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
    HBRUSH hOldBrush = dc.SelectBrush(backBrush);

    CRect rect;
    dc.GetClipBox(&rect);       // Erase the area needed
    dc.PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);

    dc.SelectBrush(hOldBrush);

    return TRUE;
}

LRESULT CProgressWnd::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    DestroyWindow();
    return 0;
}

LRESULT CProgressWnd::OnCancelClick(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    OnCancel();
    return 0;
}

void CProgressWnd::OnCancel()
{
    m_bCancelled = TRUE;
    Hide();

    if (m_bModal)
        DestroyWindow();

    if (::IsWindow(m_hWndParent))
        ::SetForegroundWindow(m_hWndParent);
}


BOOL CProgressWnd::DestroyWindow()
{
    if (m_bPersistantPosition)
        SaveCurrentSettings();

    if (m_bModal)
    {
        m_bModal = FALSE;

        if (::IsWindow(m_hWndParent))
            ::EnableWindow(m_hWndParent, TRUE);
    }

    HWND hWnd = m_hWnd;
    m_hWnd = NULL;
    m_Text.Detach();
    m_wndProgress.Detach();
    m_CancelButton.Detach();

    return ::DestroyWindow(hWnd);
}

void CProgressWnd::PeekAndPump(BOOL bCancelOnESCkey /*= TRUE*/)
{
    if (m_bModal && ::GetFocus() != m_hWnd)
        SetFocus();

    MSG msg;
    while (!m_bCancelled && ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            ::PostQuitMessage(0);
            return;
        }

        // Cancel button disabled if modal, so we fake it.
        if (bCancelOnESCkey && (msg.message == WM_CHAR) && (msg.wParam == VK_ESCAPE))
            OnCancel();

        if (m_bModal && (msg.message == WM_LBUTTONUP))
        {
            CRect rect;
            m_CancelButton.GetWindowRect(rect);
            if (rect.PtInRect(msg.pt))
                OnCancel();
        }

        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

// Restores the previous window size from the settings
void CProgressWnd::GetPreviousSettings()
{
    int x = (int)CGetSetOptions::GetProfileLong(szEntryX, -1);
    int y = (int)CGetSetOptions::GetProfileLong(szEntryY, -1);

    if (x >= 0 && x < GetSystemMetrics(SM_CXSCREEN) &&
        y >= 0 && y < GetSystemMetrics(SM_CYSCREEN))
    {
        SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }
    else
        CenterOnParent();
}

// Saves the current window position to the settings
void CProgressWnd::SaveCurrentSettings()
{   
    if (!::IsWindow(m_hWnd))
        return;

    CRect rect;
    GetWindowRect(rect);

    CGetSetOptions::SetProfileLong(szEntryX, rect.left);
    CGetSetOptions::SetProfileLong(szEntryY, rect.top);
}

void CProgressWnd::CenterOnParent()
{
    CRect rcWnd;
    GetWindowRect(&rcWnd);

    RECT rcParent = { 0 };
    if (::IsWindow(m_hWndParent))
        ::GetWindowRect(m_hWndParent, &rcParent);
    else
        ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcParent, 0);

    int x = rcParent.left + ((rcParent.right - rcParent.left) - rcWnd.Width()) / 2;
    int y = rcParent.top + ((rcParent.bottom - rcParent.top) - rcWnd.Height()) / 2;

    SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void CProgressWnd::HideCancel()
{
	m_CancelButton.ShowWindow(SW_HIDE);
}

void CProgressWnd::ShowCancel()
{
	m_CancelButton.ShowWindow(SW_SHOW);
}

void CProgressWnd::SetTitleText(CString csTitle)
{
	m_strTitle = csTitle;
	
	SetWindowText(m_strTitle);
}
