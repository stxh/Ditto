// HyperLink.cpp : implementation file
//
// HyperLink static control. WTL version.
// Converts MFC CHyperLink to WTL CWindowImpl

#include "stdafx.h"
#include "HyperLink.h"
#include "shared/TextConvert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TOOLTIP_ID 1

CHyperLink::CHyperLink()
{
    m_hLinkCursor = NULL;
    m_crLinkColour = RGB(0, 0, 238);
    m_crVisitedColour = RGB(85, 26, 139);
    m_crHoverColour = ::GetSysColor(COLOR_HIGHLIGHT);
    m_bOverControl = FALSE;
    m_bVisited = FALSE;
    m_bUnderline = TRUE;
    m_bAdjustToFit = TRUE;
    m_strURL.Empty();
    m_bIsSubclassed = FALSE;
}

CHyperLink::~CHyperLink()
{
    m_Font.DeleteObject();
}

BOOL CHyperLink::PreTranslateMessage(MSG* pMsg)
{
    m_ToolTip.RelayEvent(pMsg);
    return FALSE;
}

LRESULT CHyperLink::OnClick(WORD, WORD, HWND, BOOL&)
{
    int result = (int)GotoURL(m_strURL, SW_SHOW);
    m_bVisited = (result > HINSTANCE_ERROR);
    if (!m_bVisited) {
        MessageBeep(MB_ICONEXCLAMATION);
        ReportError(result);
    }
    else
        SetVisited();
    return 0;
}

LRESULT CHyperLink::OnSetCursor(UINT, WPARAM, LPARAM, BOOL&)
{
    if (m_hLinkCursor)
    {
        ::SetCursor(m_hLinkCursor);
        return TRUE;
    }
    return FALSE;
}

LRESULT CHyperLink::OnSetImage(UINT, WPARAM, LPARAM, BOOL&)
{
    return 0;
}

LRESULT CHyperLink::OnMouseMove(UINT, WPARAM, LPARAM lParam, BOOL&)
{
    POINT pt = { LOWORD(lParam), HIWORD(lParam) };

    if (m_bOverControl)
    {
        CRect rect;
        GetClientRect(&rect);
        if (!rect.PtInRect(pt))
        {
            m_bOverControl = FALSE;
            ReleaseCapture();
            RedrawWindow();
            return 0;
        }
    }
    else
    {
        m_bOverControl = TRUE;
        RedrawWindow();
        SetCapture();
    }
    return 0;
}

void CHyperLink::PreSubclassWindow(BOOL bDoSubclass)
{
    if (bDoSubclass && !m_bIsSubclassed)
    {
        m_bIsSubclassed = TRUE;
    }

    DWORD dwStyle = GetWindowLong(GWL_STYLE);
    ::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

    if (m_strURL.IsEmpty())
        GetWindowText(m_strURL);

    CString strWndText;
    GetWindowText(strWndText);
    if (strWndText.IsEmpty()) {
        SetWindowText(m_strURL);
    }

    HFONT hFont = GetFont();
    if (hFont)
    {
        LOGFONT lf = { 0 };
        if (GetObject(hFont, sizeof(lf), &lf))
        {
            lf.lfUnderline = m_bUnderline;
            m_Font.CreateFontIndirect(&lf);
            SetFont(m_Font);
        }
    }

    PositionWindow();
    SetDefaultCursor();

    CRect rect;
    GetClientRect(&rect);
    m_ToolTip.Create(m_hWnd);
    m_ToolTip.AddTool(m_hWnd, m_strURL, &rect, TOOLTIP_ID);
}

void CHyperLink::SetURL(CString strURL)
{
    m_strURL = strURL;

    if (::IsWindow(m_hWnd)) {
        PositionWindow();
        m_ToolTip.UpdateTipText(strURL, m_hWnd, TOOLTIP_ID);
    }
}

CString CHyperLink::GetURL() const
{
    return m_strURL;
}

void CHyperLink::SetColours(COLORREF crLinkColour, COLORREF crVisitedColour,
    COLORREF crHoverColour)
{
    m_crLinkColour = crLinkColour;
    m_crVisitedColour = crVisitedColour;

    if (crHoverColour == (COLORREF)-1)
        m_crHoverColour = ::GetSysColor(COLOR_HIGHLIGHT);
    else
        m_crHoverColour = crHoverColour;

    if (::IsWindow(m_hWnd))
        Invalidate();
}

COLORREF CHyperLink::GetLinkColour() const
{
    return m_crLinkColour;
}

COLORREF CHyperLink::GetVisitedColour() const
{
    return m_crVisitedColour;
}

COLORREF CHyperLink::GetHoverColour() const
{
    return m_crHoverColour;
}

void CHyperLink::SetVisited(BOOL bVisited)
{
    m_bVisited = bVisited;

    if (::IsWindow(m_hWnd))
        Invalidate();
}

BOOL CHyperLink::GetVisited() const
{
    return m_bVisited;
}

void CHyperLink::SetLinkCursor(HCURSOR hCursor)
{
    m_hLinkCursor = hCursor;
    if (m_hLinkCursor == NULL)
        SetDefaultCursor();
}

HCURSOR CHyperLink::GetLinkCursor() const
{
    return m_hLinkCursor;
}

