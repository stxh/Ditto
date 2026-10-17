// AeroEdit.cpp : implementation file
//

#include "stdafx.h"
#include "SymbolEdit.h"
#include "cp_main.h"
#include "QListCtrl.h"
#include "Shared\TextConvert.h"

CSymbolEdit::CSymbolEdit() :
	m_hSymbolIcon(NULL),
	m_bInternalIcon(false),
	m_colorPromptText(RGB(127, 127, 127)),
	m_centerTextDiff(0)
{
	m_fontPrompt.CreateFont(
		16,
		0,
		0,
		0,
		FW_NORMAL,
		TRUE,
		FALSE,
		0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Calibri"));

	m_mouseDownOnSearches = false;
	m_mouseHoveringOverSearches = false;
	m_mouseDownOnClose = false;
	m_mouseHoveringOverClose = false;
	m_windowDpi = NULL;
}

CSymbolEdit::~CSymbolEdit()
{
	DestroyIcon();
}

BOOL CSymbolEdit::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	// Intercept Ctrl + Z (Undo), Ctrl + X (Cut), Ctrl + C (Copy), Ctrl + V (Paste) and Ctrl + A (Select All)
	// before CEdit base class gets a hold of them.
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
			{
				int startChar;
				int endChar;
				this->GetSel(startChar, endChar);
				if (startChar == endChar)
				{
					CWindow parent = GetParent();
					if (parent)
					{
						parent.SendMessage(NM_COPY_CLIP, pMsg->wParam, pMsg->lParam);
					}
				}
				else
				{
					Copy();
				}
			}
			return TRUE;
		case 'V':
			Paste();
			return TRUE;
		case 'A':
			SetSel(0, -1);
			return TRUE;
		}
	}

	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	{
		if (pMsg->wParam == VK_RETURN)
		{
			CWindow parent = GetParent();
			if (parent)
			{
				if (CGetSetOptions::m_bFindAsYouType)
				{
					parent.SendMessage(NM_SEARCH_ENTER_PRESSED, 0, 0);
				}
				else
				{
					parent.PostMessage(CB_SEARCH, 0, 0);
				}

				AddToSearchHistory();
			}

			return TRUE;
		}
		else if (pMsg->wParam == VK_DOWN &&
			((GetKeyState(VK_CONTROL) & 0x8000) || ((GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000))))
		{
			if (ShowSearchHistoryMenu())
			{
				return TRUE;
			}
		}
		else if (pMsg->wParam == VK_DOWN ||
			pMsg->wParam == VK_UP ||
			pMsg->wParam == VK_PRIOR ||
			pMsg->wParam == VK_NEXT)
		{
			CWindow parent = GetParent();
			if (parent)
			{
				parent.SendMessage(CB_UPDOWN, pMsg->wParam, pMsg->lParam);
				return TRUE;
			}
		}
		else if (pMsg->wParam == VK_DELETE)
		{
			int startChar;
			int endChar;
			this->GetSel(startChar, endChar);
			CString cs;
			this->GetWindowText(cs);
			if(startChar == cs.GetLength() &&
				endChar == cs.GetLength())
			{ 
				CWindow parent = GetParent();
				if (parent)
				{
					parent.SendMessage(NM_DELETE, pMsg->wParam, pMsg->lParam);
					return TRUE;
				}
			}
		}
		break;
	}
	}

	return FALSE;
}

CString CSymbolEdit::SavePastSearches()
{
	TiXmlDocument doc;

	TiXmlElement* outer = new TiXmlElement("PastSearches");
	doc.LinkEndChild(outer);

	int count = (int)m_searches.size();
	for (int i = 0; i < count; i++)
	{		
		TiXmlElement* searchElement = new TiXmlElement("Search");

		CStringA t = CTextConvert::UnicodeToUTF8(m_searches[i]);
		searchElement->SetAttribute("text", t);

		outer->LinkEndChild(searchElement);
	}

	TiXmlPrinter printer;
	printer.SetLineBreak("");
	doc.Accept(&printer);
	CString cs = printer.CStr();

	return cs;
}

