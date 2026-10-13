// WTL Compatibility Layer for MFC to WTL Migration
// This file ensures WTL 10 works correctly with current ATL version

#ifndef __WTL_COMPATIBILITY_H__
#define __WTL_COMPATIBILITY_H__

#pragma once

// Include standard headers needed by compatibility layer
#include <windows.h>

// CPoint/CSize/CRect come from ATL's own atltypes.h
#include <atltypes.h>

// Include required ATL headers first
#include <atlbase.h>
#include <atlwin.h>
#include <atlstr.h>
#include <atltime.h>

// Include standard headers needed by compatibility layer
#include <map>

// WTL headers (pristine WTL 10; atlapp.h must come first)
#include <atlapp.h>
#include <atlctrls.h>
#include <atlcrack.h>
#include <atlframe.h>
#include <atldlgs.h>
#include <atlctrlx.h>
#include <atltheme.h>
#include <atlmisc.h>

// WTL keeps its window handle class in ATL; Ditto source spells it WTL::CWindow.
namespace WTL
{
	using ATL::CWindow;
}

// CWnd wrapper with GetSafeHwnd() compatibility - must be defined BEFORE WTL namespace
class CWndCompat : public ATL::CWindow
{
public:
    CWndCompat() : ATL::CWindow() {}
    CWndCompat(HWND hWnd) : ATL::CWindow(hWnd) {}
    CWndCompat(const CWndCompat& src) : ATL::CWindow(src.m_hWnd) {}
    
    HWND GetSafeHwnd() const { return m_hWnd; }
};

// MFC DECLARE_MESSAGE_MAP compatibility - use WTL message maps
#ifndef DECLARE_MESSAGE_MAP
#define DECLARE_MESSAGE_MAP() \
protected: \
    static const WTL::ATL_MSG_ENTRY* GetMessageMap(); \
    virtual const WTL::ATL_MSG_ENTRY* GetMessageMap() const { return GetMessageMap(); }
#endif

// WTL Message Map entry structure - ensure it's defined
namespace WTL
{
    struct ATL_MSG_ENTRY
    {
        UINT nMsg;
        UINT nCode;
        UINT nID;
        UINT nLastID;
        UINT_PTR nSig;
        void (*pfn)(void);
    };
}

// MFC BEGIN_MESSAGE_MAP to WTL conversion
// This is a simplified bridge that allows MFC-style message maps to work with WTL
#ifndef BEGIN_MESSAGE_MAP
#define BEGIN_MESSAGE_MAP(theClass, baseClass) \
    const WTL::ATL_MSG_ENTRY* theClass::GetMessageMap() const \
    { \
        typedef theClass _thisClass; \
        static const WTL::ATL_MSG_ENTRY entries[] = {
#endif

#ifndef END_MESSAGE_MAP
#define END_MESSAGE_MAP() \
            { 0, 0, 0, 0, 0, NULL } \
        }; \
        return entries; \
    }
#endif

// MFC ON_WM_* to WTL MESSAGE_HANDLER conversions
#ifndef ON_WM_DRAWITEM
#define ON_WM_DRAWITEM() \
    { WM_DRAWITEM, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnDrawItem },
#endif

#ifndef ON_WM_ERASEBKGND
#define ON_WM_ERASEBKGND() \
    { WM_ERASEBKGND, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnEraseBkgnd },
#endif

#ifndef ON_WM_CTLCOLOR_REFLECT
#define ON_WM_CTLCOLOR_REFLECT() \
    { WM_CTLCOLORBTN, 0, 0, 0, 0, (void (*)(void))&_thisClass::CtlColor },
#endif

#ifndef ON_WM_MOUSEMOVE
#define ON_WM_MOUSEMOVE() \
    { WM_MOUSEMOVE, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnMouseMove },
#endif

#ifndef ON_WM_MEASUREITEM
#define ON_WM_MEASUREITEM() \
    { WM_MEASUREITEM, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnMeasureItem },
#endif

#ifndef ON_WM_COMPAREITEM
#define ON_WM_COMPAREITEM() \
    { WM_COMPAREITEM, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnCompareItem },
