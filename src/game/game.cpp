#include <oni-core/game/game.h>

#include <thread>

#include <oni-core/common/consts.h>

namespace oni {
    namespace game {

        Game::Game() = default;

        Game::~Game() = default;

        void Game::run() {
            mRunTimerA.restart();
            mRunTimerB.restart();
            mFrameTimer.restart();

            if (1.0f - mRunLagAccumulator <= common::ep) {
                auto fps = mRunCounter / mRunLagAccumulator;
                auto tps = 1 * mTickCounter;
                showFPS(static_cast<unsigned short>(fps));
                showTPS(static_cast<unsigned short>(tps));
                showFET(static_cast<short>(mFrameExcessTime * 1000));

                mRunLagAccumulator = 0;
                mTickLag = 0;
                mRunCounter = 0;
                mTickCounter = 0;
                mFrameCounter = 0;
                mFrameExcessTime = 0;
            }

            tick();

            render();

            mRunLag = mRunTimerB.elapsed();

            auto excess = mTickMS - mRunLag;
            mFrameExcessTime += excess;

            if (excess > 0) {
                auto sleepFor = static_cast<int>(excess * 1000);
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
            }

            mRunCounter++;
            mFrameCounter++;
            mFrameLag += mFrameTimer.elapsed();
            mRunLagAccumulator += mRunTimerA.elapsed();
        }

        void Game::tick() {
            //while (mFrameLag > mTickMS) {
            mTickTimer.restart();

/*                auto correction = std::max((float) mTickLag, mTickMS);
                correction = std::min(mMinTickMS, correction);
                _tick(correction, keyPressed);*/

            _tick(mTickMS);
            mTickLag = mTickTimer.elapsed();

            // If it takes longer than tick frequency to run the simulations, the game will die.
            //ONI_DEBUG_ASSERT(mTickLag - mTickMS <= ep);

            //mFrameLag -= mTickMS;
            mTickCounter++;
            //}
        }

        void Game::render() {
            // Add lag
            //std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 20));
            _render();
        }

    }
}