#pragma once

#include <oni-core/game/oni-game.h>
#include <oni-core/asset/oni-asset-fwd.h>
#include <oni-core/graphic/oni-graphic-fwd.h>
#include <oni-core/io/oni-io-fwd.h>
#include <oni-core/math/oni-math-fwd.h>

typedef struct CTwBar TwBar;

namespace oni {
    class ParticleEditorGame : public Game {
    public:
        ParticleEditorGame();

        virtual ~ParticleEditorGame();

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

        void
        showFPS(i16 i161) override;

        void
        showSPS(i16 i161) override;

        void
        showPPS(i16 i161) override;

        void
        showRT(i16 i161) override;

        void
        showST(i16 i161) override;

        void
        showPT(i16 i161) override;

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

        TwBar *mParticleDef{};

        bool mWindowReady{false};
    };
}