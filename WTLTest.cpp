// WTL Test File - Basic compilation test
#include "stdafx.h"

// Test basic WTL functionality
class CTestWindow : public CWindowImpl<CTestWindow>
{
public:
	DECLARE_WND_CLASS(_T("WTL Test Window"))

	BEGIN_MSG_MAP(CTestWindow)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		return 0;
	}

	void OnDestroy()
	{
		PostQuitMessage(0);
	}
};

// Test function
bool TestWTLSetup()
{
	// Test that WTL headers are properly included
	CAppModule module;
	
	// Test basic window creation
	CTestWindow wnd;
	
	// Test string functionality
	CString testStr = _T("Hello WTL");
	testStr.Format(_T("Test: %s"), _T("WTL"));
	
	return true;
}