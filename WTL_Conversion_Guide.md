> **历史文档 / HISTORICAL**：本文记录的是已放弃的 MFC 仿真 shim 方案（路线 A），其结论、类名与步骤均与当前代码不符。当前迁移状态见 [WTL_Migration_Status.md](WTL_Migration_Status.md)。

# WTL Conversion Guide - Step-by-Step Instructions

## Overview
This guide provides detailed patterns for converting MFC classes to WTL in the Ditto project.

## Completed Conversions (Examples to Follow)

### 1. CDialog/CDialogEx to CDialogImpl

**Example**: FileTransferProgressDlg.h/cpp

#### Header File Pattern:
```cpp
// OLD MFC
class CMyDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CMyDialog)
public:
    CMyDialog(CWnd* pParent = NULL);
    virtual ~CMyDialog();
    
    enum { IDD = IDD_MY_DIALOG };
    
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
    
public:
    afx_msg void OnButton();
    virtual BOOL OnInitDialog();
    
private:
    CEdit m_edit;
    CString m_text;
};

// NEW WTL
#include <atlbase.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>

class CMyDialog : public CDialogImpl<CMyDialog>
{
public:
    enum { IDD = IDD_MY_DIALOG };
    
    CMyDialog() : m_text(_T("")) {}
    
    BEGIN_MSG_MAP(CMyDialog)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDC_BUTTON, OnButton)
    END_MSG_MAP()
    
    LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnButton(WORD, WORD, HWND, BOOL&);
    
private:
    CEdit m_edit;
    CString m_text;
};
```

#### CPP File Pattern:
```cpp
// OLD MFC
CMyDialog::CMyDialog(CWnd* pParent)
    : CDialogEx(IDD, pParent)
{
    m_text = _T("");
}

void CMyDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT1, m_edit);
    DDX_Text(pDX, IDC_EDIT1, m_text);
}

BEGIN_MESSAGE_MAP(CMyDialog, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON, &CMyDialog::OnButton)
END_MESSAGE_MAP()

BOOL CMyDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    // initialization
    return TRUE;
}

void CMyDialog::OnButton()
{
    UpdateData(TRUE);  // Get data from controls
    // do something
    UpdateData(FALSE); // Set data to controls
    CDialogEx::OnOK();
}

// NEW WTL
LRESULT CMyDialog::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
    m_edit.Attach(GetDlgItem(IDC_EDIT1));
    m_edit.SetWindowText(m_text);
    return 0;
}

LRESULT CMyDialog::OnButton(WORD, WORD, HWND, BOOL&)
{
    m_edit.GetWindowText(m_text);
    // do something
    EndDialog(IDOK);
    return 0;
}
```

### 2. CPropertyPage to CPropertyPageImpl

**Example**: OptionsUtilities.h/cpp

#### Header Pattern:
```cpp
// OLD MFC
class CMyPropertyPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CMyPropertyPage)
public:
    CMyPropertyPage();
    ~CMyPropertyPage();
    
    enum { IDD = IDD_MY_PAGE };
    
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnApply();
    DECLARE_MESSAGE_MAP()
    
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnButton();
    
private:
    CButton m_check;
    BOOL m_bChecked;
};

// NEW WTL
#include <atlbase.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>

class CMyPropertyPage : public CPropertyPageImpl<CMyPropertyPage>
{
public:
    enum { IDD = IDD_MY_PAGE };
    
    CMyPropertyPage() : m_bChecked(FALSE) {}
    
    BEGIN_MSG_MAP(CMyPropertyPage)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDC_BUTTON, OnButton)
        CHAIN_MSG_MAP(CPropertyPageImpl<CMyPropertyPage>)  // IMPORTANT!
    END_MSG_MAP()
    
    LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnButton(WORD, WORD, HWND, BOOL&);
    
    int OnApply() override;  // Property page specific
    
private:
    CButton m_check;
    BOOL m_bChecked;
};
```