#endif

#ifndef ON_WM_DELETEITEM
#define ON_WM_DELETEITEM() \
    { WM_DELETEITEM, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnDeleteItem },
#endif

#ifndef ON_MESSAGE
#define ON_MESSAGE(msg, memberFxn) \
    { msg, 0, 0, 0, 0, (void (*)(void))&_thisClass::memberFxn },
#endif

#ifndef ON_COMMAND
#define ON_COMMAND(id, memberFxn) \
    { WM_COMMAND, 0, id, id, 0, (void (*)(void))&_thisClass::memberFxn },
#endif

#ifndef ON_UPDATE_COMMAND_UI
#define ON_UPDATE_COMMAND_UI(id, memberFxn) \
    { WM_COMMAND, CN_UPDATE_COMMAND_UI, id, id, 0, (void (*)(void))&_thisClass::memberFxn },
#endif

#ifndef CN_UPDATE_COMMAND_UI
#define CN_UPDATE_COMMAND_UI (0xFFFF - 1)
#endif

#ifndef ON_COMMAND_RANGE
#define ON_COMMAND_RANGE(id, idLast, memberFxn) \
    { WM_COMMAND, 0, id, idLast, 0, (void (*)(void))&_thisClass::memberFxn },
#endif

#ifndef ON_BN_CLICKED
#define ON_BN_CLICKED(id, memberFxn) \
    { WM_COMMAND, BN_CLICKED, id, id, 0, (void (*)(void))&_thisClass::memberFxn },
#endif

#ifndef ON_EN_CHANGE
#define ON_EN_CHANGE(id, memberFxn) \
    { WM_COMMAND, EN_CHANGE, id, id, 0, (void (*)(void))&_thisClass::memberFxn },
#endif

#ifndef ON_NOTIFY
#define ON_NOTIFY(wNotifyCode, id, memberFxn) \
    { WM_NOTIFY, wNotifyCode, id, id, 0, (void (*)(void))&_thisClass::memberFxn },
#endif

#ifndef ON_WM_SIZE
#define ON_WM_SIZE() \
    { WM_SIZE, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnSize },
#endif

#ifndef ON_WM_MOVE
#define ON_WM_MOVE() \
    { WM_MOVE, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnMove },
#endif

#ifndef ON_WM_PAINT
#define ON_WM_PAINT() \
    { WM_PAINT, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnPaint },
#endif

#ifndef ON_WM_CLOSE
#define ON_WM_CLOSE() \
    { WM_CLOSE, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnClose },
#endif

#ifndef ON_WM_DESTROY
#define ON_WM_DESTROY() \
    { WM_DESTROY, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnDestroy },
#endif

#ifndef ON_WM_CREATE
#define ON_WM_CREATE() \
    { WM_CREATE, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnCreate },
#endif

#ifndef ON_WM_TIMER
#define ON_WM_TIMER() \
    { WM_TIMER, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnTimer },
#endif

#ifndef ON_WM_LBUTTONDOWN
#define ON_WM_LBUTTONDOWN() \
    { WM_LBUTTONDOWN, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnLButtonDown },
#endif

#ifndef ON_WM_LBUTTONUP
#define ON_WM_LBUTTONUP() \
    { WM_LBUTTONUP, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnLButtonUp },
#endif

#ifndef ON_WM_LBUTTONDBLCLK
#define ON_WM_LBUTTONDBLCLK() \
    { WM_LBUTTONDBLCLK, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnLButtonDblClk },
#endif

#ifndef ON_WM_NCLBUTTONDBLCLK
#define ON_WM_NCLBUTTONDBLCLK() \
    { WM_NCLBUTTONDBLCLK, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnNcLButtonDblClk },
#endif

#ifndef ON_WM_RBUTTONDOWN
#define ON_WM_RBUTTONDOWN() \
    { WM_RBUTTONDOWN, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnRButtonDown },
#endif

#ifndef ON_WM_RBUTTONUP
#define ON_WM_RBUTTONUP() \
    { WM_RBUTTONUP, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnRButtonUp },
#endif

