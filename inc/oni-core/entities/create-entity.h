#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/math/vec2.h>
#include <oni-core/math/vec3.h>
#include <oni-core/math/vec4.h>
#include <oni-core/component/physic.h>
#include <oni-core/common/typedefs-graphics.h>
#include <oni-core/entities/entity-manager.h>

class b2World;

namespace oni {
    namespace entities {
        class EntityManager;
    }

    namespace component {
        struct Texture;
        struct CarConfig;
    }

    namespace graphic {
        class SceneManager;
    }

    namespace entities {

        common::EntityID createEntity(EntityManager &manager, bool tagAsNew = true);

        void destroyEntity(EntityManager &manager, common::EntityID entityID);

        void
        assignShapeLocal(EntityManager &manager, common::EntityID entityID, const math::vec2 &size, common::real32);

        void assignShapeRotatedLocal(EntityManager &manager, common::EntityID entityID, const math::vec2 &size,
                                     common::real32, common::real32);

        void removeShape(EntityManager &manager, common::EntityID entityID);

        void assignShapeWorld(EntityManager &manager,
                              common::EntityID entityID,
                              const math::vec2 &size,
                              const math::vec3 &worldPos);

        void assignPoint(EntityManager &manager,
                         common::EntityID entityID,
                         const math::vec3 &pos);

        void assignPlacement(EntityManager &manager,
                             common::EntityID entityID,
                             const math::vec3 &worldPos,
                             const math::vec3 &scale,
                             common::real32 heading);

        void removePlacement(EntityManager &manager, common::EntityID entityID);

        void assignParticle(EntityManager &manager, common::EntityID entityID, const math::vec3 &worldPos, common::real32 life);

        void assignAppearance(EntityManager &manager, common::EntityID entityID, const math::vec4 &color);

        void removeAppearance(EntityManager &manager, common::EntityID entityID);

        void assignPhysicalProperties(EntityManager &manager,
                                      b2World &physicsWorld,
                                      common::EntityID,
                                      const math::vec3 &worldPos,
                                      const math::vec2 &size,
                                      common::real32 heading,
                                      component::PhysicalProperties &
        );

        void removePhysicalProperties(EntityManager &manager, b2World &physicsWorld, common::EntityID entityID);

        void assignTextureToLoad(EntityManager &manager, common::EntityID entity, const std::string &path);

        void assignTextureLoaded(EntityManager &manager,
                                 common::EntityID entity,
                                 const component::Texture &texture);

        void removeTexture(EntityManager &manager, common::EntityID entityID);

        void removeText(EntityManager &manager, common::EntityID entityID);

        void assignCar(EntityManager &manager, common::EntityID entityID, const math::vec3 &worldPos,
                       const component::CarConfig &carConfig);

        void removeCar(EntityManager &manager, common::EntityID entityID);

        template<class T>
        void assignTag(EntityManager &manager, common::EntityID entityID) {
            manager.assign<T>(entityID);
        }

        template<class T>
        void removeTag(EntityManager &manager, common::EntityID entityID) {
            manager.remove<T>(entityID);
        }

        void assignTransformationHierarchy(EntityManager &manager, common::EntityID parent, common::EntityID child);

        void removeTransformationHierarchy(EntityManager &manager, common::EntityID parent, common::EntityID child);
    }
}
