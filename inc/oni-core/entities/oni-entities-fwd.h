#pragma once

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    class ClientDataManager;
    class EntityFactory;
    class EntityManager;

    struct BindLifetimeParent;
    struct BindLifetimeChild;
    struct DeletedEntity;
    struct EntityOperationPolicy;
    struct EntityContext;
    struct EntityType;
    struct EntityType_Name;
    struct Component_Name;

    enum class SimMode : u8;
    enum class SnapshotType;
}