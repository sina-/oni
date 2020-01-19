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

    class JSONOutputArchive;
}


namespace oni {
    // TODO: Can I remove JSONInputArchive from the API?
    using ComponentReader = std::function<
            void(oni::EntityManager &,
                 oni::EntityID,
                 cereal::JSONInputArchive &)>;
    using ComponentReaderMap = std::unordered_map<Hash, ComponentReader>;

    using ComponentWriter = std::function<
            void(const oni::EntityManager &,
                 oni::EntityID,
                 cereal::JSONOutputArchive &)>;
    using ComponentWriterMap = std::unordered_map<Hash, ComponentWriter>;
}

#define COMPONENT_FACTORY_DEFINE(factory, NAMESPACE, COMPONENT_NAME)                    \
{                                                                                       \
        oni::ComponentReader cr = [](oni::EntityManager &em,                            \
                                 oni::EntityID id,                                      \
                                 cereal::JSONInputArchive &reader) {                    \
            auto &component = em.createComponent<NAMESPACE::COMPONENT_NAME>(id);        \
            reader(component);                                                          \
        };                                                                              \
        constexpr auto componentName = oni::ComponentName{#COMPONENT_NAME};             \
        factory->registerComponentReader(componentName, std::move(cr));                 \
                                                                                        \
        oni::ComponentWriter cw = [](const oni::EntityManager &em,                      \
                                 oni::EntityID id,                                      \
                                 cereal::JSONOutputArchive &writer) {                   \
            const auto &component = em.get<NAMESPACE::COMPONENT_NAME>(id);              \
            writer(#COMPONENT_NAME, component);                                         \
        };                                                                              \
        factory->registerComponentWriter(componentName, std::move(cw));                 \
}

namespace oni {
    class EntityFactory {
    public:
        explicit EntityFactory(ZLayerManager &);

        void
        indexEntities(EntityDefDirPath &&fp);

        void
        registerComponentReader(const ComponentName &,
                                ComponentReader &&);

        void
        registerComponentWriter(const ComponentName &,
                                ComponentWriter &&);

        EntityID
        readEntity_Local(EntityManager &primaryEm,
                         EntityManager &secondaryEm,
                         const EntityName &name);

        // TODO: Secondary entity could be client side components of a server entity, or it could be other entities
        // attached to primary entity. But they both go through the same interface and that is a bit confusing.
        // Editor might have even harder time separating these two
        void
        readEntity_Remote(EntityManager &primaryEm,
                          EntityManager &secondaryEm,
                          EntityID parentID,
                          const EntityName &name);

        void
        writeEntity_Local(const EntityManager &,
                          EntityID,
                          const EntityName &);

        const ComponentReaderMap &
        getFactoryMap() const;

    protected:
        const EntityDefDirPath &
        _getEntityPath(const EntityName &);

        virtual void
        _postProcess(EntityManager &,
                     EntityID);

    private:
        std::unordered_map<Hash, ComponentReader> mComponentReader{};
        std::unordered_map<Hash, ComponentWriter> mComponentWriter{};
        std::unordered_map<EntityName, EntityDefDirPath> mEntityPathMap{};

        ZLayerManager &mZLayerManager;
    };
}