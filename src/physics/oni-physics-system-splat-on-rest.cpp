#include <oni-core/physics/oni-physics-system.h>

#include <Box2D/Dynamics/b2Body.h>

namespace oni {
    System_SplatOnRest::System_SplatOnRest(EntityManager &em) : SystemTemplate(em) {
        assert(em.getSimMode() == SimMode::CLIENT);
    }

    void
    System_SplatOnRest::update(EntityTickContext &etc,
                               SplatOnRest &sor,
                               PhysicalBody &body,
                               Scale &scale,
                               WorldP3D &pos,
                               Orientation &ornt) {
        if (!body.value->IsAwake()) {
            auto callback = [&etc]() {
                etc.mng.deleteEntity(etc.id);
            };
            // TODO: This will create copy for all. Good place for profiling and optimization as these entities
            // are often particles
            etc.mng.enqueueEvent<Event_SplatOnRest>(pos, scale, ornt, sor.md, std::move(callback));
        }

    }

    void
    System_SplatOnRest::postUpdate(EntityManager &mng,
                                   duration32 dt) {}
}