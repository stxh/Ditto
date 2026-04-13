// Test complete stdafx.h simulation
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

// WTL Headers (replacing MFC)
#include <atlbase.h>           // ATL base classes and smart pointers
#include "E:\PubCode\Ditto\wtl\include\atlapp.h"            // WTL application classes
#include "E:\PubCode\Ditto\wtl\include\atlwin.h"            // WTL window classes
#include "E:\PubCode\Ditto\wtl\include\atlframe.h"          // WTL frame window classes
#include "E:\PubCode\Ditto\wtl\include\atldlgs.h"           // WTL dialog classes
#include "E:\PubCode\Ditto\wtl\include\atlctrls.h"          // WTL control classes
#include "E:\PubCode\Ditto\wtl\include\atlctrlx.h"          // WTL extended control classes
#include "E:\PubCode\Ditto\wtl\include\atlstr.h"            // WTL CString implementation

// Unicode support
#include "E:\PubCode\Ditto\UnicodeMacros.h"

int main() {
    return 0;
}

#endif // !defined(AFX_STDAFX_H__56F3D184_7208_47FE_AFE2_E270325F356A__INCLUDED_)