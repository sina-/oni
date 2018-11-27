#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/math/vec2.h>
#include <oni-core/math/vec3.h>
#include <oni-core/math/vec4.h>
#include <oni-core/components/physics.h>

class b2World;

namespace oni {
    namespace entities {
        class EntityManager;
    }

    namespace components {
        struct Texture;
        struct CarConfig;
    }

    namespace graphics {
        class FontManager;

        class SceneManager;
    }

    namespace entities {

        common::EntityID createEntity(EntityManager &manager);

        void assignShapeLocal(EntityManager &manager, common::EntityID entityID, const math::vec2 &size);

        void assignShapeWold(EntityManager &manager,
                             common::EntityID entityID,
                             const math::vec2 &size,
                             const math::vec3 &worldPos);

        void assignPlacement(EntityManager &manager,
                             common::EntityID entityID,
                             const math::vec3 &worldPos,
                             const math::vec3 &scale,
                             common::real32 heading);

        void assignAppearance(EntityManager &manager, common::EntityID entityID, const math::vec4 &color);

        void assignPhysicalProperties(EntityManager &manager,
                                      b2World &physicsWorld,
                                      common::EntityID,
                                      const math::vec3 &worldPos,
                                      const math::vec2 &size,
                                      components::BodyType bodyType,
                                      bool highPrecisionPhysics
        );

        void assignTagStatic(EntityManager& manager, common::EntityID entityID);

        common::EntityID createSpriteEntity(EntityManager &manager,
                                            const math::vec4 &color,
                                            const math::vec2 &size,
                                            const math::vec3 &positionInWorld);

        common::EntityID createSpriteStaticEntity(entities::EntityManager &manager,
                                                   const math::vec4 &color,
                                                   const math::vec2 &size,
                                                   const math::vec3 &positionInWorld);

        common::EntityID createDynamicEntity(EntityManager &manager, const math::vec2 &size,
                                             const math::vec3 &positionInWorld,
                                             common::real32 heading, const math::vec3 &scale);

        common::EntityID createDynamicPhysicsEntity(EntityManager &manager, b2World &physicsWorld,
                                                    const math::vec2 &size,
                                                    const math::vec3 &positionInWorld, common::real32 heading,
                                                    const math::vec3 &scale);

        common::EntityID _createStaticEntity(EntityManager &manager, const math::vec2 &size,
                                            const math::vec3 &positionInWorld);

        common::EntityID createStaticPhysicsEntity(EntityManager &manager, b2World &physicsWorld,
                                                   const math::vec2 &size,
                                                   const math::vec3 &positionInWorld);

        common::EntityID
        createTextStaticEntity(EntityManager &manager, graphics::FontManager &fontManager, const std::string &text,
                               const math::vec2 &size, const math::vec3 &positionInWorld);

        common::EntityID
        createVehicleEntity(EntityManager &manager, b2World &physicsWorld, const oni::components::CarConfig &carConfig);

        void deleteVehicleEntity(EntityManager &manager, b2World &physicsWorld, common::EntityID entityID);

        void assignTexture(EntityManager &manager, common::EntityID entity, const components::Texture &texture);
    }
}
