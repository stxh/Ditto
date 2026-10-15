// DeleteClipData.cpp : implementation file
//

#include "stdafx.h"
#include "CP_Main.h"
#include "DeleteClipData.h"
#include "Misc.h"
#include "ProgressWnd.h"
#include <atlctrlx.h>
#include <algorithm>


// CDeleteClipData dialog
CDeleteClipData::CDeleteClipData(HWND hParent)
	: m_clipTitle(_T(""))
	, m_filterByClipTitle(FALSE)
	, m_filterByCreatedDate(FALSE)
	, m_filterByLastUsedDate(FALSE)
	, m_filterByClipboardFormat(FALSE)
	, m_databaseSize(_T(""))
	, m_selectedSize(_T(""))
	, m_selectedCount(_T(""))
{
	// Initialize SYSTEMTIME values to current time
	SYSTEMTIME stNow;
	GetSystemTime(&stNow);
	m_createdDateStart = stNow;
	m_createdDateEnd = stNow;
	m_createdTimeStart = stNow;
	m_createdTimeEnd = stNow;
	m_usedTimeStart = stNow;
	m_usedTimeEnd = stNow;
	m_usedDateStart = stNow;
	m_usedDateEnd = stNow;

	m_applyingDelete = false;
	m_cancelDelete = false;
	m_hWndParent = hParent;
}

CDeleteClipData::~CDeleteClipData()
{
}


BOOL CDeleteClipData::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Initialize the list control
	m_List.Attach(GetDlgItem(IDC_LIST2));
	m_clipboardFomatCombo.Attach(GetDlgItem(IDC_COMBO_DATA_FORMAT));
	m_searchEdit.SubclassWindow(GetDlgItem(IDC_EDIT_CLIP_TITLE));

	theApp.m_Language.UpdateDeleteClipData(m_hWnd);

	m_Resize.SetParent(m_hWnd);
	m_Resize.AddControl(IDC_LIST2, DR_SizeHeight | DR_SizeWidth);
	m_Resize.AddControl(IDCANCEL, DR_MoveTop | DR_MoveLeft);
	m_Resize.AddControl(IDC_BUTTON_APPLY, DR_MoveTop | DR_MoveLeft);
	m_Resize.AddControl(IDC_STATIC_TO_DELETE_TEXT, DR_MoveTop);
	m_Resize.AddControl(IDC_STATIC_TO_DELETE_SIZE, DR_MoveTop);
	m_Resize.AddControl(IDC_STATIC_SELECTED_SIZE, DR_MoveTop);
	m_Resize.AddControl(IDC_STATIC_SELECTED_SIZE_TEXT, DR_MoveTop);
	m_Resize.AddControl(IDC_STATIC_DB_SIZE, DR_MoveTop);
	m_Resize.AddControl(IDC_STATIC_DB_SIZE_TEXT, DR_MoveTop);
	m_Resize.AddControl(IDC_BUTTON_SEARCH, DR_MoveLeft);
	m_Resize.AddControl(IDC_STATIC_GROUP_SEARCH, DR_SizeWidth);

	InitListCtrlCols();

	SetTimer(1, 500, 0);

	SetDbSize();

	::SetDlgItemText(m_hWnd, IDC_STATIC_SELECTED_SIZE, m_selectedSize);
	::SetDlgItemText(m_hWnd, IDC_STATIC_SELECTED_COUNT, m_selectedCount);

	return TRUE;
}

void CDeleteClipData::SetDbSize()
{
	__int64 size = FileSize(GetDBName());

	const int MAX_FILE_SIZE_BUFFER = 255;
	TCHAR szFileSize[MAX_FILE_SIZE_BUFFER];
	StrFormatByteSize(size, szFileSize, MAX_FILE_SIZE_BUFFER);

	m_databaseSize = szFileSize;
	::SetDlgItemText(m_hWnd, IDC_STATIC_DB_SIZE, m_databaseSize);
}

