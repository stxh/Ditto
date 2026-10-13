// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
// MIGRATED VERSION - From MFC to WTL 10 with Compatibility Layer

#if !defined(AFX_STDAFX_H__56F3D184_7208_47FE_AFE2_E270325F356A__INCLUDED_)
#define AFX_STDAFX_H__56F3D184_7208_47FE_AFE2_E270325F356A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4995)

//remove 2005 warnings
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1

#define HITTEST_RET LRESULT

#define _WIN32_WINNT 0x0A00
#define WINVER 0x0A00

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

// ATL Headers (required for WTL)
#include <atlbase.h>           // ATL base classes and smart pointers
#include <atlwin.h>           // ATL window classes - REQUIRED for WTL compatibility
#include <atlstr.h>           // ATL CString implementation
#include <atlsync.h>          // ATL CCriticalSection/CEvent/CMutex
#include <atltrace.h>         // ATL tracing support

// Windows headers (before WTL to avoid conflicts)
#include <Winsock2.h>
#include <imm.h>

// WTL Compatibility Layer (imports core WTL headers)
#include "wtl_compatibility.h"

// Win32 file helpers used in place of MFC's CFile/CFileException/CFileFind
#include "FileCompat.h"

// Win32/COM helpers used in place of MFC's COleDataObject/COleDataSource
#include "OleDataCompat.h"

// Unicode support
#include "UnicodeMacros.h"

// RichEdit support
#import "riched20.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids, exclude("UINT_PTR"), exclude("LONG_PTR") 

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// GDI+ support
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

// Standard library headers
#include <string>
#include <vector>
#include <memory>
#include <atlcomtime.h>

#ifndef ASSERT
#define ASSERT ATLASSERT
#endif

#ifndef VERIFY
#define VERIFY(expr) ATLASSERT(expr)
#endif

#ifndef TRACE
#define TRACE ATLTRACE
#endif

using ATL::CTime;

class CStringArray
{
public:
    using value_type = CString;

    INT_PTR GetSize() const { return static_cast<INT_PTR>(m_data.size()); }
    INT_PTR GetCount() const { return GetSize(); }
    INT_PTR GetUpperBound() const { return GetSize() - 1; }
    BOOL IsEmpty() const { return m_data.empty() ? TRUE : FALSE; }

    void SetSize(INT_PTR nNewSize)
    {
        ASSERT(nNewSize >= 0);
        m_data.resize(static_cast<size_t>(nNewSize));
    }

    INT_PTR Add(const CString& element)
    {
        m_data.push_back(element);
        return GetUpperBound();
    }

    void RemoveAll()
    {
        m_data.clear();
    }

    void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1)
    {
        ASSERT(nCount >= 0);
        ASSERT(0 <= nIndex && nIndex + nCount <= GetSize());
        m_data.erase(m_data.begin() + static_cast<size_t>(nIndex),
                     m_data.begin() + static_cast<size_t>(nIndex + nCount));
    }

    CString& GetAt(INT_PTR nIndex)
    {
        ASSERT(0 <= nIndex && nIndex < GetSize());
        return m_data[static_cast<size_t>(nIndex)];
    }

    const CString& GetAt(INT_PTR nIndex) const
    {
        ASSERT(0 <= nIndex && nIndex < GetSize());
        return m_data[static_cast<size_t>(nIndex)];
    }

    CString& operator[](INT_PTR nIndex) { return GetAt(nIndex); }
    const CString& operator[](INT_PTR nIndex) const { return GetAt(nIndex); }

    CString& ElementAt(INT_PTR nIndex) { return GetAt(nIndex); }
    const CString& ElementAt(INT_PTR nIndex) const { return GetAt(nIndex); }

    void SetAt(INT_PTR nIndex, const CString& element)
    {
        ASSERT(0 <= nIndex && nIndex < GetSize());
        m_data[static_cast<size_t>(nIndex)] = element;
    }

    void SetAtGrow(INT_PTR nIndex, const CString& element)
    {
        if (nIndex >= GetSize())
        {
            SetSize(nIndex + 1);
        }
        SetAt(nIndex, element);
    }

    void InsertAt(INT_PTR nIndex, const CString& element)
    {
        ASSERT(0 <= nIndex && nIndex <= GetSize());
        m_data.insert(m_data.begin() + static_cast<size_t>(nIndex), element);
    }

    INT_PTR Append(const CStringArray& src)
    {
        m_data.insert(m_data.end(), src.m_data.begin(), src.m_data.end());
        return GetSize();
    }

    void Copy(const CStringArray& src)
    {
        m_data = src.m_data;
    }

    CString* GetData()
    {
        return m_data.empty() ? nullptr : m_data.data();
    }

    const CString* GetData() const
    {
        return m_data.empty() ? nullptr : m_data.data();
    }

private:
    std::vector<CString> m_data;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__56F3D184_7208_47FE_AFE2_E270325F356A__INCLUDED_)