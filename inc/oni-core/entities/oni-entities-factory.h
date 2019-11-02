#pragma once

#include <functional>
#include <unordered_map>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/entities/oni-entities-structure.h>
#include <oni-core/util/oni-util-hash.h>
#include <oni-core/util/oni-util-structure.h>

namespace cereal {
    class JSONInputArchive;
}

namespace oni {
    // TODO: Can I remove JSONInputArchive from the API?
    using ComponentFactory = std::function<
            void(EntityManager &,
                 EntityID,
                 cereal::JSONInputArchive &)>;

    class EntityFactory {
    public:
        explicit EntityFactory(EntityDefDirPath &&);

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
        createEntity_Canon(EntityManager &,
                           const EntityName &);

        void
        createEntityExtras(EntityManager &mainEm,
                           EntityManager &supportEm,
                           EntityID,
                           const EntityName &);

    protected:
        const EntityDefDirPath &
        _getEntityPath_Canon(const EntityName &name);

        const EntityDefDirPath &
        _getEntityPath_Extra(const EntityName &name);

        virtual void
        _postProcess(EntityManager &,
                     EntityID);

    protected:
        EntityDefDirPath mEntityResourcePath{};

    private:
        std::unordered_map<Hash, ComponentFactory> mComponentFactory{};
        std::unordered_map<EntityName, EntityDefDirPath> mEntityPathMap_Canon{};
        std::unordered_map<EntityName, EntityDefDirPath> mEntityPathMap_Extra{};
    };
}