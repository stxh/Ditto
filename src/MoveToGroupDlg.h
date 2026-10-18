#pragma once

#include "GroupTree.h"

class CMoveToGroupDlg : public CDialogImpl<CMoveToGroupDlg>
{
public:
    enum { IDD = IDD_MOVE_TO_GROUP };

    CMoveToGroupDlg(CString windowTitle = _T(""));

    int GetSelectedGroup() const { return m_nSelectedGroup; }
    void SetSelectedGroup(int nGroup) { m_nSelectedGroup = nGroup; }
    void SetSlectedGroup(int nGroup) { SetSelectedGroup(nGroup); }

    BEGIN_MSG_MAP(CMoveToGroupDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnOk)
        COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
        COMMAND_HANDLER(IDC_BUTTON_NEW_GROUP, BN_CLICKED, OnButtonNewGroup)
        MSG_WM_SIZE(OnSize)
        MESSAGE_HANDLER(NM_GROUP_TREE_MESSAGE, OnTreeSelect)
    END_MSG_MAP()

    LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnOk(WORD, WORD, HWND, BOOL&);
    LRESULT OnCancel(WORD, WORD, HWND, BOOL&);
    void OnSize(UINT nType, CSize size);
    LRESULT OnButtonNewGroup(WORD, WORD, HWND, BOOL&);
    LRESULT OnTreeSelect(UINT, WPARAM, LPARAM, BOOL&);

private:
    CGroupTree m_Tree;
    int m_nSelectedGroup;
    CString m_windowTitle;
};
