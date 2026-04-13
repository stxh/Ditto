// Windows Template Library
// WTL - Windows Template Library
// Copyright (C) Microsoft Corporation, WTL Team
// All rights reserved.
//
// This file is a part of the Windows Template Library.
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/licenses/cpl1.0.php)
// which can be found in the file CPL.TXT at the root of this distribution.
// By using this software in any fashion, you are agreeing to be bound by
// the terms of this license. You must not remove this notice, or
// any other, from this software.

#ifndef __ATLFRAME_H__
#define __ATLFRAME_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#include <atlwin.h>

namespace WTL
{

// Frame window traits
class CFrameWinTraits
{
public:
	static DWORD GetWndStyle(DWORD dwStyle = 0)
	{
		return dwStyle == 0 ? (WS_OVERLAPPEDWINDOW) : dwStyle;
	}

	static DWORD GetWndExStyle(DWORD dwExStyle = 0)
	{
		return dwExStyle == 0 ? (WS_EX_APPWINDOW | WS_EX_WINDOWEDGE) : dwExStyle;
	}
};

// Control window traits
class CControlWinTraits
{
public:
	static DWORD GetWndStyle(DWORD dwStyle = 0)
	{
		return dwStyle == 0 ? (WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS) : dwStyle;
	}

	static DWORD GetWndExStyle(DWORD dwExStyle = 0)
	{
		return dwExStyle == 0 ? (0) : dwExStyle;
	}
};

}; // namespace WTL

using namespace WTL;

#endif // __ATLFRAME_H__