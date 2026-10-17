#pragma once
#include "DialogResizer.h"
#include <atlbase.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>

class CFriendPromptDlg : public CDialogImpl<CFriendPromptDlg>
{
public:
	enum { IDD = IDD_DIALOG_FREIND_PROMPT };

	CFriendPromptDlg() : m_save(false), m_clearList(false) {}

	CString GetName() { return m_name; }
	CString GetDesc() { return m_desc; }
	bool GetSave() { return m_save; }
	bool GetClearList() { return m_clearList; }

	BEGIN_MSG_MAP(CFriendPromptDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
		COMMAND_ID_HANDLER(IDOK, OnBnClickedOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnBnClickedCancel)
		COMMAND_ID_HANDLER(IDC_BUTTON_CLEAR, OnBnClickedButtonClear)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnBnClickedOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonClear(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	CString m_name;
	CString m_desc;
	CBrush m_brush;
	bool m_save;
	bool m_clearList;
	CEdit m_textBox;
	CEdit m_description;
};
