#include <oni-core/game/oni-game.h>

#include <thread>
#include <iostream>

#include <oni-core/util/oni-util-timer.h>
#include <oni-core/common/oni-common-const.h>

namespace oni {
    namespace game {

        Game::Game() = default;

        Game::Game(common::u16 rate) :
                mTickS(1.f / rate) {
        }

        Game::~Game() = default;

        common::r32
        Game::getTickFrequency() {
            return mTickS;
        }

        void
        Game::initRenderer() {
        }

        void
        Game::run() {
            initRenderer();
            auto tickTime = std::chrono::microseconds(static_cast<common::u64>(mTickS * 1000 * 1000));

            while (!shouldTerminate()) {
                mGameLoopTimer.restart();
                poll();
                sim();
                render();
                finish();
                auto elapsed = mGameLoopTimer.elapsed<std::chrono::microseconds>();
                auto excess = tickTime - elapsed;
                std::this_thread::sleep_for(excess);

                // NOTE: Display at the end for consistent frame display rate
                display();
            }
        }

        void
        Game::sim() {
            auto elapsed = mSimLoopTimer.elapsedInSeconds();
            if (elapsed >= 1.0f) {
                auto sps = mSimUpdateCounter / elapsed;
                auto st = mSimMS / elapsed;
                showSPS(static_cast<common::i16>(sps));
                showST(static_cast<common::i16>(st * 1000));

                mSimLoopTimer.restart();
                mSimUpdateCounter = 0;
            }

            mSimTimer.restart();

            _sim(mTickS);

            mSimMS = mSimTimer.elapsedInSeconds();
            ++mSimUpdateCounter;
        }

        void
        Game::poll() {
            auto elapsed = mPollLoopTimer.elapsedInSeconds();
            if (elapsed >= 1.0f) {
                auto pps = mPollUpdateCounter / elapsed;
                auto pt = mPollMS / elapsed;
                showPPS(static_cast<common::i16>(pps));
                showPT(static_cast<common::i16>(pt * 1000));

                mPollLoopTimer.restart();
                mPollUpdateCounter = 0;
            }

            mPollTimer.restart();

            _poll();

            mPollMS = mPollTimer.elapsedInSeconds();
            ++mPollUpdateCounter;
        }

        void
        Game::render() {
            auto elapsed = mRenderLoopTimer.elapsedInSeconds();
            if (elapsed >= 1.0f) {
                auto fps = mRenderUpdateCounter / elapsed;
                auto rt = mRenderMS / elapsed;
                showFPS(static_cast<common::i16>(fps));
                showRT(static_cast<common::i16>(rt * 1000));

                mRenderLoopTimer.restart();
                mRenderUpdateCounter = 0;
            }

            mRenderTimer.restart();

            _render(mTickS);

            mRenderMS = mRenderTimer.elapsedInSeconds();
            ++mRenderUpdateCounter;
        }

        void
        Game::finish() {
            _finish();
        }

        void
        Game::display() {
            _display();
        }
    }
}