void CDeleteClipData::ReadFilterState()
{
	m_clipTitle = _T("");
	GetDlgItemText(IDC_EDIT_CLIP_TITLE, m_clipTitle);

	m_filterByClipTitle = (::IsDlgButtonChecked(m_hWnd, IDC_CHECK_CLIP_TITLE) == BST_CHECKED);
	m_filterByCreatedDate = (::IsDlgButtonChecked(m_hWnd, IDC_CHECK_CREATE_DATE) == BST_CHECKED);
	m_filterByLastUsedDate = (::IsDlgButtonChecked(m_hWnd, IDC_CHECK_LAST_USE_DATE) == BST_CHECKED);
	m_filterByClipboardFormat = (::IsDlgButtonChecked(m_hWnd, IDC_CHECK_DATA_FORMAT) == BST_CHECKED);

	static const int dateCtrls[] =
	{
		IDC_DATE_CREATE_START, IDC_DATE_CREATE_END, IDC_TIME_CREATE_START, IDC_TIME_CREATE_END,
		IDC_DATE_USE_START, IDC_DATE_USE_END, IDC_TIME_USE_START, IDC_TIME_USE_END
	};
	SYSTEMTIME *values[] =
	{
		&m_createdDateStart, &m_createdDateEnd, &m_createdTimeStart, &m_createdTimeEnd,
		&m_usedDateStart, &m_usedDateEnd, &m_usedTimeStart, &m_usedTimeEnd
	};

	for (int i = 0; i < 8; i++)
	{
		HWND hPicker = ::GetDlgItem(m_hWnd, dateCtrls[i]);
		if (hPicker == NULL)
			continue;

		SYSTEMTIME st = *values[i];
		if (::SendMessage(hPicker, DTM_GETSYSTEMTIME, 0, reinterpret_cast<LPARAM>(&st)) == GDT_VALID)
			*values[i] = st;
	}
}

void CDeleteClipData::InitListCtrlCols()
{
	m_List.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

	m_List.InsertColumn(0, theApp.m_Language.GetDeleteClipDataString("Title", "Title"), LVCFMT_LEFT, 350);
	m_List.InsertColumn(1, theApp.m_Language.GetDeleteClipDataString("QuickPasteText", "Quick Paste Text"), LVCFMT_LEFT, 200);
	m_List.InsertColumn(2, theApp.m_Language.GetDeleteClipDataString("Created", "Created"), LVCFMT_LEFT, 150);
	m_List.InsertColumn(3, theApp.m_Language.GetDeleteClipDataString("LastUsed", "Last Used"), LVCFMT_LEFT, 150);
	m_List.InsertColumn(4, theApp.m_Language.GetDeleteClipDataString("Format", "Format"), LVCFMT_LEFT, 150);
	m_List.InsertColumn(5, theApp.m_Language.GetDeleteClipDataString("DataSize", "Data Size"), LVCFMT_LEFT, 100);
}

void CDeleteClipData::LoadItems()
{
	CWaitCursor wait;
	m_data.clear();	
	m_filteredOut.clear();

	if (m_clipboardFomatCombo.GetCount() == 0)
	{
		CppSQLite3Query qFormats = theApp.m_db.execQueryEx(_T("select DISTINCT(strClipBoardFormat) from Data"));
		while (qFormats.eof() == false)
		{
			CString format = qFormats.getStringField(_T("strClipBoardFormat"));
			m_clipboardFomatCombo.AddString(format);

			qFormats.nextRow();
		}
	}

	CppSQLite3Query q = theApp.m_db.execQueryEx(_T("SELECT Main.lID, Main.mText, Main.lDate, Main.lastPasteDate, Main.QuickPasteText, Data.lID AS DataID, Data.strClipBoardFormat, length(Data.ooData) AS DataLength ")
													_T("FROM Data ")
													_T("INNER JOIN Main on Main.lID = Data.lParentID ")
													_T("ORDER BY length(ooData) DESC"));

	int row = 0;
	while (q.eof() == false)
	{
		CDeleteData data;
		data.m_lID = q.getIntField(_T("lID"));
		data.m_Desc = q.getStringField(_T("mText"));
		data.m_createdDateTime = q.getIntField(_T("lDate"));
		data.m_lastUsedDateTime = q.getIntField(_T("lastPasteDate"));
		data.m_clipboardFormat = q.getStringField(_T("strClipBoardFormat"));
		data.m_dataSize = q.getIntField(_T("DataLength"));
		data.m_DatalID = q.getIntField(_T("DataID"));
		data.m_quickPasteText = q.getStringField(_T("QuickPasteText"));

		m_data.push_back(data);

		row++;

		q.nextRow();		
	}

	m_List.SetItemCountEx(row, 0);
}

