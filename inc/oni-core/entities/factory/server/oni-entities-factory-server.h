#pragma once

#include <oni-core/entities/oni-entities-factory.h>

namespace oni {
    class EntityFactory_Server : public EntityFactory {
    public:
        explicit EntityFactory_Server(ZLayerManager&);

    protected:
        void
        _postProcess(EntityManager &manager,
                     EntityID id) override;
    };
}