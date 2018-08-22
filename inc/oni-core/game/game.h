#pragma once

#include <atomic>

#include <oni-core/utils/timer.h>
#include <oni-core/common/typedefs.h>

namespace oni {
    namespace game {
        class Game {
        public:
            Game();

            Game(common::uint8 simRate, common::uint8 pollRate, common::uint8 renderRate);

            virtual ~Game();

            virtual void run();

        protected:
            common::real32 getTickFrequency();

            virtual bool shouldTerminate() = 0;

            virtual void initRenderer();

            virtual void sim() final;

            virtual void render() final;

            virtual void display() final;

            virtual void poll() final;

            virtual void _sim(common::real32 simTime) = 0;

            virtual void _render() = 0;

            virtual void _display() = 0;

            virtual void _poll() = 0;

            virtual void showFPS(common::int16 fps) = 0;

            virtual void showTPS(common::int16 tps) = 0;


            /**
             * Accumulated time in ms over 1 second spent sleeping due to excess.
             * @param fet
             */
            virtual void showFET(common::int16 fet) = 0;

        protected:
            // 60Hz
            const common::real32 mTickMS{1 / 60.0f};
            const common::real32 mRenderMS{1 / 120.0f};
            const common::real32 mPollMS{1 / 20.0f};
            // 30Hz
            // const common::real32 mMinTickMS{1 / 30.0f};

            std::atomic<bool> mShouldTerminate{false};
        };
    }
}