#ifndef ON_WM_KEYDOWN
#define ON_WM_KEYDOWN() \
    { WM_KEYDOWN, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnKeyDown },
#endif

#ifndef ON_WM_KEYUP
#define ON_WM_KEYUP() \
    { WM_KEYUP, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnKeyUp },
#endif

#ifndef ON_WM_CHAR
#define ON_WM_CHAR() \
    { WM_CHAR, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnChar },
#endif

#ifndef ON_WM_VSCROLL
#define ON_WM_VSCROLL() \
    { WM_VSCROLL, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnVScroll },
#endif

#ifndef ON_WM_HSCROLL
#define ON_WM_HSCROLL() \
    { WM_HSCROLL, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnHScroll },
#endif

#ifndef ON_WM_ENABLE
#define ON_WM_ENABLE() \
    { WM_ENABLE, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnEnable },
#endif

#ifndef ON_WM_SETFOCUS
#define ON_WM_SETFOCUS() \
    { WM_SETFOCUS, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnSetFocus },
#endif

#ifndef ON_WM_KILLFOCUS
#define ON_WM_KILLFOCUS() \
    { WM_KILLFOCUS, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnKillFocus },
#endif

#ifndef ON_WM_SETCURSOR
#define ON_WM_SETCURSOR() \
    { WM_SETCURSOR, 0, 0, 0, 0, (void (*)(void))&_thisClass::OnSetCursor },
#endif

// BEGIN_EVENTSINK_MAP / END_EVENTSINK_MAP macros
#ifndef DECLARE_EVENTSINK_MAP
#define DECLARE_EVENTSINK_MAP(theClass) \
protected: \
    static const ATL::ATL_MSG_ENTRY* PASCAL GetEventSinkMap(theClass*); \
    const ATL::ATL_MSG_ENTRY* GetEventSinkMap() const { return GetEventSinkMap(this); }
#endif

#ifndef BEGIN_EVENTSINK_MAP
#define BEGIN_EVENTSINK_MAP(theClass, baseClass) \
    const ATL::ATL_MSG_ENTRY* PASCAL theClass::GetEventSinkMap(theClass* pThis) \
    { \
        typedef theClass _thisClass; \
        UNUSED(pThis);
#endif

#ifndef END_EVENTSINK_MAP
#define END_EVENTSINK_MAP() \
        return NULL; \
    }
#endif

// afx_msg for message handlers
#ifndef afx_msg
#define afx_msg
#endif

// ASSERT/TRACE macros
#ifndef ASSERT
#define ASSERT(x) _ASSERTE(x)
#endif

#ifndef TRACE
#define TRACE __noop
#endif

// ATL/WTL provide _U_STRINGorID and CWindow themselves; do not shadow them.
// CString lives in ATL (WTL only re-exports it through a using-directive).
using ATL::CString;

// Use CWndCompat as CWnd and CWindow for compatibility
using CWnd = CWndCompat;

// Make CWindow point to the ATL/WTL class for controls
typedef ATL::CWindow CWindow;

#ifndef afx_msg
#define afx_msg
#endif

// POSITION - opaque MFC list/map cursor, 0 means null/end
typedef INT_PTR POSITION;

// CFont - Simple font wrapper
class MFCFont
{
public:
    MFCFont() : m_hFont(NULL) {}
    ~MFCFont() { DeleteObject(); }
    
    BOOL CreateFont(int nHeight, int nWidth, int nEscapement, int nOrientation, int nWeight,
        BYTE bItalic, BYTE bUnderline, BYTE cStrikeOut, BYTE nCharSet, BYTE nOutPrecision,
        BYTE nClipPrecision, BYTE nQuality, BYTE nPitchAndFamily, LPCTSTR lpszFacename)
    {
        DeleteObject();
        m_hFont = ::CreateFont(nHeight, nWidth, nEscapement, nOrientation, nWeight,
            bItalic, bUnderline, cStrikeOut, nCharSet, nOutPrecision,
            nClipPrecision, nQuality, nPitchAndFamily, lpszFacename);
        return m_hFont != NULL;
    }
    
