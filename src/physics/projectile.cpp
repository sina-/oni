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
                              entities::EntityFactory &entityFactory,
                              entities::ClientDataManager &clientData,
                              common::real64 tickTime) {
            {
                auto carView = manager.createViewScopeLock<
                        component::Placement, component::Car, component::CarConfig>();

                common::real32 bulletSpeed{200.f};

                for (auto &&entity: carView) {
                    auto clientLock = clientData.scopedLock();
                    auto *input = clientData.getClientInput(entity);
                    if (!input) {
                        continue;
                    }

                    // TODO: This will spawn bullets very fast, infact so fast that they will overlap each other.
                    if (input->isPressed(GLFW_KEY_F)) {
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
                        auto carHalfY = static_cast<common::real32>(carConfig.halfWidth);
                        auto offset = std::sqrt(carHalfX * carHalfX + carHalfY * carHalfY);

                        math::vec2 bulletSize{0.3f, 0.1f};
                        auto &carPos = carPlacement.position;
                        auto bulletOffset = std::sqrt(
                                bulletSize.x * bulletSize.x / 4.f + bulletSize.y * bulletSize.y / 4.f);
                        common::real32 fudge = 0.2f;

                        math::vec3 pos{
                                carPos.x + (offset + bulletOffset + fudge) * std::cos(carPlacement.rotation),
                                carPos.y + (offset + bulletOffset + fudge) * std::sin(carPlacement.rotation),
                                carPos.z};

                        std::string textureID = "resources/images/bullet/1.png";
                        auto bulletID = entityFactory.createEntity<component::EntityType::SIMPLE_BULLET>(pos,
                                                                                                         bulletSize,
                                                                                                         carPlacement.rotation,
                                                                                                         textureID);

                        auto *body = manager.get<component::PhysicalProperties>(bulletID).body;
                        body->ApplyForceToCenter(
                                b2Vec2(static_cast<common::real32>(std::cos(heading) * bulletSpeed),
                                       static_cast<common::real32>(std::sin(heading) * bulletSpeed)),
                                true);
                    }
                }
            }
        }
    }
}