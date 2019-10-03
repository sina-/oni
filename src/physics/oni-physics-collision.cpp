#include <oni-core/physics/oni-physics-system.h>

#include <oni-core/physics/oni-physics.h>

namespace oni {
    System_Collision::System_Collision(oni::EntityManager &em) : SystemTemplate(em) {}

    void
    System_Collision::update(EntityTickContext &etc,
                             PhysicalBody &body,
                             WorldP3D &pos) {
        auto result = Physics::isColliding(body.value);
        if (result.colliding) {
            if (mUniqueCollisions.find(result.pair) == mUniqueCollisions.end()) {
                mUniqueCollisions.emplace(result.pair);

                auto &propsA = etc.mng.get<PhysicalProperties>(result.pair.a);
                auto &propsB = etc.mng.get<PhysicalProperties>(result.pair.b);
                auto pcPair = PhysicalCatPair{propsA.physicalCategory, propsB.physicalCategory};

                etc.mng.enqueueEvent<Event_Collision>(result.pos, result.pair, pcPair);
            }
        }
    }

    void
    System_Collision::postUpdate(EntityManager &mng,
                                 duration32 dt) {
        mUniqueCollisions.clear();
    }
}