//void CDeleteClipData::AddRow(CppSQLite3Query& q, int row)
//{
//	LVITEM lvi;
//
//	lvi.mask = LVIF_TEXT;
//	lvi.iItem = row;	
//
//	lvi.iSubItem = 0;
//	lvi.pszText = (LPTSTR) (LPCTSTR) (q.getStringField(_T("mText")));
//	m_List.InsertItem(&lvi);
//
//	CTime created = q.getIntField(_T("lDate"));
//	COleDateTime dtTime(created.GetTime());
//	
//	CTime pasted = q.getIntField(_T("lastPasteDate"));
//	COleDateTime dtPastedTime(pasted.GetTime());
//
//	m_List.SetItemText(row, 1, dtTime.Format());
//	m_List.SetItemText(row, 2, dtPastedTime.Format());
//	m_List.SetItemText(row, 3, q.getStringField(_T("strClipBoardFormat")));
//
//	int dataLength = q.getIntField(_T("DataLength"));
//
//	const int MAX_FILE_SIZE_BUFFER = 255;
//	TCHAR szFileSize[MAX_FILE_SIZE_BUFFER];
//	StrFormatByteSize(dataLength, szFileSize, MAX_FILE_SIZE_BUFFER);
//
//	m_List.SetItemText(row, 4, szFileSize);
//
//	m_List.SetItemData(row, ));
//}

void CDeleteClipData::SetNotifyWnd(HWND hWnd)
{
	m_hWndParent = hWnd;
}

LRESULT CDeleteClipData::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_applyingDelete)
	{
		return 0;
	}
	DestroyWindow();
	return 0;
}

LRESULT CDeleteClipData::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_Resize.MoveControls(CSize(LOWORD(lParam), HIWORD(lParam)));

	bHandled = FALSE;
	return 0;
}

LRESULT CDeleteClipData::OnNcDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostMessage(m_hWndParent, WM_DELETE_CLIPS_CLOSED, 0, 0);

	bHandled = FALSE;
	return 0;
}

// CDeleteClipData message handlers


LRESULT CDeleteClipData::OnBnClickedButtonSearch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	FilterItems();
	return 0;
}

void CDeleteClipData::FilterItems()
{
	ReadFilterState();

	//First search the already filtered text, see if we need to add them back in
	std::vector<int> filteredRowsToDelete;
	std::vector<CDeleteData> addBackIn;
	INT_PTR count = m_filteredOut.size();
	for (int i = 0; i < count; i++)
	{
		CDeleteData data = m_filteredOut[i];

		if (MatchesFilter(&data))
		{
			addBackIn.push_back(data);
			filteredRowsToDelete.push_back(i);
		}
	}

	//next search the main list
	std::vector<int> rowsToDelete;
	count = m_data.size();
	for (int i = 0; i < count; i++)
	{
		CDeleteData data = m_data[i];

		if(MatchesFilter(&data) == false)
		{
			m_filteredOut.push_back(data);
			rowsToDelete.push_back(i);
		}
	}

	//Add back in the filtered out ones that now match
	count = addBackIn.size();
	for (int i = 0; i < count; i++)
	{
		CDeleteData data = addBackIn[i];
		m_data.push_back(data);
	}

	int toSelect = -1;

	//Remove from the main list the ones that don't match
	count = rowsToDelete.size();
	for (INT_PTR i = count - 1; i >= 0; i--)
	{
		int row = rowsToDelete[i];
		toSelect = row;

		m_data.erase(m_data.begin() + row);
	}

	//Remove the rows that were filtered out but now match
	count = filteredRowsToDelete.size();
	for (INT_PTR i = count - 1; i >= 0; i--)
	{
		int row = filteredRowsToDelete[i];
		m_filteredOut.erase(m_filteredOut.begin() + row);
	}

	if (toSelect > -1)
	{
		m_List.SetItemState(toSelect, LVIS_SELECTED, LVIS_SELECTED);
	}
	
	m_List.SetItemCountEx((int)m_data.size(), 0);
}

