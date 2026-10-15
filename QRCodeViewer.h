#pragma once

#include "GdipButton.h"
#include "DittoWindow.h"
#include "SnapWindow.h"
// QRCodeViewer

class QRCodeViewer : public CWindowImpl<QRCodeViewer, CWindow, CNullTraits>
{

public:
	DECLARE_WND_CLASS_EX(_T("DittoQRCodeViewer"), CS_CLASSDC | CS_SAVEBITS, NULL)

	QRCodeViewer();
	virtual ~QRCodeViewer();

	BOOL Create(CWindow *pParentWnd, unsigned char* bitmapData, int imageSize, CString desc, int rowHeight, LOGFONT font);

	CStatic m_desc;

	BEGIN_MSG_MAP_EX(QRCodeViewer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_DPICHANGED, OnDpiChanged)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MSG_WM_SIZE(OnSize)
		MSG_WM_NCHITTEST(OnNcHitTest)
		MSG_WM_NCPAINT(OnNcPaint)
		MSG_WM_NCCALCSIZE(OnNcCalcSize)
		MSG_WM_NCLBUTTONDOWN(OnNcLButtonDown)
		MSG_WM_NCMOUSEMOVE(OnNcMouseMove)
		MSG_WM_NCLBUTTONUP(OnNcLButtonUp)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_CTLCOLORSTATIC(OnCtlColor)
		MSG_WM_WINDOWPOSCHANGING(OnWindowPosChanging)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_MOVING(OnMoving)
		MSG_WM_ENTERSIZEMOVE(OnEnterSizeMove)
	END_MSG_MAP()

	void OnFinalMessage(HWND /*hWnd*/) { delete this; }

	unsigned char* m_bitmapData;
	int m_imageSize;
	CGdipButton m_ShowGroupsFolderBottom;
	CGdiImageDrawer m_qrCodeDrawer;
	int m_descRowHeight;
	CFont m_font;
	HBRUSH m_descBackground;
	LOGFONT m_logFont;
	int m_originalFontHeight;
	SnapWindow m_snap;
	CDittoWindow m_DittoWindow;

	void MoveControls();

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDpiChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT nType, CSize size);
	HITTEST_RET OnNcHitTest(CPoint point);
	void OnNcPaint(HRGN hRgn);
	LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lpncsp);
	void OnNcLButtonDown(UINT nHitTest, CPoint point);
	void OnNcLButtonUp(UINT nHitTest, CPoint point);
	void OnNcMouseMove(UINT nHitTest, CPoint point);
	BOOL OnEraseBkgnd(CDCHandle dc);
	HBRUSH OnCtlColor(CDCHandle dc, CWindow wnd);
	void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	void OnTimer(UINT_PTR nIDEvent);
	void OnMoving(UINT fwSide, LPRECT pRect);
	void OnEnterSizeMove();
};
