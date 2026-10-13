// Filename: ScrollHelper.h
// S.Chan, 01 Jul 2005

#ifndef SCROLL_HELPER_INCLUDED
#define SCROLL_HELPER_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CScrollHelper
{
public:
    CScrollHelper();
    ~CScrollHelper();

    void   AttachWnd(HWND hWnd);
    void   DetachWnd();

    void   SetDisplaySize(int displayWidth, int displayHeight, double zoomScale);
    const CSize& GetDisplaySize() const;

    const CSize& GetScrollPos() const;

    const CSize& GetPageSize() const;

    void   ScrollToOrigin(bool scrollLeft, bool scrollTop);

    void   OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    void   OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    BOOL   OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	BOOL   OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
    void   OnSize(UINT nType, int cx, int cy);

	BOOL   Update(CPoint changes);

private:
    int    Get32BitScrollPos(int bar, HWND hScrollBar);
    void   UpdateScrollInfo();
    void   UpdateScrollBar(int bar, int windowSize, int displaySize,
                           LONG& pageSize, LONG& scrollPos, LONG& deltaPos);

    HWND   m_attachWnd;
    CSize  m_pageSize;
    CSize  m_displaySize;
    CSize  m_scrollPos;
	double m_zoomScale;
};

#endif // SCROLL_HELPER_INCLUDED

// END