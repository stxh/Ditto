#include "stdafx.h"
#include "PropGrid.h"
#include "CP_Main.h"

CPropGrid::CPropGrid()
	: m_edit(this, 1)
	, m_combo(this, 2)
{
	m_nSelected = -1;
	m_nTop = 0;
	m_nRowHeight = 20;
	m_hFont = NULL;
	m_eEditKind = KindText;
	m_bEditing = FALSE;
}

int CPropGrid::AddCategory(LPCTSTR szName, BOOL bExpanded)
{
	Row row;
	row.name = szName;
	row.bCategory = TRUE;
	row.bExpanded = bExpanded;
	row.level = 0;
	m_rows.push_back(row);
	BuildVisible();
	return (int)m_rows.size() - 1;
}

static void FillChildRow(CPropGrid::Row& row, LPCTSTR szName, LPCTSTR szValue, LPCTSTR szDesc, int nId, CPropGrid::Kind kind)
{
	row.name = szName;
	row.value = szValue;
	row.original = szValue;
	row.desc = szDesc;
	row.dataId = nId;
	row.kind = kind;
	row.level = 1;
	row.bCategory = FALSE;
}

int CPropGrid::AddText(int, LPCTSTR szName, LPCTSTR szValue, LPCTSTR szDesc, int nId)
{
	Row row;
	FillChildRow(row, szName, szValue, szDesc, nId, KindText);
	m_rows.push_back(row);
	BuildVisible();
	return (int)m_rows.size() - 1;
}

int CPropGrid::AddInt(int, LPCTSTR szName, long nValue, LPCTSTR szDesc, int nId)
{
	Row row;
	CString szValue;
	szValue.Format(_T("%d"), nValue);
	FillChildRow(row, szName, szValue, szDesc, nId, KindInt);
	m_rows.push_back(row);
	BuildVisible();
	return (int)m_rows.size() - 1;
}

int CPropGrid::AddChoice(int, LPCTSTR szName, LPCTSTR szValue, const CString* pOptions, int nOptionCount, LPCTSTR szDesc, int nId)
{
	Row row;
	FillChildRow(row, szName, szValue, szDesc, nId, KindChoice);
	for (int i = 0; i < nOptionCount; i++)
		row.options.push_back(pOptions[i]);
	// The value is picked from the list only, as MFC's AllowEdit(FALSE) did.
	row.bAllowEdit = FALSE;
	m_rows.push_back(row);
	BuildVisible();
	return (int)m_rows.size() - 1;
}

int CPropGrid::AddFile(int, LPCTSTR szName, LPCTSTR szValue, LPCTSTR szFilter, LPCTSTR szDesc, int nId)
{
	Row row;
	FillChildRow(row, szName, szValue, szDesc, nId, KindFile);
	row.filter = szFilter;
	m_rows.push_back(row);
	BuildVisible();
	return (int)m_rows.size() - 1;
}

CPropGrid::Row* CPropGrid::FindById(int nId)
{
	for (int i = 0; i < (int)m_rows.size(); i++)
	{
		if (!m_rows[i].bCategory && m_rows[i].dataId == nId)
			return &m_rows[i];
	}
	return NULL;
}

CString CPropGrid::GetOriginal(int nId)
{
	Row* p = FindById(nId);
	return p != NULL ? p->original : CString(_T(""));
}

CString CPropGrid::GetValue(int nId)
{
	Row* p = FindById(nId);
	return p != NULL ? p->value : CString(_T(""));
}

long CPropGrid::GetIntValue(int nId)
{
	return (long)_ttol(GetValue(nId));
}

BOOL CPropGrid::GetBoolValue(int nId)
{
	return GetValue(nId).CompareNoCase(_T("True")) == 0 ? TRUE : FALSE;
}

void CPropGrid::SetValue(int nId, LPCTSTR szValue)
{
	Row* p = FindById(nId);
	if (p != NULL)
	{
		p->value = szValue;
		Invalidate();
	}
}

