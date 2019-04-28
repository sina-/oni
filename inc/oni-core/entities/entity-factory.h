#pragma once

#include <mutex>

#include <oni-core/common/typedefs.h>
#include <oni-core/component/physic.h>
#include <oni-core/common/typedefs-graphics.h>
#include <oni-core/component/entity.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/component/entity-event.h>
#include <oni-core/component/audio.h>

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
        class Texture;
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

            template<component::EntityType entityType, class ...Args>
            common::EntityID
            createEntity(const Args &... args) {
                common::EntityID entityID = createEntity();
                auto &type = createComponent<component::EntityType>(entityID);
                type = entityType;
                _createEntity<entityType>(entityID, args...);
                return entityID;
            }

            template<component::EventType eventType, class ...Args>
            common::EntityID
            createEvent(const Args &... args) {
                common::EntityID entityID = createEntity();
                auto &type = createComponent<component::EventType>(entityID);
                type = eventType;
                _createEvent<eventType>(entityID, args...);
                return entityID;
            }

            template<component::EventType eventType, class Func>
            void
            apply(Func &func) {
                _apply<eventType>(func);
            }

            void
            removeEntity(common::EntityID,
                         const component::EntityOperationPolicy &);

            void
            resetEvents();

            void
            attach(common::EntityID parent,
                   common::EntityID child,
                   component::EntityType parentType,
                   component::EntityType childType);

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

            template<component::EntityType, class ...Args>
            void
            _createEntity(common::EntityID,
                          const Args &...) = delete;

            template<component::EventType, class ...Args>
            void
            _createEvent(common::EntityID,
                         const Args &...) = delete;

            template<component::EventType eventType, class Func>
            void
            _apply(Func &func) = delete;

            template<>
            void
            _apply<component::EventType::COLLISION>(std::function<void(component::CollidingEntity &,
                                                                       component::CollisionPos &)> &);

            template<>
            void
            _apply<component::EventType::ONE_SHOT_SOUND_EFFECT>(std::function<void(component::SoundID &,
                                                                                   component::SoundPos &)> &);

            template<>
            void
            _apply<component::EventType::ROCKET_LAUNCH>(std::function<void(math::vec2 &)> &);

            void
            _removeEntity(common::EntityID,
                          component::EntityType entityType,
                          const component::EntityOperationPolicy &policy);

            template<component::EntityType entityType>
            void
            _removeEntity(common::EntityID,
                          const component::EntityOperationPolicy &policy) = delete;

            template<>
            void
            _createEntity<component::EntityType::RACE_CAR>(common::EntityID entityID,
                                                           const math::vec3 &pos,
                                                           const math::vec2 &size,
                                                           const common::real32 &heading,
                                                           const std::string &textureID);

            template<>
            void
            _createEntity<component::EntityType::VEHICLE>(common::EntityID entityID,
                                                          const math::vec3 &pos,
                                                          const math::vec2 &size,
                                                          const common::real32 &heading,
                                                          const std::string &textureID);

            template<>
            void
            _createEntity<component::EntityType::VEHICLE_GUN>(common::EntityID,
                                                              const math::vec3 &pos,
                                                              const math::vec2 &size,
                                                              const common::real32 &heading,
                                                              const std::string &textureID);

            template<>
            void
            _createEntity<component::EntityType::VEHICLE_TIRE_FRONT>(common::EntityID,
                                                                     const math::vec3 &pos,
                                                                     const math::vec2 &size,
                                                                     const common::real32 &heading,
                                                                     const std::string &textureID);

            template<>
            void
            _createEntity<component::EntityType::VEHICLE_TIRE_REAR>(common::EntityID,
                                                                    const math::vec3 &pos,
                                                                    const math::vec2 &size,
                                                                    const common::real32 &heading,
                                                                    const std::string &textureID);

            template<>
            void
            _createEntity<component::EntityType::WALL>(common::EntityID,
                                                       const math::vec3 &pos,
                                                       const math::vec2 &size,
                                                       const common::real32 &heading,
                                                       const std::string &textureID);

            template<>
            void
            _createEntity<component::EntityType::SIMPLE_SPRITE>(common::EntityID,
                                                                const math::vec3 &worldPos,
                                                                const math::vec2 &size,
                                                                const common::real32 &heading,
                                                                const math::vec4 &color);

            template<>
            void
            _createEntity<component::EntityType::SIMPLE_SPRITE>(common::EntityID,
                                                                const math::vec3 &worldPos,
                                                                const math::vec2 &size,
                                                                const common::real32 &heading,
                                                                const std::string &textureID);

            template<>
            void
            _createEntity<component::EntityType::SIMPLE_PARTICLE>(common::EntityID,
                                                                  const math::vec3 &worldPos,
                                                                  const math::vec4 &color,
                                                                  const common::real32 &halfSize,
                    // TODO: Is there a way to avoid need for const &?
                                                                  const bool &randomize);

            template<>
            void
            _createEntity<component::EntityType::SIMPLE_PARTICLE>(common::EntityID,
                                                                  const math::vec3 &worldPos,
                                                                  const std::string &textureID,
                                                                  const common::real32 &halfSize,
                                                                  const bool &randomize);

            template<>
            void
            _createEntity<component::EntityType::SIMPLE_BLAST_PARTICLE>(common::EntityID,
                                                                        const math::vec3 &worldPos,
                                                                        const std::string &textureID,
                                                                        const common::real32 &halfSize,
                                                                        const bool &randomize);

            template<>
            void
            _createEntity<component::EntityType::SIMPLE_ROCKET>(common::EntityID,
                                                                const math::vec3 &pos,
                                                                const math::vec2 &size,
                                                                const common::real32 &heading,
                                                                const std::string &textureID,
                                                                const common::real32 &velocity
            );

            template<>
            void
            _createEntity<component::EntityType::TEXT>(common::EntityID,
                                                       const math::vec3 &pos,
                                                       const std::string &text);

            template<>
            void
            _createEntity<component::EntityType::WORLD_CHUNK>(common::EntityID,
                                                              const math::vec3 &worldPos,
                                                              const math::vec2 &size,
                                                              const common::real32 &heading,
                                                              const std::string &textureID);

            template<>
            void
            _createEntity<component::EntityType::WORLD_CHUNK>(common::EntityID,
                                                              const math::vec3 &worldPos,
                                                              const math::vec2 &size,
                                                              const common::real32 &heading,
                                                              const math::vec4 &color);

        private:
            template<>
            void
            _createEvent<component::EventType::COLLISION>(common::EntityID,
                                                          const component::EntityType &,
                                                          const component::EntityType &,
                                                          const math::vec3 &worldPos);

            template<>
            void
            _createEvent<component::EventType::ONE_SHOT_SOUND_EFFECT>(common::EntityID,
                                                                      const component::SoundID &,
                                                                      const math::vec2 &worldPos);

            template<>
            void
            _createEvent<component::EventType::ROCKET_LAUNCH>(common::EntityID,
                                                              const math::vec2 &worldPos);

        private:
            template<>
            void
            _removeEntity<component::EntityType::RACE_CAR>(common::EntityID,
                                                           const component::EntityOperationPolicy &policy
            );

            template<>
            void
            _removeEntity<component::EntityType::WALL>(common::EntityID,
                                                       const component::EntityOperationPolicy &policy
            );

            template<>
            void
            _removeEntity<component::EntityType::SIMPLE_ROCKET>(common::EntityID,
                                                                const component::EntityOperationPolicy &policy
            );

        private:
            b2Body *
            createPhysicalBody(
                    const math::vec3 &worldPos,
                    const math::vec2 &size,
                    const common::real32 heading,
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

            template<class Component>
            Component &
            createComponent(common::EntityID entityID) {
                static_assert(std::is_aggregate_v<Component> || std::is_enum_v<Component>);
                return mRegistryManager->assign<Component>(entityID, Component{});
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
