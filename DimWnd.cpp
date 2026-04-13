#include "stdafx.h"
#include "DimWnd.h"

CDimWnd::CDimWnd(CWnd* pParent)
{
	m_ParentWnd = NULL;

	if (pParent != NULL)
	{
		CRect rc;
		pParent->GetWindowRect(&rc);

		Create(pParent->m_hWnd, rc, TEXT(""),
			WS_POPUP | WS_VISIBLE | WS_DISABLED,
			WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);

		SetLayeredWindowAttributes(RGB(0, 0, 0), 64, LWA_ALPHA);
		BringWindowToTop();
		ShowWindow(SW_SHOW);
		m_ParentWnd = pParent->m_hWnd;
	}
}

CDimWnd::~CDimWnd()
{
}

LRESULT CDimWnd::OnEraseBkgnd(UINT, WPARAM wParam, LPARAM, BOOL&)
{
	HDC hdc = (HDC)wParam;
	CBrush backBrush(CreateSolidBrush(RGB(0, 0, 0)));
	HBRUSH oldBrush = SelectBrush(hdc, backBrush);

	CRect rect;
	GetClientRect(&rect);
	PatBlt(hdc, rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);

	SelectBrush(hdc, oldBrush);
	return 1;
}
