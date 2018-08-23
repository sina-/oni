#include <oni-core/game/game.h>

#include <thread>
#include <iostream>

#include <oni-core/common/consts.h>

namespace oni {
    namespace game {

        Game::Game() = default;

        Game::Game(common::uint8 simRate, common::uint8 pollRate, common::uint8 renderRate) :
                mTickMS(1.0f / simRate),
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
            utils::HighResolutionTimer updateTimer{};
            common::uint16 updateCounter{0};

            while (!mShouldTerminate) {
                auto elapsed = updateTimer.elapsed();
                if (elapsed >= 1.0f) {
                    auto tps = updateCounter / elapsed;
                    showTPS(static_cast<common::int16>(tps));

                    updateTimer.restart();
                    updateCounter = 0;
                }

                simTimer.restart();

                _sim(mTickMS);

                auto simDuration = simTimer.elapsed();
                auto excess = mTickMS - simDuration;

                if (excess > 0.0f) {
                    auto sleepFor = static_cast<common::uint64>(excess * 1000);
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
                } else {
                    std::cout << "Couldn't sleep during sim()\n";
                }

                ++updateCounter;
            }
        }

        void Game::poll() {
            utils::HighResolutionTimer pollTimer{};
            utils::HighResolutionTimer updateTimer{};
            common::uint16 updateCounter{0};

            while (!mShouldTerminate) {
                auto elapsed = updateTimer.elapsed();
                if (elapsed >= 1.0f) {
                    auto pps = updateCounter / elapsed;
                    // TODO: Send it to GUI
                    std::cout << "Polling " << pps << " per second.\n";

                    updateTimer.restart();
                    updateCounter = 0;
                }

                pollTimer.restart();

                _poll();

                auto pollDuration = pollTimer.elapsed();
                auto excess = mPollMS - pollDuration;

                if (excess >= 0.0f) {
                    auto sleepFor = static_cast<common::uint64>(excess * 1000);
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
                } else {
                    std::cout << "Couldn't sleep during poll()\n";
                }

                ++updateCounter;
            }
        }

        void Game::render() {
            utils::HighResolutionTimer renderTimer{};
            utils::HighResolutionTimer updateTimer{};
            common::uint16 updateCounter{0};

            initRenderer();

            while (!mShouldTerminate) {
                auto elapsed = updateTimer.elapsed();
                if (elapsed >= 1.0f) {
                    auto fps = updateCounter / elapsed;
                    showFPS(static_cast<common::int16>(fps));

                    updateTimer.elapsed();
                    updateCounter = 0;
                }

                renderTimer.restart();

                _render();

                auto renderDuration = renderTimer.elapsed();
                auto excess = mRenderMS - renderDuration;

                if (excess >= 0.0f) {
                    auto sleepFor = static_cast<common::uint64>(excess * 1000);
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
                } else {
                    std::cout << "Couldn't sleep during render()\n";
                }

                // NOTE: Only display the result at the end after mTickMS amount of time has passed.
                display();

                ++updateCounter;
            }
        }

        void Game::display() {
            _display();
        }

        common::real32 Game::getTickFrequency() {
            return mTickMS;
        }

        void Game::initRenderer() {
        }

    }
}