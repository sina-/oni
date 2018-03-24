#pragma once

#include <utils/timer.h>

namespace oni {
    namespace game {
        class Game {
        public:
            Game() = default;

            virtual ~Game() = 0;

            virtual void update();

            virtual void tick();

            virtual void render();

        protected:
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
