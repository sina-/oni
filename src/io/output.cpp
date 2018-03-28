#include <iostream>

#include <oni-core/io/output.h>

namespace oni {
    namespace io {
        void printl(const std::string &text) {
            std::cout << text << std::endl;
        }

        void printl(int text) {
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