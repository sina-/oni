#include <oni-core/entities/factory/client/oni-entities-factory-client.h>

#include <oni-core/graphic/oni-graphic-texture-manager.h>
#include <oni-core/graphic/oni-graphic-font-manager.h>
#include <oni-core/entities/oni-entities-manager.h>


namespace oni {
    EntityFactory_Client::EntityFactory_Client(EntityDefDirPath &&fp,
                                               FontManager &fm,
                                               TextureManager &tm,
                                               ZLayerManager &zm) : EntityFactory(std::move(fp), zm),
                                                                    mFontMng(fm),
                                                                    mTextureMng(tm) {
        mEntityResourcePath.descendInto("client");
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
        // TODO: There is a still a bug here, if there are no client side components to be added the
        // these won't be correctly initialized. I should move these to deserialize method of registry
        // after I also move all the entity initialization of client side components there as well.
        if (em.has<EntityAttachee>(id)) {
            auto &attachee = em.get<EntityAttachee>(id);
            attachee.mng = &em;
        }
        if (em.has<EntityAttachment>(id)) {
            auto &attachment = em.get<EntityAttachment>(id);
            for (auto &&e: attachment.entities) {
                attachment.mngs.emplace_back(&em);
            }
        }

        EntityFactory::_postProcess(em, id);
    }
}