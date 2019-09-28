#include <oni-core/physics/oni-physics-system.h>

#include <Box2D/Dynamics/b2Body.h>

namespace oni {
    System_SyncPos::System_SyncPos(EntityManager &em) : SystemTemplate(em) {
        assert(em.getSimMode() == SimMode::SERVER ||
               em.getSimMode() == SimMode::CLIENT);
    }

    void
    System_SyncPos::update(EntityTickContext &etc,
                           PhysicalBody &body,
                           WorldP3D &ePos,
                           Orientation &ornt,
                           Scale &scale) {
        auto &bPos = body.value->GetPosition();

        if (!almost_Equal(ePos.x, bPos.x) ||
            !almost_Equal(ePos.y, bPos.y) ||
            !almost_Equal(ornt.value, body.value->GetAngle())) {

            // TODO: This is buggy, depending on the order of call to box2d world tick and car tick
            // this might or might not work. Try using a flag that signals that there are updates?
            ePos.x = bPos.x;
            ePos.y = bPos.y;

            if (etc.mng.has<WorldP3D_History>(etc.id)) {
                etc.mng.get<WorldP3D_History>(etc.id).add(ePos);
            }

            ornt.value = body.value->GetAngle();
            etc.mng.markForNetSync(etc.id);
        }
    }

    void
    System_SyncPos::postUpdate(EntityManager &mng,
                               duration32 dt) {}
}