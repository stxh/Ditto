#pragma once

#include <atlctrls.h>

class CSearchEditBox : public CWindowImpl<CSearchEditBox, WTL::CEdit>
{
public:
	DECLARE_WND_SUPERCLASS(_T("DittoSearchEditBox"), GetWndClassName())

	CSearchEditBox();
	~CSearchEditBox();

	BEGIN_MSG_MAP(CSearchEditBox)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	END_MSG_MAP()

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
