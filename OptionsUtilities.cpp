#include "stdafx.h"
#include "CP_Main.h"
#include "OptionsUtilities.h"

LRESULT COptionsUtilities::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_ePath.Attach(GetDlgItem(IDC_PATH));
	m_ePath.SetWindowText(CGetSetOptions::GetDBPath(FALSE));
	return 0;
}

LRESULT COptionsUtilities::OnCompactDB(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CompactDatabase();
	return 0;
}

LRESULT COptionsUtilities::OnRepair(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	RepairDatabase();
	return 0;
}

int COptionsUtilities::OnApply()
{
	CString csPath;
	m_ePath.GetWindowText(csPath);

	CString csOldPath = CGetSetOptions::GetDBPath(FALSE);

	if(csOldPath != csPath)
	{
		CGetSetOptions::SetDBPath(csPath);
	}
		
	return PSNRET_NOERROR;
}

LRESULT COptionsUtilities::OnGetPath(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) 
{
	OPENFILENAME FileName;
	TCHAR szFileName[400];
	TCHAR szDir[400];

	memset(&FileName, 0, sizeof(FileName));
	memset(szFileName, 0, sizeof(szFileName));
	memset(&szDir, 0, sizeof(szDir));

	FileName.lStructSize = sizeof(FileName);
	FileName.hwndOwner = m_hWnd;
	FileName.lpstrTitle = _T("Open Database");
	FileName.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	FileName.nMaxFile = 400;
	FileName.lpstrFile = szFileName;
	FileName.lpstrInitialDir = szDir;
	FileName.lpstrFilter = _T("Database Files (.MDB)\0*.mdb");
	FileName.lpstrDefExt = _T("mdb");

	if(GetOpenFileName(&FileName) == 0)
		return 0;

	CString csPath(FileName.lpstrFile);
	m_ePath.SetWindowText(csPath);		
	
	return 0;
}
