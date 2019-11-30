#pragma once

#include <oni-core/util/oni-util-enum.h>

namespace oni {
    ONI_ENUM_DEF_WITH_BASE(EntityNameEditor, EntityName,
                           { 0, "particle-emitter" },
                           { 1, "simple-particle" })
}