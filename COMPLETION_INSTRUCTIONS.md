> **历史文档 / HISTORICAL**：本文记录的是已放弃的 MFC 仿真 shim 方案（路线 A），其结论、类名与步骤均与当前代码不符。当前迁移状态见 [WTL_Migration_Status.md](WTL_Migration_Status.md)。

# How to Complete the Remaining MFC to WTL Conversions

## 🎯 Current Status
- **Completed**: 12/20 classes (60%)
- **Remaining**: 8 classes
- **Time Needed**: 4-6 hours for full completion

---

## ✅ Already Converted (Reference These!)

### Dialogs (6 classes)
- CGroupName
- CMoveToGroupDlg
- CFriendDetails
- CFileTransferProgressDlg
- CFriendPromptDlg
- GlobalClips

### Property Pages (6 classes)
- OptionsCopyBuffers (was WTL)
- OptionsStats (was WTL)
- COptionsUtilities ⭐ **Best simple example**
- COptionsKeyBoard ⭐ **Best complex example**
- COptionsTypes
- COptionsQuickPaste

---

## 📋 Step-by-Step Instructions for Remaining Classes

### STEP 1: Complete OptionFriends (20-30 min)

**Status**: Header DONE, CPP needs conversion

**Files**: OptionFriends.h, OptionFriends.cpp

**Header**: ✅ Already converted!

**CPP**: Convert these sections:

1. Remove MFC boilerplate:
```cpp
// REMOVE:
IMPLEMENT_DYNCREATE(COptionFriends, CPropertyPage)
#ifdef _DEBUG...#endif
BEGIN_MESSAGE_MAP...END_MESSAGE_MAP
DoDataExchange

// Constructor: Change from
COptionFriends::COptionFriends() : CPropertyPage(IDD)

// To:
COptionFriends::COptionFriends()
{
    m_csTitle = ...;
    m_psp.pszTitle = m_csTitle;
    m_psp.dwFlags |= PSP_USETITLE;
    m_pParent = NULL;
    m_PlaceOnClipboard = _T("");
    m_csPassword = _T("");
    m_csAdditionalPasswords = _T("");
}
```

2. OnInitDialog:
```cpp
// Change from:
BOOL COptionFriends::OnInitDialog()
{
    CPropertyPage::OnInitDialog();
    
// To:
LRESULT COptionFriends::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
    m_bDisableRecieve.Attach(GetDlgItem(IDC_CHECK_DISABLE_FRIENDS));
    m_SendRecieve.Attach(GetDlgItem(IDC_CHECK_LOG_SEND_RECIEVE));
    m_List.Attach(GetDlgItem(IDC_LIST));
    
    // ... rest of init code ...
    
    // At end, replace UpdateData(FALSE) with manual sets:
    SetDlgItemText(IDC_EDIT_PLACE_ON_CLIPBOARD, m_PlaceOnClipboard);
    SetDlgItemText(IDC_EDIT_PASSWORD, m_csPassword);
    SetDlgItemText(IDC_EDIT_ADDITIONAL, m_csAdditionalPasswords);
    
    return 0;  // NOT FALSE
}
```

3. OnApply:
```cpp
// Change from:
BOOL COptionFriends::OnApply()
{
    // Replace UpdateData(TRUE) with:
    GetDlgItemText(IDC_EDIT_PLACE_ON_CLIPBOARD, m_PlaceOnClipboard);
    GetDlgItemText(IDC_EDIT_PASSWORD, m_csPassword);
    GetDlgItemText(IDC_EDIT_ADDITIONAL, m_csAdditionalPasswords);
    
    // ... save logic ...
    
    return PSNRET_NOERROR;  // NOT CPropertyPage::OnApply()
}
```

4. Message handlers:
```cpp
// Change from:
void COptionFriends::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{
    // ... code ...
    *pResult = 0;
}

// To:
LRESULT COptionFriends::OnDblclkList(int, LPNMHDR pnmh, BOOL&)
{
    // ... code (same) ...
    return 0;
}

// Similarly for OnKeydownList and OnCheckDisableFriends
```

---

### STEP 2: Complete OptionsGeneral (30-40 min)

**Files**: OptionsGeneral.h, OptionsGeneral.cpp

**Pattern**: Same as OptionsKeyBoard (many controls)

**Key Steps**:
1. Header: Follow OptionsKeyBoard.h pattern exactly
2. CPP: Attach all controls in OnInitDialog
3. Replace UpdateData with manual Get/Set for all controls
4. Convert all message handlers to LRESULT

---

### STEP 3: Complete ScriptEditor (30-40 min)

**Files**: ScriptEditor.h, ScriptEditor.cpp

