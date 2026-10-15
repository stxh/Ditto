#include "stdafx.h"
#include "CP_Main.h"
#include "QuickPasteKeyboard.h"
#include "ActionEnums.h"
#include "HotKeys.h"

static BOOL bDummy = FALSE;

CQuickPasteKeyboard::CQuickPasteKeyboard()
{
	m_csTitle = theApp.m_Language.GetString("QuickPasteKeyboardTitle", "Quick Paste Keyboard");
	m_psp.pszTitle = m_csTitle;
	m_psp.dwFlags |= PSP_USETITLE;
}

CQuickPasteKeyboard::~CQuickPasteKeyboard()
{
}

LRESULT CQuickPasteKeyboard::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
	m_list.Attach(GetDlgItem(IDC_LIST1));
	m_hotKey1.SubclassWindow(GetDlgItem(IDC_HOTKEY1));
	m_hotKey2.SubclassWindow(GetDlgItem(IDC_HOTKEY2));
	m_assignedCombo.Attach(GetDlgItem(IDC_COMBO_ALL_ASSIGNED));
	m_mouseType1.Attach(GetDlgItem(IDC_MOUSE_1));
	m_mouseType2.Attach(GetDlgItem(IDC_MOUSE_2));

	InitListCtrlCols();
	LoadItems();

	int pos = m_mouseType1.AddString(_T("Click"));
	m_mouseType1.SetItemData(pos, VK_MOUSE_CLICK);

	pos = m_mouseType1.AddString(_T("Double Click"));
	m_mouseType1.SetItemData(pos, VK_MOUSE_DOUBLE_CLICK);

	pos = m_mouseType1.AddString(_T("Right Click"));
	m_mouseType1.SetItemData(pos, VK_MOUSE_RIGHT_CLICK);

	pos = m_mouseType1.AddString(_T("Middle Click"));
	m_mouseType1.SetItemData(pos, VK_MOUSE_MIDDLE_CLICK);

	pos = m_mouseType2.AddString(_T("Click"));
	m_mouseType2.SetItemData(pos, VK_MOUSE_CLICK);

	pos = m_mouseType2.AddString(_T("Double Click"));
	m_mouseType2.SetItemData(pos, VK_MOUSE_DOUBLE_CLICK);

	pos = m_mouseType2.AddString(_T("Right Click"));
	m_mouseType2.SetItemData(pos, VK_MOUSE_RIGHT_CLICK);

	pos = m_mouseType2.AddString(_T("Middle Click"));
	m_mouseType2.SetItemData(pos, VK_MOUSE_MIDDLE_CLICK);

	m_mouseType1.SetCurSel(0);
	m_mouseType2.SetCurSel(0);

	theApp.m_Language.UpdateOptionQuickPasteKeyboard(m_hWnd);

	return 0;
}

void CQuickPasteKeyboard::InitListCtrlCols()
{
	m_list.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

	m_list.InsertColumn(0, theApp.m_Language.GetString("QPHotKey", "Hot Key"), LVCFMT_LEFT, 150);
	m_list.InsertColumn(1, theApp.m_Language.GetString("QPCommand", "Command"), LVCFMT_LEFT, 350);
}

int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListViewCtrl* pListCtrl = (CListViewCtrl*)lParamSort;

	LVFINDINFO pInfo1, pInfo2;
	pInfo1.flags = LVFI_PARAM;
	pInfo2.flags = LVFI_PARAM;
	pInfo1.lParam = lParam1;
	pInfo2.lParam = lParam2;
	int ind1 = pListCtrl->FindItem(&pInfo1);
	int ind2 = pListCtrl->FindItem(&pInfo2);
	CString strItem1;
	CString strItem2;
	pListCtrl->GetItemText(ind1, 1, strItem1);
	pListCtrl->GetItemText(ind2, 1, strItem2);

	return strItem1.CompareNoCase(strItem2);
}

