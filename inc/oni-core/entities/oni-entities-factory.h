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
    using ComponentFactoryMap = std::unordered_map<Hash, ComponentFactory>;

    template<class C>
    using ComponentReader = std::function<
            C(cereal::JSONInputArchive &)>;
}

#define COMPONENT_FACTORY_DEFINE(factory, NAMESPACE, COMPONENT_NAME)                    \
{                                                                                       \
        oni::ComponentFactory cf = [](oni::EntityManager &em,                           \
                                 oni::EntityID id,                                      \
                                 cereal::JSONInputArchive &reader) {                    \
            auto &component = em.createComponent<NAMESPACE::COMPONENT_NAME>(id);        \
            reader(component);                                                          \
        };                                                                              \
        constexpr auto componentName = oni::ComponentName{#COMPONENT_NAME};             \
        factory->registerComponentFactory(componentName, std::move(cf));                \
}

namespace oni {
    class EntityFactory {
    public:
        explicit EntityFactory(ZLayerManager &);

        void
        indexEntities(EntityDefDirPath &&fp);

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

        const ComponentFactoryMap &
        getFactoryMap() const;

    protected:
        const EntityDefDirPath &
        _getEntityPath_Primary(const EntityName &name);

        const EntityDefDirPath &
        _getEntityPath_Secondary(const EntityName &name);

        virtual void
        _postProcess(EntityManager &,
                     EntityID);

    private:
        std::unordered_map<Hash, ComponentFactory> mComponentFactory{};
        std::unordered_map<EntityName, EntityDefDirPath> mEntityPathMap_Primary{};
        std::unordered_map<EntityName, EntityDefDirPath> mEntityPathMap_Secondary{};

        ZLayerManager &mZLayerManager;
    };
}