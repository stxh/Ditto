// OptionsSheet.cpp : implementation file
//

#include "stdafx.h"
#include "CP_Main.h"
#include "OptionsSheet.h"
#include "OptionsKeyBoard.h"
#include "OptionsGeneral.h"
#include "OptionsQuickPaste.h"
#include "OptionsStats.h"
#include "OptionsTypes.h"
#include "About.h"
#include "OptionFriends.h"
#include "OptionsCopyBuffers.h"
#include "OptionsUtilities.h"
#include "Misc.h"
#include "QuickPasteKeyboard.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsSheet
COptionsSheet::COptionsSheet(LPCTSTR pszCaption, HWND hWndParent, UINT iSelectPage)
	:CPropertySheetImpl<COptionsSheet>(pszCaption, iSelectPage, hWndParent){
	m_themeChanged = FALSE;
	m_pKeyBoardOptions = NULL;
	m_pGeneralOptions = NULL;
	//m_pQuickPasteOptions = NULL;
	m_pCopyBuffers = NULL;
	m_pStats = NULL;
	m_pTypes = NULL;
	m_pAbout = NULL;
	m_pFriends = NULL;
	m_pCopyBuffers = NULL;
	m_pQuickPasteShortCuts = NULL;
	
	m_pUtilites = NULL;
	m_hWndParent = NULL;

	m_pGeneralOptions = new COptionsGeneral;
	m_pGeneralOptions->SetParentSheet(this);
	m_pKeyBoardOptions = new COptionsKeyBoard;
	m_pKeyBoardOptions->SetParentSheet(this);
	//m_pQuickPasteOptions = new COptionsQuickPaste;
	m_pQuickPasteShortCuts = new CQuickPasteKeyboard;

	m_pCopyBuffers = new COptionsCopyBuffers;
	m_pStats = new COptionsStats;
	m_pTypes = new COptionsTypes;
	m_pAbout = new CAbout;

	AddPage(*m_pGeneralOptions);
	AddPage(*m_pTypes);
	AddPage(*m_pKeyBoardOptions);
	AddPage(*m_pCopyBuffers);
	//AddPage(m_pQuickPasteOptions);
	AddPage(*m_pQuickPasteShortCuts);
	if(CGetSetOptions::GetAllowFriends())
	{
		m_pFriends = new COptionFriends;
		AddPage(*m_pFriends);
	}
	AddPage(*m_pStats);
	AddPage(*m_pAbout);

	
}

COptionsSheet::~COptionsSheet()
{
	delete m_pGeneralOptions;
	delete m_pKeyBoardOptions;
	//delete m_pQuickPasteOptions;
	delete m_pCopyBuffers;
	delete m_pStats;
	delete m_pTypes;
	delete m_pAbout;	
	delete m_pFriends;
	delete m_pUtilites;
	delete m_pQuickPasteShortCuts;	
}

void COptionsSheet::SetNotifyWnd(HWND hWnd)
{
	m_hWndParent = hWnd;
}

BOOL COptionsSheet::OnInitDialog(HWND hWndFocus, LPARAM lParam) 
{
	BOOL bResult = TRUE;

	HICON b = (HICON)LoadImage(theApp.GetModuleInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 64, 64, LR_SHARED);
	SetIcon(b, TRUE);

	SetWindowText(_T("Options"));

	theApp.m_Language.UpdateOptionsSheet(m_hWnd);

	::ShowWindow(GetDlgItem(ID_APPLY_NOW), SW_HIDE);

	return bResult;
}

LRESULT COptionsSheet::OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT COptionsSheet::OnNcDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	bHandled = FALSE;
	::PostMessage(m_hWndParent, WM_OPTIONS_CLOSED, m_themeChanged, 0);
	return 0;
}
