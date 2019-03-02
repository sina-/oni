#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <iostream>

#include <entt/entt.hpp>

#include <oni-core/common/typedefs.h>
#include <oni-core/component/entity-definition.h>
#include <oni-core/component/tag.h>


namespace oni {
    namespace entities {
        typedef common::EntityID EntityID;

        class EntityFactory;

        class EntityManager {
        public:
            friend EntityFactory;

            template<class Entity, class... Components>
            class EntityView {
            private:
                friend EntityManager;

                explicit EntityView(entt::registry<EntityID> &registry) :
                        mView(registry.view<Components...>()) {
                }

                EntityView(entt::registry<EntityID> &registry,
                           std::unique_lock<std::mutex> registryLock) :
                        mView(registry.view<Components...>()),
                        mRegistryLock(std::move(registryLock)) {
                }

            public:
                EntityView(const EntityView &) = delete;

                EntityView(const EntityView &&) = delete;

                EntityView &operator=(const Entity &) = delete;

                ~EntityView() = default;

                size_t size() noexcept {
                    return mView.size();
                }

                auto begin() {
                    return mView.begin();
                }

                auto end() {
                    return mView.end();
                }

                template<class Component>
                Component &get(EntityID entityID) noexcept {
                    if constexpr(sizeof...(Components) == 1) {
                        return mView.get(entityID);
                    } else {
                        return mView.template get<Component>(entityID);
                    }
                }

                template<class Component>
                const Component &get(EntityID entityID) const noexcept {
                    if constexpr(sizeof...(Components) == 1) {
                        return mView.get(entityID);
                    } else {
                        return mView.template get<Component>(entityID);
                    }
                }

            private:
                entt::view<Entity, Components...> mView{};
                std::unique_lock<std::mutex> mRegistryLock{};
            };

            EntityManager() {
                mRegistry = std::make_unique<entt::registry<common::uint32 >>();
                mLoader = std::make_unique<entt::continuous_loader<EntityID>>(*mRegistry);
                //mLock = std::unique_lock<std::mutex>(mMutex, std::defer_lock);
            }

            ~EntityManager() = default;

            size_t size() noexcept {
                auto result = mRegistry->size();
                return result;
            }

            size_t capacity() noexcept {
                auto result = mRegistry->capacity();
                return result;
            }

            template<class Component>
            size_t size() noexcept {
                auto result = mRegistry->size<Component>();
                return result;
            }

            size_t alive() noexcept {
                auto result = mRegistry->alive();
                return result;
            }

            template<class... ViewComponents>
            EntityView<EntityID, ViewComponents...> createView() {
                //std::unique_lock<std::mutex> registryLock(mMutex);
                //return EntityView<EntityID, ViewComponents...>(*mRegistry, std::move(registryLock));
                return EntityView<EntityID, ViewComponents...>(*mRegistry);
            }

            template<class... ViewComponents>
            EntityView<EntityID, ViewComponents...> createViewScopeLock() {
                std::unique_lock<std::mutex> registryLock(mMutex);
                return EntityView<EntityID, ViewComponents...>(*mRegistry, std::move(registryLock));
            }

            template<class Component>
            void reset() {
                mRegistry->reset<Component>();
            }

            // TODO: This shouldn't be exposes like this! Only removeEntity from EntityFactory should handle destruction.
            void destroy(EntityID entityID) {
                mRegistry->destroy(entityID);
            }

            template<class Component>
            Component &get(EntityID entityID) noexcept {
                return mRegistry->get<Component>(entityID);
            }

            EntityID map(EntityID entityID) {
                return mLoader->map(entityID);
            }

/*            template<class Component>
            const Component &get(EntityID entityID) noexcept {
                return mRegistry->get<Component>(entityID);
            }*/

            template<class Component>
            bool has(EntityID entityID) noexcept {
                bool result{false};
                {
                    //std::lock_guard<std::mutex> registryLock(mMutex);
                    result = mRegistry->has<Component>(entityID);
                }
                return result;
            }

