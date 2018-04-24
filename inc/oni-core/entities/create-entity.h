#pragma once

#include <oni-core/components/visual.h>
#include <oni-core/entities/entity.h>
#include <oni-core/common/typedefs.h>

namespace oni {
    namespace graphics {
        class FontManager;

        class SceneManager;
    }

    namespace entities {
        class BasicEntityRepo;

        class VehicleEntityRepo;

        common::entityID createSpriteEntity(BasicEntityRepo &basicEntityRepo, components::ShaderID shaderID,
                                            const math::vec4 &color, const math::vec3 &position,
                                            const math::vec2 &size);

        common::entityID createTexturedEntity(BasicEntityRepo &basicEntityRepo, components::ShaderID shaderID,
                                              const components::Texture &entityTexture, const math::vec3 &position,
                                              const math::vec2 &size);

        common::entityID createTexturedEntityStatic(BasicEntityRepo &basicEntityRepo,
                                                    graphics::SceneManager &sceneManager,
                                                    const components::Texture &entityTexture,
                                                    const math::vec3 &position,
                                                    const math::vec2 &size,
                                                    const math::vec3 &positionInWorld,
                                                    std::string &&vertShader,
                                                    std::string &&fragShader);

        common::entityID createTextEntity(BasicEntityRepo &basicEntityRepo, graphics::FontManager &fontManager,
                                          components::ShaderID shaderID, const std::string &text,
                                          const math::vec3 &position);

        common::entityID createTextEntityStatic(BasicEntityRepo &basicEntityRepo,
                                                graphics::SceneManager &sceneManager,
                                                graphics::FontManager &fontManager,
                                                const std::string &text,
                                                const math::vec3 &position,
                                                const math::vec2 &size,
                                                const math::vec3 &positionInWorld,
                                                std::string &&vertShader,
                                                std::string &&fragShader);

        common::entityID createVehicleEntity(VehicleEntityRepo &vehicleEntityRepo, components::ShaderID shaderID,
                                             const components::Texture &entityTexture);
    }
}
