#include <oni-core/entities/oni-entities-serialization.h>

#include <cereal/archives/portable_binary.hpp>

#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/component/oni-component-physics.h>


namespace oni {
    void
    deserialize(EntityManager &manager,
                const std::string &data,
                SnapshotType snapshotType) {

    }
}