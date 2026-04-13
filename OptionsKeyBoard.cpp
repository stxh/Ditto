#include "stdafx.h"
#include "CP_Main.h"
#include "OptionsKeyBoard.h"

COptionsKeyBoard::COptionsKeyBoard()
{
	m_csTitle = theApp.m_Language.GetString("KeyboardShortcutsTitle", "Keyboard Shortcuts");
	m_psp.pszTitle = m_csTitle;
	m_psp.dwFlags |= PSP_USETITLE;
	m_pParent = NULL;
}

LRESULT COptionsKeyBoard::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&) 
{
	m_btSendPaste.Attach(GetDlgItem(IDC_CHECK_SEND_PASTE));
	m_UseUiGroupForLastTen.Attach(GetDlgItem(IDC_CHECK_USE_UI_GROUP_LAST_10));
	m_btMoveClipOnGlobal10.Attach(GetDlgItem(IDC_CHECK_MOVE_CLIPS_ON_PASTE));
	m_Nine.Attach(GetDlgItem(IDC_HOTKEY9));
	m_Eight.Attach(GetDlgItem(IDC_HOTKEY8));
	m_Seven.Attach(GetDlgItem(IDC_HOTKEY7));
	m_Six.Attach(GetDlgItem(IDC_HOTKEY6));
	m_Five.Attach(GetDlgItem(IDC_HOTKEY5));
	m_Four.Attach(GetDlgItem(IDC_HOTKEY4));
	m_Three.Attach(GetDlgItem(IDC_HOTKEY3));
	m_Two.Attach(GetDlgItem(IDC_HOTKEY2));
	m_Ten.Attach(GetDlgItem(IDC_HOTKEY10));
	m_One.Attach(GetDlgItem(IDC_HOTKEY1));
	m_HotKey.Attach(GetDlgItem(IDC_HOTKEY));
	m_HotKey2.Attach(GetDlgItem(IDC_HOTKEY_ACTIVATE_2));
	m_HotKey3.Attach(GetDlgItem(IDC_HOTKEY_ACTIVATE_3));
	m_TextOnlyKey.Attach(GetDlgItem(IDC_HOTKEY_TEXT_ONLY));
	m_saveClipboardHotKey.Attach(GetDlgItem(IDC_HOTKEY_SAVE_CLIPBOARD));
	m_copyAndSaveClipboardCtrl.Attach(GetDlgItem(IDC_HOTKEY_COPYSAVECLIPBOARD));

	m_CustomeKeysHelp.SubclassWindow(GetDlgItem(IDC_STATIC_CUSTOM_KEYS));
	m_CustomeKeysHelp.SetHyperLink(_T("https://github.com/sabrogden/Ditto/wiki/Custom-Key-Strokes"));

	m_pParent = (COptionsSheet *)GetParent();

	theApp.m_pDittoHotKey->CopyToCtrl(m_HotKey, m_hWnd, IDC_CHECK_WIN_DITTO);
	theApp.m_pDittoHotKey2->CopyToCtrl(m_HotKey2, m_hWnd, IDC_CHECK_WIN_DITTO2);
	theApp.m_pDittoHotKey3->CopyToCtrl(m_HotKey3, m_hWnd, IDC_CHECK_WIN_DITTO3);

	theApp.m_pPosOne->CopyToCtrl(m_One, m_hWnd, IDC_CHECK_WIN1);
	theApp.m_pPosTwo->CopyToCtrl(m_Two, m_hWnd, IDC_CHECK_WIN2);
	theApp.m_pPosThree->CopyToCtrl(m_Three, m_hWnd, IDC_CHECK_WIN3);
	theApp.m_pPosFour->CopyToCtrl(m_Four, m_hWnd, IDC_CHECK_WIN4);
	theApp.m_pPosFive->CopyToCtrl(m_Five, m_hWnd, IDC_CHECK_WIN5);
	theApp.m_pPosSix->CopyToCtrl(m_Six, m_hWnd, IDC_CHECK_WIN6);
	theApp.m_pPosSeven->CopyToCtrl(m_Seven, m_hWnd, IDC_CHECK_WIN7);
	theApp.m_pPosEight->CopyToCtrl(m_Eight, m_hWnd, IDC_CHECK_WIN8);
	theApp.m_pPosNine->CopyToCtrl(m_Nine, m_hWnd, IDC_CHECK_WIN9);
	theApp.m_pPosTen->CopyToCtrl(m_Ten, m_hWnd, IDC_CHECK_WIN10);
	theApp.m_pTextOnlyPaste->CopyToCtrl(m_TextOnlyKey, m_hWnd, IDC_CHECK_WIN_TEXT_ONLY);
	theApp.m_pSaveClipboard->CopyToCtrl(m_saveClipboardHotKey, m_hWnd, IDC_CHECK_WIN_SAVE_CLIPBOARD);
	theApp.m_pCopyAndSaveClipboard->CopyToCtrl(m_copyAndSaveClipboardCtrl, m_hWnd, IDC_CHECK_WIN_COPY_SAVE_CLIPBOARD);


	//Unregister hotkeys and Reregister them on cancel or ok
	g_HotKeys.UnregisterAll();

	m_btSendPaste.SetCheck(CGetSetOptions::m_bSendPasteOnFirstTenHotKeys);
	m_UseUiGroupForLastTen.SetCheck(CGetSetOptions::GetUseUISelectedGroupForLastTenCopies());

	m_btMoveClipOnGlobal10.SetCheck(CGetSetOptions::GetMoveClipsOnGlobal10());

	m_HotKey.SetFocus();

	theApp.m_Language.UpdateOptionShortcuts(this);	
		
	return 0;
}

