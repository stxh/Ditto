#pragma once

#include <atlbase.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>

class COptionsUtilities : public CPropertyPageImpl<COptionsUtilities>
{
public:
	enum { IDD = IDD_OPTIONS_UTILITIES };

	COptionsUtilities() {}

	BEGIN_MSG_MAP(COptionsUtilities)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_COMPACT_DB, OnCompactDB)
		COMMAND_ID_HANDLER(IDC_REPAIR, OnRepair)
		COMMAND_ID_HANDLER(IDC_GET_PATH, OnGetPath)
		CHAIN_MSG_MAP(CPropertyPageImpl<COptionsUtilities>)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCompactDB(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRepair(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnGetPath(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	int OnApply() override;

private:
	CEdit m_ePath;
};
