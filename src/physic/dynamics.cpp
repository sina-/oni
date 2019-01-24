#include <oni-core/physic/dynamics.h>

#include <Box2D/Box2D.h>
#include <GLFW/glfw3.h>

#include <oni-core/entities/entity-manager.h>
#include <oni-core/entities/client-data-manager.h>
#include <oni-core/physic/car.h>
#include <oni-core/graphic/window.h>
#include <oni-core/graphic/debug-draw-box2d.h>
#include <oni-core/component/geometry.h>
#include <oni-core/physic/transformation.h>
#include <oni-core/common/consts.h>

namespace oni {
    namespace physic {

        Dynamics::Dynamics(common::real32 tickFreq)
                : mTickFrequency(tickFreq) {
            b2Vec2 gravity(0.0f, 0.0f);
            mPhysicsWorld = std::make_unique<b2World>(gravity);
        }

        Dynamics::~Dynamics() = default;

        void Dynamics::tick(entities::EntityManager &manager,
                            entities::ClientDataManager &clientData,
                            common::real64 tickTime) {
            std::map<common::EntityID, component::CarInput> carInput{};
            std::vector<common::EntityID> entitiesToBeUpdated{};
            {
                // NOTE: Need to lock it because network system might remove cars for clients that have disconnected.
                // TODO: Maybe there is a better way to tick the cars without needing to lock the whole registry!
                // One solutions could be, if this loop is very heavy, is to create a copy of all the data I need to
                // operate on and do all the calculation and then lock the registry and update all the corresponding
                // entities.
                auto carView = manager.createViewScopeLock<component::Placement, component::Car,
                        component::CarConfig, component::Tag_Vehicle>();

                for (auto &&entity: carView) {
                    auto clientLock = clientData.scopedLock();
                    const auto &input = clientData.getClientInput(entity);

                    auto &car = carView.get<component::Car>(entity);
                    const auto &carConfig = carView.get<component::CarConfig>(entity);

                    if (input.isPressed(GLFW_KEY_W) || input.isPressed(GLFW_KEY_UP)) {
                        // TODO: When using game-pad, this value will vary between (0.0f...1.0f)
                        carInput[entity].throttle = 1.0f;
                    }
                    if (input.isPressed(GLFW_KEY_A) || input.isPressed(GLFW_KEY_LEFT)) {
                        carInput[entity].left = 1.0f;
                    }
                    if (input.isPressed(GLFW_KEY_S) || input.isPressed(GLFW_KEY_DOWN)) {
                        carInput[entity].throttle = -1.0f;
                    }
                    if (input.isPressed(GLFW_KEY_D) || input.isPressed(GLFW_KEY_RIGHT)) {
                        carInput[entity].right = 1.0f;
                    }
                    if (input.isPressed(GLFW_KEY_LEFT_SHIFT)) {
                        car.velocity = car.velocity + math::vec2{static_cast<common::real32>(cos(car.heading)),
                                                                 static_cast<common::real32>(sin(car.heading))};
                    }
                    if (input.isPressed(GLFW_KEY_SPACE)) {
                        if (car.accumulatedEBrake < 1.0f) {
                            car.accumulatedEBrake += 0.01f;
                        }
                        carInput[entity].eBrake = static_cast<common::real32>(car.accumulatedEBrake);
                    } else {
                        car.accumulatedEBrake = 0.0f;
                    }

                    auto steerInput = carInput[entity].left - carInput[entity].right;
                    if (car.smoothSteer) {
                        car.steer = applySmoothSteer(car, steerInput, tickTime);
                    } else {
                        car.steer = steerInput;
                    }


                    if (car.safeSteer) {
                        car.steer = applySafeSteer(car, steerInput);
                    }

                    car.steerAngle = car.steer * carConfig.maxSteer;

                    tickCar(car, carConfig, carInput[entity], tickTime);

                    auto &carPlacement = carView.get<component::Placement>(entity);
                    auto placement = component::Placement{
                            math::vec3{car.position.x, car.position.y,
                                    // TODO: Perhaps better to have car.position to be the canonical z value?
                                       carPlacement.position.z},
                            static_cast<const common::real32>(car.heading),
                            math::vec3{1.0f, 1.0f, 0.0f}};
                    Transformation::updatePlacement(manager, entity, placement);

                    auto velocity = car.velocityLocal.len();
                    car.distanceFromCamera = 1 + velocity * 2 / car.maxVelocityAbsolute;

                    entitiesToBeUpdated.push_back(entity);
                }
            }

            {
                // TODO: entity registry has pointers to mPhysicsWorld internal data structures :(
                // One way to hide it is to provide a function in physics library that creates physical entities
                // for a given entity id an maintains an internal mapping between them without leaking the
                // implementation to outside.
                mPhysicsWorld->Step(mTickFrequency, 6, 2);
            }

            {
                auto carPhysicsView = manager.createViewScopeLock<component::Car, component::Tag_Vehicle,
                        component::PhysicalProperties>();

                // Handle collision
                for (auto entity: carPhysicsView) {
                    auto *body = carPhysicsView.get<component::PhysicalProperties>(entity).body;
                    auto &car = carPhysicsView.get<component::Car>(entity);
                    // NOTE: If the car was in collision previous tick, that is what isColliding is tracking,
                    // just apply user input to box2d representation of physical body without syncing
                    // car dynamics with box2d physics, that way the next tick if the
                    // car was heading out of collision it will start sliding out and things will run smoothly according
                    // to car dynamics calculation. If the car is still heading against other objects, it will be
                    // stuck as it was and I will skip dynamics and just sync it to  position and orientation
                    // from box2d. This greatly improves game feeling when there are collisions and
                    // solves lot of stickiness issues.
                    if (car.isColliding) {
                        // TODO: Right now 30 is just an arbitrary multiplier, maybe it should be based on some value in
                        // carconfig?
                        // TODO: Test other type of forces if there is a combination of acceleration and steering to sides
                        body->ApplyForceToCenter(
                                b2Vec2(static_cast<common::real32>(std::cos(car.heading) * 30 *
                                                                   carInput[entity].throttle),
                                       static_cast<common::real32>(std::sin(car.heading) * 30 *
                                                                   carInput[entity].throttle)),
                                true);
                        car.isColliding = false;
                    } else {
                        bool collisionFound = false;
                        for (b2ContactEdge *ce = body->GetContactList(); ce && !collisionFound; ce = ce->next) {
                            b2Contact *c = ce->contact;
                            if (c->GetFixtureA()->IsSensor() || c->GetFixtureB()->IsSensor()) {
                                collisionFound = c->IsTouching();
                            }
                        }

                        // TODO: Collision listener could be a better way to handle collisions
                        if (collisionFound) {
                            car.velocity = math::vec2{body->GetLinearVelocity().x,
                                                      body->GetLinearVelocity().y};
                            car.angularVelocity = body->GetAngularVelocity();
                            car.position = math::vec2{body->GetPosition().x, body->GetPosition().y};
                            car.heading = body->GetAngle();
                            car.isColliding = true;
                        } else {
                            body->SetLinearVelocity(b2Vec2{car.velocity.x, car.velocity.y});
                            body->SetAngularVelocity(static_cast<float32>(car.angularVelocity));
                            body->SetTransform(b2Vec2{car.position.x, car.position.y},
                                               static_cast<float32>(car.heading));
                        }
                    }
                }
            }

            {
                // Handle collision for other dynamic entities
                auto dynamicEntitiesView = manager.createViewScopeLock<component::Placement, component::Tag_Dynamic,
                        component::PhysicalProperties>();

                for (auto entity: dynamicEntitiesView) {
                    auto body = dynamicEntitiesView.get<component::PhysicalProperties>(entity).body;
                    auto position = body->GetPosition();
                    auto &placement = dynamicEntitiesView.get<component::Placement>(entity);

                    // TODO: Maybe I can query box2d to find-out if an entity is updated
                    if (std::abs(placement.position.x - position.x) > common::ep ||
                        std::abs(placement.rotation - body->GetAngle()) > common::ep) {
                        placement.position = math::vec3{position.x, position.y, placement.position.z};
                        placement.rotation = body->GetAngle();

                        entitiesToBeUpdated.push_back(entity);
                    }
                }
            }

            {
                // Update tires
                auto carWithTiresView = manager.createViewScopeLock<component::Placement, component::Car,
                        component::CarConfig, component::Tag_Vehicle>();
                for (auto entity: carWithTiresView) {
                    auto car = carWithTiresView.get<component::Car>(entity);
                    auto &carTireFRPlacement = manager.get<component::Placement>(car.tireFR);
                    // TODO: I shouldn't need to do this kinda of rotation transformation, x-1.0f + 90.0f.
                    // There seems to be something wrong with the way tires are created in the beginning
                    carTireFRPlacement.rotation = static_cast<oni::common::real32>(car.steerAngle +
                                                                                   math::toRadians(90.0f));

                    auto &carTireFLPlacement = manager.get<component::Placement>(car.tireFL);
                    carTireFLPlacement.rotation = static_cast<oni::common::real32>(car.steerAngle +
                                                                                   math::toRadians(90.0f));
                }
            }

            for (auto &&entity: entitiesToBeUpdated) {
                manager.accommodate<component::Tag_OnlyComponentUpdate>(entity);
            }

        }

        b2World *Dynamics::getPhysicsWorld() {
            return mPhysicsWorld.get();
        }
    }
}