    BOOL CreateFontIndirect(const LOGFONT* lpLogFont)
    {
        DeleteObject();
        m_hFont = ::CreateFontIndirect(lpLogFont);
        return m_hFont != NULL;
    }
    
    BOOL DeleteObject()
    {
        if (m_hFont)
        {
            ::DeleteObject(m_hFont);
            m_hFont = NULL;
        }
        return TRUE;
    }
    
    BOOL GetLogFont(LOGFONT* lpLogFont) const
    {
        if (m_hFont && lpLogFont)
        {
            return ::GetObject(m_hFont, sizeof(LOGFONT), lpLogFont) != 0;
        }
        return FALSE;
    }
    
    HFONT operator=(HFONT hFont)
    {
        DeleteObject();
        m_hFont = hFont;
        return m_hFont;
    }
    
    operator HFONT() const { return m_hFont; }
    HFONT m_hFont;
};

// Remove CFont alias to avoid conflict with WTL::CFont
// Use MFCFont directly if needed for CreateFont/CreateFontIndirect methods
// For WTL controls, use WTL::CFont which has operator HFONT()

// CBrush - Simple brush wrapper
class MFCBrush
{
public:
    MFCBrush() : m_hBrush(NULL) {}
    ~MFCBrush() { DeleteObject(); }
    
    BOOL CreateSolidBrush(COLORREF crColor)
    {
        DeleteObject();
        m_hBrush = ::CreateSolidBrush(crColor);
        return m_hBrush != NULL;
    }
    
    BOOL CreateBrushIndirect(const LOGBRUSH* lpLogBrush)
    {
        DeleteObject();
        m_hBrush = ::CreateBrushIndirect(lpLogBrush);
        return m_hBrush != NULL;
    }
    
    BOOL DeleteObject()
    {
        if (m_hBrush)
        {
            ::DeleteObject(m_hBrush);
            m_hBrush = NULL;
        }
        return TRUE;
    }
    
    operator HBRUSH() const { return m_hBrush; }
    HBRUSH m_hBrush;
};

// Remove CBrush alias to avoid conflict with WTL::CBrush
// Use MFCBrush directly if needed for CreateBrushIndirect methods
// For WTL controls, use WTL::CBrush which has operator HBRUSH()

// CByteArray, CWordArray, CDWordArray, CUIntArray - use ATL's CSimpleArray
using CByteArray = ATL::CSimpleArray<BYTE>;
using CWordArray = ATL::CSimpleArray<WORD>;
using CDWordArray = ATL::CSimpleArray<DWORD>;
using CUIntArray = ATL::CSimpleArray<UINT>;

// Our own CSimpleArray for compatibility (renamed to avoid conflict with ATL)
template<typename T>
class WTLCSimpleArray
{
public:
    WTLCSimpleArray() : m_nSize(0), m_pData(NULL), m_nAllocSize(0) {}
    ~WTLCSimpleArray() { RemoveAll(); }
    
    int GetSize() const { return m_nSize; }
    T* GetData() const { return m_pData; }
    BOOL Add(const T& item)
    {
        if (m_nSize >= m_nAllocSize)
        {
            int nNewSize = m_nAllocSize == 0 ? 4 : m_nAllocSize * 2;
            T* pNewData = new T[nNewSize];
            if (m_pData)
            {
                memcpy(pNewData, m_pData, m_nSize * sizeof(T));
                delete[] m_pData;
            }
            m_pData = pNewData;
            m_nAllocSize = nNewSize;
        }
        m_pData[m_nSize++] = item;
        return TRUE;
    }
    void RemoveAll() { if (m_pData) delete[] m_pData; m_pData = NULL; m_nSize = 0; m_nAllocSize = 0; }
    T& operator[](int nIndex) { return m_pData[nIndex]; }
    const T& operator[](int nIndex) const { return m_pData[nIndex]; }
    
private:
    T* m_pData;
    int m_nSize;
    int m_nAllocSize;
};