void CQuickPasteKeyboard::LoadItems()
{
	m_list.DeleteAllItems();

	LVITEM lvi;
	CString strItem;
	int row = 0;

	for (DWORD i = ActionEnums::FIRST_ACTION + 1; i < ActionEnums::LAST_ACTION; i++)
	{
		ActionEnums::ActionEnumValues action = (ActionEnums::ActionEnumValues)i;

		if (ActionEnums::UserConfigurable(action))
		{
			lvi.mask = LVIF_TEXT;
			lvi.iItem = (int)i;

			KeyboardArray ar;
			for (int x = 0; x < 10; x++)
			{
				ar.Array[x].A = CGetSetOptions::GetActionShortCutA(action, x);
				ar.Array[x].B = CGetSetOptions::GetActionShortCutB(action, x);
			}

			CString shortCutText = GetShortCutText(ar);

			lvi.iSubItem = 0;
			lvi.pszText = (LPTSTR)(LPCTSTR)(shortCutText);
			m_list.InsertItem(&lvi);

			CString col2 = ActionEnums::EnumDescription(action);
			m_list.SetItemText(row, 1, col2);

			m_list.SetItemData(row, i);
			m_map[i] = ar;

			row++;
		}
	}

	int dummyId = -1;

	for (auto& element : CGetSetOptions::m_pasteScripts.m_list)
	{
		lvi.mask = LVIF_TEXT;
		lvi.iItem = (int)row;

		KeyboardArray ar;
		ar.m_refData = element.m_guid;
		for (int x = 0; x < 10; x++)
		{
			ar.Array[x].A = CGetSetOptions::GetActionShortCutA(ActionEnums::PASTE_SCRIPT, x, element.m_guid);
			ar.Array[x].B = CGetSetOptions::GetActionShortCutB(ActionEnums::PASTE_SCRIPT, x, element.m_guid);
		}

		CString shortCutText = GetShortCutText(ar);

		lvi.iSubItem = 0;
		lvi.pszText = (LPTSTR)(LPCTSTR)(shortCutText);
		int x = m_list.InsertItem(&lvi);

		CString col2 = ActionEnums::EnumDescription(ActionEnums::PASTE_SCRIPT);
		col2 += _T(": ");
		col2 += element.m_name;
		m_list.SetItemText(row, 1, col2);

		m_list.SetItemData(row, dummyId);
		m_map[dummyId] = ar;

		dummyId--;
		row++;
	}

	m_list.SortItems(MyCompareProc, reinterpret_cast<LPARAM>(&m_list));

	SelectedRow(0);
}

CString CQuickPasteKeyboard::GetShortCutText(KeyboardAB ab)
{
	CString shA;
	CString shB;
	if (ab.A > 0)
	{
		shA = CHotKey::GetHotKeyDisplayStatic(ab.A);

		if (ab.B > 0)
		{
			shB = CHotKey::GetHotKeyDisplayStatic(ab.B);
		}
	}

	CString combined;
	if (shA.IsEmpty() == FALSE &&
		shB.IsEmpty() == FALSE)
	{
		combined.Format(_T("%s - %s"), shA, shB);
	}
	else if (shA.IsEmpty() == FALSE)
	{
		combined.Format(_T("%s"), shA);
	}

	return combined;
}

CString CQuickPasteKeyboard::GetShortCutText(KeyboardArray ar)
{
	CString all;
	for (int i = 0; i < 10; i++)
	{
		CString combined;
		if (ar.Array[i].A > 0)
		{
			combined = GetShortCutText(ar.Array[i]);
		}
		else
		{
			break;
		}

		if (all.GetLength() > 0)
		{
			all += _T(", ");
		}

		all += combined;
	}

	return all;
}

LRESULT CQuickPasteKeyboard::OnLvnItemActivateList1(int, LPNMHDR pNMHDR, BOOL&)
{
	return 0;
}