bool CDeleteClipData::MatchesFilter(CDeleteData *pdata)
{
	if(m_filterByClipTitle &&
		m_clipTitle != _T("") &&
		pdata->m_Desc != _T(""))
	{
		if(pdata->m_Desc.MakeLower().Find(m_clipTitle.MakeLower()) == -1)
		{
			return false;
		}
	}

	if (m_filterByCreatedDate)
	{		
		COleDateTime dateStart(m_createdDateStart.wYear, m_createdDateStart.wMonth, m_createdDateStart.wDay,
			m_createdTimeStart.wHour, m_createdTimeStart.wMinute, m_createdTimeStart.wSecond);
		COleDateTime dateEnd(m_createdDateEnd.wYear, m_createdDateEnd.wMonth, m_createdDateEnd.wDay,
			m_createdTimeEnd.wHour, m_createdTimeEnd.wMinute, m_createdTimeEnd.wSecond);
		COleDateTime created(pdata->m_createdDateTime);

		if (created >= dateStart && created <= dateEnd)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if (m_filterByLastUsedDate)
	{
		COleDateTime dateStart(m_usedDateStart.wYear, m_usedDateStart.wMonth, m_usedDateStart.wDay,
			m_usedTimeStart.wHour, m_usedTimeStart.wMinute, m_usedTimeStart.wSecond);
		COleDateTime dateEnd(m_usedDateEnd.wYear, m_usedDateEnd.wMonth, m_usedDateEnd.wDay,
			m_usedTimeEnd.wHour, m_usedTimeEnd.wMinute, m_usedTimeEnd.wSecond);
		COleDateTime used(pdata->m_lastUsedDateTime);

		if (used >= dateStart && used <= dateEnd)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if (m_filterByClipboardFormat)
	{
		CString str1;
		int nSel = m_clipboardFomatCombo.GetCurSel();
		if (nSel == CB_ERR)
			return true;

		m_clipboardFomatCombo.GetLBText(nSel, str1);

		if (pdata->m_clipboardFormat == str1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	return true;
}

LRESULT CDeleteClipData::OnLvnKeydownList2(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	switch(pLVKeyDow->wVKey)
	{
	case VK_DELETE:
		this->ApplyDelete();
		break;
	}
	return 0;
}

LRESULT CDeleteClipData::OnLvnItemchangedList2(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	__int64 selectedDataSize = 0;
	int selectedCount = 0;

	for (int row = m_List.GetNextItem(-1, LVNI_SELECTED); row != -1; row = m_List.GetNextItem(row, LVNI_SELECTED))
	{
		if(row >= 0 && row < (int)m_data.size())
		{
			selectedDataSize += m_data[row].m_dataSize;
			selectedCount++;
		}
	}

	const int MAX_FILE_SIZE_BUFFER = 255;
	TCHAR szFileSize[MAX_FILE_SIZE_BUFFER];
	StrFormatByteSize(selectedDataSize, szFileSize, MAX_FILE_SIZE_BUFFER);

	m_selectedSize = szFileSize;

	CString count;
	count.Format(_T("%d"), selectedCount);

	m_selectedCount = count;

	::SetDlgItemText(m_hWnd, IDC_STATIC_SELECTED_SIZE, m_selectedSize);
	::SetDlgItemText(m_hWnd, IDC_STATIC_SELECTED_COUNT, m_selectedCount);

	return 0;
}


LRESULT CDeleteClipData::OnHdnGetdispinfoList2(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	LPNMHDDISPINFO pDispInfo = reinterpret_cast<LPNMHDDISPINFO>(pNMHDR);	
	
	if (pDispInfo->mask &LVIF_TEXT)
	{
		switch (pDispInfo->iItem)
		{
		case 0:
			break;
		}
	}

	return 0;
}


LRESULT CDeleteClipData::OnLvnGetdispinfoList2(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		if (pDispInfo->item.iItem >= 0 && pDispInfo->item.iItem < (int)m_data.size())
		{
			switch (pDispInfo->item.iSubItem)
			{
				case 0:
				{
					  lstrcpyn(pDispInfo->item.pszText, m_data[pDispInfo->item.iItem].m_Desc, pDispInfo->item.cchTextMax);
					  pDispInfo->item.pszText[pDispInfo->item.cchTextMax - 1] = '\0';
				}
				break;
				case 1:
				{
					lstrcpyn(pDispInfo->item.pszText, m_data[pDispInfo->item.iItem].m_quickPasteText, pDispInfo->item.cchTextMax);
					pDispInfo->item.pszText[pDispInfo->item.cchTextMax - 1] = '\0';
				}
				break;
				case 2:
				{
					  COleDateTime dtTime(m_data[pDispInfo->item.iItem].m_createdDateTime);
					  lstrcpyn(pDispInfo->item.pszText, dtTime.Format(), pDispInfo->item.cchTextMax);
					  pDispInfo->item.pszText[pDispInfo->item.cchTextMax - 1] = '\0';
				}
				break;
				case 3:
				{	
					  COleDateTime dtTime(m_data[pDispInfo->item.iItem].m_lastUsedDateTime);
					  lstrcpyn(pDispInfo->item.pszText, dtTime.Format(), pDispInfo->item.cchTextMax);
					  pDispInfo->item.pszText[pDispInfo->item.cchTextMax - 1] = '\0';
				}
				break;
				case 4:
				{
					  lstrcpyn(pDispInfo->item.pszText, m_data[pDispInfo->item.iItem].m_clipboardFormat, pDispInfo->item.cchTextMax);
					  pDispInfo->item.pszText[pDispInfo->item.cchTextMax - 1] = '\0';
				}
				break;
				case 5:
				{
					  const int MAX_FILE_SIZE_BUFFER = 255;
					  TCHAR szFileSize[MAX_FILE_SIZE_BUFFER];
					  StrFormatByteSize(m_data[pDispInfo->item.iItem].m_dataSize, szFileSize, MAX_FILE_SIZE_BUFFER);

					  lstrcpyn(pDispInfo->item.pszText, szFileSize, pDispInfo->item.cchTextMax);
					  pDispInfo->item.pszText[pDispInfo->item.cchTextMax - 1] = '\0';
				}
				break;
			}
		}
	}
	return 0;
}


LRESULT CDeleteClipData::OnBnClickedCheckClipTitle(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_filterByClipTitle = (::IsDlgButtonChecked(m_hWnd, IDC_CHECK_CLIP_TITLE) == BST_CHECKED);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_CLIP_TITLE), m_filterByClipTitle);	
	::SetFocus(::GetDlgItem(m_hWnd, IDC_EDIT_CLIP_TITLE));
	return 0;
}


LRESULT CDeleteClipData::OnBnClickedButtonApply(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ApplyDelete();
	return 0;
}

void CDeleteClipData::ApplyDelete()
{
	if (m_applyingDelete)
		return;

	if (::MessageBox(m_hWnd, _T("Delete selected items?  This cannot be undone!"), _T(""), MB_YESNO | MB_ICONWARNING) == IDYES)
	{
		m_List.EnableWindow(FALSE);
		m_applyingDelete = true;
		m_cancelDelete = false;

		CWaitCursor wait;

		try
		{
			theApp.m_db.execDML(_T("PRAGMA auto_vacuum = 2"));

			std::vector<int> rowsToDelete;

			for (int row = m_List.GetNextItem(-1, LVNI_SELECTED); row != -1; row = m_List.GetNextItem(row, LVNI_SELECTED))
			{
				rowsToDelete.push_back(row);
			}

			CProgressWnd progress;
			progress.Create(m_hWnd, _T("Deleting clip items"), TRUE);
			progress.SetRange(0, (int)rowsToDelete.size() + 4);
			progress.SetText(_T("Deleting selected items"));
			progress.SetStep(1);

			INT_PTR count = rowsToDelete.size();
			for (INT_PTR i = count - 1; i >= 0; i--)
			{
				progress.PeekAndPump();
				if (m_cancelDelete || progress.Cancelled())
				{
					break;
				}
				progress.StepIt();

				int row = rowsToDelete[i];

				CDeleteData data = m_data[row];
				try
				{
					//Sleep(100);
					int deleteCount = theApp.m_db.execDMLEx(_T("DELETE FROM Data where lID = %d"), data.m_DatalID);

					//If there are no more children for this clip then delete the parent
					int parentDeleteCount = theApp.m_db.execDMLEx(_T("DELETE FROM Main where lID IN ")
						_T("(")
						_T("SELECT Main.lID ")
						_T("FROM Main ")
						_T("LEFT OUTER JOIN Data on Data.lParentID = Main.lID ")
						_T("WHERE bIsGroup = 0 AND Main.lID = %d ")
						_T("Group by Main.lID ")
						_T("having Count(Data.lID) = 0 ")
						_T(")"), data.m_lID);

					m_data.erase(m_data.begin() + row);
				}
				CATCH_SQLITE_EXCEPTION
			}

			progress.StepIt();
			progress.SetText(_T("Shrinking database"));

			try
			{
				for(int i = 0; i < 100; i++)
				{
					int toDeleteCount = theApp.m_db.execScalar(_T("SELECT COUNT(clipID) FROM MainDeletes"));
					if(toDeleteCount <= 0)
						break;

					RemoveOldEntries(false);
				}
			}
			CATCH_SQLITE_EXCEPTION

			theApp.m_db.execDML(_T("PRAGMA auto_vacuum = 1"));
			theApp.m_db.execQuery(_T("VACUUM"));
			
			progress.StepIt();
			progress.SetText(_T("Refreshing database size"));

			SetDbSize();
			
			progress.StepIt();
			progress.SetText(_T("Reloading list"));

			//LoadItems();
			
			progress.StepIt();
			progress.SetText(_T("Applying filter"));
			
			FilterItems();

			m_List.SetItemCountEx((int)m_data.size(), 0);
		}
		CATCH_SQLITE_EXCEPTION

		m_applyingDelete = false;
		m_List.EnableWindow();
	}
}

LRESULT CDeleteClipData::OnBnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (m_applyingDelete)
	{
		m_cancelDelete = true;
		return 0;
	}
	DestroyWindow();
	return 0;
}


