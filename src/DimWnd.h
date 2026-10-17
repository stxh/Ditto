#pragma once

#include "stdafx.h"

class CDimWnd : public CFrameWindowImpl<CDimWnd>
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, 0)

	CDimWnd(HWND hWndParent);

	BEGIN_MSG_MAP(CDimWnd)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	END_MSG_MAP()

	LRESULT OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL&);

	~CDimWnd();

private:
	HWND m_ParentWnd;
};
