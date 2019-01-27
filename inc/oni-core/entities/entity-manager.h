#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <iostream>

#include <entt/entt.hpp>

#include <oni-core/common/typedefs.h>
#include <oni-core/component/snapshot-type.h>
#include <oni-core/component/tag.h>


namespace oni {
    namespace entities {
        typedef common::EntityID EntityType;

        class EntityManager {
        public:

            template<class Entity, class... Components>
            class EntityView {
            private:
                friend EntityManager;

                explicit EntityView(entt::Registry<EntityType> &registry) :
                        mView(registry.view<Components...>(entt::persistent_t{})) {
                }

                EntityView(entt::Registry<EntityType> &registry,
                           std::unique_lock<std::mutex> registryLock) :
                        mView(registry.view<Components...>(entt::persistent_t{})),
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
                Component &get(EntityType entityID) noexcept {
                    return mView.template get<Component>(entityID);
                }

                template<class Component>
                const Component &get(EntityType entityID) const noexcept {
                    return mView.template get<Component>(entityID);
                }

            private:
                entt::PersistentView<Entity, Components...> mView{};
                std::unique_lock<std::mutex> mRegistryLock{};
            };

            EntityManager() {
                mRegistry = std::make_unique<entt::DefaultRegistry>();
                mLoader = std::make_unique<entt::ContinuousLoader<EntityType>>(*mRegistry);
                //mLock = std::unique_lock<std::mutex>(mMutex, std::defer_lock);
            }

            ~EntityManager() = default;

            EntityType create() {
                EntityType result{};
                {
                    //std::lock_guard<std::mutex> registryLock(mMutex);
                    result = mRegistry->create();
                }
                return result;
            }

            size_t size() noexcept {
                size_t result{0};
                {
                    auto lock = scopedLock();
                    result = mRegistry->size();
                }
                return result;
            }

            template<class Component>
            size_t size() noexcept {
                size_t result{0};
                {
                    auto lock = scopedLock();
                    result = mRegistry->size<Component>();
                }
                return result;
            }

            size_t alive() noexcept {
                size_t result{0};
                {
                    auto lock = scopedLock();
                    result = mRegistry->alive();
                }
                return result;
            }

            template<class Component, class... Args>
            void assign(EntityType entityID, Args &&... args) {
                {
                    //std::lock_guard<std::mutex> registryLock(mMutex);
                    mRegistry->assign<Component>(entityID, std::forward<Args>(args)...);
                }
            }

            template<class... ViewComponents>
            EntityView<EntityType, ViewComponents...> createView() {
                //std::unique_lock<std::mutex> registryLock(mMutex);
                //return EntityView<EntityType, ViewComponents...>(*mRegistry, std::move(registryLock));
                return EntityView<EntityType, ViewComponents...>(*mRegistry);
            }

            template<class... ViewComponents>
            EntityView<EntityType, ViewComponents...> createViewScopeLock() {
                std::unique_lock<std::mutex> registryLock(mMutex);
                return EntityView<EntityType, ViewComponents...>(*mRegistry, std::move(registryLock));
            }


            template<class Component>
            void remove(EntityType entityID) {
                mRegistry->remove<Component>(entityID);
            }

            template<class Component>
            void reset() {
                mRegistry->reset<Component>();
            }

            void destroy(EntityType entityID) {
                mRegistry->destroy(entityID);
            }

            template<class Component>
            Component &get(EntityType entityID) noexcept {
                return mRegistry->get<Component>(entityID);
            }

            EntityType map(EntityType entityID) {
                return mLoader->map(entityID);
            }

/*            template<class Component>
            const Component &get(EntityType entityID) noexcept {
                return mRegistry->get<Component>(entityID);
            }*/

            template<class Component>
            bool has(EntityType entityID) noexcept {
                bool result{false};
                {
                    //std::lock_guard<std::mutex> registryLock(mMutex);
                    result = mRegistry->has<Component>(entityID);
                }
                return result;
            }

            template<class Component, class... Args>
            void replace(EntityType entityID, Args &&... args) {
                {
                    //std::lock_guard<std::mutex> registryLock(mMutex);
                    mRegistry->replace<Component>(entityID, std::forward<Args>(args)...);
                }
            }

            template<class Component, class... Args>
            void accommodate(EntityType entityID, Args &&... args) {
                {
                    //std::lock_guard<std::mutex> registryLock(mMutex);
                    mRegistry->accommodate<Component>(entityID, std::forward<Args>(args)...);
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
                                                                                           view.cbegin(),
                                                                                           view.cend());
                        }
                        break;
                    }
                    case component::SnapshotType::ONLY_NEW_ENTITIES: {
                        auto view = mRegistry->view<component::Tag_NewEntity>();
                        if (!view.empty()) {
                            mRegistry->snapshot().entities(archive).template component<ArchiveComponents...>(archive,
                                                                                                             view.cbegin(),
                                                                                                             view.cend());
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

            void addDeletedEntity(EntityType entity) {
                mDeletedEntities.push_back(entity);
            }

            bool containsDeletedEntities() {
                return !mDeletedEntities.empty();
            }

            void clearDeletedEntitiesList() {
                mDeletedEntities.clear();
            }

            const std::vector<EntityType> &getDeletedEntities() const {
                return mDeletedEntities;
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
            std::unique_ptr<entt::Registry<EntityType>> mRegistry{};
            std::unique_ptr<entt::ContinuousLoader<EntityType>> mLoader{};
            std::mutex mMutex{};
            //std::unique_lock<std::mutex> mLock{};

            std::vector<EntityType> mDeletedEntities{};
        };

    }
}
