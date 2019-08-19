#pragma once

#include <string>

#include <oni-core/common/oni-common-typedef.h>


namespace oni {
    void
    printl(const std::string &text);

    void
    printl(int text);

    void
    printl(r64 text);

    void
    printl(r32 text);
}