// CArray - MFC dynamic array template
template<typename TYPE, typename ARG_TYPE = const TYPE&>
class CArray
{
public:
    CArray() : m_nSize(0), m_pData(NULL), m_nAllocSize(0) {}
    ~CArray() { RemoveAll(); }
    
    INT_PTR GetCount() const { return m_nSize; }
    INT_PTR GetSize() const { return m_nSize; }
    INT_PTR Add(ARG_TYPE item)
    {
        if (m_nSize >= m_nAllocSize)
        {
            INT_PTR nNewSize = m_nAllocSize == 0 ? 4 : m_nAllocSize * 2;
            TYPE* pNewData = new TYPE[nNewSize];
            if (m_pData)
            {
                memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));
                delete[] m_pData;
            }
            m_pData = pNewData;
            m_nAllocSize = nNewSize;
        }
        m_pData[m_nSize++] = item;
        return m_nSize - 1;
    }
    void RemoveAll() { if (m_pData) delete[] m_pData; m_pData = NULL; m_nSize = 0; m_nAllocSize = 0; }
    TYPE* GetData() const { return m_pData; }
    void SetSize(INT_PTR nNewSize, INT_PTR nGrowBy = 0)
    {
        if (nNewSize < 0) nNewSize = 0;
        if (nNewSize == m_nSize) return;
        if (nNewSize == 0) { RemoveAll(); return; }

        INT_PTR nAlloc = nNewSize;
        if (nGrowBy > 0) nAlloc += nGrowBy;
        if (nAlloc < m_nAllocSize) nAlloc = m_nAllocSize;

        TYPE* pNewData = new TYPE[nAlloc];
        INT_PTR nCopy = nNewSize < m_nSize ? nNewSize : m_nSize;
        if (m_pData && nCopy > 0) memcpy(pNewData, m_pData, nCopy * sizeof(TYPE));
        if (m_pData) delete[] m_pData;
        m_pData = pNewData;
        m_nSize = nNewSize;
        m_nAllocSize = nAlloc;
    }
    TYPE& ElementAt(INT_PTR nIndex) { return m_pData[nIndex]; }
    const TYPE& ElementAt(INT_PTR nIndex) const { return m_pData[nIndex]; }
    TYPE& operator[](INT_PTR nIndex) { return m_pData[nIndex]; }
    const TYPE& operator[](INT_PTR nIndex) const { return m_pData[nIndex]; }
    void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1)
    {
        if (nIndex < 0 || nIndex >= m_nSize) return;
        INT_PTR nNewSize = m_nSize - nCount;
        if (nNewSize > 0)
        {
            TYPE* pNewData = new TYPE[nNewSize];
            memcpy(pNewData, m_pData, nIndex * sizeof(TYPE));
            memcpy(pNewData + nIndex, m_pData + nIndex + nCount, (m_nSize - nIndex - nCount) * sizeof(TYPE));
            delete[] m_pData;
            m_pData = pNewData;
        }
        else
        {
            delete[] m_pData;
            m_pData = NULL;
        }
        m_nSize = nNewSize;
    }
    
private:
    TYPE* m_pData;
    INT_PTR m_nSize;
    INT_PTR m_nAllocSize;
};

// CMap - MFC map template
template<typename KEY, typename ARG_KEY, typename VALUE, typename ARG_VALUE>
class CMap
{
public:
    CMap() {}
    ~CMap() { RemoveAll(); }
    
    void RemoveAll() { m_map.clear(); }
    INT_PTR GetCount() const { return (INT_PTR)m_map.size(); }
    BOOL IsEmpty() const { return m_map.empty(); }
    
    void SetAt(KEY key, VALUE val) { m_map[key] = val; }
    BOOL Lookup(ARG_KEY key, VALUE& rValue) const
    {
        auto it = m_map.find(key);
        if (it != m_map.end())
        {
            rValue = it->second;
            return TRUE;
        }
        return FALSE;
    }
    VALUE* Lookup(ARG_KEY key)
    {
        auto it = m_map.find(key);
        if (it != m_map.end())
            return &it->second;
        return NULL;
    }
    void RemoveKey(ARG_KEY key) { m_map.erase(key); }

