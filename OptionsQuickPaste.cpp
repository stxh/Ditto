#include "stdafx.h"
#include "CP_Main.h"
#include "OptionsQuickPaste.h"
#include ".\optionsquickpaste.h"

#define DEFAULT_THEME _T("(Default)")

COptionsQuickPaste::COptionsQuickPaste()
{
	m_csTitle = theApp.m_Language.GetString("QuickPasteTitle", "Quick Paste");
	m_psp.pszTitle = m_csTitle;
	m_psp.dwFlags |= PSP_USETITLE;
	m_pParent = NULL;
	memset(&m_LogFont, 0, sizeof(LOGFONT));
}

COptionsQuickPaste::~COptionsQuickPaste()
{
	if (!m_Font.IsNull())
		m_Font.DeleteObject();
}

LRESULT COptionsQuickPaste::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&) 
{
	m_EnsureEntireWindowVisible.Attach(GetDlgItem(IDC_CHECK_ENTIRE_WINDOW_IS_VISIBLE));
	m_ShowAllInMainList.Attach(GetDlgItem(IDC_CHECK_SHOW_ALL_IN_MAIN_LIST));
	m_FindAsYouType.Attach(GetDlgItem(IDC_CHECK_FIND_AS_YOU_TYPE));
	m_btDrawRTF.Attach(GetDlgItem(IDC_DRAW_RTF));
	m_btShowThumbnails.Attach(GetDlgItem(IDC_SHOW_THUMBNAILS));
	m_btDefaultButton.Attach(GetDlgItem(IDC_BUTTON_DEFAULT_FAULT));
	m_btFont.Attach(GetDlgItem(IDC_BUTTON_FONT));
	m_btShowText.Attach(GetDlgItem(IDC_SHOW_TEXT_FOR_FIRST_TEN_HOT_KEYS));
	m_eLinesPerRow.SubclassWindow(GetDlgItem(IDC_LINES_ROW));
	m_eTransparencyPercent.SubclassWindow(GetDlgItem(IDC_TRANS_PERC));
	m_btEnableTransparency.Attach(GetDlgItem(IDC_TRANSPARENCY));
	m_btDescShowLeadingWhiteSpace.Attach(GetDlgItem(IDC_DESC_SHOW_LEADING_WHITESPACE));
	m_PromptForDelete.Attach(GetDlgItem(IDC_CHECK_PROMPT_DELETE_CLIP));
	m_cbTheme.Attach(GetDlgItem(IDC_COMBO_THEME));
	m_alwaysShowScrollBar.Attach(GetDlgItem(IDC_CHECK_SHOW_SCROLL_BAR));
	m_elevatedPrivileges.Attach(GetDlgItem(IDC_CHECK_ELEVATE_PRIVILEGES));
	m_showInTaskBar.Attach(GetDlgItem(IDC_CHECK_SHOW_IN_TASKBAR));
	m_diffPathEditBox.Attach(GetDlgItem(IDC_EDIT_DIFF_PATH));

	m_btEnableTransparency.SetCheck(CGetSetOptions::GetEnableTransparency());
	m_eTransparencyPercent.SetNumber(CGetSetOptions::GetTransparencyPercent());
	m_eLinesPerRow.SetNumber(CGetSetOptions::GetLinesPerRow());
	m_alwaysShowScrollBar.SetCheck(CGetSetOptions::GetShowScrollBar());
	m_btShowThumbnails.SetCheck(CGetSetOptions::m_bDrawThumbnail);
	m_btDrawRTF.SetCheck(CGetSetOptions::m_bDrawRTF);

	m_EnsureEntireWindowVisible.SetCheck(CGetSetOptions::m_bEnsureEntireWindowCanBeSeen);
	m_ShowAllInMainList.SetCheck(CGetSetOptions::m_bShowAllClipsInMainList);
	m_FindAsYouType.SetCheck(CGetSetOptions::m_bFindAsYouType);

	if(CGetSetOptions::GetQuickPastePosition() == POS_AT_CARET)
		CheckDlgButton(IDC_AT_CARET, BST_CHECKED);
	else if(CGetSetOptions::GetQuickPastePosition() == POS_AT_CURSOR)
		CheckDlgButton(IDC_AT_CURSOR, BST_CHECKED);
	else if(CGetSetOptions::GetQuickPastePosition() == POS_AT_PREVIOUS)
		CheckDlgButton(IDC_AT_PREVIOUS, BST_CHECKED);

	m_btDescShowLeadingWhiteSpace.SetCheck(CGetSetOptions::m_bDescShowLeadingWhiteSpace);

	m_btShowText.SetCheck(CGetSetOptions::GetShowTextForFirstTenHotKeys());
	m_PromptForDelete.SetCheck(CGetSetOptions::GetPromptWhenDeletingClips());
	m_elevatedPrivileges.SetCheck(CGetSetOptions::GetPasteAsAdmin());
	m_showInTaskBar.SetCheck(CGetSetOptions::GetShowInTaskBar());

	if(CGetSetOptions::GetFont(m_LogFont))
	{		
		m_Font.CreateFontIndirect(&m_LogFont);
		m_btFont.SetFont(m_Font);
	}
	else
	{
		WTL::CFont font(m_btFont.GetFont());
		font.GetLogFont(&m_LogFont);
	}

	m_diffPathEditBox.SetWindowText(CGetSetOptions::GetDiffApp());

	CString cs;
	cs.Format(_T("Font - %s"), m_LogFont.lfFaceName);
	m_btFont.SetWindowText(cs);

	if (CGetSetOptions::GetShowIfClipWasPasted())
		CheckDlgButton(IDC_CHECK_SHOW_CLIP_WAS_PASTED, BST_CHECKED);

	FillThemes();

	theApp.m_Language.UpdateOptionQuickPaste(m_hWnd);
		
	return 0;
}

