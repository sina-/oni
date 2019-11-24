#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <entt/entt.hpp>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-tag.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-structure.h>
#include <oni-core/entities/oni-entities-view.h>
#include <oni-core/entities/oni-entities-group.h>
#include <oni-core/math/oni-math-fwd.h>
#include <oni-core/game/oni-game-event.h>
#include <oni-core/math/oni-math-function.h>
#include <oni-core/game/oni-game-event-rate-limiter.h>
#include <oni-core/physics/oni-physics-fwd.h>
#include <oni-core/entities/oni-entities-structure.h>


namespace oni {
    class EntityManager {
    public:
        EntityManager(SimMode sMode,
                      Physics *);

        ~EntityManager();

        EntityManager(const EntityManager &) = delete;

        EntityManager
        operator=(const EntityManager &) const = delete;

    public:
        // TODO: These functions shouldn't be here
        void
        setWorldP3D(EntityID,
                    r32 x,
                    r32 y,
                    r32 z);

        void
        setWorldP3D(EntityID,
                    r32 x,
                    r32 y);

        void
        setScale(EntityID,
                 r32 x,
                 r32 y);

        void
        setColor(EntityID,
                 r32 red,
                 r32 green,
                 r32 blue,
                 r32 alpha);

        void
        setRandTTL(EntityID id,
                   r32 lower,
                   r32 upper);

        void
        setRandVelocity(EntityID id,
                        u32 lower,
                        u32 upper);

        r32
        setRandOrientation(EntityID);

        r32
        setRandOrientation(EntityID,
                           r32 lower,
                           r32 upper);

        void
        setDirectionFromOrientation(EntityID);

        void
        setRandDirection(EntityID,
                         r32 lowerX,
                         r32 lowerY,
                         r32 upperX,
                         r32 upperY);

        void
        setOrientation(EntityID,
                       r32 ornt);

        void
        createPhysics(EntityID);

        void
        setText(EntityID,
                std::string_view content);

        Rand *
        getRand();

        SimMode
        getSimMode();

    public:
        void
        markForDeletion(EntityID);

        void
        flushDeletions(const EntityOperationPolicy &);

        void
        flushDeletions();

        void
        deleteEntity(EntityID);

        void
        deleteEntity(EntityID,
                     const EntityOperationPolicy &);

        static void
        attach(const EntityContext &parent,
               const EntityContext &child);

        static void
        bindLifetime(const EntityContext &parent,
                     const EntityContext &child);

    public:
        oni::size
        size();

        oni::size
        alive();

        template<class... ViewComponents>
        EntityView<EntityID, ViewComponents...>
        createView() {
            return EntityView<EntityID, ViewComponents...>(*mRegistry);
        }

        template<class... ViewComponents>
        EntityView<EntityID, ViewComponents...>
        createView() const {
            return EntityView<EntityID, ViewComponents...>(*mRegistry);
        }

        template<class... ViewComponents>
        EntityView<EntityID, ViewComponents...>
        createViewWithLock() {
//            std::unique_lock<std::mutex> registryLock(mMutex);
//            return EntityView<EntityID, ViewComponents...>(*mRegistry, std::move(registryLock));
        }

        template<class... GroupComponents>
        EntityGroup<EntityID, GroupComponents...>
        createGroup() {
            return EntityGroup<EntityID, GroupComponents...>(*mRegistry);
        }

        template<class... GroupComponents>
        EntityGroup<EntityID, GroupComponents...>
        createGroupWithLock() {
//            std::unique_lock<std::mutex> registryLock(mMutex);
//            return EntityView<EntityID, GroupComponents...>(*mRegistry, std::move(registryLock));
        }

        template<class Component>
        Component &
        get(EntityID entityID) noexcept {
            return mRegistry->get<Component>(entityID);
        }

        template<class Component>
        const Component &
        get(EntityID entityID) const noexcept {
            return mRegistry->get<Component>(entityID);
        }

        EntityID
        map(EntityID entityID);

        template<class Component>
        bool
        has(EntityID entityID) const noexcept {
            bool result{false};
            result = mRegistry->has<Component>(entityID);
            return result;
        }

        template<class Component, class... Args>
        void
        replace(EntityID entityID,
                Args &&... args) {
            mRegistry->replace<Component>(entityID, std::forward<Args>(args)...);
        }