void CSymbolEdit::LoadPastSearches(CString values)
{
	m_searches.clear();

	TiXmlDocument doc;
	CStringA xmlA = CTextConvert::UnicodeToUTF8(values);
	doc.Parse(xmlA);

	TiXmlElement *ItemHeader = doc.FirstChildElement("PastSearches");

	if (ItemHeader != NULL)
	{
		TiXmlElement *ItemElement = ItemHeader->FirstChildElement();

		int count = 0;

		while (ItemElement)
		{
			if (count < LIST_MAX_COUNT)
			{
				CString item = ItemElement->Attribute("text");

				CString toAdd = item.Left(MAX_SAVED_SEARCH_LENGTH);
				if (toAdd != _T(""))
				{
					m_searches.push_back(toAdd);
				}

				ItemElement = ItemElement->NextSiblingElement();
			}
			else
			{
				break;
			}

			count++;
		}
	}
}

void CSymbolEdit::AddToSearchHistory()
{
	CString cs;
	this->GetWindowText(cs);
	if (cs != _T(""))
	{
		//only save up to 50, had reports of somehow getting extremely large amounts of junk text
		//save and causing memory issues.
		cs = cs.Left(MAX_SAVED_SEARCH_LENGTH);

		if (m_searches.size() >= static_cast<size_t>(LIST_MAX_COUNT))
		{
			m_searches.erase(m_searches.begin());
		}

		bool existing = false;
		int count = (int)m_searches.size();
		for (int i = 0; i < count; i++)
		{
			if (m_searches[i] == cs)
			{
				m_searches.erase(m_searches.begin() + i);
				m_searches.push_back(cs);
				existing = true;
				break;
			}
		}

		if (existing == false)
		{
			m_searches.push_back(cs);
		}
	}
}

bool CSymbolEdit::ShowSearchHistoryMenu()
{
	if (m_searches.empty())
	{
		return false;
	}

	CMenu cmPopUp;
	cmPopUp.CreatePopupMenu();

	int count = min((int)m_searches.size(), LIST_MAX_COUNT);
	for (int i = count-1; i >= 0; i--)
	{
		CString text = m_searches[i];

		if (i == count - 1 &&
			m_lastSearchShortCut.Key > 0)
		{
			CString cmdShortcutText = CHotKey::GetHotKeyDisplayStatic(m_lastSearchShortCut.Key);
			if (m_lastSearchShortCut.Key2 != 0)
			{
				CString cmdShortcutText2 = CHotKey::GetHotKeyDisplayStatic(m_lastSearchShortCut.Key2);

				if (cmdShortcutText2.GetLength() > 0)
				{
					cmdShortcutText += _T(" - ");
					cmdShortcutText += cmdShortcutText2;
				}
			}

			text += "\t";
			text += cmdShortcutText;
		}

		cmPopUp.AppendMenuW(MF_STRING, (RANGE_START + i), text);
	}

	cmPopUp.AppendMenu(MF_SEPARATOR);
	cmPopUp.AppendMenuW(MF_STRING, CLEAR_LIST, _T("Clear List"));

	CRect windowRect;
	this->GetWindowRect(&windowRect);
	POINT pp;
	GetCursorPos(&pp);
	POINT x = { 0 };
	::GetCaretPos(&x);
	ClientToScreen(&x);
	x.y += windowRect.Height();

	cmPopUp.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, x.x, x.y, m_hWnd, NULL);
	
	Invalidate();
	
	return true;
}

void CSymbolEdit::DestroyIcon()
{
	// if icon was loaded internally, destroy it
	if (m_bInternalIcon || m_hSymbolIcon != NULL)
		::DestroyIcon(m_hSymbolIcon);
}