LRESULT CDeleteClipData::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(wParam)
	{
	case 1:
		LoadItems();
		::KillTimer(m_hWnd, 1);
		break;
	}

	bHandled = FALSE;
	return 0;
}


LRESULT CDeleteClipData::OnBnClickedCheckCreateDate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_filterByCreatedDate = (::IsDlgButtonChecked(m_hWnd, IDC_CHECK_CREATE_DATE) == BST_CHECKED);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_DATE_CREATE_START), m_filterByCreatedDate);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_TIME_CREATE_START), m_filterByCreatedDate);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_DATE_CREATE_END), m_filterByCreatedDate);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_TIME_CREATE_END), m_filterByCreatedDate);
	::SetFocus(::GetDlgItem(m_hWnd, IDC_DATE_CREATE_START));	
	return 0;
}


LRESULT CDeleteClipData::OnBnClickedCheckLastUseDate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_filterByLastUsedDate = (::IsDlgButtonChecked(m_hWnd, IDC_CHECK_LAST_USE_DATE) == BST_CHECKED);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_DATE_USE_START), m_filterByLastUsedDate);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_TIME_USE_START), m_filterByLastUsedDate);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_DATE_USE_END), m_filterByLastUsedDate);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_TIME_USE_END), m_filterByLastUsedDate);
	::SetFocus(::GetDlgItem(m_hWnd, IDC_DATE_USE_START));
	return 0;
}


