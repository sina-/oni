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

        class EntityFactory;

        class EntityManager {
        public:
            friend EntityFactory;

            EntityManager();

            ~EntityManager();

            EntityManager(const EntityManager &) = delete;

            EntityManager
            operator=(const EntityManager &) const = delete;

            common::EntityID
            createComplementTo(common::EntityID id);

            common::EntityID
            getComplementOf(common::EntityID id);

            bool
            hasComplement(common::EntityID id);

            size_t
            size() noexcept;

            /*
            template<class Component>
            size_t
            size() noexcept {
                auto result = mRegistry->size<Component>();
                return result;
            }
             */

            template<class... ViewComponents>
            EntityView<common::EntityID, ViewComponents...>
            createView() {
                return EntityView<common::EntityID, ViewComponents...>(*mRegistry);
            }

            template<class... ViewComponents>
            EntityView<common::EntityID, ViewComponents...>
            createViewWithLock() {
                std::unique_lock<std::mutex> registryLock(mMutex);
                return EntityView<common::EntityID, ViewComponents...>(*mRegistry, std::move(registryLock));
            }

            template<class... GroupComponents>
            EntityGroup<common::EntityID, GroupComponents...>
            createGroup() {
                return EntityGroup<common::EntityID, GroupComponents...>(*mRegistry);
            }

            template<class... GroupComponents>
            EntityGroup<common::EntityID, GroupComponents...>
            createGroupWithLock() {
                std::unique_lock<std::mutex> registryLock(mMutex);
                return EntityView<common::EntityID, GroupComponents...>(*mRegistry, std::move(registryLock));
            }

            template<class Component>
            Component &
            get(common::EntityID entityID) noexcept {
                return mRegistry->get<Component>(entityID);
            }

            common::EntityID
            map(common::EntityID entityID);

            template<class Component>
            bool
            has(common::EntityID entityID) noexcept {
                bool result{false};
                result = mRegistry->has<Component>(entityID);
                return result;
            }

            template<class Component, class... Args>
            void
            replace(common::EntityID entityID,
                    Args &&... args) {
                mRegistry->replace<Component>(entityID, std::forward<Args>(args)...);
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
            clearDeletedEntitiesList();

            const std::vector<common::EntityID> &
            getDeletedEntities() const;

            void
            tagForComponentSync(common::EntityID entity);

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
            dispatchEvents();

            template<class Component, class... Args>
            Component &
            assign(common::EntityID entityID,
                   Args &&... args) {
                return mRegistry->assign<Component>(entityID, std::forward<Args>(args)...);
            }

        private:
            common::EntityID
            create();

            template<class Component>
            void
            remove(common::EntityID entityID) {
                mRegistry->remove<Component>(entityID);
            }

            template<class Component, class... Args>
            void
            accommodate(common::EntityID entityID,
                        Args &&... args) {
                mRegistry->assign_or_replace<Component>(entityID, std::forward<Args>(args)...);
            }

            void
            destroy(common::EntityID entityID);

            template<class... Component>
            void
            destroy() {
                mRegistry->reset<Component...>();
            }

            void
            destroyAndTrack(common::EntityID entityID);

            bool
            valid(common::EntityID entityID);

        private:
            std::unique_ptr<entt::basic_registry<common::EntityID>> mRegistry{};
            std::unique_ptr<entt::basic_continuous_loader<common::EntityID>> mLoader{};
            std::unique_ptr<entt::dispatcher> mDispatcher{};
            mutable std::mutex mMutex{};

            // NOTE: Entities which are complement of entities in another registry. Used for creating components on
            // client side with matching entity from server side. For example a car that is emiting smoke would have
            // an emitter component attached to the car entity of the server only on client side.
            std::unordered_map<common::EntityID, common::EntityID> mComplementaryEntities{};

            std::vector<common::EntityID> mDeletedEntities{};
        };

    }
}
