#pragma once

#include <mutex>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/common/oni-common-typedefs-graphic.h>
#include <oni-core/component/oni-component-physic.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/entities/oni-entities-entity.h>
#include <oni-core/entities/oni-entities-manager.h>

class b2World;

class b2Body;

namespace oni {
    namespace math {
        class Rand;

        struct vec2;
        struct vec3;
        struct vec4;
    }

    namespace math {
        class ZLayerManager;
    }

    namespace component {
        struct Texture;
        union WorldP3D;
    }

    namespace entities {
        class EntityFactory {
        public:
            // TODO: Does it make sense to require every user of this class to provide all these systems? Not every user
            // might be interested in creating entities that requires those systems. I should think about if it makes
            // sense to break up this factory into factories for special purposes, maybe one per system or combination
            // of them?
            EntityFactory(entities::SimMode sMode,
                          const math::ZLayerManager &,
                          b2World &);

            // TODO: Ideally I would have one interface into Entity world where I can request entities to be created,
            // destroyed, or take a look at its components. EntityFactory uses EntityManager to store the data. Maybe
            // makes sense to merge functionalities provided by EntityManager into EntityFactory?
            EntityManager &
            getEntityManager();

            EntityManager &
            getEntityManager() const;

            void
            tagForRemoval(common::EntityID);

            void
            tagForRemoval(common::EntityID,
                          const entities::EntityOperationPolicy &);

            void
            flushEntityRemovals();

            void
            attach(common::EntityID parent,
                   common::EntityID child,
                   entities::EntityType parentType,
                   entities::EntityType childType);

            // TODO: Does it make sense to create a deleted entity event instead of keeping track of these entities?
            void
            clearDeletedEntitiesList() {
                mRegistryManager->clearDeletedEntitiesList();
            }

        public:
            common::EntityID
            createEntity_SmokeCloud();

            common::EntityID
            createEntity_SimpleSpriteColored();

            common::EntityID
            createEntity_SimpleSpriteTextured();

            common::EntityID
            createEntity_RaceCar();

            common::EntityID
            createEntity_VehicleGun();

            common::EntityID
            createEntity_Vehicle();

            common::EntityID
            createEntity_SimpleRocket();

            common::EntityID
            createEntity_Wall();

            common::EntityID
            createEntity_VehicleTireFront();

            common::EntityID
            createEntity_VehicleTireRear();

            common::EntityID
            createEntity_SimpleParticle();

            common::EntityID
            createEntity_SimpleBlastParticle();

            common::EntityID
            createEntity_Text();

            common::EntityID
            createEntity_WorldChunk();

            common::EntityID
            createEntity_DebugWorldChunk();

        public:
            void
            setWorldP3D(common::EntityID,
                        common::r32 x,
                        common::r32 y,
                        common::r32 z);

            void
            setTexture(common::EntityID,
                       std::string_view path);

            void
            setScale(common::EntityID,
                     common::r32 x,
                     common::r32 y);

            void
            setApperance(common::EntityID,
                         common::r32 red,
                         common::r32 green,
                         common::r32 blue,
                         common::r32 alpha);

            void
            setRandAge(common::EntityID,
                       common::r32 lower,
                       common::r32 upper);

            void
            setRandHeading(common::EntityID);

            void
            setHeading(common::EntityID,
                       common::r32 heading);

            void
            setRandVelocity(common::EntityID,
                            common::i32 lower,
                            common::i32 upper);

            void
            createPhysics(
                    common::EntityID,
                    const component::WorldP3D &worldPos,
                    const math::vec2 &size,
                    const common::r32 heading);

            void
            setText(common::EntityID,
                    std::string_view content);

        private:
            common::EntityID
            createEntity(entities::EntityType);

            void
            assignSimMode(common::EntityID,
                          entities::SimMode);

            void
            removeEntity(common::EntityID);

            void
            removeEntity(common::EntityID,
                         const entities::EntityOperationPolicy &);

            void
            _removeEntity(common::EntityID,
                          entities::EntityType entityType,
                          const entities::EntityOperationPolicy &policy);

            template<entities::EntityType entityType>
            void
            _removeEntity(common::EntityID,
                          const entities::EntityOperationPolicy &policy) = delete;

        private:
            template<>
            void
            _removeEntity<entities::EntityType::RACE_CAR>(common::EntityID,
                                                          const entities::EntityOperationPolicy &policy
            );

            template<>
            void
            _removeEntity<entities::EntityType::WALL>(common::EntityID,
                                                      const entities::EntityOperationPolicy &policy
            );

            template<>
            void
            _removeEntity<entities::EntityType::SIMPLE_ROCKET>(common::EntityID,
                                                               const entities::EntityOperationPolicy &policy
            );

            void
            removePhysicalBody(common::EntityID);

        private:
            template<class T>
            void
            assignTag(common::EntityID entityID) {
                mRegistryManager->assign<T>(entityID);
            }

            template<class T>
            void
            removeTag(common::EntityID entityID) {
                mRegistryManager->remove<T>(entityID);
            }

            template<class Component, class... Args>
            Component &
            createComponent(common::EntityID entityID,
                            Args &&...args) {
                static_assert(std::is_aggregate_v<Component> || std::is_enum_v<Component>);
                return mRegistryManager->assign<Component>(entityID, std::forward<Args>(args)...);
            }

            template<class Component>
            void
            removeComponent(common::EntityID entityID) {
                mRegistryManager->remove<Component>(entityID);
            }

            template<class Component>
            void
            removeComponentSafe(common::EntityID entityID) {
                if (mRegistryManager->has<Component>(entityID)) {
                    mRegistryManager->remove<Component>(entityID);
                }
            }

            void
            destroyEntity(common::EntityID entityID);

        private:
            std::unique_ptr<EntityManager> mRegistryManager;
            b2World &mPhysicsWorld;
            const math::ZLayerManager &mZLayerManager;
            std::unique_ptr<math::Rand> mRand{};
            entities::SimMode mSimMode{entities::SimMode::CLIENT};
            entities::EntityOperationPolicy mEntityOperationPolicy{};

            std::vector<common::EntityID> mEntitiesToDelete{};
            std::vector<entities::EntityOperationPolicy> mEntitiesToDeletePolicy{};
        };
    }
}
