#pragma once

class CFriendDetails : public CDialogImpl<CFriendDetails>
{
public:
	enum { IDD = IDD_FRIEND_DETAILS };

	CFriendDetails()
		: m_checkSendAll(FALSE)
	{
	}

	BOOL    m_checkSendAll;
	CString m_csDescription;
	CString m_csIP;

	BEGIN_MSG_MAP(CFriendDetails)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnOk(WORD, WORD, HWND, BOOL&);
	LRESULT OnCancel(WORD, WORD, HWND, BOOL&);
};
