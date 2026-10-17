// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
// MIGRATED VERSION - From MFC to WTL 10

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
// MFC's VERIFY always evaluates its argument; only the check itself is debug-only.
#ifdef _DEBUG
#define VERIFY(expr) ATLASSERT(expr)
#else
#define VERIFY(expr) ((void)(expr))
#endif
#endif

#ifndef TRACE
#define TRACE ATLTRACE
#endif

// MFC's DEBUG_NEW routed allocations through the leak tracker; nothing here does,
// so keep the debug-only allocations in the sources compiling as plain new.
#ifdef _DEBUG
#define DEBUG_NEW new
#endif

using ATL::CTime;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__56F3D184_7208_47FE_AFE2_E270325F356A__INCLUDED_)