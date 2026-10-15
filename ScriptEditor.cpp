#include "stdafx.h"
#include "CP_Main.h"
#include "ScriptEditor.h"
#include "ChaiScriptOnCopy.h"
#include "Shared\TextConvert.h"
#include "DittoChaiScript.h"

static BOOL bDummy = FALSE;

LRESULT CScriptEditor::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
	SetWindowText(m_title);

	m_scriptsList.Attach(GetDlgItem(IDC_LIST_SCRIPTS));
	m_rich.Attach(GetDlgItem(IDC_RICHEDIT21));

	int index = 0;
	for (auto& listItem : m_xml.m_list)
	{
		int row = m_scriptsList.AddString(listItem.m_name);
		m_scriptsList.SetItemData(row, index);
		index++;
	}

	if (index > 0)
	{
		m_scriptsList.SetSel(0);
		m_scriptsList.SetCurSel(0);
		m_scriptsList.SetCaretIndex(0);
		m_scriptsList.SetAnchorIndex(0);

		OnLbnSelchangeListScripts(0, 0, NULL, bDummy);
	}
	else
	{
		EnableDisable(FALSE);
	}

	m_resize.SetParent(m_hWnd);
	m_resize.AddControl(IDOK, DR_MoveTop | DR_MoveLeft);
	m_resize.AddControl(IDCANCEL, DR_MoveTop | DR_MoveLeft);

	m_resize.AddControl(IDC_LIST_SCRIPTS, DR_SizeHeight);
	m_resize.AddControl(IDC_EDIT_DESC, DR_SizeWidth);
	m_resize.AddControl(IDC_EDIT_NAME, DR_SizeWidth);
	m_resize.AddControl(IDC_RICHEDIT21, DR_SizeWidth | DR_SizeHeight);

	m_resize.AddControl(IDC_STATIC_RETURN_DESC, DR_MoveTop | DR_SizeWidth);
	m_resize.AddControl(IDC_STATIC_INPUT, DR_MoveTop);
	m_resize.AddControl(IDD_SCRIPT_EDITOR, DR_MoveTop | DR_SizeWidth);
	m_resize.AddControl(IDC_EDIT_OUTPUT, DR_MoveTop | DR_SizeWidth);
	m_resize.AddControl(IDC_STATIC_OUTPUT, DR_MoveTop);
	m_resize.AddControl(IDC_EDIT_INPUT, DR_MoveTop | DR_SizeWidth);

	m_resize.AddControl(IDC_BUTTON_RUN, DR_MoveTop | DR_MoveLeft);

	m_resize.AddControl(IDC_EDIT_ACTIVE_APP, DR_MoveTop | DR_MoveLeft);
	m_resize.AddControl(IDC_EDIT_ACTIVE_APP_TITLE, DR_MoveTop | DR_MoveLeft);

	m_resize.AddControl(IDC_BUTTON_DELETE_SCRIPT, DR_MoveTop);
	m_resize.AddControl(IDC_BUTTON_ADD_SCRIPT, DR_MoveTop);

	SetDlgItemText(IDC_EDIT_ACTIVE_APP, _T("App name"));
	SetDlgItemText(IDC_EDIT_ACTIVE_APP_TITLE, _T("App Title"));

	return FALSE;
}

LRESULT CScriptEditor::OnLbnSelchangeListScripts(WORD, WORD, HWND, BOOL&)
{
	int listIndex = m_scriptsList.GetCurSel();
	if (listIndex >= 0 && listIndex < (int)m_xml.m_list.size())
	{
		SetDlgItemText(IDC_EDIT_NAME, m_xml.m_list[listIndex].m_name);
		SetDlgItemText(IDC_EDIT_DESC, m_xml.m_list[listIndex].m_description);
		if (m_xml.m_list[listIndex].m_active)
		{
			CheckDlgButton(IDC_CHECK_ACTIVE, BST_CHECKED);
		}
		else
		{
			CheckDlgButton(IDC_CHECK_ACTIVE, BST_UNCHECKED);
		}
		SetDlgItemText(IDC_RICHEDIT21, m_xml.m_list[listIndex].m_script);

		GetDlgItem(IDC_EDIT_NAME).SetFocus();
	}
	return 0;
}

