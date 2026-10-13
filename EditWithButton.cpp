// EditWithButton.cpp : implementation file
//

//DISCLAIMER:
//The code in this project is Copyright (C) 2006 by Gautam Jain. You have the right to
//use and distribute the code in any way you see fit as long as this paragraph is included
//with the distribution. No warranties or claims are made as to the validity of the
//information and code contained herein, so use it at your own risk.

#include "stdafx.h"
#include "EditWithButton.h"
#include "cp_main.h"


CEditWithButton::CEditWithButton()
{
	m_iButtonClickedMessageId = NM_CANCEL_SEARCH;
	m_bButtonExistsAlways = FALSE;

	m_rcEditArea.SetRect(0, 0, 0, 0);
}

CEditWithButton::~CEditWithButton()
{
	m_bmpEmptyEdit.DeleteObject();
	m_bmpFilledEdit.DeleteObject();
}


// CEditWithButton message handlers

void CEditWithButton::PreSubclassWindow()
{
	ATLASSERT(GetStyle() & ES_MULTILINE);

	ResizeWindow();
}

BOOL CEditWithButton::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && 
		CONTROL_PRESSED)
	{
		switch (pMsg->wParam)
		{
		case 'Z':
			Undo();
			return TRUE;
		case 'X':
			Cut();
			return TRUE;
		case 'C':
			Copy();
			return TRUE;
		case 'V':
			Paste();
			return TRUE;
		case 'A':
			SetSel(0, -1);
			return TRUE;
		}
	}

	switch(pMsg->message) 
	{
	case WM_KEYDOWN:
		{
			if(pMsg->wParam == VK_RETURN)
			{
				CWindow pWnd = GetParent();
				if(pWnd)
				{
					if(CGetSetOptions::m_bFindAsYouType)
					{
						pWnd.SendMessage(NM_SEARCH_ENTER_PRESSED, 0, 0);
					}
					else
					{
						pWnd.PostMessage(CB_SEARCH, 0, 0);
					}
				}

				return TRUE;
			}	
			else if (pMsg->wParam == VK_DOWN ||
					pMsg->wParam == VK_UP ||
					pMsg->wParam == VK_F3 ||
					pMsg->wParam == VK_PRIOR ||
					pMsg->wParam == VK_NEXT)
			{
				CWindow pWnd = GetParent();
				if(pWnd)
				{
					pWnd.SendMessage(CB_UPDOWN, pMsg->wParam, pMsg->lParam);
					return TRUE;
				}
			}
			break;
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}

BOOL CEditWithButton::SetBitmaps(UINT iEmptyEdit, UINT iFilledEdit)
{
	BITMAP bmpInfo;

	m_bmpEmptyEdit.DeleteObject();
	m_bmpFilledEdit.DeleteObject();

	m_bmpEmptyEdit.LoadBitmap(iEmptyEdit);
	m_bmpFilledEdit.LoadBitmap(iFilledEdit);
	
	m_bmpEmptyEdit.GetBitmap(&bmpInfo);
	m_sizeEmptyBitmap.SetSize(bmpInfo.bmWidth,bmpInfo.bmHeight);
	
	m_bmpFilledEdit.GetBitmap(&bmpInfo);
	m_sizeFilledBitmap.SetSize(bmpInfo.bmWidth,bmpInfo.bmHeight);

	return TRUE;
}

void CEditWithButton::SetButtonArea(CRect rcButtonArea)
{
	m_rcButtonArea = rcButtonArea;
}

void CEditWithButton::ResizeWindow()
{
	if (!IsWindow()) return;

	if (m_rcBorder == CRect(0,0,0,0)) 
		return;

	CRect r;
	GetWindowRect(r);
	ScreenToClient(r);

	SetWindowPos(HWND_TOP, 0, 0, r.Width(), r.Height(), SWP_NOMOVE|SWP_NOZORDER);

	m_rcEditArea.left = r.left + m_rcBorder.left;
	m_rcEditArea.top = r.top + m_rcBorder.top;
	m_rcEditArea.right = r.right -= m_rcBorder.right;
	m_rcEditArea.bottom = r.bottom -= m_rcBorder.bottom;
	
	SetRect(&m_rcEditArea);
}

BOOL CEditWithButton::SetBorder(CRect rcEditArea)
{
	m_rcBorder = rcEditArea;

	ResizeWindow();

	return TRUE;
}

void CEditWithButton::SetButtonExistsAlways(BOOL bButtonExistsAlways)
{
	m_bButtonExistsAlways = bButtonExistsAlways;
}

LRESULT CEditWithButton::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CDCHandle dc = (HDC)wParam;
	CSize sizeBitmap;
	CBitmap* pOldBitmap = NULL;
	int iTextLength = GetWindowTextLength();

	CRect size;
	GetWindowRect(size);
    
	if (iTextLength == 0)
	{
		sizeBitmap = m_sizeEmptyBitmap;
	}
	else
	{
		sizeBitmap = m_sizeFilledBitmap;
	}

	CDC dcMemory;
	dcMemory.CreateCompatibleDC(dc);

	if (iTextLength == 0)
	{
		pOldBitmap = dcMemory.SelectObject(&m_bmpEmptyEdit);
	}
	else
	{
		pOldBitmap = dcMemory.SelectObject(&m_bmpFilledEdit);
	}

	dc.BitBlt(0,0, 50, sizeBitmap.cy, dcMemory, 
		0, 0, SRCCOPY);

	for(int i = 50; i < size.Width()-50; i++)
	{
		dc.BitBlt(i, 0, 1, sizeBitmap.cy, dcMemory, 
			50, 0, SRCCOPY);
	}

	dc.BitBlt(size.Width()-50, 0, 50, sizeBitmap.cy, dcMemory, 
		sizeBitmap.cx-50, 0, SRCCOPY);

	dcMemory.SelectObject(pOldBitmap);

	bHandled = TRUE;
	return TRUE;
}

