#pragma once

#include <string>
#include <functional>
#include <map>

#include <entt/core/hashed_string.hpp>

#include <oni-core/entities/oni-entities-entity.h>
#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/fwd.h>
#include <oni-core/util/oni-util-hash.h>

namespace cereal {
    class JSONInputArchive;
}

namespace oni {
    // TODO: don't I have something useful in IO? If not, should add one.
    struct FilePath {
        std::string value;
    };

    struct InvalidComponent {
    };

    struct Component_Name {
        //std::string value;
        HashedString value;
    };

    struct EntityType_Name {
        HashedString value;
    };

    struct EntityTypeMap {
        EntityType id{};
        EntityType_Name name{};
    };

    // TODO: Can I remove JSONInputArchive from the API?
    using ComponentFactory = std::function<
            void(EntityManager &,
                 EntityID,
                 cereal::JSONInputArchive &)>;

    class EntityFactory {
    public:
        explicit EntityFactory(FilePath);

        void
        registerEntityType(const EntityType_Name&,
                           EntityType);

        void
        registerComponentFactory(const Component_Name &,
                                 ComponentFactory &&);

        EntityID
        createEntity(EntityManager &,
                     const EntityType_Name &);

    private:
        EntityType
        getEntityType(const EntityType_Name &);

        const FilePath &
        getEntityResourcePath(const EntityType_Name &);

    private:
        std::map<Hash, ComponentFactory> mComponentFactory{};
        // TODO: This should move to entity registry. We already have the concept of entity debug name which maps
        // entity name string to an ID. This is the same thing almost.
        std::map<Hash, EntityType> mEntityNameLookup{};
        std::map<Hash, FilePath> mEntityResourcePathLookup{};
        FilePath mEntityResourcePath{};
    };
}