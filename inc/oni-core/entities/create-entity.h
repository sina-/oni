#pragma once

#include <oni-core/components/visual.h>
#include <oni-core/entities/entity.h>

namespace oni {
    namespace graphics {
        class FontManager;
    }

    namespace entities {
        class BasicEntityRepo;

        class VehicleEntityRepo;

        entities::entityID createSpriteEntity(BasicEntityRepo &basicEntityRepo, components::LayerID layerID,
                                              const math::vec4 &color, const math::vec3 &position,
                                              const math::vec2 &size);

        entities::entityID createTexturedEntity(BasicEntityRepo &basicEntityRepo, components::LayerID layerID,
                                                const components::Texture &entityTexture, const math::vec3 &position,
                                                const math::vec2 &size);

        entities::entityID createTextEntity(BasicEntityRepo &basicEntityRepo, graphics::FontManager &fontManager,
                                            components::LayerID layerID, const std::string &text,
                                            const math::vec3 &position);

        entities::entityID createVehicleEntity(VehicleEntityRepo &vehicleEntityRepo, components::LayerID layerID,
                                               const components::Texture &entityTexture);
    }
}
