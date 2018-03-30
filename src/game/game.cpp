#include <oni-core/game/game.h>
#include <oni-core/io/input.h>
#include <thread>
#include <oni-core/utils/oni-assert.h>

namespace oni {
    namespace game {

        Game::Game()
                : mRunTimer(), mRunLag(0.0f), mRunCounter(0), mFrameLag(0.0f), mFrameTimer(), mTickTimer(),
                  mTickLag(0) {}

        void Game::run() {
            mRunTimer.restart();
            mFrameTimer.restart();

            if (0.1f - mRunLag <= ep) {
                auto fps = mRunCounter / mRunLag;
                auto tps = 1 * mTickCounter;
                showFPS(static_cast<unsigned short>(fps));
                showTPS(static_cast<unsigned short>(tps));

                mRunLag = 0;
                mTickLag = 0;
                mRunCounter = 0;
                mTickCounter = 0;
                mFrameCounter = 0;
            }

            tick();

            render();

            mRunCounter++;
            mFrameCounter++;
            mRunLag += mRunTimer.elapsed();
            mFrameLag += mFrameTimer.elapsed();
        }

        void Game::tick() {
            while (mFrameLag > mTickMS) {
                mTickTimer.restart();

/*                auto correction = std::max((float) mTickLag, mTickMS);
                correction = std::min(mMinTickMS, correction);
                _tick(correction, keyPressed);*/

                _tick(mTickMS);
                mTickLag = mTickTimer.elapsed();

                // If it takes longer than tick frequency to run the simulations, the game will die.
                //ONI_DEBUG_ASSERT(mTickLag - mTickMS <= ep);

                mFrameLag -= mTickMS;
                mTickCounter++;
            }
        }

        void Game::render() {
            // Add lag
            //std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 20));
            _render();
        }

    }
}