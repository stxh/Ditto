// Filename: ScrollHelper.cpp
// 2005-07-01 nschan Initial revision.
// 2005-09-08 nschan Added GetClientRectSB() function.

#include "stdafx.h"
#include "ScrollHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static void GetClientRectSB(HWND hWnd, CRect& rect)
{
    ASSERT( hWnd != NULL );

    CRect winRect;
    ::GetWindowRect(hWnd, &winRect);
    ::ScreenToClient(hWnd, &winRect);

    ::GetClientRect(hWnd, &rect);

    int cxSB = ::GetSystemMetrics(SM_CXVSCROLL);
    int cySB = ::GetSystemMetrics(SM_CYHSCROLL);

    if ( winRect.right >= (rect.right + cxSB) )
        rect.right += cxSB;
    if ( winRect.bottom >= (rect.bottom + cySB) )
        rect.bottom += cySB;
}

// CScrollHelper /////////////////////////////////////////////////////////////////////

CScrollHelper::CScrollHelper()
{
    m_attachWnd   = NULL;
    m_pageSize    = CSize(0,0);
    m_displaySize = CSize(0,0);
    m_scrollPos   = CSize(0,0);
	m_zoomScale = 1;
}

CScrollHelper::~CScrollHelper()
{
    DetachWnd();
}

void CScrollHelper::AttachWnd(HWND hWnd)
{
    m_attachWnd = hWnd;
}

void CScrollHelper::DetachWnd()
{
    m_attachWnd = NULL;
}

void CScrollHelper::SetDisplaySize(int displayWidth, int displayHeight, double zoomScale)
{
	m_zoomScale = zoomScale;
	int cxSB = ::GetSystemMetrics(SM_CXVSCROLL);
	int cySB = ::GetSystemMetrics(SM_CYHSCROLL);	

	CRect rect;
	GetClientRectSB(m_attachWnd, rect);
	double width = (rect.Width() * (1 / m_zoomScale)) + .5;
	double height = (rect.Height() * (1 / m_zoomScale)) + .5;
	CSize windowSize(width, height);

	if (windowSize.cx >= displayWidth)
	{
		cxSB = 0;
	}

	if (windowSize.cy >= displayHeight)
	{
		cySB = 0;
	}

    m_displaySize = CSize((displayWidth) + cxSB, (displayHeight) + cySB);

    if ( m_attachWnd != NULL && ::IsWindow(m_attachWnd) )
        UpdateScrollInfo();
}

const CSize& CScrollHelper::GetDisplaySize() const
{
    return m_displaySize;
}

const CSize& CScrollHelper::GetScrollPos() const
{
    return m_scrollPos;
}

const CSize& CScrollHelper::GetPageSize() const
{
    return m_pageSize;
}

void CScrollHelper::ScrollToOrigin(bool scrollLeft, bool scrollTop)
{
    if ( m_attachWnd == NULL )
        return;

    if ( scrollLeft )
    {
        if ( m_displaySize.cx > 0 && m_pageSize.cx > 0 && m_scrollPos.cx > 0 )
        {
            int deltaPos = -m_scrollPos.cx;
            m_scrollPos.cx += deltaPos;
            ::SetScrollPos(m_attachWnd, SB_HORZ, m_scrollPos.cx, TRUE);
            ::ScrollWindow(m_attachWnd, -deltaPos, 0, NULL, NULL);
        }
    }

    if ( scrollTop )
    {
        if ( m_displaySize.cy > 0 && m_pageSize.cy > 0 && m_scrollPos.cy > 0 )
        {
            int deltaPos = -m_scrollPos.cy;
            m_scrollPos.cy += deltaPos;
            ::SetScrollPos(m_attachWnd, SB_VERT, m_scrollPos.cy, TRUE);
            ::ScrollWindow(m_attachWnd, 0, -deltaPos, NULL, NULL);
        }
    }
}

