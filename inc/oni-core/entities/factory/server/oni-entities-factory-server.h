#pragma once

#include <oni-core/entities/oni-entities-factory.h>

namespace oni {
    class EntityFactory_Server : public EntityFactory {
    public:
        EntityFactory_Server(EntityDefDirPath &&);

    protected:
        void
        _postProcess(EntityManager &manager,
                     EntityID id) override;
    };
}