// QListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "CP_Main.h"
#include "QListCtrl.h"
#include "ProcessPaste.h"
#include "BitmapHelper.h"
#include "MainTableFunctions.h"
#include "DittoCopyBuffer.h"
#include <atlbase.h>
#include "DrawHTML.h"
#include "Shared\TextConvert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ROW_BOTTOM_BORDER		4
#define ROW_LEFT_BORDER			3
#define COLOR_SHADOW			RGB(245, 245, 245)
#define DUMMY_COL_WIDTH			2

#define TIMER_SHOW_PROPERTIES	1
#define TIMER_HIDE_SCROL	2
#define TIMER_SHOW_SCROLL	3

#define VALID_TOOLTIP (m_pToolTip && ::IsWindow(m_pToolTip->m_hWnd))


/////////////////////////////////////////////////////////////////////////////
// CQListCtrl

CQListCtrl::CQListCtrl()
{
	m_pchTip = NULL;
	m_pwchTip = NULL;
	m_linesPerRow = 1;
	m_windowDpi = NULL;
	m_SmallFont = NULL;
	m_pToolTip = NULL;
	m_pFormatter = NULL;
	m_allSelected = false;
	m_rowHeight = 50;
	m_mouseOverScrollAreaStart = 0;
	m_showIfClipWasPasted = TRUE;
	m_bShowTextForFirstTenHotKeys = true;
	m_pToolTipActions = NULL;
}

CQListCtrl::~CQListCtrl()
{
	if (m_pchTip != NULL)
		delete m_pchTip;

	if (m_pwchTip != NULL)
		delete m_pwchTip;

	if (m_SmallFont)
		::DeleteObject(m_SmallFont);

	m_Font.DeleteObject();

	m_boldFont.DeleteObject();

	if (m_pFormatter)
	{
		delete m_pFormatter;
		m_pFormatter = NULL;
	}

	DeleteObject(m_SmallFont);
}

// returns the position 1-10 if the index is in the FirstTen block else -1
int CQListCtrl::GetFirstTenNum(int index)
{
	// set firstTenNum to the first ten number (1-10) corresponding to the given index
	int firstTenNum = -1; // -1 means that nItem is not in the FirstTen block.
	int count = GetItemCount();

	if (0 <= index && index <= 9)
	{
		firstTenNum = index + CGetSetOptions::m_firstTenHotKeysStart;
		firstTenNum = firstTenNum % 10;
	}

	return firstTenNum;
}



/////////////////////////////////////////////////////////////////////////////
// CQListCtrl message handlers

LRESULT CQListCtrl::OnKeydown(LPNMHDR pNMHDR)
{
	LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;

	return 0;
}

void CQListCtrl::GetSelectionIndexes(ARRAY &arr)
{
	arr.RemoveAll();

	for (int nItem = GetNextItem(-1, LVNI_SELECTED); nItem != -1; nItem = GetNextItem(nItem, LVNI_SELECTED))
		arr.Add(nItem);
}

bool CQListCtrl::PutSelectedItemOnDittoCopyBuffer(long lBuffer)
{
	bool bRet = false;
	ARRAY arr;
	GetSelectionItemData(arr);
	INT_PTR nCount = arr.GetSize();
	if (nCount > 0 && arr[0])
	{
		CDittoCopyBuffer::PutClipOnDittoCopyBuffer(arr[0], lBuffer);
		bRet = true;
	}

	return bRet;
}

void CQListCtrl::GetSelectionItemData(ARRAY &arr)
{
	DWORD dwData;
	arr.RemoveAll();
	for (int nItem = GetNextItem(-1, LVNI_SELECTED); nItem != -1; nItem = GetNextItem(nItem, LVNI_SELECTED))
	{
		dwData = GetItemData(nItem);
		arr.Add(dwData);
	}
}

void CQListCtrl::RemoveAllSelection()
{
	SetItemState(-1, 0, LVIS_SELECTED);
}

BOOL CQListCtrl::SetSelection(int nRow, BOOL bSelect)
{
	if (bSelect)
		return SetItemState(nRow, LVIS_SELECTED, LVIS_SELECTED);
	else
		return SetItemState(nRow, ~LVIS_SELECTED, LVIS_SELECTED);
}

BOOL CQListCtrl::SetText(int nRow, int nCol, CString cs)
{
	return SetItemText(nRow, nCol, cs);
}

BOOL CQListCtrl::SetCaret(int nRow, BOOL bFocus)
{
	if (bFocus)
		return SetItemState(nRow, LVIS_FOCUSED, LVIS_FOCUSED);
	else
		return SetItemState(nRow, ~LVIS_FOCUSED, LVIS_FOCUSED);
}

long CQListCtrl::GetCaret()
{
	return GetNextItem(-1, LVNI_FOCUSED);
}

// moves the caret to the given index, selects it, and ensures it is visible.
BOOL CQListCtrl::SetListPos(int index)
{
	if (index < 0 || index >= GetItemCount())
		return FALSE;

	RemoveAllSelection();
	SetCaret(index);
	SetSelection(index);
	ListView_SetSelectionMark(m_hWnd, index);
	EnsureVisible(index, FALSE);

	return TRUE;
}

BOOL CQListCtrl::SetFormattedText(int nRow, int nCol, LPCTSTR lpszFormat, ...)
{
	CString csText;
	va_list vlist;

	ASSERT(lpszFormat != NULL);
	va_start(vlist, lpszFormat);
	csText.FormatV(lpszFormat, vlist);
	va_end(vlist);

	return SetText(nRow, nCol, csText);
}

void CQListCtrl::SetNumberOfLinesPerRow(int nLines, bool force)
{
	if (m_linesPerRow != nLines ||
		force)
	{
		m_linesPerRow = nLines;

		CRect rc;
		GetWindowRect(&rc);
		WINDOWPOS wp;
		wp.hwnd = m_hWnd;
		wp.cx = rc.Width();
		wp.cy = rc.Height();
		wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
		SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
	}
}

void CQListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	TEXTMETRIC tm;
	HDC hDC = ::GetDC(NULL);
	HFONT hFontOld = (HFONT)::SelectObject(hDC, GetFont());
	GetTextMetrics(hDC, &tm);
	if (m_windowDpi != NULL)
	{
		lpMeasureItemStruct->itemHeight = ((tm.tmHeight + tm.tmExternalLeading) * m_linesPerRow) + m_windowDpi->Scale(ROW_BOTTOM_BORDER);
		m_rowHeight = lpMeasureItemStruct->itemHeight;
	}
	::SelectObject(hDC, hFontOld);
	::ReleaseDC(NULL, hDC);
}

