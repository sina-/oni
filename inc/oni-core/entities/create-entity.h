#pragma once

#include <entt/entity/registry.hpp>

#include <oni-core/components/visual.h>
#include <oni-core/common/typedefs.h>

class b2World;

namespace oni {
    namespace entities {
        class EntityManager;
    }

    namespace graphics {
        class FontManager;

        class SceneManager;
    }

    namespace entities {
        using EntityID = typename entt::DefaultRegistry::entity_type;

        EntityID createSpriteEntity(entt::DefaultRegistry &registry,
                                    const math::vec4 &color,
                                    const math::vec2 &size,
                                    const math::vec3 &positionInWorld);

        EntityID createSpriteStaticEntity(entities::EntityManager &manager,
                                          const math::vec4 &color,
                                          const math::vec2 &size,
                                          const math::vec3 &positionInWorld);

        EntityID createDynamicEntity(EntityManager &manager, const math::vec2 &size,
                                     const math::vec3 &positionInWorld,
                                     const common::real32 heading, const math::vec3 &scale);

        EntityID createDynamicPhysicsEntity(EntityManager &manager, b2World &physicsWorld,
                                            const math::vec2 &size,
                                            const math::vec3 &positionInWorld, const common::real32 heading,
                                            const math::vec3 &scale);

        EntityID createStaticEntity(EntityManager &manager, const math::vec2 &size,
                                    const math::vec3 &positionInWorld);

        EntityID createStaticPhysicsEntity(EntityManager &manager, b2World &physicsWorld,
                                           const math::vec2 &size,
                                           const math::vec3 &positionInWorld);

        EntityID createTextStaticEntity(EntityManager &manager,
                                        graphics::FontManager &fontManager,
                                        const std::string &text,
                                        const math::vec3 &position,
                                        const math::vec2 &size,
                                        const math::vec3 &positionInWorld);

        EntityID createVehicleEntity(EntityManager &manager, b2World &physicsWorld);
    }
}
