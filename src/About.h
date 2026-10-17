#pragma once

// About.h : header file

#include <atlctrlx.h>
#include "HListBox.h"

class CAbout : public CPropertyPageImpl<CAbout>
{
public:
	CAbout();
	~CAbout();

	enum { IDD = IDD_ABOUT };
	CHyperLink	m_Link;
	CHyperLink	m_HyperLink;
	CHListBox	m_List;

protected:
	CString m_csTitle;

	BEGIN_MSG_MAP(CAbout)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(CPropertyPageImpl<CAbout>)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
