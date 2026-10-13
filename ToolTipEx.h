#pragma once

#include "Resource.h"
#include "RichEditCtrlEx.h"
#include "WndEx.h"
#include "DittoWindow.h"
#include "GdipButton.h"
#include "ImageViewer.h"
#include "GroupStatic.h"
#include "Accels.h"
#include "SnapWindow.h"
#include "SimpleBrowser.h"

class CToolTipEx : public ATL::CWindow
{
public:
	CToolTipEx();

// Attributes
public:

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
	void SetNotifyWnd(WTL::CWindow *pNotify)		{ m_pNotifyWnd = pNotify;	}
	void HideWindowInXMilliSeconds(long lms);
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

	void UpdateMenuShortCut(CMenu *subMenu, int id, DWORD action);

	void DoSearch();
	void MoveControls();

	BOOL SetLogFont(LPLOGFONT lpLogFont, BOOL bRedraw /*=TRUE*/);


// Overrides
protected:
	void PostNcDestroy();
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
	WTL::CWindow *m_pNotifyWnd;
	CGdipButton m_optionsButton;
	int m_clipId;
	CString m_searchText;
	CScrollBar m_vScroll;
	CScrollBar m_hScroll;
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
	CString m_mouseDownOnLink;

// Message handlers (simplified - implement in cpp)
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOptions(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled);
	LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRememberwindowposition(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled);
	LRESULT OnSizewindowtocontent(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled);
	LRESULT OnScaleimagestofitwindow(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled);
	LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFirstHidedescriptionwindowonm(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled);
	LRESULT OnFirstWraptext(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled);
	LRESULT OnNcLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFirstAlwaysontop(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled);
	LRESULT OnEnMsgfilterRichedit21(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
	LRESULT OnDpiChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEnterSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRefreshFooter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFirstViewtext(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled);
	LRESULT OnFirstViewrtf(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled);
	LRESULT OnFirstViewhtml(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled);
	LRESULT OnFirstViewImage(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled);
	LRESULT OnEnLink(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
	LRESULT OnBrowserBeforeNavigate2(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);

	// Command handlers (implemented as message handlers)
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return OnDefault(uMsg, wParam, lParam, bHandled);
	}

	// Notification handlers
	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return OnDefault(uMsg, wParam, lParam, bHandled);
	}

	LRESULT OnDefault(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	BEGIN_MSG_MAP_EX(CToolTipEx)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_NCLBUTTONDBLCLK, OnNcLButtonDblClk)
		MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
		MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize)
		MESSAGE_HANDLER(WM_NCLBUTTONDOWN, OnNcLButtonDown)
		MESSAGE_HANDLER(WM_NCMOUSEMOVE, OnNcMouseMove)
		MESSAGE_HANDLER(WM_NCLBUTTONUP, OnNcLButtonUp)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
		MESSAGE_HANDLER(WM_MOVING, OnMoving)
		MESSAGE_HANDLER(WM_ENTERSIZEMOVE, OnEnterSizeMove)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		COMMAND_ID_HANDLER(ID_FIRST_REMEMBERWINDOWPOSITION, OnRememberwindowposition)
		COMMAND_ID_HANDLER(ID_FIRST_SIZEWINDOWTOCONTENT, OnSizewindowtocontent)
		COMMAND_ID_HANDLER(ID_FIRST_SCALEIMAGESTOFITWINDOW, OnScaleimagestofitwindow)
		COMMAND_ID_HANDLER(2, OnOptions)
		COMMAND_ID_HANDLER(ID_FIRST_HIDEDESCRIPTIONWINDOWONM, OnFirstHidedescriptionwindowonm)
		COMMAND_ID_HANDLER(ID_FIRST_WRAPTEXT, OnFirstWraptext)
		COMMAND_ID_HANDLER(ID_FIRST_ALWAYSONTOP, OnFirstAlwaysontop)
		COMMAND_ID_HANDLER(ID_FIRST_VIEWTEXT, OnFirstViewtext)
		COMMAND_ID_HANDLER(ID_FIRST_VIEWRTF, OnFirstViewrtf)
		COMMAND_ID_HANDLER(ID_FIRST_VIEWHTML, OnFirstViewhtml)
		COMMAND_ID_HANDLER(ID_FIRST_VIEWASIMAGE, OnFirstViewImage)
		NOTIFY_CODE_HANDLER(EN_MSGFILTER, OnEnMsgfilterRichedit21)
		NOTIFY_CODE_HANDLER(EN_LINK, OnEnLink)
		NOTIFY_CODE_HANDLER(SimpleBrowser::NotificationType::BeforeNavigate2, OnBrowserBeforeNavigate2)
		MESSAGE_HANDLER(WM_DPICHANGED, OnDpiChanged)
		MESSAGE_HANDLER(WM_REFRESH_FOOTER, OnRefreshFooter)
		REFLECT_NOTIFICATIONS_MSG_FILTERED(WM_DRAWITEM)
		REFLECT_NOTIFICATIONS_MSG_FILTERED(WM_CTLCOLORSTATIC)
	END_MSG_MAP()
};
