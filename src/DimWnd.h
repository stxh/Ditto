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

	// All CDimWnd instances are stack objects (MFC ~CWnd semantics); prevent the WTL default delete this.
	void OnFinalMessage(HWND /*hWnd*/) {}

private:
	HWND m_ParentWnd;
};
