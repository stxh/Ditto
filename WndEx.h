#pragma once

#include "DittoWindow.h"
#include "GdipButton.h"
#include "SnapWindow.h"

#define	SWAP_MIN_MAX			1
#define FORCE_MIN				2
#define FORCE_MAX				3

class CWndEx : public CWindowImpl<CWndEx, CWindow, CNullTraits>
{
public:
	DECLARE_WND_CLASS_EX(_T("QPasteClass"), CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, COLOR_WINDOW)

	CWndEx();
	virtual ~CWndEx();

public:
	virtual BOOL Create(const CRect& crStart, CWindow* pParentWnd);
	BOOL PreTranslateMessage(MSG* pMsg);

public:
	void InvalidateNc();
	void SetCaptionOn(int nPos, bool bOnstartup, int captionSize, int captionFontSize);
	void MinMaxWindow(long lOption = SWAP_MIN_MAX);
	void GetWindowRectEx(LPRECT lpRect);
	bool SetCaptionColorActive(BOOL bPersistant, BOOL ConnectedToClipboard);
	void SetAutoMaxDelay(long lDelay)	{ m_lDelayMaxSeconds = lDelay; }
	bool GetMinimized()	{ return m_DittoWindow.m_bMinimized; }

	void SetCustomWindowTitle(CString title);

	void SetToolTipText(CString text);

	int GetCaptionSize() { return m_DittoWindow.m_captionBorderWidth; }

	CDittoWindow m_DittoWindow;

protected:	
		
	bool m_bMaxSetTimer;	
	int m_lDelayMaxSeconds;
	CToolTipCtrl m_toolTip;
	SnapWindow m_snap;

public:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnNcPaint(HRGN hRgn);
	LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lpncsp);
	LRESULT OnNcHitTest(CPoint point);
	void OnNcLButtonDown(UINT nHitTest, CPoint point);
	void OnNcMouseMove(UINT nHitTest, CPoint point);
	void OnNcLButtonUp(UINT nHitTest, CPoint point);
	LRESULT OnEraseBkgnd(HDC hDC);
	void OnTimer(UINT_PTR nIDEvent);
	void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	void OnInitMenuPopup(HMENU hMenu, UINT nIndex, BOOL bSysMenu);

	LRESULT OnSize(UINT nType, CSize size);
	void OnMoving(UINT fwSide, LPRECT pRect);
	void OnEnterSizeMove();

	BEGIN_MSG_MAP_EX(CWndEx)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_NCPAINT(OnNcPaint)
		MSG_WM_NCCALCSIZE(OnNcCalcSize)
		MSG_WM_NCHITTEST(OnNcHitTest)
		MSG_WM_NCLBUTTONDOWN(OnNcLButtonDown)
		MSG_WM_NCMOUSEMOVE(OnNcMouseMove)
		MSG_WM_NCLBUTTONUP(OnNcLButtonUp)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_WINDOWPOSCHANGING(OnWindowPosChanging)
		MSG_WM_INITMENUPOPUP(OnInitMenuPopup)
		MSG_WM_SIZE(OnSize)
		MSG_WM_MOVING(OnMoving)
		MSG_WM_ENTERSIZEMOVE(OnEnterSizeMove)
	END_MSG_MAP()
};
