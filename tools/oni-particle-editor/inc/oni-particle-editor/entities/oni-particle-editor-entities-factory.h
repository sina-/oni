#pragma once

#include <oni-core/entities/factory/client/oni-entities-factory-client.h>

namespace oni {
    class EntityFactory_ParticleEditor : public EntityFactory_Client {
    public:
        EntityFactory_ParticleEditor(FontManager &,
                                     TextureManager &,
                                     ZLayerManager &);
    };
}