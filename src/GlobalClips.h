#pragma once

#include "DialogResizer.h"
#include "ShowTaskBarIcon.h"
#include <atlbase.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>

class GlobalClips : public CDialogImpl<GlobalClips>
{
public:
	enum { IDD = IDD_GLOBAL_CLIPS };

	GlobalClips() : m_hWndParent(NULL) {}

	void SetNotifyWnd(HWND hWnd);

	BEGIN_MSG_MAP(GlobalClips)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_NCDESTROY, OnNcDestroy)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnNcDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
	void InitListCtrlCols();
	void LoadItems();

	CDialogResizer m_Resize;
	CListViewCtrl m_List;
	HWND m_hWndParent;
	CShowTaskBarIcon m_showTaskbar;
};
