#include <entt/entt.hpp>

#include <oni-core/entities/manage-hierarchy.h>
#include <oni-core/components/hierarchy.h>
#include <oni-core/utils/oni-assert.h>
#include <oni-core/math/mat4.h>


namespace oni {
    namespace entities {

        void TransformationHierarchy::createTransformationHierarchy(entt::DefaultRegistry &registry,
                                                                    const entities::EntityID parent,
                                                                    const entities::EntityID child) {
            if (registry.has<components::TransformChildren>(parent)) {
                auto transformChildren = registry.get<components::TransformChildren>(parent);
                transformChildren.children.emplace_back(child);
                registry.replace<components::TransformChildren>(parent, transformChildren);
            } else {
                auto transformChildren = components::TransformChildren{};
                transformChildren.children.emplace_back(child);
                registry.assign<components::TransformChildren>(parent, transformChildren);
            }

            ONI_DEBUG_ASSERT(!registry.has<components::TransformParent>(child))

            auto transformParent = components::TransformParent{parent, math::mat4::identity()};
            registry.assign<components::TransformParent>(child, transformParent);
        }
    }
}