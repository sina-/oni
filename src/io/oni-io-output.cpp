#include <iostream>

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    void
    printl(const std::string &text) {
        std::cout << text << std::endl;
    }

    void
    printl(const i32 text) {
        std::cout << text << std::endl;
    }

    void
    printl(double text) {
        std::cout << text << std::endl;
    }

    void
    printl(r32 text) {
        std::cout << text << std::endl;
    }
}