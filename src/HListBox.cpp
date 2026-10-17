#include "stdafx.h"
#include "HListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHListBox

CHListBox::CHListBox()
{
 width = 0;
}

CHListBox::~CHListBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CHListBox message handlers
void CHListBox::updateWidth(LPCTSTR s)
{
     WTL::CClientDC dc(m_hWnd);
     HFONT f = (HFONT)GetFont();
     dc.SelectFont(f);
     SIZE sz;
     ::GetTextExtentPoint32(dc.m_hDC, s, (int)_tcslen(s), &sz);
     sz.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);
     if(sz.cx > width)
 	 { /* extend */
 	  width = sz.cx;
 	  SetHorizontalExtent(width);
 	 } /* extend */
}

int CHListBox::AddString(LPCTSTR s)
{
     int result = WTL::CListBox::AddString(s);
     if(result < 0)
 	 return result;
     updateWidth(s);
     return result;
}

int CHListBox::InsertString(int i, LPCTSTR s)
{
     int result = WTL::CListBox::InsertString(i, s);
     if(result < 0)
 	 return result;
     updateWidth(s);
     return result;
}

void CHListBox::ResetContent()
{
     WTL::CListBox::ResetContent();
     width = 0;
}

int CHListBox::DeleteString(int n)
{
     int result = WTL::CListBox::DeleteString(n);
     if(result < 0)
 	 return result;
     WTL::CClientDC dc(m_hWnd);

     HFONT f = (HFONT)GetFont();
     dc.SelectFont(f);

width = 0;
     for(int i = 0; i < WTL::CListBox::GetCount(); i++)
 	 { /* scan strings */
 	  CString s;
 	  WTL::CListBox::GetText(i, s);
 	  SIZE sz;
 	  ::GetTextExtentPoint32(dc.m_hDC, s, s.GetLength(), &sz);
           sz.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);
 	  if(sz.cx > width)
 	      width = sz.cx;
 	 } /* scan strings */
     WTL::CListBox::SetHorizontalExtent(width);
     return result;
}
