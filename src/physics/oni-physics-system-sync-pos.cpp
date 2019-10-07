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

            if (etc.mng.has<Car>(etc.id)) {
                auto &car = etc.mng.get<Car>(etc.id);
                body.value->SetLinearVelocity(b2Vec2{car.velocity.x, car.velocity.y});
                body.value->SetAngularVelocity(static_cast<float32>(car.angularVelocity));
                body.value->SetTransform(b2Vec2{ePos.x, ePos.y},
                                         static_cast<float32>(ornt.value));
            } else {
                // TODO: This is buggy, depending on the order of call to box2d world tick and car tick
                // this might or might not work. Try using a flag that signals that there are updates?
                ePos.x = bPos.x;
                ePos.y = bPos.y;
                ornt.value = body.value->GetAngle();
            }

            if (etc.mng.has<WorldP3D_History>(etc.id)) {
                etc.mng.get<WorldP3D_History>(etc.id).add(ePos);
            }

            etc.mng.markForNetSync(etc.id);
        }
    }

    void
    System_SyncPos::postUpdate(EntityManager &mng,
                               duration32 dt) {}
}