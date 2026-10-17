#pragma once

class CGroupName : public CDialogImpl<CGroupName>
{
public:
	enum { IDD = IDD_GROUP_NAME };

	CGroupName() = default;

	CString m_csName;

	BEGIN_MSG_MAP(CGroupName)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnOk(WORD, WORD, HWND, BOOL&);
	LRESULT OnCancel(WORD, WORD, HWND, BOOL&);
};