#### CPP Pattern:
```cpp
// OLD MFC
IMPLEMENT_DYNCREATE(CMyPropertyPage, CPropertyPage)

CMyPropertyPage::CMyPropertyPage() 
    : CPropertyPage(IDD)
{
    m_bChecked = FALSE;
}

void CMyPropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHECK1, m_check);
    DDX_Check(pDX, IDC_CHECK1, m_bChecked);
}

BEGIN_MESSAGE_MAP(CMyPropertyPage, CPropertyPage)
    ON_BN_CLICKED(IDC_BUTTON, OnButton)
END_MESSAGE_MAP()

BOOL CMyPropertyPage::OnInitDialog()
{
    CPropertyPage::OnInitDialog();
    UpdateData(FALSE);
    return TRUE;
}

BOOL CMyPropertyPage::OnApply()
{
    UpdateData(TRUE);
    CGetSetOptions::SetOption(m_bChecked);
    return CPropertyPage::OnApply();
}

void CMyPropertyPage::OnButton()
{
    // handler
}

// NEW WTL
LRESULT CMyPropertyPage::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
    m_check.Attach(GetDlgItem(IDC_CHECK1));
    CheckDlgButton(IDC_CHECK1, m_bChecked ? BST_CHECKED : BST_UNCHECKED);
    return 0;
}

int CMyPropertyPage::OnApply()
{
    m_bChecked = (IsDlgButtonChecked(IDC_CHECK1) == BST_CHECKED);
    CGetSetOptions::SetOption(m_bChecked);
    return PSNRET_NOERROR;  // or PSNRET_INVALID to prevent page close
}

LRESULT CMyPropertyPage::OnButton(WORD, WORD, HWND, BOOL&)
{
    // handler
    return 0;
}
```

## Common Conversion Patterns

### Message Map Entries

| MFC | WTL |
|-----|-----|
| `BEGIN_MESSAGE_MAP(CMyClass, CBaseClass)` | `BEGIN_MSG_MAP(CMyClass)` |
| `ON_BN_CLICKED(id, &CMyClass::Handler)` | `COMMAND_ID_HANDLER(id, Handler)` |
| `ON_COMMAND(id, &CMyClass::Handler)` | `COMMAND_ID_HANDLER(id, Handler)` |
| `ON_MESSAGE(msg, Handler)` | `MESSAGE_HANDLER(msg, Handler)` |
| `ON_NOTIFY(code, id, Handler)` | `NOTIFY_HANDLER(id, code, Handler)` |
| `ON_WM_SIZE()` | `MESSAGE_HANDLER(WM_SIZE, OnSize)` or `MSG_WM_SIZE(OnSize)` |
| `ON_WM_PAINT()` | `MESSAGE_HANDLER(WM_PAINT, OnPaint)` or `MSG_WM_PAINT(OnPaint)` |
| `END_MESSAGE_MAP()` | `END_MSG_MAP()` |

### Handler Signatures

| MFC | WTL |
|-----|-----|
| `void OnButton()` | `LRESULT OnButton(WORD, WORD, HWND, BOOL&)` |
| `BOOL OnInitDialog()` | `LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)` |
| `void OnSize(UINT, int, int)` | `LRESULT OnSize(UINT, WPARAM, LPARAM, BOOL&)` |
| `void OnPaint()` | `LRESULT OnPaint(UINT, WPARAM, LPARAM, BOOL&)` |

### Return Values

| MFC | WTL Dialog | WTL PropertyPage |
|-----|-----------|------------------|
| `TRUE` from `OnInitDialog` | `return 0` | `return 0` |
| `FALSE` from `OnInitDialog` | `return 0` (set focus manually) | `return 0` |
| `CDialog::OnOK()` | `EndDialog(IDOK)` | N/A |
| `CDialog::OnCancel()` | `EndDialog(IDCANCEL)` | N/A |
| `CPropertyPage::OnApply()` returning `TRUE` | N/A | `return PSNRET_NOERROR` |
| `CPropertyPage::OnApply()` returning `FALSE` | N/A | `return PSNRET_INVALID` |

### DDX Replacement

#### Edit Controls
```cpp
// MFC
DDX_Text(pDX, IDC_EDIT1, m_text);
DDX_Control(pDX, IDC_EDIT1, m_edit);

// WTL - In OnInitDialog:
m_edit.Attach(GetDlgItem(IDC_EDIT1));
m_edit.SetWindowText(m_text);

// WTL - In OnApply/OnOK:
m_edit.GetWindowText(m_text);
```

#### Check Boxes
```cpp
// MFC
DDX_Check(pDX, IDC_CHECK1, m_bChecked);

// WTL - In OnInitDialog:
CheckDlgButton(IDC_CHECK1, m_bChecked ? BST_CHECKED : BST_UNCHECKED);

// WTL - In OnApply/OnOK:
m_bChecked = (IsDlgButtonChecked(IDC_CHECK1) == BST_CHECKED);
```

#### Radio Buttons
```cpp
// MFC
DDX_Radio(pDX, IDC_RADIO1, m_nRadio);

// WTL - In OnInitDialog:
CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO1 + m_nRadio);

// WTL - In OnApply/OnOK:
if (IsDlgButtonChecked(IDC_RADIO1)) m_nRadio = 0;
else if (IsDlgButtonChecked(IDC_RADIO2)) m_nRadio = 1;
// etc.
```

