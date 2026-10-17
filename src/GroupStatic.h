#pragma once

class CGroupStatic : public ATL::CWindowImpl<CGroupStatic, CStatic>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CStatic::GetWndClassName())

	CGroupStatic();

	void SetTextColor(COLORREF color) { m_dwTextColor = color; }
	void SetBkColor(COLORREF color) { m_dwBkColor = color; }
	void SetFont(int nPointSize, LPCTSTR lpszFaceName, HDC hDC = NULL);
	void SetFont(HFONT hFont, BOOL bRepaint = TRUE) { ::SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(bRepaint, 0)); }
	void SetToggleCursor(bool toggle) { m_toggleCursorToHand = toggle; }

	virtual ~CGroupStatic();

protected:

	BEGIN_MSG_MAP_EX(CGroupStatic)
		MESSAGE_HANDLER(OCM_CTLCOLORSTATIC, OnCtlColorStatic)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
	END_MSG_MAP()

	LRESULT OnCtlColorStatic(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCursor(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	COLORREF m_dwTextColor;
	COLORREF m_dwBkColor;
	CFont    m_font;
	CBrush   m_brush;
	bool     m_toggleCursorToHand;
};
