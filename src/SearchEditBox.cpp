#include "stdafx.h"
#include "cp_main.h"
#include "SearchEditBox.h"

CSearchEditBox::CSearchEditBox()
{
}

CSearchEditBox::~CSearchEditBox()
{
}

LRESULT CSearchEditBox::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == VK_RETURN)
	{
		CWindow pWnd = GetParent();
		if (pWnd)
		{
			if (CGetSetOptions::m_bFindAsYouType)
			{
				pWnd.SendMessage(NM_SEARCH_ENTER_PRESSED, 0, 0);
			}
			else
			{
				pWnd.PostMessage(CB_SEARCH, 0, 0);
			}
		}

		bHandled = TRUE;
		return 0;
	}

	if (wParam == VK_DOWN ||
		wParam == VK_UP ||
		wParam == VK_F3)
	{
		if (CGetSetOptions::m_bFindAsYouType)
		{
			CWindow pWnd = GetParent();
			if (pWnd)
			{
				pWnd.SendMessage(CB_UPDOWN, wParam, lParam);
				bHandled = TRUE;
				return 0;
			}
		}
	}

	if ((wParam == 'C' && CONTROL_PRESSED) ||
		(wParam == 'X' && CONTROL_PRESSED) ||
		wParam == VK_DELETE)
	{
		LONG lEditSel = GetSel();
		if (LOWORD(lEditSel) == HIWORD(lEditSel))
		{
			CWindow pWnd = GetParent();
			if (pWnd)
			{
				pWnd.SendMessage(CB_UPDOWN, wParam, lParam);
				bHandled = TRUE;
				return 0;
			}
		}
	}

	bHandled = FALSE;
	return 0;
}
