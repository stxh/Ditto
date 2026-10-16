#include "stdafx.h"
#include "CP_Main.h"
#include "OptionsGeneral.h"
#include "InternetUpdate.h"
#include <io.h>
#include <Mmsystem.h>
#include "Path.h"
#include "DimWnd.h"
#include "AdvGeneral.h"
#include <atlctrlx.h>

using namespace nsPath;

#define DEFAULT_THEME _T("(Ditto)")

COptionsGeneral::COptionsGeneral()
{
	m_csTitle = theApp.m_Language.GetString("GeneralTitle", "General");
	m_psp.pszTitle = m_csTitle;
	m_psp.dwFlags |= PSP_USETITLE;

	memset(&m_LogFont, 0, sizeof(LOGFONT));
}

COptionsGeneral::~COptionsGeneral()
{
}

LRESULT COptionsGeneral::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
	m_brush.CreateSolidBrush(RGB(251, 251, 251));

	m_cbLanguage.Attach(GetDlgItem(IDC_COMBO_LANGUAGE));
	m_ePath.Attach(GetDlgItem(IDC_PATH));
	m_eExpireAfter.SubclassWindow(GetDlgItem(IDC_EXPIRE_AFTER));
	m_eMaxSavedCopies.SubclassWindow(GetDlgItem(IDC_MAX_SAVED_COPIES));
	m_btExpire.Attach(GetDlgItem(IDC_EXPIRE));
	m_btRunOnStartup.Attach(GetDlgItem(IDC_START_ON_STARTUP));
	m_copyAppInclude.Attach(GetDlgItem(IDC_EDIT_APP_COPY_INCLUDE));
	m_copyAppExclude.Attach(GetDlgItem(IDC_EDIT_APP_COPY_EXCLUDE));
	m_btMaximumCheck.Attach(GetDlgItem(IDC_MAXIMUM));
	m_cbTheme.Attach(GetDlgItem(IDC_COMBO_THEME));
	m_btFont.Attach(GetDlgItem(IDC_BUTTON_FONT));
	m_btDefaultButton.Attach(GetDlgItem(IDC_BUTTON_DEFAULT_FAULT));
	m_popupPositionCombo.Attach(GetDlgItem(IDC_COMBO_POPUP_POSITION));
	m_envVarLink.Attach(GetDlgItem(IDC_MFCLINK_ENV_VAR));

#ifndef _DEBUG
	if (CGetSetOptions::GetIsWindowsApp())
	{
		m_btRunOnStartup.SetCheck(BST_CHECKED);
		GetDlgItem(IDC_START_ON_STARTUP).EnableWindow(FALSE);
	}
	else
	{
		m_btRunOnStartup.SetCheck(CGetSetOptions::GetRunOnStartUp());
	}
