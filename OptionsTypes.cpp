#include "stdafx.h"
#include "CP_Main.h"
#include "OptionsTypes.h"
#include "Shared/ArrayEx.h"
#include "DimWnd.h"
#include "Misc.h"

COptionsTypes::COptionsTypes()
{
	m_csTitle = theApp.m_Language.GetString("SupportedTypesTitle", "Supported Types");
	m_psp.pszTitle = m_csTitle;
	m_psp.dwFlags |= PSP_USETITLE;
	m_bSave = false;
}

int COptionsTypes::OnApply() 
{
	if(m_bSave)
	{
		try
		{
			theApp.m_db.execDML(_T("DELETE FROM Types;"));

			CString csText;
			int nCount = m_List.GetCount();
			for(int i = 0; i < nCount; i++)
			{
				m_List.GetText(i, csText);

				theApp.m_db.execDMLEx(_T("INSERT INTO Types VALUES(NULL, '%s');"), csText);
			}
		}
		CATCH_SQLITE_EXCEPTION

		// refresh our local cache
		theApp.ReloadTypes();
	}
	
	return PSNRET_NOERROR;
}

LRESULT COptionsTypes::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&) 
{
	m_List.Attach(GetDlgItem(IDC_LIST1));

	try
	{
		CppSQLite3Query q = theApp.m_db.execQuery(_T("SELECT TypeText FROM Types"));
		if(q.eof())
		{
			m_List.AddString(_T("CF_TEXT"));
			m_List.AddString(GetFormatName(RegisterClipboardFormat(CF_RTF)));
			m_List.AddString(_T("CF_UNICODETEXT"));
			m_List.AddString(_T("CF_HDROP"));
			m_List.AddString(_T("CF_DIB"));
			m_List.AddString(GetFormatName(GetFormatID(_T("HTML Format"))));
			m_List.AddString(GetFormatName(GetFormatID(_T("PNG"))));
		}

		while(q.eof() == false)
		{
			m_List.AddString(q.getStringField(0));

			q.nextRow();
		}
	}
	CATCH_SQLITE_EXCEPTION
	
	m_List.SetFocus();

	theApp.m_Language.UpdateOptionSupportedTypes(m_hWnd);
	
	return 0;
}

LRESULT COptionsTypes::OnDelete(WORD, WORD, HWND, BOOL&) 
{
	int nCount = m_List.GetSelCount();
	if(nCount)
	{
		m_bSave = true;
		CArrayEx<int> items;
		items.SetSize(nCount);
		m_List.GetSelItems(nCount, items.GetData()); 
		items.SortDescending();

		for(int i = 0; i < nCount; i++)
			m_List.DeleteString(items[i]);
	}
	return 0;
}

#include "AddType.h"
LRESULT COptionsTypes::OnAdd(WORD, WORD, HWND, BOOL&) 
{
	CDimWnd dim(this->GetParent().m_hWnd);
	CAddType add;

	if(add.DoModal(m_hWnd) == IDOK)
	{
		int nCount = (int)add.m_csSelectedTypes.size();
		if(nCount)
		{
			m_bSave = true;
			for(int i = 0; i < nCount; i++)
			{
				if(TextAllReadyThere(add.m_csSelectedTypes[i]) == FALSE)
					m_List.AddString(add.m_csSelectedTypes[i]);
			}
		}
	}
	return 0;
}

BOOL COptionsTypes::TextAllReadyThere(const CString &cs)
{
	CString csThere;
	int nCount = m_List.GetCount();

	for(int i = 0; i < nCount; i++)
	{
		m_List.GetText(i, csThere);
		if(cs == csThere)
			return TRUE;
	}

	return FALSE;
}
