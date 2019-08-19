#pragma once

#include <string>

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    struct Address {
        std::string host;
        u16 port;
    };

    typedef std::string PeerID;
}