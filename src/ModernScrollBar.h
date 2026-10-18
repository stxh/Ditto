#pragma once

#include "DPI.h"

class CQListCtrl;

// Scrollbar orientation
enum class ScrollBarOrientation
{
	Vertical,
	Horizontal
};

// Modern scrollbar overlay control with rounded corners
// Similar to Discord, Teams, GitHub Desktop style
class CModernScrollBar : public CWindowImpl<CModernScrollBar>
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_HREDRAW | CS_VREDRAW, NULL)

	CModernScrollBar();
	virtual ~CModernScrollBar();

	// Create the scrollbar overlay
	BOOL Create(HWND hWndParent, CQListCtrl* pListCtrl, ScrollBarOrientation orientation = ScrollBarOrientation::Vertical);

	// Update scrollbar position and visibility based on list state
	void UpdateScrollBar();

	// Set scrollbar colors from theme
	void SetColors(COLORREF trackColor, COLORREF thumbColor, COLORREF thumbHoverColor);

	// Set rounded corner radius
	void SetCornerRadius(int radius) { m_cornerRadius = radius; }

	// Set scrollbar width/height (depending on orientation)
	void SetWidth(int width) { m_scrollBarWidth = width; }

	// Show/hide with fade animation
	void Show(bool animate = true);
	void Hide(bool animate = true);

	// Check if mouse is over scrollbar
	bool IsMouseOver() const { return m_isMouseOver; }

	// DPI awareness
	void SetDPI(CDPI* pDPI) { m_pDPI = pDPI; }

	// Get orientation
	ScrollBarOrientation GetOrientation() const { return m_orientation; }

	BEGIN_MSG_MAP(CModernScrollBar)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER(WM_MOUSEHOVER, OnMouseHover)
	END_MSG_MAP()

protected:
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
	void OnMouseMove(UINT nFlags, CPoint point);
	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// Calculate thumb rectangle based on list scroll position
	CRect GetThumbRect();

	// Draw rounded rectangle with GDI+
	void DrawRoundedRect(HDC hDC, CRect rect, int radius, COLORREF color);

	// Scroll list to position based on thumb drag
	void ScrollToPosition(int thumbPos);

private:
	CQListCtrl* m_pListCtrl;
	HWND m_hWndParent;
	CDPI* m_pDPI;
	ScrollBarOrientation m_orientation;

	// Colors
	COLORREF m_trackColor;
	COLORREF m_thumbColor;
	COLORREF m_thumbHoverColor;

	// Dimensions
	int m_scrollBarWidth;
	int m_scrollBarHoverWidth;
	int m_cornerRadius;
	int m_minThumbSize;  // Min thumb width or height depending on orientation

	// State
	bool m_isMouseOver;
	bool m_isDragging;
	int m_dragStartPos;  // X or Y depending on orientation
	int m_dragStartScrollPos;
	bool m_isVisible;
	bool m_trackingMouse;

	// Timer
	enum { TIMER_AUTO_HIDE = 1 };
};
