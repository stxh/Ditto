// Minimal test to isolate the problem
#define _WIN32_WINNT 0x0A00
#define WINVER 0x0A00
#define VC_EXTRALEAN

#include <atlbase.h>

// Try including WTL headers one by one
#include "E:\PubCode\Ditto\wtl\include\atlapp.h"
#include "E:\PubCode\Ditto\wtl\include\atlwin.h"

int main() {
    return 0;
}