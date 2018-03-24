#include <game/game.h>

namespace oni {
    namespace game {

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