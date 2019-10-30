#include <oni-core/entities/factory/server/oni-entities-factory-server.h>

#include <oni-core/entities/oni-entities-manager.h>

namespace oni {
    oni::EntityFactory_Server::EntityFactory_Server(EntityDefDirPath &&fp) : EntityFactory(std::move(fp)) {
        fp.descendInto("server");
    }

    void
    EntityFactory_Server::_postProcess(EntityManager &manager,
                                       EntityID id) {
        if (manager.has<PhysicalProperties>(id)) {
            manager.createPhysics(id);
        }
    }
}