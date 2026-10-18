#pragma once

#include "DialogResizer.h"
#include "PropGrid.h"

// Enter in the filter box jumps to the next matching property (same trick as
// the delete-clips dialog's search edit).
class CAdvFilterEdit : public ATL::CWindowImpl<CAdvFilterEdit, WTL::CEdit>
{
public:
	DECLARE_WND_SUPERCLASS(_T("Ditto_AdvFilterEdit"), GetWndClassName())

	BEGIN_MSG_MAP(CAdvFilterEdit)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	END_MSG_MAP()

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam == VK_RETURN)
		{
			::SendMessage(GetParent().m_hWnd, WM_COMMAND, MAKEWPARAM(IDC_BUTTON_NEXT_MATCH, BN_CLICKED), 0);
			return 0;
		}

		bHandled = FALSE;
		return 0;
	}
};

// Advanced Options.  The MFC build drove this page with CMFCPropertyGridCtrl;
// WTL has no equivalent, so the rows live in CPropGrid instead.
class CAdvGeneral : public CDialogImpl<CAdvGeneral>
{
public:
	enum { IDD = IDD_ADV_OPTIONS };

	CAdvGeneral();

	BEGIN_MSG_MAP(CAdvGeneral)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_NCLBUTTONDOWN, OnNcLButtonDown)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_BT_COMPACT_AND_REPAIR, OnBnClickedBtCompactAndRepair)
		COMMAND_ID_HANDLER(IDC_BUTTON_COPY_SCRIPTS, OnBnClickedButtonCopyScripts)
		COMMAND_ID_HANDLER(IDC_BUTTON_PASTE_SCRIPTS, OnBnClickedButtonPasteScripts)
		COMMAND_ID_HANDLER(IDC_BUTTON_NEXT_MATCH, OnBnClickedButtonNextMatch)
		COMMAND_ID_HANDLER(IDC_BUTTON_COPY_SCRIPTS2, OnBnClickedButtonCopyScripts2)
		COMMAND_HANDLER(IDC_EDIT_ADV_FILTER, EN_CHANGE, OnEnChangeAdvFilter)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnSize(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnGetMinMaxInfo(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnNcLButtonDown(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnOK(WORD, WORD, HWND, BOOL&);
	LRESULT OnCancel(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedBtCompactAndRepair(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonCopyScripts(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonPasteScripts(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonNextMatch(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonCopyScripts2(WORD, WORD, HWND, BOOL&);
	LRESULT OnEnChangeAdvFilter(WORD, WORD, HWND, BOOL&);

private:
	void Search(bool fromSelection);
	void AddTrueFalse(LPCTSTR szName, BOOL bValue, int nSettingId);
	void BuildGrid();
	void ApplySettings();

	CPropGrid m_grid;
	CDialogResizer m_Resize;
	CAdvFilterEdit m_editFilter;
	bool m_mouseDownOnCaption;
};
