#if !defined(AFX_GROUPCOMBO_H__42D00360_95BE_49B5_AA39_E3B37CFCFF5B__INCLUDED_)
#define AFX_GROUPCOMBO_H__42D00360_95BE_49B5_AA39_E3B37CFCFF5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupCombo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGroupCombo window

class CGroupCombo : public CComboBox
{
// Construction
public:
	CGroupCombo();

// Implementation
public:
	virtual ~CGroupCombo();

	void FillCombo();
	void FillCombo(long lParentID, long lSpaces);
	BOOL SetCurSelOnItemData(long lItemData);
	int GetItemDataFromCursel();

	long m_lSkipGroupID;
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_GROUPCOMBO_H__42D00360_95BE_49B5_AA39_E3B37CFCFF5B__INCLUDED_)
