//
// GdipButton.cpp : Version 1.0 - see article at CodeProject.com
//
// Author:  Darren Sessions
//          
//
// Description:
//     GdipButton is a CButton derived control that uses GDI+ 
//     to support alternate image formats
//
// History
//     Version 1.0 - 2008 June 10
//     - Initial public release
//
// License:
//     This software is released under the Code Project Open License (CPOL),
//     which may be found here:  http://www.codeproject.com/info/eula.aspx
//     You are free to use this software in any way you like, except that you 
//     may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GdipButton.h"

#include "CGdiPlusBitmap.h"
#include "MemDC.h"
#include "CP_Main.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGdipButton

CGdipButton::CGdipButton()
{
	m_pStdImage = NULL;
	m_pAltImage = NULL;

	m_bHaveBitmaps = FALSE;
	m_bHaveAltImage = FALSE;

	m_pCurBtn = NULL;

	m_bIsDisabled = FALSE;
	m_bIsToggle = FALSE;

	m_bIsHovering = FALSE;
	m_bIsTracking = FALSE;

	m_nCurType = STD_TYPE;

	m_pToolTip = NULL;

}

CGdipButton::~CGdipButton()
{
	if(m_pStdImage) delete m_pStdImage;
	if(m_pAltImage) delete m_pAltImage;
	if(m_pToolTip)	delete m_pToolTip;
}


BOOL CGdipButton::LoadStdImageDPI(int dpi, UINT id96, UINT id120, UINT id144, UINT id168, UINT id192, LPCTSTR pType, UINT id225, UINT id250, UINT id275, UINT id300, UINT id325, UINT id350)
{
	BOOL ret = FALSE;

	if (dpi >= 336 && id350 != 0)
	{
		ret = LoadStdImage(id350, pType);
	}
	else if (dpi >= 312 && id325 != 0)
	{
		ret = LoadStdImage(id325, pType);
	}
	else if (dpi >= 288 && id300 != 0)
	{
		ret = LoadStdImage(id300, pType);
	}
	else if (dpi >= 264 && id275 != 0)
	{
		ret = LoadStdImage(id275, pType);
	}
	else if (dpi >= 240 && id250 != 0)
	{
		ret = LoadStdImage(id250, pType);
	}
	else if (dpi >= 216 && id225 != 0)
	{
		ret = LoadStdImage(id225, pType);
	}
	else if (dpi >= 192)
	{
		ret = LoadStdImage(id192, pType);
	}
	else if (dpi >= 168)
	{
		ret = LoadStdImage(id168, pType);
	}
	else if (dpi >= 144)
	{
		ret = LoadStdImage(id144, pType);
	}
	else if (dpi >= 120)
	{
		ret = LoadStdImage(id120, pType);
	}
	else
	{
		ret = LoadStdImage(id96, pType);
	}

	return ret;
}

//=============================================================================
//
// LoadStdImage()
//
// Purpose:     The LoadStdImage() Loads the image for the button.  This 
//				function must be called at a minimum or the button wont do 
//				anything.
//
// Parameters:  
//		[IN]	id
//				resource id, one of the resources already imported with the 
//				resource editor, usually begins with IDR_  
//
//		[IN]	pType
//				pointer to string describing the resource type
//				
// Returns:     BOOL
//				Non zero if successful, otherwise zero
//
//=============================================================================
BOOL CGdipButton::LoadStdImage(UINT id, LPCTSTR pType)
{
	m_pStdImage = new CGdiPlusBitmapResource;
	return m_pStdImage->Load(id, pType);
}

void CGdipButton::Reset()
{
	delete m_pStdImage;
	m_pStdImage = NULL;
	delete m_pAltImage;
	m_pAltImage = NULL;
	delete m_pToolTip;
	m_pToolTip = NULL;

	m_bHaveBitmaps = FALSE;
	m_bHaveAltImage = FALSE;

	m_dcStd.DeleteDC();
	m_dcStdP.DeleteDC();
	m_dcStdH.DeleteDC();
	m_dcBk.DeleteDC();
}

