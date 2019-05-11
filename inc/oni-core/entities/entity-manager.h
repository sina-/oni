#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <iostream>

#include <entt/entt.hpp>

#include <oni-core/common/typedefs.h>
#include <oni-core/entities/entity.h>
#include <oni-core/component/tag.h>
#include <oni-core/component/visual.h>


namespace oni {
    namespace entities {
        typedef common::EntityID EntityIDType;

        class EntityFactory;

        class EntityManager {
        public:
            friend EntityFactory;

            template<class Entity, class... Components>
            class EntityView {
            private:
                friend EntityManager;

                explicit EntityView(entt::basic_registry<EntityIDType> &registry) :
                        mView(registry.view<Components...>()) {
                }

                EntityView(entt::basic_registry<EntityIDType> &registry,
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
                get(EntityIDType entityID) noexcept {
                    if constexpr(sizeof...(Components) == 1) {
                        return mView.get(entityID);
                    } else {
                        return mView.template get<Component>(entityID);
                    }
                }

                template<class Component>
                const Component &
                get(EntityIDType entityID) const noexcept {
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

            // TODO: Group like this is use-less. Each component that is used is kinda "owned" by the group and
            // you can't have two groups that share components as it will assert :( I probably need just the
            // partially-owning group where the shared component between groups is passed as template param and
            // other components as function argument.
            template<class Entity, class... Components>
            class EntityGroup {
            private:
                friend EntityManager;

                explicit EntityGroup(entt::basic_registry<EntityIDType> &registry) :
                        mGroup(registry.group<Components...>()) {
                }

                EntityGroup(entt::basic_registry<EntityIDType> &registry,
                            std::unique_lock<std::mutex> registryLock) :
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
                get(EntityIDType entityID) noexcept {
                    if constexpr(sizeof...(Components) == 1) {
                        return mGroup.get(entityID);
                    } else {
                        return mGroup.template get<Component>(entityID);
                    }
                }

                template<class Component>
                const Component &
                get(EntityIDType entityID) const noexcept {
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
                std::unique_lock<std::mutex> mRegistryLock{};
            };

            EntityManager() {
                mRegistry = std::make_unique<entt::basic_registry<common::uint32 >>();
                mLoader = std::make_unique<entt::basic_continuous_loader<EntityIDType>>(*mRegistry);
                mDispatcher = std::make_unique<entt::dispatcher>();
                //mLock = std::unique_lock<std::mutex>(mMutex, std::defer_lock);
            }

            ~EntityManager() = default;

            size_t
            size() noexcept {
                auto result = mRegistry->size();
                return result;
            }

            size_t
            capacity() noexcept {
                auto result = mRegistry->capacity();
                return result;
            }

            template<class Component>
            size_t
            size() noexcept {
                auto result = mRegistry->size<Component>();
                return result;
            }

            size_t
            alive() noexcept {
                auto result = mRegistry->alive();
                return result;
            }

            template<class... ViewComponents>
            EntityView<EntityIDType, ViewComponents...>
            createView() {
                return EntityView<EntityIDType, ViewComponents...>(*mRegistry);
            }

            template<class... ViewComponents>
            EntityView<EntityIDType, ViewComponents...>
            createViewWithLock() {
                std::unique_lock<std::mutex> registryLock(mMutex);
                return EntityView<EntityIDType, ViewComponents...>(*mRegistry, std::move(registryLock));
            }

            template<class... GroupComponents>
            EntityGroup<EntityIDType, GroupComponents...>
            createGroup() {
                return EntityGroup<EntityIDType, GroupComponents...>(*mRegistry);
            }

            template<class... GroupComponents>
            EntityGroup<EntityIDType, GroupComponents...>
            createGroupWithLock() {
                std::unique_lock<std::mutex> registryLock(mMutex);
                return EntityView<EntityIDType, GroupComponents...>(*mRegistry, std::move(registryLock));
            }

            template<class Component>
            Component &
            get(EntityIDType entityID) noexcept {
                return mRegistry->get<Component>(entityID);
            }

            EntityIDType
            map(EntityIDType entityID) {
                auto result = mLoader->map(entityID);
                if (result == entt::null) {
                    return 0;
                }
                return result;
            }

/*            template<class Component>
            const Component &get(EntityID entityID) noexcept {
                return mRegistry->get<Component>(entityID);
            }*/

            template<class Component>
            bool
            has(EntityIDType entityID) noexcept {
                assert(mRegistry->valid(entityID));

                bool result{false};
                result = mRegistry->has<Component>(entityID);
                return result;
            }

            template<class Component, class... Args>
            void
            replace(EntityIDType entityID,
                    Args &&... args) {
                {
                    //std::lock_guard<std::mutex> registryLock(mMutex);
                    mRegistry->replace<Component>(entityID, std::forward<Args>(args)...);
                }
            }

            template<class Archive, class... ArchiveComponents, class... Type, class... Member>
            void
            restore(entities::SnapshotType snapshotType,
                    Archive &archive,
                    Member Type::*... member) {
                switch (snapshotType) {
                    case entities::SnapshotType::ENTIRE_REGISTRY: {
                        mLoader->entities(archive).template component<ArchiveComponents...>(false, archive, member...);
                        break;
                    }
                    case entities::SnapshotType::ONLY_COMPONENTS: {
                        mLoader->template component<ArchiveComponents...>(true, archive, member...);
                        break;
                    }
                    case entities::SnapshotType::ONLY_NEW_ENTITIES: {
                        mLoader->entities(archive).template component<ArchiveComponents...>(true, archive, member...);
                        break;
                    }
/*                    case components::SnapshotType::REMOVE_NON_EXISTING_ENTITIES: {
                        mLoader->entities(archive);
                        break;
                    }*/
                    default: {
                        assert(false);
                    }
                }
            }

            template<class Archive, class ...ArchiveComponents>
            void
            snapshot(Archive &archive,
                     entities::SnapshotType snapshotType) {
                switch (snapshotType) {
                    case entities::SnapshotType::ONLY_COMPONENTS: {
                        // TODO: Rather not have this class know about specific components!
                        {
                            auto view = mRegistry->view<component::Tag_OnlyComponentUpdate>();
                            if (!view.empty()) {
                                mRegistry->snapshot().template component<ArchiveComponents...>(archive,
                                                                                               view.begin(),
                                                                                               view.end());
                                mRegistry->reset<component::Tag_OnlyComponentUpdate>();
                            }
                        }
                        {
                            // TODO: This component tracks all the changes server side so that if there are changes
                            // between network updates those changes won't be lost. This could happen to any other
                            // component that is updated faster than network poll rate. I need a generic solution
                            // to this problem. Maybe I should have a global tick id and every tick id has a set
                            // of components that has been updated, so that each client can be sync'd with all
                            // those changes
                            auto view = createView<component::Trail>();
                            for (auto &&entity: view) {
                                view.get<component::Trail>(entity).velocity.clear();
                                view.get<component::Trail>(entity).previousPos.clear();
                            }
                        }
                        break;
                    }
                    case entities::SnapshotType::ONLY_NEW_ENTITIES: {
                        {
                            auto view = mRegistry->view<component::Tag_RequiresNetworkSync>();
                            if (!view.empty()) {
                                mRegistry->snapshot().entities(archive).template component<ArchiveComponents...>(
                                        archive,
                                        view.begin(),
                                        view.end());
                                mRegistry->reset<component::Tag_RequiresNetworkSync>();
                            }
                        }
                        {
                            auto view = createView<component::Trail>();
                            for (auto &&entity: view) {
                                view.get<component::Trail>(entity).velocity.clear();
                                view.get<component::Trail>(entity).previousPos.clear();
                            }
                        }
                        break;
                    }
                    case entities::SnapshotType::ENTIRE_REGISTRY: {
                        mRegistry->snapshot().entities(archive).template component<ArchiveComponents...>(archive);
                        break;
                    }
/*                    case components::SnapshotType::REMOVE_NON_EXISTING_ENTITIES: {
                        mRegistry->snapshot().entities(archive).template component<>(archive,);
                        break;
                    }*/
                    default: {
                        assert(false);
                    }
                }
            }

            std::unique_lock<std::mutex>
            scopedLock() {
                return std::unique_lock<std::mutex>(mMutex);
            }

            void
            clearDeletedEntitiesList() {
                mDeletedEntities.clear();
            }

            const std::vector<EntityIDType> &
            getDeletedEntities() const {
                return mDeletedEntities;
            }

            void
            tagForComponentSync(EntityIDType entity) {
                accommodate<component::Tag_OnlyComponentUpdate>(entity);
            }

            template<class Component, class Comparator>
            void
            sort(Comparator comparator) {
                mRegistry->sort<Component, Comparator>(std::move(comparator));
            }

            template<class Event, auto Method, class MethodInstance>
            void
            registerEventHandler(MethodInstance *instance) {
                mDispatcher->sink<Event>().template connect<Method>(instance);
            }

            template<class Event, class... Args>
            void
            enqueueEvent(Args &&...args) {
                mDispatcher->enqueue<Event>(std::forward<Args>(args)...);
            }

            template<class Event>
            void
            enqueueEvent() {
                mDispatcher->enqueue<Event>();
            }

            void
            dispatchEvents() {
                mDispatcher->update();
            }

        private:
            EntityIDType
            create() {
                EntityIDType result{};
                result = mRegistry->create();
                return result;
            }

            template<class Component>
            void
            remove(EntityIDType entityID) {
                mRegistry->remove<Component>(entityID);
            }

            template<class Component, class... Args>
            Component &
            assign(EntityIDType entityID,
                   Args &&... args) {
                return mRegistry->assign<Component>(entityID, std::forward<Args>(args)...);
            }

            template<class Component, class... Args>
            void
            accommodate(EntityIDType entityID,
                        Args &&... args) {
                {
                    //std::lock_guard<std::mutex> registryLock(mMutex);
                    mRegistry->assign_or_replace<Component>(entityID, std::forward<Args>(args)...);
                }
            }

            void
            destroy(EntityIDType entityID) {
                mRegistry->destroy(entityID);
            }

            template<class... Component>
            void
            destroy() {
                mRegistry->reset<Component...>();
            }

            void
            destroyAndTrack(EntityIDType entityID) {
                mRegistry->destroy(entityID);
                mDeletedEntities.push_back(entityID);
            }

            bool
            valid(EntityIDType entityID) {
                return mRegistry->valid(entityID);
            }

        private:
            std::unique_ptr<entt::basic_registry<EntityIDType>> mRegistry{};
            std::unique_ptr<entt::basic_continuous_loader<EntityIDType>> mLoader{};
            std::unique_ptr<entt::dispatcher> mDispatcher{};
            mutable std::mutex mMutex{};
            //std::unique_lock<std::mutex> mLock{};

            std::vector<EntityIDType> mDeletedEntities{};
        };
    }
}