void CScrollHelper::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    if ( m_attachWnd == NULL )
        return;

    const int lineOffset = 60;

    int deltaPos = 0;
    switch( nSBCode )
    {
    case SB_LINELEFT:
        deltaPos = -lineOffset;
        break;

    case SB_LINERIGHT:
        deltaPos = lineOffset;
        break;

    case SB_PAGELEFT:
        deltaPos = -m_pageSize.cx;
        break;

    case SB_PAGERIGHT:
        deltaPos = m_pageSize.cx;
        break;

    case SB_THUMBTRACK:
        deltaPos = Get32BitScrollPos(SB_HORZ, pScrollBar ? pScrollBar->m_hWnd : NULL) - m_scrollPos.cx;
        break;

    case SB_THUMBPOSITION:
        deltaPos = Get32BitScrollPos(SB_HORZ, pScrollBar ? pScrollBar->m_hWnd : NULL) - m_scrollPos.cx;
        break;

    default:
        return;
    }

    int newScrollPos = m_scrollPos.cx + deltaPos;

    if ( newScrollPos < 0 )
        deltaPos = -m_scrollPos.cx;

    int maxScrollPos = m_displaySize.cx - m_pageSize.cx;
    if ( newScrollPos > maxScrollPos )
        deltaPos = maxScrollPos - m_scrollPos.cx;

    if ( deltaPos != 0 )
    {
        m_scrollPos.cx += deltaPos;
        ::SetScrollPos(m_attachWnd, SB_HORZ, m_scrollPos.cx, TRUE);
        ::ScrollWindow(m_attachWnd, -deltaPos, 0, NULL, NULL);
		::InvalidateRect(m_attachWnd, NULL, FALSE);
    }
}

void CScrollHelper::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    if ( m_attachWnd == NULL )
        return;

    const int lineOffset = 60;

    int deltaPos = 0;
    switch( nSBCode )
    {
    case SB_LINEUP:
        deltaPos = -lineOffset;
        break;

    case SB_LINEDOWN:
        deltaPos = lineOffset;
        break;

    case SB_PAGEUP:
        deltaPos = -m_pageSize.cy;
        break;

    case SB_PAGEDOWN:
        deltaPos = m_pageSize.cy;
        break;

    case SB_THUMBTRACK:
        deltaPos = Get32BitScrollPos(SB_VERT, pScrollBar ? pScrollBar->m_hWnd : NULL) - m_scrollPos.cy;
        break;

    case SB_THUMBPOSITION:
        deltaPos = Get32BitScrollPos(SB_VERT, pScrollBar ? pScrollBar->m_hWnd : NULL) - m_scrollPos.cy;
        break;

    default:
        return;
    }

    int newScrollPos = m_scrollPos.cy + deltaPos;

    if ( newScrollPos < 0 )
        deltaPos = -m_scrollPos.cy;

    int maxScrollPos = m_displaySize.cy - m_pageSize.cy;
    if ( newScrollPos > maxScrollPos )
        deltaPos = maxScrollPos - m_scrollPos.cy;

    if ( deltaPos != 0 )
    {
        m_scrollPos.cy += deltaPos;
        ::SetScrollPos(m_attachWnd, SB_VERT, m_scrollPos.cy, TRUE);
        ::ScrollWindow(m_attachWnd, 0, -deltaPos, NULL, NULL);
		::InvalidateRect(m_attachWnd, NULL, FALSE);
    }
}

BOOL CScrollHelper::Update(CPoint changes)
{
	int deltaYPos = changes.y;

	int newYScrollPos = m_scrollPos.cy + deltaYPos;

	if (newYScrollPos < 0)
		deltaYPos = -m_scrollPos.cy;

	int maxYScrollPos = m_displaySize.cy - m_pageSize.cy;
	if (newYScrollPos > maxYScrollPos)
		deltaYPos = maxYScrollPos - m_scrollPos.cy;

	if (deltaYPos != 0)
	{
		m_scrollPos.cy += deltaYPos;
		::SetScrollPos(m_attachWnd, SB_VERT, m_scrollPos.cy, TRUE);
	}

	int deltaXPos = changes.x;

	int newXScrollPos = m_scrollPos.cx + deltaXPos;

	if (newXScrollPos < 0)
		deltaXPos = -m_scrollPos.cx;

	int maxXScrollPos = m_displaySize.cx - m_pageSize.cx;
	if (newXScrollPos > maxXScrollPos)
		deltaXPos = maxXScrollPos - m_scrollPos.cx;

	if (deltaXPos != 0)
	{
		m_scrollPos.cx += deltaXPos;
		::SetScrollPos(m_attachWnd, SB_HORZ, m_scrollPos.cx, TRUE);
	}
		
	::ScrollWindow(m_attachWnd, -deltaXPos, -deltaYPos, NULL, NULL);

	return 1;
}

