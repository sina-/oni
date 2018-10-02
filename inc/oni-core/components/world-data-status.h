#pragma once

namespace oni {
    namespace components {
        enum class WorldDataStatus {
            ONLY_COMPONENT_UPDATE = 1,
            ADD_NEW_ENTITIES = 2,
            REPLACE_ALL_ENTITIES = 3,
        };

        struct TagOnlyComponentUpdate {
            template<class Archive>
            void serialize(Archive &archive) {}
        };

        struct TagAddNewEntities {
            template<class Archive>
            void serialize(Archive &archive) {}
        };
    }
}
