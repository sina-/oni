#pragma once

#include <vector>

#include <oni-core/game/oni-game.h>
#include <oni-core/fwd.h>

#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-visual.h>

#include <oni-particle-editor/entities/oni-particle-editor-entities-fwd.h>
#include <oni-particle-editor/entities/oni-particle-editor-entities-structure.h>

namespace oni {
    class ParticleEditorGame : public Game {
    public:
        ParticleEditorGame();

        ~ParticleEditorGame() override;

    protected:
        bool
        shouldTerminate() override;

        void
        _sim(r64 dt) override;

        void
        _render(r64 dt) override;

        void
        _display() override;

        void
        _poll() override;

        void
        _finish() override;

    protected:
        void
        initRenderer() override;

        void
        initSystems() override;

        void
        showFPS(i16) override;

    private:
        void
        setupTweakBar();

    private:
        struct InfoSideBar {
            Screen2D mouseScreenPos{};
            WorldP2D mouseWorldPos{};
            bool createModeOn{true};
            bool save{false};
            bool reset{false};
            u32 particleCount{};
            u32 fps{};
            EntityNameEditor entityName{EntityNameEditor::GET("particle-emitter")};
        };

        InfoSideBar mInforSideBar{};

    private:
        oni::AssetFilesIndex *mAssetFilesIdx{};
        oni::EntityManager *mEntityMng{};
        oni::Input *mInput{};
        oni::Physics *mPhysics{};
        std::vector<oni::System *> mSystems{};
        oni::SceneManager *mSceneMng{};
        oni::TextureManager *mTextureMng{};
        oni::Window *mWindow{};
        oni::ZLayerManager *mZLayerMng{};
        oni::EntityFactory_ParticleEditor *mEntityFactory{};
        oni::FontManager *mFontMng{};

        bool mWindowReady{false};
    };
}