LRESULT CDeleteClipData::OnBnClickedCheckDataFormat(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_filterByClipboardFormat = (::IsDlgButtonChecked(m_hWnd, IDC_CHECK_DATA_FORMAT) == BST_CHECKED);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_COMBO_DATA_FORMAT), m_filterByClipboardFormat);
	::SetFocus(::GetDlgItem(m_hWnd, IDC_COMBO_DATA_FORMAT));
	return 0;
}

static bool SortByTitleDesc(const CDeleteData& a1, const CDeleteData& a2)
{
	return a1.m_Desc > a2.m_Desc;
}

static bool SortByQuickPaste(const CDeleteData& a1, const CDeleteData& a2)
{
	return a1.m_quickPasteText > a2.m_quickPasteText;
}

static bool SortByCreatedDateDesc(const CDeleteData& a1, const CDeleteData& a2)
{
	return a1.m_createdDateTime > a2.m_createdDateTime;
}

static bool SortByLastUsedDateDesc(const CDeleteData& a1, const CDeleteData& a2)
{
	return a1.m_lastUsedDateTime > a2.m_lastUsedDateTime;
}

static bool SortByFormatDesc(const CDeleteData& a1, const CDeleteData& a2)
{
	return a1.m_clipboardFormat > a2.m_clipboardFormat;
}

