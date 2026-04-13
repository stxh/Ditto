// Test for macro conflicts
#define _WIN32_WINNT 0x0A00
#define WINVER 0x0A00
#define VC_EXTRALEAN
#define HITTEST_RET LRESULT

#include <iostream>

int main() {
    // Test if 'class' keyword works
    class TestClass {
    public:
        TestClass() {}
    };
    
    std::cout << "Class keyword works" << std::endl;
    return 0;
}