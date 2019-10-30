#pragma once

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    class ClientDataManager;
    class EntityFactory;
    class EntityFactory_Client;
    class EntityFactory_Server;
    class EntityManager;

    struct BindLifetimeParent;
    struct BindLifetimeChild;
    struct DeletedEntity;
    struct EntityOperationPolicy;
    struct EntityContext;
    struct EntityName;
    struct Component_Name;

    enum class SimMode : u8;
    enum class SnapshotType;
}