#endif

	m_btMaximumCheck.SetCheck(CGetSetOptions::GetCheckForMaxEntries());
	if (CGetSetOptions::GetCheckForMaxEntries() == FALSE)
	{
		m_eMaxSavedCopies.EnableWindow(FALSE);
	}
	m_btExpire.SetCheck(CGetSetOptions::GetCheckForExpiredEntries());
	if (CGetSetOptions::GetCheckForExpiredEntries() == FALSE)
	{
		m_eExpireAfter.EnableWindow(FALSE);
	}
	m_eExpireAfter.SetNumber(CGetSetOptions::GetExpiredEntries());
	m_eMaxSavedCopies.SetNumber(CGetSetOptions::GetMaxEntries());

	if (CGetSetOptions::GetDisableExpireClipsConfig())
	{
		m_btMaximumCheck.EnableWindow(FALSE);
		m_btExpire.EnableWindow(FALSE);
		m_eExpireAfter.EnableWindow(FALSE);
		m_eMaxSavedCopies.EnableWindow(FALSE);
	}

	m_copyAppInclude.SetWindowText(CGetSetOptions::GetCopyAppInclude());
	m_copyAppExclude.SetWindowText(CGetSetOptions::GetCopyAppExclude());

	CGetSetOptions::GetFont(m_LogFont);

	CString cs;
	cs.Format(_T("Font - %s (%d)"), m_LogFont.lfFaceName, GetFontSize(m_hWnd, m_LogFont));
	m_btFont.SetWindowText(cs);

	FillThemes();
	FillLanguages();

	int caretPos = m_popupPositionCombo.AddString(theApp.m_Language.GetString("AtCaret", "At Caret"));
	m_popupPositionCombo.SetItemData(caretPos, POS_AT_CARET);

	int cursorPos = m_popupPositionCombo.AddString(theApp.m_Language.GetString("AtCursor", "At Cursor"));
	m_popupPositionCombo.SetItemData(cursorPos, POS_AT_CURSOR);

	int prevPos = m_popupPositionCombo.AddString(theApp.m_Language.GetString("AtPreviousPosition", "At Previous Position"));
	m_popupPositionCombo.SetItemData(prevPos, POS_AT_PREVIOUS);

	switch (CGetSetOptions::GetQuickPastePosition())
	{
	case POS_AT_CARET:
		m_popupPositionCombo.SetCurSel(caretPos);
		break;
	case POS_AT_CURSOR:
		m_popupPositionCombo.SetCurSel(cursorPos);
		break;
	case POS_AT_PREVIOUS:
		m_popupPositionCombo.SetCurSel(prevPos);
		break;
	default:
		m_popupPositionCombo.SetCurSel(cursorPos);
	}

	theApp.m_Language.UpdateOptionGeneral(m_hWnd);

	CString csPath = CGetSetOptions::GetDBPath(false);
	m_ePath.SetWindowText(csPath);

	// TRUE: no focus set here, let the dialog manager select the first control.
	return TRUE;
}

#define NO_MATCH	-2
#define FOUND_MATCH	-1

void COptionsGeneral::FillLanguages()
{
	CString csFile = CGetSetOptions::GetPath(PATH_LANGUAGE);
	csFile += "*.xml";

	CString csLanguage = CGetSetOptions::GetLanguageFile();

	CFileFind find;
	BOOL bCont = find.FindFile(csFile);
	int nEnglishIndex = NO_MATCH;

	while (bCont)
	{
		bCont = find.FindNextFile();
		int nIndex = m_cbLanguage.AddString(find.GetFileTitle());

		if (find.GetFileTitle() == csLanguage)
		{
			nEnglishIndex = -1;
			m_cbLanguage.SetCurSel(nIndex);
		}
		else if (find.GetFileTitle() == _T("English"))
		{
			if (nEnglishIndex == NO_MATCH)
				nEnglishIndex = nIndex;
		}
	}

	if (nEnglishIndex >= 0)
	{
		m_cbLanguage.SetCurSel(nEnglishIndex);
	}
}

