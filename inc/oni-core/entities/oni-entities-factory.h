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
            EntityFactory(const math::ZLayerManager &,
                          b2World &);

            // TODO: Ideally I would have one interface into Entity world where I can request entities to be created,
            // destroyed, or take a look at its components. EntityFactory uses EntityManager to store the data. Maybe
            // makes sense to merge functionalities provided by EntityManager into EntityFactory?
            EntityManager &
            getEntityManager();

            template<entities::EntityType entityType, class ...Args>
            common::EntityID
            createEntity(SimMode entityNetworkMode,
                         const Args &... args) {
                common::EntityID entityID = createEntity();
                createComponent<entities::EntityType>(entityID, entityType);
                switch (entityNetworkMode) {
                    case SimMode::CLIENT: {
                        createComponent<component::Tag_SimModeClient>(entityID);
                        break;
                    }
                    case SimMode::SERVER: {
                        createComponent<component::Tag_SimModeServer>(entityID);
                        break;
                    }
                    default: {
                        assert(false);
                    }
                }
                _createEntity<entityType>(entityID, args...);
                return entityID;
            }

            void
            removeEntity(common::EntityID,
                         const entities::EntityOperationPolicy &);

            void
            attach(common::EntityID parent,
                   common::EntityID child,
                   entities::EntityType parentType,
                   entities::EntityType childType);

            void
            tagForNetworkSync(common::EntityID);

            // TODO: Does it make sense to create a deleted entity event instead of keeping track of these entities?
            void
            clearDeletedEntitiesList() {
                mRegistryManager->clearDeletedEntitiesList();
            }

        private:
            common::EntityID
            createEntity();

            template<entities::EntityType, class ...Args>
            void
            _createEntity(common::EntityID,
                          const Args &...) = delete;

            void
            _removeEntity(common::EntityID,
                          entities::EntityType entityType,
                          const entities::EntityOperationPolicy &policy);

            template<entities::EntityType entityType>
            void
            _removeEntity(common::EntityID,
                          const entities::EntityOperationPolicy &policy) = delete;

            template<>
            void
            _createEntity<entities::EntityType::RACE_CAR>(common::EntityID entityID,
                                                          const component::WorldP3D &pos,
                                                          const math::vec2 &size,
                                                          const component::Heading &heading,
                                                          const std::string &textureID);

            template<>
            void
            _createEntity<entities::EntityType::VEHICLE>(common::EntityID entityID,
                                                         const component::WorldP3D &pos,
                                                         const math::vec2 &size,
                                                         const component::Heading &heading,
                                                         const std::string &textureID);

            template<>
            void
            _createEntity<entities::EntityType::VEHICLE_GUN>(common::EntityID,
                                                             const component::WorldP3D &pos,
                                                             const math::vec2 &size,
                                                             const component::Heading &heading,
                                                             const std::string &textureID);

            template<>
            void
            _createEntity<entities::EntityType::VEHICLE_TIRE_FRONT>(common::EntityID,
                                                                    const component::WorldP3D &pos,
                                                                    const math::vec2 &size,
                                                                    const component::Heading &heading,
                                                                    const std::string &textureID);

            template<>
            void
            _createEntity<entities::EntityType::VEHICLE_TIRE_REAR>(common::EntityID,
                                                                   const component::WorldP3D &pos,
                                                                   const math::vec2 &size,
                                                                   const component::Heading &heading,
                                                                   const std::string &textureID);

            template<>
            void
            _createEntity<entities::EntityType::WALL>(common::EntityID,
                                                      const component::WorldP3D &pos,
                                                      const math::vec2 &size,
                                                      const component::Heading &heading,
                                                      const std::string &textureID);

            template<>
            void
            _createEntity<entities::EntityType::SIMPLE_SPRITE>(common::EntityID,
                                                               const component::WorldP3D &worldPos,
                                                               const math::vec2 &size,
                                                               const component::Heading &heading,
                                                               const math::vec4 &color);

            template<>
            void
            _createEntity<entities::EntityType::SIMPLE_SPRITE>(common::EntityID,
                                                               const component::WorldP3D &worldPos,
                                                               const math::vec2 &size,
                                                               const component::Heading &heading,
                                                               const std::string &textureID);

            template<>
            void
            _createEntity<entities::EntityType::SIMPLE_PARTICLE>(common::EntityID,
                                                                 const component::WorldP3D &worldPos,
                                                                 const math::vec4 &color,
                                                                 const common::r32 &halfSize,
                    // TODO: Is there a way to avoid need for const &?
                                                                 const bool &randomize);

            template<>
            void
            _createEntity<entities::EntityType::SIMPLE_PARTICLE>(common::EntityID,
                                                                 const component::WorldP3D &worldPos,
                                                                 const std::string &textureID,
                                                                 const common::r32 &halfSize,
                                                                 const bool &randomize);

            template<>
            void
            _createEntity<entities::EntityType::SIMPLE_BLAST_PARTICLE>(common::EntityID,
                                                                       const component::WorldP3D &worldPos,
                                                                       const std::string &textureID,
                                                                       const common::r32 &halfSize,
                                                                       const bool &randomize);

            template<>
            void
            _createEntity<entities::EntityType::SIMPLE_ROCKET>(common::EntityID,
                                                               const component::WorldP3D &pos,
                                                               const math::vec2 &size,
                                                               const component::Heading &heading,
                                                               const std::string &textureID,
                                                               const common::r32 &velocity);

            template<>
            void
            _createEntity<entities::EntityType::TEXT>(common::EntityID,
                                                      const component::WorldP3D &pos,
                                                      const std::string &text);

            template<>
            void
            _createEntity<entities::EntityType::WORLD_CHUNK>(common::EntityID,
                                                             const component::WorldP3D &worldPos,
                                                             const math::vec2 &size,
                                                             const component::Heading &heading,
                                                             const std::string &textureID);

            template<>
            void
            _createEntity<entities::EntityType::WORLD_CHUNK>(common::EntityID,
                                                             const component::WorldP3D &worldPos,
                                                             const math::vec2 &size,
                                                             const component::Heading &heading,
                                                             const math::vec4 &color);

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

        private:
            b2Body *
            createPhysicalBody(
                    const component::WorldP3D &worldPos,
                    const math::vec2 &size,
                    const common::r32 heading,
                    component::PhysicalProperties &properties
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
        };
    }
}
