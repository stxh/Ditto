// SelectPasteFormat.cpp : implementation file
//

#include "stdafx.h"
#include "SelectPasteFormat.h"

CSelectPasteFormat::CSelectPasteFormat(HWND hParent, IClipFormats* clipFormats)
    : m_hParent(hParent), m_pClipFormats(clipFormats), m_selectedFormat(0), m_pasteAsUnicode(false)
{
}

CSelectPasteFormat::~CSelectPasteFormat()
{
    if(m_Formats.m_hWnd != NULL)
    {
        m_Formats.Detach();
    }
}

LRESULT CSelectPasteFormat::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
    m_Formats.Attach(GetDlgItem(IDC_LIST1));

    if (m_pClipFormats != NULL)
    {
        int count = m_pClipFormats->Size();
        for (int i = 0; i < count; i++)
        {
            IClipFormat* pFormat = m_pClipFormats->GetAt(i);
            CString formatName = DittoAddinHelpers::GetFormatName(pFormat->Type());
            int pos = m_Formats.AddString(formatName);
            m_Formats.SetItemData(pos, pFormat->Type());
        }
    }

    m_Resize.SetParent(m_hWnd);
    m_Resize.AddControl(IDC_LIST1, DR_SizeHeight | DR_SizeWidth);
    m_Resize.AddControl(IDOK, DR_MoveTop | DR_MoveLeft);
    m_Resize.AddControl(IDCANCEL, DR_MoveTop | DR_MoveLeft);
    m_Resize.AddControl(IDC_CHECK_PASTE_AS_UNICODE, DR_MoveTop);

    CenterWindow(m_hParent);

    return TRUE;
}

LRESULT CSelectPasteFormat::OnOK(WORD, WORD, HWND, BOOL&)
{
    int pos = m_Formats.GetCurSel();
    if (pos >= 0 && pos < m_Formats.GetCount())
    {
        m_selectedFormat = static_cast<CLIPFORMAT>(m_Formats.GetItemData(pos));
    }

    m_pasteAsUnicode = (IsDlgButtonChecked(IDC_CHECK_PASTE_AS_UNICODE) == BST_CHECKED);

    EndDialog(IDOK);
    return 0;
}

LRESULT CSelectPasteFormat::OnCancel(WORD, WORD, HWND, BOOL&)
{
    EndDialog(IDCANCEL);
    return 0;
}

LRESULT CSelectPasteFormat::OnLbnDblclkList1(WORD, WORD, HWND, BOOL& bHandled)
{
    OnOK(0, IDOK, NULL, bHandled);
    return 0;
}

LRESULT CSelectPasteFormat::OnSize(UINT, WPARAM wParam, LPARAM lParam, BOOL&)
{
    int cx = LOWORD(lParam);
    int cy = HIWORD(lParam);
    m_Resize.MoveControls(CSize(cx, cy));
    return 0;
}