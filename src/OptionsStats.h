#if !defined(AFX_OPTIONSSTATS_H__5F6FFD2C_E563_4166_8294_A0CBBD17403B__INCLUDED_)
#define AFX_OPTIONSSTATS_H__5F6FFD2C_E563_4166_8294_A0CBBD17403B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsStats.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// COptionsStats dialog

class COptionsStats : public CPropertyPageImpl<COptionsStats>
{
// Construction
public:
	COptionsStats();   // standard constructor

	enum { IDD = IDD_OPTIONS_STATS };
	CString	m_eAllCopies;
	CString	m_eAllPastes;
	CString	m_eAllDate;
	CString	m_eTripCopies;
	CString	m_eTripDate;
	CString	m_eTripPastes;
	CString	m_eSavedCopies;
	CString	m_eSavedCopyData;
	CString	m_eDatabaseSize;
	CString	m_eClipsSent;
	CString	m_eClipsRecieved;
	CString	m_eLastStarted;

	CString m_csTitle;

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnResetCounts(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRemoveAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	BEGIN_MSG_MAP(COptionsStats)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_RESET_COUNTS, OnResetCounts)
		COMMAND_ID_HANDLER(IDC_REMOVE_ALL, OnRemoveAll)
	END_MSG_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSSTATS_H__5F6FFD2C_E563_4166_8294_A0CBBD17403B__INCLUDED_)
