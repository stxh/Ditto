#pragma once

#include <atlbase.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>

class COptionsTypes : public CPropertyPageImpl<COptionsTypes>
{
public:
	enum { IDD = IDD_OPTIONS_TYPES };

	COptionsTypes();

	BEGIN_MSG_MAP(COptionsTypes)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_DELETE, OnDelete)
		COMMAND_ID_HANDLER(IDC_ADD, OnAdd)
		CHAIN_MSG_MAP(CPropertyPageImpl<COptionsTypes>)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnDelete(WORD, WORD, HWND, BOOL&);
	LRESULT OnAdd(WORD, WORD, HWND, BOOL&);
	
	int OnApply() override;

private:
	bool m_bSave;
	CString m_csTitle;
	CListBox m_List;
	
	BOOL TextAllReadyThere(const CString &cs);
};
