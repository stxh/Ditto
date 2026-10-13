// ImageViewer.cpp : implementation file
//

#include "stdafx.h"
#include "CP_Main.h"
#include "ImageViewer.h"
#include "BitmapHelper.h"
#include "memdc.h"


// CImageViewer

CImageViewer::CImageViewer()
{
	m_scrollHelper.AttachWnd(m_hWnd);
	m_hoveringOverImage = false;
	m_pGdiplusBitmap = NULL;
	m_scale = 1;
}

CImageViewer::~CImageViewer()
{
	delete m_pGdiplusBitmap;
}


BOOL CImageViewer::Create(HWND hParent)
{
	BOOL bSuccess;

	// Register window class
	CString csClassName = AtlRegisterClass(NULL, CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
		LoadCursor(NULL, IDC_ARROW),
		CBrush(::GetSysColor(COLOR_BTNFACE)), NULL, NULL);

	// If no parent supplied then try and get a pointer to it anyway
	if (!hParent)
		hParent = GetMainWnd();

	// Create popup window
	bSuccess = Create(NULL,
		csClassName,
		_T(""),
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
		0, 0,
		390, 130,
		hParent,
		NULL);
	if (!bSuccess) return FALSE;

	return TRUE;
}

void CImageViewer::UpdateBitmapSize(bool setScale)
{
	if (m_pGdiplusBitmap != NULL)
	{
		if (setScale)
		{
			BOOL newScaleImage = CGetSetOptions::GetScaleImagesToDescWindow();
			if (newScaleImage)
			{
				CRect rect;
				GetClientRect(&rect);

				SCROLLINFO si;
				if (GetScrollInfo(SB_HORZ, &si) && si.nPage > 0)
				{
					int cxSB = ::GetSystemMetrics(SM_CXVSCROLL);
					rect.right += cxSB;
					int cySB = ::GetSystemMetrics(SM_CYHSCROLL);
					rect.bottom += cySB;
				}

				double w = m_pGdiplusBitmap->GetWidth();
				if (w > 0)
				{
					m_scale = rect.Width() / w;
				}
			}
			else
			{
				m_scale = 1;
			}

			m_scrollHelper.ScrollToOrigin(true, true);
		}

		m_scrollHelper.AttachWnd(m_hWnd);
		m_scrollHelper.SetDisplaySize(m_pGdiplusBitmap->GetWidth(), m_pGdiplusBitmap->GetHeight(), m_scale);

		GetParent().PostMessage(WM_REFRESH_FOOTER, 0, 0);
	}
}

LRESULT CImageViewer::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(&ps);

	CMemDCEx memDC(hdc);

	CRect rect;
	GetClientRect(&rect);

	CBrush Brush;
	Brush.CreateSolidBrush(CGetSetOptions::m_Theme.DescriptionWindowBG());

	HDC memHDC = memDC;
	HBRUSH hOldBrush = (HBRUSH)SelectObject(memHDC, Brush);

	FillRect(memHDC, &rect, Brush);

	if (m_pGdiplusBitmap)
	{
		int width = m_pGdiplusBitmap->GetWidth();
		int height = m_pGdiplusBitmap->GetHeight();

		Gdiplus::ImageAttributes attrs;
		CSize s = m_scrollHelper.GetScrollPos();

		Gdiplus::Graphics graphics(memHDC);
		graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

		Gdiplus::Rect dest((int)0, (int)0, (int)rect.Width(), (int)rect.Height());

		double nW = rect.Width() * (1 / m_scale);
		double nH = rect.Height() * (1 / m_scale);

		graphics.DrawImage(m_pGdiplusBitmap, dest, s.cx, s.cy, nW, nH, Gdiplus::UnitPixel, &attrs);
	}

	SelectObject(memHDC, hOldBrush);

	EndPaint(&ps);

	return 0;
}


