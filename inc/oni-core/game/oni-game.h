#pragma once

#include <atomic>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/util/oni-util-timer.h>

namespace oni {
    namespace game {
        class Game {
        public:
            Game();

            Game(common::u16 tickRate);

            virtual ~Game();

            virtual void
            run();

        protected:
            common::r32
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
            _sim(common::r64 simTime) = 0;

            virtual void
            _render(common::r64 simTime) = 0;

            virtual void
            _display() = 0;

            virtual void
            _poll() = 0;

            virtual void
            _finish() = 0;

            virtual void
            showFPS(common::i16) = 0;

            virtual void
            showSPS(common::i16) = 0;

            virtual void
            showPPS(common::i16) = 0;

            virtual void
            showRT(common::i16) = 0;

            virtual void
            showST(common::i16) = 0;

            virtual void
            showPT(common::i16) = 0;

        private:
            // 60Hz
            const common::r32 mTickS{1 / 60.0f};

        protected:
            utils::Timer mSimTimer{};
            utils::Timer mSimLoopTimer{};
            common::u16 mSimUpdateCounter{0};
            common::r64 mSimMS{0.0f};

            utils::Timer mPollTimer{};
            utils::Timer mPollLoopTimer{};
            common::u16 mPollUpdateCounter{0};
            common::r64 mPollMS{0.0f};

            utils::Timer mRenderTimer{};
            utils::Timer mRenderLoopTimer{};
            common::u16 mRenderUpdateCounter{0};
            common::r64 mRenderMS{0.0f};

            utils::Timer mGameLoopTimer{};
        };
    }
}
