#pragma once
#include "WndEx.h"
#include <atltypes.h>

class CDittoPopupWindow : public CWndEx
{
public:
	CDittoPopupWindow();
	~CDittoPopupWindow();

	void UpdateText(CString text);
	void SetProgressBarPercent(int percent);
	void HideProgressBar();

	void SetCopyToGroupId(int groupId) { m_groupId = groupId; }
	
protected:
	BEGIN_MSG_MAP_EX(CDittoPopupWindow)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_SIZE(OnSize)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		CHAIN_MSG_MAP(CWndEx)
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnSize(UINT nType, CSize size);
	LRESULT OnCtlColorStatic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnLButtonUp(UINT nFlags, CPoint pt);


	WTL::CStatic m_textLabel;
	WTL::CProgressBarCtrl m_progressWnd;
	WTL::CFont m_font;
	int m_groupId;

	void PumpMessages();

	void DoSize(int cx, int cy);
};
