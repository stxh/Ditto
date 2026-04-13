#if !defined(AFX_OPTIONSSHEET_H__D8A13849_DBC6_4CD6_A981_E572ECDC2E94__INCLUDED_)
#define AFX_OPTIONSSHEET_H__D8A13849_DBC6_4CD6_A981_E572ECDC2E94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include <atlbase.h>
#include <atlwin.h>
#include <atldlgs.h>

class COptionsGeneral;
class COptionsKeyBoard;
class CQuickPasteKeyboard;
class COptionsUtilities;
class COptionsStats;
class COptionsTypes;
class COptionAbout;
class COptionFriends;
class COptionsCopyBuffers;
class CQuickPasteShortCut;

class COptionsSheet : public CPropertySheetImpl<COptionsSheet>
{
public:
	COptionsSheet(LPCTSTR pszCaption, HWND hWndParent = NULL, UINT iSelectPage = 0);

	void SetNotifyWnd(HWND hWnd);

	BOOL m_themeChanged;

public:
	BEGIN_MSG_MAP(COptionsSheet)
		MSG_WM_INITDIALOG(OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_NCDESTROY, OnNcDestroy)
		CHAIN_MSG_MAP(CPropertySheetImpl<COptionsSheet>)
	END_MSG_MAP()

	BOOL OnInitDialog(HWND hWndFocus, LPARAM lParam);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	virtual ~COptionsSheet();

protected:
	HWND m_hWndParent;

	COptionsGeneral* m_pGeneralOptions;
	COptionsKeyBoard* m_pKeyBoardOptions;
	COptionsUtilities* m_pUtilites;
	COptionsStats* m_pStats;
	COptionsTypes* m_pTypes;
	COptionAbout* m_pAbout;
	COptionFriends* m_pFriends;
	COptionsCopyBuffers* m_pCopyBuffers;
	CQuickPasteKeyboard* m_pQuickPasteShortCuts;
};

#endif
