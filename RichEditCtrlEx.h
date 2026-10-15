#pragma once

#ifndef _RICHEDITCTRL_EX_H
#define _RICHEDITCTRL_EX_H

#include <atlbase.h>
#include <atlctrls.h>
#include <richedit.h>
#include "wtl_compatibility.h"

// The rich edit APIs used here go through SendMessage/EM_STREAMIN, and the text services
// are loaded at run-time from msftedit.dll, so no import library is required.

/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx window

BOOL PASCAL AfxInitRichEditEx();

class _AFX_RICHEDITEX_STATE
{
public:
    _AFX_RICHEDITEX_STATE()
	{
		m_hInstRichEdit20 = NULL;
	}
    virtual ~_AFX_RICHEDITEX_STATE()
	{
		if(m_hInstRichEdit20 != NULL)
		{
			::FreeLibrary(m_hInstRichEdit20);
		}
    }

    HINSTANCE m_hInstRichEdit20;
};

class CRichEditCtrlEx : public CRichEditCtrl
{
public:
	CRichEditCtrlEx();
	~CRichEditCtrlEx();

	BOOL Create(HWND hWndParent, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER, DWORD dwExStyle = 0, LPRECT lpRect = nullptr);

	long GetSelectionFontSize();
	CString GetSelectionFontName();
	CStringArray m_saFontList;
	void GetSystemFonts(CStringArray &saFontList);
	void SetFontSize(int nPointSize);
	void SetFontName(CString sFontName);
	void SelectColor();
	bool ParagraphIsBulleted();
	void SetParagraphBulleted();
	
	PARAFORMAT GetParagraphFormat();
	
	bool ParagraphIsRight();
	bool ParagraphIsLeft();
	bool ParagraphIsCentered();
	
	void SetParagraphRight();
	void SetParagraphLeft();
	void SetParagraphCenter();
	
	CHARFORMAT GetCharFormat(DWORD dwMask = CFM_COLOR | CFM_FACE | CFM_SIZE | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE);
	
	bool SelectionIsBold();
	bool SelectionIsItalic();
	bool SelectionIsUnderlined();

	void SetSelectionBold();
	void SetSelectionItalic();
	void SetSelectionUnderlined();

	void SetRTF(CStringA sRTF);
	void SetRTF(const char *pRTF);
	CString GetRTF();

	void SetText(CString sText);
	CString GetText();

private:
	void SetCharStyle(int MASK, int STYLE, int nStart, int nEnd);
	static DWORD CALLBACK CBStreamIn(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
	static DWORD CALLBACK CBStreamOut(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG* pcb);
	static BOOL CALLBACK CBEnumFonts(LPLOGFONT lplf, LPTEXTMETRIC lptm, DWORD dwType, LPARAM lpData);
};

#endif // _RICHEDITCTRL_EX_H