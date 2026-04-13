// DialogResizer.cpp: implementation of the CDialogResizer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DialogResizer.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDialogResizer::CDialogResizer()
{

}

CDialogResizer::~CDialogResizer()
{

}

void CDialogResizer::SetParent(HWND hWndParent)
{
	m_hWndParent = hWndParent;
	RECT cr;
	GetClientRect(m_hWndParent, &cr);

	m_DlgSize.cx = cr.right - cr.left;
	m_DlgSize.cy = cr.bottom - cr.top;
}
void CDialogResizer::AddControl(int nControlID, int nFlags)
{
	HWND hWnd = GetDlgItem(m_hWndParent, nControlID);
	if(hWnd)
		AddControl(hWnd, nFlags);
}

void CDialogResizer::AddControl(HWND hWnd, int nFlags)
{
	CDR_Data data;
	data.m_hWnd = hWnd;
	data.m_nFlags = nFlags;

	m_Controls.push_back(data);
}
void CDialogResizer::MoveControls(CSize csNewSize)
{
	int nDeltaX = csNewSize.cx - m_DlgSize.cx;
	int nDeltaY = csNewSize.cy - m_DlgSize.cy;

	if(nDeltaX == 0 && nDeltaY == 0)
		return;

	m_DlgSize = csNewSize;

	size_t nCount = m_Controls.size();
	RECT rc;
	RECT rcParent;

	GetClientRect(m_hWndParent, &rcParent);

	for(size_t i = 0; i < nCount; i++)
	{
		CDR_Data data = m_Controls[i];

		GetWindowRect(data.m_hWnd, &rc);
		MapWindowPoints(GetDesktopWindow(),  m_hWndParent, (LPPOINT)&rc, 2 );
		
		//
		//	Adjust the window horizontally
		if( data.m_nFlags & DR_MoveLeft )
		{
			rc.left += nDeltaX;
			rc.right += nDeltaX;
		}

		//
		//	Adjust the window vertically
		if( data.m_nFlags & DR_MoveTop )
		{
			rc.top += nDeltaY;
			rc.bottom += nDeltaY;
		}

		//
		//	Size the window horizontally
		if( data.m_nFlags & DR_SizeWidth )
		{
			rc.right += nDeltaX;
		}

		//	Size the window vertically
		if( data.m_nFlags & DR_SizeHeight )
		{
			rc.bottom += nDeltaY;
		}

		::SetWindowPos(data.m_hWnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOACTIVATE | SWP_NOZORDER);
	}

	::RedrawWindow(m_hWndParent, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}