LRESULT CQuickPasteKeyboard::OnLvnItemchangedList1(int, LPNMHDR pNMHDR, BOOL&)
{
	LPNMLISTVIEW pNMLV = (LPNMLISTVIEW)pNMHDR;

	if ((pNMLV->uChanged & LVIF_STATE)
		&& (pNMLV->uNewState & LVIS_SELECTED))
	{
		int id = (int)m_list.GetItemData(pNMLV->iItem);

		m_assignedCombo.ResetContent();

		bool addedItem = false;

		for (int i = 0; i < 10; i++)
		{
			if (m_map[id].Array[i].A > 0)
			{
				CString shortcut = GetShortCutText(m_map[id].Array[i]);
				int pos = m_assignedCombo.AddString(shortcut);
				m_assignedCombo.SetItemData(pos, i);

				addedItem = true;
			}
		}

		if (addedItem == false)
		{
			CString shortcut;
			int pos = m_assignedCombo.AddString(shortcut);
			m_assignedCombo.SetItemData(pos, 0);
		}

		m_assignedCombo.SetCurSel(0);

		LoadHotKey(m_map[id].Array[0]);
	}

	return 0;
}

LRESULT CQuickPasteKeyboard::OnBnClickedAssign(WORD, WORD, HWND, BOOL&)
{
	int shortCutId = SelectedCommandShortCutId();
	int id = SelectedCommandId();
	int row = SelectedCommandRow();

	if (m_map.find(id) != m_map.end() &&
		shortCutId >= 0 &&
		shortCutId < 10)
	{
		m_map[id].Array[shortCutId].Dirty = true;

		if (IsDlgButtonChecked(IDC_RADIO_KEYBOARD_1) == BST_CHECKED)
		{
			m_map[id].Array[shortCutId].A = ACCEL_MAKEKEY(LOBYTE(m_hotKey1.GetHotKey()), (HIBYTE(m_hotKey1.GetHotKey()) & ~HOTKEYF_EXT));
		}
		else if (IsDlgButtonChecked(IDC_RADIO_MOUSE_1) == BST_CHECKED)
		{
			WORD vk = (WORD)m_mouseType1.GetItemData(m_mouseType1.GetCurSel());
			WORD mod = 0;
			if (IsDlgButtonChecked(IDC_CHECK_SHIFT_1) == BST_CHECKED)
			{
				mod |= HOTKEYF_SHIFT;
			}
			if (IsDlgButtonChecked(IDC_CHECK_CONTROL_1) == BST_CHECKED)
			{
				mod |= HOTKEYF_CONTROL;
			}
			if (IsDlgButtonChecked(IDC_CHECK_ALT_1) == BST_CHECKED)
			{
				mod |= HOTKEYF_ALT;
			}

			m_map[id].Array[shortCutId].A = ACCEL_MAKEKEY(vk, mod);
		}

		if (IsDlgButtonChecked(IDC_CHECK_ENABLE_SECOND_PRESS) == BST_CHECKED)
		{
			if (IsDlgButtonChecked(IDC_RADIO_KEYBOARD_2) == BST_CHECKED)
			{
				m_map[id].Array[shortCutId].B = ACCEL_MAKEKEY(LOBYTE(m_hotKey2.GetHotKey()), (HIBYTE(m_hotKey2.GetHotKey()) & ~HOTKEYF_EXT));
			}
			else if (IsDlgButtonChecked(IDC_RADIO_MOUSE_2) == BST_CHECKED)
			{
				WORD vk = (WORD)m_mouseType2.GetItemData(m_mouseType2.GetCurSel());
				WORD mod = 0;
				if (IsDlgButtonChecked(IDC_CHECK_SHIFT_2) == BST_CHECKED)
				{
					mod |= HOTKEYF_SHIFT;
				}
				if (IsDlgButtonChecked(IDC_CHECK_CONTROL_2) == BST_CHECKED)
				{
					mod |= HOTKEYF_CONTROL;
				}
				if (IsDlgButtonChecked(IDC_CHECK_ALT_2) == BST_CHECKED)
				{
					mod |= HOTKEYF_ALT;
				}

				m_map[id].Array[shortCutId].B = ACCEL_MAKEKEY(vk, mod);
			}
		}
		else
		{
			m_map[id].Array[shortCutId].B = 0;
		}

		CString sh = GetShortCutText(m_map[id]);
		LVITEM lvi;
		lvi.mask = LVIF_TEXT;
		lvi.iItem = (int)row;
		lvi.iSubItem = 0;
		lvi.pszText = (LPTSTR)(LPCTSTR)(sh);
		m_list.SetItem(&lvi);

		CString shItem = GetShortCutText(m_map[id].Array[shortCutId]);
		int index = m_assignedCombo.GetCurSel();
		m_assignedCombo.DeleteString(index);
		int pos = m_assignedCombo.InsertString(index, shItem);
		m_assignedCombo.SetItemData(pos, shortCutId);
		m_assignedCombo.SetCurSel(pos);
	}

	return 0;
}

