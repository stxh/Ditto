// WndEx.cpp : implementation file
//

#include "stdafx.h"
#include "CP_Main.h"
#include "WndEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWndEx

#define CLOSE_WIDTH			12
#define CLOSE_HEIGHT		11
#define CLOSE_BORDER		2
#define TIMER_AUTO_MAX		5
#define TIMER_BUTTON_UP		6

CWndEx::CWndEx()
{	
	SetCaptionColorActive(false, TRUE);
	m_lDelayMaxSeconds = 2;
}

CWndEx::~CWndEx()
{
}

void CWndEx::InvalidateNc()
{
	::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
}

void CWndEx::GetWindowRectEx(LPRECT lpRect)
{
	if(m_DittoWindow.m_bMinimized)
	{
		*lpRect = m_DittoWindow.m_crFullSizeWindow;
		return;
	}
	
	::GetWindowRect(m_hWnd, lpRect);
}

BOOL CWndEx::Create(const CRect& crStart, CWindow* pParentWnd)
{
	HWND hWndParent = (pParentWnd != NULL) ? pParentWnd->m_hWnd : NULL;

	CRect rc = crStart;
	return CWindowImpl<CWndEx, CWindow, CNullTraits>::Create(hWndParent, &rc,
		_T("Quick Paste"), WS_POPUP, 0, 0U, NULL) != NULL;
}

int CWndEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_DittoWindow.DoCreate(this);
	m_DittoWindow.m_bDrawMinimize = false;
	m_DittoWindow.m_bDrawMaximize = false;

	SetWindowPos(NULL,
		lpCreateStruct->x,
		lpCreateStruct->y,
		m_DittoWindow.m_dpi.Scale(lpCreateStruct->cx),
		m_DittoWindow.m_dpi.Scale(lpCreateStruct->cy),
		SWP_NOZORDER | SWP_NOACTIVATE);

	SetCaptionColorActive(false, TRUE);
	m_DittoWindow.SetCaptionOn(this, CGetSetOptions::GetCaptionPos(), true, CGetSetOptions::m_Theme.GetCaptionSize(), CGetSetOptions::m_Theme.GetCaptionFontSize());
	SetAutoMaxDelay(CGetSetOptions::GetAutoMaxDelay());

	m_toolTip.Create(m_hWnd);
	CRect r;
	::GetWindowRect(m_hWnd, &r);
	POINT ptLT = r.TopLeft(), ptRB = r.BottomRight();
	::ScreenToClient(m_hWnd, &ptLT);
	::ScreenToClient(m_hWnd, &ptRB);
	r.SetRect(ptLT, ptRB);
	r.InflateRect(m_DittoWindow.m_dpi.Scale(25), m_DittoWindow.m_dpi.Scale(25));
	m_toolTip.AddTool(m_hWnd, _T("Ditto"), &r, 1);

	theApp.GetMessageLoop()->AddMessageFilter(this);

	return 0;
}

void CWndEx::OnDestroy()
{
	theApp.GetMessageLoop()->RemoveMessageFilter(this);
}

// The message loop calls every filter for every message, while MFC only reached a
// window through the focused control's parent chain.  Rebuild that restriction here.
bool CWndEx::IsMsgForThisWindowTree(MSG* pMsg)
{
	if (!::IsWindow(m_hWnd))
	{
		return false;
	}

	HWND hWnd = ::GetFocus();
	if (hWnd == NULL)
	{
		hWnd = pMsg->hwnd;
	}

	while (hWnd != NULL && hWnd != m_hWnd)
	{
		hWnd = ::GetParent(hWnd);
	}

	return hWnd == m_hWnd;
}

bool CWndEx::SetCaptionColorActive(BOOL bPersistant, BOOL ConnectedToClipboard)
{
	bool bResult;

	if(ConnectedToClipboard == false)
	{
		bResult = m_DittoWindow.SetCaptionColors(CGetSetOptions::m_Theme.CaptionLeftNotConnected(), CGetSetOptions::m_Theme.CaptionRightNotConnected(), CGetSetOptions::m_Theme.BorderNotConnected());
	}
	else
	{
		if(bPersistant)
		{
			bResult = m_DittoWindow.SetCaptionColors(CGetSetOptions::m_Theme.CaptionLeftTopMost(), CGetSetOptions::m_Theme.CaptionRightTopMost(), CGetSetOptions::m_Theme.BorderTopMost());
		}
		else
		{
			bResult = m_DittoWindow.SetCaptionColors(CGetSetOptions::m_Theme.CaptionLeft(), CGetSetOptions::m_Theme.CaptionRight(), CGetSetOptions::m_Theme.Border());
		}
	}

	m_DittoWindow.SetCaptionTextColor(CGetSetOptions::m_Theme.CaptionTextColor());

	return bResult;
}

void CWndEx::SetCaptionOn(int nPos, bool bOnstartup, int captionSize, int captionFontSize)
{
	m_DittoWindow.SetCaptionOn(this, nPos, bOnstartup, captionSize, captionFontSize);
}

LRESULT CWndEx::OnNcPaint(HRGN hRgn)
{
	m_DittoWindow.DoNcPaint(this);
	return 0;
}

