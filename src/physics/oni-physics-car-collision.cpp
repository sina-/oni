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
        // NOTE: If the car was in collision previous tick, that is what isColliding is tracking,
        // just apply user input to box2d representation of physical body without syncing
        // car dynamics with box2d physics, that way the next tick if the
        // car was ornt out of collision it will start sliding out and things will run smoothly according
        // to car dynamics calculation. If the car is still ornt against other objects, it will be
        // stuck as it was and I will skip dynamics and just sync it to  position and orientation
        // from box2d. This greatly improves game feeling when there are collisions and
        // solves lot of stickiness issues.
        if (car.isColliding) {
            // TODO: Right now 30 is just an arbitrary multiplier, maybe it should be based on some value in
            // carconfig?
            // TODO: Test other type of forces if there is a combination of acceleration and steering to sides
            body.value->ApplyForceToCenter(
                    b2Vec2(static_cast<r32>(std::cos(ornt.value) * 30 *
                                            input.throttle),
                           static_cast<r32>(std::sin(ornt.value) * 30 *
                                            input.throttle)),
                    true);
            car.isColliding = false;
        } else {
            auto c = Physics::isColliding(body.value);
            if (c.colliding) {
                car.velocity = vec2{body.value->GetLinearVelocity().x,
                                    body.value->GetLinearVelocity().y};
                car.angularVelocity = body.value->GetAngularVelocity();
                pos.x = body.value->GetPosition().x;
                pos.y = body.value->GetPosition().y;
                ornt.value = body.value->GetAngle();
                car.isColliding = true;
            } else {
                body.value->SetLinearVelocity(b2Vec2{car.velocity.x, car.velocity.y});
                body.value->SetAngularVelocity(static_cast<float32>(car.angularVelocity));
                body.value->SetTransform(b2Vec2{pos.x, pos.y},
                                         static_cast<float32>(ornt.value));
                hist.add(pos);
            }
        }
    }

    void
    System_CarCollision::postUpdate(EntityManager &mng,
                                    duration32 dt) {}
}