int COptionsQuickPaste::OnApply() 
{
	CGetSetOptions::SetEnableTransparency(m_btEnableTransparency.GetCheck());
	CGetSetOptions::SetTransparencyPercent(m_eTransparencyPercent.GetNumber());
	CGetSetOptions::SetLinesPerRow(m_eLinesPerRow.GetNumber());
	CGetSetOptions::SetShowScrollBar(m_alwaysShowScrollBar.GetCheck());
	
	if(IsDlgButtonChecked(IDC_AT_CARET))
		CGetSetOptions::SetQuickPastePosition(POS_AT_CARET);
	else if(IsDlgButtonChecked(IDC_AT_CURSOR))
		CGetSetOptions::SetQuickPastePosition(POS_AT_CURSOR);
	else if(IsDlgButtonChecked(IDC_AT_PREVIOUS))
		CGetSetOptions::SetQuickPastePosition(POS_AT_PREVIOUS);

	CGetSetOptions::SetDescShowLeadingWhiteSpace(m_btDescShowLeadingWhiteSpace.GetCheck());
	CGetSetOptions::SetShowTextForFirstTenHotKeys(m_btShowText.GetCheck());
	CGetSetOptions::SetDrawThumbnail(m_btShowThumbnails.GetCheck());
	CGetSetOptions::SetDrawRTF(m_btDrawRTF.GetCheck());
	CGetSetOptions::SetEnsureEntireWindowCanBeSeen(m_EnsureEntireWindowVisible.GetCheck());
	CGetSetOptions::SetShowAllClipsInMainList(m_ShowAllInMainList.GetCheck());
	CGetSetOptions::SetFindAsYouType(m_FindAsYouType.GetCheck());
	CGetSetOptions::SetPromptWhenDeletingClips(m_PromptForDelete.GetCheck());
	CGetSetOptions::SetPasteAsAdmin(m_elevatedPrivileges.GetCheck());

	BOOL prevValue = CGetSetOptions::GetShowInTaskBar();
	CGetSetOptions::SetShowInTaskBar(m_showInTaskBar.GetCheck());
	if(CGetSetOptions::GetShowInTaskBar() != prevValue)
	{
		theApp.RefreshShowInTaskBar();
	}
	
	if(m_LogFont.lfWeight != 0)
	{
		CGetSetOptions::SetFont(m_LogFont);
	}

	CString currentTheme = CGetSetOptions::GetTheme();

	CString csTheme;
	if(m_cbTheme.GetCurSel() >= 0)
	{
		m_cbTheme.GetLBText(m_cbTheme.GetCurSel(), csTheme);
		if (csTheme == DEFAULT_THEME)
		{
			CGetSetOptions::SetTheme("");
			csTheme = _T("");
		}
		else
			CGetSetOptions::SetTheme(csTheme);
	}
	else
	{
		CGetSetOptions::SetTheme("");
	}

	if (currentTheme != csTheme)
	{
		m_pParent->m_themeChanged = TRUE;
	}

	CString diffPath;
	m_diffPathEditBox.GetWindowText(diffPath);
	CGetSetOptions::SetDiffApp(diffPath);

	if (IsDlgButtonChecked(IDC_CHECK_SHOW_CLIP_WAS_PASTED))
		CGetSetOptions::SetShowIfClipWasPasted(TRUE);
	else
		CGetSetOptions::SetShowIfClipWasPasted(FALSE);
	
	return PSNRET_NOERROR;
}