BOOL CScrollHelper::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    if ( m_attachWnd == NULL )
        return FALSE;

    int scrollMin = 0, scrollMax = 0;
    ::GetScrollRange(m_attachWnd, SB_VERT, &scrollMin, &scrollMax);
    if ( scrollMin == scrollMax )
        return FALSE;

    int numScrollIncrements = abs(zDelta) / WHEEL_DELTA;

    int numScrollLinesPerIncrement = 0;
    ::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &numScrollLinesPerIncrement, 0);

    if ( numScrollLinesPerIncrement == WHEEL_PAGESCROLL )
    {
        OnVScroll(zDelta > 0 ? SB_PAGEUP : SB_PAGEDOWN, 0, NULL);
        return TRUE;
    }

    int numScrollLines = numScrollIncrements * numScrollLinesPerIncrement;

    numScrollLines = max(numScrollLines/3, 1);

    for(int i = 0; i < numScrollLines; ++i)
    {
        OnVScroll(zDelta > 0 ? SB_LINEUP : SB_LINEDOWN, 0, NULL);
    }

    return TRUE;
}

BOOL CScrollHelper::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_attachWnd == NULL)
		return FALSE;

	int scrollMin = 0, scrollMax = 0;
	::GetScrollRange(m_attachWnd, SB_VERT, &scrollMin, &scrollMax);
	if (scrollMin == scrollMax)
		return FALSE;

	int numScrollIncrements = abs(zDelta) / WHEEL_DELTA;

	int numScrollLinesPerIncrement = 0;
	::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &numScrollLinesPerIncrement, 0);

	if (numScrollLinesPerIncrement == WHEEL_PAGESCROLL)
	{
		OnHScroll(zDelta > 0 ? SB_PAGEUP : SB_PAGEDOWN, 0, NULL);
		return TRUE;
	}

	int numScrollLines = numScrollIncrements * numScrollLinesPerIncrement;

	numScrollLines = max(numScrollLines / 3, 1);

	for (int i = 0; i < numScrollLines; ++i)
	{
		OnHScroll(zDelta > 0 ? SB_LINEUP : SB_LINEDOWN, 0, NULL);
	}

	return TRUE;
}

void CScrollHelper::OnSize(UINT nType, int cx, int cy)
{
    UpdateScrollInfo();
}

int CScrollHelper::Get32BitScrollPos(int bar, HWND hScrollBar)
{
    ASSERT( m_attachWnd != NULL );
    HWND hWndScroll;
    if ( hScrollBar == NULL )
        hWndScroll = m_attachWnd;
    else
        hWndScroll = hScrollBar;

    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_TRACKPOS;
    ::GetScrollInfo(hWndScroll, bar, &si);

    int scrollPos = si.nTrackPos;

    return scrollPos;
}

void CScrollHelper::UpdateScrollInfo()
{
    if ( m_attachWnd == NULL )
        return;

    CRect rect;
    GetClientRectSB(m_attachWnd, rect);
	double width = (rect.Width() * (1 / m_zoomScale)) + .5;
	double height = (rect.Height() * (1 / m_zoomScale)) + .5;
    CSize windowSize(width, height * (1/m_zoomScale));

    CSize deltaPos(0,0);
    UpdateScrollBar(SB_HORZ, windowSize.cx, m_displaySize.cx,
        m_pageSize.cx, m_scrollPos.cx, deltaPos.cx);

    UpdateScrollBar(SB_VERT, windowSize.cy, m_displaySize.cy,
        m_pageSize.cy, m_scrollPos.cy, deltaPos.cy);

    if ( deltaPos.cx != 0 || deltaPos.cy != 0 )
    {
        ::ScrollWindow(m_attachWnd, deltaPos.cx, deltaPos.y, NULL, NULL);
    }                                                        
}

void CScrollHelper::UpdateScrollBar(int bar, int windowSize, int displaySize,
                                    LONG& pageSize, LONG& scrollPos, LONG& deltaPos)
{
    int scrollMax = 0;
    deltaPos = 0;
    if ( windowSize < displaySize )
    {
        scrollMax = displaySize - 1;
        if ( pageSize > 0 && scrollPos > 0 )
        {
        }
        pageSize = windowSize;
		scrollPos = min(scrollPos, displaySize - pageSize - 1);
        deltaPos = ::GetScrollPos(m_attachWnd, bar) - scrollPos;
    }
    else
    {
        pageSize = 0;
        scrollPos = 0;
        deltaPos = ::GetScrollPos(m_attachWnd, bar);
    }

    SCROLLINFO si;
    memset(&si, 0, sizeof(SCROLLINFO));
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask  = SIF_ALL;
    si.nMin   = 0;
    si.nMax   = scrollMax;
    si.nPage  = pageSize;
    si.nPos   = scrollPos;
    ::SetScrollInfo(m_attachWnd, bar, &si, TRUE);
}

// END