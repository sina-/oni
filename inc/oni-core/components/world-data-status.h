#pragma once

namespace oni {
    namespace components {
        enum class WorldDataStatus {
            ONLY_COMPONENT_UPDATE = 1,
            ADD_NEW_ENTITIES = 2,
            REPLACE_ALL_ENTITIES = 3,
        };
    }
}
