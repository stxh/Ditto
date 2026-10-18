// About.cpp : implementation file
// test test test

//Main branch test

#include "stdafx.h"
#include "cp_main.h"
#include "About.h"
#include "misc.h"

/////////////////////////////////////////////////////////////////////////////
// CAbout property page
CAbout::CAbout() : CPropertyPageImpl<CAbout>(CAbout::IDD)
{
	m_csTitle = theApp.m_Language.GetString("AboutTitle", "About");
	m_psp.pszTitle = m_csTitle;
	m_psp.dwFlags |= PSP_USETITLE;
}
CAbout::~CAbout()
{
}

/////////////////////////////////////////////////////////////////////////////
// CAbout message handlers

LRESULT CAbout::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_List.Attach(GetDlgItem(IDC_LIST));
	// CHyperLink is CWindowImpl-derived, so it must own a thunk; Attach() would leave
	// the handle set when the page dies and assert in ~CWindowImplRoot.
	m_HyperLink.SubclassWindow(GetDlgItem(IDC_HYPER_LINK));
	m_Link.SubclassWindow(GetDlgItem(IDC_STATIC_LINK));

	m_List.AddString(_T("DittoWTL"));

	auto runningVersion = GetRunningVersion();
	CString cs = GetVersionString(runningVersion);

	CString csText;
	csText = "    Version " + cs;
#if _WIN64
	csText += " 64bit";
#else
	csText += " 32bit";
#endif
	m_List.AddString(csText);

	const char *SqliteVersion = sqlite3_libversion();
	csText = "    Sqlite Version ";
	csText += SqliteVersion;
	m_List.AddString(csText);

	cs = CGetSetOptions::GetExeFileName();;
	csText = "    Exe Path " + cs;
	m_List.AddString(csText);

	cs = CGetSetOptions::GetDBPath();
	csText = "    DB Path " + cs;
	m_List.AddString(csText);

	m_List.AddString(_T(""));

	m_List.AddString(_T("Credits"));
	
	cs = "    Authors - Scott Brogden, sabrogden@users.sourceforge.net";
	m_List.AddString(cs);
	cs = "            - Kevin Edwards, ingenuus@users.sourceforge.net";
	m_List.AddString(cs);

	m_List.AddString(_T(""));

	m_List.AddString(_T("Addins"));

	//Show what addins are loaded
	std::vector<CString> arr;
	theApp.m_Addins.AboutScreenText(arr);
	int count = (int)arr.size();
	for(int i = 0; i < count; i++)
	{
		m_List.AddString(_T("    ") + arr[i]);
	}

	CRect rect;
	GetClientRect(&rect);	
	rect.bottom -= 30;
	
	m_List.MoveWindow(&rect);

	rect.top = rect.bottom + 10;
	rect.bottom = rect.top + 30;
	
	m_HyperLink.MoveWindow(&rect);

	m_HyperLink.SetHyperLink(_T("mailto:sabrogden@gmail.com"));

	rect.top = rect.bottom + 5;
	rect.bottom = rect.top + 5;
	m_Link.MoveWindow(&rect);
	m_Link.SetHyperLink(_T("https://ditto-cp.sourceforge.io/"));

	bHandled = FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