LRESULT CScriptEditor::OnBnClickedButtonAddScript(WORD, WORD, HWND, BOOL&)
{
	CDittoChaiScriptXmlItem newItem;
	newItem.m_name = _T("New Script");
	newItem.m_active = true;
	newItem.m_guid = NewGuidString();
	m_xml.m_list.push_back(newItem);

	int index = m_scriptsList.AddString(newItem.m_name);

	m_scriptsList.SetSel(index);
	m_scriptsList.SetCurSel(index);
	m_scriptsList.SetCaretIndex(index);
	m_scriptsList.SetAnchorIndex(index);

	SetDlgItemText(IDC_EDIT_NAME, newItem.m_name);
	SetDlgItemText(IDC_EDIT_DESC, _T(""));
	SetDlgItemText(IDC_RICHEDIT21, _T(""));
	CheckDlgButton(IDC_CHECK_ACTIVE, BST_CHECKED);

	GetDlgItem(IDC_EDIT_NAME).SetFocus();

	EnableDisable(TRUE);

	return 0;
}

LRESULT CScriptEditor::OnEnKillfocusEditName(WORD, WORD, HWND, BOOL&)
{
	int listIndex = m_scriptsList.GetCurSel();
	if (listIndex >= 0 && listIndex < (int)m_xml.m_list.size())
	{
		CString name;
		GetDlgItemText(IDC_EDIT_NAME, name);

		if (m_xml.m_list[listIndex].m_name != name)
		{
			m_xml.m_list[listIndex].m_name = name;

			m_scriptsList.SetRedraw(FALSE);

			m_scriptsList.DeleteString(listIndex);
			m_scriptsList.InsertString(listIndex, name);
			m_scriptsList.SetSel(listIndex);
			m_scriptsList.SetCurSel(listIndex);
			m_scriptsList.SetCaretIndex(listIndex);
			m_scriptsList.SetAnchorIndex(listIndex);
			m_scriptsList.SetRedraw(TRUE);
			m_scriptsList.UpdateWindow();
		}
	}
	return 0;
}

LRESULT CScriptEditor::OnEnKillfocusEditDesc(WORD, WORD, HWND, BOOL&)
{
	int listIndex = m_scriptsList.GetCurSel();
	if (listIndex >= 0 && listIndex < (int)m_xml.m_list.size())
	{
		CString desc;
		GetDlgItemText(IDC_EDIT_DESC, desc);
		m_xml.m_list[listIndex].m_description = desc;
	}
	return 0;
}

LRESULT CScriptEditor::OnEnKillfocusEditScript(WORD, WORD, HWND, BOOL&)
{
	int listIndex = m_scriptsList.GetCurSel();
	if (listIndex >= 0 && listIndex < (int)m_xml.m_list.size())
	{
		CString script;
		GetDlgItemText(IDC_RICHEDIT21, script);
		m_xml.m_list[listIndex].m_script = script;
	}
	return 0;
}

LRESULT CScriptEditor::OnBnClickedCheckActive(WORD, WORD, HWND, BOOL&)
{
	int listIndex = m_scriptsList.GetCurSel();
	if (listIndex >= 0 && listIndex < (int)m_xml.m_list.size())
	{
		if (IsDlgButtonChecked(IDC_CHECK_ACTIVE) == BST_CHECKED)
		{
			m_xml.m_list[listIndex].m_active = true;
		}
		else
		{
			m_xml.m_list[listIndex].m_active = false;
		}
	}
	return 0;
}

