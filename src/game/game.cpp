#include <oni-core/game/game.h>
#include <oni-core/utils/io.h>

namespace oni {
    namespace game {

        Game::Game() : mTickTime(0.0f), mTimer(), mFrameTime(0.0f), mCycles(0) {}

        void Game::update() {
            mTimer.restart();
            tick();
            mTickTime += mTimer.elapsed();

            mTimer.restart();
            render();
            mFrameTime += mTimer.elapsed();

            mCycles++;

            if (mFrameTime > 0.5f) {
                auto fps = mCycles / mFrameTime;
                showFPS(static_cast<unsigned short>(fps));
                mTimer.restart();

                printl(fps);

                mFrameTime = 0;
                mTickTime = 0;
                mCycles = 0;
            }
        }

        void Game::tick() {
            _tick();
        }

        void Game::render() {
            _render();
        }

    }
}