#pragma once

#include "ChaiScriptXml.h"
#include "DialogResizer.h"
#include <atlbase.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>
#include <atlctrlx.h>

class CScriptEditor : public CDialogImpl<CScriptEditor>
{
public:
	enum { IDD = IDD_SCRIPT_EDITOR };

	CChaiScriptXml m_xml;
	CString m_title;

	BEGIN_MSG_MAP(CScriptEditor)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER(IDC_LIST_SCRIPTS, LBN_SELCHANGE, OnLbnSelchangeListScripts)
		COMMAND_ID_HANDLER(IDC_BUTTON_ADD_SCRIPT, OnBnClickedButtonAddScript)
		COMMAND_HANDLER(IDC_EDIT_NAME, EN_KILLFOCUS, OnEnKillfocusEditName)
		COMMAND_HANDLER(IDC_EDIT_DESC, EN_KILLFOCUS, OnEnKillfocusEditDesc)
		COMMAND_HANDLER(IDC_RICHEDIT21, EN_KILLFOCUS, OnEnKillfocusEditScript)
		COMMAND_ID_HANDLER(IDC_CHECK_ACTIVE, OnBnClickedCheckActive)
		COMMAND_ID_HANDLER(IDC_BUTTON_DELETE_SCRIPT, OnBnClickedButtonDeleteScript)
		COMMAND_ID_HANDLER(IDC_BUTTON_RUN, OnBnClickedButtonRun)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnSize(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnOK(WORD, WORD, HWND, BOOL&);
	LRESULT OnCancel(WORD, WORD, HWND, BOOL&);
	LRESULT OnLbnSelchangeListScripts(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonAddScript(WORD, WORD, HWND, BOOL&);
	LRESULT OnEnKillfocusEditName(WORD, WORD, HWND, BOOL&);
	LRESULT OnEnKillfocusEditDesc(WORD, WORD, HWND, BOOL&);
	LRESULT OnEnKillfocusEditScript(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedCheckActive(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonDeleteScript(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonRun(WORD, WORD, HWND, BOOL&);
	BOOL PreTranslateMessage(MSG* pMsg);

private:
	void EnableDisable(BOOL enable);

	CListBox m_scriptsList;
	CDialogResizer m_resize;
	CRichEditCtrl m_rich;
};
