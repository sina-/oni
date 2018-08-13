#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace entities {
        class EntityManager;

        class TransformationHierarchy {
            TransformationHierarchy() = default;

            ~TransformationHierarchy() = default;

        public:
            static void createTransformationHierarchy(EntityManager &manager,
                                                      common::EntityID parent,
                                                      common::EntityID child);
        };
    }
}
