#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <unordered_map>

#include <entt/entt.hpp>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-tag.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-entity.h>
#include <oni-core/entities/oni-entities-view.h>
#include <oni-core/entities/oni-entities-group.h>


namespace oni {
    namespace entities {
        typedef common::EntityID EntityIDType;

        class EntityFactory;

        class EntityManager {
        public:
            friend EntityFactory;

            EntityManager() {
                mRegistry = std::make_unique<entt::basic_registry<common::u32 >>();
                mLoader = std::make_unique<entt::basic_continuous_loader<EntityIDType>>(*mRegistry);
                mDispatcher = std::make_unique<entt::dispatcher>();
                //mLock = std::unique_lock<std::mutex>(mMutex, std::defer_lock);
            }

            ~EntityManager() = default;

            EntityManager(const EntityManager &) = delete;

            EntityManager
            operator=(const EntityManager &) const = delete;

            EntityIDType
            createComplementTo(EntityIDType id) {
                assert(mComplementaryEntities.find(id) == mComplementaryEntities.end());
                auto result = create();
                assign<entities::EntityType>(result, entities::EntityType::COMPLEMENT);
                mComplementaryEntities[id] = result;
                return result;
            }

            EntityIDType
            getComplementOf(EntityIDType id) {
                assert(mComplementaryEntities.find(id) != mComplementaryEntities.end());
                return mComplementaryEntities[id];
            }

            void
            complement(EntityIDType a,
                       EntityIDType b) {
                mComplementaryEntities[a] = b;
            }

            bool
            hasComplement(EntityIDType id) {
                return mComplementaryEntities.find(id) != mComplementaryEntities.end();
            }

            size_t
            size() noexcept {
                auto result = mRegistry->size();
                return result;
            }

            template<class Component>
            size_t
            size() noexcept {
                auto result = mRegistry->size<Component>();
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

            template<class Component, class... Args>
            Component &
            assign(EntityIDType entityID,
                   Args &&... args) {
                return mRegistry->assign<Component>(entityID, std::forward<Args>(args)...);
            }

        private:
            EntityIDType
            create() {
                auto result = mRegistry->create();
                return result;
            }

            template<class Component>
            void
            remove(EntityIDType entityID) {
                mRegistry->remove<Component>(entityID);
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

            // NOTE: Entities that are complement of entities in other registry. Used for creating components on
            // client side only that are specific for server side entities.
            std::unordered_map<common::EntityID, common::EntityID> mComplementaryEntities{};

            std::vector<EntityIDType> mDeletedEntities{};
        };
    }
}
