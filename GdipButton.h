//
// GdipButton.h : Version 1.0 - see article at CodeProject.com
//
// Author:  Darren Sessions
//          
//
// Description:
//     GdipButton is a button control that uses GDI+ 
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

#pragma once

class CGdiPlusBitmapResource;

#ifdef _WIN32
#ifndef _WINDOWS_
#include <windows.h>
#endif
#endif
/////////////////////////////////////////////////////////////////////////////
// CGdipButton window

class CGdipButton : public ATL::CWindowImpl<CGdipButton, WTL::CButton>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, WTL::CButton::GetWndClassName())

	CGdipButton();
	virtual ~CGdipButton();

	// image types
	enum	{
				STD_TYPE	= 0,
				ALT_TYPE,
				DIS_TYPE
			};

	// sets the image type
	void SetImage(int type);

	void Test(CString c);
	BOOL LoadAltImage(UINT id, LPCTSTR pType);
	BOOL LoadStdImage(UINT id, LPCTSTR pType);

	BOOL LoadStdImageDPI(int dpi, UINT id96, UINT id120, UINT id144, UINT id168, UINT id192, LPCTSTR pType, UINT id225 = 0, UINT id250 = 0, UINT id275 = 0, UINT id300 = 0, UINT id325 = 0, UINT id350 = 0);

	// if false, disables the press state and uses grayscale image if it exists
	void EnableButton(BOOL bEnable = TRUE) { m_bIsDisabled = !bEnable; }

	// in toggle mode each press toggles between std and alt images
	void EnableToggle(BOOL bEnable = TRUE);

	// return the enable/disable state
	BOOL IsDisabled(void) {return (m_bIsDisabled == TRUE); }

	void SetBkGnd(HDC hDC);

	void SetToolTipText(CString spText, BOOL bActivate = TRUE);
	void SetToolTipText(UINT nId, BOOL bActivate = TRUE);
	void SetHorizontal(bool ImagesAreLaidOutHorizontally = FALSE);
	void DeleteToolTip();

	void Reset();


protected:

	// creates the offscreen DCs from the images, using hDC as the color source
	void	InitBitmaps(HDC hDC);
	void PaintBk(HDC hDC);
	void PaintBtn(HDC hDC);

	BOOL	m_bHaveAltImage;
	BOOL	m_bHaveBitmaps;

	BOOL	m_bIsDisabled;
	BOOL	m_bIsToggle;
	BOOL	m_bIsHovering;
	BOOL	m_bIsTracking;

	int		m_nCurType;

	CGdiPlusBitmapResource* m_pAltImage;
	CGdiPlusBitmapResource* m_pStdImage;

	CString			m_tooltext;
	WTL::CToolTipCtrl*	m_pToolTip;
	
	void	InitToolTip();

	BEGIN_MSG_MAP_EX(CGdipButton)
		MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_MOUSEHOVER, OnMouseHover)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
	END_MSG_MAP()

	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:

	WTL::CDC		m_dcBk;			// button background
	
	WTL::CDC		m_dcStd;		// standard button
	WTL::CDC		m_dcStdP;		// standard button pressed
	WTL::CDC		m_dcStdH;		// standard button hot

	WTL::CDC		m_dcAlt;		// alternate button
	WTL::CDC		m_dcAltP;		// alternate button pressed
	WTL::CDC		m_dcAltH;		// alternate button hot

	WTL::CDC		m_dcGS;			// grayscale button (does not have a hot or pressed state)

	WTL::CDC*	m_pCurBtn;		// current pointer to one of the above

};
