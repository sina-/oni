#pragma once

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    class ClientDataManager;
    class EntityManager;

    struct BindLifetimeParent;
    struct BindLifetimeChild;
    struct DeletedEntity;
    struct EntityOperationPolicy;
    struct EntityContext;
    struct EntityType;

    enum class SimMode : u8;
    enum class SnapshotType;
}