#include <oni-core/entities/factory/client/oni-entities-factory-client.h>

#include <oni-core/graphic/oni-graphic-texture-manager.h>
#include <oni-core/entities/oni-entities-manager.h>


namespace oni {
    EntityFactory_Client::EntityFactory_Client(EntityDefDirPath &&fp,
                                               TextureManager &tm) : EntityFactory(std::move(fp)), mTextureMng(tm) {
        mEntityResourcePath.descendInto("client");
    }

    void
    EntityFactory_Client::_postProcess(EntityManager &em,
                                       EntityID id) {
        if (em.has<Material_Definition>(id)) {
            auto &def = em.get<Material_Definition>(id);
            mTextureMng.initTexture(def.skin.texture);
        }
    }
}