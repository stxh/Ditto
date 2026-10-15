#pragma once

#include "DialogResizer.h"
#include "PropGrid.h"

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

private:
	void AddTrueFalse(LPCTSTR szName, BOOL bValue, int nSettingId);
	void BuildGrid();
	void ApplySettings();

	CPropGrid m_grid;
	CDialogResizer m_Resize;
	bool m_mouseDownOnCaption;
};
