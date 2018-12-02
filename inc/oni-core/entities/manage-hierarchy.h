#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace entities {
        class EntityManager;
        void createTransformationHierarchy(EntityManager &manager,
                                           common::EntityID parent,
                                           common::EntityID child);

        void removeTransformationHierarchy(EntityManager &manager, common::EntityID parent, common::EntityID child);
    };
}