void CGdipButton::Test(CString c)
{
	m_pStdImage = new CGdiPlusBitmapResource;
	m_pStdImage->Loads(c);
}

//=============================================================================
//
// LoadAltImage()
//
// Purpose:     The LoadAltImage() Loads the altername image for the button.  
//				This function call is optional
// Parameters:  
//		[IN]	id
//				resource id, one of the resources already imported with the 
//				resource editor, usually begins with IDR_  
//
//		[IN]	pType
//				pointer to string describing the resource type
//				
// Returns:     BOOL
//				Non zero if successful, otherwise zero
//
//=============================================================================
BOOL CGdipButton::LoadAltImage(UINT id, LPCTSTR pType)
{
	m_bHaveAltImage = TRUE;
	m_pAltImage = new CGdiPlusBitmapResource;
	return (m_pAltImage->Load(id, pType));
}


//=============================================================================
//
//	The bitmaps are created here on the first draw. Everything is done with a
//	memory DC except the background, which gets it's information from the
//	parent. The background is needed for transparent portions of PNG images.
//	An always on top app (such as Task Manager) that is in the way can cause it
//	to get an incorrect background.  To avoid this, the parent should call the
//	SetBkGnd function with a memory DC when it creates the background.
//
//=============================================================================
void CGdipButton::InitBitmaps(HDC hDC)
{
	if(m_bHaveBitmaps)
		return;

	if(!m_pStdImage)
		return; // Load the standard image with LoadStdImage()

	WTL::CBitmap bmp;

	CRect rect;
	GetClientRect(&rect);

	// do everything with mem dc
	CMemDCEx memDC(hDC, rect);

	Gdiplus::Graphics graphics(memDC.m_hDC);

	// background
	if (m_dcBk.m_hDC == NULL)
	{
		CRect rect1;
		WTL::CClientDC clDC(GetParent());
		GetWindowRect(&rect1);
		GetParent().ScreenToClient(&rect1);

		m_dcBk.CreateCompatibleDC(clDC);
		bmp.CreateCompatibleBitmap(clDC, rect.Width(), rect.Height());
		m_dcBk.SelectBitmap(bmp);
		m_dcBk.BitBlt(0, 0, rect.Width(), rect.Height(), clDC, rect1.left, rect1.top, SRCCOPY);
		bmp.DeleteObject();
	}

	// standard image
	if (m_dcStd.m_hDC == NULL)
	{
		PaintBk(memDC);

		float width = (float)m_pStdImage->m_pBitmap->GetWidth();
		float height = (float)m_pStdImage->m_pBitmap->GetHeight();

		RectF grect; grect.X = 0, grect.Y = 0; grect.Width = width; grect.Height = height;

		graphics.DrawImage(*m_pStdImage, grect, 0, 0, width, height, UnitPixel);

		m_dcStd.CreateCompatibleDC(memDC);
		bmp.CreateCompatibleBitmap(memDC, rect.Width(), rect.Height());
		m_dcStd.SelectBitmap(bmp);
		m_dcStd.BitBlt(0, 0, rect.Width(), rect.Height(), memDC, 0, 0, SRCCOPY);
		bmp.DeleteObject();

		// standard image pressed
		if (m_dcStdP.m_hDC == NULL)
		{
			PaintBk(memDC);

			float width = (float)m_pStdImage->m_pBitmap->GetWidth();
			float height = (float)m_pStdImage->m_pBitmap->GetHeight();

			RectF grect; grect.X = 0, grect.Y = 0; grect.Width = width; grect.Height = height;

			graphics.DrawImage(*m_pStdImage, grect, -1, -1, width, height, UnitPixel);

			m_dcStdP.CreateCompatibleDC(memDC);
			bmp.CreateCompatibleBitmap(memDC, rect.Width(), rect.Height());
			m_dcStdP.SelectBitmap(bmp);
			m_dcStdP.BitBlt(0, 0, rect.Width(), rect.Height(), memDC, 0, 0, SRCCOPY);
			bmp.DeleteObject();
		}

		// standard image hot
		if(m_dcStdH.m_hDC == NULL)
		{
			PaintBk(memDC);

			ColorMatrix HotMat = {	1.05f, 0.00f, 0.00f, 0.00f, 0.00f,
									0.00f, 1.05f, 0.00f, 0.00f, 0.00f,
									0.00f, 0.00f, 1.05f, 0.00f, 0.00f,
									0.00f, 0.00f, 0.00f, 1.00f, 0.00f,
									0.05f, 0.05f, 0.05f, 0.00f, 1.00f	};

			ImageAttributes ia;
			ia.SetColorMatrix(&HotMat);

			float width = (float)m_pStdImage->m_pBitmap->GetWidth();
			float height = (float)m_pStdImage->m_pBitmap->GetHeight();

			RectF grect; grect.X=0, grect.Y=0; grect.Width = width; grect.Height = height;

			graphics.DrawImage(*m_pStdImage, grect, 0, 0, width, height, UnitPixel, &ia);

			m_dcStdH.CreateCompatibleDC(memDC);
			bmp.CreateCompatibleBitmap(memDC, rect.Width(), rect.Height());
			m_dcStdH.SelectBitmap(bmp);
			m_dcStdH.BitBlt(0, 0, rect.Width(), rect.Height(), memDC, 0, 0, SRCCOPY);
			bmp.DeleteObject();
		}

		// grayscale image
		if(m_dcGS.m_hDC == NULL)
		{
			PaintBk(memDC);

			ColorMatrix GrayMat = {	0.30f, 0.30f, 0.30f, 0.00f, 0.00f,
									0.59f, 0.59f, 0.59f, 0.00f, 0.00f,
									0.11f, 0.11f, 0.11f, 0.00f, 0.00f,
									0.00f, 0.00f, 0.00f, 1.00f, 0.00f,
									0.00f, 0.00f, 0.00f, 0.00f, 1.00f	};

			ImageAttributes ia;
			ia.SetColorMatrix(&GrayMat);

			float width = (float)m_pStdImage->m_pBitmap->GetWidth();
			float height = (float)m_pStdImage->m_pBitmap->GetHeight();

			RectF grect; grect.X=0, grect.Y=0; grect.Width = width; grect.Height = height;

			graphics.DrawImage(*m_pStdImage, grect, 0, 0, width, height, UnitPixel, &ia);

			m_dcGS.CreateCompatibleDC(memDC);
			bmp.CreateCompatibleBitmap(memDC, rect.Width(), rect.Height());
			m_dcGS.SelectBitmap(bmp);
			m_dcGS.BitBlt(0, 0, rect.Width(), rect.Height(), memDC, 0, 0, SRCCOPY);
			bmp.DeleteObject();
		}
	}

	// alternate image
	if( (m_dcAlt.m_hDC == NULL) && m_bHaveAltImage )
	{
		PaintBk(memDC);

		graphics.DrawImage(*m_pAltImage, 0, 0);

		m_dcAlt.CreateCompatibleDC(memDC);
		bmp.CreateCompatibleBitmap(memDC, rect.Width(), rect.Height());
		m_dcAlt.SelectBitmap(bmp);
		m_dcAlt.BitBlt(0, 0, rect.Width(), rect.Height(), memDC, 0, 0, SRCCOPY);
		bmp.DeleteObject();

		// alternate image pressed
		if( (m_dcAltP.m_hDC == NULL) && m_bHaveAltImage )
		{
			PaintBk(memDC);

			graphics.DrawImage(*m_pAltImage, 1, 1);

			m_dcAltP.CreateCompatibleDC(memDC);
			bmp.CreateCompatibleBitmap(memDC, rect.Width(), rect.Height());
			m_dcAltP.SelectBitmap(bmp);
			m_dcAltP.BitBlt(0, 0, rect.Width(), rect.Height(), memDC, 0, 0, SRCCOPY);
			bmp.DeleteObject();
		}

		// alternate image hot
		if(m_dcAltH.m_hDC == NULL)
		{
			PaintBk(memDC);

			ColorMatrix HotMat = {	1.05f, 0.00f, 0.00f, 0.00f, 0.00f,
									0.00f, 1.05f, 0.00f, 0.00f, 0.00f,
									0.00f, 0.00f, 1.05f, 0.00f, 0.00f,
									0.00f, 0.00f, 0.00f, 1.00f, 0.00f,
									0.05f, 0.05f, 0.05f, 0.00f, 1.00f	};

			ImageAttributes ia;
			ia.SetColorMatrix(&HotMat);

			float width = (float)m_pStdImage->m_pBitmap->GetWidth();
			float height = (float)m_pStdImage->m_pBitmap->GetHeight();

			RectF grect; grect.X=0, grect.Y=0; grect.Width = width; grect.Height = height;

			graphics.DrawImage(*m_pAltImage, grect, 0, 0, width, height, UnitPixel, &ia);

			m_dcAltH.CreateCompatibleDC(memDC);
			bmp.CreateCompatibleBitmap(memDC, rect.Width(), rect.Height());
			m_dcAltH.SelectBitmap(bmp);
			m_dcAltH.BitBlt(0, 0, rect.Width(), rect.Height(), memDC, 0, 0, SRCCOPY);
			bmp.DeleteObject();
		}
	}

	if(m_pCurBtn == NULL)
	{
		m_pCurBtn = &m_dcStd;
	}

	m_bHaveBitmaps = TRUE;
}

