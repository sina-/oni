#include <oni-core/game/game.h>

#include <thread>
#include <iostream>

#include <oni-core/common/consts.h>
#include <oni-core/common/typedefs.h>

namespace oni {
    namespace game {

        Game::Game() = default;

        Game::Game(common::uint8 tickRate, common::uint8 pollRate) :
                mTickMS(1.0f / tickRate),
                mPollMS(1.0f / pollRate) {
        }

        Game::~Game() = default;

        void Game::run() {
            while (!shouldTerminate()) {
                mRunTimerA.restart();
                mRunTimerB.restart();
                mFrameTimer.restart();

                if (1.0f - mRunLagAccumulator <= common::ep) {
                    auto fps = mRunCounter / mRunLagAccumulator;
                    auto tps = 1 * mTickCounter;
                    showFPS(static_cast<common::uint16 >(fps));
                    showTPS(static_cast<common::uint16 >(tps));
                    showFET(static_cast<common::int16>(mFrameExcessTime * 1000));

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
                    auto sleepFor = static_cast<common::uint64>(excess * 1000);
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
                } else {
                    std::cout << "Couldn't sleep :(\n";
                }

                // NOTE: Only display the result at the end after mTickMS amount of time has passed.
                display();

                mRunCounter++;
                mFrameCounter++;
                mFrameLag += mFrameTimer.elapsed();
                mRunLagAccumulator += mRunTimerA.elapsed();
            }
        }

        void Game::tick() {
            //while (mFrameLag > mTickMS) {
            mTickTimer.restart();

/*                auto correction = std::max((common::real32) mTickLag, mTickMS);
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

        void Game::display() {
            _display();
        }

        common::real32 Game::getTickFrequency() {
            return mTickMS;
        }

        void Game::poll() {
            _poll();
        }

    }
}