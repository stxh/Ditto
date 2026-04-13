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

#ifndef __ATLDLGS_H__
#define __ATLDLGS_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#include <atlwin.h>

namespace WTL
{

// Dialog base class
template <class T>
class CDialogImpl : public CWindowImpl<T>
{
public:
	const WORD m_lpDialogTemplate;

	CDialogImpl() : m_lpDialogTemplate(0) { }

	BOOL EndDialog(int nRetCode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::EndDialog(m_hWnd, nRetCode);
	}

	BEGIN_MSG_MAP(CDialogImpl)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return TRUE;
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		EndDialog(IDCANCEL);
		return 0;
	}
};

// Property sheet implementation
template <class T>
class CPropertySheetImpl : public CWindowImpl<T>
{
public:
	CPropertySheetImpl() { }

	void AddPage(HPROPSHEETPAGE hPage) { }
	void SetTitle(LPCTSTR lpszText) { }
};

// Property page implementation
template <class T>
class CPropertyPageImpl : public CDialogImpl<T>
{
public:
	BEGIN_MSG_MAP(CPropertyPageImpl)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		CHAIN_MSG_MAP(CDialogImpl<T>)
	END_MSG_MAP()

	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}
};

}; // namespace WTL

using namespace WTL;

#endif // __ATLDLGS_H__