void CPropGrid::SetGridFont(HFONT hFont)
{
	m_hFont = hFont;

	LOGFONT lf = {};
	if (hFont != NULL)
		::GetObject(hFont, sizeof(lf), &lf);

	lf.lfWeight = FW_BOLD;
	m_catFont.CreateFontIndirect(&lf);

	if (hFont != NULL && m_hWnd != NULL)
	{
		CClientDC dc(m_hWnd);
		HFONT hOld = dc.SelectFont(hFont);
		TEXTMETRIC tm = {};
		dc.GetTextMetrics(&tm);
		dc.SelectFont(hOld);
		m_nRowHeight = tm.tmHeight + 6;
		if (m_nRowHeight < 18)
			m_nRowHeight = 18;
	}

	BuildVisible();
	Invalidate();
}

int CPropGrid::OnCreate(LPCREATESTRUCT)
{
	m_tip.Create(m_hWnd, NULL, NULL, TTS_ALWAYSTIP);

	TTTOOLINFOW ti = {};
	ti.cbSize = sizeof(ti);
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	ti.hwnd = m_hWnd;
	ti.uId = (UINT_PTR)m_hWnd;
	ti.lpszText = (LPWSTR)LPSTR_TEXTCALLBACKW;
	m_tip.AddTool(&ti);

	return 0;
}

int CPropGrid::NameWidth()
{
	CRect rc;
	GetClientRect(&rc);

	int nWidth = rc.Width() * 55 / 100;
	if (nWidth < 100)
		nWidth = 100;

	return nWidth;
}

void CPropGrid::BuildVisible()
{
	m_visible.clear();

	BOOL bShow = TRUE;
	for (int i = 0; i < (int)m_rows.size(); i++)
	{
		if (m_rows[i].bCategory)
		{
			bShow = m_rows[i].bExpanded;
			m_visible.push_back(i);
		}
		else if (bShow)
		{
			m_visible.push_back(i);
		}
	}

	if (m_nTop > (int)m_visible.size() - 1)
		m_nTop = m_visible.empty() ? 0 : (int)m_visible.size() - 1;
	if (m_nTop < 0)
		m_nTop = 0;

	UpdateScrollbar();
}

void CPropGrid::UpdateScrollbar()
{
	CRect rc;
	GetClientRect(&rc);

	SCROLLINFO si = {};
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL | SIF_POS;
	si.nMin = 0;
	si.nMax = (int)m_visible.size() - 1;
	si.nPage = max(1, rc.Height() / RowHeight());
	si.nPos = m_nTop;
	SetScrollInfo(SB_VERT, &si, TRUE);
}

void CPropGrid::EnsureVisible(int nVisibleIndex)
{
	CRect rc;
	GetClientRect(&rc);

	int nPage = max(1, rc.Height() / RowHeight());

	if (nVisibleIndex < m_nTop)
		m_nTop = nVisibleIndex;
	else if (nVisibleIndex >= m_nTop + nPage)
		m_nTop = nVisibleIndex - nPage + 1;

	if (m_nTop < 0)
		m_nTop = 0;

	UpdateScrollbar();
	Invalidate();
}

void CPropGrid::SetTop(int nTop)
{
	if (nTop < 0)
		nTop = 0;
	if (nTop > (int)m_visible.size() - 1)
		nTop = max(0, (int)m_visible.size() - 1);

	if (nTop == m_nTop)
		return;

	EndEdit(TRUE);

	m_nTop = nTop;
	UpdateScrollbar();
	Invalidate();
}

int CPropGrid::HitTest(CPoint pt, BOOL& bValueCell)
{
	bValueCell = (pt.x >= NameWidth());

	if (RowHeight() <= 0)
		return -1;

	int nVisible = m_nTop + pt.y / RowHeight();
	if (nVisible < 0 || nVisible >= (int)m_visible.size())
		return -1;

	return m_visible[nVisible];
}