LRESULT CQListCtrl::OnMeasureItem(UINT, WPARAM, LPARAM lParam, BOOL&)
{
	MeasureItem((LPMEASUREITEMSTRUCT)lParam);
	return TRUE;
}

LRESULT CQListCtrl::OnCustomdrawList(LPNMHDR pNMHDR)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		return CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		LVITEM   rItem;
		int      nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);
		CDCHandle hDC(pLVCD->nmcd.hdc);
		COLORREF crBkgnd;
		BOOL     bListHasFocus;
		CRect    rcItem;

		bListHasFocus = (m_hWnd == ::GetFocus());

		ZeroMemory(&rItem, sizeof(LVITEM));
		rItem.mask = LVIF_STATE;
		rItem.iItem = nItem;
		rItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
		GetItem(&rItem);

		GetItemRect(nItem, rcItem, LVIR_SELECTBOUNDS);

		COLORREF OldColor = -1;
		int nOldBKMode = -1;

		CString csText;
		LPTSTR lpszText = csText.GetBufferSetLength(CGetSetOptions::m_bDescTextSize);
		GetItemText(nItem, 0, lpszText, CGetSetOptions::m_bDescTextSize);
		csText.ReleaseBuffer();

		CString strSymbols;
		int nSymEnd = csText.Find('|');
		if (nSymEnd >= 0)
		{
			strSymbols = csText.Left(nSymEnd);
			csText = csText.Mid(nSymEnd + 1);
		}

		if (rItem.state & LVIS_SELECTED)
		{
			if (bListHasFocus)
			{
				crBkgnd = CGetSetOptions::m_Theme.ListBoxSelectedBG();
				OldColor = hDC.SetTextColor(CGetSetOptions::m_Theme.ListBoxSelectedText());
			}
			else
			{
				crBkgnd = CGetSetOptions::m_Theme.ListBoxSelectedNoFocusBG();
				OldColor = hDC.SetTextColor(CGetSetOptions::m_Theme.ListBoxSelectedNoFocusText());
			}
		}
		else
		{
			if ((nItem % 2) == 0)
			{
				crBkgnd = CGetSetOptions::m_Theme.ListBoxOddRowsBG();
				OldColor = hDC.SetTextColor(CGetSetOptions::m_Theme.ListBoxOddRowsText());
			}
			else
			{
				crBkgnd = CGetSetOptions::m_Theme.ListBoxEvenRowsBG();
				OldColor = hDC.SetTextColor(CGetSetOptions::m_Theme.ListBoxEvenRowsText());
			}
		}

		hDC.FillSolidRect(rcItem, crBkgnd);
		nOldBKMode = hDC.SetBkMode(TRANSPARENT);

		CRect rcText = rcItem;
		rcText.left += m_windowDpi->Scale(ROW_LEFT_BORDER);
		rcText.top += m_windowDpi->Scale(1);
		rcText.bottom -= m_windowDpi->Scale(1);

		if (m_showIfClipWasPasted &&
			strSymbols.GetLength() > 0 &&
			strSymbols.Find(_T("<pasted>")) >= 0)
		{
			CRect pastedRect(rcItem);
			pastedRect.left++;
			pastedRect.right = pastedRect.left + m_windowDpi->Scale(2);

			hDC.FillSolidRect(pastedRect, CGetSetOptions::m_Theme.ClipPastedColor());
		}

		int firstTenNum = GetFirstTenNum(nItem);

		if (m_bShowTextForFirstTenHotKeys && firstTenNum >= 0)
		{
			rcText.left += m_windowDpi->Scale(12);
		}
		else
		{
			rcText.left += m_windowDpi->Scale(3);
		}

		bool drawInGroupIcon = true;
		if (theApp.m_GroupID > 0)
		{
			int nFlag = strSymbols.Find(_T("<ingroup>"));
			if (nFlag >= 0)
				drawInGroupIcon = false;
		}

		DrawCopiedColorCode(csText, rcText, hDC);

		DrawBitMap(nItem, rcText, hDC, csText);

		if (strSymbols.GetLength() > 0)
		{
			if (strSymbols.Find(_T("<group>")) >= 0)
			{
				m_groupFolder.Draw(hDC, *m_windowDpi, m_hWnd, rcText.left, rcText.top, false, false);
				rcText.left += m_groupFolder.ImageWidth() + m_windowDpi->Scale(2);
			}
			if (strSymbols.Find(_T("<noautodelete>")) >= 0)
			{
				m_dontDeleteImage.Draw(hDC, *m_windowDpi, m_hWnd, rcText.left, rcText.top, false, false);
				rcText.left += m_dontDeleteImage.ImageWidth() + m_windowDpi->Scale(2);
			}
			if (strSymbols.Find(_T("<shortcut>")) >= 0)
			{
				m_shortCutImage.Draw(hDC, *m_windowDpi, m_hWnd, rcText.left, rcText.top, false, false);
				rcText.left += m_shortCutImage.ImageWidth() + m_windowDpi->Scale(2);
			}
			if (drawInGroupIcon &&
				strSymbols.Find(_T("<ingroup>")) >= 0)
			{
				m_inFolderImage.Draw(hDC, *m_windowDpi, m_hWnd, rcText.left, rcText.top, false, false);
				rcText.left += m_inFolderImage.ImageWidth() + m_windowDpi->Scale(2);
			}
			if (strSymbols.Find(_T("<qpastetext>")) >= 0)
			{
			}
			if (strSymbols.Find(_T("<sticky>")) >= 0)
			{
				m_stickyImage.Draw(hDC, *m_windowDpi, m_hWnd, rcText.left, rcText.top, false, false);
				rcText.left += m_stickyImage.ImageWidth() + m_windowDpi->Scale(2);
			}
		}

		if (DrawRtfText(nItem, rcText, hDC) == FALSE)
		{
			auto highlightColor = CGetSetOptions::m_Theme.SearchTextHighlight();
			if (m_searchText.GetLength() > 0 &&
				FindNoCaseAndInsert(csText, m_searchText, StrF(_T("\x01\x04 color='#%02x%02x%02x'\x02"), GetRValue(highlightColor), GetGValue(highlightColor), GetBValue(highlightColor)), _T("\x01\x03\x04\x02"), m_linesPerRow) > 0)
			{
				DrawHTML(hDC, csText, csText.GetLength(), rcText, DT_VCENTER | DT_EXPANDTABS | DT_NOPREFIX);
			}
			else
			{
				hDC.DrawText(csText, csText.GetLength(), rcText, DT_VCENTER | DT_EXPANDTABS | DT_NOPREFIX);
			}
		}

		if (m_bShowTextForFirstTenHotKeys && firstTenNum >= 0)
		{
			CString cs;
			if (firstTenNum == 10)
				cs = "0";
			else
				cs.Format(_T("%d"), firstTenNum);

			CRect crHotKey = rcItem;

			int extraFromClipWasPasted = 0;
			if (m_showIfClipWasPasted)
				extraFromClipWasPasted = 3;

			crHotKey.right = crHotKey.left + m_windowDpi->Scale(11);
			crHotKey.left += m_windowDpi->Scale(1 + extraFromClipWasPasted);
			crHotKey.top += m_windowDpi->Scale(1 + extraFromClipWasPasted);

			HFONT hOldFont = hDC.SelectFont(m_SmallFont);
			COLORREF localOldTextColor = hDC.SetTextColor(CGetSetOptions::m_Theme.ListSmallQuickPasteIndexColor());

			CPen pen;
			pen.CreatePen(PS_SOLID, 0, CGetSetOptions::m_Theme.ListSmallQuickPasteIndexColor());
			HPEN hOldPen = hDC.SelectPen(pen);

			hDC.DrawText(cs, cs.GetLength(), crHotKey, DT_BOTTOM);

			hDC.MoveTo(CPoint(rcItem.left + m_windowDpi->Scale(8 + extraFromClipWasPasted), rcItem.top));
			hDC.LineTo(CPoint(rcItem.left + m_windowDpi->Scale(8 + extraFromClipWasPasted), rcItem.bottom));

			hDC.SelectFont(hOldFont);
			hDC.SetTextColor(localOldTextColor);
			hDC.SelectPen(hOldPen);
		}

		if (OldColor > -1)
			hDC.SetTextColor(OldColor);

		if (nOldBKMode > -1)
			hDC.SetBkMode(nOldBKMode);

		return CDRF_SKIPDEFAULT;
	}

	return 0;
}

