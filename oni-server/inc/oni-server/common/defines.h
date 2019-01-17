#pragma once

namespace oni {
    namespace server {
        const oni::common::real32 MAJOR_LEVEL_DELTA = 0.1f;
        const oni::common::real32 MINOR_LEVEL_DELTA = 0.001f;

        const oni::common::real32 LEVEL_0 = 0.0f;
        const oni::common::real32 LEVEL_1 = LEVEL_0 + MAJOR_LEVEL_DELTA;
        const oni::common::real32 LEVEL_2 = LEVEL_1 + MAJOR_LEVEL_DELTA;
        const oni::common::real32 LEVEL_3 = LEVEL_2 + MAJOR_LEVEL_DELTA;
        const oni::common::real32 LEVEL_4 = LEVEL_3 + MAJOR_LEVEL_DELTA;
        const oni::common::real32 LEVEL_5 = LEVEL_4 + MAJOR_LEVEL_DELTA;
        const oni::common::real32 LEVEL_6 = LEVEL_5 + MAJOR_LEVEL_DELTA;
        const oni::common::real32 LEVEL_7 = LEVEL_6 + MAJOR_LEVEL_DELTA;
        const oni::common::real32 LEVEL_8 = LEVEL_7 + MAJOR_LEVEL_DELTA;
        const oni::common::real32 LEVEL_9 = LEVEL_8 + MAJOR_LEVEL_DELTA;
    }
}