#pragma once

#include <atlbase.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>
#include <atlctrlx.h>

#include "Accels.h"

// The hot key control reports Escape as a selectable key instead of closing the dialog.
class CDittoHotKeyCtrl : public ATL::CWindowImpl<CDittoHotKeyCtrl, WTL::CHotKeyCtrl>
{
public:
	DECLARE_WND_SUPERCLASS(_T("DittoHotKeyCtrl"), GetWndClassName())

	BEGIN_MSG_MAP(CDittoHotKeyCtrl)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	END_MSG_MAP()

	LRESULT OnKeyDown(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
	{
		if (wParam == VK_ESCAPE)
		{
			SetHotKey(VK_ESCAPE, CAccels::GetKeyStateModifiers());
			return 0;
		}

		bHandled = FALSE;
		return 0;
	}
};

class CQuickPasteKeyboard : public CPropertyPageImpl<CQuickPasteKeyboard>
{
public:
	enum { IDD = IDD_OPTIONS_QUICK_PASTE_KEYBOARD };

	CQuickPasteKeyboard();
	~CQuickPasteKeyboard();

	class KeyboardAB
	{
	public:
		KeyboardAB()
		{
			A = -1;
			B = -1;
			Dirty = false;
		}
		int A;
		int B;
		bool Dirty;
	};

	class KeyboardArray
	{
	public:
		KeyboardArray()
		{
			m_refData = _T("");
		}

		CString m_refData;

		KeyboardAB Array[10];
	};

	BEGIN_MSG_MAP(CQuickPasteKeyboard)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		NOTIFY_HANDLER(IDC_LIST1, LVN_ITEMACTIVATE, OnLvnItemActivateList1)
		NOTIFY_HANDLER(IDC_LIST1, LVN_ITEMCHANGED, OnLvnItemchangedList1)
		COMMAND_ID_HANDLER(IDC_ASSIGN, OnBnClickedAssign)
		COMMAND_HANDLER(IDC_COMBO_ALL_ASSIGNED, CBN_SELCHANGE, OnCbnSelchangeComboAllAssigned)
		COMMAND_ID_HANDLER(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
		COMMAND_ID_HANDLER(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
		COMMAND_HANDLER(IDC_COMBO_ENTER_MODIFIER, CBN_KILLFOCUS, OnCbnKillfocusComboEnterModifier)
		COMMAND_HANDLER(IDC_HOTKEY1, EN_KILLFOCUS, OnEnKillfocusEdit1)
		COMMAND_ID_HANDLER(IDC_BUTTON_ENTER, OnBnClickedButtonEnter)
		COMMAND_ID_HANDLER(IDC_BUTTON_ENTER2, OnBnClickedButtonEnter2)
		COMMAND_ID_HANDLER(IDC_BUTTON_RESET, OnBnClickedButtonReset)
		COMMAND_ID_HANDLER(IDC_RADIO_KEYBOARD_1, OnBnClickedRadioKeyboard1)
		COMMAND_ID_HANDLER(IDC_RADIO_MOUSE_1, OnBnClickedRadioMouse1)
		COMMAND_ID_HANDLER(IDC_RADIO_KEYBOARD_2, OnBnClickedRadioKeyboard2)
		COMMAND_ID_HANDLER(IDC_RADIO_MOUSE_2, OnBnClickedRadioMouse2)
		COMMAND_ID_HANDLER(IDC_CHECK_ENABLE_SECOND_PRESS, OnBnClickedCheckEnableSecondPress)
		CHAIN_MSG_MAP(CPropertyPageImpl<CQuickPasteKeyboard>)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnKeyUp(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnKillFocus(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnLvnItemActivateList1(int, LPNMHDR, BOOL&);
	LRESULT OnLvnItemchangedList1(int, LPNMHDR, BOOL&);
	LRESULT OnBnClickedAssign(WORD, WORD, HWND, BOOL&);
	LRESULT OnCbnSelchangeComboAllAssigned(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonRemove(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonAdd(WORD, WORD, HWND, BOOL&);
	LRESULT OnCbnKillfocusComboEnterModifier(WORD, WORD, HWND, BOOL&);
	LRESULT OnEnKillfocusEdit1(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonEnter(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonEnter2(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedButtonReset(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedRadioKeyboard1(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedRadioMouse1(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedRadioKeyboard2(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedRadioMouse2(WORD, WORD, HWND, BOOL&);
	LRESULT OnBnClickedCheckEnableSecondPress(WORD, WORD, HWND, BOOL&);

	int OnApply();

private:
	void InitListCtrlCols();
	void LoadItems();
	CString GetShortCutText(KeyboardArray ab);
	CString GetShortCutText(KeyboardAB ab);
	std::map<DWORD, KeyboardArray> m_map;
	void LoadHotKey(KeyboardAB ab);
	CString m_csTitle;
	int SelectedCommandId();
	int SelectedCommandShortCutId();
	int SelectedCommandRow();
	void SelectMouseTypeCombo(CComboBox& combo, int value);
	void SelectedRow(int row);

	CListViewCtrl m_list;
	CDittoHotKeyCtrl m_hotKey1;
	CDittoHotKeyCtrl m_hotKey2;
	CComboBox m_assignedCombo;
	CComboBox m_mouseType1;
	CComboBox m_mouseType2;
};