            template<class Component, class... Args>
            void replace(EntityID entityID, Args &&... args) {
                {
                    //std::lock_guard<std::mutex> registryLock(mMutex);
                    mRegistry->replace<Component>(entityID, std::forward<Args>(args)...);
                }
            }

            template<class Archive, class... ArchiveComponents, class... Type, class... Member>
            void restore(component::SnapshotType snapshotType, Archive &archive, Member Type::*... member) {
                switch (snapshotType) {
                    case component::SnapshotType::ENTIRE_REGISTRY: {
                        mLoader->entities(archive).template component<ArchiveComponents...>(false, archive, member...);
                        break;
                    }
                    case component::SnapshotType::ONLY_COMPONENTS: {
                        mLoader->template component<ArchiveComponents...>(true, archive, member...);
                        break;
                    }
                    case component::SnapshotType::ONLY_NEW_ENTITIES: {
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
            void snapshot(Archive &archive, component::SnapshotType snapshotType) {
                switch (snapshotType) {
                    case component::SnapshotType::ONLY_COMPONENTS: {
                        // TODO: Rather not have this class know about specific components!
                        auto view = mRegistry->view<component::Tag_OnlyComponentUpdate>();
                        if (!view.empty()) {
                            mRegistry->snapshot().template component<ArchiveComponents...>(archive,
                                                                                           view.begin(),
                                                                                           view.end());
                        }
                        break;
                    }
                    case component::SnapshotType::ONLY_NEW_ENTITIES: {
                        auto view = mRegistry->view<component::Tag_NewEntity>();
                        if (!view.empty()) {
                            mRegistry->snapshot().entities(archive).template component<ArchiveComponents...>(archive,
                                                                                                             view.begin(),
                                                                                                             view.end());
                        }
                        break;
                    }
                    case component::SnapshotType::ENTIRE_REGISTRY: {
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

            std::unique_lock<std::mutex> scopedLock() {
                return std::unique_lock<std::mutex>(mMutex);
            }

            void addDeletedEntity(EntityID entity) {
                mDeletedEntities.push_back(entity);
            }

            bool containsDeletedEntities() {
                return !mDeletedEntities.empty();
            }

            void clearDeletedEntitiesList() {
                mDeletedEntities.clear();
            }

            const std::vector<EntityID> &getDeletedEntities() const {
                return mDeletedEntities;
            }

            void tagForComponentSync(EntityID entity) {
                accommodate<component::Tag_OnlyComponentUpdate>(entity);
            }

/*
            void lock() {
                mLock.lock();
            }

            void unlock() {
                mLock.unlock();
            }
*/
            template<class Component, class Comparator>
            void sort(Comparator comparator) {
                mRegistry->sort<Component, Comparator>(std::move(comparator));
            }

        private:
            EntityID create() {
                EntityID result{};
                result = mRegistry->create();
                return result;
            }

            template<class Component>
            void remove(EntityID entityID) {
                mRegistry->remove<Component>(entityID);
            }

            template<class Component, class... Args>
            Component &assign(EntityID entityID, Args &&... args) {
                return mRegistry->assign<Component>(entityID, std::forward<Args>(args)...);
            }

            template<class Component, class... Args>
            void accommodate(EntityID entityID, Args &&... args) {
                {
                    //std::lock_guard<std::mutex> registryLock(mMutex);
                    mRegistry->assign_or_replace<Component>(entityID, std::forward<Args>(args)...);
                }
            }

        private:
            std::unique_ptr<entt::registry<EntityID>> mRegistry{};
            std::unique_ptr<entt::continuous_loader<EntityID>> mLoader{};
            std::mutex mMutex{};
            //std::unique_lock<std::mutex> mLock{};

            std::vector<EntityID> mDeletedEntities{};
        };

    }
}