CRect CPropGrid::ValueRect(int nVisibleIndex)
{
	CRect rc;
	GetClientRect(&rc);

	int nTop = (nVisibleIndex - m_nTop) * RowHeight();
	return CRect(NameWidth(), nTop, rc.Width(), nTop + RowHeight());
}

void CPropGrid::SetSelection(int nRow, BOOL bEditValue)
{
	m_nSelected = nRow;

	int nVisible = -1;
	for (int i = 0; i < (int)m_visible.size(); i++)
	{
		if (m_visible[i] == nRow)
		{
			nVisible = i;
			break;
		}
	}

	if (nVisible >= 0)
		EnsureVisible(nVisible);
	else
		Invalidate();

	if (bEditValue && nVisible >= 0)
		BeginEdit();
}

// Search support for the Advanced page filter box (upstream's CMFCPropertyGridCtrl search).
int CPropGrid::SelectNextMatch(LPCTSTR szTextLower, int nStartAfter)
{
	if (m_rows.empty() || szTextLower == NULL || *szTextLower == _T('\0'))
		return -1;

	const int nCount = (int)m_rows.size();
	for (int n = 0; n < nCount; n++)
	{
		// Wrap around so repeated Next-Match clicks cycle the whole list.
		int i = nStartAfter < 0 ? n : (nStartAfter + 1 + n) % nCount;

		if (m_rows[i].bCategory)
			continue;

		CString name = m_rows[i].name;
		name.MakeLower();
		if (name.Find(szTextLower) < 0)
			continue;

		// Expand the containing category, the way MFC's pSubItem->Show() did.
		for (int c = i - 1; c >= 0; c--)
		{
			if (m_rows[c].bCategory)
			{
				m_rows[c].bExpanded = TRUE;
				break;
			}
		}

		EndEdit(TRUE);
		BuildVisible();
		SetSelection(i, FALSE);
		return i;
	}

	return -1;
}

void CPropGrid::SelectFirst()
{
	if (m_rows.empty())
		return;

	EndEdit(TRUE);
	SetSelection(0, FALSE);
}