int COptionsKeyBoard::OnWizardNext() 
{
	return 0;
}

BOOL COptionsKeyBoard::OnWizardFinish() 
{
	return TRUE;
}

int COptionsKeyBoard::OnApply()
{
	CGetSetOptions::SetSendPasteOnFirstTenHotKeys(m_btSendPaste.GetCheck());
	CGetSetOptions::SetMoveClipsOnGlobal10(m_btMoveClipOnGlobal10.GetCheck());
	CGetSetOptions::SetUseUISelectedGroupForLastTenCopies(m_UseUiGroupForLastTen.GetCheck());
					
	INT_PTR x,y;
	CString str;
	ARRAY keys;
	
	g_HotKeys.GetKeys( keys ); // save old keys just in case new ones are invalid
	
	theApp.m_pDittoHotKey->CopyFromCtrl(m_HotKey, m_hWnd, IDC_CHECK_WIN_DITTO);
	theApp.m_pDittoHotKey2->CopyFromCtrl(m_HotKey2, m_hWnd, IDC_CHECK_WIN_DITTO2);
	theApp.m_pDittoHotKey3->CopyFromCtrl(m_HotKey3, m_hWnd, IDC_CHECK_WIN_DITTO3);
	
	theApp.m_pPosOne->CopyFromCtrl(m_One, m_hWnd, IDC_CHECK_WIN1);
	theApp.m_pPosTwo->CopyFromCtrl(m_Two, m_hWnd, IDC_CHECK_WIN2);
	theApp.m_pPosThree->CopyFromCtrl(m_Three, m_hWnd, IDC_CHECK_WIN3);
	theApp.m_pPosFour->CopyFromCtrl(m_Four, m_hWnd, IDC_CHECK_WIN4);
	theApp.m_pPosFive->CopyFromCtrl(m_Five, m_hWnd, IDC_CHECK_WIN5);
	theApp.m_pPosSix->CopyFromCtrl(m_Six, m_hWnd, IDC_CHECK_WIN6);
	theApp.m_pPosSeven->CopyFromCtrl(m_Seven, m_hWnd, IDC_CHECK_WIN7);
	theApp.m_pPosEight->CopyFromCtrl(m_Eight, m_hWnd, IDC_CHECK_WIN8);
	theApp.m_pPosNine->CopyFromCtrl(m_Nine, m_hWnd, IDC_CHECK_WIN9);
	theApp.m_pPosTen->CopyFromCtrl(m_Ten, m_hWnd, IDC_CHECK_WIN10);
	theApp.m_pTextOnlyPaste->CopyFromCtrl(m_TextOnlyKey, m_hWnd, IDC_CHECK_WIN_TEXT_ONLY);
	theApp.m_pSaveClipboard->CopyFromCtrl(m_saveClipboardHotKey, m_hWnd, IDC_CHECK_WIN_SAVE_CLIPBOARD);
	theApp.m_pCopyAndSaveClipboard->CopyFromCtrl(m_copyAndSaveClipboardCtrl, m_hWnd, IDC_CHECK_WIN_COPY_SAVE_CLIPBOARD);

	ARRAY NewKeys;
	g_HotKeys.GetKeys(NewKeys);
	
	if(g_HotKeys.FindFirstConflict(NewKeys, &x, &y))
	{
		str =  g_HotKeys.ElementAt(x)->GetName();
		str += " and ";
		str += g_HotKeys.ElementAt(y)->GetName();
		str += " cannot be the same.";
		MessageBox(str);
		g_HotKeys.SetKeys(keys); // restore the original values
		return FALSE;
	}
	
	g_HotKeys.SaveAllKeys();
	g_HotKeys.RegisterAll(true);
	
	return PSNRET_NOERROR;
}

void COptionsKeyBoard::OnReset() 
{
	g_HotKeys.RegisterAll(true);
}
