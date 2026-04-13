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

#ifndef __ATLCTRLS_H__
#define __ATLCTRLS_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#include <atlwin.h>

namespace WTL
{

// Base control class
class CWindow : public CWindowImplRoot<CWindow>
{
public:
	CWindow(HWND hWnd = NULL) { m_hWnd = hWnd; }

	CWindow& operator =(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(LPCTSTR lpstrWndClass, HWND hWndParent, RECT& rcPos, LPCTSTR lpstrWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0, UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		m_hWnd = ::CreateWindowEx(dwExStyle, lpstrWndClass, lpstrWindowName, dwStyle,
			rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top,
			hWndParent, (HMENU)nID, ModuleHelper::GetModuleInstance(), lpCreateParam);
		return m_hWnd;
	}

	BOOL Attach(HWND hWndNew)
	{
		ATLASSERT(m_hWnd == NULL);
		ATLASSERT(::IsWindow(hWndNew));
		m_hWnd = hWndNew;
		return TRUE;
	}

	HWND Detach()
	{
		HWND hWnd = m_hWnd;
		m_hWnd = NULL;
		return hWnd;
	}

	BOOL ShowWindow(int nCmdShow) { return ::ShowWindow(m_hWnd, nCmdShow); }
	BOOL UpdateWindow() { return ::UpdateWindow(m_hWnd); }
	BOOL DestroyWindow() { return ::DestroyWindow(m_hWnd); }
	BOOL IsWindow() const { return ::IsWindow(m_hWnd); }
};

// Button control
class CButton : public CWindow
{
public:
	CButton(HWND hWnd = NULL) : CWindow(hWnd) { }

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0, UINT nID = 0)
	{
		return CWindow::Create(_T("BUTTON"), hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID);
	}
};

// Edit control
class CEdit : public CWindow
{
public:
	CEdit(HWND hWnd = NULL) : CWindow(hWnd) { }

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0, UINT nID = 0)
	{
		return CWindow::Create(_T("EDIT"), hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID);
	}

	BOOL GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::GetWindowText(m_hWnd, lpszStringBuf, nMaxCount);
	}

	int GetWindowTextLength() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::GetWindowTextLength(m_hWnd);
	}

	void SetWindowText(LPCTSTR lpszString)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SetWindowText(m_hWnd, lpszString);
	}
};

// Static control
class CStatic : public CWindow
{
public:
	CStatic(HWND hWnd = NULL) : CWindow(hWnd) { }

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0, UINT nID = 0)
	{
		return CWindow::Create(_T("STATIC"), hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID);
	}
};

// Combo box control
class CComboBox : public CWindow
{
public:
	CComboBox(HWND hWnd = NULL) : CWindow(hWnd) { }

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0, UINT nID = 0)
	{
		return CWindow::Create(_T("COMBOBOX"), hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID);
	}
};

// List view control
class CListViewCtrl : public CWindow
{
public:
	CListViewCtrl(HWND hWnd = NULL) : CWindow(hWnd) { }

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0, UINT nID = 0)
	{
		return CWindow::Create(WC_LISTVIEW, hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID);
	}

	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LVCOLUMN column = { 0 };
		column.mask = LVCF_TEXT | LVCF_FMT;
		column.pszText = (LPTSTR)lpszColumnHeading;
		column.fmt = nFormat;
		if(nWidth != -1)
		{
			column.mask |= LVCF_WIDTH;
			column.cx = nWidth;
		}
		if(nSubItem != -1)
		{
			column.mask |= LVCF_SUBITEM;
			column.iSubItem = nSubItem;
		}
		return (int)::SendMessage(m_hWnd, LVM_INSERTCOLUMN, nCol, (LPARAM)&column);
	}

	int InsertItem(int nItem, LPCTSTR lpszItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LVITEM item = { 0 };
		item.mask = LVIF_TEXT;
		item.iItem = nItem;
		item.pszText = (LPTSTR)lpszItem;
		return (int)::SendMessage(m_hWnd, LVM_INSERTITEM, 0, (LPARAM)&item);
	}
};

// Tree view control
class CTreeViewCtrl : public CWindow
{
public:
	CTreeViewCtrl(HWND hWnd = NULL) : CWindow(hWnd) { }

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0, UINT nID = 0)
	{
		return CWindow::Create(WC_TREEVIEW, hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID);
	}

	HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		TVINSERTSTRUCT tvis = { 0 };
		tvis.hParent = hParent;
		tvis.hInsertAfter = hInsertAfter;
		tvis.item.mask = TVIF_TEXT;
		tvis.item.pszText = (LPTSTR)lpszItem;
		return (HTREEITEM)::SendMessage(m_hWnd, TVM_INSERTITEM, 0, (LPARAM)&tvis);
	}

	BOOL DeleteItem(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TVM_DELETEITEM, 0, (LPARAM)hItem);
	}
};

}; // namespace WTL

using namespace WTL;

#endif // __ATLCTRLS_H__