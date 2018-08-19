#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include <entt/entt.hpp>

#include <oni-core/common/typedefs.h>


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
                        mView(registry.persistent<Components...>()) {
                }

                EntityView(entt::Registry<EntityType> &registry,
                           std::unique_lock<std::mutex> registryLock) :
                        mView(registry.persistent<Components...>()),
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
                Component &get(common::EntityID entityID) noexcept {
                    return mView.template get<Component>(entityID);
                }

                template<class Component>
                const Component &get(common::EntityID entityID) const noexcept {
                    return mView.template get<Component>(entityID);
                }

            private:
                entt::PersistentView<Entity, Components...> mView{};
                std::unique_lock<std::mutex> mRegistryLock{};
            };

            EntityManager() {
                mRegistry = std::make_unique<entt::DefaultRegistry>();
                mLock = std::unique_lock<std::mutex>(mMutex, std::defer_lock);
            }

            ~EntityManager() = default;

            common::EntityID create() {
                common::EntityID result{};
                {
                    //std::lock_guard<std::mutex> registryLock(mMutex);
                    result = mRegistry->create();
                }
                return result;
            }

            size_t size() noexcept {
                size_t result{0};
                {
                    //std::lock_guard<std::mutex> registryLock(mMutex);
                    result = mRegistry->size();
                }
                return result;
            }

            template<class Component, class... Args>
            void assign(common::EntityID entityID, Args &&... args) {
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
            Component &get(common::EntityID entityID) noexcept {
                return mRegistry->get<Component>(entityID);
            }

/*            template<class Component>
            const Component &get(common::EntityID entityID) noexcept {
                return mRegistry->get<Component>(entityID);
            }*/

            template<class Component>
            bool has(common::EntityID entityID) noexcept {
                bool result{false};
                {
                    //std::lock_guard<std::mutex> registryLock(mMutex);
                    result = mRegistry->has<Component>(entityID);
                }
                return result;
            }

            template<class Component, class... Args>
            void replace(common::EntityID entityID, Args &&... args) {
                {
                    //std::lock_guard<std::mutex> registryLock(mMutex);
                    mRegistry->replace<Component>(entityID, std::forward<Args>(args)...);
                }
            }

            template<class Archive, class ...ArchiveComponents>
            void restore(Archive &archive) {
                {
                    //std::lock_guard<std::mutex> registryLock(mMutex);
                    mRegistry->restore().entities(archive).template component<ArchiveComponents...>(archive);
                }
            }

            template<class Archive, class ...ArchiveComponents>
            void snapshot(Archive &archive) {
                {
                    //std::lock_guard<std::mutex> registryLock(mMutex);
                    mRegistry->snapshot().entities(archive).template component<ArchiveComponents...>(archive);
                }
            }

            std::unique_lock<std::mutex> scopedLock() {
                return std::unique_lock<std::mutex>(mMutex);
            }

            void lock() {
                mLock.lock();
            }

            void unlock() {
                mLock.unlock();
            }

        private:
            std::unique_ptr<entt::Registry<EntityType>> mRegistry{};
            std::mutex mMutex{};
            std::unique_lock<std::mutex> mLock{};
        };

    }
}
