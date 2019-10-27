#pragma once

#include <vector>

#include <oni-core/game/oni-game.h>
#include <oni-core/fwd.h>

#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-visual.h>

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
        // TODO: How about struct EntityPreset { EntityType type; const char* name;}? This reduce name duplication
        enum EntityPreset : u16 {
            PARTICLE_EMITTER,

            LAST
        };

        struct InfoSideBar {
            Screen2D mouseScreenPos{};
            WorldP2D mouseWorldPos{};
            bool createModeOn{true};
            EntityPreset entityPreset{EntityPreset::PARTICLE_EMITTER};
        };

        struct ParticleConfig {
            Direction dir{};
            Orientation ornt{};
            Scale scale{};
            GrowOverTime got{};
            TimeToLive ttl{};
            Velocity vel{};
            Acceleration acc{};
            Material_Finish mft{};
            EntityAssetsPack eap{};
        };

        InfoSideBar mInforSideBar{};
        ParticleConfig mCurrentParticleConfig{};

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
        oni::EntityFactory *mEntityFactory{};

        bool mWindowReady{false};
    };
}