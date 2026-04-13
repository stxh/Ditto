#include "stdafx.h"
#include "cp_main.h"
#include "SelectDB.h"

LRESULT CSelectDB::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_ePath = GetDlgItem(IDC_PATH);
	m_ePath.SetWindowText(CGetSetOptions::GetDBPath(FALSE));
	m_ePath.SetFocus();
	bHandled = TRUE;
	return FALSE;
}

LRESULT CSelectDB::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CString csPath;
	m_ePath.GetWindowText(csPath);
	CGetSetOptions::SetDBPath(csPath);
	EndDialog(IDOK);
	return 0;
}

LRESULT CSelectDB::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(IDCANCEL);
	return 0;
}

LRESULT CSelectDB::OnSelect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	OPENFILENAME FileName = {0};
	TCHAR szFileName[400] = {0};
	TCHAR szDir[400] = {0};

	FileName.lStructSize = sizeof(FileName);
	FileName.hwndOwner = m_hWnd;
	FileName.lpstrTitle = _T("Open Database");
	FileName.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
	FileName.nMaxFile = _countof(szFileName);
	FileName.lpstrFile = szFileName;
	FileName.lpstrInitialDir = szDir;
	FileName.lpstrFilter = _T("Database Files (.MDB)\0*.mdb\0All Files (*.*)\0*.*\0\0");
	FileName.lpstrDefExt = _T("mdb");

	if (GetOpenFileName(&FileName) == 0)
	{
		return 0;
	}

	CString csPath(FileName.lpstrFile);

	if (!ValidDB(csPath))
	{
		MessageBox(_T("Invalid Database"), _T("Ditto"), MB_OK);
		m_ePath.SetFocus();
	}
	else
	{
		m_ePath.SetWindowText(csPath);
	}

	return 0;
}

LRESULT CSelectDB::OnUseDefault(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetDBPath(_T(""));
	CString csPath = CGetSetOptions::GetDBPath();

	if (!ValidDB(csPath))
	{
		::DeleteFile(csPath);
	}

	if (CheckDBExists(CGetSetOptions::GetDBPath()))
	{
		EndDialog(IDOK);
	}

	return 0;
}
