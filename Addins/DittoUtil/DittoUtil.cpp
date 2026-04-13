// DittoUtil.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "DittoUtil.h"
#include "PasteImageAsHtmlImage.h"

CAppModule _Module;

extern "C" BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        _Module.Init(nullptr, hInstance);
        ::DisableThreadLibraryCalls(hInstance);
        break;
    case DLL_PROCESS_DETACH:
        CPasteImageAsHtmlImage::CleanupPastedImages();
        _Module.Term();
        break;
    default:
        break;
    }

    return TRUE;
}