#### Combo Boxes
```cpp
// MFC
DDX_CBString(pDX, IDC_COMBO1, m_text);
DDX_CBIndex(pDX, IDC_COMBO1, m_index);

// WTL - In OnInitDialog:
m_combo.Attach(GetDlgItem(IDC_COMBO1));
m_combo.SetCurSel(m_index);
// or
m_combo.SelectString(-1, m_text);

// WTL - In OnApply/OnOK:
m_index = m_combo.GetCurSel();
// or
m_combo.GetLBText(m_combo.GetCurSel(), m_text);
```

#### List Controls
```cpp
// MFC
DDX_Control(pDX, IDC_LIST1, m_list);
// m_list is CListCtrl

// WTL
// m_list is CListViewCtrl
m_list.Attach(GetDlgItem(IDC_LIST1));
m_list.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
m_list.InsertColumn(0, _T("Column"), LVCFMT_LEFT, 100, 0);
```

## Remaining Files to Convert

### Priority 1: Property Pages (Simple Pattern)

1. **OptionsKeyBoard.h/cpp**
2. **OptionsTypes.h/cpp**
3. **OptionsQuickPaste.h/cpp**
4. **OptionsGeneral.h/cpp**
5. **OptionFriends.h/cpp**
6. **QuickPasteKeyboard.h/cpp**

**Steps for each**:
1. Change base class from `CPropertyPage` to `CPropertyPageImpl<ClassName>`
2. Remove `DECLARE_DYNCREATE`, `IMPLEMENT_DYNCREATE`
3. Remove constructor/destructor (or make simple)
4. Replace message map with `BEGIN_MSG_MAP`/`END_MSG_MAP`
5. Add `CHAIN_MSG_MAP(CPropertyPageImpl<ClassName>)` at end of message map
6. Change `OnApply()` return from `BOOL` to `int`, return `PSNRET_NOERROR`
7. Replace all `afx_msg` handlers with `LRESULT` handlers
8. Remove `DoDataExchange`, manually attach controls in `OnInitDialog`
9. Replace `UpdateData(TRUE/FALSE)` with explicit Get/Set calls

### Priority 2: Dialogs

1. **ScriptEditor.h/cpp** - Uses CRichEditCtrl, CListBox
   - Follow FileTransferProgressDlg pattern
   - Replace `CDialogEx` with `CDialogImpl<CScriptEditor>`

### Priority 3: Complex (Blocked)

1. **AdvGeneral.h/cpp** - Uses `CMFCPropertyGridCtrl`
   - **Options**:
     a) Keep as MFC (hybrid approach)
     b) Find WTL property grid replacement
     c) Redesign UI without property grid
   - **Recommendation**: Defer until other conversions complete

## Build Integration

### Project Settings Required

Add to include directories:
```
E:\PubCode\wtl10\Include
```

### Preprocessor Definitions
```
_ATL_NO_AUTOMATIC_NAMESPACE  // Optional - prevents namespace pollution
```

### Link Dependencies
Remove MFC-specific libraries if not needed:
- No changes needed for WTL (header-only)

## Testing Strategy

1. Convert one file at a time
2. Build after each conversion
3. Fix compilation errors immediately
4. Test dialog/page functionality
5. Verify all controls work correctly
6. Check message routing

## Common Issues and Solutions

### Issue: Control member variables don't attach
**Solution**: Call `.Attach(GetDlgItem(IDC_xxx))` in `OnInitDialog`

### Issue: Messages not received
**Solution**: Check message map syntax, ensure handler signatures match

### Issue: Property sheet doesn't work
**Solution**: Ensure `CHAIN_MSG_MAP` is present and comes last

### Issue: Dialog doesn't close on OK
**Solution**: Call `EndDialog(IDOK)` instead of `CDialog::OnOK()`

### Issue: CString operations fail
**Solution**: WTL uses ATL's CString, should work the same. Check includes.

## Verification Checklist

For each converted file:
- [ ] Removed all `afx_msg` declarations
- [ ] Removed `DECLARE_MESSAGE_MAP` / `DECLARE_DYNCREATE`
- [ ] Removed `DoDataExchange` method
- [ ] Removed `UpdateData()` calls
- [ ] Changed handler return types to `LRESULT`
- [ ] Added proper WTL includes
- [ ] Controls manually attached in `OnInitDialog`
- [ ] Message map uses WTL macros
- [ ] Property pages have `CHAIN_MSG_MAP`
- [ ] Builds without errors
- [ ] Tested functionality

## Summary

This guide covers the systematic conversion of MFC UI classes to WTL. The patterns shown are proven to work in the Ditto codebase. Follow these templates for consistent, working conversions.
