#include <oni-core/physics/projectile.h>

#include <Box2D/Box2D.h>
#include <GLFW/glfw3.h>

#include <oni-core/entities/client-data-manager.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/component/geometry.h>
#include <oni-core/entities/create-entity.h>

namespace oni {
    namespace physics {
        Projectile::Projectile(b2World *physicsWorld) : mPhysicsWorld{physicsWorld} {

        }

        Projectile::~Projectile() = default;

        void Projectile::tick(entities::EntityManager &manager,
                              entities::ClientDataManager &clientData,
                              common::real64 tickTime) {
            auto carView = manager.createViewScopeLock<
                    component::Placement, component::Car, component::CarConfig, component::Tag_Vehicle>();

            common::real32 bulletSpeed{200.f};

            for (auto &&entity: carView) {
                auto clientLock = clientData.scopedLock();
                const auto &input = clientData.getClientInput(entity);

                if (input.isPressed(GLFW_KEY_F)) {
                    auto &carPlacement = carView.get<component::Placement>(entity);
                    auto &carConfig = carView.get<component::CarConfig>(entity);
                    auto &car = carView.get<component::Car>(entity);
                    auto &heading = carView.get<component::Car>(entity).heading;
                    // NOTE: Car rotates around its center of gravity, that means radius of the circle traversed by the
                    // car nose is half bounding rectangle's diagonal.
                    // Using car heading direction and its diagonal we could find the position
                    // of car nose, which is where we like to spawn the bullet projectiles.
                    common::real32 carHalfX =
                            static_cast<common::real32 >(carConfig.cgToFront + carConfig.cgToRear) / 2.f;
                    common::real32 carHalfY = static_cast<common::real32>(carConfig.halfWidth);
                    common::real32 offset = std::sqrt(carHalfX * carHalfX + carHalfY * carHalfY);
                    auto bulletID = createBullet(manager, carPlacement, offset);
                    auto *body = manager.get<component::PhysicalProperties>(bulletID).body;
                    body->ApplyForceToCenter(
                            b2Vec2(car.velocity.x + static_cast<common::real32>(std::cos(heading) * bulletSpeed),
                                   car.velocity.y + static_cast<common::real32>(std::sin(heading) * bulletSpeed)),
                            true);
                }
            }
        }

        common::EntityID Projectile::createBullet(
                entities::EntityManager &manager,
                const component::Placement &carPlacement,
                common::real32 offset
        ) {
            auto &carPos = carPlacement.position;
            math::vec2 bulletSize{0.3f, 0.1f};
            common::real32 bulletOffset = std::sqrt(bulletSize.x * bulletSize.x / 4 + bulletSize.y * bulletSize.y / 4);
            math::vec3 bulletPos{carPos.x + (offset + bulletOffset) * std::cos(carPlacement.rotation),
                                 carPos.y + (offset + bulletOffset) * std::sin(carPlacement.rotation),
                                 carPos.z};

            std::string bulletTexture = "resources/images/bullet/1.png";

            oni::component::PhysicalProperties properties;
            properties.friction = 1.f;
            properties.density = 0.1f;
            properties.angularDamping = 2.f;
            properties.linearDamping = 0.1f;
            properties.bullet = true;
            properties.bodyType = oni::component::BodyType::DYNAMIC;

            auto bulletID = oni::entities::createEntity(manager);
            oni::entities::assignPhysicalProperties(manager, *mPhysicsWorld, bulletID,
                                                    bulletPos,
                                                    bulletSize,
                                                    carPlacement.rotation,
                                                    properties);
            oni::entities::assignShapeLocal(manager, bulletID, bulletSize, carPos.z);
            oni::entities::assignPlacement(manager, bulletID, bulletPos, math::vec3{1.f, 1.f, 0.f},
                                           carPlacement.rotation);
            oni::entities::assignTextureToLoad(manager, bulletID, bulletTexture);
            oni::entities::assignTag<component::Tag_Dynamic>(manager, bulletID);
            oni::entities::assignTag<component::Tag_Bullet>(manager, bulletID);

            return bulletID;
        }
    }
}