int CQuickPasteKeyboard::OnApply()
{
	for (std::map<DWORD, KeyboardArray>::iterator it = m_map.begin(); it != m_map.end(); ++it)
	{
		for (int i = 0; i < 10; i++)
		{
			if (it->second.Array[i].Dirty)
			{
				int actionEnum = it->first;
				if (actionEnum < 0)
				{
					actionEnum = ActionEnums::PASTE_SCRIPT;
				}

				CGetSetOptions::SetActionShortCutA(actionEnum, it->second.Array[i].A, i, it->second.m_refData);
				CGetSetOptions::SetActionShortCutB(actionEnum, it->second.Array[i].B, i, it->second.m_refData);
				it->second.Array[i].Dirty = false;
			}
		}
	}

	return PSNRET_NOERROR;
}

LRESULT CQuickPasteKeyboard::OnCbnSelchangeComboAllAssigned(WORD, WORD, HWND, BOOL&)
{
	int shortCutId = SelectedCommandShortCutId();
	int id = SelectedCommandId();

	if (m_map.find(id) != m_map.end() &&
		shortCutId >= 0 &&
		shortCutId < 10)
	{
		LoadHotKey(m_map[id].Array[shortCutId]);
	}

	return 0;
}

LRESULT CQuickPasteKeyboard::OnBnClickedButtonRemove(WORD, WORD, HWND, BOOL&)
{
	int shortCutId = SelectedCommandShortCutId();
	int id = SelectedCommandId();

	if (m_map.find(id) != m_map.end() &&
		shortCutId >= 0 &&
		shortCutId < 10)
	{
		int row = SelectedCommandRow();

		m_map[id].Array[shortCutId].Dirty = true;
		m_map[id].Array[shortCutId].A = 0;
		m_map[id].Array[shortCutId].B = 0;

		m_assignedCombo.DeleteString(m_assignedCombo.GetCurSel());

		if (m_assignedCombo.GetCount() <= 0)
		{
			CString shortcut;
			int pos = m_assignedCombo.AddString(shortcut);
			m_assignedCombo.SetItemData(pos, 0);
		}

		CString sh = GetShortCutText(m_map[id]);
		LVITEM lvi;
		lvi.mask = LVIF_TEXT;
		lvi.iItem = (int)row;
		lvi.iSubItem = 0;
		lvi.pszText = (LPTSTR)(LPCTSTR)(sh);
		m_list.SetItem(&lvi);

		m_assignedCombo.SetCurSel(0);

		LoadHotKey(m_map[id].Array[0]);
	}

	return 0;
}

