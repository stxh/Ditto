// Test for WTL compilation issues
#define _WIN32_WINNT 0x0A00
#define WINVER 0x0A00
#define VC_EXTRALEAN
#define HITTEST_RET LRESULT

// Test basic C++ keywords first
class TestClass {
public:
    int value;
};

// Now try to include WTL headers
#include <atlbase.h>
#include <atlapp.h>

int main() {
    TestClass test;
    test.value = 42;
    return 0;
}