LRESULT COptionsQuickPaste::OnButtonFont(WORD, WORD, HWND, BOOL&) 
{
	CFontDialog dlg(&m_LogFont, (CF_TTONLY | CF_SCREENFONTS), 0, m_hWnd);
	if(dlg.DoModal() == IDOK)
	{	
		if (!m_Font.IsNull())
			m_Font.DeleteObject();

		memcpy(&m_LogFont, dlg.m_cf.lpLogFont, sizeof(LOGFONT));		

		m_Font.CreateFontIndirect(&m_LogFont);

		m_btFont.SetFont(m_Font);

		CString cs;
		cs.Format(_T("Font - %s"), m_LogFont.lfFaceName);
		m_btFont.SetWindowText(cs);
	}
	return 0;
}

LRESULT COptionsQuickPaste::OnButtonDefaultFault(WORD, WORD, HWND, BOOL&) 
{
	HFONT hFont = m_btDefaultButton.GetFont();
	if (hFont != NULL)
		::GetObject(hFont, sizeof(LOGFONT), &m_LogFont);

	memset(&m_LogFont, 0, sizeof(m_LogFont));

	m_LogFont.lfHeight = -10;
	m_LogFont.lfWeight = 400;
	m_LogFont.lfCharSet = 1;
	STRCPY(m_LogFont.lfFaceName, _T("Segoe UI"));

	if (!m_Font.IsNull())
		m_Font.DeleteObject();
	m_Font.CreateFontIndirect(&m_LogFont);

	m_btFont.SetFont(m_Font);

	CString cs;
	cs.Format(_T("Font - %s"), m_LogFont.lfFaceName);
	m_btFont.SetWindowText(cs);
	return 0;
}	

void COptionsQuickPaste::FillThemes()
{
	CString csFile = CGetSetOptions::GetPath(PATH_THEMES);
	csFile += "*.xml";

	CString csTheme = CGetSetOptions::GetTheme();

	CFileFind find;
	BOOL bCont = find.FindFile(csFile);
	bool bSetCurSel = false;

	while(bCont)
	{
		bCont = find.FindNextFile();

		CDittoTheme theme;
		if (theme.Load(find.GetFileTitle(), true, false))
		{
			if (theme.FileVersion() >= 2 && theme.FileVersion() < 100)
			{
				int nIndex = m_cbTheme.AddString(find.GetFileTitle());

				if (find.GetFileTitle() == csTheme)
				{
					m_cbTheme.SetCurSel(nIndex);
					bSetCurSel = true;
				}
			}
		}
	}

	int nIndex = m_cbTheme.AddString(DEFAULT_THEME);
	if(bSetCurSel == false)
	{
		m_cbTheme.SetCurSel(nIndex);
	}
}

LRESULT COptionsQuickPaste::OnBnClickedButtonTheme(WORD, WORD, HWND, BOOL&)
{
	CDittoTheme theme;

	CString csTheme;
	m_cbTheme.GetLBText(m_cbTheme.GetCurSel(), csTheme);

	if(csTheme == DEFAULT_THEME)
		return 0;
	
	if(theme.Load(csTheme, true, false))
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


LRESULT COptionsQuickPaste::OnBnClickedButtonDiffBrowse(WORD, WORD, HWND, BOOL&)
{
	OPENFILENAME FileName;
	TCHAR szFileName[400];
	TCHAR szDir[400];

	memset(&FileName, 0, sizeof(FileName));
	memset(szFileName, 0, sizeof(szFileName));
	memset(&szDir, 0, sizeof(szDir));
	FileName.lStructSize = sizeof(FileName);
	FileName.lpstrTitle = _T("Diff Application");
	FileName.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
	FileName.nMaxFile = 400;
	FileName.lpstrFile = szFileName;
	FileName.lpstrInitialDir = szDir;
	FileName.lpstrFilter = _T("*.exe");
	FileName.lpstrDefExt = _T("");
	FileName.hwndOwner = m_hWnd;

	if(GetOpenFileName(&FileName) == 0)
		return 0;

	CString csPath(FileName.lpstrFile);

	m_diffPathEditBox.SetWindowText(csPath);
	return 0;
}
