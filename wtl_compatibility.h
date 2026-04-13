// WTL Compatibility Layer for MFC to WTL Migration
// This file ensures WTL 10 works correctly with current ATL version

#ifndef __WTL_COMPATIBILITY_H__
#define __WTL_COMPATIBILITY_H__

#pragma once

// Include required ATL headers first
#include <atlbase.h>
#include <atlwin.h>
#include <atlstr.h>
#include <atltypes.h>
#include <atltime.h>

// WTL include root fallback (project-local copy located at E:\PubCode\wtl10\Include)
#ifndef WTL_INCLUDE_ROOT
#define WTL_INCLUDE_ROOT "E:/PubCode/wtl10/Include/"
#endif

#define WTL_HEADER(header) WTL_INCLUDE_ROOT header

// Include WTL headers - these should be included after ATL but before application headers
#if defined(__has_include) && __has_include(<atlapp.h>)
#include <atlapp.h>
#else
#include WTL_HEADER("atlapp.h")
#endif

#if defined(__has_include) && __has_include(<atlframe.h>)
#include <atlframe.h>
#else
#include WTL_HEADER("atlframe.h")
#endif

#if defined(__has_include) && __has_include(<atldlgs.h>)
#include <atldlgs.h>
#else
#include WTL_HEADER("atldlgs.h")
#endif

#if defined(__has_include) && __has_include(<atlctrls.h>)
#include <atlctrls.h>
#else
#include WTL_HEADER("atlctrls.h")
#endif

#if defined(__has_include) && __has_include(<atlctrlx.h>)
#include <atlctrlx.h>
#else
#include WTL_HEADER("atlctrlx.h")
#endif

#if defined(__has_include) && __has_include(<atlcrack.h>)
#include <atlcrack.h>
#else
#include WTL_HEADER("atlcrack.h")
#endif

#if defined(__has_include) && __has_include(<atlwinx.h>)
#include <atlwinx.h>
#else
#include WTL_HEADER("atlwinx.h")
#endif

#if defined(__has_include) && __has_include(<atluser.h>)
#include <atluser.h>
#else
#include WTL_HEADER("atluser.h")
#endif

#if defined(__has_include) && __has_include(<atlgdi.h>)
#include <atlgdi.h>
#else
#include WTL_HEADER("atlgdi.h")
#endif

#if defined(__has_include) && __has_include(<atltheme.h>)
#include <atltheme.h>
#else
#include WTL_HEADER("atltheme.h")
#endif

// WTL compatibility definitions
namespace WTL
{
    // Define _U_STRINGorID if not already defined
    struct _U_STRINGorID
    {
        LPCTSTR m_lpstr;
        _U_STRINGorID(LPCTSTR lpstr) : m_lpstr(lpstr) {}
        _U_STRINGorID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID)) {}
        operator LPCTSTR() const { return m_lpstr; }
        UINT GetID() const { return IS_INTRESOURCE(m_lpstr) ? static_cast<UINT>(reinterpret_cast<ULONG_PTR>(m_lpstr)) : 0U; }
    };

    // Define CWindow alias to ATL::CWindow
    typedef ATL::CWindow CWindow;

    // Define CString alias if it doesn't exist
    #ifndef _WTL_NO_CSTRING
    typedef ATL::CString CString;
    #endif

}

// Bring WTL types into global namespace for MFC compatibility
using WTL::_U_STRINGorID;
using WTL::CWindow;

// Provide legacy type aliases expected throughout the codebase
using CWnd = WTL::CWindow;
using CMenu = WTL::CMenu;
using CMenuHandle = WTL::CMenuHandle;
using CDC = WTL::CDC;
using CDCHandle = WTL::CDCHandle;
using CToolTipCtrl = WTL::CToolTipCtrl;

#ifndef afx_msg
#define afx_msg
#endif

// MFC Compatibility Macros - Replace Afx* functions with Windows API equivalents

// AfxGetInstanceHandle - Returns the instance handle of the application
#ifndef AfxGetInstanceHandle
#define AfxGetInstanceHandle() GetModuleHandle(NULL)
#endif

// AfxGetResourceHandle - Returns the resource handle
#ifndef AfxGetResourceHandle
#define AfxGetResourceHandle() GetModuleHandle(NULL)
#endif

