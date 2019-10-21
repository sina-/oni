#pragma once

#include <string>
#include <functional>
#include <map>

#include <entt/core/hashed_string.hpp>

#include <oni-core/entities/oni-entities-entity.h>
#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/fwd.h>
#include <oni-core/util/oni-util-hash.h>
#include <oni-core/util/oni-util-file.h>

namespace cereal {
    class JSONInputArchive;
}

namespace oni {
    struct InvalidComponent {
    };

    struct Component_Name {
        //std::string value;
        HashedString value;
    };

    struct EntityType_Name {
        HashedString value;
    };

    struct EntityName {
        std::string_view value;
    };

    // TODO: Can I remove JSONInputArchive from the API?
    using ComponentFactory = std::function<
            void(EntityManager &,
                 EntityID,
                 cereal::JSONInputArchive &)>;

    class EntityFactory {
    public:
        explicit EntityFactory(FilePath, TextureManager&);

        void
        registerEntityType(const EntityType_Name &);

        void
        registerComponentFactory(const Component_Name &,
                                 ComponentFactory &&);

        EntityID
        createEntity(EntityManager &,
                     const EntityType_Name &);

    private:
        const FilePath &
        getEntityResourcePath(const EntityType_Name &);

        void
        postProcess(EntityManager &,
                    EntityID);

    private:
        std::map<Hash, ComponentFactory> mComponentFactory{};
        std::map<Hash, FilePath> mEntityResourcePathLookup{};
        FilePath mEntityResourcePath{};

        TextureManager &mTextureManager;
    };
}