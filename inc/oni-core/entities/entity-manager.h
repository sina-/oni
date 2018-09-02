#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <iostream>

#include <entt/entt.hpp>

#include <oni-core/common/typedefs.h>
#include <oni-core/components/entity-lifetime.h>


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

                EntityView &operator=(const Entity &) = delete;

                ~EntityView() = default;

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
                mLock = std::unique_lock<std::mutex>(mMutex, std::defer_lock);
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
            void restore(Archive &archive, Member Type::*... member) {
                {
                    mLoader->entities(archive).template component<ArchiveComponents...>(archive, member...)
                            .orphans().shrink();
                }
            }

            template<class Archive, class ...ArchiveComponents>
            void snapshot(Archive &archive, bool delta) {
                if (delta) {
                    // TODO: Rather not have this class know about specific components!
                    auto view = mRegistry->view<components::TagNewlyCreated>();
                    if (!view.empty()) {
                        mRegistry->snapshot().entities(archive).template component<ArchiveComponents...>(archive,
                                                                                                         view.cbegin(),
                                                                                                         view.cend());
                    }
                } else {
                    mRegistry->snapshot().entities(archive).template component<ArchiveComponents...>(archive);
                }
            }

            std::unique_lock<std::mutex> scopedLock() {
                return std::unique_lock<std::mutex>(mMutex);
            }

/*
            void lock() {
                mLock.lock();
            }

            void unlock() {
                mLock.unlock();
            }
*/

        private:
            std::unique_ptr<entt::Registry<EntityType>> mRegistry{};
            std::unique_ptr<entt::ContinuousLoader<EntityType>> mLoader{};
            std::mutex mMutex{};
            std::unique_lock<std::mutex> mLock{};
        };

    }
}
