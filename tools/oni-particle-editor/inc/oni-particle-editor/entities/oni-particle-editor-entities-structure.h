#pragma once

#include <oni-core/util/oni-util-enum.h>

namespace oni {
    ONI_ENUM_DEF_WITH_BASE(EntityNameEditor, EntityName,
                           { 1, "simple-particle" },
                           { 2, "particle-emitter" })
}