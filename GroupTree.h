#pragma once

#include <atlctrls.h>
#include "resource.h"
#include "Accels.h"

class CGroupTree : public CWindowImpl<CGroupTree, CTreeViewCtrl>
{
public:
	DECLARE_WND_SUPERCLASS(_T("Ditto_GroupTree"), CTreeViewCtrl::GetWndClassName())

	CGroupTree();
	~CGroupTree();

	BOOL PreTranslateMessage(MSG* pMsg);

	void FillTree();
	void SetNotificationWndEx(HWND hWnd)	{ m_NotificationWnd = hWnd;	}
	bool m_bHide;
	bool m_bSendAllready;
	int m_selectedFolderID;
	int GetSelectedTree();
	bool AddNode(CString csText, int id);
	void DeleteAll();

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnReflectNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMenuNewgroup32896(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled);
	LRESULT OnMenuDeletegroup(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled);
	LRESULT OnMenuProperties32898(WORD wID, WORD wNotifyCode, HWND hWnd, BOOL& bHandled);
	LRESULT OnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BEGIN_MSG_MAP_EX(CGroupTree)
		MSG_WM_CREATE(OnCreate)
		MESSAGE_HANDLER(OCM_NOTIFY, OnReflectNotify)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		COMMAND_ID_HANDLER(ID_MENU_NEWGROUP32896, OnMenuNewgroup32896)
		COMMAND_ID_HANDLER(ID_MENU_DELETEGROUP, OnMenuDeletegroup)
		COMMAND_ID_HANDLER(ID_MENU_PROPERTIES32898, OnMenuProperties32898)
		MESSAGE_HANDLER(WM_INITMENUPOPUP, OnInitMenuPopup)
	END_MSG_MAP()

	bool m_showRightClickMenu;

private:
	HWND m_NotificationWnd;

	void ShowGroupMenu();
	void UpdateMenuShortCut(HMENU hMenu, DWORD action, UINT nID);

protected:
	void FillTree(int parentId, HTREEITEM hParent);
	void SendToParent(int parentId);
	UINT GetSelectedCount() const;
	bool CheckActions(MSG * pMsg);
	bool DoAction(DWORD actionId);
	bool DoActionNewGroup();
	bool DoActionDeleteSelected();
	bool DoActionClipProperties();
	UINT DoActionPasteFromGroup(int groupId);
	bool DoActionEditSelected();
	bool DoActionShowOnly();
	bool DoActionShowOnlyNextPrev();
	bool DoActionCopyAs();
	bool DoActionPasteToGroup(int groupId);
	CString GetSelectedNodeText();
	HTREEITEM GetSelectedNode();
	int GetSelectedGroupId();
	int GetParentGroupId(int groupId);
	bool AddNewGroup(CString newGroupName);
	bool DeleteGroup(int groupId);
	bool RenameGroup(int groupId, CString newName);
	void RefreshGroupNames();
	int GetGroupIdFromName(CString name);
	int GetGroupParentId(int groupId);
	CString GetGroupName(int groupId);
	int GetGroupNodeType(int groupId);

	CString GetNodeText(HTREEITEM hItem);
	int GetNodeId(HTREEITEM hItem);
	HTREEITEM GetNodeFromId(int id);

	CAccels m_actions;

	HTREEITEM m_selectedItem;
	HTREEITEM m_rightClickItem;

	HTREEITEM m_lastCopiedItem;
	int lastCopiedGroupId;
	bool m_expandOnSelect;

	CBitmap m_bmClosedFolder;
	CBitmap m_bmOpenFolder;
};
