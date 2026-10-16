// QRCodeViewer.cpp : implementation file
//

#include "stdafx.h"
#include "CP_Main.h"
#include "QRCodeViewer.h"
#include "MainTableFunctions.h"

#define TIMER_BUTTON_UP 1

// QRCodeViewer


QRCodeViewer::QRCodeViewer()
{
	m_descBackground = NULL;
	m_bitmapData = NULL;
	m_imageSize = 0;
	m_descRowHeight = 0;
	m_originalFontHeight = 0;
}

QRCodeViewer::~QRCodeViewer()
{
	if(m_descBackground != NULL)
	{
		DeleteObject(m_descBackground);
	}
}


BOOL QRCodeViewer::Create(CWindow *pParentWnd, unsigned char* bitmapData, int imageSize, CString desc, int rowHeight, LOGFONT logFont)
{
	m_bitmapData = bitmapData;
	m_imageSize = imageSize;
	m_descRowHeight = rowHeight;
	m_descBackground = CreateSolidBrush(RGB(255, 255, 255));
	m_logFont = logFont;
	m_originalFontHeight = logFont.lfHeight;

	CRect rcStart(0, 0, 0, 0);
	if(CWindowImpl<QRCodeViewer, CWindow, CNullTraits>::Create(NULL, &rcStart, _T(""), WS_POPUP, 0, 0U, NULL) != NULL)
	{	
		m_desc.Create(m_hWnd, CRect(0, 0, 0, 0), CMainTableFunctions::GetDisplayText(CGetSetOptions::m_nLinesPerRow, desc),
			WS_CHILD | WS_VISIBLE, 0, 2);

		m_font.CreateFontIndirect(&logFont);
		m_desc.SetFont(m_font, TRUE);	

		m_DittoWindow.DoCreate(this);
		m_DittoWindow.SetCaptionColors(CGetSetOptions::m_Theme.CaptionLeft(), CGetSetOptions::m_Theme.CaptionRight(), CGetSetOptions::m_Theme.Border());
		m_DittoWindow.SetCaptionOn(this, CGetSetOptions::GetCaptionPos(), true, CGetSetOptions::m_Theme.GetCaptionSize(), CGetSetOptions::m_Theme.GetCaptionFontSize());
		m_DittoWindow.m_bDrawMinimize = false;
		m_DittoWindow.m_bDrawMaximize = true;
		m_DittoWindow.m_bDrawChevron = false;
		m_DittoWindow.m_sendWMClose = false;

		m_qrCodeDrawer.LoadRaw(m_bitmapData, m_imageSize);

		delete[] m_bitmapData;
		m_bitmapData = NULL;

		CRect parentRect;
		pParentWnd->GetWindowRect(&parentRect);

		CRect rect;
		rect.left = parentRect.left;
		rect.top = parentRect.top;

		rect.right = rect.left + m_DittoWindow.m_borderSize + m_DittoWindow.m_borderSize + m_qrCodeDrawer.ImageWidth() + (CGetSetOptions::GetQRCodeBorderPixels() * 2);
		if (m_DittoWindow.m_captionPosition == CAPTION_LEFT ||
			m_DittoWindow.m_captionPosition == CAPTION_RIGHT)
		{
			rect.right += m_DittoWindow.m_captionBorderWidth;
		}
		rect.bottom = rect.top + m_DittoWindow.m_borderSize + m_DittoWindow.m_borderSize + rowHeight + 5 + m_qrCodeDrawer.ImageHeight() + (CGetSetOptions::GetQRCodeBorderPixels() * 2);
		
		CRect center = CenterRect(rect);

		EnsureWindowVisible(&center);

		::MoveWindow(m_hWnd, center.left, center.top, center.Width(), center.Height(), TRUE);

		MoveControls();

		SetFocus();
	}
	else
	{
		delete[] m_bitmapData;
		m_bitmapData = NULL;
	}

	return TRUE;
}

LRESULT QRCodeViewer::OnSize(UINT nType, CSize size)
{
	this->Invalidate();

	MoveControls();

	return 0;
}

void QRCodeViewer::MoveControls()
{
	CRect crRect;
	GetClientRect(&crRect);
	int cx = crRect.Width();
	int cy = crRect.Height();

	if(m_desc.m_hWnd != NULL)
	{
		m_desc.MoveWindow(m_DittoWindow.m_dpi.Scale(5), cy - m_DittoWindow.m_dpi.Scale(m_descRowHeight) - m_DittoWindow.m_dpi.Scale(5), cx - m_DittoWindow.m_dpi.Scale(10), m_DittoWindow.m_dpi.Scale(m_descRowHeight));
	}
}

