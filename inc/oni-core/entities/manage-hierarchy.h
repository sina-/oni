#pragma once

#include <oni-core/entities/create-entity.h>

namespace oni {
    namespace entities {
        class TransformationHierarchy {
            TransformationHierarchy() = default;

            ~TransformationHierarchy() = default;

        public:
            static void createTransformationHierarchy(entt::DefaultRegistry &registry,
                                                      const entities::EntityID parent,
                                                      const entities::EntityID child);
        };
    }
}