**Pattern**: CDialogEx → CDialogImpl (like FileTransferProgressDlg)

**Key Points**:
- Uses CRichEditCtrl and CListBox
- Both have WTL equivalents: CRichEditCtrl (same name), CListBox (same name)
- DoModal() → DoModal() (works the same in WTL)
- EndDialog(IDOK) instead of CDialogEx::OnOK()

---

### STEP 4: Complete DimWnd (15-20 min)

**Files**: DimWnd.h, DimWnd.cpp

**Pattern**: CFrameWnd → CFrameWindowImpl

```cpp
// Header:
class CDimWnd : public CFrameWindowImpl<CDimWnd>
{
public:
    DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)  // If has resources
    // or
    DECLARE_WND_CLASS_EX(NULL, 0, -1)  // For simple window
    
    BEGIN_MSG_MAP(CDimWnd)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        // ... other messages
    END_MSG_MAP()
    
    LRESULT OnPaint(UINT, WPARAM, LPARAM, BOOL&);
};
```

---

### STEP 5: Handle QuickPasteKeyboard (1-2 hours)

**Files**: QuickPasteKeyboard.h/cpp (1036 lines!)

**Complexity**: Very high - extensive keyboard handling

**Approach**:
1. Follow OptionsKeyBoard pattern (it's similar - hot keys)
2. Many CHotKeyCtrl controls to attach
3. Complex message handling - keep logic, just change signatures
4. Test thoroughly after conversion

---

### STEP 6: Decide on AdvGeneral

**Two Options**:

#### Option A: Keep as MFC (Recommended)
1. Rename file: AdvGeneral → AdvGeneralMFC
2. Add comment explaining MFC dependency
3. Keep MFC libraries linked
4. Document as "hybrid approach"

#### Option B: Convert to WTL (4-8 hours)
1. Replace CMFCPropertyGridCtrl with custom implementation
2. Use CListViewCtrl in report mode to simulate property grid
3. Implement custom drawing and editing
4. Extensive testing required

**Recommendation**: Option A unless you have specific requirements for complete MFC removal.

---

## 🛠️ Conversion Checklist (Use for Each File)

### Header File (.h)
- [ ] Remove `#if !defined(AFX_...)`/`#define`/`#endif`
- [ ] Add WTL includes: `<atlbase.h>`, `<atlwin.h>`, `<atldlgs.h>`, `<atlctrls.h>`
- [ ] Change base class:
  - `CPropertyPage` → `CPropertyPageImpl<T>`
  - `CDialog`/`CDialogEx` → `CDialogImpl<T>`
- [ ] Remove `DECLARE_DYNCREATE` or `DECLARE_DYNAMIC`
- [ ] Remove `DECLARE_MESSAGE_MAP`
- [ ] Remove `DoDataExchange` declaration
- [ ] Add `BEGIN_MSG_MAP` / `END_MSG_MAP`
- [ ] For property pages, add `CHAIN_MSG_MAP(CPropertyPageImpl<T>)`
- [ ] Change handler signatures:
  - `afx_msg void OnButton()` → `LRESULT OnButton(WORD, WORD, HWND, BOOL&)`
  - `virtual BOOL OnInitDialog()` → `LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)`
  - `virtual BOOL OnApply()` → `int OnApply() override`
- [ ] Move public member controls to private section

### CPP File (.cpp)
- [ ] Remove `#ifdef _DEBUG`...`#endif` block
- [ ] Remove `IMPLEMENT_DYNCREATE` or `IMPLEMENT_DYNAMIC`
- [ ] Remove `BEGIN_MESSAGE_MAP` / `END_MESSAGE_MAP`
- [ ] Remove `DoDataExchange` implementation
- [ ] Update constructor:
  - Remove `: CPropertyPage(IDD)` base init
  - Initialize member variables directly
- [ ] Update OnInitDialog:
  - Remove `CPropertyPage::OnInitDialog()` call
  - Add `.Attach(GetDlgItem(IDC_xxx))` for all controls
  - Replace `UpdateData(FALSE)` with `SetDlgItemText()`, `CheckDlgButton()`, etc.
  - Change `return TRUE/FALSE` → `return 0`
- [ ] Update OnApply (property pages):
  - Replace `UpdateData(TRUE)` with `GetDlgItemText()`, `IsDlgButtonChecked()`, etc.
  - Change `return CPropertyPage::OnApply()` → `return PSNRET_NOERROR`
- [ ] Update all message handlers:
  - Change `void` → `LRESULT`
  - Add `return 0` at end
  - For dialogs, use `EndDialog(IDOK)` instead of `CDialog::OnOK()`

---

## 📝 Quick Reference: Common Conversions

### DDX Replacement
```cpp
// MFC DDX
DDX_Control(pDX, IDC_EDIT1, m_edit);
DDX_Text(pDX, IDC_EDIT1, m_text);
DDX_Check(pDX, IDC_CHECK1, m_bChecked);

// WTL Replacement (OnInitDialog)
m_edit.Attach(GetDlgItem(IDC_EDIT1));
SetDlgItemText(IDC_EDIT1, m_text);
CheckDlgButton(IDC_CHECK1, m_bChecked ? BST_CHECKED : BST_UNCHECKED);

// WTL Replacement (OnApply/OnOK)
GetDlgItemText(IDC_EDIT1, m_text);
m_bChecked = (IsDlgButtonChecked(IDC_CHECK1) == BST_CHECKED);
```

### Message Map
```cpp
// MFC
BEGIN_MESSAGE_MAP(CMyClass, CPropertyPage)
    ON_BN_CLICKED(IDC_BUTTON, OnButton)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnListChanged)
END_MESSAGE_MAP()

// WTL
BEGIN_MSG_MAP(CMyClass)
    MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    COMMAND_ID_HANDLER(IDC_BUTTON, OnButton)
    NOTIFY_HANDLER(IDC_LIST, LVN_ITEMCHANGED, OnListChanged)
    CHAIN_MSG_MAP(CPropertyPageImpl<CMyClass>)  // For property pages!
END_MSG_MAP()
```

### Control Types
```cpp
// MFC → WTL (most are the same!)
CEdit → CEdit
CButton → CButton
CListCtrl → CListViewCtrl
CTreeCtrl → CTreeViewCtrl
CComboBox → CComboBox
CHotKeyCtrl → CHotKeyCtrl
CProgressCtrl → CProgressBarCtrl
```

---

## 🧪 Testing After Conversion

1. **Build Test**:
```bash
msbuild CP_Main.sln /p:Configuration=Release /p:Platform=x64
```

2. **Verify No MFC References**:
```bash
rg "BEGIN_MESSAGE_MAP|DECLARE_MESSAGE_MAP|DoDataExchange" e:\PubCode\Ditto --type cpp -l
rg "class\s+\w+\s*:\s*public\s+(CDialog[^I]|CPropertyPage[^I])" e:\PubCode\Ditto --type h
```

3. **Functional Testing**:
- Open each converted dialog/page
- Verify all controls display correctly
- Test button clicks
- Test data entry and saving
- Verify OK/Cancel behavior

---

## 📊 Progress Tracking

Use this checklist:

- [ ] OptionFriends.cpp (header done, CPP pending)
- [ ] OptionsGeneral.h/cpp
- [ ] ScriptEditor.h/cpp
- [ ] DimWnd.h/cpp
- [ ] QuickPasteKeyboard.h/cpp (complex!)
- [ ] Decision on AdvGeneral (keep MFC or convert)
- [ ] Build successful
- [ ] All dialogs tested
- [ ] Documentation updated

---

## 🎓 Best Practices

1. **Convert one file at a time** - Don't try to convert multiple classes simultaneously
2. **Build after each conversion** - Catch errors early
3. **Use completed files as references** - Copy patterns from OptionsUtilities or OptionsKeyBoard
4. **Test immediately** - Don't wait until all conversions are done
5. **Keep notes** - Document any issues or special cases you encounter

---

## 🆘 Troubleshooting

### Build Errors

**Error**: "Cannot find atlbase.h"
**Solution**: Ensure WTL include path is set: `E:\PubCode\wtl10\Include`

**Error**: "Undefined identifier CPropertyPageImpl"
**Solution**: Include `<atldlgs.h>`

**Error**: "CHAIN_MSG_MAP not found"
**Solution**: Make sure you're inheriting from `CPropertyPageImpl`, not `CDialogImpl`

### Runtime Issues

**Problem**: Dialog doesn't show
**Solution**: Check that you're calling `DoModal()` correctly, same as before

**Problem**: Controls don't work
**Solution**: Make sure you called `.Attach(GetDlgItem(...))` for each control in `OnInitDialog`

**Problem**: Data not saving
**Solution**: Check that `OnApply` returns `PSNRET_NOERROR` and calls Get methods

---

## ✨ Summary

**Time Estimate**: 4-6 hours total
**Difficulty**: Medium (with provided patterns)
**Risk**: Low (patterns are proven)
**Result**: ~95% MFC-free application

**Key Success Factors**:
1. Follow established patterns exactly
2. Convert one class at a time
3. Build and test after each conversion
4. Use completed files as references
5. Don't rush - careful work prevents bugs

Good luck! The patterns are all established, documentation is complete, and you're 60% of the way there already. 🚀
