#pragma once

#include <atlctrls.h>

class CSearchEditBox : public WTL::CEdit
{
public:
	CSearchEditBox();
	~CSearchEditBox();

	BOOL PreTranslateMessage(MSG* pMsg);
};