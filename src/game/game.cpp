#include <oni-core/game/game.h>

#include <thread>
#include <iostream>

#include <oni-core/utils/timer.h>
#include <oni-core/common/consts.h>

namespace oni {
    namespace game {

        Game::Game() = default;

        Game::Game(common::uint8 simRate, common::uint8 pollRate, common::uint8 renderRate) :
                mSimMS(1.0f / simRate),
                mPollMS(1.0f / pollRate),
                mRenderMS(1.0f / renderRate) {
        }

        Game::~Game() = default;

        void Game::run() {
            std::thread pollThread(&Game::poll, this);
            std::thread simThread(&Game::sim, this);
            std::thread renderThread(&Game::render, this);

            while (!mShouldTerminate) {
                if (shouldTerminate()) {
                    mShouldTerminate = true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            pollThread.join();
            simThread.join();
            renderThread.join();
        }

        void Game::sim() {
            utils::HighResolutionTimer simTimer{};
            utils::HighResolutionTimer loopTimer{};
            common::uint16 updateCounter{0};
            common::real64 excessPerSecond{0.0f};

            while (!mShouldTerminate) {
                auto elapsed = loopTimer.elapsed();
                if (elapsed >= 1.0f) {
                    auto sps = updateCounter / elapsed;
                    auto set = excessPerSecond / elapsed;
                    showSPS(static_cast<common::int16>(sps));
                    showSET(static_cast<common::int16>(set));

                    loopTimer.restart();
                    updateCounter = 0;
                    excessPerSecond = 0.0f;
                }

                simTimer.restart();

                _sim(mSimMS);

                auto simDuration = simTimer.elapsed();
                auto excess = mSimMS - simDuration;
                excess *= 1000;

                if (excess > 0.0f) {
                    auto sleepFor = static_cast<common::int64>(excess);
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
                } else {
                    std::cout << "Couldn't sleep during sim(). Deficit: " << excess << "ms\n";
                }

                excessPerSecond += excess;
                ++updateCounter;
            }
        }

        void Game::poll() {
            utils::HighResolutionTimer pollTimer{};
            utils::HighResolutionTimer loopTimer{};
            common::uint16 updateCounter{0};
            common::real64 excessPerSecond{0.0f};

            while (!mShouldTerminate) {
                auto elapsed = loopTimer.elapsed();
                if (elapsed >= 1.0f) {
                    auto pps = updateCounter / elapsed;
                    auto pet = excessPerSecond / elapsed;
                    showPPS(static_cast<common::int16>(pps));
                    showPET(static_cast<common::int16>(pet));

                    loopTimer.restart();
                    updateCounter = 0;
                    excessPerSecond = 0.0f;
                }

                pollTimer.restart();

                _poll();

                auto pollDuration = pollTimer.elapsed();
                auto excess = mPollMS - pollDuration;
                excess *= 1000;

                if (excess >= 0.0f) {
                    auto sleepFor = static_cast<common::int64>(excess);
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
                } else {
                    std::cout << "Couldn't sleep during poll(). Deficit: " << excess << "ms\n";
                }

                excessPerSecond += excess;
                ++updateCounter;
            }
        }

        void Game::render() {
            utils::HighResolutionTimer renderTimer{};
            utils::HighResolutionTimer loopTimer{};
            common::uint16 updateCounter{0};
            common::real64 excessPerSecond{0.0f};

            initRenderer();

            while (!mShouldTerminate) {
                auto elapsed = loopTimer.elapsed();
                if (elapsed >= 1.0f) {
                    auto fps = updateCounter / elapsed;
                    auto ret = excessPerSecond / elapsed;
                    showFPS(static_cast<common::int16>(fps));
                    showRET(static_cast<common::int16>(ret));

                    loopTimer.restart();
                    updateCounter = 0;
                    excessPerSecond = 0.0f;
                }

                renderTimer.restart();

                _render();

                auto renderDuration = renderTimer.elapsed();
                auto excess = mRenderMS - renderDuration;
                excess *= 1000;

                if (excess >= 0.0f) {
                    auto sleepFor = static_cast<common::int64>(excess);
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
                } else {
                    std::cout << "Couldn't sleep during render(). Deficit: " << excess << "ms\n";
                }

                // NOTE: Only display the result at the end after mSimMS amount of time has passed.
                display();

                excessPerSecond += excess;
                ++updateCounter;
            }
        }

        void Game::display() {
            _display();
        }

        common::real32 Game::getTickFrequency() {
            return mSimMS;
        }

        void Game::initRenderer() {
        }

    }
}