void CQListCtrl::DrawCopiedColorCode(CString& csText, CRect& rcText, HDC hDC)
{
	if (CGetSetOptions::m_bDrawCopiedColorCode == FALSE)
		return;

	CString trimmedText = CString(csText).Trim(L'»').Trim().Trim('#');

	//draw hex color copied like #FF0000, FF0000, FF0000 other text with a space before other text
	if (trimmedText.GetLength() == 6 ||
		trimmedText.Find(' ') == 6)
	{
		int r, g, b;
		int scanRet = swscanf(trimmedText, _T("%02x%02x%02x"), &r, &g, &b);
		if (scanRet == 3)
		{
			CRect pastedRect(rcText);
			pastedRect.right = pastedRect.left + m_windowDpi->Scale(rcText.Height());

			CDCHandle(hDC).FillSolidRect(pastedRect, RGB(r, g, b));

			rcText.left += m_windowDpi->Scale(rcText.Height());
			rcText.left += m_windowDpi->Scale(ROW_LEFT_BORDER);
		}
	}

	// draw rgb color copied like 255,0,0 and rgb(255,0,0)
	int firstCommaPos = trimmedText.Find(',');
	if (firstCommaPos >= 0)
	{
		int secondCommaPos = trimmedText.Find(',', firstCommaPos+1);
		if (secondCommaPos)
		{
			int noThirdCommaPos = trimmedText.Find(',', secondCommaPos+1);
			
			if (noThirdCommaPos < 0)
			{
				int r, g, b;
				CString noRGB = trimmedText.MakeLower().Trim(_T("rgb(")).Trim(')');
				int scanRet = swscanf(noRGB, _T("%d,%d,%d"), &r, &g, &b);
				if (scanRet == 3)
				{
					CRect pastedRect(rcText);
					pastedRect.right = pastedRect.left + m_windowDpi->Scale(rcText.Height());

					CDCHandle(hDC).FillSolidRect(pastedRect, RGB(r, g, b));

					rcText.left += m_windowDpi->Scale(rcText.Height());
					rcText.left += m_windowDpi->Scale(ROW_LEFT_BORDER);
				}
			}
		}
	}
}

BOOL CQListCtrl::DrawRtfText(int nItem, CRect &crRect, HDC hDC)
{
	if (CGetSetOptions::m_bDrawRTF == FALSE)
		return FALSE;

	BOOL bRet = FALSE;

	CClipFormat* pThumbnail = GetItem_CF_RTF_ClipFormat(nItem);
	if (pThumbnail == NULL)
		return FALSE;

	// if there's no data, then we're done.
	if (pThumbnail->m_hgData == NULL)
		return FALSE;

	if (m_pFormatter == NULL)
	{
		m_pFormatter = new CFormattedTextDraw;
		m_pFormatter->Create();
	}

	if (m_rtfFormater.m_hWnd == NULL)
	{
		m_rtfFormater.Create(m_hWnd, -1,
			WS_CHILD | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_NOHIDESEL | ES_AUTOHSCROLL,
			0, CRect(0, 0, 0, 0));
	}

	if (m_pFormatter)
	{
		//somehow ms word places crazy rtf text onto the clipboard and our draw routine doesn't handle that
		//pass the rtf text into a richtext control and get it out and the contorl will clean  the rtf so our routine can draw it
		m_rtfFormater.SetRTF(pThumbnail->GetAsCStringA());
		CString betterRTF = m_rtfFormater.GetRTF();

		CComBSTR bStr(betterRTF);
		m_pFormatter->put_RTFText(bStr);

		m_pFormatter->Draw(hDC, crRect);

		bRet = TRUE;
	}

	return bRet;
}

// DrawBitMap loads a DIB from the DB, draws a crRect thumbnail of the image
//  to pDC and caches that thumbnail as a DIB in m_ThumbNails[ ItemID ].
// ALL items are cached in m_ThumbNails (those without images are cached with NULL m_hgData)
BOOL CQListCtrl::DrawBitMap(int nItem, CRect &crRect, HDC hDC, const CString &csDescription)
{
	if (CGetSetOptions::m_bDrawThumbnail == FALSE)
		return FALSE;

	CClipFormatQListCtrl *format = GetItem_CF_DIB_ClipFormat(nItem);
	if (format != NULL)
	{
		HGLOBAL smallImage = format->GetDibFittingToHeight(hDC, crRect.Height());
		if (smallImage != NULL)
		{
			//Will return the width of the bitmap in nWidth
			int nWidth = 0;
			if (CBitmapHelper::DrawDIB(hDC, smallImage, crRect.left, crRect.top, nWidth))
			{
				// adjust the rect so other information can be drawn next to the thumbnail
				crRect.left += nWidth + 3;
			}
		}
	}
	else if (csDescription.Find(_T("CF_DIB")) == 0)
	{
		crRect.left += crRect.Height();
	}

	return TRUE;
}

