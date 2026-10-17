#pragma once

#include <atlbase.h>
#include <atlwin.h>
#include "ScrollHelper.h"
#include "Clip.h"

class CImageViewer : public CWindowImpl<CImageViewer, ATL::CWindow>
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_HREDRAW | CS_VREDRAW, -1)

public:
	CImageViewer();
	~CImageViewer();

	Gdiplus::Bitmap* m_pGdiplusBitmap;
	CScrollHelper m_scrollHelper;

	void UpdateBitmapSize(bool setScale);

	BOOL Create(HWND hParent);

	bool m_hoveringOverImage;

	CPoint m_ptFirst;
	CPoint m_ptSecond;
	DWORD m_dwArguments;

	double m_scale;

	BEGIN_MSG_MAP(CImageViewer)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEHWHEEL, OnMouseHWheel)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_GESTURE, OnGesture)
		MESSAGE_HANDLER(WM_GESTURENOTIFY, OnGestureNotify)
	END_MSG_MAP()

	LRESULT OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseHWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGesture(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGestureNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};