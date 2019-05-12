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
        printl(oni::common::real64 text);

        void
        printl(common::real32 text);
    }
}
