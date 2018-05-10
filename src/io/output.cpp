#include <iostream>

#include <oni-core/io/output.h>
#include <oni-core/common/typedefs.h>

namespace oni {
    namespace io {
        void printl(const std::string &text) {
            std::cout << text << std::endl;
        }

        void printl(const common::int32 text) {
            std::cout << text << std::endl;
        }

        void printl(double text) {
            std::cout << text << std::endl;
        }

        void printl(float text) {
            std::cout << text << std::endl;
        }
    }
}