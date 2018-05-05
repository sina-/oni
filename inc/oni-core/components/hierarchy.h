#pragma once

#include <oni-core/entities/create-entity.h>
#include <oni-core/components/geometry.h>
#include <oni-core/math/mat4.h>

namespace oni {
    namespace components {
        struct TransformParent {
            entities::entityID parent{};
            // TODO: This has to be vector of all the parents transformation matrices.
            math::mat4 transform{math::mat4::identity()};
        };

        struct TransformChildren {
            std::vector<entities::entityID> children{};
        };
    }
}
