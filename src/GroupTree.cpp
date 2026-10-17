// GroupTree.cpp : implementation file
//

#include "stdafx.h"
#include "cp_main.h"
#include "GroupTree.h"
#include "ActionEnums.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupTree

CGroupTree::CGroupTree()
{
	m_bHide = true;
	m_selectedFolderID = -1;
	m_bSendAllready = false;
	m_showRightClickMenu = false;
}

CGroupTree::~CGroupTree()
{
}


/////////////////////////////////////////////////////////////////////////////
// CGroupTree message handlers

int CGroupTree::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CImageList iml;
	iml.Create(18, 16, ILC_COLOR | ILC_MASK, 0, 1);

	m_bmClosedFolder.LoadBitmap(IDB_CLOSED_FOLDER);
	iml.Add(m_bmClosedFolder, RGB(255, 0, 0));

	m_bmOpenFolder.LoadBitmap(IDB_CLOSED_FOLDER);
	iml.Add(m_bmOpenFolder, RGB(255, 0, 0));
	

	SetImageList(iml, TVSIL_NORMAL);
	iml.Detach();

	m_actions.AddAccel(ActionEnums::NEWGROUP, ACCEL_MAKEKEY(VK_F7, HOTKEYF_CONTROL));
	m_actions.AddAccel(ActionEnums::CLIP_PROPERTIES, ACCEL_MAKEKEY(VK_RETURN, HOTKEYF_ALT));
	m_actions.AddAccel(ActionEnums::DELETE_SELECTED, VK_DELETE);

	

	return 0;
}

BOOL CGroupTree::PreTranslateMessage(MSG *pMsg)
{
	if(CheckActions(pMsg))
	{
		return TRUE;
	}

	return FALSE;
}

bool CGroupTree::CheckActions(MSG * pMsg) 
{
	bool ret = false;
	CAccel a;

	if (m_actions.OnMsg(pMsg, a))
	{
		ret = DoAction(a.Cmd);
	}   

	return ret;
}

bool CGroupTree::DoAction(DWORD actionId)
{
	bool ret = false;

	switch (actionId)
	{
	case ActionEnums::NEWGROUP:
		ret = DoActionNewGroup();
		break;
	case ActionEnums::DELETE_SELECTED:
		ret = DoActionDeleteSelected();
		break;
	case ActionEnums::CLIP_PROPERTIES:
		ret = DoActionClipProperties();
		break;
	}

	return ret;
}

bool CGroupTree::DoActionNewGroup()
{
	HTREEITEM hItem = GetSelectedItem();
	if (hItem)
	{
		int id = (int) GetItemData(hItem);
		::PostMessage(m_NotificationWnd, NM_NEW_GROUP, id, 0);	
		return true;
	}

	return false;
}

bool CGroupTree::DoActionDeleteSelected()
{
	HTREEITEM hItem = GetSelectedItem();
	if (hItem)
	{
		int id = (int) GetItemData(hItem);
		if (id >= 0)
		{
			::PostMessage(m_NotificationWnd, NM_DELETE_ID, id, 0);
			return true;
		}
	}

	return false;
}

bool CGroupTree::DoActionClipProperties()
{
	HTREEITEM hItem = GetSelectedItem();
	if (hItem)
	{
		int id = (int) GetItemData(hItem);
		if (id >= 0)
		{
			::PostMessage(m_NotificationWnd, NM_SHOW_PROPERTIES, id, 0);
			return true;
		}
	}
	return false;
}

void CGroupTree::FillTree()
{	
	this->SetBkColor(CGetSetOptions::m_Theme.GroupTreeBG());
	this->SetTextColor(CGetSetOptions::m_Theme.GroupTreeText());

	DeleteAllItems();
	m_bSendAllready = false;

	HTREEITEM hItem = InsertItem(_T("-No Group-"), TVI_ROOT, TVI_LAST);
	SetItemData(hItem, -1);

	SetItemState(hItem, TVIS_EXPANDED, TVIS_EXPANDED);

	if(m_selectedFolderID < 0)
		SelectItem(hItem);
	
	FillTree(-1, hItem);
}


void CGroupTree::FillTree(int parentID, HTREEITEM hParent)
{	
	try
	{
		CppSQLite3Query q = theApp.m_db.execQueryEx(_T("SELECT lID, mText FROM Main WHERE bIsGroup = 1 AND lParentID = %d"), parentID);
			
		if(q.eof() == false)
		{
			HTREEITEM hItem;

			while(!q.eof())
			{
				if(q.getIntField(_T("lID")) == m_selectedFolderID)
				{
					hItem = InsertItem(q.getStringField(_T("mText")), 1, 1, hParent, TVI_LAST);
					SelectItem(hItem);
				}
				else
				{				
					hItem = InsertItem(q.getStringField(_T("mText")), 0, 0, hParent, TVI_LAST);
				}

				SetItemData(hItem, q.getIntField(_T("lID")));
				
				FillTree(q.getIntField(_T("lID")), hItem);

				q.nextRow();
			}
		}
	}		
	CATCH_SQLITE_EXCEPTION	
}

