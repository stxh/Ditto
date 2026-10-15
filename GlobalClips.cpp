#include "stdafx.h"
#include "CP_Main.h"
#include "GlobalClips.h"
#include "CopyProperties.h"

LRESULT GlobalClips::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_List.Attach(GetDlgItem(IDC_LIST2));

	theApp.m_Language.UpdateGlobalHotKeys(m_hWnd);

	m_Resize.SetParent(m_hWnd);
	m_Resize.AddControl(IDC_LIST2, DR_SizeHeight | DR_SizeWidth);
	m_Resize.AddControl(IDCANCEL, DR_MoveTop | DR_MoveLeft);

	InitListCtrlCols();

	LoadItems();

	// TRUE: no focus set here, let the dialog manager select the first control.
	return TRUE;
}

void GlobalClips::LoadItems()
{
	m_List.DeleteAllItems();

	LVITEM lvi;
	CString strItem;
	int count = (int)g_HotKeys.GetCount();

	int row = 0;
	for (int i = 0; i < count; i++)
	{
		CHotKey *pHotKey = g_HotKeys[i];

		if(pHotKey->m_Key <= 0)
		{
			continue;
		}

		lvi.mask =  LVIF_TEXT;
		lvi.iItem = row;

		strItem = pHotKey->m_Name;

		if(pHotKey->m_clipId > 0)
		{
			strItem = pHotKey->m_description;

			if(pHotKey->m_hkType == CHotKey::PASTE_OPEN_CLIP)
			{
				strItem.Insert(0, theApp.m_Language.GetGlobalHotKeyString("(Clip)", "(Clip) "));
			}
			else if(pHotKey->m_hkType == CHotKey::MOVE_TO_GROUP)
			{
				strItem.Insert(0, _T("(Move To Group) "));
			}
		}

		lvi.iSubItem = 0;
		lvi.pszText = (LPTSTR)(LPCTSTR)(strItem);
		m_List.InsertItem(&lvi);

		strItem = pHotKey->GetHotKeyDisplay();
		m_List.SetItemText(row, 1, strItem);

		strItem = theApp.m_Language.GetGlobalHotKeyString("Error", "Error");
		if(pHotKey->IsRegistered())
		{
			strItem = theApp.m_Language.GetGlobalHotKeyString("Yes", "Yes");
		}

		m_List.SetItemText(row, 2, strItem);

		m_List.SetItemData(row, pHotKey->m_globalId);

		row++;
	}
}

void GlobalClips::InitListCtrlCols()
{
	m_List.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
	
	m_List.InsertColumn(0, theApp.m_Language.GetGlobalHotKeyString("Description", "Description"), LVCFMT_LEFT, 200, 0);
	m_List.InsertColumn(1, theApp.m_Language.GetGlobalHotKeyString("HotKey", "Hot Key"), LVCFMT_LEFT, 100, 1);
	m_List.InsertColumn(2, theApp.m_Language.GetGlobalHotKeyString("Registered", "Registered"), LVCFMT_LEFT, 100, 2);
}

void GlobalClips::SetNotifyWnd(HWND hWnd)
{
	m_hWndParent = hWnd;
}

LRESULT GlobalClips::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DestroyWindow();
	return 0;
}

LRESULT GlobalClips::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	int cx = LOWORD(lParam);
	int cy = HIWORD(lParam);
	m_Resize.MoveControls(CSize(cx, cy));
	return 0;
}

LRESULT GlobalClips::OnNcDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	::PostMessage(m_hWndParent, WM_GLOBAL_CLIPS_CLOSED, 0, 0);
	return 0;
}