LRESULT CScriptEditor::OnSize(UINT, WPARAM wParam, LPARAM lParam, BOOL&)
{
	if (wParam != SIZE_MINIMIZED)
	{
		m_resize.MoveControls(CSize(LOWORD(lParam), HIWORD(lParam)));
	}
	return 0;
}

LRESULT CScriptEditor::OnBnClickedButtonDeleteScript(WORD, WORD, HWND, BOOL&)
{
	int listIndex = m_scriptsList.GetCurSel();
	if (listIndex >= 0 && listIndex < (int)m_xml.m_list.size())
	{
		m_xml.m_list.erase(m_xml.m_list.begin() + listIndex);
		m_scriptsList.DeleteString(listIndex);

		if (m_scriptsList.GetCount() <= listIndex)
		{
			listIndex--;
		}

		if (listIndex >= 0)
		{
			m_scriptsList.SetSel(listIndex);
			m_scriptsList.SetCurSel(listIndex);
			m_scriptsList.SetCaretIndex(listIndex);
			m_scriptsList.SetAnchorIndex(listIndex);

			OnLbnSelchangeListScripts(0, 0, NULL, bDummy);
		}
		else
		{
			EnableDisable(FALSE);
		}
	}
	return 0;
}

void CScriptEditor::EnableDisable(BOOL enable)
{
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_NAME), enable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_DESC), enable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_RICHEDIT21), enable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_INPUT), enable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_OUTPUT), enable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_RUN), enable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_CHECK_ACTIVE), enable);
}

LRESULT CScriptEditor::OnBnClickedButtonRun(WORD, WORD, HWND, BOOL&)
{
	CWaitCursor wait;

	CString input;
	GetDlgItemText(IDC_EDIT_INPUT, input);
	CString script;
	GetDlgItemText(IDC_RICHEDIT21, script);

	CString app;
	GetDlgItemText(IDC_EDIT_ACTIVE_APP, app);

	CString appTitle;
	GetDlgItemText(IDC_EDIT_ACTIVE_APP_TITLE, appTitle);

	CClip clip;

	ChaiScriptOnCopy test;
	CDittoChaiScript clipData(&clip, (LPCSTR)CTextConvert::UnicodeToAnsi(app), (LPCSTR)CTextConvert::UnicodeToAnsi(appTitle));
	clipData.SetAsciiString((LPCSTR)CTextConvert::UnicodeToAnsi(input));
	clipData.DescriptionReplaceRegex(".*", (LPCSTR)CTextConvert::UnicodeToAnsi(input));

	test.ProcessScript(clipData, (LPCSTR)CTextConvert::UnicodeToAnsi(script));

	if (test.m_lastError == _T(""))
	{
		CString currentString = CTextConvert::AnsiToUnicode(clipData.GetAsciiString().c_str());
		if (currentString == _T(""))
		{
			currentString = clipData.GetUnicodeString().c_str();
		}

		SetDlgItemText(IDC_EDIT_OUTPUT, _T("returned false\r\n") + currentString);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_OUTPUT, test.m_lastError);
	}

	return 0;
}

BOOL CScriptEditor::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_TAB) && (pMsg->hwnd == m_rich.m_hWnd))
	{
		long lStart, lEnd;
		m_rich.GetSel(lStart, lEnd);

		m_rich.SetSel(lStart, lStart);
		m_rich.ReplaceSel(_T("\t"), TRUE);

		return TRUE;
	}

	return FALSE;
}

LRESULT CScriptEditor::OnOK(WORD, WORD, HWND, BOOL&)
{
	BOOL dummy = FALSE;
	OnEnKillfocusEditName(0, 0, NULL, dummy);
	OnEnKillfocusEditDesc(0, 0, NULL, dummy);
	OnEnKillfocusEditScript(0, 0, NULL, dummy);

	EndDialog(IDOK);
	return 0;
}

LRESULT CScriptEditor::OnCancel(WORD, WORD, HWND, BOOL&)
{
	EndDialog(IDCANCEL);
	return 0;
}
