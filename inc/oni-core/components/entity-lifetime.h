#pragma once

namespace oni {
    namespace components {
        struct TagNeedsComponentSync {
            template<class Archive>
            void serialize(Archive &archive) {}
        };

        struct TagNeedsEntitySync {
            template<class Archive>
            void serialize(Archive &archive) {}
        };
    }
}
