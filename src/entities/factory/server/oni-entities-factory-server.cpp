#include <oni-core/entities/factory/server/oni-entities-factory-server.h>

#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/entities/oni-entities-serialization-json.h>
#include <oni-core/json/oni-json.h>
#include <oni-core/math/oni-math-z-layer-manager.h>

namespace oni {
    oni::EntityFactory_Server::EntityFactory_Server(ZLayerManager &zLayerMng) : EntityFactory(zLayerMng) {
        COMPONENT_FACTORY_DEFINE(this, oni, Acceleration)
        COMPONENT_FACTORY_DEFINE(this, oni, Car)
        COMPONENT_FACTORY_DEFINE(this, oni, CarConfig)
        COMPONENT_FACTORY_DEFINE(this, oni, CarInput)
        COMPONENT_FACTORY_DEFINE(this, oni, Direction)
        COMPONENT_FACTORY_DEFINE(this, oni, GrowOverTime)
        COMPONENT_FACTORY_DEFINE(this, oni, Orientation)
        COMPONENT_FACTORY_DEFINE(this, oni, Scale)
        COMPONENT_FACTORY_DEFINE(this, oni, Velocity)
        COMPONENT_FACTORY_DEFINE(this, oni, WorldP2D)
        COMPONENT_FACTORY_DEFINE(this, oni, WorldP3D)
        COMPONENT_FACTORY_DEFINE(this, oni, ZLayer)
    }

    void
    EntityFactory_Server::_postProcess(EntityManager &manager,
                                       EntityID id) {
        if (manager.has<PhysicalProperties>(id)) {
            manager.createPhysics(id);
        }

        if (manager.has<Car>(id)) {
            if (manager.has<CarConfig>(id)) {
                const auto &config = manager.get<CarConfig>(id);
                auto &car = manager.get<Car>(id);
                car.applyConfiguration(config);
            } else {
                assert(false);
            }

        }

        EntityFactory::_postProcess(manager, id);
    }
}