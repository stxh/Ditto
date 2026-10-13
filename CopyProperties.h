#pragma once

// CopyProperties.h : header file
//

#include "GroupCombo.h"
#include "RichEditCtrlEx.h"
#include "DialogResizer.h"

/////////////////////////////////////////////////////////////////////////////
// CCopyProperties dialog

class CCopyProperties : public CDialogImpl<CCopyProperties>
{
public:
	enum { IDD = IDD_COPY_PROPERTIES };

// Construction
	CCopyProperties(long lCopyID, HWND hParent = NULL, CClip *pMemoryClip = NULL); // standard constructor

// Dialog Data
	CEdit	m_QuickPasteText;
	CEdit m_description;
	CGroupCombo	m_GroupCombo;
	CHotKeyCtrl	m_HotKey;
	CHotKeyCtrl	m_MoveToGrouHotKey;
	CListBox	m_lCopyData;
	CString	m_eDate;
	CString m_lastPasteDate;
	BOOL	m_bNeverAutoDelete;
	BOOL m_hotKeyGlobal;
	BOOL m_moveToGroupHotKeyGlobal;

public:
	bool m_bChangedText;
	long m_lGroupChangedTo;  // Changed from bool to long to match the original intention
	void SetHideOnKillFocus(bool bVal)	{ m_bHideOnKillFocus = bVal; }
	void SetHandleKillFocus(bool bVal)	{ m_bHandleKillFocus = bVal; }
	void SetToTopMost(bool bVal)		{ m_bSetToTopMost = bVal; }

// Implementation
protected:

	long m_lCopyID;
	ARRAY m_DeletedData;
	bool m_bDeletedData;
	bool m_bHideOnKillFocus;
	CDialogResizer m_Resize;
	bool m_bInGroup;
	bool m_bHandleKillFocus;
	bool m_bSetToTopMost;
	CClip *m_pMemoryClip;
	CBrush m_brush;
	CClip m_clip;
	bool m_mouseDownOnCaption;

	void LoadDataIntoCClip(CClip &Clip);
	void LoadDataFromCClip(CClip &Clip);
	void UpdateSelectedDataMd5();
	BOOL CheckGlobalHotKey(CClip &clip);
	BOOL CheckMoveToGroupGlobalHotKey(CClip &clip);

	// WTL Message Map
	BEGIN_MSG_MAP(CCopyProperties)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_NCLBUTTONDOWN, OnNcLButtonDown)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_HANDLER(IDC_DELETE_COPY_DATA, BN_CLICKED, OnDeleteCopyData)
		COMMAND_HANDLER(IDC_LIST1, LBN_SELCHANGE, OnLbnSelchangeCopyData)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDeleteCopyData(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLbnSelchangeCopyData(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnNcLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