    // MFC style iteration, POSITION is a one based index, 0 means end
    POSITION GetStartPosition() const { return m_map.empty() ? (POSITION)0 : (POSITION)1; }
    void GetNextAssoc(POSITION& rNextPosition, KEY& rKey, VALUE& rValue) const
    {
        if (rNextPosition < 1 || rNextPosition > (POSITION)m_map.size())
        {
            rNextPosition = 0;
            return;
        }
        auto it = m_map.begin();
        std::advance(it, (size_t)rNextPosition - 1);
        rKey = it->first;
        rValue = it->second;
        rNextPosition = (rNextPosition < (POSITION)m_map.size()) ? rNextPosition + 1 : 0;
    }
    void GetPrevAssoc(POSITION& rPosition, KEY& rKey, VALUE& rValue) const
    {
        if (rPosition < 1 || rPosition > (POSITION)m_map.size())
        {
            rPosition = 0;
            return;
        }
        auto it = m_map.begin();
        std::advance(it, (size_t)rPosition - 1);
        rKey = it->first;
        rValue = it->second;
        rPosition = (rPosition > 1) ? rPosition - 1 : 0;
    }
    POSITION FindKey(ARG_KEY key) const
    {
        auto it = m_map.find(key);
        if (it == m_map.end()) return 0;
        return (POSITION)std::distance(m_map.begin(), it) + 1;
    }
    
private:
    std::map<KEY, VALUE> m_map;
};

// MFC Compatibility Macros
#ifndef AfxGetInstanceHandle
#define AfxGetInstanceHandle() GetModuleHandle(NULL)
#endif

#ifndef AfxGetResourceHandle
#define AfxGetResourceHandle() GetModuleHandle(NULL)
#endif

#ifndef AfxGetApp
#define AfxGetApp() NULL
#endif

#ifndef AfxGetMainWnd
#define AfxGetMainWnd() GetActiveWindow()
#endif

#ifndef AfxMessageBox
inline int AfxMessageBox(LPCTSTR lpszText, UINT nType = MB_OK, UINT nIDHelp = 0)
{
    return MessageBox(NULL, lpszText, _T("Ditto"), nType);
}
inline int AfxMessageBox(UINT nIDPrompt, UINT nType = MB_OK, UINT nIDHelp = (UINT)-1)
{
    TCHAR szText[1024];
    if (LoadString(GetModuleHandle(NULL), nIDPrompt, szText, _countof(szText)))
        return MessageBox(NULL, szText, _T("Ditto"), nType);
    return MessageBox(NULL, _T("Error loading string"), _T("Ditto"), MB_OK | MB_ICONERROR);
}
#endif

#ifndef AfxIsValidString
#define AfxIsValidString(lpsz, nLength) ((lpsz != NULL) && (nLength >= 0 || wcslen(lpsz) >= 0))
#endif

#ifndef AfxIsValidAddress
#define AfxIsValidAddress(lp, nBytes, bReadWrite) ((lp != NULL) && !IsBadReadPtr(lp, nBytes))
#endif

#ifndef AfxRegisterWndClass
inline ATOM AfxRegisterWndClass(UINT nClassStyle, HCURSOR hCursor = NULL, HBRUSH hbrBackground = NULL, HICON hIcon = NULL)
{
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = nClassStyle;
    wc.hCursor = hCursor ? hCursor : LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = hbrBackground ? hbrBackground : (HBRUSH)(COLOR_WINDOW + 1);
    wc.hIcon = hIcon ? hIcon : LoadIcon(NULL, IDI_APPLICATION);
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    
    TCHAR szClassName[256];
    static int nClassCount = 0;
    wsprintf(szClassName, _T("WTL_AutoClass_%d"), nClassCount++);
    
    return RegisterClassEx(&wc);
}
#endif

#ifndef DECLARE_FRAME_WND_CLASS
#define DECLARE_FRAME_WND_CLASS DECLARE_WND_CLASS_EX
#endif

// COleDropTarget stub - using Win32 API directly instead of MFC
typedef HANDLE COleDropTarget;

#endif // __WTL_COMPATIBILITY_H__
