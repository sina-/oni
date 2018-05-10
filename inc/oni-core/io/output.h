#pragma once

#include <string>

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace io {
        void printl(const std::string &text);

        void printl(int text);

        void printl(double text);

        void printl(common::real32 text);
    }
}
