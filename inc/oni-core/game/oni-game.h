#pragma once

#include <atomic>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/util/oni-util-timer.h>

namespace oni {
    namespace game {
        class Game {
        public:
            Game();

            Game(common::uint16 tickRate);

            virtual ~Game();

            virtual void
            run();

        protected:
            common::real32
            getTickFrequency();

            virtual bool
            shouldTerminate() = 0;

            virtual void
            initRenderer();

            virtual void
            sim() final;

            virtual void
            render() final;

            virtual void
            display() final;

            virtual void
            poll() final;

            virtual void
            finish() final;

            virtual void
            _sim(common::real64 simTime) = 0;

            virtual void
            _render(common::real64 simTime) = 0;

            virtual void
            _display() = 0;

            virtual void
            _poll() = 0;

            virtual void
            _finish() = 0;

            virtual void
            showFPS(common::int16) = 0;

            virtual void
            showSPS(common::int16) = 0;

            virtual void
            showPPS(common::int16) = 0;

            virtual void
            showRT(common::int16) = 0;

            virtual void
            showST(common::int16) = 0;

            virtual void
            showPT(common::int16) = 0;

        private:
            // 60Hz
            const common::real32 mTickS{1 / 60.0f};

        protected:
            utils::Timer mSimTimer{};
            utils::Timer mSimLoopTimer{};
            common::uint16 mSimUpdateCounter{0};
            common::real64 mSimMS{0.0f};

            utils::Timer mPollTimer{};
            utils::Timer mPollLoopTimer{};
            common::uint16 mPollUpdateCounter{0};
            common::real64 mPollMS{0.0f};

            utils::Timer mRenderTimer{};
            utils::Timer mRenderLoopTimer{};
            common::uint16 mRenderUpdateCounter{0};
            common::real64 mRenderMS{0.0f};

            utils::Timer mGameLoopTimer{};
        };
    }
}
