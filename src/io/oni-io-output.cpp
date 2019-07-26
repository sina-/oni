#include <iostream>

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    namespace io {
        void
        printl(const std::string &text) {
            std::cout << text << std::endl;
        }

        void
        printl(const common::i32 text) {
            std::cout << text << std::endl;
        }

        void
        printl(double text) {
            std::cout << text << std::endl;
        }

        void
        printl(common::r32 text) {
            std::cout << text << std::endl;
        }
    }
}