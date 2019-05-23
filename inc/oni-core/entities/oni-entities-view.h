#pragma once

#include <mutex>

#include <entt/entity/registry.hpp>

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    namespace entities {
        class EntityManager;

        template<class Entity, class... Components>
        class EntityView {
        private:
            friend EntityManager;

            explicit EntityView(entt::basic_registry<common::EntityID> &registry) :
                    mView(registry.view<Components...>()) {
            }

            EntityView(entt::basic_registry<common::EntityID> &registry,
                       std::unique_lock<std::mutex> registryLock) :
                    mView(registry.view<Components...>()),
                    mRegistryLock(std::move(registryLock)) {
            }

        public:
            EntityView(const EntityView &) = delete;

            EntityView(const EntityView &&) = delete;

            EntityView &
            operator=(const Entity &) = delete;

            ~EntityView() = default;

            size_t
            size() noexcept {
                return mView.size();
            }

            auto
            begin() {
                return mView.begin();
            }

            auto
            end() {
                return mView.end();
            }

            template<class Component>
            Component &
            get(common::EntityID entityID) noexcept {
                if constexpr(sizeof...(Components) == 1) {
                    return mView.get(entityID);
                } else {
                    return mView.template get<Component>(entityID);
                }
            }

            template<class Component>
            const Component &
            get(common::EntityID entityID) const noexcept {
                if constexpr(sizeof...(Components) == 1) {
                    return mView.get(entityID);
                } else {
                    return mView.template get<Component>(entityID);
                }
            }

        private:
            entt::basic_view<Entity, Components...> mView{};
            std::unique_lock<std::mutex> mRegistryLock{};
        };

    }
}