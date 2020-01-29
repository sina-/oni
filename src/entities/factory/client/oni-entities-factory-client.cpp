#include <oni-core/entities/factory/client/oni-entities-factory-client.h>

#include <oni-core/graphic/oni-graphic-texture-manager.h>
#include <oni-core/graphic/oni-graphic-font-manager.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/util/oni-util-file.h>


namespace oni {
    EntityFactory_Client::EntityFactory_Client(FontManager &fm,
                                               TextureManager &tm,
                                               ZLayerManager &zm) : EntityFactory(zm),
                                                                    mFontMng(fm),
                                                                    mTextureMng(tm) {
    }

    void
    EntityFactory_Client::_postProcess(EntityManager &em,
                                       EntityID id) {
        if (em.has<Material_Definition>(id)) {
            auto &mf = em.get<Material_Definition>(id);
            mTextureMng.initTexture(mf.skin.texture);
        }
        if (em.has<Material_Text>(id)) {
            auto &mt = em.get<Material_Text>(id);
            mFontMng.initializeText(mt);
        }

        EntityFactory::_postProcess(em, id);
    }
}