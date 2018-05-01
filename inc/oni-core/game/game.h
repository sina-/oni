#pragma once

#include <oni-core/utils/timer.h>

namespace oni {
    namespace game {
        class Game {
        public:
            Game();

            virtual ~Game();

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

        private:
            virtual void tick() final;

            virtual void render() final;

            virtual void display() final;

            virtual void _tick(const float tickTime) = 0;

            virtual void _render() = 0;

            virtual void _display() = 0;

            virtual void showFPS(unsigned short fps) = 0;

            virtual void showTPS(unsigned short tps) = 0;

            /**
             * Accumulated time in ms over 1 second spent sleeping due to excess.
             * @param fet
             */
            virtual void showFET(short fet) = 0;

        protected:
            utils::HighResolutionTimer mRunTimerA{};
            utils::HighResolutionTimer mRunTimerB{};
            utils::HighResolutionTimer mFrameTimer{};
            utils::HighResolutionTimer mTickTimer{};

            double mRunLagAccumulator{0.0f};
            double mRunLag{0.0f};
            double mFrameLag{0.0f};
            double mTickLag{0.0f};
            double mFrameExcessTime{0.0f};
            unsigned short mRunCounter{0};
            unsigned short mTickCounter{0};
            unsigned short mFrameCounter{0};

            // 60Hz
            const float mTickMS{1 / 60.0f};
            // 30Hz
            // const float mMinTickMS{1 / 30.0f};
        };
    }
}
