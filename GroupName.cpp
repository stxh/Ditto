// GroupName.cpp : implementation file
//

#include "stdafx.h"
#include "cp_main.h"
#include "GroupName.h"

LRESULT CGroupName::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
	CWindow nameEdit = GetDlgItem(IDC_NAME);
	if (nameEdit.IsWindow())
	{
		nameEdit.SetWindowText(m_csName);
		nameEdit.SetFocus();
		return FALSE;
	}
	return TRUE;
}

LRESULT CGroupName::OnOk(WORD, WORD, HWND, BOOL&)
{
	CWindow nameEdit = GetDlgItem(IDC_NAME);
	if (nameEdit.IsWindow())
	{
		nameEdit.GetWindowText(m_csName);
	}
	EndDialog(IDOK);
	return 0;
}

LRESULT CGroupName::OnCancel(WORD, WORD, HWND, BOOL&)
{
	EndDialog(IDCANCEL);
	return 0;
}
