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

    enum class EntityType : u16;
    enum class SimMode : u8;
    enum class SnapshotType;
}