LRESULT CEditWithButton::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CEditWithButton::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	InvalidateRect(NULL);

	bHandled = FALSE;
	return 0;
}

LRESULT CEditWithButton::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPoint point(lParam);

	CRect crClose;
	GetWindowRect(crClose);
	ScreenToClient(crClose);

	int left = crClose.right - (m_sizeFilledBitmap.cx - m_rcButtonArea.left);
	int right = crClose.right - (m_sizeFilledBitmap.cx - m_rcButtonArea.right);

	crClose.top = m_rcButtonArea.top;
	crClose.bottom = m_rcButtonArea.bottom;
	crClose.left = left;
	crClose.right = right;

	if (crClose.PtInRect(point))
	{
		if ( (GetWindowTextLength() > 0) || m_bButtonExistsAlways)
		{
			CWindow pOwner = GetOwner();
			if (pOwner)
			{
				pOwner.SendMessage(m_iButtonClickedMessageId, 0, 0);
			}
		}
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CEditWithButton::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPoint pntCursor;
	GetCursorPos(&pntCursor);
	ScreenToClient(&pntCursor);

	CRect edit;
	GetWindowRect(edit);
	ScreenToClient(edit);
	edit.left += m_rcBorder.left;
	edit.top += m_rcBorder.top;
	edit.right -= m_rcBorder.right;
	edit.bottom -= m_rcBorder.bottom;

	if (!edit.PtInRect(pntCursor))
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		bHandled = TRUE;
		return TRUE;
	}

	bHandled = FALSE;
	return FALSE;
}

LRESULT CEditWithButton::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
	if (lRes == -1)
		return -1;

	ResizeWindow();

	bHandled = TRUE;
	return 0;
}

LRESULT CEditWithButton::OnSetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DefWindowProc(WM_SETFONT, wParam, lParam);

	ResizeWindow();

	bHandled = TRUE;
	return 0;
}

LRESULT CEditWithButton::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DefWindowProc(uMsg, wParam, lParam);

	ResizeWindow();

	bHandled = TRUE;
	return 0;
}
