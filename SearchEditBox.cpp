#include "stdafx.h"
#include "cp_main.h"
#include "SearchEditBox.h"

CSearchEditBox::CSearchEditBox()
{
}

CSearchEditBox::~CSearchEditBox()
{
}

BOOL CSearchEditBox::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
		case WM_KEYDOWN:
		{
			if(pMsg->wParam == VK_RETURN)
			{
				CWnd* pWnd = GetParent();
				if(pWnd)
				{
					if(CGetSetOptions::m_bFindAsYouType)
					{
						pWnd->SendMessage(NM_SEARCH_ENTER_PRESSED, 0, 0);
					}
					else
					{
						pWnd->PostMessage(CB_SEARCH, 0, 0);
					}
				}

				return TRUE;
			}
			else if(pMsg->wParam == VK_DOWN ||
					pMsg->wParam == VK_UP ||
					pMsg->wParam == VK_F3)
			{
				if(CGetSetOptions::m_bFindAsYouType)
				{
					CWnd* pWnd = GetParent();
					if(pWnd)
					{
						pWnd->SendMessage(CB_UPDOWN, pMsg->wParam, pMsg->lParam);
						return TRUE;
					}
				}
			}
			else if(pMsg->wParam == 'C' && CONTROL_PRESSED ||
					pMsg->wParam == 'X' && CONTROL_PRESSED ||
					pMsg->wParam == VK_DELETE)
			{
				LONG lEditSel = GetSel();
				if(LOWORD(lEditSel) == HIWORD(lEditSel))
				{
					CWnd* pWnd = GetParent();
					if(pWnd)
					{
						pWnd->SendMessage(CB_UPDOWN, pMsg->wParam, pMsg->lParam);
						return TRUE;
					}
				}
			}
			break;
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}