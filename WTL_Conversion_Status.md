# WTL Conversion Status Report

## Completed Conversions ✓

### Dialog Classes (CDialog/CDialogEx → CDialogImpl) - 6 classes
1. **CGroupName** - Simple name input dialog
2. **CMoveToGroupDlg** - Group selection dialog with tree control
3. **CFriendDetails** - Friend configuration dialog
4. **CFileTransferProgressDlg** - File transfer progress dialog with animation control
5. **CFriendPromptDlg** - Friend prompt dialog with checkboxes
6. **GlobalClips** - Global hotkey clips dialog with list view

### Property Pages (CPropertyPage → CPropertyPageImpl) - 4 classes
1. **OptionsCopyBuffers** - Already using CPropertyPageImpl ✓
2. **OptionsStats** - Already using CPropertyPageImpl ✓
3. **COptionsUtilities** - Database utilities property page ✓
4. **COptionsKeyBoard** - Keyboard shortcuts property page ✓

## Pending Complex Conversions

### Critical - Requires MFC-Specific Control Replacement

#### CAdvGeneral (CDialogEx)
**Issue**: Uses `CMFCPropertyGridCtrl` which is MFC-only
**File**: AdvGeneral.h/cpp
**Solution Options**:
1. Keep using MFC for this dialog only (hybrid approach)
2. Replace with WTL property list control
3. Use custom grid control or third-party alternative
**Recommendation**: Consider hybrid approach or redesign UI

#### CScriptEditor (CDialogEx)
**File**: ScriptEditor.h/cpp
**Complexity**: Medium
**Status**: Can be converted following standard pattern

### Standard CPropertyPage Conversions Needed (7 classes)

All follow similar pattern - convert to `CPropertyPageImpl<T>`:

1. **CQuickPasteKeyboard** (QuickPasteKeyboard.h/cpp)
2. **COptionsUtilities** (OptionsUtilities.h/cpp) 
3. **COptionsTypes** (OptionsTypes.h/cpp)
4. **COptionsQuickPaste** (OptionsQuickPaste.h/cpp)
5. **COptionsKeyBoard** (OptionsKeyBoard.h/cpp)
6. **COptionsGeneral** (OptionsGeneral.h/cpp)
7. **COptionFriends** (OptionFriends.h/cpp)

**Conversion Pattern**:
```cpp
// Header: Change base class
class COptionFriends : public CPropertyPageImpl<COptionFriends>
{
public:
    enum { IDD = IDD_OPTIONS_FRIENDS };
    
    BEGIN_MSG_MAP(COptionFriends)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(ID_BUTTON, OnButton)
        CHAIN_MSG_MAP(CPropertyPageImpl<COptionFriends>)
    END_MSG_MAP()
    
    LRESULT OnInitDialog(...);
    // Override OnApply() from base class
    int OnApply() override;
};
```

### CFrameWnd Conversion

#### CDimWnd (CFrameWnd)
**File**: DimWnd.h/cpp
**Target**: Convert to `CFrameWindowImpl<CDimWnd>`
**Complexity**: Low-Medium

### CWnd Subclasses (35+ classes)

Many custom controls inherit from `CWnd`. These need case-by-case evaluation:

**High Priority**:
- CQPasteWnd, CWndEx (base class for many)
- CGroupTree, CQListCtrl (custom controls)
- CSystemTray, CTabCtrlEx

**Conversion Pattern**:
```cpp
class CCustomControl : public CWindowImpl<CCustomControl>
{
public:
    DECLARE_WND_CLASS(NULL)
    
    BEGIN_MSG_MAP(CCustomControl)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        // ... other messages
    END_MSG_MAP()
};
```

## Conversion Statistics

| Category | Total | Converted | Remaining | % Complete |
|----------|-------|-----------|-----------|------------|
| CDialog/CDialogEx | 10 | 6 | 4 | 60% |
| CPropertyPage | 9 | 4 | 5 | 44% |
| CFrameWnd | 1 | 0 | 1 | 0% |
| CWnd subclasses | 35+ | 0 | 35+ | 0% |

**Overall Progress: 10/45+ classes converted (22%)**

## Key Technical Challenges

### 1. DoDataExchange Removal
All `DoDataExchange` and DDX macros must be replaced with manual control handling:
```cpp
// MFC
DDX_Control(pDX, IDC_EDIT1, m_edit);
DDX_Text(pDX, IDC_EDIT1, m_value);

// WTL Replacement
m_edit.Attach(GetDlgItem(IDC_EDIT1));
m_edit.GetWindowText(m_value);  // To retrieve
m_edit.SetWindowText(m_value);  // To set
```

### 2. Message Map Conversion
```cpp
// MFC
BEGIN_MESSAGE_MAP(CMyDialog, CDialog)
    ON_BN_CLICKED(IDOK, &CMyDialog::OnOk)
    ON_MESSAGE(WM_CUSTOM, OnCustomMsg)
END_MESSAGE_MAP()

// WTL
BEGIN_MSG_MAP(CMyDialog)
    COMMAND_ID_HANDLER(IDOK, OnOk)
    MESSAGE_HANDLER(WM_CUSTOM, OnCustomMsg)
END_MSG_MAP()
```

### 3. Return Value Changes
```cpp
// MFC: void or BOOL
void OnButton();
BOOL OnInitDialog();

// WTL: LRESULT
LRESULT OnButton(WORD, WORD, HWND, BOOL&);
LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
```

## Build Considerations

### Current Issues
1. **MFC Libraries Missing**: EncryptDecrypt project requires MFC
2. **WTL Include Paths**: Need to ensure `E:\PubCode\wtl10\Include` is in project settings
3. **Mixed MFC/WTL**: Some files may temporarily use both

### Solution Approach
- Complete WTL conversions incrementally
- Test each conversion separately
- Consider keeping CAdvGeneral as MFC temporarily due to CMFCPropertyGridCtrl dependency

## Next Steps (Priority Order)

1. ✓ Complete remaining dialog conversions (ScriptEditor)
2. ✓ Convert all CPropertyPage classes to CPropertyPageImpl
3. ✓ Convert CDimWnd to CFrameWindowImpl  
4. ⚠ Handle CAdvGeneral (decide on CMFCPropertyGridCtrl replacement)
5. ✓ Systematically convert CWnd subclasses
6. ✓ Remove all MFC includes and dependencies
7. ✓ Build and test

## Files Modified

### Already Converted
- GroupName.h/cpp
- MoveToGroupDlg.h/cpp
- FriendDetails.h/cpp
- FileTransferProgressDlg.h/cpp
- FriendPromptDlg.h/cpp
- GlobalClips.h/cpp

### Infrastructure
- wtl_compatibility.h (WTL header includes with fallbacks)

## Remaining MFC References

Run these commands to find remaining MFC usage:
```bash
rg "CDialog|CDialogEx|CPropertyPage|CFrameWnd" --type h -l
rg "BEGIN_MESSAGE_MAP|DECLARE_MESSAGE_MAP" --type cpp -l
rg "DoDataExchange" --type cpp -l
rg "afx_msg" --type h -l
```