void CSymbolEdit::SetSymbolIcon(HICON hIcon, BOOL redraw)
{
	DestroyIcon();

	m_hSymbolIcon = hIcon;

	// icon was not loaded internally
	m_bInternalIcon = false;

	RecalcLayout();

	if (redraw)
		Invalidate(TRUE);
}

void CSymbolEdit::SetSymbolIcon(UINT id, BOOL redraw)
{
	DestroyIcon();

	m_hSymbolIcon = (HICON)::LoadImage(
		theApp.GetResourceInstance(),
		MAKEINTRESOURCE(id),
		IMAGE_ICON,
		16,
		16,
		LR_DEFAULTCOLOR | LR_LOADTRANSPARENT);

	ASSERT(m_hSymbolIcon != NULL);

	// icon was loaded internally
	m_bInternalIcon = true;

	RecalcLayout();

	if (redraw)
		Invalidate(TRUE);
}

void CSymbolEdit::SetPromptText(CString text, BOOL redraw)
{
	m_strPromptText = text;

	if (redraw)
		Invalidate(TRUE);
}

void CSymbolEdit::SetPromptText(LPCTSTR szText, BOOL redraw)
{
	m_strPromptText = szText;

	if (redraw)
		Invalidate(TRUE);
}

void CSymbolEdit::SetPromptTextColor(COLORREF color, BOOL redraw)
{
	m_colorPromptText = color;

	if (redraw)
		Invalidate(TRUE);
}

void CSymbolEdit::SetPromptFont(CFont& font, BOOL redraw)
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));

	font.GetLogFont(&lf);
	SetPromptFont(&lf);

	if (redraw)
		Invalidate(TRUE);
}

void CSymbolEdit::SetPromptFont(const LOGFONT* lpLogFont, BOOL redraw)
{
	if (!m_fontPrompt.IsNull())
		m_fontPrompt.DeleteObject();
	m_fontPrompt.CreateFontIndirect(lpLogFont);

	if (redraw)
		Invalidate(TRUE);
}

void CSymbolEdit::RecalcLayout()
{
	int width = GetSystemMetrics(SM_CXSMICON);

	if (m_hSymbolIcon)
	{
		DWORD dwMargins = GetMargins();
		SetMargins(LOWORD(dwMargins), width + 6);
	}
	else
	{
		if (m_windowDpi != NULL)
		{
			SetMargins(m_windowDpi->Scale(4), m_windowDpi->Scale(34));
		}
	}
}