LRESULT CPropGrid::OnPaint(UINT, WPARAM, LPARAM, BOOL&)
{
	CPaintDC dc(m_hWnd);

	CRect rc;
	GetClientRect(&rc);
	if (rc.Width() <= 0 || rc.Height() <= 0)
		return 0;

	CDC dcMem;
	dcMem.CreateCompatibleDC(dc);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(dc, rc.Width(), rc.Height());
	HBITMAP hOldBmp = dcMem.SelectBitmap(bmp);

	dcMem.SetBkMode(TRANSPARENT);
	dcMem.FillSolidRect(rc, GetSysColor(COLOR_WINDOW));

	HFONT hOldFont = dcMem.SelectFont(m_hFont != NULL ? m_hFont : (HFONT)GetStockObject(DEFAULT_GUI_FONT));

	const int nNameWidth = NameWidth();
	const int nRowHeight = RowHeight();

	for (int i = m_nTop; i < (int)m_visible.size(); i++)
	{
		const int nY = (i - m_nTop) * nRowHeight;
		if (nY >= rc.Height())
			break;

		Row& row = m_rows[m_visible[i]];
		CRect rcRow(0, nY, rc.Width(), nY + nRowHeight);

		if (row.bCategory)
		{
			dcMem.FillSolidRect(rcRow, RGB(240, 240, 240));
			dcMem.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));

			CRect rcGlyph(2, nY, 16, nY + nRowHeight);
			::DrawText(dcMem, row.bExpanded ? _T("-") : _T("+"), -1, rcGlyph, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			HFONT hBold = dcMem.SelectFont(m_catFont);
			CRect rcText(18, nY, nNameWidth, nY + nRowHeight);
			::DrawText(dcMem, row.name, -1, rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
			dcMem.SelectFont(hBold);
		}
		else
		{
			CRect rcName(0, nY, nNameWidth, nY + nRowHeight);
			CRect rcValue(nNameWidth, nY, rc.Width(), nY + nRowHeight);
			BOOL bSelected = (m_nSelected == m_visible[i]);

			dcMem.FillSolidRect(rcName, GetSysColor(COLOR_WINDOW));
			dcMem.FillSolidRect(rcValue, bSelected ? RGB(0, 120, 215) : GetSysColor(COLOR_WINDOW));

			CRect rcText = rcName;
			rcText.left += 20;
			dcMem.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
			::DrawText(dcMem, row.name, -1, rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

			dcMem.SetTextColor(bSelected ? RGB(255, 255, 255) : GetSysColor(COLOR_WINDOWTEXT));
			CRect rcVal = rcValue;
			rcVal.left += 3;
			if (row.kind == KindFile)
				rcVal.right -= 22;
			::DrawText(dcMem, row.value, -1, rcVal, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

			if (row.kind == KindFile)
			{
				CRect rcBtn(rcValue.right - 20, nY + 2, rcValue.right - 2, nY + nRowHeight - 2);
				dcMem.FillSolidRect(rcBtn, GetSysColor(COLOR_BTNFACE));
				dcMem.Draw3dRect(rcBtn, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNSHADOW));
				dcMem.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
				::DrawText(dcMem, _T("..."), -1, rcBtn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}

			if (bSelected)
			{
				CRect rcFocus = rcValue;
				rcFocus.DeflateRect(1, 1);
				dcMem.DrawFocusRect(rcFocus);
			}
		}

		dcMem.FillSolidRect(0, nY + nRowHeight - 1, rc.Width(), 1, RGB(225, 225, 225));
		dcMem.FillSolidRect(nNameWidth, nY, 1, nRowHeight, RGB(225, 225, 225));
	}

	dc.BitBlt(0, 0, rc.Width(), rc.Height(), dcMem, 0, 0, SRCCOPY);

	dcMem.SelectFont(hOldFont);
	dcMem.SelectBitmap(hOldBmp);
	return 0;
}

LRESULT CPropGrid::OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL&)
{
	return 1;
}

LRESULT CPropGrid::OnSize(UINT, WPARAM, LPARAM, BOOL&)
{
	UpdateScrollbar();
	Invalidate();
	return 0;
}

LRESULT CPropGrid::OnSetFocus(UINT, WPARAM, LPARAM, BOOL&)
{
	Invalidate();
	return 0;
}

LRESULT CPropGrid::OnKillFocus(UINT, WPARAM, LPARAM, BOOL&)
{
	Invalidate();
	return 0;
}

LRESULT CPropGrid::OnGetDlgCode(UINT, WPARAM, LPARAM, BOOL&)
{
	return 0x0100L | 0x0200L; // DLGC_WANTARROWS | DLGC_WANTCHARS
}

LRESULT CPropGrid::OnLButtonDown(UINT, WPARAM, LPARAM lParam, BOOL&)
{
	SetFocus();

	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	if (RowHeight() <= 0)
		return 0;

	BOOL bValueCell = FALSE;
	int nRow = HitTest(pt, bValueCell);
	if (nRow < 0)
	{
		EndEdit(TRUE);
		return 0;
	}

	Row& row = m_rows[nRow];
	const int nVisible = m_nTop + pt.y / RowHeight();

	EndEdit(TRUE);

	if (row.bCategory)
	{
		row.bExpanded = row.bExpanded ? FALSE : TRUE;
		BuildVisible();
		Invalidate();
		return 0;
	}

	if (row.kind == KindFile && bValueCell && pt.x >= ValueRect(nVisible).right - 20)
	{
		SetSelection(nRow, FALSE);
		BrowseFile();
		return 0;
	}

	SetSelection(nRow, bValueCell);
	return 0;
}

LRESULT CPropGrid::OnMouseWheel(UINT, WPARAM wParam, LPARAM, BOOL&)
{
	int nDelta = (int)(short)HIWORD(wParam);
	int nLines = nDelta / WHEEL_DELTA;
	if (nLines == 0)
		nLines = nDelta > 0 ? 1 : -1;

	SetTop(m_nTop - nLines * 3);
	return 0;
}

LRESULT CPropGrid::OnVScroll(UINT, WPARAM wParam, LPARAM, BOOL&)
{
	CRect rc;
	GetClientRect(&rc);
	int nPage = max(1, rc.Height() / RowHeight());

	switch (LOWORD(wParam))
	{
	case SB_LINEUP:			SetTop(m_nTop - 1); break;
	case SB_LINEDOWN:		SetTop(m_nTop + 1); break;
	case SB_PAGEUP:			SetTop(m_nTop - nPage); break;
	case SB_PAGEDOWN:		SetTop(m_nTop + nPage); break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:	SetTop(HIWORD(wParam)); break;
	}
	return 0;
}

LRESULT CPropGrid::OnKeyDown(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
	if (m_visible.empty())
		return 0;

	int nVisible = -1;
	for (int i = 0; i < (int)m_visible.size(); i++)
	{
		if (m_visible[i] == m_nSelected)
		{
			nVisible = i;
			break;
		}
	}

	switch (wParam)
	{
	case VK_DOWN:
	case VK_UP:
		{
			if (nVisible < 0)
				nVisible = m_nTop;
			nVisible += (wParam == VK_DOWN) ? 1 : -1;
			if (nVisible < 0)
				nVisible = 0;
			if (nVisible >= (int)m_visible.size())
				nVisible = (int)m_visible.size() - 1;
			EndEdit(TRUE);
			SetSelection(m_visible[nVisible], FALSE);
		}
		return 0;
	case VK_F2:
		EndEdit(TRUE);
		if (nVisible >= 0)
			BeginEdit();
		return 0;
	}

	bHandled = FALSE;
	return 0;
}

void CPropGrid::BeginEdit()
{
	if (m_bEditing)
		return;

	if (m_nSelected < 0 || m_nSelected >= (int)m_rows.size())
		return;

	Row& row = m_rows[m_nSelected];
	if (row.bCategory)
		return;

	int nVisible = -1;
	for (int i = 0; i < (int)m_visible.size(); i++)
	{
		if (m_visible[i] == m_nSelected)
		{
			nVisible = i;
			break;
		}
	}
	if (nVisible < 0)
		return;

	CRect rcValue = ValueRect(nVisible);
	if (row.kind == KindFile)
		rcValue.right -= 22;

	if (row.kind == KindChoice)
	{
		m_combo.Create(m_hWnd, rcValue, NULL,
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS | CBS_AUTOHSCROLL | WS_VSCROLL,
			0, IDC_PROPGRID_COMBO);

		if (m_hFont != NULL)
			m_combo.SendMessage(WM_SETFONT, (WPARAM)m_hFont, TRUE);

		for (size_t i = 0; i < row.options.size(); i++)
			m_combo.AddString(row.options[i]);

		int nSel = m_combo.FindStringExact(-1, row.value);
		m_combo.SetCurSel(nSel >= 0 ? nSel : 0);
		m_eEditKind = row.kind;
		m_bEditing = TRUE;
		m_combo.SetFocus();
		m_combo.SendMessage(CB_SHOWDROPDOWN, TRUE);
		Invalidate();
		return;
	}

	if (!row.bAllowEdit)
		return;

	DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL;

	m_edit.Create(m_hWnd, rcValue, NULL, style, 0, IDC_PROPGRID_EDIT);
	if (m_hFont != NULL)
		m_edit.SendMessage(WM_SETFONT, (WPARAM)m_hFont, TRUE);
	m_edit.SetWindowTextW(row.value);
	m_edit.SetLimitText(4000);
	m_edit.SetSel(0, -1);
	m_eEditKind = row.kind;
	m_bEditing = TRUE;
	m_edit.SetFocus();
	Invalidate();
}

void CPropGrid::EndEdit(BOOL bCommit)
{
	if (!m_bEditing)
		return;

	m_bEditing = FALSE;

	if (m_combo.m_hWnd != NULL)
	{
		if (bCommit && m_nSelected >= 0 && m_nSelected < (int)m_rows.size())
		{
			int nSel = m_combo.GetCurSel();
			if (nSel >= 0)
			{
				CString szPicked;
				m_combo.GetLBText(nSel, szPicked);
				if (!szPicked.IsEmpty())
					m_rows[m_nSelected].value = szPicked;
			}
		}
		m_combo.DestroyWindow();
	}

	if (m_edit.m_hWnd != NULL)
	{
		if (bCommit && m_nSelected >= 0 && m_nSelected < (int)m_rows.size())
		{
			CString szText;
			int nLen = m_edit.GetWindowTextLength();
			if (nLen > 0)
			{
				std::vector<TCHAR> buffer(nLen + 2, _T('\0'));
				m_edit.GetWindowText(&buffer[0], (int)buffer.size());
				szText = &buffer[0];
			}
			m_rows[m_nSelected].value = szText;
		}
		m_edit.DestroyWindow();
	}

	Invalidate();
}

LRESULT CPropGrid::OnEditKillFocus(WORD, WORD, HWND, BOOL&)
{
	EndEdit(TRUE);
	return 0;
}

LRESULT CPropGrid::OnComboCloseUp(WORD, WORD, HWND, BOOL&)
{
	EndEdit(TRUE);
	SetFocus();
	return 0;
}

LRESULT CPropGrid::OnEditKeyDown(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
	if (wParam == VK_RETURN)
	{
		EndEdit(TRUE);
		SetFocus();
		return 0;
	}
	if (wParam == VK_ESCAPE)
	{
		EndEdit(FALSE);
		SetFocus();
		return 0;
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CPropGrid::OnEditChar(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
	WCHAR ch = (WCHAR)wParam;
	// MFC's grid validated integer properties through the numeric edit it put up;
	// the minus sign has to stay reachable, so filter the characters instead.
	if (m_eEditKind == KindInt &&
		!(ch >= _T('0') && ch <= _T('9')) &&
		ch != _T('-') && ch != VK_BACK)
		return 0;

	bHandled = FALSE;
	return 0;
}

LRESULT CPropGrid::OnComboKeyDown(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
	// While the list is dropped the combo handles Enter itself and closes it,
	// which is what picks the highlighted item.
	if (wParam == VK_RETURN && !m_combo.GetDroppedState())
	{
		EndEdit(TRUE);
		SetFocus();
		return 0;
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CPropGrid::OnEditGetDlgCode(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
	// The dialog manager turns an unclaimed Enter into a press of the default
	// pushbutton, which would close the page instead of committing the row.
	if (wParam == VK_RETURN || wParam == VK_ESCAPE)
		return DLGC_WANTMESSAGE | DLGC_WANTCHARS;

	bHandled = FALSE;
	return 0;
}

void CPropGrid::BrowseFile()
{
	if (m_nSelected < 0 || m_nSelected >= (int)m_rows.size())
		return;

	Row& row = m_rows[m_nSelected];
	if (row.kind != KindFile)
		return;

	TCHAR szFile[MAX_PATH] = {};
	lstrcpyn(szFile, row.value, MAX_PATH);

	CFileDialog dlg(TRUE, NULL, szFile, OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		row.filter.IsEmpty() ? NULL : (LPCTSTR)row.filter, m_hWnd);

	if (dlg.DoModal() == IDOK)
	{
		row.value = dlg.m_ofn.lpstrFile;
		Invalidate();
	}
}

LRESULT CPropGrid::OnNeedText(WORD, LPNMHDR pNMHDR, BOOL&)
{
	LPNMTTDISPINFOW pTip = (LPNMTTDISPINFOW)pNMHDR;

	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	BOOL bValueCell = FALSE;
	int nRow = HitTest(pt, bValueCell);
	m_szTipText = (nRow >= 0) ? m_rows[nRow].desc : CString(_T(""));

	pTip->lpszText = (LPWSTR)(LPCWSTR)m_szTipText;
	pTip->hinst = NULL;
	return 0;
}