//=============================================================================
// paint the background
//=============================================================================
void CGdipButton::PaintBk(HDC hDC)
{
	if (m_dcBk.m_hDC == NULL)
		return;

	CRect rect;
	GetClientRect(&rect);
	::BitBlt(hDC, 0, 0, rect.Width(), rect.Height(), m_dcBk.m_hDC, 0, 0, SRCCOPY);
}

//=============================================================================
// paint the bitmap currently pointed to with m_pCurBtn
//=============================================================================
void CGdipButton::PaintBtn(HDC hDC)
{
	if (m_pCurBtn == NULL || m_pCurBtn->m_hDC == NULL)
		return;

	CRect rect;
	GetClientRect(&rect);
	::BitBlt(hDC, 0, 0, rect.Width(), rect.Height(), m_pCurBtn->m_hDC, 0, 0, SRCCOPY);
}

//=============================================================================
// enables the toggle mode
// returns if it doesn't have the alternate image
//=============================================================================
void CGdipButton::EnableToggle(BOOL bEnable)
{
	if(!m_bHaveAltImage) return;

	m_bIsToggle = bEnable; 

	// this actually makes it start in the std state since toggle is called before paint
	if(bEnable)	m_pCurBtn = &m_dcAlt;
	else		m_pCurBtn = &m_dcStd;

}