LRESULT CSymbolEdit::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPaintDC dc(m_hWnd);

	CRect rect;
	GetClientRect(&rect);

	DWORD margins = GetMargins();
	
	CRect textRect(rect);
	textRect.left += LOWORD(margins);
	textRect.right -= HIWORD(margins);

	dc.FillSolidRect(rect, GetSysColor(COLOR_WINDOW));	

	if (m_hSymbolIcon)
	{
		int width = GetSystemMetrics(SM_CXSMICON);
		int height = GetSystemMetrics(SM_CYSMICON);

		::DrawIconEx(
			dc.m_hDC,
			rect.right - width - 1,
			1,
			m_hSymbolIcon,
			width,
			height,
			0,
			NULL,
			DI_NORMAL);

		rect.left += LOWORD(margins) + 1;
		rect.right -= (width + 7);
	}

	CString text;
	GetWindowText(text);
	HFONT oldFont = NULL;

	if (m_hWnd == ::GetFocus() || text.GetLength() > 0)
	{
		dc.FillSolidRect(rect, CGetSetOptions::m_Theme.SearchTextBoxFocusBG());

		oldFont = dc.SelectFont(GetFont());		

		COLORREF oldColor = dc.GetTextColor();
		dc.SetTextColor(CGetSetOptions::m_Theme.SearchTextBoxFocusText());
			
		dc.DrawText(text, text.GetLength(), textRect, DT_SINGLELINE | DT_INTERNAL | DT_EDITCONTROL | DT_NOPREFIX);

		dc.SelectFont(oldFont);
		dc.SetTextColor(oldColor);
	}
	else
	{
		dc.FillSolidRect(rect, CGetSetOptions::m_Theme.MainWindowBG());
	}


	if (text.GetLength() == 0 && m_strPromptText.GetLength() > 0)
	{
		textRect.right += m_windowDpi->Scale(16);

		oldFont = dc.SelectFont(m_fontPrompt);
		COLORREF color = dc.GetTextColor();
		dc.SetTextColor(m_colorPromptText);

		dc.DrawText(m_strPromptText, m_strPromptText.GetLength(), textRect, DT_LEFT | DT_SINGLELINE | DT_EDITCONTROL | DT_VCENTER | DT_NOPREFIX);
		dc.SetTextColor(color);
		dc.SelectFont(oldFont);
	}

	int right = rect.right;
	if (text.GetLength() > 0 || m_hWnd == ::GetFocus())
	{
		m_searchesButtonRect.SetRect(rect.right - m_windowDpi->Scale(18), 0, rect.right, rect.bottom);
		right = rect.right - m_windowDpi->Scale(18);
		m_searchesButton.Draw(dc.m_hDC, *m_windowDpi, m_hWnd, m_searchesButtonRect.left, 4, m_mouseHoveringOverSearches, m_mouseDownOnSearches);
	}
	else
	{
		m_searchesButtonRect.SetRect(0, 0, 0, 0);
	}

	if (text.GetLength() > 0)
	{
		m_closeButtonRect.SetRect(right - m_windowDpi->Scale(16), 0, right, rect.bottom);
		m_closeButton.Draw(dc.m_hDC, *m_windowDpi, m_hWnd, m_closeButtonRect.left, 4, m_mouseHoveringOverClose, m_mouseDownOnClose);
	}
	else
	{
		m_closeButtonRect.SetRect(0, 0, 0, 0);
	}

	//OutputDebugString(_T("OnPaint"));

	if (text != m_lastTextOnPaint &&
		text == _T(""))
	{
		::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	}

	m_lastTextOnPaint = text;

	//OutputDebugString(_T("OnPaint \r\n"));

	return 0;
}

LRESULT CSymbolEdit::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DefWindowProc(uMsg, wParam, lParam);

	RecalcLayout();
	bHandled = FALSE;
	return 0;
}

LRESULT CSymbolEdit::OnSetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DefWindowProc(uMsg, wParam, lParam);

	RecalcLayout();

	return 0;
}

HBRUSH CSymbolEdit::CtlColor(CDCHandle dc, HWND hwndEdit)
{
	COLORREF color = -1;

	if (::GetFocus() == m_hWnd)
	{
		dc.SetTextColor(CGetSetOptions::m_Theme.SearchTextBoxFocusText());
		dc.SetBkColor(CGetSetOptions::m_Theme.SearchTextBoxFocusBG());
		color = CGetSetOptions::m_Theme.SearchTextBoxFocusBG();
	}
	else
	{
		dc.SetBkColor(CGetSetOptions::m_Theme.MainWindowBG());
		color = CGetSetOptions::m_Theme.MainWindowBG();
	}

	if (color != m_lastBrushColor)
	{
		m_brush.DeleteObject();
		m_brush.CreateSolidBrush(color);
		m_lastBrushColor = color;
	}

	return m_brush;
}

LRESULT CSymbolEdit::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetTimer(1, 500, NULL);

	CWindow parent = GetParent();
	if (parent)
	{
		if (CGetSetOptions::m_bFindAsYouType)
		{
			parent.SendMessage(NM_FOCUS_ON_SEARCH, 0, 0);
		}
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CSymbolEdit::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	AddToSearchHistory();

	SetTimer(1, 500, NULL);

	bHandled = FALSE;
	return 0;
}

