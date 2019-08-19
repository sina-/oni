#pragma once

#include <mutex>

#include <entt/entity/registry.hpp>

#include <oni-core/common/oni-common-typedef.h>


namespace oni {
    class EntityManager;

    template<class Entity, class... Components>
    class EntityView {
    private:
        friend EntityManager;

        explicit EntityView(entt::basic_registry<EntityID> &registry) :
                mView(registry.view<Components...>()) {
        }

        EntityView(entt::basic_registry<EntityID> &registry,
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

        auto
        empty() {
            return mView.empty();
        }

        template<class Component>
        Component &
        get(EntityID entityID) noexcept {
            if constexpr(sizeof...(Components) == 1) {
                return mView.get(entityID);
            } else {
                return mView.template get<Component>(entityID);
            }
        }

        template<class Component>
        const Component &
        get(EntityID entityID) const noexcept {
            if constexpr(sizeof...(Components) == 1) {
                return mView.get(entityID);
            } else {
                return mView.template get<Component>(entityID);
            }
        }

        template<typename Func>
        inline void
        each(Func func) const {
            mView.template each(std::move(func));
        };

        template<typename Comp, typename Func>
        inline void
        each(Func func) const {
            mView.template each<Comp>(std::move(func));
        };

    private:
        entt::basic_view<Entity, Components...> mView{};
        std::unique_lock<std::mutex> mRegistryLock{};
    };
}