int COptionsGeneral::OnApply()
{
#ifndef _DEBUG
	if (CGetSetOptions::GetIsWindowsApp() == FALSE)
	{
		CGetSetOptions::SetRunOnStartUp(m_btRunOnStartup.GetCheck());
	}
#endif

	CGetSetOptions::SetCheckForMaxEntries(m_btMaximumCheck.GetCheck());
	CGetSetOptions::SetCheckForExpiredEntries(m_btExpire.GetCheck());
	CGetSetOptions::SetMaxEntries(m_eMaxSavedCopies.GetNumber());
	CGetSetOptions::SetExpiredEntries(m_eExpireAfter.GetNumber());

	CString stringVal;

	m_copyAppInclude.GetWindowText(stringVal);
	CGetSetOptions::SetCopyAppInclude(stringVal);
	m_copyAppExclude.GetWindowText(stringVal);
	CGetSetOptions::SetCopyAppExclude(stringVal);

	CString csLanguage;
	if (m_cbLanguage.GetCurSel() >= 0)
	{
		m_cbLanguage.GetLBText(m_cbLanguage.GetCurSel(), csLanguage);
		CGetSetOptions::SetLanguageFile(csLanguage);
	}

	if (csLanguage.IsEmpty() == FALSE)
	{
		if (!theApp.m_Language.LoadLanguageFile(csLanguage))
		{
			CString cs;
			cs.Format(_T("Error loading language file - %s - \n\n%s"), csLanguage, theApp.m_Language.m_csLastError);

			MessageBox(cs, _T("DittoWTL"), MB_OK);
		}
	}

	CString toSavePath;
	m_ePath.GetWindowText(toSavePath);
	CString resolvedPath = CGetSetOptions::ResolvePath(toSavePath);

	bool bOpenNewDatabase = false;

	if (resolvedPath.IsEmpty() == FALSE)
	{
		if (FileExists(resolvedPath) == FALSE)
		{
			CString cs;
			cs.Format(_T("The database %s does not exist.\n\nCreate a new database?"), resolvedPath);

			if (MessageBox(cs, _T("DittoWTL"), MB_YESNO) == IDYES)
			{
				if (CreateDB(resolvedPath))
				{
					bOpenNewDatabase = true;
				}
				else
					MessageBox(_T("Error Creating Database"));
			}
			else
				return PSNRET_INVALID;
		}
		else
		{
			if (ValidDB(resolvedPath) == FALSE)
			{
				MessageBox(_T("Invalid Database"), _T("DittoWTL"), MB_OK);
				m_ePath.SetFocus();
				return PSNRET_INVALID;
			}
			else
			{
				bOpenNewDatabase = true;
			}
		}

		if (bOpenNewDatabase)
		{
			CGetSetOptions::SetDBPath(toSavePath);

			if (OpenDatabase(resolvedPath) == FALSE)
			{
				MessageBox(_T("Error Opening new database"), _T("DittoWTL"), MB_OK);
				m_ePath.SetFocus();
				return PSNRET_INVALID;
			}
			else
			{
				theApp.RefreshView();
			}
		}
	}

	CGetSetOptions::SetQuickPastePosition((int)m_popupPositionCombo.GetItemData(m_popupPositionCombo.GetCurSel()));

	if (m_LogFont.lfWeight != 0)
	{
		CGetSetOptions::SetFont(m_LogFont);
	}

	CString currentTheme = CGetSetOptions::GetTheme();

	CString csTheme = _T("");
	if (m_cbTheme.GetCurSel() >= 0)
	{
		if (m_cbTheme.GetItemData(m_cbTheme.GetCurSel()) == 1)
		{
			m_cbTheme.GetLBText(m_cbTheme.GetCurSel(), csTheme);
			CGetSetOptions::SetTheme(csTheme);
		}
		else
		{
			CGetSetOptions::SetTheme("");
		}
	}
	else
	{
		CGetSetOptions::SetTheme("");
	}

	if (currentTheme != csTheme)
	{
		m_pParent->m_themeChanged = TRUE;
	}

	return PSNRET_NOERROR;
}

BOOL COptionsGeneral::OnSetActive()
{
	return TRUE;
}

LRESULT COptionsGeneral::OnGetPath(WORD, WORD, HWND, BOOL&)
{
	OPENFILENAME FileName;
	TCHAR szFileName[400];
	TCHAR szDir[400];

	memset(&FileName, 0, sizeof(FileName));
	memset(szFileName, 0, sizeof(szFileName));
	memset(&szDir, 0, sizeof(szDir));
	FileName.lStructSize = sizeof(FileName);
	FileName.lpstrTitle = _T("Open Database");
	FileName.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
	FileName.nMaxFile = 400;
	FileName.lpstrFile = szFileName;
	FileName.lpstrInitialDir = szDir;
	FileName.lpstrFilter = _T("Ditto Databases (*.db; *.mdb)\0*.db;*.mdb\0\0");
	FileName.lpstrDefExt = _T("db");
	FileName.hwndOwner = m_hWnd;

	if (GetOpenFileName(&FileName) == 0)
		return 0;

	CString csPath(FileName.lpstrFile);
	if (FileExists(csPath))
	{
		if (ValidDB(csPath) == FALSE)
		{
			MessageBox(_T("Invalid Database"), _T("DittoWTL"), MB_OK);
			m_ePath.SetFocus();
		}
		else
		{
			m_ePath.SetWindowText(csPath);
		}
	}
	else
	{
		m_ePath.SetWindowText(csPath);
	}

	return 0;
}

