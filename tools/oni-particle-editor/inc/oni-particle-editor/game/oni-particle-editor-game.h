#pragma once

#include <vector>

#include <oni-core/game/oni-game.h>
#include <oni-core/fwd.h>

#include <oni-core/component/oni-component-geometry.h>

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

    private:
        void
        setupTweakBar();

    private:
        struct InfoSideBar {
            Screen2D mouseScreenPos{};
            WorldP2D mouseWorldPos{};
        };

        InfoSideBar mInforSideBar{};

    private:
        oni::AssetManager *mAssetMng{};
        oni::EntityManager *mEntityMng{};
        oni::Input *mInput{};
        oni::Physics *mPhysics{};
        std::vector<oni::System *> mSystems{};
        oni::SceneManager *mSceneMng{};
        oni::TextureManager *mTextureMng{};
        oni::Window *mWindow{};
        oni::ZLayerManager *mZLayerMng{};

        bool mWindowReady{false};
    };
}