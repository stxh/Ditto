// FocusHighlight.cpp : WTL entry point for the application.

#include "stdafx.h"
#include "resource.h"
#include "FocusHighlightDlg.h"

CAppModule _Module;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    HRESULT hRes = ::CoInitialize(NULL);
    ATLASSERT(SUCCEEDED(hRes));

    AtlInitCommonControls(ICC_WIN95_CLASSES);

    hRes = _Module.Init(NULL, hInstance);
    if (FAILED(hRes))
    {
        ::CoUninitialize();
        return static_cast<int>(hRes);
    }

    CFocusHighlightDlg dlg;
    dlg.DoModal();

    _Module.Term();
    ::CoUninitialize();

    return 0;
}

