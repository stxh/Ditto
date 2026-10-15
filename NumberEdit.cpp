// NumberEdit.cpp : implementation file
//

#include "stdafx.h"
#include "CP_Main.h"
#include "NumberEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumberEdit

CNumberEdit::CNumberEdit()
{
	m_dMax = LONG_MAX;
}

CNumberEdit::~CNumberEdit()
{
}


/////////////////////////////////////////////////////////////////////////////
// CNumberEdit message handlers

LRESULT CNumberEdit::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nChar = static_cast<UINT>(wParam);

	//Only allow the number 0 - 9 and a the backspace to go through
	if(((nChar < '0') || (nChar > '9')) && (nChar != VK_BACK))
		return 0;

	CString csText;
	GetWindowText(csText);

	//Save this if the validate fails then these get set back
	int nStartChar, nEndChar;
	GetSel(nStartChar, nEndChar);

	//Set the new number
	LRESULT lResult = DefWindowProc(uMsg, wParam, lParam);
	
	//If its not valid set it back to the old number
	if(!ValidateNumber(GetNumberD()))
	{
		SetWindowText(csText);
		SetSel(nStartChar, nEndChar);
	}

	return lResult;
}

BOOL CNumberEdit::ValidateNumber(double dNumber)
{
	if(dNumber > m_dMax)
		return FALSE;

	return TRUE;
}

long CNumberEdit::GetNumber()
{
	CString csText;
	GetWindowText(csText);
	
	return ATOL(csText);
}

double CNumberEdit::GetNumberD()
{
	CString csText;
	GetWindowText(csText);

#ifdef _UNICODE				
	TCHAR *pEnd;
	double d = _tcstod(csText, &pEnd);
#else
	double d = atof(csText);
#endif

	return d;
}

BOOL CNumberEdit::SetNumber(long lNumber)
{
	//Check if its a good number
	if(!ValidateNumber(lNumber))
	{
		MessageBeep(0);
		return FALSE;
	}

	//Its good
	CString csText;
	csText.Format(_T("%d"), lNumber);
	SetWindowText(csText);

	return TRUE;
}