void CQListCtrl::RefreshVisibleRows()
{
	int nTopIndex = GetTopIndex();
	int nLastIndex = nTopIndex + GetCountPerPage();
	RedrawItems(nTopIndex, nLastIndex);
}

void CQListCtrl::RefreshRow(int row)
{
	RedrawItems(row, row);
}

LRESULT CQListCtrl::OnSysKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CQListCtrl::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CDCHandle dc((HDC)wParam);

	CRect rect;
	GetClientRect(&rect);
	CBrush brush;
	brush.CreateSolidBrush(CGetSetOptions::m_Theme.MainWindowBG());
	HBRUSH hOldBrush = dc.SelectBrush(brush);
	BOOL bRes = dc.PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);
	dc.SelectBrush(hOldBrush);

	return bRes;
}

LRESULT CQListCtrl::OnToolTipText(LPNMHDR pNMHDR)
{
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText;

	LVHITTESTINFO hitTest = { };
	POINT ptCursor = { GET_X_LPARAM(::GetMessagePos()), GET_Y_LPARAM(::GetMessagePos()) };
	::ScreenToClient(m_hWnd, &ptCursor);
	hitTest.pt = ptCursor;

	int nID = (int)::SendMessage(m_hWnd, LVM_HITTEST, 0, (LPARAM)&hitTest);

	if (nID < 0)
		return 0;

	::SendMessage(pNMHDR->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 500);

	if (CGetSetOptions::m_tooltipTimeout > 0)
	{
		::SendMessage(pNMHDR->hwndFrom, TTM_SETDELAYTIME, TTDT_AUTOPOP, MAKELPARAM(CGetSetOptions::m_tooltipTimeout, 0));
	}

	GetToolTipText(nID, strTipText);

	//Replace the tabs with spaces, the tooltip didn't like the \t s
	strTipText.Replace(_T("\t"), _T("  "));

	int nLength = strTipText.GetLength() + 2;

	if (pNMHDR->code == TTN_NEEDTEXTW)
	{
		int nLength = strTipText.GetLength() + 2;

		if (m_pwchTip != NULL)
			delete m_pwchTip;

		m_pwchTip = new WCHAR[nLength];
		::lstrcpynW(m_pwchTip, strTipText, nLength);
		m_pwchTip[nLength - 1] = 0;
		pTTTW->lpszText = m_pwchTip;
	}

	return 0;
}

INT_PTR CQListCtrl::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
{
	CRect rect;
	GetClientRect(&rect);
	if (rect.PtInRect(point))
	{
		if (GetItemCount())
		{
			int nTopIndex = GetTopIndex();
			int nBottomIndex = nTopIndex + GetCountPerPage();
			if (nBottomIndex > GetItemCount()) nBottomIndex = GetItemCount();
			for (int nIndex = nTopIndex; nIndex <= nBottomIndex; nIndex++)
			{
				GetItemRect(nIndex, rect, LVIR_BOUNDS);
				if (rect.PtInRect(point))
				{
					pTI->hwnd = m_hWnd;
					pTI->uId = (UINT)(nIndex + 1);
					pTI->lpszText = LPSTR_TEXTCALLBACK;
					pTI->rect = rect;
					pTI->uFlags = TTF_TRANSPARENT;
					return pTI->uId;
				}
			}
		}
	}

	return -1;
}

LRESULT CQListCtrl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (::IsWindow(m_toolTip.m_hWnd) == FALSE)
	{
		m_toolTip.Create(m_hWnd, NULL, NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TTS_ALWAYSTIP | TTS_NOPREFIX);
	}

	if (CGetSetOptions::m_tooltipTimeout != 0)
	{
		m_toolTip.SetMaxTipWidth(500);
		if (CGetSetOptions::m_tooltipTimeout > 0)
			m_toolTip.SetDelayTime(TTDT_AUTOPOP, CGetSetOptions::m_tooltipTimeout * 1000);
		m_toolTip.Activate(TRUE);
		// one subclassed tool for the whole control, the item under the cursor is resolved on TTN_NEEDTEXT
		m_toolTip.AddTool(m_hWnd, LPSTR_TEXTCALLBACK);
	}
	else
	{
		m_toolTip.Activate(FALSE);
	}

	// This window is a superclass of SysListView32, so WM_CREATE has to reach the
	// original list-view window proc as well, otherwise the control stays uninitialised.
	bHandled = FALSE;

	return 0;
}

BOOL CQListCtrl::PreTranslateMessage(MSG* pMsg)
{
	CAccel a;
	if (m_Accels.OnMsg(pMsg, a))
	{
		switch (a.Cmd)
		{
		case COPY_BUFFER_HOT_KEY_1_ID:
			PutSelectedItemOnDittoCopyBuffer(0);
			break;
		case COPY_BUFFER_HOT_KEY_2_ID:
			PutSelectedItemOnDittoCopyBuffer(1);
			break;
		case COPY_BUFFER_HOT_KEY_3_ID:
			PutSelectedItemOnDittoCopyBuffer(2);
			break;
		default:
			if (a.RefId == CHotKey::PASTE_OPEN_CLIP)
			{
				GetParent().SendMessage(NM_SELECT_DB_ID, a.Cmd, 0);
			}
			else if (a.RefId == CHotKey::MOVE_TO_GROUP)
			{
				GetParent().SendMessage(NM_MOVE_TO_GROUP, a.Cmd, 0);
			}
		}

		return TRUE;
	}

	if (VALID_TOOLTIP)
	{
		if (m_pToolTip->OnMsg(pMsg))
			return TRUE;
	}

	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		if (HandleKeyDown(pMsg->wParam, pMsg->lParam))
			return TRUE;
		break; // end case WM_KEYDOWN
	case WM_MOUSEWHEEL:
		break;

	case WM_VSCROLL:
		ASSERT(FALSE);
		break;
	} // end switch(pMsg->message)

	return FALSE;
}