LRESULT CGroupTree::OnReflectNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPNMHDR pNMHDR = (LPNMHDR)lParam;

	switch (pNMHDR->code)
	{
	case TVN_KEYDOWN:
		{
			TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
			
			switch(pTVKeyDown->wVKey)
			{
			case VK_RETURN:
			{
				HTREEITEM hItem = GetNextItem(TVI_ROOT, TVGN_CARET);
				if(hItem)
					SendToParent((int)GetItemData(hItem));
				
				break;
			}
			case VK_ESCAPE:
				SendToParent(-1);
				break;
			}
			
			break;
		}
	case NM_DBLCLK:
		{
			HTREEITEM hItem = GetNextItem(TVI_ROOT, TVGN_CARET);
			if(hItem)
				SendToParent((int)GetItemData(hItem));
			
			break;
		}
	case NM_RCLICK:
		ShowGroupMenu();
		break;
	default:
		bHandled = FALSE;
		break;
	}

	return 0;
}

LRESULT CGroupTree::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_bHide)
	{
		if (wParam == WA_INACTIVE)
		{
			SendToParent(-1);
		}
	}

	bHandled = FALSE;
	return 0;
}

int CGroupTree::GetSelectedTree()
{
	HTREEITEM hItem =  GetNextItem(TVI_ROOT, TVGN_CARET);
	if(hItem)
		return (int)GetItemData(hItem);

	return -1;
}


void CGroupTree::SendToParent(int parentId)
{
	if(m_bSendAllready == false)
	{
		m_bSendAllready = true;
		::PostMessage(m_NotificationWnd, NM_GROUP_TREE_MESSAGE, parentId, 0);
	}
}

bool CGroupTree::AddNode(CString csText, int id)
{
	HTREEITEM hItem;

	HTREEITEM hParent =  GetNextItem(TVI_ROOT, TVGN_CARET);
	if(hParent == NULL)
		return false;
		
	hItem = InsertItem(csText, 1, 1, hParent, TVI_LAST);
	SelectItem(hItem);

	SetItemData(hItem, id);

	return true;
}

LRESULT CGroupTree::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPoint point(LOWORD(lParam), HIWORD(lParam));
	UINT nHitFlags = 0;
	HTREEITEM hClickedItem = HitTest(point, &nHitFlags);

	if (nHitFlags&TVHT_ONITEM)
		if (GetSelectedCount() < 2)
			SelectItem(hClickedItem);

	bHandled = FALSE;
	return 0;
}

UINT CGroupTree::GetSelectedCount() const
{
	// Only visible items should be selected!
	UINT uCount = 0;
	for (HTREEITEM hItem = GetRootItem(); hItem != NULL; hItem = GetNextVisibleItem(hItem))
		if (GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
			uCount++;

	return uCount;
}

void CGroupTree::ShowGroupMenu()
{
	if (m_showRightClickMenu == false)
		return;

	POINT pp;
	CMenu cmPopUp;
	CMenuHandle cmSubMenu;

	GetCursorPos(&pp);
	if (cmPopUp.LoadMenu(IDR_MENU_GROUPS) != 0)
	{
		cmSubMenu = cmPopUp.GetSubMenu(0);
		if (cmSubMenu.IsNull())
			return;

		theApp.m_Language.UpdateGroupsRightClickMenu(cmSubMenu);
		
		cmSubMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, pp.x, pp.y, m_hWnd, NULL);
	}
}

LRESULT CGroupTree::OnMenuNewgroup32896(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled)
{
	DoAction(ActionEnums::NEWGROUP);
	return 0;
}

LRESULT CGroupTree::OnMenuDeletegroup(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled)
{
	DoAction(ActionEnums::DELETE_SELECTED);
	return 0;
}

LRESULT CGroupTree::OnMenuProperties32898(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled)
{
	DoAction(ActionEnums::CLIP_PROPERTIES);
	return 0;
}

void CGroupTree::UpdateMenuShortCut(HMENU hMenu, DWORD action, UINT nID)
{
	CString shortcutText = m_actions.GetCmdKeyText(action);
	if(shortcutText.IsEmpty())
		return;

	CString needle(_T("\t"));
	needle += shortcutText;

	CMenuHandle menu(hMenu);
	CString cs;
	if(menu.GetMenuString(nID, cs, MF_BYCOMMAND) == 0)
		return;

	if(cs.Find(needle) < 0)
	{
		cs += needle;
		menu.ModifyMenu(nID, MF_BYCOMMAND | MF_STRING, nID, cs);
	}
}

LRESULT CGroupTree::OnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HMENU hMenu = (HMENU)wParam;

	UpdateMenuShortCut(hMenu, ActionEnums::NEWGROUP, ID_MENU_NEWGROUP32896);
	UpdateMenuShortCut(hMenu, ActionEnums::DELETE_SELECTED, ID_MENU_DELETEGROUP);
	UpdateMenuShortCut(hMenu, ActionEnums::CLIP_PROPERTIES, ID_MENU_PROPERTIES32898);

	bHandled = FALSE;
	return 0;
}