static bool SortByDataSizeDesc(const CDeleteData& a1, const CDeleteData& a2)
{
	return a1.m_dataSize > a2.m_dataSize;
}

static bool SortByTitleAsc(const CDeleteData& a1, const CDeleteData& a2)
{
	return a1.m_Desc < a2.m_Desc;
}

static bool SortByCreatedDateAsc(const CDeleteData& a1, const CDeleteData& a2)
{
	return a1.m_createdDateTime < a2.m_createdDateTime;
}

static bool SortByLastUsedDateAsc(const CDeleteData& a1, const CDeleteData& a2)
{
	return a1.m_lastUsedDateTime < a2.m_lastUsedDateTime;
}

static bool SortByFormatAsc(const CDeleteData& a1, const CDeleteData& a2)
{
	return a1.m_clipboardFormat < a2.m_clipboardFormat;
}

static bool SortByDataSizeAsc(const CDeleteData& a1, const CDeleteData& a2)
{
	return a1.m_dataSize < a2.m_dataSize;
}

bool desc = true;
LRESULT CDeleteClipData::OnLvnColumnclickList2(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	HD_NOTIFY *phdn = (HD_NOTIFY *)pNMHDR;

	switch (phdn->iItem)
	{
	case 0:
		if(desc)
			std::sort(m_data.begin(), m_data.end(), SortByTitleDesc);
		else
			std::sort(m_data.begin(), m_data.end(), SortByTitleAsc);
		break;
	case 1:
		if (desc)
			std::sort(m_data.begin(), m_data.end(), SortByQuickPaste);
		else
			std::sort(m_data.begin(), m_data.end(), SortByQuickPaste);
		break;
	case 2:
		if(desc)
			std::sort(m_data.begin(), m_data.end(), SortByCreatedDateDesc);
		else
			std::sort(m_data.begin(), m_data.end(), SortByCreatedDateAsc);
		break;
	case 3:
		if(desc)
			std::sort(m_data.begin(), m_data.end(), SortByLastUsedDateDesc);
		else
			std::sort(m_data.begin(), m_data.end(), SortByLastUsedDateAsc);
		break;
	case 4:
		if(desc)
			std::sort(m_data.begin(), m_data.end(), SortByFormatDesc);
		else
			std::sort(m_data.begin(), m_data.end(), SortByFormatAsc);
		break;
	case 5:
		if(desc)
			std::sort(m_data.begin(), m_data.end(), SortByDataSizeDesc);
		else
			std::sort(m_data.begin(), m_data.end(), SortByDataSizeAsc);
		break;
	}
	
	desc = !desc;

	m_List.SetItemCountEx((int)m_data.size(), 0);

	return 0;
}
