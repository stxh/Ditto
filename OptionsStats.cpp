// OptionsStats.cpp : implementation file
//

#include "stdafx.h"
#include "cp_main.h"
#include "OptionsStats.h"
#include "ProcessPaste.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <atltime.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsStats dialog

COptionsStats::COptionsStats()
{
	m_csTitle = theApp.m_Language.GetString("StatsTitle", "Stats");
	m_psp.pszTitle = m_csTitle;
	m_psp.dwFlags |= PSP_USETITLE; 

	m_eAllCopies = _T("");
	m_eAllPastes = _T("");
	m_eAllDate = _T("");
	m_eTripCopies = _T("");
	m_eTripDate = _T("");
	m_eTripPastes = _T("");
	m_eSavedCopies = _T("");
	m_eSavedCopyData = _T("");
	m_eDatabaseSize = _T("");
	m_eClipsSent = _T("");
	m_eClipsRecieved = _T("");
	m_eLastStarted = _T("");
}


/////////////////////////////////////////////////////////////////////////////
// COptionsStats message handlers


BOOL COptionsStats::OnInitDialog() 
{
	// Convert time_t values to SYSTEMTIME for display
	time_t totalDate = CGetSetOptions::GetTotalDate();
	SYSTEMTIME stTotal;
	if (totalDate != 0) {
		FILETIME ftTotal;
		LONGLONG llTotal = Int32x32To64(totalDate, 10000000) + 116444736000000000; // Convert to FILETIME
		ftTotal.dwLowDateTime = (DWORD)llTotal;
		ftTotal.dwHighDateTime = llTotal >> 32;
		FileTimeToSystemTime(&ftTotal, &stTotal);
		m_eAllDate.Format(_T("%02d/%02d/%04d"), stTotal.wMonth, stTotal.wDay, stTotal.wYear);
	} else {
		m_eAllDate = _T("");
	}

	m_eAllCopies.Format(_T("%d"), CGetSetOptions::GetTotalCopyCount());
	m_eAllPastes.Format(_T("%d"), CGetSetOptions::GetTotalPasteCount());

	time_t tripDate = CGetSetOptions::GetTripDate();
	SYSTEMTIME stTrip;
	if (tripDate != 0) {
		FILETIME ftTrip;
		LONGLONG llTrip = Int32x32To64(tripDate, 10000000) + 116444736000000000; // Convert to FILETIME
		ftTrip.dwLowDateTime = (DWORD)llTrip;
		ftTrip.dwHighDateTime = llTrip >> 32;
		FileTimeToSystemTime(&ftTrip, &stTrip);
		m_eTripDate.Format(_T("%02d/%02d/%04d"), stTrip.wMonth, stTrip.wDay, stTrip.wYear);
	} else {
		m_eTripDate = _T("");
	}

	m_eTripCopies.Format(_T("%d"), CGetSetOptions::GetTripCopyCount());
	m_eTripPastes.Format(_T("%d"), CGetSetOptions::GetTripPasteCount());

	m_eClipsSent.Format(_T("%d"), theApp.m_lClipsSent);
	m_eClipsRecieved.Format(_T("%d"), theApp.m_lClipsRecieved);

	// Format the startup time using the SYSTEMTIME value
	m_eLastStarted.Format(_T("%02d/%02d/%04d %02d:%02d:%02d"), 
		theApp.m_oldtStartUp.wMonth, theApp.m_oldtStartUp.wDay, theApp.m_oldtStartUp.wYear,
		theApp.m_oldtStartUp.wHour, theApp.m_oldtStartUp.wMinute, theApp.m_oldtStartUp.wSecond);

	// Calculate time span since startup
	SYSTEMTIME stNow;
	GetSystemTime(&stNow);

	// Convert both SYSTEMTIMEs to FILETIME for calculation
	FILETIME ftStartup, ftNow, ftDiff;
	SystemTimeToFileTime(&theApp.m_oldtStartUp, &ftStartup);
	SystemTimeToFileTime(&stNow, &ftNow);
	
	ULARGE_INTEGER uiNow, uiStart, uiDiff;
	uiNow.LowPart = ftNow.dwLowDateTime;
	uiNow.HighPart = ftNow.dwHighDateTime;
	uiStart.LowPart = ftStartup.dwLowDateTime;
	uiStart.HighPart = ftStartup.dwHighDateTime;

	// Calculate difference
	if (uiNow.QuadPart >= uiStart.QuadPart) {
		uiDiff.QuadPart = uiNow.QuadPart - uiStart.QuadPart;

		// Convert to days, hours, minutes
		ULONGLONG seconds = uiDiff.QuadPart / 10000000;  // FILETIME is in 100ns intervals
		ULONGLONG days = seconds / (24 * 3600);
		seconds %= (24 * 3600);
		ULONGLONG hours = seconds / 3600;
		seconds %= 3600;
		ULONGLONG minutes = seconds / 60;

		CString csSpan;
		csSpan.Format(_T("  -  %llu.%llu.%llu (D.H.M)"), days, hours, minutes);
		m_eLastStarted += csSpan;
	}

	try
	{
		m_eSavedCopies.Format(_T("%d"), theApp.m_db.execScalar(_T("SELECT COUNT(lID) FROM Main")));
		m_eSavedCopyData.Format(_T("%d"), theApp.m_db.execScalar(_T("SELECT COUNT(lID) FROM Data")));
	}
	CATCH_SQLITE_EXCEPTION
	
	__int64 size = FileSize(GetDBName());			

	const int MAX_FILE_SIZE_BUFFER = 255;
	TCHAR szFileSize[MAX_FILE_SIZE_BUFFER];
	StrFormatByteSize(size, szFileSize, MAX_FILE_SIZE_BUFFER);

	m_eDatabaseSize = szFileSize;

	// Set the values to the controls
	CWnd* pWnd = GetDlgItem(IDC_ALL_COPIES);
	if (pWnd) pWnd->SetWindowText(m_eAllCopies);
	pWnd = GetDlgItem(IDC_ALL_PASTES);
	if (pWnd) pWnd->SetWindowText(m_eAllPastes);
	pWnd = GetDlgItem(IDC_EDIT_ALL_DATE);
	if (pWnd) pWnd->SetWindowText(m_eAllDate);
	pWnd = GetDlgItem(IDC_TRIP_COPIES);
	if (pWnd) pWnd->SetWindowText(m_eTripCopies);
	pWnd = GetDlgItem(IDC_TRIP_DATE);
	if (pWnd) pWnd->SetWindowText(m_eTripDate);
	pWnd = GetDlgItem(IDC_TRIP_PASTES);
	if (pWnd) pWnd->SetWindowText(m_eTripPastes);
	pWnd = GetDlgItem(IDC_SAVED_COPIES);
	if (pWnd) pWnd->SetWindowText(m_eSavedCopies);
	pWnd = GetDlgItem(IDC_SAVED_COPY_DATA);
	if (pWnd) pWnd->SetWindowText(m_eSavedCopyData);
	pWnd = GetDlgItem(IDC_DATABASE_SIZE);
	if (pWnd) pWnd->SetWindowText(m_eDatabaseSize);
	pWnd = GetDlgItem(IDC_CLIPS_SENT);
	if (pWnd) pWnd->SetWindowText(m_eClipsSent);
	pWnd = GetDlgItem(IDC_CLIPS_RECIVED);
	if (pWnd) pWnd->SetWindowText(m_eClipsRecieved);
	pWnd = GetDlgItem(IDC_LAST_STARTED);
	if (pWnd) pWnd->SetWindowText(m_eLastStarted);

	theApp.m_Language.UpdateOptionStats(this);
	
	return TRUE;
}

