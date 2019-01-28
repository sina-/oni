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
            auto carView = manager.createViewScopeLock<component::Placement, component::Car, component::Tag_Vehicle>();

            for (auto &&entity: carView) {
                auto clientLock = clientData.scopedLock();
                const auto &input = clientData.getClientInput(entity);

                if (input.isPressed(GLFW_KEY_F)) {
                    auto carPlacement = carView.get<component::Placement>(entity);
                    auto &heading = carView.get<component::Car>(entity).heading;
                    auto bulletID = createBullet(manager, carPlacement);
                    auto *body = manager.get<component::PhysicalProperties>(bulletID).body;
                    body->ApplyForceToCenter(
                            b2Vec2(static_cast<common::real32>(std::cos(heading) * 200),
                                   static_cast<common::real32>(std::sin(heading) * 200)),
                            true);
                }
            }
        }

        common::EntityID
        Projectile::createBullet(entities::EntityManager &manager, const component::Placement &carPlacement) {
            auto &carPos = carPlacement.position;
            math::vec3 bulletPos{carPos.x + 2.f, carPos.y, carPos.z};
            math::vec2 bulletSize{0.3f, 0.1f};

            std::string bulletTexture = "resources/images/bullet/1.png";

            auto bulletID = oni::entities::createEntity(manager);
            oni::entities::assignPhysicalProperties(manager, *mPhysicsWorld, bulletID,
                                                    bulletPos,
                                                    bulletSize,
                                                    carPlacement.rotation,
                                                    oni::component::BodyType::DYNAMIC, true);
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