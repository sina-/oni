#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/math/vec2.h>
#include <oni-core/math/vec3.h>
#include <oni-core/math/vec4.h>
#include <oni-core/component/physic.h>
#include <oni-core/common/typedefs-graphics.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/math/z-layer-manager.h>

class b2World;

class b2Body;

namespace oni {
    namespace entities {
        class EntityManager;
    }

    namespace math {
        class ZLayerManager;
    }

    namespace entities {

        class EntityFactory {
        public:
            // TODO: Does it make sense to require every user of this class to provide all these systems? Not every user
            // might be interested in creating entities that requires those systems. I should think about if it makes
            // sense to break up this factory into factories for special purposes, maybe one per system or combination
            // of them?
            EntityFactory(EntityManager &, const math::ZLayerManager &, b2World &);

            template<component::EntityType entityType, class ...Args>
            common::EntityID createEntity(const Args &... args) {
                common::EntityID entityID = createEntity();
                auto &type = createComponent<component::EntityType>(entityID);
                type = entityType;
                _createEntity<entityType>(entityID, args...);
                return entityID;

            }

            template<component::EntityType entityType>
            void removeEntity(common::EntityID entityID) {
                _removeEntity<entityType>(entityID);
                removeComponent<component::EntityType>(entityID);
                destroyEntity(entityID);
            }

            // TODO: Remove this once this locking non-sense is over
            std::unique_lock<std::mutex> scopedLock();

        private:
            common::EntityID createEntity();

            template<component::EntityType, class ...Args>
            void _createEntity(common::EntityID, const Args &...) = delete;

            void _removeEntity(common::EntityID, component::EntityType entityType);

            template<component::EntityType entityType>
            void _removeEntity(common::EntityID) = delete;

            template<>
            void _createEntity<component::EntityType::RACE_CAR>(common::EntityID entityID,
                                                                const math::vec3 &pos,
                                                                const math::vec2 &size,
                                                                const common::real32 &heading,
                                                                const std::string &textureID);

            template<>
            void _createEntity<component::EntityType::VEHICLE>(common::EntityID entityID,
                                                               const math::vec3 &pos,
                                                               const math::vec2 &size,
                                                               const common::real32 &heading,
                                                               const std::string &textureID);

            template<>
            void _createEntity<component::EntityType::VEHICLE_GUN>(common::EntityID,
                                                                   const math::vec3 &pos,
                                                                   const math::vec2 &size,
                                                                   const common::real32 &heading,
                                                                   const std::string &textureID);

            template<>
            void _createEntity<component::EntityType::VEHICLE_TIRE>(common::EntityID,
                                                                    const math::vec3 &pos,
                                                                    const math::vec2 &size,
                                                                    const common::real32 &heading,
                                                                    const std::string &textureID);

            template<>
            void _createEntity<component::EntityType::WALL>(common::EntityID,
                                                            const math::vec3 &pos,
                                                            const math::vec2 &size,
                                                            const common::real32 &heading,
                                                            const std::string &textureID);

            template<>
            void _createEntity<component::EntityType::SIMPLE_SPRITE>(common::EntityID,
                                                                     const math::vec3 &worldPos,
                                                                     const math::vec2 &size,
                                                                     const common::real32 &heading,
                                                                     const math::vec4 &color);

            template<>
            void _createEntity<component::EntityType::SIMPLE_SPRITE>(common::EntityID,
                                                                     const math::vec3 &worldPos,
                                                                     const math::vec2 &size,
                                                                     const common::real32 &heading,
                                                                     const std::string &textureID);

            template<>
            void _createEntity<component::EntityType::SIMPLE_PARTICLE>(common::EntityID,
                                                                       const math::vec3 &worldPos);

            template<>
            void _createEntity<component::EntityType::SIMPLE_BULLET>(common::EntityID,
                                                                     const math::vec3 &pos,
                                                                     const math::vec2 &size,
                                                                     const common::real32 &heading,
                                                                     const std::string &textureID);

            template<>
            void _createEntity<component::EntityType::TEXT>(common::EntityID,
                                                            const math::vec3 &pos,
                                                            const std::string &text);


        private:
            template<>
            void _removeEntity<component::EntityType::RACE_CAR>(common::EntityID);

            template<>
            void _removeEntity<component::EntityType::VEHICLE_GUN>(common::EntityID);

            template<>
            void _removeEntity<component::EntityType::VEHICLE_TIRE>(common::EntityID);

            template<>
            void _removeEntity<component::EntityType::WALL>(common::EntityID);

            template<>
            void _removeEntity<component::EntityType::SIMPLE_BULLET>(common::EntityID);

            template<>
            void _removeEntity<component::EntityType::TEXT>(common::EntityID);

        private:
            b2Body *createPhysicalBody(
                    const math::vec3 &worldPos,
                    const math::vec2 &size,
                    const common::real32 heading,
                    component::PhysicalProperties &properties
            );

            void removePhysicalBody(common::EntityID);

        private:
            template<class T>
            void assignTag(common::EntityID entityID) {
                mManager.assign<T>(entityID);
            }

            template<class T>
            void removeTag(common::EntityID entityID) {
                mManager.remove<T>(entityID);
            }

            template<class Component>
            Component &createComponent(common::EntityID entityID) {
                return mManager.assign<Component>(entityID, Component{});
            }

            template<class Component>
            void removeComponent(common::EntityID entityID) {
                mManager.remove<Component>(entityID);
            }

            void destroyEntity(common::EntityID entityID);

        private:
            EntityManager &mManager;
            b2World &mPhysicsWorld;
            const math::ZLayerManager &mZLayerManager;
        };

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

        void assignParticle(EntityManager &manager, common::EntityID entityID, const math::vec3 &worldPos,
                            common::real32 heading, common::real32 age, common::real32 maxAge,
                            common::real32 velocity);

        void removeParticle(EntityManager &manager, common::EntityID entityID);

        void assignAppearance(EntityManager &manager, common::EntityID entityID, const math::vec4 &color);

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

        template<class T>
        void assignTag(EntityManager &manager, common::EntityID entityID) {
            manager.assign<T>(entityID);
        }

        template<class T>
        void removeTag(EntityManager &manager, common::EntityID entityID) {
            manager.remove<T>(entityID);
        }

        void attach(
                EntityManager &manager,
                common::EntityID parent,
                common::EntityID child,
                component::EntityType parentType,
                component::EntityType childType);

    }
}
