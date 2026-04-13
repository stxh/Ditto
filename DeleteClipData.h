#pragma once

#include "DialogResizer.h"
#include "ShowTaskBarIcon.h"
#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlcom.h>
#include <atlcrack.h>
#include <atlmisc.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlframe.h>
#include <vector>

// CDeleteClipData dialog

class CDeleteData
{
public:
	CDeleteData() :
		m_lID(-1),
		m_dataSize(0)
	{
	}

	long m_lID;
	long m_DatalID;
	CString m_Desc;
	time_t m_createdDateTime;
	time_t m_lastUsedDateTime;
	CString m_clipboardFormat;
	DWORD m_dataSize;
	CString m_quickPasteText;
};
	
	
	

class CDeleteClipData : public CDialogImpl<CDeleteClipData>
{
public:
	CDeleteClipData(HWND hParent = NULL);   // standard constructor
	CDeleteClipData(CWnd* pParent = NULL); // legacy constructor for compatibility
	virtual ~CDeleteClipData();

	void SetNotifyWnd(HWND hWnd);

// Dialog Data
	enum { IDD = IDD_DELETE_CLIP_DATA };

protected:
	void DoDataExchange(); // Custom DDX implementation

	// WTL Message Map
	BEGIN_MSG_MAP(CDeleteClipData)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_NCDESTROY, OnNcDestroy)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		COMMAND_ID_HANDLER(IDC_BUTTON_SEARCH, OnBnClickedButtonSearch)
		COMMAND_ID_HANDLER(IDC_CHECK_CLIP_TITLE, OnBnClickedCheckClipTitle)
		COMMAND_ID_HANDLER(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
		COMMAND_ID_HANDLER(IDCANCEL, OnBnClickedCancel)
		COMMAND_ID_HANDLER(IDC_CHECK_CREATE_DATE, OnBnClickedCheckCreateDate)
		COMMAND_ID_HANDLER(IDC_CHECK_LAST_USE_DATE, OnBnClickedCheckLastUseDate)
		COMMAND_ID_HANDLER(IDC_CHECK_DATA_FORMAT, OnBnClickedCheckDataFormat)
		NOTIFY_HANDLER(IDC_LIST2, LVN_KEYDOWN, OnLvnKeydownList2)
		NOTIFY_HANDLER(IDC_LIST2, LVN_ITEMCHANGED, OnLvnItemchangedList2)
		NOTIFY_HANDLER(0, HDN_GETDISPINFO, OnHdnGetdispinfoList2)
		NOTIFY_HANDLER(IDC_LIST2, LVN_GETDISPINFO, OnLvnGetdispinfoList2)
		NOTIFY_HANDLER(0, HDN_ITEMCLICK, OnLvnColumnclickList2)
	END_MSG_MAP()

	CDialogResizer m_Resize;
	CListViewCtrl	m_List;
	HWND m_hWndParent;
	CShowTaskBarIcon m_showTaskbar;
	std::vector<CDeleteData> m_data;
	std::vector<CDeleteData> m_toDelete;
	std::vector<CDeleteData> m_filteredOut;
	bool m_applyingDelete;
	bool m_cancelDelete;


	void InitListCtrlCols();
	BOOL OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void LoadItems();
	void FilterItems();
	bool MatchesFilter(CDeleteData *pdata);
	void ApplyDelete();

public:
	CString m_clipTitle;
	BOOL m_filterByClipTitle;
	BOOL m_filterByCreatedDate;
	BOOL m_filterByLastUsedDate;
	BOOL m_filterByClipboardFormat;
	CComboBox m_clipboardFomatCombo;
	SYSTEMTIME m_createdDateStart;
	SYSTEMTIME m_createdDateEnd;
	SYSTEMTIME m_createdTimeStart;
	SYSTEMTIME m_createdTimeEnd;
	SYSTEMTIME m_usedTimeStart;
	SYSTEMTIME m_usedTimeEnd;
	SYSTEMTIME m_usedDateStart;
	SYSTEMTIME m_usedDateEnd;
	LRESULT OnBnClickedButtonSearch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnLvnKeydownList2(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
	LRESULT OnLvnItemchangedList2(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
	LRESULT OnHdnGetdispinfoList2(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
	LRESULT OnLvnGetdispinfoList2(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
	CString m_databaseSize;
	CString m_selectedSize;
	CString m_selectedCount;
	CString m_toDeleteSize;
	LRESULT OnBnClickedCheckClipTitle(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedButtonApply(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBnClickedCheckCreateDate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedCheckLastUseDate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedCheckDataFormat(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnLvnColumnclickList2(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
	BOOL PreTranslateMessage(MSG* pMsg);
};