LRESULT CQuickPasteKeyboard::OnBnClickedButtonAdd(WORD, WORD, HWND, BOOL&)
{
	int count = m_assignedCombo.GetCount();
	if (count < 10)
	{
		int id = SelectedCommandId();
		if (m_map.find(id) != m_map.end())
		{
			for (int i = 0; i < 10; i++)
			{
				if (m_map[id].Array[i].A <= 0)
				{
					CString shortcut;
					int pos = m_assignedCombo.AddString(shortcut);
					m_assignedCombo.SetItemData(pos, i);
					m_assignedCombo.SetCurSel(pos);

					LoadHotKey(m_map[id].Array[i]);

					m_hotKey1.SetFocus();

					break;
				}
			}
		}
	}

	return 0;
}

int CQuickPasteKeyboard::SelectedCommandId()
{
	int id = -1;

	int row = m_list.GetNextItem(-1, LVNI_SELECTED);
	while (row != -1)
	{
		id = (int)m_list.GetItemData(row);
		row = m_list.GetNextItem(row, LVNI_SELECTED);
	}

	return id;
}

int CQuickPasteKeyboard::SelectedCommandRow()
{
	return m_list.GetNextItem(-1, LVNI_SELECTED);
}

int CQuickPasteKeyboard::SelectedCommandShortCutId()
{
	int index = m_assignedCombo.GetCurSel();
	int shortCutId = (int)m_assignedCombo.GetItemData(index);

	return shortCutId;
}

