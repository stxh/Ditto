#include "stdafx.h"
#include "CP_Main.h"
#include "FriendPromptDlg.h"
#include "..\Shared\Tokenizer.h"

LRESULT CFriendPromptDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_textBox.Attach(GetDlgItem(IDC_EDIT_NAME));
	m_description.Attach(GetDlgItem(IDC_EDIT_DESC));

	::SendMessage(m_hWnd, WM_SETICON, 0, NULL);

	m_brush.CreateSolidBrush(RGB(255, 255, 255));

	int extendedStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, extendedStyle | WS_EX_DLGMODALFRAME);
	::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE |	SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	CheckDlgButton(IDC_CHECK_SAVE, BST_CHECKED);

	m_textBox.SetFocus();
	
	return 0;
}

LRESULT CFriendPromptDlg::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CFriendPromptDlg::OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	return (LRESULT)GetStockObject(WHITE_BRUSH);
}

LRESULT CFriendPromptDlg::OnBnClickedOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_textBox.GetWindowTextW(m_name);
	m_description.GetWindowTextW(m_desc);

	if (IsDlgButtonChecked(IDC_CHECK_SAVE) == BST_CHECKED)
	{
		m_save = true;
	}

	EndDialog(IDOK);
	return 0;
}

LRESULT CFriendPromptDlg::OnBnClickedCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return 0;
}

LRESULT CFriendPromptDlg::OnBnClickedButtonClear(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_clearList = true;
	EndDialog(IDOK);
	return 0;
}
