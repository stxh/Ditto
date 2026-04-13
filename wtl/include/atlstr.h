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

#ifndef __ATLSTR_H__
#define __ATLSTR_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#include <atlbase.h>

namespace WTL
{

// CString implementation for WTL compatibility
class CString
{
public:
	CString() : m_pszData(NULL) { }

	CString(const CString& src) : m_pszData(NULL)
	{
		if(src.m_pszData != NULL)
		{
			int nLen = lstrlen(src.m_pszData);
			m_pszData = new TCHAR[nLen + 1];
			lstrcpy(m_pszData, src.m_pszData);
		}
	}

	CString(LPCTSTR lpsz) : m_pszData(NULL)
	{
		if(lpsz != NULL)
		{
			int nLen = lstrlen(lpsz);
			m_pszData = new TCHAR[nLen + 1];
			lstrcpy(m_pszData, lpsz);
		}
	}

	~CString()
	{
		Empty();
	}

	CString& operator=(const CString& src)
	{
		if(this != &src)
		{
			Empty();
			if(src.m_pszData != NULL)
			{
				int nLen = lstrlen(src.m_pszData);
				m_pszData = new TCHAR[nLen + 1];
				lstrcpy(m_pszData, src.m_pszData);
			}
		}
		return *this;
	}

	CString& operator=(LPCTSTR lpsz)
	{
		Empty();
		if(lpsz != NULL)
		{
			int nLen = lstrlen(lpsz);
			m_pszData = new TCHAR[nLen + 1];
			lstrcpy(m_pszData, lpsz);
		}
		return *this;
	}

	operator LPCTSTR() const { return m_pszData; }

	void Empty()
	{
		if(m_pszData != NULL)
		{
			delete[] m_pszData;
			m_pszData = NULL;
		}
	}

	int GetLength() const
	{
		return (m_pszData != NULL) ? lstrlen(m_pszData) : 0;
	}

	BOOL IsEmpty() const
	{
		return (m_pszData == NULL) || (m_pszData[0] == _T('\0'));
	}

	CString& Format(LPCTSTR lpszFormat, ...)
	{
		Empty();
		if(lpszFormat != NULL)
		{
			va_list argList;
			va_start(argList, lpszFormat);
			
			int nLen = _vsctprintf(lpszFormat, argList);
			if(nLen > 0)
			{
				m_pszData = new TCHAR[nLen + 1];
				_vstprintf_s(m_pszData, nLen + 1, lpszFormat, argList);
			}
			
			va_end(argList);
		}
		return *this;
	}

	CString& Format(UINT nFormatID, ...)
	{
		Empty();
		// Implementation for resource-based format strings
		return *this;
	}

	CString Mid(int nFirst) const
	{
		return Mid(nFirst, GetLength() - nFirst);
	}

	CString Mid(int nFirst, int nCount) const
	{
		CString strResult;
		if(m_pszData != NULL && nFirst >= 0 && nCount > 0)
		{
			int nLen = GetLength();
			if(nFirst < nLen)
			{
				if(nFirst + nCount > nLen)
					nCount = nLen - nFirst;
				
				strResult.m_pszData = new TCHAR[nCount + 1];
				_tcsncpy_s(strResult.m_pszData, nCount + 1, m_pszData + nFirst, nCount);
				strResult.m_pszData[nCount] = _T('\0');
			}
		}
		return strResult;
	}

	CString Left(int nCount) const
	{
		return Mid(0, nCount);
	}

	CString Right(int nCount) const
	{
		return Mid(GetLength() - nCount, nCount);
	}

	int Find(LPCTSTR lpszSub) const
	{
		if(m_pszData == NULL || lpszSub == NULL)
			return -1;
		
		LPCTSTR lpszFound = _tcsstr(m_pszData, lpszSub);
		return (lpszFound == NULL) ? -1 : (int)(lpszFound - m_pszData);
	}

	int Find(TCHAR ch) const
	{
		if(m_pszData == NULL)
			return -1;
		
		LPCTSTR lpszFound = _tcschr(m_pszData, ch);
		return (lpszFound == NULL) ? -1 : (int)(lpszFound - m_pszData);
	}

	CString& MakeLower()
	{
		if(m_pszData != NULL)
			_tcslwr_s(m_pszData, GetLength() + 1);
		return *this;
	}

	CString& MakeUpper()
	{
		if(m_pszData != NULL)
			_tcsupr_s(m_pszData, GetLength() + 1);
		return *this;
	}

	CString& Trim()
	{
		TrimLeft();
		TrimRight();
		return *this;
	}

	CString& TrimLeft()
	{
		if(m_pszData != NULL)
		{
			LPCTSTR lpsz = m_pszData;
			while(_istspace(*lpsz))
				lpsz++;
			
			if(lpsz != m_pszData)
			{
				int nLen = lstrlen(lpsz);
				LPTSTR pszNew = new TCHAR[nLen + 1];
				lstrcpy(pszNew, lpsz);
				delete[] m_pszData;
				m_pszData = pszNew;
			}
		}
		return *this;
	}

	CString& TrimRight()
	{
		if(m_pszData != NULL)
		{
			int nLen = GetLength();
			LPCTSTR lpsz = m_pszData + nLen;
			while(lpsz > m_pszData && _istspace(*(lpsz - 1)))
				lpsz--;
			
			if(lpsz != m_pszData + nLen)
			{
				int nNewLen = (int)(lpsz - m_pszData);
				LPTSTR pszNew = new TCHAR[nNewLen + 1];
				_tcsncpy_s(pszNew, nNewLen + 1, m_pszData, nNewLen);
				pszNew[nNewLen] = _T('\0');
				delete[] m_pszData;
				m_pszData = pszNew;
			}
		}
		return *this;
	}

private:
	LPTSTR m_pszData;
};

}; // namespace WTL

using namespace WTL;

#endif // __ATLSTR_H__