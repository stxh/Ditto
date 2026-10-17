// FriendDetails.cpp : implementation file
//

#include "stdafx.h"
#include "cp_main.h"
#include "FriendDetails.h"

LRESULT CFriendDetails::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
	::CheckDlgButton(m_hWnd, IDC_CHECK_SEND_ALL, m_checkSendAll ? BST_CHECKED : BST_UNCHECKED);
	::SetDlgItemText(m_hWnd, IDC_EDIT_DESCRIPTION, m_csDescription);
	::SetDlgItemText(m_hWnd, IDC_EDIT_IP, m_csIP);

	theApp.m_Language.UpdateOptionFriendsDetail(m_hWnd);

	return TRUE;
}

LRESULT CFriendDetails::OnOk(WORD, WORD, HWND, BOOL&)
{
	m_checkSendAll = (::IsDlgButtonChecked(m_hWnd, IDC_CHECK_SEND_ALL) == BST_CHECKED);
	::GetDlgItemText(m_hWnd, IDC_EDIT_DESCRIPTION, m_csDescription.GetBuffer(256), 256);
	m_csDescription.ReleaseBuffer();
	::GetDlgItemText(m_hWnd, IDC_EDIT_IP, m_csIP.GetBuffer(256), 256);
	m_csIP.ReleaseBuffer();

	EndDialog(IDOK);
	return 0;
}

LRESULT CFriendDetails::OnCancel(WORD, WORD, HWND, BOOL&)
{
	EndDialog(IDCANCEL);
	return 0;
}
