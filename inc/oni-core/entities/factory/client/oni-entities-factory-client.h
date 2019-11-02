#pragma once

#include <oni-core/entities/oni-entities-factory.h>

#include <oni-core/graphic/oni-graphic-fwd.h>


namespace oni {
    class EntityFactory_Client : public EntityFactory {
    public:
        EntityFactory_Client(EntityDefDirPath &&,
                             FontManager &,
                             TextureManager &);

    private:
        void
        _postProcess(EntityManager &,
                     EntityID) override;

    private:
        FontManager &mFontMng;
        TextureManager &mTextureMng;
    };
}