LRESULT CSymbolEdit::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPoint pntCursor;
	GetCursorPos(&pntCursor);
	ScreenToClient(&pntCursor);

	if(m_closeButtonRect.PtInRect(pntCursor))
	{
		HCURSOR h = ::LoadCursor(NULL, IDC_ARROW);
		::SetCursor(h);
		return TRUE;
	}

	if (m_searchesButtonRect.PtInRect(pntCursor))
	{
		HCURSOR h = ::LoadCursor(NULL, IDC_ARROW);
		::SetCursor(h);
		return TRUE;
	}

	bHandled = FALSE;
	return FALSE;
}

LRESULT CSymbolEdit::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	if (m_mouseDownOnClose)
	{
		ReleaseCapture();
		InvalidateRect(m_closeButtonRect);
	}

	if (m_mouseDownOnSearches)
	{
		ReleaseCapture();
		InvalidateRect(m_searchesButtonRect);
	}

	m_mouseDownOnClose = false;
	m_mouseDownOnSearches = false;

	if (m_closeButtonRect.PtInRect(point))
	{
		if ((GetWindowTextLength() > 0))
		{
			CWindow owner = GetParent();
			if (owner)
			{
				owner.SendMessage(NM_CANCEL_SEARCH, 0, 0);
			}
		}
	}

	if (m_searchesButtonRect.PtInRect(point))
	{
		this->ShowSearchHistoryMenu();
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CSymbolEdit::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	if (m_closeButtonRect.PtInRect(point))
	{
		m_mouseDownOnClose = true;
		SetCapture();
		InvalidateRect(m_closeButtonRect);
	}
	else
	{
		m_mouseDownOnClose = false;
	}

	if (m_searchesButtonRect.PtInRect(point))
	{
		m_mouseDownOnSearches = true;
		SetCapture();
		InvalidateRect(m_searchesButtonRect);
	}
	else
	{
		m_mouseDownOnSearches = false;
	}

	// Upstream repainted the whole control after CWnd's default handling.
	Invalidate();

	bHandled = FALSE;
	return 0;
}

LRESULT CSymbolEdit::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	if (m_closeButtonRect.PtInRect(point))
	{
		if (m_mouseHoveringOverClose == false)
		{
			m_mouseHoveringOverClose = true;
			InvalidateRect(m_closeButtonRect);
		}
	}
	else if(m_mouseHoveringOverClose)
	{
		m_mouseHoveringOverClose = false;
		InvalidateRect(m_closeButtonRect);
	}

	if (m_searchesButtonRect.PtInRect(point))
	{
		if (m_mouseHoveringOverSearches == false)
		{
			m_mouseHoveringOverSearches = true;
			InvalidateRect(m_searchesButtonRect);
		}
	}
	else if (m_mouseHoveringOverSearches)
	{
		m_mouseHoveringOverSearches = false;
		InvalidateRect(m_searchesButtonRect);
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CSymbolEdit::OnSelectSearchString(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	int index = wID - RANGE_START;

	if (wID == CLEAR_LIST)
	{
		m_searches.clear();
	}
	else if (index >= 0 &&
		index < static_cast<int>(m_searches.size()))
	{
		CString cs = m_searches[index];
		this->SetWindowTextW(cs);

		this->SetFocus();
		this->SetSel(-1);

		this->Invalidate();

		m_searches.erase(m_searches.begin() + index);
		m_searches.push_back(cs);
	}
	return 0;
}

bool CSymbolEdit::ApplyLastSearch()
{
	bool ret = false;
	if (!m_searches.empty())
	{
		CString cs = m_searches.back();
		this->SetWindowTextW(cs);

		this->SetFocus();
		this->SetSel(-1);

		this->Invalidate();

		ret = true;
	}

	return ret;
}

void CSymbolEdit::OnDpiChanged()
{
	SetDpiInfo(m_windowDpi);
}

void CSymbolEdit::SetDpiInfo(CDPI *dpi)
{ 
	m_windowDpi = dpi; 

	m_closeButton.Reset();
	m_closeButton.LoadStdImageDPI(m_windowDpi->GetDPI(), search_close_16, search_close_20, search_close_24, search_close_28, search_close_32, _T("PNG"));

	m_searchesButton.Reset();
	m_searchesButton.LoadStdImageDPI(m_windowDpi->GetDPI(), down_16, down_20, down_24, down_28, down_32, _T("PNG"));

	RecalcLayout();

	Invalidate();
}

LRESULT CSymbolEdit::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return FALSE;
}


