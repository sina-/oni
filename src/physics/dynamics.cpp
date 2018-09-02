#include <oni-core/physics/dynamics.h>

#include <Box2D/Box2D.h>

#include <oni-core/entities/entity-manager.h>
#include <oni-core/physics/car.h>
#include <oni-core/graphics/window.h>
#include <oni-core/components/geometry.h>
#include <oni-core/physics/transformation.h>
#include <oni-core/graphics/debug-draw-box2d.h>

namespace oni {
    namespace physics {

        Dynamics::Dynamics(common::real32 tickFreq)
                : mTickFrequency(tickFreq) {
            b2Vec2 gravity(0.0f, 0.0f);
            mPhysicsWorld = std::make_unique<b2World>(gravity);
            mPhysicsWorld->SetDebugDraw(mDebugDraw.get());
        }

        void Dynamics::tick(entities::EntityManager &manager, const io::Input &input, common::real64 tickTime) {
            std::vector<common::EntityID> entitiesToBeUpdated{};
            {
                // NOTE: Need to lock it because network system might remove cars for clients that have disconnected.
                // TODO: Maybe there is a better way to tick the cars without needing to lock the whole registry!
                // One solutions could be, if this loop is very heavy, is to create a copy of all the data I need to
                // operate on and do all the calculation and then lock the registry and update all the corresponding
                // entities.
                auto carView = manager.createViewScopeLock<components::Placement, components::Car,
                        components::CarConfig, components::TagVehicle>();

                // TODO: LOL you don't want to apply the same input to all the cars! Dispatch them accordingly.
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
                        car.velocity = car.velocity + math::vec2{static_cast<common::real32>(cos(car.heading)),
                                                                 static_cast<common::real32>(sin(car.heading))};
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
                    Transformation::updatePlacement(manager, entity, placement);

                    auto velocity = car.velocityLocal.len();
                    car.distanceFromCamera = 1 + velocity * 2 / car.maxVelocityAbsolute;

                    entitiesToBeUpdated.push_back(entity);
                }
            }

            {
                // TODO: manager has pointers to mPhysicsWorld internal data structures :(
                mPhysicsWorld->Step(mTickFrequency, 6, 2);
            }

            {
                auto carPhysicsView = manager.createViewScopeLock<components::Car, components::TagVehicle,
                        components::PhysicalProperties>();

                // Handle collision
                for (auto entity: carPhysicsView) {
                    bool collisionFound = false;
                    auto body = carPhysicsView.get<components::PhysicalProperties>(entity).body;

                    for (b2ContactEdge *ce = body->GetContactList(); ce; ce = ce->next) {
                        b2Contact *c = ce->contact;
                        if (c->GetFixtureA()->IsSensor() || c->GetFixtureB()->IsSensor()) {
                            collisionFound = c->IsTouching();
                        }
                    }

                    auto &car = carPhysicsView.get<components::Car>(entity);
                    // NOTE: > 0.2f is to avoid sticking to objects when velocity is low
                    // TODO: Perhaps it is better to fix this problem by handling user input when there is
                    // collision
                    // TODO: Collision listener could be a better way to handle collisions
                    if (collisionFound && car.velocityAbsolute > 0.2f) {
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
            }

            {
                // Handle collision for other dynamic entities
                auto dynamicEntitiesView = manager.createViewScopeLock<components::Placement, components::TagDynamic,
                        components::PhysicalProperties>();

                for (auto entity: dynamicEntitiesView) {
                    auto body = dynamicEntitiesView.get<components::PhysicalProperties>(entity).body;
                    auto position = body->GetPosition();
                    auto &placement = dynamicEntitiesView.get<components::Placement>(entity);
                    placement.position = math::vec3{position.x, position.y, 1.0f};
                    placement.rotation = body->GetAngle();

                    entitiesToBeUpdated.push_back(entity);
                }
            }

            {
                // Update tires
                auto carWithTiresView = manager.createViewScopeLock<components::Placement, components::Car,
                        components::CarConfig, components::TagVehicle>();
                for (auto entity: carWithTiresView) {
                    auto car = carWithTiresView.get<components::Car>(entity);
                    auto &carTireFRPlacement = manager.get<components::Placement>(car.tireFR);
                    // TODO: I shouldn't need to do this kinda of rotation transformation, x-1.0f + 90.0f.
                    // There seems to be something wrong with the way tires are created in the beginning
                    carTireFRPlacement.rotation = static_cast<oni::common::real32>(car.steerAngle +
                                                                                   math::toRadians(90.0f));

                    auto &carTireFLPlacement = manager.get<components::Placement>(car.tireFL);
                    carTireFLPlacement.rotation = static_cast<oni::common::real32>(car.steerAngle +
                                                                                   math::toRadians(90.0f));
                }
            }

            for (auto &&entity: entitiesToBeUpdated) {
                manager.accommodate<components::TagNeedsComponentSync>(entity);
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

        void Dynamics::setDebugDraw(std::unique_ptr<graphics::DebugDrawBox2D> debugDraw) {
            mDebugDraw = std::move(debugDraw);
        }

    }
}