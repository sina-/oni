#pragma once

namespace oni {
    namespace components {
        enum class LifeTime {
            NEEDS_COMPONENT_SYNC = 1,
            NEEDS_ENTITY_SYNC = 2,
            NEEDS_FULL_SYNC = 3,
        };

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
