#pragma once

#include <oni-core/game/oni-game.h>
#include <oni-core/asset/oni-asset-fwd.h>
#include <oni-core/graphic/oni-graphic-fwd.h>
#include <oni-core/io/oni-io-fwd.h>
#include <oni-core/math/oni-math-fwd.h>

namespace oni {
    class ParticleEditorGame : public Game {
    public:
        ParticleEditorGame();

        ~ParticleEditorGame() override;

    protected:
        bool
        shouldTerminate() override;

        void
        _sim(r64 simTime) override;

        void
        _render(r64 simTime) override;

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
        oni::AssetManager *mAssetManager{};
        oni::SceneManager *mSceneManager{};
        oni::TextureManager *mTextureManager{};
        oni::ZLayerManager *mZLayerManager{};
        oni::Input *mInput{};
        oni::Window *mWindow{};

        bool mWindowReady{false};
    };
}