#include <oni-core/physics/oni-physics-system.h>

#include <Box2D/Dynamics/b2Body.h>

namespace oni {
    System_JetForce::System_JetForce(EntityManager &em) : SystemTemplate(em) {
        assert(em.getSimMode() == SimMode::SERVER);
    }

    void
    System_JetForce::update(EntityTickContext &etc,
                            JetForce &jet,
                            Orientation &ornt,
                            PhysicalBody &body) {
        if (!subAndZeroClip(jet.fuze, r32(etc.dt))) {
            body.value->ApplyForceToCenter(
                    b2Vec2(cos(ornt.value) * jet.force,
                           sin(ornt.value) * jet.force),
                    true);
        } else {
            mExpiredForces.push_back(etc.id);
        }
    }

    void
    System_JetForce::postUpdate(EntityManager &mng,
                                duration32 dt) {
        for (auto &&id: mExpiredForces) {
            mng.removeComponent<JetForce>(id);
        }
        mExpiredForces.clear();
    }
}
