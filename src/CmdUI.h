#pragma once

#include <atluser.h>

// State of one command while the menu that holds it is being initialised.
// WTL has no equivalent of MFC's CCmdUI, so changes are applied to the menu
// immediately instead of being collected for a later DoUpdate().
class CmdUI
{
public:
	CmdUI(HMENU hMenu, UINT nID, int nIndex) :
		m_menu(hMenu),
		m_pMenu(hMenu != NULL ? &m_menu : NULL),
		m_nID(nID),
		m_nIndex(nIndex)
	{
	}

	void Enable(BOOL bOn = TRUE) { ::EnableMenuItem(m_menu, m_nID, MF_BYCOMMAND | (bOn ? MF_ENABLED : MF_GRAYED)); }
	void SetCheck(int nCheck = 1) { ::CheckMenuItem(m_menu, m_nID, MF_BYCOMMAND | (nCheck == 0 ? MF_UNCHECKED : MF_CHECKED)); }
	void SetRadio(BOOL bOn = TRUE) { SetCheck(bOn ? 2 : 0); }
	void SetText(LPCTSTR lpszText) { ::ModifyMenu(m_menu, m_nID, MF_BYCOMMAND | MF_STRING, m_nID, lpszText); }

	WTL::CMenuHandle* m_pMenu;
	UINT m_nID;
	int m_nIndex;

private:
	WTL::CMenuHandle m_menu;
};
