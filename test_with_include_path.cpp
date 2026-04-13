// Test with correct include path
#define _WIN32_WINNT 0x0A00
#define WINVER 0x0A00
#define VC_EXTRALEAN
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1
#define HITTEST_RET LRESULT

#pragma warning(disable : 4995)

#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlframe.h>
#include <atldlgs.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atlstr.h>

#include "E:\PubCode\Ditto\UnicodeMacros.h"

int main() {
    return 0;
}