void CQuickPasteKeyboard::LoadHotKey(KeyboardAB ab)
{
	CheckDlgButton(IDC_RADIO_MOUSE_1, BST_UNCHECKED);
	CheckDlgButton(IDC_RADIO_KEYBOARD_1, BST_UNCHECKED);
	CheckDlgButton(IDC_CHECK_SHIFT_1, BST_UNCHECKED);
	CheckDlgButton(IDC_CHECK_CONTROL_1, BST_UNCHECKED);
	CheckDlgButton(IDC_CHECK_ALT_1, BST_UNCHECKED);
	CheckDlgButton(IDC_CHECK_SHIFT_2, BST_UNCHECKED);
	CheckDlgButton(IDC_CHECK_CONTROL_2, BST_UNCHECKED);
	CheckDlgButton(IDC_CHECK_ALT_2, BST_UNCHECKED);
	m_hotKey1.SetHotKey(0, 0);
	m_hotKey2.SetHotKey(0, 0);

	int a = 0;
	if (ab.A > 0)
	{
		a = ab.A;

		switch (LOBYTE((DWORD)a))
		{
		case VK_LEFT:
		case VK_UP:
		case VK_RIGHT:
		case VK_DOWN:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_END:
		case VK_HOME:
		case VK_INSERT:
		case VK_DELETE:
		case VK_DIVIDE:
		case VK_NUMLOCK:
		{
			a = ACCEL_MAKEKEY(LOBYTE(a), (HIBYTE(a) | HOTKEYF_EXT));
			break;
		}
		}

		switch (LOBYTE((DWORD)a))
		{
		case VK_MOUSE_CLICK:
		case VK_MOUSE_DOUBLE_CLICK:
		case VK_MOUSE_RIGHT_CLICK:
		case VK_MOUSE_MIDDLE_CLICK:
			SelectMouseTypeCombo(m_mouseType1, LOBYTE((DWORD)a));
			CheckDlgButton(IDC_RADIO_MOUSE_1, BST_CHECKED);

		{
			BYTE mod = HIBYTE(a);
			if (mod & HOTKEYF_SHIFT)
				CheckDlgButton(IDC_CHECK_SHIFT_1, BST_CHECKED);
			if (mod & HOTKEYF_CONTROL)
				CheckDlgButton(IDC_CHECK_CONTROL_1, BST_CHECKED);
			if (mod & HOTKEYF_ALT)
				CheckDlgButton(IDC_CHECK_ALT_1, BST_CHECKED);
		}

			break;
		default:
			CheckDlgButton(IDC_RADIO_KEYBOARD_1, BST_CHECKED);
			m_hotKey1.SetHotKey(LOBYTE((DWORD)a), (HIBYTE((DWORD)a)));
			break;
		}

		int b = 0;
		if (ab.B > 0)
		{
			CheckDlgButton(IDC_CHECK_ENABLE_SECOND_PRESS, BST_CHECKED);

			::ShowWindow(::GetDlgItem(m_hWnd, IDC_RADIO_MOUSE_2), SW_SHOW);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_RADIO_KEYBOARD_2), SW_SHOW);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_SECOND_PRESS), SW_SHOW);

			b = ab.B;

			switch (LOBYTE((DWORD)b))
			{
			case VK_LEFT:
			case VK_UP:
			case VK_RIGHT:
			case VK_DOWN:
			case VK_PRIOR:
			case VK_NEXT:
			case VK_END:
			case VK_HOME:
			case VK_INSERT:
			case VK_DELETE:
			case VK_DIVIDE:
			case VK_NUMLOCK:
			{
				b = ACCEL_MAKEKEY(LOBYTE(b), (HIBYTE(b) | HOTKEYF_EXT));
				break;
			}
			}

			switch (LOBYTE((DWORD)b))
			{
			case VK_MOUSE_CLICK:
			case VK_MOUSE_DOUBLE_CLICK:
			case VK_MOUSE_RIGHT_CLICK:
			case VK_MOUSE_MIDDLE_CLICK:
				SelectMouseTypeCombo(m_mouseType2, LOBYTE((DWORD)b));
				CheckDlgButton(IDC_RADIO_MOUSE_2, BST_CHECKED);
				CheckDlgButton(IDC_RADIO_KEYBOARD_2, BST_UNCHECKED);

				::ShowWindow(::GetDlgItem(m_hWnd, IDC_MOUSE_2), SW_SHOW);
				::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_SHIFT_2), SW_SHOW);
				::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_CONTROL_2), SW_SHOW);
				::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_ALT_2), SW_SHOW);

			{
				BYTE mod = HIBYTE(b);
				if (mod & HOTKEYF_SHIFT)
					CheckDlgButton(IDC_CHECK_SHIFT_2, BST_CHECKED);
				if (mod & HOTKEYF_CONTROL)
					CheckDlgButton(IDC_CHECK_CONTROL_2, BST_CHECKED);
				if (mod & HOTKEYF_ALT)
					CheckDlgButton(IDC_CHECK_ALT_2, BST_CHECKED);
			}

				break;
			default:
				CheckDlgButton(IDC_RADIO_KEYBOARD_2, BST_CHECKED);
				CheckDlgButton(IDC_RADIO_MOUSE_2, BST_UNCHECKED);
				m_hotKey2.SetHotKey(LOBYTE((DWORD)b), (HIBYTE((DWORD)b)));
				break;
			}
		}
		else
		{
			CheckDlgButton(IDC_CHECK_ENABLE_SECOND_PRESS, BST_UNCHECKED);
			CheckDlgButton(IDC_RADIO_KEYBOARD_2, BST_CHECKED);

			::ShowWindow(::GetDlgItem(m_hWnd, IDC_RADIO_MOUSE_2), SW_HIDE);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_RADIO_KEYBOARD_2), SW_HIDE);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_HOTKEY2), SW_HIDE);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_ENTER2), SW_HIDE);

			::ShowWindow(::GetDlgItem(m_hWnd, IDC_MOUSE_2), SW_HIDE);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_SHIFT_2), SW_HIDE);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_CONTROL_2), SW_HIDE);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_ALT_2), SW_HIDE);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_SECOND_PRESS), SW_HIDE);
		}
	}
	else
	{
		CheckDlgButton(IDC_CHECK_ENABLE_SECOND_PRESS, BST_UNCHECKED);
		CheckDlgButton(IDC_RADIO_KEYBOARD_1, BST_CHECKED);
		CheckDlgButton(IDC_RADIO_KEYBOARD_2, BST_CHECKED);
	}

	OnBnClickedRadioMouse1(0, 0, 0, bDummy);
}