BOOL CQListCtrl::HandleKeyDown(WPARAM wParam, LPARAM lParam)
{
	if (VALID_TOOLTIP)
	{
		MSG Msg;
		Msg.lParam = lParam;
		Msg.wParam = wParam;
		Msg.message = WM_KEYDOWN;
		if (m_pToolTip->OnMsg(&Msg))
			return TRUE;
	}

	WPARAM vk = wParam;

	switch (vk)
	{
	case 'A': // Ctrl-A = Select All
		if (CONTROL_PRESSED)
		{
			int nCount = GetItemCount();
			for (int i = 0; i < nCount; i++)
			{
				SetSelection(i);
			}
			return TRUE;
		}
		break;

	case VK_HOME:

		if (GetKeyState(VK_SHIFT) & 0x8000)
		{
			int pos = GetNextItem(-1, LVNI_SELECTED) + 1;
			if (pos >= 0 && pos < GetItemCount())
			{
				for (int i = 0; i < pos; i++)
				{
					SetSelection(i, (i == 0));
				}
			}

		}
		else
		{
			SetListPos(0);
		}
		break;
	} // end switch(vk)

	return FALSE;
}

bool CQListCtrl::PostEventLoadedCheckDescription(int updatedRow)
{
	bool loadedClip = false;

	if (VALID_TOOLTIP)
	{
		int toolTipClipId = m_pToolTip->GetClipId();
		int toolTipClipRow = m_pToolTip->GetClipRow();

		if (toolTipClipRow >= 0)
		{
			log(StrF(_T("PostEventLoadedCheckDescription refreshRow: %d tt_row: %d tt_id: %d"), updatedRow, toolTipClipRow, toolTipClipId));
		}

		//We tried to show the clip but we didn't have the id yet, it was loaded in a thread, now it's being updated
		//see if we need to show this rows description
		if (toolTipClipId <= 0 &&
			toolTipClipRow == updatedRow &&
			::IsWindow(m_toolTipHwnd))
		{
			ShowFullDescription(false, true);
			loadedClip = true;
		}
	}

	return loadedClip;
}

bool CQListCtrl::ShowFullDescription(bool bFromAuto, bool fromNextPrev)
{
	if (ListView_GetSelectedCount(m_hWnd) == 0)
	{
		return false;
	}

	int clipRow = this->GetCaret();
	int clipId = this->GetItemData(clipRow);

	log(StrF(_T("Show full description row: %d id: %d"), clipRow, clipId));

	if (VALID_TOOLTIP &&
		clipId > 0 &&
		m_pToolTip->GetClipId() == clipId &&
		::IsWindow(m_toolTipHwnd))
	{
		return false;
	}

	int nItem = GetCaret();
	CRect rc, crWindow;
	GetWindowRect(&crWindow);
	GetItemRect(nItem, rc, LVIR_BOUNDS);
	ClientToScreen(rc);

	CPoint pt;

	if (CGetSetOptions::GetRememberDescPos())
	{
		CGetSetOptions::GetDescWndPoint(pt);
	}
	else if (bFromAuto == false)
	{
		pt = CPoint(rc.left, rc.bottom);
	}
	else
	{
		pt = CPoint((crWindow.left + (crWindow.right - crWindow.left) / 2), rc.bottom);
	}

	CString csDescription;
	GetToolTipText(nItem, csDescription);

	if (m_pToolTip == NULL ||
		//fromNextPrev == false ||
		::IsWindow(m_toolTipHwnd) == FALSE)
	{
		if (m_pToolTip != NULL)
		{
			m_pToolTip->DestroyWindow();
		}

		m_pToolTip = new CToolTipEx;
		m_pToolTip->Create(this);
		m_toolTipHwnd = m_pToolTip->m_hWnd;
		m_pToolTip->SetNotifyWnd(GetParent().m_hWnd);
	}
	else if (VALID_TOOLTIP)
	{
		if (fromNextPrev)
		{
			CRect r;
			m_pToolTip->GetWindowRectEx(r);
			pt = r.TopLeft();
		}

		m_pToolTip->SetGdiplusBitmap(NULL);
		m_pToolTip->SetRTFText("");
		m_pToolTip->SetToolTipText(_T(""));
		m_pToolTip->SetFolderPath(_T(""));
	}

	if (VALID_TOOLTIP)
	{
		m_pToolTip->SetTooltipActions(m_pToolTipActions);
		m_pToolTip->SetClipId(clipId);
		m_pToolTip->SetClipRow(clipRow);
		m_pToolTip->SetSearchText(m_searchText);
		LOGFONT lf;
		m_Font.GetLogFont(&lf);
		lf.lfHeight = m_windowDpi->UnScale(lf.lfHeight);
		m_pToolTip->SetLogFont(&lf, FALSE);

		m_pToolTip->SetClipData(_T(""));
		m_pToolTip->SetToolTipText(_T(""));
		m_pToolTip->SetRTFText("");
		bool bSetPlainText = false;

		CClipFormat Clip;

		try
		{
			CppSQLite3Query q = theApp.m_db.execQueryEx(_T("SELECT lID, lDate, lastPasteDate, lDontAutoDelete, QuickPasteText, lShortCut, globalShortCut, stickyClipOrder, stickyClipGroupOrder, lParentID FROM Main WHERE lID = %d"), clipId);
			if (q.eof() == false)
			{
				CString clipData;
				COleDateTime time((time_t)q.getIntField(_T("lDate")));
				clipData += "Added: " + time.Format();

				COleDateTime modified((time_t)q.getIntField(_T("lastPasteDate")));
				clipData += _T(" | Last Used: ") + modified.Format();

				if (q.getIntField(_T("lDontAutoDelete")) > 0)
				{
					clipData += _T(" | Never Auto Delete");
				}

				CString csQuickPaste = q.getStringField(_T("QuickPasteText"));
				if (csQuickPaste.IsEmpty() == FALSE)
				{
					clipData += _T(" | Quick Paste = ");
					clipData += csQuickPaste;
				}

				int shortCut = q.getIntField(_T("lShortCut"));
				if (shortCut > 0)
				{
					clipData += _T(" | ");
					clipData += CHotKey::GetHotKeyDisplayStatic(shortCut);

					BOOL globalShortCut = q.getIntField(_T("globalShortCut"));
					if (globalShortCut)
					{
						clipData += _T(" - Global Shortcut Key");
					}
				}

				if (theApp.m_GroupID > 0)
				{
					int sticky = q.getIntField(_T("stickyClipGroupOrder"));
					if (sticky != INVALID_STICKY)
					{
						clipData += _T(" | ");
						clipData += _T(" - Sticky In Group");
					}
				}
				else
				{
					int sticky = q.getIntField(_T("stickyClipOrder"));
					if (sticky != INVALID_STICKY)
					{
						clipData += _T(" | ");
						clipData += _T(" - Sticky");
					}
				}

				int parentId = q.getIntField(_T("lParentID"));
				if (parentId > 0)
				{
					CString folder = FolderPath(parentId);

					m_pToolTip->SetFolderPath(folder);
				}

				m_pToolTip->SetClipData(clipData);
			}
		}
		CATCH_SQLITE_EXCEPTION

		Clip.m_cfType = CF_UNICODETEXT;
		if (GetClipData(nItem, Clip) && Clip.m_hgData)
		{
			m_pToolTip->SetToolTipText(Clip.GetAsCString());
			bSetPlainText = true;		

			Clip.Free();
			Clip.Clear();
		}

		if (bSetPlainText == false)
		{
			Clip.m_cfType = CF_TEXT;
			if (GetClipData(nItem, Clip) && Clip.m_hgData)
			{	
				CString cs(Clip.GetAsCStringA());

				m_pToolTip->SetToolTipText(cs);
				bSetPlainText = true;

				Clip.Free();
				Clip.Clear();
			}
		}

		if (bSetPlainText == false)
		{
			m_pToolTip->SetToolTipText(csDescription);
		}

		Clip.m_cfType = RegisterClipboardFormat(CF_RTF);

		if (GetClipData(nItem, Clip) && Clip.m_hgData)
		{
			m_pToolTip->SetRTFText(Clip.GetAsCStringA());
		
			Clip.Free();
			Clip.Clear();
		}

		m_pToolTip->SetHtmlText(_T(""));
		Clip.m_cfType = GetFormatID(_T("HTML Format"));
		if (GetClipData(nItem, Clip) && Clip.m_hgData)
		{
			CString html = CTextConvert::Utf8ToUnicode(Clip.GetAsCStringA());
			m_pToolTip->SetHtmlText(html);		

			Clip.Free();
			Clip.Clear();
		}

		Clip.m_cfType = CF_DIB;
		if (GetClipData(nItem, Clip) && Clip.m_hgData)
		{
			m_pToolTip->SetGdiplusBitmap(Clip.CreateGdiplusBitmap());
		}
		else
		{
			Clip.m_cfType = theApp.m_PNG_Format;
			if (GetClipData(nItem, Clip) && Clip.m_hgData)
			{
				m_pToolTip->SetGdiplusBitmap(Clip.CreateGdiplusBitmap());
			}
		}

		m_pToolTip->Show(pt);
	}

	return true;
}

