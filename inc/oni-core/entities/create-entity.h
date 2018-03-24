#pragma once

#include <oni-core/components/visual.h>
#include <oni-core/entities/world.h>
#include <oni-core/graphics/texture.h>
#include <oni-core/entities/entity.h>
#include <oni-core/graphics/font-manager.h>

namespace oni {
    namespace entities {
        entities::entityID createSpriteEntity(World &world, components::LayerID layerID,
                                              const math::vec4 &color, const math::vec3 &position,
                                              const math::vec2 &size);

        entities::entityID createTexturedEntity(World &world, components::LayerID layerID,
                                                const components::Texture &entityTexture, const math::vec3 &position,
                                                const math::vec2 &size);

        entities::entityID createDynamicTexturedEntity(World &world, components::LayerID layerID,
                                                       const components::Texture &entityTexture,
                                                       const math::vec3 &position,
                                                       const math::vec2 &size);

        entities::entityID createTextEntity(World &world, graphics::FontManager &fontManager,
                                            components::LayerID layerID, const std::string &text,
                                            const math::vec3 &position);
    }
}
