#pragma once

#include <oni-core/utils/timer.h>

namespace oni {
    namespace game {
        class Game {
        public:
            Game();

            virtual ~Game() = default;

            /**
             * Main loop should run this function as fast as possible.
             */
            virtual void run();

            /**
             * Condition under which the game should terminate. Usually happens if user
             * exists the game.
             * @return
             */
            virtual bool shouldTerminate() = 0;

        protected:
            virtual void tick() final;

            virtual void render() final;

            virtual void _tick(const float tickTime) = 0;

            virtual void _render() = 0;

            virtual void showFPS(unsigned short fps) = 0;

            virtual void showTPS(unsigned short tps) = 0;

            /**
             * Accumulated time in ms over 1 second spent sleeping due to excess.
             * @param fet
             */
            virtual void showFET(short fet) = 0;

        protected:
            utils::HighResolutionTimer mRunTimerA;
            utils::HighResolutionTimer mRunTimerB;
            utils::HighResolutionTimer mFrameTimer;
            utils::HighResolutionTimer mTickTimer;

            double mRunLagAccumulator;
            double mRunLag;
            double mFrameLag;
            double mTickLag;
            double mFrameExcessTime;
            unsigned short mRunCounter;
            unsigned short mTickCounter = 0;
            unsigned short mFrameCounter = 0;

            // 60Hz
            const float mTickMS = 1 / 60.0f;
            // 30Hz
            const float mMinTickMS = 1 / 30.0f;

            const float ep = 0.00001f;
        };
    }
}