void CQListCtrl::GetToolTipText(int nItem, CString &csText)
{
	CWindow pParent = GetParent();
	if (pParent.m_hWnd != NULL)
	{
		CQListToolTipText info;
		memset(&info, 0, sizeof(info));
		info.hdr.code = NM_GETTOOLTIPTEXT;
		info.hdr.hwndFrom = m_hWnd;
		info.hdr.idFrom = ::GetDlgCtrlID(m_hWnd);
		info.lItem = nItem;
		//plus 100 for extra info - shortcut and such
		int maxCharacters = CGetSetOptions::GetMaxToolTipCharacters();
		info.cchTextMax = min(maxCharacters, CGetSetOptions::m_bDescTextSize) + 200;
		info.pszText = csText.GetBufferSetLength(info.cchTextMax);

		pParent.SendMessage(WM_NOTIFY, (WPARAM)info.hdr.idFrom, (LPARAM)&info);

		csText.ReleaseBuffer();
	}
}

BOOL CQListCtrl::GetClipData(int nItem, CClipFormat &Clip)
{
	return theApp.GetClipData(GetItemData(nItem), Clip);
}

DWORD CQListCtrl::GetItemData(int nItem)
{
	if ((GetStyle() & LVS_OWNERDATA))
	{
		CWindow pParent = GetParent();
		if (pParent.m_hWnd != NULL)
		{
			LV_DISPINFO info;
			memset(&info, 0, sizeof(info));
			info.hdr.code = LVN_GETDISPINFO;
			info.hdr.hwndFrom = m_hWnd;
			info.hdr.idFrom = ::GetDlgCtrlID(m_hWnd);

			info.item.iItem = nItem;
			info.item.lParam = -1;
			info.item.mask = LVIF_PARAM;

			pParent.SendMessage(WM_NOTIFY, (WPARAM)info.hdr.idFrom, (LPARAM)&info);

			return (DWORD)info.item.lParam;
		}
	}

	return (DWORD)WTL::CListViewCtrl::GetItemData(nItem);
}

CClipFormatQListCtrl* CQListCtrl::GetItem_CF_DIB_ClipFormat(int nItem)
{
	CClipFormatQListCtrl *format = NULL;

	CWindow pParent = GetParent();
	if (pParent.m_hWnd != NULL)
	{
		LV_DISPINFO info;
		memset(&info, 0, sizeof(info));
		info.hdr.code = LVN_GETDISPINFO;
		info.hdr.hwndFrom = m_hWnd;
		info.hdr.idFrom = ::GetDlgCtrlID(m_hWnd);

		info.item.iItem = nItem;
		info.item.lParam = NULL;
		info.item.mask = LVIF_CF_DIB;

		pParent.SendMessage(WM_NOTIFY, (WPARAM)info.hdr.idFrom, (LPARAM)&info);

		if (info.item.lParam != NULL)
		{
			format = (CClipFormatQListCtrl *)info.item.lParam;
		}
	}

	return format;
}

CClipFormatQListCtrl* CQListCtrl::GetItem_CF_RTF_ClipFormat(int nItem)
{
	CClipFormatQListCtrl *format = NULL;

	CWindow pParent = GetParent();
	if (pParent.m_hWnd != NULL)
	{
		LV_DISPINFO info;
		memset(&info, 0, sizeof(info));
		info.hdr.code = LVN_GETDISPINFO;
		info.hdr.hwndFrom = m_hWnd;
		info.hdr.idFrom = ::GetDlgCtrlID(m_hWnd);

		info.item.iItem = nItem;
		info.item.lParam = NULL;
		info.item.mask = LVIF_CF_RICHTEXT;

		pParent.SendMessage(WM_NOTIFY, (WPARAM)info.hdr.idFrom, (LPARAM)&info);

		if (info.item.lParam != NULL)
		{
			format = (CClipFormatQListCtrl *)info.item.lParam;
		}
	}

	return format;
}

