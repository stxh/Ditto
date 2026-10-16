> **历史文档 / HISTORICAL**：本文记录的是已放弃的 MFC 仿真 shim 方案（路线 A），其结论、类名与步骤均与当前代码不符。当前迁移状态见 [WTL_Migration_Status.md](WTL_Migration_Status.md)。

# Remaining WTL Conversions - Quick Reference

## ✅ COMPLETED (10 classes)

### Dialogs (6)
1. CGroupName
2. CMoveToGroupDlg
3. CFriendDetails
4. CFileTransferProgressDlg
5. CFriendPromptDlg
6. GlobalClips

### Property Pages (4)
1. OptionsCopyBuffers (already WTL)
2. OptionsStats (already WTL)
3. COptionsUtilities
4. COptionsKeyBoard

---

## 🚧 REMAINING - Priority Order

### HIGH PRIORITY: Property Pages (5 classes)

Follow the exact pattern from **OptionsUtilities.h/cpp** or **OptionsKeyBoard.h/cpp**

#### 1. OptionsTypes (NEXT)
**Files**: OptionsTypes.h/cpp
**Complexity**: Low
**Pattern**: Simple list box operations
**Key Changes**:
- Change base: `CPropertyPage` → `CPropertyPageImpl<COptionsTypes>`
- Remove `IMPLEMENT_DYNCREATE`, `DoDataExchange`
- Message map: `BEGIN_MESSAGE_MAP` → `BEGIN_MSG_MAP` + `CHAIN_MSG_MAP`
- OnApply: `BOOL` → `int`, return `PSNRET_NOERROR`
- OnInitDialog: `BOOL` → `LRESULT`, return `0`
- Attach controls manually in OnInitDialog

#### 2. OptionsQuickPaste
**Files**: OptionsQuickPaste.h/cpp
**Complexity**: Low-Medium
**Pattern**: Standard property page with check boxes and edit controls

#### 3. OptionsGeneral  
**Files**: OptionsGeneral.h/cpp
**Complexity**: Medium
**Pattern**: Many controls, standard operations

#### 4. OptionFriends
**Files**: OptionFriends.h/cpp
**Complexity**: Medium
**Pattern**: List control with friend management

#### 5. QuickPasteKeyboard
**Files**: QuickPasteKeyboard.h/cpp
**Complexity**: Low-Medium
**Pattern**: Keyboard configuration

### MEDIUM PRIORITY: Dialog (1 class)

#### ScriptEditor
**Files**: ScriptEditor.h/cpp
**Complexity**: Medium
**Components**: CRichEditCtrl, CListBox
**Follow Pattern**: FileTransferProgressDlg.h/cpp
**Key Changes**:
- Base class: `CDialogEx` → `CDialogImpl<CScriptEditor>`
- Remove MFC message map → WTL message map
- Remove DoDataExchange → Manual Attach in OnInitDialog
- PreTranslateMessage: Keep as-is (compatible with WTL)

### BLOCKED: Complex Dialogs (1 class)

#### AdvGeneral ⚠️
**Files**: AdvGeneral.h/cpp
**Issue**: Uses `CMFCPropertyGridCtrl` (MFC-only control)
**Options**:
1. Keep as MFC (hybrid approach) - RECOMMENDED
2. Replace with alternative WTL property grid
3. Redesign UI

**Recommendation**: Leave as MFC for now. Focus on completing other conversions first.

---

## Quick Conversion Template

### For CPropertyPage Classes:

**Header Pattern:**
```cpp
#pragma once
#include <atlbase.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>

class CMyPage : public CPropertyPageImpl<CMyPage>
{
public:
    enum { IDD = IDD_MY_DIALOG };
    
    CMyPage() { /* init members */ }
    
    BEGIN_MSG_MAP(CMyPage)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDC_BUTTON, OnButton)
        // ... other handlers
        CHAIN_MSG_MAP(CPropertyPageImpl<CMyPage>)  // CRITICAL!
    END_MSG_MAP()
    
    LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnButton(WORD, WORD, HWND, BOOL&);
    
    int OnApply() override;
    
private:
    CEdit m_edit;
    CButton m_check;
    // ... member variables
};
```