LRESULT QRCodeViewer::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPaintDC dc(m_hWnd);

	CRect thisRect;
	GetClientRect(&thisRect);
	thisRect.bottom -= m_DittoWindow.m_dpi.Scale(m_descRowHeight) - m_DittoWindow.m_dpi.Scale(5);
	
	int width = thisRect.Width() - (CGetSetOptions::GetQRCodeBorderPixels() * 2);
	int height = min(width, (thisRect.Height() - (CGetSetOptions::GetQRCodeBorderPixels() * 2)));
	width = min(width, height);
		
	CRect imageRect(0, 0, width, height);

	CRect centerRect = CenterRectFromRect(imageRect, thisRect);

	m_qrCodeDrawer.Draw(dc, m_DittoWindow.m_dpi, m_hWnd, centerRect.left, centerRect.top, false, false, width, height);

	return 0;
}

LRESULT QRCodeViewer::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(wParam)
	{
	case VK_ESCAPE:
		::SendMessage(m_hWnd, WM_CLOSE, 0, 0);
		return 0;
	}

	bHandled = FALSE;
	return 0;
}

void QRCodeViewer::OnNcPaint(HRGN hRgn)
{
	m_DittoWindow.DoNcPaint(this);
}

LRESULT QRCodeViewer::OnNcCalcSize(BOOL bCalcValidRects, LPARAM lpncsp) 
{
	LRESULT lResult = DefWindowProc(WM_NCCALCSIZE, (WPARAM)bCalcValidRects, lpncsp);

	m_DittoWindow.DoNcCalcSize(bCalcValidRects, (NCCALCSIZE_PARAMS*)lpncsp);

	return lResult;
}

HITTEST_RET QRCodeViewer::OnNcHitTest(CPoint point) 
{
	UINT Ret = m_DittoWindow.DoNcHitTest(this, point);
	if(Ret == -1)
		return DefWindowProc(WM_NCHITTEST, 0, MAKELPARAM(point.x, point.y));

	return Ret;
}

BOOL QRCodeViewer::OnEraseBkgnd(CDCHandle dc) 
{
	CRect rect;
	GetClientRect(&rect);

	CBrush myBrush;
	myBrush.CreateSolidBrush(RGB(255, 255, 255));

	HBRUSH pOld = dc.SelectBrush(myBrush);
	BOOL bRes  = dc.PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);
	dc.SelectBrush(pOld);

	return TRUE;
}

void QRCodeViewer::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	int buttonPressed = m_DittoWindow.DoNcLButtonDown(this, nHitTest, point);

	if (buttonPressed != 0)
	{
		SetTimer(TIMER_BUTTON_UP, 100, NULL);
	}
}

void QRCodeViewer::OnNcLButtonUp(UINT nHitTest, CPoint point) 
{
	long lRet = m_DittoWindow.DoNcLButtonUp(this, nHitTest, point);

	switch(lRet)
	{
	case BUTTON_CLOSE:
		::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		break;
	}

	KillTimer(TIMER_BUTTON_UP);
}

void QRCodeViewer::OnNcMouseMove(UINT nHitTest, CPoint point) 
{
	m_DittoWindow.DoNcMouseMove(this, nHitTest, point);
}

HBRUSH QRCodeViewer::OnCtlColor(CDCHandle dc, CWindow wnd)
{
	if(wnd.GetDlgCtrlID() == 2)
	{
		dc.SetBkColor(RGB(255,255,255));

		return m_descBackground;
	}

	return NULL;
}

void QRCodeViewer::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
}

void QRCodeViewer::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
		case TIMER_BUTTON_UP:
		{
			if ((GetKeyState(VK_LBUTTON) & 0x100) == 0)
			{
				m_DittoWindow.DoNcLButtonUp(this, 0, CPoint(0, 0));
				KillTimer(TIMER_BUTTON_UP);
			}
			break;
		}
	}
}

LRESULT QRCodeViewer::OnDpiChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int dpi = HIWORD(wParam);
	m_DittoWindow.OnDpiChanged(this, dpi);

	RECT* const prcNewWindow = (RECT*)lParam;
	SetWindowPos(NULL,
		prcNewWindow->left,
		prcNewWindow->top,
		prcNewWindow->right - prcNewWindow->left,
		prcNewWindow->bottom - prcNewWindow->top,
		SWP_NOZORDER | SWP_NOACTIVATE);

	log(StrF(_T("QRCodeViewer::OnDpiChanged dpi: %d width: %d, height: %d"), dpi, (prcNewWindow->right - prcNewWindow->left), (prcNewWindow->bottom - prcNewWindow->top)));

	MoveControls();

	m_logFont.lfHeight = m_DittoWindow.m_dpi.Scale(m_originalFontHeight);

	if (!m_font.IsNull())
		m_font.DeleteObject();
	m_font.CreateFontIndirect(&m_logFont);
	m_desc.SetFont(m_font, TRUE);

	return TRUE;
}

void QRCodeViewer::OnMoving(UINT fwSide, LPRECT pRect)
{
	m_snap.OnSnapMoving(m_hWnd, pRect);
}

void QRCodeViewer::OnEnterSizeMove()
{
	m_snap.OnSnapEnterSizeMove(m_hWnd);
}
