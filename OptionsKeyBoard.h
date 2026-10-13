#pragma once

#include "stdafx.h"
#include "CP_Main.h"
#include "OptionsSheet.h"
#include <atlctrlx.h>
#include <atlbase.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>

class COptionsKeyBoard : public CPropertyPageImpl<COptionsKeyBoard>
{
public:
	enum { IDD = IDD_OPTIONS_KEYSTROKES };

	COptionsKeyBoard();

	BEGIN_MSG_MAP(COptionsKeyBoard)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(CPropertyPageImpl<COptionsKeyBoard>)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
	
	int OnWizardNext() override;
	BOOL OnWizardFinish() override;
	int OnApply() override;
	void OnReset() override;

private:
	COptionsSheet *m_pParent;
	CString m_csTitle;

	CButton m_btSendPaste;
	CButton m_UseUiGroupForLastTen;
	CButton m_btMoveClipOnGlobal10;
	CHotKeyCtrl m_Nine;
	CHotKeyCtrl m_Eight;
	CHotKeyCtrl m_Seven;
	CHotKeyCtrl m_Six;
	CHotKeyCtrl m_Five;
	CHotKeyCtrl m_Four;
	CHotKeyCtrl m_Three;
	CHotKeyCtrl m_Two;
	CHotKeyCtrl m_Ten;
	CHotKeyCtrl m_One;
	CHotKeyCtrl m_HotKey;
	CHotKeyCtrl m_HotKey2;
	CHotKeyCtrl m_HotKey3;
	CHotKeyCtrl m_TextOnlyKey;
	CHyperLink m_CustomeKeysHelp;
	CHotKeyCtrl m_saveClipboardHotKey;
	CHotKeyCtrl m_copyAndSaveClipboardCtrl;
};