LRESULT CWndEx::OnNcCalcSize(BOOL bCalcValidRects, LPARAM lpncsp) 
{
	LRESULT lResult = DefWindowProc(WM_NCCALCSIZE, (WPARAM)bCalcValidRects, lpncsp);
	
	m_DittoWindow.DoNcCalcSize(bCalcValidRects, (NCCALCSIZE_PARAMS*)lpncsp);

	return lResult;
}

LRESULT CWndEx::OnNcHitTest(CPoint point) 
{
	UINT Ret = m_DittoWindow.DoNcHitTest(this, point);
	if(Ret == -1)
		return DefWindowProc(WM_NCHITTEST, 0, MAKELPARAM(point.x, point.y));

	return Ret;
}

void CWndEx::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	int buttonPressed = m_DittoWindow.DoNcLButtonDown(this, nHitTest, point);
	
	if (buttonPressed != 0)
	{
		SetTimer(TIMER_BUTTON_UP, 100, NULL);
	}
}

void CWndEx::OnNcLButtonUp(UINT nHitTest, CPoint point) 
{
	long lRet = m_DittoWindow.DoNcLButtonUp(this, nHitTest, point);
	if(lRet > 0)
	{
		if(lRet == BUTTON_CHEVRON)
		{
			MinMaxWindow(SWAP_MIN_MAX);
			OnNcPaint(NULL);
		}
		return;
	}

	KillTimer(TIMER_BUTTON_UP);
}



void CWndEx::OnNcMouseMove(UINT nHitTest, CPoint point) 
{
	m_DittoWindow.DoNcMouseMove(this, nHitTest, point);

	if((m_bMaxSetTimer == false) && m_DittoWindow.m_bMinimized)
	{
		if(::time(NULL) - m_DittoWindow.m_TimeMinimized >= m_lDelayMaxSeconds)
		{
			SetTimer(TIMER_AUTO_MAX, CGetSetOptions::GetTimeBeforeExpandWindow(), NULL);
			m_bMaxSetTimer = true;
		}
	}
}

BOOL CWndEx::PreTranslateMessage(MSG* pMsg) 
{
	if (!IsMsgForThisWindowTree(pMsg))
	{
		return FALSE;
	}

	m_toolTip.RelayEvent(pMsg);
	m_DittoWindow.DoPreTranslateMessage(pMsg);
	
	return FALSE;
}

LRESULT CWndEx::OnEraseBkgnd(HDC hDC) 
{
	return DefWindowProc(WM_ERASEBKGND, (WPARAM)hDC, 0);
}

void CWndEx::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == TIMER_AUTO_MAX)
	{
		if(m_DittoWindow.m_bMinimized)
		{
			CPoint cp;
			GetCursorPos(&cp);
			
			UINT nHitTest = (UINT)OnNcHitTest(cp);
			
			::ScreenToClient(m_hWnd, &cp);
			
			if(nHitTest == HTCAPTION)
			{
				if(m_DittoWindow.m_crCloseBT.PtInRect(cp) == false)
				{
					if(m_DittoWindow.m_crMinimizeBT.PtInRect(cp) == false)
					{
						MinMaxWindow(FORCE_MAX);
					}
				}
			}
		}
		KillTimer(TIMER_AUTO_MAX);
		m_bMaxSetTimer = false;
	}
	else if (nIDEvent == TIMER_BUTTON_UP)
	{
		if ((GetKeyState(VK_LBUTTON) & 0x100) == 0)
		{
			m_DittoWindow.DoNcLButtonUp(this, 0, CPoint(0, 0));
			KillTimer(TIMER_BUTTON_UP);
		}
	}
}

void CWndEx::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if(m_bMaxSetTimer)
	{
		KillTimer(TIMER_AUTO_MAX);
		m_bMaxSetTimer = false;
	}

	//m_DittoWindow.SnapToEdge(this, lpwndpos);
}

LRESULT CWndEx::OnSize(UINT nType, CSize size)
{
	//m_DittoWindow.DoSetRegion(this);
	return 0;
}

void CWndEx::OnInitMenuPopup(HMENU hMenu, UINT nIndex, BOOL bSysMenu)
{
	OnInitMenuPopupEx(hMenu);
} 

void CWndEx::SetToolTipText(CString text)
{
	m_toolTip.UpdateTipText((LPCTSTR)text, m_hWnd, 1);
}

void CWndEx::SetCustomWindowTitle(CString title)
{
	CString old = m_DittoWindow.m_customWindowTitle;
	m_DittoWindow.m_customWindowTitle = title;
	m_DittoWindow.m_useCustomWindowTitle = true;

	if (old != m_DittoWindow.m_customWindowTitle)
	{
		this->InvalidateNc();
	}
}

void CWndEx::MinMaxWindow(long lOption)
{
	m_DittoWindow.MinMaxWindow(this, lOption);
}


void CWndEx::OnMoving(UINT fwSide, LPRECT pRect)
{
	m_snap.OnSnapMoving(m_hWnd, pRect);
}


void CWndEx::OnEnterSizeMove()
{
	m_snap.OnSnapEnterSizeMove(m_hWnd);
}
