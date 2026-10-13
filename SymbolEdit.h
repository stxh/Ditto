/************************************************************************/
/* Created by MARIUS BANCILA
/*            www.mariusbancila.ro
/* Copyright (C) 2008 MARIUS BANCILA. All rights reserved.
/*
/* Permission is given by the author to freely redistribute and
/* include this code in any program as long as this credit is
/* given where due.
/*
/* CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS,
/* WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
/* INCLUDING, WITHOUT LIMITATION, WARRANTIES THAT THE CODE
/* IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
/* OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND
/* PERFORMANCE OF THE CODE IS WITH YOU. SHOULD ANY
/* CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT THE INITIAL
/* DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY
/* NECESSARY SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF
/* WARRANTY CONSTITUTES AN ESSENTIAL PART OF THIS LICENSE. NO USE
/* OF ANY CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
/* THIS DISCLAIMER.
/*
/************************************************************************/



#pragma once

#include "GdiImageDrawer.h"
#include "Accels.h"
#include "DPI.h"

#define RANGE_START 3000
#define CLEAR_LIST 3010
#define LIST_MAX_COUNT 10
#define MAX_SAVED_SEARCH_LENGTH 50

class CSymbolEdit : public CWindowImpl<CSymbolEdit, CEdit>
{
public:
	DECLARE_WND_SUPERCLASS(_T("Ditto_SymbolEdit"), _T("Edit"))

	CSymbolEdit();
	~CSymbolEdit();

	BOOL PreTranslateMessage(MSG* pMsg);

	void AddToSearchHistory();

	bool ShowSearchHistoryMenu();

	void SetSymbolIcon(HICON hIcon, BOOL redraw = TRUE);
	void SetSymbolIcon(UINT id, BOOL redraw = TRUE);

	void SetPromptText(CString text, BOOL redraw = TRUE);
	void SetPromptText(LPCTSTR szText, BOOL redraw = TRUE);

	void SetPromptTextColor(COLORREF color, BOOL redraw = TRUE);

	void SetPromptFont(CFont& font, BOOL redraw = TRUE);
	void SetPromptFont(const LOGFONT* lpLogFont, BOOL redraw = TRUE);

	bool ApplyLastSearch();

	void SetLastSearchAccel(CAccel a) { m_lastSearchShortCut = a; }

	CString SavePastSearches();
	void LoadPastSearches(CString values);

	void SetDpiInfo(CDPI *dpi);

	void OnDpiChanged();

protected:
	CFont m_fontPrompt;
	HICON m_hSymbolIcon;
	bool m_bInternalIcon;
	CString m_strPromptText;
	COLORREF m_colorPromptText;

	CBrush m_brush;
	COLORREF m_lastBrushColor;

	CGdiImageDrawer m_closeButton;
	CRect m_closeButtonRect;
	bool m_mouseDownOnClose;
	bool m_mouseHoveringOverClose;

	CGdiImageDrawer m_searchesButton;
	CRect m_searchesButtonRect;
	bool m_mouseDownOnSearches;
	bool m_mouseHoveringOverSearches;

	CAccel m_lastSearchShortCut;

	CStringArray m_searches;

	CDPI *m_windowDpi;

	int m_centerTextDiff;
	CString m_lastTextOnPaint;

	void DestroyIcon();
	void RecalcLayout();
	virtual void PreSubclassWindow();

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnSetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HBRUSH CtlColor(CDCHandle dc, HWND hwndEdit);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSelectSearchString(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BEGIN_MSG_MAP_EX(CSymbolEdit)
		MSG_WM_CREATE(OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SETFONT, OnSetFont)
		MSG_WM_CTLCOLOREDIT(CtlColor)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize)
		MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		COMMAND_RANGE_HANDLER(RANGE_START, (RANGE_START + LIST_MAX_COUNT), OnSelectSearchString)
	END_MSG_MAP()
};


