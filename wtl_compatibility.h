// WTL include hub for Ditto.
// The MFC-emulation types that used to live here are gone: every window class is a
// real WTL implementation and the containers are std::vector / std::map.

#ifndef __WTL_COMPATIBILITY_H__
#define __WTL_COMPATIBILITY_H__

#pragma once

#include <windows.h>

#include <atltypes.h>
#include <atlbase.h>
#include <atlwin.h>
#include <atlstr.h>
#include <atltime.h>

#include <map>
#include <vector>

// Pristine WTL 10; atlapp.h must come first.
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

using ATL::CString;

#endif // __WTL_COMPATIBILITY_H__
