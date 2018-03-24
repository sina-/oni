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
            virtual void update();

            /**
             * Condition under which the game should terminate. Usually happens if user
             * exists the game.
             * @return
             */
            virtual bool shouldTerminate() = 0;

        protected:
            virtual void tick();

            virtual void render();

            virtual void _tick() = 0;

            virtual void _render() = 0;

            virtual void showFPS(unsigned short fps) = 0;

        protected:
            utils::HighResolutionTimer mTimer;

            double mTickTime;
            double mFrameTime;
            unsigned int mCycles;
        };
    }
}