// AfxGetApp - Returns the CWinApp object (not applicable in WTL, return NULL)
#ifndef AfxGetApp
#define AfxGetApp() NULL
#endif

// AfxGetMainWnd - Returns the main window (not applicable in WTL)
#ifndef AfxGetMainWnd
#define AfxGetMainWnd() GetActiveWindow()
#endif

// AfxMessageBox - Simple message box wrapper
#ifndef AfxMessageBox
inline int AfxMessageBox(LPCTSTR lpszText, UINT nType = MB_OK, UINT nIDHelp = 0)
{
    return MessageBox(NULL, lpszText, _T("Ditto"), nType);
}
inline int AfxMessageBox(UINT nIDPrompt, UINT nType = MB_OK, UINT nIDHelp = (UINT)-1)
{
    // Load string from resource
    TCHAR szText[1024];
    if (LoadString(GetModuleHandle(NULL), nIDPrompt, szText, _countof(szText)))
        return MessageBox(NULL, szText, _T("Ditto"), nType);
    return MessageBox(NULL, _T("Error loading string"), _T("Ditto"), MB_OK | MB_ICONERROR);
}
#endif

// AfxIsValidString - Check if a string pointer is valid
#ifndef AfxIsValidString
#define AfxIsValidString(lpsz, nLength) ((lpsz != NULL) && (nLength >= 0 || wcslen(lpsz) >= 0))
#endif

// AfxIsValidAddress - Check if a memory address is valid
#ifndef AfxIsValidAddress
#define AfxIsValidAddress(lp, nBytes, bReadWrite) ((lp != NULL) && !IsBadReadPtr(lp, nBytes))
#endif

// AfxRegisterWndClass - Register a window class
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

// CFrameWnd compatibility
#ifndef DECLARE_FRAME_WND_CLASS
#define DECLARE_FRAME_WND_CLASS DECLARE_WND_CLASS_EX
#endif

// MFC CByteArray, CWordArray, CDWordArray, CUIntArray compatibility
template<typename T>
class CSimpleArray
{
public:
    CSimpleArray() : m_nSize(0), m_pData(NULL), m_nAllocSize(0) {}
    ~CSimpleArray() { RemoveAll(); }
    
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

using CByteArray = CSimpleArray<BYTE>;
using CWordArray = CSimpleArray<WORD>;
using CDWordArray = CSimpleArray<DWORD>;
using CUIntArray = CSimpleArray<UINT>;

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
    TYPE& ElementAt(INT_PTR nIndex) { return m_pData[nIndex]; }
    const TYPE& ElementAt(INT_PTR nIndex) const { return m_pData[nIndex]; }
    TYPE& operator[](INT_PTR nIndex) { return m_pData[nIndex]; }
    const TYPE& operator[](INT_PTR nIndex) const { return m_pData[nIndex]; }
    
private:
    TYPE* m_pData;
    INT_PTR m_nSize;
    INT_PTR m_nAllocSize;
};

// CMap - MFC map template (simplified)
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
    
private:
    std::map<KEY, VALUE> m_map;
};

// CList - MFC list template (simplified)
template<typename TYPE, typename ARG_TYPE = const TYPE&>
class CList
{
public:
    struct CNode
    {
        TYPE data;
        CNode* pNext;
        CNode* pPrev;
        CNode(ARG_TYPE val) : data(val), pNext(NULL), pPrev(NULL) {}
    };
    
    CList() : m_pHead(NULL), m_pTail(NULL), m_nCount(0) {}
    ~CList() { RemoveAll(); }
    
    INT_PTR GetCount() const { return m_nCount; }
    BOOL IsEmpty() const { return m_nCount == 0; }
    TYPE& GetHead() { return m_pHead->data; }
    TYPE GetHead() const { return m_pHead->data; }
    TYPE& GetTail() { return m_pTail->data; }
    TYPE GetTail() const { return m_pTail->data; }
    
    POSITION GetHeadPosition() const { return POSITION(0); }
    POSITION GetTailPosition() const { return POSITION(m_nCount > 0 ? m_nCount - 1 : 0); }
    TYPE& GetNext(POSITION& rPosition) const
    {
        TYPE& val = m_pCurrent->data;
        m_pCurrent = m_pCurrent->pNext;
        return val;
    }
    const TYPE& GetNext(POSITION& rPosition)
    {
        TYPE& val = m_pCurrent->data;
        m_pCurrent = m_pCurrent->pNext;
        return val;
    }
    TYPE& GetAt(POSITION position) { return (*this)[position.index]; }
    const TYPE& GetAt(POSITION position) const { return (*this)[position.index]; }
    