LRESULT CQuickPasteKeyboard::OnBnClickedRadioKeyboard1(WORD, WORD, HWND, BOOL&)
{
	OnBnClickedRadioMouse1(0, 0, 0, (BOOL&)bDummy);
	return 0;
}

LRESULT CQuickPasteKeyboard::OnBnClickedRadioMouse1(WORD, WORD, HWND, BOOL&)
{
	if (IsDlgButtonChecked(IDC_RADIO_KEYBOARD_1) == BST_CHECKED)
	{
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_HOTKEY1), SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_ENTER), SW_SHOW);

		::ShowWindow(::GetDlgItem(m_hWnd, IDC_MOUSE_1), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_SHIFT_1), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_CONTROL_1), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_ALT_1), SW_HIDE);
	}
	else if (IsDlgButtonChecked(IDC_RADIO_MOUSE_1) == BST_CHECKED)
	{
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_HOTKEY1), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_ENTER), SW_HIDE);

		::ShowWindow(::GetDlgItem(m_hWnd, IDC_MOUSE_1), SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_SHIFT_1), SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_CONTROL_1), SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_ALT_1), SW_SHOW);
	}

	if (IsDlgButtonChecked(IDC_CHECK_ENABLE_SECOND_PRESS) == BST_CHECKED)
	{
		if (IsDlgButtonChecked(IDC_RADIO_KEYBOARD_2) == BST_CHECKED)
		{
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_HOTKEY2), SW_SHOW);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_ENTER2), SW_SHOW);

			::ShowWindow(::GetDlgItem(m_hWnd, IDC_MOUSE_2), SW_HIDE);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_SHIFT_2), SW_HIDE);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_CONTROL_2), SW_HIDE);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_ALT_2), SW_HIDE);
		}
		else if (IsDlgButtonChecked(IDC_RADIO_MOUSE_2) == BST_CHECKED)
		{
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_HOTKEY2), SW_HIDE);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_ENTER2), SW_HIDE);

			::ShowWindow(::GetDlgItem(m_hWnd, IDC_MOUSE_2), SW_SHOW);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_SHIFT_2), SW_SHOW);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_CONTROL_2), SW_SHOW);
			::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_ALT_2), SW_SHOW);
		}
	}

	return 0;
}

LRESULT CQuickPasteKeyboard::OnBnClickedRadioKeyboard2(WORD, WORD, HWND, BOOL&)
{
	OnBnClickedRadioMouse1(0, 0, 0, bDummy);
	return 0;
}

LRESULT CQuickPasteKeyboard::OnBnClickedRadioMouse2(WORD, WORD, HWND, BOOL&)
{
	OnBnClickedRadioMouse1(0, 0, 0, bDummy);
	return 0;
}

LRESULT CQuickPasteKeyboard::OnBnClickedCheckEnableSecondPress(WORD, WORD, HWND, BOOL&)
{
	int shortCutId = SelectedCommandShortCutId();
	int id = SelectedCommandId();

	if (m_map.find(id) != m_map.end() &&
		shortCutId >= 0 &&
		shortCutId < 10 &&
		m_map[id].Array[shortCutId].B > 0)
	{
		m_map[id].Array[shortCutId].Dirty = true;
		m_map[id].Array[shortCutId].B = 0;
	}

	if (IsDlgButtonChecked(IDC_CHECK_ENABLE_SECOND_PRESS) == BST_CHECKED)
	{
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_HOTKEY2), SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_ENTER2), SW_SHOW);

		::ShowWindow(::GetDlgItem(m_hWnd, IDC_MOUSE_2), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_SHIFT_2), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_CONTROL_2), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_ALT_2), SW_HIDE);

		::ShowWindow(::GetDlgItem(m_hWnd, IDC_RADIO_MOUSE_2), SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_RADIO_KEYBOARD_2), SW_SHOW);

		::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_SECOND_PRESS), SW_SHOW);

		CheckDlgButton(IDC_RADIO_KEYBOARD_2, BST_CHECKED);
		CheckDlgButton(IDC_RADIO_MOUSE_2, BST_UNCHECKED);
		m_hotKey2.SetHotKey(0, 0);
	}
	else if (IsDlgButtonChecked(IDC_CHECK_ENABLE_SECOND_PRESS) == BST_UNCHECKED)
	{
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_HOTKEY2), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_BUTTON_ENTER2), SW_HIDE);

		::ShowWindow(::GetDlgItem(m_hWnd, IDC_MOUSE_2), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_SHIFT_2), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_CONTROL_2), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_CHECK_ALT_2), SW_HIDE);

		::ShowWindow(::GetDlgItem(m_hWnd, IDC_RADIO_KEYBOARD_2), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_RADIO_MOUSE_2), SW_HIDE);

		::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_SECOND_PRESS), SW_HIDE);
	}

	return 0;
}

