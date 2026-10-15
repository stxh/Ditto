#pragma once

//DISCLAIMER:
//The code in this project is Copyright (C) 2006 by Gautam Jain. You have the right to
//use and distribute the code in any way you see fit as long as this paragraph is included
//with the distribution. No warranties or claims are made as to the validity of the
//information and code contained herein, so use it at your own risk.


// CEditWithButton

class CEditWithButton : public CWindowImpl<CEditWithButton, CEdit>
{
public:
	DECLARE_WND_SUPERCLASS(_T("DittoEditWithButton"), GetWndClassName())

protected:
	CBitmap m_bmpEmptyEdit;
	CBitmap m_bmpFilledEdit;
	CSize   m_sizeEmptyBitmap;
	CSize   m_sizeFilledBitmap;
	CRect	m_rcEditArea;
	CRect	m_rcBorder;
	CRect	m_rcButtonArea;
	BOOL	m_bButtonExistsAlways;
	UINT	m_iButtonClickedMessageId;

public:
	BOOL SetBitmaps(UINT iEmptyEdit, UINT iFilledEdit);
	void SetButtonArea(CRect rcButtonArea);
	BOOL SetBorder(CRect rcEditArea);
	void SetButtonExistsAlways(BOOL bButtonExistsAlways);

	CEditWithButton();
	virtual ~CEditWithButton();
	virtual void PreSubclassWindow();

protected:

	void ResizeWindow();

	BEGIN_MSG_MAP(CEditWithButton)
		MESSAGE_HANDLER(WM_SETFONT, OnSetFont)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

public:
	
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