//=============================================================================
// sets the image type and disabled state then repaints
//=============================================================================
void CGdipButton::SetImage(int type)
{
	m_nCurType = type;

	(type == DIS_TYPE) ? m_bIsDisabled = TRUE : m_bIsDisabled = FALSE;

	Invalidate();
}

//=============================================================================
// disable double click 
//=============================================================================
LRESULT CGdipButton::OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// a double click is only a second single click for this button
	return 0;
}


//=============================================================================
// overide the erase function
//=============================================================================
LRESULT CGdipButton::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
{
	return 1;
}

//=============================================================================
// Paint the button depending on the state of the mouse
//=============================================================================
LRESULT CGdipButton::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)lParam;
	if(lpDIS == NULL)
		return 0;

	HDC hDC = lpDIS->hDC;

	InitBitmaps(hDC);

	// handle disabled state
	if(m_bIsDisabled)
	{
		m_pCurBtn = &m_dcGS;
		PaintBtn(hDC);
		return 0;
	}

	BOOL bIsPressed = (lpDIS->itemState & ODS_SELECTED);

	// handle toggle button
	if(m_bIsToggle && bIsPressed)
	{
		(m_nCurType == STD_TYPE) ? m_nCurType = ALT_TYPE : m_nCurType = STD_TYPE;
	}

	if(bIsPressed)
	{
		if(m_nCurType == STD_TYPE)
			m_pCurBtn = &m_dcStdP;
		else
			m_pCurBtn = &m_dcAltP;
	}
	else if(m_bIsHovering)
	{

		if(m_nCurType == STD_TYPE)
			m_pCurBtn = &m_dcStdH;
		else
			m_pCurBtn = &m_dcAltH;
	}
	else
	{
		if(m_nCurType == STD_TYPE)
			m_pCurBtn = &m_dcStd;
		else
			m_pCurBtn = &m_dcAlt;
	}

	// paint the button
	PaintBtn(hDC);

	return 0;
}

