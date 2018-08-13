#include <entt/entt.hpp>

#include <oni-core/entities/manage-hierarchy.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/components/hierarchy.h>


namespace oni {
    namespace entities {

        void TransformationHierarchy::createTransformationHierarchy(EntityManager &manager,
                                                                    const common::EntityID parent,
                                                                    const common::EntityID child) {
            if (manager.has<components::TransformChildren>(parent)) {
                auto transformChildren = manager.get<components::TransformChildren>(parent);
                transformChildren.children.emplace_back(child);
                manager.replace<components::TransformChildren>(parent, transformChildren);
            } else {
                auto transformChildren = components::TransformChildren{};
                transformChildren.children.emplace_back(child);
                manager.assign<components::TransformChildren>(parent, transformChildren);
            }

            assert(!manager.has<components::TransformParent>(child));

            auto transformParent = components::TransformParent{parent, math::mat4::identity()};
            manager.assign<components::TransformParent>(child, transformParent);
        }
    }
}