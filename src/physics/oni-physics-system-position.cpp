#include <oni-core/physics/oni-physics-system.h>

#include <oni-core/entities/oni-entities-manager.h>

namespace oni {
    System_PositionAndVelocity::System_PositionAndVelocity(EntityManager &em) : SystemTemplate(em) {}

    void
    System_PositionAndVelocity::update(EntityTickContext &etc,
                                       Velocity &velocity,
                                       Acceleration &acc,
                                       WorldP3D &pos,
                                       Direction &dir) {
        velocity.current += acc.current * etc.dt;
        auto currentVelocity = velocity.current * etc.dt;

        zeroClip(currentVelocity);

        r32 x = dir.x * currentVelocity;
        r32 y = dir.y * currentVelocity;

        pos.x += x;
        pos.y += y;
    }

    void
    System_PositionAndVelocity::postUpdate(EntityManager &mng,
                                           duration32 dt) {

    }
}