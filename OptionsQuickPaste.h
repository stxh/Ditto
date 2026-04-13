#pragma once

#include "NumberEdit.h"
#include "OptionsSheet.h"
#include <atlbase.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>

class COptionsQuickPaste : public CPropertyPageImpl<COptionsQuickPaste>
{
public:
	enum { IDD = IDD_OPTIONS_QUICK_PASTE };

	COptionsQuickPaste();
	~COptionsQuickPaste();

	BEGIN_MSG_MAP(COptionsQuickPaste)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_BUTTON_FONT, OnButtonFont)
		COMMAND_ID_HANDLER(IDC_BUTTON_DEFAULT_FAULT, OnButtonDefaultFault)
		COMMAND_ID_HANDLER(IDC_BUTTON_THEME, OnBnClickedButtonTheme)
		COMMAND_ID_HANDLER(IDC_BUTTON_DIFF_BROWSE, OnBnClickedButtonDiffBrowse)
		CHAIN_MSG_MAP(CPropertyPageImpl<COptionsQuickPaste>)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnButtonFont(WORD, WORD, HWND, BOOL&);
	LRESULT OnButtonDefaultFault(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonTheme(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonDiffBrowse(WORD, WORD, HWND, BOOL&);
	
	int OnApply() override;

private:
	COptionsSheet *m_pParent;
	CFont m_Font;
	LOGFONT m_LogFont;
	CString m_csTitle;

	void FillThemes();

	CButton m_EnsureEntireWindowVisible;
	CButton m_ShowAllInMainList;
	CButton m_FindAsYouType;
	CButton m_btDrawRTF;
	CButton m_btShowThumbnails;
	CButton m_btDefaultButton;
	CButton m_btFont;
	CButton m_btShowText;
	CNumberEdit m_eLinesPerRow;
	CNumberEdit m_eTransparencyPercent;
	CButton m_btEnableTransparency;
	CButton m_btDescShowLeadingWhiteSpace;
	CButton m_PromptForDelete;
	CButton m_elevatedPrivileges;
	CButton m_showInTaskBar;
	CComboBox m_cbTheme;
	CButton m_alwaysShowScrollBar;
	CEdit m_diffPathEditBox;
};
