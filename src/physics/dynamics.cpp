#include <oni-core/physics/dynamics.h>

#include <Box2D/Box2D.h>

#include <oni-core/physics/car.h>
#include <oni-core/graphics/window.h>
#include <oni-core/components/geometry.h>
#include <oni-core/physics/transformation.h>
#include <oni-core/graphics/debug-draw-box2d.h>

namespace oni {
    namespace physics {

        Dynamics::Dynamics(std::unique_ptr<graphics::DebugDrawBox2D> debugDrawBox2D, common::real32 tickFreq)
                : mDebugDraw(std::move(debugDrawBox2D)), mTickFrequency(tickFreq) {
            b2Vec2 gravity(0.0f, 0.0f);
            mPhysicsWorld = std::make_unique<b2World>(gravity);
            mPhysicsWorld->SetDebugDraw(mDebugDraw.get());
        }

        void Dynamics::tick(entt::DefaultRegistry &registry, const io::Input &input, common::real32 tickTime) {

            auto carView = registry.view<components::Placement, components::Car,
                    components::CarConfig, components::TagVehicle>();

            for (auto entity: carView) {
                auto &car = carView.get<components::Car>(entity);
                const auto &carConfig = carView.get<components::CarConfig>(entity);

                auto carInput = components::CarInput();

                if (input.isPressed(GLFW_KEY_W) || input.isPressed(GLFW_KEY_UP)) {
                    // TODO: When using game-pad, this value will vary between (0.0f...1.0f)
                    carInput.throttle = 1.0f;
                }
                if (input.isPressed(GLFW_KEY_A) || input.isPressed(GLFW_KEY_LEFT)) {
                    carInput.left = 1.0f;
                }
                if (input.isPressed(GLFW_KEY_S) || input.isPressed(GLFW_KEY_DOWN)) {
                    carInput.throttle = -1.0f;
                }
                if (input.isPressed(GLFW_KEY_D) || input.isPressed(GLFW_KEY_RIGHT)) {
                    carInput.right = 1.0f;
                }
                if (input.isPressed(GLFW_KEY_F)) {
                    car.velocity.x *= 1.1f;
                    car.velocity.y *= 1.1f;
                }
                if (input.isPressed(GLFW_KEY_SPACE)) {
                    if (car.accumulatedEBrake < 1.0f) {
                        car.accumulatedEBrake += 0.01f;
                    }
                    carInput.eBrake = static_cast<common::real32>(car.accumulatedEBrake);
                } else {
                    car.accumulatedEBrake = 0.0f;
                }

                auto steerInput = carInput.left - carInput.right;
                if (car.smoothSteer) {
                    car.steer = applySmoothSteer(car, steerInput, tickTime);
                } else {
                    car.steer = steerInput;
                }

                if (car.safeSteer) {
                    car.steer = applySafeSteer(car, steerInput);
                }

                car.steerAngle = car.steer * carConfig.maxSteer;

                tickCar(car, carConfig, carInput, tickTime);

                auto placement = components::Placement{math::vec3{car.position.x, car.position.y, 1.0f},
                                                       static_cast<const common::real32>(car.heading),
                                                       math::vec3{1.0f, 1.0f, 0.0f}};

                auto velocity = car.velocityLocal.len();


                car.distanceFromCamera = 1 + velocity * 2 / car.maxVelocityAbsolute;

                Transformation::updatePlacement(registry, entity, placement);
            }

            mPhysicsWorld->Step(mTickFrequency, 6, 2);

            auto carPhysicsView = registry.view<components::Car, components::TagVehicle, components::PhysicalProperties>();

            for (auto entity: carPhysicsView) {
                bool collided = false;
                auto body = carPhysicsView.get<components::PhysicalProperties>(entity).body;

                for (b2ContactEdge *ce = body->GetContactList(); ce; ce = ce->next) {
                    b2Contact *c = ce->contact;
                    if (c->GetFixtureA()->IsSensor() || c->GetFixtureB()->IsSensor()) {
                        collided = c->IsTouching();
                    }
                }

                auto &car = carPhysicsView.get<components::Car>(entity);
                // NOTE: > 0.2f is to avoid sticking to objects when velocity is low
                // TODO: Perhaps it is better to fix this problem by handling user input when there is
                // collision
                // TODO: Collision listener could be a better way to handle collisions
                if (collided && car.velocityAbsolute > 0.2f) {
                    car.velocity = math::vec2{body->GetLinearVelocity().x, body->GetLinearVelocity().y};
                    car.angularVelocity = body->GetAngularVelocity();
                    car.position = math::vec2{body->GetPosition().x, body->GetPosition().y};
                    car.heading = body->GetAngle();
                } else {
                    body->SetLinearVelocity(b2Vec2{car.velocity.x, car.velocity.y});
                    body->SetAngularVelocity(static_cast<float32>(car.angularVelocity));
                    body->SetTransform(b2Vec2{car.position.x, car.position.y}, static_cast<float32>(car.heading));
                }
            }

            auto dynamicEntitiesView = registry.view<components::Placement, components::TagDynamic, components::PhysicalProperties>();

            for (auto entity: dynamicEntitiesView) {
                auto body = dynamicEntitiesView.get<components::PhysicalProperties>(entity).body;
                auto position = body->GetPosition();
                auto placement = components::Placement{};
                placement.position = math::vec3{position.x, position.y, 1.0f};
                placement.rotation = body->GetAngle();
                registry.replace<components::Placement>(entity, placement);
            }
        }

        void Dynamics::drawDebugData() {
            mDebugDraw->Begin();
            mPhysicsWorld->DrawDebugData();
            mDebugDraw->End();
        }

        b2World *Dynamics::getPhysicsWorld() {
            return mPhysicsWorld.get();
        }

    }
}