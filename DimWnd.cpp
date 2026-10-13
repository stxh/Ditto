#include "stdafx.h"
#include "DimWnd.h"

CDimWnd::CDimWnd(HWND hWndParent)
{
	m_ParentWnd = NULL;

	if (hWndParent != NULL)
	{
		CRect rc;
		::GetWindowRect(hWndParent, &rc);

		Create(hWndParent, rc, TEXT(""),
			WS_POPUP | WS_VISIBLE | WS_DISABLED,
			WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);

		::SetLayeredWindowAttributes(m_hWnd, RGB(0, 0, 0), 64, LWA_ALPHA);
		BringWindowToTop();
		ShowWindow(SW_SHOW);
		m_ParentWnd = hWndParent;
	}
}

CDimWnd::~CDimWnd()
{
}

LRESULT CDimWnd::OnEraseBkgnd(UINT, WPARAM wParam, LPARAM, BOOL&)
{
	HDC hdc = (HDC)wParam;
	HBRUSH hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBlackBrush);

	CRect rect;
	GetClientRect(&rect);
	PatBlt(hdc, rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);

	SelectObject(hdc, oldBrush);
	DeleteObject(hBlackBrush);
	return 1;
}
