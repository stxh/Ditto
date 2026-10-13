#pragma once

#include "GdipButton.h"
#include "GdiImageDrawer.h"
#include "DPI.h"

#define BUTTON_CLOSE 1
#define BUTTON_CHEVRON 2
#define BUTTON_MINIMIZE 3
#define BUTTON_MAXIMIZE 4

class CDittoWindow
{
public:
	CDittoWindow(void);
	~CDittoWindow(void);

	void DoNcPaint(WTL::CWindow *pWnd);
	void DrawChevronBtn(WTL::CDC &dc, WTL::CWindow *pWnd);
	void DrawCloseBtn(WTL::CDC &dc, WTL::CWindow *pWnd);
	void DrawMaximizeBtn(WTL::CDC &dc, WTL::CWindow *pWnd);
	void DrawMinimizeBtn(WTL::CDC &dc, WTL::CWindow *pWnd);
	void DrawWindowIcon(WTL::CDC &dc, WTL::CWindow *pWnd);

	void DoCreate(WTL::CWindow *pWnd);
	void DoNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	UINT DoNcHitTest(WTL::CWindow *pWnd, CPoint point);
	long DoNcLButtonUp(WTL::CWindow *pWnd, UINT nHitTest, CPoint point);
	int DoNcLButtonDown(WTL::CWindow *pWnd, UINT nHitTest, CPoint point);
	void DoNcMouseMove(WTL::CWindow *pWnd, UINT nHitTest, CPoint point) ;
	bool DoPreTranslateMessage(MSG* pMsg);
	void SetCaptionOn(WTL::CWindow *pWnd, int nPos, bool bOnstartup, int captionSize, int captionFontSize);
	bool SetCaptionColors(COLORREF left, COLORREF right, COLORREF border);
	void SetCaptionTextColor(COLORREF color);
	void MinMaxWindow(WTL::CWindow *pWnd, long lOption);
	void SetTitleTextHeight(WTL::CWindow *pWnd);
	int IndexToPos(int index, bool horizontal);
	void OnDpiChanged(WTL::CWindow *pWnd, int dpi);
	
	bool m_bDrawClose;
	bool m_sendWMClose;
	bool m_bDrawChevron;
	bool m_bDrawMaximize;
	bool m_bDrawMinimize;

	CRect m_crCloseBT;
	CRect m_crChevronBT;
	CRect m_crMaximizeBT;
	CRect m_crMinimizeBT;
	CRect m_crWindowIconBT;

	CFont m_VertFont;
	CFont m_HorFont;

	bool m_bMinimized;

	bool m_bMouseDownOnChevron;
	bool m_bMouseOverChevron;
	bool m_bMouseDownOnClose;
	bool m_bMouseOverClose;
	bool m_bMouseDownOnMinimize;
	bool m_bMouseOverMinimize;
	bool m_bMouseDownOnMaximize;
	bool m_bMouseOverMaximize;

	COLORREF m_CaptionColorLeft;
	COLORREF m_CaptionColorRight;
	COLORREF m_CaptionTextColor;
	COLORREF m_border;
	
	CGdiImageDrawer m_closeButton;
	CGdiImageDrawer m_chevronRightButton;
	CGdiImageDrawer m_chevronLeftButton;
	CGdiImageDrawer m_maximizeButton;
	CGdiImageDrawer m_minimizeButton;
	//CGdiImageDrawer m_windowIcon;

	CString m_customWindowTitle;
	bool m_useCustomWindowTitle;

	int m_captionBorderWidth;
	int m_captionFontSize;

	int m_captionPosition;
	int m_borderSize;

	int m_titleTextHeight;

	bool m_buttonDownOnCaption;

	CRect m_crFullSizeWindow;
	COleDateTime m_TimeMinimized;
	COleDateTime m_TimeMaximized;


	CDPI m_dpi;
};