//=============================================================================
LRESULT CGdipButton::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
//=============================================================================
{
	m_bIsHovering = TRUE;
	Invalidate();
	DeleteToolTip();

	// Create a new Tooltip with new Button Size and Location
	SetToolTipText(m_tooltext);

	if (m_pToolTip != NULL)
	{
		if (::IsWindow(m_pToolTip->m_hWnd))
		{
			//Display ToolTip
			m_pToolTip->Update();
		}
	}

	return 0;
}


//=============================================================================
LRESULT CGdipButton::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//=============================================================================
{
	m_bIsTracking = FALSE;
	m_bIsHovering = FALSE;
	Invalidate();
	return 0;
}

//=============================================================================
LRESULT CGdipButton::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_bIsTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bIsTracking = _TrackMouseEvent(&tme);
	}
	
bHandled = FALSE;
	return 0;
}

//=============================================================================
//	
//	Call this member function with a memory DC from the code that paints 
//	the parents background.  Passing the screen DC defeats the purpose of 
//  using this function.
//
//=============================================================================
void CGdipButton::SetBkGnd(HDC hDC)
{
	CRect rect, rectS;
	WTL::CBitmap bmp;

	GetClientRect(&rect);
	GetWindowRect(&rectS);
	GetParent().ScreenToClient(&rectS);

	m_dcBk.DeleteDC();

	m_dcBk.CreateCompatibleDC(hDC);
	bmp.CreateCompatibleBitmap(hDC, rect.Width(), rect.Height());
	m_dcBk.SelectBitmap(bmp);
	m_dcBk.BitBlt(0, 0, rect.Width(), rect.Height(), hDC, rectS.left, rectS.top, SRCCOPY);
	bmp.DeleteObject();
}


//=============================================================================
// Set the tooltip with a string resource
//=============================================================================
void CGdipButton::SetToolTipText(UINT nId, BOOL bActivate)
{
	// load string resource
	m_tooltext.LoadString(nId);

	// If string resource is not empty
	if (m_tooltext.IsEmpty() == FALSE)
	{
		SetToolTipText(m_tooltext, bActivate);
	}

}

//=============================================================================
// Set the tooltip with a CString
//=============================================================================
void CGdipButton::SetToolTipText(CString spText, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (spText.IsEmpty()) return;

	// Initialize ToolTip
	InitToolTip();
	m_tooltext = spText;

	// If there is no tooltip defined then add it
	if (m_pToolTip->GetToolCount() == 0)
	{
		CRect rectBtn;
		GetClientRect(&rectBtn);
		WTL::CToolInfo ti(TTF_SUBCLASS, m_hWnd, 0, &rectBtn, (LPTSTR)(LPCTSTR)m_tooltext);
		m_pToolTip->AddTool(ti);
	}

	// Set text for tooltip
	m_pToolTip->UpdateTipText((LPCTSTR)m_tooltext, m_hWnd);
	m_pToolTip->SetDelayTime(TTDT_INITIAL, 2000);
	m_pToolTip->Activate(bActivate);
}

//=============================================================================
void CGdipButton::InitToolTip()
//=============================================================================
{
	if (m_pToolTip == NULL)
	{
		m_pToolTip = new WTL::CToolTipCtrl;
		// Create ToolTip control
		m_pToolTip->Create(m_hWnd);
		m_pToolTip->Activate(TRUE);
	}
} 

//=============================================================================
void CGdipButton::DeleteToolTip()
//=============================================================================
{
	// Destroy Tooltip incase the size of the button has changed.
	if (m_pToolTip != NULL)
	{
		delete m_pToolTip;
		m_pToolTip = NULL;
	}
}

