#include <oni-particle-editor/game/oni-particle-editor-game.h>

#include <AntTweakBar.h>
#include <entt/entt.hpp>

#include <oni-core/asset/oni-asset-manager.h>
#include <oni-core/common/oni-common-const.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/entities/oni-entities-serialization.h>
#include <oni-core/game/oni-game-event.h>
#include <oni-core/graphic/oni-graphic-brush.h>
#include <oni-core/graphic/oni-graphic-debug-draw-box2d.h>
#include <oni-core/graphic/oni-graphic-font-manager.h>
#include <oni-core/graphic/oni-graphic-scene-manager.h>
#include <oni-core/graphic/oni-graphic-system.h>
#include <oni-core/graphic/oni-graphic-texture-manager.h>
#include <oni-core/graphic/oni-graphic-window.h>
#include <oni-core/math/oni-math-rand.h>
#include <oni-core/math/oni-math-transformation.h>
#include <oni-core/math/oni-math-z-layer-manager.h>
#include <oni-core/physics/oni-physics-system.h>
#include <oni-core/physics/oni-physics.h>


namespace oni {
    ParticleEditorGame::ParticleEditorGame() {
        mAssetMng = new oni::AssetManager();
        mTextureMng = new oni::TextureManager(*mAssetMng);
        mInput = new oni::Input();
        mZLayerMng = new oni::ZLayerManager();
        mPhysics = new oni::Physics();
        mEntityMng = new oni::EntityManager(SimMode::CLIENT, mPhysics);
    }

    ParticleEditorGame::~ParticleEditorGame() {
        TwTerminate();
    }

    void
    ParticleEditorGame::initRenderer() {
        const r32 TILE_SIZE = 1.0f; // It is a assumed that 1 game unit is 1 meter for physics simulation
        const u8 TILE_COUNT_FOR_WIDTH = 16;
        const u8 TILE_COUNT_FOR_HEIGHT = 9;
        const r32 ASPECT_RATIO =
                TILE_COUNT_FOR_WIDTH / static_cast<oni::r32> (TILE_COUNT_FOR_HEIGHT);
        const r32 GAME_WIDTH = TILE_COUNT_FOR_WIDTH * TILE_SIZE;
        const r32 GAME_HEIGHT = GAME_WIDTH / ASPECT_RATIO;
        const r32 HALF_GAME_WIDTH =
                GAME_WIDTH * 0.5f;
        const r32 HALF_GAME_HEIGHT = GAME_HEIGHT * 0.5f;

        const u16 WINDOW_WIDTH = 1600;
        const u16 WINDOW_HEIGHT = 900;

        auto screenXMin = -HALF_GAME_WIDTH;
        auto screenYMin = -HALF_GAME_HEIGHT;

        auto screenXMax = HALF_GAME_WIDTH;
        auto screenYMax = HALF_GAME_HEIGHT;

        auto screenBounds = ScreenBounds{screenXMin, screenXMax, screenYMin, screenYMax};

        // NOTE: any call to GLFW functions will fail with Segfault if GLFW is uninitialized
        // (initialization happens in Window). Also any call to GLFW functions will Segfault if they are
        // called from any thread other than one where glfwInit() is called.
        mWindow = new oni::Window(*mInput, "Oni Particle Editor", WINDOW_WIDTH, WINDOW_HEIGHT);
        // NOTE: It requires OpenGL to be loaded to be able to load the texture atlas.
        mSceneMng = new oni::SceneManager(screenBounds,
                                          *mAssetMng,
                                          *mZLayerMng,
                                          *mTextureMng);
        setupTweakBar();
        mTextureMng->loadAssets();

        mWindow->setClear({});

        mWindowReady = true;
    }

    void
    ParticleEditorGame::initSystems() {
        mSystems.push_back(new oni::System_GrowOverTime(*mEntityMng));
        mSystems.push_back(new oni::System_MaterialTransition(*mEntityMng));
//        mSystems.push_back(
//                new rac::System_ParticleEmitter(*mEntityMng, *mEntityMng, *mSceneManager, *mEntityLoader));
//        mSystems.push_back(new rac::System_PositionAndVelocity(*mEntityMng));
        mSystems.push_back(new oni::System_TimeToLive(*mEntityMng));
        mSystems.push_back(new oni::System_SyncPos(*mEntityMng));
    }

    void
    ParticleEditorGame::setupTweakBar() {
        TwInit(TW_OPENGL_CORE, nullptr);
        TwWindowSize(mWindow->getWidth(), mWindow->getHeight());

        auto particleBar = TwNewBar("ParticleBar");
        TwDefine(" ParticleBar label='Particle' refresh=0.15 ");
        TwDefine(" ParticleBar valueswidth=100 ");

        TwStructMember vec2Members[] = {
                {"X", TW_TYPE_FLOAT, offsetof(vec2, x), " Step=1 "},
                {"Y", TW_TYPE_FLOAT, offsetof(vec2, y), " Step=1 "},
        };
        TwType TwVec2 = TwDefineStruct("VEC2", vec2Members, 2, sizeof(vec2), 0, 0);

        {
            TwAddVarRO(particleBar, "screen pos", TwVec2, &mInforSideBar.mouseScreenPos, " label='screen pos' ");
            TwAddVarRO(particleBar, "world pos", TwVec2, &mInforSideBar.mouseWorldPos, " label='world pos' ");
        }
    }

    bool
    ParticleEditorGame::shouldTerminate() {
        return mWindowReady && mWindow->closed();
    }

    void
    ParticleEditorGame::_sim(r64 dt) {
        for (auto &&system: mSystems) {
            system->tick(dt);
        }
    }

    void
    ParticleEditorGame::_render(r64 dt) {
        mWindow->clear();

        mSceneMng->submit(*mEntityMng);
        mSceneMng->render();

        TwDraw();
    }

    void
    ParticleEditorGame::_display() {
        mWindow->display();
    }

    void
    ParticleEditorGame::_poll() {
        mWindow->tick(*mInput);

        for (auto &&pos: mInput->getCursor()) {
            mInforSideBar.mouseScreenPos.x = pos.x;
            mInforSideBar.mouseScreenPos.y = pos.y;
        }
    }

    void
    ParticleEditorGame::_finish() {
        mInput->reset();
    }
}