LRESULT COptionsGeneral::OnButtonAbout(WORD, WORD, HWND, BOOL&)
{
	CDimWnd dim(this->GetParent().m_hWnd);

	CMultiLanguage Lang;

	CString csLanguage;
	m_cbLanguage.GetLBText(m_cbLanguage.GetCurSel(), csLanguage);

	Lang.SetOnlyGetHeader(true);

	if (Lang.LoadLanguageFile(csLanguage))
	{
		CString csMessage;

		csMessage.Format(_T("Language -  %s\n")
			_T("Version -   %d\n")
			_T("Author -   %s\n")
			_T("Notes -   %s"), csLanguage,
			Lang.GetVersion(),
			Lang.GetAuthor(),
			Lang.GetNotes());

		MessageBox(csMessage, _T("DittoWTL"), MB_OK);
	}
	else
	{
		CString csError;
		csError.Format(_T("Error loading language file - %s - reason = "), csLanguage, Lang.m_csLastError);

		MessageBox(csError, _T("DittoWTL"), MB_OK);
	}

	return 0;
}

LRESULT COptionsGeneral::OnBnClickedButtonAdvanced(WORD, WORD, HWND, BOOL&)
{
	CDimWnd dim(this->GetParent().m_hWnd);

	CAdvGeneral adv;
	adv.DoModal(m_hWnd);

	return 0;
}

LRESULT COptionsGeneral::OnCtlColor(UINT, WPARAM wParam, LPARAM, BOOL&)
{
	HDC hdc = (HDC)wParam;
	SetBkMode(hdc, TRANSPARENT);
	return (LRESULT)m_brush.m_hBrush;
}

LRESULT COptionsGeneral::OnBnClickedButtonTheme(WORD, WORD, HWND, BOOL&)
{
	CDimWnd dim(this->GetParent().m_hWnd);
	CDittoTheme theme;

	CString csTheme;
	m_cbTheme.GetLBText(m_cbTheme.GetCurSel(), csTheme);

	if (m_cbTheme.GetItemData(m_cbTheme.GetCurSel()) == 0)
		return 0;

	if (csTheme == DEFAULT_THEME)
		return 0;

	if (theme.Load(csTheme, true, false))
	{
		CString csMessage;

		csMessage.Format(_T("Theme -  %s\n")
			_T("Version -   %d\n")
			_T("Author -   %s\n")
			_T("Notes -   %s"), csTheme,
			theme.FileVersion(),
			theme.Author(),
			theme.Notes());

		MessageBox(csMessage, _T("DittoWTL"), MB_OK);
	}
	else
	{
		CString csError;
		csError.Format(_T("Error loading theme file - %s - reason = "), csTheme, theme.LastError());

		MessageBox(csError, _T("DittoWTL"), MB_OK);
	}

	return 0;
}

LRESULT COptionsGeneral::OnBnClickedButtonDefaultFault(WORD, WORD, HWND, BOOL&)
{
	memset(&m_LogFont, 0, sizeof(m_LogFont));

	m_LogFont.lfHeight = -13;
	m_LogFont.lfWeight = 400;
	m_LogFont.lfCharSet = 1;
	STRCPY(m_LogFont.lfFaceName, _T("Segoe UI"));

	CString cs;
	cs.Format(_T("Font - %s (%d)"), m_LogFont.lfFaceName, GetFontSize(m_hWnd, m_LogFont));
	m_btFont.SetWindowText(cs);

	return 0;
}

int COptionsGeneral::GetFontSize(HWND hWnd, const LOGFONT& lf)
{
	int nFontSize = -::MulDiv(lf.lfHeight, 72, 96);
	return nFontSize;
}

