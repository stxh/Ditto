#pragma once

#include <vector>

// Property grid for the Advanced Options page.  MFC shipped CMFCPropertyGridCtrl,
// WTL has no equivalent, so the page owns this owner-drawn list instead: categories
// with child rows, a name/value split, and in-place editing per value type.
class CPropGrid : public CWindowImpl<CPropGrid>
{
public:
	enum Kind { KindText, KindInt, KindChoice, KindFile };

	enum { IDC_PROPGRID_EDIT = 0x4F01, IDC_PROPGRID_COMBO = 0x4F02 };

	struct Row
	{
		Row() : dataId(0), kind(KindText), level(0), bCategory(FALSE), bExpanded(TRUE), bAllowEdit(TRUE) {}

		CString name;
		CString value;
		CString original;
		CString desc;
		std::vector<CString> options;
		CString filter;
		int dataId;
		Kind kind;
		int level;
		BOOL bCategory;
		BOOL bExpanded;
		BOOL bAllowEdit;
	};

	DECLARE_WND_CLASS_EX(_T("DittoPropGrid"), CS_DBLCLKS, COLOR_WINDOW)

	BEGIN_MSG_MAP(CPropGrid)
		MSG_WM_CREATE(OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
		COMMAND_HANDLER(IDC_PROPGRID_EDIT, EN_KILLFOCUS, OnEditKillFocus)
		COMMAND_HANDLER(IDC_PROPGRID_COMBO, CBN_CLOSEUP, OnComboCloseUp)
		NOTIFY_CODE_HANDLER(TTN_NEEDTEXTW, OnNeedText)
		// The editors are contained windows, so their keys land here instead of
		// in the dialog manager (Enter would otherwise fire the default pushbutton).
		// Contained windows dispatch through the container's message map directly,
		// where ATL's cracked handlers would write through an unset m_pCurrentMsg,
		// so these rows have to stay in the MESSAGE_HANDLER form.
		ALT_MSG_MAP(1)
			MESSAGE_HANDLER(WM_KEYDOWN, OnEditKeyDown)
			MESSAGE_HANDLER(WM_CHAR, OnEditChar)
			MESSAGE_HANDLER(WM_GETDLGCODE, OnEditGetDlgCode)
		ALT_MSG_MAP(2)
			MESSAGE_HANDLER(WM_KEYDOWN, OnComboKeyDown)
			MESSAGE_HANDLER(WM_GETDLGCODE, OnEditGetDlgCode)
	END_MSG_MAP()

	CPropGrid();

	int AddCategory(LPCTSTR szName, BOOL bExpanded = TRUE);
	int AddText(int nCategory, LPCTSTR szName, LPCTSTR szValue, LPCTSTR szDesc, int nId);
	int AddInt(int nCategory, LPCTSTR szName, long nValue, LPCTSTR szDesc, int nId);
	int AddChoice(int nCategory, LPCTSTR szName, LPCTSTR szValue, const CString* pOptions, int nOptionCount, LPCTSTR szDesc, int nId);
	int AddFile(int nCategory, LPCTSTR szName, LPCTSTR szValue, LPCTSTR szFilter, LPCTSTR szDesc, int nId);

	Row* FindById(int nId);
	CString GetOriginal(int nId);
	CString GetValue(int nId);
	long GetIntValue(int nId);
	BOOL GetBoolValue(int nId);
	void SetValue(int nId, LPCTSTR szValue);

	void SetGridFont(HFONT hFont);
	void EndEdit(BOOL bCommit);
	BOOL IsEditing() const				{ return m_bEditing; }
	const std::vector<Row>& Rows() const	{ return m_rows; }
	int SelectedRow() const				{ return m_nSelected; }

private:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnPaint(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnSize(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnSetFocus(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnKillFocus(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnLButtonDown(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnMouseWheel(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnVScroll(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnKeyDown(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnGetDlgCode(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnEditKillFocus(WORD, WORD, HWND, BOOL&);
	LRESULT OnComboCloseUp(WORD, WORD, HWND, BOOL&);
	LRESULT OnNeedText(WORD, LPNMHDR, BOOL&);
	LRESULT OnEditKeyDown(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnEditChar(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnComboKeyDown(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnEditGetDlgCode(UINT, WPARAM, LPARAM, BOOL&);

	int RowHeight() const			{ return m_nRowHeight; }
	int NameWidth();
	void BuildVisible();
	void UpdateScrollbar();
	void EnsureVisible(int nVisibleIndex);
	void SetTop(int nTop);
	void SetSelection(int nRow, BOOL bEditValue);
	int HitTest(CPoint pt, BOOL& bValueCell);
	void BeginEdit();
	void BrowseFile();
	CRect ValueRect(int nVisibleIndex);

	std::vector<Row> m_rows;
	std::vector<int> m_visible;
	int m_nSelected;
	int m_nTop;
	int m_nRowHeight;
	HFONT m_hFont;
	Kind m_eEditKind;
	CFont m_catFont;
	CContainedWindowT<CEdit> m_edit;
	CContainedWindowT<CComboBox> m_combo;
	CToolTipCtrl m_tip;
	CString m_szTipText;
	BOOL m_bEditing;
};
