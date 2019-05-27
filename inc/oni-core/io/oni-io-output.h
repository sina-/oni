#pragma once

#include <string>

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    namespace io {
        void
        printl(const std::string &text);

        void
        printl(int text);

        void
        printl(oni::common::r64 text);

        void
        printl(common::r32 text);
    }
}