#if !defined(AFX_OPTIONSGENERAL_H__A13ABBF6_7636_4426_9A31_0189D4CA8F2F__INCLUDED_)
#define AFX_OPTIONSGENERAL_H__A13ABBF6_7636_4426_9A31_0189D4CA8F2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "CP_Main.h"
#include "OptionsSheet.h"
#include "NumberEdit.h"
#include <atlctrlx.h>
#include <atlbase.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>

class COptionsGeneral : public CPropertyPageImpl<COptionsGeneral>
{
public:
	enum { IDD = IDD_OPTIONS_GENERAL };

	COptionsGeneral();
	~COptionsGeneral();

	BEGIN_MSG_MAP(COptionsGeneral)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
		COMMAND_ID_HANDLER(IDC_GET_PATH, OnGetPath)
		COMMAND_ID_HANDLER(IDC_BUTTON_ABOUT, OnButtonAbout)
		COMMAND_ID_HANDLER(IDC_BUTTON_ADVANCED, OnBnClickedButtonAdvanced)
		COMMAND_ID_HANDLER(IDC_BUTTON_THEME, OnBnClickedButtonTheme)
		COMMAND_ID_HANDLER(IDC_BUTTON_DEFAULT_FAULT, OnBnClickedButtonDefaultFault)
		COMMAND_ID_HANDLER(IDC_BUTTON_FONT, OnBnClickedButtonFont)
		COMMAND_ID_HANDLER(IDC_PATH, OnEnChangePath)
		COMMAND_ID_HANDLER(IDC_MAXIMUM, OnClickedMaximumEntries)
		COMMAND_ID_HANDLER(IDC_EXPIRE, OnClickedExpireEntries)
		CHAIN_MSG_MAP(CPropertyPageImpl<COptionsGeneral>)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnGetPath(WORD, WORD, HWND, BOOL&);
	LRESULT OnButtonAbout(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonAdvanced(WORD, WORD, HWND, BOOL&);
	LRESULT OnCtlColor(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnBnClickedButtonTheme(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonDefaultFault(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonFont(WORD, WORD, HWND, BOOL&);
	LRESULT OnEnChangePath(WORD, WORD, HWND, BOOL&);
	LRESULT OnClickedMaximumEntries(WORD, WORD, HWND, BOOL&);
	LRESULT OnClickedExpireEntries(WORD, WORD, HWND, BOOL&);

	int OnApply();
	BOOL OnSetActive();

	void SetParentSheet(COptionsSheet* pParent)	{ m_pParent = pParent; }

private:
	void FillThemes();
	void FillLanguages();
	int GetFontSize(HWND hWnd, const LOGFONT& lf);

	COptionsSheet* m_pParent;
	CString m_csTitle;
	CBrush m_brush;
	LOGFONT m_LogFont;
	CFont m_envVarFont;
	CString m_originalEnvVariables;

	CComboBox m_cbLanguage;
	CEdit m_MaxClipSize;
	CButton m_btSendPasteMessage;
	CButton m_btHideDittoOnHotKey;
	CNumberEdit m_DescTextSize;
	CEdit m_ePath;
	CNumberEdit m_eExpireAfter;
	CNumberEdit m_eMaxSavedCopies;
	CButton m_btMaximumCheck;
	CButton m_btExpire;
	CButton m_btShowIconInSysTray;
	CButton m_btRunOnStartup;
	CButton m_btSaveMultiPaste;
	CString m_csPlaySound;
	CEdit m_ClipSeparator;
	CEdit m_copyAppInclude;
	CEdit m_copyAppExclude;
	CHyperLink m_envVarLink;
	CComboBox m_cbTheme;
	CButton m_btFont;
	CButton m_btDefaultButton;
	CComboBox m_popupPositionCombo;
};

#endif
