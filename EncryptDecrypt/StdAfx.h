#pragma once

#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers

#include <SDKDDKVer.h>

#include <windows.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlstr.h>
#include <atlcoll.h>

#ifndef ASSERT
#define ASSERT ATLASSERT
#endif
