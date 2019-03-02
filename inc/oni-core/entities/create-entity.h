#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/component/physic.h>
#include <oni-core/common/typedefs-graphics.h>
#include <oni-core/entities/entity-manager.h>

class b2World;

class b2Body;

namespace oni {
    namespace math {
        class Rand;

        struct vec2;
        struct vec3;
        struct vec4;
    }
    namespace entities {
        class EntityManager;
    }

    namespace math {
        class ZLayerManager;
    }

    namespace component {
        class Texture;
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

            template<component::EntityType, class ...Args>
            void _createEntity(common::EntityID, const Args &...) = delete;

            template<component::EntityType entityType>
            void _removeEntity(common::EntityID) = delete;

        private:
            common::EntityID createEntity();

            void _removeEntity(common::EntityID, component::EntityType entityType);

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
                                                                       const math::vec3 &worldPos,
                    // TODO: Is there a way to avoid need for const &?
                                                                       const bool &randomize);

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
            void _removeEntity<component::EntityType::SIMPLE_PARTICLE>(common::EntityID);

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
            std::unique_ptr<math::Rand> mRand{};
        };

        void attach(
                EntityManager &manager,
                common::EntityID parent,
                common::EntityID child,
                component::EntityType parentType,
                component::EntityType childType);

    }
}