LRESULT COptionsGeneral::OnBnClickedButtonFont(WORD, WORD, HWND, BOOL&)
{
	CDPI dpi;

	m_LogFont.lfHeight = dpi.Scale(m_LogFont.lfHeight);

	CFontDialog dlg(&m_LogFont, (CF_TTONLY | CF_SCREENFONTS), 0, m_hWnd);
	if (dlg.DoModal() == IDOK)
	{
		memcpy(&m_LogFont, dlg.m_cf.lpLogFont, sizeof(LOGFONT));

		m_LogFont.lfHeight = dpi.UnScale(m_LogFont.lfHeight);

		CString cs;
		cs.Format(_T("Font - %s (%d)"), m_LogFont.lfFaceName, GetFontSize(m_hWnd, m_LogFont));
		m_btFont.SetWindowText(cs);
	}

	return 0;
}

LRESULT COptionsGeneral::OnEnChangePath(WORD, WORD, HWND, BOOL&)
{
	if (m_originalEnvVariables == _T(""))
	{
		m_envVarLink.GetWindowText(m_originalEnvVariables);
	}

	CString toSavePath;
	m_ePath.GetWindowText(toSavePath);

	if (toSavePath.Find(_T("%")) >= 0)
	{
		CString resolvedPath = CGetSetOptions::ResolvePath(toSavePath);
		m_envVarLink.SetWindowText(resolvedPath);
	}
	else
	{
		m_envVarLink.SetWindowText(m_originalEnvVariables);
	}

	return 0;
}

LRESULT COptionsGeneral::OnClickedMaximumEntries(WORD, WORD, HWND, BOOL&)
{
	if (m_btMaximumCheck.GetCheck() == BST_CHECKED)
	{
		m_eMaxSavedCopies.EnableWindow(TRUE);
	}
	else
	{
		m_eMaxSavedCopies.EnableWindow(FALSE);
	}

	return 0;
}

LRESULT COptionsGeneral::OnClickedExpireEntries(WORD, WORD, HWND, BOOL&)
{
	if (m_btExpire.GetCheck() == BST_CHECKED)
	{
		m_eExpireAfter.EnableWindow(TRUE);
	}
	else
	{
		m_eExpireAfter.EnableWindow(FALSE);
	}

	return 0;
}

void COptionsGeneral::FillThemes()
{
	CString csFile = CGetSetOptions::GetPath(PATH_THEMES);
	csFile += "*.xml";

	CString csTheme = CGetSetOptions::GetTheme();

	m_cbTheme.Clear();

	int windowsSettingIndex = m_cbTheme.AddString(theApp.m_Language.GetString("FollowWindowsTheme", "(Follow windows light/dark themes)"));
	m_cbTheme.SetItemData(windowsSettingIndex, 0);

	CFileFind find;
	BOOL bCont = find.FindFile(csFile);
	bool bSetCurSel = false;

	while (bCont)
	{
		bCont = find.FindNextFile();

		CDittoTheme theme;
		if (theme.Load(find.GetFileTitle(), true, false))
		{
			if (theme.FileVersion() >= 2 && theme.FileVersion() < 100)
			{
				int nIndex = m_cbTheme.AddString(find.GetFileTitle());
				m_cbTheme.SetItemData(nIndex, 1);

				if (find.GetFileTitle() == csTheme)
				{
					m_cbTheme.SetCurSel(nIndex);
					bSetCurSel = true;
				}
			}
		}
	}

	int nIndex = m_cbTheme.AddString(DEFAULT_THEME);
	m_cbTheme.SetItemData(nIndex, 1);
	if (csTheme == DEFAULT_THEME)
	{
		m_cbTheme.SetCurSel(nIndex);
		bSetCurSel = true;
	}

	if (bSetCurSel == false)
	{
		int count = m_cbTheme.GetCount();
		for (int i = 0; i < count; i++)
		{
			if (m_cbTheme.GetItemData(i) == 0)
			{
				m_cbTheme.SetCurSel(i);
				break;
			}
		}
	}
}