LRESULT CImageViewer::OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nSBCode = (UINT)LOWORD(wParam);
	UINT nPos = (UINT)HIWORD(wParam);
	HWND hScrollBar = (HWND)lParam;

	CScrollBar scrollBar;
	if (hScrollBar)
		scrollBar.Attach(hScrollBar);

	m_scrollHelper.OnHScroll(nSBCode, nPos, &scrollBar);

	if (hScrollBar)
		scrollBar.Detach();

	return 0;
}

LRESULT CImageViewer::OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nSBCode = (UINT)LOWORD(wParam);
	UINT nPos = (UINT)HIWORD(wParam);
	HWND hScrollBar = (HWND)lParam;

	CScrollBar scrollBar;
	if (hScrollBar)
		scrollBar.Attach(hScrollBar);

	m_scrollHelper.OnVScroll(nSBCode, nPos, &scrollBar);

	if (hScrollBar)
		scrollBar.Detach();

	return 0;
}

LRESULT CImageViewer::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	OutputDebugString(_T("OnMouseWheel\r\n"));

	UINT nFlags = (UINT)LOWORD(wParam);
	short zDelta = (short)HIWORD(wParam);
	CPoint pt;
	pt.x = (short)LOWORD(lParam);
	pt.y = (short)HIWORD(lParam);

	if (nFlags == MK_CONTROL)
	{
		LockWindowUpdate();
		CPoint delta;
		double oldScale = m_scale;

		if (zDelta > 0)
		{
			m_scale += .1;
		}
		else
		{
			m_scale -= .1;
		}

		m_scale = max(m_scale, 0.01);

		::ScreenToClient(m_hWnd, &pt);
		UpdateBitmapSize(false);

		if (oldScale > 0 && m_scale > 0)
		{
			delta.x = round((pt.x * (1 / oldScale)) - (pt.x * (1 / m_scale)));
			delta.y = round((pt.y * (1 / oldScale)) - (pt.y * (1 / m_scale)));

			m_scrollHelper.Update(delta);
		}

		Invalidate();

		UnlockWindowUpdate();
	}
	else
	{
		BOOL wasScrolled = m_scrollHelper.OnMouseWheel(nFlags, zDelta, pt);
		return wasScrolled;
	}

	return TRUE;
}

LRESULT CImageViewer::OnMouseHWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	OutputDebugString(_T("OnMouseHWheel\r\n"));

	UINT nFlags = (UINT)LOWORD(wParam);
	short zDelta = (short)HIWORD(wParam);
	CPoint pt;
	pt.x = (short)LOWORD(lParam);
	pt.y = (short)HIWORD(lParam);

	BOOL wasScrolled = m_scrollHelper.OnMouseHWheel(nFlags, -zDelta, pt);

	return 0;
}

LRESULT CImageViewer::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nType = (UINT)wParam;
	int cx = (int)(short)LOWORD(lParam);
	int cy = (int)(short)HIWORD(lParam);

	m_scrollHelper.OnSize(nType, cx, cy);

	return 0;
}

LRESULT CImageViewer::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hWnd = (HWND)wParam;
	UINT nHitTest = (UINT)LOWORD(lParam);
	UINT message = (UINT)HIWORD(lParam);

	if (this->m_pGdiplusBitmap &&
		hWnd == m_hWnd &&
		nHitTest == HTCLIENT)
	{
		if (CGetSetOptions::GetScaleImagesToDescWindow())
		{
			::SetCursor(LoadCursor(ATL_BASE_MODULE, MAKEINTRESOURCE(IDC_CURSOR_ZOOM_IN)));
		}
		else
		{
			::SetCursor(LoadCursor(ATL_BASE_MODULE, MAKEINTRESOURCE(IDC_CURSOR_ZOOM_OUT)));
		}

		m_hoveringOverImage = true;

		return TRUE;
	}

	m_hoveringOverImage = false;

	return DefWindowProc(uMsg, wParam, lParam);
}


