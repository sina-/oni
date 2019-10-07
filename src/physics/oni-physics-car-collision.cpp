#include <oni-core/physics/oni-physics-system.h>

#include <Box2D/Dynamics/b2Body.h>

#include <oni-core/io/oni-io-input.h>
#include <oni-core/physics/oni-physics.h>

namespace oni {
    System_CarCollision::System_CarCollision(EntityManager &em) : SystemTemplate(em) {
        assert(em.getSimMode() == SimMode::SERVER);
    }

    void
    System_CarCollision::update(EntityTickContext &etc,
                                Car &car,
                                PhysicalBody &body,
                                CarInput &input,
                                Orientation &ornt,
                                WorldP3D &pos,
                                WorldP3D_History &hist) {

    }

    void
    System_CarCollision::postUpdate(EntityManager &mng,
                                    duration32 dt) {}
}