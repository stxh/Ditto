// HyperLink.h : header file
//
// HyperLink static control. Will open the default browser with the given URL
// when the user clicks on the link.
// Converted from MFC to WTL

#ifndef __HYPERLINK_H__
#define __HYPERLINK_H__

#pragma once

#include <atlbase.h>
#include <atlwin.h>
#include <atlctrls.h>

class CHyperLink : public CWindowImpl<CHyperLink, CStatic>
{
public:
    DECLARE_WND_CLASS_EX(NULL, CS_HREDRAW | CS_VREDRAW, -1)

    CHyperLink();
    virtual ~CHyperLink();

    void SetURL(CString strURL);
    CString GetURL() const;

    void SetColours(COLORREF crLinkColour, COLORREF crVisitedColour,
        COLORREF crHoverColour = (COLORREF)-1);
    COLORREF GetLinkColour() const;
    COLORREF GetVisitedColour() const;
    COLORREF GetHoverColour() const;

    void SetVisited(BOOL bVisited = TRUE);
    BOOL GetVisited() const;

    void SetLinkCursor(HCURSOR hCursor);
    HCURSOR GetLinkCursor() const;

    void SetUnderline(BOOL bUnderline = TRUE);
    BOOL GetUnderline() const;

    void SetAutoSize(BOOL bAutoSize = TRUE);
    BOOL GetAutoSize() const;

    static HINSTANCE GotoURL(LPCTSTR url, int showcmd);

    BOOL PreTranslateMessage(MSG* pMsg);
    void PreSubclassWindow(BOOL bDoSubclass = TRUE);

    void SetHyperLink(LPCTSTR lpstrURL);

    BEGIN_MSG_MAP(CHyperLink)
        MESSAGE_RANGE_HANDLER(WM_MOUSEMOVE, WM_MOUSELEAVE, OnMouseMove)
        MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
        MESSAGE_HANDLER(STM_SETIMAGE, OnSetImage)
        COMMAND_ID_HANDLER(0, OnClick)
        REFLECTED_COMMAND_ID_HANDLER(0, OnClick)
    END_MSG_MAP()

    LRESULT OnMouseMove(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSetCursor(UINT, WPARAM, LPARAM, BOOL& bHandled);
    LRESULT OnSetImage(UINT, WPARAM, LPARAM, BOOL& bHandled);
    LRESULT OnClick(WORD, WORD, HWND, BOOL&);

protected:
    void ReportError(int nError);
    static LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata);
    void PositionWindow();
    void SetDefaultCursor();

    COLORREF m_crLinkColour, m_crVisitedColour;
    COLORREF m_crHoverColour;
    BOOL m_bOverControl;
    BOOL m_bVisited;
    BOOL m_bUnderline;
    BOOL m_bAdjustToFit;
    CString m_strURL;
    CFont m_Font;
    HCURSOR m_hLinkCursor;
    CToolTipCtrl m_ToolTip;
    BOOL m_bIsSubclassed;
};

#endif // __HYPERLINK_H__
