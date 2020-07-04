#include <oni-particle-editor/game/oni-particle-editor-game.h>

#include <AntTweakBar.h>
#include <entt/entt.hpp>

#include <oni-core/asset/oni-asset-manager.h>
#include <oni-core/common/oni-common-const.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/entities/factory/client/oni-entities-factory-client.h>
#include <oni-core/entities/oni-entities-serialization-network.h>
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

#include <oni-particle-editor/entities/oni-particle-editor-entities-structure.h>
#include <oni-particle-editor/entities/oni-particle-editor-entities-factory.h>

namespace {
    oni::EntityID
    findHotEntity(const oni::WorldP2D &mosPos) {
        return oni::EntityManager::nullEntity();
    }

    void
    createWindow(const oni::WorldP3D &mosPos) {
        printf("pos: %f, %f\n", mosPos.x, mosPos.y);
    }

    void
    updateLatestMosPos(
            const oni::SceneManager &sceneManager,
            const oni::Input &input,
            oni::WorldP3D &mosPos) {
        for (auto &&pos: input.getCursor()) {
            mosPos = sceneManager.unProject(pos);
            mosPos.z = 1.f;
        }
    }
}

namespace oni {
    ParticleEditorGame::ParticleEditorGame() {
        mAssetFilesIdx = new AssetFilesIndex({{"oni-resources/textures"}},
                                             {{"oni-resources/audio", "index", "wav"}});
        mTextureMng = new oni::TextureManager(*mAssetFilesIdx);
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

        // TODO:
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
                                          *mAssetFilesIdx,
                                          *mZLayerMng,
                                          *mTextureMng);
        // setupTweakBar();

        mTextureMng->cacheAllAssets();

        mWindow->setClear({});

        mFontMng = new oni::FontManager("oni-resources/fonts/FreeSans.ttf", 24,
                                        oni::r32(WINDOW_WIDTH) / GAME_WIDTH,
                                        oni::r32(WINDOW_HEIGHT) / GAME_HEIGHT);

        mEntityFactory = new EntityFactory_ParticleEditor(*mFontMng,
                                                          *mTextureMng,
                                                          *mZLayerMng);
        mEntityFactory->indexEntities({{"oni-resources/entities/client"}});