void CQuickPasteKeyboard::SelectMouseTypeCombo(CComboBox& combo, int value)
{
	int count = combo.GetCount();
	for (int i = 0; i < count; i++)
	{
		int itemData = (int)combo.GetItemData(i);
		if (itemData == value)
		{
			combo.SetCurSel(i);
			break;
		}
	}
}

void CQuickPasteKeyboard::SelectedRow(int row)
{
	m_list.SetItemState(row, LVIS_FOCUSED, LVIS_FOCUSED);
	m_list.SetItemState(row, LVIS_SELECTED, LVIS_SELECTED);
	m_list.SetSelectionMark(row);
	m_list.EnsureVisible(row, FALSE);
}

LRESULT CQuickPasteKeyboard::OnCbnKillfocusComboEnterModifier(WORD, WORD, HWND, BOOL&)
{
	return 0;
}

LRESULT CQuickPasteKeyboard::OnEnKillfocusEdit1(WORD, WORD, HWND, BOOL&)
{
	return 0;
}

LRESULT CQuickPasteKeyboard::OnBnClickedButtonEnter(WORD, WORD, HWND, BOOL&)
{
	m_hotKey1.SetHotKey(VK_RETURN, CAccels::GetKeyStateModifiers());

	return 0;
}

LRESULT CQuickPasteKeyboard::OnBnClickedButtonEnter2(WORD, WORD, HWND, BOOL&)
{
	m_hotKey2.SetHotKey(VK_RETURN, CAccels::GetKeyStateModifiers());

	return 0;
}

LRESULT CQuickPasteKeyboard::OnKillFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	bHandled = FALSE;

	return 0;
}

LRESULT CQuickPasteKeyboard::OnKeyUp(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	bHandled = FALSE;

	return 0;
}

LRESULT CQuickPasteKeyboard::OnBnClickedButtonReset(WORD, WORD, HWND, BOOL&)
{
	for (std::map<DWORD, KeyboardArray>::iterator it = m_map.begin(); it != m_map.end(); ++it)
	{
		for (int i = 0; i < 10; i++)
		{
			int newA = ActionEnums::GetDefaultShortCutKeyA((ActionEnums::ActionEnumValues)it->first, i);
			int newB = ActionEnums::GetDefaultShortCutKeyB((ActionEnums::ActionEnumValues)it->first, i);
			if (it->second.Array[i].A != newA ||
				it->second.Array[i].B != newB)
			{
				it->second.Array[i].A = newA;
				it->second.Array[i].B = newB;
				it->second.Array[i].Dirty = true;
			}
		}
	}

	int count = m_list.GetItemCount();
	for (int row = 0; row < count; row++)
	{
		int actionId = (int)m_list.GetItemData(row);

		CString sh = GetShortCutText(m_map[actionId]);

		LVITEM lvi;
		lvi.mask = LVIF_TEXT;
		lvi.iItem = row;
		lvi.iSubItem = 0;
		lvi.pszText = (LPTSTR)(LPCTSTR)(sh);
		m_list.SetItem(&lvi);
	}

	SelectedRow(0);

	return 0;
}
