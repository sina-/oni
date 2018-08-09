#pragma once

#include <cstdint>
#include <chrono>

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace utils {
        class HighResolutionTimer {
        public:
            HighResolutionTimer() : mStartTime(now()) {}

            void restart() {
                mStartTime = now();
            }

            static common::uint64 now() {
                return common::uint64(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            }

            // Return elapsed time in seconds
            oni::common::real64 elapsed() const { return oni::common::real64(now() - mStartTime) * 1e-9; }

            common::uint64 elapsed_nanoseconds() const { return now() - mStartTime; }

        private:
            common::uint64 mStartTime{0};
        };
    }
}