void CQListCtrl::DestroyAndCreateAccelerator(BOOL bCreate, CppSQLite3DB &db)
{
	m_Accels.RemoveAll();

	if (bCreate)
	{
		CMainTableFunctions::LoadAcceleratorKeys(m_Accels, db);

		LoadDittoCopyBufferHotkeys();
	}
}

void CQListCtrl::LoadDittoCopyBufferHotkeys()
{
	CCopyBufferItem Item;
	CAccel a;

	CGetSetOptions::GetCopyBufferItem(0, Item);
	if (Item.m_lCopyHotKey > 0)
	{
		a.Cmd = COPY_BUFFER_HOT_KEY_1_ID;
		a.Key = Item.m_lCopyHotKey;
		m_Accels.AddAccel(a);
	}

	CGetSetOptions::GetCopyBufferItem(1, Item);
	if (Item.m_lCopyHotKey > 0)
	{
		a.Cmd = COPY_BUFFER_HOT_KEY_2_ID;
		a.Key = Item.m_lCopyHotKey;
		m_Accels.AddAccel(a);
	}

	CGetSetOptions::GetCopyBufferItem(2, Item);
	if (Item.m_lCopyHotKey > 0)
	{
		a.Cmd = COPY_BUFFER_HOT_KEY_3_ID;
		a.Key = Item.m_lCopyHotKey;
		m_Accels.AddAccel(a);
	}
}

LRESULT CQListCtrl::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//if(FocusOnToolTip() == FALSE)
		//m_pToolTip->Hide();
	return 0;
}

HWND CQListCtrl::GetToolTipHWnd()
{
	if (VALID_TOOLTIP)
		return m_pToolTip->m_hWnd;

	return NULL;
}

BOOL CQListCtrl::SetItemCountEx(int iCount, DWORD dwFlags /* = 0 */)
{
	return (BOOL)::SendMessage(m_hWnd, LVM_SETITEMCOUNT, iCount, dwFlags);
}

LRESULT CQListCtrl::OnSelectionChange(LPNMHDR pNMHDR)
{
	NMLISTVIEW *pnmv = (NMLISTVIEW *)pNMHDR;

	if ((pnmv->uNewState == 3) ||
		(pnmv->uNewState == 1))
	{
		if (VALID_TOOLTIP &&
			::IsWindowVisible(m_pToolTip->m_hWnd))
		{
			this->ShowFullDescription(false, true);
		}
		if (CGetSetOptions::m_bAllwaysShowDescription)
		{
			KillTimer(TIMER_SHOW_PROPERTIES);
			SetTimer(TIMER_SHOW_PROPERTIES, 300, NULL);
		}
		if (ListView_GetSelectedCount(m_hWnd) > 0)
			theApp.SetStatus(NULL, FALSE);
	}

	if (ListView_GetSelectedCount(m_hWnd) == this->GetItemCount())
	{
		if (m_allSelected == false)
		{
			Log(StrF(_T("List box Select All")));

			GetParent().SendMessage(NM_ALL_SELECTED, 0, 0);
			m_allSelected = true;
		}
	}
	else if (m_allSelected == true)
	{
		Log(StrF(_T("List box REMOVED Select All")));
		m_allSelected = false;
	}

	return 0;
}

LRESULT CQListCtrl::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT_PTR nIDEvent = (UINT_PTR)wParam;
	//http://support.microsoft.com/kb/200054
	//OnTimer() Is Not Called Repeatedly for a List Control
	bool callBase = true;

	switch (nIDEvent)
	{
	case TIMER_SHOW_PROPERTIES:
	{
		if (theApp.m_bShowingQuickPaste)
			ShowFullDescription(true);
		KillTimer(TIMER_SHOW_PROPERTIES);

		callBase = false;
	}
	break;

	case TIMER_HIDE_SCROL:
	{
		CPoint cursorPos;
		GetCursorPos(&cursorPos);

		CRect crWindow;
		this->GetWindowRect(&crWindow);



		//check and see if they moved out of the scroll area
		//If they did tell our parent so
		if (MouseInScrollBarArea(crWindow, cursorPos) == false)
		{
			StopHideScrollBarTimer();
		}

		callBase = false;
	}
	break;

	case TIMER_SHOW_SCROLL:
	{
		CPoint cursorPos;
		GetCursorPos(&cursorPos);

		CRect crWindow;
		this->GetWindowRect(&crWindow);

		//Adjust for the v-scroll bar being off of the screen
		crWindow.right -= m_windowDpi->Scale(GetSystemMetrics(SM_CXVSCROLL));
		crWindow.bottom -= m_windowDpi->Scale(::GetSystemMetrics(SM_CXHSCROLL));

		//Check and see if we are still in the cursor area
		if (MouseInScrollBarArea(crWindow, cursorPos))
		{
			m_timerToHideScrollAreaSet = true;
			GetParent().SendMessage(NM_SHOW_HIDE_SCROLLBARS, 1, 0);

			//Start looking to hide the scroll bars
			SetTimer(TIMER_HIDE_SCROL, 1000, NULL);
		}

		KillTimer(TIMER_SHOW_SCROLL);

		callBase = false;
	}
	break;
	}

	if (callBase)
		bHandled = FALSE;

	return 0;
}

void CQListCtrl::SetLogFont(LOGFONT &font)
{
	m_Font.DeleteObject();
	m_boldFont.DeleteObject();

	m_Font.CreateFontIndirect(&font);
	font.lfWeight = 600;
	m_boldFont.CreateFontIndirect(&font);

	SetFont(m_Font);
}

LRESULT CQListCtrl::OnOdCacheHint(LPNMHDR pNMHDR)
{
	NMLVCACHEHINT* pcachehint = (NMLVCACHEHINT*)pNMHDR;

	GetParent().SendMessage(NM_FILL_REST_OF_LIST, pcachehint->iFrom, pcachehint->iTo);
	return 0;
}

BOOL CQListCtrl::OnItemDeleted(long lID)
{
	m_RTFData.erase(lID);
	return TRUE;
}

