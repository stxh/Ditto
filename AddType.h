#pragma once

// AddType.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddType dialog

class CAddType : public CDialogImpl<CAddType>
{
public:
	enum { IDD = IDD_ADD_TYPE };

// Construction
	CAddType() {} // standard constructor

// Dialog Data
	CListBox	m_lbCandidateTypes;
	CString	m_eCustomType;

	CStringArray m_csSelectedTypes;

	void AddCommonTypes();
	void AddCurrentClipboardTypes();
	void ShowCurrentTypes();
	void AddSelectedTypes();

// Implementation
protected:
	// WTL Message Map
	BEGIN_MSG_MAP(CAddType)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_RADIO_PRIMARY_TYPES, BN_CLICKED, OnBnClickedRadioPrimaryTypes)
		COMMAND_HANDLER(IDC_RADIO_CURRENT_TYPES, BN_CLICKED, OnBnClickedRadioCurrentTypes)
		COMMAND_HANDLER(IDC_RADIO_CUSTOM_TYPE, BN_CLICKED, OnBnClickedRadioCustomType)
		COMMAND_HANDLER(IDC_ADD_3, BN_CLICKED, OnBnClickedAdd)
		COMMAND_HANDLER(IDC_LIST1, LBN_DBLCLK, OnLbnDblclkList)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBnClickedRadioPrimaryTypes(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedRadioCurrentTypes(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedRadioCustomType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnLbnDblclkList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};
