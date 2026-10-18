#include "stdafx.h"
#include "cp_main.h"
#include "MoveToGroupDlg.h"
#include "GroupName.h"
#include "ProcessPaste.h"

CMoveToGroupDlg::CMoveToGroupDlg()
    : m_nSelectedGroup(-1)
{
}

LRESULT CMoveToGroupDlg::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
    HWND hTree = GetDlgItem(IDC_TREE);
    if (hTree != nullptr)
    {
        m_Tree.SubclassWindow(hTree);
    }

    m_Tree.m_selectedFolderID = m_nSelectedGroup;
    m_Tree.SetNotificationWndEx(m_hWnd);
    m_Tree.FillTree();

    if (!m_windowTitle.IsEmpty())
        SetWindowText(m_windowTitle);
    else
        theApp.m_Language.UpdateMoveToGroups(m_hWnd);

    return TRUE;
}

LRESULT CMoveToGroupDlg::OnOk(WORD, WORD, HWND, BOOL&)
{
    m_nSelectedGroup = m_Tree.GetSelectedTree();
    EndDialog(IDOK);
    return 0;
}

LRESULT CMoveToGroupDlg::OnCancel(WORD, WORD, HWND, BOOL&)
{
    EndDialog(IDCANCEL);
    return 0;
}

void CMoveToGroupDlg::OnSize(UINT nType, CSize size)
{
    UNREFERENCED_PARAMETER(nType);
    if (m_Tree.IsWindow())
    {
        m_Tree.SetWindowPos(nullptr, 0, 0, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

LRESULT CMoveToGroupDlg::OnButtonNewGroup(WORD, WORD, HWND, BOOL&)
{
    CGroupName nameDlg;
    if (nameDlg.DoModal() != IDOK)
    {
        return 0;
    }

    CString csName = nameDlg.m_csName;

    long lID = NewGroupID(m_Tree.GetSelectedTree(), csName);
    if (lID >= 0)
    {
        m_Tree.AddNode(csName, static_cast<int>(lID));
    }

    m_Tree.SetFocus();
    return 0;
}

LRESULT CMoveToGroupDlg::OnTreeSelect(UINT, WPARAM wParam, LPARAM, BOOL&)
{
    int nID = static_cast<int>(wParam);
    if (nID != 0)
    {
        m_nSelectedGroup = nID;
        EndDialog(IDOK);
    }
    else
    {
        EndDialog(IDCANCEL);
    }

    return 0;
}