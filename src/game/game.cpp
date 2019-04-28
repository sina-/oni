#include <oni-core/game/game.h>

#include <thread>
#include <iostream>

#include <oni-core/utils/timer.h>
#include <oni-core/common/consts.h>

namespace oni {
    namespace game {

        Game::Game() = default;

        Game::Game(common::uint8 simRate,
                   common::uint8 pollRate,
                   common::uint8 renderRate) :
                mSimS(1.0f / simRate),
                mPollS(1.0f / pollRate),
                mRenderS(1.0f / renderRate) {
        }

        Game::~Game() = default;

        void
        Game::run() {
            initRenderer();

            while (!mShouldTerminate) {
                if (shouldTerminate()) {
                    mShouldTerminate = true;
                }
                sim();
                poll();
                render();
            }
        }

        void
        Game::sim() {
            auto elapsed = mSimLoopTimer.elapsedInSeconds();
            if (elapsed >= 1.0f) {
                auto sps = mSimUpdateCounter / elapsed;
                auto set = mSimExcessPerSecond / elapsed;
                showSPS(static_cast<common::int16>(sps));
                showSET(static_cast<common::int16>(set));

                mSimLoopTimer.restart();
                mSimUpdateCounter = 0;
                mSimExcessPerSecond = 0.0f;
            }

            mSimTimer.restart();

            _sim(mSimS);

            auto simDuration = mSimTimer.elapsedInSeconds();
            auto excess = mTickS - simDuration;
            excess *= 1000;

            if (excess > 0.0f) {
                auto sleepFor = static_cast<common::int64>(excess);
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
            } else {
                std::cout << "WARN: Couldn't sleep during sim(). Deficit: " << excess << "ms\n";
            }

            mSimExcessPerSecond += excess;
            ++mSimUpdateCounter;
        }

        void
        Game::poll() {
            auto elapsed = mPollLoopTimer.elapsedInSeconds();
            if (elapsed >= 1.0f) {
                auto pps = mPollUpdateCounter / elapsed;
                auto pet = mPollExcessPerSecond / elapsed;
                showPPS(static_cast<common::int16>(pps));
                showPET(static_cast<common::int16>(pet));

                mPollLoopTimer.restart();
                mPollUpdateCounter = 0;
                mPollExcessPerSecond = 0.0f;
            }

            mPollTimer.restart();

            _poll();

            auto pollDuration = mPollTimer.elapsedInSeconds();
            auto excess = mTickS - pollDuration;
            excess *= 1000;

            if (excess >= 0.0f) {
                auto sleepFor = static_cast<common::int64>(excess);
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
            } else {
                std::cout << "WARN: Couldn't sleep during poll(). Deficit: " << excess << "ms\n";
            }

            mPollExcessPerSecond += excess;
            ++mPollUpdateCounter;
        }

        void
        Game::render() {
            auto elapsed = mRenderLoopTimer.elapsedInSeconds();
            if (elapsed >= 1.0f) {
                auto fps = mRenderUpdateCounter / elapsed;
                auto ret = mRenderExcessPerSecond / elapsed;
                showFPS(static_cast<common::int16>(fps));
                showRET(static_cast<common::int16>(ret));

                mRenderLoopTimer.restart();
                mRenderUpdateCounter = 0;
                mRenderExcessPerSecond = 0.0f;
            }

            mRenderTimer.restart();

            _render(mSimS);

            auto renderDuration = mRenderTimer.elapsedInSeconds();
            auto excess = mTickS - renderDuration;
            excess *= 1000;

            if (excess >= 0.0f) {
                auto sleepFor = static_cast<common::int64>(excess);
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
            } else {
                std::cout << "WARN: Couldn't sleep during render(). Render time: " << renderDuration << ". Deficit: "
                          << excess << "ms\n";
            }

            // NOTE: Display the result at the end after mSimS amount of time has passed.
            display();

            mRenderExcessPerSecond += excess;
            ++mRenderUpdateCounter;
        }

        void
        Game::display() {
            _display();
        }

        common::real32
        Game::getTickFrequency() {
            return mSimS;
        }

        void
        Game::initRenderer() {
        }

    }
}