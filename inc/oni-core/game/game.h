#pragma once

#include <atomic>

#include <oni-core/common/typedefs.h>
#include <oni-core/utils/timer.h>

namespace oni {
    namespace game {
        class Game {
        public:
            Game();

            Game(common::uint8 simRate,
                 common::uint8 pollRate,
                 common::uint8 renderRate);

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
            _sim(common::real64 simTime) = 0;

            virtual void
            _render(common::real64 simTime) = 0;

            virtual void
            _display() = 0;

            virtual void
            _poll() = 0;

            virtual void
            showFPS(common::int16 fps) = 0;

            virtual void
            showSPS(common::int16 tps) = 0;

            virtual void
            showPPS(common::int16 pps) = 0;

            virtual void
            showRET(common::int16 ret) = 0;

            virtual void
            showSET(common::int16 ret) = 0;

            virtual void
            showPET(common::int16 ret) = 0;

        protected:
            // 60Hz
            const common::real64 mSimS{1 / 60.0f};
            const common::real64 mRenderS{1 / 60.0f};
            const common::real64 mPollS{1 / 60.0f};
            // 30Hz
            const common::real32 mTickS{1 / 180.0f};

            std::atomic<bool> mShouldTerminate{false};

        protected:
            utils::Timer mSimTimer{};
            utils::Timer mSimLoopTimer{};
            common::uint16 mSimUpdateCounter{0};
            common::real64 mSimExcessPerSecond{0.0f};

            utils::Timer mPollTimer{};
            utils::Timer mPollLoopTimer{};
            common::uint16 mPollUpdateCounter{0};
            common::real64 mPollExcessPerSecond{0.0f};

            utils::Timer mRenderTimer{};
            utils::Timer mRenderLoopTimer{};
            common::uint16 mRenderUpdateCounter{0};
            common::real64 mRenderExcessPerSecond{0.0f};
        };
    }
}
