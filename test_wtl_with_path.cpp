// Test for WTL compilation issues with correct include path
#define _WIN32_WINNT 0x0A00
#define WINVER 0x0A00
#define VC_EXTRALEAN
#define HITTEST_RET LRESULT

// Test basic C++ keywords first
class TestClass {
public:
    int value;
};

// Now try to include WTL headers with correct path
#include <atlbase.h>
#include "E:\PubCode\Ditto\wtl\include\atlapp.h"

int main() {
    TestClass test;
    test.value = 42;
    return 0;
}