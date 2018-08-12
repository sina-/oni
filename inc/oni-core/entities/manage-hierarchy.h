#pragma once

#include <oni-core/entities/create-entity.h>

namespace oni {
    namespace entities {
        class EntityManager;

        class TransformationHierarchy {
            TransformationHierarchy() = default;

            ~TransformationHierarchy() = default;

        public:
            static void createTransformationHierarchy(EntityManager &manager,
                                                      const entities::EntityID parent,
                                                      const entities::EntityID child);
        };
    }
}
