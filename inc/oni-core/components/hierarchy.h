#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/components/geometry.h>
#include <oni-core/math/mat4.h>

namespace oni {
    namespace components {
        struct TransformParent {
            common::EntityID parent{};
            // TODO: This has to be vector of all the parents transformation matrices.
            // As it is we can not have grand-parents and such. One way to solve the
            // problem is to have a map of parentEntity-to-transform and when a parent
            // transform is updated we only propagate that down the hierarchy.
            math::mat4 transform{math::mat4::identity()};

            template<class Archive>
            void serialize(Archive &archive) {
                archive(parent, transform);
            }
        };

        struct TransformChildren {
            std::vector<common::EntityID> children{};
        };
    }
}
