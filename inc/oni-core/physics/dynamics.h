#pragma once

#include <oni-core/physics/physic.h>

class b2World;

namespace oni {
    namespace graphics{
        class DebugDrawBox2D;
    }
    namespace physics {
        class Dynamics : public Physic {

        public:
            explicit Dynamics(std::unique_ptr<graphics::DebugDrawBox2D> debugDraw, common::real32 tickFreq);

            ~Dynamics() override = default;

            void drawDebugData();

            // TODO: Ideally I shouldn't expose this dude!
            b2World * getPhysicsWorld();

            void tick(entt::DefaultRegistry &registry, const io::Input &input, common::real32 tickTime) override;

        private:
            std::unique_ptr<b2World> mPhysicsWorld{};
            std::unique_ptr<graphics::DebugDrawBox2D> mDebugDraw{};
            common::real32 mTickFrequency{};
        };
    }
}