LRESULT CQListCtrl::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nFlags = (UINT)wParam;
	CPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	if (CGetSetOptions::m_showScrollBar == FALSE)
	{
		CPoint cursorPos;
		GetCursorPos(&cursorPos);

		CRect crWindow;
		this->GetWindowRect(&crWindow);
		ScreenToClient(&crWindow);

		crWindow.right -= m_windowDpi->Scale(::GetSystemMetrics(SM_CXVSCROLL));
		crWindow.bottom -= m_windowDpi->Scale(::GetSystemMetrics(SM_CXHSCROLL));

		if (MouseInScrollBarArea(crWindow, point))
		{
			if ((GetTickCount() - m_mouseOverScrollAreaStart) > 500)
			{
				SetTimer(TIMER_SHOW_SCROLL, 500, NULL);

				m_mouseOverScrollAreaStart = GetTickCount();
			}
		}
		else
		{
			if (m_timerToHideScrollAreaSet)
			{
				StopHideScrollBarTimer();
			}
			KillTimer(TIMER_SHOW_SCROLL);
		}
	}

	bHandled = FALSE;

	return 0;
}

bool CQListCtrl::MouseInScrollBarArea(CRect crWindow, CPoint point)
{
	CRect crRight(crWindow);
	CRect crBottom(crWindow);

	crRight.left = crRight.right - m_windowDpi->Scale(::GetSystemMetrics(SM_CXVSCROLL));
	crBottom.top = crBottom.bottom - m_windowDpi->Scale(::GetSystemMetrics(SM_CYHSCROLL));

	/*CString cs;
	cs.Format(_T("point.x: %d, Width: %d, Height: %d\n"), point.x, crWindow.Width(), crWindow.Height());
	OutputDebugString(cs);*/

	if (crRight.PtInRect(point) || crBottom.PtInRect(point))
	{
		return true;
	}

	return false;
}

void CQListCtrl::StopHideScrollBarTimer()
{
	GetParent().SendMessage(NM_SHOW_HIDE_SCROLLBARS, 0, 0);

	m_timerToHideScrollAreaSet = false;
	KillTimer(TIMER_HIDE_SCROL);
}

void CQListCtrl::SetSearchText(CString text)
{
	m_searchText = text;
}

void CQListCtrl::HidePopup(bool checkShowPersistant)
{
	if (VALID_TOOLTIP)
	{
		if (checkShowPersistant == false ||
			m_pToolTip->GetShowPersistant() == false)
		{
			m_pToolTip->Hide();
		}
	}
}

BOOL CQListCtrl::IsToolTipWindowVisible()
{
	if (VALID_TOOLTIP)
	{
		return ::IsWindowVisible(m_toolTipHwnd);
	}

	return FALSE;
}

void CQListCtrl::ToggleToolTipShowPersistant()
{
	if (VALID_TOOLTIP)
	{
		m_pToolTip->ToggleShowPersistant();
	}
}

bool CQListCtrl::ToggleToolTipWordWrap()
{
	bool didWordWrap = false;
	if (VALID_TOOLTIP)
	{
		didWordWrap = m_pToolTip->ToggleWordWrap();
	}

	return didWordWrap;
}


BOOL CQListCtrl::IsToolTipWindowFocus()
{
	if (VALID_TOOLTIP)
	{
		return ::GetFocus() == m_toolTipHwnd ||
			::GetParent(::GetFocus()) == m_toolTipHwnd;
	}

	return FALSE;
}

bool CQListCtrl::IsToolTipShowPersistant()
{
	if (VALID_TOOLTIP)
	{
		return m_pToolTip->GetShowPersistant();
	}

	return false;
}

void CQListCtrl::DoToolTipSearch()
{
	if (VALID_TOOLTIP)
	{
		return m_pToolTip->DoSearch();
	}
}

void CQListCtrl::HideToolTip()
{
	if (VALID_TOOLTIP)
	{
		m_pToolTip->Hide();
	}
}

void CQListCtrl::OnDpiChanged()
{
	SetDpiInfo(m_windowDpi);
}

void CQListCtrl::SetDpiInfo(CDPI *dpi)
{
	m_windowDpi = dpi;

	m_groupFolder.Reset();
	m_groupFolder.LoadStdImageDPI(m_windowDpi->GetDPI(), IDB_OPEN_FOLDER_16_16, IDB_OPEN_FOLDER_20_20, IDB_OPEN_FOLDER_24_24, IDB_OPEN_FOLDER_24_24, IDB_OPEN_FOLDER_32_32, _T("PNG"));

	m_dontDeleteImage.Reset();
	m_dontDeleteImage.LoadStdImageDPI(m_windowDpi->GetDPI(), IDB_YELLOW_STAR_16_16, IDB_YELLOW_STAR_20_20, IDB_YELLOW_STAR_24_24, IDB_YELLOW_STAR_24_24, IDB_YELLOW_STAR_32_32, _T("PNG"));

	m_inFolderImage.Reset();
	m_inFolderImage.LoadStdImageDPI(m_windowDpi->GetDPI(), IDB_IN_FOLDER_16_16, IDB_IN_FOLDER_20_20, IDB_IN_FOLDER_24_24, IDB_IN_FOLDER_24_24, IDB_IN_FOLDER_32_32, _T("PNG"));

	m_shortCutImage.Reset();
	m_shortCutImage.LoadStdImageDPI(m_windowDpi->GetDPI(), IDB_KEY_16_16, IDB_KEY_20_20, IDB_KEY_24_24, IDB_KEY_24_24, IDB_KEY_32_32, _T("PNG"));

	m_stickyImage.Reset();
	m_stickyImage.LoadStdImageDPI(m_windowDpi->GetDPI(), IDB_STICKY_16_16, IDB_STICKY_20_20, IDB_STICKY_24_24, IDB_STICKY_24_24, IDB_STICKY_32_32, _T("PNG"));

	DeleteObject(m_SmallFont);

	CreateSmallFont();
}

void CQListCtrl::CreateSmallFont()
{
	LOGFONT lf;

	lf.lfHeight = -MulDiv(CGetSetOptions::GetFirstTenHotKeysFontSize(), m_windowDpi->GetDPI(), 72);
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_LIGHT;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = ANSI_CHARSET;
	lf.lfOutPrecision = OUT_STRING_PRECIS;
	lf.lfClipPrecision = CLIP_STROKE_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE;
	lstrcpy(lf.lfFaceName, _T("Small Font"));

	m_SmallFont = ::CreateFontIndirect(&lf);
}

LRESULT CQListCtrl::OnMouseHWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	short zDelta = (short)HIWORD(wParam);
	if (zDelta < 0)
	{
		this->SendMessage(WM_HSCROLL, SB_LINERIGHT, NULL);
	}
	else
	{
		this->SendMessage(WM_HSCROLL, SB_LINELEFT, NULL);
	}

	return 0;
}
