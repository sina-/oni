#pragma once

#include <mutex>

#include <entt/entity/registry.hpp>

#include <oni-core/common/oni-common-typedef.h>


namespace oni {
    namespace entities {
        class EntityManager;

        // TODO: Group like this is use-less. Each component that is used is kinda "owned" by the group and
        // you can't have two groups that share components as it will assert :( I probably need just the
        // partially-owning group where the shared component between groups is passed as template param and
        // other components as function argument.
        template<class Entity, class... Components>
        class EntityGroup {
        private:
            friend EntityManager;

            explicit EntityGroup(entt::basic_registry <common::EntityID> &registry) :
                    mGroup(registry.group<Components...>()) {
            }

            EntityGroup(entt::basic_registry <common::EntityID> &registry,
                        std::unique_lock <std::mutex> registryLock) :
                    mGroup(registry.group<Components...>()),
                    mRegistryLock(std::move(registryLock)) {
            }

        public:
            EntityGroup(const EntityGroup &) = delete;

            EntityGroup(const EntityGroup &&) = delete;

            EntityGroup &
            operator=(const Entity &) = delete;

            ~EntityGroup() = default;

            size_t
            size() noexcept {
                return mGroup.size();
            }

            auto
            begin() {
                return mGroup.begin();
            }

            auto
            end() {
                return mGroup.end();
            }

            template<class Component>
            Component &
            get(common::EntityID entityID) noexcept {
                if constexpr(sizeof...(Components) == 1) {
                    return mGroup.get(entityID);
                } else {
                    return mGroup.template get<Component>(entityID);
                }
            }

            template<class Component>
            const Component &
            get(common::EntityID entityID) const noexcept {
                if constexpr(sizeof...(Components) == 1) {
                    return mGroup.get(entityID);
                } else {
                    return mGroup.template get<Component>(entityID);
                }
            }

            template<class Func>
            void
            apply(Func &func) {
                mGroup.each(func);
            }

        private:
            entt::basic_group<Entity, entt::get_t<>, Components...> mGroup;
            std::unique_lock <std::mutex> mRegistryLock{};
        };

    }
}
