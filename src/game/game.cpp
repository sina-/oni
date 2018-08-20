#include <oni-core/game/game.h>

#include <thread>
#include <iostream>

#include <oni-core/common/consts.h>

namespace oni {
    namespace game {

        Game::Game() = default;

        Game::Game(common::uint8 tickRate, common::uint8 pollRate) :
                mTickMS(1.0f / tickRate),
                mPollMS(1.0f / pollRate) {
        }

        Game::~Game() = default;

        void Game::run() {
            std::thread pollThread(&Game::poll, this);
            std::thread tickThread(&Game::tick, this);
            std::thread renderThread(&Game::render, this);

            while (!mShouldTerminate) {
                if (shouldTerminate()) {
                    mShouldTerminate = true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            pollThread.join();
            tickThread.join();
            renderThread.join();
        }

        void Game::tick() {
            utils::HighResolutionTimer tickTimer{};
            utils::HighResolutionTimer updateTimer{};
            common::real64 lagAccumulator{0.0f};
            common::uint16 updateCounter{0};

            while (!mShouldTerminate) {
                tickTimer.restart();
                updateTimer.restart();

                if (1.0f - lagAccumulator <= common::ep) {
                    auto tps = updateCounter / lagAccumulator;
                    showTPS(static_cast<common::int16>(tps));

                    lagAccumulator = 0.0f;
                    updateCounter = 0;
                }

                _tick(mTickMS);

                auto lag = tickTimer.elapsed();
                auto excess = mTickMS - lag;

                if (excess > 0.0f) {
                    auto sleepFor = static_cast<common::uint64>(excess * 1000);
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
                } else {
                    std::cout << "Couldn't sleep during tick()\n";
                }

                ++updateCounter;
                lagAccumulator += updateTimer.elapsed();
            }
        }

        void Game::poll() {
            utils::HighResolutionTimer pollTimer{};
            utils::HighResolutionTimer updateTimer{};
            common::real64 lagAccumulator{0.0f};
            common::uint16 updateCounter{0};

            while (!mShouldTerminate) {
                pollTimer.restart();
                updateTimer.restart();

                if (1.0f - lagAccumulator <= common::ep) {
                    auto pps = updateCounter / lagAccumulator;
                    // TODO: Send it to GUI
                    std::cout << "Polling " << pps << " per second.\n";

                    lagAccumulator = 0.0f;
                    updateCounter = 0;
                }

                _poll();

                auto lag = pollTimer.elapsed();
                auto excess = mTickMS - lag;

                if (excess > 0.0f) {
                    auto sleepFor = static_cast<common::uint64>(excess * 1000);
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
                } else {
                    std::cout << "Couldn't sleep during poll()\n";
                }

                ++updateCounter;
                lagAccumulator += updateTimer.elapsed();
            }
        }

        void Game::render() {
            utils::HighResolutionTimer renderTimer{};
            utils::HighResolutionTimer updateTimer{};
            common::real64 lagAccumulator{0.0f};
            common::uint16 updateCounter{0};

            initRenderer();

            while (!mShouldTerminate) {
                renderTimer.restart();
                updateTimer.restart();

                if (1.0f - lagAccumulator <= common::ep) {
                    auto fps = updateCounter / lagAccumulator;
                    showFPS(static_cast<common::int16>(fps));

                    lagAccumulator = 0.0f;
                    updateCounter = 0;
                }

                _render();

                auto lag = renderTimer.elapsed();
                auto excess = mTickMS - lag;

                if (excess > 0.0f) {
                    auto sleepFor = static_cast<common::uint64>(excess * 1000);
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
                } else {
                    std::cout << "Couldn't sleep during render()\n";
                }

                // NOTE: Only display the result at the end after mTickMS amount of time has passed.
                display();

                ++updateCounter;
                lagAccumulator += updateTimer.elapsed();
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