LRESULT CImageViewer::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nFlags = (UINT)wParam;
	CPoint point;
	point.x = (int)(short)LOWORD(lParam);
	point.y = (int)(short)HIWORD(lParam);

	if (this->m_pGdiplusBitmap &&
		m_hoveringOverImage)
	{
		CGetSetOptions::SetScaleImagesToDescWindow(!CGetSetOptions::GetScaleImagesToDescWindow());

		UpdateBitmapSize(true);

		Invalidate();

		return 0;
	}

	return DefWindowProc(uMsg, wParam, lParam);
}

LRESULT CImageViewer::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return FALSE;
}

LRESULT CImageViewer::OnGesture(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPoint ptZoomCenter;
	double k;
	GESTUREINFO gi;

	ZeroMemory(&gi, sizeof(GESTUREINFO));

	gi.cbSize = sizeof(GESTUREINFO);

	BOOL bResult = GetGestureInfo((HGESTUREINFO)lParam, &gi);
	BOOL bHandledGesture = FALSE;

	if (bResult) {
		switch (gi.dwID) {
		case GID_ZOOM:
			bHandledGesture = TRUE;

			switch (gi.dwFlags)
			{
			case GF_BEGIN:
				m_dwArguments = LODWORD(gi.ullArguments);
				m_ptFirst.x = gi.ptsLocation.x;
				m_ptFirst.y = gi.ptsLocation.y;
				::ScreenToClient(m_hWnd, &m_ptFirst);
				OutputDebugString(_T("zoom start\r\n"));
				break;

			case GF_END:
				OutputDebugString(_T("zoom end\r\n"));
				break;

			default:
				m_ptSecond.x = gi.ptsLocation.x;
				m_ptSecond.y = gi.ptsLocation.y;
				::ScreenToClient(m_hWnd, &m_ptSecond);

				ptZoomCenter.x = (m_ptFirst.x + m_ptSecond.x) / 2;
				ptZoomCenter.y = (m_ptFirst.y + m_ptSecond.y) / 2;

				k = (double)(LODWORD(gi.ullArguments)) / (double)(m_dwArguments);

				CString cs;
				cs.Format(_T("ZOOM k: %f, x: %d, y: %d\r\n"), k, ptZoomCenter.x, ptZoomCenter.y);
				OutputDebugString(cs);

				m_ptFirst = m_ptSecond;
				break;
			}
			break;
		case GID_PAN:
			bHandledGesture = TRUE;
			switch (gi.dwFlags)
			{
			case GF_BEGIN:
				m_ptFirst.x = gi.ptsLocation.x;
				m_ptFirst.y = gi.ptsLocation.y;
				::ScreenToClient(m_hWnd, &m_ptFirst);
				break;

			default:
				m_ptSecond.x = gi.ptsLocation.x;
				m_ptSecond.y = gi.ptsLocation.y;
				::ScreenToClient(m_hWnd, &m_ptSecond);

				int xDiff = m_ptSecond.x - m_ptFirst.x;

				int yDiff = m_ptSecond.y - m_ptFirst.y;

				m_scrollHelper.Update(CPoint(-xDiff, -yDiff));

				m_ptFirst = m_ptSecond;
				break;
			}

			break;
		case GID_ROTATE:
			OutputDebugString(_T("rotate\r\n"));
			bHandledGesture = TRUE;
			break;
		case GID_TWOFINGERTAP:
			OutputDebugString(_T("two finger\r\n"));
			bHandledGesture = TRUE;
			break;
		case GID_PRESSANDTAP:
			OutputDebugString(_T("press and tap\r\n"));
			bHandledGesture = TRUE;
			break;
		default:
			OutputDebugString(_T("default\r\n"));
			break;
		}
	}
	else {
		DWORD dwErr = GetLastError();
		if (dwErr > 0) {
			OutputDebugString(_T("error\r\n"));
		}
	}

	return FALSE;
}

LRESULT CImageViewer::OnGestureNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	GESTURECONFIG gc = {
		0,
		GC_ALLGESTURES,
		0
	};

	BOOL bResult = ::SetGestureConfig(
		m_hWnd,
		0,
		1,
		&gc,
		sizeof(GESTURECONFIG)
	);

	return TRUE;
}