        mWindowReady = true;
    }

    void
    ParticleEditorGame::initSystems() {
        mSystems.push_back(new oni::System_GrowOverTime(*mEntityMng));
        mSystems.push_back(new oni::System_MaterialTransition(*mEntityMng));
        mSystems.push_back(new oni::System_ParticleEmitter(*mEntityMng, *mEntityMng, *mSceneMng, *mEntityFactory));
        mSystems.push_back(new oni::System_TimeToLive(*mEntityMng));
        mSystems.push_back(new oni::System_SyncPos(*mEntityMng));
        mSystems.push_back(new oni::System_ParticleEmitter(*mEntityMng, *mEntityMng, *mSceneMng, *mEntityFactory));
        mSystems.push_back(new oni::System_PositionAndVelocity(*mEntityMng));
    }

    void
    ParticleEditorGame::setupTweakBar() {
        TwInit(TW_OPENGL_CORE, nullptr);
        TwWindowSize(mWindow->getWidth(), mWindow->getHeight());

        mEntityBar = TwNewBar("ParticleBar");
        TwDefine(" ParticleBar label='Particle' ");
        TwDefine(" ParticleBar valueswidth=200 ");
        TwDefine(" ParticleBar refresh=0.15 ");
        TwDefine(" ParticleBar size='350 500' ");
        TwDefine(" ParticleBar contained=true ");

        auto adaptor = [](const Enum &mf) -> TwEnumVal {
            return {mf.id, mf.name.str.data()};
        };

        TwStructMember vec2Members[] = {
                {"X", TW_TYPE_FLOAT, offsetof(vec2, x), " Step=0.01 "},
                {"Y", TW_TYPE_FLOAT, offsetof(vec2, y), " Step=0.01 "},
        };
        auto TwCustomVec2 = TwDefineStruct("VEC2", vec2Members, 2, sizeof(vec2), nullptr, nullptr);

        auto TW_EntityName = TwDefineEnum("EntityName",
                                          EntityNameEditor::adapt<TwEnumVal>(adaptor),
                                          EntityNameEditor::size());

        TwStructMember GrowOverTime_TW_Members[] = {
                {"period",  TW_TYPE_FLOAT, offsetof(GrowOverTime, period)},
                {"elapsed", TW_TYPE_FLOAT, offsetof(GrowOverTime, elapsed)},
                {"factor",  TW_TYPE_FLOAT, offsetof(GrowOverTime, factor)},
                {"maxSize", TW_TYPE_DIR3F, offsetof(GrowOverTime, maxSize)}
        };
        auto TW_GrowOverTime = TwDefineStruct("GrowOverTime", GrowOverTime_TW_Members, 4, sizeof(GrowOverTime),
                                              nullptr, nullptr);
        auto TW_Material_Finish = TwDefineEnum("Material_Finish",
                                               Material_Finish::adapt<TwEnumVal>(adaptor),
                                               Material_Finish::size());

        TwStructMember Material_Definition_TW_Members[] = {
                {"finish", TW_Material_Finish, offsetof(Material_Definition, finish)},
        };
        auto TW_Material_Definition = TwDefineStruct("Material_Definition", Material_Definition_TW_Members, 1,
                                                     sizeof(Material_Definition),
                                                     nullptr, nullptr);

        TwAddVarRO(mEntityBar, "screen pos", TwCustomVec2, &mInfoSideBar.mouseScreenPos, " label='screen pos' ");
        TwAddVarRO(mEntityBar, "world pos", TwCustomVec2, &mInfoSideBar.mouseWorldPos, " label='world pos' ");
        TwAddVarRW(mEntityBar, "create entity", TW_TYPE_BOOL8, &mInfoSideBar.createModeOn,
                   " label='create entity' ");
        TwAddVarRW(mEntityBar, "save entity", TW_TYPE_BOOL8, &mInfoSideBar.save,
                   " label='save entity' ");
        TwAddVarRW(mEntityBar, "reset", TW_TYPE_BOOL8, &mInfoSideBar.reset, " label='reset' ");
        TwAddVarRO(mEntityBar, "particle count", TW_TYPE_UINT32, &mInfoSideBar.particleCount,
                   " label='particle count' ");
        TwAddVarRO(mEntityBar, "fps", TW_TYPE_UINT32, &mInfoSideBar.fps,
                   " label='fps' ");
        TwAddVarRW(mEntityBar, "entity name", TW_EntityName, &mInfoSideBar.entityName, " label='entity name' ");

//        TwAddVarRW(particleBar, "particle", TW_EntityName, &mParticleEmitter.particle, nullptr);
//        TwAddVarRW(particleBar, "orientMin", TW_TYPE_FLOAT, &mParticleEmitter.orientMin, nullptr);
//        TwAddVarRW(particleBar, "orientMax", TW_TYPE_FLOAT, &mParticleEmitter.orientMax, nullptr);
//        TwAddVarRW(particleBar, "size", TW_TYPE_DIR3F, &mParticleEmitter.size, nullptr);
//        TwAddVarRW(particleBar, "growth", TW_GrowOverTime, &mParticleEmitter.growth, nullptr);
//        TwAddVarRW(particleBar, "initialVMin", TW_TYPE_FLOAT, &mParticleEmitter.initialVMin, nullptr);
//        TwAddVarRW(particleBar, "initialVMax", TW_TYPE_FLOAT, &mParticleEmitter.initialVMax, nullptr);
//        TwAddVarRW(particleBar, "acc", TW_TYPE_FLOAT, &mParticleEmitter.acc, nullptr);
//        TwAddVarRW(particleBar, "material", TW_Material_Definition, &mParticleEmitter.material, nullptr);

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

#if 0
        auto view = mEntityMng->createView<EntityName>();
        mInforSideBar.particleCount = 0;
        for (auto &&id: view) {
            auto entityName = view.get<EntityName>(id);
            constexpr auto particle = EntityNameEditor::GET("simple-particle");
            if (entityName == particle) {
                ++mInforSideBar.particleCount;
            }
        }

        // NOTE: This is needed because AntTweakBar can only update the id field not the HashedString.
        mInforSideBar.entityName = EntityNameEditor::at(mInforSideBar.entityName.id);
#endif
    }

    void
    ParticleEditorGame::_render(r64 dt) {
        mWindow->clear();

        mSceneMng->submit(*mEntityMng);
        mSceneMng->render();

#if 0
        TwDraw();
#endif
    }

    void
    ParticleEditorGame::_display() {
        mWindow->display();
    }

    void
    ParticleEditorGame::_poll() {
        Window::tick(*mInput);
        updateLatestMosPos(*mSceneMng, *mInput, mLatestMosPos);

#if 0
        for (auto &&pos: mInput->getCursor()) {
            mInforSideBar.mouseScreenPos.x = pos.x;
            mInforSideBar.mouseScreenPos.y = pos.y;
        }

        auto mouseWorldP = mSceneMng->unProject(mInforSideBar.mouseScreenPos);
        mInforSideBar.mouseWorldPos.x = mouseWorldP.x;
        mInforSideBar.mouseWorldPos.y = mouseWorldP.y;
#endif

        EntityID hotEntity = findHotEntity(mInfoSideBar.mouseWorldPos);

        if (mInput->isMosButtonReleased() && mClickLimiter.tryFire()) {
            createWindow(mLatestMosPos);

            // TODO: Find a better way to specify editing area?
            if (mInfoSideBar.createModeOn && mInfoSideBar.mouseScreenPos.x > 450) {

                mEntityMng->reset();

                if (mInfoSideBar.reset) {
                } else {
                    // TODO: This is kinda about how the pattern goes, UI is just components with values.
                    // How to proceed from this? Notice that this has no dependency to JSON! Is that good? Or bad?
                    // Would it help if it did?
                    // Well, I probably need to write the part where this data is serialized from entity registry
                    // to json, that should help a bit decide how the intermediate stage of the entity looks like.
                    // EntityFactory probably should have a function that, just like its sibling, saveEntity()
                    // where I can give it an entity id and it dumps it into a json from which it can
                    // recreate it.
                    // Notice there is a slight difference between when an entity is created from json and when it is
                    // written to json from editor. When it is created json works as a template, values are defaults,
                    // but when it is written it is more specific than a generic template, some values are actual values
                    // that the game will use, but pos for example, still is a template. Unless I am editing a pre-saved
                    // entity.


                    // TODO: Editor is basically a smart-json editor. Depending on the type of entity selected editor
                    // will open the correct JSON and automatically give a UI where you can edit the values on
                    // the fly! and when done, save button will write back the data to disk. That's it.
                    hotEntity = mEntityFactory->loadEntity_Local(*mEntityMng, *mEntityMng, mInfoSideBar.entityName);
                    auto &pos = mEntityMng->get<WorldP3D>(hotEntity);
                    pos = mInfoSideBar.mouseWorldPos.to3D(0.5);

                    static int count = 0;
                    count++;
                    std::string name = (std::string("test") + std::to_string(count));
                    std::string label = std::string(" label='") + name + std::string("' ");
                    TwAddVarRO(mEntityBar, name.c_str(), TW_TYPE_UINT32,
                               &mInfoSideBar.fps, label.c_str());
                }
            } else if (mInfoSideBar.save && hotEntity != EntityManager::nullEntity()) {
                mEntityFactory->saveEntity_Local(*mEntityMng, hotEntity, mEntityMng->get<EntityNameEditor>(hotEntity));
            }
        }
    }

    void
    ParticleEditorGame::_finish() {
        mInput->reset();
    }

    void
    ParticleEditorGame::showFPS(i16 fps) {
        mInfoSideBar.fps = fps;
    }
}