        template<class Archive, class... ArchiveComponents, class... Type, class... Member>
        void
        restore(SnapshotType snapshotType,
                Archive &archive,
                Member Type::*... member) {
            switch (snapshotType) {
                case SnapshotType::ENTIRE_REGISTRY: {
                    mLoader->entities(archive).template component<ArchiveComponents...>(false, archive, member...);
                    break;
                }
                case SnapshotType::ONLY_COMPONENTS: {
                    mLoader->template component<ArchiveComponents...>(true, archive, member...);
                    break;
                }
                case SnapshotType::ONLY_NEW_ENTITIES: {
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
                 SnapshotType snapshotType) {
            switch (snapshotType) {
                case SnapshotType::ONLY_COMPONENTS: {
                    {
                        auto view = mRegistry->view<Tag_NetworkSyncComponent>();
                        if (!view.empty()) {
                            mRegistry->snapshot().template component<ArchiveComponents...>(archive,
                                                                                           view.begin(),
                                                                                           view.end());
                            mRegistry->reset<Tag_NetworkSyncComponent>();
                        }
                    }
                    break;
                }
                case SnapshotType::ONLY_NEW_ENTITIES: {
                    {
                        auto view = mRegistry->view<Tag_NetworkSyncEntity>();
                        if (!view.empty()) {
                            mRegistry->snapshot().entities(archive).template component<ArchiveComponents...>(
                                    archive,
                                    view.begin(),
                                    view.end());
                            mRegistry->reset<Tag_NetworkSyncEntity>();
                        }
                    }
                    break;
                }
                case SnapshotType::ENTIRE_REGISTRY: {
                    mRegistry->snapshot().entities(archive).template component<ArchiveComponents...>(archive);
                    break;
                }
                default: {
                    assert(false);
                }
            }
        }

//        std::unique_lock<std::mutex>
//        scopedLock() {
//            return std::unique_lock<std::mutex>(mMutex);
//        }

        void
        clearDeletedEntitiesList();

        const std::vector<DeletedEntity> &
        getDeletedEntities() const;

        void
        markForNetSync(EntityID entity);

        template<class Component, class Comparator>
        void
        sort(Comparator comparator) {
            mRegistry->sort<Component, Comparator>(std::move(comparator));
        }

        template<class Event, auto Method, class MethodInstance>
        void
        registerEventHandler(MethodInstance *instance,
                             EventDispatcherType type) {
            auto idx = enumCast(type);
            mDispatcher[idx]->sink<Event>().template connect<Method>(instance);
        }

        template<class Event, auto Method>
        void
        registerEventHandler(EventDispatcherType type) {
            auto idx = enumCast(type);
            mDispatcher[idx]->sink<Event>().template connect<Method>();
        }

        template<class Event>
        void
        rateLimitEvent(EventDispatcherType type,
                       const CoolDown &cd) {
            mEventRateLimiter->registerEventCD<Event>(type, cd);
        }

        template<class Event, class... Args>
        void
        enqueueEvent(Args &&...args) {
            // TODO: Inefficient, as not all the dispatchers are interested in all event types! But it works for now.
            for (auto i = 0; i < NumEventDispatcher; ++i) {
                auto type = EventDispatcherType(i);
                if (mEventRateLimiter->canFire<Event>(type)) {
                    mDispatcher[i]->enqueue<Event>(std::forward<Args>(args)...);
                }
            }
        }

        template<class Event>
        void
        enqueueEvent() {
            for (auto i = 0; i < NumEventDispatcher; ++i) {
                mDispatcher[i]->enqueue<Event>();
            }
        }

        void
        dispatchEvents(EventDispatcherType type);

        void
        dispatchEventsAndFlush(EventDispatcherType type);

        template<class ...Component, class Func>
        void
        update(Func f,
               duration32 d) {
            auto view = createView<Component...>();
            auto context = EntityTickContext{*this, 0, d};

            for (auto &&id: view) {
                context.id = id;
                f(context, view.template get<Component>(id)...);
            }
        }

    public:
        EntityID
        createEntity();

        EntityID
        createEntity(const EntityName &);

        template<class Component, class... Args>
        Component &
        createComponent(EntityID entityID,
                        Args &&... args) {
            static_assert(std::is_aggregate_v<Component> || std::is_enum_v<Component>);
            return mRegistry->assign<Component>(entityID, std::forward<Args>(args)...);
        }

        template<class Component>
        void
        removeComponent(EntityID entityID) {
            mRegistry->remove<Component>(entityID);
        }

        template<class Component>
        auto
        componentID() {
            return mRegistry->type<Component>();
        }

        template<class Component>
        void
        removeComponentSafe(EntityID entityID) {
            if (mRegistry->has<Component>(entityID)) {
                mRegistry->remove<Component>(entityID);
            }
        }

        template<class Tag>
        void
        assignTag(EntityID id) {
            mRegistry->assign<Tag>(id);
        }

        void
        printEntityType(EntityID id) const;

        static void
        printEntityType(const b2Body *);

        bool
        valid(EntityID entityID);

        static constexpr inline auto
        nullEntity() {
            return entt::null;
        }

    private:
        void
        removePhysicalBody(EntityID);

        template<class Component, class... Args>
        void
        accommodate(EntityID entityID,
                    Args &&... args) {
            mRegistry->assign_or_replace<Component>(entityID, std::forward<Args>(args)...);
        }

        void
        destroy(EntityID entityID);

        template<class... Component>
        void
        destroy() {
            mRegistry->reset<Component...>();
        }

        void
        destroyAndTrack(EntityID entityID);

    public:
        // TODO: Yeah this doesn't belong into this class and it is just a big mess.
        Physics *mPhysics;

    private:
        std::unique_ptr<entt::basic_registry<EntityID>> mRegistry{};
        std::unique_ptr<entt::basic_continuous_loader<EntityID>> mLoader{};
        // TODO: There is no need to bind this to registry.
        // TODO: Do I even need to use entt for this?
        std::array<std::unique_ptr<entt::dispatcher>, NumEventDispatcher> mDispatcher{};
//        mutable std::mutex mMutex{};

    private:
        std::unique_ptr<Rand> mRand{};

        SimMode mSimMode{SimMode::CLIENT};
        EntityOperationPolicy mEntityOperationPolicy{};

        std::unordered_set<EntityID> mEntitiesToDelete{};
        std::vector<DeletedEntity> mDeletedEntities{};
        std::unique_ptr<EventRateLimiter> mEventRateLimiter;
    };
}
