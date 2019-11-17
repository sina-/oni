#pragma once

#include <functional>
#include <unordered_map>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/math/oni-math-fwd.h>
#include <oni-core/util/oni-util-fwd.h>
#include <oni-core/util/oni-util-structure.h>
#include <oni-core/entities/oni-entities-structure.h>

namespace cereal {
    class JSONInputArchive;
}


namespace oni {
    // TODO: Can I remove JSONInputArchive from the API?
    using ComponentFactory = std::function<
            void(oni::EntityManager &,
                 oni::EntityID,
                 cereal::JSONInputArchive &)>;
}

#define COMPONENT_FACTORY_DEFINE(factory, COMPONENT_NAME)                               \
{                                                                                       \
        oni::ComponentFactory cf = [](oni::EntityManager &em,                           \
                                 oni::EntityID id,                                      \
                                 cereal::JSONInputArchive &reader) {                    \
            auto &component = em.createComponent<COMPONENT_NAME>(id);                   \
            reader(component);                                                          \
        };                                                                              \
        constexpr auto componentName = oni::ComponentName{#COMPONENT_NAME};             \
        factory->registerComponentFactory(componentName, std::move(cf));                \
}

namespace oni {
    class EntityFactory {
    public:
        explicit EntityFactory(EntityDefDirPath &&,
                               ZLayerManager &);

        // TODO: Similar to Assets, I should just pass an index file to retrieve all the entity types. Or even
        // better is to just set the top directory and let the factory traverse and index all the types??
        void
        registerEntityType_Canon(const EntityName &name);

        void
        registerEntityType_Extra(const EntityName &name);

        void
        registerComponentFactory(const ComponentName &,
                                 ComponentFactory &&);

        EntityID
        createEntity_Primary(EntityManager &primaryEm,
                             EntityManager &secondaryEm,
                             const EntityName &name);

        void
        createEntity_Secondary(EntityManager &primaryEm,
                               EntityManager &secondaryEm,
                               EntityID parentID,
                               const EntityName &name);

    protected:
        const EntityDefDirPath &
        _getEntityPath_Primary(const EntityName &name);

        const EntityDefDirPath &
        _getEntityPath_Secondary(const EntityName &name);

        virtual void
        _postProcess(EntityManager &,
                     EntityID);

    protected:
        EntityDefDirPath mEntityResourcePath{};

    private:
        std::unordered_map<Hash, ComponentFactory> mComponentFactory{};
        std::unordered_map<EntityName, EntityDefDirPath> mEntityPathMap_Canon{};
        std::unordered_map<EntityName, EntityDefDirPath> mEntityPathMap_Extra{};

        ZLayerManager &mZLayerManager;
    };
}