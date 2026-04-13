#pragma once

class CSelectDB : public CDialogImpl<CSelectDB>
{
public:
	enum { IDD = IDD_SELECT_DB };

	CSelectDB() = default;

	CEdit m_ePath;

	BEGIN_MSG_MAP(CSelectDB)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER(IDC_SELECT, BN_CLICKED, OnSelect)
		COMMAND_HANDLER(IDC_USE_DEFAULT, BN_CLICKED, OnUseDefault)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnUseDefault(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};
