// Test to check available ATL types
#define _WIN32_WINNT 0x0A00
#define WINVER 0x0A00
#define VC_EXTRALEAN

#include <atlbase.h>
#include <windows.h>

// Test if ATL types are available
int main() {
    // Try to use ATL types
    ATL::CComModule module;
    return 0;
}