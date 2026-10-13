//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMemDC - memory DC
//
// Author: Keith Rule, keithr@europa.com,  Copyright 1996-1997, Keith Rule
//
// You may freely use or modify this code provided this copyright is included in all derived versions.
//
// History - 10/3/97 Fixed scrolling bug.
//                   Added print support.
//				 - 14/7/99 Added optional clip rect parameter [jgh]
//
//				 - 06/06/08  Added option to copy screen on construction
//
#if !defined(AFX_CMemDC_H__F666A491_3847_11D3_A58E_00805FC1DE10__INCLUDED_)
#define AFX_CMemDC_H__F666A491_3847_11D3_A58E_00805FC1DE10__INCLUDED_

class CMemDCEx : public WTL::CDC {
private:
	WTL::CBitmap m_bitmap;  // Offscreen bitmap
	HBITMAP      m_oldBitmap; // bitmap originally found in CMemDC
	HDC          m_hSrcDC;  // Source DC passed in constructor
	CRect        m_rect;    // Rectangle of drawing area.
	BOOL         m_bMemDC;  // TRUE if the memory DC was created.
public:
	CMemDCEx(HDC hDC, CRect rect = CRect(0, 0, 0, 0), BOOL bCopyFirst = FALSE) :
		WTL::CDC(), m_oldBitmap(NULL), m_hSrcDC(hDC), m_bMemDC(FALSE)
	{
		ATLASSERT(m_hSrcDC != NULL);

		if (m_hSrcDC != NULL)
		{
			m_bMemDC = TRUE;
			CreateCompatibleDC(m_hSrcDC);

			if (rect.IsRectEmpty())
				GetClipBox(&m_rect);
			else
				m_rect = rect;

			m_bitmap.CreateCompatibleBitmap(m_hSrcDC, m_rect.Width(), m_rect.Height());
			m_oldBitmap = SelectBitmap(m_bitmap);
			SetWindowOrg(m_rect.left, m_rect.top);

			if (bCopyFirst)
				BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
					m_hSrcDC, m_rect.left, m_rect.top, SRCCOPY);
		}
	}

	~CMemDCEx()
	{
		if (m_bMemDC)
		{
			// Copy the offscreen bitmap onto the source DC.
			BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
				m_hSrcDC, m_rect.left, m_rect.top, SRCCOPY);
			// Swap back the original bitmap.
			SelectBitmap(m_oldBitmap);
		}
		else
		{
			// Don't let the base class delete a DC we don't own.
			Detach();
		}
	}
};

#endif
// End CMemDC
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