LRESULT COptionsStats::OnResetCounts(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) 
{
	CGetSetOptions::SetTripCopyCount(0);
	CGetSetOptions::SetTripPasteCount(0);
	CGetSetOptions::SetTripDate(0);

	m_eTripDate.Empty();
	m_eTripCopies.Empty();
	m_eTripPastes.Empty();

	// Update the controls directly
	CWnd* pWnd = GetDlgItem(IDC_TRIP_DATE);
	if (pWnd) pWnd->SetWindowText(m_eTripDate);
	pWnd = GetDlgItem(IDC_TRIP_COPIES);
	if (pWnd) pWnd->SetWindowText(m_eTripCopies);
	pWnd = GetDlgItem(IDC_TRIP_PASTES);
	if (pWnd) pWnd->SetWindowText(m_eTripPastes);

	return 0;
}

LRESULT COptionsStats::OnRemoveAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) 
{
	if(MessageBox(theApp.m_Language.GetString("Remove_All", "This will remove all Copy Entries!

Continue?"), _T("Warning"), MB_YESNO) == IDYES)
	{
		if( DeleteAllIDs() )
		{
			m_eSavedCopies.Empty();
			m_eSavedCopyData.Empty();

			struct _stat buf;
			int nResult;
			nResult = STAT(GetDBName(), &buf);
			if(nResult == 0)
			{
				m_eDatabaseSize.Format(_T("%d KB"), (buf.st_size/1024));
				
				// Update the control directly
				CWnd* pWnd = GetDlgItem(IDC_DATABASE_SIZE);
				if (pWnd) pWnd->SetWindowText(m_eDatabaseSize);
			}
		}
	}
	return 0;
}
