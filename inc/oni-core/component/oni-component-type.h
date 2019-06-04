#pragma once

#include <vector>

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    namespace component {
        enum class ComponentType : common::u16 {
            UNKNOWN,
            SMOKE_EMITTER_CD,

            LAST
        };

        // TODO: Another approach to this struct is to have a way to mark any component as needing to have a client
        // side complement, in that case, sever can create that component just like any other and the client upon
        // receiving the entities also creates a client mirror of it where client systems are allowed to modify them
        // instead of server registry components. I have to use this solution for a while and see how it works, the
        // other approach doesn't necessarily make things better, since this list is explicit and easy to work with.
        struct ComplementaryComponents {
            std::vector<ComponentType> types{};
        };
    }
}