**CPP Pattern:**
```cpp
#include "stdafx.h"
#include "CP_Main.h"
#include "MyPage.h"

CMyPage::CMyPage()
{
    // Initialize title if needed
    m_csTitle = _T("My Title");
    m_psp.pszTitle = m_csTitle;
    m_psp.dwFlags |= PSP_USETITLE;
}

LRESULT CMyPage::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
    // Attach all controls
    m_edit.Attach(GetDlgItem(IDC_EDIT1));
    m_check.Attach(GetDlgItem(IDC_CHECK1));
    
    // Set initial values
    m_edit.SetWindowText(m_text);
    CheckDlgButton(IDC_CHECK1, m_bChecked ? BST_CHECKED : BST_UNCHECKED);
    
    return 0;  // WTL: return 0, not TRUE/FALSE
}

int CMyPage::OnApply()
{
    // Get values from controls
    m_edit.GetWindowText(m_text);
    m_bChecked = (IsDlgButtonChecked(IDC_CHECK1) == BST_CHECKED);
    
    // Save settings
    CGetSetOptions::SetSomething(m_text);
    
    return PSNRET_NOERROR;  // WTL: not TRUE/FALSE
}

LRESULT CMyPage::OnButton(WORD, WORD, HWND, BOOL&)
{
    // Handle button click
    return 0;
}
```

### Key Conversions:

| MFC | WTL |
|-----|-----|
| `IMPLEMENT_DYNCREATE(CMyPage, CPropertyPage)` | Remove |
| `CMyPage() : CPropertyPage(IDD)` | `CMyPage()` (no base init) |
| `void DoDataExchange(CDataExchange*)` | Remove entirely |
| `DDX_Control(pDX, IDC_EDIT, m_edit)` | `m_edit.Attach(GetDlgItem(IDC_EDIT))` in OnInitDialog |
| `UpdateData(TRUE)` | Manual `GetWindowText()` calls |
| `UpdateData(FALSE)` | Manual `SetWindowText()` calls |
| `BOOL OnInitDialog()` + `return TRUE` | `LRESULT OnInitDialog(...)` + `return 0` |
| `BOOL OnApply()` + `return CPropertyPage::OnApply()` | `int OnApply()` + `return PSNRET_NOERROR` |
| `BEGIN_MESSAGE_MAP(CMyPage, CPropertyPage)` | `BEGIN_MSG_MAP(CMyPage)` + `CHAIN_MSG_MAP(...)` |

---

## Testing Checklist

After each conversion:
- [ ] File compiles without errors
- [ ] No MFC-specific code remains (`afx_msg`, `DoDataExchange`, etc.)
- [ ] All controls are attached in OnInitDialog
- [ ] Message map includes `CHAIN_MSG_MAP` for property pages
- [ ] OnApply returns `PSNRET_NOERROR` not BOOL
- [ ] OnInitDialog returns `0` not TRUE/FALSE
- [ ] Test dialog/page functionality if possible

---

## Estimated Time Remaining

Based on patterns established:
- **Simple Property Page**: 10-15 minutes each
- **Medium Property Page**: 20-30 minutes each  
- **Dialog (ScriptEditor)**: 30-40 minutes
- **Total for remaining high priority**: ~2-3 hours

## Next Steps

1. Convert OptionsTypes (simplest remaining)
2. Convert OptionsQuickPaste
3. Convert OptionsGeneral
4. Convert OptionFriends
5. Convert QuickPasteKeyboard
6. Convert ScriptEditor
7. Document AdvGeneral as "MFC Required" in status
8. Build and test all changes
9. Update final statistics

---

## Build Commands

```bash
# Check for remaining MFC references
rg "CDialog[^I]|CPropertyPage[^I]" --type h -l
rg "BEGIN_MESSAGE_MAP|DoDataExchange" --type cpp -l
rg "afx_msg|DECLARE_MESSAGE_MAP" --type h -l

# Build project
msbuild CP_Main.sln /p:Configuration=Release /p:Platform=x64
```
