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

#endif // __WTL_COMPATIBILITY_H__