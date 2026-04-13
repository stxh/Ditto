#if !defined(AFX_ABOUT_H__02A95202_4AEF_4854_9DC0_161326D7B914__INCLUDED_)
#define AFX_ABOUT_H__02A95202_4AEF_4854_9DC0_161326D7B914__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// About.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAbout dialog

#include "hyperlink.h"
#include "HListBox.h"

class CAbout : public CPropertyPageImpl<CAbout>
{
public:
	CAbout();
	~CAbout();

// Dialog Data
	enum { IDD = IDD_ABOUT };
	CHyperLink	m_Link;
	CHyperLink	m_HyperLink;
	CHListBox	m_List;

// Implementation
protected:
	CString m_csTitle;

	// WTL Message Map
	BEGIN_MSG_MAP(CAbout)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(CPropertyPageImpl<CAbout>)
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUT_H__02A95202_4AEF_4854_9DC0_161326D7B914__INCLUDED_)
