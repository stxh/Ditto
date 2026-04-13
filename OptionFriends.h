#pragma once

#include "OptionsSheet.h"
#include <atlbase.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>

class COptionFriends : public CPropertyPageImpl<COptionFriends>
{
public:
	enum { IDD = IDD_OPTIONS_FRIENDS };

	COptionFriends();
	~COptionFriends();

	BEGIN_MSG_MAP(COptionFriends)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		NOTIFY_HANDLER(IDC_LIST, NM_DBLCLK, OnDblclkList)
		NOTIFY_HANDLER(IDC_LIST, LVN_KEYDOWN, OnKeydownList)
		COMMAND_ID_HANDLER(IDC_CHECK_DISABLE_FRIENDS, OnCheckDisableFriends)
		CHAIN_MSG_MAP(CPropertyPageImpl<COptionFriends>)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnDblclkList(int, LPNMHDR pnmh, BOOL&);
	LRESULT OnKeydownList(int, LPNMHDR pnmh, BOOL&);
	LRESULT OnCheckDisableFriends(WORD, WORD, HWND, BOOL&);
	
	int OnApply() override;

private:
	void InitListCtrlCols();
	void InsertItems();
	BOOL EditItem(int nItem);

	CString m_csTitle;
	COptionsSheet *m_pParent;
	CButton m_bDisableRecieve;
	CButton m_SendRecieve;
	CListViewCtrl m_List;
	CString m_PlaceOnClipboard;
	CString m_csPassword;
	CString m_csAdditionalPasswords;
};
