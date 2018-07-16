#pragma once

#include <entt/entity/registry.hpp>

#include <oni-core/components/visual.h>
#include <oni-core/common/typedefs.h>

class b2World;

namespace oni {
    namespace graphics {
        class FontManager;

        class SceneManager;
    }

    namespace entities {
        using entityID = typename entt::DefaultRegistry::entity_type;

        // TODO: These functions are good candidates for templating.
        entityID createSpriteEntity(entt::DefaultRegistry &registry,
                                    const math::vec4 &color,
                                    const math::vec2 &size,
                                    const math::vec3 &positionInWorld);

        entityID createSpriteStaticEntity(entt::DefaultRegistry &registry,
                                          const math::vec4 &color,
                                          const math::vec2 &size,
                                          const math::vec3 &positionInWorld);

        entityID createDynamicEntity(entt::DefaultRegistry &registry, const math::vec2 &size,
                                     const math::vec3 &positionInWorld,
                                     const common::real32 heading, const math::vec3 &scale);

        entityID createDynamicPhysicsEntity(entt::DefaultRegistry &registry, b2World &physicsWorld,
                                            const math::vec2 &size,
                                            const math::vec3 &positionInWorld, const common::real32 heading,
                                            const math::vec3 &scale);

        entityID createStaticEntity(entt::DefaultRegistry &registry, const math::vec2 &size,
                                    const math::vec3 &positionInWorld);

        entityID createStaticPhysicsEntity(entt::DefaultRegistry &registry, b2World &physicsWorld,
                                           const math::vec2 &size,
                                           const math::vec3 &positionInWorld);

        entityID createTextEntity(entt::DefaultRegistry &registry,
                                  graphics::FontManager &fontManager,
                                  const std::string &text,
                                  const math::vec3 &position);

        entityID createTextStaticEntity(entt::DefaultRegistry &registry,
                                        graphics::FontManager &fontManager,
                                        const std::string &text,
                                        const math::vec3 &position,
                                        const math::vec2 &size,
                                        const math::vec3 &positionInWorld);

        entityID createVehicleEntity(entt::DefaultRegistry &registry, b2World &physicsWorld);

        void assignTexture(entt::DefaultRegistry &registry,
                           entityID entity,
                           const components::Texture &texture);
    }
}
