
// FocusHighlightDlg.h : header file
//

#pragma once

#include "ExternalWindowTracker.h"

class CFocusHighlightDlg : public CDialogImpl<CFocusHighlightDlg>
{
public:
	enum { IDD = IDD_FOCUSHIGHLIGHT_DIALOG };

	CFocusHighlightDlg();

	BEGIN_MSG_MAP(CFocusHighlightDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		COMMAND_ID_HANDLER(IDC_BUTTON_HIGHLIGHT_ACTIVE, OnHighlightActive)
		COMMAND_ID_HANDLER(IDC_BUTTON_HIGHLIGHT_FOCUS, OnHighlightFocus)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnTimer(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnHighlightActive(WORD, WORD, HWND, BOOL&);
	LRESULT OnHighlightFocus(WORD, WORD, HWND, BOOL&);
	LRESULT OnCloseCmd(WORD, WORD wID, HWND, BOOL&);

private:
	void StayOnTop();
	void HighlightWindow(HWND hWnd);

	ExternalWindowTracker m_tracker;
	CEdit m_activeEdit;
	CEdit m_focusEdit;
	CButton m_trackFocusChangesCheck;
};
