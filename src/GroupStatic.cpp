// GroupStatic.cpp : implementation file
//

#include "stdafx.h"
#include "cp_main.h"
#include "GroupStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupStatic

CGroupStatic::CGroupStatic()
{
	m_dwTextColor = 0;
	m_dwBkColor = RGB(255, 255, 255);
	m_toggleCursorToHand = false;
}

CGroupStatic::~CGroupStatic()
{
}

/////////////////////////////////////////////////////////////////////////////
// CGroupStatic message handlers

LRESULT CGroupStatic::OnCtlColorStatic(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hdcChild = (HDC)wParam;

	// Set the foreground color
	::SetTextColor( hdcChild, m_dwTextColor );

	// If a background color is pre-determined
	if(m_dwBkColor != -1)
	{
		::SetBkMode(hdcChild, TRANSPARENT);
		::SetBkColor(hdcChild, m_dwBkColor);
		if (!m_brush.IsNull())
			m_brush.DeleteObject();
		m_brush.CreateSolidBrush(m_dwBkColor);
		return (LRESULT)(HBRUSH)m_brush;
	}

	// Determine the current background color based on my parent window
	static COLORREF clrPrevValid = -1;
	HWND hParent = ::GetParent(m_hWnd);
	HDC  hParentDc = ::GetDC(hParent);

	// Get the color based on the 0, 0 reference
	COLORREF clrParentBkground = ::GetPixel(hParentDc, 0, 0);
	::ReleaseDC(hParent, hParentDc);

	// If found (not off of the screen or under another window)
	// set my current color to it
	if(clrParentBkground == -1)
	{  clrParentBkground = clrPrevValid;  }
	else
	{  clrPrevValid = clrParentBkground;  }

	// If either the current, or previous color found was not valid
	// allow the parent to perform default processing
	if(clrParentBkground == -1)
	{
		bHandled = FALSE;
		return 0;
	}

	// Set the background mode to transparent
	::SetBkMode(hdcChild, TRANSPARENT);

	// Set the background color and brush based on my parent's color
	::SetBkColor(hdcChild, clrParentBkground);
	if (!m_brush.IsNull())
		m_brush.DeleteObject();
	m_brush.CreateSolidBrush(clrParentBkground);
	return (LRESULT)(HBRUSH)m_brush;
}

LRESULT CGroupStatic::OnSetCursor(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_toggleCursorToHand)
	{
		HCURSOR h = ::LoadCursor(NULL, IDC_HAND);
		::SetCursor(h);
		return 1;
	}

	bHandled = FALSE;
	return 0;
}

/*************************************************************************
*
*************************************************************************/
void CGroupStatic::SetFont( int nPointSize, LPCTSTR lpszFaceName, HDC hDC )
{
	// If a font has been allocated, delete it
	if (m_font.m_hFont != NULL)
		m_font.DeleteObject();

	// Create a font using the given attributes
	m_font.CreatePointFont(nPointSize, lpszFaceName, hDC);

	// Set the window's current font to the specified font
	SetFont((HFONT)m_font, TRUE);
}
