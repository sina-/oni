#pragma once

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    class ClientDataManager;
    class EntityManager;

    struct DeletedEntity;
    struct EntityOperationPolicy;

    enum class EntityType : u16;
    enum class SimMode : u8;
    enum class SnapshotType;
}