void CHyperLink::SetUnderline(BOOL bUnderline)
{
    m_bUnderline = bUnderline;

    if (::IsWindow(m_hWnd))
    {
        HFONT hFont = GetFont();
        if (hFont)
        {
            LOGFONT lf = { 0 };
            if (GetObject(hFont, sizeof(lf), &lf))
            {
                lf.lfUnderline = m_bUnderline;
                m_Font.DeleteObject();
                m_Font.CreateFontIndirect(&lf);
                SetFont(m_Font);
            }
        }
        Invalidate();
    }
}

BOOL CHyperLink::GetUnderline() const
{
    return m_bUnderline;
}

void CHyperLink::SetAutoSize(BOOL bAutoSize)
{
    m_bAdjustToFit = bAutoSize;

    if (::IsWindow(m_hWnd))
        PositionWindow();
}

BOOL CHyperLink::GetAutoSize() const
{
    return m_bAdjustToFit;
}

void CHyperLink::SetHyperLink(LPCTSTR lpstrURL)
{
    SetURL(lpstrURL);
}

void CHyperLink::PositionWindow()
{
    if (!::IsWindow(m_hWnd) || !m_bAdjustToFit)
        return;

    CRect rect;
    GetWindowRect(&rect);

    HWND hParent = GetParent();
    if (hParent)
        ::ScreenToClient(hParent, &rect);

    CString strWndText;
    GetWindowText(strWndText);

    HDC hDC = GetDC();
    HFONT hOldFont = SelectObject(hDC, m_Font);
    SIZE size;
    GetTextExtentPoint32(hDC, strWndText, strWndText.GetLength(), &size);
    SelectObject(hDC, hOldFont);
    ReleaseDC(hDC);

    DWORD dwStyle = GetWindowLong(GWL_STYLE);

    if (dwStyle & SS_CENTERIMAGE)
        rect.DeflateRect(0, (rect.Height() - size.cy) / 2);
    else
        rect.bottom = rect.top + size.cy;

    if (dwStyle & SS_CENTER)
        rect.DeflateRect((rect.Width() - size.cx) / 2, 0);
    else if (dwStyle & SS_RIGHT)
        rect.left = rect.right - size.cx;
    else
        rect.right = rect.left + size.cx;

    SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
}

void CHyperLink::SetDefaultCursor()
{
    if (m_hLinkCursor == NULL)
    {
        CString strWndDir;
        GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
        strWndDir.ReleaseBuffer();

        strWndDir += _T("\\winhlp32.exe");
        HMODULE hModule = LoadLibrary(strWndDir);
        if (hModule) {
            HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
            if (hHandCursor)
                m_hLinkCursor = CopyCursor(hHandCursor);
            FreeLibrary(hModule);
        }
    }
}

LONG CHyperLink::GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
    HKEY hkey;
    LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);

    if (retval == ERROR_SUCCESS) {
        long datasize = MAX_PATH;
        TCHAR data[MAX_PATH];
        RegQueryValue(hkey, NULL, data, &datasize);
        lstrcpy(retdata, data);
        RegCloseKey(hkey);
    }

    return retval;
}

void CHyperLink::ReportError(int nError)
{
    CString str;
    switch (nError) {
    case 0: str = _T("The operating system is out of memory or resources."); break;
    case SE_ERR_PNF: str = _T("The specified path was not found."); break;
    case SE_ERR_FNF: str = _T("The specified file was not found."); break;
    case ERROR_BAD_FORMAT: str = _T("The .EXE file is invalid."); break;
    case SE_ERR_ACCESSDENIED: str = _T("The operating system denied access."); break;
    case SE_ERR_ASSOCINCOMPLETE: str = _T("The filename association is incomplete."); break;
    case SE_ERR_DDEBUSY: str = _T("The DDE transaction could not be completed."); break;
    case SE_ERR_DDEFAIL: str = _T("The DDE transaction failed."); break;
    case SE_ERR_DDETIMEOUT: str = _T("The DDE transaction timed out."); break;
    case SE_ERR_DLLNOTFOUND: str = _T("The specified dynamic-link library was not found."); break;
    case SE_ERR_NOASSOC: str = _T("No application associated with the given extension."); break;
    case SE_ERR_OOM: str = _T("Not enough memory."); break;
    case SE_ERR_SHARE: str = _T("A sharing violation occurred."); break;
    default: str.Format(_T("Unknown Error (%d) occurred."), nError); break;
    }
    str = _T("Unable to open hyperlink:\n\n") + str;
    MessageBox(NULL, str, _T("Error"), MB_ICONEXCLAMATION | MB_OK);
}

HINSTANCE CHyperLink::GotoURL(LPCTSTR url, int showcmd)
{
    TCHAR key[MAX_PATH + MAX_PATH];

    HINSTANCE result = ShellExecute(NULL, _T("open"), url, NULL, NULL, showcmd);

    if ((UINT)(INT_PTR)result <= HINSTANCE_ERROR) {
        if (GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS) {
            lstrcat(key, _T("\\shell\\open\\command"));

            if (GetRegKey(HKEY_CLASSES_ROOT, key, key) == ERROR_SUCCESS) {
                TCHAR* pos;
                pos = _tcsstr(key, _T("\"%1\""));
                if (pos == NULL) {
                    pos = _tcsstr(key, _T("%1"));
                    if (pos == NULL)
                        pos = key + lstrlen(key) - 1;
                    else
                        *pos = '\0';
                }
                else
                    *pos = '\0';

                lstrcat(pos, _T(" "));
                lstrcat(pos, url);

                CStringA keyA = CTextConvert::UnicodeToAnsi(key);
                result = (HINSTANCE)(INT_PTR)WinExec(keyA, showcmd);
            }
        }
    }

    return result;
}
