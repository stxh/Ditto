// AddType.cpp : implementation file
//

#include "stdafx.h"
#include "cp_main.h"
#include "Misc.h"
#include "AddType.h"

/////////////////////////////////////////////////////////////////////////////
// CAddType dialog

/////////////////////////////////////////////////////////////////////////////
// CAddType message handlers
LRESULT CAddType::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
{
	
	::CheckDlgButton(m_hWnd, IDC_RADIO_CURRENT_TYPES, BST_CHECKED);
	OnBnClickedRadioCurrentTypes();
	
	m_lbCandidateTypes.SetFocus();

	theApp.m_Language.UpdateOptionSupportedTypesAdd(this);
	bHandled = FALSE;
	return FALSE;
}

void CAddType::AddCurrentClipboardTypes()
{
	m_lbCandidateTypes.ResetContent();

	// Open clipboard to enumerate formats
	if (OpenClipboard())
	{
		UINT format = 0;
		while ((format = EnumClipboardFormats(format)) != 0)
		{
			CString formatName = GetFormatName(format);
			if (!formatName.IsEmpty())
			{
				m_lbCandidateTypes.AddString(formatName);
			}
		}
		CloseClipboard();
	}
}

void CAddType::AddCommonTypes()
{
	m_lbCandidateTypes.ResetContent();
	for (auto systemClipFormat : GetSystemClipFormats()) {
		m_lbCandidateTypes.AddString(GetFormatName(systemClipFormat));
	}
	m_lbCandidateTypes.AddString(GetFormatName(RegisterClipboardFormat(CF_RTF)));
	m_lbCandidateTypes.AddString(GetFormatName(RegisterClipboardFormat(CF_RTFNOOBJS)));
	m_lbCandidateTypes.AddString(GetFormatName(RegisterClipboardFormat(CF_RETEXTOBJ)));
	m_lbCandidateTypes.AddString(GetFormatName(RegisterClipboardFormat(_T("HTML Format"))));
}

LRESULT CAddType::OnBnClickedRadioPrimaryTypes(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	AddCommonTypes();
	::ShowWindow(::GetDlgItem(m_hWnd, IDC_LIST1), SW_SHOW);
	::ShowWindow(::GetDlgItem(m_hWnd, IDC_EDIT1), SW_HIDE);
	m_lbCandidateTypes.SetFocus();
	if (m_lbCandidateTypes.GetCount() > 0)
	{
		m_lbCandidateTypes.SetCurSel(0);
		m_lbCandidateTypes.SetSel(0);
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CAddType::OnBnClickedRadioCurrentTypes(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	AddCurrentClipboardTypes();
	::ShowWindow(::GetDlgItem(m_hWnd, IDC_LIST1), SW_SHOW);
	::ShowWindow(::GetDlgItem(m_hWnd, IDC_EDIT1), SW_HIDE);
	m_lbCandidateTypes.SetFocus();
	if (m_lbCandidateTypes.GetCount() > 0)
	{
		m_lbCandidateTypes.SetCurSel(0);
		m_lbCandidateTypes.SetSel(0);

	}
	bHandled = TRUE;
	return 0;
}

LRESULT CAddType::OnBnClickedRadioCustomType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	::ShowWindow(::GetDlgItem(m_hWnd, IDC_EDIT1), SW_SHOW);
	::ShowWindow(::GetDlgItem(m_hWnd, IDC_LIST1), SW_HIDE);
	::SetFocus(::GetDlgItem(m_hWnd, IDC_EDIT1));
	bHandled = TRUE;
	return 0;
}

LRESULT CAddType::OnBnClickedAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_csSelectedTypes.RemoveAll();

	if (::SendDlgItemMessage(m_hWnd, IDC_RADIO_PRIMARY_TYPES, BM_GETCHECK, 0, 0) == BST_CHECKED ||
		::SendDlgItemMessage(m_hWnd, IDC_RADIO_CURRENT_TYPES, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		int nCount = m_lbCandidateTypes.GetSelCount();
		if (nCount)
		{
			CString cs;
			CArray<int, int> items;
			items.SetSize(nCount);
			m_lbCandidateTypes.GetSelItems(nCount, items.GetData());

			for (int i = 0; i < nCount; i++)
			{
				m_lbCandidateTypes.GetText(items[i], cs);
				m_csSelectedTypes.Add(cs);
			}
		}
	}
	else if (::SendDlgItemMessage(m_hWnd, IDC_RADIO_CUSTOM_TYPE, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		// Get text from edit control
		::GetDlgItemText(m_hWnd, IDC_EDIT1, m_eCustomType.GetBuffer(50), 50);
		m_eCustomType.ReleaseBuffer();
		m_csSelectedTypes.Add(m_eCustomType);
	}

	EndDialog(IDOK);
	bHandled = TRUE;
	return 0;
}


LRESULT CAddType::OnLbnDblclkList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	OnBnClickedAdd();
	bHandled = TRUE;
	return 0;
}
