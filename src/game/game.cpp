#include <oni-core/game/game.h>
#include <oni-core/utils/io.h>
#include <thread>

namespace oni {
    namespace game {

        Game::Game() : mTimer(), mFrameTime(0.0f), mCycles(0), mFrameLag(0.0f) {}

        void Game::run() {
            mFrameTime += mTimer.elapsed();
            mFrameLag += mTimer.elapsed();
            mTimer.restart();

            if (mFrameTime > 0.5f) {
                auto fps = mCycles / mFrameTime;
                showFPS(static_cast<unsigned short>(fps));

                mFrameTime = 0;
                mCycles = 0;
            }

            tick(getKey());

            render();

            // Add lag
            //std::this_thread::sleep_for(std::chrono::milliseconds(100));

            mCycles++;
        }

        void Game::tick(int keyPressed) {
            while (mFrameLag >= mTickTime) {
                _tick(mTickTime, keyPressed);
                mFrameLag -= mTickTime;
            }
        }

        void Game::render() {
            _render();
        }

    }
}