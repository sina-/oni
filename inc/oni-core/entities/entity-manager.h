#pragma once

#include <memory>
#include <vector>

#include <entt/entt.hpp>

#include <oni-core/common/typedefs.h>
#include <oni-core/entities/create-entity.h>

/*
namespace entt {
    template<typename Entity>
    class Registry;

    using DefaultRegistry = Registry<oni::common::uint32>;

    template<typename Entity, typename... Components>
    class PersistentView;
}*/

namespace oni {
    namespace entities {
        typedef common::uint32 EntityType;

        class EntityManager {
        public:

            template<class Entity, class... Components>
            class EntityView {
            private:
                friend EntityManager;

                explicit EntityView(entt::Registry<EntityType> &registry) : mView(
                        registry.persistent<Components...>()) {
                }

            public:
                ~EntityView() = default;

                // TODO: Can I avoid creating a vector?
                std::vector<entities::EntityID> getEntities() {
                    return std::vector<entities::EntityID>(mView.begin(), mView.end());
                };

                template<class Component>
                Component &get(entities::EntityID entityID) noexcept {
                    return mView.template get<Component>(entityID);
                }

                template<class Component>
                const Component &get(entities::EntityID entityID) const noexcept {
                    return mView.template get<Component>(entityID);
                }

            private:
                entt::PersistentView<Entity, Components...> mView{};
            };

            EntityManager();

            ~EntityManager();

            EntityID create();

            size_t size() const noexcept;

            template<class Component, class... Args>
            Component &assign(EntityID entityID, Args &&... args) {
                return mRegistry->assign<Component>(entityID, std::forward<Args>(args)...);
            }

            template<class... ViewComponents>
            EntityView<EntityType, ViewComponents...> createView() {
                return EntityView<EntityType, ViewComponents...>(*mRegistry);
            }

            template<class Component>
            Component &get(entities::EntityID entityID) noexcept {
                return mRegistry->get<Component>(entityID);
            }

            template<class Component>
            const Component &get(entities::EntityID entityID) const noexcept {
                return mRegistry->get<Component>(entityID);
            }

            template<class Component>
            bool has(entities::EntityID entityID) const noexcept {
                return mRegistry->has<Component>(entityID);
            }

            template<class Component, class... Args>
            Component &replace(EntityID entityID, Args &&... args) {
                return mRegistry->replace<Component>(entityID, std::forward<Args>(args)...);
            }

            template<class Archive, class ...ArchiveComponents>
            void restore(Archive &archive) {
                mRegistry->restore().entities(archive).template component<ArchiveComponents...>(archive);
            };

            template<class Archive, class ...ArchiveComponents>
            void snapshot(Archive &archive) {
                mRegistry->snapshot().entities(archive).template component<ArchiveComponents...>(archive);
            };

        private:
            std::unique_ptr<entt::Registry<EntityType>> mRegistry{};
        };

    }
}
