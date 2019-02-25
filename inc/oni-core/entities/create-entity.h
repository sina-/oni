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

    namespace component {
        struct Texture;
        struct CarConfig;
    }

    namespace math {
        class ZLayerManager;
    }

    namespace entities {

        class EntityFactory {
        public:
            EntityFactory(EntityManager &, const math::ZLayerManager &, b2World &);

            template<class... Args>
            common::EntityID createEntity(component::EntityType entityType, Args &&... args) {
                common::EntityID entityID = createEntity(true);
                auto &type = createComponent<component::EntityType>(entityID);
                type = entityType;

                switch (entityType) {
                    case component::EntityType::RACE_CAR: {
                        createCar(entityID, std::forward<Args>(args)...);
                        break;
                    }
                    case component::EntityType::VEHICLE_GUN: {
                        createGun(entityID, std::forward<Args>(args)...);
                        break;
                    }
                    case component::EntityType::VEHICLE_TIRE: {
                        createTire(entityID, std::forward<Args>(args)...);
                        break;
                    }
                    case component::EntityType::WALL: {
                        createWall(entityID, std::forward<Args>(args)...);
                        break;
                    }
                    case component::EntityType::UNKNOWN: {
                        mManager.remove<component::EntityType>(entityID);
                        mManager.destroy(entityID);
                        assert(false);
                        break;
                    }
                    default: {
                        mManager.remove<component::EntityType>(entityID);
                        mManager.destroy(entityID);
                        assert(false);
                        break;
                    }
                }

                return entityID;
            }

            void removeEntity(common::EntityID, component::EntityType);

            std::unique_lock<std::mutex> scopedLock();

        private:
            common::EntityID createEntity(bool tagNew);

            template<class ...Args>
            void createCar(common::EntityID, Args &&... args) {
                assert(false);
            }

            template<class ...Args>
            void createGun(common::EntityID, Args &&... args) {
                assert(false);
            }

            template<class ...Args>
            void createTire(common::EntityID, Args &&... args) {
                assert(false);
            }

            template<class ...Args>
            void createWall(common::EntityID, Args &&... args) {
                assert(false);
            }

        private:
            void removeRaceCar(common::EntityID);

            void removeTire(common::EntityID);

            void removeGun(common::EntityID);

            void removeWall(common::EntityID);

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
