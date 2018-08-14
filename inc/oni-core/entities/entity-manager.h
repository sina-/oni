#pragma once

#include <memory>
#include <vector>

#include <entt/entt.hpp>

#include <oni-core/common/typedefs.h>

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
        typedef common::EntityID EntityType;

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

                auto begin() {
                    return mView.begin();
                }

                auto end() {
                    return mView.end();
                }

                template<class Component>
                Component &get(common::EntityID entityID) noexcept {
                    return mView.template get<Component>(entityID);
                }

                template<class Component>
                const Component &get(common::EntityID entityID) const noexcept {
                    return mView.template get<Component>(entityID);
                }

            private:
                entt::PersistentView<Entity, Components...> mView{};
            };

            EntityManager() {
                mRegistry = std::make_unique<entt::DefaultRegistry>();
            }

            ~EntityManager() = default;

            common::EntityID create() {
                common::EntityID result{};
                {
                    result = mRegistry->create();
                }
                return result;
            }

            size_t size() const noexcept{
                size_t result{0};
                {
                   result =  mRegistry->size();
                }
                return result;
            }

            template<class Component, class... Args>
            void assign(common::EntityID entityID, Args &&... args) {
                {
                    mRegistry->assign<Component>(entityID, std::forward<Args>(args)...);
                }
            }

            template<class... ViewComponents>
            EntityView<EntityType, ViewComponents...> createView() {
                return EntityView<EntityType, ViewComponents...>(*mRegistry);
            }

            template<class Component>
            Component &get(common::EntityID entityID) noexcept {
                return mRegistry->get<Component>(entityID);
            }

            template<class Component>
            const Component &get(common::EntityID entityID) const noexcept {
                return mRegistry->get<Component>(entityID);
            }

            template<class Component>
            bool has(common::EntityID entityID) const noexcept {
                bool result{false};
                {
                    result = mRegistry->has<Component>(entityID);
                }
                return result;
            }

            template<class Component, class... Args>
            void replace(common::EntityID entityID, Args &&... args) {
                {
                    mRegistry->replace<Component>(entityID, std::forward<Args>(args)...);
                }
            }

            template<class Archive, class ...ArchiveComponents>
            void restore(Archive &archive) {
                {
                    mRegistry->restore().entities(archive).template component<ArchiveComponents...>(archive);
                }
            }

            template<class Archive, class ...ArchiveComponents>
            void snapshot(Archive &archive) {
                {
                    mRegistry->snapshot().entities(archive).template component<ArchiveComponents...>(archive);
                }
            }

        private:
            std::unique_ptr<entt::Registry<EntityType>> mRegistry{};
        };

    }
}
