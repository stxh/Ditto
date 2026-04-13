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

#ifndef __ATLAPP_H__
#define __ATLAPP_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef ATLASSERT
	#define ATLASSERT(expr) _ASSERTE(expr)
#endif

#include <atlbase.h>

namespace WTL
{

class CAppModule : public CComModule
{
public:
	DWORD m_dwMainThreadID;
	HINSTANCE m_hInstance;
	HINSTANCE m_hInstanceResource;

	CAppModule() : m_dwMainThreadID(0), m_hInstance(NULL), m_hInstanceResource(NULL)
	{ }

	HRESULT Init(HINSTANCE hInstance, const GUID* pLibID = NULL)
	{
		m_dwMainThreadID = ::GetCurrentThreadId();
		m_hInstance = hInstance;
		m_hInstanceResource = hInstance;
		return CComModule::Init(NULL, hInstance, pLibID);
	}

	void Term()
	{
		CComModule::Term();
	}
};

}; // namespace WTL

using namespace WTL;

#endif // __ATLAPP_H__