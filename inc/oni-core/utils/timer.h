#pragma once

#include <cstdint>
#include <chrono>

namespace oni {
    namespace utils {
        class HighResolutionTimer {
        public:
            HighResolutionTimer() : mStartTime(takeTimeStamp()) {}

            void restart() {
                mStartTime = takeTimeStamp();
            }

            double elapsed() const { return double(takeTimeStamp() - mStartTime) * 1e-9; }

            std::uint64_t elapsed_nanoseconds() const { return takeTimeStamp() - mStartTime; }

        protected:
            std::uint64_t takeTimeStamp() const {
                return std::uint64_t(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            }

        private:
            std::uint64_t mStartTime;
        };
    }
}
