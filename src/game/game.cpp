#include <oni-core/game/game.h>
#include <oni-core/utils/io.h>
#include <thread>
#include <oni-core/utils/oni-assert.h>

namespace oni {
    namespace game {

        Game::Game()
                : mUpdateTimer(), mUpdateLag(0.0f), mCycles(0), mFrameLag(0.0f), mFrameTimer(), mTickTimer(),
                  mTickLag(0) {}

        void Game::run() {
            mUpdateTimer.restart();

            if (mUpdateLag > 0.5f) {
                auto fps = mCycles / mUpdateLag;
                showFPS(static_cast<unsigned short>(fps));

                mUpdateLag = 0;
                mCycles = 0;
            }

            // TODO: Create an input class that can be polled for input status.
            tick(getKey());

            mFrameTimer.restart();
            render();
            mFrameLag += mFrameTimer.elapsed();

            mCycles++;
            mUpdateLag += mUpdateTimer.elapsed();
        }

        void Game::tick(int keyPressed) {
            while (mFrameLag >= mTickMS) {

                /*
                auto correction = (float) std::max(mTickLag, mTickMS);
                correction = std::min((float)mMinTickMS, correction);
                _tick(fix, keyPressed);
                */

                mTickTimer.restart();
                _tick(mTickMS, keyPressed);
                mTickLag = mTickTimer.elapsed();

                // If it takes longer than tick frequency to run the simulations movements will slow down.
                ONI_DEBUG_ASSERT(mTickLag <= mTickMS);

                mFrameLag -= mTickMS;
            }
        }

        void Game::render() {
            // Add lag
            //std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 2));
            _render();
        }

    }
}