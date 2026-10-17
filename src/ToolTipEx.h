#pragma once

#include "..\resource.h"
#include "RichEditCtrlEx.h"
#include "WndEx.h"
#include "DittoWindow.h"
#include "GdipButton.h"
#include "ImageViewer.h"
#include "GroupStatic.h"
#include "Accels.h"
#include "SnapWindow.h"
#include "SimpleBrowser.h"

class CToolTipEx : public CWindowImpl<CToolTipEx, CWindow, CNullTraits>
{
public:
	DECLARE_WND_CLASS_EX(_T("DittoToolTipEx"), CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, 0)

	CToolTipEx();

// Operations
public:
	BOOL OnMsg(MSG* pMsg);
	BOOL Create(WTL::CWindow* pParentWnd);
	BOOL Show(CPoint point);
	BOOL Hide();
	void SetToolTipText(const CString &csText);
	void SetRTFText(const CStringA &rtf);
	void SetHtmlText(const CString &html);
	void SetGdiplusBitmap(Gdiplus::Bitmap *gdiplusBitmap);
	void SetNotifyWnd(HWND hWndNotify)		{ m_hWndNotify = hWndNotify;	}
	CRect GetBoundsRect();

	void SetClipId(int clipId) { m_clipId = clipId; }
	int GetClipId() { return m_clipId; }

	void SetClipRow(int clipRow) { m_clipRow = clipRow; }
	int GetClipRow() { return m_clipRow; }

	void SetSearchText(CString text) { m_searchText = text; }

	void SetClipData(CString data) { m_clipData = data; m_originalClipData = data; }
	void SetFolderPath(CString path) { m_folderPath = path; }

	bool GetShowPersistant() { return m_showPersistant; }
	void ToggleShowPersistant();
	bool ToggleWordWrap();
	void SetTooltipActions(CAccels *pToolTipActions) { m_pToolTipActions = pToolTipActions; }

	void GetWindowRectEx(LPRECT lpRect);

	void UpdateMenuShortCut(CMenuHandle subMenu, int id, DWORD action);

	void DoSearch();
	void MoveControls();

	BOOL SetLogFont(LPLOGFONT lpLogFont, BOOL bRedraw);

// Overrides
protected:
	BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CToolTipEx();
	
protected:
	DWORD m_dwTextStyle;
	CRect m_rectMargin;
	CString m_csText;
	CFont m_Font;
	int m_fontHeight;
	CStringA m_csRTF;
	CString m_html;
	CRichEditCtrlEx m_RichEdit;
	SimpleBrowser m_browser;
	HWND m_hWndNotify;
	CGdipButton m_optionsButton;
	int m_clipId;
	CString m_searchText;
	CDittoWindow m_DittoWindow;
	CImageViewer m_imageViewer;
	CGroupStatic m_clipDataStatic;
	CGroupStatic m_folderPathStatic;
	CString m_clipData;
	CString m_originalClipData;
	CFont m_clipDataFont;
	bool m_saveWindowLockout;
	int m_clipRow;
	bool m_showPersistant;
	CAccels *m_pToolTipActions;
	bool m_bMaxSetTimer;
	int m_lDelayMaxSeconds;
	SnapWindow m_snap;
	CString m_folderPath;
	bool m_showingText;
	bool m_showingRTF;
	bool m_showingHTML;
	bool m_showingImage;

protected:
	CString GetFieldFromString(CString ref, int nIndex, TCHAR ch);
	BOOL IsCursorInToolTip();
	void HighlightSearchText();
	void ApplyWordWrap();
	void SaveWindowSize();
	CString GetRichEditText(const CHARRANGE& chrg);
	void ShowOptionsMenu();
	CString m_mouseDownOnLink;

	// message handlers
	LRESULT OnSize(UINT nType, CSize size);
	LRESULT OnNcHitTest(CPoint point);
	void OnActivate(UINT nState, BOOL bMinimized, HWND hWndOther);
	void OnNcMouseMove(UINT nHitTest, CPoint point);
	void OnNcLButtonUp(UINT nHitTest, CPoint point);
	void OnNcLButtonDown(UINT nHitTest, CPoint point);
	LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam);
	LRESULT OnNcPaint(HRGN hRgn);
	void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	LRESULT OnEraseBkgnd(HDC hDC);
	void OnTimer(UINT_PTR nIDEvent);
	void OnRButtonDown(UINT nFlags, CPoint point);
	void OnSetFocus(HWND hOldWnd);
	void OnPaint(HDC hDC);
	void OnMoving(UINT fwSide, LPRECT pRect);
	void OnEnterSizeMove();
	void OnHScroll(int nSBCode, int nPos, HWND hWndScrollBar);
	void OnCommand(int code, int id, HWND hWndCtl);
	LRESULT OnNotify(int idCtrl, LPNMHDR pnmh);
	LRESULT OnDpiChanged(UINT dpiX, UINT dpiY, PRECT prcNewWindow);
	LRESULT OnRefreshFooter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// the object is heap allocated by its callers; ATL calls this once the
	// window is fully destroyed, which is the only safe point to free it
	virtual void OnFinalMessage(HWND hWnd);

	// menu commands
	void OnOptions();
	void OnRememberwindowposition();
	void OnSizewindowtocontent();
	void OnScaleimagestofitwindow();
	void OnFirstHidedescriptionwindowonm();
	void OnFirstWraptext();
	void OnFirstAlwaysontop();
	void OnFirstViewtext();
	void OnFirstViewrtf();
	void OnFirstViewhtml();
	void OnFirstViewImage();

	// notifications
	LRESULT OnEnLink(int idCtrl, LPNMHDR pNMHDR);
	LRESULT OnBrowserBeforeNavigate2(int idCtrl, LPNMHDR pNMHDR);
	LRESULT OnEnMsgfilter(int idCtrl, LPNMHDR pNMHDR);

	BEGIN_MSG_MAP_EX(CToolTipEx)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_SIZE(OnSize)
		MSG_WM_NCHITTEST(OnNcHitTest)
		MSG_WM_ACTIVATE(OnActivate)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_NCLBUTTONDBLCLK(OnNcLButtonDblClk)
		MSG_WM_NCPAINT(OnNcPaint)
		MSG_WM_NCCALCSIZE(OnNcCalcSize)
		MSG_WM_NCLBUTTONDOWN(OnNcLButtonDown)
		MSG_WM_NCMOUSEMOVE(OnNcMouseMove)
		MSG_WM_NCLBUTTONUP(OnNcLButtonUp)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		MSG_WM_SETFOCUS(OnSetFocus)
		MSG_WM_MOVING(OnMoving)
		MSG_WM_ENTERSIZEMOVE(OnEnterSizeMove)
		MSG_WM_HSCROLL(OnHScroll)
		MSG_WM_COMMAND(OnCommand)
		MSG_WM_NOTIFY(OnNotify)
		MSG_WM_DPICHANGED(OnDpiChanged)
		MESSAGE_HANDLER(WM_REFRESH_FOOTER, OnRefreshFooter)
		REFLECT_NOTIFICATIONS_MSG_FILTERED(WM_DRAWITEM)
		REFLECT_NOTIFICATIONS_MSG_FILTERED(WM_CTLCOLORSTATIC)
	END_MSG_MAP()
};