LRESULT CSymbolEdit::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CString text;
	GetWindowText(text);

	NCCALCSIZE_PARAMS* lpncsp = (NCCALCSIZE_PARAMS*)lParam;

	if (m_windowDpi != NULL)
	{
		lpncsp->rgrc[0].left += m_windowDpi->Scale(1);
		lpncsp->rgrc[0].top += m_windowDpi->Scale(1);
		lpncsp->rgrc[0].right -= m_windowDpi->Scale(1);
		lpncsp->rgrc[0].bottom -= m_windowDpi->Scale(1);


		CRect rectWnd, rectClient;

		CFontHandle pFont = GetFont();
		CRect rectText;


		CDCHandle pDC = GetDC();

		HFONT oldFont = pDC.SelectFont(pFont);
		pDC.DrawText(_T("Ky"), -1, rectText, DT_CALCRECT | DT_LEFT);
		UINT uiVClientHeight = rectText.Height();

		pDC.SelectFont(oldFont);
		ReleaseDC(pDC);



		GetWindowRect(rectWnd);

		rectWnd.DeflateRect(m_windowDpi->Scale(1), m_windowDpi->Scale(1));

		m_centerTextDiff = (rectWnd.Height() - uiVClientHeight) / 2;

		if (m_centerTextDiff < 0 || m_centerTextDiff > uiVClientHeight)
		{
			m_centerTextDiff = 0;
		}

		lpncsp->rgrc[0].top += m_centerTextDiff;
		lpncsp->rgrc[0].bottom -= m_centerTextDiff;
	}

	return 0;
}


LRESULT CSymbolEdit::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CString text;
	GetWindowText(text);

	CWindowDC dc(m_hWnd);
	
	CRect r;
	this->GetWindowRect(r);
	this->ScreenToClient(r);

	CRect t(0, 0, r.Width(), m_centerTextDiff+ m_windowDpi->Scale(1));

	CRect b(0, r.Height() - m_centerTextDiff- m_windowDpi->Scale(1), r.Width(), r.Height());

	COLORREF c = CGetSetOptions::m_Theme.MainWindowBG();

	if (m_hWnd == ::GetFocus() || text.GetLength() > 0)
	{		
		dc.FillSolidRect(t, CGetSetOptions::m_Theme.SearchTextBoxFocusBG());
		dc.FillSolidRect(b, CGetSetOptions::m_Theme.SearchTextBoxFocusBG());

		c = CGetSetOptions::m_Theme.SearchTextBoxFocusBorder();
	}
	else
	{
		dc.FillSolidRect(t, CGetSetOptions::m_Theme.MainWindowBG());
		dc.FillSolidRect(b, CGetSetOptions::m_Theme.MainWindowBG());
	}	

	{
		CWindowDC dc(m_hWnd);

		CRect rcFrame;
		this->GetWindowRect(rcFrame);
		this->ScreenToClient(rcFrame);

		CRect rcBorder(0, 0, rcFrame.Width(), rcFrame.Height());

		int border = m_windowDpi->Scale(1);
		CBrush borderBrush;
		borderBrush.CreateSolidBrush(c);

		for (int x = 0; x < border; x++)
		{
			dc.FrameRect(rcBorder, borderBrush);
			rcBorder.DeflateRect(1, 1, 1, 1);
		}
	}

	return 0;
}

LRESULT CSymbolEdit::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT_PTR nIDEvent = wParam;
	switch (nIDEvent)
	{
	case 1:
		KillTimer(1);
		::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
		break;
	}

	bHandled = FALSE;
	return 0;
}
