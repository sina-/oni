#include <oni-core/physics/projectile.h>

#include <Box2D/Box2D.h>
#include <GLFW/glfw3.h>

#include <oni-core/entities/client-data-manager.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/entities/entity-factory.h>
#include <oni-core/component/gameplay.h>

namespace oni {
    namespace physics {
        Projectile::Projectile(b2World *physicsWorld) :
                mPhysicsWorld{physicsWorld},
                mGunCoolDownS{0.2f} {}

        Projectile::~Projectile() = default;

        void Projectile::tick(entities::EntityFactory &entityFactory,
                              entities::ClientDataManager &clientData,
                              common::real64 tickTime) {
            // Update cool-downs
            {
                auto view = entityFactory.getEntityManager().createViewScopeLock<component::GunCoolDown>();
                for (auto &&entity: view) {
                    auto &coolDown = view.get<component::GunCoolDown>(entity);
                    if (coolDown.value > 0) {
                        coolDown.value -= tickTime;
                    }
                }
            }

            // Spawn bullets
            {
                auto carView = entityFactory.getEntityManager().createViewScopeLock<
                        component::Placement,
                        component::Car,
                        component::CarConfig,
                        component::EntityAttachment>();

                common::real32 bulletSpeed{20.f};

                for (auto &&entity: carView) {
                    auto clientLock = clientData.scopedLock();
                    auto *input = clientData.getClientInput(entity);
                    if (!input) {
                        continue;
                    }

                    if (input->isPressed(GLFW_KEY_F)) {
                        const auto &carPlacement = carView.get<component::Placement>(entity);
                        const auto &carConfig = carView.get<component::CarConfig>(entity);
                        const auto &car = carView.get<component::Car>(entity);
                        const auto &heading = carView.get<component::Car>(entity).heading;
                        const auto &attachments = carView.get<component::EntityAttachment>(entity);
                        fireBullet(entityFactory, bulletSpeed, carPlacement, carConfig, heading, attachments);
                    }
                }
            }
        }

        void Projectile::fireBullet(entities::EntityFactory &entityFactory,
                                    common::real32 bulletSpeed,
                                    const component::Placement &carPlacement,
                                    const component::CarConfig &carConfig,
                                    const common::CarSimDouble &heading,
                                    const component::EntityAttachment &attachments) {
            for (common::size i = 0; i < attachments.entities.size(); ++i) {
                if (attachments.entityTypes[i] == component::EntityType::VEHICLE_GUN) {
                    auto &gunCoolDown = entityFactory.getEntityManager().get<component::GunCoolDown>(
                            attachments.entities[i]);
                    if (gunCoolDown.value > 0) {
                        continue;
                    }
                    gunCoolDown.value = mGunCoolDownS;

                    // NOTE: Car rotates around its center of gravity, that means radius of the circle traversed by the
                    // car nose is half bounding rectangle's diagonal.
                    // Using car heading direction and its diagonal we could find the position
                    // of car nose, which is where we like to spawn the bullet projectiles.
                    // TODO: This looks unnecessary, can I not just use attached gun position to find the bullet
                    // spawn location?
                    common::real32 carHalfX =
                            static_cast<common::real32 >(carConfig.cgToFront + carConfig.cgToRear) / 2.f;
                    auto carHalfY = static_cast<common::real32>(carConfig.halfWidth);
                    auto offset = sqrt(carHalfX * carHalfX + carHalfY * carHalfY);

                    math::vec2 bulletSize{0.3f, 0.1f};
                    auto &carPos = carPlacement.position;
                    auto bulletOffset = sqrt(
                            bulletSize.x * bulletSize.x / 4.f + bulletSize.y * bulletSize.y / 4.f);
                    common::real32 fudge = 0.2f;

                    math::vec3 pos{
                            carPos.x + (offset + bulletOffset + fudge) * cos(carPlacement.rotation),
                            carPos.y + (offset + bulletOffset + fudge) * sin(carPlacement.rotation),
                            carPos.z};

                    std::string textureID = "resources/images/bullet/1.png";

                    auto bulletEntity = entityFactory.createEntity<component::EntityType::SIMPLE_BULLET>(pos,
                                                                                                         bulletSize,
                                                                                                         carPlacement.rotation,
                                                                                                         textureID,
                                                                                                         bulletSpeed);
                    entityFactory.tagForNetworkSync(bulletEntity);

                    std::string soundID = "resources/audio/rocket/1.wav";
                    entityFactory.createEvent<component::EventType::SOUND_EFFECT>(soundID, pos.getXY());

                }
            }
        }
    }
}