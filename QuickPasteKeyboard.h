#pragma once

#include <atlbase.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>
#include <atlctrlx.h>

class CQuickPasteKeyboard : public CPropertyPageImpl<CQuickPasteKeyboard>
{
public:
	enum { IDD = IDD_OPTIONS_QUICK_PASTE_KEYBOARD };

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
		COMMAND_ID_HANDLER(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
		COMMAND_ID_HANDLER(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
		COMMAND_ID_HANDLER(IDC_COMBO_ENTER_MODIFIER, OnCbnKillfocusComboEnterModifier)
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

	int OnApply() override;
	BOOL PreTranslateMessage(MSG* pMsg);
	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);

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
	void OnCbnSelchangeComboAllAssigned(WORD, WORD, HWND, BOOL&);

	CListCtrl m_list;
	CHotKeyCtrl m_hotKey1;
	CHotKeyCtrl m_hotKey2;
	CComboBox m_assignedCombo;
	CComboBox m_mouseType1;
	CComboBox m_mouseType2;
};