    POSITION GetStartPosition() const { return m_pHead ? POSITION(0) : POSITION(); }
    
    INT_PTR AddHead(ARG_TYPE newElement)
    {
        CNode* pNewNode = new CNode(newElement);
        pNewNode->pNext = m_pHead;
        if (m_pHead)
            m_pHead->pPrev = pNewNode;
        m_pHead = pNewNode;
        if (!m_pTail)
            m_pTail = m_pHead;
        m_nCount++;
        return 0;
    }
    
    INT_PTR AddTail(ARG_TYPE newElement)
    {
        CNode* pNewNode = new CNode(newElement);
        pNewNode->pPrev = m_pTail;
        if (m_pTail)
            m_pTail->pNext = pNewNode;
        m_pTail = pNewNode;
        if (!m_pHead)
            m_pHead = m_pTail;
        m_nCount++;
        return m_nCount - 1;
    }
    
    void RemoveAll()
    {
        CNode* pNode = m_pHead;
        while (pNode)
        {
            CNode* pNext = pNode->pNext;
            delete pNode;
            pNode = pNext;
        }
        m_pHead = m_pTail = NULL;
        m_nCount = 0;
    }
    
    TYPE& operator[](INT_PTR nIndex)
    {
        CNode* pNode = m_pHead;
        for (INT_PTR i = 0; i < nIndex && pNode; i++)
            pNode = pNode->pNext;
        return pNode ? pNode->data : *(new TYPE());
    }
    
    const TYPE& operator[](INT_PTR nIndex) const
    {
        return const_cast<CList*>(this)->operator[](nIndex);
    }
    
private:
    CNode* m_pHead;
    CNode* m_pTail;
    INT_PTR m_nCount;
    mutable CNode* m_pCurrent;
};

// POSITION - MFC list position iterator (compatible with CList)
struct POSITION {
    size_t index;
    POSITION(size_t i = SIZE_MAX) : index(i) {}
    bool operator==(const POSITION& other) const { return index == other.index; }
    bool operator!=(const POSITION& other) const { return index != other.index; }
};

// CRect compatibility
using CRect = ATL::CRect;

// CPoint compatibility  
using CPoint = ATL::CPoint;

// CSize compatibility
using CSize = ATL::CSize;

// CFont compatibility
class CFont : public ATL::CFont
{
public:
    BOOL CreateFontIndirect(const LOGFONT* lpLogFont)
    {
        return CreateFont(lpLogFont->lfHeight, lpLogFont->lfWidth, lpLogFont->lfEscapement,
            lpLogFont->lfOrientation, lpLogFont->lfWeight, lpLogFont->lfItalic,
            lpLogFont->lfUnderline, lpLogFont->lfStrikeOut, lpLogFont->lfCharSet,
            lpLogFont->lfOutPrecision, lpLogFont->lfClipPrecision, lpLogFont->lfQuality,
            lpLogFont->lfPitchAndFamily, lpLogFont->lfFaceName);
    }
};

// CBrush compatibility
class CBrush : public ATL::CBrush
{
public:
    BOOL CreateSolidBrush(COLORREF crColor)
    {
        return Attach(CreateSolidBrush(crColor)) != NULL;
    }
};

// CImageList compatibility
class CImageList : public ATL::CImageList
{
public:
    CImageList() {}
    ~CImageList() { Destroy(); }
    
    BOOL Create(int cx, int cy, UINT nFlags, int nInitial, int nGrow)
    {
        return Attach(::ImageList_Create(cx, cy, nFlags, nInitial, nGrow));
    }
    
    BOOL Destroy()
    {
        if (m_hImageList != NULL)
        {
            ::ImageList_Destroy(m_hImageList);
            m_hImageList = NULL;
        }
        return TRUE;
    }
    
    int Add(HICON hIcon)
    {
        return ::ImageList_AddIcon(m_hImageList, hIcon);
    }
    
    BOOL Draw(HDC hdc, int i, int x, int y, UINT fStyle)
    {
        return ::ImageList_Draw(m_hImageList, i, hdc, x, y, fStyle);
    }
};

#endif // __WTL_COMPATIBILITY_H__
