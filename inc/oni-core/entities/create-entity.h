#pragma once

#include <entt/entt.hpp>

#include <oni-core/components/visual.h>
#include <oni-core/common/typedefs.h>

namespace oni {
    namespace graphics {
        class FontManager;

        class SceneManager;
    }

    namespace entities {
        using entityID = typename entt::DefaultRegistry::entity_type;

        // TODO: These functions are good candidates for templating.
        entityID createSpriteEntity(entt::DefaultRegistry &registry, graphics::SceneManager &sceneManager,
                                    const math::vec4 &color,
                                    const math::vec2 &size, const math::vec3 &positionInWorld);

        entityID createSpriteStaticEntity(entt::DefaultRegistry &registry,
                                          graphics::SceneManager &sceneManager,
                                          const math::vec4 &color, const math::vec2 &size,
                                          const math::vec3 &positionInWorld);

        entityID createTexturedDynamicEntity(entt::DefaultRegistry &registry,
                                             graphics::SceneManager &sceneManager,
                                             const components::Texture &entityTexture, const math::vec2 &size,
                                             const math::vec3 &positionInWorld);

        entityID createTexturedStaticEntity(entt::DefaultRegistry &registry,
                                            graphics::SceneManager &sceneManager,
                                            const components::Texture &entityTexture, const math::vec2 &size,
                                            const math::vec3 &positionInWorld);

        entityID createTextEntity(entt::DefaultRegistry &registry, graphics::FontManager &fontManager,
                                  components::ShaderID shaderID, const std::string &text,
                                  const math::vec3 &position);

        entityID createTextStaticEntity(entt::DefaultRegistry &registry,
                                        graphics::SceneManager &sceneManager,
                                        graphics::FontManager &fontManager,
                                        const std::string &text,
                                        const math::vec3 &position,
                                        const math::vec2 &size,
                                        const math::vec3 &positionInWorld,
                                        std::string &&vertShader,
                                        std::string &&fragShader);

        entityID createVehicleEntity(entt::DefaultRegistry &registry, graphics::SceneManager &sceneManager,
                                     const components::Texture &entityTexture);
    }
}
